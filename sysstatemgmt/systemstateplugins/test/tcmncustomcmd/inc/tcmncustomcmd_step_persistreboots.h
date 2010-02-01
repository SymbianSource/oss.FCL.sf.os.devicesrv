// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TCMNCUSTOMCMD_STEP_PERSISTREBOOTS_H__
#define __TCMNCUSTOMCMD_STEP_PERSISTREBOOTS_H__

#include <test/testexecutestepbase.h>

_LIT(KTCCustomCmdTestPersistReboots, "CCustomCmdTestPersistReboots");

class CCustomCmdTestPersistReboots: public CTestStep
	{
public:
	CCustomCmdTestPersistReboots();
	~CCustomCmdTestPersistReboots();

	//from CTestStep
	TVerdict doTestStepL();

private:
	void TestCustomCmdPersistRebootsL();
	};

#endif	// __TCMNCUSTOMCMD_STEP_PERSISTREBOOTS_H__