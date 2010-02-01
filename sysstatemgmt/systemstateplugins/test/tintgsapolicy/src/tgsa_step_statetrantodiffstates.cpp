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

#include "tgsa_step_statetrantodiffstates.h"
#include "gsatestapps.h"

#include <ssm/ssmstates.hrh>
#include <ssm/ssmstate.h>

/**
Old Test CaseID 		AFSS-GSA-0030
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0032
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0033
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0034
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0035
 */


void CGsaStateTranToDiffStatesTest::DoStateRequestTransitionToDiffStatesL()
	{
	INFO_PRINTF1(_L("DoStateRequestTransitionToDiffStates test started...."));
	
	RFs fs;
	TInt err = fs.Connect();
	TEST(err == KErrNone);
	User::LeaveIfError(err);
	CleanupClosePushL(fs);
		
	if (iProcessName == KTestProcTranFromNormal)
		{
		RFileReadStream fileReadStream;
		err = fileReadStream.Open(fs, KGsaTestStateTranFromNormalResult, EFileRead);
		TEST(err == KErrNone);
		User::LeaveIfError(err);
		CleanupClosePushL(fileReadStream);

		//System state before starting the test.
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmNormal);

		//Scenario 1 - Request for state change from Normal to invalid state results in KErrNotSupported
		INFO_PRINTF1(_L("Request for state change from Normal to invalid state"));
		TInt stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNotSupported);
		INFO_PRINTF2(_L("Actual : %d Expected : -5"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmNormal);
		
		//Scenario 2 - Request for state change from Normal to fail state with invalid substate results in KErrNotSupported
		INFO_PRINTF1(_L("Request for state change from Normal to fail state with invalid substate"));
		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNotSupported);
		INFO_PRINTF2(_L("Actual : %d Expected : -5"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmNormal);

		//Scenario 3 - Request for two state changes(1. fail state 2. shutdown state) one after the other without waiting 
		// for the first one to complete. Cancels both transition request. 		
		INFO_PRINTF1(_L("Request for two state changes(1. fail state 2. shutdown state)"));
		INFO_PRINTF1(_L("one after the other without waiting for the first one to complete."));
		INFO_PRINTF1(_L("Cancel the fail state transition request."));
		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrCancel);
		INFO_PRINTF2(_L("First request to the fail state will be completed with %d Expected -3"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmNormal);

		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrCancel);
		INFO_PRINTF2(_L("Second request to transit to the shutdown state will be completed with %d Expected -3"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmNormal);

		//Scenario 4 - Request for two state changes one after the other without waiting for the first one to complete
		// Once first and second request are completed the system will be in fail state, now issue another state transition
		// request to fail state again.
		// The first request to the fail state will be completed with KErrCancel 
		// the second request to transit to the fail state will be completed with KErrNone.
		// the third request to transit to the fail state will be completed with KErrNotSupported.
		INFO_PRINTF1(_L("Request for two state changes one after the other without waiting"));
		INFO_PRINTF1(_L("for the first one to complete. Once first and second request are completed the system will be in"));
		INFO_PRINTF1(_L("fail state, now issue another state transition request to fail state again."));
		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrCancel);
		INFO_PRINTF2(_L("First request to the fail state will be completed with %d Expected -3"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmNormal);

		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNone);
		INFO_PRINTF2(_L("Second request to the fail state will be completed with %d Expected 0"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmNormal);

		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNotSupported);
		INFO_PRINTF2(_L("Third request to the fail state will be completed with %d Expected -5"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmNormal);

		//Finally the system should be in fail state i.e., just before the test application is closed.
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmFail);
		INFO_PRINTF1(_L("Test Process Exit Reason"));
		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNone && stateTranResult != KTestAppFailure);
		INFO_PRINTF2(_L("Test process exits with %d Expected 0"), stateTranResult);

		CleanupStack::PopAndDestroy();
		// Delete the result file
		//err = fs.Delete(KGsaTestStateTranFromNormalResult);
		}
	else if (iProcessName == KTestProcTranToShutdown)
		{
		RFileReadStream fileReadStream;
		err = fileReadStream.Open(fs, KGsaTestStateTranToShutdownResult, EFileRead);
		TEST(err == KErrNone);
		User::LeaveIfError(err);
		CleanupClosePushL(fileReadStream);

		//System state before starting the test.
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmNormal);

		//Scenario 1 - Request for state transition to shutdown state - invalid substate
		// request to transit to the shutdown state will be completed with KErrNotSupported
		INFO_PRINTF1(_L("Request for state transition to shutdown state - invalid substate"));
		TInt stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNotSupported);
		INFO_PRINTF2(_L("Actual : %d Expected : -5"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmNormal);
		
		//Scenario 2 - Request for state transition to shutdown state - critical substate(Restart)
		// Request for state transition to shutdown state - critical substate(Standby)
		// first request to transit to the shutdown state critical substate(Restart)will be completed with KErrCancel
		// second request to transit to the shutdown state critical substate(Standby)will be completed with KErrNone
		INFO_PRINTF1(_L("Request for state transition to shutdown state - critical substate(Standby)"));
		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNone);
		INFO_PRINTF2(_L("Actual : %d Expected : 0"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmNormal);

		INFO_PRINTF1(_L("Request for state transition to shutdown state - critical substate(Restart)"));
		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrCancel);
		INFO_PRINTF2(_L("Actual : %d Expected : -3"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmNormal);

		//Finally the system should be in fail state i.e., just before the test application is closed.
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmShutdown);
		
		INFO_PRINTF1(_L("Test Process Exit Reason"));
		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNone && stateTranResult != KTestAppFailure);
		INFO_PRINTF2(_L("Test process exits with %d Expected 0"), stateTranResult);

		CleanupStack::PopAndDestroy();
		// Delete the result file
		//err = fs.Delete(KGsaTestStateTranToShutdownResult);
		}
	else if (iProcessName == KTestProcTranFromShutdown)
		{
		RFileReadStream fileReadStream;
		err = fileReadStream.Open(fs, KGsaTestStateTranFromShutdownResult, EFileRead);
		TEST(err == KErrNone);
		User::LeaveIfError(err);
		CleanupClosePushL(fileReadStream);

		//System state before starting the test.
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmNormal);

		//Scenario 1 - Request for state change from Normal to shutdown state to critical substate
		// Cancel the request immediately
		INFO_PRINTF1(_L("Request for state change from Normal to shutdown critical substate and Cancel the request"));
		TInt stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrCancel);
		INFO_PRINTF2(_L("Actual : %d Expected : -3"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmNormal);

		//Scenario 2 - Request for state change from Normal to shutdown state to any substate
		// Cancel the request immediately, so that we can proceed with the next test scenario.

		INFO_PRINTF1(_L("Request for state change from Normal to shutdown any substate and Cancel the request"));
		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrCancel);
		INFO_PRINTF2(_L("Actual : %d Expected : -3"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmNormal);

		//Scenario 3 - Request for three state changes(1. shutdown state 2. invalid state 3. fail state) one after the other 
		// i.e., second request is issued once first one is completed and the third one is issued after the second request is completed.
		// Cancel the third request.
		// first request to the shutdown state will be completed with KErrNone 
		// second request to transit to the fail state will be completed with KErrNotSupported
		// third request to transit to the fail state will be completed with KErrCancel
		INFO_PRINTF1(_L("Request for 3 state changes(1. shutdown state 2. invalid state 3. fail state)"));
		INFO_PRINTF1(_L("one after the other. i.e., second request is issued once first one is completed"));
		INFO_PRINTF1(_L("and the third one is issued after the second request is completed."));
		INFO_PRINTF1(_L("Cancel the third request."));
		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNone);
		INFO_PRINTF2(_L("First request to the shutdown state will be completed with %d Expected 0"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmNormal);

		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNotSupported);
		INFO_PRINTF2(_L("Second request to transit to the fail state will be completed with %d Expected -5"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmNormal);

		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrCancel);
		INFO_PRINTF2(_L("Third request to transit to the fail state will be completed with %d Expected -3"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmShutdown);

		//Scenario 4 - The system is now in shutdown state, request for state transition to normal state.
		// request to transit to the normal state will be completed with KErrNotSupported
		INFO_PRINTF1(_L("The system is now in shutdown state, request for state transition to normal state"));
		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNotSupported);
		INFO_PRINTF2(_L("Request to transit to the normal state will be completed with %d Expected -5"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmShutdown);

		//Scenario 5 - The system is now in shutdown state, request for state transition to fail state.
		// request to transit to the fail state will be completed with KErrNone
		INFO_PRINTF1(_L("The system is now in shutdown state, request for state transition to fail state"));
		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNone);
		INFO_PRINTF2(_L("Request to transit to the normal state will be completed with %d Expected 0"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmShutdown);

		//Finally the system should be in fail state i.e., just before the test application is closed.
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmFail);
		
		INFO_PRINTF1(_L("Test Process Exit Reason"));
		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNone && stateTranResult != KTestAppFailure);
		INFO_PRINTF2(_L("Test process exits with %d Expected 0"), stateTranResult);

		CleanupStack::PopAndDestroy();
		// Delete the result file
		//err = fs.Delete(KGsaTestStateTranFromShutdownResult);
		}
	else if(iProcessName == KTestProcTranFromStartup)
		{
		RFileReadStream fileReadStream;
		err = fileReadStream.Open(fs, KGsaTestStateTranFromStartupResult, EFileRead);
		TEST(err == KErrNone);
		User::LeaveIfError(err);
		CleanupClosePushL(fileReadStream);

		//System state before starting the test.
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmStartup);

		//Scenario 1 - Request for state change from Startup to invalid state
		INFO_PRINTF1(_L("Request for state change from Startup to invalid state"));
		TInt stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNotSupported);
		INFO_PRINTF2(_L("Actual : %d Expected : -5"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmStartup);
		
		//Scenario 2 - Request for state change from Startup to fail state with invalid substate
		INFO_PRINTF1(_L("Request for state change from Startup to fail state with invalid substate"));
		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNotSupported);
		INFO_PRINTF2(_L("Actual : %d Expected : -5"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmStartup);

		//Scenario 3 - Request for state change from Startup to shutdown invalid substate
		INFO_PRINTF1(_L("Request for state change from Startup to shutdown invalid substate"));
		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNotSupported);
		INFO_PRINTF2(_L("Request for state change from Startup to shutdown invalid state completed with %d Expected -5"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmStartup);

		//Scenario 4 - Request for two state changes to fail state one after the other
		//The first request to the fail state will be completed with KErrCancel and the second request to transit to the fail state will be completed with KErrNone.
		INFO_PRINTF1(_L("Request for two state changes to fail state one after the other"));
		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrCancel);
		INFO_PRINTF2(_L("First request to the fail state will be completed with %d Expected -3"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmStartup);

		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNone);
		INFO_PRINTF2(_L("Second request to the fail state will be completed with %d Expected 0"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmStartup);

		//Finally the system should be in fail state i.e., just before the test application is closed.
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmFail);
		INFO_PRINTF1(_L("Test Process Exit Reason"));
		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNone && stateTranResult != KTestAppFailure);
		INFO_PRINTF2(_L("Test process exits with %d Expected 0"), stateTranResult);
		
		CleanupStack::PopAndDestroy();
		// Delete the result file
		//err = fs.Delete(KGsaTestStateTranFromStartupResult);
		}
	else if (iProcessName == KTGsaStateTranFromStartuptoShutdownStep)
		{
		RFileReadStream fileReadStream;
		User::LeaveIfError(fileReadStream.Open(fs, KGsaTestStateTranFromStartupResult, EFileRead));
		CleanupClosePushL(fileReadStream);
		
		//Scenario 1 - Request for state change from Startup to shutdown critical substate
		INFO_PRINTF1(_L("Request for state change from Startup to shutdown critical substate"));
		TInt stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNone);
		INFO_PRINTF2(_L("Request for state change from Startup to shutdown critical substate completed with %d Expected 0"), stateTranResult);
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmStartup);
		
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmStartup);
		
		//Finally the system should be in Shutdown state i.e., just before the test application is closed.
		CurrentSystemStateForStateTranTestL(fileReadStream, ESsmShutdown);
		INFO_PRINTF1(_L("Test Process Exit Reason"));
		stateTranResult = fileReadStream.ReadInt32L();
		TEST(stateTranResult == KErrNone && stateTranResult != KTestAppFailure);
		INFO_PRINTF2(_L("Test process exits with %d Expected 0"), stateTranResult);
		
		CleanupStack::PopAndDestroy();
		}
	else
		{
		TEST(err == KErrArgument);
		return;
		}

	CleanupStack::PopAndDestroy();
	INFO_PRINTF1(_L("case completed\n"));
	}

// Tests the current system state
void CGsaStateTranToDiffStatesTest::CurrentSystemStateForStateTranTestL(RFileReadStream& aFileReadStream, TInt aSystemState)
	{
	TInt currentState = aFileReadStream.ReadUint16L();
	TSsmState currentSystemState(currentState, KSsmAnySubState);
	TSsmState expectedSystemState(aSystemState, KSsmAnySubState);

	TEST(currentSystemState == expectedSystemState);
	TPtrC currentSystemStateName(currentSystemState.Name());
	TPtrC expectedSystemStateName(expectedSystemState.Name());
	INFO_PRINTF3(_L("Current system state %S Expected %S"), &currentSystemStateName, &expectedSystemStateName);
	}

//---------------- CGsaStateTranToDiffStatesTest step ---------------------------------------


CGsaStateTranToDiffStatesTest::~CGsaStateTranToDiffStatesTest()
	{
	}

CGsaStateTranToDiffStatesTest::CGsaStateTranToDiffStatesTest(const TDesC& aProcessName) : iProcessName(aProcessName)
	{
	if (iProcessName == KTestProcTranToShutdown)
		SetTestStepName(KTGsaStateTranToShutdownStep);
	else if (iProcessName == KTestProcTranFromShutdown)
		SetTestStepName(KTGsaStateTranFromShutdownStep);
	else if (iProcessName == KTestProcTranFromNormal)
		SetTestStepName(KTGsaStateTranFromNormalStep);
	else if (iProcessName == KTestProcTranFromStartup)
		SetTestStepName(KTGsaStateTranFromStartupStep);
	else if (iProcessName == KTGsaStateTranFromStartuptoShutdownStep)
		SetTestStepName(KTGsaStateTranFromStartuptoShutdownStep);
	else
		
		{
		// If the test script is none of the above then we would fail the test
		TEST(KErrArgument);
		}
	}

TVerdict CGsaStateTranToDiffStatesTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CGsaStateTranToDiffStatesTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

TVerdict CGsaStateTranToDiffStatesTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CGsaStateTranToDiffStatesTest started...."));
	
	__UHEAP_MARK;
	// Run the tests
	TRAPD(err, DoStateRequestTransitionToDiffStatesL());
	INFO_PRINTF2(_L("DoStateRequestTransitionToDiffStatesL completed with %d...."),err);
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CGsaStateTranToDiffStatesTest completed!!"));
	return TestStepResult();
	}





