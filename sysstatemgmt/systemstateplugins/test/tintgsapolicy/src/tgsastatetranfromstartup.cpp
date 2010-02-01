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

class CStateTranFromStartup : public CTGsaStateTransition
	{
public:
	static CStateTranFromStartup* NewL();
	~CStateTranFromStartup();

private:
	void ConstructL();
	CStateTranFromStartup();

public:
	void DoTestStateTranFromStartupL();
	void DoTestStateTranFromStartuptoShutdownL();
	};

static TInt StopScheduler(TAny* aStateTranFromStartup)
 	{
 	CStateTranFromStartup* stateTran = reinterpret_cast<CStateTranFromStartup*>(aStateTranFromStartup);
 	TRAPD(err,stateTran->CallStopSchedulerL());
 	return err;
 	}

CStateTranFromStartup* CStateTranFromStartup::NewL()
	{
	CStateTranFromStartup* self = new(ELeave) CStateTranFromStartup();
	self->ConstructL();
	return self;
	}

void CStateTranFromStartup::ConstructL()
	{
	CTGsaStateTransition::ConstructL(KGsaTestStateTranFromStartupResult);
	}

CStateTranFromStartup::~CStateTranFromStartup()
	{
	}

CStateTranFromStartup::CStateTranFromStartup()
	{
	}

// Tests the Fail state transitions from startup under different scenarios
void CStateTranFromStartup::DoTestStateTranFromStartupL()
	{
	CActiveScheduler* sched=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	
	// Notifier used to get the notification when system gets to fail state
	CSsmStateAwareSession* notif_for_failstate = CSsmStateAwareSession::NewLC(KSM2UiServicesDomain3);
	notif_for_failstate->AddSubscriberL(*this);

	RSsmStateManager stateMan;
	TInt err = stateMan.Connect();
	CleanupClosePushL(stateMan);
	if (err == KErrNone)
		RDebug::Print(_L("\nStateTran Test: RSsmStateManager sess Connect %d\n"), err);
	else
		{
		RDebug::Print(_L("\nStateTran Test: Unable to connect to RSsmStateManager sess\n"));	
		User::Leave(KTestAppFailure);
		}

	// Creates and open the result file for streaming
	err = iFileWriteStream.Replace(iFs, KGsaTestStateTranFromStartupResult, EFileWrite);
	RDebug::Print(_L("\nStateTran Test: iFileWriteStream open with %d\n"), err);

	CurrentSystemStateL();
	//Scenario 1 - Request for state change from Startup to invalid state
	RDebug::Print(_L("\nStateTran Test: Scenario 1 \n"));
	TSsmState state(10, KSsmAnySubState);
	TSsmStateTransition stateTrans_InvalidState(state, 1);

	TRequestStatus status;
	stateMan.RequestStateTransition(stateTrans_InvalidState, status);
	RDebug::Print(_L("\nStateTran Test: Start RequestStateTransition from Startup to Invalid state with %d\n "), status.Int());
	
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

	//Scenario 2 - Request for state change from Startup to fail state with invalid substate
	RDebug::Print(_L("\nStateTran Test: Scenario 2 \n"));
	const TInt KInvalidFailSubState = 20;
	state.Set(ESsmFail, KInvalidFailSubState);
	TSsmStateTransition stateTrans_InvalidSubState(state, 1);

	stateMan.RequestStateTransition(stateTrans_InvalidSubState, status);
	RDebug::Print(_L("\nStateTran Test: Start RequestStateTransition from Startup to fail state with invalid substate %d\n "), status.Int());
	
	User::WaitForRequest(status);
	iFileWriteStream.WriteInt32L(status.Int());
	if (status.Int() == KErrNotSupported)
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Startup to fail state with invalid substate completes with %d Expected -5\n "), status.Int());
		CurrentSystemStateL();
		}
	else
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Startup to fail state with invalid substate Failed"));
		User::Leave(KTestAppFailure);		
		}

	//Scenario 3 - Request for state change from Startup to shutdown invalid substate
	RDebug::Print(_L("\nStateTran Test: Scenario 3 \n"));
	TRequestStatus status1;
	const TInt KInvalidShutdownSubState = 15;
	TSsmState shutdownState_Invalid(ESsmShutdown, KInvalidShutdownSubState);
	TSsmStateTransition stateTrans_ShutdownInvalid(shutdownState_Invalid, 1);

	stateMan.RequestStateTransition(stateTrans_ShutdownInvalid, status);
	RDebug::Print(_L("\nStateTran Test: Start RequestStateTransition from Startup to Shutdown state with Invalid Substate %d\n "), status.Int());
	
	User::WaitForRequest(status);
	iFileWriteStream.WriteInt32L(status.Int());
	if (status.Int() == KErrNotSupported)
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Startup to Shutdown state with invalid substate completes with %d Expected -5\n "), status.Int());
		CurrentSystemStateL();
		}
	else
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Startup to fail state with invalid substate Failed"));
		User::Leave(KTestAppFailure);		
		}

	//Scenario 4 - Request for two state changes to fail state one after the other
	//The first request to the fail state will be completed with KErrCancel and the second request to transit to the fail state will be completed with KErrNone.
	RDebug::Print(_L("\nStateTran Test: Scenario 4 \n"));
	state.Set(ESsmFail, KSsmAnySubState);
	TSsmStateTransition stateTrans_fail(state, 1);

	stateMan.RequestStateTransition(stateTrans_fail, status);
	RDebug::Print(_L("\nStateTran Test: Start First RequestStateTransition from Startup to fail state %d\n "), status.Int());
	
	stateMan.RequestStateTransition(stateTrans_fail, status1);
	RDebug::Print(_L("\nStateTran Test: Start Second RequestStateTransition from Startup to fail state %d\n "), status1.Int());
	
	User::WaitForRequest(status);
	iFileWriteStream.WriteInt32L(status.Int());
	if (status.Int() == KErrCancel)
		{
		RDebug::Print(_L("\nStateTran Test: First RequestStateTransition from Startup to fail state completes with %d Expected -3\n "), status.Int());
		CurrentSystemStateL();
		}
	else
		{
		RDebug::Print(_L("\nStateTran Test: First RequestStateTransition from Startup to fail state Failed"));
		User::Leave(KTestAppFailure);		
		}

	User::WaitForRequest(status1);
	iFileWriteStream.WriteInt32L(status1.Int());
	if (status1.Int() == KErrNone)
		{
		RDebug::Print(_L("\nStateTran Test: Second RequestStateTransition from Startup to fail state completes %d Expected 0\n "), status1.Int());
		CurrentSystemStateL();
		}
	else
		{
		RDebug::Print(_L("\nStateTran Test: Second RequestStateTransition from Startup to fail state Failed"));
		User::Leave(KTestAppFailure);		
		}

	// This async callback is used to stop the scheduler once the system goes into fail state.
	iAsyncCallbackForStopScheduler =  new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack stopSchedulerCallback(StopScheduler, this);
	iAsyncCallbackForStopScheduler->Set(stopSchedulerCallback);

	iAsyncCallbackForStopScheduler->CallBack();
	sched->Start();

	CleanupStack::PopAndDestroy(3);
	}

// Tests the Shutdown state transitions from startup scenario
void CStateTranFromStartup::DoTestStateTranFromStartuptoShutdownL()
	{
	RDebug::Printf("I am in CStateTranFromStartup::DoTestStateTranFromStartupShutdownL");
	CActiveScheduler* sched=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	
	// Notifier used to get the notification when system gets to Shutdown state
	CSsmStateAwareSession* notif_for_shutdownstate = CSsmStateAwareSession::NewLC(KSM2UiServicesDomain3);
	notif_for_shutdownstate->AddSubscriberL(*this);

	RSsmStateManager stateMan;
	TRequestStatus status;
	TInt err = stateMan.Connect();
	CleanupClosePushL(stateMan);
	if (err == KErrNone)
		RDebug::Print(_L("\nStateTran Shutdown Test: RSsmStateManager sess Connect %d\n"), err);
	else
		{
		RDebug::Print(_L("\nStateTran Shutdown Test: Unable to connect to RSsmStateManager sess\n"));	
		User::Leave(KTestAppFailure);
		}

		// Creates and open the result file for streaming
	err = iFileWriteStream.Replace(iFs, KGsaTestStateTranFromStartupResult, EFileWrite);
	RDebug::Print(_L("\nStateTran Shutdown Test: iFileWriteStream open with %d\n"), err);

	CurrentSystemStateL();

	RDebug::Print(_L("\nStateTran Shutdown Test Scenario \n"));
	TSsmState shutdownState_Critical(ESsmShutdown, ESsmShutdownSubStateCritical);
	TSsmStateTransition stateTrans_ShutdownCritical(shutdownState_Critical, 1);

	stateMan.RequestStateTransition(stateTrans_ShutdownCritical, status);
	RDebug::Print(_L("\nStateTran Shutdown Test: Start RequestStateTransition from Startup to shutdown state %d\n "), status.Int());
		
	User::WaitForRequest(status);
	iFileWriteStream.WriteInt32L(status.Int());
	RDebug::Printf("The value of status is %d",status.Int());
	if (status.Int() == KErrNone)
		{
		RDebug::Print(_L("\nStateTran Shutdown Test: RequestStateTransition from Startup to Shutdown state completes with %d Expected 0\n "), status.Int());
		CurrentSystemStateL();
		}
	else
		{
		RDebug::Print(_L("\nStateTran Shutdown Test: RequestStateTransition from Startup to Shutdown state Failed"));
		User::Leave(KTestAppFailure);		
		}

	// This async callback is used to stop the scheduler once the system goes into Shutdown state.
	iAsyncCallbackForStopScheduler =  new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack stopSchedulerCallback(StopScheduler, this);
	iAsyncCallbackForStopScheduler->Set(stopSchedulerCallback);

	iAsyncCallbackForStopScheduler->CallBack();
	sched->Start();

	CleanupStack::PopAndDestroy(3);

	}

TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	TInt testCompletionReason = KErrNone;
	TInt argc = User::CommandLineLength();
	if (cleanup)
		{
		__UHEAP_MARK;
		CStateTranFromStartup* stateTran = NULL;
		TRAP(r, stateTran = CStateTranFromStartup::NewL());
		if (r == KErrNone)
			{
			if(argc>0)
				{
				TRAP(testCompletionReason, stateTran->DoTestStateTranFromStartuptoShutdownL());
				}
			else
				{
				TRAP(testCompletionReason, stateTran->DoTestStateTranFromStartupL());
				}
			// testCompletionReason is the reason with which the test case completes.
			TRAP(r, stateTran->CommitTestResultsL(testCompletionReason));
			
			// Rendezvous'ng the process as it started as WaitForSignal execution behaviour
			RProcess::Rendezvous(KErrNone);
			delete stateTran;
			}
		delete cleanup;
		__UHEAP_MARKEND;

		if (r != KErrNone)
			{
			User::Panic(_L("tgsastatetranfromstartuppanic"), r);
			}
		}

	__UHEAP_MARKEND;
	return r;
	}



