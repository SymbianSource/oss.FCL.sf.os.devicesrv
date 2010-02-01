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
* Name        : strtsecuritynotetype.h
* Part of     : System Startup / StrtSecNote
* Definition of TStrtSecurityNoteType enumeration.
* Version     : %version: 1 % << Don't touch! Updated by Synergy at check-out.
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



#ifndef __STRTSECURITYNOTETYPE_H__
#define __STRTSECURITYNOTETYPE_H__

/*
* @publishedPartner
* @released
*/

/**  This enum defines security query and note types */
enum TStrtSecurityNoteType
    {
    ESecNoteNone = 0,
    ESecCodePIN1 = 1,
    ESecCodePIN2 = 2,
    ESecCodeUPIN = 3,
    ESecCodePUK1 = 4,
    ESecCodePUK2 = 5,
    ESecCodeUPUK = 6,
    ESecCodePasswd = 7,
    ESecCodeMasterCode = 8,
    ESecCodeSPC = 9,
    ESecCodeHiddenKey = 10,
    ESecCodeAppPIN1 = 11,
    ESecCodeAppPIN2 = 12,
    ESecInfoSimLockRestrOn = 13,
    ESecInfoSimInvalid = 14,
    ESecNoteTypeLimit
    };

#endif // __STRTSECURITYNOTETYPE_H__
