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
* Description: Implementation of CSsmCollationObserver class.
*
*/
#include "ssmcollationobserversup.h"
#include "ssmlocalepskeys.h"
#include "ssmmapperutilityinternalpskeys.h"
#include "ssmcommonlocale.h"
#include "trace.h"
#include <centralrepository.h>
#include <CommonEngineDomainCRKeys.h>

// ---------------------------------------------------------------------------
// CSsmCollationObserver::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmUtility* CSsmCollationObserver::NewL()
    {
    FUNC_LOG;
    CSsmCollationObserver* self = new( ELeave ) CSsmCollationObserver();
    return self;
    }

// ---------------------------------------------------------------------------
// CSsmCollationObserver::InitializeL()
// ---------------------------------------------------------------------------
//
void CSsmCollationObserver::InitializeL()
	{

	}

// ---------------------------------------------------------------------------
// CSsmCollationObserver::StartL()
// ---------------------------------------------------------------------------
//
void CSsmCollationObserver::StartL()
	{
	FUNC_LOG;
	TInt errorCode = iCollationProperty.Attach( KPSStarterUid, KSSMCollationPSKey );
	ERROR_2( errorCode, "Failed to attach to property 0x%08x::0x%08x", KPSStarterUid.iUid, KSSMCollationPSKey );
	Activate();
	}

// ---------------------------------------------------------------------------
// CSsmCollationObserver::Release()
// ---------------------------------------------------------------------------
//
void CSsmCollationObserver::Release()
	{
	FUNC_LOG;
	delete this;
	}

// ---------------------------------------------------------------------------
// CSsmCollationObserver::~CSsmCollationObserver()
// ---------------------------------------------------------------------------
//
CSsmCollationObserver::~CSsmCollationObserver()
    {
    FUNC_LOG;
    Cancel();
    iCollationProperty.Close();
    }

// ---------------------------------------------------------------------------
// CSsmCollationObserver::DoCancel()
// ---------------------------------------------------------------------------
//
void CSsmCollationObserver::DoCancel()
    {
    FUNC_LOG;
    iCollationProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CSsmCollationObserver::RunL()
// ---------------------------------------------------------------------------
//
void CSsmCollationObserver::RunL()
    {
    FUNC_LOG;
    const TInt errorCode = iStatus.Int();
    if (KErrNone == errorCode )
        {
        Activate();
        LoadCollation();
        }
    else
    	{
    	INFO_1( "CSsmCollationObserver::RunL()completed with: %d - not reactivating", iStatus.Int() );
    	}
    }

// ---------------------------------------------------------------------------
// CSsmCollationObserver::LoadCollation()
// ---------------------------------------------------------------------------
//
void CSsmCollationObserver::LoadCollation()
    {
    FUNC_LOG;
    TInt collation(0);
    TInt errorCode = iCollationProperty.Get( collation );

    if( KErrNone != errorCode )
        {
        INFO_1( "Failed to get KSSMCollationPSKey value with error  %d", errorCode );
        return;
        }

    // Dot plus three digit locale
    TBuf<KMaxDllExtensionLength> extension; 
    extension.Format( KDllExtensionFormat, collation );

    // Padd ".1" to ".001" for compatibility.   
    for( ; extension.Length() < KMinDllExtensionLength ;) 
        {
        extension.Insert( KDllExtensionPaddingPosition,
                          KDllExtensionPadding );
        }

    TBuf<KMaxDllNameLength> collationDllName( KCollationDllNameBase );
    collationDllName.Append( extension );
    INFO_1( "Loading Collation DLL named '%S'", &collationDllName );

    TExtendedLocale extLocale;
    extLocale.LoadSystemSettings();
    errorCode = extLocale.LoadLocaleAspect( collationDllName );

    if ( KErrNone == errorCode )
        {
        errorCode = extLocale.SaveSystemSettings();
        ERROR( errorCode, "Failed to save locale (Collation) settings" );
        TLocale().Set();
        StoreCollationToCentRep( collation );
        }
    else
        {
        INFO_1( "Failed to load locale (Collation) with error  %d", errorCode );
        }
    }

// ---------------------------------------------------------------------------
// CSsmCollationObserver::StoreCollationToCentRep
// ---------------------------------------------------------------------------
//
void CSsmCollationObserver::StoreCollationToCentRep( const TInt aCollation )
    {
    FUNC_LOG;
    CRepository* cenrep = NULL;
    TRAPD( errorCode, cenrep = CRepository::NewL( KCRUidCommonEngineKeys ) );
    ERROR_1( errorCode, "Failed to initialize cen rep %d", errorCode );
    
    if ( KErrNone == errorCode )
        {
        errorCode = cenrep->Set( KGSCollation, aCollation);
        ERROR_1( errorCode, "Failed to store collation code to CentRep, %d", 
                                                                  errorCode );
        delete cenrep;
        }
    }

// ---------------------------------------------------------------------------
// CSsmCollationObserver::CSsmCollationObserver()
// ---------------------------------------------------------------------------
//
CSsmCollationObserver::CSsmCollationObserver() : CActive( EPriorityStandard )
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CSsmCollationObserver::Activate()
// ---------------------------------------------------------------------------
//
void CSsmCollationObserver::Activate()
    {
	FUNC_LOG;
    ASSERT( !IsActive() );
    iCollationProperty.Subscribe( iStatus );
    SetActive();
    }
