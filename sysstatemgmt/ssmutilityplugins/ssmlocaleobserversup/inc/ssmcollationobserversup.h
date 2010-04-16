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
* Description: Declaration of CSsmCollationObserver class.
*
*/

#ifndef __SSMCOLLATIONOBSERVER_H__
#define __SSMCOLLATIONOBSERVER_H__

#include <e32base.h>
#include <ssm/ssmutility.h>
#include <e32property.h>

/**
* SSM Utility plugin to observe the Collation code changes and load the new collation
* when it is changed.
*/
NONSHARABLE_CLASS( CSsmCollationObserver ) : public CActive, public MSsmUtility
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
    ~CSsmCollationObserver();
    
    /**
     * C++ default constructor.
     */
    CSsmCollationObserver();

    /**
    * Activate the active object.
    */
    void Activate();

    /**
    * Loads the Collation set in the PS key.
    */
    void LoadCollation();
    
    /**
    * Store the Collation code to Central Repository.
    * @param aCollation The Collation code to store.
    */
	void StoreCollationToCentRep( const TInt aCollation );
	
private: // data

    /**
    * Used for observing Collation code changes.
    */
	RProperty iCollationProperty;
    };
#endif // __SSMCOLLATIONOBSERVER_H__
