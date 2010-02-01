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

#ifndef _HWRMHAPTICSIVTCONSTANTS_H
#define _HWRMHAPTICSIVTCONSTANTS_H

#include <e32def.h>
#include <limits.h>

/**
 *  Invalid index constant
 */
const TInt KVibeInvalidIndex = -1;

/**
 *  Maximum effect name length constant 
 */
const TInt KVibeMaxEffectNameLength       = 128; // Maximum effect name length

/**
 * 'Infinite' time 
 */
const TInt KVibeTimeInfinite = INT_MAX;

/**
 * 'Infinite' repeat count value
 */
const TUint8 KVibeRepeatCountInfinite = 255;



#endif // _HWRMHAPTICSIVTCONSTANTS_H
