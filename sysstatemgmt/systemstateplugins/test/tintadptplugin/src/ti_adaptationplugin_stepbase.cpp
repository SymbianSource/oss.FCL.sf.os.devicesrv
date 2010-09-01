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
#include "ti_adaptationplugin_stepbase.h"

#include <e32debug.h>

static TInt StopScheduler(TAny* aTestAdaptStep)
	{
	// We would stop the sceduler once all the required transitions are completed
	RDebug::Print(_L("Inside Stop Scheduler..."));
	CTestAdaptStep*  testAdaptStep = static_cast<CTestAdaptStep*>(aTestAdaptStep);
	testAdaptStep->StopActiveScheduler();
	return KErrNone;
	}

void CTestAdaptStep::StopActiveScheduler()
	{
	// We would stop the sceduler once all the required transitions are completed
	if (iAdptTransitionCompleted)
		{
		CActiveScheduler::Stop();
		}
	else
		{
		iAsyncStopScheduler->CallBack();
		}
	}

void CTestAdaptStep::SetActivationCompleted()
	{
	iAdptTransitionCompleted = ETrue;
	}

TVerdict CTestAdaptStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("Starting the scheduler in CTestEmergencyAdaptStep::doTestStepPreambleL ..."));
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);

	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack stop(StopScheduler, this);
	iAsyncStopScheduler->Set(stop);
	
	return CTestStep::doTestStepPreambleL();
	}

/** */
TVerdict CTestAdaptStep::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}
