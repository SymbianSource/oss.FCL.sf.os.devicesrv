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


#ifndef __TCMD_STEP_COMMANDLIST_H__
#define __TCMD_STEP_COMMANDLIST_H__

#include <test/testexecutestepbase.h>

_LIT(KTCommandList,"CommandList");

class CCommandListTest : public CTestStep
	{
public:
	CCommandListTest();
	~CCommandListTest();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
		
private:
	void Test1L();
	void Test2L();
	void Test3L();
	void Test4L();

	};

#endif
