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

#include "tcmd_step_coopsysperformrestartactions.h"
#include "ssmcustomcmdfactory.h"
#include "cmdcoopsysperformrestartactions.h"

CCustomCmdTestCoopSysPerformRestartActions::~CCustomCmdTestCoopSysPerformRestartActions()
	{
	}

CCustomCmdTestCoopSysPerformRestartActions::CCustomCmdTestCoopSysPerformRestartActions()
	{
	SetTestStepName(KTCCustomCmdTestCoopSysPerformRestartActions);
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0002
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0002
 */

void CCustomCmdTestCoopSysPerformRestartActions::TestCustomCmdCoopSysPerformRestartActionsL()
	{
	_LIT(KTESTLOG, "TestCustomCmdCoopSysPerformRestartActionsL");
	INFO_PRINTF1(KTESTLOG);

	//Create command for Cooperative system perform restart actions
	MSsmCustomCommand* customCmdCoopSysPerformRestartActions = SsmCustomCmdFactory::CmdCoopSysPerformRestartActionsNewL();
	CSsmCustomCommandEnv* cmdEnv = NULL;

	//Initialise the command. CSsmCustomCommandEnv is not used inside the Initialize so passing
	//null should be fine.
	customCmdCoopSysPerformRestartActions->Initialize(cmdEnv);
	TRequestStatus status;

	//Some dummy reason code
	TInt reason = 1;
	TPckgC<TInt> pckgReasonCode(reason);

	//Execute the command with some dummy reason
	customCmdCoopSysPerformRestartActions->Execute(pckgReasonCode, status);
	
	//Wait for the command to be completed
	User::WaitForRequest(status);
	TEST(KErrNone == status.Int());

	//Execute the command once again
	customCmdCoopSysPerformRestartActions->Execute(pckgReasonCode, status);
	TEST(KRequestPending == status.Int());
	
	//Cancel the commmand execution.
	customCmdCoopSysPerformRestartActions->ExecuteCancel();
	
	//Refernce plugins doesnt have Cancel implementation and also by the time ExecuteCancel()
	// is called it might have completed executing Execute() function. Thats is the reason for 
	//checking the status to KErrNone instead of KErrCancel.
	User::WaitForRequest(status);
	TEST(KErrNone == status.Int());

	//Close the command
	customCmdCoopSysPerformRestartActions->Close();
	
	//Releasing the comand will delete itself.
	customCmdCoopSysPerformRestartActions->Release();
	}

TVerdict CCustomCmdTestCoopSysPerformRestartActions::doTestStepL()
	{
	__UHEAP_MARK;
	TestCustomCmdCoopSysPerformRestartActionsL();
	__UHEAP_MARKEND;
	return TestStepResult();
	}

