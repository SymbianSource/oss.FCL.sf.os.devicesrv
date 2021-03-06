// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32property.h>
const TUint32 KEmergencyCallRfAdaptationPluginPropertyKey = 0x2000E657;
const TUid KPropertyCategory={0x2000D75B};

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
	// Setting the P and S key will route the request to the reference plugins instead of the actual plugins
	TInt err = RProperty::Define(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey, RProperty::EInt);
	TEST(KErrNone == err || KErrAlreadyExists == err);
    err = RProperty::Set(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey, 1);
    TEST(KErrNone == err);

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
	err = RProperty::Delete(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey);
	TEST(KErrNone == err);
	}

TVerdict CCustomCmdTestActivateRfForEmergencyCall::doTestStepL()
	{
	__UHEAP_MARK;
	TestCustomCmdActivateRfForEmergencyCallL();
	__UHEAP_MARKEND;
	return TestStepResult();
	}
