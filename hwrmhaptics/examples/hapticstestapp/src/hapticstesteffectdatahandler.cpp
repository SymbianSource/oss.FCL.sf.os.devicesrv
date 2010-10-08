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
* Description:  Active object listening waiting for async haptics completion.
*
*/


#include <eikmenup.h> 
#include <eikon.hrh>

#include "hapticstest.hrh"
#include "hapticstesteffectdatahandler.h"


_LIT( KManualEffectName, "manual" );

const TInt KEffectModificationInterval = 50000; // microseconds
const TInt KMaxEffectHistorySize = 25;
const TInt KDefaultNonInfiniteDuration = 1000; // milliseconds

// default values for manual effects
const TInt KDefaultEffectMagnitude = KHWRMHapticsMaxMagnitude;
const TInt KDefaultEffectAttackLevel = KHWRMHapticsMaxMagnitude * 0.5;
const TInt KDefaultEffectFadeLevel = KHWRMHapticsMaxMagnitude * 0.5;
const TInt KDefaultEffectAttackTime = 200; // milliseconds
const TInt KDefaultEffectFadeTime = 200; // milliseconds
const TInt KDefaultEffectPeriod = 300; // milliseconds
const CHWRMHaptics::THWRMHapticsEffectStyles KDefaultEffectStyle = CHWRMHaptics::EHWRMHapticsStyleStrong;


// ---------------------------------------------------------------------------
// Symbian two-phased constructor.
// ---------------------------------------------------------------------------
//
CHapticsTestEffectDataHandler* CHapticsTestEffectDataHandler::NewL()
    {
    CHapticsTestEffectDataHandler* self = 
        CHapticsTestEffectDataHandler::NewLC();

    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// Symbian two-phased constructor.
// ---------------------------------------------------------------------------
//
CHapticsTestEffectDataHandler* CHapticsTestEffectDataHandler::NewLC()
    {
    CHapticsTestEffectDataHandler* self =
         new ( ELeave ) CHapticsTestEffectDataHandler();
    CleanupStack::PushL( self );
    
    self->ConstructL();
    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHapticsTestEffectDataHandler::~CHapticsTestEffectDataHandler()
    {
    ResetModifiableEffectTimer();

    if ( iModifyEffect )
        {
        delete iModifyEffect;
        iModifyEffect = NULL;
        }

    iPlayedEffectArray.Close();
    }

// ---------------------------------------------------------------------------
// Adds effect data to effect history array.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::AddEffectInfo( TInt aEffectHandle, 
                                                   TInt aFileHandle,
                                                   TInt aIndex )
    {
    TPlayedEffect newEffect;
    newEffect.iEffectHandle = aEffectHandle;
    newEffect.iFileHandle = aFileHandle;
    newEffect.iEffectIndex = aIndex;

    // add item to the beginning of the array, ignore possible error
    iPlayedEffectArray.Insert( newEffect, 0 );
    
    // if there are too many items, remove the last one
    TInt count = iPlayedEffectArray.Count();
    if ( count > KMaxEffectHistorySize )
        {
        iPlayedEffectArray.Remove( count - 1 );
        }
    }

// ---------------------------------------------------------------------------
// Deletest all effect info items from the list that have
// the given filehandle in their data.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::DeleteEffectInfo( TInt aFileHandle )
    {
    for ( TInt i = iPlayedEffectArray.Count() - 1; i >= 0 ; --i )
        {
        if ( iPlayedEffectArray[i].iFileHandle == aFileHandle )
            {
            iPlayedEffectArray.Remove( i );
            }
        }
    }

// ---------------------------------------------------------------------------
// Returns the amount of effect data stored in the history data array.
// ---------------------------------------------------------------------------
//
TInt CHapticsTestEffectDataHandler::Count() const
    {
    return iPlayedEffectArray.Count();
    }

// ---------------------------------------------------------------------------
// Returns the effect handle of the given effect data item.
// ---------------------------------------------------------------------------
//
TInt CHapticsTestEffectDataHandler::EffectHandle( TInt aIndex ) const
    {
    return iPlayedEffectArray[aIndex].iEffectHandle;
    }

// ---------------------------------------------------------------------------
// Returns the file handle of the given effect data item.
// ---------------------------------------------------------------------------
//
TInt CHapticsTestEffectDataHandler::FileHandle( TInt aIndex ) const
    {
    return iPlayedEffectArray[aIndex].iFileHandle;
    }

// ---------------------------------------------------------------------------
// Returns the effect index of the given effect data item.
// ---------------------------------------------------------------------------
//
TInt CHapticsTestEffectDataHandler::EffectIndex( TInt aIndex ) const
    {
    return iPlayedEffectArray[aIndex].iEffectIndex;
    }

// ---------------------------------------------------------------------------
// Resets the modifiable effect data with the data received in the
// given magsweep effect.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::ResetModifiableMagSweepEffectL( 
                const CHWRMHaptics::THWRMHapticsMagSweepEffect& aMagSweep,
                TInt aEffectHandle, CHWRMHaptics* aHaptics )
    {
    // reset effect data
    if ( iModifyEffect )
        {
        delete iModifyEffect;
        iModifyEffect = NULL;
        }

    // create new effect data
    iModifyEffect = new (ELeave) TModifyEffect();
    iModifyEffect->iHaptics = aHaptics;
    iModifyEffect->iEffectType = CHWRMHaptics::EHWRMHapticsTypeMagSweep;
    iModifyEffect->iEffectHandle = aEffectHandle;
    iModifyEffect->iModifyCount = 0;

    // store default effect values
    iModifyEffect->iDuration = aMagSweep.iDuration;
    iModifyEffect->iMagnitude = aMagSweep.iMagnitude;
    iModifyEffect->iStyle = aMagSweep.iStyle;
    iModifyEffect->iAttackLevel = aMagSweep.iAttackLevel;
    iModifyEffect->iFadeLevel = aMagSweep.iFadeLevel;
    iModifyEffect->iAttackTime = aMagSweep.iAttackTime;
    iModifyEffect->iFadeTime = aMagSweep.iFadeTime;
    
    // no period for magsweep, use default value
    iModifyEffect->iPeriod = KDefaultEffectPeriod;
    }

// ---------------------------------------------------------------------------
// Resets the modifiable effect data with the data received in the
// given periodic effect.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::ResetModifiablePeriodicEffectL( 
                const CHWRMHaptics::THWRMHapticsPeriodicEffect& aPeriodic,
                TInt aEffectHandle, CHWRMHaptics* aHaptics )
    {
    // reset effect data
    if ( iModifyEffect )
        {
        delete iModifyEffect;
        iModifyEffect = NULL;
        }

    // create new effect data
    iModifyEffect = new (ELeave) TModifyEffect();
    iModifyEffect->iHaptics = aHaptics;
    iModifyEffect->iEffectType = CHWRMHaptics::EHWRMHapticsTypePeriodic;
    iModifyEffect->iEffectHandle = aEffectHandle;
    iModifyEffect->iModifyCount = 0;

    // store default effect values
    iModifyEffect->iDuration = aPeriodic.iDuration;
    iModifyEffect->iMagnitude = aPeriodic.iMagnitude;
    iModifyEffect->iStyle = aPeriodic.iStyle;
    iModifyEffect->iAttackLevel = aPeriodic.iAttackLevel;
    iModifyEffect->iFadeLevel = aPeriodic.iFadeLevel;
    iModifyEffect->iAttackTime = aPeriodic.iAttackTime;
    iModifyEffect->iFadeTime = aPeriodic.iFadeTime;
    iModifyEffect->iPeriod = aPeriodic.iPeriod;
    }

// ---------------------------------------------------------------------------
// Starts the timer for constantly modifying an effect.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::StartModifiableEffectTimerL( TInt (*aFunction)(TAny* aPtr) )
    {
    ResetModifiableEffectTimer();

    // create and start periodic timer
    iModifyTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iModifyTimer->Start( KEffectModificationInterval,
                         KEffectModificationInterval,
                         TCallBack( aFunction, this ) );
    }

// ---------------------------------------------------------------------------
// Cancels and deletes the timer.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::ResetModifiableEffectTimer()
    {
    // reset timer
    if ( iModifyTimer )
        {
        iModifyTimer->Cancel();
        delete iModifyTimer;
        iModifyTimer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Returns whether or not a modifiable effect exists.
// ---------------------------------------------------------------------------
//
TBool CHapticsTestEffectDataHandler::ModifiableEffectExists() const
    {
    return iModifyEffect ? ETrue : EFalse;
    }

// ---------------------------------------------------------------------------
// Returns whether or not the currently stored effect is a magsweep effect.
// ---------------------------------------------------------------------------
//
TBool CHapticsTestEffectDataHandler::ModifiableEffectIsMagSweep() const
    {
    return ( iModifyEffect->iEffectType == CHWRMHaptics::EHWRMHapticsTypeMagSweep );
    }

// ---------------------------------------------------------------------------
// Returns whether or not the currently stored effect is a periodic effect.
// ---------------------------------------------------------------------------
//
TBool CHapticsTestEffectDataHandler::ModifiableEffectIsPeriodic() const
    {
    return ( iModifyEffect->iEffectType == CHWRMHaptics::EHWRMHapticsTypePeriodic );
    }

// ---------------------------------------------------------------------------
// Returns the effect handle of the modifiable effect.
// ---------------------------------------------------------------------------
//
TInt CHapticsTestEffectDataHandler::ModifiableEffectHandle() const
    {
    return iModifyEffect->iEffectHandle;
    }

// ---------------------------------------------------------------------------
// Returns the amount of times the current modifiable effect
// has been modified already.
// ---------------------------------------------------------------------------
//
TInt CHapticsTestEffectDataHandler::ModifyCount() const
    {
    return iModifyEffect->iModifyCount;
    }

// ---------------------------------------------------------------------------
// Returns the pointer to the haptics client instance, which
// has been set to the effect modification data.
// ---------------------------------------------------------------------------
//
CHWRMHaptics* CHapticsTestEffectDataHandler::Haptics() const
    {
    return iModifyEffect->iHaptics;
    }

// ---------------------------------------------------------------------------
// Fills in default magsweep effect data into the given structure.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::FillDefaultMagSweepData( 
                    CHWRMHaptics::THWRMHapticsMagSweepEffect& aMagSweep,
                    TBool aInfinite, TBool aAutoModifiable,
                    CHWRMHaptics* aHaptics ) const
    {
    if ( aInfinite )
        {
        aMagSweep.iDuration = aHaptics->InfiniteDuration();
        }
    else
        {
        aMagSweep.iDuration = KDefaultNonInfiniteDuration;
        }
    
    if ( aAutoModifiable )
        {
        // start modifiable effect from minimum magnitude
        aMagSweep.iMagnitude = KHWRMHapticsMinMagnitude;
        aMagSweep.iAttackLevel = KHWRMHapticsMinMagnitude;
        aMagSweep.iFadeLevel = KHWRMHapticsMinMagnitude;
        }
    else
        {
        // non-modifiable effect
        aMagSweep.iMagnitude = KDefaultEffectMagnitude;
        aMagSweep.iAttackLevel = KDefaultEffectAttackLevel;
        aMagSweep.iFadeLevel = KDefaultEffectFadeLevel;
        }

    aMagSweep.iStyle = KDefaultEffectStyle;
    aMagSweep.iAttackTime = KDefaultEffectAttackTime;
    aMagSweep.iFadeTime = KDefaultEffectFadeTime;
    }

// ---------------------------------------------------------------------------
// Fills in default periodic effect data into the given structure.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::FillDefaultPeriodicData( 
                    CHWRMHaptics::THWRMHapticsPeriodicEffect& aPeriodic,
                    TBool aInfinite, TBool /*aAutoModifiable*/,
                    CHWRMHaptics* aHaptics ) const
    {
    if ( aInfinite )
        {
        aPeriodic.iDuration = aHaptics->InfiniteDuration();
        }
    else
        {
        aPeriodic.iDuration = KDefaultNonInfiniteDuration;
        }
    
    aPeriodic.iMagnitude = KDefaultEffectMagnitude;
    aPeriodic.iAttackLevel = KDefaultEffectAttackLevel;
    aPeriodic.iFadeLevel = KDefaultEffectFadeLevel;

    aPeriodic.iPeriod = KDefaultEffectPeriod;
    aPeriodic.iStyle = KDefaultEffectStyle;
    aPeriodic.iAttackTime = KDefaultEffectAttackTime;
    aPeriodic.iFadeTime = KDefaultEffectFadeTime;
    }

// ---------------------------------------------------------------------------
// Fills in the modifiable effect data into the given magsweep structure.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::FillModifiableMagSweepData(
            CHWRMHaptics::THWRMHapticsMagSweepEffect& aMagSweep ) const
    {
    aMagSweep.iDuration = iModifyEffect->iDuration;
    aMagSweep.iStyle = iModifyEffect->iStyle;
    aMagSweep.iAttackTime = iModifyEffect->iAttackTime;
    aMagSweep.iFadeTime = iModifyEffect->iFadeTime;
    aMagSweep.iMagnitude = iModifyEffect->iMagnitude;
    aMagSweep.iAttackLevel = iModifyEffect->iAttackLevel;
    aMagSweep.iFadeLevel = iModifyEffect->iFadeLevel;
    }

// ---------------------------------------------------------------------------
// Fills in the modifiable effect data into the given periodic structure.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::FillModifiablePeriodicData( 
            CHWRMHaptics::THWRMHapticsPeriodicEffect& aPeriodic ) const
    {
    aPeriodic.iDuration = iModifyEffect->iDuration;
    aPeriodic.iStyle = iModifyEffect->iStyle;
    aPeriodic.iAttackTime = iModifyEffect->iAttackTime;
    aPeriodic.iFadeTime = iModifyEffect->iFadeTime;
    aPeriodic.iMagnitude = iModifyEffect->iMagnitude;
    aPeriodic.iAttackLevel = iModifyEffect->iAttackLevel;
    aPeriodic.iFadeLevel = iModifyEffect->iFadeLevel;
    aPeriodic.iPeriod = iModifyEffect->iPeriod;
    }

// ---------------------------------------------------------------------------
// Sets the modifiable effect's modify count.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::SetModifyCount( TInt aModifyCount )
    {
    iModifyEffect->iModifyCount = aModifyCount;
    }

// ---------------------------------------------------------------------------
// Sets the modifiable effect's duration value.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::SetDuration( TInt aDuration )
    {
    iModifyEffect->iDuration = aDuration;
    }

// ---------------------------------------------------------------------------
// Sets the modifiable effect's magnitude value.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::SetMagnitude( TInt aMagnitude )
    {
    iModifyEffect->iMagnitude = aMagnitude;
    }

// ---------------------------------------------------------------------------
// Sets the modifiable effect's style value.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::SetStyle( 
                        CHWRMHaptics::THWRMHapticsEffectStyles aStyle )
    {
    iModifyEffect->iStyle = aStyle;
    }

// ---------------------------------------------------------------------------
// Sets the modifiable effect's attack level value.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::SetAttackLevel( TInt aAttackLevel )
    {
    iModifyEffect->iAttackLevel = aAttackLevel;
    }

// ---------------------------------------------------------------------------
// Sets the modifiable effect's fade level value.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::SetFadeLevel( TInt aFadeLevel )
    {
    iModifyEffect->iFadeLevel = aFadeLevel;
    }

// ---------------------------------------------------------------------------
// Sets the modifiable effect's attack time value.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::SetAttackTime( TInt aAttackTime )
    {
    iModifyEffect->iAttackTime = aAttackTime;
    }

// ---------------------------------------------------------------------------
// Sets the modifiable effect's fade time value.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::SetFadeTime( TInt aFadeTime )
    {
    iModifyEffect->iFadeTime = aFadeTime;
    }

// ---------------------------------------------------------------------------
// Sets the modifiable effect's period value.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::SetPeriod( TInt aPeriod )
    {
    iModifyEffect->iPeriod = aPeriod;
    }

// ---------------------------------------------------------------------------
// Creates all submenu items for a change effect state submenu.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::DynInitChangeEffectStateSubmenuL( 
                                                CEikMenuPane* aMenuPane, 
                                                TInt aCommandId,
                                                CHWRMHaptics* aHaptics )
    {
    // get effect count
    TInt effectCount = iPlayedEffectArray.Count();
    
    // fill effect history data to submenu, if any data available
    if ( effectCount )
        {
        // there is data, hide "none found"
        aMenuPane->SetItemDimmed( EHapticsTestChangeStateNoneFound, ETrue );
        
        // make played effects visible in the submenu
        CEikMenuPaneItem::SData data;
        for ( TInt i = 0; i < effectCount; ++i )
            {
            // buffer for effect name
            HBufC8* effectName = HBufC8::NewL( aHaptics->MaxEffectNameLength() );
            TPtr8 namePtr = effectName->Des();
            
            // effect name
            if ( iPlayedEffectArray[i].iFileHandle > 0 &&
                 iPlayedEffectArray[i].iEffectIndex > 0 )
                {
                aHaptics->GetEffectName( iPlayedEffectArray[i].iFileHandle,
                                         iPlayedEffectArray[i].iEffectIndex,
                                         namePtr );
                }
            else
                {
                namePtr.Append( KManualEffectName );
                }

            // insert data to submenu item
            data.iText.Copy( *effectName );
            data.iCommandId = aCommandId;
            data.iCascadeId = 0;
            data.iFlags = 0;
            data.iExtraText = KNullDesC;
            
            delete effectName;
            
            aMenuPane->AddMenuItemL( data );
            }
        }
    }

// ---------------------------------------------------------------------------
// Creates the checkmark to the duration, which is currently in use
// in the duration modification submenu.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::DynInitDurationSubmenu( 
                                        CEikMenuPane* aMenuPane ) const
    {
    // set check mark to correct duration value
    if ( iModifyEffect->iDuration == KEffectDuration1 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectDuration1,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iDuration == KEffectDuration5 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectDuration5,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iDuration == KEffectDuration10 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectDuration10,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iDuration == KEffectDuration30 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectDuration30,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iDuration == KEffectDuration60 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectDuration60,
                   EEikMenuItemSymbolOn );
        }
    }

// ---------------------------------------------------------------------------
// Creates the checkmark to the magnitude, which is currently in use
// in the magnitude modification submenu.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::DynInitMagnitudeSubmenu( 
                                            CEikMenuPane* aMenuPane ) const
    {
    // set check mark to correct magnitude value
    if ( iModifyEffect->iMagnitude == KHWRMHapticsMinMagnitude )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectMagnitudeMin,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iMagnitude == KEffectMagnitude25 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectMagnitude25,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iMagnitude == KEffectMagnitude50 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectMagnitude50,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iMagnitude == KEffectMagnitude75 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectMagnitude75,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iMagnitude == KHWRMHapticsMaxMagnitude )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectMagnitudeMax,
                   EEikMenuItemSymbolOn );
        }
    }

// ---------------------------------------------------------------------------
// Creates the checkmark to the style, which is currently in use
// in the style modification submenu.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::DynInitStyleSubmenu( 
                                            CEikMenuPane* aMenuPane ) const
    {
    // set check mark to correct style value
    if ( iModifyEffect->iStyle == CHWRMHaptics::EHWRMHapticsStyleSmooth )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectStyleSmooth,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iStyle == CHWRMHaptics::EHWRMHapticsStyleStrong )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectStyleStrong,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iStyle == CHWRMHaptics::EHWRMHapticsStyleSharp )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectStyleSharp,
                   EEikMenuItemSymbolOn );
        }
    }

// ---------------------------------------------------------------------------
// Creates the checkmark to the attack level, which is currently in use
// in the attack level modification submenu.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::DynInitAttackLevelSubmenu( 
                                        CEikMenuPane* aMenuPane ) const
    {
    // set check mark to correct attack level value
    if ( iModifyEffect->iAttackLevel == KHWRMHapticsMinMagnitude )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectAttackMin,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iAttackLevel == KEffectMagnitude25 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectAttack25,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iAttackLevel == KEffectMagnitude50 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectAttack50,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iAttackLevel == KEffectMagnitude75 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectAttack75,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iAttackLevel == KHWRMHapticsMaxMagnitude )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectAttackMax,
                   EEikMenuItemSymbolOn );
        }
    }

// ---------------------------------------------------------------------------
// Creates the checkmark to the fade level, which is currently in use
// in the fade level modification submenu.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::DynInitFadeLevelSubmenu( 
                                        CEikMenuPane* aMenuPane ) const
    {
    // set check mark to correct fade level value
    if ( iModifyEffect->iFadeLevel == KHWRMHapticsMinMagnitude )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectFadeMin,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iFadeLevel == KEffectMagnitude25 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectFade25,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iFadeLevel == KEffectMagnitude50 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectFade50,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iFadeLevel == KEffectMagnitude75 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectFade75,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iFadeLevel == KHWRMHapticsMaxMagnitude )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectFadeMax,
                   EEikMenuItemSymbolOn );
        }
    }

// ---------------------------------------------------------------------------
// Creates the checkmark to the attack time, which is currently in use
// in the attack time modification submenu.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::DynInitAttackTimeSubmenu( 
                                            CEikMenuPane* aMenuPane ) const
    {
    // set check mark to correct attack time value
    if ( iModifyEffect->iAttackTime == KEffectAttackTime02 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectAttackTime02,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iAttackTime == KEffectAttackTime1 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectAttackTime1,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iAttackTime == KEffectAttackTime5 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectAttackTime5,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iAttackTime == KEffectAttackTime10 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectAttackTime10,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iAttackTime == KEffectAttackTime30 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectAttackTime30,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iAttackTime == KEffectAttackTime60 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectAttackTime60,
                   EEikMenuItemSymbolOn );
        }
    }

// ---------------------------------------------------------------------------
// Creates the checkmark to the fade time, which is currently in use
// in the fade time modification submenu.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::DynInitFadeTimeSubmenu( 
                                            CEikMenuPane* aMenuPane ) const
    {
    // set check mark to correct fade time value
    if ( iModifyEffect->iFadeTime == KEffectFadeTime02 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectFadeTime02,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iFadeTime == KEffectFadeTime1 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectFadeTime1,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iFadeTime == KEffectFadeTime5 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectFadeTime5,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iFadeTime == KEffectFadeTime10 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectFadeTime10,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iFadeTime == KEffectFadeTime30 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectFadeTime30,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iFadeTime == KEffectFadeTime60 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectFadeTime60,
                   EEikMenuItemSymbolOn );
        }
    }

// ---------------------------------------------------------------------------
// Creates the checkmark to the period, which is currently in use
// in the period modification submenu.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::DynInitPeriodSubmenu( 
                                            CEikMenuPane* aMenuPane ) const
    {
    // set check mark to correct period value
    if ( iModifyEffect->iPeriod == KEffectPeriod50 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectPeriod50,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iPeriod == KEffectPeriod100 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectPeriod100,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iPeriod == KEffectPeriod300 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectPeriod300,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iPeriod == KEffectPeriod600 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectPeriod600,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iPeriod == KEffectPeriod1000 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectPeriod1000,
                   EEikMenuItemSymbolOn );
        }
    else if ( iModifyEffect->iPeriod == KEffectPeriod3000 )
        {
        aMenuPane->SetItemButtonState( 
                   EHapticsTestModifyEffectPeriod3000,
                   EEikMenuItemSymbolOn );
        }
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CHapticsTestEffectDataHandler::CHapticsTestEffectDataHandler() 
    {
    }
    
// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CHapticsTestEffectDataHandler::ConstructL()
    {
    }

// End of file
