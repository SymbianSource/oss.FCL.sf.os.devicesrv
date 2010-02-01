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
* Description:  Timeline effect internal getters header file.
*
*/

#ifndef TIMELINEIVTGETTERS_H
#define TIMELINEIVTGETTERS_H


#include "ivtinternal.h"

/**
 * Utility class for getting various timeline effect specific information
 * from data buffer.
 */
class TimelineIVTGetters
    {
public:
    /**
     * Method for getting the effect Id from effect data buffer.
     * Takes pointer to the current position in effect data buffer (i.e.,
     * position from which the Id is read) and sets the pointer to next
     * position.
     *
     * @param aProp  Pointer to the current position in effect data buffer
     * @param aPProp Double pointer to which new position in effect data
     *               buffer is written (out param).
     *
     * @return Id of the effect (that is in current position of the effect
     *         data buffer).
     */
    static TVibeIVTEffectID VibeIVTGetEffectID( const TUint8*  aProp,
                                                const TUint8** aPProp );

    /**
     * Method for getting the effect time offset from effect data buffer.
     * Takes pointer to the current position in effect data buffer (i.e.,
     * position from which the time offset is read) and sets the pointer 
     * to next position.
     *
     * @param aProp  Pointer to the current position in effect data buffer
     * @param aPProp Double pointer to which new position in effect data
     *               buffer is written (out param).
     *
     * @return Time offset of the effect (that is in current position of 
     *         the effect data buffer).
     */
    static TInt VibeIVTGetTimeOffset( const TUint8*  aProp,
                                      const TUint8** aPProp );

    /**
     * Method for getting the count value from effect data buffer.
     * Takes pointer to the current position in effect data buffer (i.e.,
     * position from which the count is read) and sets the pointer to next
     * position.
     *
     * @param aProp  Pointer to the current position in effect data buffer
     * @param aPProp Double pointer to which new position in effect data
     *               buffer is written (out param).
     *
     * @return Count value (that is in current position of the effect
     *         data buffer).
     */
    static TVibeIVTCount VibeIVTGetCount( const TUint8*  aProp,
                                          const TUint8** aPProp );

    /**
     * Method for getting the effect duration from effect data buffer.
     * Takes pointer to the current position in effect data buffer (i.e.,
     * position from which the duration is read) and sets the pointer to next
     * position.
     *
     * @param aProp  Pointer to the current position in effect data buffer
     * @param aPProp Double pointer to which new position in effect data
     *               buffer is written (out param).
     *
     * @return Duration of the effect (that is in current position of the 
     *         effect data buffer).
     */
    static TVibeIVTMessageTime VibeIVTGetDuration( const TUint8*  aProp,
                                                   const TUint8** aPProp );

    /**
     * Method for getting the effect magnitude from effect data buffer.
     * Takes pointer to the current position in effect data buffer (i.e.,
     * position from which the magnitude is read) and sets the pointer to next
     * position.
     *
     * @param aProp  Pointer to the current position in effect data buffer
     * @param aPProp Double pointer to which new position in effect data
     *               buffer is written (out param).
     *
     * @return Magnitude of the effect (that is in current position of the 
     *         effect data buffer).
     */
    static TVibeIVTMagnitude VibeIVTGetMagnitude( 
        const TUint8*  aProp,
        const TUint8** aPProp );

    /**
     * Method for getting the effect frequency period from effect data buffer.
     * Takes pointer to the current position in effect data buffer (i.e.,
     * position from which the frequency period is read) and sets the pointer
     * to nextposition.
     *
     * @param aProp  Pointer to the current position in effect data buffer.
     * @param aPProp Double pointer to which new position in effect data
     *               buffer is written (out param).
     *
     * @return Frequency period of the effect (that is in current position of
     *         the effect data buffer).
     */
    static TVibeIVTFreqPer VibeIVTGetFreqPer( const TUint8*  aProp,
                                              const TUint8** aPProp );
                                   
    /**
     * Method for getting the event time offset from effect data buffer.
     * Takes pointer to the current position in effect data buffer (i.e.,
     * position from which the event offset is read).
     *
     * @param aProp  Pointer to the current position in effect data buffer
     *
     * @return Time offset of the event (that is in current position of 
     *         the effect data buffer). Time unit depends on the given data.
     */
    static TInt VibeIVTGetEventTimeOffset( const TUint8* aProp );

private:

    /**
     * Method for getting the override values from effect data buffer.
     * Takes pointer to the current position in effect data buffer (i.e.,
     * position from which the overrides are read) and sets the pointer 
     * to next position.
     *
     * @param aProp          Pointer to the current position in effect data 
     *                       buffer.
     * @param aOverrideFlags Override flags are read into this (out) param.
     * @param aDuration      Override duration is read into this (out) param.
     * @param aMagnitude     Override magnitude is read into this (out) param.
     * @param aFreqPer       Override frequency period is read into this (out)
     *                       param.
     * @param aPProp         Double pointer to which new position in effect 
     *                       data buffer is written (out param).
     *
     */
    static void VibeIVTGetOverrides( const TUint8*          aProp,
                                     TVibeIVTOverrideFlags* aOverrideFlags,
                                     TVibeIVTMessageTime*   aDuration,
                                     TVibeIVTMagnitude*     aMagnitude,
                                     TVibeIVTFreqPer*       aFreqPer,
                                     const TUint8**         aPProp );

    /**
     * Method for getting the launch playback event time offset from effect
     * data buffer.
     * 
     * @param aProp  Pointer to the current position in effect data buffer.
     *
     * @return The time offset of the launch playback event. Time unit 
     * depends on the given data.
     */
    static TInt VibeIVTGetLaunchPlaybackEventTimeOffset( 
        const TUint8* aProp );

    /**
     * Method for getting the repeat bar event time offset from effect
     * data buffer.
     * 
     * @param aProp  Pointer to the current position in effect data buffer.
     *
     * @return The time offset of the repeat bar event.  Time unit
     * depends on the given data.
     */
    static TInt VibeIVTGetRepeatBarEventTimeOffset( const TUint8* aProp );      
    };

#endif /* TIMELINEIVTGETTERS_H*/
