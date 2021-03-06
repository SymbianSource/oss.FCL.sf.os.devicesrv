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


#ifndef __TCMD_GOODSUP_H__
#define __TCMD_GOODSUP_H__

#include <e32base.h>
#include <ssm/ssmutility.h>
		
NONSHARABLE_CLASS(CCmdGoodSup) : public CActive, public MSsmUtility
	{
public:
	IMPORT_C static MSsmUtility* NewL();
	
	//from MSsmUtility
	void InitializeL();
	void StartL();
	void Release();

	//from CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
private:
	//from CBase
	CCmdGoodSup();
	~CCmdGoodSup();
	};

#endif
