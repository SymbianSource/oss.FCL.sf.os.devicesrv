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
 

#ifndef __TSUS_STEP_ADAPTIONSERVER_H__
#define __TSUS_STEP_ADAPTIONSERVER_H__


#include <test/testexecutestepbase.h>
#include "ssmadaptationcli.h"


_LIT(KTSusAdaptionServerStep,"SusAdaptionServerStep");

class RTestSusAdaptationCli : public RSsmEmergencyCallRfAdaptation
    {
public:
    TInt SetHeapFailure(TInt aFailureRate);
    TInt UnSetHeapFailure();
    };

class CSusAdaptionServerTest : public CTestStep
	{
public:
	CSusAdaptionServerTest ();
	~CSusAdaptionServerTest ();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
private:
	void doTestForMemoryLeaksL();
	void doTestProcessCriticalL();
	void doTestOOML();
	void doTestForEmergencyCallOOM();
	void StartAndDestroyServerL();
	void doTestForSettingPriorityClient();
	void doTestForNormalEmergencyCallinOOM();
	};

#endif
