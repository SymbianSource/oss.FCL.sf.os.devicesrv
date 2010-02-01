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

#include "testprocgoodsession.h"
#include "tsysmon_steprestartthrottle.h"


CStepRestartThrottle::CStepRestartThrottle()
	{
	SetTestStepName(KCTestCaseRestartThrottle);
	}
/**
Old Test CaseID 		APPFWK-SYSMON-0004
New Test CaseID 		DEVSRVS-SHMA-SYSMON-0004
 */

TVerdict CStepRestartThrottle::doTestStepL()
	{
	TInt err = iProcStartSignalSem.CreateGlobal(KStartShmaTestProcGood,0);
	INFO_PRINTF2(_L("Semaphore created with err %d"), err);
	TEST(KErrNone == err);
		
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0004"));
	
	INFO_PRINTF1(_L("New Test step"));
	INFO_PRINTF1(_L("Test System Monitor functionality using startupproperties"));
	RProcess process;
	CleanupClosePushL(process);
	StartAndMonitorUsingStartupPropL(process, EWaitForStart);
	
	iProcStartSignalSem.Wait();
	//Killing the process will cause sysmon to restart after KWaitTime (declared in sysmon_patch.h)
	process.Kill(KErrNone);
	CleanupStack::Pop(&process);
	INFO_PRINTF1(_L("Killed monitored process."));
	DoTestMonitor();
	
	INFO_PRINTF1(_L("Test System Monitor functionality using ssmstartupproperties"));
	CleanupClosePushL(process);
	StartAndMonitorUsingSsmStartupPropL(process, EWaitForStart);

	iProcStartSignalSem.Wait();
	//Killing the process will cause sysmon to restart after KWaitTime (declared in sysmon_patch.h)
	process.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&process);
	INFO_PRINTF1(_L("Killed monitored process."));
	DoTestMonitor();
	
	INFO_PRINTF1(_L("Test complete"));	
	iProcStartSignalSem.Close();
	return TestStepResult();	
	}

void CStepRestartThrottle::DoTestMonitor()
	{
	//Sleep 14 seconds and then assert that the server is not yet restarted
	INFO_PRINTF1(_L("Going to sleep for 14 seconds."));		
	User::After(KThrottleTime - 1000000);
	RTestProcGoodSession server;
	TInt err = server.Connect();
	TESTE(KErrNotFound == err, err);
	server.Close();
	if(KErrNotFound == err)
		{
		INFO_PRINTF1(_L("Asserted that server is not yet restarted."));		
		}
	else
		{
		ERR_PRINTF1(_L("Error - the restart took place too soon."));		
		}
		
	//Allow the process to be restarted
	INFO_PRINTF1(_L("Waiting till the process is restarted using semaphore."));		
	//Wait for the process to be restarted 
	iProcStartSignalSem.Wait();
		
	//Assert that the server is running
	INFO_PRINTF1(_L("Woke up, now the server should be restarted (sysmon KWaitTime is 15s)."));		
	err = server.Connect();
	TESTE(KErrNone == err, err);
	if(KErrNone == err)
		{
		INFO_PRINTF1(_L("Yes, asserted that server is running, going to cancel monitoring shutdown server"));		
		}
	server.CancelMonitor();
	server.ShutDown();
	server.Close();
	}
