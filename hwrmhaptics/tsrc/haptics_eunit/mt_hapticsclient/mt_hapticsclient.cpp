/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/


#include "MT_HapticsClient.h"
#include <Digia/EUnit/EUnitMacros.h>
#include <Digia/EUnit/EUnitDecorators.h>
#include <limits.h>

//
// EUnit test log: \epoc32\winscw\c\shared\EUnit\Logs\EUnit_log.xml
//

// ---------------------------------------------------------
// Couple of static TUint8 arrays containing bytes for simple
// timeline and magsweep effects (that are used in PlayEffect
// and PlayEffectRepeat method related tests below).
// These are obtained using VibeTonz Studio's export to C-file
// functionality. 
// ---------------------------------------------------------
static const TUint8 Timeline4HapticsMT_ivt[] = 
{
    0x01, 0x00, 0x02, 0x00, 0x1c, 0x00, 0x28, 0x00, 0x00, 0x00, 0x07, 0x00, 0xf1, 0xe0, 0x01, 0xe2,
    0x00, 0x00, 0xff, 0x30, 0xc8, 0x00, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x20, 0x3e, 0x00, 0x00, 0x5f,
    0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 0x12, 0x00, 0x54, 0x00, 0x69, 0x00, 0x6d, 0x00, 0x65, 0x00,
    0x6c, 0x00, 0x69, 0x00, 0x6e, 0x00, 0x65, 0x00, 0x00, 0x00, 0x4d, 0x00, 0x61, 0x00, 0x67, 0x00,
    0x53, 0x00, 0x77, 0x00, 0x65, 0x00, 0x65, 0x00, 0x70, 0x00, 0x00, 0x00
};

static const TUint8 Magsweep4HapticsMT_ivt[] = 
{
    0x01, 0x00, 0x01, 0x00, 0x12, 0x00, 0x14, 0x00, 0x00, 0x00, 0x30, 0xc8, 0x00, 0xc8, 0x00, 0x00,
    0x00, 0x00, 0x20, 0x3e, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x81, 0x00, 0x00, 0x4d, 0x00, 0x61, 0x00,
    0x67, 0x00, 0x53, 0x00, 0x77, 0x00, 0x65, 0x00, 0x65, 0x00, 0x70, 0x00, 0x00, 0x00
};

static const TUint8 Periodic4HapticsMT_ivt[] = 
{
    0x01, 0x00, 0x01, 0x00, 0x12, 0x00, 0x14, 0x00, 0x00, 0x00, 0x30, 0xc8, 0x00, 0xc8, 0x00, 0x00,
    0x00, 0x00, 0x20, 0x3e, 0x00, 0x00, 0x5f, 0x81, 0x3e, 0x81, 0x00, 0x00, 0x50, 0x00, 0x65, 0x00,
    0x72, 0x00, 0x69, 0x00, 0x6f, 0x00, 0x64, 0x00, 0x69, 0x00, 0x63, 0x00, 0x00, 0x00
};

//    | Object name | Type     | Duration | Magnitude | Period |
//--------------------------------------------------------------
// 1. | Timeline1   | Timeline | 3915     |           |        |
// 2. | Timeline2   | Timeline | 5806     |           |        |
// 3. | Periodic1   | Periodic | 1000     | 7480      | 100    |
// 4. | Periodic2   | Periodic | 2000     | 7480      | 100    |
// 5. | MagSweep1   | MagSweep | 3000     | 7480      |        |
// 6. | MagSweep2   | MagSweep | 4000     | 7480      |        |
// -------------------------------------------------------------
static const TUint8 EffectSetMT_ivt[] = 
{
    0x01, 0x00, 0x06, 0x00, 0x66, 0x00, 0x84, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x1a, 0x00, 0x2a, 0x00,
    0x3a, 0x00, 0x4a, 0x00, 0xf1, 0xe0, 0x02, 0xe2, 0x00, 0x00, 0xf1, 0xe0, 0x04, 0xe2, 0x03, 0x93,
    0xff, 0xf1, 0xe0, 0x03, 0xe2, 0x00, 0x00, 0xf1, 0xe0, 0x05, 0xe2, 0x07, 0x0e, 0xff, 0x30, 0xc8,
    0x00, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x20, 0x3e, 0x00, 0x00, 0x5f, 0x81, 0x3e, 0x81, 0x30, 0xc8,
    0x00, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x20, 0x7d, 0x00, 0x00, 0x5f, 0x81, 0x3e, 0x01, 0x30, 0xc8,
    0x00, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x20, 0xbb, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x81, 0x30, 0xc8,
    0x00, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x20, 0xfa, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x14, 0x00, 0x28, 0x00, 0x3c, 0x00, 0x50, 0x00, 0x64, 0x00, 0x54, 0x00, 0x69, 0x00, 0x6d, 0x00,
    0x65, 0x00, 0x6c, 0x00, 0x69, 0x00, 0x6e, 0x00, 0x65, 0x00, 0x31, 0x00, 0x00, 0x00, 0x54, 0x00,
    0x69, 0x00, 0x6d, 0x00, 0x65, 0x00, 0x6c, 0x00, 0x69, 0x00, 0x6e, 0x00, 0x65, 0x00, 0x32, 0x00,
    0x00, 0x00, 0x50, 0x00, 0x65, 0x00, 0x72, 0x00, 0x69, 0x00, 0x6f, 0x00, 0x64, 0x00, 0x69, 0x00,
    0x63, 0x00, 0x31, 0x00, 0x00, 0x00, 0x50, 0x00, 0x65, 0x00, 0x72, 0x00, 0x69, 0x00, 0x6f, 0x00,
    0x64, 0x00, 0x69, 0x00, 0x63, 0x00, 0x32, 0x00, 0x00, 0x00, 0x4d, 0x00, 0x61, 0x00, 0x67, 0x00,
    0x53, 0x00, 0x77, 0x00, 0x65, 0x00, 0x65, 0x00, 0x70, 0x00, 0x31, 0x00, 0x00, 0x00, 0x4d, 0x00,
    0x61, 0x00, 0x67, 0x00, 0x53, 0x00, 0x77, 0x00, 0x65, 0x00, 0x65, 0x00, 0x70, 0x00, 0x32, 0x00,
    0x00, 0x00
};

static const TUint8 CorruptedMT_ivt[] = 
{
    0x00, 0x01, 0x01, 0x01, 0x12, 0x00, 0x14, 0x00, 0x00, 0x00, 0x30, 0xc8, 0x00, 0xc8, 0x00, 0x00,
    0x00, 0x00, 0x20, 0x3e, 0x00, 0x00, 0x5f, 0x81, 0x3e, 0x81, 0x00, 0x00, 0x50, 0x00, 0x65, 0x00,
    0x72, 0x00, 0x69, 0x00, 0x6f, 0x00, 0x64, 0x00, 0x69, 0x00, 0x63, 0x00, 0x00, 0x00
};

//offset value for 
const TInt KEventOffset = 1000;

// imaginary license key
_LIT8( KPlainKey,     "73JJNDJ38EKDKD983783JDJD84U48DJ4" );

// ---------------------------------------------------------
// VibeStatus to S60 status mappings are tested by sending these
// special repeat values to server (calling PlayEffectRepeat).
// The test plugin will acknowledge these special values and 
// returns corresponding VibeStatus error code..
// ---------------------------------------------------------
//
enum repeat_values_4_vibestatustests
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


// - Haptics status callback implementation ---------------------------------
THapticsStatusCallback::THapticsStatusCallback( MT_HapticsClient* aMTCallback )
    : iHapticsStatus( MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ),
      iMTCallback( aMTCallback )
    {
    }

THapticsStatusCallback::~THapticsStatusCallback()
    {
    }

MHWRMHapticsObserver::THWRMHapticsStatus THapticsStatusCallback::Status()
    {
    return iHapticsStatus;
    }

// ---------------------------------------------------------
// from MHWRMHapticsObserver. Method which is called
// by the haptics client, when the haptics status of
// that client changes.
// ---------------------------------------------------------
//
void THapticsStatusCallback::HapticsStatusChangedL( THWRMHapticsStatus aStatus )
    {
    iHapticsStatus = aStatus;
    
    // inform callback of new value
    iMTCallback->HapticsStatusReceived();
    }

// ---------------------------------------------------------
// from MHWRMHapticsObserver. Method is for future use,
// hence the empty implementation.
// ---------------------------------------------------------
//
void THapticsStatusCallback::EffectCompletedL( TInt /*aError*/, TInt /*aEffectHandle*/ )
    {
    }


// - Actuator status callback implementation ---------------------------------
TActuatorStatusCallback::TActuatorStatusCallback( MT_HapticsClient* aMTCallback ) : iMTCallback( aMTCallback ) 
    {
    }

TActuatorStatusCallback::~TActuatorStatusCallback()
    {
    iActuatorStatus.Close();
    }

MHWRMHapticsActuatorObserver::THWRMActuatorEvents TActuatorStatusCallback::Status( THWRMLogicalActuators aActuator )
    {
    TInt index = iActuatorStatus.Find( aActuator );
    if ( index == KErrNotFound )
        {
        return (MHWRMHapticsActuatorObserver::THWRMActuatorEvents)0;
        }        
    return (MHWRMHapticsActuatorObserver::THWRMActuatorEvents)(iActuatorStatus[index+1]-KEventOffset);
    }

// ---------------------------------------------------------
// from MHWRMHapticsObserver. Method which is called
// by the haptics client, when the haptics status of
// that client changes.
// ---------------------------------------------------------
//
void TActuatorStatusCallback::ActuatorEventL( THWRMActuatorEvents aEvent,
                                              THWRMLogicalActuators aActuator )
    {
    TInt index = iActuatorStatus.Find( aActuator );
    if ( index == KErrNotFound )
        {
        iActuatorStatus.Append( aActuator );
        iActuatorStatus.Append( aEvent + KEventOffset );
        }
    else
        {
        iActuatorStatus.Remove( index );
        iActuatorStatus.Remove( index );
        iActuatorStatus.Append( aActuator );
        iActuatorStatus.Append( aEvent + KEventOffset );
        }
    
    // inform callback of new value
    iMTCallback->HapticsStatusReceived();
    }

// ---------------------------------------------------------
// MT_HapticsTestAsyncPlaySender methods
// ---------------------------------------------------------
//
MT_HapticsTestAsyncPlaySender::MT_HapticsTestAsyncPlaySender(
    MT_HapticsClient* aMTCallback ) 
    : CActive( EPriorityStandard ), iMTCallback( aMTCallback )
    {
    CActiveScheduler::Add( this );
    }

MT_HapticsTestAsyncPlaySender::~MT_HapticsTestAsyncPlaySender()
    {
    }

void MT_HapticsTestAsyncPlaySender::PlayRepeat( 
    CHWRMHaptics* aHaptics,
    TInt  aFileHandle,
    TInt aEffectIndex,
    TUint8 aRepeat,
    TInt& aEffectHandle,
    TRequestStatus& aClientStatus )
    {
    aClientStatus = KRequestPending;
    iClientStatus = &aClientStatus;
    aHaptics->PlayEffectRepeat( aFileHandle, 
                                aEffectIndex,
                                aRepeat,
                                aEffectHandle,
                                iStatus );
    SetActive();  
    }

void MT_HapticsTestAsyncPlaySender::Play( 
        CHWRMHaptics* aHaptics,
       TInt  aFileHandle,
       TInt aEffectIndex,
       TInt& aEffectHandle,
       TRequestStatus& aClientStatus )
    {
    aClientStatus = KRequestPending;
    iClientStatus = &aClientStatus;
    aHaptics->PlayEffect( aFileHandle, 
                          aEffectIndex,
                          aEffectHandle,
                          iStatus );
    SetActive();
    }

void MT_HapticsTestAsyncPlaySender::RunL()
    {
    *iClientStatus = iStatus.Int();
    iMTCallback->HapticsStatusReceived();
    }

void MT_HapticsTestAsyncPlaySender::DoCancel()
    {
    }

TInt MT_HapticsTestAsyncPlaySender::RunError( TInt /* aError */ )
    {
    return KErrNone;
    }
       
// - Construction -----------------------------------------------------------

MT_HapticsClient* MT_HapticsClient::NewL()
    {
    MT_HapticsClient* self = MT_HapticsClient::NewLC();
    CleanupStack::Pop();
    return self;
    }

MT_HapticsClient* MT_HapticsClient::NewLC()
    {
    MT_HapticsClient* self = new( ELeave ) MT_HapticsClient();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

MT_HapticsClient::~MT_HapticsClient()
    {
    delete iSender;
    }

// ---------------------------------------------------------
// Method, which is called by the status callback, when a status
// has been received. Stops the wait loop waiting for a new status.
// ---------------------------------------------------------
//
void MT_HapticsClient::HapticsStatusReceived()
    {
    iWait.AsyncStop();
    }

MT_HapticsClient::MT_HapticsClient()
    {
    }

void MT_HapticsClient::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iSender = new ( ELeave ) MT_HapticsTestAsyncPlaySender( this );
    }

// - Test methods -----------------------------------------------------------

// ---------------------------------------------------------
// Default test env construction
// ---------------------------------------------------------
//
void MT_HapticsClient::SetupL()
    {
    iHaptics = CHWRMHaptics::NewL( NULL, NULL );
    }

// ---------------------------------------------------------
// Default test env destruction
// ---------------------------------------------------------
//
void MT_HapticsClient::Teardown()
    {
    if( iHaptics )
        {
        delete iHaptics;
        iHaptics = NULL;
        }
    }
    
// ---------------------------------------------------------
// Empty method to be used in test case definition, when
// there is not need to do anything in setup and/or 
// teardown.
// ---------------------------------------------------------
//
void MT_HapticsClient::Dummy()
    {
    // nothing done here
    }

// ---------------------------------------------------------
// Helper methods for creating various effects
// ---------------------------------------------------------
//
void MT_HapticsClient::CreateMagSweepEffect( CHWRMHaptics::THWRMHapticsMagSweepEffect& aEffect )
    {
    aEffect.iDuration    = 1000;
    aEffect.iMagnitude   = 5000;
    aEffect.iStyle       = CHWRMHaptics::EHWRMHapticsStyleSharp;
    aEffect.iAttackTime  = 0;
    aEffect.iAttackLevel = 0;
    aEffect.iFadeTime    = 0;
    aEffect.iFadeLevel   = 0;
    }

void MT_HapticsClient::CreatePeriodicEffect( CHWRMHaptics::THWRMHapticsPeriodicEffect& aEffect )
    {
    aEffect.iDuration    = 1000;
    aEffect.iMagnitude   = 5000;
    aEffect.iPeriod      = 100;
    aEffect.iStyle       = CHWRMHaptics::EHWRMHapticsStyleSharp;
    aEffect.iAttackTime  = 0;
    aEffect.iAttackLevel = 0;
    aEffect.iFadeTime    = 0;
    aEffect.iFadeLevel   = 0;
    }

void MT_HapticsClient::CreateIVTEffect( RBuf8& aBuffer, CHWRMHaptics::THWRMHapticsEffectTypes aType)
    {
    // Note: only magsweep and timeline IVT effects supported so far..
    switch ( aType )
        {
        case CHWRMHaptics::EHWRMHapticsTypePeriodic:
            {
            aBuffer.ReAlloc( sizeof( Periodic4HapticsMT_ivt ) );
            TUint8* tmpPtr = const_cast<TUint8*>( aBuffer.Ptr() );
            memcpy( tmpPtr, Periodic4HapticsMT_ivt, sizeof( Periodic4HapticsMT_ivt ) );
            aBuffer.SetLength( sizeof( Periodic4HapticsMT_ivt ) );
            break;  
            }
        case CHWRMHaptics::EHWRMHapticsTypeMagSweep:
            {
            aBuffer.ReAlloc( sizeof( Magsweep4HapticsMT_ivt ) );
            TUint8* tmpPtr = const_cast<TUint8*>( aBuffer.Ptr() );
            memcpy( tmpPtr, Magsweep4HapticsMT_ivt, sizeof( Magsweep4HapticsMT_ivt ) );
            aBuffer.SetLength( sizeof( Magsweep4HapticsMT_ivt ) );
            break;
            }
        case CHWRMHaptics::EHWRMHapticsTypeTimeline:
            {
            aBuffer.ReAlloc( sizeof( Timeline4HapticsMT_ivt ) );
            TUint8* tmpPtr = const_cast<TUint8*>( aBuffer.Ptr() );
            memcpy( tmpPtr, Timeline4HapticsMT_ivt, sizeof( Timeline4HapticsMT_ivt ) );
            aBuffer.SetLength( sizeof( Timeline4HapticsMT_ivt ) );
            break;  
            }
        default:
            break; // no support for other types at the moment
        }
    }


void MT_HapticsClient::CreateDummyStreamSample( TInt aSize, RBuf8& aBuffer )
    {
    aBuffer.ReAlloc( aSize );
    TUint8* tmpPtr = const_cast<TUint8*>( aBuffer.Ptr() );
    memset( tmpPtr, 0x00, aSize );
    aBuffer.SetLength( aSize );
    }

// ---------------------------------------------------------
// Open and close haptics client-server session, i.e.
// create and delete haptics client.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_CreateAndDeleteL()
    {
    // create iHaptics instance without observers
    iHaptics = CHWRMHaptics::NewL( NULL, NULL );

    // the connection has been opened successfully, if iHaptics is valid
    EUNIT_ASSERT_DESC( iHaptics, "Haptics client creation failed!");

    // the connection is closed, when iHaptics is deleted
    delete iHaptics;
    iHaptics = NULL;
    }
    
// ---------------------------------------------------------
// Create several simultaneous sessions to haptics server.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_SeveralSessionsL()
    {
    // create haptics instances without observers. This creates
    // client-server sessions (session instances in the server)
    CHWRMHaptics* haptics1 = CHWRMHaptics::NewL( NULL, NULL );
   	CleanupStack::PushL( haptics1 ); //remove once CHWRMHaptics::NewLC() is used
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics2 ); //remove once CHWRMHaptics::NewLC() is used
    CHWRMHaptics* haptics3 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics3 ); //remove once CHWRMHaptics::NewLC() is used

    // the connection has been opened successfully, if instances are valid
    EUNIT_ASSERT_DESC( haptics1, "First haptics client creation failed!");
    EUNIT_ASSERT_DESC( haptics2, "Second haptics client creation failed!");
    EUNIT_ASSERT_DESC( haptics3, "Third haptics client creation failed!");

    // delete haptics instances. This removes the sessions from 
    // haptics server, and closes the server, when the last session 
    // is closed
    CleanupStack::PopAndDestroy(3,haptics1);
    //delete haptics1; haptics1 = NULL;
    //delete haptics2; haptics2 = NULL;
    //delete haptics3; haptics3 = NULL;
    }

// ---------------------------------------------------------
// Create several consecutive sessions to haptics server.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_ConsecutiveSessionsL()
    {
    // Do create and delete three times in a row
    T_CreateAndDeleteL();
    T_CreateAndDeleteL();
    T_CreateAndDeleteL();
    }

// ---------------------------------------------------------
// Requests the supported actuators from haptics.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_SupportedActuators()
    {
    TUint32 actuators = 0;
    TInt err = KErrNone;
    err = iHaptics->SupportedActuators( actuators );
    
    EUNIT_ASSERT_DESC( (err == KErrNone), "Error returned from SupportedActuators()!" );
    EUNIT_ASSERT_DESC( actuators, "Actuators empty!");
    }

// ---------------------------------------------------------
// Opens an actuator, which is supported by haptics.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_OpenSupportedActuator()
    {
    TUint32 actuators = 0;
    TInt err = iHaptics->SupportedActuators( actuators );
    
    // Open using first supported logical actuator type
    if( EHWRMLogicalActuatorAny & actuators )
        {
        TRAPD( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorAny ) );
        EUNIT_ASSERT_DESC( (err == KErrNone), "Opening actuator with type Any failed!" );
        }
    else if( EHWRMLogicalActuatorDevice & actuators )
        {
        TRAPD( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorDevice ) );
        EUNIT_ASSERT_DESC( (err == KErrNone), "Opening actuator with type Device failed!" );
        }
    else if( EHWRMLogicalActuatorPrimaryDisplay & actuators )
        {
        TRAPD( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorPrimaryDisplay ) );
        EUNIT_ASSERT_DESC( (err == KErrNone), "Opening actuator with type PrimaryDisplay failed!" );
        }
    else if( EHWRMLogicalActuatorSecondaryDisplay & actuators )
        {
        TRAPD( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorSecondaryDisplay ) );
        EUNIT_ASSERT_DESC( (err == KErrNone), "Opening actuator with type SecondaryDisplay failed!" );
        }
    else if( EHWRMLogicalActuatorGame & actuators )
        {
        TRAPD( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorGame ) );
        EUNIT_ASSERT_DESC( (err == KErrNone), "Opening actuator with type Game failed!" );
        }
    else if( EHWRMLogicalActuatorGameLeft & actuators )
        {
        TRAPD( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorGameLeft ) );
        EUNIT_ASSERT_DESC( (err == KErrNone), "Opening actuator with type GameLeft failed!" );
        }
    else if( EHWRMLogicalActuatorGameRight & actuators )
        {
        TRAPD( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorGameRight ) );
        EUNIT_ASSERT_DESC( (err == KErrNone), "Opening actuator with type GameRight failed!" );
        }
    else if( EHWRMLogicalActuatorExternalVibra & actuators )
        {
        TRAPD( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorExternalVibra ) );
        EUNIT_ASSERT_DESC( (err == KErrNone), "Opening actuator with type ExternalVibra failed!" );
        }
    }

// ---------------------------------------------------------
// Attemps to open an actuator, which is NOT supported 
// by haptics.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_OpenNotSupportedActuator()
    {
    TUint32 actuators = 0;
    TInt err = iHaptics->SupportedActuators( actuators );
    
    EUNIT_ASSERT_DESC( (err == KErrNone), "Error returned from SupportedActuators()!" );

    // Open using first NOT supported logical actuator type
    if( !( EHWRMLogicalActuatorAny & actuators ) )
        {
        TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorAny ) );
        }
    else if( !( EHWRMLogicalActuatorDevice & actuators ) )
        {
        TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorDevice ) );
        }
    else if( !( EHWRMLogicalActuatorPrimaryDisplay & actuators ) )
        {
        TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorPrimaryDisplay ) );
        }
    else if( !( EHWRMLogicalActuatorSecondaryDisplay & actuators ) )
        {
        TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorSecondaryDisplay ) );
        }
    else if( !( EHWRMLogicalActuatorGame & actuators ) )
        {
        TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorGame ) );
        }
    else if( !( EHWRMLogicalActuatorGameLeft & actuators ) )
        {
        TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorGameLeft ) );
        }
    else if( !( EHWRMLogicalActuatorGameRight & actuators ) )
        {
        TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorGameRight ) );
        }
    else if( !( EHWRMLogicalActuatorExternalVibra & actuators ) )
        {
        TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorExternalVibra ) );
        }
    
    EUNIT_ASSERT_DESC( (err == KErrNotSupported), "Invalid error code value!" );
    EUNIT_ASSERT_DESC( (err != KErrNone), "Opening unsupported actuator succeeded!" );
    }

// ---------------------------------------------------------
// Attemps to open actuators, using all available logical
// actuator types.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_OpenAllActuators()
    {
    TInt actuatorMask = EHWRMLogicalActuatorPrimaryDisplay | EHWRMLogicalActuatorSecondaryDisplay;
    
    TRAPD( err, iHaptics->OpenActuatorL( (THWRMLogicalActuators)actuatorMask ) );
    EUNIT_ASSERT_DESC( (err == KErrArgument), "Opening actuator with several type mask succeeded!" );

    TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorPrimaryDisplay ) );
    EUNIT_ASSERT_DESC( (err == KErrNotSupported), "Opening actuator with type PrimaryDisplay succeeded!" );

    TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorSecondaryDisplay ) );
    EUNIT_ASSERT_DESC( (err == KErrNotSupported), "Opening actuator with type SecondaryDisplay succeeded!" );

    TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorGame ) );
    EUNIT_ASSERT_DESC( (err == KErrNotSupported), "Opening actuator with type Game succeeded!" );

    TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorGameLeft ) );
    EUNIT_ASSERT_DESC( (err == KErrNotSupported), "Opening actuator with type GameLeft succeeded!" );

    TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorGameRight ) );
    EUNIT_ASSERT_DESC( (err == KErrNotSupported), "Opening actuator with type GameRight succeeded!" );

    TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorAny ) );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Opening actuator with type Any failed!" );

    TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorAny ) );
    EUNIT_ASSERT_DESC( (err == KErrAlreadyExists), "Opening actuator with type Any; wrong error code" );

    TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorDevice ) );
    EUNIT_ASSERT_DESC( (err == KErrInUse), "Opening actuator with type Device; wrong error code" );

    TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorExternalVibra ) );
    EUNIT_ASSERT_DESC( (err == KErrInUse), "Opening actuator with type ExternalVibra; wrong error code!" );
    }

// ---------------------------------------------------------
// Creates a couple of haptic client instances, and opens
// actuators in both of them.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_SeveralClientsWithOpenActuatorsL()
    {
    // create first haptics client instance
    CHWRMHaptics* haptics1 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics1 );

    // create second haptics client instance
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics2 );

    // fetch supported actuator types
    TUint32 actuators = 0;
    TInt err = haptics1->SupportedActuators( actuators );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Error returned from SupportedActuators()!" );

    // open actuators for both instances using supported types
    if( EHWRMLogicalActuatorAny & actuators )
        {
        TRAPD( err, haptics1->OpenActuatorL( EHWRMLogicalActuatorAny ) );
        EUNIT_ASSERT_DESC( (err == KErrNone), "Opening actuator with type Any failed!" );

        TRAP( err, haptics2->OpenActuatorL( EHWRMLogicalActuatorAny ) );
        EUNIT_ASSERT_DESC( (err == KErrNone), "Opening actuator with type Any failed!" );
        }
    else
        {
        EUNIT_ASSERT_DESC( EFalse, "Actuator type Any not supported!" );
        }

    CleanupStack::PopAndDestroy( haptics2 );
    CleanupStack::PopAndDestroy( haptics1 );
    }

// ---------------------------------------------------------
// Plays a magsweep effect using synchronous API call.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_PlayMagSweepEffectSynchronous()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // create a magsweep effect
    CHWRMHaptics::THWRMHapticsMagSweepEffect effect;
    CreateMagSweepEffect( effect );
    
    // play effect  
    TInt effectHandle;
    TInt err = iHaptics->PlayMagSweepEffect( effect, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing magsweep effect synchronously failed!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing magsweep effect synchronously returned empty effect handle!" );

    // clean up.. (Teardown will complete these)
    err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing magsweep effect synchronously failed! (StopAllPlayingEffects)" );
    }
    

// ---------------------------------------------------------
// Plays a magsweep effect using asynchronous API call.
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_PlayMagSweepEffectAsynchronous()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // create a magsweep effect
    CHWRMHaptics::THWRMHapticsMagSweepEffect effect;
    CreateMagSweepEffect( effect );
    
    // play effect  
    TInt effectHandle;
    TRequestStatus status = KRequestPending;
    iHaptics->PlayMagSweepEffect( effect, effectHandle, status ); 
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Playing magsweep effect asynchronously failed!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing magsweep effect asynchronously returned empty effect handle!" );

    // clean up.. (Teardown will complete these)
    TInt err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing magsweep effect asynchronously failed! (StopAllPlayingEffects)" );
    
    }
     

// ---------------------------------------------------------
// Plays a periodic effect using synchronous API call.
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_PlayPeriodicEffectSynchronous()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // create a magsweep effect
    CHWRMHaptics::THWRMHapticsPeriodicEffect effect;
    CreatePeriodicEffect( effect );
    
    // play effect  
    TInt effectHandle;
    TInt err = iHaptics->PlayPeriodicEffect( effect, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing periodic effect synchronously failed!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing periodic effect returned empty effect handle!" );

    // clean up.. (Teardown will complete these)
    err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing periodic effect synchronously failed! (StopAllPlayingEffects)" );
    }
    
// ---------------------------------------------------------
// Plays a periodic effect using synchronous API call.
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_PlayPeriodicEffectAsynchronous()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // create a magsweep effect
    CHWRMHaptics::THWRMHapticsPeriodicEffect effect;
    CreatePeriodicEffect( effect );
    
    // play effect  
    TInt effectHandle;
    TRequestStatus status = KRequestPending;
    iHaptics->PlayPeriodicEffect( effect, effectHandle, status ); 
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Playing periodic effect asynchronously failed!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing periodic effect asynchronously returned empty effect handle!" );

    // clean up.. (Teardown will complete these)
    TInt err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing periodic effect asynchronously failed! (StopAllPlayingEffects)" );
    
    }


// ---------------------------------------------------------
// Plays an IVT effect using synchronous API call.
// The effect is given as TDes8C& parameter directly to the API call.
// ---------------------------------------------------------
// 
void MT_HapticsClient::T_PlayIVTEffectWithDirectDataSynchronous()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // read magsweep effect to buffer
    RBuf8 effBuf;
    CleanupClosePushL( effBuf );
    CreateIVTEffect( effBuf, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    
    // play the effect  
    TInt effectHandle;
    TInt err = iHaptics->PlayEffect( effBuf, 0, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect synchronously failed!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playingeffect IVT synchronously returned empty effect handle!" );

    // clean up.. (Teardown will complete these)
    CleanupStack::PopAndDestroy(); // effBuf
    err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect synchronously failed! (StopAllPlayingEffects)" );

    }
    
// ---------------------------------------------------------
// Plays an IVT effect using asynchronous API call.
// The effect is given as TDes8C& parameter directly to the API call.
// ---------------------------------------------------------
//   
void MT_HapticsClient::T_PlayIVTEffectWithDirectDataAsynchronous()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // read magsweep effect to buffer
    RBuf8 effBuf;
    CleanupClosePushL( effBuf );
    CreateIVTEffect( effBuf, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    
    // play effect  
    TInt effectHandle;
    TRequestStatus status = KRequestPending;
    iHaptics->PlayEffect( effBuf, 0, effectHandle, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Playing IVT effect asynchronously failed!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing IVT effect asynchronously returned empty effect handle!" );
    
    // clean up.. (Teardown will complete these)
    CleanupStack::PopAndDestroy(); // effBuf
    TInt err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect asynchronously failed! (StopAllPlayingEffects)" );

    }
    
// ---------------------------------------------------------
// Plays repeatedly an IVT effect using synchronous API call.
// The effect is given as TDes8C& parameter directly to the API call.
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_PlayIVTEffectRepeatWithDirectDataSynchronous()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // read timeline effect to buffer
    RBuf8 effBuf;
    CleanupClosePushL( effBuf );
    CreateIVTEffect( effBuf, CHWRMHaptics::EHWRMHapticsTypeTimeline );
    
    // play the effect (repeat 2 times)
    TInt effectHandle;
    TInt err = iHaptics->PlayEffectRepeat( effBuf, 0, 2, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect repeatedly synchronously failed!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing IVT effect repeatedly synchronously returned empty effect handle!" );

    // clean up.. (Teardown will complete these)
    CleanupStack::PopAndDestroy(); // effBuf
    err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect repeatedly synchronously failed! (StopAllPlayingEffects)" );

    }

// ---------------------------------------------------------
// Plays repeatedly an IVT effect using asynchronous API call.
// The effect is given as TDes8C& parameter directly to the API call.
// ---------------------------------------------------------
//      
void MT_HapticsClient::T_PlayIVTEffectRepeatWithDirectDataAsynchronous()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // read timeline effect to buffer
    RBuf8 effBuf;
    CleanupClosePushL( effBuf );
    CreateIVTEffect( effBuf, CHWRMHaptics::EHWRMHapticsTypeTimeline );
    
    // play effect  (repeat 2 times)
    TInt effectHandle;
    TRequestStatus status = KRequestPending;
    iHaptics->PlayEffectRepeat( effBuf, 0, 2, effectHandle, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Playing IVT effect repeatedly asynchronously failed!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing IVT effect repeatedly asynchronously returned empty effect handle!" );
    
    // clean up.. (Teardown will complete these)
    CleanupStack::PopAndDestroy(); // effBuf
    TInt err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect repeatedly asynchronously failed! (StopAllPlayingEffects)" );

    }

// ---------------------------------------------------------
// Gets the effect state while an effect:
//    a) is being played
//    b) has been paused
//    c) has been completed (i.e., is not playing any more)
// ---------------------------------------------------------
// 
void MT_HapticsClient::T_GetEffectState()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // create a magsweep effect
    CHWRMHaptics::THWRMHapticsMagSweepEffect effect;
    CreateMagSweepEffect( effect );
    
    // play effect  
    TInt effectHandle;
    TInt err = iHaptics->PlayMagSweepEffect( effect, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "GetEffectState failed! (playing returned error)" );
    EUNIT_ASSERT_DESC( effectHandle, "GetEffectState failed! (empty effect handle)" );

    // get state
    TInt effectState;
    err = iHaptics->GetEffectState( effectHandle, effectState );
    EUNIT_ASSERT_DESC( (err == KErrNone), "GetEffectState failed! (1st call)" );
    EUNIT_ASSERT_DESC( (effectState == CHWRMHaptics::EHWRMHapticsEffectPlaying), "GetEffectState failed! (1st call - wrong state)" );
    
    // pause the effect and get state
    err = iHaptics->PausePlayingEffect( effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "GetEffectState failed! (Pause)" );

    err = iHaptics->GetEffectState( effectHandle, effectState );
    EUNIT_ASSERT_DESC( (err == KErrNone), "GetEffectState failed! (2nd call)" );
    EUNIT_ASSERT_DESC( (effectState == CHWRMHaptics::EHWRMHapticsEffectPaused), "GetEffectState failed! (2nd call - wrong state)" ); 

    // stop the effect and get state again
    err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "GetEffectState failed! (StopAllPlayingEffects)" );
    
    err = iHaptics->GetEffectState( effectHandle, effectState );
    EUNIT_ASSERT_DESC( (err == KErrNone), "GetEffectState failed! (3rd call)" );
    EUNIT_ASSERT_DESC( (effectState == CHWRMHaptics::EHWRMHapticsEffectNotPlaying), "GetEffectState failed! (3rd call - wrong state)" );
    }

// ---------------------------------------------------------
// Plays repeatedly an IVT effect using synchronous API call.
// The effect is given as TDes8C& parameter directly to the API call.
// Since the effect in question is not a timeline effect (in 
// this test we use magsweep effect instead), the effect is 
// played only once.
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_PlayNonTimelineIVTEffectRepeatWithDirectDataSynchronous()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // read magsweep effect to buffer
    RBuf8 effBuf;
    CleanupClosePushL( effBuf );
    CreateIVTEffect( effBuf, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    
    // play the effect (try to repeat 2 times.. in practice will only repeat once 
    // due to the effect being non timeline effect)
    TInt effectHandle;
    TInt err = iHaptics->PlayEffectRepeat( effBuf, 0, 2, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect (non-timeline) repeatedly synchronously failed!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing IVT effect (non-timeline) repeatedly synchronously returned empty effect handle!" );

    // clean up.. (Teardown will complete these)
    CleanupStack::PopAndDestroy(); // effBuf
    err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect (non-timeline) repeatedly synchronously failed! (StopAllPlayingEffects)" );

    }

// ---------------------------------------------------------
// Plays repeatedly an IVT effect using asynchronous API call.
// The effect is given as TDes8C& parameter directly to the API call.
// Since the effect in question is not a timeline effect (in 
// this test we use magsweep effect instead), the effect is 
// played only once.
// ---------------------------------------------------------
// 
void MT_HapticsClient::T_PlayNonTimelineIVTEffectRepeatWithDirectDataAsynchronous()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // read magsweep effect to buffer
    RBuf8 effBuf;
    CleanupClosePushL( effBuf );
    CreateIVTEffect( effBuf, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    
    // play the effect (try to repeat 2 times.. in practice will only repeat once 
    // due to the effect being non timeline effect)
    TInt effectHandle;
    TRequestStatus status = KRequestPending;
    iHaptics->PlayEffectRepeat( effBuf, 0, 2, effectHandle, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Playing IVT effect (non-timeline) repeatedly asynchronously failed!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing IVT effect (non-timeline) repeatedly asynchronously returned empty effect handle!" );
    
    // clean up.. (Teardown will complete these)
    CleanupStack::PopAndDestroy(); // effBuf
    TInt err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect (non-timeline) repeatedly asynchronously failed! (StopAllPlayingEffects)" );

    }
    
void MT_HapticsClient::T_PlayIVTEffectRepeatWithDirectDataWithZeroRepeatValueSynchronous()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // read timeline effect to buffer
    RBuf8 effBuf;
    CleanupClosePushL( effBuf );
    CreateIVTEffect( effBuf, CHWRMHaptics::EHWRMHapticsTypeTimeline );
    
    // play the effect (repeat 0 times)
    TInt effectHandle;
    TInt err = iHaptics->PlayEffectRepeat( effBuf, 0, 0, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect repeatedly (zero repeats) synchronously failed!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing IVT effect repeatedly (zero repeats) synchronously returned empty effect handle!" );

    // clean up.. (Teardown will complete these)
    CleanupStack::PopAndDestroy(); // effBuf
    err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect repeatedly (zero repeats) synchronously failed! (StopAllPlayingEffects)" );

    }


void MT_HapticsClient::T_PlayIVTEffectRepeatWithDirectDataWithZeroRepeatValueAsynchronous()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // read timeline effect to buffer
    RBuf8 effBuf;
    CleanupClosePushL( effBuf );
    CreateIVTEffect( effBuf, CHWRMHaptics::EHWRMHapticsTypeTimeline );
    
    // play effect  (repeat 0 times)
    TInt effectHandle;
    TRequestStatus status = KRequestPending;
    iHaptics->PlayEffectRepeat( effBuf, 0, 0, effectHandle, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Playing IVT effect repeatedly (zero repeats) asynchronously failed!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing IVT effect repeatedly (zero repeats) asynchronously returned empty effect handle!" );
    
    // clean up.. (Teardown will complete these)
    CleanupStack::PopAndDestroy(); // effBuf
    TInt err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect repeatedly (zero repeats) asynchronously failed! (StopAllPlayingEffects)" );

    }   

    
// ---------------------------------------------------------
// Loading IVT data 
// ---------------------------------------------------------
//
TInt MT_HapticsClient::LoadIVTDataInternal( TInt& aFileHandle, 
                                            CHWRMHaptics::THWRMHapticsEffectTypes aType,
                                            TBool aOpenActuator )
    {
    // NOTE: we can use the same IVT data as was used in previous "call PlayXXXEffect()
    // with direct data cases (i.e., there's no need e.g. to read data from file..)
    
    // open actuator.. 
    if ( aOpenActuator )
        {
        T_OpenSupportedActuator();
        }

    // read effect to buffer
    RBuf8 effBuf;
    CleanupClosePushL( effBuf );
    CreateIVTEffect( effBuf, aType );

    TInt retVal = iHaptics->LoadEffectData( effBuf, aFileHandle );
    
    CleanupStack::PopAndDestroy(); // effBuf
    return retVal;
    }

TInt MT_HapticsClient::LoadIVTDataInternal( TInt& aFileHandle, 
                                            const TUint8 aIvtData[],
                                            TInt aIvtDataSize,
                                            TBool aOpenActuator )
    {
    // open actuator.. 
    if ( aOpenActuator )
        {
        T_OpenSupportedActuator();
        }

    // read effect to buffer
    RBuf8 effBuf;
    CleanupClosePushL( effBuf );
    effBuf.ReAlloc( aIvtDataSize );
    TUint8* tmpPtr = const_cast<TUint8*>( effBuf.Ptr() );
    memcpy( tmpPtr, aIvtData, aIvtDataSize );
    effBuf.SetLength( aIvtDataSize );

    TInt retVal = iHaptics->LoadEffectData( effBuf, aFileHandle );
    
    CleanupStack::PopAndDestroy(); // effBuf
    return retVal;
    }

void MT_HapticsClient::T_LoadIVTData()
    {
    TInt fileHandle;
    TInt err = LoadIVTDataInternal( fileHandle, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed!" );
    EUNIT_ASSERT_DESC( fileHandle, "Loading IVT data failed, empty fileHandle!" );
    
    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed! (LoadIVTData case)" );
    }


// ---------------------------------------------------------
// Deleting previously loaded IVT data by calling DeleteIVTData
// ---------------------------------------------------------
//
void MT_HapticsClient::T_DeleteIVTData()
    {
    TInt fileHandle;
    TInt err = LoadIVTDataInternal( fileHandle, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (DeleteIVTData case)!" );
    EUNIT_ASSERT_DESC( fileHandle, "Loading IVT data failed, empty fileHandle (DeleteIVTData case)!" );
    
    err = iHaptics->DeleteEffectData( fileHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete IVT data failed!" );
    }


// ---------------------------------------------------------
// Deleting previously loaded IVT data by calling DeleteAllIVTData
// instead of DeleteIVTData
// ---------------------------------------------------------
//
void MT_HapticsClient::T_DeleteAllIVTData()
    {
    TInt fileHandle;
    TInt err = LoadIVTDataInternal( fileHandle, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (DeleteAllIVTData case)!" );
    EUNIT_ASSERT_DESC( fileHandle, "Loading IVT data failed, empty fileHandle (DeleteAllIVTData case)!" );
    
    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed!" );
    }


// ---------------------------------------------------------
// Loads and deletes IVT-data.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_LoadAndDeleteIVTData()
    {
    TInt fileHandle1 = KErrNotFound;
    TInt fileHandle2 = KErrNotFound;
    TInt fileHandle3 = KErrNotFound;

    // load IVT-data (open actuator on the first load only inside LoadIVTDataInternal())
    TInt err1 = LoadIVTDataInternal( fileHandle1, CHWRMHaptics::EHWRMHapticsTypePeriodic );
    TInt err2 = LoadIVTDataInternal( fileHandle2, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    TInt err3 = LoadIVTDataInternal( fileHandle3, CHWRMHaptics::EHWRMHapticsTypeTimeline, EFalse );
    EUNIT_ASSERT_DESC( (err1 == KErrNone), "Loading IVT data1 failed!" );
    EUNIT_ASSERT_DESC( (err2 == KErrNone), "Loading IVT data2 failed!" );
    EUNIT_ASSERT_DESC( (err3 == KErrNone), "Loading IVT data3 failed!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle1!" );
    EUNIT_ASSERT_DESC( fileHandle2, "Loading IVT data failed, empty fileHandle2!" );
    EUNIT_ASSERT_DESC( fileHandle3, "Loading IVT data failed, empty fileHandle3!" );

    // check that filehandles are unique
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle3), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle2), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle2 != fileHandle3), "Received same filehandles!" );

    // delete each IVT-data
    TInt err = iHaptics->DeleteEffectData( fileHandle1 );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete IVT data failed!" );
    err = iHaptics->DeleteEffectData( fileHandle2 );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete IVT data failed!" );
    err = iHaptics->DeleteEffectData( fileHandle3 );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete IVT data failed!" );

    // reload IVT-data
    fileHandle1 = KErrNotFound;
    fileHandle2 = KErrNotFound;
    fileHandle3 = KErrNotFound;
    err1 = LoadIVTDataInternal( fileHandle1, CHWRMHaptics::EHWRMHapticsTypePeriodic, EFalse );
    err2 = LoadIVTDataInternal( fileHandle2, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    err3 = LoadIVTDataInternal( fileHandle3, CHWRMHaptics::EHWRMHapticsTypeTimeline, EFalse );
    EUNIT_ASSERT_DESC( (err1 == KErrNone), "Loading IVT data1 failed!" );
    EUNIT_ASSERT_DESC( (err2 == KErrNone), "Loading IVT data2 failed!" );
    EUNIT_ASSERT_DESC( (err3 == KErrNone), "Loading IVT data3 failed!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle1!" );
    EUNIT_ASSERT_DESC( fileHandle2, "Loading IVT data failed, empty fileHandle2!" );
    EUNIT_ASSERT_DESC( fileHandle3, "Loading IVT data failed, empty fileHandle3!" );

    // check that filehandles are unique
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle3), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle2), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle2 != fileHandle3), "Received same filehandles!" );
    }


// ---------------------------------------------------------
// Loads and deletes IVT-data (mixed order).
// ---------------------------------------------------------
//
void MT_HapticsClient::T_LoadAndDeleteIVTDataMixed()
    {
    TInt fileHandle1 = KErrNotFound;
    TInt fileHandle2 = KErrNotFound;
    TInt fileHandle3 = KErrNotFound;

    // load IVT-data (open actuator on the first load only inside LoadIVTDataInternal())
    TInt err1 = LoadIVTDataInternal( fileHandle1, CHWRMHaptics::EHWRMHapticsTypePeriodic );
    TInt err2 = LoadIVTDataInternal( fileHandle2, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    TInt err3 = LoadIVTDataInternal( fileHandle3, CHWRMHaptics::EHWRMHapticsTypeTimeline, EFalse );
    EUNIT_ASSERT_DESC( (err1 == KErrNone), "Loading IVT data1 failed!" );
    EUNIT_ASSERT_DESC( (err2 == KErrNone), "Loading IVT data2 failed!" );
    EUNIT_ASSERT_DESC( (err3 == KErrNone), "Loading IVT data3 failed!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle1!" );
    EUNIT_ASSERT_DESC( fileHandle2, "Loading IVT data failed, empty fileHandle2!" );
    EUNIT_ASSERT_DESC( fileHandle3, "Loading IVT data failed, empty fileHandle3!" );

    // check that filehandles are unique
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle3), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle2), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle2 != fileHandle3), "Received same filehandles!" );

    // delete second IVT-data
    TInt err = iHaptics->DeleteEffectData( fileHandle2 );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete IVT data failed!" );

    // load third IVT-data once more (filehandle should not change)
    TInt newHandle = KErrNotFound;
    err3 = LoadIVTDataInternal( newHandle, CHWRMHaptics::EHWRMHapticsTypeTimeline, EFalse );
    EUNIT_ASSERT_DESC( (err3 == KErrNone), "Loading IVT data3 failed!" );
    EUNIT_ASSERT_DESC( newHandle, "Loading IVT data failed, empty newHandle!" );
    EUNIT_ASSERT_DESC( (fileHandle3 == newHandle), "Received incorrect filehandle!" );

    // load second IVT-data once more (filehandle should change)
    newHandle = KErrNotFound;
    err2 = LoadIVTDataInternal( newHandle, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    EUNIT_ASSERT_DESC( (err2 == KErrNone), "Loading IVT data2 failed!" );
    EUNIT_ASSERT_DESC( newHandle, "Loading IVT data failed, empty newHandle!" );
    EUNIT_ASSERT_DESC( (fileHandle2 != newHandle), "Received incorrect filehandle!" );
    fileHandle2 = newHandle;

    // delete all IVT-data
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed!" );

    fileHandle1 = KErrNotFound;
    fileHandle2 = KErrNotFound;
    fileHandle3 = KErrNotFound;

    // reload IVT-data
    err1 = LoadIVTDataInternal( fileHandle1, CHWRMHaptics::EHWRMHapticsTypePeriodic, EFalse );
    err2 = LoadIVTDataInternal( fileHandle2, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    err3 = LoadIVTDataInternal( fileHandle3, CHWRMHaptics::EHWRMHapticsTypeTimeline, EFalse );
    EUNIT_ASSERT_DESC( (err1 == KErrNone), "Loading IVT data1 failed!" );
    EUNIT_ASSERT_DESC( (err2 == KErrNone), "Loading IVT data2 failed!" );
    EUNIT_ASSERT_DESC( (err3 == KErrNone), "Loading IVT data3 failed!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle1!" );
    EUNIT_ASSERT_DESC( fileHandle2, "Loading IVT data failed, empty fileHandle2!" );
    EUNIT_ASSERT_DESC( fileHandle3, "Loading IVT data failed, empty fileHandle3!" );

    // check that filehandles are unique
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle3), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle2), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle2 != fileHandle3), "Received same filehandles!" );
    }


// ---------------------------------------------------------
// Loads and deletes IVT-data. Plays using the filehandles
// received by calling the synchronous play-method.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_LoadDeletePlaySync()
    {
    TInt effectHandle = 0;
    TInt fileHandle1 = KErrNotFound;
    TInt fileHandle2 = KErrNotFound;
    TInt fileHandle3 = KErrNotFound;

    // load IVT-data (open actuator on the first load only inside LoadIVTDataInternal())
    TInt err1 = LoadIVTDataInternal( fileHandle1, CHWRMHaptics::EHWRMHapticsTypePeriodic );
    TInt err2 = LoadIVTDataInternal( fileHandle2, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    TInt err3 = LoadIVTDataInternal( fileHandle3, CHWRMHaptics::EHWRMHapticsTypeTimeline, EFalse );
    EUNIT_ASSERT_DESC( (err1 == KErrNone), "Loading IVT data1 failed!" );
    EUNIT_ASSERT_DESC( (err2 == KErrNone), "Loading IVT data2 failed!" );
    EUNIT_ASSERT_DESC( (err3 == KErrNone), "Loading IVT data3 failed!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle1!" );
    EUNIT_ASSERT_DESC( fileHandle2, "Loading IVT data failed, empty fileHandle2!" );
    EUNIT_ASSERT_DESC( fileHandle3, "Loading IVT data failed, empty fileHandle3!" );

    // check that filehandles are unique
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle3), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle2), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle2 != fileHandle3), "Received same filehandles!" );

    // play the first effect of each data
    TInt err = iHaptics->PlayEffect( fileHandle1, 0, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );
    err = iHaptics->PlayEffect( fileHandle2, 0, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );
    err = iHaptics->PlayEffect( fileHandle3, 0, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );

    // play the first effect of the second data
    err = iHaptics->PlayEffect( fileHandle2, 0, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );

    // delete second IVT-data
    err = iHaptics->DeleteEffectData( fileHandle2 );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete IVT data failed!" );

    // play the first effect of the second data
    err = iHaptics->PlayEffect( fileHandle2, 0, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrBadHandle), "Playing deleted IVT-data succeeded!" );

    // load second IVT-data once more (filehandle should change)
    TInt newHandle = KErrNotFound;
    err2 = LoadIVTDataInternal( newHandle, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    EUNIT_ASSERT_DESC( (err2 == KErrNone), "Loading IVT data2 failed!" );
    EUNIT_ASSERT_DESC( newHandle, "Loading IVT data failed, empty newHandle!" );
    EUNIT_ASSERT_DESC( (fileHandle2 != newHandle), "Received incorrect filehandle!" );
    fileHandle2 = newHandle;

    // play the first effect of the second data
    err = iHaptics->PlayEffect( fileHandle2, 0, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );

    // delete all IVT-data
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed!" );

    // play the first effect of each data (each should fail, since data is deleted)
    err = iHaptics->PlayEffect( fileHandle1, 0, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrBadHandle), "Playing deleted IVT-data succeeded!" );
    err = iHaptics->PlayEffect( fileHandle2, 0, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrBadHandle), "Playing deleted IVT-data succeeded!" );
    err = iHaptics->PlayEffect( fileHandle3, 0, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrBadHandle), "Playing deleted IVT-data succeeded!" );

    // load IVT-data
    err1 = LoadIVTDataInternal( fileHandle1, CHWRMHaptics::EHWRMHapticsTypePeriodic, EFalse );
    err2 = LoadIVTDataInternal( fileHandle2, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    err3 = LoadIVTDataInternal( fileHandle3, CHWRMHaptics::EHWRMHapticsTypeTimeline, EFalse );
    EUNIT_ASSERT_DESC( (err1 == KErrNone), "Loading IVT data1 failed!" );
    EUNIT_ASSERT_DESC( (err2 == KErrNone), "Loading IVT data2 failed!" );
    EUNIT_ASSERT_DESC( (err3 == KErrNone), "Loading IVT data3 failed!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle1!" );
    EUNIT_ASSERT_DESC( fileHandle2, "Loading IVT data failed, empty fileHandle2!" );
    EUNIT_ASSERT_DESC( fileHandle3, "Loading IVT data failed, empty fileHandle3!" );

    // check that filehandles are unique
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle3), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle2), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle2 != fileHandle3), "Received same filehandles!" );

    // play the first effect of each data
    err = iHaptics->PlayEffect( fileHandle1, 0, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );
    err = iHaptics->PlayEffect( fileHandle2, 0, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );
    err = iHaptics->PlayEffect( fileHandle3, 0, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );
    }


// ---------------------------------------------------------
// Loads and deletes IVT-data. Plays using the filehandles
// received by calling the asynchronous play-method.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_LoadDeletePlayAsync()
    {
    TRequestStatus status1 = KRequestPending;
    TRequestStatus status2 = KRequestPending;
    TRequestStatus status3 = KRequestPending;

    TInt effectHandle = 0;

    TInt fileHandle1 = KErrNotFound;
    TInt fileHandle2 = KErrNotFound;
    TInt fileHandle3 = KErrNotFound;

    // load IVT-data (open actuator on the first load only inside LoadIVTDataInternal())
    TInt err1 = LoadIVTDataInternal( fileHandle1, CHWRMHaptics::EHWRMHapticsTypePeriodic );
    TInt err2 = LoadIVTDataInternal( fileHandle2, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    TInt err3 = LoadIVTDataInternal( fileHandle3, CHWRMHaptics::EHWRMHapticsTypeTimeline, EFalse );
    EUNIT_ASSERT_DESC( (err1 == KErrNone), "Loading IVT data1 failed!" );
    EUNIT_ASSERT_DESC( (err2 == KErrNone), "Loading IVT data2 failed!" );
    EUNIT_ASSERT_DESC( (err3 == KErrNone), "Loading IVT data3 failed!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle1!" );
    EUNIT_ASSERT_DESC( fileHandle2, "Loading IVT data failed, empty fileHandle2!" );
    EUNIT_ASSERT_DESC( fileHandle3, "Loading IVT data failed, empty fileHandle3!" );

    // check that filehandles are unique
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle3), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle2), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle2 != fileHandle3), "Received same filehandles!" );

    // play the first effect of each data
    // iHaptics->PlayEffect( fileHandle1, 0, effectHandle, status1 );
    // LoopForCompletion( status1 );
    iSender->Play( iHaptics, fileHandle1, 0, effectHandle, status1 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status1 == KErrNone), "Playing loaded IVT data failed (asynch)!" );

    iSender->Play( iHaptics, fileHandle2, 0, effectHandle, status2 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status2.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );

    iSender->Play( iHaptics, fileHandle3, 0, effectHandle, status3 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status3.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );

    // play the first effect of the second data
    status2 = KRequestPending;
    iSender->Play( iHaptics, fileHandle2, 0, effectHandle, status2 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status2.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );

    // delete second IVT-data
    TInt err = iHaptics->DeleteEffectData( fileHandle2 );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete IVT data failed!" );

    // play the first effect of the second data
    status2 = KRequestPending;
    iSender->Play( iHaptics, fileHandle2, 0, effectHandle, status2 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status2.Int() == KErrBadHandle), "Playing deleted IVT data succeeded!" );

    // load second IVT-data once more (filehandle should change)
    TInt newHandle = KErrNotFound;
    err2 = LoadIVTDataInternal( newHandle, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    EUNIT_ASSERT_DESC( (err2 == KErrNone), "Loading IVT data2 failed!" );
    EUNIT_ASSERT_DESC( newHandle, "Loading IVT data failed, empty newHandle!" );
    EUNIT_ASSERT_DESC( (fileHandle2 != newHandle), "Received incorrect filehandle!" );
    fileHandle2 = newHandle;

    // play the first effect of the second data
    status2 = KRequestPending;
    iSender->Play( iHaptics, fileHandle2, 0, effectHandle, status2 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status2.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );

    // delete all IVT-data
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed!" );

    // play the first effect of each data (each should fail, since data is deleted)
    status1 = KRequestPending;
    status2 = KRequestPending;
    status3 = KRequestPending;
    iSender->Play( iHaptics, fileHandle1, 0, effectHandle, status1 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status1.Int() == KErrBadHandle), "Playing deleted IVT-data succeeded!" );
    iSender->Play( iHaptics, fileHandle2, 0, effectHandle, status2 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status2.Int() == KErrBadHandle), "Playing deleted IVT-data succeeded!" );
    iSender->Play( iHaptics, fileHandle3, 0, effectHandle, status3 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status3.Int() == KErrBadHandle), "Playing deleted IVT-data succeeded!" );

    // load IVT-data
    err1 = LoadIVTDataInternal( fileHandle1, CHWRMHaptics::EHWRMHapticsTypePeriodic, EFalse );
    err2 = LoadIVTDataInternal( fileHandle2, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    err3 = LoadIVTDataInternal( fileHandle3, CHWRMHaptics::EHWRMHapticsTypeTimeline, EFalse );
    EUNIT_ASSERT_DESC( (err1 == KErrNone), "Loading IVT data1 failed!" );
    EUNIT_ASSERT_DESC( (err2 == KErrNone), "Loading IVT data2 failed!" );
    EUNIT_ASSERT_DESC( (err3 == KErrNone), "Loading IVT data3 failed!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle1!" );
    EUNIT_ASSERT_DESC( fileHandle2, "Loading IVT data failed, empty fileHandle2!" );
    EUNIT_ASSERT_DESC( fileHandle3, "Loading IVT data failed, empty fileHandle3!" );

    // check that filehandles are unique
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle3), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle2), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle2 != fileHandle3), "Received same filehandles!" );

    // play the first effect of each data
    status1 = KRequestPending;
    status2 = KRequestPending;
    status3 = KRequestPending;
    iSender->Play( iHaptics, fileHandle1, 0, effectHandle, status1 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status1.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );
    iSender->Play( iHaptics, fileHandle2, 0, effectHandle, status2 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status2.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );
    iSender->Play( iHaptics, fileHandle3, 0, effectHandle, status3 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status3.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );
    }


// ---------------------------------------------------------
// Loads and deletes IVT-data. Plays using the filehandles
// received by calling the synchronous PlayRepeat-method.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_LoadDeletePlayRepeatSync()
    {
    TInt effectHandle = 0;
    TInt repeat = 2;
    TInt fileHandle1 = KErrNotFound;
    TInt fileHandle2 = KErrNotFound;
    TInt fileHandle3 = KErrNotFound;

    // load IVT-data (open actuator on the first load only inside LoadIVTDataInternal())
    TInt err1 = LoadIVTDataInternal( fileHandle1, CHWRMHaptics::EHWRMHapticsTypePeriodic );
    TInt err2 = LoadIVTDataInternal( fileHandle2, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    TInt err3 = LoadIVTDataInternal( fileHandle3, CHWRMHaptics::EHWRMHapticsTypeTimeline, EFalse );
    EUNIT_ASSERT_DESC( (err1 == KErrNone), "Loading IVT data1 failed!" );
    EUNIT_ASSERT_DESC( (err2 == KErrNone), "Loading IVT data2 failed!" );
    EUNIT_ASSERT_DESC( (err3 == KErrNone), "Loading IVT data3 failed!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle1!" );
    EUNIT_ASSERT_DESC( fileHandle2, "Loading IVT data failed, empty fileHandle2!" );
    EUNIT_ASSERT_DESC( fileHandle3, "Loading IVT data failed, empty fileHandle3!" );

    // check that filehandles are unique
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle3), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle2), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle2 != fileHandle3), "Received same filehandles!" );

    // play the first effect of each data
    TInt err = iHaptics->PlayEffectRepeat( fileHandle1, 0, repeat, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );
    err = iHaptics->PlayEffectRepeat( fileHandle2, 0, repeat, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );
    err = iHaptics->PlayEffectRepeat( fileHandle3, 0, repeat, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );

    // play the first effect of the second data
    err = iHaptics->PlayEffectRepeat( fileHandle2, 0, repeat, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );

    // delete second IVT-data
    err = iHaptics->DeleteEffectData( fileHandle2 );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete IVT data failed!" );

    // play the first effect of the second data
    err = iHaptics->PlayEffectRepeat( fileHandle2, 0, repeat, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrBadHandle), "Playing deleted IVT-data succeeded!" );

    // load second IVT-data once more (filehandle should change)
    TInt newHandle = KErrNotFound;
    err2 = LoadIVTDataInternal( newHandle, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    EUNIT_ASSERT_DESC( (err2 == KErrNone), "Loading IVT data2 failed!" );
    EUNIT_ASSERT_DESC( newHandle, "Loading IVT data failed, empty newHandle!" );
    EUNIT_ASSERT_DESC( (fileHandle2 != newHandle), "Received incorrect filehandle!" );
    fileHandle2 = newHandle;

    // play the first effect of the second data
    err = iHaptics->PlayEffectRepeat( fileHandle2, 0, repeat, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );

    // delete all IVT-data
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed!" );

    // play the first effect of each data (each should fail, since data is deleted)
    err = iHaptics->PlayEffectRepeat( fileHandle1, 0, repeat, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrBadHandle), "Playing deleted IVT-data succeeded!" );
    err = iHaptics->PlayEffectRepeat( fileHandle2, 0, repeat, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrBadHandle), "Playing deleted IVT-data succeeded!" );
    err = iHaptics->PlayEffectRepeat( fileHandle3, 0, repeat, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrBadHandle), "Playing deleted IVT-data succeeded!" );

    // load IVT-data
    err1 = LoadIVTDataInternal( fileHandle1, CHWRMHaptics::EHWRMHapticsTypePeriodic, EFalse );
    err2 = LoadIVTDataInternal( fileHandle2, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    err3 = LoadIVTDataInternal( fileHandle3, CHWRMHaptics::EHWRMHapticsTypeTimeline, EFalse );
    EUNIT_ASSERT_DESC( (err1 == KErrNone), "Loading IVT data1 failed!" );
    EUNIT_ASSERT_DESC( (err2 == KErrNone), "Loading IVT data2 failed!" );
    EUNIT_ASSERT_DESC( (err3 == KErrNone), "Loading IVT data3 failed!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle1!" );
    EUNIT_ASSERT_DESC( fileHandle2, "Loading IVT data failed, empty fileHandle2!" );
    EUNIT_ASSERT_DESC( fileHandle3, "Loading IVT data failed, empty fileHandle3!" );

    // check that filehandles are unique
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle3), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle2), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle2 != fileHandle3), "Received same filehandles!" );

    // play the first effect of each data
    err = iHaptics->PlayEffectRepeat( fileHandle1, 0, repeat, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );
    err = iHaptics->PlayEffectRepeat( fileHandle2, 0, repeat, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );
    err = iHaptics->PlayEffectRepeat( fileHandle3, 0, repeat, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );
    }


// ---------------------------------------------------------
// Loads and deletes IVT-data. Plays using the filehandles
// received by calling the asynchronous PlayRepeat-method.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_LoadDeletePlayRepeatAsync()
    {
    TRequestStatus status1 = KRequestPending;
    TRequestStatus status2 = KRequestPending;
    TRequestStatus status3 = KRequestPending;

    TInt effectHandle = 0;
    TInt repeat = 7;

    TInt fileHandle1 = KErrNotFound;
    TInt fileHandle2 = KErrNotFound;
    TInt fileHandle3 = KErrNotFound;

    // load IVT-data (open actuator on the first load only inside LoadIVTDataInternal())
    TInt err1 = LoadIVTDataInternal( fileHandle1, CHWRMHaptics::EHWRMHapticsTypePeriodic );
    TInt err2 = LoadIVTDataInternal( fileHandle2, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    TInt err3 = LoadIVTDataInternal( fileHandle3, CHWRMHaptics::EHWRMHapticsTypeTimeline, EFalse );
    EUNIT_ASSERT_DESC( (err1 == KErrNone), "Loading IVT data1 failed!" );
    EUNIT_ASSERT_DESC( (err2 == KErrNone), "Loading IVT data2 failed!" );
    EUNIT_ASSERT_DESC( (err3 == KErrNone), "Loading IVT data3 failed!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle1!" );
    EUNIT_ASSERT_DESC( fileHandle2, "Loading IVT data failed, empty fileHandle2!" );
    EUNIT_ASSERT_DESC( fileHandle3, "Loading IVT data failed, empty fileHandle3!" );

    // check that filehandles are unique
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle3), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle2), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle2 != fileHandle3), "Received same filehandles!" );

    // play the first effect of each data
    iSender->PlayRepeat( iHaptics, fileHandle1, 0, repeat, effectHandle, status1 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status1.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );
    iSender->PlayRepeat( iHaptics, fileHandle2, 0, repeat, effectHandle, status2 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status2.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );
    iSender->PlayRepeat( iHaptics, fileHandle3, 0, repeat, effectHandle, status3 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status3.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );

    // play the first effect of the second data
    status2 = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle2, 0, repeat, effectHandle, status2 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status2.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );

    // delete second IVT-data
    TInt err = iHaptics->DeleteEffectData( fileHandle2 );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete IVT data failed!" );

    // play the first effect of the second data
    status2 = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle2, 0, repeat, effectHandle, status2 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status2.Int() == KErrBadHandle), "Playing deleted IVT data succeeded!" );

    // load second IVT-data once more (filehandle should change)
    TInt newHandle = KErrNotFound;
    err2 = LoadIVTDataInternal( newHandle, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    EUNIT_ASSERT_DESC( (err2 == KErrNone), "Loading IVT data2 failed!" );
    EUNIT_ASSERT_DESC( newHandle, "Loading IVT data failed, empty newHandle!" );
    EUNIT_ASSERT_DESC( (fileHandle2 != newHandle), "Received incorrect filehandle!" );
    fileHandle2 = newHandle;

    // play the first effect of the second data
    status2 = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle2, 0, repeat, effectHandle, status2 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status2.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );

    // delete all IVT-data
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed!" );

    // play the first effect of each data (each should fail, since data is deleted)
    status1 = KRequestPending;
    status2 = KRequestPending;
    status3 = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle1, 0, repeat, effectHandle, status1 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status1.Int() == KErrBadHandle), "Playing deleted IVT-data succeeded!" );
    iSender->PlayRepeat( iHaptics, fileHandle2, 0, repeat, effectHandle, status2 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status2.Int() == KErrBadHandle), "Playing deleted IVT-data succeeded!" );
    iSender->PlayRepeat( iHaptics, fileHandle3, 0, repeat, effectHandle, status3 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status3.Int() == KErrBadHandle), "Playing deleted IVT-data succeeded!" );

    // load IVT-data
    err1 = LoadIVTDataInternal( fileHandle1, CHWRMHaptics::EHWRMHapticsTypePeriodic, EFalse );
    err2 = LoadIVTDataInternal( fileHandle2, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    err3 = LoadIVTDataInternal( fileHandle3, CHWRMHaptics::EHWRMHapticsTypeTimeline, EFalse );
    EUNIT_ASSERT_DESC( (err1 == KErrNone), "Loading IVT data1 failed!" );
    EUNIT_ASSERT_DESC( (err2 == KErrNone), "Loading IVT data2 failed!" );
    EUNIT_ASSERT_DESC( (err3 == KErrNone), "Loading IVT data3 failed!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle1!" );
    EUNIT_ASSERT_DESC( fileHandle2, "Loading IVT data failed, empty fileHandle2!" );
    EUNIT_ASSERT_DESC( fileHandle3, "Loading IVT data failed, empty fileHandle3!" );

    // check that filehandles are unique
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle3), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle1 != fileHandle2), "Received same filehandles!" );
    EUNIT_ASSERT_DESC( (fileHandle2 != fileHandle3), "Received same filehandles!" );

    // play the first effect of each data
    status1 = KRequestPending;
    status2 = KRequestPending;
    status3 = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle1, 0, repeat, effectHandle, status1 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status1.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );
    iSender->PlayRepeat( iHaptics, fileHandle2, 0, repeat, effectHandle, status2 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status2.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );
    iSender->PlayRepeat( iHaptics, fileHandle3, 0, repeat, effectHandle, status3 );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status3.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );
    }


// ---------------------------------------------------------
// Playing the previously loaded IVT data (synchronous version)
// ---------------------------------------------------------
//
void MT_HapticsClient::T_PlayIVTEffectWithLoadedDataSynchronous()
    {
    // load the IVT data 
    TInt fileHandle;
    TInt err = LoadIVTDataInternal( fileHandle, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (Playing synchronously case)!" );
    EUNIT_ASSERT_DESC( fileHandle, "Loading IVT data failed, empty fileHandle (Playing synchronously case)!" );
    
    // play the effect.. 
    TInt effectHandle;
    err = iHaptics->PlayEffect( fileHandle, 0, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing loaded IVT data failed, empty effectHandle (synch)!" );

    
    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect synchronously failed! (StopAllPlayingEffects)" );
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed! (Playing synchronously case))" );

    }


// ---------------------------------------------------------
// Playing the previously loaded IVT data (asynchronous version)
// ---------------------------------------------------------
//
void MT_HapticsClient::T_PlayIVTEffectWithLoadedDataAsynchronous()
    {
    // load the IVT data 
    TInt fileHandle;
    TInt err = LoadIVTDataInternal( fileHandle, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (Playing asynchronously case)!" );
    EUNIT_ASSERT_DESC( fileHandle, "Loading IVT data failed, empty fileHandle (Playing asynchronously case)!" );

    // play the effect.. 
    TInt effectHandle;
    TRequestStatus status = KRequestPending;
    iSender->Play( iHaptics, fileHandle, 0, effectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing loaded IVT data failed, empty effectHandle (asynch)!" );
    
    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect asynchronously failed! (StopAllPlayingEffects)" );
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed! (Playing asynchronously case))" );

    }


// ---------------------------------------------------------
// Playing with repeat the previously loaded IVT data 
// (synchronous version). Uses timeline effect.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_PlayIVTEffectRepeatWithLoadedDataSynchronous()
    {
    // load the IVT data 
    TInt fileHandle;
    TInt err = LoadIVTDataInternal( fileHandle, CHWRMHaptics::EHWRMHapticsTypeTimeline );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (Repeat IVT synchronously case)!" );
    EUNIT_ASSERT_DESC( fileHandle, "Loading IVT data failed, empty fileHandle (Repeat IVT synchronously case)!" );
    
    // play the effect repeatedly (repeated 3 times) 
    TInt effectHandle;
    err = iHaptics->PlayEffectRepeat( fileHandle, 0, 3, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing repeatedly loaded IVT data failed (synch)!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing repeatedly loaded IVT data failed, empty effectHandle (synch)!" );

    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect repeatedly synchronously failed! (StopAllPlayingEffects)" );
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed! (Repeat IVT synchronously case))" );

    }


// ---------------------------------------------------------
// Playing with repeat the previously loaded IVT data 
// (asynchronous version). Uses timeline effect.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_PlayIVTEffectRepeatWithLoadedDataAsynchronous()
    {
    // load the IVT data 
    TInt fileHandle;
    TInt err = LoadIVTDataInternal( fileHandle, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (Repeat IVT asynchronously case)!" );
    EUNIT_ASSERT_DESC( fileHandle, "Loading IVT data failed, empty fileHandle (Repeat IVT asynchronously case)!" );

    // play the effect (repeated 5 times) 
    TInt effectHandle;
    TRequestStatus status = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle, 0, 5, effectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Playing repeatedly loaded IVT data failed (asynch)!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing repeatedly loaded IVT data failed, empty effectHandle (asynch)!" );
    
    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect repeatedly asynchronously failed! (StopAllPlayingEffects)" );
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed! (Repeat IVT asynchronously case))" );

    }

// ---------------------------------------------------------
// VibeStatus to S60 status conversions
// ---------------------------------------------------------
//
void MT_HapticsClient::T_VibeStatusToS60StatusConversions()
    {
    // this is a bit ugly, i.e., we're using special repeat 
    // values and then the test adaptation stub acknowledges
    // these special value and repeats accordingly with some
    // (Vibe-error) status
    
    // load the IVT data 
    TInt fileHandle;
    TInt err = LoadIVTDataInternal( fileHandle, CHWRMHaptics::EHWRMHapticsTypeTimeline );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (Status conversion case)!" );
    EUNIT_ASSERT_DESC( fileHandle, "Loading IVT data failed, empty fileHandle (Status conversion case)!" );
    
    // the actual status conversion cases (done with special repeat values
    // using PlayIVTEffectRepeat)
    TInt dummyEffectHandle; // we're not interested about this in these cases
    // first the synch cases..
    err = iHaptics->PlayEffectRepeat( fileHandle, 0, SUCCESS_TEST, dummyEffectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Status conversion subcase SUCCESS_TEST failed (synch)!" );

    err = iHaptics->PlayEffectRepeat( fileHandle, 0, ALREADY_INITIALIZED_TEST, dummyEffectHandle );
    EUNIT_ASSERT_DESC( (err == KErrAlreadyExists), "Status conversion subcase ALREADY_INITIALIZED_TEST failed (synch)!" );

    err = iHaptics->PlayEffectRepeat( fileHandle, 0, NOT_INITIALIZED_TEST, dummyEffectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNotReady), "Status conversion subcase NOT_INITIALIZED_TEST failed (synch)!" );

    err = iHaptics->PlayEffectRepeat( fileHandle, 0, INVALID_ARGUMENT_TEST, dummyEffectHandle );
    EUNIT_ASSERT_DESC( (err == KErrArgument), "Status conversion subcase INVALID_ARGUMENT_TEST failed (synch)!" );

    err = iHaptics->PlayEffectRepeat( fileHandle, 0, FAIL_TEST, dummyEffectHandle );
    EUNIT_ASSERT_DESC( (err == KErrGeneral), "Status conversion subcase FAIL_TEST failed (synch)!" );

    err = iHaptics->PlayEffectRepeat( fileHandle, 0, INCOMPATIBLE_EFFECT_TYPE_TEST, dummyEffectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNotSupported), "Status conversion subcase INCOMPATIBLE_EFFECT_TYPE_TEST failed (synch)!" );

    err = iHaptics->PlayEffectRepeat( fileHandle, 0, INCOMPATIBLE_CAPABILITY_TYPE_TEST, dummyEffectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNotSupported), "Status conversion subcase INCOMPATIBLE_CAPABILITY_TYPE_TEST failed (synch)!" );

    err = iHaptics->PlayEffectRepeat( fileHandle, 0, INCOMPATIBLE_PROPERTY_TYPE_TEST, dummyEffectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNotSupported), "Status conversion subcase INCOMPATIBLE_PROPERTY_TYPE_TEST failed (synch)!" );

    err = iHaptics->PlayEffectRepeat( fileHandle, 0, DEVICE_NEEDS_LICENSE_TEST, dummyEffectHandle );
    EUNIT_ASSERT_DESC( (err == KErrAccessDenied), "Status conversion subcase DEVICE_NEEDS_LICENSE_TEST failed (synch)!" );

    err = iHaptics->PlayEffectRepeat( fileHandle, 0, NOT_ENOUGH_MEMORY_TEST, dummyEffectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNoMemory), "Status conversion subcase NOT_ENOUGH_MEMORY_TEST failed (synch)!" );

    err = iHaptics->PlayEffectRepeat( fileHandle, 0, SERVICE_NOT_RUNNING_TEST, dummyEffectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNotReady), "Status conversion subcase SERVICE_NOT_RUNNING_TEST failed (synch)!" );

    err = iHaptics->PlayEffectRepeat( fileHandle, 0, INSUFFICIENT_PRIORITY_TEST, dummyEffectHandle );
    EUNIT_ASSERT_DESC( (err == KErrAccessDenied), "Status conversion subcase INSUFFICIENT_PRIORITY_TEST failed (synch)!" );

    err = iHaptics->PlayEffectRepeat( fileHandle, 0, SERVICE_BUSY_TEST, dummyEffectHandle );
    EUNIT_ASSERT_DESC( (err == KErrInUse), "Status conversion subcase SERVICE_BUSY_TEST failed (synch)!" );

    err = iHaptics->PlayEffectRepeat( fileHandle, 0, WRN_NOT_PLAYING_TEST, dummyEffectHandle );
    EUNIT_ASSERT_DESC( (err == KErrArgument), "Status conversion subcase WRN_NOT_PLAYING_TEST failed (synch)!" );

    err = iHaptics->PlayEffectRepeat( fileHandle, 0, WRN_INSUFFICIENT_PRIORITY_TEST, dummyEffectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Status conversion subcase WRN_INSUFFICIENT_PRIORITY_TEST failed (synch)!" );

    // then the asycnh cases
    TRequestStatus status;
    
    status = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle, 0, SUCCESS_TEST, dummyEffectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Status conversion subcase SUCCESS_TEST failed (asynch)!" );
 
    status = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle, 0, ALREADY_INITIALIZED_TEST, dummyEffectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrAlreadyExists), "Status conversion subcase ALREADY_INITIALIZED_TEST failed (asynch)!" );
    
    status = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle, 0, NOT_INITIALIZED_TEST, dummyEffectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrNotReady), "Status conversion subcase NOT_INITIALIZED_TEST failed (asynch)!" );
        
    status = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle, 0, INVALID_ARGUMENT_TEST, dummyEffectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrArgument), "Status conversion subcase INVALID_ARGUMENT_TEST failed (asynch)!" );

    status = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle, 0, FAIL_TEST, dummyEffectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrGeneral), "Status conversion subcase FAIL_TEST failed (asynch)!" );

    status = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle, 0, INCOMPATIBLE_EFFECT_TYPE_TEST, dummyEffectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrNotSupported), "Status conversion subcase INCOMPATIBLE_EFFECT_TYPE_TEST failed (asynch)!" );
 
    status = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle, 0, INCOMPATIBLE_CAPABILITY_TYPE_TEST, dummyEffectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrNotSupported), "Status conversion subcase INCOMPATIBLE_CAPABILITY_TYPE_TEST failed (asynch)!" );
        
    status = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle, 0, INCOMPATIBLE_PROPERTY_TYPE_TEST, dummyEffectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrNotSupported), "Status conversion subcase INCOMPATIBLE_PROPERTY_TYPE_TEST failed (asynch)!" );

    status = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle, 0, DEVICE_NEEDS_LICENSE_TEST, dummyEffectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrAccessDenied), "Status conversion subcase DEVICE_NEEDS_LICENSE_TEST failed (asynch)!" );

    status = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle, 0, NOT_ENOUGH_MEMORY_TEST, dummyEffectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrNoMemory), "Status conversion subcase NOT_ENOUGH_MEMORY_TEST failed (asynch)!" );
 
    status = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle, 0, SERVICE_NOT_RUNNING_TEST, dummyEffectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrNotReady), "Status conversion subcase SERVICE_NOT_RUNNING_TEST failed (asynch)!" );
        
    status = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle, 0, INSUFFICIENT_PRIORITY_TEST, dummyEffectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrAccessDenied), "Status conversion subcase INSUFFICIENT_PRIORITY_TEST failed (asynch)!" );

    status = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle, 0, SERVICE_BUSY_TEST, dummyEffectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrInUse), "Status conversion subcase SERVICE_BUSY_TEST failed (asynch)!" );

    status = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle, 0, WRN_NOT_PLAYING_TEST, dummyEffectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrArgument), "Status conversion subcase WRN_NOT_PLAYING_TEST failed (asynch)!" );

    status = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle, 0, WRN_INSUFFICIENT_PRIORITY_TEST, dummyEffectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Status conversion subcase WRN_INSUFFICIENT_PRIORITY_TEST failed (asynch)!" ); 
    }

// ---------------------------------------------------------
// Load multiple IVT-data
// ---------------------------------------------------------
//
void MT_HapticsClient::T_LoadMultipleIVTData()
    {
    TInt fileHandle;
    TInt prevFileHandle;

    // load the IVT data 
    TInt err = LoadIVTDataInternal( fileHandle, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (Load multiple IVT-data case)!" );
    EUNIT_ASSERT_DESC( fileHandle, "Loading IVT data failed, empty fileHandle (Load multiple IVT-data case)!" );
    prevFileHandle = fileHandle;

    // load the IVT data 
    fileHandle = 0;
    err = LoadIVTDataInternal( fileHandle, CHWRMHaptics::EHWRMHapticsTypeTimeline ,EFalse );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (Load multiple IVT-data case)!" );
    EUNIT_ASSERT_DESC( fileHandle, "Loading IVT data failed, empty fileHandle (Load multiple IVT-data case)!" );
    EUNIT_ASSERT_DESC( fileHandle!=prevFileHandle, "Loading IVT data failed, empty fileHandle (Playing synchronously case)!" );
    
    // try to load the same IVT data as before
    fileHandle = 0;
    err = LoadIVTDataInternal( fileHandle, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (Load multiple IVT-data case)!" );
    EUNIT_ASSERT_DESC( fileHandle == prevFileHandle, "Loading IVT data failed, wrong fileHandle (Load multiple IVT-data case)!" );

    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed! (Playing synchronously case))" );
    }

// ---------------------------------------------------------
// Playing from multiple IVT data (synchronous version)
// ---------------------------------------------------------
//
void MT_HapticsClient::T_PlayIVTEffectsFromMultipleIVTDataSynchronous()
    {
    // load the IVT data 
    TInt fileHandle1;
    TInt err = LoadIVTDataInternal( fileHandle1, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (Playing synchronously case)!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle (Playing synchronously case)!" );

    // load the IVT data 
    TInt fileHandle2;
    err = LoadIVTDataInternal( fileHandle2, CHWRMHaptics::EHWRMHapticsTypeTimeline, EFalse );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (Playing synchronously case)!" );
    EUNIT_ASSERT_DESC( fileHandle2, "Loading IVT data failed, empty fileHandle (Playing synchronously case)!" );
    
    // play the effect..
    TInt effectHandle;
    err = iHaptics->PlayEffect( fileHandle1, 0, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing loaded IVT data failed, empty effectHandle (synch)!" );

    // play the effect.. 
    effectHandle = 0;
    err = iHaptics->PlayEffect( fileHandle2, 0, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing loaded IVT data failed, empty effectHandle (synch)!" );

    // play the effect.. 
    effectHandle = 0;
    err = iHaptics->PlayEffect( fileHandle2, 0, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing loaded IVT data failed (synch)!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing loaded IVT data failed, empty effectHandle (synch)!" );

    // play the effect.. 
    effectHandle = 0;
    TInt unknownFileHandle = 3;
    err = iHaptics->PlayEffect( unknownFileHandle, 0, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrBadHandle), "Playing loaded IVT data failed, not bad handle (synch)!" );
    EUNIT_ASSERT_DESC( !effectHandle, "Playing loaded IVT data failed, not empty effectHandle (synch)!" );

    
    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect synchronously failed! (StopAllPlayingEffects)" );
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed! (Playing synchronously case))" );

    }

// ---------------------------------------------------------
// Playing the previously loaded IVT data (asynchronous version)
// ---------------------------------------------------------
//
void MT_HapticsClient::T_PlayIVTEffectsFromMultipleIVTDataAsynchronous()
    {
    // load the IVT data 
    TInt fileHandle1;
    TInt err = LoadIVTDataInternal( fileHandle1, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (Playing asynchronously case)!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle (Playing asynchronously case)!" );

    // load the IVT data 
    TInt fileHandle2;
    err = LoadIVTDataInternal( fileHandle2, CHWRMHaptics::EHWRMHapticsTypeTimeline, EFalse );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (Playing asynchronously case)!" );
    EUNIT_ASSERT_DESC( fileHandle2, "Loading IVT data failed, empty fileHandle (Playing asynchronously case)!" );

    // play the effect..
    TInt effectHandle;
    TRequestStatus status = KRequestPending;

    iSender->Play( iHaptics, fileHandle1, 0, effectHandle, status );
    iWait.Start(); 
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing loaded IVT data failed, empty effectHandle (asynch)!" );
    
    // play the effect..
    effectHandle = 0;
    status = KRequestPending;
    iSender->Play( iHaptics, fileHandle2, 0, effectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Playing loaded IVT data failed (asynch)!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing loaded IVT data failed, empty effectHandle (asynch)!" );
    
    // play the effect..
    effectHandle = 0;
    status = KRequestPending;
    TInt unknownFileHandle = 3;
    iSender->Play( iHaptics, unknownFileHandle, 0, effectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrBadHandle), "Playing loaded IVT data failed, not bad handle (asynch)!" );
    EUNIT_ASSERT_DESC( !effectHandle, "Playing loaded IVT data failed, not empty effectHandle (asynch)!" );
    
    
    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect asynchronously failed! (StopAllPlayingEffects)" );
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed! (Playing asynchronously case))" );

    }

// ---------------------------------------------------------
// Playing with repeat the previously loaded IVT data 
// (synchronous version). Uses timeline effect.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_PlayIVTEffectRepeatFromMultipleIVTDataSynchronous()
    {
    // load the IVT data 
    TInt fileHandle1;
    TInt err = LoadIVTDataInternal( fileHandle1, CHWRMHaptics::EHWRMHapticsTypeTimeline );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (Repeat IVT synchronously case)!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle (Repeat IVT synchronously case)!" );

    // load the IVT data 
    TInt fileHandle2;
    err = LoadIVTDataInternal( fileHandle2, CHWRMHaptics::EHWRMHapticsTypeTimeline, EFalse );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (Repeat IVT synchronously case)!" );
    EUNIT_ASSERT_DESC( fileHandle2, "Loading IVT data failed, empty fileHandle (Repeat IVT synchronously case)!" );
    
    // play the effect repeatedly (repeated 3 times) 
    TInt effectHandle;
    err = iHaptics->PlayEffectRepeat( fileHandle1, 0, 3, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing repeatedly loaded IVT data failed (synch)!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing repeatedly loaded IVT data failed, empty effectHandle (synch)!" );

    // play the effect repeatedly (repeated 3 times) 
    effectHandle = 0;
    err = iHaptics->PlayEffectRepeat( fileHandle2, 0, 3, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing repeatedly loaded IVT data failed (synch)!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing repeatedly loaded IVT data failed, empty effectHandle (synch)!" );

    // play the effect repeatedly (repeated 3 times) 
    effectHandle = 0;
    TInt unknownFileHandle = 3;
    err = iHaptics->PlayEffectRepeat( unknownFileHandle, 0, 3, effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrBadHandle), "Playing repeatedly loaded IVT data failed, not bad handle (synch)!" );
    EUNIT_ASSERT_DESC( !effectHandle, "Playing repeatedly loaded IVT data failed, not empty effectHandle (synch)!" );

    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect repeatedly synchronously failed! (StopAllPlayingEffects)" );
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed! (Repeat IVT synchronously case))" );

    }


// ---------------------------------------------------------
// Playing with repeat the previously loaded IVT data 
// (asynchronous version). Uses timeline effect.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_PlayIVTEffectRepeatFromMultipleIVTDataAsynchronous()
    {
    // load the IVT data 
    TInt fileHandle1;
    TInt err = LoadIVTDataInternal( fileHandle1, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (Repeat IVT asynchronously case)!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle (Repeat IVT asynchronously case)!" );
    
    // load the IVT data 
    TInt fileHandle2;
    err = LoadIVTDataInternal( fileHandle2, CHWRMHaptics::EHWRMHapticsTypeMagSweep, EFalse );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (Repeat IVT asynchronously case)!" );
    EUNIT_ASSERT_DESC( fileHandle2, "Loading IVT data failed, empty fileHandle (Repeat IVT asynchronously case)!" );
    
    // play the effect (repeated 5 times) 
    TInt effectHandle;
    TRequestStatus status = KRequestPending;

    iSender->PlayRepeat( iHaptics, fileHandle1, 0, 5, effectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Playing repeatedly loaded IVT data failed (asynch)!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing repeatedly loaded IVT data failed, empty effectHandle (asynch)!" );

    // play the effect (repeated 5 times) 
    effectHandle = 0;
    iSender->PlayRepeat( iHaptics, fileHandle2, 0, 5, effectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Playing repeatedly loaded IVT data failed (asynch)!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing repeatedly loaded IVT data failed, empty effectHandle (asynch)!" );

    // play the effect (repeated 5 times) 
    effectHandle = 0;
    status = KRequestPending;
    TInt unknownFileHandle = 3;
    iSender->PlayRepeat( iHaptics, unknownFileHandle, 0, 5, effectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrBadHandle), "Playing repeatedly loaded IVT data failed, not bad handle (asynch)!" );
    EUNIT_ASSERT_DESC( !effectHandle, "Playing repeatedly loaded IVT data failed, not empty effectHandle (asynch)!" );

    
    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect repeatedly asynchronously failed! (StopAllPlayingEffects)" );
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed! (Repeat IVT asynchronously case))" );
    }

// ---------------------------------------------------------
// Releasing previously reserved haptics
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_ReserveAndReleaseHaptics()
    {
    // reserve
    TRAPD( err, iHaptics->ReserveHapticsL() );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Release haptics case failed (ReserveHapticsL leaves)!" );

    // release (does not return an error code)
    iHaptics->ReleaseHaptics();
    }

// ---------------------------------------------------------
// Reserving haptics several (in this case three) times
// ---------------------------------------------------------
//
void MT_HapticsClient::T_ReserveHapticsSeveralTimes()
    {
    TRAPD( err, iHaptics->ReserveHapticsL() );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Reserve haptics (1st call) failed!" );
    TRAP( err, iHaptics->ReserveHapticsL() );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Reserve haptics (2nd call) failed!" );
    TRAP( err, iHaptics->ReserveHapticsL() );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Reserve haptics (3rd call) failed!" );
    }

// ---------------------------------------------------------
// Reserving haptics
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_ReserveHapticsNoReleaseL()
    {
    // create first haptics instance
    CHWRMHaptics* haptics1 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics1 );

    // reserve
    TRAPD( err, haptics1->ReserveHapticsL() );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Reserve haptics failed!" );

    // create second haptics instance and open actuator
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( NULL, NULL );
    TRAP( err, haptics2->OpenActuatorL( EHWRMLogicalActuatorAny ) );

    // play an effect using second client (should fail because 
    // of reservation)
    RBuf8 effBuf;
    CreateIVTEffect( effBuf, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    TInt effectHandle = 0;
    err = haptics2->PlayEffect( effBuf, 0, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrInUse), "Playing IVT effect succeeded although server reserved!" );

    // destroy first client --> should release reservation
    CleanupStack::PopAndDestroy( haptics1 );

    // play an effect using second client (should succeed, since no
    // reservation)
    err = haptics2->PlayEffect( effBuf, 0, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect failed (no reservation)!" );

    // cleanup
    effBuf.Close();
    delete haptics2;
    }

// ---------------------------------------------------------
// Releasing haptics when it has not been previously reserved
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_ReleaseHapticsWithoutReservingFirst()
    {
    // no return values nor leave to check, just execute
    iHaptics->ReleaseHaptics();
    }

// ---------------------------------------------------------
// Tries to reserve haptics with forceNoCoeEnv flag on.
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_ReserveNoAutomaticFocusNotTrusted()
    {
    TRAPD( err, iHaptics->ReserveHapticsL( ETrue ) );
    EUNIT_ASSERT_DESC( (err == KErrAccessDenied), "Reservation succeeded although not a trusted client!" );
    }

// ---------------------------------------------------------
// Tries to reserve haptics, when it has already been 
// reserved for another client with higher priority.
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_ReserveAlreadyReservedHigherL()
    {
    // create haptics instances and open actuators
    CHWRMHaptics* haptics1 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics1 );
    TRAPD( err, haptics1->OpenActuatorL( EHWRMLogicalActuatorAny ) );
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics2 );
    TRAP( err, haptics2->OpenActuatorL( EHWRMLogicalActuatorAny ) );

    // set first client priority (max)
    haptics1->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsPriority, 
                                 KHWRMHapticsMaxDevicePriority );

    // reserve
    TRAP( err, haptics1->ReserveHapticsL() );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Reserve haptics failed!" );

    // set second client priority (min)
    haptics2->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsPriority, 
                                 KHWRMHapticsMinDevicePriority );

    // reserve
    TRAP( err, haptics2->ReserveHapticsL() );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Reserve haptics failed!" );

    // play effect using the second client
    RBuf8 effBuf;
    CreateIVTEffect( effBuf, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    TInt effectHandle = 0;
    err = haptics2->PlayEffect( effBuf, 0, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect failed!" );

    // release first client's reservation
    haptics1->ReleaseHaptics();

    // play effect using the second client
    err = haptics2->PlayEffect( effBuf, 0, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect failed!" );
    
    // release second client's reservation
    haptics2->ReleaseHaptics();

    // cleanup
    effBuf.Close();
    CleanupStack::PopAndDestroy( haptics2 );
    CleanupStack::PopAndDestroy( haptics1 );
    }

// ---------------------------------------------------------
// Tries to reserve haptics, when it has already been 
// reserved for another client with lower priority.
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_ReserveAlreadyReservedLowerL()
    {
    // create haptics instances and open actuators
    CHWRMHaptics* haptics1 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics1 );
    TRAPD( err, haptics1->OpenActuatorL( EHWRMLogicalActuatorAny ) );
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics2 );
    TRAP( err, haptics2->OpenActuatorL( EHWRMLogicalActuatorAny ) );

    // set first client priority (min)
    haptics1->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsPriority, 
                                 KHWRMHapticsMinDevicePriority );

    // reserve
    TRAP( err, haptics1->ReserveHapticsL() );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Reserve haptics failed!" );

    // set second client priority (max)
    haptics2->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsPriority, 
                                 KHWRMHapticsMaxDevicePriority );

    // reserve
    TRAP( err, haptics2->ReserveHapticsL() );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Reserve haptics failed!" );
    
    // play effect using first client
    RBuf8 effBuf;
    CreateIVTEffect( effBuf, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    TInt effectHandle = 0;
    err = haptics1->PlayEffect( effBuf, 0, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect failed!" );

    // play effect using second client
    err = haptics2->PlayEffect( effBuf, 0, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect failed!" );

    // release second client
    haptics2->ReleaseHaptics();

    // play effect using first client
    err = haptics1->PlayEffect( effBuf, 0, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect failed!" );

    // play effect using second client
    err = haptics2->PlayEffect( effBuf, 0, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect failed!" );

    // release first client
    haptics1->ReleaseHaptics();

    // cleanup
    effBuf.Close();
    CleanupStack::PopAndDestroy( haptics2 );
    CleanupStack::PopAndDestroy( haptics1 );
    }

// ---------------------------------------------------------
// Tries to play effects, when haptics has been reserved
// for a higher priority client.
// ---------------------------------------------------------
// 
void MT_HapticsClient::T_PlayWhenReservedL()
    {
    // create haptics instances and open actuators
    CHWRMHaptics* haptics1 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics1 );
    TRAPD( err, haptics1->OpenActuatorL( EHWRMLogicalActuatorAny ) );
    TRAP( err, iHaptics->OpenActuatorL( EHWRMLogicalActuatorAny ) );

    // set first client priority (max)
    haptics1->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsPriority, 
                                 KHWRMHapticsMaxDevicePriority );

    // reserve for first client
    TRAP( err, haptics1->ReserveHapticsL() );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Reserve haptics failed!" );

    // set second client priority (min)
    iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsPriority, 
                                 KHWRMHapticsMinDevicePriority );

    // play effects using second client, when reserved for the first
    // ---> 

    // create effect data for playing effect directly using IVT-data
    RBuf8 effBuf;
    CreateIVTEffect( effBuf, CHWRMHaptics::EHWRMHapticsTypeMagSweep );

    // load the IVT data for playing effects using filehandle
    TInt fileHandle = 0;
    err = LoadIVTDataInternal( fileHandle, CHWRMHaptics::EHWRMHapticsTypePeriodic, EFalse );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed!" );

    // create a magsweep effect
    CHWRMHaptics::THWRMHapticsMagSweepEffect magSweepEffect;
    CreateMagSweepEffect( magSweepEffect );

    // create a periodic effect
    CHWRMHaptics::THWRMHapticsPeriodicEffect periodicEffect;
    CreatePeriodicEffect( periodicEffect );

    TInt effectHandle = 0;
    TRequestStatus status = KRequestPending;

    // synchronous play with data
    err = iHaptics->PlayEffect( effBuf, 0, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrInUse), "Playing IVT effect succeeded although reserved!" );
    
    // synchronous play-repeat with data
    err = iHaptics->PlayEffectRepeat( effBuf, 0, 2, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrInUse), "Playing IVT effect succeeded although reserved!" );
    
    // asynchronous play with data
    status = KRequestPending;
    iHaptics->PlayEffect( effBuf, 0, effectHandle, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrInUse), "Playing IVT effect succeeded although reserved!" );

    // asynchronous play-repeat with data
    status = KRequestPending;
    iHaptics->PlayEffectRepeat( effBuf, 0, 2, effectHandle, status ); 
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrInUse), "Playing IVT effect succeeded although reserved!" );

    // synchronous play with filehandle
    err = iHaptics->PlayEffect( fileHandle, 0, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrInUse), "Playing IVT effect succeeded although reserved!" );
    
    // synchronous play-repeat with filehandle
    err = iHaptics->PlayEffectRepeat( fileHandle, 0, 2, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrInUse), "Playing IVT effect succeeded although reserved!" );

    // asynchronous play with filehandle
    status = KRequestPending;
    iSender->Play( iHaptics, fileHandle, 0, effectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrInUse), "Playing IVT effect succeeded although reserved!" );

    // asynchronous play-repeat with filehandle
    status = KRequestPending;
    iSender->PlayRepeat( iHaptics, fileHandle, 0, 2, effectHandle, status );
    iWait.Start();
    EUNIT_ASSERT_DESC( (status.Int() == KErrInUse), "Playing IVT effect succeeded although reserved!" );

    // synchronous manual magsweep
    err = iHaptics->PlayMagSweepEffect( magSweepEffect, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrInUse), "Playing IVT effect succeeded although reserved!" );

    // synchronous manual periodic
    err = iHaptics->PlayPeriodicEffect( periodicEffect, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrInUse), "Playing IVT effect succeeded although reserved!" );

    // asynchronous manual magsweep
    status = KRequestPending;
    iHaptics->PlayMagSweepEffect( magSweepEffect, effectHandle, status ); 
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrInUse), "Playing IVT effect succeeded although reserved!" );

    // asynchronous manual periodic
    status = KRequestPending;
    iHaptics->PlayPeriodicEffect( periodicEffect, effectHandle, status ); 
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrInUse), "Playing IVT effect succeeded although reserved!" );

    // <--- 

    // cleanup
    effBuf.Close();
    CleanupStack::PopAndDestroy( haptics1 );
    }

// ---------------------------------------------------------
// Getting effect count from preloaded IVT-data
// ---------------------------------------------------------
// 
void MT_HapticsClient::T_GetIVTEffectCount()
    {
    TInt fileHandle1;
    TInt fileHandle2;
    TInt corruptedFile;
    TInt unknownFileHandle = 9;
    
    // load the IVT effect set 1
    TInt err = LoadIVTDataInternal( fileHandle1, EffectSetMT_ivt,
                                    sizeof( EffectSetMT_ivt ) );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (GetIVTEffectCount case)!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle (GetIVTEffectCount case)!" );

    // load corrupted IVT effect set
    err = LoadIVTDataInternal( corruptedFile, CorruptedMT_ivt,
                                    sizeof( CorruptedMT_ivt ), EFalse );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (GetIVTEffectCount case)!" );
    EUNIT_ASSERT_DESC( corruptedFile, "Loading IVT data failed, empty fileHandle (GetIVTEffectCount case)!" );

    // load the IVT effect set 2
    err = LoadIVTDataInternal( fileHandle2, Magsweep4HapticsMT_ivt,
                                    sizeof( Magsweep4HapticsMT_ivt ), EFalse );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (GetIVTEffectCount case)!" );
    EUNIT_ASSERT_DESC( fileHandle2, "Loading IVT data failed, empty fileHandle (GetIVTEffectCount case)!" );


    TInt effectCount;
    // get effect count from IVT-data 1
    err = iHaptics->GetEffectCount( fileHandle1, effectCount );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Get effect count failed (GetIVTEffectCount case)!" );
    EUNIT_ASSERT_DESC( (effectCount == 6), "Wrong effect count (GetIVTEffectCount case)!" );

    // get effect count from corrupted IVT-data
    err = iHaptics->GetEffectCount( corruptedFile, effectCount );
    EUNIT_ASSERT_DESC( (err == KErrArgument), "Get count from corrupted IVT-data, incorrect error code (GetIVTEffectCount case)!" );

    // get effect count from IVT-data 2
    err = iHaptics->GetEffectCount( fileHandle2, effectCount );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Get effect count failed (GetIVTEffectCount case)!" );
    EUNIT_ASSERT_DESC( (effectCount == 1), "Wrong effect count (GetIVTEffectCount case)!" );

    // get effect count using unknown file handle
    err = iHaptics->GetEffectCount( unknownFileHandle, effectCount );
    EUNIT_ASSERT_DESC( (err == KErrBadHandle), "Incorrect error code (GetIVTEffectCount case)!" );

    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed! (GetIVTEffectCount case))" );
    }

// ---------------------------------------------------------
// Getting effect duration from preloaded IVT-data
// ---------------------------------------------------------
// 
void MT_HapticsClient::T_GetIVTEffectDuration()
    {
    TInt fileHandle1;
    TInt corruptedFile;
    TInt unknownFileHandle = 9;
    
    // load the IVT effect set 1
    TInt err = LoadIVTDataInternal( fileHandle1, EffectSetMT_ivt,
                                    sizeof( EffectSetMT_ivt ) );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (GetIVTEffectDuration case)!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle (GetIVTEffectDuration case)!" );

    // load corrupted IVT effect set
    err = LoadIVTDataInternal( corruptedFile, CorruptedMT_ivt,
                                    sizeof( CorruptedMT_ivt ), EFalse );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (GetIVTEffectDuration case)!" );
    EUNIT_ASSERT_DESC( corruptedFile, "Loading IVT data failed, empty fileHandle (GetIVTEffectDuration case)!" );

    TInt effectDuration;
    // duration of the last effect
    TInt index = 5;
    err = iHaptics->GetEffectDuration( fileHandle1, index, effectDuration );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Get duration failed (GetIVTEffectDuration case)!" );
    EUNIT_ASSERT_DESC( (effectDuration == 4000), "Wrong duration (GetIVTEffectDuration case)!" );

    // get duration using effect index out of bounds
    index = 6;
    err = iHaptics->GetEffectDuration( fileHandle1, index, effectDuration );
    EUNIT_ASSERT_DESC( (err != KErrNone), "Get duration succeeded (GetIVTEffectDuration case)!" );

    // get duration from corrupted IVT-data
    index = 0; err = KErrNone;
    err = iHaptics->GetEffectDuration( corruptedFile, index, effectDuration );
    EUNIT_ASSERT_DESC( (err == KErrArgument), "Get duration from corrupted data, incorrect error code (GetIVTEffectDuration case)!" );

    // duration of the first effect
    index = 0;
    err = iHaptics->GetEffectDuration( fileHandle1, index, effectDuration );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Get duration failed (GetIVTEffectDuration case)!" );
    EUNIT_ASSERT_DESC( (effectDuration == 3915), "Wrong duration (GetIVTEffectDuration case)!" );

    // get duration using unknown file handle
    index = 0;
    err = iHaptics->GetEffectDuration( unknownFileHandle, index, effectDuration );
    EUNIT_ASSERT_DESC( (err == KErrBadHandle), "Incorrect error code (GetIVTEffectDuration case)!" );

    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed! (GetIVTEffectDuration case))" );
    }

// ---------------------------------------------------------
// Getting effect index from preloaded IVT-data
// ---------------------------------------------------------
// 
void MT_HapticsClient::T_GetIVTEffectIndexFromName()
    {
    TInt fileHandle1;
    TInt corruptedFile;
    TInt unknownFileHandle = 9;
    
    // load the IVT effect set 1
    TInt err = LoadIVTDataInternal( fileHandle1, EffectSetMT_ivt,
                                    sizeof( EffectSetMT_ivt ) );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (GetEffectIndexFromName case)!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle (GetEffectIndexFromName case)!" );

    // load corrupted IVT effect set
    err = LoadIVTDataInternal( corruptedFile, CorruptedMT_ivt,
                                    sizeof( CorruptedMT_ivt ), EFalse );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (GetEffectIndexFromName case)!" );
    EUNIT_ASSERT_DESC( corruptedFile, "Loading IVT data failed, empty fileHandle (GetEffectIndexFromName case)!" );


    TInt effectIndex;
    _LIT8( KFirstName, "Timeline1" );
    _LIT8( KLastName, "MagSweep2" );
    _LIT8( KUnknownName, "Tiline1" );
    TBuf8<128> effectName;

    // get index of the first name
    effectName = KFirstName;
    err = iHaptics->GetEffectIndexFromName( fileHandle1, effectName, effectIndex );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Get effect index failed (GetEffectIndexFromName case)!" );
    EUNIT_ASSERT_DESC( (effectIndex == 0), "Wrong effect index (GetEffectIndexFromName case)!" );
    
    // get index of the last name
    effectName = KLastName;
    err = iHaptics->GetEffectIndexFromName( fileHandle1, effectName, effectIndex );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Get effect index failed (GetEffectIndexFromName case)!" );
    EUNIT_ASSERT_DESC( (effectIndex == 5), "Wrong effect index (GetEffectIndexFromName case)!" );

    // get index from corrupted IVT-data
    effectName = KFirstName;
    err = iHaptics->GetEffectIndexFromName( corruptedFile, effectName, effectIndex );
    EUNIT_ASSERT_DESC( (err == KErrArgument), "Get effect index from corrupted data, incorrect error code (GetEffectIndexFromName case)!" );

    // get index using unknown name
    effectName = KUnknownName;
    err = iHaptics->GetEffectIndexFromName( fileHandle1, effectName, effectIndex );
    EUNIT_ASSERT_DESC( (err != KErrNone), "Get effect succeeded (GetEffectIndexFromName case)!" );

    // get index using unknown file handle
    effectName = KUnknownName;
    err = iHaptics->GetEffectIndexFromName( unknownFileHandle, effectName, effectIndex );
    EUNIT_ASSERT_DESC( (err == KErrBadHandle), "Incorrect error code (GetEffectIndexFromName case)!" );


    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed! (GetEffectIndexFromName case))" );
    }


// ---------------------------------------------------------
// Getting effect type from preloaded IVT-data
// ---------------------------------------------------------
// 
void MT_HapticsClient::T_GetIVTEffectType()
    {
    TInt fileHandle1;
    TInt corruptedFile;
    TInt unknownFileHandle = 9;
    
    // load the IVT effect set 1
    TInt err = LoadIVTDataInternal( fileHandle1, EffectSetMT_ivt,
                                    sizeof( EffectSetMT_ivt ) );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (GetIVTEffectType case)!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle (GetIVTEffectType case)!" );

    // load corrupted IVT effect set
    err = LoadIVTDataInternal( corruptedFile, CorruptedMT_ivt,
                                    sizeof( CorruptedMT_ivt ), EFalse );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (GetIVTEffectType case)!" );
    EUNIT_ASSERT_DESC( corruptedFile, "Loading IVT data failed, empty fileHandle (GetIVTEffectType case)!" );


    // get type of the first effect
    TInt effectType;
    TInt effectIndex = 0;
    err = iHaptics->GetEffectType( fileHandle1, effectIndex, effectType );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Get effect type failed (GetIVTEffectType case)!" );
    EUNIT_ASSERT_DESC( (effectType == CHWRMHaptics::EHWRMHapticsTypeTimeline), "Wrong effect type (GetIVTEffectType case)!" );    

    // get type from corrupted IVT-data
    effectIndex = 5;
    err = iHaptics->GetEffectType( corruptedFile, effectIndex, effectType );
    EUNIT_ASSERT_DESC( (err == KErrArgument), "Get effect type from corrupted data, incorrect error code (GetIVTEffectType case)!" );

    // get type of the last effect
    effectIndex = 5;
    err = iHaptics->GetEffectType( fileHandle1, effectIndex, effectType );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Get effect type failed (GetIVTEffectType case)!" );
    EUNIT_ASSERT_DESC( (effectType == CHWRMHaptics::EHWRMHapticsTypeMagSweep), "Wrong effect type (GetIVTEffectType case)!" );    

    // get type using effect index out of bounds
    effectIndex = 6;
    err = iHaptics->GetEffectType( fileHandle1, effectIndex, effectType );
    EUNIT_ASSERT_DESC( (err != KErrNone), "Get effect type succeeded (GetIVTEffectType case)!" );

    // get type using unknown file handle
    effectIndex = 6;
    err = iHaptics->GetEffectType( unknownFileHandle, effectIndex, effectType );
    EUNIT_ASSERT_DESC( (err == KErrBadHandle), "Incorrect error code (GetIVTEffectType case)!" );


    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed! (GetIVTEffectType case))" );
    }

// ---------------------------------------------------------
// Getting effect name from preloaded IVT-data
// ---------------------------------------------------------
// 
void MT_HapticsClient::T_GetIVTEffectName()
    {
    TInt fileHandle1;
    TInt corruptedFile;
    TInt unknownFileHandle = 9;
    
    // load the IVT effect set 1
    TInt err = LoadIVTDataInternal( fileHandle1, EffectSetMT_ivt,
                                    sizeof( EffectSetMT_ivt ) );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (GetIVTEffectDuration case)!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle (GetIVTEffectName case)!" );

    // load corrupted IVT effect set
    err = LoadIVTDataInternal( corruptedFile, CorruptedMT_ivt,
                                    sizeof( CorruptedMT_ivt ), EFalse );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (GetIVTEffectDuration case)!" );
    EUNIT_ASSERT_DESC( corruptedFile, "Loading IVT data failed, empty fileHandle (GetIVTEffectName case)!" );


    // get name of the first effect
    TBuf8<128> effectName;
    TInt effectIndex = 0;
    _LIT8( KTimeline1, "Timeline1" );
    _LIT8( KMagSweep2, "MagSweep2" );
    err = iHaptics->GetEffectName( fileHandle1, effectIndex, effectName );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Get effect name failed (GetIVTEffectName case)!" );
    EUNIT_ASSERT_DESC( (effectName == KTimeline1), "Wrong effect name (GetIVTEffectName case)!" );    

    // get name from corrupted IVT-data
    effectIndex = 0;
    err = iHaptics->GetEffectName( corruptedFile, effectIndex, effectName );
    EUNIT_ASSERT_DESC( (err == KErrArgument), "Get effect name from corrupted data, incorrect error code (GetIVTEffectName case)!" );

    // get name of the last effect
    effectIndex = 5;
    err = iHaptics->GetEffectName( fileHandle1, effectIndex, effectName );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Get effect name failed (GetIVTEffectName case)!" );
    EUNIT_ASSERT_DESC( (effectName == KMagSweep2), "Wrong effect name (GetIVTEffectType case)!" );    

    // get name using effect index out of bounds
    effectIndex = 6;
    err = iHaptics->GetEffectName( fileHandle1, effectIndex, effectName );
    EUNIT_ASSERT_DESC( (err != KErrNone), "Get effect name succeeded (GetIVTEffectName case)!" );

    // get name using unknown file handle
    effectIndex = 0;
    err = iHaptics->GetEffectName( unknownFileHandle, effectIndex, effectName );
    EUNIT_ASSERT_DESC( (err == KErrBadHandle), "Incorrect error code (GetIVTEffectName case)!" );


    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed! (GetIVTEffectName case))" );
    }

// ---------------------------------------------------------
// Getting MagSweep effect definition from preloaded IVT-data
// ---------------------------------------------------------
// 
void MT_HapticsClient::T_GetIVTMagSweepEffectDefinition()
    {
    TInt fileHandle1;
    TInt corruptedFile;
    TInt unknownFileHandle = 9;
    
    // load the IVT effect set 1
    TInt err = LoadIVTDataInternal( fileHandle1, EffectSetMT_ivt,
                                    sizeof( EffectSetMT_ivt ) );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (GetIVTMagSweepEffectDefinition case)!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle (GetIVTMagSweepEffectDefinition case)!" );

    // load corrupted IVT effect set
    err = LoadIVTDataInternal( corruptedFile, CorruptedMT_ivt,
                                    sizeof( CorruptedMT_ivt ), EFalse );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (GetIVTMagSweepEffectDefinition case)!" );
    EUNIT_ASSERT_DESC( corruptedFile, "Loading IVT data failed, empty fileHandle (GetIVTMagSweepEffectDefinition case)!" );


    // get definition of the first MagSweep effect
    CHWRMHaptics::THWRMHapticsMagSweepEffect effectDefinition;
    TInt effectIndex = 4;
    err = iHaptics->GetMagSweepEffectDefinition( fileHandle1, effectIndex, effectDefinition );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Get effect definition failed (GetIVTMagSweepEffectDefinition case)!" );
    EUNIT_ASSERT_DESC( (effectDefinition.iDuration == 3000), "Wrong effect definition (GetIVTMagSweepEffectDefinition case)!" );    

    // get definition from corrupted IVT-data
    effectIndex = 0;
    err = iHaptics->GetMagSweepEffectDefinition( corruptedFile, effectIndex, effectDefinition );
    EUNIT_ASSERT_DESC( (err == KErrArgument), "Get effect definition from corrupted data, incorrect error code (GetIVTMagSweepEffectDefinition case)!" );

    // get definition of the last MagSweep effect
    effectIndex = 5;
    err = iHaptics->GetMagSweepEffectDefinition( fileHandle1, effectIndex, effectDefinition );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Get effect definition failed (GetIVTMagSweepEffectDefinition case)!" );
    EUNIT_ASSERT_DESC( (effectDefinition.iDuration == 4000), "Wrong effect definition (GetIVTMagSweepEffectDefinition case)!" );    

    // get definition using effect index out of bounds
    effectIndex = 6;
    err = iHaptics->GetMagSweepEffectDefinition( fileHandle1, effectIndex, effectDefinition );
    EUNIT_ASSERT_DESC( (err != KErrNone), "Get effect definition succeeded (GetIVTMagSweepEffectDefinition case)!" );

    // get definition using effect index not pointing to MagSweep effect
    effectIndex = 2;
    err = iHaptics->GetMagSweepEffectDefinition( fileHandle1, effectIndex, effectDefinition );
    EUNIT_ASSERT_DESC( (err != KErrNone), "Get effect definition succeeded (GetIVTMagSweepEffectDefinition case)!" );

    // get definition using unknown file handle
    effectIndex = 0;
    err = iHaptics->GetMagSweepEffectDefinition( unknownFileHandle, effectIndex, effectDefinition );
    EUNIT_ASSERT_DESC( (err == KErrBadHandle), "Incorrect error code (GetIVTMagSweepEffectDefinition case)!" );


    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed! (GetIVTMagSweepEffectDefinition case))" );
    }

// ---------------------------------------------------------
// Getting Periodic effect definition from preloaded IVT-data
// ---------------------------------------------------------
// 
void MT_HapticsClient::T_GetIVTPeriodicEffectDefinition()
    {
    TInt fileHandle1;
    TInt corruptedFile;
    TInt unknownFileHandle = 9;
    
    // load the IVT effect set 1
    TInt err = LoadIVTDataInternal( fileHandle1, EffectSetMT_ivt,
                                    sizeof( EffectSetMT_ivt ) );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (GetIVTPeriodicEffectDefinition case)!" );
    EUNIT_ASSERT_DESC( fileHandle1, "Loading IVT data failed, empty fileHandle (GetIVTPeriodicEffectDefinition case)!" );

    // load corrupted IVT effect set
    err = LoadIVTDataInternal( corruptedFile, CorruptedMT_ivt,
                                    sizeof( CorruptedMT_ivt ), EFalse );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Loading IVT data failed (GetIVTPeriodicEffectDefinition case)!" );
    EUNIT_ASSERT_DESC( corruptedFile, "Loading IVT data failed, empty fileHandle (GetIVTPeriodicEffectDefinition case)!" );


    // get definition of the first Periodic effect
    CHWRMHaptics::THWRMHapticsPeriodicEffect effectDefinition;
    TInt effectIndex = 2;
    err = iHaptics->GetPeriodicEffectDefinition( fileHandle1, effectIndex, effectDefinition );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Get effect definition failed (GetIVTPeriodicEffectDefinition case)!" );
    EUNIT_ASSERT_DESC( (effectDefinition.iDuration == 1000), "Wrong effect definition (GetIVTPeriodicEffectDefinition case)!" );    

    // get definition from corrupted IVT-data
    effectIndex = 0;
    err = iHaptics->GetPeriodicEffectDefinition( corruptedFile, effectIndex, effectDefinition );
    EUNIT_ASSERT_DESC( (err == KErrArgument), "Get effect definition from corrupted data, incorrect error code (GetIVTPeriodicEffectDefinition case)!" );

    // get definition of the last Periodic effect
    effectIndex = 3;
    err = iHaptics->GetPeriodicEffectDefinition( fileHandle1, effectIndex, effectDefinition );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Get effect definition failed (GetIVTPeriodicEffectDefinition case)!" );
    EUNIT_ASSERT_DESC( (effectDefinition.iDuration == 2000), "Wrong effect definition (GetIVTPeriodicEffectDefinition case)!" );    

    // get definition using effect index out of bounds
    effectIndex = 6;
    err = iHaptics->GetPeriodicEffectDefinition( fileHandle1, effectIndex, effectDefinition );
    EUNIT_ASSERT_DESC( (err != KErrNone), "Get effect definition succeeded (GetIVTPeriodicEffectDefinition case)!" );

    // get definition using effect index not pointing to Periodic effect
    effectIndex = 5;
    err = iHaptics->GetPeriodicEffectDefinition( fileHandle1, effectIndex, effectDefinition );
    EUNIT_ASSERT_DESC( (err != KErrNone), "Get effect definition succeeded (GetIVTPeriodicEffectDefinition case)!" );

    // get definition using unknown file handle
    effectIndex = 0;
    err = iHaptics->GetPeriodicEffectDefinition( unknownFileHandle, effectIndex, effectDefinition );
    EUNIT_ASSERT_DESC( (err == KErrBadHandle), "Incorrect error code (GetIVTPeriodicEffectDefinition case)!" );


    // clean up.. (Teardown() will handle the rest)
    err = iHaptics->DeleteAllEffectData();
    EUNIT_ASSERT_DESC( (err == KErrNone), "Delete All IVT data failed! (GetIVTPeriodicEffectDefinition case))" );
    }

// ---------------------------------------------------------
// Open asynchronously and close haptics client-server session, i.e.
// create and delete haptics client.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_CreateAndDeleteAsynchronousL()
    {
    // create iHaptics instance without observers
    TRequestStatus status;
    iHaptics = CHWRMHaptics::NewL( NULL, NULL, status );
    User::WaitForRequest( status );

    // the connection has been opened successfully, if iHaptics is valid and
    // error code is not returned
    EUNIT_ASSERT_DESC( status == KErrNone, "Haptics client creation failed!");
    EUNIT_ASSERT_DESC( iHaptics, "Haptics client creation failed!");

    // the connection is closed, when iHaptics is deleted
    delete iHaptics;
    iHaptics = NULL;
    }
    
// ---------------------------------------------------------
// Create several simultaneous sessions to haptics server
// asynchronously.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_SeveralSessionsAsynchronousL()
    {
    // create haptics instances without observers. This creates
    // client-server sessions (session instances in the server)
    TRequestStatus status1;
    TRequestStatus status2;
    TRequestStatus status3;
    CHWRMHaptics* haptics1 = CHWRMHaptics::NewL( NULL, NULL, status1 );
    CleanupStack::PushL( haptics1 ); //remove once CHWRMHaptics::NewLC() is used
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( NULL, NULL, status2 );
    CleanupStack::PushL( haptics2 ); //remove once CHWRMHaptics::NewLC() is used
    CHWRMHaptics* haptics3 = CHWRMHaptics::NewL( NULL, NULL, status3 );
    CleanupStack::PushL( haptics3 ); //remove once CHWRMHaptics::NewLC() is used
    User::WaitForRequest( status1 );
    User::WaitForRequest( status2 );
    User::WaitForRequest( status3 );

    // the connection has been opened successfully, if instances are valid and
    // error code is not returned
    EUNIT_ASSERT_DESC( status1 == KErrNone, "First haptics client creation failed!");
    EUNIT_ASSERT_DESC( status2 == KErrNone, "Second haptics client creation failed!");
    EUNIT_ASSERT_DESC( status3 == KErrNone, "Third haptics client creation failed!");
    EUNIT_ASSERT_DESC( haptics1, "First haptics client creation failed!");
    EUNIT_ASSERT_DESC( haptics2, "Second haptics client creation failed!");
    EUNIT_ASSERT_DESC( haptics3, "Third haptics client creation failed!");

    // delete haptics instances. This removes the sessions from 
    // haptics server, and closes the server, when the last session 
    // is closed
    CleanupStack::PopAndDestroy(3, haptics1);
    //delete haptics1; haptics1 = NULL;
    //delete haptics2; haptics2 = NULL;
    //delete haptics3; haptics3 = NULL;
    }

// ---------------------------------------------------------
// Create several consecutive sessions asynchronously
// to haptics server.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_ConsecutiveSessionsAsynchronousL()
    {
    // Do create and delete three times in a row
    T_CreateAndDeleteAsynchronousL();
    T_CreateAndDeleteAsynchronousL();
    T_CreateAndDeleteAsynchronousL();
    }
    
// ---------------------------------------------------------
// Attemps to pause an effect.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_PauseEffect()
    {
    // open a supported actuator
    T_OpenSupportedActuator();
    
    // pause effect testing
    TInt err = iHaptics->PausePlayingEffect( 0 );
    EUNIT_ASSERT_DESC( err == KErrNone, "Pausing an effect with handle 0 failed!");
    err = iHaptics->PausePlayingEffect( INT_MIN );
    EUNIT_ASSERT_DESC( err == KErrNone, "Pausing an effect with handle INT_MIN failed!");
    err = iHaptics->PausePlayingEffect( INT_MAX );
    EUNIT_ASSERT_DESC( err == KErrNone, "Pausing an effect with handle INT_MAX failed!");
    }

// ---------------------------------------------------------
// Attemps to resume an effect.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_ResumeEffect()
    {
    // open a supported actuator
    T_OpenSupportedActuator();
    
    // resume effect testing
    TInt err = iHaptics->ResumePausedEffect( 0 );
    EUNIT_ASSERT_DESC( err == KErrNone, "Resuming an effect with handle 0 failed!");
    err = iHaptics->ResumePausedEffect( INT_MIN );
    EUNIT_ASSERT_DESC( err == KErrNone, "Resuming an effect with handle INT_MIN failed!");
    err = iHaptics->ResumePausedEffect( INT_MAX );
    EUNIT_ASSERT_DESC( err == KErrNone, "Resuming an effect with handle INT_MAX failed!");
    }

// ---------------------------------------------------------
// Attemps to stop an effect.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_StopEffect()
    {
    // open a supported actuator
    T_OpenSupportedActuator();
    
    // stop effect testing
    TInt err = iHaptics->StopPlayingEffect( 0 );
    EUNIT_ASSERT_DESC( err == KErrNone, "Stopping an effect with handle 0 failed!");
    err = iHaptics->StopPlayingEffect( INT_MIN );
    EUNIT_ASSERT_DESC( err == KErrNone, "Stopping an effect with handle INT_MIN failed!");
    err = iHaptics->StopPlayingEffect( INT_MAX );
    EUNIT_ASSERT_DESC( err == KErrNone, "Stopping an effect with handle INT_MAX failed!");
    }

// ---------------------------------------------------------
// Attemps to stop all effects.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_StopAllEffects()
    {
    // open a supported actuator
    T_OpenSupportedActuator();
    
    // resume effect testing
    TInt err = iHaptics->StopAllPlayingEffects();
    EUNIT_ASSERT_DESC( err == KErrNone, "Stopping all effects failed!");
    }

// ---------------------------------------------------------
// Executes sequence: play effect, pause, resume, stop.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_PlayPauseResumeStop()
    {
    // open a supported actuator
    T_OpenSupportedActuator();
    
    // read magsweep effect to buffer
    RBuf8 effBuf;
    CleanupClosePushL( effBuf );
    CreateIVTEffect( effBuf, CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    
    // play the effect  
    TInt effectHandle = 0;
    TInt err = iHaptics->PlayEffect( effBuf, 0, effectHandle ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Playing IVT effect synchronously failed!" );
    EUNIT_ASSERT_DESC( effectHandle, "Playing IVT effect synchronously returned empty effect handle!" );

    // cleanup buffer
    CleanupStack::PopAndDestroy( &effBuf );

    // pause the effect
    err = iHaptics->PausePlayingEffect( effectHandle );
    EUNIT_ASSERT_DESC( err == KErrNone, "Pausing the effect failed!");

    // resume the effect
    err = iHaptics->ResumePausedEffect( effectHandle );
    EUNIT_ASSERT_DESC( err == KErrNone, "Resuming the effect failed!");

    // stop the effect
    err = iHaptics->StopPlayingEffect( effectHandle );
    EUNIT_ASSERT_DESC( err == KErrNone, "Stopping the effect failed!");
    }

// ---------------------------------------------------------
// Constant getters testing.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_ConstantGetters()
    {
    const TInt expectedInfRepeat          = 255;     // VIBE_REPEAT_COUNT_INFINITE
    const TInt expectedInfDuration        = INT_MAX; // VIBE_TIME_INFINITE;
    const TInt expectedMaxEffNameLen      = 128;     // VIBE_MAX_EFFECT_NAME_LENGTH
    const TInt expectedMaxDevNameLen      = 64;      // VIBE_MAX_DEVICE_NAME_LENGTH
    const TInt expectedMaxCapaStrLen      = 64;      // VIBE_MAX_CAPABILITY_STRING_LENGTH
    const TInt expectedMaxPropStrLen      = 64;      // VIBE_MAX_PROPERTY_STRING_LENGTH
    const TInt expectedMaxStrmSampleLen   = 255;     // VIBE_MAX_STREAMING_SAMPLE_SIZE
    const TInt expectedDefaultDevPriority = 0;       // VIBE_DEVPRIORITY_DEFAULT

    TInt infRepeat = iHaptics->InfiniteRepeat();
    EUNIT_ASSERT_DESC( KErrNotReady == infRepeat, "InfiniteRepeat() wrong error code" );
    TInt infDuration = iHaptics->InfiniteDuration();
    EUNIT_ASSERT_DESC( KErrNotReady == infDuration, "InfiniteDuration() wrong error code" );
    TInt maxEffNameLen = iHaptics->MaxEffectNameLength();
    EUNIT_ASSERT_DESC( KErrNotReady == maxEffNameLen, "MaxEffectNameLength() wrong error code" );
    TInt maxDevNameLen = iHaptics->MaxDeviceNameLength();
    EUNIT_ASSERT_DESC( KErrNotReady == maxDevNameLen, "MaxDeviceNameLength() wrong error code" );
    TInt maxCapaStrLen = iHaptics->MaxCapabilityStringLength();
    EUNIT_ASSERT_DESC( KErrNotReady == maxCapaStrLen, "MaxCapabilityStringLength() wrong error code" );
    TInt maxPropStrLen = iHaptics->MaxPropertyStringLength();
    EUNIT_ASSERT_DESC( KErrNotReady == maxPropStrLen, "MaxPropertyStringLength() wrong error code" );
    TInt maxStrmSampleLen = iHaptics->MaxStreamingSampleSize();
    EUNIT_ASSERT_DESC( KErrNotReady == maxStrmSampleLen, "MaxStreamingSampleSize() wrong error code" );
    TInt defaultDevPriority = iHaptics->DefaultDevicePriority();
    EUNIT_ASSERT_DESC( KErrNotReady == defaultDevPriority, "DefaultDevicePriority() wrong error code" );

    // open a supported actuator
    T_OpenSupportedActuator();

    infRepeat = iHaptics->InfiniteRepeat();
    EUNIT_ASSERT_DESC( expectedInfRepeat == infRepeat, "InfiniteRepeat() failed" );
    infDuration = iHaptics->InfiniteDuration();
    EUNIT_ASSERT_DESC( expectedInfDuration == infDuration, "InfiniteDuration() failed" );
    maxEffNameLen = iHaptics->MaxEffectNameLength();
    EUNIT_ASSERT_DESC( expectedMaxEffNameLen == maxEffNameLen, "MaxEffectNameLength() failed" );
    maxDevNameLen = iHaptics->MaxDeviceNameLength();
    EUNIT_ASSERT_DESC( expectedMaxDevNameLen == maxDevNameLen, "MaxDeviceNameLength() failed" );
    maxCapaStrLen = iHaptics->MaxCapabilityStringLength();
    EUNIT_ASSERT_DESC( expectedMaxCapaStrLen == maxCapaStrLen, "MaxCapabilityStringLength() failed" );
    maxPropStrLen = iHaptics->MaxPropertyStringLength();
    EUNIT_ASSERT_DESC( expectedMaxPropStrLen == maxPropStrLen, "MaxPropertyStringLength() failed" );
    maxStrmSampleLen = iHaptics->MaxStreamingSampleSize();
    EUNIT_ASSERT_DESC( expectedMaxStrmSampleLen == maxStrmSampleLen, "MaxStreamingSampleSize() failed" );
    defaultDevPriority = iHaptics->DefaultDevicePriority();
    EUNIT_ASSERT_DESC( expectedDefaultDevPriority == defaultDevPriority, "DefaultDevicePriority() failed" );
    }


// ---------------------------------------------------------
// Modifies playing magsweep effect using synchronous API call.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_ModifyMagSweepEffectSynchronous()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // modify effect
    CHWRMHaptics::THWRMHapticsMagSweepEffect modifiedEffect;
    modifiedEffect.iDuration    = 3030;
    modifiedEffect.iMagnitude   = 7070;
    modifiedEffect.iStyle       = CHWRMHaptics::EHWRMHapticsStyleStrong;
    modifiedEffect.iAttackTime  = 2000;
    modifiedEffect.iAttackLevel = 500;
    modifiedEffect.iFadeTime    = 100;
    modifiedEffect.iFadeLevel   = 10;

    TInt err = iHaptics->ModifyPlayingMagSweepEffect( 0, modifiedEffect );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Modifying magsweep effect with handle 0 failed!" );

    err = iHaptics->ModifyPlayingMagSweepEffect( INT_MIN, modifiedEffect );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Modifying magsweep effect with handle INT_MIN failed!" );

    err = iHaptics->ModifyPlayingMagSweepEffect( INT_MAX, modifiedEffect );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Modifying magsweep effect with handle INT_MAX failed!" );
    }


// ---------------------------------------------------------
// Modifies playing magsweep effect using asynchronous API call.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_ModifyMagSweepEffectAsynchronous()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // modify effect
    CHWRMHaptics::THWRMHapticsMagSweepEffect modifiedEffect;
    modifiedEffect.iDuration    = 5050;
    modifiedEffect.iMagnitude   = 1010;
    modifiedEffect.iStyle       = CHWRMHaptics::EHWRMHapticsStyleStrong;
    modifiedEffect.iAttackTime  = 2000;
    modifiedEffect.iAttackLevel = 500;
    modifiedEffect.iFadeTime    = 100;
    modifiedEffect.iFadeLevel   = 10;

    TRequestStatus status = KRequestPending;
    iHaptics->ModifyPlayingMagSweepEffect( 0, modifiedEffect, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Modifying magsweep effect with handle 0 failed!" );

    status = KRequestPending;
    iHaptics->ModifyPlayingMagSweepEffect( INT_MIN, modifiedEffect, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Modifying magsweep effect with handle INT_MIN failed!" );

    status = KRequestPending;
    iHaptics->ModifyPlayingMagSweepEffect( INT_MAX, modifiedEffect, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Modifying magsweep effect with handle INT_MAX failed!" );
    }


// ---------------------------------------------------------
// Modifies playing periodic effect using synchronous API call.
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_ModifyPeriodicEffectSynchronous()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // modify effect
    CHWRMHaptics::THWRMHapticsPeriodicEffect modifiedEffect;
    modifiedEffect.iDuration    = 6060;
    modifiedEffect.iMagnitude   = 2020;
    modifiedEffect.iPeriod      = 200;
    modifiedEffect.iStyle       = CHWRMHaptics::EHWRMHapticsStyleStrong;
    modifiedEffect.iAttackTime  = 2000;
    modifiedEffect.iAttackLevel = 500;
    modifiedEffect.iFadeTime    = 100;
    modifiedEffect.iFadeLevel   = 10;

    TInt err = iHaptics->ModifyPlayingPeriodicEffect( 0, modifiedEffect );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Modifying periodic effect with handle 0 failed!" );
    
    err = iHaptics->ModifyPlayingPeriodicEffect( INT_MIN, modifiedEffect );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Modifying periodic effect with handle INT_MIN failed!" );

    err = iHaptics->ModifyPlayingPeriodicEffect( INT_MAX, modifiedEffect );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Modifying periodic effect with handle INT_MAX failed!" );
    }


// ---------------------------------------------------------
// Modifies playing periodic effect using asynchronous API call.
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_ModifyPeriodicEffectAsynchronous()
    {
    // open actuator.. 
    T_OpenSupportedActuator();

    // modify effect
    CHWRMHaptics::THWRMHapticsPeriodicEffect modifiedEffect;
    modifiedEffect.iDuration    = 4040;
    modifiedEffect.iMagnitude   = 6060;
    modifiedEffect.iPeriod      = 200;
    modifiedEffect.iStyle       = CHWRMHaptics::EHWRMHapticsStyleStrong;
    modifiedEffect.iAttackTime  = 2000;
    modifiedEffect.iAttackLevel = 500;
    modifiedEffect.iFadeTime    = 100;
    modifiedEffect.iFadeLevel   = 10;

    TRequestStatus status = KRequestPending;
    iHaptics->ModifyPlayingPeriodicEffect( 0, modifiedEffect, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Modifying periodic effect with handle 0 failed!" );

    status = KRequestPending;
    iHaptics->ModifyPlayingPeriodicEffect( INT_MIN, modifiedEffect, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Modifying periodic effect with handle INT_MIN failed!" );

    status = KRequestPending;
    iHaptics->ModifyPlayingPeriodicEffect( INT_MAX, modifiedEffect, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone), "Modifying periodic effect with handle INT_MAX failed!" );
    }

// ---------------------------------------------------------
// Gets device TInt property
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_GetDeviceProperty_TInt_TInt()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // get the same value 1
    TInt retValue (0);
    TInt err = iHaptics->GetDeviceProperty( CHWRMHaptics::EHWRMHapticsStrength, retValue );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Getting device property (Strength) failed(1)!" );

    err = iHaptics->GetDeviceProperty( CHWRMHaptics::EHWRMHapticsDisableEffects, retValue );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Getting device property (disableEffects) failed(1)!" );
    }

// ---------------------------------------------------------
// Gets device TDesC property
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_GetDeviceProperty_TInt_TDesC()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // get property 
    TBuf8<100> retValue;
    TInt err = iHaptics->GetDeviceProperty( CHWRMHaptics::EHWRMHapticsLicensekey, retValue );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Setting device property (EHWRMHapticsLicensekey) failed!" );
    }

// ---------------------------------------------------------
// Sets device TInt property
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_SetDeviceProperty_TInt_TInt()
    {
    const TInt KSomeValue = 50; 
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // set property 
    TInt err = iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsStrength, KSomeValue ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Setting device property (Strength) failed!" );

    err = iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsDisableEffects, 0 ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Setting device property (disableEffects) failed!" );
    }

// ---------------------------------------------------------
// Sets device TDesC property
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_SetDeviceProperty_TInt_TDesC()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // set property    
    TInt err = iHaptics->SetDeviceProperty( 
                CHWRMHaptics::EHWRMHapticsLicensekey, 
                KNullDesC8 ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Setting device property (NULL license key) failed!" );

    err = iHaptics->SetDeviceProperty( 
                CHWRMHaptics::EHWRMHapticsLicensekey, 
                KPlainKey ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Setting device property (license key) failed!" );
    }

// ---------------------------------------------------------
// Gets device TInt capability
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_GetDeviceCapability_TInt_TInt()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // get the value
    TInt retValue (0);
    TInt expectedValue (1); // from testplugin's stubresponse.txt
    TInt err = iHaptics->GetDeviceCapability( CHWRMHaptics::EHWRMHapticsMaxNestedRepeats, retValue ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), 
                       "Getting device property (MaxNestedRepeats) failed!" );
    EUNIT_ASSERT_DESC( expectedValue == retValue, 
                       "Getting device property (MaxNestedRepeats) failed, wrong value!")                   
    }

// ---------------------------------------------------------
// Gets device TDesC capability
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_GetDeviceCapability_TInt_TDesC()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // get the same value
    TBuf8<100> retValue;
    retValue.FillZ(); // sets length, important here..
    TInt err = iHaptics->GetDeviceCapability( CHWRMHaptics::EHWRMHapticsDeviceName, retValue ); 
    EUNIT_ASSERT_DESC( (err == KErrNone), "Getting device property (DevName) failed!" );
    // the following are based on what the testplugin retunrs (i.e., bytes 
    // containing the ASCII character values for "S","T","U" and "B")
    const TInt expectedSize( 4 );
    EUNIT_ASSERT_DESC( expectedSize == retValue.Size(),
        "Getting device property (DevName) failed, wrong length!" );
    const TUint8 ascii4S( 0x53 );
    const TUint8 ascii4T( 0x54 );
    const TUint8 ascii4U( 0x55 );
    const TUint8 ascii4B( 0x42 );   
    EUNIT_ASSERT_DESC( ascii4S == retValue[0] && ascii4T == retValue[1] &&
                       ascii4U == retValue[2] && ascii4B == retValue[3],
        "Getting device property (DevName) failed, wrong data!" );
    }

// ---------------------------------------------------------
// Fetches the haptics status using the getter method.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_GetHapticsStatus()
    {
    MHWRMHapticsObserver::THWRMHapticsStatus status = iHaptics->HapticsStatus();
    EUNIT_ASSERT_DESC( (status == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable), 
                       "Invalid haptics status received!" );
    }

// ---------------------------------------------------------
// Fetches the haptics status using the observer.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_ObserveHapticsStatus()
    {
    // create callback instance
    THapticsStatusCallback callback( this );
    
    // create haptics client with observer
    CHWRMHaptics* haptics = CHWRMHaptics::NewL( &callback, NULL );

    // wait for the callback to be called
    iWait.Start();
    
    // check status (since execution of this method continues, the status
    // has been updated in the callback)
    EUNIT_ASSERT_DESC( (callback.Status() == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable), 
                       "Invalid haptics status received!" );

    // cleanup
    delete haptics;
    }

// ---------------------------------------------------------
// Fetches status values, when one client is reserving 
// haptics.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_GetHapticsStatusWithReservations()
    {
    // create haptics instances
    CHWRMHaptics* haptics1 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics1 );
    
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics2 );

    // make reservation using the first client
    haptics1->ReserveHapticsL();

    // check client status values
    MHWRMHapticsObserver::THWRMHapticsStatus status1 = haptics1->HapticsStatus();
    EUNIT_ASSERT_DESC( ( status1 == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );
    MHWRMHapticsObserver::THWRMHapticsStatus status2 = haptics2->HapticsStatus();
    EUNIT_ASSERT_DESC( ( status2 == MHWRMHapticsObserver::EHWRMHapticsStatusReserved ), 
                       "Invalid haptics status received!" );

    // release reservation
    haptics1->ReleaseHaptics();

    // check client status values
    status1 = haptics1->HapticsStatus();
    EUNIT_ASSERT_DESC( ( status1 == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );
    status2 = haptics2->HapticsStatus();
    EUNIT_ASSERT_DESC( ( status2 == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );

    // cleanup
    CleanupStack::PopAndDestroy( haptics2 );
    CleanupStack::PopAndDestroy( haptics1 );
    }

// ---------------------------------------------------------
// Status value observing, when one client is reserving
// haptics.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_ObserveHapticsStatusWithReservations()
    {
    // create callback instances
    THapticsStatusCallback callback1( this );
    THapticsStatusCallback callback2( this );

    // create haptics instances
    CHWRMHaptics* haptics1 = CHWRMHaptics::NewL( &callback1, NULL );
    CleanupStack::PushL( haptics1 );
    
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( &callback2, NULL );
    CleanupStack::PushL( haptics2 );

    // wait for the initial notifications to arrive
    iWait.Start();
    iWait.Start();

    // make reservation using the first client
    haptics1->ReserveHapticsL();
    
    // wait for the callback to be called (only second client 
    // should receive notification)
    iWait.Start();

    // check client status values
    MHWRMHapticsObserver::THWRMHapticsStatus status2 = callback2.Status();
    EUNIT_ASSERT_DESC( ( status2 == MHWRMHapticsObserver::EHWRMHapticsStatusReserved ), 
                       "Invalid haptics status received!" );

    // release reservation
    haptics1->ReleaseHaptics();

    // wait for the callback to be called (only second client 
    // should receive notification)
    iWait.Start();

    // check client status values
    status2 = callback2.Status();
    EUNIT_ASSERT_DESC( ( status2 == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );

    // cleanup
    CleanupStack::PopAndDestroy( haptics2 );
    CleanupStack::PopAndDestroy( haptics1 );
    }

// ---------------------------------------------------------
// Haptics status, when several clients are making 
// reservations with equal priorities.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_GetHapticsStatusWithSeveralReservations()
    {
    // create haptics instances
    CHWRMHaptics* haptics1 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics1 );
    
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics2 );
    
    // make reservation using both clients (second should become suspended)
    haptics1->ReserveHapticsL();
    haptics2->ReserveHapticsL();

    // check client status values
    MHWRMHapticsObserver::THWRMHapticsStatus status1 = haptics1->HapticsStatus();
    EUNIT_ASSERT_DESC( ( status1 == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );
    MHWRMHapticsObserver::THWRMHapticsStatus status2 = haptics2->HapticsStatus();
    EUNIT_ASSERT_DESC( ( status2 == MHWRMHapticsObserver::EHWRMHapticsStatusSuspended ), 
                       "Invalid haptics status received!" );
    
    // release reservation of the first client
    haptics1->ReleaseHaptics();
    
    // check client status values
    status1 = haptics1->HapticsStatus();
    EUNIT_ASSERT_DESC( ( status1 == MHWRMHapticsObserver::EHWRMHapticsStatusReserved ), 
                       "Invalid haptics status received!" );
    status2 = haptics2->HapticsStatus();
    EUNIT_ASSERT_DESC( ( status2 == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );
    
    // release reservation of the second client
    haptics2->ReleaseHaptics();
    
    // check client status values
    status1 = haptics1->HapticsStatus();
    EUNIT_ASSERT_DESC( ( status1 == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );
    status2 = haptics2->HapticsStatus();
    EUNIT_ASSERT_DESC( ( status2 == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );

    // cleanup
    CleanupStack::PopAndDestroy( haptics2 );
    CleanupStack::PopAndDestroy( haptics1 );
    }

// ---------------------------------------------------------
// Haptics status using observation, when several clients 
// are making reservations with equal priorities.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_ObserveHapticsStatusWithSeveralReservations()
    {
    // create callback instances
    THapticsStatusCallback callback1( this );
    THapticsStatusCallback callback2( this );

    // create haptics instances
    CHWRMHaptics* haptics1 = CHWRMHaptics::NewL( &callback1, NULL );
    CleanupStack::PushL( haptics1 );
    
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( &callback2, NULL );
    CleanupStack::PushL( haptics2 );

    // wait for the initial notifications to arrive
    iWait.Start();
    iWait.Start();

    // make reservation using the first client
    haptics1->ReserveHapticsL();

    // wait for the callback to be called (only second client 
    // should receive notification)
    iWait.Start();

    // check client status values
    MHWRMHapticsObserver::THWRMHapticsStatus status2 = callback2.Status();
    EUNIT_ASSERT_DESC( ( status2 == MHWRMHapticsObserver::EHWRMHapticsStatusReserved ), 
                       "Invalid haptics status received!" );

    // make reservation using the second client
    haptics2->ReserveHapticsL();

    // wait for the callback to be called (only second client 
    // should receive notification)
    iWait.Start();

    // check client status values
    status2 = callback2.Status();
    EUNIT_ASSERT_DESC( ( status2 == MHWRMHapticsObserver::EHWRMHapticsStatusSuspended ), 
                       "Invalid haptics status received!" );

    // release first client's reservation
    haptics1->ReleaseHaptics();

    // wait for the callback to be called (both clients should
    // receive new status)
    iWait.Start();
    iWait.Start();

    // check client status values
    MHWRMHapticsObserver::THWRMHapticsStatus status1 = callback1.Status();
    EUNIT_ASSERT_DESC( ( status1 == MHWRMHapticsObserver::EHWRMHapticsStatusReserved ), 
                       "Invalid haptics status received!" );
    status2 = callback2.Status();
    EUNIT_ASSERT_DESC( ( status2 == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );

    // release second client's reservation
    haptics2->ReleaseHaptics();

    // wait for the callback to be called (only first client 
    // should receive notification)
    iWait.Start();

    // check client status values
    status1 = callback1.Status();
    EUNIT_ASSERT_DESC( ( status1 == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );

    // cleanup
    CleanupStack::PopAndDestroy( haptics2 );
    CleanupStack::PopAndDestroy( haptics1 );
    }

// ---------------------------------------------------------
// Several clients with one making a reservation --> tests
// that all other clients become "reserved".
// ---------------------------------------------------------
//
void MT_HapticsClient::T_GetHapticsStatusWithSeveralClientsOneReservation()
    {
    // create haptics instances
    CHWRMHaptics* haptics1 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics1 );
    
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics2 );

    CHWRMHaptics* haptics3 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics3 );

    // reservation using first client
    haptics1->ReserveHapticsL();
    
    // check client status values
    MHWRMHapticsObserver::THWRMHapticsStatus status1 = haptics1->HapticsStatus();
    EUNIT_ASSERT_DESC( ( status1 == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );
    MHWRMHapticsObserver::THWRMHapticsStatus status2 = haptics2->HapticsStatus();
    EUNIT_ASSERT_DESC( ( status2 == MHWRMHapticsObserver::EHWRMHapticsStatusReserved ), 
                       "Invalid haptics status received!" );
    MHWRMHapticsObserver::THWRMHapticsStatus status3 = haptics3->HapticsStatus();
    EUNIT_ASSERT_DESC( ( status3 == MHWRMHapticsObserver::EHWRMHapticsStatusReserved ), 
                       "Invalid haptics status received!" );

    // release reservation
    haptics1->ReleaseHaptics();

    // check client status values
    status1 = haptics1->HapticsStatus();
    EUNIT_ASSERT_DESC( ( status1 == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );
    status2 = haptics2->HapticsStatus();
    EUNIT_ASSERT_DESC( ( status2 == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );
    status3 = haptics3->HapticsStatus();
    EUNIT_ASSERT_DESC( ( status3 == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );

    // cleanup
    CleanupStack::PopAndDestroy( haptics3 );
    CleanupStack::PopAndDestroy( haptics2 );
    CleanupStack::PopAndDestroy( haptics1 );
    }

// ---------------------------------------------------------
// Several clients with one making a reservation --> tests
// that all other clients become "reserved" using observers.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_ObserveHapticsStatusWithSeveralClientsOneReservations()
    {
    // create callback instances
    THapticsStatusCallback callback1( this );
    THapticsStatusCallback callback2( this );
    THapticsStatusCallback callback3( this );

    // create haptics instances
    CHWRMHaptics* haptics1 = CHWRMHaptics::NewL( &callback1, NULL );
    CleanupStack::PushL( haptics1 );
    
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( &callback2, NULL );
    CleanupStack::PushL( haptics2 );

    CHWRMHaptics* haptics3 = CHWRMHaptics::NewL( &callback3, NULL );
    CleanupStack::PushL( haptics3 );

    // wait for the initial notifications to arrive
    iWait.Start();
    iWait.Start();
    iWait.Start();

    // reservation using first client
    haptics1->ReserveHapticsL();

    // wait for the notifications for client2 and client3
    iWait.Start();
    iWait.Start();

    // check client status values
    MHWRMHapticsObserver::THWRMHapticsStatus status2 = callback2.Status();
    EUNIT_ASSERT_DESC( ( status2 == MHWRMHapticsObserver::EHWRMHapticsStatusReserved ), 
                       "Invalid haptics status received!" );
    MHWRMHapticsObserver::THWRMHapticsStatus status3 = callback3.Status();
    EUNIT_ASSERT_DESC( ( status3 == MHWRMHapticsObserver::EHWRMHapticsStatusReserved ), 
                       "Invalid haptics status received!" );

    // release reservation using first client
    haptics1->ReleaseHaptics();

    // wait for the notifications for client2 and client3
    iWait.Start();
    iWait.Start();

    // check client status values
    status2 = callback2.Status();
    EUNIT_ASSERT_DESC( ( status2 == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );
    status3 = callback3.Status();
    EUNIT_ASSERT_DESC( ( status3 == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );

    // cleanup
    CleanupStack::PopAndDestroy( haptics3 );
    CleanupStack::PopAndDestroy( haptics2 );
    CleanupStack::PopAndDestroy( haptics1 );
    }

// ---------------------------------------------------------
// Creates streaming effect.
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_CreateStreamingEffect()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    TInt effectHandle;
    TInt err = iHaptics->CreateStreamingEffect( effectHandle );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Creating streaming effect failed!" );
    EUNIT_ASSERT_DESC( effectHandle, "Creating streaming effect returned empty effect handle!" );
    }

// ---------------------------------------------------------
// Plays streaming sample using synchronous API call.
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_PlayStreamingSample()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    RBuf8 validStreamSample;
    RBuf8 tooBigStreamSample;
    CleanupClosePushL( validStreamSample );
    CleanupClosePushL( tooBigStreamSample );
    
    TInt maxValidSize = iHaptics->MaxStreamingSampleSize();
    CreateDummyStreamSample( maxValidSize, validStreamSample );

    // valid sized sample 0 handle 0 offset
    TInt err = iHaptics->PlayStreamingSample( 0, validStreamSample );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Play streaming sample failed!" );

    // normal sample MIN handle MIN offset
    err = iHaptics->PlayStreamingSample( INT_MIN, validStreamSample );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Play streaming sample failed!" );

    // normal sample MAX handle MAX offset
    err = iHaptics->PlayStreamingSample( INT_MAX, validStreamSample );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Play streaming sample failed!" );
    
    // over sized sample
    CreateDummyStreamSample( maxValidSize+1, tooBigStreamSample );
    err = iHaptics->PlayStreamingSample( INT_MAX, tooBigStreamSample );
    EUNIT_ASSERT_DESC( (err == KErrArgument), "Play streaming sample (over sized sample), wrong error code" );


    // Haptics reserved for higher priority client
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics2 );

    TRAPD( error, haptics2->OpenActuatorL( EHWRMLogicalActuatorAny ) );

    // set new client priority to maximum
    haptics2->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsPriority, 
                                 KHWRMHapticsMaxDevicePriority );
    // set default client priority to min
    iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsPriority, 
                                 KHWRMHapticsMinDevicePriority );
    TRAP( error, haptics2->ReserveHapticsL() );

    err = iHaptics->PlayStreamingSample( INT_MIN, validStreamSample );
    EUNIT_ASSERT_DESC( ( err == KErrInUse ), "Play streaming sample (haptics reserved), wrong error code" );



    // clean up.. (Teardown will complete these)
    CleanupStack::PopAndDestroy( haptics2 );
    CleanupStack::PopAndDestroy(); //tooBigStreamSample
    CleanupStack::PopAndDestroy(); //validStreamSample
    }

// ---------------------------------------------------------
// Plays streaming sample using asynchronous API call.
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_PlayStreamingSampleAsync()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    RBuf8 validStreamSample;
    RBuf8 tooBigStreamSample;
    CleanupClosePushL( validStreamSample );
    CleanupClosePushL( tooBigStreamSample );
    
    TInt maxValidSize = iHaptics->MaxStreamingSampleSize();
    CreateDummyStreamSample( maxValidSize, validStreamSample );

    // normal sample 0 handle 0 offset
    TRequestStatus status = KRequestPending;
    iHaptics->PlayStreamingSample( 0, validStreamSample, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone ), "Play streaming sample failed!" );

    // normal sample MIN handle MIN offset
    status = KRequestPending;
    iHaptics->PlayStreamingSample( INT_MIN, validStreamSample, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone ), "Play streaming sample failed!" );

    // normal sample MAX handle MAX offset
    status = KRequestPending;
    iHaptics->PlayStreamingSample( INT_MAX, validStreamSample, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone ), "Play streaming sample failed!" );
    
    // over sized sample
    CreateDummyStreamSample( maxValidSize+1, tooBigStreamSample );
    status = KRequestPending;
    iHaptics->PlayStreamingSample( INT_MAX, tooBigStreamSample, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrArgument ), "Play streaming sample (over sized sample), wrong error code" );

    // Haptics reserved for higher priority client
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics2 );

    TRAPD( error, haptics2->OpenActuatorL( EHWRMLogicalActuatorAny ) );

    // set new client priority to maximum
    haptics2->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsPriority, 
                                 KHWRMHapticsMaxDevicePriority );
    // set default client priority to min
    iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsPriority, 
                                 KHWRMHapticsMinDevicePriority );
    TRAP( error, haptics2->ReserveHapticsL() );

    status = KRequestPending;
    iHaptics->PlayStreamingSample( INT_MIN, validStreamSample, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrInUse ), "Play streaming sample (haptics reserved), wrong error code" );



    // clean up.. (Teardown will complete these)
    CleanupStack::PopAndDestroy( haptics2 );
    CleanupStack::PopAndDestroy(); //tooBigStreamSample
    CleanupStack::PopAndDestroy(); //validStreamSample
    }

// ---------------------------------------------------------
// Plays streaming sample with offset using
// synchronous API call.
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_PlayStreamingSampleWithOffset()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    RBuf8 validStreamSample;
    RBuf8 tooBigStreamSample;
    CleanupClosePushL( validStreamSample );
    CleanupClosePushL( tooBigStreamSample );
    
    TInt maxValidSize = iHaptics->MaxStreamingSampleSize();
    CreateDummyStreamSample( maxValidSize, validStreamSample );

    // valid sized sample 0 handle 0 offset
    TInt err = iHaptics->PlayStreamingSampleWithOffset( 0, validStreamSample, 0 );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Play streaming sample with offset failed!" );

    // normal sample MIN handle MIN offset
    err = iHaptics->PlayStreamingSampleWithOffset( INT_MIN, validStreamSample, INT_MIN );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Play streaming sample with offset failed!" );

    // normal sample MAX handle MAX offset
    err = iHaptics->PlayStreamingSampleWithOffset( INT_MAX, validStreamSample, INT_MAX );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Play streaming sample with offset failed!" );
    
    // over sized sample
    CreateDummyStreamSample( maxValidSize+1, tooBigStreamSample );
    err = iHaptics->PlayStreamingSampleWithOffset( INT_MAX, tooBigStreamSample, INT_MAX );
    EUNIT_ASSERT_DESC( (err == KErrArgument), "Play streaming sample with offset (over sized sample), wrong error code" );

    // Haptics reserved for higher priority client
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics2 );

    TRAPD( error, haptics2->OpenActuatorL( EHWRMLogicalActuatorAny ) );

    // set new client priority to maximum
    haptics2->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsPriority, 
                                 KHWRMHapticsMaxDevicePriority );
    // set default client priority to min
    iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsPriority, 
                                 KHWRMHapticsMinDevicePriority );
    TRAP( error, haptics2->ReserveHapticsL() );

    err = iHaptics->PlayStreamingSample( INT_MIN, validStreamSample );
    EUNIT_ASSERT_DESC( ( err == KErrInUse ), "Play streaming sample (haptics reserved), wrong error code" );



    // clean up.. (Teardown will complete these)
    CleanupStack::PopAndDestroy( haptics2 );
    CleanupStack::PopAndDestroy(); //tooBigStreamSample
    CleanupStack::PopAndDestroy(); //validStreamSample
    }

// ---------------------------------------------------------
// Plays streaming sample with offset using
// asynchronous API call.
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_PlayStreamingSampleWithOffsetAsync()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    RBuf8 validStreamSample;
    RBuf8 tooBigStreamSample;
    CleanupClosePushL( validStreamSample );
    CleanupClosePushL( tooBigStreamSample );
    
    TInt maxValidSize = iHaptics->MaxStreamingSampleSize();
    CreateDummyStreamSample( maxValidSize, validStreamSample );

    // normal sample 0 handle 0 offset
    TRequestStatus status = KRequestPending;
    iHaptics->PlayStreamingSampleWithOffset( 0, validStreamSample, 0, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone ), "Play streaming sample with offset failed!" );

    // normal sample MIN handle MIN offset
    status = KRequestPending;
    iHaptics->PlayStreamingSampleWithOffset( INT_MIN, validStreamSample, INT_MIN, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone ), "Play streaming sample with offset failed!" );

    // normal sample MAX handle MAX offset
    status = KRequestPending;
    iHaptics->PlayStreamingSampleWithOffset( INT_MAX, validStreamSample, INT_MAX, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrNone ), "Play streaming sample with offset failed!" );
    
    // over sized sample
    CreateDummyStreamSample( maxValidSize+1, tooBigStreamSample );
    status = KRequestPending;
    iHaptics->PlayStreamingSampleWithOffset( INT_MAX, tooBigStreamSample, INT_MAX, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrArgument ), "Play streaming sample with offset (over sized sample), wrong error code" );

    // Haptics reserved for higher priority client
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( NULL, NULL );
    CleanupStack::PushL( haptics2 );

    TRAPD( error, haptics2->OpenActuatorL( EHWRMLogicalActuatorAny ) );

    // set new client priority to maximum
    haptics2->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsPriority, 
                                 KHWRMHapticsMaxDevicePriority );
    // set default client priority to min
    iHaptics->SetDeviceProperty( CHWRMHaptics::EHWRMHapticsPriority, 
                                 KHWRMHapticsMinDevicePriority );
    TRAP( error, haptics2->ReserveHapticsL() );

    status = KRequestPending;
    iHaptics->PlayStreamingSample( INT_MIN, validStreamSample, status );
    User::WaitForRequest( status );
    EUNIT_ASSERT_DESC( (status.Int() == KErrInUse ), "Play streaming sample with offset (haptics reserved), wrong error code" );



    // clean up.. (Teardown will complete these)
    CleanupStack::PopAndDestroy( haptics2 );
    CleanupStack::PopAndDestroy(); //tooBigStreamSample
    CleanupStack::PopAndDestroy(); //validStreamSample
    }

// ---------------------------------------------------------
// Destroys streaming effect.
// ---------------------------------------------------------
//  
void MT_HapticsClient::T_DestroyStreamingEffect()
    {
    // open actuator.. 
    T_OpenSupportedActuator();
    
    // zero handle
    TInt err = iHaptics->DestroyStreamingEffect( 0 );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Destroying streaming effect with handle 0 failed!" );

    // minimum handle value
    err = iHaptics->DestroyStreamingEffect( INT_MIN );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Destroying streaming effect with handle INT_MIN failed!" );

    // maximum handle value
    err = iHaptics->DestroyStreamingEffect( INT_MAX );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Destroying streaming effect with handle INT_MAX failed!" );
    }


// ---------------------------------------------------------
// Fetches actuator state using the observer.
// ---------------------------------------------------------
//
void MT_HapticsClient::T_ObserveActuatorStatus()
    {
    // Create 2 actuator status observer instances
    TActuatorStatusCallback callback1( this );
    TActuatorStatusCallback callback2( this );
    
    // create 1st haptics client with actuator observer
    CHWRMHaptics* haptics1 = CHWRMHaptics::NewL( NULL, &callback1 );
    CleanupStack::PushL( haptics1 );

    //Open actuator
    TRAPD( err, haptics1->OpenActuatorL( EHWRMLogicalActuatorAny ) );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Opening actuator with type Any failed!" );

    // wait for the callback to be called
    iWait.Start();

    // check actuator state (since execution of this method continues, the state
    // has been updated in the callback)
    EUNIT_ASSERT_DESC( (callback1.Status(EHWRMLogicalActuatorAny) == MHWRMHapticsActuatorObserver::EHWRMActuatorEnabled),
                       "Invalid actuator state received!" );

    // create 2nd haptics client with actuator observer
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( NULL, &callback2 );
    CleanupStack::PushL( haptics2 );
    
    //Open actuator
    TRAP( err, haptics2->OpenActuatorL( EHWRMLogicalActuatorDevice ) );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Opening actuator with type Device failed!" );

    // wait for the callback to be called
    iWait.Start();

    // check actuator state (since execution of this method continues, the state
    // has been updated in the callback)
    EUNIT_ASSERT_DESC( (callback2.Status(EHWRMLogicalActuatorDevice) == MHWRMHapticsActuatorObserver::EHWRMActuatorEnabled),
                       "Invalid actuator state received!" );


    // cleanup
    CleanupStack::PopAndDestroy( haptics2 );
    CleanupStack::PopAndDestroy( haptics1 );
    }


// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void MT_HapticsClient::T_ObserveActuatorStatusAndHapticsStatus()
    {
    //  Create 3 actuator status observers.
    TActuatorStatusCallback actuatorCallback1( this );
    TActuatorStatusCallback actuatorCallback2( this );
    TActuatorStatusCallback actuatorCallback3( this );

    // Create 3 haptics status observers.
    THapticsStatusCallback hapticsCallback1( this );
    THapticsStatusCallback hapticsCallback2( this );
    THapticsStatusCallback hapticsCallback3( this );
    
    // create 1st haptics client with observers
    CHWRMHaptics* haptics1 = CHWRMHaptics::NewL( &hapticsCallback1, &actuatorCallback1 );
    CleanupStack::PushL( haptics1 );

    //Open actuator
    TRAPD( err, haptics1->OpenActuatorL( EHWRMLogicalActuatorDevice ) );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Opening actuator with type Device failed!" );

    // wait for the haptics status callback to be called
    iWait.Start();
    // wait for the actuator status callback to be called
    iWait.Start();

    // check actuator status (since execution of this method continues, the status
    // has been updated in the callback)
    EUNIT_ASSERT_DESC( (actuatorCallback1.Status(EHWRMLogicalActuatorDevice) == MHWRMHapticsActuatorObserver::EHWRMActuatorEnabled),
                       "Invalid actuator status received!" );
    // check haptics status
    EUNIT_ASSERT_DESC( ( hapticsCallback1.Status() == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );

    haptics1->ReserveHapticsL();


    // create 2nd and 3rd haptics clients with observers
    CHWRMHaptics* haptics2 = CHWRMHaptics::NewL( &hapticsCallback2, &actuatorCallback2 );
    CleanupStack::PushL( haptics2 );
    CHWRMHaptics* haptics3 = CHWRMHaptics::NewL( &hapticsCallback3, &actuatorCallback3 );
    CleanupStack::PushL( haptics3 );

    // reserve haptics for 2nd client
    haptics2->ReserveHapticsL();

    //Open actuator
    TRAP( err, haptics2->OpenActuatorL( EHWRMLogicalActuatorAny ) );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Opening actuator with type Any failed!" );

    TRAP( err, haptics3->OpenActuatorL( EHWRMLogicalActuatorDevice ) );
    EUNIT_ASSERT_DESC( (err == KErrNone), "Opening actuator with type Device failed!" );

    // wait for the haptics status callback to be called
    iWait.Start();
    iWait.Start();
    iWait.Start();
    // wait for the actuator status callback to be called
    iWait.Start();
    iWait.Start();

    // check haptics status (since execution of this method continues, the status
    // has been updated in the callback)
    EUNIT_ASSERT_DESC( ( hapticsCallback2.Status() == MHWRMHapticsObserver::EHWRMHapticsStatusSuspended ), 
                       "Invalid haptics status received!" );
    EUNIT_ASSERT_DESC( ( hapticsCallback3.Status() == MHWRMHapticsObserver::EHWRMHapticsStatusReserved ), 
                       "Invalid haptics status received!" );
    // check actuator status
    EUNIT_ASSERT_DESC( (actuatorCallback2.Status(EHWRMLogicalActuatorAny) == MHWRMHapticsActuatorObserver::EHWRMActuatorEnabled),
                       "Invalid actuator status received!" );
    EUNIT_ASSERT_DESC( (actuatorCallback3.Status(EHWRMLogicalActuatorDevice) == MHWRMHapticsActuatorObserver::EHWRMActuatorEnabled),
                       "Invalid actuator status received!" );

    // release haptics
    haptics1->ReleaseHaptics();

    // wait for the haptics status callback to be called    
    iWait.Start();
    iWait.Start();

    EUNIT_ASSERT_DESC( ( hapticsCallback1.Status() == MHWRMHapticsObserver::EHWRMHapticsStatusReserved ), 
                       "Invalid haptics status received!" );
    EUNIT_ASSERT_DESC( ( hapticsCallback2.Status() == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );


    // release haptics
    haptics2->ReleaseHaptics();

    // wait for the haptics status callback to be called    
    iWait.Start();
    iWait.Start();

    EUNIT_ASSERT_DESC( ( hapticsCallback1.Status() == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );
    EUNIT_ASSERT_DESC( ( hapticsCallback3.Status() == MHWRMHapticsObserver::EHWRMHapticsStatusAvailable ), 
                       "Invalid haptics status received!" );

    // cleanup
    CleanupStack::PopAndDestroy( haptics3 );
    CleanupStack::PopAndDestroy( haptics2 );
    CleanupStack::PopAndDestroy( haptics1 );
    }
	
void MT_HapticsClient::T_PluginMgrTransTimerExpires()
    {
    // open actuator.. 
    T_OpenSupportedActuator();

    _LIT8 ( KTestPluginMgrTransTimerExpires, "KTestPluginMgrTransTimerExpires" );
    // set property with the TestCase ID string, so that the test plug-in doesn't send the response for the current
    // transaction, till the Plug-in manager Transction timer expires.
    // Out-come: Plug-in manager should hanlde this scenario and send cancel transaction command to test plug-in.
    TInt err = iHaptics->SetDeviceProperty( 
                CHWRMHaptics::EHWRMHapticsLicensekey, 
                KTestPluginMgrTransTimerExpires ); 
    EUNIT_ASSERT_DESC( (err == KErrTimedOut), "Setting device property (license key) failed!" );
    }



// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    MT_HapticsClient,
    "Haptics Client Module Tests",
    "MODULE" )

// 1,01
EUNIT_TEST(
    "Create and destroy",
    "HapticsClient",
    "NewL",
    "FUNCTIONALITY",
    Dummy, T_CreateAndDeleteL, Dummy)

// 1,02
EUNIT_TEST(
    "Several sessions",
    "HapticsClient",
    "NewL",
    "FUNCTIONALITY",
    Dummy, T_SeveralSessionsL, Dummy)

// 1,03
EUNIT_TEST(
    "Consecutive sessions",
    "HapticsClient",
    "NewL",
    "FUNCTIONALITY",
    Dummy, T_ConsecutiveSessionsL, Dummy)

// 1,04
EUNIT_TEST(
    "Supported actuators",
    "HapticsClient",
    "SupportedActuators",
    "FUNCTIONALITY",
    SetupL, T_SupportedActuators, Teardown)

// 1,05
EUNIT_TEST(
    "Open supported actuator",
    "HapticsClient",
    "OpenActuatorL",
    "FUNCTIONALITY",
    SetupL, T_OpenSupportedActuator, Teardown)

// 1,06
EUNIT_TEST(
    "Open unsupported actuator",
    "HapticsClient",
    "OpenActuatorL",
    "FUNCTIONALITY",
    SetupL, T_OpenNotSupportedActuator, Teardown)

// 1,07
EUNIT_TEST(
    "Open all actuators",
    "HapticsClient",
    "OpenActuatorL",
    "FUNCTIONALITY",
    SetupL, T_OpenAllActuators, Teardown)

// 1,08
EUNIT_TEST(
    "2 clients open actuators",
    "HapticsClient",
    "OpenActuatorL",
    "FUNCTIONALITY",
    Dummy, T_SeveralClientsWithOpenActuatorsL, Dummy)

// 1,09
EUNIT_TEST(
    "Play MagSweep synch (data)",
    "HapticsClient",
    "PlayMagSweepEffect",
    "FUNCTIONALITY",
    SetupL, T_PlayMagSweepEffectSynchronous, Teardown)

// 1,10
EUNIT_TEST(
    "Play MagSweep asynch (data)",
    "HapticsClient",
    "PlayMagSweepEffect",
    "FUNCTIONALITY",
    SetupL, T_PlayMagSweepEffectAsynchronous, Teardown)

// 1,11
EUNIT_TEST(
    "Play Periodic synch (data)",
    "HapticsClient",
    "PlayPeriodicEffect",
    "FUNCTIONALITY",
    SetupL, T_PlayPeriodicEffectSynchronous, Teardown)

// 1,12
EUNIT_TEST(
    "Play Periodic asynch (data)",
    "HapticsClient",
    "PlayPeriodicEffect",
    "FUNCTIONALITY",
    SetupL, T_PlayPeriodicEffectAsynchronous, Teardown)

// 1,13
EUNIT_TEST(
    "Play IVT synch (data)",
    "HapticsClient",
    "PlayEffect",
    "FUNCTIONALITY",
    SetupL, T_PlayIVTEffectWithDirectDataSynchronous, Teardown)

// 1,14
EUNIT_TEST(
    "Play IVT asynch (data)",
    "HapticsClient",
    "PlayEffect",
    "FUNCTIONALITY",
    SetupL, T_PlayIVTEffectWithDirectDataAsynchronous, Teardown)

// 1,15
EUNIT_TEST(
    "Repeat IVT synch (data)",
    "HapticsClient",
    "PlayEffectRepeat",
    "FUNCTIONALITY",
    SetupL, T_PlayIVTEffectRepeatWithDirectDataSynchronous, Teardown)

// 1,16
EUNIT_TEST(
    "Repeat IVT asynch (data)",
    "HapticsClient",
    "PlayEffectRepeat",
    "FUNCTIONALITY",
    SetupL, T_PlayIVTEffectRepeatWithDirectDataAsynchronous, Teardown)

// 1,17
EUNIT_TEST(
    "Get effect state",
    "HapticsClient",
    "GetEffectState",
    "FUNCTIONALITY",
    SetupL, T_GetEffectState, Teardown)

// 1,18
EUNIT_TEST(
    "Repeat IVT synch (data - non-timeline)",
    "HapticsClient",
    "PlayEffectRepeat",
    "FUNCTIONALITY",
    SetupL, T_PlayNonTimelineIVTEffectRepeatWithDirectDataSynchronous, Teardown)

// 1,19
EUNIT_TEST(
    "Repeat IVT asynch (data - non-timeline)",
    "HapticsClient",
    "PlayEffectRepeat",
    "FUNCTIONALITY",
    SetupL, T_PlayNonTimelineIVTEffectRepeatWithDirectDataAsynchronous, Teardown)

// 1,20
EUNIT_TEST(
    "Zero-repeat IVT synch (data)",
    "HapticsClient",
    "PlayEffectRepeat",
    "FUNCTIONALITY",
    SetupL, T_PlayIVTEffectRepeatWithDirectDataWithZeroRepeatValueSynchronous, Teardown)

// 1,21
EUNIT_TEST(
    "Zero-repeat IVT asynch (data)",
    "HapticsClient",
    "PlayEffectRepeat",
    "FUNCTIONALITY",
    SetupL, T_PlayIVTEffectRepeatWithDirectDataWithZeroRepeatValueAsynchronous, Teardown)

// 1,22
EUNIT_TEST(
    "Load IVT data",
    "HapticsClient",
    "LoadData",
    "FUNCTIONALITY",
    SetupL, T_LoadIVTData, Teardown)

// 1,23
EUNIT_TEST(
    "Delete IVT data",
    "HapticsClient",
    "DeleteEffectData",
    "FUNCTIONALITY",
    SetupL, T_DeleteIVTData, Teardown)

// 1,24
EUNIT_TEST(
    "Delete All IVT data",
    "HapticsClient",
    "DeleteAllEffectData",
    "FUNCTIONALITY",
    SetupL, T_DeleteAllIVTData, Teardown)

// 1,35
EUNIT_TEST(
    "Load and delete multiple",
    "HapticsClient",
    "LoadAndDeleteEffectData",
    "FUNCTIONALITY",
    SetupL, T_LoadAndDeleteIVTData, Teardown)

// 1,36
EUNIT_TEST(
    "Load and delete mixed",
    "HapticsClient",
    "LoadAndDeleteEffectDataMixed",
    "FUNCTIONALITY",
    SetupL, T_LoadAndDeleteIVTDataMixed, Teardown)

// 1,37
EUNIT_TEST(
    "Load, Del, Play sync",
    "HapticsClient",
    "LoadDeletePlaySync",
    "FUNCTIONALITY",
    SetupL, T_LoadDeletePlaySync, Teardown)

// 1,38
EUNIT_TEST(
    "Load, Del, Play async",
    "HapticsClient",
    "LoadDeletePlayAsync",
    "FUNCTIONALITY",
    SetupL, T_LoadDeletePlayAsync, Teardown)

// 1,39
EUNIT_TEST(
    "Load, Del, Repeat sync",
    "HapticsClient",
    "LoadDeletePlayRepeatSync",
    "FUNCTIONALITY",
    SetupL, T_LoadDeletePlayRepeatSync, Teardown)

// 1,40
EUNIT_TEST(
    "Load, Del, Repeat async",
    "HapticsClient",
    "LoadDeletePlayRepeatAsync",
    "FUNCTIONALITY",
    SetupL, T_LoadDeletePlayRepeatAsync, Teardown)

// 1,25
EUNIT_TEST(
    "Play loaded IVT synch",
    "HapticsClient",
    "PlayIVTEffect",
    "FUNCTIONALITY",
    SetupL, T_PlayIVTEffectWithLoadedDataSynchronous, Teardown)

// 1,26
EUNIT_TEST(
    "Play loaded IVT asynch",
    "HapticsClient",
    "PlayIVTEffect",
    "FUNCTIONALITY",
    SetupL, T_PlayIVTEffectWithLoadedDataAsynchronous, Teardown)

// 1,27
EUNIT_TEST(
    "Repeat loaded IVT synch",
    "HapticsClient",
    "PlayIVTEffectRepeat",
    "FUNCTIONALITY",
    SetupL, T_PlayIVTEffectRepeatWithLoadedDataSynchronous, Teardown)

// 1,28
EUNIT_TEST(
    "Repeat loaded IVT asynch",
    "HapticsClient",
    "PlayIVTEffectRepeat",
    "FUNCTIONALITY",
    SetupL, T_PlayIVTEffectRepeatWithLoadedDataAsynchronous, Teardown)

// 1,29
EUNIT_TEST(
    "Load multiple IVT-data",
    "HapticsClient",
    "LoadIVTData",
    "FUNCTIONALITY",
    SetupL, T_LoadMultipleIVTData, Teardown)

// 1,30
EUNIT_TEST(
    "Play from multiple IVT-data",
    "HapticsClient",
    "PlayIVTEffect",
    "FUNCTIONALITY",
    SetupL, T_PlayIVTEffectsFromMultipleIVTDataSynchronous, Teardown)

// 1,31
EUNIT_TEST(
    "Play from multiple IVT-data",
    "HapticsClient",
    "PlayIVTEffect",
    "FUNCTIONALITY",
    SetupL, T_PlayIVTEffectsFromMultipleIVTDataAsynchronous, Teardown)

// 1,32
EUNIT_TEST(
    "Play repeat from multiple IVT-data",
    "HapticsClient",
    "PlayIVTEffectRepeat",
    "FUNCTIONALITY",
    SetupL, T_PlayIVTEffectRepeatFromMultipleIVTDataSynchronous, Teardown)

// 1,33
EUNIT_TEST(
    "Play repeat from multiple IVT-data",
    "HapticsClient",
    "PlayIVTEffectRepeat",
    "FUNCTIONALITY",
    SetupL, T_PlayIVTEffectRepeatFromMultipleIVTDataAsynchronous, Teardown)

// 1,34
EUNIT_TEST(
    "Status conversions",
    "HapticsClient",
    "PlayIVTEffectRepeat",
    "FUNCTIONALITY",
    SetupL, T_VibeStatusToS60StatusConversions, Teardown)

// 1,48
EUNIT_TEST(
    "Reserve and release",
    "HapticsClient",
    "ReserveHapticsL",
    "FUNCTIONALITY",
    SetupL, T_ReserveAndReleaseHaptics, Teardown)

// 1,49
EUNIT_TEST(
    "Reserve several times",
    "HapticsClient",
    "ReserveHapticsL",
    "FUNCTIONALITY",
    SetupL, T_ReserveHapticsSeveralTimes, Teardown)

// 1,50
EUNIT_TEST(
    "Reserve, no release",
    "HapticsClient",
    "ReleaseHaptics",
    "FUNCTIONALITY",
    Dummy, T_ReserveHapticsNoReleaseL, Dummy)

// 1,51
EUNIT_TEST(
    "Release, no reserve",
    "HapticsClient",
    "ReleaseHaptics",
    "FUNCTIONALITY",
    SetupL, T_ReleaseHapticsWithoutReservingFirst, Teardown)

// 1,52
EUNIT_TEST(
    "Reserve forceNoCoe",
    "HapticsClient",
    "ReserveHapticsL",
    "FUNCTIONALITY",
    SetupL, T_ReserveNoAutomaticFocusNotTrusted, Teardown)

// 1,53
EUNIT_TEST(
    "Reserve reserved higher",
    "HapticsClient",
    "ReserveHapticsL",
    "FUNCTIONALITY",
    Dummy, T_ReserveAlreadyReservedHigherL, Dummy)

// 1,54
EUNIT_TEST(
    "Reserve reserved lower",
    "HapticsClient",
    "ReserveHapticsL",
    "FUNCTIONALITY",
    Dummy, T_ReserveAlreadyReservedLowerL, Dummy)

// 1,55
EUNIT_TEST(
    "Play when reserved",
    "HapticsClient",
    "ReserveHapticsL",
    "FUNCTIONALITY",
    SetupL, T_PlayWhenReservedL, Teardown)

// 1,41
EUNIT_TEST(
    "Get effect count",
    "HapticsClient",
    "GetIVTEffectCount",
    "FUNCTIONALITY",
    SetupL, T_GetIVTEffectCount, Teardown)

// 1,42
EUNIT_TEST(
    "Get effect duration",
    "HapticsClient",
    "GetIVTEffectDuration",
    "FUNCTIONALITY",
    SetupL, T_GetIVTEffectDuration, Teardown)

// 1,43
EUNIT_TEST(
    "Get effect index from name",
    "HapticsClient",
    "GetEffectIndexFromName",
    "FUNCTIONALITY",
    SetupL, T_GetIVTEffectIndexFromName, Teardown)

// 1,44
EUNIT_TEST(
    "Get effect type",
    "HapticsClient",
    "GetEffectType",
    "FUNCTIONALITY",
    SetupL, T_GetIVTEffectType, Teardown)

// 1,45
EUNIT_TEST(
    "Get effect name",
    "HapticsClient",
    "GetEffectName",
    "FUNCTIONALITY",
    SetupL, T_GetIVTEffectName, Teardown)

// 1,46
EUNIT_TEST(
    "Get MagSweep effect definition",
    "HapticsClient",
    "GetMagSweepEffectDefinition",
    "FUNCTIONALITY",
    SetupL, T_GetIVTMagSweepEffectDefinition, Teardown)

// 1,47
EUNIT_TEST(
    "Get Periodic effect definition",
    "HapticsClient",
    "GetPeriodicEffectDefinition",
    "FUNCTIONALITY",
    SetupL, T_GetIVTPeriodicEffectDefinition, Teardown)

// 1,56
EUNIT_TEST(
    "Create and destroy asynchronous",
    "HapticsClient",
    "NewL",
    "FUNCTIONALITY",
    Dummy, T_CreateAndDeleteAsynchronousL, Dummy)

// 1,57
EUNIT_TEST(
    "Several sessions asynchronous",
    "HapticsClient",
    "NewL",
    "FUNCTIONALITY",
    Dummy, T_SeveralSessionsAsynchronousL, Dummy)

// 1,58
EUNIT_TEST(
    "Consecutive sessions asynchronous",
    "HapticsClient",
    "NewL",
    "FUNCTIONALITY",
    Dummy, T_ConsecutiveSessionsAsynchronousL, Dummy)
    
// 1,59
EUNIT_TEST(
    "Constant getters",
    "HapticsClient",
    "InfiniteRepeat", // and others.. 
    "FUNCTIONALITY",
    SetupL, T_ConstantGetters, Teardown)

// 1,60
EUNIT_TEST(
    "Pause effect",
    "HapticsClient",
    "PausePlayingEffect",
    "FUNCTIONALITY",
    SetupL, T_PauseEffect, Teardown)

// 1,61
EUNIT_TEST(
    "Resume effect",
    "HapticsClient",
    "ResumePausedEffect",
    "FUNCTIONALITY",
    SetupL, T_ResumeEffect, Teardown)

// 1,62
EUNIT_TEST(
    "Stop effect",
    "HapticsClient",
    "StopPlayingEffect",
    "FUNCTIONALITY",
    SetupL, T_StopEffect, Teardown)

// 1,63
EUNIT_TEST(
    "Stop all effects",
    "HapticsClient",
    "StopAllPlayingEffects",
    "FUNCTIONALITY",
    SetupL, T_StopAllEffects, Teardown)

// 1,64
EUNIT_TEST(
    "Play, pause, resume, stop",
    "HapticsClient",
    "StopPlayingEffect",
    "FUNCTIONALITY",
    SetupL, T_PlayPauseResumeStop, Teardown)

// 1,65
EUNIT_TEST(
    "Modify playing magsweep effect synchronous",
    "HapticsClient",
    "ModifyPlayingMagSweepEffect",
    "FUNCTIONALITY",
    SetupL, T_ModifyMagSweepEffectSynchronous, Teardown)

// 1,66
EUNIT_TEST(
    "Modify playing magsweep effect asynchronous",
    "HapticsClient",
    "ModifyPlayingMagSweepEffect",
    "FUNCTIONALITY",
    SetupL, T_ModifyMagSweepEffectAsynchronous, Teardown)

// 1,67
EUNIT_TEST(
    "Modify playing periodic effect synchronous",
    "HapticsClient",
    "ModifyPlayingPeriodicEffect",
    "FUNCTIONALITY",
    SetupL, T_ModifyPeriodicEffectSynchronous, Teardown)

// 1,68
EUNIT_TEST(
    "Modify playing periodic effect asynchronous",
    "HapticsClient",
    "ModifyPlayingPeriodicEffect",
    "FUNCTIONALITY",
    SetupL, T_ModifyPeriodicEffectAsynchronous, Teardown)

// 1,69
EUNIT_TEST(
    "Get device property of TInt value",
    "HapticsClient",
    "GetDeviceProperty_TInt_TInt",
    "FUNCTIONALITY",
    SetupL, T_GetDeviceProperty_TInt_TInt, Teardown)
    
// 1,70
EUNIT_TEST(
    "Get device property of TDesC value",
    "HapticsClient",
    "GetDeviceProperty_TInt_TDesC",
    "FUNCTIONALITY",
    SetupL, T_GetDeviceProperty_TInt_TDesC, Teardown)

// 1,71
EUNIT_TEST(
    "Set device property of TInt value",
    "HapticsClient",
    "SetDeviceProperty_TInt_TInt",
    "FUNCTIONALITY",
    SetupL, T_SetDeviceProperty_TInt_TInt, Teardown)

// 1,72
EUNIT_TEST(
    "Set device property of TDesC value",
    "HapticsClient",
    "SetDeviceProperty_TInt_TDesC",
    "FUNCTIONALITY",
    SetupL, T_SetDeviceProperty_TInt_TDesC, Teardown)

// 1,73    
EUNIT_TEST(
    "Get device capability of TInt value",
    "HapticsClient",
    "GetDeviceCapability_TInt_TInt",
    "FUNCTIONALITY",
    SetupL, T_GetDeviceCapability_TInt_TInt, Teardown)
    
// 1,74    
EUNIT_TEST(
    "Get device capability of TDesC value",
    "HapticsClient",
    "GetDeviceCapability_TInt_TDesC",
    "FUNCTIONALITY",
    SetupL, T_GetDeviceCapability_TInt_TDesC, Teardown)

// 1,75
EUNIT_TEST(
    "Fetch status",
    "HapticsClient",
    "HapticsStatus",
    "FUNCTIONALITY",
    SetupL, T_GetHapticsStatus, Teardown)

// 1,76
EUNIT_TEST(
    "Observe status",
    "HapticsClient",
    "HapticsStatus",
    "FUNCTIONALITY",
    Dummy, T_ObserveHapticsStatus, Dummy)

// 1,77
EUNIT_TEST(
    "Status, reservation",
    "HapticsClient",
    "HapticsStatus",
    "FUNCTIONALITY",
    Dummy, T_GetHapticsStatusWithReservations, Dummy)

// 1,78
EUNIT_TEST(
    "Status, observe, reservation",
    "HapticsClient",
    "HapticsStatus",
    "FUNCTIONALITY",
    Dummy, T_ObserveHapticsStatusWithReservations, Dummy)

// 1,79
EUNIT_TEST(
    "Status, 2 reservations",
    "HapticsClient",
    "HapticsStatus",
    "FUNCTIONALITY",
    Dummy, T_GetHapticsStatusWithSeveralReservations, Dummy)

// 1,80
EUNIT_TEST(
    "Status, observe, 2 reservation",
    "HapticsClient",
    "HapticsStatus",
    "FUNCTIONALITY",
    Dummy, T_ObserveHapticsStatusWithSeveralReservations, Dummy)

// 1,81
EUNIT_TEST(
    "Status, many clients",
    "HapticsClient",
    "HapticsStatus",
    "FUNCTIONALITY",
    Dummy, T_GetHapticsStatusWithSeveralClientsOneReservation, Dummy)

// 1,82
EUNIT_TEST(
    "Status, observer, many clients",
    "HapticsClient",
    "HapticsStatus",
    "FUNCTIONALITY",
    Dummy, T_ObserveHapticsStatusWithSeveralClientsOneReservations, Dummy)


// 1,83
EUNIT_TEST(
    "Create streaming effect",
    "HapticsClient",
    "CreateStreamingEffect",
    "FUNCTIONALITY",
    SetupL, T_CreateStreamingEffect, Teardown)
    
// 1,84
EUNIT_TEST(
    "Play streaming sample",
    "HapticsClient",
    "PlayStreamingSample",
    "FUNCTIONALITY",
    SetupL, T_PlayStreamingSample, Teardown)
    
// 1,85
EUNIT_TEST(
    "Play streaming sample, async",
    "HapticsClient",
    "PlayStreamingSample",
    "FUNCTIONALITY",
    SetupL, T_PlayStreamingSampleAsync, Teardown)
    
// 1,86
EUNIT_TEST(
    "Play streaming sample with offset",
    "HapticsClient",
    "PlayStreamingSampleWithOffset",
    "FUNCTIONALITY",
    SetupL, T_PlayStreamingSampleWithOffset, Teardown)
    
// 1,87
EUNIT_TEST(
    "Play streaming sample with offset, async",
    "HapticsClient",
    "PlayStreamingSampleWithOffset",
    "FUNCTIONALITY",
    SetupL, T_PlayStreamingSampleWithOffsetAsync, Teardown)
    
// 1,88
EUNIT_TEST(
    "Destroy streaming effect",
    "HapticsClient",
    "DestroyStreamingEffect",
    "FUNCTIONALITY",
    SetupL, T_DestroyStreamingEffect, Teardown)
    
// 1,89
EUNIT_TEST(
    "Actuator status",
    "HapticsClient",
    "ActuatorStatus",
    "FUNCTIONALITY",
    SetupL, T_ObserveActuatorStatus, Teardown)
    
// 1,89
EUNIT_TEST(
    "Actuator/Haptics status",
    "HapticsClient",
    "ActuatorStatus",
    "FUNCTIONALITY",
    SetupL, T_ObserveActuatorStatusAndHapticsStatus, Teardown)

// 1,90
EUNIT_TEST(
    "Plugin Manager Timer Expires",
    "HapticsClient",
    "ActuatorStatus",
    "FUNCTIONALITY",
    SetupL, T_PluginMgrTransTimerExpires, Teardown)
    
    
#if 0
// Copy this as basis for new ones.. 
EUNIT_TEST(
    "",
    "HapticsClient",
    "",
    "FUNCTIONALITY",
    SetupL, Dummy, Teardown)
#endif

        
EUNIT_END_TEST_TABLE
