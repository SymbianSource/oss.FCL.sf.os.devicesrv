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
// Name        : strtdevlockcheck.cpp
// Part of     : System Startup / StrtDevLockPlg
// Implementation of CStrtDevLockCheck class.
// Version     : %version: 1 %
// This material, including documentation and any related computer
// programs, is protected by copyright controlled by Nokia.  All
// rights are reserved.  Copying, including reproducing, storing,
// adapting or translating, any or all of this material requires the
// prior written consent of Nokia.  This material also contains
// confidential information which may not be disclosed to others
// without the prior written consent of Nokia.
// Template version: 4.1.1
// Nokia Core OS *
// File renamed from strtdevlockcheck.cpp to cmddevicesecuritycheck.cpp as part of Core OS transfer.
//




/**
 @file
 @internalComponent
 @released
*/

#include "ssmsecuritychecknotifier.h"
#include "cmddevicesecuritycheck.h"
#include "ssmuiproviderdll.h"
#include "ssmrefcustomcmdcommon.h"
#include "ssmdebug.h"

CCustomCmdDeviceSecurityCheck* CCustomCmdDeviceSecurityCheck::NewL()
	{
	CCustomCmdDeviceSecurityCheck* self = new (ELeave) CCustomCmdDeviceSecurityCheck();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CCustomCmdDeviceSecurityCheck::ConstructL()
	{
    TInt errorCode = iServer.Connect();

	if(KErrNone != errorCode)
		{
		DEBUGPRINT2A("Failed to connect to TelServer with error %d", errorCode);
		User::Leave(errorCode);
		}

    iTsyModuleName = CSsmUiSpecific::GetTsyModuleNameL();
    errorCode = iServer.LoadPhoneModule(*iTsyModuleName);

    if (KErrNone != errorCode && KErrAlreadyExists != errorCode)
        {
        // KErrAlreadyExists may be returned if some other has already loaded
        // the TSY module.
        DEBUGPRINT2A("Failed to load phone module with error %d", errorCode);
        User::Leave(errorCode);
        }

    HBufC* tsyPhoneName = CSsmUiSpecific::PhoneTsyNameL();
    errorCode = iPhone.Open(iServer, *tsyPhoneName);
    delete tsyPhoneName;

    if(KErrNone != errorCode)
    	{
    	DEBUGPRINT2A("Failed to open phone subsession with error %d", errorCode);
    	User::Leave(errorCode);	
    	}

	//Add active object to active scheduler
	CActiveScheduler::Add(this);
	}

CCustomCmdDeviceSecurityCheck::CCustomCmdDeviceSecurityCheck()
								: CActive(EPriorityStandard),
								iState(EGetLockPhoneDevice),
								iLockInfoPckg(iLockInfo)
	{
	}

CCustomCmdDeviceSecurityCheck::~CCustomCmdDeviceSecurityCheck()
	{
	ExecuteCancel();
    delete iSsmSecurityCheckNotifier;

    iPhone.Close();
    if (iServer.Handle())
        {
        iServer.UnloadPhoneModule(*iTsyModuleName);
        iServer.Close();
        }
    delete iTsyModuleName;
	}

TInt CCustomCmdDeviceSecurityCheck::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
	return KErrNone;
	}

void CCustomCmdDeviceSecurityCheck::Close()
	{
	}

void CCustomCmdDeviceSecurityCheck::Release()
	{
	delete this;
	}

void CCustomCmdDeviceSecurityCheck::Execute(const TDesC8& /*aParams*/, TRequestStatus& aStatus)
	{
	//Set the user request to pending
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;

#ifdef __WINS__
#ifdef TEST_CUSTCMD_MACRO
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
#else
    // The device lock status can not be read in the emulator.
    // Just pretend the lock is not enabled.
	TInt errorCode = RProperty::Set(CSsmUiSpecific::StarterPSUid(), KStarterSecurityPhase, EStarterSecurityPhaseSimOk);
    CompleteClientRequest(errorCode);
#endif //TEST_CUSTCMD_MACRO

#else
    if (CSsmUiSpecific::IsNormalBoot() || CSsmUiSpecific::IsSimChangedReset())
        {
        iPhone.GetLockInfo( iStatus, RMobilePhone::ELockPhoneDevice, iLockInfoPckg );
        SetActive();
        }
    else
        {
        TInt errorCode = RProperty::Set(
        		CSsmUiSpecific::StarterPSUid(), KStarterSecurityPhase, EStarterSecurityPhaseSecOk );
        CompleteClientRequest( errorCode );
        }
#endif //__WINS__
	}

void CCustomCmdDeviceSecurityCheck::ExecuteCancel()
	{
	//Call cancel method of CActive	
	Cancel();
	}

void CCustomCmdDeviceSecurityCheck::RunL()
	{
    switch (iState)
        {
        case EGetLockPhoneDevice:
            LockPhoneDeviceRespReceivedL();
            break;
        case EGetLockPhoneToIcc:
            LockPhoneToIccRespReceivedL();
            break;
        case EQuerySecCode:
            SecCodeQueryRespReceivedL();
            break;
        default:
        	__ASSERT_ALWAYS((iState <= EDeviceSecurityCheckNone) || (iState >= EDeviceSecurityCheckMaxState),
        					PanicNow(KPanicCustomCmdDeviceSecurityCheck, EInvalidDeviceSecurityState));
            break;
        }
	}

void CCustomCmdDeviceSecurityCheck::LockPhoneDeviceRespReceivedL()
    {
    if (iStatus.Int() == KErrNone)
        {
        DEBUGPRINT3A("ELockPhoneDevice: status: %d, setting: %d",
                iLockInfo.iStatus, iLockInfo.iSetting);

        DEBUGPRINT3A("ELockPhoneDevice: RMobilePhone::EStatusLocked: %d, RMobilePhone::ELockSetEnabled: %d",
                RMobilePhone::EStatusLocked, RMobilePhone::ELockSetEnabled);

        //Ask for security code if status is locked and lock is enabled
        if (iLockInfo.iStatus == RMobilePhone::EStatusLocked &&
             iLockInfo.iSetting == RMobilePhone::ELockSetEnabled)
        	{
            QuerySecCodeL();
            }
        else
            {
            DEBUGPRINT2A("ELockPhoneDevice: Check ELockPhoneToICC while SimPresent = %d", CSsmUiSpecific::IsSimPresent());
            iState = EGetLockPhoneToIcc;
            iPhone.GetLockInfo(
                iStatus, RMobilePhone::ELockPhoneToICC, iLockInfoPckg );
            SetActive();
            }
        }
    else
        {
        CompleteClientRequest(iStatus.Int());
        }
    }

void CCustomCmdDeviceSecurityCheck::LockPhoneToIccRespReceivedL()
    {
    if (iStatus.Int() == KErrNone)
        {
        DEBUGPRINT4A("1ELockPhoneDevice: status: %d, setting: %d, IsSimPresent: %d",
                iLockInfo.iStatus, iLockInfo.iSetting, CSsmUiSpecific::IsSimPresent());

        DEBUGPRINT3A("1ELockPhoneDevice: RMobilePhone::EStatusLocked: %d, RMobilePhone::ELockSetEnabled: %d",
                RMobilePhone::EStatusLocked, RMobilePhone::ELockSetEnabled);

        if (iLockInfo.iSetting == RMobilePhone::ELockSetEnabled &&
           (EFalse == CSsmUiSpecific::IsSimPresent() || ( iLockInfo.iStatus == RMobilePhone::EStatusLocked)))
            {
            QuerySecCodeL();
            }
        else
            {
            CompleteClientRequest(iStatus.Int());
            }
        }
    else
        {
        CompleteClientRequest(iStatus.Int());
        }
    }

void CCustomCmdDeviceSecurityCheck::SecCodeQueryRespReceivedL()
    {
    __ASSERT_DEBUG(NULL != iSsmSecurityCheckNotifier, PanicNow(KPanicCustomCmdDeviceSecurityCheck, ECmdNullPtr));
    if (iStatus.Int() == KErrNone &&  iSsmSecurityCheckNotifier->SecurityCheckResult() == KErrNone)
        {
        CompleteClientRequest(KErrNone);
        }
    else if (iStatus.Int() != KErrNone)
        {
        // Fatal error
        CompleteClientRequest(iStatus.Int());
        }
    else
        {
        // Wrong code, try again
        QuerySecCodeL(); 
        }
    }

void CCustomCmdDeviceSecurityCheck::QuerySecCodeL()
    {
    iState = EQuerySecCode;
    TInt errorCode = KErrNone;

    if (!iSsmSecurityCheckNotifier)
        {
        TRAP(errorCode, iSsmSecurityCheckNotifier = CSsmSecurityCheckNotifier::NewL(ESecCodePasswd));
        }

    if (KErrNone == errorCode)
        {
        iSsmSecurityCheckNotifier->ShowNoteL(iStatus);
        SetActive();
        }
    else
        {
        DEBUGPRINT2A("Failed to create CSsmSecurityCheckNotifier with error %d", errorCode);
        CompleteClientRequest(errorCode);
        }
    }

void CCustomCmdDeviceSecurityCheck::DoCancel()
	{
    if (iSsmSecurityCheckNotifier)
        {
        iSsmSecurityCheckNotifier->Cancel();
        }
    iPhone.CancelAsyncRequest(EMobilePhoneGetLockInfo);
    CompleteClientRequest(KErrCancel);
	}

TInt CCustomCmdDeviceSecurityCheck::RunError(TInt aError)
	{
	//Call cancel method of CActive	
	Cancel();
	CompleteClientRequest(aError);
	return KErrNone;
	}

void CCustomCmdDeviceSecurityCheck::CompleteClientRequest(TInt aReason)
	{
	//Complete client request with reason code
	if (iExecuteRequest)
        {
    	TInt errorCode = RProperty::Set(CSsmUiSpecific::StarterPSUid(), KStarterSecurityPhase, 
    			aReason == KErrNone ? EStarterSecurityPhaseSecOk : EStarterSecurityPhaseSecNok );
        User::RequestComplete(iExecuteRequest, aReason);
        }
	}
