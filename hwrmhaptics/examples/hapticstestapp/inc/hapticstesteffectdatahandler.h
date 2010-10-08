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
* Description:  Handles and stores effect related data.
*
*/


#ifndef __HAPTICSTEST_EFFECTDATAHANDLER_H__
#define __HAPTICSTEST_EFFECTDATAHANDLER_H__

#include <e32base.h>
#include <hwrmhaptics.h>

// values used for modifying effects
const TInt KEffectMagnitude25 = 0.25 * KHWRMHapticsMaxMagnitude;
const TInt KEffectMagnitude50 = 0.50 * KHWRMHapticsMaxMagnitude;
const TInt KEffectMagnitude75 = 0.75 * KHWRMHapticsMaxMagnitude;
const TInt KEffectDuration1 = 1000;
const TInt KEffectDuration5 = 5000;
const TInt KEffectDuration10 = 10000;
const TInt KEffectDuration30 = 30000;
const TInt KEffectDuration60 = 60000;
const TInt KEffectAttackTime02 = 200;
const TInt KEffectAttackTime1 = 1000;
const TInt KEffectAttackTime5 = 5000;
const TInt KEffectAttackTime10 = 10000;
const TInt KEffectAttackTime30 = 30000;
const TInt KEffectAttackTime60 = 60000;
const TInt KEffectFadeTime02 = 200;
const TInt KEffectFadeTime1 = 1000;
const TInt KEffectFadeTime5 = 5000;
const TInt KEffectFadeTime10 = 10000;
const TInt KEffectFadeTime30 = 30000;
const TInt KEffectFadeTime60 = 60000;
const TInt KEffectPeriod50 = 50;
const TInt KEffectPeriod100 = 100;
const TInt KEffectPeriod300 = 300;
const TInt KEffectPeriod600 = 600;
const TInt KEffectPeriod1000 = 1000;
const TInt KEffectPeriod3000 = 3000;

/**
 * Handles effect data storage of both history data of 
 * played effects (array) and single effect data of the 
 * effect, which can be modified during runtime.
 */
class CHapticsTestEffectDataHandler : public CBase
    {
public:

    /**
     * Symbian two-phased construction.
     *
     * @return Pointer to the created instance.
     */
    static CHapticsTestEffectDataHandler* NewL();

    /**
     * Symbian two-phased construction. Leaves the pointer
     * onto the cleanupstack.
     *
     * @return Pointer to the created instance.
     */
    static CHapticsTestEffectDataHandler* NewLC();

    /**
     * Destructor.
     */
    virtual ~CHapticsTestEffectDataHandler();

public: // effect history array related

    /**
     * Adds effect data to effect history array.
     * 
     * @param aEffectHandle Handle to the effect just played.
     * @param aFileHandle Handle to IVT-file used for the effect. The 
     * default value KErrNotFound indicates that an IVT-file was not used.
     * @param aIndex Index of the effect in the effect file. The default
     * value KErrNotFound indicates that an IVT-file was not used.
     */
    void AddEffectInfo( TInt aEffectHandle,
                        TInt aFileHandle = KErrNotFound,
                        TInt aIndex = KErrNotFound );

    /**
     * Deletest all effect info items from the list that have the 
     * given filehandle in their data. This method is used, when deleting
     * IVT-data, i.e. that data is no longer available.
     * 
     * @param aFileHandle Handle to IVT-file used for the effect.
     */
    void DeleteEffectInfo( TInt aFileHandle );

    /**
     * Returns the amount of effect data stored in the history data array.
     */
    TInt Count() const;

    /**
     * Returns the effect handle of the given effect data item.
     *
     * @param aIndex Index of the effect data in the history array.
     * @return The effect handle of the effect.
     */
    TInt EffectHandle( TInt aIndex ) const;

    /**
     * Returns the file handle of the given effect data item.
     *
     * @param aIndex Index of the effect data in the history array.
     * @return The file handle of the effect.
     */
    TInt FileHandle( TInt aIndex ) const;

    /**
     * Returns the effect index of the given effect data item.
     *
     * @param aIndex Index of the effect data in the history array.
     * @return The effect index of the effect.
     */
    TInt EffectIndex( TInt aIndex ) const;

public: // modifiable effect related

    /**
     * Resets the modifiable effect data with the data received in the
     * given magsweep effect.
     *
     * @param aMagSweep MagSweep effect data.
     * @param aEffectHandle Effect handle to this effect.
     * @param aHaptics Haptics client instance.
     */
    void ResetModifiableMagSweepEffectL( const CHWRMHaptics::THWRMHapticsMagSweepEffect& aMagSweep,
                                         TInt aEffectHandle,
                                         CHWRMHaptics* aHaptics );

    /**
     * Resets the modifiable effect data with the data received in the
     * given periodic effect.
     *
     * @param aPeriodic Periodic effect data.
     * @param aEffectHandle Effect handle to this effect.
     * @param aHaptics Haptics client instance.
     */
    void ResetModifiablePeriodicEffectL( const CHWRMHaptics::THWRMHapticsPeriodicEffect& aPeriodic,
                                         TInt aEffectHandle,
                                         CHWRMHaptics* aHaptics );

    /**
     * Starts the timer, which calls the ModifyPlayingEffect() callback
     * after a certain time period for modifying the last stored effect
     * using ResetModifiableEffectL().
     *
     * @param aFunction Callback method for the started timer.
     */
    void StartModifiableEffectTimerL( TInt (*aFunction)(TAny* aPtr) );

    /**
     * Stops and deletes the auto-modifiable effect timer.
     */
    void ResetModifiableEffectTimer();
    
    /**
     * Returns whether or not a modifiable effect exists.
     */
    TBool ModifiableEffectExists() const;

    /**
     * Returns whether or not the currently stored effect is a 
     * magsweep effect.
     */
    TBool ModifiableEffectIsMagSweep() const;

    /**
     * Returns whether or not the currently stored effect is a 
     * periodic effect.
     */
    TBool ModifiableEffectIsPeriodic() const;
    
    /**
     * Returns the effect handle of the modifiable effect.
     */
    TInt ModifiableEffectHandle() const;
    
    /**
     * Returns the amount of times the current modifiable effect
     * has been modified already.
     */
    TInt ModifyCount() const;
    
    /**
     * Returns the pointer to the haptics client instance, which
     * has been set to the effect modification data.
     */
    CHWRMHaptics* Haptics() const;
    
    /**
     * Fills in default magsweep effect data into the given structure.
     * 
     * @param aMagSweep MagSweep effect data structure for the method to fill.
     * @param aInfinite Whether or not the duration should be infinite.
     * @param aAutoModifiable Whether or not the created data is used 
     * for automatic modification using a timer.
     * @param aHaptics Haptics client instance.
     */
    void FillDefaultMagSweepData( CHWRMHaptics::THWRMHapticsMagSweepEffect& aMagSweep,
                                  TBool aInfinite, TBool aAutoModifiable,
                                  CHWRMHaptics* aHaptics ) const;

    /**
     * Fills in default periodic effect data into the given structure.
     * 
     * @param aPeriodic Periodic effect data structure for the method to fill.
     * @param aInfinite Whether or not the duration should be infinite.
     * @param aAutoModifiable Whether or not the created data is used 
     * for automatic modification using a timer.
     * @param aHaptics Haptics client instance.
     */
    void FillDefaultPeriodicData( CHWRMHaptics::THWRMHapticsPeriodicEffect& aPeriodic,
                                  TBool aInfinite, TBool aAutoModifiable,
                                  CHWRMHaptics* aHaptics ) const;

    /**
     * Fills in the modifiable effect data into the given magsweep structure.
     *
     * @param aMagSweep MagSweep effect data structure for the method to fill.
     */
    void FillModifiableMagSweepData( CHWRMHaptics::THWRMHapticsMagSweepEffect& aMagSweep ) const;

    /**
     * Fills in the modifiable effect data into the given periodic structure.
     *
     * @param aPeriodic Periodic effect data structure for the method to fill.
     */
    void FillModifiablePeriodicData( CHWRMHaptics::THWRMHapticsPeriodicEffect& aPeriodic ) const;

    /**
     * Sets the modifiable effect's modify count.
     */
    void SetModifyCount( TInt aModifyCount );

    /**
     * Sets the modifiable effect's duration value.
     */
    void SetDuration( TInt aDuration );

    /**
     * Sets the modifiable effect's magnitude value.
     */
    void SetMagnitude( TInt aMagnitude );

    /**
     * Sets the modifiable effect's style value.
     */
    void SetStyle( CHWRMHaptics::THWRMHapticsEffectStyles aStyle );

    /**
     * Sets the modifiable effect's attack level value.
     */
    void SetAttackLevel( TInt aAttackLevel );

    /**
     * Sets the modifiable effect's fade level value.
     */
    void SetFadeLevel( TInt aFadeLevel );

    /**
     * Sets the modifiable effect's attack time value.
     */
    void SetAttackTime( TInt aAttackTime );

    /**
     * Sets the modifiable effect's fade time value.
     */
    void SetFadeTime( TInt aFadeTime );

    /**
     * Sets the modifiable effect's period value.
     */
    void SetPeriod( TInt aPeriod );

public: // submenu handling for effect data related submenus

    /**
     * Creates all submenu items for a change effect state submenu
     * (pause, resume, stop). Fetches all the names of the effects
     * from currently loaded IVT-data.
     *
     * @param aMenuPane Menu object to which menu items are added.
     * @param aCommandId Id of an options menu command to be used
     * for all items.
     * @param aHaptics Haptics client instance used for fetching effect names.
     */
    void DynInitChangeEffectStateSubmenuL( CEikMenuPane* aMenuPane, 
                                           TInt aCommandId,
                                           CHWRMHaptics* aHaptics );
         
    /**
     * Creates the checkmark to the duration, which is currently in use
     * in the duration modification submenu.
     */
    void DynInitDurationSubmenu( CEikMenuPane* aMenuPane ) const;

    /**
     * Creates the checkmark to the magnitude, which is currently in use
     * in the magnitude modification submenu.
     */
    void DynInitMagnitudeSubmenu( CEikMenuPane* aMenuPane ) const;

    /**
     * Creates the checkmark to the style, which is currently in use
     * in the style modification submenu.
     */
    void DynInitStyleSubmenu( CEikMenuPane* aMenuPane ) const;

    /**
     * Creates the checkmark to the attack level, which is currently in use
     * in the attack level modification submenu.
     */
    void DynInitAttackLevelSubmenu( CEikMenuPane* aMenuPane ) const;

    /**
     * Creates the checkmark to the fade level, which is currently in use
     * in the fade level modification submenu.
     */
    void DynInitFadeLevelSubmenu( CEikMenuPane* aMenuPane ) const;

    /**
     * Creates the checkmark to the attack time, which is currently in use
     * in the attack time modification submenu.
     */
    void DynInitAttackTimeSubmenu( CEikMenuPane* aMenuPane ) const;

    /**
     * Creates the checkmark to the fade time, which is currently in use
     * in the fade time modification submenu.
     */
    void DynInitFadeTimeSubmenu( CEikMenuPane* aMenuPane ) const;

    /**
     * Creates the checkmark to the period, which is currently in use
     * in the period modification submenu.
     */
    void DynInitPeriodSubmenu( CEikMenuPane* aMenuPane ) const;

private:
    
    /**
     * Internal structure for maintaining information about played
     * effect. The data is used, when pausing, stopping and resuming
     * effect play.
     */
    struct TPlayedEffect
        {
        /**
         * Handle to IVT-data file, from which the effect has been played.
         * Needed for fetching effect related information.
         */
        TInt iFileHandle;

        /**
         * Index of the effect in the effect file. Needed for fetching
         * effect related information.
         */
        TInt iEffectIndex;

        /**
         * Handle to the effect itself.
         */
        TInt iEffectHandle;
        }; 

    /**
     * Internal structure for maintaining information about an effect, 
     * which can be modified during application execution.
     */
    struct TModifyEffect
        {
        /**
         * Haptics client instance. Not owned.
         * The pointer is included in this data so that the 
         * timer callback method is able to access haptics.
         */
        CHWRMHaptics* iHaptics;
        
        /**
         * Type of the effect being modified.
         */
        CHWRMHaptics::THWRMHapticsEffectTypes iEffectType;

        /**
         * Handle to the effect itself.
         */
        TInt iEffectHandle;
        
        /**
         * Keeps track on how many modifications have been done.
         */
        TInt iModifyCount;
        
        /**
         * Duration value used for the effect.
         */
        TInt iDuration;
        
        /**
         * Magnitude value used for the effect.
         */
        TInt iMagnitude;

        /**
         * Style of the effect.
         */
        TInt iStyle;

        /**
         * Attack level value used for the effect.
         */
        TInt iAttackLevel;

        /**
         * Fade level value used for the effect.
         */
        TInt iFadeLevel;

        /**
         * Attack time value used for the effect.
         */
        TInt iAttackTime;

        /**
         * Fade time value used for the effect.
         */
        TInt iFadeTime;

        /**
         * Period value used for the (periodic) effect.
         */
        TInt iPeriod;
        };

private:

    /**
     * C++ constructor.
     */
    CHapticsTestEffectDataHandler();

    /**
     * Second phase construction.
     */
    void ConstructL();

private:

    /**
     * Array containing history data of effect play. Used for
     * pausing, resuming and stopping effects.
     */
    RArray<TPlayedEffect> iPlayedEffectArray;

    /**
     * Timer used for constant modification of an effect.
     */
    CPeriodic* iModifyTimer;

    /**
     * Effect data stored for effect modification.
     */
    TModifyEffect* iModifyEffect;
    };


#endif // __HAPTICSTEST_EFFECTDATAHANDLER_H__
