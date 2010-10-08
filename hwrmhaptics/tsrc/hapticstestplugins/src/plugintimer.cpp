/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Haptics test adaptation plugin timer implementation
*
*/


#include "plugintimer.h"
#include <hwrmpluginservice.h>
#include <hwrmvibracommands.h>
#include "hwrmhapticstrace.h"

// -----------------------------------------------------------------------------
// CPluginTimer::NewL
// Instantiates a CPluginTimer object.
// Also adds the newly constructed object to active scheduler and starts the 
// embedded CTimer through After() method call.
// -----------------------------------------------------------------------------
//
CPluginTimer* CPluginTimer::NewL(
        const TTimeIntervalMicroSeconds32& aInterval, 
        MHWRMHapticsPluginCallback* aService,
        TInt aCommandId,
        TUint8 aTransId, 
        TInt aRetVal,
        MPluginTimerCallback* aCallback )
    {
    CPluginTimer* self = new( ELeave ) CPluginTimer( 0, aService, aCommandId, aTransId, aRetVal, aCallback );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    CActiveScheduler::Add( self );

    COMPONENT_TRACE( (_L("HWRM Plugin - CPluginTimer::NewL - Setting timer: %d"), aInterval) );

    self->After( aInterval );

    return self;
    }

// -----------------------------------------------------------------------------
// CPluginTimer::~CPluginTimer
// Destructor
// -----------------------------------------------------------------------------
//
CPluginTimer::~CPluginTimer()    
    {
    // PCLint demands
    iCallback = NULL;
    iService = NULL; 
    }

// -----------------------------------------------------------------------------
// CPluginTimer::CPluginTimer
// Constructor
// -----------------------------------------------------------------------------
//
CPluginTimer::CPluginTimer( TInt aPriority, 
                            MHWRMHapticsPluginCallback* aService,
                            TInt aCommandId,
                            TUint8 aTransId,
                            TInt aRetVal,
                            MPluginTimerCallback* aCallback )
    : CTimer( aPriority ),
      iService( aService ),
      iCommandId( aCommandId ),
      iTransId( aTransId ),
      iRetVal( aRetVal ),
      iCallback( aCallback )
    {
    // empty
    }

// -----------------------------------------------------------------------------
// CPluginTimer::ConstructL
// Two-phase (leavable) construction method. Calls the parent class ConstructL.
// -----------------------------------------------------------------------------
//
void CPluginTimer::ConstructL()
    {
    CTimer::ConstructL();
    }

// -----------------------------------------------------------------------------
// CPluginTimer::RunL
// Active scheduler calls RunL when timer expires. This in turn dispatches the 
// further handling to the test stub object through MPluginTimerCallback interface.
// -----------------------------------------------------------------------------
//    
void CPluginTimer::RunL()
    {
    iCallback->GenericTimerFired( iService, iCommandId, iTransId, iRetVal );
    }

// -----------------------------------------------------------------------------
// CPluginTimer::TransId
// Getter method for transaction Id.
// -----------------------------------------------------------------------------
//
TUint8 CPluginTimer::TransId() const
    {
    return  iTransId;
    }

// End of files
