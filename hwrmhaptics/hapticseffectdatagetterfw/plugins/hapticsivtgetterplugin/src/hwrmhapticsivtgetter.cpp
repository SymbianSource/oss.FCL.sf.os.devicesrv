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
* Description:  Plugin for reading effect information from IVT effect data.
*
*/

#include <ecom/implementationproxy.h>

#include <stdlib.h>
#include <string.h>
#include <hwrmhaptics.h>

#include "hwrmhapticsivtgetter.h"
#include "hwrmhapticstrace.h"
#include "ivtinternal.h"
#include "ivthelper.h"

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsIVTGetter::CHWRMHapticsIVTGetter()
    {
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsIVTGetter::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// 2-phased constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsIVTGetter* CHWRMHapticsIVTGetter::NewL()
    {
    CHWRMHapticsIVTGetter* self = new ( ELeave ) CHWRMHapticsIVTGetter();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsIVTGetter::~CHWRMHapticsIVTGetter()
    {
    }
    
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//    
TInt CHWRMHapticsIVTGetter::GetEffectCount( const TDesC8& aData, 
                                            TInt& aCount  )
    {
    TInt status( KErrArgument );
    if ( IVTHelper::IsValidIVTAddress( aData.Ptr() ) )
        {
        aCount = IVTHelper::GetNumEffects( aData.Ptr() );        
        status = KErrNone;
        }
    return status;
    }
    
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
TInt CHWRMHapticsIVTGetter::GetEffectName(  const TDesC8& aData,
                                            TInt aEffectIndex, 
                                            TDes8& aEffectName )
    {
    TUint8* effectNameData( 0 );
    char effectNameTemp[KVibeMaxEffectNameLength];
    memset( effectNameTemp, 0, 
            KVibeMaxEffectNameLength * sizeof( char ) );
    TInt status( KErrNone );

    if ( IVTHelper::IsValidIVTAddress( aData.Ptr() ) &&
         0 <= aEffectIndex && 
         aEffectIndex < IVTHelper::GetNumEffects( aData.Ptr() ) )
        {
        effectNameData = IVTHelper::GetEffectNameData( aData.Ptr(), aEffectIndex );
        if ( effectNameData )
            {
            // Convert the name from wchar (VibeWChar) into the 
            // temporary char format buffer first
            TInt stringSize = 
                wcstombs( effectNameTemp, 
                          reinterpret_cast<const wchar_t*>( effectNameData ), 
                          KVibeMaxEffectNameLength );                          
            if ( aData.Size() < stringSize )
                {
                status = KErrArgument;
                }
            else
                {
                aEffectName.Copy( reinterpret_cast<TUint8*>( effectNameTemp ),
                                  stringSize );
                }    
            }
        else
            {
            // IVT data doesn't contain effect name block
            // so return empty string
            aEffectName.Copy( KNullDesC );
            }
        }
    else    
        {
        COMPONENT_TRACE( _L("ImmVibeGetIVTEffectName: invalid args.") );
        status = KErrArgument;
        }
    return status;
    }

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
TInt CHWRMHapticsIVTGetter::GetEffectIndexFromName( const TDesC8& aData,
                                                    const TDesC8& aEffectName,
                                                    TInt& aEffectIndex )
    {
    TInt i( 0 );
    TInt numEffects( 0 );
    TInt status( KErrGeneral );

    if ( IVTHelper::IsValidIVTAddress( aData.Ptr() ) )
        {
        HBufC8* temp = HBufC8::New( aEffectName.Size() + 1 );
        if ( temp )
            {
            TPtr8 effectName = temp->Des();
            effectName.Copy( aEffectName );
            effectName.ZeroTerminate();
        
            aEffectIndex = KVibeInvalidIndex;
            numEffects = IVTHelper::GetNumEffects( aData.Ptr() );
            for (i = 0; i < numEffects; i++)
                {
                TUint8 *effectNameData = IVTHelper::GetEffectNameData( aData.Ptr(), i );
                if ( effectNameData )
                    {
                    // Convert the name from wchar (VibeWChar) into char format
                    char effectNameFromIVT[KVibeMaxEffectNameLength];
                    memset( effectNameFromIVT, 0, 
                            KVibeMaxEffectNameLength * sizeof( char ) );
                    wcstombs( effectNameFromIVT, 
                              reinterpret_cast<const wchar_t*>( effectNameData ),
                              KVibeMaxEffectNameLength );
                              
                    if ( !strcmp( reinterpret_cast<const char*>
                            ( effectName.Ptr() ), effectNameFromIVT ) )
                        {
                        // Match found, break from the for loop
                        aEffectIndex = i;
                        i = numEffects;
                        status = KErrNone;
                        }
                    }
                }       
            delete temp;
            temp = NULL;
            }
        }
    else
        {
        COMPONENT_TRACE( _L("ImmVibeGetIVTEffectIndexFromName: invalid args.") );
        status = KErrArgument;   
        }
    return status;
    }

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
TInt CHWRMHapticsIVTGetter::GetEffectType( const TDesC8& aData, 
                                           TInt aEffectIndex, 
                                           TInt& aEffectType )
    {
    TUint8 *effectPtr( 0 );
    TInt status( KErrNone );

    if ( IVTHelper::IsValidIVTAddress( aData.Ptr() ) &&
         0 <= aEffectIndex && 
         aEffectIndex < IVTHelper::GetNumEffects( aData.Ptr() ) )
        {
        aEffectType = 0;
        effectPtr = IVTHelper::GetEffectStorage( aData.Ptr(), aEffectIndex );
        if ( effectPtr )
            {
            // Skip over periodic modifier, if any
            if ( KVibeMessageTypeSetPeriodicModifier == 
                 VIBE_IVT_GET_MESSAGE_TYPE( effectPtr ) )
                {
                effectPtr += KVibeIvtMessageSize;
                }
            if ( KVibeMessageTypeSetPeriodic ==
                 VIBE_IVT_GET_MESSAGE_TYPE( effectPtr ) )
                {
                // It is either MagSweep or Periodic effect
                aEffectType = KVibePeriodicTypeConstant == 
                               VIBE_IVT_GET_PERIODIC_TYPE( effectPtr ) ?
                                CHWRMHaptics::EHWRMHapticsTypeMagSweep :
                                CHWRMHaptics::EHWRMHapticsTypePeriodic;
                }
            else
                {
                // It has to be timeline effect
                aEffectType = CHWRMHaptics::EHWRMHapticsTypeTimeline;    
                }   
            }
        else
            {
            COMPONENT_TRACE( _L("ImmVibeGetIVTEffectType: GetEffectStorage failed.") );
            status = KErrGeneral;
            }
        }
    else    
        {
        COMPONENT_TRACE( _L("ImmVibeGetIVTEffectType: invalid args.") );
        status = KErrArgument;
        }

    return status;
    }

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
TInt CHWRMHapticsIVTGetter::GetMagSweepEffectDefinition( 
                      const TDesC8& aData, 
                      TInt aEffectIndex, 
                      CHWRMHaptics::THWRMHapticsMagSweepEffect& aEffect )
    {                                 
    TVibeEffectDefinition effectDefinition;
    TInt effectType( 0 );
    TInt status ( KErrNone );

    memset( &effectDefinition, 0, sizeof( effectDefinition ) );

    // Validity of aEffectIndex is already done in ImmVibeGetIVTEffectType
    // For params NULL values just mean "not to be returned"
    if ( IVTHelper::IsValidIVTAddress( aData.Ptr() ) )
        {
        status = GetEffectType( aData, aEffectIndex, effectType );
        if ( KErrNone == status )
            {
            if ( CHWRMHaptics::EHWRMHapticsTypeMagSweep == effectType )
                {
                status = 
                    GetEffectByIndex( aData, 
                                      aEffectIndex, 
                                      &effectDefinition );
                if ( KErrNone == status )
                    {
                    aEffect.iDuration = 
                            effectDefinition.iEffectCommonParams.iDuration;

                    aEffect.iMagnitude = 
                            effectDefinition.iEffectTypeParams.
                                iMagSweepEffectDefinition.iMagnitude;

                    aEffect.iStyle = 
                            effectDefinition.iEffectCommonParams.iControlMode;

                    aEffect.iAttackTime = 
                            effectDefinition.iEffectTypeParams.
                                iMagSweepEffectDefinition.iEnvelope.
                                    iAttackTime;

                    aEffect.iAttackLevel = 
                            effectDefinition.iEffectTypeParams.
                                iMagSweepEffectDefinition.iEnvelope.
                                    iAttackLevel;

                    aEffect.iFadeTime =
                            effectDefinition.iEffectTypeParams.
                                iMagSweepEffectDefinition.iEnvelope.iFadeTime;

                    aEffect.iFadeLevel = 
                            effectDefinition.iEffectTypeParams.
                                iMagSweepEffectDefinition.iEnvelope.
                                    iFadeLevel;
                    }
                else
                    {
                    COMPONENT_TRACE( _L("ImmVibeGetIVTMagSweepEffectDefinition: VibeAPIInternalGetIVTEffectByIndex failed.") );
                    }    
                }
            else
                {
                COMPONENT_TRACE( _L("ImmVibeGetIVTMagSweepEffectDefinition: EffectType invalid.") );
                status = KErrNotSupported;
                }
            }
        else
            {
            COMPONENT_TRACE( _L("ImmVibeGetIVTMagSweepEffectDefinition: ImmVibeGetIVTEffectType failed.") );
            }
        }
    else
        {
        COMPONENT_TRACE( _L("ImmVibeGetIVTMagSweepEffectDefinition: invalid data buffer.") );
        status = KErrArgument;
        }
    return status;
    }

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
TInt CHWRMHapticsIVTGetter::GetPeriodicEffectDefinition( 
                      const TDesC8& aData, 
                      TInt aEffectIndex, 
                      CHWRMHaptics::THWRMHapticsPeriodicEffect& aEffect )
    {
    TVibeEffectDefinition effectDefinition;
    TInt effectType( 0 );
    TInt status( KErrNone );

    memset( &effectDefinition, 0, sizeof( effectDefinition ) );

    // Validity of aEffectIndex is already done in ImmVibeGetIVTEffectType
    // For params NULL values just mean "not to be returned"
    if ( IVTHelper::IsValidIVTAddress( aData.Ptr() ) )
        {
        status = GetEffectType( aData, aEffectIndex, effectType );
        if ( KErrNone == status )
            {
            if ( CHWRMHaptics::EHWRMHapticsTypePeriodic == effectType )
                {
                status = 
                    GetEffectByIndex( aData, 
                                      aEffectIndex, 
                                      &effectDefinition );
                if ( KErrNone == status )
                    {
                    aEffect.iDuration = 
                            effectDefinition.iEffectCommonParams.iDuration;

                    aEffect.iMagnitude = 
                            effectDefinition.iEffectTypeParams.
                                iPeriodicEffectDefinition.iMagnitude;

                    aEffect.iPeriod = 
                            effectDefinition.iEffectTypeParams.
                                iPeriodicEffectDefinition.iPeriod;

                    aEffect.iStyle = 
                            (effectDefinition.iEffectCommonParams.iControlMode
                             & KVibeStyleMask) | 
                            ( ( effectDefinition.iEffectTypeParams.
                                iPeriodicEffectDefinition.iType
                                << KVibeWavetypeShift ) & KVibeWavetypeMask );

                    aEffect.iAttackTime =
                            effectDefinition.iEffectTypeParams.
                                iPeriodicEffectDefinition.iEnvelope.
                                    iAttackTime;
 
                    aEffect.iAttackLevel = 
                            effectDefinition.iEffectTypeParams.
                                iPeriodicEffectDefinition.iEnvelope.
                                    iAttackLevel;

                    aEffect.iFadeTime = 
                            effectDefinition.iEffectTypeParams.
                                iPeriodicEffectDefinition.iEnvelope.iFadeTime;

                    aEffect.iFadeLevel = 
                            effectDefinition.iEffectTypeParams.
                                iPeriodicEffectDefinition.iEnvelope.
                                    iFadeLevel;
                    }
                else
                    {
                    COMPONENT_TRACE( _L("ImmVibeGetIVTPeriodicEffectDefinition: VibeAPIInternalGetIVTEffectByIndex failed.") );    
                    }    
                }
            else
                {
                COMPONENT_TRACE( _L("ImmVibeGetIVTPeriodicEffectDefinition: EffectType invalid.") );
                status = KErrNotSupported;
                }    
            }
        else
            {
            COMPONENT_TRACE( _L("ImmVibeGetIVTPeriodicEffectDefinition: ImmVibeGetIVTEffectType failed.") );
            }    
        }
    else
        {
        COMPONENT_TRACE( _L("ImmVibeGetIVTPeriodicEffectDefinition: pIVT invalid.") );
        status = KErrArgument;
        }
    return status;
    }

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
TInt CHWRMHapticsIVTGetter::GetEffectDuration( const TDesC8& aData, 
                                               TInt aEffectIndex, 
                                               TInt& aEffectDuration )
    {
    TInt status( KErrNone );

    if ( IVTHelper::IsValidIVTAddress( aData.Ptr() ) &&
         0 <= aEffectIndex && 
         aEffectIndex < IVTHelper::GetNumEffects( aData.Ptr() ) )
        {
        aEffectDuration = 0;
        TInt effectCount = 
            IVTHelper::GetIVTEffectDuration( aData.Ptr(), aEffectIndex );
        if ( effectCount < 0 )
            {
            COMPONENT_TRACE( _L("ImmVibeGetIVTEffectDuration failed.") );
            aEffectDuration = 0;
            status = effectCount;
            }
        else
            {
            // On success, GetIVTEffectDuration returns the duration
            aEffectDuration = effectCount;
            }
        }
    else 
        {
        COMPONENT_TRACE( _L("ImmVibeGetIVTEffectDuration: invalid args.") );
        status = KErrArgument;
        }
            
    return status;
    }
                            
                         
// ---------------------------------------------------------------------------
// Internal helper method for getting effect definition by its index 
// from the effect data buffer.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsIVTGetter::GetEffectByIndex( 
                                      const TDesC8& aData,  
                                      TInt aEffectIndex,  
                                      TVibeEffectDefinition* aEffectDefinition )
    {   
    TInt periodicType;
    TUint8* effectPtr;
    TVibeEnvelopeParams envelope;
    TInt status( KErrNone );

    memset( &envelope, 0, sizeof( envelope ) );

    if ( aEffectDefinition && IVTHelper::IsValidIVTAddress( aData.Ptr() ) && 
         0 <= aEffectIndex && 
         aEffectIndex < IVTHelper::GetNumEffects( aData.Ptr() ) )
        {
        memset( reinterpret_cast<void*>( aEffectDefinition ), 
                0, 
                sizeof( TVibeEffectDefinition ) );
        effectPtr = IVTHelper::GetEffectStorage( aData.Ptr(), aEffectIndex );
        if ( effectPtr )
            {
            // Get IVT basis effect envelope
            if ( KVibeMessageTypeSetPeriodicModifier == 
                 VIBE_IVT_GET_MESSAGE_TYPE( effectPtr ) )
                {
                IVTHelper::ExtractEnvelopeFromPacket( effectPtr, &envelope );
                effectPtr += KVibeIvtMessageSize;
                }
            if ( KVibeMessageTypeSetPeriodic == 
                 VIBE_IVT_GET_MESSAGE_TYPE( effectPtr ) )
                {
                // Get IVT basis effect definition excluding envelope
                IVTHelper::ExtractEffectDefinitionFromPacket( 
                    effectPtr, 
                    aEffectDefinition, 
                    &periodicType );
                if ( KVibePeriodicTypeConstant == periodicType )
                    {
                    memcpy( &( aEffectDefinition->iEffectTypeParams.
                                    iMagSweepEffectDefinition.iEnvelope ),
                            &envelope, sizeof( envelope ) );
                    }
                else
                    {
                    memcpy( &( aEffectDefinition->iEffectTypeParams.
                                    iPeriodicEffectDefinition.iEnvelope ),
                            &envelope, sizeof( envelope ) );

                    }
                }
            else
                {
                COMPONENT_TRACE( _L("VibeAPIInternalGetIVTEffectByIndex: Invalid vibe message type.") );
                status = KErrGeneral;   
                }    
            }
        else
            {
            COMPONENT_TRACE( _L("VibeAPIInternalGetIVTEffectByIndex: GetEffectStorage failed.") );
            status = KErrGeneral;
            }    
        }
    else
        {
        COMPONENT_TRACE( _L("VibeAPIInternalGetIVTEffectByIndex: invalid arguments.") ); 
        status = KErrArgument;
        }
        
    return status;
    }
                            
                                

//---------------------------------------------------------------------------
// ImplementationTable[]
//
//---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x20021203, CHWRMHapticsIVTGetter::NewL )
    };

//---------------------------------------------------------------------------
// TImplementationProxy* ImplementationGroupProxy()
//
//---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
   
// End of file
