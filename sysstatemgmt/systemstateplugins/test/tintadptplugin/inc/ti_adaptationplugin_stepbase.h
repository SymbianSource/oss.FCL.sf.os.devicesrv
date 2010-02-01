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

#ifndef __TI_ADAPTATIONPLUGIN_STEPBASE_H__
#define __TI_ADAPTATIONPLUGIN_STEPBASE_H__

#include <e32std.h>
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include <ssm/ssmadaptationcli.h>


class CTestAdaptStep : public CTestStep
	{
public:
	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	void SetActivationCompleted();
	void StopActiveScheduler();
		
protected:
	CActiveScheduler* iActiveScheduler;
	CAsyncCallBack* iAsyncStopScheduler;
	TBool iAdptTransitionCompleted;
	};

#endif