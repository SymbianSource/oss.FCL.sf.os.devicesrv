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
 

#ifndef __TSSM_STEP_SWPPOLICYRESOLVER_H__
#define __TSSM_STEP_SWPPOLICYRESOLVER_H__

#include <test/testexecutestepbase.h>

_LIT(KTSsmSwpPolicyResolverStep,"SsmSwpPolicyResolverStep");

class CSsmSwpPolicyResolverTest : public CTestStep
	{
public:
	CSsmSwpPolicyResolverTest();
	~CSsmSwpPolicyResolverTest();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

private:	
	void doTestPolicyResolverForMemoryLeaksL();
	void doTestPolicyResolverFileErrHandlingL();
	void doTestPolicyResolverStateL();
	void doTestPolicyResolverHeapFailureL();
	};

#endif
