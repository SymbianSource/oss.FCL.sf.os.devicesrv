// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CMDDELETETEMPFILES_H
#define CMDDELETETEMPFILES_H

#include <ssm/ssmcustomcommand.h>
#include <f32file.h>
class CSsmCustomCommandEnv;

NONSHARABLE_CLASS(CCustomCmdDeleteTempFiles) : public CBase, public MSsmCustomCommand
    {
public:
	static CCustomCmdDeleteTempFiles* NewL();

	// From MSsmCustomCommand
	TInt Initialize(CSsmCustomCommandEnv* aCmdEnv);
	void Execute(const TDesC8& aParams, TRequestStatus& aRequest);
	void ExecuteCancel();
	void Close();
	void Release();

private:
	CCustomCmdDeleteTempFiles();
	~CCustomCmdDeleteTempFiles();
	void DeleteTempFilesL();

private:
    RFs iFs; //Not owned
    };
#endif // CMDDELETETEMPFILES_H
