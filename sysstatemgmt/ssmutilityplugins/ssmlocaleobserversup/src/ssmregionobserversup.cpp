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
* Description: Implementation of CSsmRegionObserver class.
*
*/
#include "ssmregionobserversup.h"
#include "ssmlocalepskeys.h"
#include "ssmmapperutilityinternalpskeys.h"
#include "ssmcommonlocale.h"
#include "trace.h"
#include <centralrepository.h>
#include <CommonEngineDomainCRKeys.h>

// ---------------------------------------------------------------------------
// CSsmRegionObserver::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmUtility* CSsmRegionObserver::NewL()
    {
    FUNC_LOG;
    CSsmRegionObserver* self = new( ELeave ) CSsmRegionObserver();
    return self;
    }

// ---------------------------------------------------------------------------
// CSsmRegionObserver::InitializeL
// ---------------------------------------------------------------------------
//
void CSsmRegionObserver::InitializeL()
    {

    }

// ---------------------------------------------------------------------------
// CSsmRegionObserver::StartL
// ---------------------------------------------------------------------------
//
void CSsmRegionObserver::StartL()
    {
    FUNC_LOG;
    TInt errorCode = iRegionProperty.Attach( KPSStarterUid, KSSMRegionPSKey );
    ERROR_2( errorCode, "Failed to attach to property 0x%08x::0x%08x", KPSStarterUid.iUid, KSSMRegionPSKey );
    Activate();
    }

// ---------------------------------------------------------------------------
// CSsmRegionObserver::Release
// ---------------------------------------------------------------------------
//
void CSsmRegionObserver::Release()
    {
    FUNC_LOG;
    delete this;
    }

// ---------------------------------------------------------------------------
// CSsmRegionObserver::~CSsmRegionObserver
// ---------------------------------------------------------------------------
//
CSsmRegionObserver::~CSsmRegionObserver()
    {
    FUNC_LOG;
    Cancel();
    iRegionProperty.Close();
    }

// ---------------------------------------------------------------------------
// CSsmRegionObserver::DoCancel
// ---------------------------------------------------------------------------
//
void CSsmRegionObserver::DoCancel()
    {
    FUNC_LOG;
    iRegionProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CSsmRegionObserver::RunL
// ---------------------------------------------------------------------------
//
void CSsmRegionObserver::RunL()
    {
    FUNC_LOG;
    const TInt errorCode = iStatus.Int();
    if ( KErrNone == errorCode )
        {
        Activate();
        LoadRegion();
        }
    else
        {
        INFO_1("CSsmRegionObserver::RunL()completed with: %d - not reactivating", iStatus.Int());
        }
    }

// ---------------------------------------------------------------------------
// CSsmRegionObserver::LoadRegion
// ---------------------------------------------------------------------------
//
void CSsmRegionObserver::LoadRegion()
    {
    TInt region(0);
    TInt errorCode = iRegionProperty.Get( region);
    if( KErrNone != errorCode )
         {
         INFO_1( "Failed to get KSSMRegionPSKey value with error  %d", errorCode );
         return;
         }

    // Dot plus five digit locale
    TBuf<KMaxDllExtensionLength> extension; 
    extension.Format( KDllExtensionFormat, region);
    
    // Padd ".1" to ".01" for compatibility.   
    for( ; extension.Length() < KMinDllExtensionLength ;) 
        {
        extension.Insert( KDllExtensionPaddingPosition,
                          KDllExtensionPadding );
        }

    TBuf<KMaxDllNameLength> regionDllName( KRegionDllNameBase );
    regionDllName.Append( extension );
    INFO_1( "Loading region DLL named '%S'", &regionDllName );

    TExtendedLocale extLocale;
    extLocale.LoadSystemSettings();
    errorCode = extLocale.LoadLocaleAspect(regionDllName );
    
    ERROR( errorCode, "Failed to load region" );

    if ( KErrNone == errorCode )
        {
        errorCode = extLocale.SaveSystemSettings();
        ERROR( errorCode, "Failed to set region" );
        TLocale().Set();
        StoreRegionToCentRep( region );
        }
    }

// ---------------------------------------------------------------------------
// CSsmRegionObserver::StoreRegionToCentRep
// ---------------------------------------------------------------------------
//
void CSsmRegionObserver::StoreRegionToCentRep( const TInt aRegion )
    {
    FUNC_LOG;
    CRepository* cenrep = NULL;
    TRAPD( errorCode, cenrep = CRepository::NewL( KCRUidCommonEngineKeys ) );
    ERROR_1( errorCode, "Failed to initialize cen rep %d", errorCode );
    
    if ( KErrNone == errorCode )
        {
        errorCode = cenrep->Set( KGSRegion, aRegion);
        ERROR_1( errorCode, "Failed to store region code to CentRep, %d", 
                                                                  errorCode );
        delete cenrep;
        }
    }

// ---------------------------------------------------------------------------
// CSsmRegionObserver::CSsmRegionObserver
// ---------------------------------------------------------------------------
//
CSsmRegionObserver::CSsmRegionObserver() : CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CSsmRegionObserver::Activate
// ---------------------------------------------------------------------------
//
void CSsmRegionObserver::Activate()
    {
    FUNC_LOG;
    ASSERT( !IsActive() );
    iRegionProperty.Subscribe( iStatus );
    SetActive();
    }

