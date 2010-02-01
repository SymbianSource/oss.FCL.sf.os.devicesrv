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
* Description:  Basis and timeline effects related constant, macro and function
*               prototype definition header file. 
*/

#ifndef _HWRMHAPTICSVIBEIVTINTERNAL_H
#define _HWRMHAPTICSVIBEIVTINTERNAL_H


#include "ivtconstants.h"

/**
 * A few typedefs for internal Vibe (integer) types
 */
 
typedef TUint16 TVibeIVTMessageTime;
typedef TUint16 TVibeIVTEffectID;
typedef TUint8  TVibeIVTOverrideFlags;
typedef TInt8   TVibeIVTMagnitude;
typedef TUint16 TVibeIVTFreqPer;
typedef TUint8  TVibeIVTCount;

/**
 * Number of bytes in a basis effect message
 */
const TInt KVibeIvtMessageSize = 8;

/**
 * Basis effect message types 
 */
const TUint8 KVibeMessageTypeSetPeriodic         = 2;
const TUint8 KVibeMessageTypeSetPeriodicModifier = 3;

/**
 * Periodic type constants. MagSweep effects use KVibePeriodicTypeConstant
 * and Periodic effects KVibePeriodicTypeSquare.
 * Others are reserved for future developement.
 */
const TInt KVibePeriodicTypeConstant         = 0;
const TInt KVibePeriodicTypeSquare           = 1;
const TInt KVibePeriodicTypeTriangle         = 2; // For future dev
const TInt KVibePeriodicTypeSine             = 3; // For future dev
const TInt KVibePeriodicTypeSawtooth         = 4; // For future dev
const TInt KVibePeriodicTypeInvertedSawtooth = 5; // For future dev

/**
 * Timeline effect message property types 
 */
const TUint8 KVibeIvtPropertyTypeLaunchPlayback = 0xf1;
const TUint8 KVibeIvtPropertyTypeRepeatBar      = 0xf2;
const TUint8 KVibeIvtPropertyTypeEndOfList      = 0xff;
const TUint8 KVibeIvtPropertyTypeEffectIdU8     = 0xe0;
const TUint8 KVibeIvtPropertyTypeEffectIdU16    = 0xe1;
const TUint8 KVibeIvtPropertyTypeTimeOffsetU16  = 0xe2;
const TUint8 KVibeIvtPropertyTypeTimeOffsetU24  = 0xe3;
const TUint8 KVibeIvtPropertyTypeTimeOffsetU32  = 0xe4;
const TUint8 KVibeIvtPropertyTypeCountU8        = 0xe5;
const TUint8 KVibeIvtPropertyTypeDurationMsU16  = 0xd0;
const TUint8 KVibeIvtPropertyTypeMagnitudeS8    = 0xd1;
const TUint8 KVibeIvtPropertyTypeFreqPeriodU16  = 0xd2;

/**
 * Override flags
 */
const TUint8 KVibeIvtOverrideFlagDuration   = ( 1 << 0 );
const TUint8 KVibeIvtOverrideFlagMagnitude  = ( 1 << 1 );
const TUint8 KVibeIvtOverrideFlagFreqPeriod = ( 1 << 2 );

/**
 * Style mask
 */
// #define VIBE_STYLE_MASK     0x0F
const TInt KVibeStyleMask = 0x0f;

/**
 * Wave type bit shift value and mask
 */
const TInt KVibeWavetypeShift = 4;
const TInt KVibeWavetypeMask  = 0xf0;

/**
 * Const for repeat bar stack size
 */
const TInt KRepeatBarStackMaxSize = 4;

/**
 * Couple of firmware (IVT player) specific constants
 */
const TInt KVibeFirmwareTimeInfinite = 0xFFFF;
const TInt KVibeMaxFirmwareMagnitude = 127;
/**
 * General macro for extracting bits from buffer (pointed by p) with mask (m)
 * and then shifting right with given shift (s) value
 */ 
#define VIBE_GET_BIT_FIELD(p,m,s) \
    ( ( *( p ) & ( m ) ) >> ( s ) )

/**
 * Basis effect macros for extracting values from IVT buffer 
 */
#define VIBE_IVT_GET_MESSAGE_TYPE(p) \
    VIBE_GET_BIT_FIELD(p, 0xf0, 4)

#define VIBE_IVT_GET_PERIODIC_DURATION(p) \
    ( ( ( ( VIBE_GET_BIT_FIELD( ( p ) + 1, 0xff, 0 ) ) | \
    ( VIBE_GET_BIT_FIELD( ( p ) + 2, 0x0f, 0 ) << 8 ) ) << 4 ) | \
    ( VIBE_GET_BIT_FIELD( ( p ) + 7, 0xf0, 4 ) ) )

#define VIBE_IVT_GET_PERIODIC_START_DELAY(p) \
    ( VIBE_GET_BIT_FIELD( ( p ) + 2, 0xf0, 0 ) | \
    ( VIBE_GET_BIT_FIELD( ( p ) + 3, 0xff, 0 ) << 4 ) )

#define VIBE_IVT_GET_PERIODIC_MAGNITUDE(p) \
    VIBE_GET_BIT_FIELD( ( p ) + 4, 0x7f, 0 )
    
#define VIBE_IVT_GET_PERIODIC_START_FLAG(p) \
    VIBE_GET_BIT_FIELD( ( p ) + 4, 0x80, 7 )

#define VIBE_IVT_GET_PERIODIC_TYPE(p) \
    VIBE_GET_BIT_FIELD( ( p ) + 5, 0x0f, 0 )
    
#define VIBE_IVT_GET_PERIODIC_CONTROLMODE(p) \
    VIBE_GET_BIT_FIELD( ( p ) + 7, 0x0f, 0 )
    
#define VIBE_IVT_GET_PERIODIC_FREQPER(p) \
    ( VIBE_GET_BIT_FIELD( ( p ) + 5, 0xf0, 4 ) | \
    ( VIBE_GET_BIT_FIELD( ( p ) + 6, 0xff, 0 ) << 4 ) )
    
#define VIBE_IVT_GET_PERIODIC_MODIFIER_IMPULSE_TIME(p) \
    ( VIBE_GET_BIT_FIELD( ( p ) + 1, 0xff, 0 ) | \
    ( VIBE_GET_BIT_FIELD( (p ) + 2, 0x3f, 0 ) << 8 ) )
    
#define VIBE_IVT_GET_PERIODIC_MODIFIER_FADE_TIME(p) \
    ( VIBE_GET_BIT_FIELD( ( p ) + 3, 0xff, 0 ) | \
    ( VIBE_GET_BIT_FIELD( ( p ) + 4, 0x3f, 0 ) << 8 ) )
    
#define VIBE_IVT_GET_PERIODIC_MODIFIER_IMPULSE_LEVEL(p) \
    VIBE_GET_BIT_FIELD( ( p ) + 5, 0x7f, 0 )
    
#define VIBE_IVT_GET_PERIODIC_MODIFIER_FADE_LEVEL(p) \
    VIBE_GET_BIT_FIELD( ( p ) + 6, 0x7f, 0 )

/** 
 * Timeline effect specific macro 
 */
#define VIBE_IVT_IS_OVERRIDE(p) \
    ( 0xd0 == ( *( p ) & 0xf0 ) )

/**
 * Macros to determine effect type macros */
#define VIBE_IVT_IS_BASIS_EFFECT(p) \
    ( 0 == ( 0x0f & *( p ) ) )
    
#define VIBE_IVT_IS_TIMELINE_EFFECT(p) \
    ( 0xf0 == ( 0xf0 & *( p ) ) )

/**
 * Structure for IVT header
 */
struct TVibeIVTHeader
    {
    /**
     * IVT file format major version. (In current version: 0x1)
     */
    TUint8 iVersionMajor;

    /**
     * IVT file format major version. (In current version: 0x0)
     */
    TUint8 iVersionMinor;
    
    /**
     * Lower byte of Number of effects (within the IVT data)
     */
    TUint8 iEffects_0_7;
    
    /**
     * Upper byte of Number of effects (within the IVT data)
     */
    TUint8 iEffects_8_15;

    /** 
     * Lower byte of effect storage block size
     */
    TUint8 iEffectStorageBlockSize_0_7;

    /** 
     * Upper byte of effect storage block size
     */
    TUint8 iEffectStorageBlockSize_8_15;
    
    /** 
     * Lower byte of effect name block size
     */
    TUint8 iEffectNameBlockSize_0_7;
    
    /** 
     * Upper byte of effect name block size
     */
    TUint8 iEffectNameBlockSize_8_15;
    };

/**
 * Structure for effect's envelope parameters (attack and fade params)
 */
struct TVibeEnvelopeParams
    {
    /**
     * Magnitude at the start of the envelope
     */
    TInt iAttackLevel;
    
    /**
     * Time to reach the sustain level, in milliseconds
     */
    TInt iAttackTime;
    
    /**
     * Magnitude at the end of the envelope
     */
    TInt iFadeLevel;
    
    /**
     * Time to reach the fade level, in milliseonds
     */
    TInt iFadeTime;
    };

/**
 * Stucture for Periodic effect definition.
 */
struct TVibePeriodicEffectDefinition
    {
    /**
     * Type (bitmasked Wave Type) of the effect
     */
    TInt iType;

    /**
     * Magnitude of the effect
     */
    TInt iMagnitude;

    /**
     * Period of the effect
     */
    TInt iPeriod;
    
    /**
     * Envelope params of the effect
     */
    TVibeEnvelopeParams iEnvelope;
    };

/**
 * Stucture for MagSweep effect definition.
 */                 
struct TVibeMagSweepEffectDefinition
    {
    /**
     * Magnitude of the effect
     */
    TInt iMagnitude;
    
    /**
     * Envelope params of the effect
     */
    TVibeEnvelopeParams iEnvelope;            
    };  

/**
 * Union that contains either Periodic or MagSweep effect definition
 */
union TVibeEffectDefTypeParams
    {
    /**
     * Periodic effect definition
     */
    TVibePeriodicEffectDefinition iPeriodicEffectDefinition;
    
    /**
     * MagSweep effect definition
     */
    TVibeMagSweepEffectDefinition iMagSweepEffectDefinition;
    };

/**
 * Structure for common effect parameters
 */
struct TVibeEffectCommonParams
    {
    /** 
     * Type of the effect
     */
    TInt iEffectType;           

    /**
     * Playing duration of the effect in milliseconds
     */
    TInt iDuration;        

    /** 
     * Control mode (a.k.a. Style) of the effect
     */
    TInt iControlMode;
    };

/**
 * Structure for complete effect definition 
 */
struct TVibeEffectDefinition
    {
    /**
     * Common effect parameters (a structure)
     */
    TVibeEffectCommonParams  iEffectCommonParams;
    
    /**
     * Effect type specific parameters (an union)
     */
    TVibeEffectDefTypeParams iEffectTypeParams;
    };

#endif /* _HWRMHAPTICSVIBEIVTINTERNAL_H */
