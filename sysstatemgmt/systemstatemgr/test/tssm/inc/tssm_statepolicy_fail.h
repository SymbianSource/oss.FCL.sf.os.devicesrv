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


#ifndef __TSSM_STATEPOLICY_FAIL_H__
#define __TSSM_STATEPOLICY_FAIL_H__

#include <e32base.h>
#include <ssm/ssmstatepolicy.h>
#include <ssm/ssmstate.h>


const TUint16 ESsmTestFailState = 0x009C;			//Simulates ESsmFail, while preventing the system from using the real ESsmFail policy

//Use this known panic reasons / category if the test fails (as TEST macro is not available here)
enum TStateFailTestPanicReason
	{
	EStateFailTestFailed1 = 4040,
	EStateFailTestFailed2
	};
_LIT(KStateFailTestPanicCategory,"StateFailTest");


NONSHARABLE_CLASS(CSsmStatePolicyFail) : public CBase, public MSsmStatePolicy
	{
public:
	IMPORT_C static MSsmStatePolicy* NewL(); //lint !e1511
	
	//from CSsmStatePolicy
	void ConstructL();
	void Initialize(TRequestStatus& aStatus);
	void InitializeCancel();
	void Release();
	TResponse TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage);
	void PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus);
	void PrepareCommandListCancel();
	CSsmCommandList* CommandList();
	TBool GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState);
	
private:
	//from CBase
	CSsmStatePolicyFail();
	~CSsmStatePolicyFail();

private:
	CSsmCommandList* iCommandList;
	};

#endif
