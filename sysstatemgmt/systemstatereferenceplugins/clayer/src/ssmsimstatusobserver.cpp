// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ssmsimstatusobserver.h"
#include "ssmuiproviderdll.h"
#include "ssmdebug.h"
#include "startupdomainpskeys.h"
#include <ssm/ssmstatemanager.h>

#include <e32property.h>
#include <ssm/ssmstates.hrh>
#include <ssm/startupreason.h>
#include <ssm/starterclient.h>

/**
 * Allocates and returns a new SIM status observer.
 * 
 * Uses the parameters provided by the SSM UI provider DLL for object construction.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C MSsmUtility* CSsmSimStatusObserver::NewL()
	{
	CSsmSimStatusObserver* self = new (ELeave) CSsmSimStatusObserver();
	return static_cast<MSsmUtility*>(self);
	}
	
/**
 * Frees any resources allocated to this object. 
 * 
 * @internalComponent
 */
CSsmSimStatusObserver::~CSsmSimStatusObserver()
	{
	Deque();
	iSimAdaptation.Close();
	iSimStatusProperty.Close();
	}
	
/**
 * Initialises this object
 * 
 * @internalComponent
 */
void CSsmSimStatusObserver::InitializeL()
	{
	TInt err = iSimAdaptation.Connect();
	if(KErrNone != err)
		{
		DEBUGPRINT2A("CSsmSimStatusObserver error connecting to SIM adaptation: %d", err);
		User::Leave(err);
		}
	iPreviousEventType = (TSsmSimEventType) -1; 
	}

/**
 * Starts requesting notification for SIM events.
 * 
 * @internalComponent
 */
void CSsmSimStatusObserver::StartL()
	{
	iSimAdaptation.NotifySimEvent(iSimEventPckg, iStatus);
	SetActive();
	}

/**
 * Destroys this object and frees any resources
 * allocated to it.
 * 
 * @internalComponent
 */
void CSsmSimStatusObserver::Release()
	{
	delete this;
	}

/**
 * CActive::RunL() implementation
 * 
 * @internalComponent
 */
void CSsmSimStatusObserver::RunL()
	{
	if(KErrNone != iStatus.Int())
		{
		DEBUGPRINT2A("CSsmSimStatusObserver RunL status error: %d", iStatus.Int());
		User::Leave(iStatus.Int());
		}
	// The adaptation will have queued any events, so we shouldn't miss any while we handle this event
	switch(iSimEventPckg())
		{
		case ESsmSimUsable:
			RequestSimStatusChangeL(ESimUsable);
			break;
		case ESsmSimReadable:
			RequestSimStatusChangeL(ESimReadable);
			break;
		case ESsmSimNotReady:
			RequestSimStatusChangeL(ESimNotReady);
			break;
		case ESsmSimRemoved:
			RequestSimStatusChangeL(ESimNotPresent);
			break;
		default:
			DEBUGPRINT2A("CSsmSimStatusObserver unknown SIM event: %d", iSimEventPckg());
			User::Leave(KErrNotSupported);
			break;
		}
	iPreviousEventType = iSimEventPckg();
	// Re-request notification
	iSimAdaptation.NotifySimEvent(iSimEventPckg, iStatus);
	SetActive();
	}

/**
 * Requests a change to the SIM status system wide property.
 * 
 * @internalComponent
 */
void CSsmSimStatusObserver::RequestSimStatusChangeL(TPSSimStatus aSimStatus)
	{
	DEBUGPRINT2A("CSsmSimStatusObserver SIM status change: %d", aSimStatus);
	
	if ( (ESimNotPresent != aSimStatus) && ( ESsmSimRemoved == iPreviousEventType))
		{
		DEBUGPRINT1A("CSsmSimStatusObserver Switching Off the RF");
		RStarterSession startersession;
		if ( KErrNone == startersession.Connect())		
			{
			//IGNORE the error returned by DeactivateRfAfterEmergencyCall()
			TInt err = startersession.DeactivateRfAfterEmergencyCall();
			DEBUGPRINT2A("CSsmSimStatusObserver DeactivateRfAfterEmergencyCall() returned err: %d", err);
			}		
		startersession.Close();
		}
	
	RSsmStateManager ssm;
	User::LeaveIfError(ssm.Connect());
	CleanupClosePushL(ssm);
	const TUint KStartupSimSecurityStatusKey = {CSsmUiSpecific::SimStatusPropertyKey()};
	TSsmSwp swpInfo(KStartupSimSecurityStatusKey, aSimStatus);
	User::LeaveIfError(ssm.RequestSwpChange(swpInfo));
	CleanupStack::PopAndDestroy(&ssm);
	}

/**
 * Cancels the outstanding notification request for this observer.
 * 
 * @internalComponent
 */
void CSsmSimStatusObserver::DoCancel()
	{
	iSimAdaptation.NotifyCancel();
	}

/**
 * CActive::RunError implementation
 * 
 * @internalComponent
 */
#ifdef _DEBUG
TInt CSsmSimStatusObserver::RunError(TInt aError)
#else 
TInt CSsmSimStatusObserver::RunError(TInt)
#endif // _DEBUG
	{
	DEBUGPRINT2A("CSsmSimStatusObserver RunError received error: %d", aError);
	//Errors are thrown if SWP transitions are not allowed.
	//So, handles such errors gracefully.
	return KErrNone;
	}

/**
 * Constructs this object using the parameters provided by the SSM UI provider DLL.
 * 
 * @internalComponent
 */
CSsmSimStatusObserver::CSsmSimStatusObserver()
: CActive(EPriorityNormal)
	{
	CActiveScheduler::Add(this);
	}
	
