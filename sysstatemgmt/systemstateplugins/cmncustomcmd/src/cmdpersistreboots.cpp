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

#include <bautils.h>

#include "cmdpersistreboots.h"
#include "ssmdebug.h"
#include "ssmpanic.h"

_LIT(KBootUpFolder, ":\\private\\2000d75b\\bootupinfo\\");
_LIT(KBootUpFile, "bootupcount.bin");

/**
 * Constructs and returns a new custom command, leaving on errors.
 * 
 * @internalComponent
 */
CCustomCmdPersistReboots* CCustomCmdPersistReboots::NewL()
	{
	CCustomCmdPersistReboots* self = new (ELeave) CCustomCmdPersistReboots();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CCustomCmdPersistReboots::ConstructL()
	{
	const TChar drive = RFs::GetSystemDriveChar();
	TInt length = KBootUpFolder().Length() + KBootUpFile().Length() + 1 /* for RFs::GetSystemDriveChar() */;
	iBootupInfoPath.CreateL(length);
	iBootupInfoPath.Append(drive);
	iBootupInfoPath.Append(KBootUpFolder());
	iBootupInfoPath.Append(KBootUpFile());
	
	}

void CCustomCmdPersistReboots::CreateLogIfNotExistL()
	{
	__ASSERT_DEBUG(NULL != iFs.Handle(),PanicNow(KPanicSsmCustCmdRfs, EInvalidRFs));
	const TBool found = BaflUtils::FileExists(iFs, iBootupInfoPath);

	if(!found)
		{
		TInt err = iFs.MkDirAll(iBootupInfoPath);
		if(KErrAlreadyExists != err && KErrNone != err)
			User::Leave(err);
		// create log file to store the number for bootup attempts
		RFile file;
		User::LeaveIfError(file.Create(iFs, iBootupInfoPath, EFileShareExclusive | EFileWrite | EFileRead));
		file.Close();
		LogBootupCountL(0);
		}	
	}

CCustomCmdPersistReboots::CCustomCmdPersistReboots()
	{
	}

CCustomCmdPersistReboots::~CCustomCmdPersistReboots()
	{
	iBootupInfoPath.Close();
	}

/**
 * Initializes this custom command.
 * 
 * @internalComponent
 */
TInt CCustomCmdPersistReboots::Initialize(CSsmCustomCommandEnv* aCmdEnv)
	{
	iFs = aCmdEnv->Rfs();
	TRAPD(ret, CreateLogIfNotExistL());
	return ret;
	}

/**
 * Destory this object and any resources allocated to it.
 * 
 * @internalComponent
 */
void CCustomCmdPersistReboots::Close()
	{
	}

void CCustomCmdPersistReboots::Release()
	{
	delete this;
	}

/**
 * Issues the required rendezvous.
 * 
 * @internalComponent
 */
void CCustomCmdPersistReboots::Execute(const TDesC8& aParams, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	TCustCmdPersistRebootExecuteOption commandOption = EUndefined;

	TInt err = KErrNone;
	TRAP(err, commandOption = ExtractExecuteOptionL(aParams));

	if(KErrNone != err)
		{
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, err);
		return;
		}

	switch (commandOption)
		{
		case EIncrementBootCount:
			{
			TRAP(err, IncrementBootCountL());
			break;
			}
		case EResetBootCount:
			{
			TRAP(err, ResetBootCountL());
			break;
			}
		default:
			{
			err = KErrArgument;
			break;	
			}
		}

	TRequestStatus* statusPtr = &aStatus;
	User::RequestComplete(statusPtr, err);
	}

/**
 * Cancels the requested execute
 * 
 * @internalComponent
 */
void CCustomCmdPersistReboots::ExecuteCancel()
	{
	// Nothing to do as request already completed in Execute()
	}

TUint8 CCustomCmdPersistReboots::BootCountFromLogL()
	{
	__ASSERT_DEBUG(NULL != iFs.Handle(),PanicNow(KPanicSsmCustCmdRfs, EInvalidRFs));
	RFileReadStream file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.Open(iFs, iBootupInfoPath, EFileRead));
	TUint8 bootCount = file.ReadUint8L();
	CleanupStack::PopAndDestroy(&file);
	return bootCount;
	}

void CCustomCmdPersistReboots::IncrementBootCountL()
	{
	TUint8 bootCount = BootCountFromLogL();
	LogBootupCountL(++bootCount);
	DEBUGPRINT2A(" Tried booting the device (%d) time(s)", bootCount);
	}

void CCustomCmdPersistReboots::ResetBootCountL()
	{
	LogBootupCountL(0);	//always reset to 0
	DEBUGPRINT1A(" Boot count reset to (0)");
	}

TCustCmdPersistRebootExecuteOption CCustomCmdPersistReboots::ExtractExecuteOptionL(const TDesC8& aParams)
	{
	RDesReadStream readStream(aParams);
	CleanupClosePushL(readStream);
	TCustCmdPersistRebootExecuteOption executeOption = (TCustCmdPersistRebootExecuteOption)readStream.ReadInt8L();
	CleanupStack::PopAndDestroy(&readStream);
	return executeOption;
	}

void CCustomCmdPersistReboots::LogBootupCountL(TUint8 aCount)
	{
	__ASSERT_DEBUG(NULL != iFs.Handle(),PanicNow(KPanicSsmCustCmdRfs, EInvalidRFs));
	RFileWriteStream file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.Open(iFs, iBootupInfoPath, EFileWrite));
	file.WriteUint8L(aCount);
	file.CommitL();
	CleanupStack::PopAndDestroy(&file);
	}
