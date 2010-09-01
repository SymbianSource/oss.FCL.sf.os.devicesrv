// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "simadaptationref.h"

/**
Function to create new Sim Adaptation Plugin.

@return	a new plugin object for Sim Adaptations.
*/
EXPORT_C MSimAdaptation* CreateSimAdaptationRefL()
	{
	CSimAdaptationRef* simAdaptationRef = CSimAdaptationRef::NewL();
	return (static_cast<MSimAdaptation*>(simAdaptationRef));
	}

CSimAdaptationRef* CSimAdaptationRef::NewL()
	{
	CSimAdaptationRef* self = new(ELeave) CSimAdaptationRef;
	
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();

	return self;
	}

CSimAdaptationRef::~CSimAdaptationRef()
	{
	delete iTimer;
	}

CSimAdaptationRef::CSimAdaptationRef()
	{
	}

void CSimAdaptationRef::ConstructL()
	{
	iTimer = CSimRefAdaptationTimer::NewL();
	}

//from MSimAdaptation
void CSimAdaptationRef::Release()
	{
	delete this;
	}

void CSimAdaptationRef::GetSimOwned(TDes8& /*aOwnedPckg*/, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	TRequestStatus* pStatus = &aStatus;
	User::RequestComplete(pStatus, KErrNone);
	}

/**
  Cancel the outstanding request. Reference implementation completes the requests immediately so there is nothing to Cancel.
  On a device, Cancel() needs an implementation as the Request might be outstanding and it needs to be cancelled.
*/
void CSimAdaptationRef::GetCancel()
	{
	}

/**
  The reference implementation completes with KErrNotSupported since there is no SIM support on HRP/Techview.
  On a device, Sim Adaptation Plug-in would complete 'aTypePckg' with one of the event types in TSsmSimEventType.
  
  
  The above mentioned implementation is modified to facilitate testing and increase the code coverage of the Adaptation 
  server code.The modified functionality is as follows.
  
  
  Instead of completing the notification request with KErrNotSupported the it is passed to 
  CSimAdaptationRefEventHandler which is an active object wiht lower priority compared to 
  the server active object.Because of this a delay will be introduced in completing the notification
  request so that other active objects can run avoiding infinite loop and starvation of other 
  active objects,which will be the case if the notification request is immediately completed.
  This is strictly for testing purposes.On a device this call will be replaced by Sim Adaptation
  Plug-in  completing 'aTypePckg' with one of the event types in TSsmSimEventType.

  
  
*/
void CSimAdaptationRef::NotifySimEvent(TDes8& /*aTypePckg*/, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iTimer->After(2000000,aStatus);
	}

/**
  Cancel the outstanding request. Reference implementation completes the requests immediately so there is nothing to Cancel.
  On a device, Cancel() needs an implementation as the Request might be outstanding and it needs to be cancelled.
*/
void CSimAdaptationRef::NotifyCancel()
	{
	if(iTimer->IsActive())
		{
		iTimer->Cancel();			
		}
	}



CSimRefAdaptationTimer::CSimRefAdaptationTimer():CTimer(CActive::EPriorityUserInput)
	{
   	CActiveScheduler::Add(this);
	}

CSimRefAdaptationTimer::~CSimRefAdaptationTimer()
	{
	Cancel();	
	}

CSimRefAdaptationTimer* CSimRefAdaptationTimer::NewL()
	{
   	CSimRefAdaptationTimer* self=new (ELeave) CSimRefAdaptationTimer();
   	CleanupStack::PushL(self);
   	self->ConstructL();
   	CleanupStack::Pop();
    return self;		
	}

void CSimRefAdaptationTimer::After(TTimeIntervalMicroSeconds32 aCancelDelay, TRequestStatus& aStatus)
	{
	iReqStatus = &aStatus;
	if(!IsActive())
	CTimer::After(aCancelDelay);	
	}



void CSimRefAdaptationTimer::DoCancel()
	{
	User::RequestComplete(iReqStatus, KErrCancel);	
	CTimer::DoCancel();	
	}


void CSimRefAdaptationTimer::RunL()
	{
	User::RequestComplete(iReqStatus, KErrNone);	
	}

	
