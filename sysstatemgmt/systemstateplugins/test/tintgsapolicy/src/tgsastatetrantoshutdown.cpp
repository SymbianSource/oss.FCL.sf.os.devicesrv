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

class CStateTranToShutdown : public CTGsaStateTransition
	{
public:
	static CStateTranToShutdown* NewL();
	~CStateTranToShutdown();
	void StartTestL();

private:
	void ConstructL();
	CStateTranToShutdown();

public:
	void DoTestStateTranToShutdownL();
	};

static TInt StopScheduler(TAny* aStateTranToShutdown)
 	{
 	CStateTranToShutdown* stateTran = reinterpret_cast<CStateTranToShutdown*>(aStateTranToShutdown);
 	TRAPD(err,stateTran->CallStopSchedulerL());
 	return err;
 	}

static TInt StartTest(TAny* aStateTranToShutdown)
 	{
 	TInt testCompletionReason = KErrNone;
 	CStateTranToShutdown* stateTran = reinterpret_cast<CStateTranToShutdown*>(aStateTranToShutdown);
 	TBool stateTest = stateTran->DoStartStateTranTest();
 	if (stateTest)
		TRAP(testCompletionReason, stateTran->DoTestStateTranToShutdownL());

 	return testCompletionReason;
 	}

CStateTranToShutdown* CStateTranToShutdown::NewL()
	{
	CStateTranToShutdown* self = new(ELeave) CStateTranToShutdown();
	self->ConstructL();
	return self;
	}

void CStateTranToShutdown::ConstructL()
	{
	CTGsaStateTransition::ConstructL(KGsaTestStateTranResultFilePath);
	}

CStateTranToShutdown::~CStateTranToShutdown()
	{
	}

CStateTranToShutdown::CStateTranToShutdown()
	{
	}

// Tests the Shutdown state transitions under different scenarios
void CStateTranToShutdown::DoTestStateTranToShutdownL()
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
	err = iFileWriteStream.Replace(iFs, KGsaTestStateTranToShutdownResult, EFileWrite);
	RDebug::Print(_L("\nStateTran Test: iFileWriteStream open with %d\n"), err);


	// Gets the Current system state before starting the test
	CurrentSystemStateL();

	//Scenario 1 - Request for state transition to shutdown state - invalid substate
	// request to transit to the shutdown state will be completed with KErrNotSupported
	RDebug::Print(_L("\nScenario 1 \n"));

	TRequestStatus status_invalidsubstate;
	TSsmState shutdownState_Invalid(ESsmShutdown, 150);
	TSsmStateTransition stateTrans_Invalid(shutdownState_Invalid, 1);

	stateMan.RequestStateTransition(stateTrans_Invalid, status_invalidsubstate);
	RDebug::Print(_L("\n Start RequestStateTransition from Normal to shutdown state with invalid substate %d\n "), status_invalidsubstate.Int());
	
	User::WaitForRequest(status_invalidsubstate);
	iFileWriteStream.WriteInt32L(status_invalidsubstate.Int());
	if (status_invalidsubstate.Int() == KErrNotSupported)
		{
		CurrentSystemStateL();
		RDebug::Print(_L("\n RequestStateTransition from Normal to shutdown state with invalid substate completes with %d Expected -5\n "), status_invalidsubstate.Int());
		}
	else
		User::Leave(KTestAppFailure);

	//Scenario 2 - Request for state transition to shutdown state - critical substate(Restart)
	// Request for state transition to shutdown state - critical substate(Standby)
	// first request to transit to the shutdown state critical substate(Restart)will be completed with KErrCancel
	// second request to transit to the shutdown state critical substate(Standby)will be completed with KErrNone
	RDebug::Print(_L("\nScenario 2 \n"));
	TRequestStatus status_shutdowncritsubstate;
	TSsmState shutdownState_Critical(ESsmShutdown, ESsmShutdownSubStateCritical);
	TSsmStateTransition stateTrans_Shutdowncritical_Restart(shutdownState_Critical, 3);

	stateMan.RequestStateTransition(stateTrans_Shutdowncritical_Restart, status_shutdowncritsubstate);
	RDebug::Print(_L("\n RequestStateTransition from Normal to shutdown state critical substate Restart%d\n "), status_shutdowncritsubstate.Int());

	TRequestStatus status_shutdowncritsubstate1;
	TSsmStateTransition stateTrans_Shutdowncritical_Standby(shutdownState_Critical, 1);

	stateMan.RequestStateTransition(stateTrans_Shutdowncritical_Standby, status_shutdowncritsubstate1);
	RDebug::Print(_L("\n RequestStateTransition from Normal to shutdown state critical substate Standby %d\n "), status_shutdowncritsubstate.Int());

	User::WaitForRequest(status_shutdowncritsubstate1);
	iFileWriteStream.WriteInt32L(status_shutdowncritsubstate1.Int());
	if (status_shutdowncritsubstate1.Int() == KErrNone)
		{
		RDebug::Print(_L("\n RequestStateTransition from Normal to shutdown state critical substate completed Standby %d Expected 0\n "), status_shutdowncritsubstate1.Int());
		CurrentSystemStateL();
		}
	else
		User::Leave(KTestAppFailure);

	User::WaitForRequest(status_shutdowncritsubstate);
	iFileWriteStream.WriteInt32L(status_shutdowncritsubstate.Int());
	if (status_shutdowncritsubstate.Int() == KErrCancel)
		{
		CurrentSystemStateL();
		RDebug::Print(_L("\n RequestStateTransition from Normal to shutdown state critical substate completed Restart %d Expected -3\n "), status_shutdowncritsubstate.Int());
		}
	else
		User::Leave(KTestAppFailure);

	}

void CStateTranToShutdown::StartTestL()
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
		CStateTranToShutdown* stateTran = NULL;
		TRAP(r, stateTran = CStateTranToShutdown::NewL());
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
			User::Panic(_L("tgsastatetrantoshutdownpanic"), r);
			}
		}

	__UHEAP_MARKEND;
	return r;
	}



