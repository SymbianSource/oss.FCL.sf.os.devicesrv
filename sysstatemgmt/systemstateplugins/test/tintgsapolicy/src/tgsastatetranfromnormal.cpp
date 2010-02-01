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

class CStateTranFromNormal : public CTGsaStateTransition
	{
public:
	static CStateTranFromNormal* NewL();
	~CStateTranFromNormal();
	void StartTestL();

private:
	void ConstructL();
	CStateTranFromNormal();

public:
	void DoTestStateTranFromNormalL();
	};

static TInt StopScheduler(TAny* aStateTranFromNormal)
 	{
 	CStateTranFromNormal* stateTran = reinterpret_cast<CStateTranFromNormal*>(aStateTranFromNormal);
 	TRAPD(err,stateTran->CallStopSchedulerL());
 	return err;
 	}

static TInt StartTest(TAny* aStateTranFromNormal)
 	{
 	TInt testCompletionReason = KErrNone;
 	CStateTranFromNormal* stateTran = reinterpret_cast<CStateTranFromNormal*>(aStateTranFromNormal);
 	TBool stateTest = stateTran->DoStartStateTranTest();
 	if (stateTest)
		TRAP(testCompletionReason, stateTran->DoTestStateTranFromNormalL());

 	return testCompletionReason;
 	}

CStateTranFromNormal* CStateTranFromNormal::NewL()
	{
	CStateTranFromNormal* self = new(ELeave) CStateTranFromNormal();
	self->ConstructL();
	return self;
	}

void CStateTranFromNormal::ConstructL()
	{
	CTGsaStateTransition::ConstructL(KGsaTestStateTranResultFilePath);
	}

CStateTranFromNormal::~CStateTranFromNormal()
	{
	}

CStateTranFromNormal::CStateTranFromNormal()
	{
	}

// Tests the Fail state transitions under different scenarios
void CStateTranFromNormal::DoTestStateTranFromNormalL()
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
	err = iFileWriteStream.Replace(iFs, KGsaTestStateTranFromNormalResult, EFileWrite);
	RDebug::Print(_L("\nStateTran Test: iFileWriteStream open with %d\n"), err);

	// Gets the Current system state
	CurrentSystemStateL();

	//Scenario 1 - Request for state change from Normal to invalid state
	RDebug::Print(_L("\nStateTran Test: Scenario 1 \n"));
	TSsmState state(10, KSsmAnySubState);
	TSsmStateTransition stateTrans_InvalidState(state, 1);

	TRequestStatus status;
	stateMan.RequestStateTransition(stateTrans_InvalidState, status);
	RDebug::Print(_L("\nStateTran Test: Start RequestStateTransition from Normal to Invalid state with %d\n "), status.Int());
	
	User::WaitForRequest(status);
	iFileWriteStream.WriteInt32L(status.Int());
	if (status.Int() == KErrNotSupported)
		{
		CurrentSystemStateL();
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Normal to Invalid state completes with %d Expected -5\n "), status.Int());
		}
	else
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Normal to Invalid state Failed"));
		User::Leave(KTestAppFailure);
		}

	//Scenario 2 - Request for state change from Normal to fail state with invalid substate
	RDebug::Print(_L("\nStateTran Test: Scenario 2 \n"));
	const TInt KInvalidFailSubState = 20;
	state.Set(ESsmFail, KInvalidFailSubState);
	TSsmStateTransition stateTrans_InvalidSubState(state, 1);

	stateMan.RequestStateTransition(stateTrans_InvalidSubState, status);
	RDebug::Print(_L("\nStateTran Test: Start RequestStateTransition from Normal to fail state with invalid substate %d\n "), status.Int());
	
	User::WaitForRequest(status);
	iFileWriteStream.WriteInt32L(status.Int());
	if (status.Int() == KErrNotSupported)
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Normal to fail state with invalid substate completes with %d Expected -5\n "), status.Int());
		CurrentSystemStateL();
		}
	else
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Normal to fail state with invalid substate Failed"));
		User::Leave(KTestAppFailure);		
		}

	TRequestStatus status1;
	TSsmState shutdownState_Critical(ESsmShutdown, ESsmShutdownSubStateCritical);
	TSsmStateTransition stateTrans_Shutdowncritical(shutdownState_Critical, 1);

	TRequestStatus status2;
	state.Set(ESsmFail, KSsmAnySubState);
	TSsmStateTransition stateTrans3(state, 1);

	//Scenario 3 - Request for two state changes(1. fail state 2. shutdown state) one after the other without waiting 
	// for the first one to complete. Cancels the state transition request. The first request to the fail state will be 
	// completed with KErrCancel and the second request to transit to the shutdown state is also completed with KErrCancel.

	RDebug::Print(_L("\nStateTran Test: Scenario 3 \n"));
	stateMan.RequestStateTransition(stateTrans3, status1);
	RDebug::Print(_L("\nStateTran Test: Start RequestStateTransition from Normal to fail state %d\n "), status1.Int());
	
	stateMan.RequestStateTransition(stateTrans_Shutdowncritical, status2);
	RDebug::Print(_L("\nStateTran Test: Start RequestStateTransition from Normal to shutdown state %d\n "), status2.Int());
	
	stateMan.RequestStateTransitionCancel();
	User::WaitForRequest(status1);
	iFileWriteStream.WriteInt32L(status1.Int());
	if (status1.Int() == KErrCancel)
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Normal to fail state completes with %d Expected -3\n "), status1.Int());
		CurrentSystemStateL();
		}
	else
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Normal to fail state Failed"));
		User::Leave(KTestAppFailure);		
		}

	User::WaitForRequest(status2);
	iFileWriteStream.WriteInt32L(status2.Int());
	if (status2.Int() == KErrCancel)
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Normal to shutdown state completes %d Expected -3\n "), status2.Int());
		CurrentSystemStateL();
		}
	else
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Normal to shutdown state Failed"));
		User::Leave(KTestAppFailure);		
		}

	//Scenario 4 - Request for two state changes one after the other without waiting for the first one to complete
	// Once first and second request are completed the system will be in fail state, now issue another state transition
	// request to fail state again.
	// The first request to the fail state will be completed with KErrCancel 
	// the second request to transit to the fail state will be completed with KErrNone.
	// the third request to transit to the fail state will be completed with KErrNotSupported.
	
	RDebug::Print(_L("\nStateTran Test: Scenario 4 \n"));
	stateMan.RequestStateTransition(stateTrans3, status1);
	RDebug::Print(_L("\nStateTran Test: Start First RequestStateTransition from Normal to fail state %d\n "), status1.Int());
	
	stateMan.RequestStateTransition(stateTrans3, status2);
	RDebug::Print(_L("\nStateTran Test: Start Second RequestStateTransition from Normal to fail state %d\n "), status2.Int());

	User::WaitForRequest(status1);
	iFileWriteStream.WriteInt32L(status1.Int());
	if (status1.Int() == KErrCancel)
		{
		RDebug::Print(_L("\nStateTran Test: First RequestStateTransition from Normal to fail state completes with %d Expected -3\n "), status1.Int());
		CurrentSystemStateL();
		}
	else
		{
		RDebug::Print(_L("\nStateTran Test: First RequestStateTransition from Normal to fail state Failed"));
		User::Leave(KTestAppFailure);		
		}

	User::WaitForRequest(status2);
	iFileWriteStream.WriteInt32L(status2.Int());
	if (status2.Int() == KErrNone)
		{
		RDebug::Print(_L("\nStateTran Test: Second RequestStateTransition from Normal to fail state %d Expected 0\n "), status2.Int());
		CurrentSystemStateL();
		}
	else
		{
		RDebug::Print(_L("\nStateTran Test: Second RequestStateTransition from Normal to fail state Failed"));
		User::Leave(KTestAppFailure);		
		}
	
	stateMan.RequestStateTransition(stateTrans3, status2); //System is in fail state
	RDebug::Print(_L("\nStateTran Test: Start RequestStateTransition from fail to fail state %d\n "), status2.Int());
	
	User::WaitForRequest(status2);
	iFileWriteStream.WriteInt32L(status2.Int());
	if (status2.Int() == KErrNotSupported)
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from fail to fail state completed with %d Expected -5\n "), status2.Int());
		CurrentSystemStateL();
		}
	else
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from fail to fail state Failed"));
		User::Leave(KTestAppFailure);
		}
	}

void CStateTranFromNormal::StartTestL()
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

TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	TInt testCompletionReason = KErrNone;
	if (cleanup)
		{
		__UHEAP_MARK;
		CStateTranFromNormal* stateTran = NULL;
		TRAP(r, stateTran = CStateTranFromNormal::NewL());
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



