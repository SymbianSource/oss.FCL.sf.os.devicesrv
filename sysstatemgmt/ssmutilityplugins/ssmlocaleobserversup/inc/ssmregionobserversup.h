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
* Description: Declaration of CSsmRegionObserver class.
*
*/
#ifndef __SSMREGIONOBSERVER_H__
#define __SSMREGIONOBSERVER_H__

#include <e32base.h>
#include <ssm/ssmutility.h>
#include <e32property.h>
#include "trace.h"

/**
* SSM Utility plugin to observe the Region code changes and load the new Region
* when it is changed.
*/
NONSHARABLE_CLASS( CSsmRegionObserver ) : public CActive, public MSsmUtility
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
    ~CSsmRegionObserver();
    
    /**
     * C++ default constructor.
     */
    CSsmRegionObserver();

    /**
    * Activate the active object.
    */
    void Activate();

    /**
    * Loads the Region set in the PS key.
    */
    void LoadRegion();
    
    /**
    * Store the Region code to Central Repository.
    * @param aRegion The Region code to store.
    */
    void StoreRegionToCentRep( const TInt aRegion);
    
private: // data

    /**
    * Used for observing Region code changes.
    */
    RProperty iRegionProperty;
    };
#endif // __SSMREGIONOBSERVER_H__
