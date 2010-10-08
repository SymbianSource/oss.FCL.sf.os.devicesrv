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
// Contributors:
//
// Description: Wrapper for Testing CStateAwareSession2 class.
//
//
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#ifndef __T_STATEAWARESESSIONWRAPPER2_H__
#define __T_STATEAWARESESSIONWRAPPER2_H__

#include <ssm/ssmstatemanager.h>
#include <ssm/ssmstateawaresession.h>

#include <test/testexecutestepbase.h>
#include <test/datawrapper.h>

// This structure is used to hold data read from the test ini file for CTestCStateAwareSession
struct TIniData3
    {
    TBool iAcknowledge;
    TInt iAsyncIndex;
    TIniData3();
    };

class CTestCStateAwareSession : public CDataWrapper, public MStateChangeNotificationSubscriber2
    {
public:
    CTestCStateAwareSession();
    ~CTestCStateAwareSession();

    TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/);

    TAny* GetObject();
    void  SetObjectL(TAny* aAny);
    void  DisownObjectL();
    
    void DoCmdNew();
    void DoCmdDestructor();
    void DoCmdRequestStateNotification();
    void DoCmdAcknowledgeStateNotification(const TDesC& aSection);
    void DoCmdAcknowledgeAndRequestStateNotification(const TDesC& aSection);
    void DoCmdRequestStateNotificationCancel();
    void DoCmdReturnCount(const TDesC& aSection);
    void DoCmdReadIniData(const TDesC& aSection);
    
    //helper
    void DoCleanup();
    
    //from MStateChangeNotificationSubscriber2
    void HandleTransition(TInt aError);
    TInt HandleDeferralError(TInt aError);
    
private:
    CSsmStateAwareSession2* iSsmStateAwareSession;
    TIniData3 iIniData;
    };

#endif // __T_STATEAWARESESSIONWRAPPER_H__
