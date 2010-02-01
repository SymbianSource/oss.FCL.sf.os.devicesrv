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
* Name        : starterdomaincrkeys.h
* Part of     : System Startup / Starter
* Interface   : Domain, Startup Status API.
* Domain Central Repository definitions of System Startup subsystem.
* Version     : %version: 1 %
* This material, including documentation and any related computer
* programs, is protected by copyright controlled by Nokia.  All
* rights are reserved.  Copying, including reproducing, storing,
* adapting or translating, any or all of this material requires the
* prior written consent of Nokia.  This material also contains
* confidential information which may not be disclosed to others
* without the prior written consent of Nokia.
* Template version: 4.0
* Nokia Core OS *
*
*/



#ifndef STARTERDOMAINCRKEYS_H
#define STARTERDOMAINCRKEYS_H

#include <centralrepository.h>
#include <ssm/startupreason.h>

/*
* @publishedPartner
* @released
*/
// =============================================================================
// Startup Status API
// =============================================================================
const TUid KCRUidStartup = { 0x101F8762 };

/**
* Used by Starter to store the additional startup reason (language switch, RFS,
* etc.) over boot.
*
* Contains one of the values of TStartupReason enumeration defined in
* startupreason.h.
*/
const TUint32 KStartupReason = 0x00000002;

/**
* Used by Starter to check whether this is the first boot. The value is set by
* Startup Application.
*/
const TUint32 KStartupFirstBoot = 0x00000003;
enum TStartupFirstBoot
    {
    EStartupIsFirstBoot = 0,
    EStartupNotFirstBoot
    };

#endif // STARTERDOMAINCRKEYS_H

// End of File

