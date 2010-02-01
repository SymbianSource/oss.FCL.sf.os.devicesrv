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

#include <eikstart.h> 
#include <eikapp.h>
#include "ssmtestappsucessfulafterretry.h"
#include "ssmtestapps.h"


/**
Standard DLL entry point function.
Creates and returns an instance of the CApaApplication-derived class.
@return an instance of the CApaApplication-derived class
*/
TInt E32Main()
	{
	return EikStart::RunApplication(CTestFAndFApplication::NewApplication);
	}

CTestFAndFApplication::CTestFAndFApplication()
	{
	}

CTestFAndFApplication::~CTestFAndFApplication()
	{
	}
		
/**
@return The application's UID 
*/
TUid CTestFAndFApplication::AppDllUid() const
	{
	const TUid dll = {KTestAppForFAndFUid};
	return dll;
	}

/**
@return CTestFAndFApplication or NULL if KErrNoMemory
*/
CApaApplication* CTestFAndFApplication::NewApplication()
	{
	// As the framework has at this point not started up enough, and therefore the TRAP-harness and 
	// exception handlers aren’t available yet, this factory function is a non-leaving function and 
	// can't use the new(Eleave) operator.
	return new CTestFAndFApplication();
	}
	
/**
Called by the UI framework at application start-up to create an instance of the document class.
@leave	KErrNoMemory
@return A CTestFAndFDocument
*/
CApaDocument* CTestFAndFApplication::CreateDocumentL()
	{
	return CTestFAndFDocument::NewL(*this);
	}

CTestFAndFDocument::CTestFAndFDocument(CEikApplication& aApp) : CEikDocument(aApp)
	{
	}

CTestFAndFDocument::~CTestFAndFDocument()
	{
	}

/**
Factory function for this class
@return a new CEndTaskTestDocument instance.
*/
CTestFAndFDocument* CTestFAndFDocument::NewL(CEikApplication& aApp)
	{
	return new(ELeave) CTestFAndFDocument(aApp);
	}



/**
Called by the UI framework to construct the application UI class. 
Note that the app UI's ConstructL() is called by the UI framework.
*/
CEikAppUi* CTestFAndFDocument::CreateAppUiL()
	{
	return new(ELeave) CTestFAndFAppUi();
	}



CTestFAndFAppUi::CTestFAndFAppUi()
	{
	}



CTestFAndFAppUi::~CTestFAndFAppUi()
	{
	}



void CTestFAndFAppUi::ConstructL()
	{
	// Complete the UI framework's construction of the App UI.
	BaseConstructL(CEikAppUi::ENoAppResourceFile);
	
	RFs fs;
	RFileReadStream readStream;
	RFileWriteStream writeStream;
					
	CleanupClosePushL(fs);
	CleanupClosePushL(readStream);
	CleanupClosePushL(writeStream);

	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(readStream.Open(fs, KFileForFAndF, EFileRead));
	
	TInt retryCount = readStream.ReadInt8L();
	TInt failCount = readStream.ReadInt8L();
	TInt delayRequired = readStream.ReadInt8L();
	readStream.Close();	
	
	if(0 == failCount)
		{
		RProcess::Rendezvous(KErrNone);
		}
	else
		{
		User::LeaveIfError(writeStream.Open(fs, KFileForFAndF, EFileWrite));

		writeStream.WriteInt8L(retryCount+1);
		writeStream.WriteInt8L(--failCount);
		writeStream.CommitL();
		writeStream.Close();

		// This delay is required for testing the absolute timeout of startsafe, this is just 
		// random time delay been used.
		if(delayRequired)
			User::After(500000);
									

		// StartSafe will restart the process
		RProcess::Rendezvous(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3, &fs);
	}


