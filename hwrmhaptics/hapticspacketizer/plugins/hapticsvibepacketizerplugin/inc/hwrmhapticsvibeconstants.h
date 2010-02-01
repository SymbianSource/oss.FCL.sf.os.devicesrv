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
* Description:  VibeTonz related const definitions header file
*
*/

#ifndef _HWRMHAPTICSVIBECONSTANTS_H
#define _HWRMHAPTICSVIBECONSTANTS_H

#include <e32def.h>
#include <limits.h>

/**
 *  Invalid index constant
 */
const TInt KVibeInvalidIndex = -1;

/**
 *  String maximum length constants 
 */
const TInt KVibeMaxEffectNameLength       = 128; // Maximum effect name length
const TInt KVibeMaxDeviceNameLength       = 64;  // Maximum device name length
const TInt KVibeMaxCapabilityStringLength = 64;  // Maximum capability string length
const TInt KVibeMaxPropertyStringLength   = 64;  // Maximum property string length

/**
 * 'Infinite' time 
 */
const TInt KVibeTimeInfinite = INT_MAX;

/**
 *  Device Default priority value 
 */
const TInt KVibeDevicePriorityDefault = 0;

/**
 * 'Infinite' repeat count value
 */
const TUint8 KVibeRepeatCountInfinite = 255;

/**
 * Maximum size of streaming sample (in bytes)
 */
const TInt KVibeMaxStreamingSampleSize = 255;

/**
 * Data value types (used as property and capability data value type)
 */
const TUint8 KVibeValuetypeUnspecified = 0;
const TUint8 KVibeValuetypeInt32       = 1;         
const TUint8 KVibeValuetypeBool        = 2;
const TUint8 KVibeValuetypeString      = 3;

/**
 *  Error and Return value codes.
 */
// Success
const TInt KVibeStatusSuccess = 0;

// Effect is not playing
const TInt KVibeWarningNotPlaying = 1;

// Effect doesn't have enough priority to play,
// higher priority effect is playing on the device
const TInt KVibeWarningInsufficientPriority = 2;

// Effects are disabled on the device
const TInt KVibeWarningEffectsDisabled = 3;

// Trying to resume an effect that is not paused
const TInt KVibeWarningNotPaused = 4;

// The API is already initialized (this code is obsolete)
const TInt KVibeErrAlreadyInitialized = -1;

// The API is not initialized
const TInt KVibeErrNotInitialized = -2;

// Invalid argument was used in a API function call
const TInt KVibeErrInvalidArgument = -3;

// Generic error
const TInt KVibeErrFail = -4;

// Incompatible Effect type has been passed into an API call
const TInt KVibeErrIncompatibleEffectType = -5;

// Incompatible Capability type was used in one of the following API call:
//    -ImmVibeGetDeviceCapabilityString
//    -ImmVibeSetDeviceCapabilityString
//    -ImmVibeGetDeviceCapabilityInt32
//    -ImmVibeGetDeviceCapabilityInt32
const TInt KVibeErrIncompatibleCapabilityType = -6;

// Incompatible Property type was used in on of the following API calls:
//    -ImmVibeGetDevicePropertyString
//    -ImmVibeSetDevicePropertyString
//    -ImmVibeGetDevicePropertyBool
//    -ImmVibeGetDevicePropertyInt32
//    -ImmVibeSetDevicePropertyInt32 
const TInt KVibeErrIncompatiblePropertyType = -7;

// Access to the instance of haptics is locked until a valid license key is provided.
// Use ImmVibeSetDevicePropertyString to set the license key.
// The following API functions may return this error
//    -ImmVibeGetDevicePropertyBool
//    -ImmVibeSetDevicePropertyBool
//    -ImmVibeGetDevicePropertyInt32
//    -ImmVibeSetDevicePropertyInt32
//    -ImmVibeGetDevicePropertyString
//    -ImmVibeSetDevicePropertyString (except for setting the license key)
//    -ImmVibePlayIVTEffect
//    -ImmVibePlayIVTEffectRepeat
//    -ImmVibeStopPlayingEffect
//    -ImmVibePlayMagSweepEffect
//    -ImmVibePlayPeriodicEffect
//    -ImmVibeModifyPlayingMagSweepEffect
//    -ImmVibeModifyPlayingPeriodicEffect
//    -ImmVibeStopAllPlayingEffects
const TInt KVibeErrDeviceNeedsLicense = -8;

// Cannot allocate memory to complete the request
const TInt KVibeErrNotEnoughMemory = -9;

// ImmVibe service is not running
const TInt KVibeErrServiceNotRunning = -10;

// Insufficient priority to complete the request (insufficient license key priority)
const TInt KVibeErrInsufficientPriority = -11;

// ImmVibe service is busy and failed to complete the request
const TInt KVibeErrServiceBusy = -12;

#endif // _HWRMHAPTICSVIBECONSTANTS_H
