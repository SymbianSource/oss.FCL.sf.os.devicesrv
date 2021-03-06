// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "appfwk_sysstart_test_step_DllCdNormal.h"
#include "appfwk_sysstart_test_dll.h"

/**
Old Test CaseID 		APPFWK-SYSSTART-0012
New Test CaseID 		DEVSRVS-SYSSTART-STARTUP-0012
 */


void CAppfwkSysStartTestStepDllCdNormal::TestDllCdNormalL()
	{
	TTestDllResults expectedTestDllResults;
	
	INFO_PRINTF1(_L("Test - TestDllFn1"));
	expectedTestDllResults.count=1;
	expectedTestDllResults.arg1 =0;
	expectedTestDllResults.arg2 =0;
	expectedTestDllResults.arg3 =0;
	TestDllResultsL(KTDllFn1ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn2"));
	expectedTestDllResults.arg1 =1000;
	TestDllResultsL(KTDllFn2ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn3"));
	expectedTestDllResults.arg1 =2000;
	expectedTestDllResults.arg2 =2001;
	TestDllResultsL(KTDllFn3ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn4"));
	expectedTestDllResults.arg1 =3000;
	expectedTestDllResults.arg2 =3001;
	expectedTestDllResults.arg3 =3002;
	TestDllResultsL(KTDllFn4ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn5"));
	expectedTestDllResults.arg1 =2000;
	expectedTestDllResults.arg2 =2001;
	expectedTestDllResults.arg3 =0;
	TestDllResultsL(KTDllFn5ResultFileName, expectedTestDllResults);
	
	INFO_PRINTF1(_L("Test - TestDllFn6"));
	expectedTestDllResults.arg1 =1000;
	expectedTestDllResults.arg2 =0;
	TestDllResultsL(KTDllFn6ResultFileName, expectedTestDllResults);
	}
	

/**
   Destructor
 */
CAppfwkSysStartTestStepDllCdNormal::~CAppfwkSysStartTestStepDllCdNormal()
	{
	}


/**
   Constructor
 */
CAppfwkSysStartTestStepDllCdNormal::CAppfwkSysStartTestStepDllCdNormal()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCAppfwkSysStartTestStepDllCdNormal);
	}


/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkSysStartTestStepDllCdNormal::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0012: DllCdNormal - Started"));
	
	iFs.Connect();
	CleanupClosePushL(iFs);

 	__UHEAP_MARK;
 	
	TestDllCdNormalL();

	CleanupStack::PopAndDestroy(&iFs);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("APPFWK-SYSSTART-0012: DllCdNormal - Finished"));
	
	return TestStepResult();
	}

