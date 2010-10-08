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
* Description:  Haptics adaptation test plugin implementation.
*
*/

#include <s32mem.h>
#include <hwrmhapticscommands.h>

#include "hwrmhapticstestplugin.h"
#include "hwrmhapticstrace.h"
#include "hwrmhapticsvibeconstants.h"
#include "plugintimer.h"

// -----------------------------------------------------------------------------
// Enumeration for special test case in which it is tested that HapticsClient 
// correctly converts VibeStatus codes to Symbian/S60 error codes.
// -----------------------------------------------------------------------------
//
enum vibestatustests
    {
    SUCCESS_TEST = 100,
    ALREADY_INITIALIZED_TEST,
    NOT_INITIALIZED_TEST,
    INVALID_ARGUMENT_TEST,
    FAIL_TEST,
    INCOMPATIBLE_EFFECT_TYPE_TEST,
    INCOMPATIBLE_CAPABILITY_TYPE_TEST,
    INCOMPATIBLE_PROPERTY_TYPE_TEST,
    DEVICE_NEEDS_LICENSE_TEST,
    NOT_ENOUGH_MEMORY_TEST,
    SERVICE_NOT_RUNNING_TEST,
    INSUFFICIENT_PRIORITY_TEST,
    SERVICE_BUSY_TEST,
    WRN_NOT_PLAYING_TEST,
    WRN_INSUFFICIENT_PRIORITY_TEST
    };

// -----------------------------------------------------------------------------
// CHWRMHapticsTestPlugin::NewL
// Static instantiation method. Allocates mem and constructs new test plugin object.
// -----------------------------------------------------------------------------
//
CHWRMHapticsTestPlugin* CHWRMHapticsTestPlugin::NewL( 
                                    MHWRMHapticsPluginCallback* aCallback )
    {
    COMPONENT_TRACE( ( _L("CHWRMHapticsTestPlugin::NewL()") ) );
    CHWRMHapticsTestPlugin* self = 
        new ( ELeave ) CHWRMHapticsTestPlugin( aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHWRMHapticsTestPlugin::~CHWRMHapticsTestPlugin
// Destructor
// -----------------------------------------------------------------------------
//
CHWRMHapticsTestPlugin::~CHWRMHapticsTestPlugin()
    {
    COMPONENT_TRACE( ( _L("CHWRMHapticsTestPlugin::~CFmtxPlugin()") ) );
    
    iStorage.Close();
    
    iTimers.ResetAndDestroy();
    iVibeCmdPacket = NULL;
 #ifdef PUBLISH_STATE_INFO
    iCmdProperty.Close();
    iDataProperty.Close();
    iVibeCmdProperty.Close();
#endif /* PUBLISH_STATE_INFO */
    }

// -----------------------------------------------------------------------------
// CHWRMHapticsTestPlugin::CHWRMHapticsTestPlugin
// Constructor
// -----------------------------------------------------------------------------
//
CHWRMHapticsTestPlugin::CHWRMHapticsTestPlugin( 
    MHWRMHapticsPluginCallback* aCallback )
    {
    COMPONENT_TRACE( ( _L("CHWRMHapticsTestPlugin::CHWRMHapticsTestPlugin()") ) );
    iResponseCallback = aCallback;
    }

// -----------------------------------------------------------------------------
// CHWRMHapticsTestPlugin::ConstructL
// Two-phase construction ConstructL that can leave.
// -----------------------------------------------------------------------------
//
void CHWRMHapticsTestPlugin::ConstructL()
    {
    COMPONENT_TRACE( ( _L("CHWRMHapticsTestPlugin::ConstructL") ) );
    
    // Create response data buffer
    iStorage.Create( KHWRMHapticsRespMaxSize );
    
    // Read in response file contents
    RFs fileServer;
    User::LeaveIfError( fileServer.Connect() );
    
    RFile file;
    TInt err = file.Open( fileServer, KStubResponseFile, EFileRead );
    if( err !=KErrNone )
        COMPONENT_TRACE( _L("HWRM HapticsPlugin: Couldn't open file successfully") );
    
    file.Read( iFileData );
    
    file.Close();
    fileServer.Close();
    
#ifdef PUBLISH_STATE_INFO
    RProperty::Define( KPSUidHWResourceNotification, KHWRMTestHapticsCommand, RProperty::EInt );
    RProperty::Define( KPSUidHWResourceNotification, KHWRMTestHapticsDataPckg, RProperty::EByteArray, 512 );
    RProperty::Define( KPSUidHWResourceNotification, KHWRMTestVibCmdData, RProperty::EInt );
    iCmdProperty.Attach( KPSUidHWResourceNotification, KHWRMTestHapticsCommand );
    iDataProperty.Attach( KPSUidHWResourceNotification, KHWRMTestHapticsDataPckg );
    iVibeCmdProperty.Attach( KPSUidHWResourceNotification, KHWRMTestVibCmdData );
#endif /* PUBLISH_STATE_INFO */
    
    // inform haptics of the plugin state
    iResponseCallback->PluginEnabled( EHWRMLogicalActuatorAny, ETrue );
    iResponseCallback->PluginEnabled( EHWRMLogicalActuatorDevice, ETrue );
    }


// -----------------------------------------------------------------------------
// CHWRMHapticsTestPlugin::ProcessCommandL
// Implementation of CHWRMHapticsPluginService::ProcessCommandL. 
// This method processes the haptics command in adaptation layer. In this test
// plugin simply a timer instance is created with the given data and later, after
// the timer expiry, the semi-hard-coded response is created in GenericTimerFired
// method below. 
// -----------------------------------------------------------------------------
//     
void CHWRMHapticsTestPlugin::ProcessCommandL( TInt aCommandId,
                                              TUint8 aTransId,
                                              TDesC8& aData )
    {
    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Processing command: 0x%x, TransId: 0x%x" ), aCommandId, aTransId ) );
    
    TInt retVal( KErrNone );
    
    switch ( aCommandId )
        {
        case HWRMHapticsCommand::EHapticsCmdId :
            {
            COMPONENT_TRACE( _L("HWRM HapticsPlugin: Processed EHapticsCmdId") );
            }
            break;
        default :
            {
            COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Unknown Command: 0x%x" ), aCommandId ) );
#ifdef _DEBUG
            User::Invariant(); // panic here to notice the problem early
#endif
            }
            break;
        }
    
    
    iVibeCmdPacket = reinterpret_cast<TVibePacket*>( const_cast<TUint8*>( aData.Ptr() ) ); 
    // check for NULL pointer
    if ( NULL == iVibeCmdPacket )
        {
        User::Leave( KErrBadDescriptor );
        }
    
    TInt timeout( KTimeOut );
    
    _LIT8 ( KTestPluginMgrTransTimerExpires, "KTestPluginMgrTransTimerExpires" );
    if ( aData.FindF ( KTestPluginMgrTransTimerExpires ) != KErrNotFound )
        {
		COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: set the timeout that is more than plug-in manager transaction generic timer time-out period") ) );
        // set the timeout that is more than plug-in manager transaction generic timer time-out period i.e. > KDefaultPluginTimeout
        timeout = 4000000;
        }

#ifdef PUBLISH_STATE_INFO
    // publish
    iCmdProperty.Set( aCommandId );
    iDataProperty.Set( aData );
    iVibeCmdProperty.Set( iVibeCmdPacket->iSimpleRsp.iCmdCode );
#endif /* PUBLISH_STATE_INFO */

    // create new timer and append it to the timer (pointer) array
    CPluginTimer* timer = CPluginTimer::NewL( timeout, iResponseCallback, aCommandId, aTransId, retVal, this );
    CleanupStack::PushL( timer );
    iTimers.AppendL( timer );
    CleanupStack::Pop( timer );
    
    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Processing command - return") ) );
    }


// -----------------------------------------------------------------------------
// CHWRMHapticsTestPlugin::CancelCommandL
// Implementation of CHWRMHapticsPluginService::CancelCommandL.
// Method for cancelling ongoing (emulated) command execution. In practice 
// removes the timer that was started in order to emulate the given command.
// -----------------------------------------------------------------------------
// 
void CHWRMHapticsTestPlugin::CancelCommandL(
    TUint8 aTransId, 
    TInt /*aCommandId*/ )
    {
    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Cancelling command - iTimers.Count(): %d " ), iTimers.Count() ) );

    for( TInt i = 0; i < iTimers.Count(); i++ )
        {
        if ( iTimers[i]->TransId() == aTransId )
            {
            delete iTimers[i];
            iTimers.Remove( i );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CHWRMHapticsTestPlugin::GenericTimerFired
// Callback method that is called from the CPluginTimer after timer expiry. 
// This method emulates reception of response from the ISA layer to a haptics
// command. The actual response is read from a file, converted to binary and 
// then the Haptics plugin manager's callback method (ProcessResponseL) is 
// called with that binary data.
// -----------------------------------------------------------------------------
// 
void CHWRMHapticsTestPlugin::GenericTimerFired(
    MHWRMHapticsPluginCallback* aService,
    TInt aCommandId,
    TUint8 aTransId,
    TInt aRetVal )
    {
    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: GenericTimerFired (0x%x, 0x%x, %d)" ), aCommandId, aTransId, aRetVal ) );

    __ASSERT_ALWAYS( aService != NULL, User::Invariant() );
    
    TInt err = KErrNone;
    TBuf8<KHWRMHapticsRespMaxSize> binRespData;
    if ( NULL != iVibeCmdPacket )
        {
        TBool jumpCases = EFalse;
        
        // Special case for vibeStatus to S60 err code manipulations:
        if ( iVibeCmdPacket->iSimpleRsp.iCmdCode == KVibeCmdPlayIVTEffectNoData )
            {
            TVibePacketPlayIVTEffectRequest* playReq = reinterpret_cast<TVibePacketPlayIVTEffectRequest*>( iVibeCmdPacket );
            if ( playReq )
                {
                jumpCases = ETrue;
                
                // the repeat value is used to indicate which sub-case is ongoing.. 
                // values between 100...114 are treated as special values.
                switch ( playReq->iRepeat )
                    {
                    case SUCCESS_TEST:
                        {
                        COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Returning status VIBE_S_SUCCESS") ) );
                        this->GetBinRespCode( iFileData.Find( KVibraCmdStatusCaseSuccess ), binRespData );
                        break;
                        }
                    case ALREADY_INITIALIZED_TEST:
                        {
                        COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Returning status VIBE_E_ALREADY_INITIALIZED") ) );
                        this->GetBinRespCode( iFileData.Find( KVibraCmdStatusCaseAlreadyInit ), binRespData );
                        break;
                        }                        
                    case NOT_INITIALIZED_TEST:
                        {
                        COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Returning status VIBE_E_NOT_INITIALIZED") ) );
                        this->GetBinRespCode( iFileData.Find( KVibraCmdStatusCaseNotInit ), binRespData );
                        break;
                        }
                    case INVALID_ARGUMENT_TEST:
                        {
                        COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Returning status VIBE_E_INVALID_ARGUMENT") ) );
                        this->GetBinRespCode( iFileData.Find( KVibraCmdStatusCaseInvalidArg ), binRespData );
                        break;
                        }        
                    case FAIL_TEST:
                        {
                        COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Returning status VIBE_E_FAIL") ) );
                        this->GetBinRespCode( iFileData.Find( KVibraCmdStatusCaseFail ), binRespData );
                        break;
                        }
                    case INCOMPATIBLE_EFFECT_TYPE_TEST:
                        {
                        COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Returning status VIBE_E_INCOMPATIBLE_EFFECT_TYPE") ) );
                        this->GetBinRespCode( iFileData.Find( KVibraCmdStatusCaseIncompEffType ), binRespData );
                        break;
                        }                        
                    case INCOMPATIBLE_CAPABILITY_TYPE_TEST:
                        {
                        COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Returning status VIBE_E_INCOMPATIBLE_CAPABILITY_TYPE") ) );
                        this->GetBinRespCode( iFileData.Find( KVibraCmdStatusCaseIncompCapaType ), binRespData );
                        break;
                        }
                    case INCOMPATIBLE_PROPERTY_TYPE_TEST:
                        {
                        COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Returning status VIBE_E_INCOMPATIBLE_PROPERTY_TYPE") ) );
                        this->GetBinRespCode( iFileData.Find( KVibraCmdStatusCaseIncompPropType ), binRespData );
                        break;
                        }                 
                    case DEVICE_NEEDS_LICENSE_TEST:
                        {
                        COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Returning status VIBE_E_DEVICE_NEEDS_LICENSE") ) );
                        this->GetBinRespCode( iFileData.Find( KVibraCmdStatusCaseDevNeedsLicense ), binRespData );
                        break;
                        }
                    case NOT_ENOUGH_MEMORY_TEST:
                        {
                        COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Returning status VIBE_E_NOT_ENOUGH_MEMORY") ) );
                        this->GetBinRespCode( iFileData.Find( KVibraCmdStatusCaseNotEnoughMem ), binRespData );
                        break;
                        }                        
                    case SERVICE_NOT_RUNNING_TEST:
                        {
                        COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Returning status VIBE_E_SERVICE_NOT_RUNNING") ) );
                        this->GetBinRespCode( iFileData.Find( KVibraCmdStatusCaseSrvNotRunning ), binRespData );
                        break;
                        }
                    case INSUFFICIENT_PRIORITY_TEST:
                        {
                        COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Returning status VIBE_E_INSUFFICIENT_PRIORITY") ) );
                        this->GetBinRespCode( iFileData.Find( KVibraCmdStatusCaseErrInsuffPriority ), binRespData );
                        break;
                        } 
                    case SERVICE_BUSY_TEST:
                        {
                        COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Returning status VIBE_E_SERVICE_BUSY") ) );
                        this->GetBinRespCode( iFileData.Find( KVibraCmdStatusCaseSrvBusy ), binRespData );
                        break;
                        }
                    case WRN_NOT_PLAYING_TEST:
                        {
                        COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Returning status VIBE_W_NOT_PLAYING") ) );
                        this->GetBinRespCode( iFileData.Find( KVibraCmdStatusCaseWrnNotPlaying ), binRespData );
                        break;
                        }                        
                    case WRN_INSUFFICIENT_PRIORITY_TEST:
                        {
                        COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: Returning status VIBE_W_INSUFFICIENT_PRIORITY") ) );
                        this->GetBinRespCode( iFileData.Find( KVibraCmdStatusCaseWrnInsuffPriority ), binRespData );
                        break;
                        }
                    default:
                        {
                        jumpCases = EFalse;
                        break;    
                        }
                    }
                if ( jumpCases )
                    {
                    iEffectState = ENotPlaying; 
                    }
                }
            }
            
        if ( !jumpCases )
            {
            // Other cases..
            switch( iVibeCmdPacket->iSimpleRsp.iCmdCode )
                {
            
                case KVibeCmdInitialize:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_INITIALIZE command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) );    
                    TInt pos = iFileData.Find( KVibraCmdInitialize );
                    iEffectState = ENotPlaying;
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdOpenDevice:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_OPENDEVICE command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) );
                    TInt pos = iFileData.Find( KVibraCmdOpenDevice );
                    iEffectState = ENotPlaying; 
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdSetDeviceProperty:
                    {
                     COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_SETDEVICEPROPERTY command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) );
                     TInt pos = iFileData.Find( KVibraCmdSetDeviceProperty );
                     this->GetBinRespCode( pos, binRespData );
                     break;
                    }
                case KVibeCmdPlayMagSweepEffect:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_PLAYMAGSWEEPEFFECT command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) ); 
                    TInt pos = iFileData.Find( KVibraCmdPlayMagSweepEffect );
                    iEffectState = EPlaying;
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdCloseDevice:
                    {
                     COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_CLOSEDEVICE command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) );
                     TInt pos = iFileData.Find( KVibraCmdCloseDevice );
                     iEffectState = ENotPlaying;
                     this->GetBinRespCode( pos, binRespData );
                     break;
                    }
                case KVibeCmdDeviceCount:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_DEVICE_COUNT command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) ); 
                    TInt pos = iFileData.Find( KVibraCmdDeviceCount );
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdGetDeviceProperty:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_GETDEVICEPROPERTY command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) );
                    TInt pos = 0;
                    switch (iVibeCmdPacket->iGetDevicePropertyReq.iPropertyValueType)
                        {
                        case KVibeValuetypeInt32:
                            pos = iFileData.Find( KVibraCmdGetDevicePropertyInteger );
                            break;
                        case KVibeValuetypeString:
                            pos = iFileData.Find( KVibraCmdGetDevicePropertyString );
                            break;                        
                        case KVibeValuetypeBool:
                            pos = iFileData.Find( KVibraCmdGetDevicePropertyBool );
                            break;                        
                        default:
#ifdef _DEBUG                        
                            User::Invariant(); // wrong value type
#endif                            
                            ;
                        }
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdGetDeviceState:
                    {
                     COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_GETDEVICESTATE command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) );
                     TInt pos = iFileData.Find( KVibraCmdGetDeviceState );
                     this->GetBinRespCode( pos, binRespData );
                     break;
                    }
                case KVibeCmdGetDeviceCapabilities:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_GETDEVICECAPABILITIES command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) ); 
                    TInt pos = 0;
                    switch (iVibeCmdPacket->iGetDeviceCapsReq.iCapabilityValueType)
                        {
                        case KVibeValuetypeInt32:
                            pos = iFileData.Find( KVibraCmdGetDeviceCapabilitiesInteger );
                            break;
                        case KVibeValuetypeString:
                            pos = iFileData.Find( KVibraCmdGetDeviceCapabilitiesString );
                            break;
                        default:
#ifdef _DEBUG                        
                            User::Invariant(); // wrong value type
#endif                            
                            ;
                        }
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdPlayPeriodicEffect:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_PLAYPERIODICEFFECT command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) );
                    TInt pos = iFileData.Find( KVibraCmdPlayPeriodicEffect );
                    iEffectState = EPlaying;
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdPlayIVTEffectIncludeData:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_PLAYIVTEFFECT_INCLUDEIVTDATA command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) ); 
                    TInt pos = iFileData.Find( KVibraCmdPlayIVTEffectIncludeIVTData );
                    iEffectState = EPlaying;
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdPlayIVTEffectNoData:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_PLAYIVTEFFECT_NOIVTDATA command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) );
                    TInt pos = iFileData.Find( KVibraCmdPlayIVTEffectNoIVTData );
                    iEffectState = EPlaying;
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdStopEffect:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_STOPEFFECT command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) );
                    TInt pos = iFileData.Find( KVibraCmdStopEffect );
                    iEffectState = ENotPlaying;
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdStopAllEffects:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_STOPALLEFFECTS command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) ); 
                    TInt pos = iFileData.Find( KVibraCmdStopAllEffects );
                    iEffectState = ENotPlaying;
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdPlayStreamingSample:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_PLAYSTREAMINGSAMPLE command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) );
                    TInt pos = iFileData.Find( KVibraCmdPlayStreamingSample );
                    iEffectState = EPlaying;
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
            
                case KVibeCmdCreateStreamingEffect:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_CREATESTREAMINGEFFECT command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) );
                    TInt pos = iFileData.Find( KVibraCmdCreateStreamingEffect );
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdDestroyStreamingEffect:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_DESTROYSTREAMINGEFFECT command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) ); 
                    TInt pos = iFileData.Find( KVibraCmdDestroyStreamingEffect );
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdModifyPlayingMagSweepEffect:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_MODIFYPLAYINGMAGSWEEPEFFECT command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) );
                    TInt pos = iFileData.Find( KVibraCmdModifyPlayingMagSweepEffect );
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdModifyPlayingPeriodicEffect:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_MODIFYPLAYINGPERIODICEFFECT: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) );
                    TInt pos = iFileData.Find( KVibraCmdModifyPlayingPeriodicEffect );
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdPausePlayingEffect:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_PAUSEPLAYINGEFFECT command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) ); 
                    TInt pos = iFileData.Find( KVibraCmdPausePlayingEffect );
                    iEffectState = EPaused;
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdResumePausedEffect:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_RESUMEPAUSEDEFFECT command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) );
                    TInt pos = iFileData.Find( KVibraCmdResumePausedEffect );
                    iEffectState = EPlaying; 
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
            
                case KVibeCmdGetEffectState:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_GETEFFECTSTATE command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) );
                    TInt pos;
                    if ( iEffectState == ENotPlaying ) 
                        {
                        pos = iFileData.Find( KVibraCmdGetEffectStateNotPlaying );
                        }
                    else if ( iEffectState == EPlaying ) 
                        {   
                        pos = iFileData.Find( KVibraCmdGetEffectStatePlaying );
                        }
                    else
                        {
                        pos = iFileData.Find( KVibraCmdGetEffectStatePaused );
                        }
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdSetKernelParameter:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_SETKERNELPARAMETER command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) ); 
                    TInt pos = iFileData.Find( KVibraCmdSetKernelParameter );
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdGetKernelParameter:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_GETKERNELPARAMETER command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) );
                    TInt pos = iFileData.Find( KVibraCmdGetKernelParameter );
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                case KVibeCmdSetLicense:
                    {
                    COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: VIBEPKT_CMD_SETLICENSE command: 0x%x" ), iVibeCmdPacket->iSimpleRsp.iCmdCode ) );
                    TInt pos = iFileData.Find( KVibraCmdSetLicense );
                    this->GetBinRespCode( pos, binRespData );
                    break;
                    }
                default:
                    {
                    // do panic --> packetized message not recognized
                    break;
                    }    
                }//switch
            }

            CHWRMHapticsRespData* respData;
            TInt32 errorCode( KErrNone );
            respData = CHWRMHapticsRespData::NewLC( errorCode, binRespData );
            RDesWriteStream writer( iStorage );
            CleanupClosePushL( writer );
            // write to resp data
            writer << *respData;
            // remove from stack , it will commit the actual data to the stoarage.
            CleanupStack::PopAndDestroy( &writer ); 
            CleanupStack::PopAndDestroy( respData ); 
            
        } // if block end for pRsp check
    
    
    TRAP( err, aService->ProcessResponseL( aCommandId, aTransId, iStorage ) );               
    // delete obsolete timers
    for( TInt i = ( iTimers.Count()-1 ); i > -1 ; i-- )
        {
        if ( !iTimers[i]->IsActive() )
            {
            delete iTimers[i];
            iTimers.Remove( i );            
            COMPONENT_TRACE( ( _L("HWRM HapticsPlugin: GenericTimerFired - Removed obsolete timer") ) );
            }
        }
    }


// -----------------------------------------------------------------------------
// CHWRMHapticsTestPlugin::Hex2Bin
// Converts hexadecimal string given as first parameter to binary data that is 
// stored to the object referenced by the second parameter.
// -----------------------------------------------------------------------------
// 
void CHWRMHapticsTestPlugin::Hex2Bin( const TDesC8& aSrcBuffer, TDes8& aDstBuffer )
     {
     TInt len = aSrcBuffer.Length();
     aDstBuffer.SetLength( len >> 1 );
     for ( TInt n=0; n<len; n+=2 )
         {
         TUint8 hiNibble = ( (aSrcBuffer[n] > '9' ) ? aSrcBuffer[n] - 'A' + 10: aSrcBuffer[n] - '0' );
         TUint8 loNibble = ( (aSrcBuffer[n+1] > '9' ) ? aSrcBuffer[n+1] - 'A' + 10: aSrcBuffer[n+1] - '0' );
 
         aDstBuffer[n>>1] = static_cast<TUint8>( ( ( hiNibble << 4 ) + loNibble ) );
         }
     }


// -----------------------------------------------------------------------------
// CHWRMHapticsTestPlugin::GetBinRespCode
// Helper method that gets the response data string from given position (line)
// of the (buffered in) response data file, and with the above Hex2Bin method
// converts the string into binary data that is then stored to the TDes8 object
// whose reference is given as second param (aBinRespData).
// -----------------------------------------------------------------------------
// 
void CHWRMHapticsTestPlugin::GetBinRespCode( TInt aCmdPosition, TDes8& aBinRespData )
    {
     TBuf8<KRespFileData> tempFileData;
     TBuf8<KHexRespCode>  hexRespCode;
     
     tempFileData.Copy( iFileData.Mid( aCmdPosition ) );
     TInt pos2 = tempFileData.Locate( '"' );
     TInt pos3 = tempFileData.Locate( '\r' );
     TInt respCodeLength = ( pos3-2 ) - pos2;
     
     hexRespCode.Copy( tempFileData.Mid( pos2+1, respCodeLength ) );
     
     CHWRMHapticsTestPlugin::Hex2Bin( hexRespCode, aBinRespData );     
    }



