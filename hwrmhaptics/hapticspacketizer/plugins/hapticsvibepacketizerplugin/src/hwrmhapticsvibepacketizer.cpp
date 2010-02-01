/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Plugin for handling vibe command packages.
*
*/

#include <ecom/implementationproxy.h>
#include <badesca.h> 
#include <hwrmhaptics.h>

#include "hwrmhapticsvibepacketizer.h"
#include "hwrmhapticsvibepackets.h"
#include "hwrmhapticsvibeconstants.h"

// Default request message size (enough for any request message)
const TInt KMsgDefaultSize = 80;

// const for shifts
const TInt KShiftByte = 8;
const TInt KShift2Bytes = 16;
const TInt KShift3Bytes = 24;

const TInt KArrayElements = 10;

// ---------------------------------------------------------------------------
// Two phased constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsVibePacketizer* CHWRMHapticsVibePacketizer::NewL()
    {
    CHWRMHapticsVibePacketizer* self = 
        new ( ELeave ) CHWRMHapticsVibePacketizer();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsVibePacketizer::~CHWRMHapticsVibePacketizer()
    {
    iReqBuf.Close();
    iDataBuf.Close();

    if( iReturnArray )
        {
        delete iReturnArray;
        iReturnArray = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsVibePacketizer::CHWRMHapticsVibePacketizer()
    : iDeviceHandle( KErrNotFound )
    {
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsVibePacketizer::ConstructL()
    {
    User::LeaveIfError( iReqBuf.CreateMax( KMsgDefaultSize ) ); 
    User::LeaveIfError( iDataBuf.CreateMax( KMsgDefaultSize ) );

    iReturnArray = new (ELeave) CDesC8ArraySeg( KArrayElements );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncOpenDeviceReq( 
    THWRMLogicalActuators aLogicalActuator, RBuf8& aBuffer )
    {
    TVibePacketOpenDeviceRequest* pReq = 
        reinterpret_cast<TVibePacketOpenDeviceRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );

    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdOpenDevice;
        pReq->iDeviceIndex_7_0 = 
            static_cast<TUint8>( aLogicalActuator & 0xFF );
        pReq->iDeviceIndex_15_8 = 
            static_cast<TUint8>( ( aLogicalActuator >> KShiftByte ) & 0xFF );
        pReq->iDeviceIndex_23_16 = 
            static_cast<TUint8>( 
                ( aLogicalActuator >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceIndex_31_24 = 
            static_cast<TUint8>( 
                ( aLogicalActuator >> KShift3Bytes ) & 0xFF );
        iReqBuf.SetLength( sizeof ( TVibePacketOpenDeviceRequest ) );
        }
    else
        {
        iReqBuf.SetLength( 0 );
        }

    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncCloseDeviceReq( TInt aDeviceHandle, 
                                                    RBuf8& aBuffer )
    {
    TVibePacketCloseDeviceRequest* pReq = 
        reinterpret_cast<TVibePacketCloseDeviceRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );

    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdCloseDevice;
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes) & 0xFF );
        iReqBuf.SetLength( sizeof( TVibePacketCloseDeviceRequest ) );
        }
    else
        {
        iReqBuf.SetLength( 0 );    
        }    

    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncPlayMagSweepEffectReq( 
    TInt aDeviceHandle, CHWRMHaptics::THWRMHapticsMagSweepEffect aEffect, 
    RBuf8& aBuffer )
    {
    TVibePacketPlayBasisEffectRequest* pReq = 
        reinterpret_cast<TVibePacketPlayBasisEffectRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );
    
    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdPlayMagSweepEffect;
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        pReq->iDuration_7_0 = 
            static_cast<TUint8>( aEffect.iDuration & 0xFF );
        pReq->iDuration_15_8 = 
            static_cast<TUint8>( ( aEffect.iDuration >> KShiftByte ) & 0xFF );
        pReq->iDuration_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iDuration >> KShift2Bytes ) & 0xFF );
        pReq->iDuration_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iDuration >> KShift3Bytes ) & 0xFF );
        pReq->iMagnitude_7_0 = 
            static_cast<TUint8>( aEffect.iMagnitude & 0xFF );
        pReq->iMagnitude_15_8 = 
            static_cast<TUint8>( 
                ( aEffect.iMagnitude >> KShiftByte ) & 0xFF );
        pReq->iMagnitude_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iMagnitude >> KShift2Bytes ) & 0xFF );
        pReq->iMagnitude_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iMagnitude >> KShift3Bytes ) & 0xFF );
        pReq->iEffectTypeStyle = 
            static_cast<TUint8>( ( aEffect.iStyle << 4 ) & 0xF0 );
        pReq->iPeriod_7_0 = 0x0;
        pReq->iPeriod_15_8 = 0x0;
        pReq->iPeriod_23_16 = 0x0;
        pReq->iPeriod_31_24 = 0x0;
        pReq->iImpulseTime_7_0 = 
            static_cast<TUint8>( aEffect.iAttackTime & 0xFF );
        pReq->iImpulseTime_15_8 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackTime >> KShiftByte ) & 0xFF );
        pReq->iImpulseTime_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackTime >> KShift2Bytes ) & 0xFF );
        pReq->iImpulseTime_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackTime >> KShift3Bytes ) & 0xFF );
        pReq->iImpulseLevel_7_0 = 
            static_cast<TUint8>( aEffect.iAttackLevel & 0xFF );
        pReq->iImpulseLevel_15_8 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackLevel >> KShiftByte ) & 0xFF );
        pReq->iImpulseLevel_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackLevel >> KShift2Bytes ) & 0xFF );
        pReq->iImpulseLevel_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackLevel >> KShift3Bytes ) & 0xFF );
        pReq->iFadeTime_7_0 = 
            static_cast<TUint8>( aEffect.iFadeTime & 0xFF );
        pReq->iFadeTime_15_8 = 
            static_cast<TUint8>( ( aEffect.iFadeTime >> KShiftByte ) & 0xFF );
        pReq->iFadeTime_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeTime >> KShift2Bytes ) & 0xFF );
        pReq->iFadeTime_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeTime >> KShift3Bytes ) & 0xFF );
        pReq->iFadeLevel_7_0 = 
            static_cast<TUint8>( aEffect.iFadeLevel & 0xFF );
        pReq->iFadeLevel_15_8 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeLevel >> KShiftByte ) & 0xFF );
        pReq->iFadeLevel_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeLevel >> KShift2Bytes ) & 0xFF );
        pReq->iFadeLevel_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeLevel >> KShift3Bytes ) & 0xFF );
        iReqBuf.SetLength( sizeof( TVibePacketPlayBasisEffectRequest ) );
        }
    else
        {
        iReqBuf.SetLength( 0 );    
        }

    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncPlayPeriodicEffectReq( TInt aDeviceHandle,
    CHWRMHaptics::THWRMHapticsPeriodicEffect aEffect, RBuf8& aBuffer )
    {
    TVibePacketPlayBasisEffectRequest* pReq = 
        reinterpret_cast<TVibePacketPlayBasisEffectRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );

    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdPlayPeriodicEffect;
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        pReq->iDuration_7_0 = 
            static_cast<TUint8>( aEffect.iDuration & 0xFF );
        pReq->iDuration_15_8 = 
            static_cast<TUint8>( ( aEffect.iDuration >> KShiftByte ) & 0xFF );
        pReq->iDuration_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iDuration >> KShift2Bytes ) & 0xFF );
        pReq->iDuration_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iDuration >> KShift3Bytes ) & 0xFF );
        pReq->iMagnitude_7_0 = 
            static_cast<TUint8>( aEffect.iMagnitude & 0xFF );
        pReq->iMagnitude_15_8 = 
            static_cast<TUint8>(    
                ( aEffect.iMagnitude >> KShiftByte ) & 0xFF );
        pReq->iMagnitude_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iMagnitude >> KShift2Bytes ) & 0xFF );
        pReq->iMagnitude_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iMagnitude >> KShift3Bytes ) & 0xFF );
        pReq->iEffectTypeStyle = 
            static_cast<TUint8>( ( ( aEffect.iStyle << 4 ) & 0xF0 ) | 0x01 );
        pReq->iPeriod_7_0 = 
            static_cast<TUint8>( aEffect.iPeriod & 0xFF );
        pReq->iPeriod_15_8 = 
            static_cast<TUint8>( ( aEffect.iPeriod >> KShiftByte ) & 0xFF );
        pReq->iPeriod_23_16 = 
            static_cast<TUint8>( ( aEffect.iPeriod >> KShift2Bytes ) & 0xFF );
        pReq->iPeriod_31_24 = 
            static_cast<TUint8>( ( aEffect.iPeriod >> KShift3Bytes ) & 0xFF );
        pReq->iImpulseTime_7_0 = 
            static_cast<TUint8>( aEffect.iAttackTime & 0xFF );
        pReq->iImpulseTime_15_8 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackTime >> KShiftByte ) & 0xFF );
        pReq->iImpulseTime_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackTime >> KShift2Bytes ) & 0xFF );
        pReq->iImpulseTime_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackTime >> KShift3Bytes ) & 0xFF );
        pReq->iImpulseLevel_7_0 = 
            static_cast<TUint8>( aEffect.iAttackLevel & 0xFF );
        pReq->iImpulseLevel_15_8 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackLevel >> KShiftByte ) & 0xFF );
        pReq->iImpulseLevel_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackLevel >> KShift2Bytes ) & 0xFF );
        pReq->iImpulseLevel_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackLevel >> KShift3Bytes ) & 0xFF );
        pReq->iFadeTime_7_0 = 
            static_cast<TUint8>( aEffect.iFadeTime & 0xFF );
        pReq->iFadeTime_15_8 = 
            static_cast<TUint8>( ( aEffect.iFadeTime >> KShiftByte ) & 0xFF );
        pReq->iFadeTime_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeTime >> KShift2Bytes ) & 0xFF );
        pReq->iFadeTime_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeTime >> KShift3Bytes ) & 0xFF );
        pReq->iFadeLevel_7_0 = 
            static_cast<TUint8>( aEffect.iFadeLevel & 0xFF );
        pReq->iFadeLevel_15_8 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeLevel >> KShiftByte ) & 0xFF );
        pReq->iFadeLevel_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeLevel >> KShift2Bytes ) & 0xFF );
        pReq->iFadeLevel_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeLevel >> KShift3Bytes ) & 0xFF );
        iReqBuf.SetLength( sizeof( TVibePacketPlayBasisEffectRequest ) );
        }
    else
        {
        iReqBuf.SetLength( 0 );    
        }
    
    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncModifyPlayingMagSweepEffectReq( 
    TInt aDeviceHandle, TInt aEffectHandle, 
    CHWRMHaptics::THWRMHapticsMagSweepEffect aEffect, RBuf8& aBuffer )
    {
    TVibePacketModifyBasisEffectRequest* pReq = 
        reinterpret_cast<TVibePacketModifyBasisEffectRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );

    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdModifyPlayingMagSweepEffect;
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        pReq->iEffectHandle_7_0 = 
            static_cast<TUint8>( aEffectHandle & 0xFF );
        pReq->iEffectHandle_15_8 = 
            static_cast<TUint8>( ( aEffectHandle >> KShiftByte ) & 0xFF );
        pReq->iEffectHandle_23_16 = 
            static_cast<TUint8>( ( aEffectHandle >> KShift2Bytes ) & 0xFF );
        pReq->iEffectHandle_31_24 = 
            static_cast<TUint8>( ( aEffectHandle >> KShift3Bytes ) & 0xFF );
        pReq->iDuration_7_0 = 
            static_cast<TUint8>( aEffect.iDuration & 0xFF );
        pReq->iDuration_15_8 = 
            static_cast<TUint8>( ( aEffect.iDuration >> KShiftByte ) & 0xFF );
        pReq->iDuration_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iDuration >> KShift2Bytes ) & 0xFF );
        pReq->iDuration_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iDuration >> KShift3Bytes ) & 0xFF );
        pReq->iMagnitude_7_0 = 
            static_cast<TUint8>( aEffect.iMagnitude & 0xFF );
        pReq->iMagnitude_15_8 = 
            static_cast<TUint8>( 
                ( aEffect.iMagnitude >> KShiftByte ) & 0xFF );
        pReq->iMagnitude_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iMagnitude >> KShift2Bytes ) & 0xFF );
        pReq->iMagnitude_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iMagnitude >> KShift3Bytes ) & 0xFF );
        pReq->iEffectTypeStyle = 
            static_cast<TUint8>( ( aEffect.iStyle << 4 ) & 0xF0 );
        pReq->iPeriod_7_0 = 0x0;
        pReq->iPeriod_15_8 = 0x0;
        pReq->iPeriod_23_16 = 0x0;
        pReq->iPeriod_31_24 = 0x0;
        pReq->iImpulseTime_7_0 = 
            static_cast<TUint8>( aEffect.iAttackTime & 0xFF );
        pReq->iImpulseTime_15_8 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackTime >> KShiftByte ) & 0xFF );
        pReq->iImpulseTime_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackTime >> KShift2Bytes ) & 0xFF );
        pReq->iImpulseTime_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackTime >> KShift3Bytes ) & 0xFF );
        pReq->iImpulseLevel_7_0 = 
            static_cast<TUint8>( aEffect.iAttackLevel & 0xFF );
        pReq->iImpulseLevel_15_8 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackLevel >> KShiftByte ) & 0xFF );
        pReq->iImpulseLevel_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackLevel >> KShift2Bytes ) & 0xFF );
        pReq->iImpulseLevel_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackLevel >> KShift3Bytes ) & 0xFF );
        pReq->iFadeTime_7_0 = 
            static_cast<TUint8>( aEffect.iFadeTime & 0xFF );
        pReq->iFadeTime_15_8 = 
            static_cast<TUint8>( ( aEffect.iFadeTime >> KShiftByte ) & 0xFF );
        pReq->iFadeTime_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeTime >> KShift2Bytes ) & 0xFF );
        pReq->iFadeTime_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeTime >> KShift3Bytes ) & 0xFF );
        pReq->iFadeLevel_7_0 = 
            static_cast<TUint8>( aEffect.iFadeLevel & 0xFF );
        pReq->iFadeLevel_15_8 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeLevel >> KShiftByte ) & 0xFF );
        pReq->iFadeLevel_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeLevel >> KShift2Bytes ) & 0xFF );
        pReq->iFadeLevel_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeLevel >> KShift3Bytes ) & 0xFF );
        iReqBuf.SetLength( sizeof( TVibePacketModifyBasisEffectRequest ) );
        }
    else
        {
        iReqBuf.SetLength( 0 );    
        } 
    
    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncModifyPlayingPeriodicEffectReq( 
    TInt aDeviceHandle, TInt aEffectHandle, 
    CHWRMHaptics::THWRMHapticsPeriodicEffect aEffect, RBuf8& aBuffer )
    {
    TVibePacketModifyBasisEffectRequest* pReq = 
        reinterpret_cast<TVibePacketModifyBasisEffectRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );

    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdModifyPlayingPeriodicEffect;
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        pReq->iEffectHandle_7_0 = 
            static_cast<TUint8>( aEffectHandle & 0xFF );
        pReq->iEffectHandle_15_8 = 
            static_cast<TUint8>( ( aEffectHandle >> KShiftByte ) & 0xFF );
        pReq->iEffectHandle_23_16 = 
            static_cast<TUint8>( ( aEffectHandle >> KShift2Bytes ) & 0xFF );
        pReq->iEffectHandle_31_24 = 
            static_cast<TUint8>( ( aEffectHandle >> KShift3Bytes ) & 0xFF );
        pReq->iDuration_7_0 = 
            static_cast<TUint8>( aEffect.iDuration & 0xFF );
        pReq->iDuration_15_8 = 
            static_cast<TUint8>( 
                ( aEffect.iDuration >> KShiftByte ) & 0xFF );
        pReq->iDuration_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iDuration >> KShift2Bytes ) & 0xFF );
        pReq->iDuration_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iDuration >> KShift3Bytes ) & 0xFF );
        pReq->iMagnitude_7_0 = 
            static_cast<TUint8>( aEffect.iMagnitude & 0xFF );
        pReq->iMagnitude_15_8 = 
            static_cast<TUint8>( 
                ( aEffect.iMagnitude >> KShiftByte ) & 0xFF );
        pReq->iMagnitude_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iMagnitude >> KShift2Bytes ) & 0xFF );
        pReq->iMagnitude_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iMagnitude >> KShift3Bytes ) & 0xFF );
        pReq->iEffectTypeStyle = 
            static_cast<TUint8>( ( ( aEffect.iStyle << 4 ) & 0xF0 ) | 0x01 );
        pReq->iPeriod_7_0 = 
            static_cast<TUint8>( aEffect.iPeriod & 0xFF );
        pReq->iPeriod_15_8 = 
            static_cast<TUint8>( ( aEffect.iPeriod >> KShiftByte ) & 0xFF );
        pReq->iPeriod_23_16 = 
            static_cast<TUint8>( ( aEffect.iPeriod >> KShift2Bytes ) & 0xFF );
        pReq->iPeriod_31_24 = 
            static_cast<TUint8>( ( aEffect.iPeriod >> KShift3Bytes ) & 0xFF );
        pReq->iImpulseTime_7_0 = 
            static_cast<TUint8>( aEffect.iAttackTime & 0xFF );
        pReq->iImpulseTime_15_8 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackTime >> KShiftByte ) & 0xFF );
        pReq->iImpulseTime_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackTime >> KShift2Bytes ) & 0xFF );
        pReq->iImpulseTime_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackTime >> KShift3Bytes ) & 0xFF );
        pReq->iImpulseLevel_7_0 = 
            static_cast<TUint8>( aEffect.iAttackLevel & 0xFF );
        pReq->iImpulseLevel_15_8 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackLevel >> KShiftByte ) & 0xFF );
        pReq->iImpulseLevel_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackLevel >> KShift2Bytes ) & 0xFF );
        pReq->iImpulseLevel_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iAttackLevel >> KShift3Bytes ) & 0xFF );
        pReq->iFadeTime_7_0 = 
            static_cast<TUint8>( aEffect.iFadeTime & 0xFF );
        pReq->iFadeTime_15_8 = 
            static_cast<TUint8>( ( aEffect.iFadeTime >> KShiftByte ) & 0xFF );
        pReq->iFadeTime_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeTime >> KShift2Bytes ) & 0xFF );
        pReq->iFadeTime_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeTime >> KShift3Bytes ) & 0xFF );
        pReq->iFadeLevel_7_0 = 
            static_cast<TUint8>( aEffect.iFadeLevel & 0xFF );
        pReq->iFadeLevel_15_8 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeLevel >> KShiftByte ) & 0xFF );
        pReq->iFadeLevel_23_16 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeLevel >> KShift2Bytes ) & 0xFF );
        pReq->iFadeLevel_31_24 = 
            static_cast<TUint8>( 
                ( aEffect.iFadeLevel >> KShift3Bytes ) & 0xFF );
        iReqBuf.SetLength( sizeof( TVibePacketModifyBasisEffectRequest ) );
        }
    else
        {
        iReqBuf.SetLength( 0 );    
        }

    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncPausePlayingEffectReq( 
    TInt aDeviceHandle, TInt aEffectHandle, RBuf8& aBuffer )
    {
    TVibePacketPausePlayingEffectRequest* pReq = 
        reinterpret_cast<TVibePacketPausePlayingEffectRequest*>
            (const_cast<TUint8*>( iReqBuf.Ptr() ) );
    
    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdPausePlayingEffect;
        pReq->iEffectHandle_7_0 = 
            static_cast<TUint8>( aEffectHandle & 0xFF );
        pReq->iEffectHandle_15_8 = 
            static_cast<TUint8>( ( aEffectHandle >> KShiftByte ) & 0xFF );
        pReq->iEffectHandle_23_16 = 
            static_cast<TUint8>( ( aEffectHandle >> KShift2Bytes ) & 0xFF );
        pReq->iEffectHandle_31_24 = 
            static_cast<TUint8>( ( aEffectHandle >> KShift3Bytes ) & 0xFF );        
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        iReqBuf.SetLength( sizeof( TVibePacketPausePlayingEffectRequest ) );
        }
    else
        {
        iReqBuf.SetLength( 0 );    
        }    

    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncResumePausedEffectReq( 
    TInt aDeviceHandle, TInt aEffectHandle, RBuf8& aBuffer )
    {
    TVibePacketResumePlayingEffectRequest* pReq = 
        reinterpret_cast<TVibePacketResumePlayingEffectRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );

    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdResumePausedEffect;
        pReq->iEffectHandle_7_0 = 
            static_cast<TUint8>( aEffectHandle & 0xFF );
        pReq->iEffectHandle_15_8 = 
            static_cast<TUint8>( ( aEffectHandle >> KShiftByte ) & 0xFF );
        pReq->iEffectHandle_23_16 = 
            static_cast<TUint8>( ( aEffectHandle >> KShift2Bytes ) & 0xFF );
        pReq->iEffectHandle_31_24 = 
            static_cast<TUint8>( ( aEffectHandle >> KShift3Bytes ) & 0xFF );
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        iReqBuf.SetLength( sizeof( TVibePacketResumePlayingEffectRequest ) );
        }
    else
        {
        iReqBuf.SetLength( 0 );    
        }
    
    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncStopPlayingEffectReq( 
    TInt aDeviceHandle, TInt aEffectHandle, RBuf8& aBuffer )
    {
    TVibePacketStopEffectRequest* pReq = 
        reinterpret_cast<TVibePacketStopEffectRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );

    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdStopEffect;
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        pReq->iEffectHandle_7_0 = 
            static_cast<TUint8>( aEffectHandle & 0xFF );
        pReq->iEffectHandle_15_8 = 
            static_cast<TUint8>( ( aEffectHandle >> KShiftByte ) & 0xFF );
        pReq->iEffectHandle_23_16 = 
            static_cast<TUint8>( ( aEffectHandle >> KShift2Bytes ) & 0xFF );
        pReq->iEffectHandle_31_24 = 
            static_cast<TUint8>( ( aEffectHandle >> KShift3Bytes ) & 0xFF );
        iReqBuf.SetLength( sizeof( TVibePacketStopEffectRequest ) );
        }
    else
        {
        iReqBuf.SetLength( 0 );    
        }

    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncStopAllPlayingEffectsReq( 
    TInt aDeviceHandle, RBuf8& aBuffer )
    {
    TVibePacketStopAllEffectsRequest* pReq = 
        reinterpret_cast<TVibePacketStopAllEffectsRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );

    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdStopAllEffects;
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        iReqBuf.SetLength( sizeof( TVibePacketStopAllEffectsRequest ) );
        }
    else
        {
        iReqBuf.SetLength( 0 );    
        }
    
    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncPlayEffectIncludeEffectDataReq( 
    TInt aDeviceHandle, const TDesC8& aData, TInt aEffectIndex, 
    RBuf8& aBuffer )
    {
    iDataBuf.Close();
    TInt err = iDataBuf.Create( aData.Size() + KMsgDefaultSize );

    TVibePacketPlayIVTEffectRequest* pReq = 
        reinterpret_cast<TVibePacketPlayIVTEffectRequest*>
            ( const_cast<TUint8*>( iDataBuf.Ptr() ) );

    if ( !err && pReq && 
         ( ( aData.Size() + KMsgDefaultSize ) <= KVibePacketMaxSize ) )
        {
        pReq->iCmdCode = KVibeCmdPlayIVTEffectIncludeData;
        pReq->iIvtDataSize = aData.Size();
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        pReq->iIvtIndex_7_0 = 
            static_cast<TUint8>( aEffectIndex & 0xFF );
        pReq->iIvtIndex_15_8 = 
            static_cast<TUint8>( ( aEffectIndex >> KShiftByte ) & 0xFF );
        pReq->iRepeat = 0x0;
        
        memcpy( pReq->iIvtData, aData.Ptr(), aData.Size() );

        // the data buffer length is set to the size of the request packet
        // plus size of the data minus 1 (minus 1, because first byte of the
        // data is already calculated as part of the request packet size).
        iDataBuf.SetLength( 
            sizeof( TVibePacketPlayIVTEffectRequest ) + aData.Size() -1 );
        }
    else
        {
        iDataBuf.SetLength( 0 );    
        }

    return aBuffer.Create( iDataBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncPlayEffectRepeatIncludeEffectDataReq( 
    TInt aDeviceHandle, const TDesC8& aData, TInt aEffectIndex, 
    TUint8 aRepeat, RBuf8& aBuffer )
    {
    iDataBuf.Close();
    TInt err = iDataBuf.Create( aData.Size() + KMsgDefaultSize );

    TVibePacketPlayIVTEffectRequest* pReq = 
        reinterpret_cast<TVibePacketPlayIVTEffectRequest*>
            ( const_cast<TUint8*>( iDataBuf.Ptr() ) );

    if ( !err && pReq && 
        ( ( aData.Size() + KMsgDefaultSize ) <= KVibePacketMaxSize ) )
        {
        pReq->iCmdCode = KVibeCmdPlayIVTEffectIncludeData;
        pReq->iIvtDataSize = aData.Size();
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        pReq->iIvtIndex_7_0 = 
            static_cast<TUint8>( aEffectIndex & 0xFF );
        pReq->iIvtIndex_15_8 = 
            static_cast<TUint8>( ( aEffectIndex >> KShiftByte ) & 0xFF );
        pReq->iRepeat = aRepeat;
        
        memcpy( pReq->iIvtData, aData.Ptr(), aData.Size() );
        
        // the data buffer length is set to the size of the request packet
        // plus size of the data minus 1 (minus 1, because first byte of the
        // data is already calculated as part of the request packet size).
        iDataBuf.SetLength( 
            sizeof( TVibePacketPlayIVTEffectRequest ) + aData.Size() -1 );
        } 
    else
        {
        iDataBuf.SetLength( 0 );    
        }

    return aBuffer.Create( iDataBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncPlayEffectRepeatNoDataReq( 
    TInt aDeviceHandle, TInt aEffectIndex, TUint8 aRepeat, RBuf8& aBuffer )
    {
    TVibePacketPlayIVTEffectRequest* pReq = 
        reinterpret_cast<TVibePacketPlayIVTEffectRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );

    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdPlayIVTEffectNoData;
        pReq->iIvtDataSize = 0x0;
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        pReq->iIvtIndex_7_0 = 
            static_cast<TUint8>( aEffectIndex & 0xFF );
        pReq->iIvtIndex_15_8 = 
            static_cast<TUint8>( ( aEffectIndex >> KShiftByte ) & 0xFF );
        pReq->iRepeat = aRepeat;
        iReqBuf.SetLength( sizeof( TVibePacketPlayIVTEffectRequest ) );
        } 
    else
        {
        iReqBuf.SetLength( 0 );    
        }

    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncPlayEffectNoDataReq( 
    TInt aDeviceHandle, TInt aEffectIndex, RBuf8& aBuffer )
    {
    TVibePacketPlayIVTEffectRequest* pReq = 
        reinterpret_cast<TVibePacketPlayIVTEffectRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );

    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdPlayIVTEffectNoData;
        pReq->iIvtDataSize = 0x0;
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        pReq->iIvtIndex_7_0 = 
            static_cast<TUint8>( aEffectIndex & 0xFF );
        pReq->iIvtIndex_15_8 = 
            static_cast<TUint8>( ( aEffectIndex >> KShiftByte ) & 0xFF );
        pReq->iRepeat = 0x0;
        iReqBuf.SetLength( sizeof( TVibePacketPlayIVTEffectRequest ) );
        } 
    else
        {
        iReqBuf.SetLength( 0 );    
        }

    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncCreateStreamingEffectReq( 
    TInt aDeviceHandle, RBuf8& aBuffer )
    {
    TVibePacketCreateStreamingEffectRequest* pReq = 
        reinterpret_cast<TVibePacketCreateStreamingEffectRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );

    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdCreateStreamingEffect;
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        iReqBuf.SetLength( 
            sizeof( TVibePacketCreateStreamingEffectRequest ) );
        } 
    else
        {
        iReqBuf.SetLength( 0 );    
        }

    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncDestroyStreamingEffectReq( 
    TInt aDeviceHandle, TInt aEffectHandle, RBuf8& aBuffer )
    {
    TVibePacketDestroyStreamingEffectRequest* pReq = 
        reinterpret_cast<TVibePacketDestroyStreamingEffectRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );

    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdDestroyStreamingEffect;
        pReq->iEffectHandle_7_0 = 
            static_cast<TUint8>( aEffectHandle & 0xFF );
        pReq->iEffectHandle_15_8 = 
            static_cast<TUint8>( ( aEffectHandle >> KShiftByte ) & 0xFF );
        pReq->iEffectHandle_23_16 = 
            static_cast<TUint8>( ( aEffectHandle >> KShift2Bytes ) & 0xFF );
        pReq->iEffectHandle_31_24 = 
            static_cast<TUint8>( ( aEffectHandle >> KShift3Bytes ) & 0xFF );
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        iReqBuf.SetLength( 
            sizeof( TVibePacketDestroyStreamingEffectRequest ) );
        } 
    else
        {
        iReqBuf.SetLength( 0 );    
        }

    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncPlayStreamingSampleReq( 
    TInt aDeviceHandle, const TDesC8& aStreamingSample, TInt aEffectHandle,
    RBuf8& aBuffer )
    {
    return EncPlayStreamingSampleInternal( aDeviceHandle,
                                           aEffectHandle,
                                           aStreamingSample,
                                           0, aBuffer );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncPlayStreamingSampleWithOffsetReq(
    TInt aDeviceHandle, const TDesC8& aStreamingSample, TInt aOffsetTime, 
    TInt aEffectHandle, RBuf8& aBuffer )
    {
    return EncPlayStreamingSampleInternal( aDeviceHandle,
                                           aEffectHandle,
                                           aStreamingSample,
                                           aOffsetTime,
                                           aBuffer );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncGetEffectStateReq( 
    TInt aDeviceHandle, TInt aEffectHandle, RBuf8& aBuffer )
    {
    TVibePacketGetEffectStateRequest* pReq = 
        reinterpret_cast<TVibePacketGetEffectStateRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );

    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdGetEffectState;
        pReq->iEffectHandle_7_0 = 
            static_cast<TUint8>( aEffectHandle & 0xFF );
        pReq->iEffectHandle_15_8 = 
            static_cast<TUint8>( ( aEffectHandle >> KShiftByte ) & 0xFF );
        pReq->iEffectHandle_23_16 = 
            static_cast<TUint8>( ( aEffectHandle >> KShift2Bytes ) & 0xFF );
        pReq->iEffectHandle_31_24 = 
            static_cast<TUint8>( ( aEffectHandle >> KShift3Bytes ) & 0xFF );
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        iReqBuf.SetLength( sizeof( TVibePacketGetEffectStateRequest ) );
        } 
    else
        {
        iReqBuf.SetLength( 0 );    
        }

    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncSetDevicePropertyBoolReq( 
    TInt aDeviceHandle, TBool aDevPropValue, TInt aDevPropType,
    RBuf8& aBuffer )
    {
    TVibePacketSetDevicePropertyRequest* pReq = 
        reinterpret_cast<TVibePacketSetDevicePropertyRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );
   
    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdSetDeviceProperty;
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        pReq->iPropertyType_7_0 = 
            static_cast<TUint8>( aDevPropType & 0xFF );
        pReq->iPropertyType_15_8 = 
            static_cast<TUint8>( ( aDevPropType >> KShiftByte ) & 0xFF );
        pReq->iPropertyType_23_16 = 
            static_cast<TUint8>( ( aDevPropType >> KShift2Bytes ) & 0xFF );
        pReq->iPropertyType_31_24 = 
            static_cast<TUint8>( ( aDevPropType >> KShift3Bytes ) & 0xFF );
        pReq->iPropertyValueType = 
            static_cast<TUint8>( KVibeValuetypeBool );
        pReq->iSize = 1; // size of bool data is 1 byte
        pReq->iPropertyValue[0] = 
            static_cast<TUint8>( aDevPropValue );
        iReqBuf.SetLength( sizeof( TVibePacketSetDevicePropertyRequest ) );
        } 
    else
        {
        iReqBuf.SetLength( 0 );    
        }

    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncSetDevicePropertyIntReq( 
    TInt aDeviceHandle, TInt aDevPropValue, TInt aDevPropType, 
    RBuf8& aBuffer )
    {
    TVibePacketSetDevicePropertyRequest* pReq = 
        reinterpret_cast<TVibePacketSetDevicePropertyRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );
   
    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdSetDeviceProperty;
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        pReq->iPropertyType_7_0 = 
            static_cast<TUint8>( aDevPropType & 0xFF );
        pReq->iPropertyType_15_8 = 
            static_cast<TUint8>( ( aDevPropType >> KShiftByte ) & 0xFF );
        pReq->iPropertyType_23_16 = 
            static_cast<TUint8>( ( aDevPropType >> KShift2Bytes ) & 0xFF );
        pReq->iPropertyType_31_24 = 
            static_cast<TUint8>( ( aDevPropType >> KShift3Bytes ) & 0xFF );
        pReq->iPropertyValueType = 
            static_cast<TUint8>( KVibeValuetypeInt32 );
        pReq->iSize = 4; // Size of int32 value is 4 bytes
        pReq->iPropertyValue[0] = 
            static_cast<TUint8>( aDevPropValue & 0xFF );
        pReq->iPropertyValue[1] = 
            static_cast<TUint8>( ( aDevPropValue >> KShiftByte ) & 0xFF );
        pReq->iPropertyValue[2] = 
            static_cast<TUint8>( ( aDevPropValue >> KShift2Bytes ) & 0xFF );
        pReq->iPropertyValue[3] = 
            static_cast<TUint8>( ( aDevPropValue >> KShift3Bytes ) & 0xFF );
        // the data buffer length is set to the size of the request packet
        // plus size of the data minus 1. Since int32 type data is always 
        // 4 bytes, the extra size needed for data is 3 bytes (4-1).
        iReqBuf.SetLength(  
            sizeof( TVibePacketSetDevicePropertyRequest ) +3 );
        } 
    else
        {
        iReqBuf.SetLength( 0 );
        }

    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncSetDevicePropertyStringReq( 
    TInt aDeviceHandle, const TDesC8& aDevPropValue, TInt aDevPropType,
    RBuf8& aBuffer )
    {
    iDataBuf.Close();
    TInt err = iDataBuf.Create( aDevPropValue.Size() + KMsgDefaultSize );

    TVibePacketSetDevicePropertyRequest* pReq = 
        reinterpret_cast<TVibePacketSetDevicePropertyRequest*>
            ( const_cast<TUint8*>( iDataBuf.Ptr() ) );
   
    if ( !err && pReq && 
        ( ( aDevPropValue.Size() + KMsgDefaultSize ) <= KVibePacketMaxSize ) )
        {
        pReq->iCmdCode = KVibeCmdSetDeviceProperty;
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        pReq->iPropertyType_7_0 = 
            static_cast<TUint8>( aDevPropType & 0xFF );
        pReq->iPropertyType_15_8 = 
            static_cast<TUint8>( ( aDevPropType >> KShiftByte ) & 0xFF );
        pReq->iPropertyType_23_16 = 
            static_cast<TUint8>( ( aDevPropType >> KShift2Bytes ) & 0xFF );
        pReq->iPropertyType_31_24 = 
            static_cast<TUint8>( ( aDevPropType >> KShift3Bytes ) & 0xFF );
        pReq->iPropertyValueType = 
            static_cast<TUint8>( KVibeValuetypeString );
        pReq->iSize = 
            static_cast<TUint8>( aDevPropValue.Size() );
        
        memcpy ( pReq->iPropertyValue, 
                 reinterpret_cast<TUint8*>( 
                    const_cast<TUint8*>( aDevPropValue.Ptr() ) ), 
                 aDevPropValue.Size() );
        
        // the data buffer length is set to the size of the request packet
        // plus size of the data minus 1 (minus 1, because first byte of the
        // data is already calculated as part of the request packet size).
        iDataBuf.SetLength( 
            sizeof( TVibePacketSetDevicePropertyRequest ) + pReq->iSize -1 );
        } 
    else
        {
        iDataBuf.SetLength( 0 );    
        }

    return aBuffer.Create( iDataBuf );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncSetPlatformLicenseKeyReq( 
    TInt aDeviceHandle, RBuf8& aBuffer )
    {
    TInt ret = EncSetDevicePropertyStringReq( aDeviceHandle, KNullDesC8, 
                                         CHWRMHaptics::EHWRMHapticsLicensekey,
                                         aBuffer );
    
    // change the command id from general device property to setting
    // license key
    if ( ret == KErrNone )
        {
        TVibePacketSetDevicePropertyRequest* tmpReq =
            reinterpret_cast<TVibePacketSetDevicePropertyRequest*>
                ( const_cast<TUint8*>( aBuffer.Ptr() ) );
        
        if ( tmpReq )
            {
            tmpReq->iCmdCode = KVibeCmdSetLicense;
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncGetDevicePropertyBoolReq( 
    TInt aDeviceHandle, TInt aDevPropType, RBuf8& aBuffer )
    {
    return EncGetDevicePropertyReqInternal( 
        KVibeValuetypeBool, aDeviceHandle, aDevPropType, aBuffer );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncGetDevicePropertyIntReq( 
    TInt aDeviceHandle, TInt aDevPropType, RBuf8& aBuffer )
    {
    return EncGetDevicePropertyReqInternal( 
        KVibeValuetypeInt32, aDeviceHandle, aDevPropType, aBuffer );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncGetDevicePropertyStringReq( 
    TInt aDeviceHandle, TInt aDevPropType, RBuf8& aBuffer )
    {
    return EncGetDevicePropertyReqInternal( 
        KVibeValuetypeString, aDeviceHandle, aDevPropType, aBuffer );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncGetDeviceCapabilityIntReq( 
    TInt aDeviceHandle, TInt aDevCapType, RBuf8& aBuffer )
    {
    return EncGetDeviceCapabilityReqInternal( KVibeValuetypeInt32, 
                                              aDeviceHandle, 
                                              aDevCapType,
                                              aBuffer );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncGetDeviceCapabilityStringReq( 
    TInt aDeviceHandle, TInt aDevCapType, RBuf8& aBuffer )
    {
    return EncGetDeviceCapabilityReqInternal( KVibeValuetypeString, 
                                              aDeviceHandle, 
                                              aDevCapType,
                                              aBuffer );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CDesC8ArraySeg* CHWRMHapticsVibePacketizer::DecodeMessageL( 
    const TDesC8& aData, TInt& aStatus )
    {
    // Clear Array before use
    iReturnArray->Reset();
    
    TVibePacket* pRsp = 
        reinterpret_cast<TVibePacket*>( const_cast<TUint8*>( aData.Ptr() ) );

    switch( pRsp->iSimpleRsp.iCmdCode )
        {
         case KVibeCmdOpenDevice:
            {
            aStatus = pRsp->iOpenDeviceRsp.iVibeStatus;
            iDeviceHandle = 
                pRsp->iOpenDeviceRsp.iDeviceHandle_7_0 |
                pRsp->iOpenDeviceRsp.iDeviceHandle_15_8  << KShiftByte  |
                pRsp->iOpenDeviceRsp.iDeviceHandle_23_16 << KShift2Bytes |
                pRsp->iOpenDeviceRsp.iDeviceHandle_31_24 << KShift3Bytes;
            
            TPckg<TInt> deviceHandlePckg( iDeviceHandle );
                              
            iReturnArray->AppendL( deviceHandlePckg );
            break;
            }
        
         case KVibeCmdCloseDevice:
             {
             aStatus = pRsp->iCloseDeviceRsp.iVibeStatus;
             iDeviceHandle = KErrNotFound;
             break;
             }
        
        case KVibeCmdSetDeviceProperty: // flow through
        case KVibeCmdSetLicense:
            {
            aStatus = pRsp->iSetDevicePropertyRsp.iVibeStatus;
            break;
            }
         
        case KVibeCmdGetDeviceProperty:
            {
            TInt propertyValueType =
                pRsp->iGetDevicePropertyRsp.iPropertyValueType;
             
            switch( propertyValueType )
                {
                case KVibeValuetypeBool:
                    {
                    aStatus =  pRsp->iGetDevicePropertyRsp.iVibeStatus;
                    
                    TUint8* intArr = 
                        pRsp->iGetDevicePropertyRsp.iPropertyValue;
    
                    TUint value = ( TUint( intArr[0] ) );
                    TPckg<TBool> devicePropertyValuePckg( value );
                       
                    iReturnArray->AppendL( devicePropertyValuePckg );
                    break;
                    }
                case KVibeValuetypeInt32:
                    {
                    aStatus =  pRsp->iGetDevicePropertyRsp.iVibeStatus;
                    
                    TUint8* intArr = 
                        pRsp->iGetDevicePropertyRsp.iPropertyValue;
    
                    TUint value = ( TUint( intArr[0] ) )
                                | ( TUint( intArr[1] ) << KShiftByte )
                                | ( TUint( intArr[2] ) << KShift2Bytes )
                                | ( TUint( intArr[3] ) << KShift3Bytes );
                    TPckg<TInt> devicePropertyValuePckg( value );
                       
                    iReturnArray->AppendL( devicePropertyValuePckg );
                    break;
                    }
                     
                case KVibeValuetypeString:
                    {
                    aStatus = pRsp->iGetDevicePropertyRsp.iVibeStatus;
                    TInt stringSize = pRsp->iGetDevicePropertyRsp.iSize;
                    
                    if( stringSize > MaxPropertyStringLength() )
                        {
                        // truncate string if too long
                        stringSize = MaxPropertyStringLength();
                        }
                    
                    TUint8* text = reinterpret_cast<TUint8*>( 
                            pRsp->iGetDevicePropertyRsp.iPropertyValue );
                    TPtrC8 ptr( text, stringSize );
                        
                    iReturnArray->AppendL( ptr );
                    break;
                    }
                default:
                    break;
                }// switch
             break;
             }
         
         case KVibeCmdPlayMagSweepEffect: // flow through
         case KVibeCmdPlayPeriodicEffect:
             {
             aStatus = pRsp->iPlayBasisEffectRsp.iVibeStatus;
             TInt effectHandle = 
                pRsp->iPlayBasisEffectRsp.iEffectHandle_7_0 |
                pRsp->iPlayBasisEffectRsp.iEffectHandle_15_8  << KShiftByte  |
                pRsp->iPlayBasisEffectRsp.iEffectHandle_23_16 << KShift2Bytes |
                pRsp->iPlayBasisEffectRsp.iEffectHandle_31_24 << KShift3Bytes;
                        
             TPckg<TInt> effectHandlePckg( effectHandle );
             iReturnArray->AppendL( effectHandlePckg );
             break;
             }
     
         case KVibeCmdPlayIVTEffectIncludeData: // flow through
         case KVibeCmdPlayIVTEffectNoData:   
             {
             aStatus = pRsp->iPlayIVTEffectRsp.iVibeStatus;
             TInt effectHandle = 
                pRsp->iPlayIVTEffectRsp.iEffectHandle_7_0 |
                pRsp->iPlayIVTEffectRsp.iEffectHandle_15_8  << KShiftByte  |
                pRsp->iPlayIVTEffectRsp.iEffectHandle_23_16 << KShift2Bytes |
                pRsp->iPlayIVTEffectRsp.iEffectHandle_31_24 << KShift3Bytes;
           
             TPckg<TInt> effectHandlePckg( effectHandle );
             iReturnArray->AppendL( effectHandlePckg );
             break;
             }
                      
         case KVibeCmdModifyPlayingMagSweepEffect: // flow through
         case KVibeCmdModifyPlayingPeriodicEffect:
             {
             aStatus = pRsp->iModifyBasisEffectRsp.iVibeStatus;
             break;
             }

        case KVibeCmdStopEffect:
             {
             aStatus = pRsp->iStopEffectRsp.iVibeStatus;
             break;
             }

        case KVibeCmdStopAllEffects:
             {
             aStatus = pRsp->iStopAllEffectsRsp.iVibeStatus;
             break;
             }
         
        case KVibeCmdGetDeviceCapabilities:
            {
            TInt capabilityValueType = 
                pRsp->iGetDeviceCapsRsp.iCapabilityValueType;
            
            switch( capabilityValueType )
                {
                case KVibeValuetypeInt32:
                    {
                    aStatus = pRsp->iGetDeviceCapsRsp.iVibeStatus;
                    TUint8* intArr = 
                        pRsp->iGetDeviceCapsRsp.iCapabilityValue;

                    TUint value = ( TUint( intArr[0] ) )
                                | ( TUint( intArr[1] ) << KShiftByte )
                                | ( TUint( intArr[2] ) << KShift2Bytes )
                                | ( TUint( intArr[3] ) << KShift3Bytes );
                     
                    TPckg<TInt> deviceCapabilityValuePckg( value );
                    iReturnArray->AppendL( deviceCapabilityValuePckg );
                    break;
                    }
                 
                case KVibeValuetypeString:
                    {
                    aStatus = pRsp->iGetDeviceCapsRsp.iVibeStatus;
                    TUint8* text = 
                        reinterpret_cast<TUint8*>(
                            pRsp->iGetDeviceCapsRsp.iCapabilityValue );
                    TInt stringSize ( pRsp->iGetDeviceCapsRsp.iSize );

                    if( stringSize > MaxCapabilityStringLength() )
                        {
                        // truncate string if too long
                        stringSize = MaxCapabilityStringLength();
                        }
                    TPtrC8 ptr( text, stringSize );

                    iReturnArray->AppendL( ptr );
                    break;
                    }
                default:
                   break;
                }// switch
            break;
            }
         
        case KVibeCmdPlayStreamingSample:
            {
            aStatus = pRsp->iPlayStreamingSampleRsp.iVibeStatus;
            break;
            }
         
        case KVibeCmdCreateStreamingEffect:
            {
            aStatus = pRsp->iCreateStreamingEffectRsp.iVibeStatus;
            
            TInt effectHandle = 
                pRsp->iCreateStreamingEffectRsp.iEffectHandle_7_0 |
                pRsp->iCreateStreamingEffectRsp.iEffectHandle_15_8  
                    << KShiftByte  |
                pRsp->iCreateStreamingEffectRsp.iEffectHandle_23_16     
                    << KShift2Bytes |
                pRsp->iCreateStreamingEffectRsp.iEffectHandle_31_24 
                    << KShift3Bytes;
                                 
            TPckg<TInt> effectHandlePckg( effectHandle );
            iReturnArray->AppendL( effectHandlePckg );
            break;
            }
         
        case KVibeCmdDestroyStreamingEffect:
             {
             aStatus = pRsp->iDestroyStreamingEffectRsp.iVibeStatus;
             break;
             }
         
        case KVibeCmdPausePlayingEffect:
             {
             aStatus = pRsp->iPausePlayingEffectRsp.iVibeStatus;
             break;
             }
         
        case KVibeCmdResumePausedEffect:
             {
             aStatus = pRsp->iResumePausedEffectRsp.iVibeStatus;
             break;
             }
         
        case KVibeCmdGetEffectState:
             {
             aStatus = pRsp->iGetEffectStateRsp.iVibeStatus;
             TInt effectState = pRsp->iGetEffectStateRsp.iEffectState;
             
             TPckg<TInt> effectStatePckg( effectState );
             iReturnArray->AppendL( effectStatePckg );
             break;
             }
                   
        default:
            User::Leave( KErrGeneral );
        }
    
    // convert vibe error code to Symbian error code
    aStatus = MapError( aStatus );
    
    return iReturnArray;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::DeviceHandle()
    {
    return iDeviceHandle;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::InfiniteRepeat()
    {
    return static_cast<TInt>( KVibeRepeatCountInfinite );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::InfiniteDuration()
    {
    return KVibeTimeInfinite;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::MaxEffectNameLength()
    {
    return KVibeMaxEffectNameLength;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::MaxDeviceNameLength()
    {
    return KVibeMaxDeviceNameLength;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::MaxCapabilityStringLength()
    {
    return KVibeMaxCapabilityStringLength;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::MaxPropertyStringLength()
    {
    return KVibeMaxPropertyStringLength;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::MaxStreamingSampleSize()
    {
    return KVibeMaxStreamingSampleSize;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::DefaultDevicePriority()
    {
    return KVibeDevicePriorityDefault;
    }
    
// ---------------------------------------------------------------------------
// Internal helper method. Fills Get Device Capability request data.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncGetDeviceCapabilityReqInternal( 
    TInt aValueType, TInt aDeviceHandle, TInt aDevCapType, RBuf8& aBuffer )
    {
    TVibePacketGetDeviceCapabilitiesRequest* pReq = 
        reinterpret_cast<TVibePacketGetDeviceCapabilitiesRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );
        
    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdGetDeviceCapabilities;
        pReq->iCapabilityValueType = 
            static_cast<TUint8>( aValueType );
        pReq->iDeviceIndex_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceIndex_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceIndex_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceIndex_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        pReq->iCapabilityType_7_0 = 
            static_cast<TUint8>( aDevCapType & 0xFF );
        pReq->iCapabilityType_15_8 = 
            static_cast<TUint8>( ( aDevCapType >> KShiftByte ) & 0xFF );
        pReq->iCapabilityType_23_16 = 
            static_cast<TUint8>( ( aDevCapType >> KShift2Bytes ) & 0xFF );
        pReq->iCapabilityType_31_24 = 
            static_cast<TUint8>( ( aDevCapType >> KShift3Bytes ) & 0xFF );
        iReqBuf.SetLength( 
            sizeof ( TVibePacketGetDeviceCapabilitiesRequest ) );
        }
    else
        {
        iReqBuf.SetLength( 0 );
        }
    
    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// Internal helper method. Fills Get Device Property request data.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncGetDevicePropertyReqInternal( 
    TInt aValueType, TInt aDeviceHandle, TInt aDevPropType, RBuf8& aBuffer )
    {
    TVibePacketGetDevicePropertyRequest* pReq = 
        reinterpret_cast<TVibePacketGetDevicePropertyRequest*>
            ( const_cast<TUint8*>( iReqBuf.Ptr() ) );
        
    if ( pReq )
        {
        pReq->iCmdCode = KVibeCmdGetDeviceProperty;
        pReq->iPropertyValueType = 
            static_cast<TUint8>( aValueType );
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        pReq->iPropertyType_7_0 = 
            static_cast<TUint8>( aDevPropType & 0xFF );
        pReq->iPropertyType_15_8 = 
            static_cast<TUint8>( ( aDevPropType >> KShiftByte ) & 0xFF );
        pReq->iPropertyType_23_16 = 
            static_cast<TUint8>( ( aDevPropType >> KShift2Bytes ) & 0xFF );
        pReq->iPropertyType_31_24 = 
            static_cast<TUint8>( ( aDevPropType >> KShift3Bytes ) & 0xFF );
        iReqBuf.SetLength( sizeof ( TVibePacketGetDevicePropertyRequest ) );
        }
    else
        {
        iReqBuf.SetLength( 0 );
        }
    
    return aBuffer.Create( iReqBuf );
    }

// ---------------------------------------------------------------------------
// Internal helper method. Fills Play Streaming Sample request data. 
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::EncPlayStreamingSampleInternal( 
    TInt aDeviceHandle, TInt aEffectHandle, const TDesC8& aStreamingSample, 
    TInt aOffsetTime, RBuf8& aBuffer )
    {
    iDataBuf.Close();
    TInt err = iDataBuf.Create( aStreamingSample.Size() + KMsgDefaultSize );

    TVibePacketPlayStreamingSampleRequest* pReq = 
        reinterpret_cast<TVibePacketPlayStreamingSampleRequest*>
            ( const_cast<TUint8*>( iDataBuf.Ptr() ) );
    
    if ( !err && pReq && 
        ( ( aStreamingSample.Size() + KMsgDefaultSize ) <= 
            KVibePacketMaxSize ) )
        {
        pReq->iCmdCode = KVibeCmdPlayStreamingSample;
        pReq->iDeviceHandle_7_0 = 
            static_cast<TUint8>( aDeviceHandle & 0xFF );
        pReq->iDeviceHandle_15_8 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShiftByte ) & 0xFF );
        pReq->iDeviceHandle_23_16 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift2Bytes ) & 0xFF );
        pReq->iDeviceHandle_31_24 = 
            static_cast<TUint8>( ( aDeviceHandle >> KShift3Bytes ) & 0xFF );
        pReq->iEffectHandle_7_0 = 
            static_cast<TUint8>( aEffectHandle & 0xFF );
        pReq->iEffectHandle_15_8 = 
            static_cast<TUint8>( ( aEffectHandle >> KShiftByte ) & 0xFF );
        pReq->iEffectHandle_23_16 = 
            static_cast<TUint8>( ( aEffectHandle >> KShift2Bytes ) & 0xFF );
        pReq->iEffectHandle_31_24 = 
            static_cast<TUint8>( ( aEffectHandle >> KShift3Bytes ) & 0xFF );
        pReq->iSize = static_cast<TUint8>( aStreamingSample.Size() );
        pReq->iEffectOffsetTime_7_0 = 
            static_cast<TUint8>( aOffsetTime & 0xFF );
        pReq->iEffectOffsetTime_15_8 = 
            static_cast<TUint8>( ( aOffsetTime >> KShiftByte ) & 0xFF );
        
        memcpy( pReq->iStreamingData, 
                reinterpret_cast<TUint8*>( 
                    const_cast<TUint8*>( aStreamingSample.Ptr() ) ),
                aStreamingSample.Size() );

        iDataBuf.SetLength( 
            sizeof ( TVibePacketPlayStreamingSampleRequest ) 
                + aStreamingSample.Size() -1 );
        }
    else
        {
        iDataBuf.SetLength( 0 );
        }
    
    return aBuffer.Create( iDataBuf );
    }

// ---------------------------------------------------------------------------
// Maps Vibe error code to Symbian error code.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsVibePacketizer::MapError( TInt aVibeError )
    {
    TInt error = KErrNone;
    
    switch ( aVibeError )
        {
        case KVibeErrAlreadyInitialized:
            {
            error = KErrAlreadyExists;
            break;
            }

        case KVibeErrNotInitialized:
            {
            error = KErrNotReady;
            break;
            }

        case KVibeWarningNotPlaying: // flow through
        case KVibeErrInvalidArgument:
            {
            error = KErrArgument;
            break;
            }

        case KVibeErrFail:
            {
            error = KErrGeneral;
            break;
            }

        case KVibeErrIncompatibleEffectType: // flow through
        case KVibeErrIncompatibleCapabilityType: // flow through
        case KVibeErrIncompatiblePropertyType:
            {
            error = KErrNotSupported;
            break;
            }

        case KVibeErrDeviceNeedsLicense:
            {
            error = KErrAccessDenied;
            break;
            }

        case KVibeErrNotEnoughMemory:
            {
            error = KErrNoMemory;
            break;
            }

        case KVibeErrServiceNotRunning:
            {
            error = KErrNotReady;
            break;
            }

        case KVibeErrInsufficientPriority:
            {
            error = KErrAccessDenied;
            break;
            }

        case KVibeErrServiceBusy:
            {
            error = KErrInUse; 
            break;
            }
        
        case KVibeStatusSuccess:
            {
            error = KErrNone;
            break;
            }

        default: 
            {
            // All the warning codes, except KVibeWarningNotPlaying
            // are handled here. Thus they are converted to KErrNone.
            break;
            }
        }
    
    return error;
    }

// ---------------------------------------------------------------------------
// ImplementationTable[]
// ---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x20021207, CHWRMHapticsVibePacketizer::NewL )
    };

// ---------------------------------------------------------------------------
// TImplementationProxy* ImplementationGroupProxy()
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
   
// End of file
