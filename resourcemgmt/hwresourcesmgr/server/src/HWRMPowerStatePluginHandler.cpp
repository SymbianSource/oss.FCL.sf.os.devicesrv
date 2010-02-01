// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//




// INCLUDE FILES
#include <ecom/ecom.h>
#include <centralrepository.h>
#include "HWRMPowerStatePluginHandler.h"
#include "HWRMtrace.h"
#include "HWRMConfiguration.h"
#include "HWRMPrivateCRKeys.h"
#include "HWRMPrivatePSKeys.h"


// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMPluginHandler::CHWRMPluginHandler
// C++ constructor.
// -----------------------------------------------------------------------------
//
CHWRMPowerStatePluginHandler::CHWRMPowerStatePluginHandler(TBool aChargerBlockVibra,
    TBool aChargerBlockFeedbackVibration)
    : iPlugin(NULL),
    iChargerBlockVibra(aChargerBlockVibra),
    iChargerBlockVibraFeedback(aChargerBlockFeedbackVibration)
    {
    COMPONENT_TRACE1(_L("HWRM Server - CHWRMPowerStatePluginHandler::CHWRMPowerStatePluginHandler()") );
    COMPONENT_TRACE1(_L("HWRM Server - CHWRMPowerStatePluginHandler::CHWRMPowerStatePluginHandler - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMPowerStatePluginHandler::ConstructL
// 2nd phase constructor gets plugin instance.
// -----------------------------------------------------------------------------
//
void CHWRMPowerStatePluginHandler::ConstructL()
    {
    COMPONENT_TRACE1(_L("HWRM Server - CHWRMPowerStatePluginHandler::ConstructL()") );

    // Define and attach to the P&S values
    // Battery level
    User::LeaveIfError( RProperty::Define( KPSUidHWRMPowerState,
                                           KHWRMBatteryLevel, RProperty::EInt,
                                           KNoCapability, KHWRMSidOnlyPolicy ) );
    User::LeaveIfError( iBattLevelProp.Attach( KPSUidHWRMPowerState,
                                               KHWRMBatteryLevel ) );
    // Battery status
    User::LeaveIfError( RProperty::Define( KPSUidHWRMPowerState,
                                           KHWRMBatteryStatus, RProperty::EInt,
                                           KNoCapability, KHWRMSidOnlyPolicy ) );
    User::LeaveIfError( iBattStatusProp.Attach( KPSUidHWRMPowerState,
                                                KHWRMBatteryStatus ) );
    // Charging status
    User::LeaveIfError( RProperty::Define( KPSUidHWRMPowerState,
                                           KHWRMChargingStatus, RProperty::EInt,
                                           KNoCapability, KHWRMSidOnlyPolicy ) );
    User::LeaveIfError( iChargingStatusProp.Attach( KPSUidHWRMPowerState,
                                                    KHWRMChargingStatus ) );
    
    // Get plugin instance
    iPlugin = CHWRMPowerStatePlugin::NewL( this );

    COMPONENT_TRACE1(_L("HWRM Server - CHWRMPowerStatePluginHandler::ConstructL - return") );
    }

// -----------------------------------------------------------------------------
// CHWRMPowerStatePluginHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
// 
CHWRMPowerStatePluginHandler* CHWRMPowerStatePluginHandler::NewL(TBool aChargerBlockVibra,
    TBool aChargerBlockFeedbackVibration)
    {
    COMPONENT_TRACE1(_L("HWRM Server - CHWRMPowerStatePluginHandler::NewL()") );
    
    CHWRMPowerStatePluginHandler* self = new( ELeave ) 
        CHWRMPowerStatePluginHandler(aChargerBlockVibra, aChargerBlockFeedbackVibration);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    COMPONENT_TRACE2(_L("HWRM Server - CHWRMPowerStatePluginHandler::NewL - return 0x%x"), self );

    return self;
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMPowerStatePluginHandler::~CHWRMPowerStatePluginHandler()
    {
    COMPONENT_TRACE1(_L("HWRM Server - CHWRMPowerStatePluginHandler::~CHWRMPowerStatePluginHandler()") );
    
    // delete the plugin
    delete iPlugin;

    // close the P&S properties
    iBattLevelProp.Close();
    iBattStatusProp.Close();
    iChargingStatusProp.Close();

    COMPONENT_TRACE1(_L("HWRM Server - CHWRMPowerStatePluginHandler::~CHWRMPowerStatePluginHandler - return ") );
    }

// -----------------------------------------------------------------------------
// CHWRMPowerStatePluginHandler::NotifyStateChange
// Implementation of power state callback method.
// -----------------------------------------------------------------------------
//
void CHWRMPowerStatePluginHandler::NotifyStateChange( const TUint32 aKey,
                                                      TInt aValue )
    {
    COMPONENT_TRACE3(_L("HWRM Server - CHWRMPowerStatePluginHandler::NotifyStateChangeL(%d, %d)"), aKey, aValue );

    // update P&S key with the new values
    switch( aKey )
        {
        case KHWRMBatteryLevel:
            {
            COMPONENT_TRACE2(_L("HWRM Server - KHWRMBatteryLevel key value=%d"), aValue );
            iBattLevelProp.Set( aValue );
            break;
            }
        case KHWRMBatteryStatus:
            {
            COMPONENT_TRACE2(_L("HWRM Server - KHWRMBatteryStatus key value=%d"), aValue );
            iBattStatusProp.Set( aValue );
            break;
            }
        case KHWRMChargingStatus:
            {
            COMPONENT_TRACE2(_L("HWRM Server - KHWRMChargingStatus key value=%d"), aValue );
            iChargingStatusProp.Set( aValue );                        
            if ( iChargerBlockVibra || iChargerBlockVibraFeedback )
                {
                if ( aValue == EChargingStatusCharging ||
                     aValue == EChargingStatusNotCharging ||
                     aValue == EChargingStatusAlmostComplete ||
                     aValue == EChargingStatusChargingComplete ||
                     aValue == EChargingStatusChargingContinued )
                    {
                    RProperty::Set(KPSUidHWRMPrivate, KHWRMInternalVibraBlocked, ETrue);
                    }
                else
                    {
                    RProperty::Set(KPSUidHWRMPrivate, KHWRMInternalVibraBlocked, EFalse);
                    }
                }
            break;
            }
        default:
            break;
        }
    
    COMPONENT_TRACE1(_L("HWRM Server - CHWRMPowerStatePluginHandler::NotifyStateChangeL - return") );
    }


//  End of File  
