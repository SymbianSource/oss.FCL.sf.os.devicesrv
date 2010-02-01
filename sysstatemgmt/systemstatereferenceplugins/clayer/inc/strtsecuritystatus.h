/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : strtsecuritystatus.h
* Part of     : System Startup / Starter
* Interface   : None
* TStrtSecurityStatus enumeration.
* Version     : %version: ou1s60rt#4 %
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



#ifndef __STRTSECURITYSTATUS_H__
#define __STRTSECURITYSTATUS_H__

/*
* @publishedPartner
* @released
*/

enum TStrtSecurityStatus
    {
    EStrtSimlessOffline = 100,
    EStrtSecurityCheckPassed,
    EStrtSecurityCheckFailed
    };

#endif // __STRTSECURITYSTATUS_H__
