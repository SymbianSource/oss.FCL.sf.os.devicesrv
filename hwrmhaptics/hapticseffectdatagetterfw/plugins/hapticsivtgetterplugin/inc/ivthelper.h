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
* Description:  IVT effect getter methods high level helper functions header file.
*
*/

#ifndef IVTHELPER_H
#define IVTHELPER_H

#include "ivtinternal.h"

/**
 * Utility class for parsing various information from effect data buffer.
 */
class IVTHelper
    {
public:
    /**
     * Method for checking that the IVT header is correctly formatted.
     * (This includes also version number check).
     *
     * @param aIVT Pointer to the effect data buffer.
     *
     * @return ETrue if header is correctly formatted, EFalse otherwise.
     */
    static TBool IsValidIVTAddress( const TUint8* aIVT );

    /**
     * Method for getting number of effects within an effect data buffer.
     *
     * @param aIVT Pointer to the effect data buffer.
     *
     * @return Number (count) of effects contained within the effect data 
     *         buffer.
     */
    static TUint16 GetNumEffects( const TUint8* aIVT );

    /**
     * Method for getting the start address of the sub-buffer (from the 
     * effect data buffer containing possibly several effect definitions)
     * that contains effect definition of the given effect.
     *
     * @param aIVT         Pointer to the effect data buffer.
     * @param aEffectIndex Index of the effect (within the effect data 
     *                     buffer).
     *
     * @return Pointer to the effect definition sub-buffer. In case of 
     *         any error, NULL is returned.
     */
    static TUint8* GetEffectStorage( const TUint8* aIVT, TInt aEffectIndex );

    /**
     * Method for extracting envelope (attack and fade times and levels) from
     * a buffer of effect-specific data. I.e., this is given an address to the 
     * effect specific part in effect data buffer, not the whole effect data 
     * buffer. The aPacket param can be obtained e.g. by first calling the
     * above GetEffectStorage method.
     *
     * @param aPacket   Pointer to effect specific data (sub-)buffer.
     * @param aEnvelope Pointer to a TVibeEnvelopeParams structure to which 
     *                  the envelope params are read.
     *
     * @return ETrue on success, EFalse otherwise.
     */
    static TBool ExtractEnvelopeFromPacket( 
        const TUint8*       aPacket, 
        TVibeEnvelopeParams* aEnvelope );
        
    /**
     * Method for extracting effect definition from a buffer of effect-
     * specific data. I.e., this is given an address to the effect-specific
     * part in effect data buffer, not the whole effect data 
     * buffer. The aPacket param can be obtained e.g. by first calling the
     * above GetEffectStorage method.
     *
     * @param aPacket           Pointer to effect specific data (sub-)buffer.
     * @param aEffectDefinition Pointer to a TVibeEffectDefinition structure 
     *                          to which the effect definition params are
     *                          read.
     * @param aPeriodicType     The type of the effect (basis vs timeline) is
     *                          written to the TInt located in this address.
     *
     * @return ETrue on success, EFalse otherwise.
     */
    static TBool ExtractEffectDefinitionFromPacket( 
        const TUint8*          aPacket, 
        TVibeEffectDefinition* aEffectDefinition,
        TInt*                  aPeriodicType );

    /**
     * Method for getting the effect name data sub-buffer address.
     * 
     * @param aIVT         Pointer to the effect data buffer.
     * @param aEffectIndex Index of the effect (within the effect data 
     *                     buffer) which name data is queried.
     *
     * @return Pointer to the (sub-)buffer containing the effect name data.
     *         In case of error, returns NULL pointer.
     */ 
    static TUint8* GetEffectNameData( const TUint8* aIVT, TInt aEffectIndex );
    
    /**
     * Method for getting the effect duration.
     * 
     * @param aIVT         Pointer to the effect data buffer.
     * @param aEffectIndex Index of the effect (within the effect data 
     *                     buffer) which duration is queried.
     *
     * @return Duration of the effect.
     *         In case of error, one of Vibe error codes.
     */ 
    static TInt GetIVTEffectDuration( const TUint8* aIVT, TInt aEffectIndex );

private:

    /**
     * Internal structure for repeat bar.
     * iEvent not owned.
     */
    struct TVibeRepeatBar
        {
        TInt          iTimeOffsetEnd;
        TInt          iTimeDuration;
        TVibeIVTCount iCount;
        const TUint8* iEvent;
        };

    /**
     * Structure for timeline effect information
     * None of the pointer members are owned.
     */ 
    struct TVibeTimeLineInfo 
        {
        TVibeRepeatBar   iRepeatBarStack[KRepeatBarStackMaxSize];
        TInt           iRepeatBarStackTop;
        TUint8*        iOffsetSubBlock;
        TUint8*        iDataSubBlock;
        const TUint8*  iNextEvent;
        TInt           iTimeAllEffectsEnd;
        // all events times offest are relative to this time 
        // when computing effect end time
        TInt           iTimeOffsetZero;
        TInt           iTimeRepeat;
        TInt           iTimeEvent;
        };

    /**
     * Method for getting the IVT header from effect data buffer.
     * 
     * @param aIVT         Pointer to the effect data buffer.
     *
     * @return Pointer to the IVT header.
     */
    static TVibeIVTHeader* GetIVTHeader( const TUint8* aIVT );

    /**
     * Method for checking if the given effect data buffer contains any effect
     * definitions or not.
     *
     * @param aIVT         Pointer to the effect data buffer.
     *
     * @return ETrue if the buffer contains effect definition, EFalse otherwise.
     */
    static TBool IsEmpty( const TUint8* aIVT);

    /**
     * Method for getting storage block size from effect data buffer.
     *
     * @param aIVT         Pointer to the effect data buffer.
     *
     * @return Storage block size.
     */
    static TUint16 GetStorageBlockSize( const TUint8* aIVT );

    /**
     * Method for getting name block size from effect data buffer.
     *
     * @param aIVT         Pointer to the effect data buffer.
     *
     * @return Name block size.
     */
    static TUint16 GetNameBlockSize( const TUint8* aIVT );

    /**
     * Method for checking if the given effect data buffer contains any effect
     * name block(s) or not.
     *
     * @param aIVT         Pointer to the effect data buffer.
     *
     * @return ETrue if the buffer contains effect name block(s), EFalse otherwise.
     */
    static TBool HasNameBlock( const TUint8* aIVT );

    /**
     * Method for getting the start address of the storage block within the 
     * effect data buffer.
     *
     * @param aIVT         Pointer to the effect data buffer.
     *
     * @return Pointer to the start of the storage block. In case of error 
     *         returns NULL.
     */
    static TUint8* GetStorageBlock( const TUint8* aIVT );

    /**
     * Method for getting the start address of the storage offset subblock
     * within the effect data buffer.
     *
     * @param aIVT         Pointer to the effect data buffer.
     *
     * @return Pointer to the start of the storage offset subblock. 
     *         In case of error returns NULL.
     */
    static TUint8* GetStorageOffsetSubBlock( const TUint8* aIVT );
    
    /**
     * Method for getting the start address of the storage data subblock
     * within the effect data buffer.
     *
     * @param aIVT         Pointer to the effect data buffer.
     *
     * @return Pointer to the start of the storage data subblock. 
     *         In case of error returns NULL.
     */
    static TUint8* GetStorageDataSubBlock( const TUint8* aIVT );
    
    /**
     * Method for getting the start address of the name block within the 
     * effect data buffer.
     *
     * @param aIVT         Pointer to the effect data buffer.
     *
     * @return Pointer to the start of the name block. In case of error 
     *         returns NULL.
     */
    static TUint8* GetNameBlock( const TUint8* aIVT );
    
    /**
     * Method for getting the start address of the name offset subblock
     * within the effect data buffer.
     *
     * @param aIVT         Pointer to the effect data buffer.
     *
     * @return Pointer to the start of the name offset subblock. 
     *         In case of error returns NULL.
     */
    static TUint8* GetNameOffsetSubBlock( const TUint8* aIVT );
    
    /**
     * Method for getting the start address of the name data subblock
     * within the effect data buffer.
     *
     * @param aIVT         Pointer to the effect data buffer.
     *
     * @return Pointer to the start of the name data subblock. 
     *         In case of error returns NULL.
     */
    static TUint8* GetNameDataSubBlock( const TUint8* aIVT );

    /**
     * Method for getting size of the effect data buffer (as calculated
     * from the header fields).
     *
     * @param aIVT         Pointer to the effect data buffer.
     *
     * @return Size of the effect data buffer.
     */
    static TUint GetIVTSize( const TUint8* aIVT );

    /**
     * Method for converting firmware representation of magnitude to the
     * one used in user interface (i.e., returned by various magnitude
     * getters).
     *
     * @param aFwMagnitude Magnitude in firmware representation.
     *
     * @return Magnitude as seen by users.
     */
    static TInt ConvertToUserMagnitude( TInt aFwMagnitude );

    /**
     * Method for converting firmware representation of duration to the
     * one used in user interface (i.e., returned by various duration
     * getters).
     *
     * @param aFwDuration Duration in firmware representation.
     *
     * @return Duration as seen by users.
     */
    static TInt ConvertToUserDuration( TVibeIVTMessageTime aFwDuration );
    
    /**
     * Method for converting firmware representation of frequency period to
     * the one used in user interface (i.e., returned by various frequency
     * period getters).
     *
     * @param aFwFrequency Frequency period in firmware representation.
     *
     * @return Frequency period as seen by users.
     */
    static TInt ConvertToUserFrequency( TVibeIVTFreqPer aFwFrequency );
    
    /**
     * Helper method for processing Launch Playback events when calculating
     * timeline effect's duration.
     *
     * @param Pointer to a timeline info structure which is to be processed.
     *
     * @return KVibeStatusSuccess in case of successful processing, otherwise
     *         one of Vibe error statuses.
     */
    static TInt TimelineDurationProcessEvent( 
        TVibeTimeLineInfo* aTimelineInfo );
        
    /**
     * Helper method for processing Repeat Bar events when calculating
     * timeline effect's duration.
     *
     * @param Pointer to a timeline info structure which is to be processed.
     *
     * @return KVibeStatusSuccess in case of successful processing, otherwise
     *         one of Vibe error statuses.
     */
    static TInt TimelineDurationProcessRepeatInStack( 
        TVibeTimeLineInfo* aTimelineInfo );
    };
    
#endif /* IVTHELPER_H */
