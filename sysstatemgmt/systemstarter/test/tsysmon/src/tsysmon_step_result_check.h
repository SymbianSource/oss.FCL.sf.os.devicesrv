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

#if (!defined __RESULTCHECK_STEP_H__)
#define __RESULTCHECK_STEP_H__
#include <test/testexecutestepbase.h>
#include <sysmonclisess.h>
#include "sysmontesthelper.h"

class CResultCheckStep : public CTestStep
	{
public:
	CResultCheckStep();
	~CResultCheckStep();
	
	//from CTestStep
	TVerdict doTestStepL();
	};

_LIT(KResultCheckStep,"ResultCheck");

#endif
