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

#ifndef __SSMTESTAPPSUCESSFULAFTERRETRY_H
#define __SSMTESTAPPSUCESSFULAFTERRETRY_H

#include <s32file.h>
#include <f32file.h>

_LIT(KFileForFAndF, "c:\\countforfnf.dat");	// this file should be deleted by the test application

/**
Application class
*/
#include <eikapp.h>

class CTestFAndFApplication : public CEikApplication
	{
public:
	static CApaApplication* NewApplication();
	~CTestFAndFApplication();
	
private:
CTestFAndFApplication();
	
	// from CApaApplication
	TUid AppDllUid() const;
	CApaDocument* CreateDocumentL();
	};

/**
Document class
*/
#include <eikdoc.h>

class CEikAppUi;
class CEikApplication;
class CTestFAndFDocument : public CEikDocument
	{
public:
	static CTestFAndFDocument* NewL(CEikApplication& aApp);
	~CTestFAndFDocument();
	
private:
	CTestFAndFDocument(CEikApplication& aApp);
	
	// from CEikDocument
	CEikAppUi* CreateAppUiL();
	};

/**
Application UI class, root of all graphical user interface in this application
*/
#include <eikappui.h>

class CTestAppAo;
class CTestFAndFAppUi : public CEikAppUi
    {
public:
	CTestFAndFAppUi();
	~CTestFAndFAppUi();
	
	// from CEikAppUi
	void ConstructL();
	};

#endif // __SSMTESTAPPSUCESSFILAFTERRETRY_H
