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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/

#include "tgsa_step_startup.h"
#include "gsastatepolicystartup.h"
#include <ssm/ssmstatetransition.h>
#include "ssmdebug.h"
#include <e32uid.h>
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmcommand.h>


#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
TSsmCommandType ArrCriticalStartUp[] = { ESsmCmdCustomCommand,ESsmCmdPublishSystemState,ESsmCmdStartProcess, ESsmCmdStartProcess, ESsmCmdStartProcess, ESsmCmdStartProcess,ESsmCmdStartProcess, ESsmCmdSetPAndSKey, ESsmCmdStartProcess, ESsmCmdStartProcess, ESsmCmdStartProcess, ESsmCmdLoadSup };
#else
TSsmCommandType ArrCriticalStartUp[] = { ESsmCmdCustomCommand,ESsmCmdPublishSystemState,ESsmCmdStartProcess, ESsmCmdStartProcess, ESsmCmdStartProcess, ESsmCmdStartProcess,ESsmCmdStartProcess, ESsmCmdSetPAndSKey, ESsmCmdStartProcess, ESsmCmdStartProcess, ESsmCmdStartProcess };
#endif
TSsmCommandType ArrDynamicStartUp[] = {ESsmCmdPublishSystemState, ESsmCmdStartProcess, ESsmCmdStartProcess, ESsmCmdStartApp, ESsmCmdMultipleWait, ESsmCmdSetPAndSKey};
TSsmCommandType ArrNetworkingStartUp[] = { ESsmCmdPublishSystemState,ESsmCmdStartProcess, ESsmCmdStartProcess, ESsmCmdStartProcess };
#ifdef TEST_SSM_GRACEFUL_OFFLINE
TSsmCommandType ArrNonCriticalStartUp[] = { ESsmCmdSetPAndSKey, ESsmCmdSetPAndSKey,ESsmCmdSetPAndSKey,ESsmCmdPublishSystemState ,ESsmCmdStartProcess,ESsmCmdStartProcess,ESsmCmdStartProcess,ESsmCmdStartProcess,ESsmCmdStartProcess,ESsmCmdStartProcess,ESsmCmdAMAStarter};
#else
TSsmCommandType ArrNonCriticalStartUp[] = { ESsmCmdPublishSystemState ,ESsmCmdStartProcess,ESsmCmdStartProcess,ESsmCmdStartProcess,ESsmCmdStartProcess,ESsmCmdStartProcess,ESsmCmdStartProcess,ESsmCmdAMAStarter};
#endif

CGsaStartupTest::~CGsaStartupTest()
	{
	}

CGsaStartupTest::CGsaStartupTest() : CGsaTestBase(KTGsaStartupStep)
	{
	}

TVerdict CGsaStartupTest::doTestStepL()
	{
	INFO_PRINTF1(_L("> CGsaStartupTest::doTestStepL"));
	TInt err = KErrNone;

	__UHEAP_MARK;
	TRAP(err, doTestNewL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestInitializeL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestPrepareCommandListL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestCommandListL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestGetNextStateL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	return TestStepResult();
	}

/**
Old Test CaseID 		AFSS-GSA-0010
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0010
 */

void CGsaStartupTest::doTestNewL()
	{
	INFO_PRINTF1(_L("> CGsaStartupTest::doTestNewL"));

	TInt expectedErr = KErrNone;
	MSsmStatePolicy* policy = NULL;
	//test should pass
	TRAPD(err, policy = CGsaStatePolicyStartup::NewL());
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	TEST(policy != NULL);
	policy->Release();
	}

/**
Old Test CaseID 		AFSS-GSA-0011
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0010
 */

void CGsaStartupTest::doTestInitializeL()
	{
	INFO_PRINTF1(_L("> CGsaStartupTest::doTestInitializeL"));
	CGsaStatePolicyStartup* policy = CreateAndInitializeStartUpPolicyLC();
	CleanupStack::PopAndDestroy(policy);
	}

/**
Old Test CaseID 		AFSS-GSA-0012
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0010
 */
 void CGsaStartupTest::doTestPrepareCommandListL()
	{
	INFO_PRINTF1(_L("> CGsaStartupTest::doTestPrepareCommandListL"));
	TestPrepareCommandListL(ESsmStartup ,ESsmStartupSubStateCriticalStatic, KErrNone);
	TestPrepareCommandListL(ESsmStartup ,KSsmAnySubState, KErrNone);
	TestPrepareCommandListL(ESsmStartup ,ESsmStartupSubStateCriticalDynamic, KErrNone);
	TestPrepareCommandListL(ESsmStartup ,ESsmStartupSubStateNetworkingCritical, KErrNone);
	TestPrepareCommandListL(ESsmStartup ,ESsmStartupSubStateNonCritical, KErrNone);
	TestPrepareCommandListL(ESsmStartup ,100, KErrNotFound);
	}

/**
Old Test CaseID 		AFSS-GSA-0013
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0010
 */

void CGsaStartupTest::doTestCommandListL()
	{
	INFO_PRINTF1(_L("> CGsaStartupTest::doTestCommandListL"));

	TestCommandListL(ESsmStartup, ESsmStartupSubStateCriticalStatic, sizeof(ArrCriticalStartUp) / sizeof(ArrCriticalStartUp[0]));
	TestCommandListL(ESsmStartup, KSsmAnySubState, sizeof(ArrCriticalStartUp) / sizeof(ArrCriticalStartUp[0]));
	TestCommandListL(ESsmStartup, ESsmStartupSubStateCriticalDynamic, sizeof(ArrDynamicStartUp) / sizeof(ArrDynamicStartUp[0]));
	TestCommandListL(ESsmStartup, ESsmStartupSubStateNetworkingCritical, sizeof(ArrNetworkingStartUp) / sizeof(ArrNetworkingStartUp[0]));
	TestCommandListL(ESsmStartup, ESsmStartupSubStateNonCritical, sizeof(ArrNonCriticalStartUp) / sizeof(ArrNonCriticalStartUp[0]));
	}

/**
Old Test CaseID 		AFSS-GSA-0014
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0010
 */


void CGsaStartupTest::doTestGetNextStateL()
	{
	CGsaStatePolicyStartup* policy = CreateAndInitializeStartUpPolicyLC();

	TSsmState state(ESsmStartup, KSsmAnySubState);
	TSsmState result(ESsmStartup, ESsmStartupSubStateCriticalDynamic);
	// KSsmAnySubState (KSsmAnySubState) will return ESsmStartUpSubStateCriticalDynamic only for the first time, 
	// next call will always return the next sub state for transition.
	TestGetNextState(policy, state, KErrNone, result);

	TSsmState firstState(ESsmStartup, ESsmStartupSubStateCriticalStatic);
	TSsmState firstResult(ESsmStartup, ESsmStartupSubStateCriticalDynamic);
	TestGetNextState(policy, firstState, KErrNone, firstResult);

	TSsmState secondState(ESsmStartup, ESsmStartupSubStateCriticalDynamic);
	TSsmState secondResult(ESsmStartup, ESsmStartupSubStateNetworkingCritical);
	TestGetNextState(policy, secondState, KErrNone, secondResult);

	TSsmState thirdState(ESsmStartup, ESsmStartupSubStateNetworkingCritical);
	TSsmState thirdResult(ESsmStartup, ESsmStartupSubStateNonCritical);
	TestGetNextState(policy, thirdState, KErrNone, thirdResult);
#ifdef __WINS__
	TSsmState fourthState(ESsmStartup, ESsmStartupSubStateNonCritical);
	TSsmState fourthResult(ESsmNormal, KSsmAnySubState);
	TestGetNextState(policy, fourthState, KErrNone, fourthResult);
#else
	TSsmState fourthState(ESsmStartup, ESsmStartupSubStateNonCritical);
	TSsmState fourthResult(ESsmStartup, 0x48);//Added 0x48 sub-state for new test for ARMV5 
	TestGetNextState(policy, fourthState, KErrNone, fourthResult);
	//This new state test Added for the one more sub-state available in ARMV5 compare to WINSCW
	TSsmState fourthToNextState(ESsmStartup, 0x48);
	TSsmState fourthToNextStateResult(ESsmNormal, KSsmAnySubState);
	TestGetNextState(policy, fourthToNextState, KErrNone, fourthToNextStateResult);
#endif

	TSsmState fifthState(ESsmStartup, KSsmAnySubState);
	TSsmState fifthResult(ESsmStartup, ESsmStartupSubStateCriticalDynamic);
	// KSsmAnySubState (0xffff) will return ESsmStartUpSubStateCriticalDynamic only for the first time, 
	// next call will always return the next sub state for transition.
	TestGetNextState(policy, fifthState, KErrNone, fifthResult);
	
	//Commented this test because CGsaStatePolicyStartup::GETNExtstate panicks if substate is unknown
	//TSsmState unknownState(ESsmStartup, 100);
	//TSsmState knownResult(ESsmStartup, 0);
	//TestGetNextState(policy, unknownState, KErrNone, knownResult);

	CleanupStack::PopAndDestroy(policy);
	}

/**
 Helper function to create and initialize start-up state policy.
*/
CGsaStatePolicyStartup* CGsaStartupTest::CreateAndInitializeStartUpPolicyLC()
	{
	TRequestStatus trs;
	TRequestStatus expectedTrs = KErrNone;
	CGsaStatePolicyStartup* self = static_cast<CGsaStatePolicyStartup*>(CGsaStatePolicyStartup::NewL());
	CleanupStack::PushL(self);
	self->Initialize(trs);
	TEST(trs == KRequestPending);

	// Test for cancelling the initialize request
	self->InitializeCancel();
	expectedTrs = KErrCancel;
	TEST(trs.Int() == expectedTrs.Int());
	INFO_PRINTF3(_L("Startup Policy Initialization cancelled with status : %d. expected status : %d"), trs.Int(), expectedTrs.Int());

	// Call Initialize again to complete the intialization of the command list
	trs = KErrNone;
	self->Initialize(trs);
	TEST(trs == KRequestPending);

	StartScheduler();
	User::WaitForRequest(trs);
	expectedTrs = KErrNone;
	INFO_PRINTF3(_L("StartUp Policy Initialized with status : %d. expected status : %d"), trs.Int(), expectedTrs.Int());
	TEST( trs.Int() == expectedTrs.Int());

	return self;
	}

/**
 Helper function to test preparation of command lists for start-up state policy.
*/
void CGsaStartupTest::TestPrepareCommandListL(TUint16 aMainState, TUint16 aSubState, TInt aExpectedResult)
	{
	// Initialize //
	TRequestStatus trs;
	CGsaStatePolicyStartup* policy = static_cast<CGsaStatePolicyStartup*>(CGsaStatePolicyStartup::NewL());
	CleanupStack::PushL(policy);
	policy->Initialize(trs);
	StartScheduler();
	User::WaitForRequest(trs);
	TEST( trs.Int() == KErrNone);

	// PrepareCommandList //
	trs = -1;
	TSsmState state = TSsmState(aMainState, aSubState);
	policy->PrepareCommandList(state, 0, trs);
	TEST(trs == KRequestPending);

	// Test for cancelling the prepare command list request
	policy->PrepareCommandListCancel();
	TEST(trs.Int() == KErrCancel);
	INFO_PRINTF2(_L("Startup Policy PrepareCommandList cancelled with status : %d. expected status : -3"), trs.Int());

	// Call PrepareCommandList again to complete the preparation of the command list
	trs = KErrNone;
	policy->PrepareCommandList(state, 0, trs);
	TEST(trs == KRequestPending);

	StartScheduler();
	User::WaitForRequest(trs);

	INFO_PRINTF4(_L("PrepareCommandList() completed for Sub State: 0x%x with Status: %d. Expected Status: %d"), aSubState, trs.Int(), aExpectedResult);
	TEST( trs.Int() == aExpectedResult);
	CleanupStack::PopAndDestroy(policy);
	}

/**
 Helper function to test the command type of each command present in command lists for start-up state policy.
*/
void CGsaStartupTest::TestCommandListL(TUint16 aMainState, TUint16 aSubState, TInt aNumSubStates)
	{
	// Initialize //
	TRequestStatus trs;
	CGsaStatePolicyStartup* policy = static_cast<CGsaStatePolicyStartup*>(CGsaStatePolicyStartup::NewL());
	CleanupStack::PushL(policy);
	policy->Initialize(trs);
	StartScheduler();
	User::WaitForRequest(trs);
	TEST( trs.Int() == KErrNone);

	// PrepareCommandList //
	trs = -1;
	TSsmState state = TSsmState(aMainState, aSubState);
	policy->PrepareCommandList(state, 0, trs);
	StartScheduler();
	User::WaitForRequest(trs);
	TEST( trs.Int() == KErrNone);

	// CommandList //
	CSsmCommandList* cmdList = policy->CommandList();
	TEST(cmdList != NULL);
	const TInt count = cmdList->Count();
	INFO_PRINTF2(_L("CommandList() has %d commands"), count);
	TEST( count == aNumSubStates);

	for (TInt i = 0; i < count ; i++)
		{
		const MSsmCommand* const command = (*cmdList)[i];
		const TSsmCommandType cmdType = static_cast<TSsmCommandType>(command->Type());
		INFO_PRINTF3(_L("command number is : %d command type is : %d"), i, cmdType);
		
		switch (aSubState)
			{
			case ESsmStartupSubStateCriticalStatic:
				{
				TEST (ArrCriticalStartUp[i] == cmdType);
				break;
				}
			case KSsmAnySubState:
				{
				TEST (ArrCriticalStartUp[i] == cmdType);
				break;
				}
			case ESsmStartupSubStateCriticalDynamic:
				{
				TEST (ArrDynamicStartUp[i] == cmdType);
				break;
				}
			case ESsmStartupSubStateNetworkingCritical:
				{
				TEST (ArrNetworkingStartUp[i] == cmdType);
				break;
				}
			case ESsmStartupSubStateNonCritical:
				{
				TEST (ArrNonCriticalStartUp[i] == cmdType);
				break;
				}
			default:
				{
				break;
				}
			}
		}
	delete cmdList;
	CleanupStack::PopAndDestroy(policy);
	}

/**
 Helper function to test the GetNextState for each substate within start-up state.
*/
void CGsaStartupTest::TestGetNextState(CGsaStatePolicyStartup* aPolicy, TSsmState aCurrentTransition, TInt aError, TSsmState aResult)
	{
	TRequestStatus trs;
	TSsmState state(ESsmStartup, KSsmAnySubState);
	aPolicy->PrepareCommandList(aCurrentTransition, KErrNone, trs);
	StartScheduler();
	User::WaitForRequest(trs);

	TSsmState nextState;
	TBool res = aPolicy->GetNextState(aCurrentTransition, KErrNone, aError, ECmdCriticalSeverity, nextState);
	
	TEST(res);	
	
	INFO_PRINTF7(_L("Current State : 0x%x.0x%x Next State 0x%x.0x%x Expected Next State 0x%x.0x%x"), aCurrentTransition.MainState(), aCurrentTransition.SubState(), nextState.MainState(), nextState.SubState(), aResult.MainState(), aResult.SubState());

	TEST(nextState == aResult);
	}

