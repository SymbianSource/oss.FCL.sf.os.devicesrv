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

#include "tcmd_step_coopsysperformrfsactions.h"
#include "ssmcustomcmdfactory.h"
#include "cmdcoopsysperformrfsactions.h"

CCustomCmdTestCoopSysPerformRfsActions::~CCustomCmdTestCoopSysPerformRfsActions()
	{
	}

CCustomCmdTestCoopSysPerformRfsActions::CCustomCmdTestCoopSysPerformRfsActions()
	{
	SetTestStepName(KTCCustomCmdTestCoopSysPerformRfsActions);
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0003
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0003
 */

void CCustomCmdTestCoopSysPerformRfsActions::TestCustomCmdCoopSysPerformRfsActionsL()
	{
	_LIT(KTESTLOG, "TestCustomCmdCoopSysPerformRfsActionsL");
	INFO_PRINTF1(KTESTLOG);

	MSsmCustomCommand* customCmdCoopSysPerformRfsActions = SsmCustomCmdFactory::CmdCcoopSysPerformRfsActionsNewL();
	CSsmCustomCommandEnv* cmdEnv = NULL;
	
	//Initialise the command. CSsmCustomCommandEnv is not used inside the Initialize so passing
	//null should be fine.
	customCmdCoopSysPerformRfsActions->Initialize(cmdEnv);
	TRequestStatus status;

	TSsmRfsType ssmRfsType;
	ssmRfsType = ESsmShallowRfs;
	TPckgC<TSsmRfsType> pckgSsmRfsType(ssmRfsType);
	
	//Execute the command
	customCmdCoopSysPerformRfsActions->Execute(pckgSsmRfsType, status);
	
	//Wait for the command to be completed
	User::WaitForRequest(status);
	TEST(KErrNone == status.Int());

	//Execute the command once again
	customCmdCoopSysPerformRfsActions->Execute(pckgSsmRfsType, status);
	TEST(KRequestPending == status.Int());
	
	//Cancel the commmand execution.
	customCmdCoopSysPerformRfsActions->ExecuteCancel();
	
	//Refernce plugins doesnt have Cancel implementation and also by the time ExecuteCancel()
	// is called it might have completed executing Execute() function. Thats is the reason for 
	//checking the status to KErrNone instead of KErrCancel.
	User::WaitForRequest(status);
	TEST(KErrNone == status.Int());

	
	//Create an invalid enum value
	TSsmRfsType ssmRfsTypeInvalid = static_cast<TSsmRfsType>(ESsmRfsTypeExtensionBase - 1);
	
	TPckgC<TSsmRfsType> pckgSsmRfsTypeInvalid(ssmRfsTypeInvalid);
	customCmdCoopSysPerformRfsActions->Execute(pckgSsmRfsTypeInvalid, status);
	TEST(KErrArgument == status.Int());

	//Close the command
	customCmdCoopSysPerformRfsActions->Close();
	
	//Releasing the comand will delete itself.
	customCmdCoopSysPerformRfsActions->Release();
	}

TVerdict CCustomCmdTestCoopSysPerformRfsActions::doTestStepL()
	{
	__UHEAP_MARK;
	TestCustomCmdCoopSysPerformRfsActionsL();
	__UHEAP_MARKEND;
	return TestStepResult();
	}
