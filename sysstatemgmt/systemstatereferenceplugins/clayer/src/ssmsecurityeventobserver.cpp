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
// Name        : strtsecurityeventobserver.cpp
// Part of     : System Startup / StrtSecObs
// Implementation of CStrtSecurityEventObserver class
// Version     : %version: 1 % << Don't touch! Updated by Synergy at check-out.
// This material, including documentation and any related computer
// programs, is protected by copyright controlled by Nokia.  All
// rights are reserved.  Copying, including reproducing, storing,
// adapting or translating, any or all of this material requires the
// prior written consent of Nokia.  This material also contains
// confidential information which may not be disclosed to others
// without the prior written consent of Nokia.
// Template version: 4.1.1
// Nokia Core OS *
// File renamed from strtsecurityeventobserver.cpp to ssmsecurityeventobserver.cpp as part of Core OS transfer.
//



#include "strtsecphaseobserver.h"
#include "ssmsecurityeventobserver.h"
#include "ssmsecuritynotecontroller.h"
#include "ssmuiproviderdll.h"
#include "ssmdebug.h"

const TInt KDefaultPhoneIndex = 0;

EXPORT_C MSsmUtility* CStrtSecurityEventObserver::NewL()
    {
    CStrtSecurityEventObserver* self = new( ELeave ) CStrtSecurityEventObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self );
    return self;
    }

void CStrtSecurityEventObserver::InitializeL()
	{

	}
void CStrtSecurityEventObserver::StartL()
	{
	Activate();
	}
void CStrtSecurityEventObserver::Release()
	{
	delete this;
	}

CStrtSecurityEventObserver::~CStrtSecurityEventObserver()
    {
    Cancel();
    iPhone.Close();

    if (iTelServer.Handle())
        {
        iTelServer.UnloadPhoneModule(*iTsyModuleName);
        iTelServer.Close();
        }
    delete iTsyModuleName;
    delete iController;
    delete iSecPhaseObserver;
    }

void CStrtSecurityEventObserver::DoCancel()
    {
    iPhone.CancelAsyncRequest(EMobilePhoneNotifySecurityEvent);
    }

void CStrtSecurityEventObserver::RunL()
    {
    TInt errorCode = iStatus.Int();
    if (errorCode == KErrNone)
        {
        DEBUGPRINT2A("SecurityEvent %d occurred", iEvent);
        ProcessSecurityEvent();
        Activate();
        }
    else
    	{
    	DEBUGPRINT2A("Security Event Observer received error: %d - not reactivating", iStatus.Int());
    	}
    }

CStrtSecurityEventObserver::CStrtSecurityEventObserver()
  : CActive(EPriorityStandard),
    iSecPhaseObserver(NULL),
    iController(NULL)    
    {
    CActiveScheduler::Add(this);
    }

void CStrtSecurityEventObserver::ConstructL()
    {
    iTsyModuleName = CSsmUiSpecific::GetTsyModuleNameL();
    
    TInt errorCode = iTelServer.Connect();
    User::LeaveIfError(errorCode);

    errorCode = iTelServer.LoadPhoneModule(*iTsyModuleName);
    if (errorCode != KErrNone && errorCode != KErrAlreadyExists)
        {
        // KErrAlreadyExists may be returned if some other has already loaded
        // the TSY module.
        User::Leave(errorCode);
        }

    RTelServer::TPhoneInfo info;
    errorCode = iTelServer.GetPhoneInfo(KDefaultPhoneIndex, info);
    User::LeaveIfError(errorCode);

    errorCode = iPhone.Open(iTelServer, info.iName);
    User::LeaveIfError(errorCode);

    iSecPhaseObserver = CStrtSecPhaseObserver::NewL();
    iController = CStrtSecurityNoteController::NewL();
    }

void CStrtSecurityEventObserver::Activate()
    {
    iPhone.NotifySecurityEvent(iStatus, iEvent);
    SetActive();
    }

void CStrtSecurityEventObserver::ProcessSecurityEvent()
    {
    switch (iEvent)
        {
        case RMobilePhone::EPin1Required:
            SimCodeRequest(ESecCodePIN1);
            break;
        case RMobilePhone::EPuk1Required:
            SimCodeRequest(ESecCodePUK1);
            break;
        case RMobilePhone::EPin2Required:
            SimCodeRequest(ESecCodePIN2);
            break;
        case RMobilePhone::EPuk2Required:
            SimCodeRequest(ESecCodePUK2);
            break;
        case RMobilePhone::EPhonePasswordRequired:
            SecCodeRequest(ESecCodePasswd);
            break;
        case RMobilePhone::EPin1Verified:
            CodeVerifyIndication(ESecCodePIN1);
            break;
        case RMobilePhone::EPin2Verified:
            CodeVerifyIndication(ESecCodePIN2);
            break;
        case RMobilePhone::EPuk1Verified:
            CodeVerifyIndication(ESecCodePUK1);
            break;
        case RMobilePhone::EPuk2Verified:
            CodeVerifyIndication(ESecCodePUK2);
            break;
        case RMobilePhone::EPhonePasswordVerified:
            CodeVerifyIndication(ESecCodePasswd);
            break;
        case RMobilePhone::EUSIMAppPinRequired:
            OtherCodeRequest(ESecCodeAppPIN1);
            break;
        case RMobilePhone::EUSIMAppPinVerified:
            CodeVerifyIndication(ESecCodeAppPIN1);
            break;
        case RMobilePhone::ESecondUSIMAppPinRequired:
            OtherCodeRequest(ESecCodeAppPIN2);
            break;
        case RMobilePhone::ESecondUSIMAppPinVerified:
            CodeVerifyIndication(ESecCodeAppPIN2);
            break;
        case RMobilePhone::EUniversalPinRequired:
            SimCodeRequest(ESecCodeUPIN);
            break;
        case RMobilePhone::EUniversalPinVerified:
            CodeVerifyIndication(ESecCodeUPIN);
            break;
        case RMobilePhone::EUniversalPukRequired:
            SimCodeRequest(ESecCodeUPUK);
            break;
        case RMobilePhone::EUniversalPukVerified:
            CodeVerifyIndication(ESecCodeUPUK);
            break;
        default:
        	DEBUGPRINT2A("Event %d not handled", iEvent);
            break;
        }
    }

void CStrtSecurityEventObserver::SimCodeRequest(
    const TStrtSecurityNoteType aNoteType )
    {
    DEBUGPRINT2A("Sim code request: %d", aNoteType);
    if (iSecPhaseObserver->IsSecurityPhaseSimOk())
        {
        iController->SecurityNoteRequested(aNoteType);
        }
    else
        {
        DEBUGPRINT1A("Startup SIM phase is not over yet, discarding request");
        }
    }

void CStrtSecurityEventObserver::SecCodeRequest(
    const TStrtSecurityNoteType aNoteType )
    {
    DEBUGPRINT2A("Security code request: %d", aNoteType);
    if (iSecPhaseObserver->IsSecurityPhaseSecOk())
        {
        iController->SecurityNoteRequested(aNoteType);
        }
    else
        {
        DEBUGPRINT1A("Startup SEC phase is not over yet, discarding request");
        }
    }

void CStrtSecurityEventObserver::OtherCodeRequest(
    const TStrtSecurityNoteType aNoteType )
    {
    DEBUGPRINT2A("Other code request: %d", aNoteType);
    iController->SecurityNoteRequested(aNoteType);
    }

void CStrtSecurityEventObserver::CodeVerifyIndication(
    const TStrtSecurityNoteType aNoteType )
    {
    DEBUGPRINT2A("Security code verified: %d", aNoteType);
    iController->SecurityCodeVerified(aNoteType);
    }
