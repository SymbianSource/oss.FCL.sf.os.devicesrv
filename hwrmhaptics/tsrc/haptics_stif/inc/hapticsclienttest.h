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


#ifndef HAPTICSCLIENTTEST_H
#define HAPTICSCLIENTTEST_H

// INCLUDES
#include <hwrmhaptics.h>
#include "hapticsclienttest_main.h"


/**
 * Callback class for haptics status observation.
 */
NONSHARABLE_CLASS( THapticsStatusCallback ) : public MHWRMHapticsObserver
    {
public:

    /**
     * C++ constructor.
     */
    THapticsStatusCallback( CHapticsClientTest* aMTCallback );
    
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
    CHapticsClientTest* iMTCallback;
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
    TActuatorStatusCallback( CHapticsClientTest* aMTCallback );
    
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
    CHapticsClientTest* iMTCallback;
    };

/**
 * Class for making async play-with-filehandle calls on behalf of the
 * CHapticsClientTest
 */ 
NONSHARABLE_CLASS( MT_HapticsTestAsyncPlaySender ) : public CActive
    {
public: 

    /**
     * Constructor
     *
     * @param aUi   Pointer to the test appl UI object.
     */
    MT_HapticsTestAsyncPlaySender( CHapticsClientTest* aMTCallback );
                                 
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
     * Pointer to the CHapticsClientTest.
     * Not owned.
     */
    CHapticsClientTest* iMTCallback;
    
    TRequestStatus* iClientStatus;
    };


#endif      //  HAPTICSCLIENTTEST_H
