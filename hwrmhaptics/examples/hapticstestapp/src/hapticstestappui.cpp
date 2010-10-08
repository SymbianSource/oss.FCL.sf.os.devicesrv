/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  AppUi class implementation.
*
*/


#include <avkon.hrh>
#include <e32math.h>
#include <eikmenup.h>
#include <aknlistquerydialog.h>
#include <hapticstest.rsg>
#include <utf.h>

#include "hapticstest.pan"
#include "hapticstestappui.h"
#include "hapticstestappview.h"
#include "hapticstesteffectdatahandler.h"
#include "hapticstest.hrh"

_LIT( KSessionPathMem, "C:\\Data\\Others\\" );
_LIT( KSessionPathCrd, "E:\\Others\\" );
_LIT( KIVTFileType, "*.ivt" );

const TInt KSampleCount = 8;

static const TUint8 KStreamArray[KSampleCount][10] =
    {
    0x00, 0x00, 0x20, 0x13, 0x00, 0x00, 0x7f, 0x01, 0x5d, 0x41,
    0x29, 0x00, 0x20, 0x08, 0x00, 0x00, 0x7f, 0xc1, 0x4d, 0x42,           
    0x51, 0x00, 0x20, 0x08, 0x00, 0x00, 0x7f, 0x01, 0x5d, 0xc1,
    0x65, 0x00, 0x20, 0x07, 0x00, 0x00, 0x7f, 0x01, 0x5d, 0xd1,
    0x7a, 0x00, 0x20, 0x08, 0x00, 0x00, 0x7f, 0xc1, 0x4d, 0x42,
    0x8e, 0x00, 0x20, 0x16, 0x00, 0x00, 0x7f, 0x01, 0x5d, 0x01,
    0xa2, 0x00, 0x20, 0x08, 0x00, 0x00, 0x7f, 0xc1, 0x4d, 0x42,
    0x06, 0x00, 0x20, 0x08, 0x00, 0x00, 0x7f, 0xc1, 0x4d, 0x42    
    };




// ---------------------------------------------------------
// ConstructL is called by the application framework
// ---------------------------------------------------------
//
void CHapticsTestAppUi::ConstructL()
    {
    BaseConstructL();

    // create appview
    iAppView = CHapticsTestAppView::NewL( ClientRect() );    
    AddToStackL( iAppView );

    // create effect data handler
    iEffectData = CHapticsTestEffectDataHandler::NewL();

    // create async play sender
    iAsyncPlaySender = new ( ELeave ) CHapticsTestAsyncPlaySender( this );
    
    FindIVTFiles();

    iAppView->InsertOutput( _L("Haptics usage from options menu.") );
    }

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
//
CHapticsTestAppUi::CHapticsTestAppUi()
    : iSynchronous( ETrue ), iUseHandle( EFalse ), iUsesMemCard( EFalse )
    {
    // no implementation required
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
//
CHapticsTestAppUi::~CHapticsTestAppUi()
    {
    if (iAppView)
        {
        RemoveFromStack( iAppView );
        delete iAppView;
        iAppView = NULL;
        }

    if( iHaptics )
        {
        delete iHaptics;
        iHaptics = NULL;
        }
    
    if ( iTempHaptics )
        {
        delete iTempHaptics;
        iTempHaptics = NULL;
        }
    
    delete iEffectData;

    iIVTFileArray.Close();

    while ( iLoadedIVTFileArray.Count() )
        {
        delete iLoadedIVTFileArray[0].iDataBuffer;
        iLoadedIVTFileArray.Remove( 0 );
        }
    
    iLoadedIVTFileArray.Close();

    if ( iAsyncPlaySender )
        {
        iAsyncPlaySender->Cancel();
        delete iAsyncPlaySender;
        iAsyncPlaySender = NULL;
        }
    }

// ---------------------------------------------------------
// handle any menu commands
// ---------------------------------------------------------
//
void CHapticsTestAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        case EHapticsTestOpenHaptics:
            {
            OpenHapticsL();
            }
            break;

        case EHapticsTestAutoInit:
            {
            // automatically create haptics instance, fetch supported
            // actuators, and open Any-actuator
            OpenHapticsL();
            FetchSupportedActuators();
            OpenActuator( EHWRMLogicalActuatorAny );
            }
            break;

        case EHapticsTestSupportedActuators:
            {
            FetchSupportedActuators();
            }
            break;

        case EHapticsTestOpenActuator:
        case EHapticsTestOpenActuatorAny:
            {
            OpenActuator( EHWRMLogicalActuatorAny );
            }
            break;
        case EHapticsTestOpenActuatorDevice:
            {
            OpenActuator( EHWRMLogicalActuatorDevice );
            }
            break;
        case EHapticsTestOpenActuatorPrimaryDisplay:
            {
            OpenActuator( EHWRMLogicalActuatorPrimaryDisplay );
            }
            break;
        case EHapticsTestOpenActuatorSecondaryDisplay:
            {
            OpenActuator( EHWRMLogicalActuatorSecondaryDisplay );
            }
            break;
        case EHapticsTestOpenActuatorGame:
            {
            OpenActuator( EHWRMLogicalActuatorGame );
            }
            break;
        case EHapticsTestOpenActuatorGameLeft:
            {
            OpenActuator( EHWRMLogicalActuatorGameLeft );
            }
            break;
        case EHapticsTestOpenActuatorGameRight:
            {
            OpenActuator( EHWRMLogicalActuatorGameRight );
            }
            break;
        case EHapticsTestOpenActuatorExternalVibra:
            {
            OpenActuator( EHWRMLogicalActuatorExternalVibra );
            }
            break;
        
        case EHapticsTestLoadIVTFile:
        case EHapticsTestLoadIVTFileNoneFound:
            {
            // nothing to do
            }
            break;
        case EHapticsTestLoadIVTFileSelected:
            {
            // open selected file
            TInt index = iMenuPane->SelectedItem();
            LoadIVTDataL( iIVTFileArray[index] );
            }
            break;
        
        case EHapticsTestPlayEffect:
        case EHapticsTestPlayEffectNoneFound:
            {
            // nothing to do
            }
            break;
        case EHapticsTestPlayEffectSelected:
            {
            // play selected effect
            TInt index = CountFileHandleAndEffectIndex( iMenuPane->SelectedItem() );
            PlayEffect( index );
            PrintEffectInfo( index );
            }
            break;
        case EHapticsTestPlayEffectRepeat:
        case EHapticsTestPlayEffectRepeatNoneFound:
            {
            // nothing to do
            }
            break;        
        case EHapticsTestPlayEffectRepeatSelected:
            {
            // play selected effect repeatedly
            TInt index = CountFileHandleAndEffectIndex( iMenuPane->SelectedItem() );
            RepeatEffect( index, GetNumberOfRepeats() );
            PrintEffectInfo( index );
            }
            break;
    
        case EHapticsTestPlayEffectManual:
            {
            // nothing to do
            }
            break;
        case EHapticsTestPlayEffectManualMagSweep:
            {
            PlayMagSweepL();
            }
            break;
        case EHapticsTestPlayEffectManualMagSweepInf:
            {
            PlayMagSweepL( ETrue );
            }
            break;
        case EHapticsTestPlayEffectManualMagSweepInfMod:
            {
            PlayMagSweepL( ETrue, ETrue );
            }
            break;
        case EHapticsTestPlayEffectManualPeriodic:
            {
            PlayPeriodicL();
            }
            break;
        case EHapticsTestPlayEffectManualPeriodicInf:
            {
            PlayPeriodicL( ETrue );
            }
            break;
        case EHapticsTestPlayEffectManualPeriodicInfMod:
            {
            PlayPeriodicL( ETrue, ETrue );
            }
            break;
        case EHapticsTestPlayEffectManualRepeat:
            {
            PlayRepeatL( GetNumberOfRepeats() );
            }
            break;

        case EHapticsTestChangeState:
        case EHapticsTestChangeStateNoneFound:
            {
            // nothing to do
            }
            break;

        case EHapticsTestChangeStatePause:
            {
            // get itemindex and pause effect
            TInt itemIndex = iMenuPane->SelectedItem();
            PauseEffect( itemIndex );
            }
            break;

        case EHapticsTestChangeStateResume:
            {
            // get itemindex and resume effect
            TInt itemIndex = iMenuPane->SelectedItem();
            ResumeEffect( itemIndex );
            }
            break;

        case EHapticsTestChangeStateStop:
            {
            // get itemindex and stop effect
            TInt itemIndex = iMenuPane->SelectedItem();
            StopEffect( itemIndex );
            }
            break;

        case EHapticsTestChangeStateStopAll:
            {
            TInt err = iHaptics->StopAllPlayingEffects();
            if ( err )
                {
                TBuf<8> errBuf;
                errBuf.AppendNum( err );
                iAppView->InsertOutput( _L("Stopping all effects failed, err = "), EFalse );
                iAppView->InsertOutput( errBuf );
                }
            else
                {
                iAppView->InsertOutput( _L("Stopping all effects succeeded.") );
                
                // reset the auto-modification
                iEffectData->ResetModifiableEffectTimer();
                }
            }
            break;

        case EHapticsTestModifyEffect:
            {
            // nothing to do
            }
            break;
        case EHapticsTestModifyEffectDuration1:
            {
            iEffectData->SetDuration( KEffectDuration1 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectDuration5:
            {
            iEffectData->SetDuration( KEffectDuration5 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectDuration10:
            {
            iEffectData->SetDuration( KEffectDuration10 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectDuration30:
            {
            iEffectData->SetDuration( KEffectDuration30 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectDuration60:
            {
            iEffectData->SetDuration( KEffectDuration60 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectMagnitudeMin:
            {
            iEffectData->SetMagnitude( KHWRMHapticsMinMagnitude );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectMagnitude25:
            {
            iEffectData->SetMagnitude( KEffectMagnitude25 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectMagnitude50:
            {
            iEffectData->SetMagnitude( KEffectMagnitude50 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectMagnitude75:
            {
            iEffectData->SetMagnitude( KEffectMagnitude75 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectMagnitudeMax:
            {
            iEffectData->SetMagnitude( KHWRMHapticsMaxMagnitude );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectStyleSmooth:
            {
            iEffectData->SetStyle( CHWRMHaptics::EHWRMHapticsStyleSmooth );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectStyleStrong:
            {
            iEffectData->SetStyle( CHWRMHaptics::EHWRMHapticsStyleStrong );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectStyleSharp:
            {
            iEffectData->SetStyle( CHWRMHaptics::EHWRMHapticsStyleSharp );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectAttackMin:
            {
            iEffectData->SetAttackLevel( KHWRMHapticsMinMagnitude );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectAttack25:
            {
            iEffectData->SetAttackLevel( KEffectMagnitude25 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectAttack50:
            {
            iEffectData->SetAttackLevel( KEffectMagnitude50 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectAttack75:
            {
            iEffectData->SetAttackLevel( KEffectMagnitude75 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectAttackMax:
            {
            iEffectData->SetAttackLevel( KHWRMHapticsMaxMagnitude );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectFadeMin:
            {
            iEffectData->SetFadeLevel( KHWRMHapticsMinMagnitude );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectFade25:
            {
            iEffectData->SetFadeLevel( KEffectMagnitude25 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectFade50:
            {
            iEffectData->SetFadeLevel( KEffectMagnitude50 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectFade75:
            {
            iEffectData->SetFadeLevel( KEffectMagnitude75 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectFadeMax:
            {
            iEffectData->SetFadeLevel( KHWRMHapticsMaxMagnitude );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectAttackTime02:
            {
            iEffectData->SetAttackTime( KEffectAttackTime02 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectAttackTime1:
            {
            iEffectData->SetAttackTime( KEffectAttackTime1 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectAttackTime5:
            {
            iEffectData->SetAttackTime( KEffectAttackTime5 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectAttackTime10:
            {
            iEffectData->SetAttackTime( KEffectAttackTime10 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectAttackTime30:
            {
            iEffectData->SetAttackTime( KEffectAttackTime30 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectAttackTime60:
            {
            iEffectData->SetAttackTime( KEffectAttackTime60 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectFadeTime02:
            {
            iEffectData->SetFadeTime( KEffectFadeTime02 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectFadeTime1:
            {
            iEffectData->SetFadeTime( KEffectFadeTime1 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectFadeTime5:
            {
            iEffectData->SetFadeTime( KEffectFadeTime5 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectFadeTime10:
            {
            iEffectData->SetFadeTime( KEffectFadeTime10 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectFadeTime30:
            {
            iEffectData->SetFadeTime( KEffectFadeTime30 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectFadeTime60:
            {
            iEffectData->SetFadeTime( KEffectFadeTime60 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectPeriod50:
            {
            iEffectData->SetPeriod( KEffectPeriod50 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectPeriod100:
            {
            iEffectData->SetPeriod( KEffectPeriod100 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectPeriod300:
            {
            iEffectData->SetPeriod( KEffectPeriod300 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectPeriod600:
            {
            iEffectData->SetPeriod( KEffectPeriod600 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectPeriod1000:
            {
            iEffectData->SetPeriod( KEffectPeriod1000 );
            ModifyEffect();
            }
            break;
        case EHapticsTestModifyEffectPeriod3000:
            {
            iEffectData->SetPeriod( KEffectPeriod3000 );
            ModifyEffect();
            }
            break;
        
        case EHapticsTestCloseHaptics:
            {
            CloseHaptics();
            }
            break;

        case EHapticsTestSynchronousCalls:
            {
            iSynchronous = ETrue;
            iAppView->InsertOutput( _L("Using synchronous methods.") );
            }
            break;

        case EHapticsTestAsynchronousCalls:
            {
            iSynchronous = EFalse;
            iAppView->InsertOutput( _L("Using asynchronous methods.") );
            }
            break;

        case EHapticsTestUsingFileHandle:
            {
            iUseHandle = ETrue;
            iAppView->InsertOutput( _L("Using file handle in play methods.") );
            }
            break;

        case EHapticsTestNotUsingFileHandle:
            {
            iUseHandle = EFalse;
            iAppView->InsertOutput( _L("Using data buffer in play methods.") );
            }
            break;

        case EHapticsTestShowEffectInfo:
            {
            iShowEffectInfo = ETrue;
            iAppView->InsertOutput( _L("Shows information of played effect.") );
            }
            break;

        case EHapticsTestHideEffectInfo:
            {
            iShowEffectInfo = EFalse;
            iAppView->InsertOutput( _L("Stops showing information of played effect.") );
            }
            break;

        case EHapticsTestDeleteIVTData:
        case EHapticsTestDeleteIVTDataNoneFound:
            {
            // nothing to do    
            }
            break;

        case EHapticsTestDeleteIVTDataSelected:
            {
            // delete selected loaded IVT file
            TInt index = iMenuPane->SelectedItem();
            DeleteLoadedIVTData( index );
            }
            break;
            
        case EHapticsTestDeleteAllIVTData:
            {
            // delete all loaded IVT files
            DeleteAllLoadedIVTData();    
            }
            break;

        case EHapticsTestReserve:
            {
            // nothing to do
            }
        case EHapticsTestReserveNotTrusted:
            {
            ReserveHaptics( iHaptics, EFalse );
            }
            break;
        case EHapticsTestReserveTrusted:
            {
            ReserveHaptics( iHaptics, ETrue );
            }
            break;
        case EHapticsTestReserveNotTrustedTemp:
            {
            // create the temp haptics client, if it does not exist
            if ( !iTempHaptics )
                {
                iTempHaptics = CHWRMHaptics::NewL( NULL, NULL );
                }

            ReserveHaptics( iTempHaptics, EFalse );
            }
            break;
        case EHapticsTestRelease:
            {
            iHaptics->ReleaseHaptics();
            iAppView->InsertOutput( _L("Reservation released.") );
            }
            break;
        case EHapticsTestReleaseTemp:
            {
            if ( iTempHaptics )
                {
                iTempHaptics->ReleaseHaptics();
                iAppView->InsertOutput( _L("Temp client's reservation released.") );
                
                delete iTempHaptics;
                iTempHaptics = NULL;
                }
            else
                {
                iAppView->InsertOutput( _L("Temp client not created/reserved.") );
                }
            }
            break;

        case EHapticsTestConstGetters:
            {
            ConstGetters();
            }
            break;

        case EHapticsTestGetStatus:
            {
            // fetch haptics status, and print it
            MHWRMHapticsObserver::THWRMHapticsStatus status = 
                iHaptics->HapticsStatus();
            
            if ( iTempHaptics )
                {
                iAppView->InsertOutput( _L("[Haptics client] - "), EFalse );
                PrintStatusInfo( status );
                
                status = iTempHaptics->HapticsStatus();
                iAppView->InsertOutput( _L("[Temporary client] - "), EFalse );
                PrintStatusInfo( status );
                }
            else
                {
                PrintStatusInfo( status );
                }
            
            }
            break;

        case EHapticsTestSetPropertyPriorityMin:
            {
            SetPropertyPriorityMin();    
            }
            break;

        case EHapticsTestSetPropertyPriorityDefault:
            {
            SetPropertyPriorityDefault();    
            }
            break;

        case EHapticsTestSetPropertyPriorityMax:
            {
            SetPropertyPriorityMax();
            }
            break;

        case EHapticsTestSetPropertyDisableEffectsTrue:
            {
            SetPropertyDisableEffectsTrue();    
            }
            break;

        case EHapticsTestSetPropertyDisableEffectsFalse:
            {
            SetPropertyDisableEffectsFalse();    
            }
            break;

        case EHapticsTestSetPropertyStrengthMute:
            {
            SetPropertyStrengthMin();    
            }
            break;

        case EHapticsTestSetPropertyStrengthHalf:
            {
            SetPropertyStrengthMiddle();    
            }
            break;

        case EHapticsTestSetPropertyStrengthFull:
            {
            SetPropertyStrengthMax();    
            }
            break;

        case EHapticsTestGetPropertyPriority:
            {
            GetPropertyPriority();    
            }
            break;

        case EHapticsTestGetPropertyDisableEffects:
            {
            GetPropertyDisableEffects();    
            }
            break;

        case EHapticsTestGetPropertyStrength:
            {
            GetPropertyStrength();    
            }
            break;


        case EHapticsTestGetCapabilityDeviceCategory:
            {
            GetCapabilityDeviceCategory();    
            }
            break;

        case EHapticsTestGetCapabilityMaxNestedRepeats:
            {
            GetCapabilityMaxNestedRepeats();    
            }
            break;


        case EHapticsTestGetCapabilityNumActuators:
            {
            GetCapabilityNumActuators();    
            }
            break;


        case EHapticsTestGetCapabilityActuatorType:
            {
            GetCapabilityActuatorType();    
            }
            break;


        case EHapticsTestGetCapabilityNumEffectSlots:
            {
            GetCapabilityNumEffectSlots();    
            }
            break;

        case EHapticsTestGetCapabilityNumEffectStyles:
            {
            GetCapabilityNumEffectStyles();    
            }
            break;

        case EHapticsTestGetCapabilityMinPeriod:
            {
            GetCapabilityMinPeriod();    
            }
            break;

        case EHapticsTestGetCapabilityMaxPeriod:
            {
            GetCapabilityMaxPeriod();    
            }
            break;

        case EHapticsTestGetCapabilityMaxEffectDuration:
            {
            GetCapabilityMaxEffectDuration();    
            }
            break;

        case EHapticsTestGetCapabilitySupportedEffects:
            {
            GetCapabilitySupportedEffects();    
            }
            break;

        case EHapticsTestGetCapabilityDeviceName:
            {
            GetCapabilityDeviceName();    
            }
            break;

        case EHapticsTestGetCapabilityMaxEnvelopeTime:
            {
            GetCapabilityMaxEnvelopeTime();    
            }
            break;

        case EHapticsTestGetCapabilityAPIVersionNumber:
            {
            GetCapabilityAPIVersionNumber();    
            }
            break;

        case EHapticsTestGetCapabilityMaxIVTSize:
            {
            GetCapabilityMaxIVTSize();    
            }
            break;

        case EHapticsTestClearOutput:
            {
            iAppView->ClearOutput();
            }
            break;

        case EHapticsTestStreaming:
            {
            StartStreaming();
            }
            break;

        case EHapticsTestPlayStreamSample:
        case EHapticsTestNextStreamSample:
            {
            PlayStreamingSample( iCurrentSample );
            iCurrentSample >= KSampleCount ? iCurrentSample = 0 : iCurrentSample++;
            }
            break;

        case EHapticsTestPlayAllSamples:
            {
            for ( TInt n = 0; n < 8; n++ )
                {
                PlayStreamingSample( n );                
                User::After( 300000 );
                }
            }
            break;

        case EHapticsTestStopStreaming:
            {
            StopStreaming();
            }
            break;

        case EHapticsTestPanic:
        default:
            Panic( EHapticsTestBasicUi );
            break;
        }
    }

// ---------------------------------------------------------
// Manages the options menu contents based on user actions and data.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::DynInitMenuPaneL( TInt aResourceId, 
                                          CEikMenuPane* aMenuPane )
    {
    CAknAppUi::DynInitMenuPaneL( aResourceId, aMenuPane );

    // store menupane to be used in HandleCommandL()
    iMenuPane = aMenuPane;

    if ( aResourceId == R_HAPTICSTEST_MENU )
        {
        if ( !iHaptics )
            {
            // haptics client not created, hide commands using it
            aMenuPane->SetItemDimmed( EHapticsTestReserve, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestConstGetters, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestGetStatus, ETrue );
            }
        if ( !iActuatorOpened )
            {
            // Actuator has not been opened yet, hide ivt-file loading
            // submenu and general play-submenu
            aMenuPane->SetItemDimmed( EHapticsTestGeneralPlayEffect, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestLoadIVTFile, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestPropertiesSubmenu, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestCapabilitiesSubmenu, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestStreaming, ETrue );
            }
        if ( 0 == iLoadedIVTFileArray.Count() )
            {
            // also hide the deleteIVT data related menu items
            aMenuPane->SetItemDimmed( EHapticsTestDeleteIVTData, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestDeleteAllIVTData, ETrue );
            }
        if ( !iEffectData->Count() )
            {
            // hide the pause/resume/stop submenu
            aMenuPane->SetItemDimmed( EHapticsTestChangeState, ETrue );
            }
        if ( !iEffectData->ModifiableEffectExists() )
            {
            aMenuPane->SetItemDimmed( EHapticsTestModifyEffect, ETrue );
            }
        if ( iStreaming )
            {
            aMenuPane->SetItemDimmed( EHapticsTestStreaming, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestReserve, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestLoadIVTFile, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestGeneralPlayEffect, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestChangeState, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestModifyEffect, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestDeleteIVTData, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestPropertiesSubmenu, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestCapabilitiesSubmenu, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestDeleteAllIVTData, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestGetStatus, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestConstGetters, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestClearOutput, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestPanic, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestInit, ETrue );
            aMenuPane->SetItemDimmed( EHapticsToggle, ETrue );
            aMenuPane->SetItemDimmed( EAknSoftkeyExit, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestPlayStreamSample, iCurrentSample );
            aMenuPane->SetItemDimmed( EHapticsTestNextStreamSample, !iCurrentSample );
            }
        else
            {
            aMenuPane->SetItemDimmed( EHapticsTestPlayAllSamples, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestPlayStreamSample, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestNextStreamSample, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestStopStreaming, ETrue );
            }
        }
    else if ( aResourceId == R_HAPTICSTEST_INIT_SUBMENU )
        {
        if ( !iHaptics )
            {
            // haptics client not created, hide commands using it
            aMenuPane->SetItemDimmed( EHapticsTestSupportedActuators, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestOpenActuator, ETrue );
            }
        if ( !iSupportedActuators )
            {
            // either there are no supported actuators, or they have not
            // been fethed yet --> hide actuator opening submenu
            aMenuPane->SetItemDimmed( EHapticsTestOpenActuator, ETrue );
            }
        }
    else if ( aResourceId == R_HAPTICSTEST_GENERALPLAY_SUBMENU )
        {
        if ( !iActuatorOpened )
            {
            // Actuator has not been opened yet, hide the manual effect
            // play submenu
            aMenuPane->SetItemDimmed( EHapticsTestPlayEffectManual, ETrue );
            }
        if ( 0 == iLoadedIVTFileArray.Count() )
            {
            // IVT-data has not been loaded, hide the effect submenu
            aMenuPane->SetItemDimmed( EHapticsTestPlayEffect, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestPlayEffectRepeat, ETrue );
            }
        }
    else if ( aResourceId == R_HAPTICSTEST_TOGGLE_SUBMENU )
        {
        if ( iSynchronous )
            {
            // synchronous calls in ON, hide synchronous call selection
            aMenuPane->SetItemDimmed( EHapticsTestSynchronousCalls, ETrue );
            }
        else
            {
            // asynchronous calls in ON, hide asynchronous call selection
            aMenuPane->SetItemDimmed( EHapticsTestAsynchronousCalls, ETrue );
            }
        if ( iUseHandle )
            {
            aMenuPane->SetItemDimmed( EHapticsTestUsingFileHandle, ETrue );
            if ( iShowEffectInfo )
                {
                aMenuPane->SetItemDimmed( EHapticsTestShowEffectInfo, ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed( EHapticsTestHideEffectInfo, ETrue );
                }
            }
        else
            {
            aMenuPane->SetItemDimmed( EHapticsTestNotUsingFileHandle, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestShowEffectInfo, ETrue );
            aMenuPane->SetItemDimmed( EHapticsTestHideEffectInfo, ETrue );
            }    
        }
    else if ( aResourceId == R_HAPTICSTEST_ACTUATORS_SUBMENU )
        {
        if ( !(EHWRMLogicalActuatorAny & iSupportedActuators) )
            {
            aMenuPane->SetItemDimmed( EHapticsTestOpenActuatorAny, ETrue );
            }
        if ( !(EHWRMLogicalActuatorDevice & iSupportedActuators) )
            {
            aMenuPane->SetItemDimmed( EHapticsTestOpenActuatorDevice, ETrue );
            }
        if ( !(EHWRMLogicalActuatorPrimaryDisplay & iSupportedActuators) )
            {
            aMenuPane->SetItemDimmed( EHapticsTestOpenActuatorPrimaryDisplay, ETrue );
            }
        if ( !(EHWRMLogicalActuatorSecondaryDisplay & iSupportedActuators) )
            {
            aMenuPane->SetItemDimmed( EHapticsTestOpenActuatorSecondaryDisplay, ETrue );
            }
        if ( !(EHWRMLogicalActuatorGame & iSupportedActuators) )
            {
            aMenuPane->SetItemDimmed( EHapticsTestOpenActuatorGame, ETrue );
            }
        if ( !(EHWRMLogicalActuatorGameLeft & iSupportedActuators) )
            {
            aMenuPane->SetItemDimmed( EHapticsTestOpenActuatorGameLeft, ETrue );
            }
        if ( !(EHWRMLogicalActuatorGameRight & iSupportedActuators) )
            {
            aMenuPane->SetItemDimmed( EHapticsTestOpenActuatorGameRight, ETrue );
            }
        if ( !(EHWRMLogicalActuatorExternalVibra & iSupportedActuators) )
            {
            aMenuPane->SetItemDimmed( EHapticsTestOpenActuatorExternalVibra, ETrue );
            }
        }
    else if ( aResourceId == R_HAPTICSTEST_IVTFILES_SUBMENU )
        {
        if ( iIVTFileArray.Count() )
            {
            // ivt-files present, hide the "no files" -item
            aMenuPane->SetItemDimmed( EHapticsTestLoadIVTFileNoneFound, ETrue );
            
            // make file names visible in the submenu
            CEikMenuPaneItem::SData data;
            for ( TInt i = 0; i < iIVTFileArray.Count(); ++i )
                {
                data.iText.Copy( iIVTFileArray[i] );
                data.iCommandId = EHapticsTestLoadIVTFileSelected;
                data.iCascadeId = 0;
                data.iFlags = 0;
                data.iExtraText = KNullDesC;
                
                aMenuPane->AddMenuItemL( data );
                }
            }
        }
    else if ( aResourceId == R_HAPTICSTEST_PLAYEFFECT_SUBMENU )
        {
        CreatePlayEffectSubmenuL( aMenuPane, 
                                  EHapticsTestPlayEffectSelected,
                                  EHapticsTestPlayEffectNoneFound );
        }
    else if ( aResourceId == R_HAPTICSTEST_PLAYEFFECTREPEAT_SUBMENU )
        {
        CreatePlayEffectSubmenuL( aMenuPane, 
                                  EHapticsTestPlayEffectRepeatSelected,
                                  EHapticsTestPlayEffectRepeatNoneFound );
        }
    else if ( aResourceId == R_HAPTICSTEST_DELETEIVTDATA_SUBMENU )
        {
        if ( iLoadedIVTFileArray.Count() )
            {
            // loaded ivt-files present, hide the "no files" -item
            aMenuPane->SetItemDimmed( EHapticsTestDeleteIVTDataNoneFound, ETrue );
            
            // make loaded ivt-files visible in the submenu
            CEikMenuPaneItem::SData data;
            for ( TInt i = 0; i < iLoadedIVTFileArray.Count(); ++i )
                {
                data.iText.Copy( iLoadedIVTFileArray[i].iFileName );
                data.iCommandId = EHapticsTestDeleteIVTDataSelected;
                data.iCascadeId = 0;
                data.iFlags = 0;
                data.iExtraText = KNullDesC;
                
                aMenuPane->AddMenuItemL( data );
                }
            }
        }
    else if ( aResourceId == R_HAPTICSTEST_PAUSE_SUBMENU )
        {
        // fill effect history data to pause submenu
        iEffectData->DynInitChangeEffectStateSubmenuL( 
                        aMenuPane, EHapticsTestChangeStatePause, iHaptics );
        }
    else if ( aResourceId == R_HAPTICSTEST_RESUME_SUBMENU )
        {
        // fill effect history data to resume submenu
        iEffectData->DynInitChangeEffectStateSubmenuL( 
                        aMenuPane, EHapticsTestChangeStateResume, iHaptics );
        }
    else if ( aResourceId == R_HAPTICSTEST_STOP_SUBMENU )
        {
        // fill effect history data to stop submenu
        iEffectData->DynInitChangeEffectStateSubmenuL(
                        aMenuPane, EHapticsTestChangeStateStop, iHaptics );
        }
    else if ( aResourceId == R_HAPTICSTEST_MODIFYDURATION_SUBMENU )
        {
        iEffectData->DynInitDurationSubmenu( aMenuPane );
        }
    else if ( aResourceId == R_HAPTICSTEST_MODIFYMAGNITUDE_SUBMENU )
        {
        iEffectData->DynInitMagnitudeSubmenu( aMenuPane );
        }
    else if ( aResourceId == R_HAPTICSTEST_MODIFYSTYLE_SUBMENU )
        {
        iEffectData->DynInitStyleSubmenu( aMenuPane );
        }
    else if ( aResourceId == R_HAPTICSTEST_MODIFYATTACK_SUBMENU )
        {
        iEffectData->DynInitAttackLevelSubmenu( aMenuPane );
        }
    else if ( aResourceId == R_HAPTICSTEST_MODIFYFADE_SUBMENU )
        {
        iEffectData->DynInitFadeLevelSubmenu( aMenuPane );
        }
    else if ( aResourceId == R_HAPTICSTEST_MODIFYATTACKTIME_SUBMENU )
        {
        iEffectData->DynInitAttackTimeSubmenu( aMenuPane );
        }
    else if ( aResourceId == R_HAPTICSTEST_MODIFYFADETIME_SUBMENU )
        {
        iEffectData->DynInitFadeTimeSubmenu( aMenuPane );
        }
    else if ( aResourceId == R_HAPTICSTEST_MODIFYPERIOD_SUBMENU )
        {
        iEffectData->DynInitPeriodSubmenu( aMenuPane );
        }
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CHapticsTestAppUi::HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination)
        {
    CAknAppUi::HandleWsEventL(aEvent, aDestination);
    
    TInt eventType = aEvent.Type();
    if (eventType == EEventKeyUp)
        {
        TKeyEvent key = *(aEvent.Key());
        GetAliasKeyCodeL(key.iCode, key, EEventKey);
        TUint code = key.iScanCode;
        
        if (code == '5' && !iActuatorOpened )
            {
            HandleCommandL(EHapticsTestOpenHaptics); // open client
            HandleCommandL(EHapticsTestSupportedActuators); // ask supported actuators
            HandleCommandL(EHapticsTestOpenActuatorAny); // open any actuator

            HandleCommandL(EHapticsTestGetCapabilityDeviceName);
            }
        }
    }

// ---------------------------------------------------------
// Prints received haptics status notifications onto
// the screen.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::HapticsStatusChangedL( THWRMHapticsStatus aStatus )
    {
    PrintStatusInfo( aStatus );
    }

// ---------------------------------------------------------
// Empty implementation.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::EffectCompletedL( TInt /*aError*/, 
                                          TInt /*aEffectHandle*/ )
    {
    }

// ---------------------------------------------------------
// Outputs received actuator events onto the screen.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::ActuatorEventL( THWRMActuatorEvents aEvent, 
                                        THWRMLogicalActuators aActuator )
    {
    // actuator name
    TBuf<64> actuator;
    switch( aActuator )
        {
        case EHWRMLogicalActuatorAny:
            actuator.Append( _L("Any") );
            break;
        case EHWRMLogicalActuatorDevice:
            actuator.Append( _L("Device") );
            break;
        case EHWRMLogicalActuatorPrimaryDisplay:
            actuator.Append( _L("PrimaryDisplay") );
            break;
        case EHWRMLogicalActuatorSecondaryDisplay:
            actuator.Append( _L("SecondaryDisplay") );
            break;
        case EHWRMLogicalActuatorGame:
            actuator.Append( _L("Game") );
            break;
        case EHWRMLogicalActuatorGameLeft:
            actuator.Append( _L("GameLeft") );
            break;
        case EHWRMLogicalActuatorGameRight:
            actuator.Append( _L("GameRight") );
            break;
        case EHWRMLogicalActuatorExternalVibra:
            actuator.Append( _L("ExternalVibra") );
            break;
        default:
            actuator.Append( _L("<unknown>") );
            break;
        }

    iAppView->InsertOutput( _L("Received actuator event: "), EFalse );
    iAppView->InsertOutput( actuator, EFalse );
    
    switch( aEvent )
        {
        case MHWRMHapticsActuatorObserver::EHWRMActuatorAttached:
            {
            iAppView->InsertOutput( _L(" Attached.") );
            break;
            }
        case MHWRMHapticsActuatorObserver::EHWRMActuatorDetached:
            {
            iAppView->InsertOutput( _L(" Deattached.") );
            break;
            }
        case MHWRMHapticsActuatorObserver::EHWRMActuatorEnabled:
            {
            iAppView->InsertOutput( _L(" Enabled.") );
            break;
            }
        case MHWRMHapticsActuatorObserver::EHWRMActuatorDisabled:
            {
            iAppView->InsertOutput( _L(" Disabled.") );
            break;
            }
        default:
            {
            iAppView->InsertOutput( _L(" <unknown status>") );
            break;
            }
        }
    }

// ---------------------------------------------------------
// Creates haptics client instance.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::OpenHapticsL()
    {
    if ( iHaptics )
        {
        iAppView->InsertOutput( _L("Already created.") );
        }
    else
        {
        // Create haptics client
        if ( iSynchronous )
            {
            iHaptics = CHWRMHaptics::NewL( this, this );
            }
        else
            {
            // asynchronous creation
            TRequestStatus status;
            iHaptics = CHWRMHaptics::NewL( this, this, status );
            User::WaitForRequest( status );
            User::LeaveIfError( status.Int() );
            }

        if ( iHaptics )
            {
            iAppView->InsertOutput( _L("Haptics client created.") );
            }
        else
            {
            iAppView->InsertOutput( _L("Error: Creation failed.") );
            }
        }
    }

// ---------------------------------------------------------
// Deletes haptics client instance, and resets member variables
// so that options-menu is updated accordingly.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::CloseHaptics()
    {
    if( iHaptics )
        {
        delete iHaptics;
        iHaptics = NULL;
        iAppView->InsertOutput( _L("Haptics client deleted.") );

        // uninit members
        iSupportedActuators = 0;
        iActuatorOpened = EFalse;
        iIVTFileHandle = 0;
        
        while ( iLoadedIVTFileArray.Count() )
            {
            if ( iLoadedIVTFileArray[0].iDataBuffer )
                {
                delete iLoadedIVTFileArray[0].iDataBuffer;
                iLoadedIVTFileArray[0].iDataBuffer = NULL;
                }
            iLoadedIVTFileArray.Remove( 0 );
            }
        
        iIVTDataBuffer = NULL;
        }
    else
        {
        iAppView->InsertOutput( _L("Error: Haptics client doesn\'t exist.") );
        }             
    }

// ---------------------------------------------------------
// Fetches the supported actuators from haptics server, 
// and stores the mask to iSupportedActuators.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::FetchSupportedActuators()
    {
    TInt err = iHaptics->SupportedActuators( iSupportedActuators );

    if ( err )
        {
        iAppView->InsertOutput( _L("Error: Getting supported actuators failed.") );
        }
    else
        {
        PrintSupportedActuators();
        }
    }

// ---------------------------------------------------------
// Prints the given supported actuators to the UI.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::PrintSupportedActuators()
    {
    iAppView->InsertOutput( _L("Supported actuators:") );
    
    if( EHWRMLogicalActuatorAny & iSupportedActuators )
        {
        iAppView->InsertOutput( _L("    EHWRMLogicalActuatorAny") );
        }
    if( EHWRMLogicalActuatorDevice & iSupportedActuators )
        {
        iAppView->InsertOutput( _L("    EHWRMLogicalActuatorDevice") );
        }
    if( EHWRMLogicalActuatorPrimaryDisplay & iSupportedActuators )
        {
        iAppView->InsertOutput( _L("    EHWRMLogicalActuatorPrimaryDisplay") );
        }
    if( EHWRMLogicalActuatorSecondaryDisplay & iSupportedActuators )
        {
        iAppView->InsertOutput( _L("    EHWRMLogicalActuatorSecondaryDisplay") );
        }
    if( EHWRMLogicalActuatorGame & iSupportedActuators )
        {
        iAppView->InsertOutput( _L("    EHWRMLogicalActuatorGame") );
        }
    if( EHWRMLogicalActuatorGameLeft & iSupportedActuators )
        {
        iAppView->InsertOutput( _L("    EHWRMLogicalActuatorGameLeft") );
        }
    if( EHWRMLogicalActuatorGameRight & iSupportedActuators )
        {
        iAppView->InsertOutput( _L("    EHWRMLogicalActuatorGameRight") );
        }
    if( EHWRMLogicalActuatorExternalVibra & iSupportedActuators )
        {
        iAppView->InsertOutput( _L("    EHWRMLogicalActuatorExternalVibra") );
        }
    if( EHWRMLogicalActuatorLast & iSupportedActuators )
        {
        // ?????
        iAppView->InsertOutput( _L("    EHWRMLogicalActuatorLast") );
        }
    }

// ---------------------------------------------------------
// Attemps to open the given actuator. Prints the result
// of the attemp to the UI.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::OpenActuator( THWRMLogicalActuators aActuator )
    {
    TRAPD( err, iHaptics->OpenActuatorL( aActuator ) );
    TBuf<100> buf;
    if ( err )
        {
        buf.Append( _L("Error: Opening actuator failed; ") );
        buf.AppendNum( err );
        iAppView->InsertOutput( buf );
        
        iActuatorOpened = EFalse;
        }
    else
        {
        iAppView->InsertOutput( _L("Actuator opened successfully.") );
        
        iActuatorOpened = ETrue;

        TInt err = iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsLicensekey,
                                                KNullDesC8() );
        if (err)                                        
            {
            buf.Append( _L("Error: Setting licensefailed: ") );
            buf.AppendNum( err );
            iAppView->InsertOutput( buf );
        
            iActuatorOpened = EFalse;
            return;
            }
        iAppView->InsertOutput( _L("License is set.") );

        err = iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsPriority, 
                                           KHWRMHapticsMaxDevicePriority );

        }
    }

// ---------------------------------------------------------
// Reserves haptics for this application.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::ReserveHaptics( CHWRMHaptics* aHaptics, 
                                        TBool aForceNoCoe )
    {
    TInt err = KErrNone;
    TBuf<24> forceNoCoe;
    
    if ( aForceNoCoe )
        {
        forceNoCoe.Append( _L("(trusted).") );
        TRAP( err, aHaptics->ReserveHapticsL( ETrue ) );
        }
    else
        {
        forceNoCoe.Append( _L("(not trusted).") );
        TRAP( err, aHaptics->ReserveHapticsL() );
        }
    
    if ( err )
        {
        TBuf<8> errorCode;
        errorCode.AppendNum( err );

        iAppView->InsertOutput( _L("Reserving haptics failed err = "), EFalse );
        iAppView->InsertOutput( errorCode );
        }
    else
        {
        iAppView->InsertOutput( _L("Reserved haptics "), EFalse );
        iAppView->InsertOutput( forceNoCoe );
        }
    }

// ---------------------------------------------------------
// Searches for IVT-files in the filesystem, and writes
// them to iIVTFileArray.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::FindIVTFiles()
    {
    iAppView->InsertOutput( _L("Searching IVT-files...") );

    // open file server session
    RFs rfs;
    TInt err = rfs.Connect();
    
    if ( err )
        {
        iAppView->InsertOutput( _L("Error occured while opening fileserver session!") );
        return;
        }

    // set path where to search files
    CDir* dirList = 0;    
    TInt memCrdUsageErr = KErrNone;
    
#ifdef __MARM_ARMV5__
    // first, try to use mem card.. (in ARMV5 builds)
    memCrdUsageErr = rfs.SetSessionPath( KSessionPathCrd );

    if ( !memCrdUsageErr )
        {
        memCrdUsageErr = rfs.GetDir( KIVTFileType, KEntryAttMaskSupported,
                                     ESortByName, dirList );
        }
    if ( !memCrdUsageErr && dirList )
        {
        iUsesMemCard = ETrue;
        }
#endif

    if ( !iUsesMemCard )
        {
        // secondly, try to use phone mem.. (actually first in non-ARMV5 builds)
        err = rfs.SetSessionPath( KSessionPathMem );

        if ( err )
            {
            iAppView->InsertOutput( _L("Setting path for fileserver failed!") );
            return;
            }

        // get directory file listing of IVT-files
        err = rfs.GetDir( KIVTFileType, KEntryAttMaskSupported,
                          ESortByName, dirList );
    
        if ( err || !dirList )
            {
            iAppView->InsertOutput( _L("Fetching folder content failed!") );
            return;
            }
        }

    // file server session not needed anymore, close it
    rfs.Close();

    // go through all found ivt-files and read directory 
    // entries to filenamearray
    for( TInt i = 0; i < dirList->Count(); i++ )
        {
        iIVTFileArray.Append( (*dirList)[i].iName );

        // print findings to the UI
        iAppView->InsertOutput( _L("    "), EFalse );
        iAppView->InsertOutput( (*dirList)[i].iName );
        }
    
    delete dirList;
    dirList = NULL;
    }

// ---------------------------------------------------------
// Loads IVT data from the given file to haptics.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::LoadIVTDataL( TFileName& aFile )
    {
    // open file server session
    RFs rfs;
    if ( rfs.Connect() )
        {
        iAppView->InsertOutput( _L("Error occured while opening fileserver session!") );
        return;
        }

    // set path where to search files
#ifdef __MARM_ARMV5__
    if ( iUsesMemCard )
        {
        rfs.SetSessionPath( KSessionPathCrd );
        }
    else    
        {
#endif

        rfs.SetSessionPath( KSessionPathMem );

#ifdef __MARM_ARMV5__
        }
#endif
        
    RFile effectFile;
    TInt fileSize( 0 );

    User::LeaveIfError( effectFile.Open( rfs, aFile, EFileRead ) );

    effectFile.Size( fileSize );

    // create buffer for the IVT-data
    HBufC8* IVTDataBuffer = HBufC8::NewLC( fileSize );
    TPtr8 dataBufPtr = IVTDataBuffer->Des();

    // read the data from file to buffer
    effectFile.Read( dataBufPtr );

    // close file handle and file server session
    effectFile.Close();
    rfs.Close();

    // load data to haptics
    TInt IVTFileHandle = 0;
    TInt err = iHaptics->LoadEffectData( *IVTDataBuffer, IVTFileHandle );

    if ( err )
        {
        CleanupStack::PopAndDestroy( IVTDataBuffer ); 
        
        if ( err == KErrAlreadyExists )
            {
            iAppView->InsertOutput( _L("IVT-data already loaded!") );
            }
        else
            {
            iAppView->InsertOutput( _L("Error loading IVT-data!") );
            }
        }
    else
        {
        TInt count;
        iHaptics->GetEffectCount( IVTFileHandle, count );

        TLoadedIVTFileItem newItem;
        newItem.iFileName.Copy( aFile );
        newItem.iFileHandle = IVTFileHandle;
        newItem.iDataBuffer = IVTDataBuffer;
        if ( KErrNotFound == iLoadedIVTFileArray.Find( newItem, TIdentityRelation<TLoadedIVTFileItem>( TLoadedIVTFileItem::Match ) ) )
            {
            iLoadedIVTFileArray.AppendL( newItem );
            CleanupStack::Pop( IVTDataBuffer ); // ownership now in the array
            TBuf<24> handleBuf;
            handleBuf.AppendNum( IVTFileHandle );
            iAppView->InsertOutput( _L("IVT-data loaded succesfully!") );
            iAppView->InsertOutput( _L("    File: "), EFalse );
            iAppView->InsertOutput( aFile );
            iAppView->InsertOutput( _L("    FileHandle: "), EFalse );
            iAppView->InsertOutput( handleBuf, EFalse );
            }
        else
            {
            CleanupStack::PopAndDestroy( IVTDataBuffer ); 
            iAppView->InsertOutput( _L("IVT-data already loaded!") );
            iAppView->InsertOutput( _L("    File: "), EFalse );
            iAppView->InsertOutput( aFile ); 
            }    
        iAppView->InsertOutput( _L("    Effect count: "), EFalse );
        TBuf<5> countBuf;
        countBuf.AppendNum( count );
        iAppView->InsertOutput( countBuf );
        }
    }

// ---------------------------------------------------------
// Plays the effect of the given index in the currently
// loaded IVT-file.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::PlayEffect( TInt aIndex )
    {
    TInt err = KErrNone;
    iEffectHandle = KErrNotFound;
    TBool usingAsyncSender( EFalse );
    
    if ( iSynchronous )
        {
        iAppView->InsertOutput( _L("Playing effect synchronously...") );
        if ( iUseHandle )
            {
            err = iHaptics->PlayEffect( iIVTFileHandle,
                                        aIndex, iEffectHandle );
            }
        else
            {
            err = iHaptics->PlayEffect( *iIVTDataBuffer,
                                        aIndex, iEffectHandle );
            }
        }
    else
        {
        iAppView->InsertOutput( _L("Playing effect asynchronously...") );
        
        if ( iUseHandle )
            {
            usingAsyncSender = ETrue;
            iAsyncPlaySender->Play( iHaptics, iIVTFileHandle, 
                                    aIndex, iEffectHandle );
            }
        else
            {
            TRequestStatus status;
            iHaptics->PlayEffect( *iIVTDataBuffer,
                                  aIndex, iEffectHandle, status );
            User::WaitForRequest( status );
            err = status.Int();                      
            }    
        }
        
    if ( !usingAsyncSender )
        {
        // print error/success
        PrintPlayEffectStatus( err );
        GetCurrentEffectState();

        // if effect played successfully, add history data
        if ( !err )
            {
            iEffectData->AddEffectInfo( iEffectHandle, iIVTFileHandle, aIndex );
            }
        }
    }

// ---------------------------------------------------------
// Plays the effect of the given index in the currently
// loaded IVT-file repeatedly
// ---------------------------------------------------------
//
void CHapticsTestAppUi::RepeatEffect( TInt aIndex, TInt aRepeats )
    {
    TInt err = KErrNone;
    iEffectHandle = KErrNotFound;
    TBool usingAsyncSender( EFalse );
    
    if ( iSynchronous )
        {
        _LIT( KInsertedText, "Playing Repeat %d times synchronously..." );
        TBuf<50> buf;
        buf.Format( KInsertedText, aRepeats );
        iAppView->InsertOutput( buf );
        
        if ( iUseHandle )
            {
            err = iHaptics->PlayEffectRepeat( iIVTFileHandle,
                                              aIndex, aRepeats, iEffectHandle );
            }
        else
            {
            err = iHaptics->PlayEffectRepeat( *iIVTDataBuffer,
                                              aIndex, aRepeats, iEffectHandle );
            }
        }
    else
        {
        _LIT( KInsertedText, "Playing Repeat %d times asynchronously..." );
        TBuf<50> buf;
        buf.Format( KInsertedText, aRepeats );
        iAppView->InsertOutput( buf );

        if ( iUseHandle )
            {            
            usingAsyncSender = ETrue;
            iAsyncPlaySender->PlayRepeat( iHaptics,
                                          iIVTFileHandle,
                                          aIndex, 
                                          aRepeats, 
                                          iEffectHandle );
            }
        else
            {
            TRequestStatus status;
            iHaptics->PlayEffectRepeat( *iIVTDataBuffer,
                                        aIndex, aRepeats, iEffectHandle, status );
            User::WaitForRequest( status );
            err = status.Int();             
            }    
        }    

    if ( !usingAsyncSender )
        {
        // print error/success
        PrintPlayEffectStatus( err );
        GetCurrentEffectState();

        // if effect played successfully, add history data
        if ( !err )
            {
            iEffectData->AddEffectInfo( iEffectHandle, iIVTFileHandle, aIndex );
            }
        }
    }

// ---------------------------------------------------------
// Plays a magsweep effect using manual definition.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::PlayMagSweepL( TBool aInfinite, TBool aModifiable )
    {
    // construct default magsweep data
    CHWRMHaptics::THWRMHapticsMagSweepEffect magsweep;
    iEffectData->FillDefaultMagSweepData( magsweep, aInfinite, 
                                          aModifiable, iHaptics );
    
    TInt err = KErrNone;
    iEffectHandle = KErrNotFound;
    
    if ( iSynchronous )
        {
        iAppView->InsertOutput( _L("Playing MagSweep synchronously...") );

        // synchronous call
        err = iHaptics->PlayMagSweepEffect( magsweep, iEffectHandle );
        }
    else
        {
        iAppView->InsertOutput( _L("Playing MagSweep asynchronously...") );

        // asynchronous call
        TRequestStatus status;
        iHaptics->PlayMagSweepEffect( magsweep, iEffectHandle,
                                      status );
        User::WaitForRequest( status );
        err = status.Int();
        }

    // print error/success
    PrintPlayEffectStatus( err );
    GetCurrentEffectState();

    // if effect played successfully, add history data
    if ( !err )
        {
        iEffectData->AddEffectInfo( iEffectHandle );
        iEffectData->ResetModifiableMagSweepEffectL( magsweep, iEffectHandle,
                                                     iHaptics );

        // if this effect should be auto-modifiable, reset modification timer
        if ( aModifiable )
            {
            iEffectData->StartModifiableEffectTimerL( ModifyPlayingEffect );
            }
        }
    }

// ---------------------------------------------------------
// Plays a periodic effect using manual definition.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::PlayPeriodicL( TBool aInfinite, TBool aModifiable )
    {
    // construct default periodic data
    CHWRMHaptics::THWRMHapticsPeriodicEffect periodic;
    iEffectData->FillDefaultPeriodicData( periodic, aInfinite, 
                                          aModifiable, iHaptics );

    TInt err = KErrNone;
    iEffectHandle = KErrNotFound;
    
    if ( iSynchronous )
        {
        iAppView->InsertOutput( _L("Playing Periodic synchronously...") );

        // synchronous call
        err = iHaptics->PlayPeriodicEffect( periodic, iEffectHandle );
        }
    else
        {
        iAppView->InsertOutput( _L("Playing Periodic asynchronously...") );

        // asynchronous call
        TRequestStatus status;
        iHaptics->PlayPeriodicEffect( periodic, iEffectHandle,
                                      status );
        User::WaitForRequest( status );
        err = status.Int();
        }

    // print error/success
    PrintPlayEffectStatus( err );
    GetCurrentEffectState();

    // if effect played successfully, add history data
    if ( !err )
        {
        iEffectData->AddEffectInfo( iEffectHandle );
        iEffectData->ResetModifiablePeriodicEffectL( periodic, iEffectHandle,
                                                     iHaptics );
        
        // if this effect should be auto-modifiable, reset modification timer
        if ( aModifiable )
            {
            iEffectData->StartModifiableEffectTimerL( ModifyPlayingEffect );
            }
        }
    }

// ---------------------------------------------------------
// Plays repeatedly a timeline effect using manual definition.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::PlayRepeatL( TInt aRepeats )
    {
    TInt err = KErrNone;
    iEffectHandle = KErrNotFound;
    
    // This is a simple effect (just plays magsweep type thingie for a second)
    // produced from VibeStudio (export to C-file functionality there..)
    static const TUint8 simpleTimelineEffect[] = 
        {  
        0x01, 0x00, 0x02, 0x00, 0x1c, 0x00, 0x28, 0x00, 0x00, 0x00, 
        0x07, 0x00, 0xf1, 0xe0, 0x01, 0xe2, 0x00, 0x00, 0xff, 0x30, 
        0xc8, 0x00, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x20, 0x3e, 0x00, 
        0x00, 0x5f, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 0x12, 0x00, 
        0x54, 0x00, 0x69, 0x00, 0x6d, 0x00, 0x65, 0x00, 0x6c, 0x00, 
        0x69, 0x00, 0x6e, 0x00, 0x65, 0x00, 0x00, 0x00, 0x4d, 0x00, 
        0x61, 0x00, 0x67, 0x00, 0x53, 0x00, 0x77, 0x00, 0x65, 0x00, 
        0x65, 0x00, 0x70, 0x00, 0x00, 0x00
        };
        
    RBuf8 effectBuffer;
    CleanupClosePushL( effectBuffer );
    effectBuffer.ReAlloc( sizeof( simpleTimelineEffect ) );
    TUint8* tmpPtr = const_cast<TUint8*>( effectBuffer.Ptr() );
    memcpy( tmpPtr, simpleTimelineEffect, sizeof ( simpleTimelineEffect ) );
    effectBuffer.SetLength( sizeof ( simpleTimelineEffect ) );
    
    if ( iSynchronous )
        {
        _LIT( KInsertedText, "Playing Repeat %d times synchronously..." );
        TBuf<50> buf;
        buf.Format( KInsertedText, aRepeats );
        iAppView->InsertOutput( buf );
    
        err = iHaptics->PlayEffectRepeat( effectBuffer, 0, aRepeats, iEffectHandle );
        }
    else
        {
        _LIT( KInsertedText, "Playing Repeat %d times asynchronously..." );
        TBuf<50> buf;
        buf.Format( KInsertedText, aRepeats );
        iAppView->InsertOutput( buf );

        TRequestStatus status;
        iHaptics->PlayEffectRepeat( effectBuffer, 0, aRepeats, iEffectHandle, status );
        User::WaitForRequest( status );
        err = status.Int();
        }

    CleanupStack::PopAndDestroy(); // effectBuffer (closed here..)
    
    // print error/success
    PrintPlayEffectStatus( err );
    GetCurrentEffectState();

    // if effect played successfully, add history data
    if ( !err )
        {
        iEffectData->AddEffectInfo( iEffectHandle );
        }
    }

// ---------------------------------------------------------
// Pauses the effect of the given index in the effect
// history data.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::PauseEffect( TInt aIndex )
    {
    // get effect handle
    TInt effectHandle = iEffectData->EffectHandle(aIndex);

    // pause effect
    TInt err = iHaptics->PausePlayingEffect( effectHandle );

    if ( err )
        {
        TBuf<8> errBuf;
        errBuf.AppendNum( err );
        iAppView->InsertOutput( _L("Pausing effect failed, err = "), EFalse );
        iAppView->InsertOutput( errBuf );
        }
    else
        {
        iAppView->InsertOutput( _L("Pausing effect succeeded.") );
        }
    }

// ---------------------------------------------------------
// Resumes the effect of the given index in the effect
// history data.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::ResumeEffect( TInt aIndex )
    {
    // get effect handle
    TInt effectHandle = iEffectData->EffectHandle(aIndex);

    // resume effect
    TInt err = iHaptics->ResumePausedEffect( effectHandle );

    if ( err )
        {
        TBuf<8> errBuf;
        errBuf.AppendNum( err );
        iAppView->InsertOutput( _L("Resuming effect failed, err = "), EFalse );
        iAppView->InsertOutput( errBuf );
        }
    else
        {
        iAppView->InsertOutput( _L("Resuming effect succeeded.") );
        }
    }

// ---------------------------------------------------------
// Stops the effect of the given index in the effect
// history data.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::StopEffect( TInt aIndex )
    {
    // get effect handle
    TInt effectHandle = iEffectData->EffectHandle(aIndex);

    // stop effect
    TInt err = iHaptics->StopPlayingEffect( effectHandle );

    if ( err )
        {
        TBuf<8> errBuf;
        errBuf.AppendNum( err );
        iAppView->InsertOutput( _L("Stopping effect failed, err = "), EFalse );
        iAppView->InsertOutput( errBuf );
        }
    else
        {
        iAppView->InsertOutput( _L("Stopping effect succeeded.") );
        }
    
    // stop auto-modify effect, if it is on
    iEffectData->ResetModifiableEffectTimer();
    }

// ---------------------------------------------------------
// Deletes loaded IVT data
// ---------------------------------------------------------
//
void CHapticsTestAppUi::DeleteLoadedIVTData( TInt aIndex )
    {
    if ( 0 > aIndex || aIndex >= iLoadedIVTFileArray.Count() )
        {
        iAppView->InsertOutput( _L("Error: Delete index mismatch!") );
        return; 
        }
    
    TInt err = iHaptics->DeleteEffectData( iLoadedIVTFileArray[aIndex].iFileHandle );
    if ( KErrNone == err )
        {
        // delete effect history
        iEffectData->DeleteEffectInfo( iLoadedIVTFileArray[aIndex].iFileHandle );
        
        // delete ivt-data item
        if ( iLoadedIVTFileArray[aIndex].iDataBuffer )
            {
            delete iLoadedIVTFileArray[aIndex].iDataBuffer;
            }

        iLoadedIVTFileArray.Remove( aIndex );    
        
        iAppView->InsertOutput( _L("IVT data deletion succeeded") );
        }
    else
        {
        _LIT( KInsertedText, "IVT data deletion failed: %d" );
        TBuf<80> buf;
        buf.Format( KInsertedText, err );
        iAppView->InsertOutput( buf );
        }
    }

// ---------------------------------------------------------
// Deletes all loaded IVT datas
// ---------------------------------------------------------
//
void CHapticsTestAppUi::DeleteAllLoadedIVTData()
    {
    iAppView->InsertOutput( _L("Deleting all loaded IVT datas.") );
    
    TInt err = iHaptics->DeleteAllEffectData();
    
    if ( KErrNone == err )
        {
        iAppView->InsertOutput( _L("All IVT data deletion succeeded") );
        
        while ( iLoadedIVTFileArray.Count() )
            {
            // delete effect history
            iEffectData->DeleteEffectInfo( iLoadedIVTFileArray[0].iFileHandle );

            // delete ivt-data item
            if ( iLoadedIVTFileArray[0].iDataBuffer )
                {
                delete iLoadedIVTFileArray[0].iDataBuffer;
                }
            
            iLoadedIVTFileArray.Remove( 0 );
            }
        }
    else
        {
        _LIT( KInsertedText, "All IVT data deletion failed: %d" );
        TBuf<80> buf;
        buf.Format( KInsertedText, err );
        iAppView->InsertOutput( buf );
        }
    }


// ---------------------------------------------------------
// Fetches the current state of the last played effect 
// (iEffectHandle), and prints the result to the UI.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::GetCurrentEffectState()
    {
    TInt effectState = KErrNotFound;
    TInt err = iHaptics->GetEffectState( iEffectHandle, effectState );

    if ( err )
        {
        TBuf<16> errCode;
        errCode.AppendNum( err );
        iAppView->InsertOutput( _L("    Fetching effect state failed! Err: "), EFalse );
        iAppView->InsertOutput( errCode );
        }
    else
        {
        TBuf<16> stateBuf;
        
        switch ( effectState )
            {
            case CHWRMHaptics::EHWRMHapticsEffectNotPlaying:
                stateBuf.Append( _L("Not playing.") );
                break;
            case CHWRMHaptics::EHWRMHapticsEffectPlaying:
                stateBuf.Append( _L("Playing.") );
                break;
            case CHWRMHaptics::EHWRMHapticsEffectPaused:
                stateBuf.Append( _L("Paused.") );
                break;
            default:
                stateBuf.Append( _L("Unknown!") );
                break;
            }
        
        iAppView->InsertOutput( _L("    Effect state: "), EFalse );
        iAppView->InsertOutput( stateBuf );
        }
    }

// ---------------------------------------------------------
// Prints the status of a "play effect".
// ---------------------------------------------------------
//
void CHapticsTestAppUi::PrintPlayEffectStatus( TInt aErrorCode ) const
    {
    if ( aErrorCode )
        {
        TBuf<16> errCode;
        errCode.AppendNum( aErrorCode );
        iAppView->InsertOutput( _L("Playing effect failed! Err: "), EFalse );
        iAppView->InsertOutput( errCode );
        }
    else
        {
        TBuf<16> handleBuf;
        handleBuf.AppendNum( iEffectHandle );
        iAppView->InsertOutput( _L("Played effect successfully!") );
        iAppView->InsertOutput( _L("    Effect handle: "), EFalse );
        iAppView->InsertOutput( handleBuf );
        }
    }

// ---------------------------------------------------------
// Queries user how many repeats to have in case of playing
// some effect repeatedly.
// ---------------------------------------------------------
// 
TInt CHapticsTestAppUi::GetNumberOfRepeats()
    {
    TInt repeatsIndex;
    TInt retVal = 2; // default is 2 repeats
    
    // Uses non-leavable new for CAknListQueryDialog construction
    CAknListQueryDialog* query = new CAknListQueryDialog( &repeatsIndex );

    if ( query )
        {
        query->PrepareLC( R_HAPTICS_REPEATEFFECT_REPEATS_QUERY );
    
        if ( query->RunLD() )
            {
            switch ( repeatsIndex )
                {
                case 0:
                    retVal = 0;   
                    break;
                case 1:
                    retVal = 1;   
                    break;
                case 2:
                    retVal = 2;   
                    break;
                case 3:
                    retVal = 3;   
                    break;
                case 4:
                    retVal = 7;   
                    break;
                case 5:
                    retVal = iHaptics->InfiniteRepeat();
                    break;
                default:
                    break; // uses default (2) as return value
                };
            }
        }
    return retVal;    
    }

// ---------------------------------------------------------
// Calculates the effect index from the submenu index.
// ---------------------------------------------------------
// 
TInt CHapticsTestAppUi::CountFileHandleAndEffectIndex(TInt aSubmenuIndex)
    {
    TInt effectFileCount = iLoadedIVTFileArray.Count();
    TInt retVal = 0xFFFFFFFF;
    TInt totalRunningEffectCount = 0;
    TInt effectCountAfterPreviousFile = 0;
    
    for ( TInt i = 0; i < effectFileCount; ++i )
        {
        TInt effectsInThisFile = 0;
        iHaptics->GetEffectCount( iLoadedIVTFileArray[i].iFileHandle, effectsInThisFile );
        effectCountAfterPreviousFile = totalRunningEffectCount;
        totalRunningEffectCount += effectsInThisFile;
        if ( aSubmenuIndex < totalRunningEffectCount )
            {
            iIVTFileHandle = iLoadedIVTFileArray[i].iFileHandle;
            iIVTDataBuffer = iLoadedIVTFileArray[i].iDataBuffer;
            retVal = aSubmenuIndex - effectCountAfterPreviousFile;
            break;
            }
        }
    return retVal;    
    }

// ---------------------------------------------------------
// Creates all submenu items for a play effect submenu.
// Fetches all the names of the effects from currently 
// loaded IVT-data.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::CreatePlayEffectSubmenuL( CEikMenuPane* aMenuPane, 
                                                  TInt aCommandId,
                                                  TInt aDimCommandId )
    {
    TInt loadedIVTFileCount = iLoadedIVTFileArray.Count();
    TBool dimmingNeeded = EFalse;
            
    if ( loadedIVTFileCount )
        {
        // make all effect names (in all loaded files) visible in the submenu
        CEikMenuPaneItem::SData data;
        for ( TInt j = 0; j < loadedIVTFileCount; ++j)
            {
            if ( iLoadedIVTFileArray[j].iDataBuffer )
                {
                TInt effectCount = 0;
                iHaptics->GetEffectCount( iLoadedIVTFileArray[j].iFileHandle, effectCount );
                HBufC8* tempBuf8 = HBufC8::New( iHaptics->MaxEffectNameLength() );
                TPtr8 effectName = tempBuf8->Des();
                for ( TInt i = 0; i < effectCount; ++i )
                    {
                    // fetch effect name
                    iHaptics->GetEffectName( iLoadedIVTFileArray[j].iFileHandle, i, effectName );
                
                    data.iText.Copy( effectName );
                    data.iCommandId = aCommandId;
                    data.iCascadeId = 0;
                    data.iFlags = 0;
                    data.iExtraText = KNullDesC;
                
                    aMenuPane->AddMenuItemL( data );
                    dimmingNeeded = ETrue;
                    }
                delete tempBuf8;
                }
            }
        }
    if ( dimmingNeeded )
        {
        // effects found in IVT-data, hide the "no effects" -item
        aMenuPane->SetItemDimmed( aDimCommandId, ETrue );
        }
    }

// ---------------------------------------------------------
// Prints effect information got from getter functions
// ---------------------------------------------------------
//
void CHapticsTestAppUi::PrintEffectInfo( TInt aIndex )
    {
    if ( iUseHandle && iShowEffectInfo )
        {
        TInt duration;
        iHaptics->GetEffectDuration( iIVTFileHandle,
                                                   aIndex,
                                                   duration );

        HBufC8* tempBuf8 = HBufC8::New( iHaptics->MaxEffectNameLength() );
        TPtr8 effectName = tempBuf8->Des();

        iHaptics->GetEffectName( iIVTFileHandle, aIndex, effectName );
        
        TInt effectIndex;
        iHaptics->GetEffectIndexFromName( iIVTFileHandle,
                                          effectName, 
                                          effectIndex );

        TInt type;
        iHaptics->GetEffectType( iIVTFileHandle, aIndex, type );


        HBufC* tempBuf16 = HBufC::New( iHaptics->MaxEffectNameLength() );
        TPtr buf = tempBuf16->Des();
        
        iAppView->InsertOutput( _L("    Effect name: "), EFalse );
        buf.Copy( effectName );
        iAppView->InsertOutput( buf, EFalse );

        buf.Delete( 0, buf.MaxLength() );
        buf.AppendNum( effectIndex );
        iAppView->InsertOutput( _L("    Idx: "), EFalse );
        iAppView->InsertOutput( buf );

        buf.Delete( 0, buf.MaxLength() );
        buf.AppendNum( duration );
        iAppView->InsertOutput( _L("    Effect duration: "), EFalse );
        iAppView->InsertOutput( buf );

        buf.Delete( 0, buf.MaxLength() );
        buf.AppendNum( type );
        iAppView->InsertOutput( _L("    Effect type: "), EFalse );
        iAppView->InsertOutput( buf );

        if ( type == CHWRMHaptics::EHWRMHapticsTypeMagSweep )
            {
            CHWRMHaptics::THWRMHapticsMagSweepEffect def;
            iHaptics->GetMagSweepEffectDefinition( iIVTFileHandle,
                                                   aIndex,
                                                   def );
            iAppView->InsertOutput( _L("    MagSweep definition -> Duration:"), EFalse );
            buf.Delete( 0, buf.MaxLength() );
            buf.AppendNum( def.iDuration );
            iAppView->InsertOutput( buf );
            iAppView->InsertOutput( _L("    MagSweep definition -> Magnitude: "), EFalse );
            buf.Delete( 0, buf.MaxLength() );
            buf.AppendNum( def.iMagnitude );
            iAppView->InsertOutput( buf );
            iAppView->InsertOutput( _L("    MagSweep definition -> Style: "), EFalse );
            buf.Delete( 0, buf.MaxLength() );
            buf.AppendNum( def.iStyle );
            iAppView->InsertOutput( buf );
            iAppView->InsertOutput( _L("    MagSweep definition -> Attack time: "), EFalse );
            buf.Delete( 0, buf.MaxLength() );
            buf.AppendNum( def.iAttackTime );
            iAppView->InsertOutput( buf );
            iAppView->InsertOutput( _L("    MagSweep definition -> Attack level: "), EFalse );
            buf.Delete( 0, buf.MaxLength() );
            buf.AppendNum( def.iAttackLevel );
            iAppView->InsertOutput( buf );
            iAppView->InsertOutput( _L("    MagSweep definition -> Fade time: "), EFalse );
            buf.Delete( 0, buf.MaxLength() );
            buf.AppendNum( def.iFadeTime );
            iAppView->InsertOutput( buf );
            iAppView->InsertOutput( _L("    MagSweep definition -> Fade level: "), EFalse );
            buf.Delete( 0, buf.MaxLength() );
            buf.AppendNum( def.iFadeLevel );
            iAppView->InsertOutput( buf );
            }
        else if ( type == CHWRMHaptics::EHWRMHapticsTypePeriodic )
            {
            CHWRMHaptics::THWRMHapticsPeriodicEffect def;
            iHaptics->GetPeriodicEffectDefinition( iIVTFileHandle,
                                                   aIndex,
                                                   def );
            iAppView->InsertOutput( _L("    Periodic definition -> Duration: "), EFalse );
            buf.Delete( 0, buf.MaxLength() );
            buf.AppendNum( def.iDuration );
            iAppView->InsertOutput( buf );
            iAppView->InsertOutput( _L("    Periodic definition -> Magnitude: "), EFalse );
            buf.Delete( 0, buf.MaxLength() );
            buf.AppendNum( def.iMagnitude );
            iAppView->InsertOutput( buf );
            iAppView->InsertOutput( _L("    Periodic definition -> Period: "), EFalse );
            buf.Delete( 0, buf.MaxLength() );
            buf.AppendNum( def.iPeriod );
            iAppView->InsertOutput( buf );
            iAppView->InsertOutput( _L("    Periodic definition -> Style: "), EFalse );
            buf.Delete( 0, buf.MaxLength() );
            buf.AppendNum( def.iStyle );
            iAppView->InsertOutput( buf );
            iAppView->InsertOutput( _L("    Periodic definition -> Attack time: "), EFalse );
            buf.Delete( 0, buf.MaxLength() );
            buf.AppendNum( def.iAttackTime );
            iAppView->InsertOutput( buf );
            iAppView->InsertOutput( _L("    Periodic definition -> Attack level: "), EFalse );
            buf.Delete( 0, buf.MaxLength() );
            buf.AppendNum( def.iAttackLevel );
            iAppView->InsertOutput( buf );
            iAppView->InsertOutput( _L("    Periodic definition -> Fade time: "), EFalse );
            buf.Delete( 0, buf.MaxLength() );
            buf.AppendNum( def.iFadeTime );
            iAppView->InsertOutput( buf );
            iAppView->InsertOutput( _L("    Periodic definition -> Attack level: "), EFalse );
            buf.Delete( 0, buf.MaxLength() );
            buf.AppendNum( def.iFadeLevel );
            iAppView->InsertOutput( buf );
            }
        delete tempBuf8;
        delete tempBuf16;
        }
    }

// ---------------------------------------------------------
// Prints the given status onto the screen.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::PrintStatusInfo( 
    MHWRMHapticsObserver::THWRMHapticsStatus aStatus ) const
    {
    iAppView->InsertOutput( _L("Received status: "), EFalse );
    
    switch( aStatus )
        {
        case MHWRMHapticsObserver::EHWRMHapticsStatusAvailable:
            {
            iAppView->InsertOutput( _L("Available.") );
            break;
            }
        case MHWRMHapticsObserver::EHWRMHapticsStatusReserved:
            {
            iAppView->InsertOutput( _L("Reserved.") );
            break;
            }
        case MHWRMHapticsObserver::EHWRMHapticsStatusSuspended:
            {
            iAppView->InsertOutput( _L("Suspended.") );
            break;
            }
        default:
            {
            iAppView->InsertOutput( _L("<unknown status>") );
            break;
            }
        }
    }

// ---------------------------------------------------------
// Calls the getter methods for system constants
// and prints out the results.
// ---------------------------------------------------------
//
void CHapticsTestAppUi::ConstGetters() const
    {
    _LIT( KNumFormat, "%d" );
    TBuf<50> buf;
    iAppView->InsertOutput( _L("    Infinite repeats: "), EFalse );
    buf.Format( KNumFormat, iHaptics->InfiniteRepeat() );
    iAppView->InsertOutput( buf );
    iAppView->InsertOutput( _L("    Infinite duration: "), EFalse );
    buf.Format( KNumFormat, iHaptics->InfiniteDuration() );
    iAppView->InsertOutput( buf );
    iAppView->InsertOutput( _L("    Max effect name length: "), EFalse );
    buf.Format( KNumFormat, iHaptics->MaxEffectNameLength() );
    iAppView->InsertOutput( buf );
    iAppView->InsertOutput( _L("    Max device name length: "), EFalse );
    buf.Format( KNumFormat, iHaptics->MaxDeviceNameLength() );
    iAppView->InsertOutput( buf );
    iAppView->InsertOutput( _L("    Max capability string length: "), EFalse );
    buf.Format( KNumFormat, iHaptics->MaxCapabilityStringLength() );
    iAppView->InsertOutput( buf );
    iAppView->InsertOutput( _L("    Max property string length: "), EFalse );
    buf.Format( KNumFormat, iHaptics->MaxPropertyStringLength() );
    iAppView->InsertOutput( buf );
    iAppView->InsertOutput( _L("    Max streaming sample size: "), EFalse );
    buf.Format( KNumFormat, iHaptics->MaxStreamingSampleSize() );
    iAppView->InsertOutput( buf );
    iAppView->InsertOutput( _L("    Default device priority: "), EFalse );
    buf.Format( KNumFormat, iHaptics->DefaultDevicePriority() );
    iAppView->InsertOutput( buf );
    }

// ---------------------------------------------------------------------------
// Modifies an effect using the values set for iModifyEffect.
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::ModifyEffect()
    {
    if ( !iEffectData->ModifiableEffectExists() )
        {
        return;
        }

    TInt err = KErrNone;
    TInt effectHandle = iEffectData->ModifiableEffectHandle();
    
    if ( iEffectData->ModifiableEffectIsMagSweep() )
        {
        // fill magsweep data
        CHWRMHaptics::THWRMHapticsMagSweepEffect magsweep;
        iEffectData->FillModifiableMagSweepData( magsweep );

        // execute modification
        if ( iSynchronous )
            {
            err = iHaptics->ModifyPlayingMagSweepEffect( effectHandle, magsweep );
            }
        else
            {
            TRequestStatus status;
            iHaptics->ModifyPlayingMagSweepEffect( effectHandle, magsweep, status );
            User::WaitForRequest( status );
            err = status.Int();
            }
        }
    else if ( iEffectData->ModifiableEffectIsPeriodic() )
        {
        // fill periodic data
        CHWRMHaptics::THWRMHapticsPeriodicEffect periodic;
        iEffectData->FillModifiablePeriodicData( periodic );

        // execute modification
        if ( iSynchronous )
            {
            err = iHaptics->ModifyPlayingPeriodicEffect( effectHandle, periodic );
            }
        else
            {
            TRequestStatus status;
            iHaptics->ModifyPlayingPeriodicEffect( effectHandle, periodic, status );
            User::WaitForRequest( status );
            err = status.Int();
            }
        }
    
    if ( err )
        {
        TBuf<8> errBuf;
        errBuf.AppendNum( err );
        iAppView->InsertOutput( _L("Modifying effect failed, err = "), EFalse );
        iAppView->InsertOutput( errBuf );
        }
    else
        {
        iAppView->InsertOutput( _L("Modifying effect succeeded.") );
        }
    }

// ---------------------------------------------------------------------------
// Static callback method from a periodic timer used for
// continuous effect modification (magnitude/period).
// ---------------------------------------------------------------------------
//
TInt CHapticsTestAppUi::ModifyPlayingEffect( TAny* aPtr )
    {
    CHapticsTestEffectDataHandler* effectData = 
        static_cast<CHapticsTestEffectDataHandler*>( aPtr );

    // hundred modifications to get to KPi (up and down with magnitude)
    TReal stepLengthRad = KPi/100 * effectData->ModifyCount();

    // calculate absolute sin value
    TReal sinValue = 0;
    TInt errSin = Math::Sin( sinValue, stepLengthRad );
    sinValue = Abs( sinValue );

    // increase the amount of modifications done
    effectData->SetModifyCount( effectData->ModifyCount() + 1 );
    
    TInt err = KErrNone;
    TInt effectHandle = effectData->ModifiableEffectHandle();
    
    if ( effectData->ModifiableEffectIsMagSweep() )
        {
        // modify magsweep effect
        CHWRMHaptics::THWRMHapticsMagSweepEffect magsweep;
        effectData->FillModifiableMagSweepData( magsweep );

        // new magnitude value according to sin value        
        magsweep.iMagnitude = KHWRMHapticsMaxMagnitude * sinValue;
        
        // modify effect
        err = effectData->Haptics()->ModifyPlayingMagSweepEffect( 
              effectHandle, magsweep );
        }
    else if ( effectData->ModifiableEffectIsPeriodic() )
        {
        // modify periodic effect
        CHWRMHaptics::THWRMHapticsPeriodicEffect periodic;
        effectData->FillModifiablePeriodicData( periodic );

        // period getting smaller when magnitude getting higher
        periodic.iPeriod = 20 + (1-sinValue)*200;

        // modify effect
        err = effectData->Haptics()->ModifyPlayingPeriodicEffect(
              effectHandle, periodic );
        }
    
    if ( err )
        {
        CEikonEnv::Static()->InfoMsg( _L("Modify failed!") );
        }

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::GetCapabilityDeviceCategory()
    {
    TBuf<50> buf;
    iAppView->InsertOutput( _L("    Device category: "), EFalse );
    TInt retValue( CHWRMHaptics::EHWRMHapticsEmbedded );
    TInt err( iHaptics->GetDeviceCapability(
        CHWRMHaptics::EHWRMHapticsDeviceCategory, retValue ) );

    if ( err )
        {
        _LIT( KFailedFormat, "GetDeviceCapability failed (%d)!" );
        buf.Format( KFailedFormat, err );
        }
    else
        {
        switch ( retValue )
            {
            case CHWRMHaptics::EHWRMHapticsVirtual: 
                buf.Append( _L("Virtual") );
                break;
            case CHWRMHaptics::EHWRMHapticsEmbedded: 
                buf.Append( _L("Embedded") );
                break;
            default:
                buf.Append( _L("Unknown") );
                break;
            }
        }
    iAppView->InsertOutput( buf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::GetCapabilityMaxNestedRepeats()
    {
    TBuf<50> buf;
    iAppView->InsertOutput( _L("    Max nested repeats: "), EFalse );
    TInt retValue( 0 );
    TInt err( iHaptics->GetDeviceCapability( 
        CHWRMHaptics::EHWRMHapticsMaxNestedRepeats, retValue ) );

    if ( err )
        {
        _LIT( KFailedFormat, "GetDeviceCapability failed (%d)!" );
        buf.Format( KFailedFormat, err );
        }
    else
        {
        _LIT( KNumFormat, "%d" );
        buf.Format( KNumFormat, retValue );
        }

    iAppView->InsertOutput( buf );
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::GetCapabilityNumActuators()
    {
    TBuf<50> buf;
    iAppView->InsertOutput( _L("    Number of actuators: "), EFalse );
    TInt retValue( 0 );
    TInt err( iHaptics->GetDeviceCapability(
        CHWRMHaptics::EHWRMHapticsNumActuators, retValue ) );

    if ( err )
        {
        _LIT( KFailedFormat, "GetDeviceCapability failed (%d)!" );
        buf.Format( KFailedFormat, err );
        }
    else
        {
        _LIT( KNumFormat, "%d" );
        buf.Format( KNumFormat, retValue );
        }

    iAppView->InsertOutput( buf );
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::GetCapabilityActuatorType() 
    {
    TBuf<50> buf;
    iAppView->InsertOutput( _L("    Actuator type: "), EFalse );
    TInt retValue( 0 );
    TInt err( iHaptics->GetDeviceCapability( 
        CHWRMHaptics::EHWRMHapticsActuatorType, retValue ) );
        
    if ( err )
        {
        _LIT( KFailedFormat, "GetDeviceCapability failed (%d)!" );
        buf.Format( KFailedFormat, err );
        }
    else
        {
        switch (retValue)
            {
            case CHWRMHaptics::EHWRMHapticsTypeERM: 
                buf.Append( _L("EHWRMHapticsTypeERM") );
                break;
            case CHWRMHaptics::EHWRMHapticsTypeLRA: 
                buf.Append( _L("EHWRMHapticsTypeLRA") );
                break;
            default:
                buf.Append( _L("Unknown") );
                break;
            }
        }
    iAppView->InsertOutput( buf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::GetCapabilityNumEffectSlots()
    {
    TBuf<50> buf;
    iAppView->InsertOutput( _L("    Number of effect slots: "), EFalse );
    TInt retValue( 0 );
    TInt err( iHaptics->GetDeviceCapability(
        CHWRMHaptics::EHWRMHapticsNumEffectSlots, retValue ) );

    if ( err )
        {
        _LIT( KFailedFormat, "GetDeviceCapability failed (%d)!" );
        buf.Format( KFailedFormat, err );
        }
    else
        {
        _LIT( KNumFormat, "%d" );
        buf.Format( KNumFormat, retValue );
        }

    iAppView->InsertOutput( buf );
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::GetCapabilityNumEffectStyles()
    {
    TBuf<150> buf;
    iAppView->InsertOutput( _L("    Effect styles: "), EFalse );
    TInt retValue( 0 );
    TInt err( iHaptics->GetDeviceCapability(
        CHWRMHaptics::EHWRMHapticsSupportedStyles, retValue ) );

    if ( err )
        {
        _LIT( KFailedFormat, "GetDeviceCapability failed (%d)!" );
        buf.Format( KFailedFormat, err );
        }
    else
        {
        if ( retValue & CHWRMHaptics::EHWRMHapticsSupportSmooth )
            {
            buf.Append( _L(" EHWRMHapticsSupportSmooth") );
            }

        if ( retValue & CHWRMHaptics::EHWRMHapticsSupportStrong )
            {
            buf.Append( _L(" EHWRMHapticsSupportStrong") );
            }

        if ( retValue & CHWRMHaptics::EHWRMHapticsSupportSharp )
            {
            buf.Append( _L(" EHWRMHapticsSupportSharp") );
            }
        }
    iAppView->InsertOutput( buf );    
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::GetCapabilityMinPeriod()
    {
    TBuf<50> buf;
    iAppView->InsertOutput( _L("    Minimum period for periodic effects: "), EFalse );
    TInt retValue( 0 );
    TInt err( iHaptics->GetDeviceCapability( 
        CHWRMHaptics::EHWRMHapticsMinPeriod, retValue ) );

    if ( err )
        {
        _LIT( KFailedFormat, "GetDeviceCapability failed (%d)!" );
        buf.Format( KFailedFormat, err );
        }
    else
        {
        _LIT( KNumFormat, "%d" );
        buf.Format( KNumFormat, retValue );
        }

    iAppView->InsertOutput( buf );
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::GetCapabilityMaxPeriod()
    {
    TBuf<50> buf;
    iAppView->InsertOutput( _L("    Maximum period for periodic effects: "), EFalse );
    TInt retValue( 0 );
    TInt err( iHaptics->GetDeviceCapability( 
        CHWRMHaptics::EHWRMHapticsMaxPeriod, retValue ) );

    if ( err )
        {
        _LIT( KFailedFormat, "GetDeviceCapability failed (%d)!" );
        buf.Format( KFailedFormat, err );
        }
    else
        {
        _LIT( KNumFormat, "%d" );
        buf.Format( KNumFormat, retValue );
        }

    iAppView->InsertOutput( buf );
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::GetCapabilityMaxEffectDuration()
    {
    TBuf<50> buf;
    iAppView->InsertOutput( _L("    Maximum effect duration: "), EFalse );
    TInt retValue( 0 );
    TInt err( iHaptics->GetDeviceCapability(
        CHWRMHaptics::EHWRMHapticsMaxEffectDuration, retValue ) );

    if ( err )
        {
        _LIT( KFailedFormat, "GetDeviceCapability failed (%d)!" );
        buf.Format( KFailedFormat, err );
        }
    else
        {
        _LIT( KNumFormat, "%d" );
        buf.Format( KNumFormat, retValue );
        }

    iAppView->InsertOutput( buf );
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::GetCapabilitySupportedEffects()
    {
    TBuf<150> buf;
    iAppView->InsertOutput( _L("    Supported effect types: "), EFalse );
    TInt retValue( 0 );
    TInt err( iHaptics->GetDeviceCapability(
        CHWRMHaptics::EHWRMHapticsSupportedEffects, retValue ) );
        
    if ( err )
        {
        _LIT( KFailedFormat, "GetDeviceCapability failed (%d)!" );
        buf.Format( KFailedFormat, err );
        }
    else
        {
        if ( retValue & CHWRMHaptics::EHWRMHapticsSupportPeriodic )
            {
            buf.Append( _L(" EHWRMHapticsSupportPeriodic") );
            }

        if ( retValue & CHWRMHaptics::EHWRMHapticsSupportMagSweep )
            {
            buf.Append( _L(" EHWRMHapticsSupportMagSweep") );
            }

        if ( retValue & CHWRMHaptics::EHWRMHapticsSupportTimeline )
            {
            buf.Append( _L(" EHWRMHapticsSupportTimeline") );
            }

        if ( retValue & CHWRMHaptics::EHWRMHapticsSupportStreaming )
            {
            buf.Append( _L(" EHWRMHapticsSupportStreaming") );
            }
        }
    iAppView->InsertOutput( buf );    
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::GetCapabilityDeviceName()
    {
    TBuf<100> buf;
    iAppView->InsertOutput( _L("    Device name: "), EFalse );
    TBuf8<100> retValue;
    TInt err( iHaptics->GetDeviceCapability(
        CHWRMHaptics::EHWRMHapticsDeviceName, retValue ) );
    if ( err )
        {
        _LIT( KFailedFormat, "GetDeviceCapability failed (%d)!" );
        buf.Format( KFailedFormat, err );
        }
    else
        {
        CnvUtfConverter::ConvertToUnicodeFromUtf8( buf, retValue );
        }
        
    iAppView->InsertOutput( buf );
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::GetCapabilityMaxEnvelopeTime()
    {    
    TBuf<50> buf;
    iAppView->InsertOutput( _L("    Maximum envelop time: "), EFalse );
    TInt retValue( 0 );
    TInt err( iHaptics->GetDeviceCapability(
        CHWRMHaptics::EHWRMHapticsMaxEnvelopeTime, retValue ) );

    if ( err )
        {
        _LIT( KFailedFormat, "GetDeviceCapability failed (%d)!" );
        buf.Format( KFailedFormat, err );
        }
    else
        {
        _LIT( KNumFormat, "%d" );
        buf.Format( KNumFormat, retValue );
        }
        
    iAppView->InsertOutput( buf );
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::GetCapabilityAPIVersionNumber()
    {
    TBuf<50> buf;
    TInt retValue( 0 );
    iAppView->InsertOutput( _L("    API version: "), EFalse );
    TInt err( iHaptics->GetDeviceCapability(
        CHWRMHaptics::EHWRMHapticsAPIVersionNumber, retValue ) );

    if ( err )
        {
        _LIT( KFailedFormat, "GetDeviceCapability failed (%d)!" );
        buf.Format( KFailedFormat, err );
        }
    else
        {
        _LIT( KNumFormat, "%d" );
        buf.Format( KNumFormat, retValue );
        }

    iAppView->InsertOutput( buf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::GetCapabilityMaxIVTSize()
    {
    TBuf<50> buf;
    iAppView->InsertOutput( _L("    Maximum size of IVT file: "), EFalse );
    TInt retValue( 0 );
    TInt err( iHaptics->GetDeviceCapability(
        CHWRMHaptics::EHWRMHapticsMaxEffectDataSize, retValue ) );

    if ( err )
        {
        _LIT( KFailedFormat, "GetDeviceCapability failed (%d)!" );
        buf.Format( KFailedFormat, err );
        }
    else
        {
        _LIT( KNumFormat, "%d" );
        buf.Format( KNumFormat, retValue );
        }

    iAppView->InsertOutput( buf, ETrue );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::SetPropertyPriorityMin()
    {
    iAppView->InsertOutput( _L("    Setting priority to minimum"), ETrue );
    TInt value(KHWRMHapticsMinDevicePriority);
    TRAPD( err, iHaptics->SetDeviceProperty(CHWRMHaptics::EHWRMHapticsPriority, value));
    if ( err )
        {
        CEikonEnv::Static()->InfoMsg( _L("SetDeviceProperty failed!") );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::SetPropertyPriorityDefault()
    {
    iAppView->InsertOutput( _L("    Setting priority to default" ), ETrue );
    TInt value( KHWRMHapticsMinDevicePriority );
    
    TRAPD( err1, value = iHaptics->DefaultDevicePriority() );
    if ( err1 )
        {
        CEikonEnv::Static()->InfoMsg( _L( "DefaultDevicePriority() failed!" ) );
        return;
        }
    
    TRAPD( err2, iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsPriority, value ) );
    if ( err2 )
        {
        CEikonEnv::Static()->InfoMsg( _L("SetDeviceProperty failed!") );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::SetPropertyPriorityMax()
    {
    iAppView->InsertOutput( _L("    Setting priority to maximum"), ETrue );
    TInt value(KHWRMHapticsMaxDevicePriority);
    TRAPD( err, iHaptics->SetDeviceProperty(CHWRMHaptics::EHWRMHapticsPriority, value));
    if ( err )
        {
        CEikonEnv::Static()->InfoMsg( _L("SetDeviceProperty failed!") );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::GetPropertyPriority()
    {
    _LIT( KNumFormat, "%d" );
    TBuf<50> buf;
    iAppView->InsertOutput( _L("    Device priority: "), EFalse );
    TInt retValue(0);
    TRAPD( err, iHaptics->GetDeviceProperty(CHWRMHaptics::EHWRMHapticsPriority, retValue));
    if ( err )
        {
        CEikonEnv::Static()->InfoMsg( _L("GetDeviceProperty failed!") );
        }

    buf.Format( KNumFormat, retValue );
    iAppView->InsertOutput( buf, ETrue );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::SetPropertyDisableEffectsTrue()
    {
    iAppView->InsertOutput( _L("    Disabling effects" ), ETrue );
    TBool value( ETrue );
    TRAPD( err2, iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsDisableEffects, value ) );
    if ( err2 )
        {
        CEikonEnv::Static()->InfoMsg( _L("SetDeviceProperty failed!") );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::SetPropertyDisableEffectsFalse()
    {
    iAppView->InsertOutput( _L("    Enabling effects" ), ETrue );
    TBool value( EFalse );
    TRAPD( err2, iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsDisableEffects, value ) );
    if ( err2 )
        {
        CEikonEnv::Static()->InfoMsg( _L("SetDeviceProperty failed!") );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::GetPropertyDisableEffects()
    {
    iAppView->InsertOutput( _L("    Effects disabled: "), EFalse );
    TInt retValue( EFalse );
    TRAPD( err, iHaptics->GetDeviceProperty( CHWRMHaptics::EHWRMHapticsDisableEffects, retValue ) );
    if ( err )
        {
        CEikonEnv::Static()->InfoMsg( _L("GetDeviceProperty failed!") );
        }
    else 
        {
        if ( retValue )
            {
            iAppView->InsertOutput( _L("TRUE"), ETrue );
            }
        else
            {
            iAppView->InsertOutput( _L("FALSE"), ETrue );
            }
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::SetPropertyStrengthMin()
    {
    iAppView->InsertOutput( _L("    Setting strength to mininum" ), ETrue );
    TBool value( KHWRMHapticsMinStrength );
    TRAPD( err2, iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsStrength, value ) );
    if ( err2 )
        {
        CEikonEnv::Static()->InfoMsg( _L("SetDeviceProperty failed!") );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::SetPropertyStrengthMiddle()
    {
    iAppView->InsertOutput( _L("    Setting strength to middle" ), ETrue );
    TBool value( ( KHWRMHapticsMaxStrength - KHWRMHapticsMinStrength ) / 2 );
    TRAPD( err2, iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsStrength, value ) );
    if ( err2 )
        {
        CEikonEnv::Static()->InfoMsg( _L("SetDeviceProperty failed!") );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::SetPropertyStrengthMax()
    {
    iAppView->InsertOutput( _L("    Setting strength to max" ), ETrue );
    TBool value( KHWRMHapticsMaxStrength );
    TRAPD( err2, iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsStrength, value ) );
    if ( err2 )
        {
        CEikonEnv::Static()->InfoMsg( _L("SetDeviceProperty failed!") );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::GetPropertyStrength()
    {
    _LIT( KNumFormat, "%d" );
    TBuf<50> buf;
    iAppView->InsertOutput( _L("    Strength level: "), EFalse );
    TInt retValue(0);
    TRAPD( err, iHaptics->GetDeviceProperty(CHWRMHaptics::EHWRMHapticsStrength, retValue));
    if ( err )
        {
        CEikonEnv::Static()->InfoMsg( _L("GetDeviceProperty failed!") );
        }

    buf.Format( KNumFormat, retValue );
    iAppView->InsertOutput( buf, ETrue );
    }


// ---------------------------------------------------------------------------
// Enables streaming mode.
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::StartStreaming()
    {
    iCurrentSample = 0;
    iEffectHandle = KErrNotFound;
    TInt err = iHaptics->CreateStreamingEffect( iEffectHandle );

    if ( err )
        {
        TBuf<16> errCode;
        errCode.AppendNum( err );
        iAppView->InsertOutput( _L("Create streaming effect failed! Err: "), EFalse );
        iAppView->InsertOutput( errCode );
        }
    else
        {
        TBuf<16> handleBuf;
        handleBuf.AppendNum( iEffectHandle );
        iAppView->InsertOutput( _L("Streaming effect created successfully!") );
        iAppView->InsertOutput( _L("    Effect handle: "), EFalse );
        iAppView->InsertOutput( handleBuf );

        iStreaming = ETrue;
        }
    }


// ---------------------------------------------------------------------------
// Plays one streaming sample from KStreamArray.
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::PlayStreamingSample( TInt aSampleId )
    {
    RBuf8 sampleBuffer;
    sampleBuffer.ReAlloc( sizeof( KStreamArray[aSampleId] ) );
    TUint8* tmpPtr = const_cast<TUint8*>( sampleBuffer.Ptr() );
    memcpy( tmpPtr, KStreamArray[aSampleId], sizeof( KStreamArray[aSampleId] ) );
    sampleBuffer.SetLength( sizeof( KStreamArray[aSampleId] ) );

    iHaptics->PlayStreamingSample( iEffectHandle, sampleBuffer );
                                         
    sampleBuffer.Close();
    }


// ---------------------------------------------------------------------------
// Disables streaming mode.
// ---------------------------------------------------------------------------
//
void CHapticsTestAppUi::StopStreaming()
    {
    iStreaming = EFalse;

    TInt err = iHaptics->DestroyStreamingEffect( iEffectHandle );            
    if ( err )
        {
        TBuf<16> errCode;
        errCode.AppendNum( err );
        iAppView->InsertOutput( _L("Destoy streaming effect failed! Err: "), EFalse );
        iAppView->InsertOutput( errCode );
        }
    else
        {
        TBuf<16> handleBuf;
        handleBuf.AppendNum( iEffectHandle );
        iAppView->InsertOutput( _L("Streaming effect destroyed successfully!") );
        }
    }
    
// End of file
