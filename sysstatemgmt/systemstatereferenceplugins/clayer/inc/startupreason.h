/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : startupreason.h
* Part of     : System Startup / Starter
* Interface   : Domain, Startup Reason API
* Contains enumeration of valid system startup reasons.
* Version     : %version: 2 %
* This material, including documentation and any related computer
* programs, is protected by copyright controlled by Nokia.  All
* rights are reserved.  Copying, including reproducing, storing,
* adapting or translating, any or all of this material requires the
* prior written consent of Nokia.  This material also contains
* confidential information which may not be disclosed to others
* without the prior written consent of Nokia.
* Template version: 4.1
* Nokia Core OS *
*
*/



#ifndef __STARTUPREASON_H__
#define __STARTUPREASON_H__

/*
* @publishedPartner
* @released
*/

/**
* Enumeration of valid system startup reasons.
* Startup reason indicates why the system was started up.
*/
enum TStartupReason
    {
    ENormalStartup = 100,          // Normal startup. Not a reset.
    ELanguageSwitchReset = 101,    // A reset due to display language switch.
    ENormalRFSReset = 102,         // A reset due to restoring factory settings.
    EDeepRFSReset = 103,           // A reset due to restoring factory settings (deep).
    EFirmwareUpdate = 105,         // A reset due to firmware update
    EUnknownReset = 106,           // A reset due to an error.
    EOperatorSettingReset = 107,   // A reset due to removing operator settings.
    ENetworkModeChangeReset = 108, // A reset due to network mode change.
    ESIMStatusChangeReset = 109,   // A reset due removing/inserting SIM card.
    EDRMReset = 110,
    EDataRestoreReset = 111,       // A reset after restoring backed-up data.
    EFieldTestReset = 112          // A reset required by Field Test software.
    };

#endif // __STARTUPREASON_H__
