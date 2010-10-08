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

#include <ssm/ssmstateawaresession.h>
#include "ssmstatemonitor.h"
#include "cmnpanic.h"
#include "ssmdebug.h"

/**

 */
CSsmStateMonitor* CSsmStateMonitor::NewL(CSsmStateAwareSession& aObserver, TDmDomainId aId)
	{
	CSsmStateMonitor* self = new (ELeave) CSsmStateMonitor(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aId);
	CleanupStack::Pop(self);
	return self;
	}

/**

 */
CSsmStateMonitor::CSsmStateMonitor(CSsmStateAwareSession& aObserver) : 
	CActive(CActive::EPriorityStandard),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

/**

 */
CSsmStateMonitor::~CSsmStateMonitor()
	{
	Cancel();
	iSas.Close();
	}

/**

 */
void CSsmStateMonitor::ConstructL(TDmDomainId aId)
	{
	SSMLOGLEAVEIFERROR(iSas.Connect(aId)); //will leave in release builds as well
	
	//Setup change subscription
	iStatus = KRequestPending;
	iSas.RequestStateNotification(iStatus);
	SetActive();
	
	//Read current state and store locally
	iState = iSas.State();
	}

/**

 */
void CSsmStateMonitor::DoCancel()
	{
	iSas.RequestStateNotificationCancel();
	}

/**

 */
void CSsmStateMonitor::RunL()
	{
	SSMLOGLEAVEIFERROR(iStatus.Int());
	
	//Re-issue a subscription request before retrieving the current value
	iStatus = KRequestPending;
	iSas.AcknowledgeAndRequestStateNotification(KErrNone, iStatus);
	SetActive();
	
	//Update local copy
	iState = iSas.State();
	
	//Notify our clients
	iObserver.NotifySubscribers(iState);			
	}

/**
@panic ECmnStateMonError1 if an error occurs in RunL
 */
TInt CSsmStateMonitor::RunError(TInt aError)
	{
	iSas.AcknowledgeStateNotification(aError);
	DEBUGPRINT2A("CSsmStateMonitor::RunError: %d", aError);
	User::Panic(KPanicSsmCmn, ECmnStateMonError1); //A panic here is easier to track than a panic in CActiveScheduler
	return aError;  //lint !e527 Unreachable
	}

/**

 */
TSsmState CSsmStateMonitor::State() const
	{
	return iState;
	}


CSsmDeferralMonitor::CSsmDeferralMonitor(RSsmStateAwareSession& aStateAwareSession, CSsmStateAwareSession2& aOwnerActiveObject)
    : CActive(CActive::EPriorityHigh), iSsmStateAwareSession(aStateAwareSession), iOwnerActiveObject(aOwnerActiveObject), iCeaseDeferral(EFalse)
    {
    CActiveScheduler::Add(this);
    }

CSsmDeferralMonitor::~CSsmDeferralMonitor()
    {
    Cancel();
    }

/**
 * Defers the Acknowledgement.
 */
void CSsmDeferralMonitor::DeferNotification()
    {
    __ASSERT_ALWAYS(!IsActive(), User::Panic(KPanicSsmCmn, ECmnErrDeferNotif)); 
    iSsmStateAwareSession.DeferAcknowledgement(iStatus);
    SetActive();
    }

/**
Informs the object that the state transition has
been _successfully_ acknowledged
*/
void CSsmDeferralMonitor::NotifyOfAcknowledgement()
    {
    if (IsActive())
        {
        iCeaseDeferral = ETrue;
        }
    }

void CSsmDeferralMonitor::RunL()
    {
    const TInt error = iStatus.Int();
    TBool ceaseDeferral = iCeaseDeferral;
    iCeaseDeferral = EFalse;
    // We are leaving with error after resetting iCeaseDeferral. This is because
    // we might again RequestStateNotification() in HandleDeferralError(), in which case 
    // we need to reset iCeaseDeferral to EFalse so that deferral may happen again.
    User::LeaveIfError(error);

    if(!ceaseDeferral)
        {
        DeferNotification();
        }
	else
        {
        // At this point we know error == KErrNone
        // However, we return the error code KErrCompletion, as this
        // is what would have happened, had the acknowledgment come in
        // a little earlier,
        // whilst the deferral was still outstanding on the server.
        User::Leave(KErrCompletion);
        }
    }

/**
 Handle errors thrown from RunL() - call HandleDeferralErrror()    
*/
TInt CSsmDeferralMonitor::RunError(TInt aError)
    {
    DEBUGPRINT2A("CSsmDeferralMonitor::RunError: %d", aError);
    return iOwnerActiveObject.HandleDeferralError(aError);
    }

/**
 * Cancels the outstanding deferral request.
 */
void CSsmDeferralMonitor::DoCancel()
    {
    iSsmStateAwareSession.CancelDeferral();
    }


