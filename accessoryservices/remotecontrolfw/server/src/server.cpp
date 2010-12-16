// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Remote Control server implementation.
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include <remcon/remcontargetselectorplugin.h>
#include <remcon/remcontargetselectorplugininterface.h>
#include <remcon/remconbearerinterface.h>
#include <remcon/remconbearerbulkinterface.h>
#include "server.h"
#include "session.h"
#include "serversecuritypolicy.h"
#include "utils.h"
#include "bearermanager.h"
#include "messagequeue.h"
#include "convertermanager.h"
#include "remconmessage.h"
#include "connections.h"
#include "messagerecipients.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

PANICCATEGORY("server");

#ifdef __FLOG_ACTIVE
#define LOGSESSIONS							LogSessions()
#define LOGREMOTES							LogRemotes()
#define LOGOUTGOINGCMDPENDINGTSP			LogOutgoingCmdPendingTsp()
#define LOGOUTGOINGNOTIFYCMDPENDINGTSP		LogOutgoingNotifyCmdPendingTsp()
#define LOGOUTGOINGRSPPENDINGTSP			LogOutgoingRspPendingTsp()
#define LOGOUTGOINGPENDINGSEND				LogOutgoingPendingSend()
#define LOGOUTGOINGSENT						LogOutgoingSent()
#define LOGINCOMINGCMDPENDINGADDRESS			LogIncomingCmdPendingAddress()
#define LOGINCOMINGNOTIFYCMDPENDINGADDRESS		LogIncomingNotifyCmdPendingAddress()
#define LOGINCOMINGNOTIFYCMDPENDINGREADDRESS	LogIncomingNotifyCmdPendingReAddress()
#define LOGINCOMINGPENDINGDELIVERY			LogIncomingPendingDelivery()
#define LOGINCOMINGDELIVERED				LogIncomingDelivered()
#else
#define LOGSESSIONS
#define LOGREMOTES
#define LOGOUTGOINGCMDPENDINGTSP
#define LOGOUTGOINGNOTIFYCMDPENDINGTSP
#define LOGOUTGOINGRSPPENDINGTSP
#define LOGOUTGOINGPENDINGSEND
#define LOGOUTGOINGSENT
#define LOGINCOMINGCMDPENDINGADDRESS
#define LOGINCOMINGNOTIFYCMDPENDINGADDRESS
#define LOGINCOMINGNOTIFYCMDPENDINGREADDRESS
#define LOGINCOMINGPENDINGDELIVERY
#define LOGINCOMINGDELIVERED
#endif // __FLOG_ACTIVE

TInt BulkMain(TAny* aParam);

CRemConServer* CRemConServer::NewLC()
	{
	LOG_STATIC_FUNC;
	CRemConServer* self = new(ELeave) CRemConServer();
	CleanupStack::PushL(self);
	// StartL is where the kernel checks that there isn't already an instance 
	// of the same server running, so do it before ConstructL.
	self->StartL(KRemConServerName);
	self->ConstructL();
	return self;
	}

CRemConServer::~CRemConServer()
	{
	LOG_FUNC;

	delete iBearerManager;
	delete iShutdownTimer;
	
	// There should be no watcher as there should be no bulk thread running
	ASSERT_DEBUG(!iBulkThreadWatcher);

	iSessionsLock.Wait();
	iSessions.Close();
	iSessionsLock.Close();

	// Destroy TSP before iIncomingPendingAddress in case the TSP is 
	// addressing a message on it at the time.
	// The TSP should not be handling outgoing commands or responses as all the sessions 
	// have gone, and they clean up their outgoing messages when they close.
	ASSERT_DEBUG(!iTspHandlingOutgoingCommand);
	ASSERT_DEBUG(!iTspHandlingOutgoingResponse);
	ASSERT_DEBUG(!iTspHandlingOutgoingNotifyCommand);	
	// We can't assert anything about iTspAddressingIncomingCommand- it isn't 
	// interesting.
	delete iTsp;

	delete iOutgoingCmdPendingTsp;
	delete iOutgoingNotifyCmdPendingTsp;
	delete iOutgoingRspPendingTsp;
	delete iOutgoingRspPendingSend;
	delete iOutgoingPendingSend;
	delete iOutgoingSent;
	delete iIncomingCmdPendingAddress;
	delete iIncomingNotifyCmdPendingAddress;
	delete iIncomingNotifyCmdPendingReAddress;
	delete iIncomingPendingDelivery;
	delete iIncomingDelivered;
	
	delete iMessageRecipientsList;

	iTspConnections.Reset();
	iTspIncomingCmdClients.Reset();
	iTspIncomingNotifyCmdClients.Reset();
	
	delete iConverterManager;

	// Clean up the connection information (must be done after the bearer 
	// manager is destroyed).
	LOGREMOTES;
	delete iConnections;

	// This is the odd ECOM code for cleaning our session. NB This must be 
	// done AFTER destroying all the other things in this thread which use 
	// ECOM!
	if ( iEcom )
		{
		iEcom->Close();
		}
	REComSession::FinalClose();
	}

CRemConServer::CRemConServer()
 :	CPolicyServer(CActive::EPriorityHigh, KRemConServerPolicy),
 	iTspConnections(_FOFF(TRemConAddress, iLink)),
	iTspIncomingCmdClients(_FOFF(TClientInfo, iLink)),
	iTspIncomingNotifyCmdClients(_FOFF(TClientInfo, iLink2)),
	iTspIf4Stub(*this)
	{
	LOG_FUNC;
	// NB CRemConServer uses CActive::EPriorityHigh to help it get priority 
	// over other AOs in its thread. (The fact that it's added to the AS 
	// before anything else helps too.) This is so that client requests are 
	// not blocked by other AOs in the thread being very busy. This relies of 
	// course on the cooperation of the other AOs running in RemCon's thread 
	// over which we have no control (e.g. those in externally-supplied 
	// bearers or the Target Selector Plugin). In Symbian OS, it's the best we 
	// can do.
	
	// This is needed for BC reasons, as the TUint32 padding in TClientInfo is now replaced with
	// a second TSglQueLink. Therefore in order to maintain BC we need these two classes
	// to be the same size.
	__ASSERT_COMPILE(sizeof(TUint32) == sizeof(TSglQueLink));
	}

void CRemConServer::ConstructL()
	{
	LOG_FUNC;
	// Open ECOM session.
	iEcom = &(REComSession::OpenL());
	LEAVEIFERRORL(iSessionsLock.CreateLocal());

	iShutdownTimer = CPeriodic::NewL(CActive::EPriorityStandard);

	// Make the connection object (implicitly containing no addresses) before
	// creating the bearer manager, as some bearers might call the bearer
	// manager back synchronously with a new connection, and we need
	// iConnections to be able to handle that.
	iConnections = CConnections::NewL();

	// Make the queues before making the bearer manager because otherwise a 
	// 'connection up' which is indicated synchronously with 
	// CBearerManager::NewL will blow us up (we address some of the queues 
	// when that happens).
	iOutgoingCmdPendingTsp = CMessageQueue::NewL();
	iOutgoingNotifyCmdPendingTsp = CMessageQueue::NewL();
	iOutgoingRspPendingTsp = CMessageQueue::NewL();
	iOutgoingRspPendingSend = CMessageQueue::NewL();
	iOutgoingPendingSend = CMessageQueue::NewL();
	iOutgoingSent = CMessageQueue::NewL();
	iIncomingCmdPendingAddress = CMessageQueue::NewL();
	iIncomingNotifyCmdPendingAddress = CMessageQueue::NewL();
	iIncomingNotifyCmdPendingReAddress = CMessageQueue::NewL();
	iIncomingPendingDelivery = CMessageQueue::NewL();
	iIncomingDelivered = CMessageQueue::NewL();

	iMessageRecipientsList = CMessageRecipientsList::NewL();
	
	// Make bearer manager. This makes the bearers, and connects them up to 
	// the event handler.
	iBearerManager = CBearerManager::NewL(*this);
	
	// We must load the bearers before the TSP as the TSP loader checks
	// whether there are any bearers with interface V2	
	ASSERT_ALWAYS(!iTspIf);
	
	iConverterManager = CConverterManager::NewL();

	LoadTspL();

	LOGREMOTES;
	}

CSession2* CRemConServer::NewSessionL(const TVersion& aVersion, 
	const RMessage2& aMessage) const
	{
	LOG(KNullDesC8);
	LOG_FUNC;
	LOG3(_L("\taVersion = (%d,%d,%d)"), aVersion.iMajor, aVersion.iMinor, aVersion.iBuild);
		
	// Version number check...
	TVersion v(KRemConSrvMajorVersionNumber,
		KRemConSrvMinorVersionNumber,
		KRemConSrvBuildNumber);

	if ( !User::QueryVersionSupported(v, aVersion) )
		{
		LEAVEIFERRORL(KErrNotSupported);
		}

	CRemConServer* ncThis = const_cast<CRemConServer*>(this);
	
	CRemConSession* sess = NULL;
	ASSERT_DEBUG(iBearerManager);
	TRAPD(err, sess = CRemConSession::NewL(*ncThis, 
				*iBearerManager, 
				aMessage, 
				(ncThis->iSessionId)++)
			);
	if ( err != KErrNone )
		{
		// Session creation might have failed- if it has we need to check if 
		// we need to shut down again.
		const_cast<CRemConServer*>(this)->StartShutdownTimerIfNoSessionsOrBulkThread();
		LEAVEIFERRORL(err);
		}

	LOG1(_L("\tsess = 0x%08x"), sess);
	return sess;
	}

void CRemConServer::StartShutdownTimerIfNoSessionsOrBulkThread()
	{
	LOG_FUNC;
	iSessionsLock.Wait();
	if ( iSessions.Count() == 0 && !iBulkThreadOpen)
		{
		LOG(_L("\tno remaining sessions- starting shutdown timer"));
		// Should have been created during our construction.
		ASSERT_DEBUG(iShutdownTimer);
		// Start the shutdown timer. It's actually a CPeriodic- the first 
		// event will be in KShutdownDelay microseconds' time.
		// NB The shutdown timer might already be active, in the following 
		// case: this function is being called by NewSessionL because there 
		// was a failure creating a new session, BUT this function had already 
		// been called by the session's destructor (i.e. the failure was in 
		// the session's ConstructL, NOT its new(ELeave)). To protect against 
		// KERN-EXEC 15 just check for if the timer is already active. 
		if ( !iShutdownTimer->IsActive() )
			{
			iShutdownTimer->Start(KShutdownDelay, 
				// Delay of subsequent firings (will not happen because we kill 
				// ourselves after the first).
				0, 
				TCallBack(CRemConServer::TimerFired, this)
				);
			}
		else
			{
			LOG(_L("\tshutdown timer was already active"));
			}
		}
	iSessionsLock.Signal();
	}

TInt CRemConServer::TimerFired(TAny* aThis)
	{
	LOG_STATIC_FUNC
	static_cast<void>(aThis);
	
#if defined(__FLOG_ACTIVE) || defined(_DEBUG)
	CRemConServer* self = static_cast<CRemConServer*>(aThis);
	// We should have sent 'this' to this callback. 
	ASSERT_DEBUG(self);
	LOG1(_L8("\tauto shutdown- terminating the server [0x%08x]"), self);
#endif // __FLOG_ACTIVE || _DEBUG
	
	// Stop our Active Scheduler. This returns the flow of execution to after 
	// the CActiveScheduler::Start call in the server startup code, and 
	// terminates the server.
	CActiveScheduler::Stop();
	
	return KErrNone;
	}

void CRemConServer::InitialiseBulkServerThreadL()
	{
	LOG_FUNC
	// Set up the configuration of the thread
	iBulkServerThread.SetPriority(EPriorityLess);
	
	iBulkThreadWatcher = new(ELeave) CBulkThreadWatcher(*this);
	CleanupDeleteAndNullPushL(iBulkThreadWatcher);
	
	// Create the communication between the servers.
	LEAVEIFERRORL(iBulkServerMsgQueue.CreateLocal(2)); // only ever two outstanding messages
	CleanupClosePushL(iBulkServerMsgQueue); // member variable, but closing again should be fine.
	
	// Load the server pointer (for TClientInfo info)
	TBulkServerMsg ctrlMsg;
	ctrlMsg.iType = EControlServer;
	ctrlMsg.iData = this;
	iBulkServerMsgQueue.SendBlocking(ctrlMsg); // this should not block as there is enough room in queue.
	
	// Load the bearer manager pointer
	TBulkServerMsg manMsg;
	manMsg.iType = EBearerManager;
	manMsg.iData = iBearerManager;
	iBulkServerMsgQueue.SendBlocking(manMsg); // this should not block as there is enough room in queue.
	
	TRequestStatus stat;
	iBulkServerThread.Rendezvous(stat);
	
	// Mark bulk server thread ready to run
	iBulkServerThread.Resume();
	User::WaitForRequest(stat); 	// wait for start or death
	
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	TInt err = (iBulkServerThread.ExitType() == EExitPanic) ? KErrServerTerminated : stat.Int();
	LEAVEIFERRORL(err);
	
	// Everything appears to be running...so watch the thread until it dies...
	iBulkThreadWatcher->StartL();
	
	CleanupStack::Pop(2, &iBulkThreadWatcher); // iBulkServerMsgQueue, iBulkThreadWatcher
	}


TInt CRemConServer::BulkServerRequired()
	{
	LOG_FUNC
	// If the bulk server is required then try to create it
	TThreadFunction bulkServerThreadFunction(BulkMain);
	_LIT(KBulkServerThreadName, "RemConBulkServerThread");
	const TInt KMaxBulkServerThreadHeapSize = 0x100000;
	
	TInt err = KErrNone;
	if(!iBulkThreadOpen)
		{
		err = iBulkServerThread.Create(KBulkServerThreadName, bulkServerThreadFunction, KDefaultStackSize, KMinHeapSize, KMaxBulkServerThreadHeapSize, &iBulkServerMsgQueue, EOwnerThread);
		if(err == KErrNone)
			{
			TRAP(err, InitialiseBulkServerThreadL());
			if(err == KErrNone)
				{
				iBulkThreadOpen = ETrue;
				}
			}
		}
	return err;
	}

TInt CRemConServer::ClientOpened(CRemConSession& aSession)
	{
	LOG_FUNC;
	LOG1(_L("\t&aSession = 0x%08x"), &aSession);
	LOGSESSIONS;

	// Register the session by appending it to our array.
	iSessionsLock.Wait();
	TInt ret = iSessions.Append(&aSession);
	iSessionsLock.Signal();

	if ( ret == KErrNone )
		{
		// Should have been created during our construction.
		ASSERT_DEBUG(iShutdownTimer);
		iShutdownTimer->Cancel();
		}

	LOGSESSIONS;
	LOG1(_L("\tret = %d"), ret);
	return ret;
	}

// this function is called by the session when the session type is set
void CRemConServer::ClientTypeSet(CRemConSession& aSession)
	{
	LOG_FUNC;
	LOG1(_L("\t&aSession = 0x%08x"), &aSession);
	LOGSESSIONS;

	/* When a client (session) has its type set (controller or target) then 
	   it will still have a bearer uid of NullUid. In this case we want to 
	   update any bearers which now have a controller or target count moving 
	   (from 0) to 1. */

	/* tell the bearer manager that someones set a client type
	   The bearer manager maintains controller and target counts for all bearers
	   and will tell bearers when they need to know things have changed */
	ASSERT_DEBUG(iBearerManager);
	iBearerManager->ClientTypeSet(aSession.Type() == ERemConClientTypeController);

	LOGSESSIONS;
	}

// this function is called by the session when the client has registered its features
void CRemConServer::TargetClientAvailable(CRemConSession& aSession)
	{
	LOG_FUNC;
	LOG1(_L("\t&aSession = 0x%08x"), &aSession);
	LOGSESSIONS;

	ASSERT_DEBUG(iBearerManager);
	iBearerManager->TargetClientAvailable(aSession.Id(), aSession.PlayerType(), aSession.PlayerSubType(), aSession.Name());
	if(iTspIf5)
		{
		iTspIf5->TargetClientAvailable(aSession.ClientInfo());
		}

	LOGSESSIONS;
	}

// this function is called by the session when the client has registered its features
void CRemConServer::ControllerClientAvailable()
	{
	LOG_FUNC;
	LOGSESSIONS;

	ASSERT_DEBUG(iBearerManager);
	iBearerManager->ControllerClientAvailable();
	
	LOGSESSIONS;
	}

// this function is called by the session when it goes connection oriented
void CRemConServer::ClientGoConnectionOriented(CRemConSession& aSession, TUid aUid)
	{
	LOG_FUNC;
	LOG1(_L("\t&aSession = 0x%08x"), &aSession);
	LOGSESSIONS;

	(void)&aSession; // get rid of unused warning.
	
	ASSERT_DEBUG(aSession.Type() == ERemConClientTypeController);

	/* now tell the bearer manager that someones went connection oriented
	   The bearer manager maintains controller and target counts for all bearers
	   and will tell bearers when they need to know things have changed */
	ASSERT_DEBUG(iBearerManager);
	iBearerManager->ClientConnectionOriented(aUid);

	LOGSESSIONS;
	}

// this is called by the session when the client goes connectionless
void CRemConServer::ClientGoConnectionless(CRemConSession& aSession, TUid aUid)
	{
	LOG_FUNC;
	LOG1(_L("\t&aSession = 0x%08x"), &aSession);
	LOGSESSIONS;

	(void)&aSession; // get rid of unused warning.
	
	ASSERT_DEBUG(aSession.Type() == ERemConClientTypeController);
	/* now tell the bearer manager that someones went connection less
	   The bearer manager maintains controller and target counts for all bearers
	   and will tell bearers when they need to know things have changed */
	ASSERT_DEBUG(iBearerManager);
	iBearerManager->ClientConnectionless(aUid);

	LOGSESSIONS;
	}

// called by session when session is closed.
void CRemConServer::ClientClosed(CRemConSession& aSession, TUid aUid)
	{
	LOG_FUNC;
	LOG1(_L("\t&aSession = 0x%08x"), &aSession);
	LOGSESSIONS;

	iSessionsLock.Wait();
	// Find this session in the array and remove it (if it's there).
	const TUint sessCount = iSessions.Count();
	for ( TUint ii = 0 ; ii < sessCount ; ++ii )
		{
		if ( iSessions[ii] == &aSession )
			{
			// We've found the session in our array.

			// 1. Remove the session from our array.
			iSessions.Remove(ii);

			// 2a. Tell the TSP if the session that has gone away is a target
			if((aSession.Type() == ERemConClientTypeTarget) && iTspIf5)
				{
				iTspIf5->TargetClientUnavailable(aSession.ClientInfo());
				}
			
			// 2b. Tell the bearers about the session going away, if it was the 
			// last controller or last target.
			// If the session hasn't already set its type, then it doesn't 
			// count (we won't have told the bearers about it to begin with).
			// The bearer manager maintains controller and target counts for all bearers
			// and will tell bearers when they need to know things have changed */
			if ( aSession.Type() != ERemConClientTypeUndefined )
				{
				ASSERT_DEBUG(iBearerManager);
				iBearerManager->ClientClosed(aSession.Type() == ERemConClientTypeController, aUid, aSession.Id());
				}

			// 3. Remove queued messages belonging to this session that: 
			// (a) are outgoing, awaiting access to the TSP 
			// (OutgoingPendingTsp, OutgoingNotifyPendingTsp), 
			// (b) are outgoing, awaiting a bearer connection 
			// (OutgoingPendingSend), 
			// (c) have been sent (OutgoingSent)
			// (d) are pending delivery to this session 
			// (IncomingPendingDelivery)
			// (e) have been delivered to this session and are awaiting 
			// responses (IncomingDelivered).
			TSglQueIter<CRemConMessage>& cmdIter = OutgoingCmdPendingTsp().SetToFirst();
			CRemConMessage* msg;
			TBool first = ETrue;
			while ( ( msg = cmdIter++ ) != NULL )
				{
				if ( msg->SessionId() == aSession.Id() )
					{
					// If the message is currently being worked on by the 
					// TSP, cancel the TSP before destroying it.
					if ( iTspHandlingOutgoingCommand && first )
						{
						ASSERT_DEBUG(iTspIf);
						iTspIf->CancelOutgoingCommand();
						iTspHandlingOutgoingCommand = EFalse;
						}
					OutgoingCmdPendingTsp().RemoveAndDestroy(*msg);
					}
				first = EFalse;
				}
			
			cmdIter = OutgoingNotifyCmdPendingTsp().SetToFirst();
			first = ETrue;
			while ( ( msg = cmdIter++ ) != NULL )
				{
				if ( msg->SessionId() == aSession.Id() )
					{
					// If the message is currently being worked on by the 
					// TSP, cancel the TSP before destroying it.
					if ( iTspHandlingOutgoingNotifyCommand && first )
						{
						ASSERT_DEBUG(iTspIf3);
						iTspIf3->CancelOutgoingNotifyCommand();
						iTspHandlingOutgoingNotifyCommand = EFalse;
						}
					OutgoingNotifyCmdPendingTsp().RemoveAndDestroy(*msg);
					}
				first = EFalse;
				}

			if ( aSession.Type() == ERemConClientTypeTarget )
				{
				// Remove the clients from the DeliveredMessageClients list
				ASSERT_DEBUG(iMessageRecipientsList);
				TSglQueIter<CMessageRecipients>& messageRecipientsIter = iMessageRecipientsList->Iter();
				
				messageRecipientsIter.SetToFirst();
				CMessageRecipients* message;
				while ((message = messageRecipientsIter++) != NULL)
					{
					message->RemoveAndDestroyClient(aSession.ClientInfo());
					if (message->Clients().IsEmpty())
						{
						iMessageRecipientsList->Messages().Remove(*message);
						// Inform bearer that it won't be getting a response
						
						// First we need to find the command - it could be in
						// OutgoingRspPendingTsp, IncomingDelivered or IncomingPendingDelivery
						
						CRemConMessage* msg;
						
						msg = OutgoingRspPendingTsp().Message(message->TransactionId());
						
						if (!msg)
							{
							msg = IncomingDelivered().Message(message->TransactionId());
							}
						if (!msg)
							{
							msg = IncomingPendingDelivery().Message(message->TransactionId());
							}
						
						if(msg)
							{
							// As this is the last message with this transaction ID, it should have this session ID
							ASSERT_DEBUG(msg->SessionId() == aSession.Id());
	
							// Now we've found the message, we can pass the reject to the bearer
							// Send reject to the bearer
							SendReject(msg->Addr(), msg->InterfaceUid(), msg->OperationId(), msg->TransactionId());
							}
						delete message;
						}
					TSglQueIter<CRemConMessage>& rspIter = OutgoingRspPendingTsp().SetToFirst();
	
					CRemConMessage* msg;
					TBool first = ETrue;
					while ( ( msg = rspIter++ ) != NULL )
						{
						if ( msg->SessionId() == aSession.Id() )
							{
							// If the message is currently being worked on by the 
							// TSP, cancel the TSP before destroying it.
							if (iTspIf2 && iTspHandlingOutgoingResponse && first )
								{
								iTspIf2->CancelOutgoingResponse();
								iTspHandlingOutgoingResponse = EFalse;
								}
							OutgoingRspPendingTsp().RemoveAndDestroy(*msg);
							}
						first = EFalse;
						}
					
					}
				}

			TSglQueIter<CRemConMessage>& sendIter = OutgoingPendingSend().SetToFirst();
			while ( ( msg = sendIter++ ) != NULL )
				{
				if ( msg->SessionId() == aSession.Id() )
					{
					if (msg->MsgType() == ERemConResponse)
						{
						SendReject(msg->Addr(), msg->InterfaceUid(), msg->OperationId(), msg->TransactionId());
						}
					OutgoingPendingSend().RemoveAndDestroy(*msg);
					}
				}

			OutgoingSent().RemoveAndDestroy(aSession.Id());
			
			TSglQueIter<CRemConMessage>& pendingDeliveryIter = IncomingPendingDelivery().SetToFirst();
			while ( ( msg = pendingDeliveryIter++ ) != NULL )
				{
				if ( msg->SessionId() == aSession.Id() )
					{
					if (msg->MsgType() == ERemConNotifyCommand)
						{
						SendReject(msg->Addr(), msg->InterfaceUid(), msg->OperationId(), msg->TransactionId());
						}
					IncomingPendingDelivery().RemoveAndDestroy(*msg);
					}
				}

			TSglQueIter<CRemConMessage>& deliveredIter = IncomingDelivered().SetToFirst();
			while ( ( msg = deliveredIter++ ) != NULL )
				{
				if ( msg->SessionId() == aSession.Id() )
					{
					if (msg->MsgType() == ERemConNotifyCommand)
						{
						SendReject(msg->Addr(), msg->InterfaceUid(), msg->OperationId(), msg->TransactionId());
						}
					IncomingDelivered().RemoveAndDestroy(*msg);
					}
				}
					
			break;
			} // End found session in our array
		}
	iSessionsLock.Signal();

	StartShutdownTimerIfNoSessionsOrBulkThread();

	LOGSESSIONS;
	}

TBool CRemConServer::TargetClientWithSameProcessId(TProcessId aProcId) const
	{
	LOG_FUNC;
	LOG1(_L("\taProcId = %d"), static_cast<TUint>(aProcId));

	TBool ret = EFalse;

	const CRemConSession* const sess = TargetSession(aProcId);
	if ( sess )
		{
		ret = ETrue;
		}

	LOG1(_L("\tret = %d"), ret);
	return ret;
	}

#ifdef __FLOG_ACTIVE
void CRemConServer::LogSessions() const
	{
	iSessionsLock.Wait();
	const TUint count = iSessions.Count();
	LOG1(_L("\tNumber of sessions = %d"), count);
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConSession* const session = iSessions[ii];
		ASSERT_DEBUG(session);
		LOG5(_L("\t\tsession %d [0x%08x], Id = %d, Type = %d, ProcessId = %d"), 
			ii, 
			session,
			session->Id(),
			session->Type(),
			static_cast<TUint>(session->ClientInfo().ProcessId())
			);
		}
	iSessionsLock.Signal();
	}

void CRemConServer::LogRemotes() const
	{
	// Called from dtor- this may not have been made yet.
	if ( iConnections )
		{
		iConnections->LogConnections();
		}
	}
#endif // __FLOG_ACTIVE

void CRemConServer::MrctspoDoOutgoingNotifyCommandAddressed(TRemConAddress* aConnection, TInt aError)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taError = %d"), aError);
	LOGOUTGOINGNOTIFYCMDPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;

	ASSERT_DEBUG(iTspHandlingOutgoingNotifyCommand);
	iTspHandlingOutgoingNotifyCommand = EFalse;

	CRemConMessage& msg = OutgoingNotifyCmdPendingTsp().First();
	ASSERT_DEBUG(msg.Addr().IsNull());
	CRemConSession* const sess = Session(msg.SessionId());
	// Session closure removes messages from the outgoing queue and cancels 
	// the TSP request if relevant. If sess is NULL here, then this processing 
	// has gone wrong.
	ASSERT_DEBUG(sess);
	
	sess->SendError() = KErrNone;
	if ( (aError != KErrNone) || !aConnection)
		{
		sess->SendError() = aError;
		sess->CompleteSendNotify();
		}
	else
		{
		// Message addressed OK.
		if ( aConnection != NULL )
			{
			TBool sync = EFalse;
			TRAPD(err, SendCmdToRemoteL(msg, *aConnection, sync));
			if ( err || sync )
				{
				sess->SendError() = err;
				sess->CompleteSendNotify();
				}
			
			delete aConnection;
			aConnection = NULL;
			}
		} 

	// We've now finished with the addressed message, so destroy it.
	OutgoingNotifyCmdPendingTsp().RemoveAndDestroy(msg);

	// Check for more notify commands to address.
	if ( !OutgoingNotifyCmdPendingTsp().IsEmpty() )
		{
		LOG(_L8("\tmore outgoing notify commands awaiting TSP..."));
		TspOutgoingNotifyCommand();
		}

	LOGOUTGOINGNOTIFYCMDPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;
	}

void CRemConServer::MrctspoDoOutgoingCommandAddressed(TInt aError)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taError = %d"), aError);
	LOGOUTGOINGCMDPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;

	ASSERT_DEBUG(iTspHandlingOutgoingCommand);
	iTspHandlingOutgoingCommand = EFalse;

	// If aError is KErrNone,
	// Remove the addressed message from the iOutgoingPendingTsp queue.
	// Send the command to the requested bearer(s), putting items on the 
	// iOutgoingSent queue to await responses
	// Complete the controller client's message. 

	// Any error at any point during the above should roll back everything 
	// (apart from removing the original message from iOutgoingPendingTsp) 
	// and error the sending controller.
	
	// Finally, check iOutgoingPendingTsp for more commands to give to 
	// the TSP.

	// The head item, currently being dealt with, is always at index 0.
	// NB This msg will be destroyed by the end of the function, and copies 
	// taken to add to iOutgoingSent. 
	// Note that if the client went away while the address request was 
	// outstanding, this item will still be on this queue because we protect 
	// it (see ClientClosed).
	CRemConMessage& msg = OutgoingCmdPendingTsp().First();
	// Check that the message isn't addressed already. If this fails, it's 
	// possible that the TSP has called OutgoingCommandAddressed in response 
	// to a PermitOutgoingCommand request.
	ASSERT_DEBUG(msg.Addr().IsNull());
	CRemConSession* const sess = Session(msg.SessionId());
	// Session closure removes messages from the outgoing queue and cancels 
	// the TSP request if relevant. If sess is NULL here, then this processing 
	// has gone wrong.
	ASSERT_DEBUG(sess);
	// The number of remotes the command was sent to.
	sess->NumRemotes() = 0;
	sess->SendError() = KErrNone;
	if ( aError != KErrNone )
		{
		sess->SendError() = aError;
		sess->NumRemotesToTry() = 0;
		}
	else
		{
		// Message addressed OK.
		// Work out how many remotes the TSP said to send to.
		TSglQueIter<TRemConAddress> iter(iTspConnections);
		while ( iter++ )
			{
			++sess->NumRemotesToTry();
			}
		iter.SetToFirst();
		// Try to connect and send a message to each specified remote.
		TRemConAddress* conn;
		while ( ( conn = iter++ ) != NULL )
			{
			LOG2(_L("\tsending message to remote [0x%08x] BearerUid = 0x%08x"), 
				conn, conn->BearerUid());

			// We send to as many of the remotes as we can. We remember 
			// how many remotes got sent to successfully, and complete the 
			// client's request with either KErrNone or _one of_ the 
			// errors that were raised. 
			TBool sync = EFalse;
			TRAPD(err, SendCmdToRemoteL(msg, *conn, sync));
			if ( err || sync )
				{
				// We have finished trying to process this (copy of this) 
				// message, so we can adjust our 'remotes' counter / 
				// completion error.
				if ( err == KErrNone )
					{
					++sess->NumRemotes();
					}
				else
					{
					sess->SendError() = err;
					}
				--sess->NumRemotesToTry();
				}
			// else we didn't actually make a send attempt because conn was 
			// down. This particular message will undergo an actual 
			// (bearer-level) send attempt later on when the connection comes 
			// up. For now, however, we cannot legally complete the client's 
			// request.

			iTspConnections.Remove(*conn);
			delete conn;
			} // End while 
		} // End if TSP addressed command OK
	if ( sess->NumRemotesToTry() == 0 )
		{
		sess->CompleteSend();
		}

	// We've now finished with the addressed message, so destroy it.
	OutgoingCmdPendingTsp().RemoveAndDestroy(msg);

	// Check for more commands to address.
	if ( !OutgoingCmdPendingTsp().IsEmpty() )
		{
		LOG(_L("\tmore outgoing commands awaiting TSP..."));
		TspOutgoingCommand();
		}

	LOGOUTGOINGCMDPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;
	}

void CRemConServer::MrctspoDoOutgoingCommandPermitted(TBool aIsPermitted)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taIsPermitted = %d"), aIsPermitted);
	LOGOUTGOINGCMDPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;

	ASSERT_DEBUG(iTspHandlingOutgoingCommand);
	iTspHandlingOutgoingCommand = EFalse;

	// If aIsPermitted is EFalse, complete the sending session with 
	// KErrPermissionDenied and destroy the message. Otherwise try to send the 
	// message and complete the sending session. If it sent OK, move the 
	// message to the 'outgoing sent' queue.
	// At the end, check the 'pending TSP' queue again.
	// The head item, currently being dealt with, is always at index 0.
	// Note that if the client went away while the address request was 
	// outstanding, this item will still be on this queue because we protect 
	// it (see ClientClosed).
	CRemConMessage& msg = OutgoingCmdPendingTsp().First();
	// Check that the message is addressed already. If this fails, it's 
	// possible that the TSP has called OutgoingCommandPermitted in response 
	// to a AddressOutgoingCommand request.
	ASSERT_DEBUG(!msg.Addr().IsNull());
	CRemConSession* const sess = Session(msg.SessionId());
	// Session closure removes messages from the outgoing queue and cancels 
	// the TSP request if relevant. If sess is NULL here, then this processing 
	// has gone wrong.
	ASSERT_DEBUG(sess);
	TInt err = KErrPermissionDenied;
	if ( aIsPermitted )
		{
		TBool sync = EFalse;
		TRAP(err, SendCmdToRemoteL(msg, msg.Addr(), sync));
		if ( err || sync )
			{
			// We made a send attempt at the bearer level. Complete the 
			// client's message according to err.
			--sess->NumRemotesToTry();
			sess->NumRemotes() = ( err == KErrNone ) ? 1 : 0;
			sess->SendError() = err;
			sess->CompleteSend();
			}
		// else the message is waiting until a bearer-level connection 
		// comes up. Only then can we complete the client's message.
		}
	else
		{
		// The send wasn't permitted, so complete the client's message.
		// This was a connection-oriented send, so we know the number of 
		// remotes it was sent to is 0.
		--sess->NumRemotesToTry();
		sess->NumRemotes() = 0;
		sess->SendError() = KErrPermissionDenied;
		sess->CompleteSend();
		}

	// We've now finished with the message, so destroy it.
	OutgoingCmdPendingTsp().RemoveAndDestroy(msg);

	// Check for more commands to give to the TSP.
	if ( !OutgoingCmdPendingTsp().IsEmpty() )
		{
		LOG(_L("\tmore outgoing commands awaiting TSP..."));
		TspOutgoingCommand();
		}

	LOGOUTGOINGCMDPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;
	}

void CRemConServer::MrctspoDoOutgoingNotifyCommandPermitted(TBool aIsPermitted)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taIsPermitted = %d"), aIsPermitted);
	LOGOUTGOINGCMDPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;

	ASSERT_DEBUG(iTspHandlingOutgoingNotifyCommand);
	iTspHandlingOutgoingNotifyCommand = EFalse;

	// If aIsPermitted is EFalse, complete the sending session with 
	// KErrPermissionDenied and destroy the message. Otherwise try to send the 
	// message and complete the sending session. If it sent OK, move the 
	// message to the 'outgoing sent' queue.
	// At the end, check the 'pending TSP' queue again.
	// The head item, currently being dealt with, is always at index 0.
	// Note that if the client went away while the address request was 
	// outstanding, this item will still be on this queue because we protect 
	// it (see ClientClosed).
	CRemConMessage& msg = OutgoingNotifyCmdPendingTsp().First();
	// Check that the message is addressed already. If this fails, it's 
	// possible that the TSP has called OutgoingCommandPermitted in response 
	// to a AddressOutgoingCommand request.
	ASSERT_DEBUG(!msg.Addr().IsNull());
	CRemConSession* const sess = Session(msg.SessionId());
	// Session closure removes messages from the outgoing queue and cancels 
	// the TSP request if relevant. If sess is NULL here, then this processing 
	// has gone wrong.
	ASSERT_DEBUG(sess);
	TInt err = KErrPermissionDenied;
	if ( aIsPermitted )
		{
		TBool sync = EFalse;
		TRAP(err, SendCmdToRemoteL(msg, msg.Addr(), sync));
		if ( err || sync )
			{
			sess->SendError() = err;
			sess->CompleteSendNotify();
			}
		// else the message is waiting until a bearer-level connection 
		// comes up. Only then can we complete the client's message.
		}
	else
		{
		sess->SendError() = KErrPermissionDenied;
		sess->CompleteSendNotify();
		}

	// We've now finished with the message, so destroy it.
	OutgoingNotifyCmdPendingTsp().RemoveAndDestroy(msg);

	// Check for more commands to give to the TSP.
	if ( !OutgoingNotifyCmdPendingTsp().IsEmpty() )
		{
		LOG(_L("\tmore outgoing commands awaiting TSP..."));
		TspOutgoingNotifyCommand();
		}

	LOGOUTGOINGCMDPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;
	}

void CRemConServer::MrctspoDoOutgoingResponsePermitted(TBool aIsPermitted)
	{
	LOG_FUNC;
	LOGOUTGOINGRSPPENDINGTSP;
	ASSERT_DEBUG(iTspHandlingOutgoingResponse);
	iTspHandlingOutgoingResponse = EFalse;
	ASSERT_DEBUG(iOutgoingRspPendingTsp);
	CRemConMessage& msg = iOutgoingRspPendingTsp->First();
	
	iOutgoingRspPendingTsp->Remove(msg);
	
	CRemConSession* const sess = Session(msg.SessionId());	
	// Session closure removes messages from the outgoing queue and cancels 
	// the TSP request if relevant. If sess is NULL here, then this processing 
	// has gone wrong.
	ASSERT_DEBUG(sess);

	if (aIsPermitted)
		{
		ASSERT_DEBUG(iMessageRecipientsList);
		iMessageRecipientsList->RemoveAndDestroyMessage(msg.TransactionId());
		CompleteSendResponse(msg, *sess); // Ownership of msg is always taken
		}
	else
		{
		CMessageRecipients* messageClients = iMessageRecipientsList->Message(msg.TransactionId());
		if (messageClients)
			{
			messageClients->RemoveAndDestroyClient(sess->ClientInfo()); // Remove the current client info from the list
			if (messageClients->Clients().IsEmpty())
				{
				iMessageRecipientsList->RemoveAndDestroyMessage(msg.TransactionId());
				// The bearer won't be getting a response
				
				SendReject(msg.Addr(), msg.InterfaceUid(), msg.OperationId(), msg.TransactionId());
				}
			}
		--sess->NumRemotesToTry();
		sess->SendError() = KErrNone;
		if (sess->NumRemotesToTry() == 0)
			{
			sess->CompleteSend();
			}
		delete &msg;
		}
	if (!iOutgoingRspPendingTsp->IsEmpty())
		{
		PermitOutgoingResponse();
		}
	LOGOUTGOINGRSPPENDINGTSP;
	}

void CRemConServer::MrctspoDoIncomingNotifyAddressed(TClientInfo* aClientInfo, TInt aError)
	{
	LOG_FUNC;
	LOG(KNullDesC8());
	LOG1(_L("\taError = %d"), aError);
	LOGINCOMINGNOTIFYCMDPENDINGADDRESS;
	LOGINCOMINGNOTIFYCMDPENDINGREADDRESS;
	LOGINCOMINGDELIVERED;

	// Send the command to the requested target client, and remove the 
	// addressed message from the iIncomingNotifyCmdPendingAddress queue. Any error at 
	// any point should be ignored- just complete as much as we can. 
	// Then check iIncomingPendingAddress for more commands to give to the 
	// TSP. 

	ASSERT_DEBUG(iTspAddressingIncomingNotifyCommand || iTspReAddressingIncomingNotifyCommands);
	
	if (iTspAddressingIncomingNotifyCommand)
		{
		iTspAddressingIncomingNotifyCommand = EFalse;
		if (!iTspDropIncomingNotifyCommand)
			{
			// We know that the queue is not empty because we put something on the 
			// queue to call AddressIncomingNotifyCommand, which results in one call to this 
			// function, which is the only place where messages are removed from the 
			// queue.
			CRemConMessage& msg = IncomingNotifyCmdPendingAddress().First();
			TBool cmdDelivered = EFalse;
		
			// If the TSP errored, can't complete to any clients.
			if ( aError == KErrNone && aClientInfo)
				{
				LOG1(_L("\t\tprocess ID %d"), static_cast<TUint>(aClientInfo->ProcessId()));
				// Get the corresponding session.
				CRemConSession* const sess = TargetSession(aClientInfo->ProcessId());
				// NB The set of open sessions may have changed while the request 
				// was out on the TSP. If the TSP indicates a session that has 
				// gone away, then ignore that session. 
				if ( sess )
					{
					TRAPD(err, DeliverCmdToClientL(msg, *sess));
					if (err == KErrNone)
						{
						cmdDelivered = ETrue;
						}
					// If we couldn't deliver an instance of the command to a 
					// target, there's not much we can do. 
					}
				}
			
			if (!cmdDelivered)
				{
				// The command wasn't delivered to any clients
				
				// The command wasn't delivered to any clients
				// Tell bearer it won't be getting a response		
		
				SendReject(msg.Addr(), msg.InterfaceUid(), msg.OperationId(), msg.TransactionId());
				}

			
			// Destroy the message we've just dealt with.
			IncomingNotifyCmdPendingAddress().RemoveAndDestroy(msg);
			}
		iTspDropIncomingNotifyCommand = EFalse;
		if ( !IncomingNotifyCmdPendingAddress().IsEmpty() )
			{
			LOG(_L("\tmore incoming commands awaiting addressing..."));
			AddressIncomingNotifyCommand();
			}
		}
	else
		{
		if (!iTspDropIncomingNotifyCommand)
			{
			CRemConMessage& msg = IncomingNotifyCmdPendingReAddress().First();
			if(aError == KErrNone && aClientInfo)
				{
				LOG1(_L("\t\tprocess ID %d"), static_cast<TUint>(aClientInfo->ProcessId()));
				// Get the corresponding session.
				CRemConSession* const sess = TargetSession(aClientInfo->ProcessId());
				if (sess)
					{
					if (sess->Id() == msg.SessionId())
						{
						// Don't do anything - it's already on IncomingDelivered
						}
					else
						{
						// Remove the original message from IncomingDelivered
						CRemConMessage* deliveredMsg = NULL;
						TSglQueIter<CRemConMessage> iter = IncomingDelivered().SetToFirst();
						while ((deliveredMsg = iter++) != NULL)
							{
							if (deliveredMsg->TransactionId() == msg.TransactionId())
								{
								// We need to update the subtype now, in case the client has sent an interim response while the notify
								// was being readdressed.
								msg.MsgSubType() = deliveredMsg->MsgSubType();
								// Deliver to the client
								TRAPD(err, DeliverCmdToClientL(msg, *sess));
								if (err == KErrNone)
									{
									// Only remove the current message if the delivery to the new client suceeded.
									IncomingDelivered().RemoveAndDestroy(*deliveredMsg);
									}
								break;
								}
							}
						}
					}
				else
					{
					SendReject(msg.Addr(), msg.InterfaceUid(), msg.OperationId(), msg.TransactionId());
					}
				}
			IncomingNotifyCmdPendingReAddress().RemoveAndDestroy(msg);
			}
			
		iTspDropIncomingNotifyCommand = EFalse;
	
		if ( !IncomingNotifyCmdPendingReAddress().IsEmpty() )
			{
			LOG(_L("\tmore incoming commands awaiting readdressing..."));
			ReAddressIncomingNotifyCommand();
			}
		else
			{
			iTspReAddressingIncomingNotifyCommands = EFalse;
			if ( !IncomingNotifyCmdPendingAddress().IsEmpty() )
				{
				LOG(_L("\tmore incoming commands awaiting addressing..."));
				AddressIncomingNotifyCommand();
				}
			}
		}
	LOGINCOMINGNOTIFYCMDPENDINGADDRESS;
	LOGINCOMINGNOTIFYCMDPENDINGREADDRESS;
	LOGINCOMINGDELIVERED;
	}

void CRemConServer::MrctspoDoReAddressNotifies()
	{
	LOG_FUNC;
	LOGINCOMINGNOTIFYCMDPENDINGREADDRESS;
	LOGINCOMINGDELIVERED;
	ASSERT_DEBUG(iTspReAddressingIncomingNotifyCommands == EFalse && iTspAddressingIncomingNotifyCommand == EFalse);
	TSglQueIter<CRemConMessage> iter = IncomingDelivered().SetToFirst();
	CRemConMessage* deliveredMsg;
	while ((deliveredMsg = iter++) != NULL)
		{
		if (deliveredMsg->MsgType() == ERemConNotifyCommand)
			{
			CRemConMessage* newMsg = NULL;
			TRAPD(err, newMsg = CRemConMessage::CopyL(*deliveredMsg));
			if (err == KErrNone)
				{
				IncomingNotifyCmdPendingReAddress().Append(*newMsg);
				}
			// If we couldn't copy the message, there isn't much we can do now.
			}
		}
	if (!IncomingNotifyCmdPendingReAddress().IsEmpty())
		{
		iTspReAddressingIncomingNotifyCommands = ETrue;
		ReAddressIncomingNotifyCommand();
		}
	LOGINCOMINGNOTIFYCMDPENDINGREADDRESS;
	}

void CRemConServer::MrctspoDoIncomingCommandPermitted(TBool aIsPermitted)
	{
	LOG_FUNC
	
	MrctspoDoIncomingCommandAddressed(aIsPermitted ? KErrNone : KErrAccessDenied);
	}

void CRemConServer::MrctspoDoIncomingNotifyPermitted(TBool aIsPermitted)
	{
	LOG_FUNC
	
	if(aIsPermitted)
		{
		TClientInfo* clientInfo = ClientIdToClientInfo(IncomingNotifyCmdPendingAddress().First().Client());
		MrctspoDoIncomingNotifyAddressed(clientInfo, KErrNone);
		}
	else
		{
		MrctspoDoIncomingNotifyAddressed(NULL, KErrAccessDenied);
		}
	}

void CRemConServer::TspOutgoingCommand()
	{
	LOG_FUNC;

	ASSERT_DEBUG(iTspIf);
	ASSERT_DEBUG(iTspHandlingOutgoingCommand == EFalse);
	// For TSPs which complete this request synchronously this will become 
	// recursive, but the depth is not expected to be great (== number of 
	// messages awaiting access to the TSP).

	// Work out whether the next command to deal with is awaiting (a) 
	// addressing or (b) permission.
	// The head item is at index 0. This function should only be called if the 
	// queue is not empty.
	CRemConMessage& msg = OutgoingCmdPendingTsp().First();
	CRemConSession* const sess = Session(msg.SessionId());
	// The session should exist- if it closed after asking to send this 
	// message, then the message should have been removed from the outgoing 
	// pending TSP queue at that time.
	ASSERT_DEBUG(sess);
	iTspHandlingOutgoingCommand = ETrue;
	if ( msg.Addr().IsNull() )
		{
		// Null address means it's awaiting an address.
		// Check the array of outgoing addresses is ready for this new request 
		// on the TSP.
		ASSERT_DEBUG(iTspConnections.IsEmpty());
		ASSERT_DEBUG(iBearerManager);
		iTspIf->AddressOutgoingCommand(
				msg.InterfaceUid(),
				msg.OperationId(), 
				sess->ClientInfo(),
				iTspConnections,
				iBearerManager->BearerSecurityPolicies());	
		}
	else
		{
		// Non-null address means it's awaiting permission to send.
		sess->NumRemotesToTry() = 1;
		iTspIf->PermitOutgoingCommand(
			msg.InterfaceUid(),
			msg.OperationId(), 
			sess->ClientInfo(),
			msg.Addr());
		}
	}

void CRemConServer::TspOutgoingNotifyCommand()
	{
	LOG_FUNC;

	ASSERT_DEBUG(iTspIf3);
	ASSERT_DEBUG(iTspHandlingOutgoingNotifyCommand == EFalse);
	// For TSPs which complete this request synchronously this will become 
	// recursive, but the depth is not expected to be great (== number of 
	// messages awaiting access to the TSP).

	// Work out whether the next command to deal with is awaiting (a) 
	// addressing or (b) permission.
	// The head item is at index 0. This function should only be called if the 
	// queue is not empty.
	CRemConMessage& msg = OutgoingNotifyCmdPendingTsp().First();
	CRemConSession* const sess = Session(msg.SessionId());
	// The session should exist- if it closed after asking to send this 
	// message, then the message should have been removed from the outgoing 
	// pending TSP queue at that time.
	ASSERT_DEBUG(sess);
	iTspHandlingOutgoingNotifyCommand = ETrue;
	if ( msg.Addr().IsNull() )
		{
		ASSERT_DEBUG(iBearerManager);

		iTspIf3->AddressOutgoingNotify(
				msg.InterfaceUid(),
				msg.OperationId(), 
				sess->ClientInfo(),
				iBearerManager->BearerSecurityPolicies());
		}
	else
		{
		// Non-null address means it's awaiting permission to send.
		sess->NumRemotesToTry() = 1;
		iTspIf3->PermitOutgoingNotifyCommand(
			msg.InterfaceUid(),
			msg.OperationId(), 
			sess->ClientInfo(),
			msg.Addr());
		}
	}

void CRemConServer::AddressIncomingCommand()
	{
	LOG_FUNC;

	ASSERT_DEBUG(iTspIf);
	ASSERT_DEBUG(iTspAddressingIncomingCommand == EFalse);
	// For TSPs which complete this request synchronously this will become 
	// recursive, but the depth is not expected to be great (== number of 
	// messages awaiting addressing).
	
	// There are two possibilities here, either the bearer has already provided
	// us with an address, in which case we just provide the TSP the command
	// for information, and to give it the opportunity to reject the command,
	// or we don't have an address, in which case we ask the TSP for one
	
	// This function should only be called if we know this queue is not 
	// empty.
	CRemConMessage& msg = IncomingCmdPendingAddress().First();
	iTspIncomingCmdClients.Reset();
	iTspAddressingIncomingCommand = ETrue;
	
	if(msg.Client() == KNullClientId)
		{
		// Prepare the array of target process IDs for the TSP.
		iSessionsLock.Wait();
		const TUint count = iSessions.Count();
		for ( TUint ii = 0 ; ii < count ; ++ii )
			{
			CRemConSession* const sess = iSessions[ii];
			ASSERT_DEBUG(sess);
			if ( sess->Type() == ERemConClientTypeTarget )
				{
				iTspIncomingCmdClients.AddLast(sess->ClientInfo());
				}
			}
		iSessionsLock.Signal();
		
		iTspIf->AddressIncomingCommand(
			msg.InterfaceUid(),
			msg.OperationId(),
			iTspIncomingCmdClients);
		}
	else
		{
		iTspIncomingCmdClients.AddLast(*ClientIdToClientInfo(msg.Client()));
		ASSERT_DEBUG(iTspIf4);
		iTspIf4->PermitIncomingCommand(
			msg.InterfaceUid(),
			msg.OperationId(),
			*iTspIncomingCmdClients.First());
		}
	}

void CRemConServer::AddressIncomingNotifyCommand()
	{
	LOG_FUNC;

	ASSERT_DEBUG(iTspIf2);
	ASSERT_DEBUG(iTspAddressingIncomingNotifyCommand == EFalse);
	// For TSPs which complete this request synchronously this will become 
	// recursive, but the depth is not expected to be great (== number of 
	// messages awaiting addressing).
	
	// There are two possibilities here, either the bearer has already provided
	// us with an address, in which case we just provide the TSP the command
	// for information, and to give it the opportunity to reject the command,
	// or we don't have an address, in which case we ask the TSP for one
	
	// This function should only be called if we know this queue is not 
	// empty
	CRemConMessage& msg = IncomingNotifyCmdPendingAddress().First();
	iTspIncomingNotifyCmdClients.Reset();
	iTspAddressingIncomingNotifyCommand = ETrue;
	
	if(!FindDuplicateNotify(msg))
		{
		if(msg.Client() == KNullClientId)
			{
			// Prepare the array of target process IDs for the TSP.
			iSessionsLock.Wait();
			const TUint count = iSessions.Count();
			for ( TUint ii = 0 ; ii < count ; ++ii )
				{
				CRemConSession* const sess = iSessions[ii];
				ASSERT_DEBUG(sess);
				if ( sess->Type() == ERemConClientTypeTarget )
					{
					iTspIncomingNotifyCmdClients.AddLast(sess->ClientInfo());
					}
				}
			iSessionsLock.Signal();
			
			// Only send the notify to the TSP if there isn't an identical one on either incomingpendingdelivery or incomingdelivered
			iTspIf2->AddressIncomingNotify(
					msg.InterfaceUid(),
					msg.OperationId(),
					iTspIncomingNotifyCmdClients);
			}
		else
			{
			iTspIncomingNotifyCmdClients.AddLast(*ClientIdToClientInfo(msg.Client()));
			ASSERT_DEBUG(iTspIf4);
			iTspIf4->PermitIncomingNotify(
				msg.InterfaceUid(),
				msg.OperationId(),
				*iTspIncomingNotifyCmdClients.First());
			}
		}
	else
		{
		// Otherwise, we can call IncomingNotifyAddressed with NULL, and it will be rejected back to the bearer
		MrctspoDoIncomingNotifyAddressed(NULL, KErrArgument);
		}
	}

void CRemConServer::ReAddressIncomingNotifyCommand()
	{
	LOG_FUNC;
	LOGINCOMINGNOTIFYCMDPENDINGREADDRESS;
	ASSERT_DEBUG(iTspIf2);
	ASSERT_DEBUG(iTspReAddressingIncomingNotifyCommands);
	// For TSPs which complete this request synchronously this will become 
	// recursive, but the depth is not expected to be great (== number of 
	// messages awaiting addressing).
	// Prepare the array of target process IDs for the TSP.
	iTspIncomingNotifyCmdClients.Reset();
	iSessionsLock.Wait();
	const TUint count = iSessions.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConSession* const sess = iSessions[ii];
		ASSERT_DEBUG(sess);
		if ( sess->Type() == ERemConClientTypeTarget )
			{
			iTspIncomingNotifyCmdClients.AddLast(sess->ClientInfo());
			}
		}
	iSessionsLock.Signal();
	
	// This function should only be called if we know this queue is not 
	// empty.
	CRemConMessage& msg = IncomingNotifyCmdPendingReAddress().First();
	iTspIf2->AddressIncomingNotify(
		msg.InterfaceUid(),
		msg.OperationId(),
		iTspIncomingNotifyCmdClients);
	}


void CRemConServer::PermitOutgoingResponse()
	{
	LOG_FUNC;
	LOGOUTGOINGRSPPENDINGTSP;
	
	ASSERT_DEBUG(iTspHandlingOutgoingResponse == EFalse);
	ASSERT_DEBUG(!OutgoingRspPendingTsp().IsEmpty());
	while (!iTspHandlingOutgoingResponse && !OutgoingRspPendingTsp().IsEmpty())
		{
		CRemConMessage& msg = OutgoingRspPendingTsp().First();
		CRemConSession* session = Session(msg.SessionId());
		// The session should exist- if it closed after asking to send this 
		// message, then the message should have been removed from the outgoing 
		// pending TSP queue at that time.		
		ASSERT_DEBUG(session);
		ASSERT_DEBUG(iMessageRecipientsList);
		CMessageRecipients* message = iMessageRecipientsList->Message(msg.TransactionId());

		if (message) // If we aren't returned a client list, this means that the message has been delivered elsewhere
			{
			iTspHandlingOutgoingResponse = ETrue;
			if (iTspIf2)
				{
				iTspIf2->PermitOutgoingResponse(
						msg.InterfaceUid(),
						msg.OperationId(),
						session->ClientInfo(),
						message->ConstIter()
						);
				}
			else
				{
				OutgoingResponsePermitted(ETrue);
				}
			}
		else
			{
			session->NumRemotes() = 0;
			--session->NumRemotesToTry();
			session->SendError() = KErrNone;
			session->CompleteSend();
			OutgoingRspPendingTsp().RemoveAndDestroy(msg);
			}
		}
	LOGOUTGOINGRSPPENDINGTSP;
	}

void CRemConServer::SendCmdToRemoteL(const CRemConMessage& aMsg, const TRemConAddress& aConn, TBool& aSync)
	{
	LOG_FUNC;

	ASSERT_DEBUG(  
			       (aMsg.MsgType() == ERemConCommand)
			    || (aMsg.MsgType() == ERemConNotifyCommand)
			    );


	// Populate aSync immediately in case the function leaves.
	aSync = EFalse;
	TConnectionState conState = ConnectionState(aConn);
	if ( conState == EConnected)
		{
		aSync = ETrue;
		}

	// Take a copy of the message and set the right address.
	CRemConMessage* newMsg = CRemConMessage::CopyL(aMsg);
	newMsg->Addr() = aConn;
	CleanupStack::PushL(newMsg);
	ASSERT_DEBUG(iBearerManager);

	// If we're connected (and not mid-connect/disconnect) we can send immediately,
	// otherwise we queue and try to bring up the connection.
	switch ( conState )
		{
		case EConnected:
			{
			// We have a connection to send over. Try to send it.
			LEAVEIFERRORL(iBearerManager->Send(*newMsg));
			// If Send worked, then the bearer has taken ownership of the 
			// message's data.
			newMsg->OperationData().Assign(NULL); 
			CLEANUPSTACK_POP1(newMsg);
			OutgoingSent().Append(*newMsg); 
			break;
			}
		case EDisconnected:
			{
			// No connection. Try to bring one up. If we can't then destroy the 
			// new message and leave.
			CLEANUPSTACK_POP1(newMsg);
			// We need to put the message on the queue before trying to connect 
			// because the connect confirmation might come back synchronously.
			OutgoingPendingSend().Append(*newMsg);
			TInt err = iBearerManager->Connect(newMsg->Addr());
			if ( err != KErrNone )
				{
				OutgoingPendingSend().RemoveAndDestroy(*newMsg);
				aSync = ETrue;
				LEAVEIFERRORL(err); // will destroy newMsg
				}
			break;
			}
		case EConnecting:
		case EDisconnecting:
			{
			// Just queue if connection is coming up or going down.
			//
			// If connection is coming up, the message will be
			// sent when ConnectConfirm fires.
			//
			// If connection is going down: When CBearerManager gets
			// the DisconnectConfirm it calls us back at RemoveConnection,
			// whereat we will reconnect the bearer that was mid-disconnect;
			// the message will be sent when that completes.
			CLEANUPSTACK_POP1(newMsg);
			OutgoingPendingSend().Append(*newMsg);
			break;
			}
		default:
			{
			DEBUG_PANIC_LINENUM;
			break;
			}
		}

	LOG1(_L("\taSync = %d"), aSync);
	}

void CRemConServer::DeliverCmdToClientL(const CRemConMessage& aMsg, CRemConSession& aSess)
	{
	LOG_FUNC;

	ASSERT_DEBUG((aMsg.MsgType() == ERemConCommand) || (aMsg.MsgType() == ERemConNotifyCommand)); 
	// Take a copy of the message and set the right session ID (important to 
	// set the selected session's ID because this is how we match up the 
	// client's response, if aMsg is a command). 
	CRemConMessage* const newMsg = CRemConMessage::CopyL(aMsg);
	newMsg->SessionId() = aSess.Id();
	LEAVEIFERRORL(DeliverMessageToClient(*newMsg, aSess));
	}

TInt CRemConServer::DeliverMessageToClient(CRemConMessage& aMsg, CRemConSession& aSess)
	{
	LOG_FUNC;
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;
	TInt err = KErrNone;

	if ( aSess.ClientAvailable() )
		{
		// First off check if the client supports this
		if(!aSess.SupportedMessage(aMsg))
			{
			err = KErrArgument;

			// 'Take ownership' of it by destroying it- it's finished with.
			delete &aMsg;
			}
		else if ( aSess.CurrentReceiveMessage().Handle() )
			{
			// If the client can take the message now put it on the right queue.

			err = aSess.WriteMessageToClient (aMsg);
			// If the message was a command, and it was delivered with no error,
			// then put it in the 'incoming delivered' log. Otherwise, delete it
			// because it's finished with.
			if ((aMsg.MsgType() == ERemConCommand) || (aMsg.MsgType() == ERemConNotifyCommand))
				{
				if (err == KErrNone )
					{
					// We'll need to remember it for the response coming back.
					IncomingDelivered().Append(aMsg);
					}
				else
					{
					// 'Take ownership' of it by destroying it- it's finished with.
					delete &aMsg;
					}
				}
			else
				{
				// 'Take ownership' of it by destroying it- it's finished with.
				delete &aMsg;
				}
			}
		else
			{
			IncomingPendingDelivery().Append(aMsg);
			}
		}
	else
		{
		// The client has not yet registered the interfaces they're interested
		// in, so put the message on the incoming pending delivery queue until
		// they do.
		// Return KErrNone to avoid sending a reject. We're assuming that the
		// client will eventually successfully receive the message, as we
		// don't have a better basis on which to operate. This is identical to
		// the case where a client doesn't have an outstanding receive request
		// at this time.
		IncomingPendingDelivery().Append(aMsg);
		}
	
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;
	return err;
	}
														  
void CRemConServer::MrctspoDoIncomingCommandAddressed(TInt aError)
	{
	LOG(KNullDesC8());
	LOG_FUNC;
	LOG1(_L("\taError = %d"), aError);
	LOGINCOMINGCMDPENDINGADDRESS;
	LOGINCOMINGDELIVERED;

	// Send the command to the requested target client(s), and remove the 
	// addressed message from the iIncomingPendingAddress queue. Any error at 
	// any point should be ignored- just complete as much as we can. 
	// Then check iIncomingPendingAddress for more commands to give to the 
	// TSP. 

	ASSERT_DEBUG(iTspAddressingIncomingCommand);
	
	iTspAddressingIncomingCommand = EFalse;

	if (!iTspDropIncomingCommand)
		{
		// If we did not call CommandExpired on the first item in the queue,
		// we know that the queue is not empty because we put something on the 
		// queue to call AddressIncomingCommand, which results in one call to this 
		// function, which is the only place where messages are removed from the 
		// queue.
		CRemConMessage& msg = IncomingCmdPendingAddress().First();
		TBool cmdDelivered = EFalse;
	
		// If the TSP errored, can't complete to any clients.
		if ( aError == KErrNone )
			{
			TSglQueIter<TClientInfo> iter(iTspIncomingCmdClients);
			
			TClientInfo* procId;
			CMessageRecipients* messageRecipients = NULL;
			TBool canDeliver = ETrue;
			TRAPD(err, messageRecipients = CMessageRecipients::NewL());
			if (err != KErrNone)
				{
				// If we didn't manage to create the list of clients we're delivering to,
				// we shouldn't deliver the message to the clients
				canDeliver = EFalse;
				}
			else
				{
				messageRecipients->TransactionId() = msg.TransactionId();
				}
			if (canDeliver)
				{
				while ( ( procId = iter++ ) != NULL )
					{
					LOG1(_L("\t\tprocess ID %d"), static_cast<TUint>(procId->ProcessId()));
					// Get the corresponding session.
					CRemConSession* const sess = TargetSession(procId->ProcessId());
					// NB The set of open sessions may have changed while the request 
					// was out on the TSP. If the TSP indicates a session that has 
					// gone away, then ignore that session. 
					if ( sess )
						{
						TInt err = KErrNone;
						TClientInfo* clientInfo = NULL;
						TRAP(err, clientInfo = new (ELeave) TClientInfo);
						if (err == KErrNone)
							{
							// If we didn't manage to create the TClientInfo, we shouldn't deliver to the client
							TRAP(err, DeliverCmdToClientL(msg, *sess));
							}
						if (err == KErrNone)
							{
							cmdDelivered = ETrue;
							// Add to the delivered information queue
							clientInfo->ProcessId() = procId->ProcessId();
							clientInfo->SecureId() = procId->SecureId();
							messageRecipients->Clients().AddLast(*clientInfo);
							}
						else
							{
							delete clientInfo;
							}
						// If we couldn't deliver an instance of the command to a 
						// target, there's not much we can do. 
						}
					}
				if ( messageRecipients->Clients().IsEmpty())
					{
					delete messageRecipients;
					}
				else
					{
					ASSERT_DEBUG(iMessageRecipientsList);
					iMessageRecipientsList->Messages().AddLast (*messageRecipients);
					}
				}
			}
	
		if ( !cmdDelivered)
			{
			// The command wasn't delivered to any clients
			// Tell bearer it won't be getting a response		
			
			SendReject(msg.Addr(), msg.InterfaceUid(), msg.OperationId(), msg.TransactionId());
			}
		// Destroy the message we've just dealt with.
		IncomingCmdPendingAddress().RemoveAndDestroy(msg);
		
		}

	iTspDropIncomingCommand = EFalse;
	
	if ( !IncomingCmdPendingAddress().IsEmpty() )
		{
		LOG(_L("\tmore incoming commands awaiting addressing..."));
		AddressIncomingCommand();
		}
	
	LOGINCOMINGCMDPENDINGADDRESS;
	LOGINCOMINGDELIVERED;
	}

TInt CRemConServer::MrctspoDoGetConnections(TSglQue<TRemConAddress>& aConnections)
	{
	LOG_FUNC;

	ASSERT_DEBUG(aConnections.IsEmpty());

	// Get an owned copy of the current set of connections in the system.
	CConnections* conns = NULL;
	TRAPD(err, conns = CConnections::CopyL(Connections()));
	LOG1(_L("\terr = %d"), err);
	if ( err == KErrNone )
		{
		// Pass these into aConnections, taking ownership of them.
		TSglQueIter<TRemConAddress>& iter = conns->SetToFirst();
		TRemConAddress* addr;
		while ( ( addr = iter++ ) != NULL )
			{
			conns->Remove(*addr);
			aConnections.AddLast(*addr);
			};
		delete conns;
		}
  
	return err;
	}

TInt CRemConServer::MrctspoSetLocalAddressedClient(const TUid& aBearerUid, const TClientInfo& aClientInfo)
	{
	LOG_FUNC;
	
	TRemConClientId id = KNullClientId;
	iSessionsLock.Wait();
	const TUint count = iSessions.Count();
	for(TUint i=0; i<count; i++)
		{
		CRemConSession* const sess = iSessions[i];
		ASSERT_DEBUG(sess);
		if(sess->ClientInfo().ProcessId() == aClientInfo.ProcessId())
			{
			id = sess->Id();
			break;
			}
		}
	iSessionsLock.Signal();
	
	if(id != KNullClientId)
		{
		ASSERT_DEBUG(iBearerManager);
		return iBearerManager->SetLocalAddressedClient(aBearerUid, id);
		}
	else
		{
		return KErrNotFound;
		}
	}

void CRemConServer::LoadTspL()
	{
	LOG_FUNC;

	// Instantiate one-and-only implementation of the target selector plugin 
	// interface.
	const TUid KUidTargetSelectorInterface = TUid::Uid(KRemConTargetSelectorInterfaceUid);
	RImplInfoPtrArray implementations;
	const TEComResolverParams noResolverParams;
	REComSession::ListImplementationsL(KUidTargetSelectorInterface, 
		noResolverParams, 
		KRomOnlyResolverUid,
		implementations);
	CleanupResetAndDestroyPushL(implementations);
	LOG1(_L("\tnumber of implementations of target selector plugin interface: %d"), implementations.Count());
	// We use ASSERT_ALWAYS here because this assumption is a crucial 
	// licensee-facing one- it's not a case of simply 'run the tests on a 
	// debug build and fix it if it's broken'.
	ASSERT_ALWAYS( implementations.Count() == 1 );
	CImplementationInformation* impl = implementations[0];
	ASSERT_DEBUG(impl);
	LOG(_L("\tloading TSP with:"));
	LOG1(_L("\t\timplementation uid 0x%08x"), impl->ImplementationUid());
	LOG1(_L("\t\tversion number %d"), impl->Version());
	TBuf8<KMaxName> buf8;
	buf8.Copy(impl->DisplayName());
	LOG1(_L8("\t\tdisplay name \"%S\""), &buf8);
	LOG1(_L("\t\tROM only %d"), impl->RomOnly());
	LOG1(_L("\t\tROM based %d"), impl->RomBased());
	iTsp = CRemConTargetSelectorPlugin::NewL(impl->ImplementationUid(), *this);
	CleanupStack::PopAndDestroy(&implementations);

	iTspIf = reinterpret_cast<MRemConTargetSelectorPluginInterface*>(
			iTsp->GetInterface(TUid::Uid(KRemConTargetSelectorInterface1))
		);
	
	iTspIf2 = reinterpret_cast<MRemConTargetSelectorPluginInterfaceV2*>(
			iTsp->GetInterface(TUid::Uid(KRemConTargetSelectorInterface2))
		);
	
	iTspIf3 = reinterpret_cast<MRemConTargetSelectorPluginInterfaceV3*>(
			iTsp->GetInterface(TUid::Uid(KRemConTargetSelectorInterface3))
		);
	
	iTspIf4 = reinterpret_cast<MRemConTargetSelectorPluginInterfaceV4*>(
			iTsp->GetInterface(TUid::Uid(KRemConTargetSelectorInterface4))
		);
	
	iTspIf5 = reinterpret_cast<MRemConTargetSelectorPluginInterfaceV5*>(
			iTsp->GetInterface(TUid::Uid(KRemConTargetSelectorInterface5))
		);
	
	// If the TSP doesn't implement the required interface, panic server 
	// startup.
	ASSERT_ALWAYS(iTspIf);
	
	// We always need a V4 interface to allow simpler handling by the 
	// bearers of bearer addressing so if we don't have a V4 interface
	// from the TSP itself use a stub object to implement default
	// behaviour.
	if(!iTspIf4)
		{
		iTspIf4 = &iTspIf4Stub;
		}
	}

void CRemConServer::SendCommand(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	
	if (aMsg.MsgType() == ERemConCommand)
		{
		LOGOUTGOINGCMDPENDINGTSP;
		
		OutgoingCmdPendingTsp().Append(aMsg);
		if ( !iTspHandlingOutgoingCommand )
			{				  
			TspOutgoingCommand();
			}
		
		LOGOUTGOINGCMDPENDINGTSP;
		}
	else
		{
		// Check the command is a notify command
		ASSERT_DEBUG(aMsg.MsgType() == ERemConNotifyCommand);
		
		if(iTspIf3)
			{
			OutgoingNotifyCmdPendingTsp().Append(aMsg);
			if ( !iTspHandlingOutgoingNotifyCommand )
				{				  
				TspOutgoingNotifyCommand();
				}
			}
		else
			{
			CRemConSession* const sess = Session(aMsg.SessionId());
			delete &aMsg;
			ASSERT_DEBUG(sess);
			sess->SendError() = KErrNotSupported;
			sess->CompleteSend();
			}
		}
	
	}

void CRemConServer::SendResponse(CRemConMessage& aMsg, CRemConSession& aSess)
	{
	LOG_FUNC;
	LOGINCOMINGDELIVERED;
	LOGOUTGOINGRSPPENDINGTSP;
	LOGOUTGOINGPENDINGSEND;

	CRemConMessage* response = &aMsg;
	CRemConMessage* newResponse = NULL;
	
	// Find the first command ('John') in the iIncomingDelivered queue with 
	// the same session ID, interface UID and operation ID as the response 
	// we're sending, and send the response to the same address that John came 
	// from.
	TSglQueIter<CRemConMessage>& iter = IncomingDelivered().SetToFirst();
	CRemConMessage* msg;
	TBool found = EFalse;
	while ( ( msg = iter++ ) != NULL )
		{
		if (	msg->SessionId() == aSess.Id()
			&&	msg->InterfaceUid() == response->InterfaceUid()
			&&	msg->OperationId() == response->OperationId()
			&&  (
					(msg->MsgSubType() == ERemConMessageDefault && response->MsgSubType() == ERemConMessageDefault)
				||  (msg->MsgSubType() == ERemConNotifyCommandAwaitingInterim && response->MsgSubType() == ERemConNotifyResponseInterim)
				||  (msg->MsgSubType() == ERemConNotifyCommandAwaitingChanged && response->MsgSubType() == ERemConNotifyResponseChanged)
				)
			)
			{
			LOG1(_L("\tfound a matching item in the incoming delivered commands log: [0x%08x]"), msg);
			found = ETrue;
			++aSess.NumRemotesToTry();

			// Set the right address and transaction id in the outgoing message
			response->Addr() = msg->Addr();
			response->TransactionId() = msg->TransactionId();
			
			if(msg->MsgType() == ERemConCommand)
				{
				// Check the normal command and response have the default subtype set
				ASSERT_DEBUG(msg->MsgSubType() == ERemConMessageDefault && response->MsgSubType() == ERemConMessageDefault);

				// Remove the item from the 'incoming delivered' queue now we've 
				// addressed a response using it.
				IncomingDelivered().RemoveAndDestroy(*msg);
				
				// At this point we need to ask the TSP if we can deliver the outgoing response,
				// if there is a TSP supporting the V2 interface. If there isn't, we should just deliver
				// the message to the bearer
				OutgoingRspPendingTsp().Append(*response);
				if (!iTspHandlingOutgoingResponse)
					{
					PermitOutgoingResponse();
					}
				}
			else
				{
				// Check the command is a notify command
				ASSERT_DEBUG(msg->MsgType() == ERemConNotifyCommand);
				
				// Check the command has a valid subtype for a notify command
				ASSERT_DEBUG(msg->MsgSubType() == ERemConNotifyCommandAwaitingInterim || msg->MsgSubType() == ERemConNotifyCommandAwaitingChanged);
				
				// Check the response has a valid subtype for a notify response
				ASSERT_DEBUG(response->MsgSubType() == ERemConNotifyResponseInterim || response->MsgSubType() == ERemConNotifyResponseChanged);

				switch(msg->MsgSubType())
					{
				case ERemConNotifyCommandAwaitingChanged:
					IncomingDelivered().RemoveAndDestroy(*msg);
					
					// As this is a changed notification, there could be several notifications outstanding
					// that should all be completed with this message.
					// We therefore need to take a copy of the message, but the response should not be
					// completed back to the client. We therefore set the Session ID of the new meessage to 0
					
					TRAPD(err, newResponse = CRemConMessage::CopyL(*response));
					if (err != KErrNone)
						{
						newResponse = NULL;
						}
					break;
				case ERemConNotifyCommandAwaitingInterim:
					msg->MsgSubType() = ERemConNotifyCommandAwaitingChanged;
					break;
					}
				OutgoingRspPendingSend().Append(*response);
				response = newResponse;
				}
				if (!newResponse)
					{
					break;
					}
			}
		}
	
	if (newResponse)
		{
		delete newResponse;
		}


	TSglQueIter<CRemConMessage>& rspIter = OutgoingRspPendingSend().SetToFirst();
	while ((msg = rspIter++) != NULL)
		{
		OutgoingRspPendingSend().Remove(*msg);
		CompleteSendResponse(*msg, aSess);
		}

	
	// If the command was not found, then the app has sent a response to a
	// non-existant command.  It may do this in good intention if the server 
	// has transparently died and been restarted between the app's reception
	// of the command and it sending its response, so we can't panic it.
	// Just drop the message.
	if ( !found )
		{
		// Complete the message with KErrNone.  We have done all we can with
		// it.  Any other error may encourage retries from the application,
		// which would be useless in this situation.
		aSess.NumRemotes() = 0;
		aSess.SendError() = KErrNone;
		delete &aMsg;
		}
	
	if (aSess.NumRemotesToTry() == 0)
		{
		aSess.CompleteSend();
		}

	LOGOUTGOINGRSPPENDINGTSP;
	LOGINCOMINGDELIVERED;
	LOGOUTGOINGPENDINGSEND;
	}

void CRemConServer::CompleteSendResponse(CRemConMessage& aMsg, CRemConSession& aSess)
	{
	LOG_FUNC;
	LOGOUTGOINGPENDINGSEND;
	// If the bearer-level connection exists, then send the message. 
	// Otherwise, queue the message and request the connection to come 
	// up. The message will be sent when ConnectConfirm or 
	// ConnectIndicate is called (assuming no error).
	ASSERT_DEBUG(aMsg.MsgType() == ERemConResponse);
	ASSERT_DEBUG(iBearerManager);
	// If we're connected (and not mid-connect/disconnect) we can send immediately,
	// otherwise we queue and try to bring up the connection.
	switch ( ConnectionState(aMsg.Addr()) )
		{
		case EConnected:
			{
			// We're already connected
			// If the bearer couldn't send, we need to error the client.
			TInt err = iBearerManager->Send(aMsg);
			// Complete client's message. Bearer-level error means the 
			// response got sent to zero remotes- bearer-level success 
			// means it got sent to precisely 1.
			--aSess.NumRemotesToTry();
			if (err == KErrNone)
				{
				++aSess.NumRemotes();
				}
			aSess.SendError() = err;
			if (aSess.NumRemotesToTry() == 0)
				{
				aSess.CompleteSend();
				}
			// We've now finished with the response.
			delete &aMsg;
			break;
			}
		case EDisconnected:
			{
			// We're not connected. 
			// Queue the response...
			OutgoingPendingSend().Append(aMsg);
			// ... and ask the bearer to establish a connection. If we 
			// couldn't then complete the client's message and clean up.
			TInt err = iBearerManager->Connect(aMsg.Addr());
			if ( err != KErrNone )
				{
				OutgoingPendingSend().RemoveAndDestroy(aMsg);
				aSess.NumRemotes() = 0;
				--aSess.NumRemotesToTry();
				aSess.SendError() = err;
				aSess.CompleteSend();
				}
			break;
			}
		case EConnecting:
		case EDisconnecting:
			{
			// Just queue if connection is coming up or going down.
			//
			// If connection is coming up, the message will be
			// sent when ConnectConfirm fires.
			//
			// If connection is going down: When CBearerManager gets
			// the DisconnectConfirm it calls us back at RemoveConnection,
			// whereat we will reconnect the bearer that was mid-disconnect;
			// the message will be sent when that completes.
			OutgoingPendingSend().Append(aMsg);
			break;
			}
		default:
			{
			DEBUG_PANIC_LINENUM;
			break;
			}
		}
	LOGOUTGOINGPENDINGSEND;
	}


void CRemConServer::SendReject(TRemConAddress aAddr, TUid aInterfaceUid, TUint aOperationId, TUint aTransactionId)
	{
	LOG_FUNC;
	LOGOUTGOINGPENDINGSEND;
	// If the bearer-level connection exists, then send the message. 
	// Otherwise, queue the message and request the connection to come 
	// up. The message will be sent when ConnectConfirm or 
	// ConnectIndicate is called (assuming no error).
	
	CRemConMessage* rejectMsg = NULL;
	RBuf8 data;
	data = KNullDesC8;
	TRAPD(err, rejectMsg = CRemConMessage::NewL(aAddr, KNullClientId, ERemConReject, ERemConMessageDefault, aInterfaceUid, aOperationId, data, 0, aTransactionId));

	if ( err == KErrNone)
		{
		// SendReject will always take ownership of rejectMsg
		
		ASSERT_DEBUG(iBearerManager);
		switch ( ConnectionState(rejectMsg->Addr()) )
			{
			case EConnected:
				{
				// We're already connected
				err = iBearerManager->Send(*rejectMsg);
				// We've now finished with the response.
				delete rejectMsg;
				break;
				}
			case EDisconnected:
				{
				// We're not connected. 
				// Queue the response...
				OutgoingPendingSend().Append(*rejectMsg);
				// ... and ask the bearer to establish a connection. If we 
				// couldn't then clean up.
				err = iBearerManager->Connect (rejectMsg->Addr ());
				if ( err != KErrNone)
					{
					OutgoingPendingSend().RemoveAndDestroy(*rejectMsg);
					}
				break;
				}
			case EConnecting:
			case EDisconnecting:
				{
				// Just queue if connection is coming up or going down.
				//
				// If connection is coming up, the message will be
				// sent when ConnectConfirm fires.
				//
				// If connection is going down: When CBearerManager gets
				// the DisconnectConfirm it calls us back at RemoveConnection,
				// whereat we will reconnect the bearer that was mid-disconnect;
				// the message will be sent when that completes.
				OutgoingPendingSend().Append(*rejectMsg);
				break;
				}
			default:
				{
				DEBUG_PANIC_LINENUM;
				break;
				}
			}
		}
	LOGOUTGOINGPENDINGSEND;
	}

void CRemConServer::SendCancel(CRemConSession& aSess)
	{
	LOG_FUNC;
	LOGOUTGOINGCMDPENDINGTSP;
	
	TSglQueIter<CRemConMessage>& iter = OutgoingCmdPendingTsp().SetToFirst();
	CRemConMessage* msg;
	TBool first = ETrue;
	while ( ( msg = iter++ ) != NULL )
		{
		// A client can only have one send outstanding at once, so there can 
		// only be one message on the queue belonging to it.
		if ( msg->SessionId() == aSess.Id() )
			{
			ASSERT_DEBUG(msg->MsgType() == ERemConCommand);
			LOG1(_L("\tfound a command belonging to this client in the outgoing pending TSP queue: [0x%08x]"), msg);

			// If the TSP is currently handling this command, we must stop 
			// them! The one the TSP is handling, if any, is always the first 
			// in the queue.
			if ( iTspHandlingOutgoingCommand && first )
				{
				LOG(_L("\tTSP is processing this command- cancel it"));
				ASSERT_DEBUG(iTspIf);
				iTspIf->CancelOutgoingCommand();
				iTspHandlingOutgoingCommand = EFalse;
				iTspConnections.Reset();
				}
			// Can now destroy the message.
			OutgoingCmdPendingTsp().RemoveAndDestroy(*msg);
			}
		first = EFalse;
		}

	LOGOUTGOINGCMDPENDINGTSP;
	}

void CRemConServer::NewResponse(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	LOGOUTGOINGSENT;
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;

	// Look through the 'outgoing sent' queue for items with the same address, 
	// interface UID, operation ID and transaction ID. We assume there will 
	// only be one such item.
	TBool sentCommandFound = EFalse;
	TSglQueIter<CRemConMessage>& iter = OutgoingSent().SetToFirst();
	CRemConMessage* cmd;
	while ( ( cmd = iter++ ) != NULL )
		{
		if (	cmd->Addr() == aMsg.Addr()
			&&	cmd->InterfaceUid() == aMsg.InterfaceUid()
			&&	cmd->OperationId() == aMsg.OperationId()
			&&	cmd->TransactionId() == aMsg.TransactionId()
			)
			{
			LOG1(_L("\tfound a matching item in the sent commands log: [0x%08x]"), cmd);
			sentCommandFound = ETrue;
			CRemConSession* const session = Session(cmd->SessionId());
			// When sessions close, their messages are removed from the logs, 
			// so the session here _should_ exist.
			ASSERT_DEBUG(session);
			aMsg.SessionId() = cmd->SessionId();
			(void) DeliverMessageToClient(aMsg, *session);
			// Remove the found item from the sent message log.
			OutgoingSent().RemoveAndDestroy(*cmd);
			break;
			}
		}
	if ( !sentCommandFound )
		{
		// Either:
		// (a) the remote is buggy (sent a response when there wasn't an 
		// originating command), 
		// (b) the client closed their controller session before the response 
		// came back (this cleans up the 'sent commands' log)
		LOG(_L("\tno matching item found in sent commands log- response dropped"));
		delete &aMsg;
		}

	LOGOUTGOINGSENT;
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;
	}

void CRemConServer::NewNotifyResponse(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	LOGOUTGOINGSENT;
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;

	// Look through the 'outgoing sent' queue for items with the same address, 
	// interface UID, operation ID and transaction ID. We assume there will 
	// only be one such item.
	TBool sentCommandFound = EFalse;
	TSglQueIter<CRemConMessage>& iter = OutgoingSent().SetToFirst();
	CRemConMessage* cmd;
	TRemConMessageSubType submessagetype;
	submessagetype = aMsg.MsgSubType();
	
	while ( ( cmd = iter++ ) != NULL )
		{
		if (	cmd->Addr() == aMsg.Addr()
			&&	cmd->InterfaceUid() == aMsg.InterfaceUid()
			&&	cmd->OperationId() == aMsg.OperationId()
			&&	cmd->TransactionId() == aMsg.TransactionId()
			)
			{
			LOG1(_L("\tfound a matching item in the sent commands log: [0x%08x]"), cmd);
			sentCommandFound = ETrue;
			CRemConSession* const session = Session(cmd->SessionId());
			// When sessions close, their messages are removed from the logs, 
			// so the session here _should_ exist.
			ASSERT_DEBUG(session);
			aMsg.SessionId() = cmd->SessionId();			
			(void) DeliverMessageToClient(aMsg, *session);
			
			//Do not remove the command if it is an interim response because
			//we need to wait for the changed response.
			//If the changed response received or error occurs, then remove
			//the command from the sent message log
			if (submessagetype != ERemConNotifyResponseInterim)
				{
				OutgoingSent().RemoveAndDestroy(*cmd);
				}

			break;
			}
		}
	if ( !sentCommandFound )
		{
		// Either:
		// (a) the remote is buggy (sent a response when there wasn't an 
		// originating command), 
		// (b) the client closed their controller session before the response 
		// came back (this cleans up the 'sent commands' log)
		LOG(_L("\tno matching item found in sent commands log- response dropped"));
		delete &aMsg;
		}

	LOGOUTGOINGSENT;
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;
	}

void CRemConServer::NewCommand(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	LOGINCOMINGCMDPENDINGADDRESS;

	IncomingCmdPendingAddress().Append(aMsg);
	if ( !iTspAddressingIncomingCommand)
		{
		AddressIncomingCommand();
		}

	LOGINCOMINGCMDPENDINGADDRESS;
	}

void CRemConServer::NewNotifyCommand(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	LOGINCOMINGNOTIFYCMDPENDINGADDRESS;
	if (!iTspIf2)
		{
		SendReject(aMsg.Addr(), aMsg.InterfaceUid(), aMsg.OperationId(), aMsg.TransactionId());
		delete &aMsg;
		}
	else
		{
		IncomingNotifyCmdPendingAddress().Append(aMsg);
		if ( !iTspAddressingIncomingNotifyCommand && !iTspReAddressingIncomingNotifyCommands)
			{
			AddressIncomingNotifyCommand();
			}
		}

	LOGINCOMINGNOTIFYCMDPENDINGADDRESS;
	}

#ifdef __FLOG_ACTIVE
void CRemConServer::LogOutgoingNotifyCmdPendingTsp() const
	{
	LOG(_L("Logging outgoing notify pending TSP commands"));
	FTRACE(const_cast<CRemConServer*>(this)->OutgoingNotifyCmdPendingTsp().LogQueue();)
	}

void CRemConServer::LogOutgoingCmdPendingTsp() const
	{
	LOG(_L("Logging outgoing pending TSP commands"));
	FTRACE(const_cast<CRemConServer*>(this)->OutgoingCmdPendingTsp().LogQueue();)
	}

void CRemConServer::LogOutgoingRspPendingTsp() const
	{
	LOG(_L("Logging outgoing pending TSP responses"));
	FTRACE(const_cast<CRemConServer*>(this)->OutgoingRspPendingTsp().LogQueue();)
	}


void CRemConServer::LogOutgoingPendingSend() const
	{
	LOG(_L("Logging outgoing pending send"));
	FTRACE(const_cast<CRemConServer*>(this)->OutgoingPendingSend().LogQueue();)
	}

void CRemConServer::LogOutgoingSent() const
	{
	LOG(_L("Logging outgoing sent"));
	FTRACE(const_cast<CRemConServer*>(this)->OutgoingSent().LogQueue();)
	}

void CRemConServer::LogIncomingCmdPendingAddress() const
	{
	LOG(_L("Logging incoming pending address commands"));
	FTRACE(const_cast<CRemConServer*>(this)->IncomingCmdPendingAddress().LogQueue();)
	}

void CRemConServer::LogIncomingNotifyCmdPendingAddress() const
	{
	LOG(_L("Logging incoming pending address notify commands"));
	FTRACE(const_cast<CRemConServer*>(this)->IncomingNotifyCmdPendingAddress().LogQueue();)
	}

void CRemConServer::LogIncomingNotifyCmdPendingReAddress() const
	{
	LOG(_L("Logging incoming pending readdress notify commands"));
	FTRACE(const_cast<CRemConServer*>(this)->IncomingNotifyCmdPendingReAddress().LogQueue();)
	}

void CRemConServer::LogIncomingPendingDelivery() const
	{
	LOG(_L("Logging incoming pending delivery"));
	FTRACE(const_cast<CRemConServer*>(this)->IncomingPendingDelivery().LogQueue();)
	}

void CRemConServer::LogIncomingDelivered() const
	{
	LOG(_L("Logging incoming delivered"));
	FTRACE(const_cast<CRemConServer*>(this)->IncomingDelivered().LogQueue();)
	}
#endif // __FLOG_ACTIVE

CRemConSession* CRemConServer::Session(TUint aSessionId) const
	{
	CRemConSession* sess = NULL;

	iSessionsLock.Wait();
	const TUint count = iSessions.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConSession* const temp = iSessions[ii];
		ASSERT_DEBUG(temp);
		if ( temp->Id() == aSessionId )
			{
			sess = temp;
			break;
			}
		}
	iSessionsLock.Signal();

	return sess;
	}

CRemConSession* CRemConServer::TargetSession(TProcessId aProcessId) const
	{
	CRemConSession* sess = NULL;

	iSessionsLock.Wait();
	const TUint count = iSessions.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConSession* const temp = iSessions[ii];
		ASSERT_DEBUG(temp);
		if (	temp->ClientInfo().ProcessId() == aProcessId 
			&&	temp->Type() == ERemConClientTypeTarget )
			{
			sess = temp;
			break;
			}
		}
	iSessionsLock.Signal();

	return sess;
	}

MRemConConverterInterface* CRemConServer::Converter(TUid aInterfaceUid, 
							   TUid aBearerUid) const
	{
	ASSERT_DEBUG(iConverterManager);
	return iConverterManager->Converter(aInterfaceUid, aBearerUid);
	}

MRemConConverterInterface* CRemConServer::Converter(const TDesC8& aInterfaceData, 
							   TUid aBearerUid) const
	{
	ASSERT_DEBUG(iConverterManager);
	return iConverterManager->Converter(aInterfaceData, aBearerUid);
	}

void CRemConServer::ReceiveRequest(CRemConSession& aSession)
	{
	LOG_FUNC;
	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;

	// Find the first message in IncomingPendingDelivery for this session.
	TSglQueIter<CRemConMessage>& iter = IncomingPendingDelivery().SetToFirst();
	CRemConMessage* msg;
	while ( ( msg = iter++ ) != NULL )
		{
		if ( msg->SessionId() == aSession.Id() )
			{
			TInt err = aSession.WriteMessageToClient(*msg);
			IncomingPendingDelivery().Remove(*msg);
			if ( msg->MsgType() == ERemConCommand || msg->MsgType() == ERemConNotifyCommand)
				{
				if (err == KErrNone )
					{
					// We'll need to remember it for the response coming back.
					IncomingDelivered().Append(*msg); 
					}
				else
					{
					// Tell bearer it won't be getting a response
					CMessageRecipients* messageRecipients = iMessageRecipientsList->Message (msg->TransactionId ());

					// If we aren't returned a client list, this means that the message has been delivered elsewhere
					if (messageRecipients)
						{
						messageRecipients->RemoveAndDestroyClient (aSession.ClientInfo ());

						if ( messageRecipients->Clients().IsEmpty ())
							{
							iMessageRecipientsList->Messages().Remove (*messageRecipients);
							delete messageRecipients;

							SendReject(msg->Addr(), msg->InterfaceUid(), msg->OperationId(), msg->TransactionId());
							}
						}
					
					// 'Take ownership' of it by destroying it- it's finished with.
					delete msg;
					}
				}
			else
				{
				// 'Take ownership' of it by destroying it- it's finished with.
				delete msg;				
				}
			
			break;
			}
		}

	LOGINCOMINGPENDINGDELIVERY;
	LOGINCOMINGDELIVERED;
	}

TBool CRemConServer::FindDuplicateNotify(CRemConMessage& aMsg)
	{
	ASSERT_DEBUG(aMsg.MsgType() == ERemConNotifyCommand);
	TSglQueIter<CRemConMessage>& deliveredIter = IncomingDelivered().SetToFirst();
	CRemConMessage* msg;
	while ((msg = deliveredIter++) != NULL)
		{
		if (msg->MsgType() == ERemConNotifyCommand
				&& msg->Addr() == aMsg.Addr()
				&& msg->InterfaceUid() == aMsg.InterfaceUid()
				&& msg->OperationId() == aMsg.OperationId()
				&& msg->Client() == aMsg.Client())
			{
			return ETrue;
			}
		}
	TSglQueIter<CRemConMessage>& pendingDeliveryIter = IncomingPendingDelivery().SetToFirst();
	while ((msg = pendingDeliveryIter++) != NULL)
		{
		if (msg->MsgType() == ERemConNotifyCommand
				&& msg->Addr() == aMsg.Addr()
				&& msg->InterfaceUid() == aMsg.InterfaceUid()
				&& msg->OperationId() == aMsg.OperationId()
				&& msg->Client() == aMsg.Client())
			{
			return ETrue;
			}
		}
	return EFalse;
	}

TInt UidCompare(const TUid& aFirst, const TUid& aSecond)
	{
	if(aFirst.iUid < aSecond.iUid)
		{
		return -1;
		}
	else if(aFirst.iUid == aSecond.iUid)
		{
		return 0;
		}
	else
		{
		return 1;
		}
	}

/**
Collect all supported interfaces of controller clients.  

@param aSupportedInterfaces An empty RArray which will be populated with the current
		supported interfaces.  Ownership is retained by the caller.
@return KErrNone if any interfaces were able to be retrieved.  This does not 
		imply that every session's interfaces were able to be retrieved.
		KErrNoMemory if no interfaces could be retrived.
*/
TInt CRemConServer::ControllerSupportedInterfaces(RArray<TUid>& aSupportedInterfaces)
	{
	LOG_FUNC
	ASSERT_DEBUG(aSupportedInterfaces.Count() == 0);
	
	TLinearOrder<TUid> uidCompare(&UidCompare);
	RArray<TUid> sessionFeatures;
	TInt err = KErrNone;
	for(TInt i=0; i<iSessions.Count(); i++)
		{
		ASSERT_DEBUG(iSessions[i]);
		if(iSessions[i]->Type() == ERemConClientTypeController)
			{
			err = iSessions[i]->SupportedInterfaces(sessionFeatures);
			ASSERT_DEBUG(err == KErrNone || err == KErrNoMemory);
			
			if(!err)
				{
				for(TInt j=0; j<sessionFeatures.Count(); j++)
					{
					// Ignore failure here, we're trying this best effort
					// InsertInOrder is used rather than just bunging the
					// interface on the end as we want no duplicates 
					(void)aSupportedInterfaces.InsertInOrder(sessionFeatures[j], uidCompare);
					}
				}
			sessionFeatures.Reset();
			}
		}
	
	if(aSupportedInterfaces.Count() > 0)
		{
		return KErrNone;
		}
	else
		{
		return KErrNoMemory;
		}
	}

CMessageQueue& CRemConServer::OutgoingCmdPendingTsp()
	{
	ASSERT_DEBUG(iOutgoingCmdPendingTsp);
	return *iOutgoingCmdPendingTsp;
	}

CMessageQueue& CRemConServer::OutgoingNotifyCmdPendingTsp()
	{
	ASSERT_DEBUG(iOutgoingNotifyCmdPendingTsp);
	return *iOutgoingNotifyCmdPendingTsp;
	}

CMessageQueue& CRemConServer::OutgoingRspPendingTsp()
	{
	ASSERT_DEBUG(iOutgoingRspPendingTsp);
	return *iOutgoingRspPendingTsp;
	}

CMessageQueue& CRemConServer::OutgoingRspPendingSend()
	{
	ASSERT_DEBUG(iOutgoingRspPendingSend);
	return *iOutgoingRspPendingSend;
	}

CMessageQueue& CRemConServer::OutgoingPendingSend()
	{
	ASSERT_DEBUG(iOutgoingPendingSend);
	return *iOutgoingPendingSend;
	}

CMessageQueue& CRemConServer::OutgoingSent()
	{
	ASSERT_DEBUG(iOutgoingSent);
	return *iOutgoingSent;
	}

CMessageQueue& CRemConServer::IncomingCmdPendingAddress()
	{
	ASSERT_DEBUG(iIncomingCmdPendingAddress);
	return *iIncomingCmdPendingAddress;
	}

CMessageQueue& CRemConServer::IncomingNotifyCmdPendingAddress()
	{
	ASSERT_DEBUG(iIncomingNotifyCmdPendingAddress);
	return *iIncomingNotifyCmdPendingAddress;
	}

CMessageQueue& CRemConServer::IncomingPendingDelivery()
	{
	ASSERT_DEBUG(iIncomingPendingDelivery);
	return *iIncomingPendingDelivery;
	}

CMessageQueue& CRemConServer::IncomingDelivered()
	{
	ASSERT_DEBUG(iIncomingDelivered);
	return *iIncomingDelivered;
	}


CMessageQueue& CRemConServer::IncomingNotifyCmdPendingReAddress()
	{
	ASSERT_DEBUG(iIncomingNotifyCmdPendingReAddress);
	return *iIncomingNotifyCmdPendingReAddress;
	}

CConnections& CRemConServer::Connections()
	{
	ASSERT_DEBUG(iConnections);
	return *iConnections;
	}

TInt CRemConServer::HandleConnection(const TRemConAddress& aAddr, TInt aError)
	{
	LOG_FUNC;
	LOG2(_L("\taError = %d, aAddr.BearerUid = 0x%08x"), aError, aAddr.BearerUid());
	LOGREMOTES;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;

	// Try to update the record of the current connection status.
	// If this fails (it involves memory allocation) we need to return the 
	// error so the connection will be dropped.
	if ( aError == KErrNone )
		{
		ASSERT_DEBUG(iConnections);
		TRemConAddress* addr = new TRemConAddress;
		if ( !addr )
			{
			aError = KErrNoMemory;
			}
		else
			{
			*addr = aAddr;
			iConnections->Append(*addr);
			}
		}

	// If we have a real new connection and we could handle it, aError is 
	// now KErrNone. In this case, sessions' notifications need completing.
	if ( aError == KErrNone )
		{
		iSessionsLock.Wait();
		const TUint count = iSessions.Count();
		for ( TUint ii = 0 ; ii < count ; ++ii )
			{
			ASSERT_DEBUG(iSessions[ii]);
			iSessions[ii]->ConnectionsChanged();
			}
		iSessionsLock.Signal();
		}

	// Complete the specific client request(s) that caused a ConnectRequest on 
	// the bearer. Tell all sessions- they remember the address they wanted to 
	// connect to, and will filter on the address we give them. NB This 
	// function is called by ConnectIndicate as well as by ConnectConfirm, but 
	// the client doesn't care which end brought the connection up. 
	const TUint count = Sessions().Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		ASSERT_DEBUG(Sessions()[ii]);
		Sessions()[ii]->CompleteConnect(aAddr, aError);
		}

	// Any messages waiting on OutgoingPendingSend for this connection need to 
	// be handled.
	TSglQueIter<CRemConMessage>& iter = OutgoingPendingSend().SetToFirst();
	CRemConMessage* msg;
	TBool moveToSent = EFalse;
	while ( ( msg = iter++ ) != NULL )
		{
		if ( msg->Addr() == aAddr )
			{
			CRemConSession* const sess = Session(msg->SessionId());
			// The session should exist- if it doesn't then this message 
			// wasn't cleaned from OutgoingPendingSend correctly when the 
			// session closed. The exceptions are Reject, which can be put
			// on the queue without a session, and notify changed responses when they are being
			// delivered to multiple controllers
			ASSERT_DEBUG(sess || msg->MsgType() == ERemConReject || msg->MsgSubType() == ERemConNotifyResponseChanged);

			if ( aError == KErrNone)
				{
				// We have a connection!
				ASSERT_DEBUG(iBearerManager);
				TInt err = iBearerManager->Send(*msg);
				if ( err == KErrNone 
				    && ((msg->MsgType() == ERemConCommand)||(msg->MsgType() == ERemConNotifyCommand)))
					{
					// If the send succeeded and it was a command, we move the 
					// message to the 'sent' log. Otherwise, it's simply 
					// deleted because we've finished with it.
					moveToSent = ETrue;
					}
				if ( sess)
					{
					if ( err == KErrNone)
						{
						++sess->NumRemotes();
						}
					else
						{
						sess->SendError ()= err;
						}
					}
				}
			else
				{
				// No connection, remember the error.
				if ( sess)
					{
					sess->SendError ()= aError;
					}
				}
			if ( sess)
				{
				--sess->NumRemotesToTry();

				// If we have now dealt with all the messages on the 
				// OutgoingPendingSend queue for this session, we can complete 
				// their send. In this case we should by now have collected the 
				// number of remotes and the send error. (NB A client can only 
				// have one send outstanding at any one time; a client may have 
				// more than one message on the 'pending send' queue if the TSP 
				// said to send to more than one remote.)
				// If the message is a notify then it can only have been sent to
				// one Remote so the NumRemotesToTry is not used
				if(msg->MsgType() == ERemConNotifyCommand)
					{
					sess->CompleteSendNotify();
					}
				else if ( sess->NumRemotesToTry ()== 0)
					{
					sess->CompleteSend ();
					}
				}

			if ( moveToSent)
				{
				OutgoingPendingSend().Remove (*msg);
				OutgoingSent().Append (*msg);
				}
			else
				{
				OutgoingPendingSend().RemoveAndDestroy (*msg);
				}
			}
		}

	LOGREMOTES;
	LOGOUTGOINGPENDINGSEND;
	LOGOUTGOINGSENT;
	LOG1(_L("\taError = %d"), aError);
	return aError;
	}

void CRemConServer::RemoveConnection(const TRemConAddress& aAddr)
	{
	LOG_FUNC;
	LOG1(_L("\taAddr.BearerUid = 0x%08x"), aAddr.BearerUid());
	LOGREMOTES;

	// Remove the address from the record of current connections and inform 
	// the sessions so they can complete outstanding connection status 
	// notifications.

	ASSERT_DEBUG(iConnections);
	TSglQueIter<TRemConAddress>& addrIter = iConnections->SetToFirst();
	TRemConAddress* addr;
	while ( ( addr = addrIter++ ) != NULL )
		{
		if ( *addr == aAddr )
			{
			iConnections->Remove(*addr);
			delete addr;
			}
		}

	iSessionsLock.Wait();
	const TUint count = iSessions.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		ASSERT_DEBUG(iSessions[ii]);
		iSessions[ii]->ConnectionsChanged();
		}
	iSessionsLock.Signal();
	
	// If there are any messages waiting on OutgoingPendingSend for this connection,
	// we re-connect it - they'll be picked up in HandleConnection above.

	TSglQueIter<CRemConMessage> iter = OutgoingPendingSend().SetToFirst();
	CRemConMessage* msg;
	TBool needToReconnect = false;
	while ( ( msg = iter++ ) != NULL )
		{
		if (msg->Addr() == aAddr)
			{
			needToReconnect = true;
			break;
			}
		}
	
	if (needToReconnect)
		{
		ASSERT_DEBUG(iBearerManager);
		TInt err = iBearerManager->Connect(aAddr);
		if ( err != KErrNone )
			{
			// This fails if:
			// 1. we're already connecting (in which case, we don't care)
			// 2. we can't add aAddr to the connecting list
			// The semantics of this observer don't let us return an error or leave, so
			// we can't do much about it here. Log it, and the next command will
			// invoke Connect from a better situation.
			LOG1(_L("\tFailed to re-connect bearer after connection removed: %d"), err);
			}
		}

	LOGREMOTES;
	}

TConnectionState CRemConServer::ConnectionState(const TRemConAddress& aAddr)
	{
	LOG_FUNC;
	LOG1(_L("\taaAddr.BearerUid = 0x%08x"), aAddr.BearerUid());
	
	TConnectionState ret;

	// Because 'connection state' knowledge is spread across the system
	// we have asserts in following if statement to ensure that our state
	// is consistent

	// Check if connecting
	ASSERT_DEBUG(iBearerManager);
	if ( iBearerManager->IsConnecting(aAddr) )
		{
		ASSERT_DEBUG(!iBearerManager->IsDisconnecting(aAddr));
		ASSERT_DEBUG(!Connections().Find(aAddr)); 
		ret = EConnecting;
		}
	// Check if disconnecting
	else if ( iBearerManager->IsDisconnecting(aAddr) )
		{
		ASSERT_DEBUG(!iBearerManager->IsConnecting(aAddr));
		// NB Connection remains in connections list until we get DisconnectConfirm
		ASSERT_DEBUG(Connections().Find(aAddr));
		ret = EDisconnecting;	
		}
	// Check if connected
	else if ( Connections().Find(aAddr) )
		{
		ASSERT_DEBUG(!iBearerManager->IsConnecting(aAddr));
		ASSERT_DEBUG(!iBearerManager->IsDisconnecting(aAddr));
		ret = EConnected;
		}
	// otherwise it's disconnected
	else
		{
		ret = EDisconnected;
		}
		
	LOG1(_L("\tret(connection state) = %d"), ret);
	return ret;
	}

void CRemConServer::CommandExpired(TUint aTransactionId)
	{
	LOG_FUNC;

	CRemConMessage* msg;

	TBool first = ETrue;
	
	TSglQueIter<CRemConMessage> addressCommandIter = IncomingCmdPendingAddress().SetToFirst();
	
	while ((msg = addressCommandIter++) != NULL)
		{
		if (msg->TransactionId() == aTransactionId)
			{
			IncomingCmdPendingAddress().RemoveAndDestroy(*msg);
			if (first && iTspAddressingIncomingCommand)
				{
				iTspDropIncomingCommand = ETrue;
				}
			}
		first = EFalse;
		}
	
	TSglQueIter<CRemConMessage> addressNotifyIter = IncomingNotifyCmdPendingAddress().SetToFirst();

	first = ETrue;
	
	while ((msg = addressNotifyIter++) != NULL)
		{
		if (msg->TransactionId() == aTransactionId)
			{
			IncomingNotifyCmdPendingAddress().RemoveAndDestroy(*msg);
			if (first && iTspAddressingIncomingNotifyCommand)
				{
				iTspDropIncomingNotifyCommand = ETrue;
				}
			}
		first = EFalse;
		}

	TSglQueIter<CRemConMessage> reAddressNotifyIter = IncomingNotifyCmdPendingReAddress().SetToFirst();

	
	while ((msg = reAddressNotifyIter++) != NULL)
		{
		if (msg->TransactionId() == aTransactionId)
			{
			IncomingNotifyCmdPendingReAddress().RemoveAndDestroy(*msg);
			if (first && iTspReAddressingIncomingNotifyCommands)
				{
				iTspDropIncomingNotifyCommand = ETrue;
				}

			}
		first = EFalse;
		}
	
	TSglQueIter<CRemConMessage> pendingDeliveryIter = IncomingPendingDelivery().SetToFirst();
	
	while ((msg = pendingDeliveryIter++) != NULL)
		{
		if (msg->TransactionId() == aTransactionId)
			{
			IncomingPendingDelivery().RemoveAndDestroy(*msg);
			}
		}
	
	TSglQueIter<CRemConMessage> deliveredIter = IncomingDelivered().SetToFirst();
	
	while ((msg = deliveredIter++) != NULL)
		{
		if (msg->TransactionId() == aTransactionId)
			{
			IncomingDelivered().RemoveAndDestroy(*msg);
			}
		}
	ASSERT_DEBUG(iMessageRecipientsList);
	iMessageRecipientsList->RemoveAndDestroyMessage(aTransactionId);
	}

TClientInfo* CRemConServer::ClientIdToClientInfo(TRemConClientId aId)
	{
	TClientInfo* clientInfo = NULL;
	
	iSessionsLock.Wait();
	const TUint count = iSessions.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConSession* const sess = iSessions[ii];
		ASSERT_DEBUG(sess);
		if (sess->Id() == aId)
			{
			clientInfo = &sess->ClientInfo();
			break;
			}
		}
	iSessionsLock.Signal();
	
	return clientInfo;
	}

TInt CRemConServer::SupportedInterfaces(const TRemConClientId& aId, RArray<TUid>& aUids)
	{
	iSessionsLock.Wait();
	const TUint count = iSessions.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConSession* const sess = iSessions[ii];
		ASSERT_DEBUG(sess);
		if (sess->Id() == aId)
			{
			iSessionsLock.Signal();
			return sess->SupportedInterfaces(aUids);
			}
		}
	iSessionsLock.Signal();
	
	return KErrNotFound;
	}

TInt CRemConServer::SupportedOperations(const TRemConClientId& aId, TUid aInterfaceUid, RArray<TUint>& aOperations)
	{
	iSessionsLock.Wait();
	const TUint count = iSessions.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConSession* const sess = iSessions[ii];
		ASSERT_DEBUG(sess);
		if (sess->Id() == aId)
			{
			iSessionsLock.Signal();
			return sess->SupportedOperations(aInterfaceUid, aOperations);
			}
		}
	iSessionsLock.Signal();
	
	return KErrNotFound;
	}

void CRemConServer::SetRemoteAddressedClient(const TUid& aBearerUid, const TRemConClientId& aId)
	{
	LOG_FUNC
	
	TClientInfo* clientInfo = ClientIdToClientInfo(aId);
	// Bearer must supply valid client id
	ASSERT_DEBUG(clientInfo);

	ASSERT_DEBUG(iTspIf4);
	iTspIf4->SetRemoteAddressedClient(aBearerUid, *clientInfo);
	}

TInt CRemConServer::RegisterLocalAddressedClientObserver(const TUid& aBearerUid)
	{
	return iTspIf5 ? iTspIf5->RegisterLocalAddressedClientObserver(aBearerUid) : KErrNotSupported;
	}

TInt CRemConServer::UnregisterLocalAddressedClientObserver(const TUid& aBearerUid)
	{
	return iTspIf5 ? iTspIf5->UnregisterLocalAddressedClientObserver(aBearerUid) : KErrNotSupported;
	}

TRemConClientId CRemConServer::ClientIdByProcessId(TProcessId aProcessId)
	{
	LOG_FUNC
	TRemConClientId ret = KNullClientId;
	iSessionsLock.Wait();
	CRemConSession* session = TargetSession(aProcessId);
	if(session)
		{
		ret = session->Id();
		}
	iSessionsLock.Signal();
	return ret;
	}

void CRemConServer::BulkInterfacesForClientL(TRemConClientId aId, RArray<TUid>& aUids)
	{
	LOG_FUNC
	iSessionsLock.Wait();
	CleanupSignalPushL(iSessionsLock);
	CRemConSession* session = Session(aId);
	if(!session)
		{
		LEAVEL(KErrNotFound);
		}
	LEAVEIFERRORL(session->SupportedBulkInterfaces(aUids));
	CleanupStack::PopAndDestroy(&iSessionsLock);
	}


// Helper Active Objects

CBulkThreadWatcher::CBulkThreadWatcher(CRemConServer& aServer)
	: CActive(CActive::EPriorityStandard)
	, iServer(aServer)
	{
	LOG_FUNC;
	}

CBulkThreadWatcher::~CBulkThreadWatcher()
	{
	LOG_FUNC;
	Cancel();
	}

void CBulkThreadWatcher::StartL()
	{
	LOG_FUNC;
	// Add to scheduler jit
	CActiveScheduler::Add(this);
	iServer.iBulkServerThread.Logon(iStatus);
	if(iStatus.Int() == KErrNoMemory)
		{
		User::WaitForRequest(iStatus); // swallow the signal...
		// if no memory then we have to fail now otherwise
		// we are in an odd state where we don't know if the
		// bulk thread is running or not.
		LEAVEL(KErrNoMemory);
		}
	else
		{
		// Otherwise the request is handled by the active scheduler.
		SetActive();
		}
	}

void CBulkThreadWatcher::RunL()
	{
	LOG_FUNC;
	LOG1(_L("\tiStatus.Int() = %d"), iStatus.Int());
	// Thread is dead so kill handle.
	iServer.iBulkServerThread.Close();
	iServer.iBulkThreadOpen = EFalse;
	iServer.StartShutdownTimerIfNoSessionsOrBulkThread();
	iServer.iBulkThreadWatcher = NULL;
	delete this; // end...
	}

void CBulkThreadWatcher::DoCancel()
	{
	LOG_FUNC;
	iServer.iBulkServerThread.LogonCancel(iStatus);
	}

