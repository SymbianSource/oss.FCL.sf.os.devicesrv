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
* Description:  AppUi class definition.
*
*/


#ifndef __HAPTICSTEST_APPUI_H__
#define __HAPTICSTEST_APPUI_H__

#include <aknappui.h>
#include <hwrmhaptics.h>
#include <hwrmlogicalactuators.h>
#include "hapticstestasyncplaysender.h"

// Forward reference
class CHapticsTestAppView;
class CHWRMHaptics;
class CHapticsTestEffectDataHandler;
class CHapticsTestAsyncPlaySender;

/**
 * @class CHapticsTestAppUi
 *
 * An instance of class ChapticstestAppUi is the UserInterface part of the AVKON
 * application framework for the hapticstest example application
 */
class CHapticsTestAppUi : public CAknAppUi, 
                          public MHWRMHapticsObserver,
                          public MHWRMHapticsActuatorObserver
    {
public:

   /**
    * Perform the second phase construction of a ChapticstestAppUi object
    * this needs to be public due to the way the framework constructs the AppUi 
    */
    void ConstructL();

   /**
    * Perform the first phase of two phase construction.
    * This needs to be public due to the way the framework constructs the AppUi 
    */
    CHapticsTestAppUi();
   
   /**
    * Destroy the object and release all memory objects
    */
    ~CHapticsTestAppUi();

public: // from CAknAppUi

   /**
    * Handle user menu selections
    * 
    * @param aCommand the enumerated code for the option selected
    */
    void HandleCommandL(TInt aCommand);

    void HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination);
    
public: // from MEikMenuObserver
    
    /**
     * @see MEikMenuObserver
     *
     * Manages the options menu contents based on user actions and data.
     */
    virtual void DynInitMenuPaneL( TInt aResourceId, 
                                   CEikMenuPane* aMenuPane ); 

public: // from MHWRMHapticsObserver

    /**
     * @see MHWRMHapticsObserver
     *
     * Outputs received status values onto the screen.
     */
    virtual void HapticsStatusChangedL( THWRMHapticsStatus aStatus );

    /**
     * @see MHWRMHapticsObserver
     *
     * Future use --> empty implementation.
     */
    virtual void EffectCompletedL( TInt aError, TInt aEffectHandle );

public:

    /**
     * @see MHWRMHapticsActuatorObserver
     *
     * Outputs received actuator events onto the screen.
     */
    virtual void ActuatorEventL( THWRMActuatorEvents aEvent, 
                                 THWRMLogicalActuators aActuator );

private:

    /**
     * Creates haptics client instance.
     */
    void OpenHapticsL();

    /**
     * Deletes haptics client instance, and resets member variables
     * so that options-menu is updated accordingly; i.e. actions, which
     * require usage of haptics client become hidden.
     */
    void CloseHaptics();
    
    /**
     * Fetches the supported actuators from haptics server, and stores
     * the mask to iSupportedActuators.
     */
    void FetchSupportedActuators();
    
    /**
     * Prints the supported actuators to the UI according to
     * the value in iSupportedActuators.
     */
    void PrintSupportedActuators();
    
    /**
     * Attemps to open the given actuator. Prints the result
     * of the attemp to the UI.
     *
     * @param aActuator The logical actuator type to open.
     */
    void OpenActuator( THWRMLogicalActuators aActuator );

    /**
     * Reserves haptics for this application.
     *
     * @param aHaptics Haptics client instance, which should be used to 
     * make the reservation.
     * @param aForceNoCoe If true, forces for haptics not to do automatic
     * reserve/release on focus gain/lost (in this case this application
     * must be trusted, otherwise the reservation call will end to 
     * KErrAccessDenied). If false, haptics will use automatic reserve/release
     * on focus gain/lost (default).
     */
    void ReserveHaptics( CHWRMHaptics* aHaptics, TBool aForceNoCoe );

    /**
     * Searches for IVT-files in the filesystem, and writes
     * them to iIVTFileArray.
     */
    void FindIVTFiles();
    
    /**
     * Loads IVT data from the given file to haptics.
     *
     * @param aFile File where the IVT-data is stored.
     */
    void LoadIVTDataL( TFileName& aFile );

    /**
     * Plays the effect of the given index in the currently
     * loaded IVT-file using its filehandle or IVT-data.
     *
     * @param aIndex Index of the effect in the IVT-file.
     */
    void PlayEffect( TInt aIndex );

    /**
     * Plays the effect of the given index in the currently
     * loaded IVT-file repeatedly using its filehandle or IVT-data.
     *
     * @param aIndex Index of the effect in the IVT-file.
     * @param aRepeat Number of times the effect is to be repeated.
     */
    void RepeatEffect( TInt aIndex, TInt aRepeats );

    /**
     * Plays a magsweep effect using manual definition.
     *
     * @param aInfinite Whether or not the effect should last infinitely.
     * @param aModifiable Whether or not the effect should stored to member
     * variables as the current modifiable effect.
     */
    void PlayMagSweepL( TBool aInfinite = EFalse, TBool aModifiable = EFalse );

    /**
     * Plays a periodic effect using manual definition.
     *
     * @param aInfinite Whether or not the effect should last infinitely.
     * @param aModifiable Whether or not the effect should stored to member
     * variables as the current modifiable effect.
     */
    void PlayPeriodicL( TBool aInfinite = EFalse, TBool aModifiable = EFalse );

    /**
     * Repeats a timeline effect using manual definition.
     *
     * @param aRepeat Number of times the effect is to be repeated.
     */
    void PlayRepeatL( TInt aRepeat );

    /**
     * Pauses the effect of the given index in the effect history data.
     *
     * @param aIndex Index of the effect in the effect history data.
     */
    void PauseEffect( TInt aIndex );

    /**
     * Resumes the effect of the given index in the effect history data.
     *
     * @param aIndex Index of the effect in the effect history data.
     */
    void ResumeEffect( TInt aIndex );

    /**
     * Stops the effect of the given index in the effect history data.
     *
     * @param aIndex Index of the effect in the effect history data.
     */
    void StopEffect( TInt aIndex );

    /**
     * Deletes loaded IVT data.
     *
     * @param aIndex Array index to the loaded IVT file array item
     *               (defined below) that is to be deleted.
     */
    void DeleteLoadedIVTData( TInt aIndex );
    
    /**
     * Deletes all loaded IVT datas.
     */
    void DeleteAllLoadedIVTData(); 

    /**
     * Fetches the current state of the last played effect 
     * (iEffectHandle), and prints the result to the UI.
     */
    void GetCurrentEffectState();
    
    /**
     * Prints the status of a "play effect".
     *
     * @param aErrorCode Return code from playing an effect.
     */
    void PrintPlayEffectStatus( TInt aErrorCode ) const;

    /**
     * Gets the number of repeats from user in case of 
     * "play effect repeatedly" cases.
     * In practice opens a list query where user may choose
     * one of the available repeats values.
     *
     * @returns Number of repeats (one of 0,1,2,3,7,255), 
     *          where 255 represents infinite value in vibra player.
     */
    TInt GetNumberOfRepeats();

    /**
     * Creates all submenu items for a play effect submenu.
     * Fetches all the names of the effects from currently 
     * loaded IVT-data.
     *
     * @param aMenuPane Menu object to which menu items are added.
     * @param aCommandId Id of an options menu command to be used
     * for all items.
     * @param aDimCommand Id of a command, which needs to be dimmed,
     * when the submenu contains items.
     */
    void CreatePlayEffectSubmenuL( CEikMenuPane* aMenuPane, 
                                   TInt aCommandId,
                                   TInt aDimCommandId );
         
    /**
     * Counts the actual effect index (within an IVT file) based 
     * on the index of the selected item in effects submenu. 
     * Also sets the iIVTFileHandle and iIVTDataBuffer members
     * based on the file containing the selected effect.
     * 
     * @param aSubmenuIndex Index from the submenu that contains
     *                      all currently loaded effects (i.e.,  
     *                      that contains all effects in all 
     *                      currently loaded IVT files).
     *
     * @returns Effect index.
     */
    TInt CountFileHandleAndEffectIndex( TInt aSubmenuIndex );

    /**
     * Prints effect information got from getter functions.
     * 
     * @param aIndex Index of the effect in the IVT-file.
     */
    void PrintEffectInfo( TInt aIndex );

    /**
     * Prints the given status onto the screen.
     * 
     * @param aStatus Status value to be printed.
     */
    void PrintStatusInfo( MHWRMHapticsObserver::THWRMHapticsStatus aStatus ) const;
    
    /**
     * Runs test for all getter methods for constants 
     * and prints out the results.
     */
     void ConstGetters() const;

    /**
     * Modifies an effect using the values set for iModifyEffect.
     */
     void ModifyEffect();

    /**
     * Sets property Priority to min 
     */
     void SetPropertyPriorityMin();    

    /**
     * Sets property Priority to default 
     */
     void SetPropertyPriorityDefault();    
     
    /**
     * Sets property Priority to max 
     */
     void SetPropertyPriorityMax();    

    /**
     * Sets property Disable effects to ETrue
     */
     void SetPropertyDisableEffectsTrue();    

    /**
     * Sets property Disable effects to EFalse
     */
     void SetPropertyDisableEffectsFalse();    

    /**
     * Sets property Strength to Min level
     */
     void SetPropertyStrengthMin();    

    /**
     * Sets property Strength to middle level
     */
     void SetPropertyStrengthMiddle();    

    /**
     * Sets property Strength to Max level
     */
     void SetPropertyStrengthMax();    

    /**
     * Gets Priority property value
     */
     void GetPropertyPriority();    
     
    /**
     * Gets Disable Effects property value
     */
     void GetPropertyDisableEffects();    
      
    /**
     * Gets Strength property value
     */
     void GetPropertyStrength();    
      
    /**
     * Gets device category capability.
     */
     void GetCapabilityDeviceCategory();    

    /**
     * Gets max nested repeats..
     */
     void GetCapabilityMaxNestedRepeats();
     
    /**
     * Gets number of actuators capability.
     */
     void GetCapabilityNumActuators();    

    /**
     * Gets actuator type capability.
     */
     void GetCapabilityActuatorType();    

    /**
     * Gets number of effect slots.
     */
     void GetCapabilityNumEffectSlots();    

    /**
     * Gets number of effect styles.
     */
     void GetCapabilityNumEffectStyles();    

    /**
     * Gets minimum period of periodic effects.
     */
     void GetCapabilityMinPeriod();    

    /**
     * Gets maximum period of periodic effects.
     */
     void GetCapabilityMaxPeriod();    

    /**
     * Gets maximum effect duration..
     */
     void GetCapabilityMaxEffectDuration();    

    /**
     * Gets supported effect types..
     */
     void GetCapabilitySupportedEffects();    

    /**
     * Gets device name capability.
     */
     void GetCapabilityDeviceName();    

    /**
     * Gets max envelope type.
     */
     void GetCapabilityMaxEnvelopeTime();    

    /**
     * Gets API version number.
     */
     void GetCapabilityAPIVersionNumber();    

    /**
     * Gets max size of IVT file.
     */
     void GetCapabilityMaxIVTSize();    

    /**
     * Callback method for CPeriodic timer for modifying a certain effect
     * after each time period.
     *
     * @param aPtr Pointer to a TModifyEffect data used for the modification.
     * @return System wide error code.
     */
     static TInt ModifyPlayingEffect( TAny* aPtr );

    /**
     * Enables streaming mode.
     */
    void StartStreaming();
    
    /**
     * Plays one streaming sample from KStreamArray.
     *
     * @param aSampleId Index to KStreamArray.
     */
    void PlayStreamingSample( TInt aSampleId );
    
    /**
     * Disables streaming mode.
     */
    void StopStreaming();

private:

    /**
     * Internal struct for IVT files that contain file name,
     * file handle (received from haptics) and IVT data 
     * as data buffer.
     */
    struct TLoadedIVTFileItem
        {
        TFileName iFileName;
        TInt iFileHandle;
        HBufC8* iDataBuffer; // owns
        
        // ctor
        TLoadedIVTFileItem() : iFileHandle( 0 ), iDataBuffer ( NULL )
            {
            // empty
            }

        // static matcher method used in checking whether the 
        // given file (name) is already in the loaded IVT 
        // files storage.
        static TBool Match( const TLoadedIVTFileItem& a, const TLoadedIVTFileItem& b )
            {
            TBool retVal = EFalse;
            if ( 0 == a.iFileName.CompareF( b.iFileName ) )
                {
                retVal = ETrue;
                }
            return retVal;    
            }
        }; 

private:

    /**
     * Menu pane pointer received in DynInitMenuPaneL() used
     * in handling dynamic submenus (in HandleCommandL()).
     * Not owned.
     */
    CEikMenuPane* iMenuPane;

    /**
     * Application view instance. Owned.
     */
    CHapticsTestAppView* iAppView;

    /**
     * Haptics client instance. Owned.
     */
    CHWRMHaptics* iHaptics;

    /**
     * Second haptics client instance used for making reservations,
     * which cause iHaptics unable to use play-commands. Owned.
     */
    CHWRMHaptics* iTempHaptics;

    /**
     * Holds the information which logical actuators are supported
     * by the haptics system. In the options menu, this value is used
     * in construction of the actuator opening submenu.
     */
    TUint32 iSupportedActuators;
    
    /**
     * Constains the names of the IVT-files found in the 
     * filesystem, when the application was launched.
     */ 
    RArray<TFileName> iIVTFileArray;
    
    /**
     * Array that contains above defined items that represent
     * currently loaded IVT files.
     */
    RArray<TLoadedIVTFileItem> iLoadedIVTFileArray; 

    /**
     * Whether or not an actuator has been successfully opened.
     */    
    TBool iActuatorOpened;

    /**
     * Filehandle to loaded IVT-data.
     */    
    TInt iIVTFileHandle;

    /**
     * Buffer containing latest loaded IVT-data.
     */    
    HBufC8* iIVTDataBuffer; // not owned

    /**
     * Effect handle to the last played effect.
     */    
    TInt iEffectHandle;

    /**
     * Whether or not to use synchronous calls to haptics by defualt.
     */    
    TBool iSynchronous;

    /**
     * Whether or not to use fileHandle when calling "play" Haptics API
     * methods.
     */
    TBool iUseHandle;

    /**
     * Whether or not files are read from mem card (if not, they're
     * read from phone memory).
     */
    TBool iUsesMemCard;

    /**
     * Is effect information showed or not
     */
    TBool iShowEffectInfo;

    /**
     * Objects used for storaging effect related history data and 
     * modifiable effect data.
     */
    CHapticsTestEffectDataHandler* iEffectData;

    /**
     * Is effect streaming on or not
     */
    TBool iStreaming;
    
    /**
     * played stream sample number
     */
    TInt iCurrentSample;

    /**
     * Active object for sending async play commands
     */
    CHapticsTestAsyncPlaySender* iAsyncPlaySender;
    friend class CHapticsTestAsyncPlaySender;
    };


#endif // __HAPTICSTEST_APPUI_H__

