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
* Description:  Haptics test (adaptation) plugin header file.
*
*/


#ifndef __HWRMHAPTICSTESTPLUGIN_H_
#define __HWRMHAPTICSTESTPLUGIN_H_

#include <f32file.h>
#include <HWRMHapticsPluginService.h>
#include <HWRMHapticsCommands.h>
#include "PluginTimer.h"
#include "hwrmhapticsvibepackets.h"

#ifdef PUBLISH_STATE_INFO
#include <e32property.h>
const TUid KPSUidHWResourceNotification = { 0x101F7A04 }; // HWRM private PS Uid
const TUint32 KHWRMTestHapticsCommand   = 0x11110001;
const TUint32 KHWRMTestHapticsDataPckg  = 0x11110002;
const TUint32   KHWRMTestVibCmdData     = 0x11110003;
#endif // PUBLISH_STATE_INFO

#ifdef __WINS__
    _LIT( KStubResponseFile, "C:\\system\\apps\\haptics\\stubresponse.txt" );
#else
    _LIT( KStubResponseFile, "Z:\\system\\apps\\haptics\\stubresponse.txt" );
#endif

_LIT8( KVibraCmdInitialize,                     "VIBEPKT_CMD_INITIALIZE" );
_LIT8( KVibraCmdOpenDevice,                     "VIBEPKT_CMD_OPENDEVICE" );
_LIT8( KVibraCmdSetDeviceProperty,              "VIBEPKT_CMD_SETDEVICEPROPERTY" );
_LIT8( KVibraCmdPlayMagSweepEffect,              "VIBEPKT_CMD_PLAYMAGSWEEPEFFECT" );
_LIT8( KVibraCmdCloseDevice,                     "VIBEPKT_CMD_CLOSEDEVICE" );
_LIT8( KVibraCmdDeviceCount,                     "VIBEPKT_CMD_DEVICE_COUNT" );
_LIT8( KVibraCmdGetDeviceProperty,               "VIBEPKT_CMD_GETDEVICEPROPERTY" );
_LIT8( KVibraCmdGetDevicePropertyInteger,        "VIBEPKT_CMD_GETDEVICEPROPERTY_INTEGER" );
_LIT8( KVibraCmdGetDevicePropertyString,         "VIBEPKT_CMD_GETDEVICEPROPERTY_STRING" );
_LIT8( KVibraCmdGetDevicePropertyBool,           "VIBEPKT_CMD_GETDEVICEPROPERTY_BOOL" );
_LIT8( KVibraCmdGetDeviceState,                  "VIBEPKT_CMD_GETDEVICESTATE" );
_LIT8( KVibraCmdGetDeviceCapabilities,           "VIBEPKT_CMD_GETDEVICECAPABILITIES" );
_LIT8( KVibraCmdGetDeviceCapabilitiesInteger,    "VIBEPKT_CMD_GETDEVICECAPABILITIES_INTEGER" );
_LIT8( KVibraCmdGetDeviceCapabilitiesString,     "VIBEPKT_CMD_GETDEVICECAPABILITIES_STRING" );
_LIT8( KVibraCmdPlayPeriodicEffect,              "VIBEPKT_CMD_PLAYPERIODICEFFECT" );
_LIT8( KVibraCmdPlayIVTEffectIncludeIVTData,     "VIBEPKT_CMD_PLAYIVTEFFECT_INCLUDEIVTDATA" );
_LIT8( KVibraCmdPlayIVTEffectNoIVTData,          "VIBEPKT_CMD_PLAYIVTEFFECT_NOIVTDATA" );
_LIT8( KVibraCmdStopEffect,                      "VIBEPKT_CMD_STOPEFFECT" );
_LIT8( KVibraCmdStopAllEffects,                  "VIBEPKT_CMD_STOPALLEFFECTS" );
_LIT8( KVibraCmdPlayStreamingSample,             "VIBEPKT_CMD_PLAYSTREAMINGSAMPLE" );
_LIT8( KVibraCmdCreateStreamingEffect,           "VIBEPKT_CMD_CREATESTREAMINGEFFECT" );
_LIT8( KVibraCmdDestroyStreamingEffect,          "VIBEPKT_CMD_DESTROYSTREAMINGEFFECT" );
_LIT8( KVibraCmdModifyPlayingMagSweepEffect,     "VIBEPKT_CMD_MODIFYPLAYINGMAGSWEEPEFFECT" );
_LIT8( KVibraCmdModifyPlayingPeriodicEffect,     "VIBEPKT_CMD_MODIFYPLAYINGPERIODICEFFECT" );
_LIT8( KVibraCmdPausePlayingEffect,              "VIBEPKT_CMD_PAUSEPLAYINGEFFECT" );
_LIT8( KVibraCmdResumePausedEffect,              "VIBEPKT_CMD_RESUMEPAUSEDEFFECT" );
_LIT8( KVibraCmdGetEffectStateNotPlaying,        "VIBEPKT_CMD_GETEFFECTSTATE_NOTPLAYING" );
_LIT8( KVibraCmdGetEffectStatePlaying,           "VIBEPKT_CMD_GETEFFECTSTATE_PLAYING" );
_LIT8( KVibraCmdGetEffectStatePaused,            "VIBEPKT_CMD_GETEFFECTSTATE_PAUSED" );
_LIT8( KVibraCmdSetKernelParameter,              "VIBEPKT_CMD_SETKERNELPARAMETER" );
_LIT8( KVibraCmdGetKernelParameter,              "VIBEPKT_CMD_GETKERNELPARAMETER" );
_LIT8( KVibraCmdSetLicense,                      "VIBEPKT_CMD_SETLICENSE" );

// for vibe status to S60 status cases..
_LIT8(KVibraCmdStatusCaseSuccess,               "VIBEPKT_CMD_STATUSCASESUCCESS");
_LIT8(KVibraCmdStatusCaseAlreadyInit,           "VIBEPKT_CMD_STATUSCASEALREADYINIT");
_LIT8(KVibraCmdStatusCaseNotInit,               "VIBEPKT_CMD_STATUSCASENOTINIT");
_LIT8(KVibraCmdStatusCaseInvalidArg,            "VIBEPKT_CMD_STATUSCASEINVALIDARG");
_LIT8(KVibraCmdStatusCaseFail,                  "VIBEPKT_CMD_STATUSCASEFAIL");
_LIT8(KVibraCmdStatusCaseIncompEffType,         "VIBEPKT_CMD_STATUSCASEINCOMPEFFTYPE");
_LIT8(KVibraCmdStatusCaseIncompCapaType,        "VIBEPKT_CMD_STATUSCASEINCOMPCAPATYPE");
_LIT8(KVibraCmdStatusCaseIncompPropType,        "VIBEPKT_CMD_STATUSCASEINCOMPPROPTYPE");
_LIT8(KVibraCmdStatusCaseDevNeedsLicense,       "VIBEPKT_CMD_STATUSCASEDEVNEEDSLICENSE");
_LIT8(KVibraCmdStatusCaseNotEnoughMem,          "VIBEPKT_CMD_STATUSCASENOTENOUGHMEM");
_LIT8(KVibraCmdStatusCaseSrvNotRunning,         "VIBEPKT_CMD_STATUSCASESRVNOTRUNNING");
_LIT8(KVibraCmdStatusCaseErrInsuffPriority,     "VIBEPKT_CMD_STATUSCASEERRINSUFFPRIORITY");
_LIT8(KVibraCmdStatusCaseSrvBusy,               "VIBEPKT_CMD_STATUSCASESRVBUSY");
_LIT8(KVibraCmdStatusCaseWrnNotPlaying,         "VIBEPKT_CMD_STATUSCASEWRNNOTPLAYING");
_LIT8(KVibraCmdStatusCaseWrnInsuffPriority,     "VIBEPKT_CMD_STATUSCASEWRNINSUFFPRIORITY");

const TInt KRespFileData = 3000;
const TInt KHexRespCode  = 100;
const TInt KBinRespCode  = 100;
const TInt KTimeOut      = 500;

class CPluginTimer;

/**
 * Haptics test plugin.
 * Test implementation that handles reception and responding to 
 * haptics commands in adaptation layer in semi-hardcoded manner.
 * (Where semi-hardcoded means that responses are chosen, based on
 * the current state of the test stub, from a file containing hardcoded 
 * response alternatives.
 *
 * @since S60 5.1
 */
class CHWRMHapticsTestPlugin : public CHWRMHapticsPluginService,
                               public MPluginTimerCallback
    {
public:
    /**
     * Static method for instantiating test plugin objects. 
     * Uses two-phase construction.
     *
     * @param aCallback Callback to haptics subsystem.
     */
    static CHWRMHapticsTestPlugin* NewL( MHWRMHapticsPluginCallback* aCallback );
    
    /**
     * Destructor
     */
    virtual ~CHWRMHapticsTestPlugin();

    /**
     * Method to invoke a particular command in the plugin.
     * Implements the CHWRMHapticsPluginService::ProcessCommandL 
     * abstract interface method. 
     *
     * @param aCommandId Command ID
     * @param aTransId   Transaction ID
     * @param aData      Data associated with command
     */
    virtual void ProcessCommandL( TInt aCommandId,
                                  TUint8 aTransId,
                                  TDesC8& aData );

    /**
     * Method to cancel a particular command.
     * Implements the CHWRMHapticsPluginService::CancelCommandL
     * abstract interface method. 
     *
     * @param aTransId   Transaction ID
     * @param aCommandId Command ID to optionally double check with the 
     *                   transaction ID
     */
    virtual void CancelCommandL( TUint8 aTransId, 
                                 TInt aCommandId );

    /**
     * The callback method that is called from CPluginTimer when timer
     * has expired.
     * This method basically does all the processing needed to return a
     * (emulated) response to a Haptics command.
     *
     * @param aService   Pointer to the implementation of 
     *                   MHWRMHapticsPluginCallback callback
     *                   interface whose ProcessResponseL is called
     *                   with (Haptics command's emulated) response
     *                   data.
     * @param aCommandId Id of the (emulated) Haptics command.
     * @param aTransId   Id of the transaction to which the (emulated)
     *                   haptics command belongs to.
     * @param aRetVal    Return value to be inserted to the response
     *                   of the (emulated) Haptics command.
     */
    void GenericTimerFired(MHWRMHapticsPluginCallback* aService,
                           TInt aCommandId,
                           TUint8 aTransId,
                           TInt aRetVal);
    /**
     * Static method to convert hex string to binary format
     *
     * @param aSrcBuffer Reference to a descriptor containing the hex buffer.
     * @param aDstBuffer Reference to a descriptor containing the buffer 
     *                   to which binary data is written.
     */ 
    static void Hex2Bin(const TDesC8& aSrcBuffer, TDes8& aDstBuffer);
       
    /**
     * Method that gets the hex data string from file buffer and converts it to binary format.
     *
     * @param aCmdPosition   Command position in the file data. The command position
     *                       is calculated in GenericTimerFired method based on
     *                       emulated Haptics command's command Id
     * @param aRetvalPackage Reference to a descriptor containing the buffer
     *                       to which binary data is written.
     */
    void CHWRMHapticsTestPlugin::GetBinRespCode(TInt aCmdPosition, TDes8& aRetvalPackage);
          
private:

    /**
     * Constructor.
     *
     * @param aCallback Callback to haptics subsystem.
     */
    CHWRMHapticsTestPlugin( MHWRMHapticsPluginCallback* aCallback );
    
    /**
     * Two-phase construction ConstructL
     */
    void ConstructL();
        

private: // data

    /**
     * Array of pointers to running CPluginTimer timer objects
     */
    RPointerArray<CPluginTimer> iTimers;
        
    /**
     * File data buffer
     */
    TBuf8<KRespFileData>  iFileData;
    
    /**
     * Emulated haptics command's response data
     */
    HWRMHapticsCommand::RHWRMHapticsRespData iStorage;
    
    /**
     * Emulated haptics command (stored as a VibeTonz packet pointer)
     */   
    TVibePacket* iVibeCmdPacket;

    /**
     * Effect state. Used to distinguish (in an emulated way) different 
     * states of effect ("Playing","Paused","Not playing").
     */
    enum TEffectState 
        {
        ENotPlaying = 0,
        EPlaying,
        EPaused
        };
    TEffectState iEffectState;
        
#ifdef PUBLISH_STATE_INFO

    /**
     * P&S property for publishing (emulated) Haptics command's command Id
     */
    RProperty iCmdProperty;
    
    /**
     * P&S property for publishing (emulated) Haptics command's data
     */
    RProperty iDataProperty;
    
    /**
     * P&S property for publishing (emulated) Haptics command's Vibe command code
     */
    RProperty iVibeCmdProperty; 
#endif // PUBLISH_STATE_INFO                
    };

#endif // __HWRMHAPTICSDPLUGIN_H_
