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
* Description:  Haptics test (adaptation) plugin timer header file
*
*/



#ifndef __HWRMHAPTICSTESTPLUGINTIMER_H_
#define __HWRMHAPTICSTESTPLUGINTIMER_H_

#include <hwrmhapticspluginservice.h>

/**
 *  An interface for handling timer expiration events.
 *
 * @since S60 5.1
 */
class MPluginTimerCallback
    {
public:
    /** 
     * Method that is called when the timer expires.
     * Must be implemented by classes that support this interface.
     *
     * @param aService Pointer to an object that implements
     *                 the MHWRMHapticsPluginCallback interface
     *                 and thus handles the callbacks from this
     *                 test plugin. In practice this is pointer 
     *                 to the Haptics plugin manager object.
     * @param aCommandId ID of the command that was being executed
     *                   (or actually emulated as being executed) 
     *                   at the time the timer expired.
     * @param aTransId Transaction ID of the command that was being executed
     *                 (or actually emulated as being executed) 
     *                 at the time the timer expired.
     * @param aRetVal  (Emulated) return value of the command that was
     *                 being executed (or actually emulated as being executed)
     *                 at the time the timer expired.
     */
    virtual void GenericTimerFired( MHWRMHapticsPluginCallback* aService,
                                    TInt aCommandId,
                                    TUint8 aTransId,
                                    TInt aRetVal ) = 0;
    };


/**
 * Timer class that triggers command execution emulation. (This test
 * stub emulates the real adaptation layer haptics command execution
 * and the emulation "round" is triggered from this class).
 *
 * @since S60 5.1
 */
class CPluginTimer : public CTimer
    {
public:
    /**
     * Static method for instantiation.
     * The instantiation also adds the instance to active scheduler and
     * triggers start of the (embedded CTimer) timer.
     *
     * @param aInterval  Timeout value for command execution emulation.
     * @param aService   Pointer to the object that handles the responses
     *                   from command execution emulation. I.e., pointer
     *                   to an object outside the test stub plugin that
     *                   handles responses from all kinds of haptics plugins.
     *                   In practice this is the Haptics plugin manager.
     * @param aCommandId Id of the command whose execution is emulated.
     * @param aTransId   Id of the transaction to which the emulated command
     *                   belongs to. 
     * @param aRetVal    Return value of the emulated command.
     * @param aCallback  Pointer to the object that further handles the
     *                   command emulation (i.e., pointer to the actual test
     *                   stub plugin object). 
     */
    static CPluginTimer* NewL(
            const TTimeIntervalMicroSeconds32& aInterval, 
            MHWRMHapticsPluginCallback* aService,
            TInt aCommandId,
            TUint8 aTransId,
            TInt aRetVal,
            MPluginTimerCallback* aCallback);

    /**
     * Destructor
     */        
    ~CPluginTimer();
        
    /**
     * RunL handles (embedded CTimer) timer expirations.
     *
     * @see CActive
     * @see CTimer
     */
    void RunL();
        
    /**
     * Getter for transaction Id
     */
    TUint8 TransId() const;

private:
    /**
     * Constructor
     * 
     * @param aPriority
     * @param aService   Pointer to an object that implements
     *                   the MHWRMHapticsPluginCallback interface
     *                   and thus handles the callbacks from this 
     *                   whole test plugin.
     * @param aCommandId Id of the command for which the timer-based
     *                   command execution emulation is started.
     * @param aTransId   Transaction Id 
     * @param aRetVal
     * @param aCallback  Pointer to an object that implements the above 
     *                   declared MPluginTimerCallback interface. In 
     *                   practice this is the CHWRMHapticsTestPlugin
     *                   object.
     */
    CPluginTimer( TInt aPriority, 
                  MHWRMHapticsPluginCallback* aService,
                  TInt aCommandId,
                  TUint8 aTransId,
                  TInt aRetVal,
                  MPluginTimerCallback* aCallback );
                  
    /**
     * Two-phase construction. ConstructL is needed because parent
     * class (CTimer) requires two-phase construction.
     */
    void ConstructL();
          
private: // data
    
    /**
     * Pointer to an object that handles responses from this 
     * test plugin (i.e., in practice the Haptics plugin manager).
     */ 
    MHWRMHapticsPluginCallback* iService;  // Not owned
        
    /**
     * Id of the command whose execution is being emulated by
     * the test plugin
     */     
    TInt iCommandId;
 
    /**
     * Id of the transaction to which the command, whose execution
     * is being emulated by the test plugin, belongs to
     */
    TUint8 iTransId;

    /**
     * Return value that the emulated command execution is to return.
     */
    TInt iRetVal;
       
    /**
     * Pointer to an object that further handles the timer expiries
     * from this CPluginTimer object.
     * I.e, pointer to the actual test plugin object
     */   
    MPluginTimerCallback* iCallback;  // Not owned
    
    };


#endif // __HWRMHAPTICSTESTPLUGINTIMER_H_