/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name        : strtsecuritynoteobserver.h
* Part of     : System Startup / Starter
* Interface   : None
* Declaration of CStrtSecurityNoteObserver class.
* Version     : %version: ou1s60rt#4 %
* This material, including documentation and any related computer
* programs, is protected by copyright controlled by Nokia.  All
* rights are reserved.  Copying, including reproducing, storing,
* adapting or translating, any or all of this material requires the
* prior written consent of Nokia.  This material also contains
* confidential information which may not be disclosed to others
* without the prior written consent of Nokia.
* Template version: 4.0
* Nokia Core OS *
* File renamed from strtsecuritynoteobserver.h to securitynoteobserver.h as part of Core OS transfer.
*
*/




/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#ifndef __SECURITYNOTEOBSERVER_H__
#define __SECURITYNOTEOBSERVER_H__

#include <e32base.h>

NONSHARABLE_CLASS(CSecurityNoteObserver) : public CActive
	{
public:
	static CSecurityNoteObserver* NewL();
	~CSecurityNoteObserver();

private:
	//from CBase
	CSecurityNoteObserver(const TUid& aCategory, const TUint aKey);
	TBool PropertyChanged(const TInt aCurrentValue);
	void StartL();

	//from CActive
	void DoCancel();
	void RunL();
	void Activate();
	
private:
	// Category of the P&S key to observe.
	TUid iCategory;
	
	// Identifies the P&S key to observe.
	TUint iKey;
	
	// Publish & subscribe interface for the SIM indicator flag key.
	RProperty iProperty;
	};

#endif //__SECURITYNOTEOBSERVER_H__
