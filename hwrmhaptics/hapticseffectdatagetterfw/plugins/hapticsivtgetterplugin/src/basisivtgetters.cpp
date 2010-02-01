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
* Description:  IVT effect get functions' high level helper functions 
*               (for basis effects) implementation file.
*/

#include "basisivtgetters.h"

// ---------------------------------------------------------------------------
// Gets the periodic effect data from the effect data buffer to which pointer
// is given as argument.
// ---------------------------------------------------------------------------
//
void BasisIVTGetters::VibeIVTGetPeriodic(
        const TUint8* aMsg, 
        TVibeIVTMessageTime* aDuration, 
        TVibeIVTMessageTime* aStartDelay, 
        TVibeIVTMagnitude* aMagnitude, 
        TInt* aStartFlag, 
        TInt* aType, 
        TVibeIVTFreqPer* aFreqPer, 
        TInt* aControlMode )
    {
    if ( aMsg )
        {
        if ( aDuration )
            {
            *aDuration = VIBE_IVT_GET_PERIODIC_DURATION( aMsg );
            }
        if ( aStartDelay )
            {
            *aStartDelay = VIBE_IVT_GET_PERIODIC_START_DELAY( aMsg );
            }
        if ( aMagnitude )
            {
            *aMagnitude = VIBE_IVT_GET_PERIODIC_MAGNITUDE( aMsg );
            }
        if ( aStartFlag )
            {
            *aStartFlag = VIBE_IVT_GET_PERIODIC_START_FLAG( aMsg );
            }
        if ( aType )
            {
            *aType = VIBE_IVT_GET_PERIODIC_TYPE( aMsg );
            }
        if ( aFreqPer )
            {
            *aFreqPer = VIBE_IVT_GET_PERIODIC_FREQPER( aMsg );
            }
        if ( aControlMode )
            {
            *aControlMode   = VIBE_IVT_GET_PERIODIC_CONTROLMODE( aMsg );
            }
        }
    }

// ---------------------------------------------------------------------------
// Gets the periodic modifier data from the effect data buffer to which
// pointer is given as argument.
// ---------------------------------------------------------------------------
//
void BasisIVTGetters::VibeIVTGetPeriodicModifier( 
        const TUint8* aMsg, 
        TVibeIVTMessageTime* aImpulseTime, 
        TVibeIVTMessageTime* aFadeTime, 
        TVibeIVTMagnitude* aImpulseLevel, 
        TVibeIVTMagnitude* aFadeLevel )
    {
    if ( aMsg )
        {
        if ( aImpulseTime )
            {
            *aImpulseTime =
                VIBE_IVT_GET_PERIODIC_MODIFIER_IMPULSE_TIME( aMsg );
            }
        if ( aFadeTime )
            {
            *aFadeTime = VIBE_IVT_GET_PERIODIC_MODIFIER_FADE_TIME( aMsg );
            }
        if ( aImpulseLevel )
            {
            *aImpulseLevel =
                VIBE_IVT_GET_PERIODIC_MODIFIER_IMPULSE_LEVEL( aMsg );
            }
        if ( aFadeLevel )
            {
            *aFadeLevel = VIBE_IVT_GET_PERIODIC_MODIFIER_FADE_LEVEL( aMsg );
            }
        }
    }

// end of file
