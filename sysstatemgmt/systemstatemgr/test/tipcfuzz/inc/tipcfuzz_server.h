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
 

#ifndef __TIPCFUZZ_SERVER_H__
#define __TIPCFUZZ_SERVER_H__

#include <test/testexecuteserverbase.h>

class CIpcFuzzTestServer : public CTestServer
	{
public:
	static CIpcFuzzTestServer * NewLC();

	//from CTestServer
	CTestStep* CreateTestStep(const TDesC& aStepName);
	};

#endif //__TIPCFUZZ_SERVER_H__
