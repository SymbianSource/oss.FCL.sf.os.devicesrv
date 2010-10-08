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
// Description: Wrapper for testing RStateAwareSession class.
//
//
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#ifndef __T_STATEAWARESESSIONWRAPPER_H__
#define __T_STATEAWARESESSIONWRAPPER_H__

#include <ssm/ssmstatemanager.h>
#include <ssm/ssmstateawaresession.h>

#include <test/testexecutestepbase.h>
#include <test/datawrapper.h>

// This structure is used to hold data read from the test ini file for CTestRStateAwareSession1
struct TIniData1
    {
    TBool iCancelDeferral;
    TBool iAckLastState;
    TBool iDeferTwice;
    TInt  iCount;
    TBool iAcknowledge;
    TIniData1();
    };

// This structure is used to hold data read from the test ini file for CTestRStateAwareSession2
struct TIniData2
    {
    TInt  iCount;
    TBool iAcknowledge;
    TIniData2();
    };

//Client 1
class CTestRStateAwareSession1 : public CDataWrapper
    {
public:
    static CTestRStateAwareSession1*    NewL();
    ~CTestRStateAwareSession1();

    TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

    TAny* GetObject();
    void  SetObjectL(TAny* aAny);
    void  DisownObjectL();
    
    
    void DoCmdNew();
    void DoCmdDestructor();
    void DoCmdConnect(const TDesC& aSection);
    void DoCmdClose();
    void DoCmdRequestStateNotification(const TInt aAsyncErrorIndex);
    void DoCmdAcknowledgeStateNotification(const TDesC& aSection);
    void DoCmdAcknowledgeAndRequestStateNotification(const TDesC& aSection, const TInt aAsyncErrorIndex);
    void DoCmdRequestStateNotificationCancel();
    void DoCmdDeferAcknowledgement(const TInt aAsyncErrorIndex);
    void DoCmdReturnCount(const TDesC& aSection);
    void DoCmdReadIniData(const TDesC& aSection);
    
    //helper
    void DoCleanup();

protected:
    CTestRStateAwareSession1();
    void    ConstructL();

    void    RunL(CActive* aActive, TInt aIndex);
    void    DoCancel(CActive* aActive, TInt aIndex);


private:
    RSsmStateAwareSession* iSsmStateAwareSession;
    CActiveCallback* iActiveNotifyOnChange;
    CActiveCallback* iActiveNotifyOnChange2;
    // The iCount variable is used to store the no. of deferral calls made. 
    // It is incremented each time a deferral call is made.
    TInt iCount;  
    TIniData1 iIniData;
    };

//Client 2
class CTestRStateAwareSession2 : public CDataWrapper
    {
public:
    static CTestRStateAwareSession2*    NewL();
    ~CTestRStateAwareSession2();

    TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

    TAny* GetObject();
    void  SetObjectL(TAny* aAny);
    void  DisownObjectL();
    
    
    void DoCmdNew();
    void DoCmdDestructor();
    void DoCmdConnect(const TDesC& aSection);
    void DoCmdClose();
    void DoCmdRequestStateNotification(const TInt aAsyncErrorIndex);
    void DoCmdAcknowledgeStateNotification(const TDesC& aSection);
    void DoCmdAcknowledgeAndRequestStateNotification(const TDesC& aSection, const TInt aAsyncErrorIndex);
    void DoCmdRequestStateNotificationCancel();
    void DoCmdReadIniData(const TDesC& aSection);
    
    //helper
    void DoCleanup();

protected:
    CTestRStateAwareSession2();
    void    ConstructL();

    void    RunL(CActive* aActive, TInt aIndex);
    void    DoCancel(CActive* aActive, TInt aIndex);


private:
    RSsmStateAwareSession* iSsmStateAwareSession;
    CActiveCallback* iActiveNotifyOnChange;
    TInt iCount;
    TIniData2 iIniData;
    };

#endif // __T_STATEAWARESESSIONWRAPPER_H__
