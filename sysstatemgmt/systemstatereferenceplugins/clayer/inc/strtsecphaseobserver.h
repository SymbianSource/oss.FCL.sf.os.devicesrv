/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : strtsecphaseobserver.h
* Part of     : System Startup / StrtSecObs
* Declaration of CStrtSecPhaseObserver class
* Version     : %version: 1 % << Don't touch! Updated by Synergy at check-out.
* This material, including documentation and any related computer
* programs, is protected by copyright controlled by Nokia.  All
* rights are reserved.  Copying, including reproducing, storing,
* adapting or translating, any or all of this material requires the
* prior written consent of Nokia.  This material also contains
* confidential information which may not be disclosed to others
* without the prior written consent of Nokia.
* Template version: 4.2
* Nokia Core OS *
*
*/



#ifndef C_STRTSECPHASEOBSERVER_H
#define C_STRTSECPHASEOBSERVER_H

#include <e32base.h>
#include <e32property.h>

/*
* @publishedPartner
* @released
*/

/**
 *  Monitors changes in KStarterSecurityPhase P&S property.
 *
 *  lib : strtsecobs.lib
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CStrtSecPhaseObserver ) : public CActive
    {

public:

    /**
     * Two-phased constructor.
     */
    static CStrtSecPhaseObserver* NewL();

    /**
    * Destructor.
    */
    virtual ~CStrtSecPhaseObserver();
    
    /**
     * Used to ask is startup SIM phase over.
     *    
     * @return TRUE if startup SIM phase is over.
     */
    TBool IsSecurityPhaseSimOk() const;
    
    /**
     * Used to ask is startup SEC phase over.
     *     
     * @return TRUE if startup SEC phase is over.
     */
    TBool IsSecurityPhaseSecOk() const;

private:
// from base class CActive

    /**
     * From CActive.
     * Implements cancellation of an outstanding request.
     */
    void DoCancel();

    /**
     * From CActive.
     * Handles an active object’s request completion event.
     * This RunL will never leave, so RunError not needed.
     */
    void RunL();

private:

    /**
     * C++ default constructor.
     */
    CStrtSecPhaseObserver();

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Subscribe to the property and set active. Should not be called if already
     * active.
     */
    void IssueRequest();

private: // data

    /**
     * Property to observe.
     */
    RProperty iProperty;

    /**
     * Current value of the property.
     */
    TInt iValue;

#ifdef TEST_CLAYER_MACRO
	friend class CLayerTestSsmEventObserver;
#endif //TEST_CLAYER_MACRO
    };

#endif // C_STRTSECPHASEOBSERVER_H
