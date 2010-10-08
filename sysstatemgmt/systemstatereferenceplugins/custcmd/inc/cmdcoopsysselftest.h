// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released
*/

#ifndef __CMDCOOPSYSSELFTEST_H__
#define __CMDCOOPSYSSELFTEST_H__

#include <ssm/ssmcustomcommand.h>
#ifdef SYMBIAN_INCLUDE_APP_CENTRIC
#include <r_sec_logical_channel.h>
#else
#include <ssm/ssmadaptationcli.h>
#endif //SYMBIAN_INCLUDE_APP_CENTRIC


NONSHARABLE_CLASS (CCustomCmdCoopSysSelfTest) : public CBase, public MSsmCustomCommand
	{
public:
	static CCustomCmdCoopSysSelfTest* NewL();

	// from MSsmcustomCommand
	TInt Initialize(CSsmCustomCommandEnv* aCmdEnv);
	void Execute(const TDesC8& aParams, TRequestStatus& aStatus);
	void Close();
	void Release();
	void ExecuteCancel();

private:
	CCustomCmdCoopSysSelfTest();
	~CCustomCmdCoopSysSelfTest();

private:
//Read the selftest result from Security Driver if SYMBIAN_INCLUDE_APP_CENTRIC is enabled.
#ifdef SYMBIAN_INCLUDE_APP_CENTRIC
	RSecEnvLogicalChannel iSecEnvLogicalChannel;
	TBool iSsmLoadSecLdd;
	TSecSelfTest iParam;
#else
	RSsmStateAdaptation iSsmStateAdaptation;
#endif //SYMBIAN_INCLUDE_APP_CENTRIC
	};

#endif // __CMDCOOPSYSSELFTEST_H__