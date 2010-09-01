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

#include "gsatestapps.h"

#include <ssm/ssmstates.hrh>
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmstate.h>
#include <ssm/ssmstatetransition.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmstateawaresession.h>
#include <ssm/ssmdomaindefs.h>

#include "tgsastatetran_base.h"

class CStateTranFromShutdown : public CTGsaStateTransition
	{
public:
	static CStateTranFromShutdown* NewL();
	~CStateTranFromShutdown();
	void StartTestL();
	void CallStopSchedulerL();

private:
	void ConstructL();
	CStateTranFromShutdown();

public:
	void DoTestStateTranFromShutdownL();
	};

static TInt StopScheduler(TAny* aStateTranFromShutdown)
 	{
 	CStateTranFromShutdown* stateTran = reinterpret_cast<CStateTranFromShutdown*>(aStateTranFromShutdown);
 	TRAPD(err,stateTran->CallStopSchedulerL());
 	return err;
 	}

static TInt StartTest(TAny* aStateTranFromShutdown)
 	{
 	TInt testCompletionReason = KErrNone;
 	CStateTranFromShutdown* stateTran = reinterpret_cast<CStateTranFromShutdown*>(aStateTranFromShutdown);
 	TBool stateTest = stateTran->DoStartStateTranTest();
 	if (stateTest)
 		{
 		RDebug::Print(_L("\nStateTran Test: Start the tgsastatetranfromshutdown test\n"));
		TRAP(testCompletionReason, stateTran->DoTestStateTranFromShutdownL());
 		}

 	return testCompletionReason;
 	}

CStateTranFromShutdown* CStateTranFromShutdown::NewL()
	{
	CStateTranFromShutdown* self = new(ELeave) CStateTranFromShutdown();
	self->ConstructL();
	return self;
	}

void CStateTranFromShutdown::ConstructL()
	{
	CTGsaStateTransition::ConstructL(KGsaTestStateTranResultFilePath);
	}

CStateTranFromShutdown::~CStateTranFromShutdown()
	{
	}

CStateTranFromShutdown::CStateTranFromShutdown()
	{
	}

// Tests the Shutdown state transitions under different scenarios
void CStateTranFromShutdown::DoTestStateTranFromShutdownL()
	{
	// This async callback is used to stop the scheduler once the system goes into fail state.
	iAsyncCallbackForStopScheduler =  new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack stopSchedulerCallback(StopScheduler, this);
	iAsyncCallbackForStopScheduler->Set(stopSchedulerCallback);

	// call the callback function which keeps looping until the system state gets into fail state.
	iAsyncCallbackForStopScheduler->CallBack();

	RSsmStateManager stateMan;
	TInt err = stateMan.Connect();
	if (err == KErrNone)
		RDebug::Print(_L("\nStateTran Test: RSsmStateManager sess Connect %d\n"), err);
	else
		{
		RDebug::Print(_L("\nStateTran Test: Unable to connect to RSsmStateManager sess\n"));	
		User::Leave(KTestAppFailure);
		}

	// Creates and open the result file for streaming
	err = iFileWriteStream.Replace(iFs, KGsaTestStateTranFromShutdownResult, EFileWrite);
	RDebug::Print(_L("\nStateTran Test: iFileWriteStream open with %d\n"), err);


	// Gets the Current system state before starting the test
	CurrentSystemStateL();

	//Scenario 1 - Request for state change from Normal to shutdown state to critical substate
	// Cancel the request immediately
	RDebug::Print(_L("\nScenario 1 \n"));
	TRequestStatus status_shutdowncritsubstate;
	TSsmState shutdownState_Critical(ESsmShutdown, ESsmShutdownSubStateCritical);
	TSsmStateTransition stateTrans_Shutdowncritical(shutdownState_Critical, 1);

	stateMan.RequestStateTransition(stateTrans_Shutdowncritical, status_shutdowncritsubstate);
	RDebug::Print(_L("\n RequestStateTransition from Normal to shutdown state critical substate %d\n "), status_shutdowncritsubstate.Int());

	stateMan.RequestStateTransitionCancel();
	
	iFileWriteStream.WriteInt32L(status_shutdowncritsubstate.Int());
	if (status_shutdowncritsubstate.Int() == KErrCancel)
		{
		CurrentSystemStateL();
		RDebug::Print(_L("\n RequestStateTransition from Normal to shutdown state critical substate completed %d Expected -3\n "), status_shutdowncritsubstate.Int());
		}
	else
		User::Leave(KTestAppFailure);

	//Scenario 2 - Request for state change from Normal to shutdown state to any substate
	// Cancel the request immediately, so that we can proceed with the next test scenario.
	RDebug::Print(_L("\nScenario 2 \n"));
	TRequestStatus status_shutdownanysubstate;
	TSsmState shutdownState_Any(ESsmShutdown, KSsmAnySubState);
	TSsmStateTransition stateTrans_shutdownAnySubState(shutdownState_Any, 1);

	stateMan.RequestStateTransition(stateTrans_shutdownAnySubState, status_shutdownanysubstate);
	RDebug::Print(_L("\n RequestStateTransition from Normal to shutdown state any substate %d\n "), status_shutdownanysubstate.Int());

	stateMan.RequestStateTransitionCancel();
	
	iFileWriteStream.WriteInt32L(status_shutdownanysubstate.Int());
	if (status_shutdownanysubstate.Int() == KErrCancel)
		{
		CurrentSystemStateL();
		RDebug::Print(_L("\n RequestStateTransition from Normal to shutdown state any substate completed %d Expected -3\n "), status_shutdownanysubstate.Int());
		}
	else
		User::Leave(KTestAppFailure);

	//Scenario 3 - Request for three state changes(1. shutdown state 2. invalid state 3. fail state) one after the other 
	// i.e., second request is issued once first one is completed and the third one is issued after the second request is completed.
	// Cancel the third request.
	// first request to the shutdown state will be completed with KErrNone 
	// second request to transit to the fail state will be completed with KErrNotSupported
	// third request to transit to the fail state will be completed with KErrCancel
	RDebug::Print(_L("\nScenario 3 \n"));

	TRequestStatus status;
	stateMan.RequestStateTransition(stateTrans_Shutdowncritical, status);
	RDebug::Print(_L("\n Start First RequestStateTransition from Normal to shutdown state %d\n "), status.Int());
	
	User::WaitForRequest(status);
	iFileWriteStream.WriteInt32L(status.Int());
	if (status.Int() == KErrNone)
		{
		CurrentSystemStateL();
		RDebug::Print(_L("\n First RequestStateTransition from Normal to shutdown state completes with %d Expected 0\n "), status.Int());
		}
	else
		User::Leave(KTestAppFailure);

	TRequestStatus status1;
	TSsmState state(10, KSsmAnySubState);
	TSsmStateTransition stateTrans_InvalidState(state, 1);

	stateMan.RequestStateTransition(stateTrans_InvalidState, status1);
	RDebug::Print(_L("\n Start Second RequestStateTransition from shutdown to invalid state %d\n "), status1.Int());

	User::WaitForRequest(status1);

	iFileWriteStream.WriteInt32L(status1.Int());
	if (status1.Int() == KErrNotSupported)
		{
		CurrentSystemStateL();
		RDebug::Print(_L("\n Second RequestStateTransition from shutdown to invalid state completes %d Expected -5\n "), status1.Int());
		}
	else
		User::Leave(KTestAppFailure);

	TRequestStatus status2;
	state.Set(ESsmFail, KSsmAnySubState);
	TSsmStateTransition stateTrans3(state, 1);

	stateMan.RequestStateTransition(stateTrans3, status2);
	RDebug::Print(_L("\n Start Third RequestStateTransition from Shutdown to fail state %d\n "), status2.Int());

	stateMan.RequestStateTransitionCancel();
	User::WaitForRequest(status2);

	iFileWriteStream.WriteInt32L(status2.Int());
	if (status2.Int() == KErrCancel)
		{
		CurrentSystemStateL();
		RDebug::Print(_L("\n Third RequestStateTransitionCancel from Shutdown to fail state completes %d Expected -3\n "), status2.Int());
		}
	else
		User::Leave(KTestAppFailure);
	
	//Scenario 4 - The system is now in shutdown state, request for state transition to normal state.
	// request to transit to the normal state will be completed with KErrNotSupported
	RDebug::Print(_L("\nScenario 4 \n"));
	
	TSsmState state_Normal(ESsmNormal, KSsmAnySubState);
	TSsmStateTransition stateTrans_NormalState(state_Normal, 1);

	stateMan.RequestStateTransition(stateTrans_NormalState, status1);
	RDebug::Print(_L("\n RequestStateTransition from Shutdown to normal state %d\n "), status1.Int());
	
	User::WaitForRequest(status1);

	iFileWriteStream.WriteInt32L(status1.Int());
	if (status1.Int() == KErrNotSupported)
		{
		CurrentSystemStateL();
		RDebug::Print(_L("\n RequestStateTransition from Shutdown to normal state completes %d Expected -5\n "), status1.Int());
		}
	else
		User::Leave(KTestAppFailure);

	//Scenario 5 - The system is now in shutdown state, request for state transition to fail state.
	// request to transit to the fail state will be completed with KErrNone
	RDebug::Print(_L("\nScenario 5 \n"));
	
	stateMan.RequestStateTransition(stateTrans3, status1);
	RDebug::Print(_L("\n RequestStateTransition from Shutdown to fail state %d\n "), status1.Int());
	
	User::WaitForRequest(status1);

	iFileWriteStream.WriteInt32L(status1.Int());
	if (status1.Int() == KErrNone)
		{
		CurrentSystemStateL();
		RDebug::Print(_L("\n RequestStateTransition from Shutdown to fail state completes %d Expected 0\n "), status1.Int());
		}
	else
		User::Leave(KTestAppFailure);
	}

void CStateTranFromShutdown::StartTestL()
	{
	CActiveScheduler* sched=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	// Notifier used to get the notification when system gets to fail state
	CSsmStateAwareSession* notif_for_failstate = CSsmStateAwareSession::NewLC(KSM2UiServicesDomain3);
	notif_for_failstate->AddSubscriberL(*this);

	// This async callback is used to start the state transition test once system enters to normal state.
	iAsyncCallbackForStartTest =  new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack startTestCallback(StartTest, this);
	iAsyncCallbackForStartTest->Set(startTestCallback);

	// call the callback function which keeps looping until the system state gets into normal state.
	iAsyncCallbackForStartTest->CallBack();
	
	RProcess::Rendezvous(KErrNone);
	sched->Start();

	CleanupStack::PopAndDestroy(2);
	}

void CStateTranFromShutdown::CallStopSchedulerL()
	{
	TSsmState currenState;
	currenState = iSsmStateAwareSess.State();

	TUint16 currentMainState = currenState.MainState();
	// Active scheduler is stopped once the system goes into fail state, else it issues a request for callback again
	if (currentMainState == ESsmFail)
		{
 		RDebug::Print(_L("\nStateTran Test: Stop Active Scheduler\n"));
		iFileWriteStream.WriteUint16L(currentMainState);
		CActiveScheduler::Stop();
		}
	else
		{
		iAsyncCallbackForStopScheduler->CallBack();
		}
	}

TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	TInt testCompletionReason = KErrNone;
	if (cleanup)
		{
		__UHEAP_MARK;
		CStateTranFromShutdown* stateTran = NULL;
		TRAP(r, stateTran = CStateTranFromShutdown::NewL());
		if (r == KErrNone)
			{
			TRAP(testCompletionReason, stateTran->StartTestL());
			// testCompletionReason is the reason with which the test case completes.
			TRAP(r, stateTran->CommitTestResultsL(testCompletionReason));
			delete stateTran;
			}
		delete cleanup;
		__UHEAP_MARKEND;

		if (r != KErrNone)
			{
			User::Panic(_L("tgsastatetranfromnormalPanic"), r);
			}
		}

	__UHEAP_MARKEND;
	return r;
	}



