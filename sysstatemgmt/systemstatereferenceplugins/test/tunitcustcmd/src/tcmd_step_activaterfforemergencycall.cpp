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

#include "tcmd_step_activaterfforemergencycall.h"
#include "ssmcustomcmdfactory.h"
#include "cmdactivaterfforemergencycall.h"

CCustomCmdTestActivateRfForEmergencyCall::~CCustomCmdTestActivateRfForEmergencyCall()
	{
	}

CCustomCmdTestActivateRfForEmergencyCall::CCustomCmdTestActivateRfForEmergencyCall()
	{
	SetTestStepName(KTCCustomCmdTestActivateRfForEmergencyCall);
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0001
New Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0001
 */

void CCustomCmdTestActivateRfForEmergencyCall::TestCustomCmdActivateRfForEmergencyCallL()
	{
	_LIT(KTESTLOG, "TestCustomCmdActivateRfForEmergencyCallL");
	INFO_PRINTF1(KTESTLOG);

	//Create Deactivate Rf For Emergency Call custom command
	MSsmCustomCommand* customCmdActivateRfForEmergencyCall = SsmCustomCmdFactory::CmdActivateRfForEmergencyCallNewL();
	CSsmCustomCommandEnv* cmdEnv = NULL;

	//Initialise the command. CSsmCustomCommandEnv is not used inside the Initialize so passing
	//null should be fine.
	customCmdActivateRfForEmergencyCall->Initialize(cmdEnv);

	TRequestStatus status;
	TBufC8<1> dummy;

	//Command parameter is not used inside the Execute. So passing any dummy data should be fine
	//Execute the command
	customCmdActivateRfForEmergencyCall->Execute(dummy, status);
	TEST(KRequestPending == status.Int());

	//Wait for the request to be completed
	User::WaitForRequest(status);
	TEST(KErrNone == status.Int());

	//Execute the command once again
	customCmdActivateRfForEmergencyCall->Execute(dummy, status);

	//Cancel the commmand execution.
	customCmdActivateRfForEmergencyCall->ExecuteCancel();

	//Refernce plugins doesnt have Cancel implementation and also by the time ExecuteCancel()
	//is called it might have completed executing Execute() function. Thats is the reason for 
	//checking the status to KErrNone instead of KErrCancel.
	User::WaitForRequest(status);
	TEST(KErrNone == status.Int());

	//Close the command
	customCmdActivateRfForEmergencyCall->Close();

	//Releasing the comand will delete itself.
	customCmdActivateRfForEmergencyCall->Release();
	}

TVerdict CCustomCmdTestActivateRfForEmergencyCall::doTestStepL()
	{
	__UHEAP_MARK;
	TestCustomCmdActivateRfForEmergencyCallL();
	__UHEAP_MARKEND;
	return TestStepResult();
	}
