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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "susadaptionserver.h"

#include "tsus_step_adaptionserver.h"

/**
 * Client interface to simulate Heap failure at server side
 */
TInt RTestSusAdaptationCli::SetHeapFailure(TInt aFailureRate)
    {
    if(Handle())
        {
        return SendReceive(EDebugSetHeapFailure, TIpcArgs(aFailureRate));
        }
    return KErrDisconnected;
    }

/**
 * Client interface to restore Heap failure at server side
 */
TInt RTestSusAdaptationCli::UnSetHeapFailure()
    {
    if(Handle())
        {
        return SendReceive(EDebugUnSetHeapFailure);
        }
    return KErrDisconnected;
    }
 
CSusAdaptionServerTest::~CSusAdaptionServerTest()
	{
	}

CSusAdaptionServerTest::CSusAdaptionServerTest()
	{
	SetTestStepName(KTSusAdaptionServerStep);
	}

/** */
TVerdict CSusAdaptionServerTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

/** 
 Old Test CaseID 		APPFWK-SUS-0011
 New Test CaseID 		DEVSRVS-SSMA-SUS-0011
 */

TVerdict CSusAdaptionServerTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSusAdaptionServerTest tests started...."));

	__UHEAP_MARK;		
	TRAPD(err, doTestForMemoryLeaksL());
	TEST(err == KErrNone);

	TRAP(err, doTestProcessCriticalL());
	TEST(KErrNone == err || KErrAlreadyExists == err);
	TRAP(err, doTestOOML());
	TEST(err == KErrNone);	
	
	doTestForNormalEmergencyCallinOOM();
	doTestForEmergencyCallOOM();
	doTestForSettingPriorityClient();
	
	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("....CSusAdaptionServerTest tests completed!"));
	return TestStepResult();	
	}

/** */
TVerdict CSusAdaptionServerTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}


void CSusAdaptionServerTest::doTestForMemoryLeaksL()
	{
	INFO_PRINTF1(_L("doTestForMemoryLeaksL tests started...."));
	
	INFO_PRINTF1(_L("Checking for memoryleaks in SusAdaptionServer destructor"));

	INFO_PRINTF1(_L("Create active scheduler."));
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL (sched );
	CActiveScheduler::Install (sched );
	
	__UHEAP_MARK;
	StartAndDestroyServerL();
	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("Destroy active scheduler."));
	CleanupStack::PopAndDestroy(sched);

	INFO_PRINTF1(_L("doTestForMemoryLeaksL tests completed."));
	}


	
void CSusAdaptionServerTest::doTestProcessCriticalL()
	{
	INFO_PRINTF1(_L("doTestProcessCriticalL tests started...."));

	INFO_PRINTF1(_L("Checking that SusAdaptionServer sets it thread to critical"));
	
	INFO_PRINTF1(_L("Get the critical state of the current thread"));
	User::TCritical before = User::Critical();
	INFO_PRINTF3(_L("The critical state of the current thread: expected %d, actual %d"), User::ENotCritical, before);
	TESTL(before == User::ENotCritical);
	
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL (sched );
	CActiveScheduler::Install (sched );
	INFO_PRINTF1(_L("Create SSM adaptation server."));
	CSsmAdaptationServer* server = CSsmAdaptationServer::NewLC();
	
	INFO_PRINTF1(_L("Get the critical state of the current thread"));
	User::TCritical critical = User::Critical();
	INFO_PRINTF3(_L("The critical state of the current thread: expected %d, actual %d"), User::ESystemCritical, critical);
	TEST(critical == User::ESystemCritical);

	INFO_PRINTF1(_L("Destroy SSM adaptation server."));
	CleanupStack::PopAndDestroy(server);
	CleanupStack::PopAndDestroy(sched);
	
	User::TCritical after = User::Critical();
	INFO_PRINTF3(_L("The critical state of the current thread: expected %d, actual %d"), User::ENotCritical, after);
	TESTL(after == User::ENotCritical);

	INFO_PRINTF1(_L("doTestProcessCriticalL tests completed."));
	}
	
void CSusAdaptionServerTest::doTestOOML()
	{
	INFO_PRINTF1(_L("doTestOOML tests started...."));

	INFO_PRINTF1(_L("Create active scheduler."));
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL (sched );
	CActiveScheduler::Install (sched );
	
	TInt ret = KErrNoMemory;
	for(TInt fail=0; ret == KErrNoMemory; fail++)
		{
		__UHEAP_FAILNEXT(fail);
		__UHEAP_MARK;
		INFO_PRINTF2(_L("Iteration #: %d"), fail);
		TRAP(ret, StartAndDestroyServerL());
		INFO_PRINTF2(_L("StartAndDestroyServerL() returned: %d"), ret);
		__UHEAP_SETFAIL(RHeap::ENone,0);// turn failure off
		__UHEAP_MARKEND;
		TEST((ret==KErrNoMemory || ret==KErrNone));
		}

	INFO_PRINTF1(_L("Destroy active scheduler."));
	CleanupStack::PopAndDestroy(sched);
	INFO_PRINTF1(_L("doTestOOML tests completed."));
	}

void CSusAdaptionServerTest::StartAndDestroyServerL()
	{
	INFO_PRINTF1(_L("Create SSM adaptation server."));
	CSsmAdaptationServer* server = NULL;
	TRAPD(err, server = CSsmAdaptationServer::NewLC(); CleanupStack::Pop(server));
	TEST(err == KErrAlreadyExists || err == KErrNone);
	if(err != KErrNone && err != KErrAlreadyExists)
		User::LeaveIfError(err);
	INFO_PRINTF1(_L("Destroy SSM adaptation server."));
	delete server;
	}

/**
 * Tests for client calling Activate/Deactivate Rf without setting as priorityclient.
 */
void CSusAdaptionServerTest::doTestForNormalEmergencyCallinOOM()
    {
    INFO_PRINTF1(_L("doTestForNormalEmergencyCallinOOM started."));
    __UHEAP_MARK;
    RTestSusAdaptationCli adaptationclitest;    
    TInt err = adaptationclitest.Connect();
    INFO_PRINTF2(_L("Connect() returned Error %d : Expected Error is KErrNone."),err);
    TEST(err == KErrNone);
    TRequestStatus status;
    // Simulate OOM condition
    User::__DbgSetAllocFail(EFalse, RAllocator::EDeterministic, 1);
    User::__DbgSetAllocFail(ETrue, RAllocator::EDeterministic, 1);
    //Simulate OOM at Serverside
    adaptationclitest.SetHeapFailure(1);
    //Normal client calling ActivateRfForEmergencyCall Rf in OOM condition. This call must fail with KErrNoMemory 
    //as there is no memory reserved.
    adaptationclitest.ActivateRfForEmergencyCall(status);
    // Wait for completion of requests
    User::WaitForRequest(status);    
    //Restore OOM condition
    adaptationclitest.UnSetHeapFailure();
    adaptationclitest.Close();
    User::__DbgSetAllocFail(EFalse, RAllocator::ENone, 1);
    User::__DbgSetAllocFail(ETrue, RAllocator::ENone, 1);
    TEST(status.Int() == KErrNoMemory);
    __UHEAP_MARKEND;
    INFO_PRINTF1(_L("doTestForNormalEmergencyCallinOOM completed.")); 
    }


/**
 * Tests for performing ActivateRfForEmergencyCall + DeactivateRfForEmergencyCall 
 *  during OOM condition by priority clients.
 */
void CSusAdaptionServerTest::doTestForEmergencyCallOOM()
    {
    INFO_PRINTF1(_L("doTestForEmergencyCallOOM started."));
    __UHEAP_MARK;
    RTestSusAdaptationCli adaptationclitest;    
    TInt err = adaptationclitest.Connect();
    INFO_PRINTF2(_L("Connect() returned Error %d : Expected Error is KErrNone."),err);
    TEST(err == KErrNone);
    err = adaptationclitest.SetAsPriorityClient();
    INFO_PRINTF2(_L("SetAsPriorityClient() returned Error %d : Expected Error is KErrNone."),err);
    TEST(err == KErrNone);
    //Activate Rf + Deactivate Rf during non OOM situation.
    //Calls will be queued and processed one after the other. After 3 consecutive requests for 
    //ActivateRfForEmergencyCall(), first call will be submitted directly to process and other two calls 
    // will get queued. By end of this process will be having (count + 2) reserved memory in queue.i.e 4 reserved slots
    TRequestStatus status1,status2,status3,status4,status5,status6; 
    adaptationclitest.ActivateRfForEmergencyCall(status1); 
    adaptationclitest.ActivateRfForEmergencyCall(status2);
    adaptationclitest.ActivateRfForEmergencyCall(status3);
    //Wait for request to complete
    User::WaitForRequest(status1);
    User::WaitForRequest(status2);
    User::WaitForRequest(status3);
    TEST(status1.Int() == KErrNone);
    TEST(status2.Int() == KErrNone);
    TEST(status3.Int() == KErrNone);
    // Simulate OOM condition
    User::__DbgSetAllocFail(EFalse, RAllocator::EDeterministic, 1);
    User::__DbgSetAllocFail(ETrue, RAllocator::EDeterministic, 1);
    //Simulate OOM at Serverside
    adaptationclitest.SetHeapFailure(1);
 
    //After the above 2 calls to Activate the Rfs, will be left with 4 reserved memory in queue, so in OOM
    //condition client can queue 4 activate/deactivate requests.The 5th request will fail with KErrNoMemory
    adaptationclitest.ActivateRfForEmergencyCall(status1); 
    adaptationclitest.ActivateRfForEmergencyCall(status2);
    adaptationclitest.DeactivateRfForEmergencyCall(status3);
    adaptationclitest.DeactivateRfForEmergencyCall(status4);
    adaptationclitest.DeactivateRfForEmergencyCall(status5);
    adaptationclitest.DeactivateRfForEmergencyCall(status6);
    // Wait for completion of requests
    User::WaitForRequest(status1);
    User::WaitForRequest(status2);
    User::WaitForRequest(status3);
    User::WaitForRequest(status4);
    User::WaitForRequest(status5);
    User::WaitForRequest(status6);    
    
    //Restore OOM condition
    adaptationclitest.UnSetHeapFailure();
    User::__DbgSetAllocFail(EFalse, RAllocator::ENone, 1);
    User::__DbgSetAllocFail(ETrue, RAllocator::ENone, 1);
    TEST(status1.Int() == KErrNone);
    TEST(status2.Int() == KErrNone);
    TEST(status3.Int() == KErrNone);
    TEST(status4.Int() == KErrNone);
    TEST(status5.Int() == KErrNone);
    //6th request will fail with KErrNoMemory if, above 5 requests are still queued and there is no reserved slot for
    //6th one..otherwise, 6th request will be queued and processed
    TEST(status6.Int() == KErrNoMemory || status6.Int() == KErrNone);
    
    adaptationclitest.Close();
    __UHEAP_MARKEND;
    INFO_PRINTF1(_L("doTestForEmergencyCallOOM completed."));        
    }

/**
 * Tests to Set PriorityClient from two different clients.
 */
void CSusAdaptionServerTest::doTestForSettingPriorityClient()
    {
    INFO_PRINTF1(_L("doTestForSettingPriorityClient started."));
    __UHEAP_MARK;
    RTestSusAdaptationCli adaptationclitest1;
    TInt err = adaptationclitest1.Connect();
    INFO_PRINTF2(_L("adaptationclitest1.Connect() returned Error %d : Expected Error is KErrNone."),err);   
    TEST(err == KErrNone);
    
    RTestSusAdaptationCli adaptationclitest2;
    err = adaptationclitest2.Connect();
    INFO_PRINTF2(_L("adaptationclitest2.Connect() returned Error %d : Expected Error is KErrNone."),err);   
    TEST(err == KErrNone);
       
    err = adaptationclitest1.SetAsPriorityClient();
    INFO_PRINTF2(_L("adaptationclitest1.SetAsPriorityClient() returned Error %d : Expected Error is KErrNone."),err);
    TEST(err == KErrNone);
    
    err = adaptationclitest2.SetAsPriorityClient();
    INFO_PRINTF2(_L("adaptationclitest2.SetAsPriorityClient() returned Error %d : Expected Error is KErrAlreadyExists."),err);
    TEST(err == KErrAlreadyExists);
    
    // Restore OOM condition
    adaptationclitest1.Close();
    adaptationclitest2.Close();
    __UHEAP_MARKEND;
    INFO_PRINTF1(_L("doTestForSettingPriorityClient completed."));
    }

