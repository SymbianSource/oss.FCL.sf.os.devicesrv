/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation of SsmLanguageLoader class.
*
*/

#include <e32std.h>
#include <hal.h>

#include "ssmlanguageloader.h"
#include "trace.h"

_LIT( KLocaleDllNameBase, "ELOCL" );
_LIT( KLocaleDllExtensionFormat, ".%u" );
_LIT( KLocaleDllExtensionPadding, "0" );
_LIT( KDefaultLocaleDllNameExtension, ".LOC" );
const TInt KMaxLocaleDllNameLength = 16;
const TInt KMaxLocaleDllExtensionLength = 6;
const TInt KMinLocaleDllExtensionLength = 3;
const TInt KLocaleDllExtensionPaddingPosition = 1;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// SsmLanguageLoader::LoadLanguage
//
// ---------------------------------------------------------------------------
//
TInt SsmLanguageLoader::LoadLanguage( const TInt aLanguage )
    {
    FUNC_LOG;
    INFO_1( "Loading language %d", aLanguage );

    TInt errorCode = StoreLanguageToHal( aLanguage );
    ERROR( errorCode, "Failed to store language code to HAL" );

    errorCode = LoadLocaleDll( aLanguage );
    ERROR( errorCode, "Failed to load locale DLL" );

    return errorCode;
    }


// -----------------------------------------------------------------------------
// SsmLanguageLoader::StoreLanguageToHal
//
// -----------------------------------------------------------------------------
//
TInt SsmLanguageLoader::StoreLanguageToHal( const TInt aLanguage )
    {
    FUNC_LOG;

    TInt errorCode = HAL::Set( HAL::ELanguageIndex, aLanguage );
    return errorCode;
    }


// -----------------------------------------------------------------------------
// SsmLanguageLoader::LoadLocaleDll
//
// -----------------------------------------------------------------------------
//
TInt SsmLanguageLoader::LoadLocaleDll( const TInt aLanguage )
    {
    FUNC_LOG;

    // Dot plus five digit locale
    TBuf<KMaxLocaleDllExtensionLength> extension; 
    extension.Format( KLocaleDllExtensionFormat, aLanguage );
    
    // Padd ".1" to ".01" for compatibility.
    if ( extension.Length() < KMinLocaleDllExtensionLength ) 
        {
        extension.Insert( KLocaleDllExtensionPaddingPosition,
                          KLocaleDllExtensionPadding );
        }
    
    TBuf<KMaxLocaleDllNameLength> localeDllName;
    localeDllName = KLocaleDllNameBase;
    localeDllName.Append( extension );

    INFO_1( "Loading DLL named '%S'", &localeDllName );

    TInt errorCode = ChangeLocale( localeDllName );

    if ( errorCode == KErrNotFound ) // Try default locale
        {
        INFO( "SsmLanguageLoader: Loading default locale" );

        localeDllName = KLocaleDllNameBase;
        localeDllName.Append( KDefaultLocaleDllNameExtension );

        errorCode = ChangeLocale( localeDllName );
        }

    if ( errorCode == KErrNone )
        {
        TLocale().Set();
        }

    return errorCode;
    }


// -----------------------------------------------------------------------------
// SsmLanguageLoader::ChangeLocale
//
// -----------------------------------------------------------------------------
//
TInt SsmLanguageLoader::ChangeLocale( const TDesC& aLocaleDllName )
    {
    FUNC_LOG;

    TExtendedLocale extLocale;
    TInt errorCode = extLocale.LoadLocale( aLocaleDllName );
    ERROR( errorCode, "Failed to load locale" );

    if ( errorCode == KErrNone )
        {
        errorCode = extLocale.SaveSystemSettings();
        ERROR( errorCode, "Failed to set locale" );
        }

    return errorCode;
    }
