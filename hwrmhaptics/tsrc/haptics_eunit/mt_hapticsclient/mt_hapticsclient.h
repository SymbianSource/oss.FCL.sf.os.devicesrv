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


#ifndef __MT_HAPTICSCLIENT_H__
#define __MT_HAPTICSCLIENT_H__

// INCLUDES
#include <Digia/EUnit/CEUnitTestSuiteClass.h>
#include <Digia/EUnit/EUnitDecorators.h>
#include <hwrmhaptics.h>

class MT_HapticsClient;

/**
 * Callback class for haptics status observation.
 */
NONSHARABLE_CLASS( THapticsStatusCallback ) : public MHWRMHapticsObserver
    {
public:

    /**
     * C++ constructor.
     */
    THapticsStatusCallback( MT_HapticsClient* aMTCallback );
    
    /**
     * Destructor.
     */
    ~THapticsStatusCallback();
    
    /**
     * Returns the last received status.
     *
     * @return Last status value.
     */
    MHWRMHapticsObserver::THWRMHapticsStatus Status();
    
    /**
     * Starts a new wait loop under the current active scheduler.
     * This method can be used in module tests to wait for the 
     * notification to be received. When the notification is 
     * received (i.e. callback is called by haptics), the waiting
     * is stopped and the execution continues whereever this method
     * was called.
     */
    void WaitForNotification();

public: // from MHWRMHapticsObserver

    virtual void HapticsStatusChangedL( THWRMHapticsStatus aStatus );
    virtual void EffectCompletedL( TInt aError, TInt aEffectHandle );

private:
    
    /**
     * Last received haptics status through status observation.
     */
    MHWRMHapticsObserver::THWRMHapticsStatus iHapticsStatus;
    
    /**
     * Callback which is notified, when a status has been received.
     */
    MT_HapticsClient* iMTCallback;
    };


/**
 * Callback class for actuator status observation.
 */
NONSHARABLE_CLASS( TActuatorStatusCallback ) : public MHWRMHapticsActuatorObserver
    {
public:

    /**
     * C++ constructor.
     */
    TActuatorStatusCallback( MT_HapticsClient* aMTCallback );
    
    /**
     * Destructor.
     */
    ~TActuatorStatusCallback();
    
    /**
     * Returns the last received state.
     *
     * @return Last status value.
     */
    MHWRMHapticsActuatorObserver::THWRMActuatorEvents Status( THWRMLogicalActuators aActuator );
    
    /**
     * Starts a new wait loop under the current active scheduler.
     * This method can be used in module tests to wait for the 
     * notification to be received. When the notification is 
     * received (i.e. callback is called by haptics), the waiting
     * is stopped and the execution continues whereever this method
     * was called.
     */
    void WaitForNotification();

public: // from MHWRMHapticsActuatorObserver

    virtual void ActuatorEventL( THWRMActuatorEvents aEvent,
                                 THWRMLogicalActuators aActuator );

private:
    
    /**
     * Last received actuator status through state observation.
     */
    RArray<TInt> iActuatorStatus;

    /**
     * Callback which is notified, when a state has been received.
     */
    MT_HapticsClient* iMTCallback;
    };

/**
 * Class for making async play-with-filehandle calls on behalf of the
 * MT_HapticsClient
 */ 
NONSHARABLE_CLASS( MT_HapticsTestAsyncPlaySender ) : public CActive
    {
public: 

    /**
     * Constructor
     *
     * @param aUi   Pointer to the test appl UI object.
     */
    MT_HapticsTestAsyncPlaySender( MT_HapticsClient* aMTCallback );
                                 
    /**
     * Destructor
     */
    virtual ~MT_HapticsTestAsyncPlaySender();
 
    /**
     * Method for fileHandle type playing of effects repeatedly in asynch mode
     * 
     * @param aHaptics Pointer to the haptics interface object.
     * @param aFileHandle Handle for the effect data buffer from which to
     *                    play the effects.
     * @param aEffectIndex Index of the effect that is being played.
     * @param aRepeat Number of repeats.
     * @param aEffectHandle Out-parameter to which the handle for the played
     *        effect is returned.
     */
    void PlayRepeat( CHWRMHaptics* aHaptics,
                     TInt  aFileHandle,
                     TInt aEffectIndex,
                     TUint8 aRepeat,
                     TInt& aEffectHandle,
                     TRequestStatus& aClientStatus );

    /**
     * Method for fileHandle type playing of effects in asynch mode
     * 
     * @param aHaptics Pointer to the haptics interface object.
     * @param aFileHandle Handle for the effect data buffer from which to
     *                    play the effects.
     * @param aEffectIndex Index of the effect that is being played.
     * @param aEffectHandle Out-parameter to which the handle for the played
     *        effect is returned.
     */
    void Play( CHWRMHaptics* aHaptics,
               TInt  aFileHandle,
               TInt aEffectIndex,
               TInt& aEffectHandle,
               TRequestStatus& aClientStatus );
 
public:

    /**
     * From CActive
     */
    virtual void RunL();
    
    /**
     * From CActive
     */
    virtual void DoCancel();

    /**
     * From CActive
     */
    virtual TInt RunError( TInt aError ); 
 
private:

    /**
     * Pointer to the MT_HapticsClient.
     * Not owned.
     */
    MT_HapticsClient* iMTCallback;
    
    TRequestStatus* iClientStatus;
    };

/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( MT_HapticsClient ) : public CEUnitTestSuiteClass
    {
public:  // Constructors and destructor

    static MT_HapticsClient* NewL();
    static MT_HapticsClient* NewLC();
    ~MT_HapticsClient();

    /**
     * Method, which is called by the status callback, when a status
     * has been received. Stops the wait loop waiting for a new status.
     */    
    void HapticsStatusReceived();

private: // Constructors

    MT_HapticsClient();
    void ConstructL();

private: // New methods

    /**
     * Test construction; Creates haptics client instance.
     */
    void SetupL();

    /**
     * Test teardown; Deletes haptics client instance.
     */
    void Teardown();

    /**
     * Empty method to be used as setup() and teardown() in test
     * cases, where all test code is in the actual test method.
     */
    void Dummy();

    /**
     * Helpers for creating effects
     */
    void CreateMagSweepEffect( CHWRMHaptics::THWRMHapticsMagSweepEffect& aEffect );
    void CreatePeriodicEffect( CHWRMHaptics::THWRMHapticsPeriodicEffect& aEffect );
    void CreateIVTEffect( RBuf8& aBuffer, CHWRMHaptics::THWRMHapticsEffectTypes aType );

    /**
     * Helper for such cases where it is first needed to load IVT data
     */
    TInt LoadIVTDataInternal( TInt& aFileHandle, CHWRMHaptics::THWRMHapticsEffectTypes aType,
                              TBool aOpenActuator = ETrue );

    /**
     * Helper for such cases where it is first needed to load IVT data
     */
    TInt LoadIVTDataInternal( TInt& aFileHandle, 
                              const TUint8 aIvtData[],
                              TInt aIvtDataSize,
                              TBool aOpenActuator = ETrue );

    /**
     * Helper for such cases where contents of stream sample
     * is not relevant.
     */
    void CreateDummyStreamSample( TInt aSize, RBuf8& aBuffer );
    
    void T_CreateAndDeleteL();
    void T_SeveralSessionsL();
    void T_ConsecutiveSessionsL();
    void T_SupportedActuators();
    void T_OpenSupportedActuator();
    void T_OpenNotSupportedActuator();
    void T_OpenAllActuators();
    void T_SeveralClientsWithOpenActuatorsL();
    void T_PlayMagSweepEffectSynchronous();
    void T_PlayMagSweepEffectAsynchronous();
    void T_PlayPeriodicEffectSynchronous();
    void T_PlayPeriodicEffectAsynchronous();
    void T_PlayIVTEffectWithDirectDataSynchronous();
    void T_PlayIVTEffectWithDirectDataAsynchronous();
    void T_PlayIVTEffectRepeatWithDirectDataSynchronous();
    void T_PlayIVTEffectRepeatWithDirectDataAsynchronous();
    void T_GetEffectState();
    void T_PlayNonTimelineIVTEffectRepeatWithDirectDataSynchronous();
    void T_PlayNonTimelineIVTEffectRepeatWithDirectDataAsynchronous();
    void T_PlayIVTEffectRepeatWithDirectDataWithZeroRepeatValueSynchronous();
    void T_PlayIVTEffectRepeatWithDirectDataWithZeroRepeatValueAsynchronous();  
    void T_LoadMultipleIVTData();
    void T_PlayIVTEffectsFromMultipleIVTDataSynchronous();
    void T_PlayIVTEffectsFromMultipleIVTDataAsynchronous();
    void T_PlayIVTEffectRepeatFromMultipleIVTDataSynchronous();
    void T_PlayIVTEffectRepeatFromMultipleIVTDataAsynchronous();
    void T_LoadIVTData();
    void T_DeleteIVTData();
    void T_DeleteAllIVTData();
    void T_LoadAndDeleteIVTData();
    void T_LoadAndDeleteIVTDataMixed();
    void T_LoadDeletePlaySync();
    void T_LoadDeletePlayAsync();
    void T_LoadDeletePlayRepeatSync();
    void T_LoadDeletePlayRepeatAsync();
    void T_PlayIVTEffectWithLoadedDataSynchronous();
    void T_PlayIVTEffectWithLoadedDataAsynchronous();
    void T_PlayIVTEffectRepeatWithLoadedDataSynchronous();
    void T_PlayIVTEffectRepeatWithLoadedDataAsynchronous();
    void T_ReserveAndReleaseHaptics();
    void T_ReserveHapticsSeveralTimes();
    void T_ReserveHapticsNoReleaseL();
    void T_ReleaseHapticsWithoutReservingFirst();
    void T_ReserveNoAutomaticFocusNotTrusted();
    void T_ReserveAlreadyReservedHigherL();
    void T_ReserveAlreadyReservedLowerL();
    void T_PlayWhenReservedL();
    void T_GetIVTEffectCount();
    void T_GetIVTEffectDuration();
    void T_GetIVTEffectIndexFromName();
    void T_GetIVTEffectType();
    void T_GetIVTEffectName();
    void T_GetIVTMagSweepEffectDefinition();
    void T_GetIVTPeriodicEffectDefinition();
    void T_CreateAndDeleteAsynchronousL();
    void T_SeveralSessionsAsynchronousL();
    void T_ConsecutiveSessionsAsynchronousL();
    void T_PauseEffect();
    void T_ResumeEffect();
    void T_StopEffect();
    void T_StopAllEffects();
    void T_PlayPauseResumeStop();
    void T_ConstantGetters();
    void T_ModifyMagSweepEffectSynchronous();
    void T_ModifyMagSweepEffectAsynchronous();
    void T_ModifyPeriodicEffectSynchronous();
    void T_ModifyPeriodicEffectAsynchronous();

    void T_VibeStatusToS60StatusConversions();

    void T_GetDeviceProperty_TInt_TInt();
    void T_GetDeviceProperty_TInt_TDesC();
    void T_SetDeviceProperty_TInt_TInt();
    void T_SetDeviceProperty_TInt_TDesC();    
    void T_GetDeviceCapability_TInt_TInt();
    void T_GetDeviceCapability_TInt_TDesC();
    
    void T_GetHapticsStatus();
    void T_ObserveHapticsStatus();
    void T_GetHapticsStatusWithReservations();
    void T_ObserveHapticsStatusWithReservations();
    void T_GetHapticsStatusWithSeveralReservations();
    void T_ObserveHapticsStatusWithSeveralReservations();
    void T_GetHapticsStatusWithSeveralClientsOneReservation();
    void T_ObserveHapticsStatusWithSeveralClientsOneReservations();
    void T_CreateStreamingEffect();
    void T_PlayStreamingSample();
    void T_PlayStreamingSampleAsync();
    void T_PlayStreamingSampleWithOffset();
    void T_PlayStreamingSampleWithOffsetAsync();
    void T_DestroyStreamingEffect();
    void T_ObserveActuatorStatus();
    void T_ObserveActuatorStatusAndHapticsStatus();
	void T_PluginMgrTransTimerExpires();

private: // Data

    /**
     * Haptics client instance. Owned.
     */
    CHWRMHaptics* iHaptics;

    /**
     * Used to control tests, which are observing haptics status using
     * status callback.
     */
    CActiveSchedulerWait iWait;
    
    /**
     * Async play-with-handle methods sender.
     * Owned.
     */ 
    MT_HapticsTestAsyncPlaySender* iSender;
    
    EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __MT_HAPTICSCLIENT_H__
