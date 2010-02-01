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
 @internalTechnology
*/

#include "te_sysutilsunittestepbase.h"
#include <e32property.h>
#include "sysutilconstants.h"

CTeSysUtilsSuiteStepBase::~CTeSysUtilsSuiteStepBase()
	{
	iFs.Close();
	}

CTeSysUtilsSuiteStepBase::CTeSysUtilsSuiteStepBase()
	{
	}
