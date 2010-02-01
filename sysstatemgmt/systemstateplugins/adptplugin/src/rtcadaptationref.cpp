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

#include "rtcadaptationref.h"

#include <e32property.h>

const TUid KAlarmServerUID = {0x101f5027};
const TInt KTestRTCValueKey = 200;

/**
Function to create new Rtc Adaptation Plugin.

@return	a new instance of MRtcAdaptation for RTC (Real Time Clock) Adaptations.
*/
EXPORT_C MRtcAdaptation* CreateRtcAdaptationRefL()
	{
	CRtcAdaptationRef* rtcAdaptationRef = CRtcAdaptationRef::NewL();
	return (static_cast<MRtcAdaptation*>(rtcAdaptationRef));
	}

CRtcAdaptationRef* CRtcAdaptationRef::NewL()
	{
	CRtcAdaptationRef* self = new(ELeave) CRtcAdaptationRef;
	return self;	
	}

CRtcAdaptationRef::~CRtcAdaptationRef()
	{
	}

CRtcAdaptationRef::CRtcAdaptationRef()
	{
	}

/**
 Deletes and frees memory allocated.
*/
void CRtcAdaptationRef::Release()
	{
	delete this;
	}

/**
 Check that the RTC is valid.
 Reference implementation completes the request with KErrNotSupported as support for RTC is not available on Techview/H4 hrp.

 @param aValidityPckg on return contains the status of the validity of the RTC as a boolean value
 @param aStatus to complete when the operation has finished

 @see TRequestStatus
*/
void CRtcAdaptationRef::ValidateRtc(TDes8& /*aValidityPckg*/, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNotSupported);
	}

/**
 Set a device wake-up alarm time, in UTC (coordinated universal time), in the RTC.
 Reference implementation completes the request with KErrNotSupported as support for RTC is not available on Techview/H4 hrp.
 For testing purposes it sets a pub sub property defined in test code.

 @param aAlarmTimePckg requested wake up time
 @param aStatus to complete when the operation has finished

 @see TRequestStatus
*/
void CRtcAdaptationRef::SetWakeupAlarm(TDesC8& aAlarmTimePckg, TRequestStatus& aStatus)
	{
	// Set this pub sub property (for testing purposes)
	// The property is defined in the test code.  In normal operation this will fail silently because the property has not been defined.
	RProperty::Set(KAlarmServerUID, KTestRTCValueKey, aAlarmTimePckg);

	aStatus = KRequestPending;
	TRequestStatus* status = &aStatus;
	// No support for RTC on HRP/Techview.
	User::RequestComplete(status, KErrNotSupported);
	}

/**
 Delete the current device wake-up alarm time in the RTC.
 Reference implementation completes the request with KErrNotSupported as support for RTC is not available on Techview/H4 hrp.
 For testing purposes it sets a pub sub property to a NULL value defined in test code.

 @param aStatus to complete when the operation has finished

 @see TRequestStatus
*/
void CRtcAdaptationRef::UnsetWakeupAlarm(TRequestStatus& aStatus)
	{
	// Set this pub sub property to a NULL value because we are unsetting the RTC (for testing purposes)
	// The property is defined in the test code. In normal operation this will fail silently because the property has not been defined.
	TTime nullTime(Time::NullTTime());
	TPckgC<TTime> wakeupAlarmTimePckg(nullTime);
	RProperty::Set(KAlarmServerUID, KTestRTCValueKey, wakeupAlarmTimePckg);

	aStatus = KRequestPending;
	TRequestStatus* status = &aStatus;
	// No support for RTC on HRP/Techview.
	User::RequestComplete(status, KErrNotSupported);
	}

/**
  Cancel the outstanding request. Reference implementation completes the requests immediately so there is nothing to Cancel.
  On a device, Cancel() needs an implementation as the Request might be outstanding and it needs to be cancelled.
*/
void CRtcAdaptationRef::Cancel()
	{
	}
