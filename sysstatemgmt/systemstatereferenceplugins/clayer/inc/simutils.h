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
* Name        : simutils.h
* Part of     : System Startup / Starter
* Interface   : Domain, SIM Utils API
* This file contains SIM Utils definitions of Startup subsystem.
* Version     : %version:  1 %
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

#ifndef __SIMUTILS_H__
#define __SIMUTILS_H__

#include <e32property.h>

/*
* @file
* @publishedPartner
* @released
*/

const TInt KSimUtilsEnumerationFirstValue = 100;

// =============================================================================
// SIM Utils API
// =============================================================================

// Use TUid KPSUidStartup = { 0x101F8766 };

/**
* Indicates the current SIM card status.
*/
const TUint32 KPSSimStatus = 0x00000031;
enum TPSSimStatus
    {
    ESimStatusUninitialized = KSimUtilsEnumerationFirstValue,
    ESimUsable,         // The Sim card is fully usable.
    ESimReadable,       // The SIM card is not fully usable, but the emergency number can be read.
    ESimNotReady,       // The Sim card is present but not ready or usable.
    ESimNotPresent,     // The Sim card is not present.
    ESimNotSupported    // SIM/RUIM is not supported. Some CDMA phones do not support a SIM/RUIM at all.
    };

/**
* Indicates whether the current SIM card is owned.
*/
const TUint32 KPSSimOwned = 0x00000032;
enum TPSSimOwned
    {
    ESimOwnedUninitialized = KSimUtilsEnumerationFirstValue,
    ESimOwned,          // The current Sim card is owned.
    ESimNotOwned        // The current Sim card is not owned.
    };

/**
* Indicates whether the current SIM card is same than the previous one.
*/
const TUint32 KPSSimChanged = 0x00000033;
enum TPSSimChanged
    {
    ESimChangedUninitialized = KSimUtilsEnumerationFirstValue,
    ESimChanged,        // The Sim card has been changed.
    ESimNotChanged      // The Sim card has not been changed.
    };

#endif // __SIMUTILS_H__
