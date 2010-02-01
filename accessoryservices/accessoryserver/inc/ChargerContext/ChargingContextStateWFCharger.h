/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Charging Context Controller's "Wait For Charger" state.
*
*/



#ifndef C_CHARGINGCONTEXTSTATEWFCHARGER_H
#define C_CHARGINGCONTEXTSTATEWFCHARGER_H


#include "FiniteStateMachine.h"
#include "AccSrvChargingContextController.h"


class CAccSrvChargingContextController;



/**
 * "Wait For Charger" state implementation for Charging Context 
 * Controller.
 * State waits until charger is connected before moving to
 * next state "charging".
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib AccServer.exe
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CChargingContextStateWFCharger ): public CBase, public MFSMState
    {


public:

    
    /**
     * Two-phased constructor.
     * @param aFSMBody Body of the Finite State Machine.
     * @param aChargingContextController Host class object that owns the FSM.
     */
    static CChargingContextStateWFCharger* NewL(
            MFSMBody* aFSMBody, 
            CAccSrvChargingContextController* aChargingContextController );
    

    /**
    * Destructor.
    */
    virtual ~CChargingContextStateWFCharger();

    
// from base class MFSMState


    /**
     * From MFSMState.
     * Function is called when entering the state. State can perform
     * necessary entry actions if any.
     *
     * @since S60 5.1
     * @param none. 
     * @return aFSMBody Interface pointer of a FSM body.
     */
    virtual void Enter();

    /**
     * From MFSMState.
     * Function is called by FSM Body whenever an event that might have 
     * impact on the state has been fired. Implementation has to found
     * out what event has occurred and decide whether it enforces
     * transition to another state.
     *
     * @since S60 5.1
     * @param none. 
     * @return none.
     */
    virtual void Input();

    /**
     * From MFSMState.
     * Function returns state's identification.
     *
     * @since S60 5.1
     * @param none. 
     * @return TFSMState State's identification.
     */
    virtual TFSMState Id();

    /**
     * From MFSMState.
     * Function returns state's name.
     *
     * @since S60 5.1
     * @param none. 
     * @return TPtrC Name of the state.
     */
    virtual TPtrC Name();

    
protected:


private:


    /**
     * C++ default constructor.
     */
    CChargingContextStateWFCharger();

    /**
     * By default Symbian OS constructor is private.
     * @param aFSMBody Body of the Finite State Machine.
     * @param aChargingContextController Host class object that owns the FSM.
     */
    void ConstructL(
            MFSMBody* aFSMBody, 
            CAccSrvChargingContextController* aChargingContextController );

private: // data

    /**
     * Host class object that owns the FSM.
     * Not own.
     */
    CAccSrvChargingContextController* iChargingContextController;

    /**
     * FSM body.
     * Not own.
     */
    MFSMBody* iFSMBody;

    };

#endif // C_CHARGINGCONTEXTSTATEWFCHARGER_H
