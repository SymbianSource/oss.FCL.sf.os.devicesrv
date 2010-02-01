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

#include "tcmncustomcmd_step_persistreboots.h"
#include "ssmcmncustomcmdfactory.h"
#include "cmdpersistreboots.h"
#include <ssm/ssmcustomcommand.h>

CCustomCmdTestPersistReboots::~CCustomCmdTestPersistReboots()
	{
	}

CCustomCmdTestPersistReboots::CCustomCmdTestPersistReboots()
	{
	SetTestStepName(KTCCustomCmdTestPersistReboots);
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0001
New Test CaseID 		DEVSRVS-SSPLUGINS-CUSTCMD-0001
 */

void CCustomCmdTestPersistReboots::TestCustomCmdPersistRebootsL()
	{
	INFO_PRINTF1(_L("TestCustomCmdPersistRebootsL"));

	//Create Persist Reboots custom command
	MSsmCustomCommand* customCmdPersistReboots = SsmCmnCustomCmdFactory::CmdPersistRebootsNewL();
	RFs aFs;
	aFs.Connect();
	CSsmCustomCommandEnv* cmdEnv = CSsmCustomCommandEnv::NewL(aFs);
	CleanupStack::PushL(cmdEnv);
	
	//Initialise the command.
	customCmdPersistReboots->Initialize(cmdEnv);

	TRequestStatus status;
	
	//Create an invalid enum value to pass to Execute
	TCustCmdPersistRebootExecuteOption executeOption=static_cast<TCustCmdPersistRebootExecuteOption>(EIncrementBootCount+100);
	TPckgBuf<TCustCmdPersistRebootExecuteOption> pckgExecuteOption(executeOption);
	
	
	customCmdPersistReboots->Execute(pckgExecuteOption, status);
	customCmdPersistReboots->ExecuteCancel();
	
	//Reference plugins doesnt have Cancel implementation and also by the time ExecuteCancel()
	//is called it might have completed executing Execute() function. Thats is the reason for 
	//checking the status to KErrNone instead of KErrCancel.
	User::WaitForRequest(status);
	TEST(KErrArgument == status.Int());
	
	//EIncrementBootCount is incremented in the loop so EResetBootCount also tested
	for (int i=0;i<2;i++)
		{
			executeOption=static_cast<TCustCmdPersistRebootExecuteOption>(EIncrementBootCount+i);
			TPckgBuf<TCustCmdPersistRebootExecuteOption> pckgExecuteOption(executeOption);
			
			customCmdPersistReboots->Execute(pckgExecuteOption, status);
			User::WaitForRequest(status);
			TEST(KErrNone == status.Int());
		}
	
	
	customCmdPersistReboots->Close();
	customCmdPersistReboots->Release();
	CleanupStack::PopAndDestroy(cmdEnv);
	INFO_PRINTF1(_L("Ending the CCustomCmdTestPersistReboots tests "));

	}

TVerdict CCustomCmdTestPersistReboots::doTestStepL()
	{
	__UHEAP_MARK;
	TestCustomCmdPersistRebootsL();
	__UHEAP_MARKEND;
	return TestStepResult();
	}
