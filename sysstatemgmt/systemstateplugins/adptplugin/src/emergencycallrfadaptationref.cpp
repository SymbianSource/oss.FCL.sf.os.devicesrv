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

#include "emergencycallrfadaptationref.h"

/**
 Function to create new Emergency Call RF Adaptation Plugin.

 @return	new instance of MEmergencyCallRfAdaptation for Emergency Call RF Adaptations.

 @internalComponent
 @released
*/
EXPORT_C MEmergencyCallRfAdaptation* CreateEmergencyCallRfAdaptationRefL()
	{
	CEmergencyCallRfAdaptationRef* emergencyCallRfAdaptation = CEmergencyCallRfAdaptationRef::NewL();
	return (static_cast<MEmergencyCallRfAdaptation*> (emergencyCallRfAdaptation));
	}

CEmergencyCallRfAdaptationRef* CEmergencyCallRfAdaptationRef::NewL()
	{
	CEmergencyCallRfAdaptationRef* self = new(ELeave) CEmergencyCallRfAdaptationRef;
	return self;
	}

CEmergencyCallRfAdaptationRef::~CEmergencyCallRfAdaptationRef()
	{
	}

CEmergencyCallRfAdaptationRef::CEmergencyCallRfAdaptationRef()
	{
	}

/**
 Deletes and frees memory allocated.
*/
void CEmergencyCallRfAdaptationRef::Release()
	{
	delete this;
	}

/**
 Request RF activation so that an emergency call can be made.
 Reference implementation completes the request with KErrNone as support for RF activation is not available on Techview/H4 hrp.

 @param aStatus to complete when the operation has finished
 @see TRequestStatus 
*/
void CEmergencyCallRfAdaptationRef::ActivateRfForEmergencyCall(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

/**
 Request RF deactivation after an emergency call have been made.
 Reference implementation completes the request with KErrNone as support for RF deactivation is not available on Techview/H4 hrp.

 @param aStatus to complete when the operation has finished
 @see TRequestStatus 
*/
void CEmergencyCallRfAdaptationRef::DeactivateRfForEmergencyCall(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);	
	}

/**
  Cancel the outstanding request. Reference implementation completes the requests immediately so there is nothing to cancel.
  On a device, Cancel() needs an implementation as the Request might be outstanding and it needs to be cancelled.
*/
void CEmergencyCallRfAdaptationRef::Cancel()
	{
	}
