/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Declaration of CSsmUiLanguageObserver class.
*
*/
#ifndef __SSMUILANGUAGEOBSERVER_H__
#define __SSMUILANGUAGEOBSERVER_H__

#include <e32base.h>
#include <ssm/ssmutility.h>
#include <e32property.h>
#include "trace.h"

/**
* SSM Utility plugin to observe the UI Language code changes and load the new UI Language
* when it is changed.
*/
NONSHARABLE_CLASS( CSsmUiLanguageObserver ) : public CActive, public MSsmUtility
    {
public:
	//From MSsmUtility
	void InitializeL();
	void StartL();
	void Release();
    
public:
    /**
     * Two-phased constructor.
     */
    IMPORT_C static MSsmUtility* NewL();

protected:
    // from base class CActive

    /**
     * Implements cancellation of an outstanding request.
     */
    void DoCancel();

    /**
     * Handles an active object’s request completion event.
     * This RunL will never leave, so RunError not needed.
     */
    void RunL();

private:
    /**
    * Destructor.
    */
    ~CSsmUiLanguageObserver();
    
    /**
     * C++ default constructor.
     */
    CSsmUiLanguageObserver();

    /**
    * Activate the active object.
    */
    void Activate();

    /**
    * Loads the Language set in the PS key.
    */
    void LoadUILanguage();

    /**
    * Store the Language code to Central Repository.
    * @param aUILanguage The Language code to store.
    */
    void StoreUILanguageToCentRep( const TInt aUILanguage);
	
private: // data

    /**
    * Used for observing UI Language code changes.
    */
	RProperty iUILanguageProperty;
    };
#endif // __SSMUILANGUAGEOBSERVER_H__
