// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "stateadaptationref.h"

/**
Static method to create new State Adaptation Plugin.

@return	a new plugin object for State Adaptation.
*/
EXPORT_C MStateAdaptation* CreateStateAdaptationL()
	{
	CStateAdaptationRef* stateAdaptationRef = CStateAdaptationRef::NewL();
	return (static_cast<MStateAdaptation*>(stateAdaptationRef));
	}

CStateAdaptationRef* CStateAdaptationRef::NewL()
	{
	CStateAdaptationRef* self = new(ELeave) CStateAdaptationRef;
	
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();

	return self;
	}

CStateAdaptationRef::~CStateAdaptationRef()
	{
	delete iTimer;
	}

CStateAdaptationRef::CStateAdaptationRef()
	{
	}

void CStateAdaptationRef::ConstructL()
	{
	iTimer = CStateRefAdaptationTimer::NewL();
	}

//from MStateAdaptation
void CStateAdaptationRef::Release()
	{
	delete this;
	}

void CStateAdaptationRef::RequestCoopSysStateChange(TSsmState /*aState*/, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CStateAdaptationRef::RequestCoopSysSelfTest(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CStateAdaptationRef::RequestCoopSysPerformRestartActions(TInt /*aReason*/, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CStateAdaptationRef::RequestCoopSysPerformShutdownActions(TInt /*aReason*/, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CStateAdaptationRef::RequestCoopSysPerformRfsActions(TSsmRfsType /*aRfsType*/, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

/**
  Cancel the notification request. Reference implementation completes the requests immediately so there is nothing to Cancel.
  On a device, Cancel() needs an implementation as the Request might be outstanding and it needs to be cancelled.
*/
void CStateAdaptationRef::RequestCancel()
	{
	}

/**
  The reference implementation completes with KErrNotSupported since there isn't a Cooperating System on HRP/Techview.
  On a device, State Adaptation Plug-in would request for notification from the Cooperating System for 'aEvent'.
  
  The above mentioned implementation is modified to facilitate testing and increase the code coverage of the Adaptation 
  server code.The modified functionality is as follows.
  
  
  Instead of completing the notification request with KErrNotSupported the it is passed to 
  CStateAdaptationRefEventHandler which is an active object wiht lower priority compared to 
  the server active object.Because of this a delay will be introduced in completing the notification
  request so that other active objects can run avoiding infinite loop and starvation of other 
  active objects,which will be the case if the notification request is immediately completed.
  This is strictly for testing purposes.On a device this call will be replaced by a notification request
  to cooperative system.
  
  
*/
void CStateAdaptationRef::NotifyCoopSysEvent(TDes8& /*aEvent*/, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iTimer->After(2000000,aStatus);
	}

/**
  Cancel the notification request. Reference implementation completes the requests immediately so there is nothing to Cancel.
  On a device, Cancel() needs an implementation as the Request might be outstanding and it needs to be cancelled.
*/
void CStateAdaptationRef::NotifyCancel()
	{
	if(iTimer->IsActive())
		{
		iTimer->Cancel();			
		}	
	}



CStateRefAdaptationTimer::CStateRefAdaptationTimer():CTimer(CActive::EPriorityUserInput)
	{
   	CActiveScheduler::Add(this);
	}

CStateRefAdaptationTimer::~CStateRefAdaptationTimer()
	{
	Cancel();	
	}

CStateRefAdaptationTimer* CStateRefAdaptationTimer::NewL()
	{
   	CStateRefAdaptationTimer* self=new (ELeave) CStateRefAdaptationTimer();
   	CleanupStack::PushL(self);
   	self->ConstructL();
   	CleanupStack::Pop();
    return self;		
	}

void CStateRefAdaptationTimer::After(TTimeIntervalMicroSeconds32 aCancelDelay, TRequestStatus& aStatus)
	{
	iReqStatus = &aStatus;
	if(!IsActive())
	CTimer::After(aCancelDelay);	
	}



void CStateRefAdaptationTimer::DoCancel()
	{
	User::RequestComplete(iReqStatus, KErrCancel);	
	CTimer::DoCancel();	
	}


void CStateRefAdaptationTimer::RunL()
	{
	User::RequestComplete(iReqStatus, KErrNone);	
	}

	
