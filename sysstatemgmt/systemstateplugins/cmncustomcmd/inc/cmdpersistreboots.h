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
 @internalComponent
 @released
*/

#ifndef __CMDPERSISTREBOOTS_H__
#define __CMDPERSISTREBOOTS_H__

#include <s32file.h>
#include <s32mem.h>

#include <ssm/ssmcustomcommand.h>
#include <ssm/cmdpersistreboots.hrh>

NONSHARABLE_CLASS (CCustomCmdPersistReboots) : public CBase, public MSsmCustomCommand
	{
public:
	static CCustomCmdPersistReboots* NewL();
	TUint8 BootCountFromLogL();

	// From MSsmCustomCommand
	TInt Initialize(CSsmCustomCommandEnv* aCmdEnv);
	void Execute(const TDesC8& aParams, TRequestStatus& aStatus);
	void Close();
	void Release();
	void ExecuteCancel();
	
private:
	CCustomCmdPersistReboots();
	~CCustomCmdPersistReboots();

	void ConstructL();
	void IncrementBootCountL();
	void ResetBootCountL();
	TCustCmdPersistRebootExecuteOption ExtractExecuteOptionL(const TDesC8& aParams);
	void LogBootupCountL(TUint8 aCount);
	void CreateLogIfNotExistL();
private:
	RFs iFs;
	RBuf iBootupInfoPath;
	};

#endif // __CMDPERSISTREBOOTS_H__
