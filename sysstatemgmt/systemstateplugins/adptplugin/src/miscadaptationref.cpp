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

#include "miscadaptationref.h"
#include <ssm/startupadaptationcommands.h>

_LIT(KTestCmdSecurityCheckTestFile, "c:\\cmdsecuritychecktest\\pinchecksecuritycaseno.txt");

/**
 Function to create new Misc Adaptation Plugin. Used for a collection of activities.

 @return	new instance of MEmergencyCallRfAdaptation for Miscellaneous Adaptations.

 @internalComponent
 @released
*/
EXPORT_C MMiscAdaptation* CreateMiscAdaptationRefL()
	{
	CMiscAdaptationRef* miscAdaptationRef = CMiscAdaptationRef::NewL();
	return (static_cast<MMiscAdaptation*>(miscAdaptationRef));
	}

CMiscAdaptationRef* CMiscAdaptationRef::NewL()
	{
	CMiscAdaptationRef* self = new(ELeave) CMiscAdaptationRef;
	self->ConstructL();
	return self;
	}

CMiscAdaptationRef::~CMiscAdaptationRef()
	{
	iFs.Close();
	}

CMiscAdaptationRef::CMiscAdaptationRef()
	{
	}

void CMiscAdaptationRef::ConstructL()
	{
	iTestCaseNum = 1;
	User::LeaveIfError(iFs.Connect());
	}

/** 
 Deletes and frees memory allocated.
*/
void CMiscAdaptationRef::Release()
	{
	delete this;
	}

/**
 Get the start-up mode from the Cooperating System.
 Reference implementation completes with KErrNone as a cooperating system does not exist on Techview/H4 hrp.

 @param aModePckg should contain start-up of the Cooperating System on completion
 @param aStatus to complete when the operation has finished

 @see TRequestStatus
*/
void CMiscAdaptationRef::GetGlobalStartupMode(TDes8& /*aModePckg*/, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

/**
 Prepare language codes stored in the SIM in preferred language lists. The next call should be GetSimLanguagesL()
 Reference implementation completes with KErrNotSupported as SIM support is not available on Techview/H4 hrp.

 @param aPriority priority for which the language lists need to be prepared
 @param aSizePckg on return contains the size of the buffer, which a client should allocate and pass as one of the parameters (TInt aCount) to GetSimLanguagesL()
 @param aStatus to complete when the operation has finished

 @see TSsmLanguageListPriority
 @see TRequestStatus
 @see GetSimLanguagesL
*/
void CMiscAdaptationRef::PrepareSimLanguages(TSsmLanguageListPriority /*aPriority*/, TDes8& /*aSizePckg*/, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	TRequestStatus* status = &aStatus;
	// No support for SIM on H4hrp/Techview.
	User::RequestComplete(status, KErrNotSupported);
	}

/**
 Get language codes stored in the SIM in preferred language lists. PrepareSimLanguages() call should precede this call.
 Reference implementation completes with KErrNotSupported as SIM support is not available on Techview/H4 hrp.

 @param aBuf should contain the language lists on completion
 @param aCount to contain the count of the languages

 @see PrepareSimLanguages
*/
void CMiscAdaptationRef::GetSimLanguagesL(CBufBase* /*aBuf*/, TInt /*aCount*/)
	{
	User::Leave(KErrNotSupported);
	}

void CMiscAdaptationRef::GetHiddenReset(TDes8& /*aHiddenResetPckg*/, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	TRequestStatus* status = &aStatus;
	// No support for Reset in HRP/Techview.
	User::RequestComplete(status, KErrNotSupported);
	}


/**
  Cancel the outstanding request. Reference implementation completes the requests immediately so there is nothing to Cancel.
  On a device, Cancel() needs an implementation as the Request might be outstanding and it needs to be cancelled.
*/
void CMiscAdaptationRef::Cancel()
	{
	}

/**
 Request the next security state.
 Techview/H4hrp does not support SIM operations so a reference implementation would always return 'KErrNotSupported'.
 The function is modified to suit testing needs for PinCheck Security Command.
 A real implementation is intended to differ a lot based on the runtime changes/events on the device.
 
 Each switch case represents a PIN Check security plug-in functionality in different status are as follows..
 
 EPINCHECK01 : SIM present and PIN1 required.
 EPINCHECK02 : SIM present and rejected
 EPINCHECK03 : SIM blocked PUK required
 EPINCHECK04 : SIM locked security code required.
 EPINCHECK05 : SIM locked security code required when sim lockRestrictionOn
 EPINCHECK06 : SIM blocked and UPUK.
 EPINCHECK07 : SIM blocked security code required when sim lockRestrictionOn
 EPINCHECK08 : SIM present and PIN1 required and SimLock status is wrong
 iTestCaseNum will be reset to 1 at EPINCHECK08 to test all above scenarios for wrong entered pin
 
 @param aState next security state
 @param aResponsePckg response to state change request on completion
 @param aStatus to complete when the operation has finished

 @see TRequestStatus
*/
void CMiscAdaptationRef::SecurityStateChange(TInt aState, TDes8& aResponsePckg, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	TRequestStatus* status = &aStatus;
	StartupAdaptation::TSecurityStateInfo info = StartupAdaptation::EYes;
	
	//Read Test case number from the file 
	TInt err = iFile.Open(iFs, KTestCmdSecurityCheckTestFile, EFileRead);
	if (err == KErrNone)
		{
		TRAP(err,iTestCaseNum = iFile.ReadInt32L());
		iFile.Close();
		if(err)
			{
			RDebug::Printf("Error while reading tst case number from the file pinchecksecuritycaseno.txt");
			User::RequestComplete(status, err);
			return;
			}
		}	
	switch(iTestCaseNum)
		{
		case EPINCHECK01:
			switch(aState)
			{
			case StartupAdaptation::ESIMPresent:
				info = StartupAdaptation::EYes;
				break;
            case StartupAdaptation::ESIMInvalid:
                info = StartupAdaptation::ENo;
                break;
			case StartupAdaptation::ESIMRejected:
				info = StartupAdaptation::ENo;
				break;
			case StartupAdaptation::ESIMBlocked:
				info = StartupAdaptation::ENo;
				break;
			case StartupAdaptation::EPINRequired:
				info = StartupAdaptation::EPIN1Required;
				break;
			case StartupAdaptation::ESIMLock:
				info = StartupAdaptation::ESimLockOk;
				break;
			case StartupAdaptation::ESecurityCheckOK:
				info = StartupAdaptation::EYes;	
				break;	
			case StartupAdaptation::ESecurityCheckFailed:
				info = StartupAdaptation::EYes;
				break;	
			default:
				break;
			}
			break;
		case EPINCHECK02:
			switch(aState)
			{
			case StartupAdaptation::ESIMPresent:
				info = StartupAdaptation::EYes;
				break;
            case StartupAdaptation::ESIMInvalid:
                info = StartupAdaptation::ENo;
                break;
			case StartupAdaptation::ESIMRejected:
				info = StartupAdaptation::EYes;
				break;
			case StartupAdaptation::ESecurityCheckFailed:
				info = StartupAdaptation::EYes;
				break;	
			default:
			break;
			}
			break;
		case EPINCHECK03:
			switch(aState)
			{
			case StartupAdaptation::ESIMPresent:
				info = StartupAdaptation::EYes;
				break;
            case StartupAdaptation::ESIMInvalid:
                info = StartupAdaptation::ENo;
                break;
			case StartupAdaptation::ESIMRejected:
				info = StartupAdaptation::ENo;
				break;
			case StartupAdaptation::ESIMBlocked:
				info = StartupAdaptation::EPUK1Required;
				break;	
			case StartupAdaptation::ESIMLock:
				info = StartupAdaptation::ESimLockOk;
				break;
			case StartupAdaptation::ESecurityCheckOK:
				info = StartupAdaptation::EYes;	
				break;	
			case StartupAdaptation::ESecurityCheckFailed:
				info = StartupAdaptation::EYes;
				break;	
			default:
				break;
			}
			break;
		case EPINCHECK04:
			switch(aState)
			{
			case StartupAdaptation::ESIMPresent:
				info = StartupAdaptation::EYes;
				break;
            case StartupAdaptation::ESIMInvalid:
                info = StartupAdaptation::ENo;
                break;
			case StartupAdaptation::ESIMRejected:
				info = StartupAdaptation::ENo;
				break;
			case StartupAdaptation::ESIMBlocked:
				info = StartupAdaptation::EPUK1Required;
				break;	
			case StartupAdaptation::ESIMLock:
				info = StartupAdaptation::ESimLockOk;
				break;
			case StartupAdaptation::ESecurityCheckOK:
				info = StartupAdaptation::EYes;	
				break;	
			case StartupAdaptation::ESecurityCheckFailed:
				info = StartupAdaptation::EYes;
				break;	
			default:
				break;
			}	
			break;
		case EPINCHECK05:
			switch(aState)
			{
			case StartupAdaptation::ESIMPresent:
				info = StartupAdaptation::EYes;
				break;
            case StartupAdaptation::ESIMInvalid:
                info = StartupAdaptation::ENo;
                break;
			case StartupAdaptation::ESIMRejected:
				info = StartupAdaptation::ENo;
				break;
			case StartupAdaptation::ESIMBlocked:
				info = StartupAdaptation::EUPUKRequired;
				break;	
			case StartupAdaptation::ESIMLock:
				info = StartupAdaptation::ESimLockRestrictionOn;
				break;
			case StartupAdaptation::ESecurityCheckOK:
				info = StartupAdaptation::EYes;	
				break;	
			case StartupAdaptation::ESecurityCheckFailed:
				info = StartupAdaptation::EYes;
				break;	
			default:
				break;	
			}	
			break;
		case EPINCHECK06:
			switch(aState)
			{
			case StartupAdaptation::ESIMPresent:
				info = StartupAdaptation::EYes;
				break;
            case StartupAdaptation::ESIMInvalid:
                info = StartupAdaptation::ENo;
                break;
			case StartupAdaptation::ESIMRejected:
				info = StartupAdaptation::ENo;
				break;
			case StartupAdaptation::ESIMBlocked:
				info = StartupAdaptation::EUPUKRequired;
				break;	
			case StartupAdaptation::ESIMLock:
				info = StartupAdaptation::ESimLockRestrictionPending;
				break;
			case StartupAdaptation::ESecurityCheckOK:
				info = StartupAdaptation::EYes;	
				break;	
			case StartupAdaptation::ESecurityCheckFailed:
				info = StartupAdaptation::EYes;	
				break;	
			default:
				break;	
			}
			break;
		case EPINCHECK07:
			switch(aState)
			{
			case StartupAdaptation::ESIMPresent:
				info = StartupAdaptation::EYes;
				break;
            case StartupAdaptation::ESIMInvalid:
                info = StartupAdaptation::ENo;
                break;
			case StartupAdaptation::ESIMRejected:
				info = StartupAdaptation::ENo;
				break;
			case StartupAdaptation::ESIMBlocked:
				info = StartupAdaptation::EUPUKRequired;
				break;	
			case StartupAdaptation::ESIMLock:
				info = StartupAdaptation::ESimLockRestricted;
				break;
			case StartupAdaptation::ESecurityCheckOK:
				info = StartupAdaptation::EYes;	
				break;	
			case StartupAdaptation::ESecurityCheckFailed:
				info = StartupAdaptation::EYes;	
				break;	
			default:
				break;	
			}
			break;
		case EPINCHECK08:
		switch(aState)
			{
			case StartupAdaptation::ESIMPresent:
				info = StartupAdaptation::EYes;
				break;
			case StartupAdaptation::ESIMInvalid:
			    info = StartupAdaptation::ENo;
			    break;
			case StartupAdaptation::ESIMRejected:
				info = StartupAdaptation::ENo;
				break;
			case StartupAdaptation::ESIMBlocked:
				info = StartupAdaptation::ENo;
				break;
			case StartupAdaptation::EPINRequired:
				info = StartupAdaptation::EPIN1Required;
				break;
			case StartupAdaptation::ESIMLock:
				info = StartupAdaptation::ESimLockOk;
				break;
			case StartupAdaptation::ESecurityCheckOK:
				info = StartupAdaptation::EYes;	
				break;	
			case StartupAdaptation::ESecurityCheckFailed:
				info = StartupAdaptation::EYes;	
				break;	
			default:
				break;
			}	
			break;	
		default:
			break;
		}
	TPckgBuf<StartupAdaptation::TSecurityStateInfo> securityStateInfoResult(info);
	aResponsePckg = securityStateInfoResult;	
	User::RequestComplete(status, KErrNone);
	}

