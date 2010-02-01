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
* Name        : startupadaptation.inl
* Part of     : System Startup / Starter
* Interface   : Domain, Startup Adaptation API
* Implementation for inline functions of CStartupAdaptation
* class.
* Version     : %version: ou1s60rt#7 %
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



#include <ecom/ecom.h>

/*
* @file
* @publishedPartner
* @released
*/

/**
* Interface UID for StartupAdaptation interface.
*/
static const TUid KStartupAdaptationIfUid = {0x101F8768};

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CStartupAdaptation::NewL
//
// -----------------------------------------------------------------------------
//
inline CStartupAdaptation* CStartupAdaptation::NewL(
    MStartupAdaptationObserver& aObserver )
    {
    // Set resolve parameters to use in listing available implementations.
    TEComResolverParams resolverParams;
    resolverParams.SetGenericMatch( ETrue );

    // List available implementations.
    RImplInfoPtrArray implInfoArray;
    REComSession::ListImplementationsL(
        KStartupAdaptationIfUid,
        resolverParams,
        KRomOnlyResolverUid,
        implInfoArray );

    // There must be one and only one Startup Adaptation plug-in.
    if ( implInfoArray.Count() < 1 )
        {
        User::Leave( KErrNotFound );
        }
    if ( implInfoArray.Count() > 1 )
        {
        User::Leave( KErrGeneral );
        }

    // Create instance using ECom
    TUid dtorIDKey;
    TAny* ptr = REComSession::CreateImplementationL(
        implInfoArray[ 0 ]->ImplementationUid(),
        dtorIDKey,
        &aObserver );

    implInfoArray.ResetAndDestroy();
    implInfoArray.Close();

    CStartupAdaptation* plugin = reinterpret_cast<CStartupAdaptation*>( ptr );
    plugin->iDtorIDKey = dtorIDKey;

    return plugin;
    }


// Destructor
inline CStartupAdaptation::~CStartupAdaptation()
    {
    REComSession::DestroyedImplementation( iDtorIDKey );
    }
