// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
//Contributors:
//
// Description: Test wrapper for CSsmStateAwareSession2

#include <e32std.h>
#include <e32capability.h>
#include <test/datawrapper.h>
#include <ssm/ssmdomaindefs.h> 
#include <ssm/ssmsubstates.hrh>
#include "ssmsubstateext.hrh"

#include "t_stateawaresession2.h"

const TDmDomainId KTestDdmDomain = {KSM2AppServicesDomain3};

//
_LIT(KError,                     "error");
_LIT(KAsyncIndex,                "asyncIndex");
_LIT(KAcknowledge,               "acknowledge");

//commands
_LIT(KCmdNewL,                                      "NewL");
_LIT(KCmdReadIniData,                               "ReadIniData");
_LIT(KCmdRequestStateNotification,                  "RequestStateNotification");
_LIT(KCmdAcknowledgeStateNotification,              "AcknowledgeStateNotification");
_LIT(KCmdRequestStateNotificationCancel,            "RequestStateNotificationCancel");
_LIT(KCmdDestructor,                                "~");

/**
 * Constructor
 */
TIniData3::TIniData3()
         : iAcknowledge(ETrue)
         , iAsyncIndex(0)
         {}
/**
* @return pointer to the object that the data wraps
*/
TAny*  CTestCStateAwareSession::GetObject()
    {
    return iSsmStateAwareSession;
    }

/**
 * SetObjectL
 */
void CTestCStateAwareSession::SetObjectL(TAny* aAny)
    {
    DoCleanup();
    iSsmStateAwareSession=static_cast<CSsmStateAwareSession2*> (aAny);
    }

/**
 * DisownObjectL
 */
void CTestCStateAwareSession::DisownObjectL()
    {
    iSsmStateAwareSession=NULL;
    }

/**
 * Constructor.
 */
CTestCStateAwareSession::CTestCStateAwareSession():
CDataWrapper()
    {
    }

/**
 * Destructor.
 */
CTestCStateAwareSession::~CTestCStateAwareSession()
    {
    DoCleanup();
    }

/**
 * Process a command read from the ini file
 *
 * @param aCommand  the command to process
 * @param aSection      the entry in the ini file requiring the command to be processed
 *
 * @return ETrue if the command is processed
 */
TBool CTestCStateAwareSession::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
    {
    TBool retVal = ETrue;
    if (aCommand == KCmdNewL)
        {
        DoCmdNew();
        }
    else if (aCommand == KCmdReadIniData)
        {
        DoCmdReadIniData(aSection);
        }
    else if (aCommand == KCmdAcknowledgeStateNotification)
        {
        DoCmdAcknowledgeStateNotification(aSection);
        }
    else if (aCommand == KCmdRequestStateNotification)
        {
        DoCmdRequestStateNotification();
        }
    else if (aCommand == KCmdRequestStateNotificationCancel)
        {
        DoCmdRequestStateNotificationCancel();
        }
    else if (aCommand == KCmdDestructor)
        {
        DoCmdDestructor();
        }
    else
        {
        retVal = EFalse;
        }    
   
    return retVal;
    }

/**
 * Creates CSsmStateAwareSession2 class instance
 */
void CTestCStateAwareSession::DoCmdNew()
    {
    INFO_PRINTF1(_L("Create CSsmStateAwareSession2 class instance"));
    DoCleanup();
    
    TRAPD(err, iSsmStateAwareSession = CSsmStateAwareSession2::NewL(KTestDdmDomain, *this));
    
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("CSsmStateAwareSession2::NewL() error %d"), err);
        SetError(err);
        }
    }

/*
 * Helper function to read ini file data
 */
void CTestCStateAwareSession::DoCmdReadIniData(const TDesC& aSection)
    {
    if( !GetIntFromConfig(aSection, KAsyncIndex(), iIniData.iAsyncIndex ) ||
        !GetBoolFromConfig(aSection, KAcknowledge(), iIniData.iAcknowledge ) )
        {
        ERR_PRINTF1(_L("Error reading ini file data"));
        SetError(KErrNotFound);
        }
    }

/**
 * Contains cleanup implementation
 */
void CTestCStateAwareSession::DoCleanup()
    {
    if(iSsmStateAwareSession != NULL)
        {
        INFO_PRINTF1(_L("Deleting current CSsmStateAwareSession2"));
        delete iSsmStateAwareSession;
        iSsmStateAwareSession = NULL;
        }
    }

/**
 * Destroy CSsmStateAwareSession2 object
 */
void CTestCStateAwareSession::DoCmdDestructor()
    {
    INFO_PRINTF1(_L("Destroying the CStateAwareSession2 object"));
    DoCleanup();
    }

/**
 * DoCmdRequestStateNotification 
 */
void CTestCStateAwareSession::DoCmdRequestStateNotification()
    {
    INFO_PRINTF1(_L("CTestCStateAwareSession: RequestStateNotification"));
    iSsmStateAwareSession->RequestStateNotification();
    IncOutstanding();
    }

/**
 * DoCmdAcknowledgeStateNotification
 */
void CTestCStateAwareSession::DoCmdAcknowledgeStateNotification(const TDesC& aSection)
    {
    INFO_PRINTF1(_L("CTestCStateAwareSession: AcknowledgeStateNotification"));
    TInt error;
    if (!GetIntFromConfig(aSection, KError(), error ))
        {
        //set default error value if value is not provided in .ini file
        error = KErrNone;
        }
   iSsmStateAwareSession->AcknowledgeStateNotification(error);    
    }

/**
 * DoCmdAcknowledgeAndRequestStateNotification
 */
void CTestCStateAwareSession::DoCmdAcknowledgeAndRequestStateNotification(const TDesC& aSection)
    {
    INFO_PRINTF1(_L("CTestCStateAwareSession: AcknowledgeAndRequestStateNotification"));
    TInt error;
    if (!GetIntFromConfig(aSection, KError(), error ))
        {
        //set default error value if value is not provided in .ini file
        error = KErrNone;
        }
    iSsmStateAwareSession->AcknowledgeAndRequestStateNotification(error);
    IncOutstanding();
    }

/**
 * DoCmdRequestStateNotificationCancel
 */
void CTestCStateAwareSession::DoCmdRequestStateNotificationCancel()
    {
    INFO_PRINTF1(_L("CTestCStateAwareSession: RequestStateNotificationCancel"));
    iSsmStateAwareSession->RequestStateNotificationCancel();
    }

/**
 * HandleTransition
 */
void CTestCStateAwareSession::HandleTransition(TInt aError)
    { 
    INFO_PRINTF1(_L("CTestCStateAwareSession::HandleTransition"));
    TSsmState currentState;
    currentState = iSsmStateAwareSession->GetState();
    TUint16 currentMainState = currentState.MainState();
    TUint16 currentSubState = currentState.SubState();
    if(currentMainState == ESsmShutdown && 
       currentSubState == ESsmShutdownSubStateCritical && 
       iIniData.iAcknowledge)
        {    
        iSsmStateAwareSession->AcknowledgeStateNotification(aError);
        }
    }

/**
 * HandleDeferralError
 */
TInt CTestCStateAwareSession::HandleDeferralError(TInt aError)
    {
    INFO_PRINTF2(_L("CTestCStateAwareSession::HandleDeferralError error = %d"), aError);
    RDebug::Printf("Entered CTestCStateAwareSession::HandleDeferralError() with error %d", aError);
    TSsmState currentState;
    currentState = iSsmStateAwareSession->GetState();
    TUint16 currentMainState = currentState.MainState();
    TUint16 currentSubState = currentState.SubState();
    if(currentMainState == ESsmShutdown && 
       currentSubState == ESsmNormalShutdown)
        {
        iSsmStateAwareSession->AcknowledgeAndRequestStateNotification(KErrNone);
        IncOutstanding();
        }
    else
        {
        SetAsyncError(iIniData.iAsyncIndex, aError);
        }
    DecOutstanding();
    return KErrNone;
    }
