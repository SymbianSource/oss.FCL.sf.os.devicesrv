/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CSsmUiLanguageObserver class.
*
*/
#include "ssmuilanguageobserversup.h"
#include "ssmlocalepskeys.h"
#include "ssmmapperutilityinternalpskeys.h"
#include "ssmcommonlocale.h"
#include "trace.h"
#include <centralrepository.h>
#include <CommonEngineDomainCRKeys.h>

// ---------------------------------------------------------------------------
// CSsmUiLanguageObserver::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmUtility* CSsmUiLanguageObserver::NewL()
    {
    FUNC_LOG;
    CSsmUiLanguageObserver* self = new( ELeave ) CSsmUiLanguageObserver();
    return self;
    }

// ---------------------------------------------------------------------------
// CSsmUiLanguageObserver::InitializeL
// ---------------------------------------------------------------------------
//
void CSsmUiLanguageObserver::InitializeL()
	{

	}

// ---------------------------------------------------------------------------
// CSsmUiLanguageObserver::StartL
// ---------------------------------------------------------------------------
//
void CSsmUiLanguageObserver::StartL()
	{
	FUNC_LOG;
	TInt errorCode = iUILanguageProperty.Attach( KPSStarterUid, KSSMUILanguagePSKey );
	ERROR_2( errorCode, "Failed to attach to property 0x%08x::0x%08x", KPSStarterUid.iUid, KSSMUILanguagePSKey );
	Activate();
	}

// ---------------------------------------------------------------------------
// CSsmUiLanguageObserver::Release
// ---------------------------------------------------------------------------
//
void CSsmUiLanguageObserver::Release()
	{
	FUNC_LOG;
	delete this;
	}

// ---------------------------------------------------------------------------
// CSsmUiLanguageObserver::~CSsmUiLanguageObserver
// ---------------------------------------------------------------------------
//
CSsmUiLanguageObserver::~CSsmUiLanguageObserver()
    {
    FUNC_LOG;
    Cancel();
    iUILanguageProperty.Close();
    }

// ---------------------------------------------------------------------------
// CSsmUiLanguageObserver::DoCancel
// ---------------------------------------------------------------------------
//
void CSsmUiLanguageObserver::DoCancel()
    {
    FUNC_LOG;
    iUILanguageProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CSsmUiLanguageObserver::RunL
// ---------------------------------------------------------------------------
//
void CSsmUiLanguageObserver::RunL()
    {
    FUNC_LOG;
    const TInt errorCode = iStatus.Int();
    if (KErrNone == errorCode )
        {
        Activate();
        LoadUILanguage();
        }
    else
    	{
    	INFO_1("CSsmUiLanguageObserver::RunL()completed with: %d - not reactivating", iStatus.Int());
    	}
    }

// ---------------------------------------------------------------------------
// CSsmUiLanguageObserver::LoadUILanguage
// ---------------------------------------------------------------------------
//
void CSsmUiLanguageObserver::LoadUILanguage()
    {
    FUNC_LOG;
    TInt uiLanguage(0);
    TInt errorCode = iUILanguageProperty.Get( uiLanguage );

    if( KErrNone != errorCode )
         {
         INFO_1( "Failed to get KSSMUILanguagePSKey value with error  %d", errorCode );
         return;
         }

    // Dot plus five digit locale
    TBuf<KMaxDllExtensionLength> extension; 
    extension.Format( KDllExtensionFormat, uiLanguage );
    
    // Padd ".1" to ".001" for compatibility.   
    for( ; extension.Length() < KMinDllExtensionLength ;) 
        {
        extension.Insert( KDllExtensionPaddingPosition,
                          KDllExtensionPadding );
        }

    TBuf<KMaxDllNameLength> uiLanguageDllName( KLanguageDllNameBase );
    uiLanguageDllName.Append( extension );
    INFO_1( "Loading UI Language DLL named '%S'", &uiLanguageDllName );

    TExtendedLocale extLocale;
    extLocale.LoadSystemSettings();
    errorCode = extLocale.LoadLocaleAspect( uiLanguageDllName );
    ERROR( errorCode, "Failed to load locale" );

    if ( errorCode == KErrNone )
        {
        errorCode = extLocale.SaveSystemSettings();
        ERROR( errorCode, "Failed to set locale" );
        TLocale().Set();
        StoreUILanguageToCentRep( uiLanguage );
        }
    }

// ---------------------------------------------------------------------------
// CSsmUiLanguageObserver::StoreUILanguageToCentRep
// ---------------------------------------------------------------------------
//
void CSsmUiLanguageObserver::StoreUILanguageToCentRep( const TInt aUILanguage )
    {
    FUNC_LOG;
    CRepository* cenrep = NULL;
    TRAPD( errorCode, cenrep = CRepository::NewL( KCRUidCommonEngineKeys ) );
    ERROR_1( errorCode, "Failed to initialize cen rep %d", errorCode );
    
    if ( KErrNone == errorCode )
        {
        errorCode = cenrep->Set( KGSDisplayTxtLang, aUILanguage);
        ERROR_1( errorCode, "Failed to store UI Language code to CentRep, %d", 
                                                                  errorCode );
        delete cenrep;
        }
    }

// ---------------------------------------------------------------------------
// CSsmUiLanguageObserver::CSsmUiLanguageObserver
// ---------------------------------------------------------------------------
//
CSsmUiLanguageObserver::CSsmUiLanguageObserver() : CActive( EPriorityStandard )
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CSsmUiLanguageObserver::Activate
// ---------------------------------------------------------------------------
//
void CSsmUiLanguageObserver::Activate()
    {
	FUNC_LOG;
    ASSERT( !IsActive() );
    iUILanguageProperty.Subscribe( iStatus );
    SetActive();
    }
