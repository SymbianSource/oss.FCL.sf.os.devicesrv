/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* file te_sysutilstestpersistenceini.h
* 
*
*/



#include "te_sysutilssuitestepbase.h"

_LIT(KSysUtilsTestPersistenceIniStep, "SysUtilsTestPersistenceIniStep");

class CSysUtilsTestPersistenceIniStep : public CTeSysUtilsSuiteStepBase
	{
public:
	CSysUtilsTestPersistenceIniStep();
	~CSysUtilsTestPersistenceIniStep();
	virtual TVerdict doTestStepL();
	};
