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
*               (for timeline effects) implementation file.
*/

#include "timelineivtgetters.h"

// ---------------------------------------------------------------------------
// Gets the effectID from the current location of the data as pointed by the
// aProp pointer argument and advances the aPProp argument's pointer value to 
// the next data location
// ---------------------------------------------------------------------------
//
TVibeIVTEffectID TimelineIVTGetters::VibeIVTGetEffectID(    
        const TUint8* aProp, 
        const TUint8** aPProp )
    {
    TVibeIVTEffectID effectID( 0 );

    // aPProp can be NULL if the caller doesn't want to retrieve it
    if ( aProp )
        {
        switch ( *( aProp++ ) ) 
            {
            case KVibeIvtPropertyTypeEffectIdU16:
                effectID |= *( aProp++ ) << 8;
                // fall through
            case KVibeIvtPropertyTypeEffectIdU8:
                effectID |= *( aProp++ );
                break;
            default:
                {
                // We shouldn't come here
                break;
                }
            }
        if ( aPProp ) 
            {
            *aPProp = aProp;
            }
        }
    return effectID;
    }

// ---------------------------------------------------------------------------
// Gets the time offset from the current location of the data as pointed by 
// the aProp pointer argument and advances the aPProp argument's pointer value 
// to the next data location
// ---------------------------------------------------------------------------
//
TInt TimelineIVTGetters::VibeIVTGetTimeOffset( const TUint8* aProp, 
                                               const TUint8** aPProp )
    {
    TInt time( 0 );

    // aPProp can be NULL if the caller doesn't want to retrieve it
    if ( aProp )
        {   
        switch ( *( aProp++ ) ) 
            {
            case KVibeIvtPropertyTypeTimeOffsetU32:
                time |= *( aProp++ ) << 24;
                // fall through
            case KVibeIvtPropertyTypeTimeOffsetU24: 
                time |= *( aProp++ ) << 16;
                // fall through
            case KVibeIvtPropertyTypeTimeOffsetU16: 
                time |= *( aProp++ ) << 8;
                time |= *( aProp++ );
                break;
            default:
                {
                // We should never come here.
                break;
                }    
            }
        if ( aPProp )
            {
            *aPProp = aProp;
            }
        }
    return time;
    }

// ---------------------------------------------------------------------------
// Gets the count value from the current location of the data as pointed by 
// the aProp pointer argument and advances the aPProp argument's pointer value
// to the next data location
// ---------------------------------------------------------------------------
//
TVibeIVTCount TimelineIVTGetters::VibeIVTGetCount( const TUint8* aProp,
                                                   const TUint8** aPProp )
    {
    TVibeIVTCount count( 0 );

    // aPProp can be NULL if the caller doesn't want to retrieve it
    if ( aProp )
        {
        switch ( *( aProp++ ) ) 
            {
            case KVibeIvtPropertyTypeCountU8: 
                count |= *( aProp++ );
                break;
            default:
                {
                // We should never come here.
                break;
                }
            }
        if ( aPProp ) 
            {
            *aPProp = aProp;
            }
        }
    return count;
    }

// ---------------------------------------------------------------------------
// Gets the duration value from the current location of the data as pointed by
// the aProp pointer argument and advances the aPProp argument's pointer value
// to the next data location
// ---------------------------------------------------------------------------
//
TVibeIVTMessageTime TimelineIVTGetters::VibeIVTGetDuration( 
        const TUint8* aProp,
        const TUint8** aPProp )
    {
    TVibeIVTMessageTime duration( 0 );

    // aPProp can be NULL if the caller doesn't want to retrieve it
    if ( aProp )
        {
        switch ( *( aProp++ ) )
            {
            case KVibeIvtPropertyTypeDurationMsU16: 
                duration |= *( aProp++ ) << 8;
                duration |= *( aProp++ );
                break;
            default:
                {
                // We should never come here.
                break;
                }
            }
        if ( aPProp ) 
            {
            *aPProp = aProp;
            }
        }
    return duration;
    }

// ---------------------------------------------------------------------------
// Gets the magnitude from the current location of the data as pointed by the
// aProp pointer argument and advances the aPProp argument's pointer value to 
// the next data location
// ---------------------------------------------------------------------------
//
TVibeIVTMagnitude TimelineIVTGetters::VibeIVTGetMagnitude( 
        const TUint8* aProp, 
        const TUint8** aPProp )
    {
    TVibeIVTMagnitude magnitude( 0 );

    // aPProp can be NULL if the caller doesn't want to retrieve it
    if ( aProp )
        {
        switch ( *( aProp++ ) ) 
            {
            case KVibeIvtPropertyTypeMagnitudeS8: 
                magnitude |= *( aProp++ );
                break;
            default:
                {
                // We should never come here.
                break;
                }
            }
        if ( aPProp )
            {
            *aPProp = aProp;
            }
        }
    return magnitude;
    }

// ---------------------------------------------------------------------------
// Gets the frequency value from the current location of the data as pointed 
// by the aProp pointer argument and advances the aPProp argument's pointer 
// value to the next data location
// ---------------------------------------------------------------------------
//
TVibeIVTFreqPer TimelineIVTGetters::VibeIVTGetFreqPer( 
        const TUint8* aProp, 
        const TUint8** aPProp )
    {
    TVibeIVTFreqPer freqper( 0 );

    // aPProp can be NULL if the caller doesn't want to retrieve it
    if ( aProp )
        {
        switch ( *( aProp++ ) ) 
            {
            case KVibeIvtPropertyTypeFreqPeriodU16: 
                freqper |= *( aProp++ ) << 8;
                freqper |= *( aProp++ );
                break;
            default:
                {
                // We should never come here.
                break;
                }
            }
        if ( aPProp )
            {
            *aPProp = aProp;
            }
        }
    return freqper;
    }

// ---------------------------------------------------------------------------
// Gets the event offset time from the current location of the 
// data as pointed by the aProp pointer argument.
// This is the generic version that usually is called and that in turn uses
// the above more specific getters.
// ---------------------------------------------------------------------------
//
TInt TimelineIVTGetters::VibeIVTGetEventTimeOffset( const TUint8* aProp )
    {
    // Default return value a time that's guaranteed to never play
    TInt offset( KVibeTimeInfinite );
    if ( aProp )
        {
        // Return the start time depending on the event type
        switch ( *aProp ) 
            {
            case KVibeIvtPropertyTypeLaunchPlayback:
                offset = VibeIVTGetLaunchPlaybackEventTimeOffset( aProp );
                break;
            case KVibeIvtPropertyTypeRepeatBar:
                offset = VibeIVTGetRepeatBarEventTimeOffset( aProp );
                break;
            default:
                {
                // We should never come here.
                break;
                }
            }
        }
    return offset;
    }

// ---------------------------------------------------------------------------
// Gets the override value(s) from the current location of the data as pointed
// by the aProp pointer argument and advances the aPProp argument's pointer 
// value to the next data location
// ---------------------------------------------------------------------------
//
void TimelineIVTGetters::VibeIVTGetOverrides( 
        const TUint8* aProp, 
        TVibeIVTOverrideFlags* aOverrideFlags, 
        TVibeIVTMessageTime* aDuration, 
        TVibeIVTMagnitude* aMagnitude, 
        TVibeIVTFreqPer* aFreqPer, 
        const TUint8** aPProp )
    {
    // aPProp can be NULL if the caller doesn't want to retrieve it
    if ( aProp && aOverrideFlags && aDuration && aMagnitude && aFreqPer )
        {
        *aOverrideFlags = 0;
        while ( VIBE_IVT_IS_OVERRIDE( aProp ) ) 
            {
            switch ( *aProp ) 
                {
                case KVibeIvtPropertyTypeDurationMsU16:
                    *aOverrideFlags |= KVibeIvtOverrideFlagDuration;
                    *aDuration = VibeIVTGetDuration( aProp, &aProp );
                    break;
                case KVibeIvtPropertyTypeMagnitudeS8:
                    *aOverrideFlags |= KVibeIvtOverrideFlagMagnitude;
                    *aMagnitude = VibeIVTGetMagnitude( aProp, &aProp );
                    break;
                case KVibeIvtPropertyTypeFreqPeriodU16:
                    *aOverrideFlags |= KVibeIvtOverrideFlagFreqPeriod;
                    *aFreqPer = VibeIVTGetFreqPer( aProp, &aProp );
                    break;
                default:
                    {
                    // We should never come here.
                    break;
                    }
                }
            }
        if ( aPProp )
            {
            *aPProp = aProp;
            }
        }
    }

// ---------------------------------------------------------------------------
// Gets the launch playback event offset time from the current location of the 
// data as pointed by the aProp pointer argument
// ---------------------------------------------------------------------------
//
TInt TimelineIVTGetters::VibeIVTGetLaunchPlaybackEventTimeOffset( 
        const TUint8* aProp )
    {
    TInt offset( 0 );
    if ( aProp )
        {
        VibeIVTGetEffectID( aProp + 1, &aProp ); // skip the return value
        offset = VibeIVTGetTimeOffset( aProp, 0 );
        }
    return offset;    
    }

// ---------------------------------------------------------------------------
// Gets the repeat event offset time from the current location of the 
// data as pointed by the aProp pointer argument
// ---------------------------------------------------------------------------
//
TInt TimelineIVTGetters::VibeIVTGetRepeatBarEventTimeOffset( 
        const TUint8* aProp )
    {
    return aProp ? VibeIVTGetTimeOffset( aProp + 1, 0 ) : 0;
    }

// end of file
