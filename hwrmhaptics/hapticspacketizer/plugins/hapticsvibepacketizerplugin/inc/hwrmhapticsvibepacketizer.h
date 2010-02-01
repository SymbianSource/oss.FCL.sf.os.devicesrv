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

#ifndef C_HWRMHAPTICSVIBEPACKETIZER_H
#define C_HWRMHAPTICSVIBEPACKETIZER_H

#include <e32base.h>
#include <hwrmhapticspacketizer.h>

class CDesC8ArraySeg;

/**
 *  ECom plugin for handling vibe command packages.
 *
 *  @lib hwrmhapticsvibepacketizerplugin.lib
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CHWRMHapticsVibePacketizer ): public CHWRMHapticsPacketizer
    {
public:

    /**
     * Two phased constructor.
     * 
     * @return An instance of haptics vibe packetizer.
     */
    static CHWRMHapticsVibePacketizer* NewL();

    /**
     * Destructor.
     */
    virtual ~CHWRMHapticsVibePacketizer();

public: // From CHWRMHapticsPacketizer

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncOpenDeviceReq( THWRMLogicalActuators aLogicalActuator,
                                   RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncCloseDeviceReq( TInt aDeviceHandle, RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncPlayMagSweepEffectReq( 
                        TInt aDeviceHandle, 
                        CHWRMHaptics::THWRMHapticsMagSweepEffect aEffect,
                        RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncPlayPeriodicEffectReq( 
                        TInt aDeviceHandle, 
                        CHWRMHaptics::THWRMHapticsPeriodicEffect aEffect,
                        RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncModifyPlayingMagSweepEffectReq( 
                        TInt aDeviceHandle, 
                        TInt aEffectHandle, 
                        CHWRMHaptics::THWRMHapticsMagSweepEffect aEffect,
                        RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncModifyPlayingPeriodicEffectReq( 
                        TInt aDeviceHandle, 
                        TInt aEffectHandle, 
                        CHWRMHaptics::THWRMHapticsPeriodicEffect aEffect,
                        RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncPausePlayingEffectReq( TInt aDeviceHandle,
                                           TInt aEffectHandle,
                                           RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncResumePausedEffectReq( TInt aDeviceHandle, 
                                           TInt aEffectHandle,
                                           RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncStopPlayingEffectReq( TInt aDeviceHandle, 
                                          TInt aEffectHandle,
                                          RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncStopAllPlayingEffectsReq( TInt aDeviceHandle,
                                              RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncPlayEffectIncludeEffectDataReq( TInt aDeviceHandle,
                                                    const TDesC8& aData, 
                                                    TInt aEffectIndex,
                                                    RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncPlayEffectRepeatIncludeEffectDataReq( TInt aDeviceHandle,
                                                          const TDesC8& aData,
                                                          TInt aEffectIndex, 
                                                          TUint8 aRepeat,
                                                          RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncPlayEffectRepeatNoDataReq( TInt aDeviceHandle,
                                               TInt aEffectIndex, 
                                               TUint8 aRepeat,
                                               RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncPlayEffectNoDataReq( TInt aDeviceHandle,
                                         TInt aEffectIndex,
                                         RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncCreateStreamingEffectReq( TInt aDeviceHandle, 
                                              RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncDestroyStreamingEffectReq( TInt aDeviceHandle,
                                               TInt aEffectHandle,
                                               RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncPlayStreamingSampleReq( TInt aDeviceHandle,
                                            const TDesC8& aStreamingSample, 
                                            TInt aEffectHandle,
                                            RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncPlayStreamingSampleWithOffsetReq( 
                                            TInt aDeviceHandle,
                                            const TDesC8& aStreamingSample,
                                            TInt aOffsetTime,
                                            TInt aEffectHandle,
                                            RBuf8& aBuffer );
    
    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncGetEffectStateReq( TInt aDeviceHandle, 
                                       TInt aEffectHandle,
                                       RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncSetDevicePropertyBoolReq( TInt aDeviceHandle, 
                                              TBool aDevPropValue,
                                              TInt aDevPropType,
                                              RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncSetDevicePropertyIntReq( TInt aDeviceHandle, 
                                             TInt aDevPropValue, 
                                             TInt aDevPropType,
                                             RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncSetDevicePropertyStringReq( TInt aDeviceHandle, 
                                                const TDesC8& aDevPropValue,
                                                TInt aDevPropType,
                                                RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncSetPlatformLicenseKeyReq( TInt aDeviceHandle,
                                              RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncGetDevicePropertyBoolReq( TInt aDeviceHandle,
                                              TInt aDevPropType,
                                              RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncGetDevicePropertyIntReq( TInt aDeviceHandle,
                                             TInt aDevPropType,
                                             RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncGetDevicePropertyStringReq( TInt aDeviceHandle,
                                                TInt aDevPropType,
                                                RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncGetDeviceCapabilityIntReq( TInt aDeviceHandle, 
                                               TInt aDevCapType,
                                               RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt EncGetDeviceCapabilityStringReq( TInt aDeviceHandle,
                                                  TInt aDevCapType,
                                                  RBuf8& aBuffer );

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual CDesC8ArraySeg* DecodeMessageL( const TDesC8& aData, 
                                            TInt& aStatus );
    
    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt DeviceHandle();

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt InfiniteRepeat();

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt InfiniteDuration();

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt MaxEffectNameLength();

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt MaxDeviceNameLength();

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt MaxCapabilityStringLength();

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt MaxPropertyStringLength();

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt MaxStreamingSampleSize();

    /**
     * @see MHWRMHapticsPacketizer
     */ 
    virtual TInt DefaultDevicePriority();

private:

    /**
     * Constructor.
     */
    CHWRMHapticsVibePacketizer();

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Internal helper method. Fills Get Device Capability request data.
     */
    TInt EncGetDeviceCapabilityReqInternal( TInt aValueType,
                                            TInt aDeviceHandle,
                                            TInt aDevCapType,
                                            RBuf8& aBuffer );
                                                     
    /**
     * Internal helper method. Fills Get Device Property request data.
     */                                                 
    TInt EncGetDevicePropertyReqInternal( TInt aValueType,
                                          TInt aDeviceHandle,
                                          TInt aDevPropType,
                                          RBuf8& aBuffer );
    
    /**
     * Internal helper method. Fills Play Streaming Sample request data. 
     */
    TInt EncPlayStreamingSampleInternal( TInt aDeviceHandle,
                                         TInt aEffectHandle,
                                         const TDesC8& aStreamingSample,
                                         TInt aOffsetTime,
                                         RBuf8& aBuffer );

    /**
     * Maps Vibe error code to Symbian error code.
     * 
     * @param  aVibeError Vibe error code.
     * @return Symbian system wide error code.
     */
    TInt MapError( TInt aVibeError );

private: // data

    /**
     * Buffer for request data.
     */ 
    RBuf8 iReqBuf;
    
    /**
     * Buffer for streaming or IVT data.
     */ 
    RBuf8 iDataBuf;

    /**
     * Array used for decoded values in DecodeMessageL. Owned.
     */
    CDesC8ArraySeg* iReturnArray;
    
    /**
     * Stores the latest value of opened device handle. Initiated
     * to KErrNotFound.
     */
    TInt iDeviceHandle;
    };

#endif // C_HWRMHAPTICSVIBEPACKETIZER_H
