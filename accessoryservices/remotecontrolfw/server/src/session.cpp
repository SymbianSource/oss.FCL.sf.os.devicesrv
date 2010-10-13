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
// Remote Control session implementation.
// 
//

/**
 @file
 @internalComponent
*/
#include "e32base.h"

#include <bluetooth/logger.h>
#include <remcon/remconbearerinterface.h>
#include <remcon/remconifdetails.h>
#include <s32mem.h>
#include "utils.h"
#include "server.h"
#include "bearermanager.h"
#include "remconmessage.h"
#include "connections.h"
#include "activehelper.h"
#include "session.h"
#include "messagequeue.h"
#include "remconserver.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

#ifdef _DEBUG
PANICCATEGORY("session");
#endif

CRemConSession* CRemConSession::NewL(CRemConServer& aServer,
		CBearerManager& aBearerManager,
		const RMessage2& aMessage,
		TUint aId)
	{
	LOG_STATIC_FUNC
	CRemConSession* self = new(ELeave) CRemConSession(aServer, aBearerManager, aId);
	CleanupStack::PushL(self);
	self->ConstructL(aMessage);
	CLEANUPSTACK_POP1(self);
	return self;
	}

CRemConSession::CRemConSession(CRemConServer& aServer,
		CBearerManager& aBearerManager,
		TUint aId)
 :	iServer(aServer),
	iBearerManager(aBearerManager),
	iId(aId)
	{
	LOG_FUNC
	}

void CRemConSession::ConstructL(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Get the client's process ID.
	RThread thread;
	LEAVEIFERRORL(aMessage.Client(thread));
	CleanupClosePushL(thread);
	RProcess process;
	LEAVEIFERRORL(thread.Process(process));
	iClientInfo.ProcessId() = process.Id();
	process.Close();
	iClientInfo.SecureId() = thread.SecureId();
	CleanupStack::PopAndDestroy(&thread);

	iSendQueue = CMessageQueue::NewL();
	
	TCallBack cb(SendNextCb, this);
	
	iSendNextCallBack = new(ELeave) CAsyncCallBack(cb, CActive::EPriorityStandard);
	
	// Tell the server about us.
	LEAVEIFERRORL(iServer.ClientOpened(*this));

	// Set our pointer into the connection history at the current/'Last' item.
	iServer.SetConnectionHistoryPointer(Id());
	
	iPendingMsgProcessor = new (ELeave) CActiveHelper(*this);
	}

CRemConSession::~CRemConSession()
	{
	LOG(KNullDesC8);
	LOG_FUNC;
	
	delete iPendingMsgProcessor;
	delete iSendNextCallBack;
	delete iSendQueue;
	// we will need to tell the server which bearer this used to be connected to
	// this enables the server to not inform a bearer that is already connected
	// that its been connected
	// Tell the server we've gone away- it may start its shutdown timer.
	iServer.ClientClosed(*this, iRemoteAddress.BearerUid());
	delete iInterestedAPIs;
	iPlayerName.Close();
	}

void CRemConSession::ServiceL(const RMessage2& aMessage)
	{
	LOG(KNullDesC8);
	LOG_FUNC;
	LOG1(_L("\taMessage.Function() = %d"), aMessage.Function());

	// Switch on the IPC number and call a 'message handler'. Message handlers 
	// complete aMessage (either with Complete or Panic), or make a note of 
	// the message for later asynchronous completion.
	// Message handlers should not leave- the server does not have an Error 
	// function. 

	switch ( aMessage.Function() )
		{
	// Heap failure testing APIs.
	case ERemConDbgMarkHeap:
#ifdef _DEBUG
		LOG(_L("\tmark heap"));
		__UHEAP_MARK;
#endif // _DEBUG
		CompleteClient(aMessage, KErrNone);
		break;

	case ERemConDbgCheckHeap:
#ifdef _DEBUG
		LOG1(_L("\tcheck heap (expecting %d cells)"), aMessage.Int0());
		__UHEAP_CHECK(aMessage.Int0());
#endif // _DEBUG
		CompleteClient(aMessage, KErrNone);
		break;

	case ERemConDbgMarkEnd:
#ifdef _DEBUG
		LOG1(_L("\tmark end (expecting %d cells)"), aMessage.Int0());
		__UHEAP_MARKENDC(aMessage.Int0());
#endif // _DEBUG
		CompleteClient(aMessage, KErrNone);
		break;

	case ERemConDbgFailNext:
#ifdef _DEBUG
		{
		LOG1(_L("\tfail next (simulating failure after %d allocation(s))"), aMessage.Int0());
		if ( aMessage.Int0() == 0 )
			{
			__UHEAP_RESET;
			}
		else
			{
			__UHEAP_FAILNEXT(aMessage.Int0());
			}
		}
#endif // _DEBUG
		CompleteClient(aMessage, KErrNone);
		break;

	case ERemConSetClientType:
		SetClientType(aMessage);
		// This is a sync API- check that the message has been completed.
		// (NB We don't check the converse for async APIs because the message 
		// may have been panicked synchronously.)
		ASSERT_DEBUG(aMessage.IsNull());
		break;

	case ERemConGoConnectionOriented:
		GoConnectionOriented(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;

	case ERemConGoConnectionless:
		GoConnectionless(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;

	case ERemConConnectBearer:
		ConnectBearer(aMessage);
		break;

	case ERemConConnectBearerCancel:
		ConnectBearerCancel(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;

	case ERemConDisconnectBearer:
		DisconnectBearer(aMessage);
		break;

	case ERemConDisconnectBearerCancel:
		DisconnectBearerCancel(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;

	case ERemConSend:
		Send(aMessage);
		break;
		
	case ERemConSendNotify:
		SendNotify(aMessage);
		break;
		
	case ERemConSendUnreliable:
		SendUnreliable(aMessage);
		break;
	
	case ERemConSendCancel:
		SendCancel(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;

	case ERemConReceive:
		Receive(aMessage);
		break;

	case ERemConReceiveCancel:
		ReceiveCancel(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;

	case ERemConGetConnectionCount:
		GetConnectionCount(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;

	case ERemConGetConnections:
		GetConnections(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;

	case ERemConNotifyConnectionsChange:
		NotifyConnectionsChange(aMessage);
		break;

	case ERemConNotifyConnectionsChangeCancel:
		NotifyConnectionsChangeCancel(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;
		
	case ERemConRegisterInterestedAPIs:
		RegisterInterestedAPIs(aMessage);
		ASSERT_DEBUG(aMessage.IsNull());
		break;
		
	default:
		// Unknown message
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicIllegalIpc);
		break;
		}
	}

void CRemConSession::CompleteClient(const RMessage2& aMessage, TInt aError)
	{
	LOG1(_L("\tcompleting client message with %d"), aError);
	TBool cleanClientInfoMessage = (iClientInfo.Message().Handle() == aMessage.Handle());
	aMessage.Complete(aError);
	if(cleanClientInfoMessage)
		{
		iClientInfo.Message() = RMessage2();
		}
	}

void CRemConSession::SetClientType(const RMessage2& aMessage)
	{
	LOG_FUNC;

	if ( iType != ERemConClientTypeUndefined )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientTypeAlreadySet);
		return;
		}

	const TRemConClientType type = static_cast<TRemConClientType>(aMessage.Int0());
	LOG1(_L("\trequested (TRemConClientType) type = %d"), type);
	
	TInt err = aMessage.GetDesLength(1);
	if(err >= 0)
		{
		TRAP(err, DoSetClientTypeL(aMessage));
		if(err == KErrBadDescriptor)
			{
			PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
			return;
			}
		}
	else if(err == KErrBadDescriptor)
		{
		// The additional parameters are optional (i.e. old or controller clients won't provide them).
		err = KErrNone;
		}
	
	if(err != KErrNone)
		{
		CompleteClient(aMessage, err);
		}
	else
		{
		switch ( type )
			{
		case ERemConClientTypeController:
			iType = type;
			CompleteClient(aMessage, KErrNone);
			break;
	
		case ERemConClientTypeTarget:
			// Check that there aren't already any target clients with the
			// same process ID.
			if ( !iServer.TargetClientWithSameProcessId(iClientInfo.ProcessId()) )
				{
				iType = type;
				CompleteClient(aMessage, KErrNone);
				}
			else
				{
				CompleteClient(aMessage, KErrInUse);
				}
			break;
	
		default:
			PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadType);
			break;
			}
		}

	if ( iType != ERemConClientTypeUndefined )
		{
		// The type got set, so tell the server, so it can tell the bearers if 
		// necessary. 
		iServer.ClientTypeSet(*this);
		}
	}

void CRemConSession::DoSetClientTypeL(const RMessage2& aMessage)
	{
	// Retrieve the client type information 
	RBuf8 typeBuf;
	typeBuf.CreateL(sizeof(TPlayerTypeInformation));
	CleanupClosePushL(typeBuf);
	aMessage.ReadL(1, typeBuf);
			
	const TPlayerTypeInformation* Ptr = reinterpret_cast<const TPlayerTypeInformation*> (typeBuf.Ptr());
	iPlayerType.iPlayerType = (*Ptr).iPlayerType;
	iPlayerType.iPlayerSubType = (*Ptr).iPlayerSubType;
	// Retrieve the client player name inforamtion
	iPlayerName.CreateL(aMessage.Int2());
	CleanupClosePushL(iPlayerName);
	aMessage.ReadL(3, iPlayerName);	
	CleanupStack::Pop(&iPlayerName);
	
	CleanupStack::PopAndDestroy(&typeBuf); 
	}
void CRemConSession::GoConnectionOriented(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our type set...
	if ( iType != ERemConClientTypeController )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadType);
		return;
		}

	if ( !iRemoteAddress.IsNull() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicAlreadyConnectionOriented);
		return;
		}

	if ( iConnectBearerMsg.Handle() || iDisconnectBearerMsg.Handle() || iSendMsg.Handle())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
		return;
		}
	if (iSending != ENotSending)
		{
		DoSendCancel();
		}
	EmptySendQueue();
	
	// Get the desired address from the message and check it.
	const TUid uid = TUid::Uid(aMessage.Int0());
	LOG1(_L("\tuid = 0x%08x"), uid);
	// Check the requested bearer exists.
	TBool bearerExists = iBearerManager.BearerExists(uid);
	if ( !bearerExists)
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerPluginIncorrectInterface);
		return;
		}
	// Check the bearer-specific part of the address.
	TBuf8<TRemConAddress::KMaxAddrSize> buf;
	TInt err = aMessage.Read(1, buf);
	if ( err != KErrNone )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
		return;
		}

	// Do security check- if this client won't be allowed to use the bearer 
	// then fail the request. 
	// NB This security check (repeated in debug at ConnectBearer and 
	// DisconnectBearer time) is all that stands between a connection-oriented 
	// client and the bearer, and is all the caps checking that RemCon does!
	err = KErrPermissionDenied;
	if ( iBearerManager.CheckPolicy(uid, aMessage) )
		{
		err = KErrNone;
		}
		
		
	// if alls well and we're connection oriented then set up as such
	if (KErrNone == err)
		{
		// The client has passed all our checks- set our data member.
		iRemoteAddress.BearerUid() = uid;
		iRemoteAddress.Addr() = buf;
		// tell the server
		iServer.ClientGoConnectionOriented(*this,uid);
		}
				
	CompleteClient(aMessage, err);
	}

void CRemConSession::GoConnectionless(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our type set...
	if ( iType != ERemConClientTypeController )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadType);
		return;
		}

	if ( iRemoteAddress.IsNull() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicNotConnectionOriented);
		return;
		}

	if ( iConnectBearerMsg.Handle() || iDisconnectBearerMsg.Handle() || iSendMsg.Handle())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
		return;
		}
	
	if (iSending != ENotSending)
		{
		DoSendCancel();
		}
	EmptySendQueue();
	
	// we will need to tell the server which bearer this used to be connected to
	// this enables the server to not inform a bearer that is already connected
	// that its been connected
	TUid oldUid = iRemoteAddress.BearerUid();
	
	iRemoteAddress.BearerUid() = KNullUid;	

	// tell the server
	iServer.ClientGoConnectionless(*this, oldUid);

	CompleteClient(aMessage, KErrNone);
	}

void CRemConSession::ConnectBearer(const RMessage2& aMessage)
	{
	LOG_FUNC;

	if ( iType != ERemConClientTypeController )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadType);
		return;
		}

	if ( iConnectBearerMsg.Handle() || iDisconnectBearerMsg.Handle() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
		return;
		}

	if ( iRemoteAddress.IsNull() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicNotConnectionOriented);
		return;
		}

	// Check the requested bearer exists.
	TBool bearerExists = iBearerManager.BearerExists(iRemoteAddress.BearerUid());
	// This check was done at GoConnectionOriented time.
	ASSERT_DEBUG(bearerExists);
	// So was this one.
	ASSERT_DEBUG(iBearerManager.CheckPolicy(iRemoteAddress.BearerUid(), aMessage));

	// Check the state of our given connection at the bearer level. If it is: 
	// -) disconnected request the connection to come up,
	// -) connecting or disconnecting, add message to the queue of pending 
	//		messages, and process it once connecting/disconnecting has been completed
	// -) connected, complete the client's message,

	TConnectionState conState;
	conState = iServer.ConnectionState(iRemoteAddress);

	if ( conState == EDisconnected )
		{
		// The bearer may indicate connection synchronously, so set this 
		// message _before_ we ask them
		iConnectBearerMsg = aMessage;
		TInt err = iBearerManager.Connect(iRemoteAddress);
		if ( err != KErrNone )
			{
			CompleteClient(iConnectBearerMsg, err);
			}
		}
	else if ( conState == EDisconnecting ||  conState == EConnecting )
		{
		if ( iPendingMsg.Handle() )
			{
			PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
			return;
			}
		// Store the message, it will get processed later.
		iPendingMsg = aMessage;
		}
	else // EConnected
		{
		CompleteClient(aMessage, KErrNone);
		}
		
	}

void CRemConSession::ConnectBearerCancel(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our type set...
	if ( iType != ERemConClientTypeController )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadType);
		return;
		}

	if ( iConnectBearerMsg.Handle() )
		{
		CompleteClient(iConnectBearerMsg, KErrCancel);
		}
	else if ( iPendingMsg.Handle() && ( iPendingMsg.Function() == ERemConConnectBearer ))
		{
		CompleteClient(iPendingMsg, KErrCancel);
		}
		
	CompleteClient(aMessage, KErrNone);
	// At no point do we make any change to the processes going on underneath
	// us- 'Cancel' APIs are just for cancelling interest in an async
	// operation.
	}

void CRemConSession::DisconnectBearer(const RMessage2& aMessage)
	{
	LOG_FUNC;

	if ( iType != ERemConClientTypeController )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadType);
		return;
		}

	if ( iDisconnectBearerMsg.Handle() || iConnectBearerMsg.Handle() || iSendMsg.Handle())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
		return;
		}

	if ( iRemoteAddress.IsNull() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicNotConnectionOriented);
		return;
		}

	if (iSending != ENotSending)
		{
		DoSendCancel();
		}
	EmptySendQueue();
	
	// Check the requested bearer exists.
	TBool bearerExists = iBearerManager.BearerExists(iRemoteAddress.BearerUid());
	// This check was done at GoConnectionOriented time.
	ASSERT_DEBUG(bearerExists);
	// So was this one.
	ASSERT_DEBUG(iBearerManager.CheckPolicy(iRemoteAddress.BearerUid(), aMessage));

	// Check the state of the given connection. If it is:
	// -) connected, request connection to go away,
	// -) disconnected, compete the client's message,
	// -) connecting or disconnecting, add message to the queue of pending 
	//		messages, and process it once connecting/disconnecting has been completed

	TInt err;
	TConnectionState conState;
	conState = iServer.ConnectionState(iRemoteAddress);

	if ( conState == EConnected )
		{
		// The bearer may indicate disconnection synchronously, so set this 
		// message _before_ we ask them
		iDisconnectBearerMsg = aMessage;
		err = iBearerManager.Disconnect(iRemoteAddress);
		if ( err != KErrNone )
			{
			CompleteClient(iDisconnectBearerMsg, err);
			}
		}
	else if ( conState == EDisconnecting ||  conState == EConnecting )
		{
		if ( iPendingMsg.Handle() )
			{
			PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
			return;
			}
		// Store the message, it will get processed later.
		iPendingMsg = aMessage;
		}
	else //disconnected
		{
		CompleteClient(aMessage, KErrNone);	
		}
	}

void CRemConSession::DisconnectBearerCancel(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our type set...
	if ( iType != ERemConClientTypeController )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadType);
		return;
		}

	if ( iDisconnectBearerMsg.Handle() )
		{
		CompleteClient(iDisconnectBearerMsg, KErrCancel);
		}
	else if ( iPendingMsg.Handle() && (iPendingMsg.Function() == ERemConDisconnectBearer ))
		{
		CompleteClient(iPendingMsg, KErrCancel);
		}
		
	CompleteClient(aMessage, KErrNone);
	}

void CRemConSession::Send(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we're not already sending...
	if ( iSendMsg.Handle())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicSendAlreadyOutstanding);
		return;
		}
	
	iSendMsg = aMessage;
	
	// Check we've had our type set...
	if (	Type() != ERemConClientTypeController
		&&	Type() != ERemConClientTypeTarget
		)
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientTypeNotSet);
		return;
		}

	// Check we don't have a disconnect outstanding- this makes no sense from 
	// a client viewpoint (they should cancel the disconnect first).
	// [The client is allowed to have a connect request outstanding- the 
	// bearer manager makes sure a bearer-level connect is not posted on the 
	// same address twice.]
	if ( iDisconnectBearerMsg.Handle() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
		return;
		}

	TRAPD(err, DoSendL(aMessage));
	if ( err != KErrNone )
		{
		CompleteClient(aMessage, err);
		}
	}

/**
Sends a notify message to the remote device.

This function is intended for the RemCon controller client to send a notify
command to the remote device.
*/
void CRemConSession::SendNotify(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we're not already sending...
	if ( iSendMsg.Handle())
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicSendAlreadyOutstanding);
		return;
		}
	
	iSendMsg = aMessage;
	
	// Check we've had our type set...
	if (Type() != ERemConClientTypeController)
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientTypeNotSet);
		return;
		}

	// Check we don't have a disconnect outstanding- this makes no sense from 
	// a client viewpoint (they should cancel the disconnect first).
	// [The client is allowed to have a connect request outstanding- the 
	// bearer manager makes sure a bearer-level connect is not posted on the 
	// same address twice.]
	if ( iDisconnectBearerMsg.Handle() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
		return;
		}

	TRAPD(err, DoSendNotifyL(aMessage));
	if ( err != KErrNone )
		{
		CompleteClient(aMessage, err);
		}
	}

void CRemConSession::DoSendL(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Get the data the client wants to send.
	const TUid interfaceUid = TUid::Uid(aMessage.Int0());
	LOG1(_L("\tinterfaceUid = 0x%08x"), interfaceUid);

	if (aMessage.GetDesLengthL(1) != sizeof(TOperationInformation))
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
		return;
		}

	TPckgBuf<TOperationInformation> opInfoPckg;
	
	TInt err= aMessage.Read(
			1, // location of the descriptor in the client's message (as we expect them to have set it up)
			opInfoPckg, // descriptor to write to from client memory space
			0 // offset into our descriptor to put the client's data
			);
	
	if ( err != KErrNone )
		{
		LOG1(_L("\taMessage.Read = %d"), err);
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
		return;
		}	
	
	const TUint operationId = opInfoPckg().iOperationId;
	LOG1(_L("\toperationId = 0x%02x"), operationId);
	
	const TRemConMessageSubType messageSubType = opInfoPckg().iMessageSubType;
	LOG1(_L("\tmessageSubType = 0x%02x"), messageSubType);

	
	const TUint dataLength = (TUint)aMessage.GetDesLengthL(3);
	LOG1(_L("\tdataLength = %d"), dataLength);
	
	// If the client wanted to send some operation-associated data, read it 
	// from them.
	RBuf8 sendDes;
	if ( dataLength != 0 )
		{
		sendDes.CreateL(dataLength);
		TInt err = aMessage.Read(
			3, // location of the descriptor in the client's message (as we expect them to have set it up)
			sendDes, // descriptor to write to from client memory space
			0 // offset into our descriptor to put the client's data
			);
		// NB We don't do LEAVEIFERRORL(aMessage.Read) because a bad client 
		// descriptor is a panicking offence for them, not an 'error the 
		// request' offence.
		if ( err != KErrNone )
			{
			LOG1(_L("\taMessage.Read = %d"), err);
			sendDes.Close();
			PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
			return;
			}
		}
	CleanupClosePushL(sendDes);

	// Before we ask the server to send, we must set our ClientInfo 
	// correctly so the TSP can get information about the client. 
	iClientInfo.Message() = aMessage;

	CRemConMessage* msg = NULL;
	
	// Examine the session type.
	switch ( iType )
		{
	case ERemConClientTypeTarget:
		{
		LOG(_L("\tTARGET send"));

		msg = CRemConMessage::NewL(
			TRemConAddress(), // we don't know which remotes it's going to yet
			ERemConResponse, // targets can only send responses
			messageSubType,
			interfaceUid,
			operationId,
			sendDes, // msg takes ownership
			Id(), // session id to match this response against the originating command
			0, // transaction id not yet known
			ETrue);
		CLEANUPSTACK_POP1(&sendDes); // now owned by msg
		
		}
		break;

	case ERemConClientTypeController:
		{
		LOG(_L("\tCONTROLLER send"));
        if (  (messageSubType == ERemConNotifyCommandAwaitingInterim)
           || (messageSubType == ERemConNotifyCommandAwaitingChanged)
            )
        	{
        	LOG(_L("\terror, not allowed to use Send() to send notify command"));
        	CleanupStack::PopAndDestroy(&sendDes);
        	PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicIllegalIpc);
        	return;
        	}
        else
        	{
        	msg = CRemConMessage::NewL(
        			iRemoteAddress, // either specified (if we're connection-oriented) or null (we're connectionless- this field will be filled in by the TSP)
        			ERemConCommand, 
        			messageSubType,
        			interfaceUid,
        			operationId,
        			sendDes, // msg takes ownership
        			Id(), // session id for when the response comes back
        			0, // we let the bearer manager invent a new transaction id when the message gets to it
        			ETrue);
        	}		
		CLEANUPSTACK_POP1(&sendDes); // now owned by msg

		}
		break;

	default:
		DEBUG_PANIC_LINENUM;
		break;
		}
	
	ASSERT_DEBUG(iSendQueue);
	// We know msg is valid at this stage as the code would leave or panic earlier if msg was 
	// not set.
	ASSERT_DEBUG(msg);
	
	if (iSending != ENotSending || !iSendQueue->IsEmpty())
		{
		iSendQueue->Append(*msg);
		}
	else
		{
		// we know msg cannot be null here as said above.
		SendToServer(*msg);
		}
	}

/**
@see CRemConSession::SendNotify
*/
void CRemConSession::DoSendNotifyL(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Get the data the client wants to send.
	const TUid interfaceUid = TUid::Uid(aMessage.Int0());
	LOG1(_L("\tinterfaceUid = 0x%08x"), interfaceUid);

	if (aMessage.GetDesLengthL(1) != sizeof(TOperationInformation))
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
		return;
		}

	TPckgBuf<TOperationInformation> opInfoPckg;	
	TInt err= aMessage.Read(
			1, // location of the descriptor in the client's message (as we expect them to have set it up)
			opInfoPckg, // descriptor to write to from client memory space
			0 // offset into our descriptor to put the client's data
			);
	
	if ( err != KErrNone )
		{
		LOG1(_L("\taMessage.Read = %d"), err);
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
		return;
		}	
	
	const TUint operationId = opInfoPckg().iOperationId;
	LOG1(_L("\toperationId = 0x%02x"), operationId);
	
	const TRemConMessageSubType messageSubType = opInfoPckg().iMessageSubType;
	LOG1(_L("\tmessageSubType = 0x%02x"), messageSubType);
	
	const TUint dataLength = (TUint)aMessage.GetDesLengthL(2);
	LOG1(_L("\tdataLength = %d"), dataLength);
	
	// If the client wanted to send some operation-associated data, read it 
	// from them.
	RBuf8 sendDes;
	if ( dataLength != 0 )
		{
		sendDes.CreateL(dataLength);
		TInt err = aMessage.Read(
			2, // location of the descriptor in the client's message (as we expect them to have set it up)
			sendDes, // descriptor to write to from client memory space
			0 // offset into our descriptor to put the client's data
			);
		// NB We don't do LEAVEIFERRORL(aMessage.Read) because a bad client 
		// descriptor is a panicking offence for them, not an 'error the 
		// request' offence.
		if ( err != KErrNone )
			{
			LOG1(_L("\taMessage.Read = %d"), err);
			sendDes.Close();
			PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
			return;
			}
		}

	// Before we ask the server to send, we must set our ClientInfo 
	// correctly so the TSP can get information about the client. 
	iClientInfo.Message() = aMessage;

	CRemConMessage* msg = NULL;
	
	if (messageSubType != ERemConNotifyCommandAwaitingInterim)
		{
		sendDes.Close();
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicIllegalIpc);
		return;
		}
	
	CleanupClosePushL(sendDes);
	msg = CRemConMessage::NewL(
			iRemoteAddress, // either specified (if we're connection-oriented) or null (we're connectionless- this field will be filled in by the TSP)
			ERemConNotifyCommand, 
			messageSubType,
			interfaceUid,
			operationId,
			sendDes, // msg takes ownership
			Id(), // session id for when the response comes back
			0, // we let the bearer manager invent a new transaction id when the message gets to it
			ETrue);	
	CLEANUPSTACK_POP1(&sendDes); // now owned by msg
	
	LOG(_L("\tCONTROLLER send"));
	ASSERT_DEBUG(iSendQueue);
	if (iSending != ENotSending || !iSendQueue->IsEmpty())
		{
		iSendQueue->Append(*msg);
		}
	else
		{
		SendToServer(*msg);
		}
	}

void CRemConSession::SendUnreliable(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our type set...
	if (	Type() != ERemConClientTypeController
		&&	Type() != ERemConClientTypeTarget
		)
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientTypeNotSet);
		return;
		}

	// Check we don't have a disconnect outstanding- this makes no sense from 
	// a client viewpoint (they should cancel the disconnect first).
	// [The client is allowed to have a connect request outstanding- the 
	// bearer manager makes sure a bearer-level connect is not posted on the 
	// same address twice.]
	if ( iDisconnectBearerMsg.Handle() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBearerControlOutstanding);
		return;
		}

	CRemConMessage* msg = NULL;
	TRAPD(err, msg = DoCreateUnreliableMessageL(aMessage));
	CompleteClient(aMessage, err);
	if (err == KErrNone)
		{
		ASSERT_DEBUG(iSendQueue);
		if (iSending || !iSendQueue->IsEmpty())
			{
			iSendQueue->Append(*msg);
			}
		else
			{
			SendToServer(*msg);
			}
		}
	}

CRemConMessage* CRemConSession::DoCreateUnreliableMessageL(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Get the data the client wants to send.
	const TUid interfaceUid = TUid::Uid(aMessage.Int0());
	LOG1(_L("\tinterfaceUid = 0x%08x"), interfaceUid);

	if (aMessage.GetDesLengthL(1) != sizeof(TOperationInformation))
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
		LEAVEL(KErrBadDescriptor);
		}

	TPckgBuf<TOperationInformation> opInfoPckg;
	
	TInt err= aMessage.Read(
			1, // location of the descriptor in the client's message (as we expect them to have set it up)
			opInfoPckg, // descriptor to write to from client memory space
			0 // offset into our descriptor to put the client's data
			);
	
	if ( err != KErrNone )
		{
		LOG1(_L("\taMessage.Read = %d"), err);
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
		LEAVEL(KErrBadDescriptor);
		}	
	
	const TUint operationId = opInfoPckg().iOperationId;
	LOG1(_L("\toperationId = 0x%02x"), operationId);
	
	const TRemConMessageSubType messageSubType = opInfoPckg().iMessageSubType;
	LOG1(_L("\tmessageSubType = 0x%02x"), messageSubType);

	
	const TUint dataLength = (TUint)aMessage.GetDesLengthL(2);
	LOG1(_L("\tdataLength = %d"), dataLength);
	
	// If the client wanted to send some operation-associated data, read it 
	// from them.
	RBuf8 sendDes;
	if ( dataLength != 0 )
		{
		sendDes.CreateL(dataLength);
		TInt err = aMessage.Read(
			2, // location of the descriptor in the client's message (as we expect them to have set it up)
			sendDes, // descriptor to write to from client memory space
			0 // offset into our descriptor to put the client's data
			);
		// NB We don't do LEAVEIFERRORL(aMessage.Read) because a bad client 
		// descriptor is a panicking offence for them, not an 'error the 
		// request' offence.
		if ( err != KErrNone )
			{
			LOG1(_L("\taMessage.Read = %d"), err);
			sendDes.Close();
			PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
			LEAVEL(KErrBadDescriptor);
			}
		}
	CleanupClosePushL(sendDes);

	// Before we ask the server to send, we must set our ClientInfo 
	// correctly so the TSP can get information about the client. 
	iClientInfo.Message() = aMessage;

	CRemConMessage* msg = NULL;
	
	// Examine the session type.
	switch ( iType )
		{
	case ERemConClientTypeTarget:
		{
		LOG(_L("\tTARGET send"));

		msg = CRemConMessage::NewL(
			TRemConAddress(), // we don't know which remotes it's going to yet
			ERemConResponse, // targets can only send responses
			messageSubType,
			interfaceUid,
			operationId,
			sendDes, // msg takes ownership
			Id(), // session id to match this response against the originating command
			0, // transaction id not yet known
			EFalse);
		CLEANUPSTACK_POP1(&sendDes); // now owned by msg
		break;
		}

	case ERemConClientTypeController:
		{
		LOG(_L("\tCONTROLLER send"));
		
		// A client is not allowed to send an unreliable notify command.
		if	(	(messageSubType == ERemConNotifyCommandAwaitingInterim)
			||	(messageSubType == ERemConNotifyCommandAwaitingChanged)
			)
			{
			LOG(_L8("\tNot allowed to send unreliable notify command"));
			CleanupStack::PopAndDestroy(&sendDes);
			PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicIllegalIpc);
			LEAVEL(KErrBadDescriptor);
			}
		
		msg = CRemConMessage::NewL(
			iRemoteAddress, // either specified (if we're connection-oriented) or null (we're connectionless- this field will be filled in by the TSP)
			ERemConCommand, // controllers can only send commands
			messageSubType,
			interfaceUid,
			operationId,
			sendDes, // msg takes ownership
			Id(), // session id for when the response comes back
			0, // we let the bearer manager invent a new transaction id when the message gets to it
			EFalse);
		CLEANUPSTACK_POP1(&sendDes); // now owned by msg
		
		}
		break;

	default:
		DEBUG_PANIC_LINENUM;
		break;
		}

	return msg;
	}

void CRemConSession::SendToServer(CRemConMessage& aMsg)
	{
	LOG_FUNC;
	
	// Set our completion members.
	NumRemotes() = 0;
	NumRemotesToTry() = 0;
	SendError() = KErrNone;

	
	iSending = (aMsg.IsReliableSend()) ? ESendingReliable: ESendingUnreliable;
	switch ( iType )
		{
	case ERemConClientTypeTarget:
		{
		iServer.SendResponse(aMsg, *this);
		break;
		}
	case ERemConClientTypeController:
		{
		iServer.SendCommand(aMsg);
		break;
		}
	default:
		DEBUG_PANIC_LINENUM;
		break;
		}
	}


void CRemConSession::SendCancel(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our type set...
	if (	Type() != ERemConClientTypeController
		&&	Type() != ERemConClientTypeTarget
		)
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientTypeNotSet);
		return;
		}

	// See comments in ConnectBearerCancel.
	if ( iSendMsg.Handle() )
		{
		DoSendCancel();
		}
	
	if (iSendMsg.Handle())
		{
		CRemConMessage* msg;
		TBool first = ETrue;
		ASSERT_DEBUG(iSendQueue);
		TSglQueIter<CRemConMessage>& iter = iSendQueue->SetToFirst();
		while ((msg = iter++) != NULL)
			{
			if (msg->IsReliableSend())
				{
				CompleteClient(iSendMsg, KErrCancel);
				iSendQueue->RemoveAndDestroy(*msg);
				if (first)
					{
					ASSERT_DEBUG(iSendNextCallBack);
					iSendNextCallBack->Cancel();
					}
				break;
				}
			first = EFalse;
			}
		}
	
	CompleteClient(aMessage, KErrNone);
	}

void CRemConSession::DoSendCancel()
	{
	LOG_FUNC;
	// We must tell the server, and pull the CRemConMessage from the 
	// 'outgoing pending TSP' queue if it's on it. If the TSP is currently 
	// processing the CRemConMessage, we must tell it to stop before we 
	// can complete the RMessage2 iSendMsg- the TSP might still be 
	// dereferencing bits of it. (The TSP is given iSendMsg so it can 
	// access the client's secure ID and do a capability check.)
	// NB This only matters for commands- responses don't go through the 
	// TSP.
	// Not also that this processing *stops* this 
	// CRemConSession::SendCancel method from being the very simple 'I'm 
	// no longer interested in the completion of the asynchronous request' 
	// type of API it (and all cancels) should be. It actually does work 
	// as well. As long as this work is implemented _synchronously_, we 
	// should be OK.
	iServer.SendCancel(*this);

	NumRemotesToTry() = 0;
	iSendError = KErrCancel;
	CompleteSend();
	
	}

void CRemConSession::EmptySendQueue()
	{
	ASSERT_DEBUG(!iSendMsg.Handle())
	ASSERT_DEBUG(iSendNextCallBack);
	iSendNextCallBack->Cancel();
	CRemConMessage* msg;
	ASSERT_DEBUG(iSendQueue);
	TSglQueIter<CRemConMessage>& iter = iSendQueue->SetToFirst();
	while ((msg = iter++) != NULL)
		{
		iSendQueue->RemoveAndDestroy(*msg);
		}
	}

void CRemConSession::Receive(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Messages are pushed from bearers, so we 
	// (a) do some sanity checking, 
	// (b) check the queue of incoming messages in case there's anything 
	// already waiting to be given to the client.

	if ( iReceiveMsg.Handle() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicReceiveAlreadyOutstanding);
		return;
		}

	// Check we've had our type set...
	if (	Type() != ERemConClientTypeController
		&&	Type() != ERemConClientTypeTarget
		)
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientTypeNotSet);
		return;
		}

	iReceiveMsg = aMessage;
	// If there's anything waiting to be given to us, ReceiveRequest will call 
	// back to us with it.
	iServer.ReceiveRequest(*this);
	}

void CRemConSession::ReceiveCancel(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our type set...
	if (	Type() != ERemConClientTypeController
		&&	Type() != ERemConClientTypeTarget
		)
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientTypeNotSet);
		return;
		}

	// See comments in ConnectBearerCancel.
	if ( iReceiveMsg.Handle() )
		{
		CompleteClient(iReceiveMsg, KErrCancel);
		}
	CompleteClient(aMessage, KErrNone);
	}

void CRemConSession::GetConnectionCount(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our type set...
	if (	Type() != ERemConClientTypeController
		&&	Type() != ERemConClientTypeTarget
		)
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientTypeNotSet);
		return;
		}

	// Get the answer to the question- the number of connections at the 
	// current point in time (i.e. the latest entry in the connection 
	// history).
	const TUint connCount = iServer.Connections().Count();
	LOG1(_L("\tconnCount = %d"), connCount);
	TPckg<TUint> count(connCount);
	TInt err = aMessage.Write(0, count);

	// If the client was told the answer with no error, then remember the 
	// current point in the connection history, so that when the client asks 
	// for the connections themselves, we give them a consistent answer.
	if ( err == KErrNone )
		{
		iServer.SetConnectionHistoryPointer(Id());
		iInGetConnectionsProcedure = ETrue;
		}
	CompleteClient(aMessage, err);
	}

void CRemConSession::GetConnections(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our type set...
	if (	Type() != ERemConClientTypeController
		&&	Type() != ERemConClientTypeTarget
		)
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientTypeNotSet);
		return;
		}

	iInGetConnectionsProcedure = EFalse;

	// Get the array of connections at the point in the history we're 
	// interested in and write it back to the client. NB This is not 
	// necessarily the Last item in the history but the item that we were 
	// pointing at when GetConnectionCount was called.
	const CConnections& conns = iServer.Connections(iId);
	const TUint count = conns.Count();
	LOG1(_L("\tcount = %d"), count);
	RBuf8 buf;
	TInt err = buf.Create(count * sizeof(TRemConAddress));
	if ( err == KErrNone )
		{
		TSglQueIter<TRemConAddress>& iter = conns.SetToFirst();
		TRemConAddress* addr;
		while ( ( addr = iter++ ) != NULL )
			{
			buf.Append((TUint8*)addr, sizeof(TRemConAddress));
			}

		// Write back to the client...
		err = aMessage.Write(0, buf);
		buf.Close();
		if ( err != KErrNone )
			{
			// We don't need to call SetConnectionHistoryPointer here because 
			// the server will do it when it cleans up the panicked client.
			PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicBadDescriptor);
			return;
			}	   		
		}

	// Whether or not there was an error, we're no longer interested in the 
	// history item we're currently registered as being interested in, so tell 
	// the server to bump up our pointer to the current (latest) one. NB This 
	// may in fact be the same record, if no connection changes have occurred 
	// since GetConnectionCount was called, but it's still important to give 
	// the server a chance to remove obsolete history records.
	iServer.SetConnectionHistoryPointer(Id());
	CompleteClient(aMessage, err);
	}

void CRemConSession::NotifyConnectionsChange(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Messages are pushed to us from bearers, so we don't need anything more 
	// than some sanity checking here.
	// Check we've had our type set...
	if (	Type() != ERemConClientTypeController
		&&	Type() != ERemConClientTypeTarget
		)
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientTypeNotSet);
		return;
		}

	if ( iNotifyConnectionsChangeMsg.Handle() )
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicConnectionsNotificationAlreadyOutstanding);
		}
	else
		{
		iNotifyConnectionsChangeMsg = aMessage;
		// Check the connection history for any more recent items than that we 
		// currently know about. If our pointer into the connection history 
		// isn't pointing at the 'current' item, we can complete the 
		// notification immediately and move the pointer up.
		if ( !iServer.ConnectionHistoryPointerAtLatest(Id()) )
			{
			CompleteClient(iNotifyConnectionsChangeMsg, KErrNone);
			iServer.SetConnectionHistoryPointer(Id());
			}
		}
	}

void CRemConSession::NotifyConnectionsChangeCancel(const RMessage2& aMessage)
	{
	LOG_FUNC;

	// Check we've had our type set...
	if (	Type() != ERemConClientTypeController
		&&	Type() != ERemConClientTypeTarget
		)
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicClientTypeNotSet);
		return;
		}

	// See comments in ConnectBearerCancel.
	if ( iNotifyConnectionsChangeMsg.Handle() )
		{
		CompleteClient(iNotifyConnectionsChangeMsg, KErrCancel);
		}
	CompleteClient(aMessage, KErrNone);
	}

void CRemConSession::RegisterInterestedAPIs(const RMessage2& aMessage)
	{
	LOG_FUNC;
	
	if(iType == ERemConClientTypeUndefined)
		{
		PANIC_MSG(aMessage, KRemConClientPanicCat, ERemConClientPanicRegisterInterestedAPIsInNonTargetSession);
		}
	
	TRAPD(err, DoRegisterInterestedAPIsL(aMessage));
	
	if(iType == ERemConClientTypeController)
		{
		iServer.ControllerClientAvailable();
		}
	else if(err == KErrNone) // must be target
		{
		iServer.TargetClientAvailable(*this);
		
		TInt count = iInterestedAPIs->Array().Count();
		for(TInt ix=0; ix<count; ++ix)
			{
			CRemConInterfaceDetails* details = iInterestedAPIs->Array()[ix];
			ASSERT_DEBUG(details);
			if(details->IsBulk())
				{
				iServer.BulkServerRequired();
				break;
				}
			}
		}
	
	CompleteClient(aMessage, err);
	}

void CRemConSession::DoRegisterInterestedAPIsL(const RMessage2& aMessage)
	{
	LOG_FUNC;
	
	RBuf8 buf;
	buf.CreateL(aMessage.GetDesLengthL(0));
	CleanupClosePushL(buf);
	
	aMessage.ReadL(0, buf);
	RDesReadStream ipcStream(buf);
	
	iInterestedAPIs = CRemConInterfaceDetailsArray::InternalizeL(ipcStream);
	
	ipcStream.Close(); 
	CleanupStack::PopAndDestroy(&buf); 
	}

void CRemConSession::CompleteConnect(const TRemConAddress& aAddr, TInt aError)
	{
	LOG_FUNC;
	LOG2(_L("\taError = %d, aAddr.BearerUid = 0x%08x"), aError, aAddr.BearerUid());

	LOG1(_L("\tiRemoteAddress.BearerUid = 0x%08x"), iRemoteAddress.BearerUid());
	LOG1(_L("\tiConnectBearerMsg.Handle = %d"), iConnectBearerMsg.Handle());

	if ( iRemoteAddress == aAddr )
		{
		if ( iConnectBearerMsg.Handle() )
			{
			// We are a session that has an outstanding request on this specific 
			// connection address.
			CompleteClient(iConnectBearerMsg, aError);
			}
		else 
			{
			// Connect bearer message is not valid. 
			// Check for pending messages.
			CheckForPendingMsg();
			}
		}
	}

void CRemConSession::CompleteDisconnect(const TRemConAddress& aAddr, TInt aError)
	{
	LOG_FUNC;
	LOG2(_L("\taError = %d, aAddr.BearerUid = 0x%08x"), aError, aAddr.BearerUid());

	LOG1(_L("\tiRemoteAddress.BearerUid = 0x%08x"), iRemoteAddress.BearerUid());
	LOG1(_L("\tiDisconnectBearerMsg.Handle = %d"), iDisconnectBearerMsg.Handle());

	if ( iRemoteAddress == aAddr )
		{
		if ( iDisconnectBearerMsg.Handle() )
			{
			// We are a session that has an outstanding request on this specific 
			// connection address.
			CompleteClient(iDisconnectBearerMsg, aError);
			}
		else 
			{
			// Diconnect bearer message is not valid. 
			// Check for pending messages.
			CheckForPendingMsg();
			}

		}
	}

void CRemConSession::ConnectionsChanged()
	{
	LOG_FUNC;
	
	LOG1(_L("\tiInGetConnectionsProcedure = %d"), iInGetConnectionsProcedure);
	// Only update the connections history pointer if we're not in the middle 
	// of a 'GetConnections' procedure. 
	if ( !iInGetConnectionsProcedure )
		{
		iServer.SetConnectionHistoryPointer(Id());
		}
	LOG1(_L("\tiNotifyConnectionsChangeMsg.Handle = %d"), iNotifyConnectionsChangeMsg.Handle());
	if ( iNotifyConnectionsChangeMsg.Handle() )
		{
		CompleteClient(iNotifyConnectionsChangeMsg, KErrNone);
		}
	}

void CRemConSession::CompleteSend()
	{
	LOG_FUNC;
	LOG2(_L("\tiNumRemotes = %d, iSendError = %d"), iNumRemotes, iSendError);

	ASSERT_DEBUG(NumRemotesToTry() == 0);
	NumRemotesToTry() = -1;

	
	if (iSending == ESendingReliable)
		{
		if ( iSendError == KErrNone )
			{
			TPckg<TUint> count(iNumRemotes);
			// 2 is the slot in the client's message for the number of remotes the 
			// message got sent to.
			iSendError = iSendMsg.Write(2, count);
			}
		CompleteClient(iSendMsg, iSendError);
		}
	
	ASSERT_DEBUG(iSendQueue);
	if (!iSendQueue->IsEmpty())
		{
		ASSERT_DEBUG(iSendNextCallBack);
		iSendNextCallBack->CallBack();
		}
	iSending = ENotSending;
	}

void CRemConSession::CompleteSendNotify()
	{
	LOG_FUNC;
	LOG1(_L("\tiSendError = %d"), iSendError);

	if (iSending == ESendingReliable)
		{
		CompleteClient(iSendMsg, iSendError);
		}
	
	ASSERT_DEBUG(iSendQueue);
	if (!iSendQueue->IsEmpty())
		{
		ASSERT_DEBUG(iSendNextCallBack);
		iSendNextCallBack->CallBack();
		}
	iSending = ENotSending;
	}

TInt CRemConSession::SendNextCb(TAny* aThis)
	{
	static_cast<CRemConSession*>(aThis)->DoSendNext();
	return KErrNone;
	}

void CRemConSession::DoSendNext()
	{
	ASSERT_DEBUG(iSendQueue);
	CRemConMessage& msg = iSendQueue->First();
	iSendQueue->Remove(msg);
	SendToServer(msg);
	}

void CRemConSession::PanicSend(TRemConClientPanic aCode)
	{
	LOG_FUNC;
	LOG1(_L("\taCode = %d"), aCode);

	PANIC_MSG(iSendMsg, KRemConClientPanicCat, aCode);
	}

TBool CRemConSession::SupportedMessage(const CRemConMessage& aMsg)
    {
    // Return true unless this is a command for an unsupported interface
    return !(aMsg.MsgType() == ERemConCommand && !FindInterfaceByUid(aMsg.InterfaceUid()));
    }

TInt CRemConSession::WriteMessageToClient(const CRemConMessage& aMsg)
	{
	LOG_FUNC;

	ASSERT_DEBUG(SupportedMessage(aMsg));
	ASSERT_DEBUG(iReceiveMsg.Handle());
	TRAPD(err, WriteMessageToClientL(aMsg));
	CompleteClient(iReceiveMsg, err);

	LOG1(_L("\terr = %d"), err);
	return err;
	}
	
void CRemConSession::WriteMessageToClientL(const CRemConMessage& aMsg)
	{
	LOG_FUNC;
	
	//check if our client is interested in this API
	//Only need to check commands because it is safe to assume that we are interested 
	//in the response if we have sent out a command.
	if(aMsg.MsgType() == ERemConCommand && !FindInterfaceByUid(aMsg.InterfaceUid()))
		{
		//The server will clean up the resource allocated for this msg
		LEAVEL(KErrArgument);
		}

	// This logging code left in for maintenance.
	//LOG1(_L("\t\tOperationData = \"%S\""), &aMsg.OperationData());
	
	TRemConClientReceivePackage receivePackage;
	receivePackage.iInterfaceUid = aMsg.InterfaceUid();
	receivePackage.iOperationId = aMsg.OperationId();
	receivePackage.iMessageSubType = aMsg.MsgSubType();
	receivePackage.iRemoteAddress = aMsg.Addr();
	
	TPckgC<TRemConClientReceivePackage> recPckg(receivePackage);
	LEAVEIFERRORL(iReceiveMsg.Write(0, recPckg));
	
	// Note that we do not panic the client if their descriptor is not 
	// big enough to hold the operation-specific data. If we did, then 
	// a buggy remote could take down a client of RemCon. Just error 
	// the client instead.
	LEAVEIFERRORL(iReceiveMsg.Write(1, aMsg.OperationData()));
	}
	
void CRemConSession::CheckForPendingMsg() const
	{
	LOG_FUNC;
	if (iPendingMsg.Handle())
		{
		ASSERT_DEBUG(iPendingMsgProcessor);
		iPendingMsgProcessor->Complete();
		}
	}

void CRemConSession::ProcessPendingMsgL()
	{
	LOG_FUNC;
	if (!iPendingMsg.Handle())
		{
		// This means that the pending connect or disconnect message,
		// has been cancelled by the time we got here.
		// (It was cancelled between two following calls:
		// iPendingMsgProcessor::Complete and iPendingMsgProcessor::RunL
		return;
		}
		
	ServiceL(iPendingMsg);
	if (iPendingMsg.Handle())
		{
		// This means that the pending msg has not been completed in ServiceL call.
		// It was stored either in iConnectBearerMsg or iDisconnectBearerMsg member.
		// This also means that this message is not "pending" any more 
		// (as processing of its copy has been started). 
		// However because the copy will get completed we need to 
		// clean iPendingMsg.iHandle here
		// To supress coverity error for uninitialized use of 'emptyMsg' coverity annotations
		// are used as the in-line default constructor of RMessage2 doesn't initialize all member variables.
		// coverity[var_decl]
		RMessage2 emptyMsg;
		iPendingMsg = emptyMsg;
		}
	}


TInt CRemConSession::SupportedInterfaces(RArray<TUid>& aUids)
	{
	LOG_FUNC
	ASSERT_DEBUG(iInterestedAPIs);
	TInt err = KErrNone;
	
	aUids.Reset();
	TInt count = iInterestedAPIs->Array().Count();
	for(TInt i=0; (i<count) && (err == KErrNone); i++)
		{
		CRemConInterfaceDetails* details = iInterestedAPIs->Array()[i];
		ASSERT_DEBUG(details);
		err = aUids.Append(details->Uid());
		}
	
	return err;
	}

TInt CRemConSession::SupportedBulkInterfaces(RArray<TUid>& aUids)
	{
	LOG_FUNC
	ASSERT_DEBUG(iInterestedAPIs);
	TInt err = KErrNone;
	
	aUids.Reset();
	TInt count = iInterestedAPIs->Array().Count();
	for(TInt i=0; (i<count) && (err == KErrNone); i++)
		{
		CRemConInterfaceDetails* details = iInterestedAPIs->Array()[i];
		ASSERT_DEBUG(details);
		if(details->IsBulk())
			{
			err = aUids.Append(details->Uid());
			}
		}
	
	return err;
	}

TInt CRemConSession::SupportedOperations(TUid aInterfaceUid, RArray<TUint>& aOperations)
	{
	LOG_FUNC
	TInt err = KErrNotSupported;
	CRemConInterfaceDetails* details = FindInterfaceByUid(aInterfaceUid);
	
	if(details)
		{
		TRAP(err, details->GetRemConInterfaceFeaturesL(aOperations));
		}
	return err;
	}

TBool CRemConSession::ClientAvailable() const
	{
	// Client is available as soon as it has registered the APIs
	// it is interested in.
	return !!iInterestedAPIs;
	}

CRemConInterfaceDetails* CRemConSession::FindInterfaceByUid(TUid aUid) const
	{
	LOG_FUNC
	ASSERT_DEBUG(iInterestedAPIs);
	TInt count = iInterestedAPIs->Array().Count();
	for(TInt ix=0; ix<count; ++ix)
		{
		CRemConInterfaceDetails* details = iInterestedAPIs->Array()[ix];
		ASSERT_DEBUG(details);
		if(details->Uid() == aUid)
			{
			return details;
			}
		}
	return NULL;
	}


