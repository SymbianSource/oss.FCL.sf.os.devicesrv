// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <e32base.h>
#include <domainmember.h>
#include <ssm/ssmstateawaresession.h>

#include "cmnpanic.h"
#include "ssmdebug.h"
#include "ssmstatemonitor.h"

/**
 @internalComponent
 @released 
 */
class RSsmStateAwareSession::RPrivateImpl : public RDmDomain
	{
	};

//
//---------------- class RSsmStateAwareSession ------------------
//

/**
 Default constructor
 */
EXPORT_C RSsmStateAwareSession::RSsmStateAwareSession()
	: iPimpl(NULL)
	{
	}

/**
 Connects to the domain identified by the specified domain Id.
 
 @param aId The identifier of the domain to be connected to.
 @return KErrNone, if successful; otherwise one of the other system-wide error codes. 
 */
EXPORT_C TInt RSsmStateAwareSession::Connect(TDmDomainId aId)
	{
	iPimpl = new RPrivateImpl;
	if(!iPimpl)
		{
		return KErrNoMemory;
		}
	TInt err = iPimpl->Connect(KDmHierarchyIdStartup, aId);
	if (KErrNone != err)
		{
		Close();
		}
	return err;
	}

/**
 Disconnect from the domain.  
 */
EXPORT_C void RSsmStateAwareSession::Close()
	{
	if(iPimpl)
		{
		iPimpl->Close();
		}
	delete iPimpl;
	iPimpl = NULL;
	}

/**
 Reads the System Wide State.
 @return The System Wide State
 @panic ECmnErrState if not connected to a domain
 */
EXPORT_C TSsmState RSsmStateAwareSession::State() const
	{
	__ASSERT_ALWAYS(iPimpl, User::Panic(KPanicSsmCmn, ECmnErrState));
	
	TUint32 ds = 0;
	//returned TDmDomainState is currently only 8 bits
	ds = iPimpl->GetState(); //lint !e613 Possible use of NULL pointer - caught by ASSERT_ALWAYS
	TSsmState state;
	state.SetFromInt(ds);
	return state;
	}

/**
 Use to get a notification when the System State changes.
 @param aStatus The TRequestStatus to be completed when the System State gets changed.
 @panic ECmnErrRqstStateNotif if not connected to a domain
 */
EXPORT_C void RSsmStateAwareSession::RequestStateNotification(TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iPimpl, User::Panic(KPanicSsmCmn, ECmnErrRqstStateNotif));
	iPimpl->RequestTransitionNotification(aStatus);  //lint !e613 Possible use of NULL pointer - caught by ASSERT_ALWAYS
	}

/**
 Cancel an outstanding @c RequestStateNotification operation.
 @panic ECmnErrRqstStateNotifCancel if not connected to a domain
 */
EXPORT_C void RSsmStateAwareSession::RequestStateNotificationCancel()
	{
	__ASSERT_ALWAYS(iPimpl, User::Panic(KPanicSsmCmn, ECmnErrRqstStateNotifCancel));
	iPimpl->CancelTransitionNotification();  //lint !e613 Possible use of NULL pointer - caught by ASSERT_ALWAYS
		
	}

/**
 Acknowledges the state change.
 An application must acknowledge that it has performed all actions required by the last known state of the domain. 
 
 @panic ECmnErrAcknldgStateNotif if not connected to a domain
 */
EXPORT_C void RSsmStateAwareSession::AcknowledgeStateNotification(TInt aError)
	{
	__ASSERT_ALWAYS(iPimpl, User::Panic(KPanicSsmCmn, ECmnErrAcknldgStateNotif));
	iPimpl->AcknowledgeLastState(aError);  //lint !e613 Possible use of NULL pointer - caught by ASSERT_ALWAYS
	}

/**
 Acknowledges the state change.
 An application must acknowledge that it has performed all actions required by the last known state of the domain. 
 This function appears like an atomic function and minimize the risk for missing a state notification.
 @panic ECmnErrAcknldgRqstStateNotif if not connected to a domain
 */
EXPORT_C void RSsmStateAwareSession::AcknowledgeAndRequestStateNotification(TInt aError, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iPimpl, User::Panic(KPanicSsmCmn, ECmnErrAcknldgRqstStateNotif));
	//Typical pattern of using P&S is to subscribe first then get current state
	iPimpl->RequestTransitionNotification(aStatus);  //lint !e613 Possible use of NULL pointer - caught by ASSERT_ALWAYS
	//Tell domain manager that we have processed the last state change.
	iPimpl->AcknowledgeLastState(aError);  //lint !e613 Possible use of NULL pointer - caught by ASSERT_ALWAYS
	}

#ifdef SYMBIAN_INCLUDE_APP_CENTRIC
/**
Defers the acknowledgement requesting more time.
To be sure of deferring in time a client should call this immediately
after receiving notification. This asynchronous call will complete once the original deadline
is reached (ie. one period earlier than the final deadline), at which point the member must either
defer again or acknowledge the transition. In the meantime, the member should perform
its transition actions, whilst remaining responsive to new completion events.

@note Deferrals are not always possible,
whether the member will actually be given more time depends on if
   - The current transition allows deferrals at all.
   - The member still has deferrals left - there may be a maximum number
     allowed.
   - The deferral request was received in time.

@param aStatus Status of request
   - KErrNone Request has completed i.e. The member must either defer again or acknowledge.
   - KErrCompletion The deferral was obsoleted by a subsequent call to AcknowledgeLastState.
   - KErrNotSupported The current transition may not be deferred, or maximum deferral count reached.
   - KErrCancel The deferral was cancelled.
   - KErrNotReady Deferral attempted before a transition notification was received
     or after the deadline for the previous one.
   - KErrPermissionDenied The member lacked the necessary capabilities.
   - KErrAlreadyExists A deferral was already outstanding.
     Both new and existing calls will complete with this error.

This function is provided for members to inform the Domain Manager that they
are still active and are responding to a transition notification.
For example, a server may have to persist data using
the file server before shut down. Since this task should be allowed to complete
before shutdown continues, the member should defer the transition, and then persist
the data, using asynchronous calls.

At least one of the below capabilities is required in order to defer a
domain transition. Without them, the client will get KErrPermissionDenied.

@capability WriteDeviceData
@capability ProtServ

@pre The member has been notified of a transition which it has not yet acknowledged.
 */
EXPORT_C void RSsmStateAwareSession::DeferAcknowledgement(TRequestStatus& aStatus)
    {
    __ASSERT_ALWAYS(iPimpl, User::Panic(KPanicSsmCmn, ECmnErrDeferAcknNotif));    
    iPimpl->DeferAcknowledgement(aStatus); //lint !e613 Possible use of NULL pointer - caught by ASSERT_ALWAYS
    
    }

/**
Cancels the deferred call.
Will cancel a call of DeferAcknowledgement, if one was pending.
If none was pending, it does nothing.
*/
EXPORT_C void RSsmStateAwareSession::CancelDeferral()
    {
    __ASSERT_ALWAYS(iPimpl, User::Panic(KPanicSsmCmn, ECmnErrCancelDeferNotif));    
    iPimpl->CancelDeferral();  //lint !e613 Possible use of NULL pointer - caught by ASSERT_ALWAYS   
    }
#else
EXPORT_C void RSsmStateAwareSession::DeferAcknowledgement(TRequestStatus& aStatus)
    {
    TRequestStatus* pStatus = &aStatus;     
    User::RequestComplete(pStatus, KErrNotSupported);
    }

EXPORT_C void RSsmStateAwareSession::CancelDeferral()
    {
    }
#endif
//
//---------------- class CSsmStateAwareSession ------------------
//

/**
 Factory method that returns an instance of this object, connected to the domain 
 identified by the specified domain Id and setup to subscribe on System State changes.
 @param aId The identifier of the domain to be connected to.
 @return A new instance of this class, connected to the domain @c aId.   
 */
EXPORT_C CSsmStateAwareSession* CSsmStateAwareSession::NewL(TDmDomainId aId)
	{
	CSsmStateAwareSession* self = CSsmStateAwareSession::NewLC(aId);
 	CleanupStack::Pop(self);
 	return self;
	}

/**
 Factory method that returns an instance of this object, connected to the domain 
 identified by the specified domain Id and setup to subscribe on System State changes.
 @param aId The identifier of the domain to be connected to.
 @return A new instance of this class, connected to the domain @c aId.   
 */
EXPORT_C CSsmStateAwareSession* CSsmStateAwareSession::NewLC(TDmDomainId aId)
	{
	CSsmStateAwareSession* self = new (ELeave) CSsmStateAwareSession;
	CleanupStack::PushL(self);
	self->ConstructL(aId);
	return self;
	}

/**
 @internalComponent 
 */
void CSsmStateAwareSession::ConstructL(TDmDomainId aId)
	{
	iMonitor = CSsmStateMonitor::NewL(*this, aId);
	}

/**
 Used to coordinate array granularity with Compress() operations.
 @internalComponent 
 */
const TInt KDefaultPtrArrayGranularity = 8;

/**
 @internalComponent 
 */
CSsmStateAwareSession::CSsmStateAwareSession() : iSubscribers(KDefaultPtrArrayGranularity)
	{
	}

/**
 Destructor
 */
EXPORT_C CSsmStateAwareSession::~CSsmStateAwareSession()
	{
	iSubscribers.Close();
	delete 	iMonitor;
	}

/**
 Reads the System State
 @panic ECmnErrStateMon if not connected to a domain
 @return The System State 
 */
EXPORT_C TSsmState CSsmStateAwareSession::State() const
	{
	__ASSERT_ALWAYS(iMonitor, User::Panic(KPanicSsmCmn, ECmnErrStateMon));
	return iMonitor->State();
	}

/**
 Register for a callback when the System State changes.
 @param aSubscriber The object to receive the callback
 */
EXPORT_C void CSsmStateAwareSession::AddSubscriberL(MStateChangeNotificationSubscriber& aSubscriber)
	{
	iSubscribers.AppendL(&aSubscriber);
	}

/**
 Cancel callback subscription for System State Changes
 @param aSubscriber The object for which to cancel subsription.
 */
EXPORT_C void CSsmStateAwareSession::RemoveSubscriber(const MStateChangeNotificationSubscriber& aSubscriber)
	{
	const TInt index = iSubscribers.Find(&aSubscriber);
	if(index > KErrNotFound)
		{
		iSubscribers.Remove(index);
		
		const TInt count = iSubscribers.Count();
		if((count % KDefaultPtrArrayGranularity) == 0)
			{
			iSubscribers.Compress();
			}
		}
	}

/**
 Called from CSsmStateMonitor when the System State have changed.
 @internalComponent
 */
void CSsmStateAwareSession::NotifySubscribers(TSsmState aSsmState)
	{
	TInt count = iSubscribers.Count();
	while (count--)
		{
#ifdef _DEBUG
			TRAPD(err, iSubscribers[count]->StateChanged(aSsmState));
			if(KErrNone != err)
				{
				DEBUGPRINT2A("Illegal leave (leavecode: %d) detected. Will be ignored", err);
				}
#else
			TRAP_IGNORE(iSubscribers[count]->StateChanged(aSsmState));
#endif
		}
	} //lint !e1746 Suppress parameter 'aSsmState' could be made const reference

//
//---------------- class CExtendedSsmStateAwareSession ------------------
//
#ifdef SYMBIAN_INCLUDE_APP_CENTRIC
/**
Factory method that returns an instance of this object, connected to the domain 
identified by the specified domain Id and setup to subscribe on System State changes.
@param aDomainId    The Id of the domain to connect to.
@param aSubscriber reference to MStateChangeNotificationSubscriber2 
*/
EXPORT_C CSsmStateAwareSession2* CSsmStateAwareSession2::NewL(TDmDomainId aDomainId, MStateChangeNotificationSubscriber2& aSubscriber)
    {
    CSsmStateAwareSession2* self = new (ELeave)CSsmStateAwareSession2(aSubscriber);
    CleanupStack::PushL(self);
    self->ConstructL(aDomainId);
    CleanupStack::Pop(self);
    return self;
    }

CSsmStateAwareSession2::CSsmStateAwareSession2(MStateChangeNotificationSubscriber2& aSubscriber):
CActive(CActive::EPriorityStandard), iDeferNotification(NULL), iSubscriber(&aSubscriber)
    {
    CActiveScheduler::Add(this);
    }

void CSsmStateAwareSession2::ConstructL(TDmDomainId aDomainId)
    {
    User::LeaveIfError(iSsmStateAwareSession.Connect(aDomainId));
    iDeferNotification = new (ELeave) CSsmDeferralMonitor(iSsmStateAwareSession, *this);    
 
    }
/**
Destructor.
Cleanup the internal CSsmDeferralMonitor active object.
*/
EXPORT_C  CSsmStateAwareSession2::~CSsmStateAwareSession2()        
    {
    Cancel();
    delete iDeferNotification;
    iSsmStateAwareSession.Close();
    }

/**
 Gets the TSsmState state.
 */
EXPORT_C TSsmState CSsmStateAwareSession2::GetState()
    {
    return iSsmStateAwareSession.State();
    }

/**
 Use to get a notification when the System State changes.
 */
EXPORT_C void CSsmStateAwareSession2::RequestStateNotification()
    {
    __ASSERT_ALWAYS(!IsActive(), User::Panic(KPanicSsmCmn, ECmnErrRqstStateNotif)); 
    iSsmStateAwareSession.RequestStateNotification(iStatus);
    SetActive();    
    }

/**
 Cancels an outstanding RequestStateNotification operation.
 */
EXPORT_C void CSsmStateAwareSession2::RequestStateNotificationCancel()
    {
	iDeferNotification->Cancel();
    iSsmStateAwareSession.RequestStateNotificationCancel();
    }

/**
 Acknowledges the state change.
 An application must acknowledge that it has performed all actions required by the last known state of the domain.
 @param set aError while acknowledging.
 */
EXPORT_C void CSsmStateAwareSession2::AcknowledgeStateNotification(TInt aError)
    {
    iSsmStateAwareSession.AcknowledgeStateNotification(aError);
    iDeferNotification->NotifyOfAcknowledgement();
    }

/**
 Acknowledges the state change.
 An application must acknowledge that it has performed all actions required by the last known state of the domain. 
 This function appears like an atomic function and minimize the risk for missing a state notification.
 @param aError for aknowledging with error.
 */
EXPORT_C void CSsmStateAwareSession2::AcknowledgeAndRequestStateNotification(TInt aError)
    {
    __ASSERT_ALWAYS(!IsActive(), User::Panic(KPanicSsmCmn, ECmnErrAcknldgRqstStateNotif)); 
    iSsmStateAwareSession.AcknowledgeAndRequestStateNotification(aError,iStatus);
	iDeferNotification->NotifyOfAcknowledgement();
    SetActive();
    }

/**
 Handles the client deferral error.
 @param aError Error code to handle
 */
TInt CSsmStateAwareSession2::HandleDeferralError(TInt aError)
    {
    return (iSubscriber->HandleDeferralError(aError));
    }

/**
 Handle completion of request notifications, begins deferrals.
@note Clients should not need to override this, they
will be notified of events via interface HandleTransition().
 */
void CSsmStateAwareSession2::RunL()
    {
    iDeferNotification->DeferNotification();
    iSubscriber->HandleTransition(iStatus.Int());    
    }

/**
Cancels an outstanding notification request.
Any outstanding notification request completes with KErrCancel.
*/
void CSsmStateAwareSession2::DoCancel()
    {
    iDeferNotification->Cancel();
    iSsmStateAwareSession.RequestStateNotificationCancel();
    }

#else
EXPORT_C CSsmStateAwareSession2* CSsmStateAwareSession2::NewL(TDmDomainId /*aDomainId*/, MStateChangeNotificationSubscriber2& /*aSubscriber*/)
    {
    //This functionality is not supported , so returning NULL.
    return NULL;
    }

//All the below functionality will not be provided.

EXPORT_C  CSsmStateAwareSession2::~CSsmStateAwareSession2()        
    {    
    }

EXPORT_C TSsmState CSsmStateAwareSession2::GetState()
    {
    //returning dummyState to get rid of compiler warning. 
    TSsmState dummyState;
    return dummyState;
    }

EXPORT_C void CSsmStateAwareSession2::RequestStateNotification()
    {    
    }

EXPORT_C void CSsmStateAwareSession2::RequestStateNotificationCancel()
    {    
    }

EXPORT_C void CSsmStateAwareSession2::AcknowledgeStateNotification(TInt /*aError*/)
    {    
    }

EXPORT_C void CSsmStateAwareSession2::AcknowledgeAndRequestStateNotification(TInt /*aError*/)
    {    
    }

void CSsmStateAwareSession2::RunL()
    {        
    }

void CSsmStateAwareSession2::DoCancel()
    {    
    }
	
TInt CSsmStateAwareSession2::HandleDeferralError(TInt aError)
	{
	return aError;
	}

#endif
