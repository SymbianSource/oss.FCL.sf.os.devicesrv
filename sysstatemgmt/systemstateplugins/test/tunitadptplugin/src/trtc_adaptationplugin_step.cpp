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



#include <s32mem.h>
#include "trtc_adaptationplugin_step.h"

//
// Run the tests
//


CTestRtcAdaptationPlugin::CTestRtcAdaptationPlugin()
	:CAdaptationTestBase(KTCTestRtcAdaptationPlugin)
	{
	
	}

CTestRtcAdaptationPlugin::~CTestRtcAdaptationPlugin()
	{

	}



void CTestRtcAdaptationPlugin::TestValidateRtc()
	{
	INFO_PRINTF1(_L(" >CTestRtcAdaptationPlugin::TestValidateRtc"));

	TRequestStatus status;
	TPckgBuf <TTime> validityPckg;
	iSsmRtcAdaptation.ValidateRtc(validityPckg, status);
	User::WaitForRequest(status);
	if(iSsmRtcAdaptation.Handle())
		{
		TEST(KErrNotSupported == status.Int());	
		INFO_PRINTF3(_L("CTestRtcAdaptationPlugin::TestValidateRtc completed with %d error : expected %d>"),status.Int(),KErrNone);	
		}
	else
		{
		TEST(KErrDisconnected == status.Int());	
		INFO_PRINTF3(_L("CTestRtcAdaptationPlugin::TestValidateRtc completed with %d error : expected %d>"),status.Int(),KErrDisconnected);	
		}
	}

void CTestRtcAdaptationPlugin::TestSetWakeupAlarm()
	{
	INFO_PRINTF1(_L(" >CTestRtcAdaptationPlugin::TestSetWakeupAlarm"));

	TRequestStatus status;
	TPckgBuf <TTime> alarmTimePckg;
	iSsmRtcAdaptation.SetWakeupAlarm(alarmTimePckg, status);
	User::WaitForRequest(status);

	if(iSsmRtcAdaptation.Handle())
		{
		TEST(KErrNotSupported == status.Int());	
		INFO_PRINTF3(_L("CTestRtcAdaptationPlugin::TestSetWakeupAlarm completed with %d error : expected %d>"),status.Int(),KErrNone);	
		}
	else
		{
		TEST(KErrDisconnected == status.Int());	
		INFO_PRINTF3(_L("CTestRtcAdaptationPlugin::TestSetWakeupAlarm completed with %d error : expected %d>"),status.Int(),KErrDisconnected);	
		}
	}

void CTestRtcAdaptationPlugin::TestUnsetWakeupAlarm()
	{
	INFO_PRINTF1(_L(" >CTestRtcAdaptationPlugin::TestUnsetWakeupAlarm"));

	TRequestStatus status;
	iSsmRtcAdaptation.UnsetWakeupAlarm(status);
	User::WaitForRequest(status);

	if(iSsmRtcAdaptation.Handle())
		{
		TEST(KErrNotSupported == status.Int());	
		INFO_PRINTF3(_L("CTestRtcAdaptationPlugin::TestUnsetWakeupAlarm completed with %d error : expected %d>"),status.Int(),KErrNone);	
		}
	else
		{
		TEST(KErrDisconnected == status.Int());	
		INFO_PRINTF3(_L("CTestRtcAdaptationPlugin::TestUnsetWakeupAlarm completed with %d error : expected %d>"),status.Int(),KErrDisconnected);	
		}
	}

void CTestRtcAdaptationPlugin::TestCancel()
	{
	INFO_PRINTF1(_L(" >CTestRtcAdaptationPlugin::TestCancel"));
	iSsmRtcAdaptation.Cancel();
	INFO_PRINTF1(_L(" CTestRtcAdaptationPlugin::TestCancel>"));	
	}


//from CAdaptationTestBase
TVerdict CTestRtcAdaptationPlugin::doTestStepL()
	{
	TInt err = KErrNone;

	__UHEAP_MARK;
	TRAP(err, TestValidateRtc());
	TEST(err == KErrNone);

	TRAP(err, TestSetWakeupAlarm());
	TEST(err == KErrNone);
	
	TRAP(err, TestUnsetWakeupAlarm());
	TEST(err == KErrNone);
	
	TestCancel();
	
	User::LeaveIfError(iSsmRtcAdaptation.Connect());
	
	TRAP(err, TestValidateRtc());
	TEST(err == KErrNone);

	TRAP(err, TestSetWakeupAlarm());
	TEST(err == KErrNone);
	
	TRAP(err, TestUnsetWakeupAlarm());
	TEST(err == KErrNone);
	
	TestCancel();
	//TestRelease();			// have to test this part too ...


	__UHEAP_MARKEND;

	return TestStepResult();
	}
