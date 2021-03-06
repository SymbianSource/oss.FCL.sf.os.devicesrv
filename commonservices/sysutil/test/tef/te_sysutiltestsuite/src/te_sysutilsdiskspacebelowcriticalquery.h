/**
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
*
*/



/**
 @file
 @internalTechnology
*/

#ifndef TE_SYSUTILSDISKSPACEBELOWCRITICALQUERY_H_
#define TE_SYSUTILSDISKSPACEBELOWCRITICALQUERY_H_
#include "te_sysutilssuitestepbase.h"
#include <sysutil.h>

class CSysUtilsDiskSpaceBelowCriticalQueryStep : public CTeSysUtilsSuiteStepBase
	{
public:
	CSysUtilsDiskSpaceBelowCriticalQueryStep();
	~CSysUtilsDiskSpaceBelowCriticalQueryStep();
	virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:
	RFs iFs;
	};

_LIT(KSysUtilsDiskSpaceBelowCriticalQueryStep,"SysUtilsDiskSpaceBelowCriticalQuery");

#endif /*TE_SYSUTILSDISKSPACEBELOWCRITICALQUERY_H_*/
