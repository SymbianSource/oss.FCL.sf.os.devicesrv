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
* Description:  Definition for asynchronous play commands sender object.
*
*/


#ifndef __HAPTICSTEST_ASYNCPLAYSENDER_H__
#define __HAPTICSTEST_ASYNCPLAYSENDER_H__

#include <e32base.h>
class CHapticsTestAppView;
class CHapticsTestAppUi;

/**
 * Active Object for sending async play methods
 */
class CHapticsTestAsyncPlaySender : public CActive
    {
public: 

    /**
     * Constructor
     *
     * @param aUi   Pointer to the test appl UI object.
     */
    CHapticsTestAsyncPlaySender( CHapticsTestAppUi* aUi );
                                 
    /**
     * Destructor
     */
    virtual ~CHapticsTestAsyncPlaySender();
 
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
                     TInt& aEffectHandle );

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
               TInt& aEffectHandle );
 
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
     * Pointer to the application UI object.
     * Not owned.
     */
    CHapticsTestAppUi* iUi;

    /**
     * Index of the effect for which the play request was made.
     */
    TInt iIndex;
    };

#endif // __HAPTICSTEST_ASYNCPLAYSENDER_H__

