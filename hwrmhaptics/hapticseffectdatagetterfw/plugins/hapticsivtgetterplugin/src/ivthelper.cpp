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
*               implementation file.
*/

#include <string.h>
#include <hwrmhaptics.h>

#include "ivtinternal.h"
#include "ivthelper.h"
#include "timelineivtgetters.h"
#include "basisivtgetters.h"

// ---------------------------------------------------------------------------
// Checks the validity of IVT header. I.e., basic sanity check.
// ---------------------------------------------------------------------------
//
TBool IVTHelper::IsValidIVTAddress( const TUint8* aIVT )
    {
    TBool validity( EFalse );
    // For now, all IVT have the following version number: 
    //   major version number = 1, 
    //   minor version number = 0
    if ( aIVT &&
         1 == GetIVTHeader( aIVT )->iVersionMajor &&
         0 == GetIVTHeader( aIVT )->iVersionMinor )
        {
        if ( IsEmpty ( aIVT ) )
            {
            // IVT containing zero effects is considered valid.
            validity = ETrue;
            }
        else
            {
            // For Non empty IVT, StorageDataSubBlock must start with 
            // basic effect or timeline definition.
            if ( VIBE_IVT_IS_BASIS_EFFECT( 
                    GetStorageDataSubBlock( aIVT ) ) ||
                 VIBE_IVT_IS_TIMELINE_EFFECT( 
                    GetStorageDataSubBlock( aIVT ) ) )
                {
                validity = ETrue;
                }
            }
        }
    return validity;
    }

// ---------------------------------------------------------------------------
// Gets the number of contained effect definitions from the effect data buffer.
// For empty data buffer returns 0.
// ---------------------------------------------------------------------------
//
TUint16 IVTHelper::GetNumEffects( const TUint8* aIVT ) 
    {
    return aIVT ? 
        GetIVTHeader( aIVT )->iEffects_0_7 + 
            ( GetIVTHeader( aIVT )->iEffects_8_15 << 8 ) : 0;
    }

// ---------------------------------------------------------------------------
// Gets the effect storage of the given effect (as indicated by index 
// argument) from the effect data buffer. 
// ---------------------------------------------------------------------------
//
TUint8* IVTHelper::GetEffectStorage( const TUint8* aIVT, TInt aEffectIndex ) 
    { 
    // First get the storage data subblock pointer, and then advance that 
    // according to correct effect's storage offset.
    // The offset is stored in two bytes (corresponding to the index of the 
    // effect) of the storage offset subblock data area.
    return ( IsEmpty( aIVT ) || aEffectIndex >= GetNumEffects( aIVT ) ) ? 0 : 
            GetStorageDataSubBlock( aIVT ) 
            + GetStorageOffsetSubBlock( aIVT )[2 * aEffectIndex]
            + ( GetStorageOffsetSubBlock( aIVT )[2 * aEffectIndex + 1] << 8 );
    }

// ---------------------------------------------------------------------------
// Gets the the envelope params (attack and fade params) from the effect data.
//       The effect data pointer must point to a specific effect data, not the
//       whole effect data buffer as in above functions. 
// ---------------------------------------------------------------------------
//   
TBool IVTHelper::ExtractEnvelopeFromPacket( const TUint8* aPacket, 
                                            TVibeEnvelopeParams* aEnvelope )
    {
    TBool status( ETrue );
    if ( aPacket && aEnvelope )
        {
        TVibeIVTMessageTime attackTime( 0 );
        TVibeIVTMessageTime fadeTime( 0 );
        TVibeIVTMagnitude attackLevel( 0 );
        TVibeIVTMagnitude fadeLevel( 0 );

        BasisIVTGetters::VibeIVTGetPeriodicModifier( aPacket,
                                                     &attackTime,
                                                     &fadeTime,
                                                     &attackLevel,
                                                     &fadeLevel );

        aEnvelope->iAttackTime  = attackTime;
        aEnvelope->iFadeTime    = fadeTime;
        aEnvelope->iAttackLevel = ConvertToUserMagnitude( attackLevel );
        aEnvelope->iFadeLevel   = ConvertToUserMagnitude( fadeLevel );
        }
    else
        {
        status = EFalse;
        }
    return status;
    }

// ---------------------------------------------------------------------------
// Gets the the effect definition from the effect data.
//       The effect data pointer must point to a specific effect data, not the
//       whole effect data buffer as in above functions. 
// ---------------------------------------------------------------------------
// 
TBool IVTHelper::ExtractEffectDefinitionFromPacket( 
        const TUint8*          aPacket, 
        TVibeEffectDefinition* aEffectDefinition, 
        TInt*                  aPeriodicType )
    {
    TBool status( ETrue );
    if ( aPacket && aEffectDefinition && aPeriodicType )
        {
        TVibeIVTMessageTime duration( 0 );
        TVibeIVTMessageTime startDelay( 0 );
        TVibeIVTMagnitude   magnitude( 0 );
        TInt                type( 0 );
        TVibeIVTFreqPer     freqPer( 0 );
        TInt                controlMode( 0 );

        BasisIVTGetters::VibeIVTGetPeriodic( aPacket,
                                             &duration,
                                             &startDelay,
                                             &magnitude,
                                             0,
                                             &type,
                                             &freqPer,
                                             &controlMode );

        *aPeriodicType = VIBE_IVT_GET_PERIODIC_TYPE( aPacket );

        if ( KVibePeriodicTypeSquare == *aPeriodicType )
            {
            // Fill the Periodic specific definition data
            aEffectDefinition->iEffectCommonParams.iEffectType = 
                CHWRMHaptics::EHWRMHapticsTypePeriodic;
            aEffectDefinition->iEffectCommonParams.iDuration = 
                ConvertToUserDuration( duration );
            aEffectDefinition->iEffectCommonParams.iControlMode = 
                controlMode;
            aEffectDefinition->iEffectTypeParams.
                iPeriodicEffectDefinition.iType = type;
            aEffectDefinition->iEffectTypeParams.
                iPeriodicEffectDefinition.iPeriod = 
                    ConvertToUserFrequency( freqPer );
            aEffectDefinition->iEffectTypeParams.
                iPeriodicEffectDefinition.iMagnitude = 
                    ConvertToUserMagnitude( magnitude );
            }
        else if ( KVibePeriodicTypeConstant == *aPeriodicType )
            {
            // Fill the MagSweep specific definition data
            aEffectDefinition->iEffectCommonParams.iEffectType = 
                CHWRMHaptics::EHWRMHapticsTypeMagSweep;
            aEffectDefinition->iEffectCommonParams.iDuration = 
                ConvertToUserDuration( duration );
            aEffectDefinition->iEffectCommonParams.iControlMode = 
                controlMode;
            aEffectDefinition->iEffectTypeParams.
                iMagSweepEffectDefinition.iMagnitude = 
                    ConvertToUserMagnitude( magnitude );
            }
        else
            {
            status = EFalse;
            }    
        }
    else
        {
        status = EFalse;
        }    
    return status;
    }

// ---------------------------------------------------------------------------
// Gets the effect name data of the given effect (as indicated by index 
// argument) from the effect data buffer. 
// ---------------------------------------------------------------------------
//
TUint8* IVTHelper::GetEffectNameData( const TUint8* aIVT, TInt aEffectIndex ) 
    {
    // First get the name data subblock pointer, and then advance that
    // according to effect's name offset.
    // The offset is stored in two bytes (corresponding to the index of the 
    // effect) of the name offset subblock data area.
    return HasNameBlock( aIVT ) ?
        ( GetNameDataSubBlock( aIVT ) 
          + GetNameOffsetSubBlock( aIVT )[2 * aEffectIndex]
          + ( GetNameOffsetSubBlock( aIVT )[2 * aEffectIndex + 1] << 8 ) ) :
        0; 
    }

// ---------------------------------------------------------------------------
//  GetIVTEffectDuration()
//
// IVT effect duration computation:
// - For Basis effect: this function extracts duration from the BasicEffect 
//   definition
// - For Timeline effects, this function loops trough all the timeline events 
//   and compute each basic effects's end time and keep the biggest value of
//   them.
//
// The formula:
// BasicEffect's End time = ZeroOffsetTime + EventOffsetTime + 
//    EffectDuration (Effect definition's duration or override duration)
//
// Every time the end of repeat event is reached, "ZeroOffsetTime" is always 
// adjusted (ZeroOffsetTime = ZeroOffsetTime + Repeat duration) and this ´
// function goes back to the next event following the current repeat event.
// 
// Return values:
//   -negative values: errors
//   -Positive values: duration
// ---------------------------------------------------------------------------
//
TInt IVTHelper::GetIVTEffectDuration( const TUint8* aIVT, TInt aEffectIndex )
    {
    TInt status( KErrNone );
    TInt duration( 0 );
    
    if ( aIVT && 0 <= aEffectIndex && aEffectIndex < GetNumEffects( aIVT ) )
        {
        TUint8* effectPtr( 0 );
        TVibeTimeLineInfo timelineInfo;
        memset( &timelineInfo, 0, sizeof( timelineInfo ) );
        
        timelineInfo.iOffsetSubBlock = 
            reinterpret_cast<TUint8*>( GetIVTHeader( aIVT ) + 1 );
        timelineInfo.iDataSubBlock = 
            timelineInfo.iOffsetSubBlock
            + 2*( GetIVTHeader( aIVT )->iEffects_0_7 
                  + ( GetIVTHeader( aIVT )->iEffects_8_15 << 8 ) );
        effectPtr = 
            timelineInfo.iDataSubBlock 
            + timelineInfo.iOffsetSubBlock[2*aEffectIndex] 
            + ( timelineInfo.iOffsetSubBlock[2*aEffectIndex+1] << 8 );

        // Effect type specific handling
        if ( VIBE_IVT_IS_BASIS_EFFECT( effectPtr ) )
            {
            // Basis effect
            // Skip any messages preceding the SetPeriodic message
            while ( KVibeMessageTypeSetPeriodic != 
                    VIBE_IVT_GET_MESSAGE_TYPE( effectPtr ) )
                {
                effectPtr += KVibeIvtMessageSize;
                }
            duration = ConvertToUserDuration( 
                VIBE_IVT_GET_PERIODIC_DURATION( effectPtr ) );
            }
        else if ( VIBE_IVT_IS_TIMELINE_EFFECT( effectPtr ) )
            {
            // Timeline effect
            timelineInfo.iNextEvent = effectPtr;
            timelineInfo.iTimeRepeat = KVibeTimeInfinite;
            timelineInfo.iRepeatBarStackTop = -1;

            // Process the timeline effect
            // The looping stops either:
            //    - due to processing error (e.g. invalid effect data)
            //    - when it has been determined that effect is 'infinite'
            //    - when the whole effect has been processed
            do
            {
            // Process next event or next closing of repeat bar,
            // whichever comes first, provided that its time is up
            if ( timelineInfo.iTimeEvent < timelineInfo.iTimeRepeat )
                {
                status = TimelineDurationProcessEvent( &timelineInfo );
                }
            else
                {
                status = 
                    TimelineDurationProcessRepeatInStack( &timelineInfo );
                }

            // Only continue loop if the above succeeded and didn't result
            // in infinite all effects end value
            if ( KErrNone == status &&
                 KVibeTimeInfinite != timelineInfo.iTimeAllEffectsEnd )
                {
                
                // Check whether there are more events
                if ( timelineInfo.iNextEvent )
                    {
                    // Update times for the next event handling round
                    timelineInfo.iTimeEvent =
                        TimelineIVTGetters::VibeIVTGetEventTimeOffset( 
                            timelineInfo.iNextEvent );
                    }
                else
                    {
                    timelineInfo.iTimeEvent = KVibeTimeInfinite;
                    }
                
                // Check whether there are more repeat bars in stack
                if ( 0 <= timelineInfo.iRepeatBarStackTop )
                    {
                    // Update repeat time value for next repeat bar 
                    // handling round
                    timelineInfo.iTimeRepeat = 
                        timelineInfo.iRepeatBarStack
                            [timelineInfo.iRepeatBarStackTop].iTimeOffsetEnd;
                    }
                else
                    {
                    timelineInfo.iTimeRepeat = KVibeTimeInfinite;
                    }
                }
            } while( KErrNone == status &&
                     KVibeTimeInfinite != timelineInfo.iTimeAllEffectsEnd &&
                     ! ( KVibeTimeInfinite == timelineInfo.iTimeEvent &&
                         KVibeTimeInfinite == timelineInfo.iTimeRepeat ) );

            duration = timelineInfo.iTimeAllEffectsEnd;
            }
        else
            {
            // Invalid effect type
            status = KErrGeneral;
            }
        }
    else
        {
        status = KErrGeneral;
        }
    // return either duration or error status
    return KErrNone == status ? duration : status;
    }

// ---------------------------------------------------------------------------
// Gets the TVibeIVTHeader from the effect data buffer. Basically just a cast
// ---------------------------------------------------------------------------
//
TVibeIVTHeader* IVTHelper::GetIVTHeader( const TUint8* aIVT )
    { 
    return reinterpret_cast<TVibeIVTHeader*>( const_cast<TUint8*>( aIVT ) ); 
    }

// ---------------------------------------------------------------------------
// Returns ETrue if data buffer is either empty or contains zero effects.
// ---------------------------------------------------------------------------
//
TBool IVTHelper::IsEmpty( const TUint8* aIVT )
    {
    // If aIVT is NULL, the GetNumEffects returns 0, i.e., no need
    // to check for NULL here.
    return 0 == GetNumEffects( aIVT ) ? ETrue : EFalse;  
    }

// ---------------------------------------------------------------------------
// Gets the effect storage block size from the effect data buffer.
// ---------------------------------------------------------------------------
//
TUint16 IVTHelper::GetStorageBlockSize( const TUint8* aIVT ) 
    { 
    return IsEmpty( aIVT ) ? 0 : 
        GetIVTHeader( aIVT )->iEffectStorageBlockSize_0_7 + 
        ( GetIVTHeader( aIVT )->iEffectStorageBlockSize_8_15 << 8 ); 
    }

// ---------------------------------------------------------------------------
// Gets the effect storage block size from the effect data buffer.
// ---------------------------------------------------------------------------
//
TUint16 IVTHelper::GetNameBlockSize( const TUint8* aIVT ) 
    { 
    return IsEmpty( aIVT ) ? 0 : 
        GetIVTHeader( aIVT )->iEffectNameBlockSize_0_7 + 
        ( GetIVTHeader( aIVT )->iEffectNameBlockSize_8_15 << 8 );
    }

// ---------------------------------------------------------------------------
// Returns ETrue if data buffer contains name block.
// ---------------------------------------------------------------------------
//
TBool IVTHelper::HasNameBlock( const TUint8* aIVT ) 
    { 
    return 0 != GetNameBlockSize( aIVT ) ? ETrue : EFalse; 
    }

// ---------------------------------------------------------------------------
// Gets the storage block from the effect data buffer.
// ---------------------------------------------------------------------------
//
TUint8* IVTHelper::GetStorageBlock( const TUint8* aIVT ) 
    {
    // Storage block starts after the IVT header
    return IsEmpty( aIVT ) ? 0 : 
        reinterpret_cast<TUint8*>( GetIVTHeader( aIVT ) + 1 ); 
    }

// ---------------------------------------------------------------------------
// Gets the storage offset subblock from the effect data buffer. 
// ---------------------------------------------------------------------------
//
TUint8* IVTHelper::GetStorageOffsetSubBlock( const TUint8* aIVT )
    {
    // The storage offset subblock is in the beginning of the storage block
    return GetStorageBlock( aIVT ); 
    }

// ---------------------------------------------------------------------------
// Gets the storage data subblock from the effect data buffer. 
// ---------------------------------------------------------------------------
//
TUint8* IVTHelper::GetStorageDataSubBlock( const TUint8* aIVT )
    {
    // First get the storage offset subblock start point and then advance
    // over the offsets (there are 2 bytes of offset value per effect)
    return IsEmpty( aIVT ) ? 0 : 
        GetStorageOffsetSubBlock( aIVT ) + 2* GetNumEffects( aIVT ); 
    }

// ---------------------------------------------------------------------------
// Gets the name block from the effect data buffer. 
// ---------------------------------------------------------------------------
//
TUint8* IVTHelper::GetNameBlock( const TUint8* aIVT ) 
    {
    // Name block starts right after the storage block
    return IsEmpty( aIVT ) ? 0 : 
        GetStorageBlock( aIVT ) + GetStorageBlockSize( aIVT ); 
    }

// ---------------------------------------------------------------------------
// Gets the name offset subblock from the effect data buffer. 
// ---------------------------------------------------------------------------
//
TUint8* IVTHelper::GetNameOffsetSubBlock( const TUint8* aIVT ) 
    {
    // Name offset subblock is in the beginning of the name block
    return GetNameBlock( aIVT );
    }

// ---------------------------------------------------------------------------
// Gets the name data subblock from the effect data buffer. 
// ---------------------------------------------------------------------------
//
TUint8* IVTHelper::GetNameDataSubBlock( const TUint8* aIVT ) 
    {
    // First get the name offset subblock start point and then advance
    // over the offsets (there are 2 bytes of offset value per effect)
    return IsEmpty( aIVT ) ? 0 : 
        GetNameOffsetSubBlock( aIVT ) + 2 * GetNumEffects( aIVT ); 
    }


// ---------------------------------------------------------------------------
// Calculates the size of the effect data buffer. 
// ---------------------------------------------------------------------------
//
TUint IVTHelper::GetIVTSize( const TUint8* aIVT)
    {
    TUint size( 0 );
    if ( aIVT )
        {
        size = sizeof( TVibeIVTHeader )
            + GetIVTHeader( aIVT )->iEffectNameBlockSize_0_7
            + ( GetIVTHeader( aIVT )->iEffectNameBlockSize_8_15 << 8 )
            + GetIVTHeader( aIVT )->iEffectStorageBlockSize_0_7
            + ( GetIVTHeader( aIVT )->iEffectStorageBlockSize_8_15 << 8 );
        }
    return size;
    }

// ---------------------------------------------------------------------------
// Converts magnitude from firmware to user interface representation
// ---------------------------------------------------------------------------
//
TInt IVTHelper::ConvertToUserMagnitude( TInt aFwMagnitude )
    {
    return ( aFwMagnitude * KHWRMHapticsMaxMagnitude ) / KVibeMaxFirmwareMagnitude; 
    };
    
// ---------------------------------------------------------------------------
// Converts duration from firmware to user interface representation
// ---------------------------------------------------------------------------
//  
TInt IVTHelper::ConvertToUserDuration( TVibeIVTMessageTime aFwDuration )
    {
    return KVibeFirmwareTimeInfinite == aFwDuration ? KVibeTimeInfinite :
            static_cast<TInt>( aFwDuration );
    };

// ---------------------------------------------------------------------------
// Converts frequency from firmware to user interface representation
// ---------------------------------------------------------------------------
//
TInt IVTHelper::ConvertToUserFrequency( TVibeIVTFreqPer aFwFrequency )
    {
    // map received value to a value between 100 and 1000000
    TInt tmpFreq = aFwFrequency >= 2900 ? 
                    1000000 :
                   aFwFrequency >= 2000 ? 
                    static_cast<TInt>( ( aFwFrequency - 1900 ) * 1000 ) :
                   aFwFrequency >= 1000 ? 
                    static_cast<TInt>( ( aFwFrequency - 900 ) * 100 ) :
                   aFwFrequency > 0 ?
                    static_cast<TInt>( ( aFwFrequency + 10 ) * 10 ) :
                    100;
                    
    return 1000000 / tmpFreq ;
    };

// ---------------------------------------------------------------------------
// Helper function to process timeline events (substructures within timeline
// effects).
// Used by GetIVTEffectDuration in case it is called for timeline effect.
// ---------------------------------------------------------------------------
//
TInt IVTHelper::TimelineDurationProcessEvent( TVibeTimeLineInfo* aTimelineInfo )
    {
    TInt status( KErrNone );

    if ( aTimelineInfo && aTimelineInfo->iNextEvent )
        {
        // pointer for propagating through the timeline info
        const TUint8* propPtr = aTimelineInfo->iNextEvent;

        // event specific handling
        switch ( *propPtr )
            {
            case KVibeIvtPropertyTypeLaunchPlayback:
                // Process launch playback event
                {
                TVibeIVTMessageTime timeLaunchEventDuration( 0 );
                TBool overrides( EFalse );
                TInt effectID = TimelineIVTGetters::VibeIVTGetEffectID( 
                                    propPtr + 1, &propPtr );
                TInt timeOffset = TimelineIVTGetters::VibeIVTGetTimeOffset( 
                                    propPtr, &propPtr );
                
                // loop through overrides and save override duration if found
                while ( VIBE_IVT_IS_OVERRIDE( propPtr ) )
                    {
                    // override-type specific handling
                    switch ( *propPtr )
                        {
                        case KVibeIvtPropertyTypeDurationMsU16:
                            {
                            overrides = ETrue;
                            timeLaunchEventDuration = 
                                TimelineIVTGetters::VibeIVTGetDuration( 
                                    propPtr, &propPtr );   
                            if ( KVibeFirmwareTimeInfinite == 
                                 timeLaunchEventDuration )
                                {
                                aTimelineInfo->iTimeAllEffectsEnd = 
                                    KVibeTimeInfinite;
                                }
                            break;    
                            }
                        case KVibeIvtPropertyTypeMagnitudeS8:
                            {
                            // skip this, following updates propPtr correctly
                            TimelineIVTGetters::VibeIVTGetMagnitude( 
                                propPtr, &propPtr );
                            break;
                            }
                        case KVibeIvtPropertyTypeFreqPeriodU16:
                            {
                            // skip this, following updates propPtr correctly
                            TimelineIVTGetters::VibeIVTGetFreqPer( 
                                propPtr, &propPtr );
                            break;    
                            }    
                        }
                    } // override looping

                // We only need to continue if there was no override 
                // for duration
                if ( !overrides )
                    {
                    // No override for duration. Get the basis effect 
                    // duration instead.
                    // First, get a pointer to the beginning of the effect 
                    // definition of the corresponding effect.
                    TUint8* timelineBasisEffect = 
                        aTimelineInfo->iDataSubBlock 
                        + aTimelineInfo->iOffsetSubBlock[2*effectID] 
                        + ( aTimelineInfo->iOffsetSubBlock[2*effectID+1]
                            << 8 );
                    
                    // Skip any (possible) preceding messages before the
                    // SetPeriodic message
                    while ( KVibeMessageTypeSetPeriodic != 
                            VIBE_IVT_GET_MESSAGE_TYPE( timelineBasisEffect ) )
                        {
                        timelineBasisEffect += KVibeIvtMessageSize;
                        }
                    
                    timeLaunchEventDuration = 
                        VIBE_IVT_GET_PERIODIC_DURATION( timelineBasisEffect );
                        
                    if ( KVibeFirmwareTimeInfinite == 
                         timeLaunchEventDuration )
                        {
                        // Set the all effects end to infinite value
                        aTimelineInfo->iTimeAllEffectsEnd = KVibeTimeInfinite;
                        }
                    }
                    
                if ( KVibeTimeInfinite != aTimelineInfo->iTimeAllEffectsEnd )
                    {
                    // If the all effects end value is not 'infinite',
                    // we need to set that value to current effects end
                    // value (but only if its later than the current all 
                    // effects end value)
                    TInt timeEffectEnd = 
                        aTimelineInfo->iTimeOffsetZero 
                        + ConvertToUserDuration( timeLaunchEventDuration )
                        + timeOffset;
                    if ( timeEffectEnd > aTimelineInfo->iTimeAllEffectsEnd )
                        {
                        aTimelineInfo->iTimeAllEffectsEnd = timeEffectEnd;
                        }
                    
                    // also update the iNextEvent pointer (note this too has
                    // only meaning if the all effects end value is not 
                    // 'infinite'
                    aTimelineInfo->iNextEvent = propPtr;
                    }
                break;
                }
            case KVibeIvtPropertyTypeRepeatBar:
                {
                // Process repeat playback event
                TInt timeOffsetStart = 
                    TimelineIVTGetters::VibeIVTGetTimeOffset( 
                        propPtr + 1, &propPtr );
                TInt timeOffsetEnd = 
                    TimelineIVTGetters::VibeIVTGetTimeOffset( propPtr, &propPtr );
                TInt count = 
                    TimelineIVTGetters::VibeIVTGetCount( propPtr, &propPtr );
                    
                if ( KVibeRepeatCountInfinite == count )
                    {
                    aTimelineInfo->iTimeAllEffectsEnd = KVibeTimeInfinite;
                    }
                else if ( KRepeatBarStackMaxSize > 
                          aTimelineInfo->iRepeatBarStackTop + 1 )
                    {
                    // Push repeat bar to repeat bar stack
                    ++aTimelineInfo->iRepeatBarStackTop;
                    TVibeRepeatBar& tmp =
                        aTimelineInfo->iRepeatBarStack
                            [aTimelineInfo->iRepeatBarStackTop];
                    tmp.iTimeOffsetEnd = timeOffsetEnd;
                    tmp.iTimeDuration = timeOffsetEnd - timeOffsetStart;
                    tmp.iCount = count;
                    tmp.iEvent = propPtr; // First event following this repeat
                                          // event, could be outside the 
                                          // repeat bar

                    // update the iNextEvent pointer
                    aTimelineInfo->iNextEvent = propPtr;
                    }
                else
                    {
                    // More than KHpeRepeatBarStackNum repeat bars in
                    // the timeline. Bail out.
                    status = KErrGeneral;
                    }    
                break;
                }
            case KVibeIvtPropertyTypeEndOfList:
                {
                // End of event list
                aTimelineInfo->iNextEvent = 0;
                break;
                }
            default:
                {
                status = KErrGeneral;
                break;
                }
            }
        }
    else
        {
        status = KErrGeneral;   
        }
    return status;
    }

// ---------------------------------------------------------------------------
// Helper function to process timeline repeat bars that are closing.
// Used by GetIVTEffectDuration in case it is called for timeline effect.
// ---------------------------------------------------------------------------
//
TInt IVTHelper::TimelineDurationProcessRepeatInStack( 
        TVibeTimeLineInfo* aTimelineInfo )
    {
    TInt status( KErrNone );
    
    // sanity check
    if ( aTimelineInfo &&
         0 <= aTimelineInfo->iRepeatBarStackTop &&
         KVibeRepeatCountInfinite != 
            aTimelineInfo->
                iRepeatBarStack[aTimelineInfo->iRepeatBarStackTop].iCount )
        {
        // Processing needed only if count isn't already zero
        if ( 0 < 
             aTimelineInfo->
                iRepeatBarStack[aTimelineInfo->iRepeatBarStackTop].iCount-- )
            {
            // update iNextEvent pointer
            aTimelineInfo->iNextEvent = 
                aTimelineInfo->iRepeatBarStack
                    [aTimelineInfo->iRepeatBarStackTop].iEvent;

            // Update the all effects end value if the end of repeat
            // is later than the current value
            TInt timeRepeat =
                aTimelineInfo->iTimeOffsetZero 
                + aTimelineInfo->
                    iRepeatBarStack[aTimelineInfo->iRepeatBarStackTop].
                        iTimeOffsetEnd;
            if ( timeRepeat > aTimelineInfo->iTimeAllEffectsEnd )
                {
                aTimelineInfo->iTimeAllEffectsEnd = timeRepeat;
                }
            
            // Add the repeat bar duration to the effect start time:
            // repeating the last T time is like adding T to the ZeroTime
            aTimelineInfo->iTimeOffsetZero += 
                aTimelineInfo->iRepeatBarStack
                    [aTimelineInfo->iRepeatBarStackTop].iTimeDuration;
               
            // If count has reached zero, update the repeat bar stack
            // top value (the current top has now been handled)     
            if (0 == aTimelineInfo->iRepeatBarStack
                        [aTimelineInfo->iRepeatBarStackTop].iCount)
                {
                --aTimelineInfo->iRepeatBarStackTop;
                }
            }
        }
    else
        {
        status = KErrGeneral;
        }
    return status;
    }

// end of file

