/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Basis effect internal getters header file.
*
*/

#ifndef BASISIVTGETTERS_H
#define BASISIVTGETTERS_H


#include "ivtinternal.h"


/**
 * Utility class for getting various basis effect specific information
 * from data buffer.
 */
class BasisIVTGetters
    {
public:

    /** 
     * Method for getting basis effect properties.
     *
     * @param aMsg         Pointer to the data buffer containing the 
     *                     effect definition.
     * @param aDuration    Duration of the effect (out param).
     * @param aStartDelay  Start delay of the effect (out param). 
     * @param aMagnitude   Magnitude of the effect (out param). 
     * @param aStartFlag   Start flag of the effect (out param). 
     * @param aType        Type of the effect (out param). 
     * @param aFreqPer     Period of the effect (out param). 
     * @param aControlMode Control mode of the effect (out param). 
     */
    static void VibeIVTGetPeriodic( 
        const TUint8*        aMsg,
        TVibeIVTMessageTime* aDuration,
        TVibeIVTMessageTime* aStartDelay,
        TVibeIVTMagnitude*   aMagnitude,
        TInt*                aStartFlag,
        TInt*                aType,
        TVibeIVTFreqPer*     aFreqPer,
        TInt*                aControlMode );

    /** 
     * Method for getting basis effect modifier (envelope) properties.
     *
     * @param aMsg          Pointer to the data buffer containing the 
     *                      effect definition.
     * @param aImpulseTime  Impulse (attack) time of the effect 
     *                      (out param). 
     * @param aFadeTime     Fade time of the effect (out param). 
     * @param aImpulseLevel Impulse (attack) level of the effect 
     *                      (out param). 
     * @param aFadeLevel    Fade level of the effect (out param). 
     */
    static void VibeIVTGetPeriodicModifier( 
        const TUint8*        aMsg,
        TVibeIVTMessageTime* aImpulseTime,
        TVibeIVTMessageTime* aFadeTime,
        TVibeIVTMagnitude*   aImpulseLevel,
        TVibeIVTMagnitude*   aFadeLevel );

};

#endif /* BASISIVTGETTERS_H */
