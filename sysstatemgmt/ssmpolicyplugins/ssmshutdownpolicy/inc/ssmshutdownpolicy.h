/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Declaration of CSsmShutdownPolicy class.
*
*/

#ifndef C_SSMSHUTDOWNPOLICY_H
#define C_SSMSHUTDOWNPOLICY_H

#include "ssmstatepolicybase.h"

/**
*  SSM shutdown policy plugin class
*
*/
NONSHARABLE_CLASS( CSsmShutdownPolicy ) : public CSsmStatePolicyBase
    {

public:

    /**
    * Constructs a CSsmShutdownPolicy object.
    *
    * @return The new object
    */
    IMPORT_C static MSsmStatePolicy* NewL();

    /**
    * Destructor.
    */
    virtual ~CSsmShutdownPolicy();

private: // From MSsmStatePolicy

	/**
    * Used to determine the next minor state transition.
	*
	* @param aCurrentTransition Contains the last executed state transition.
	* @param aReason Contains the reason as given by the request
	* @param aError Contains the completion code from the last executed sub-state transition
	* @param aSeverity Contains the severity of the failed command in case the sub-state transition ended with an error
	* @param aNextState The next System State to head for, if there is one
	* @return ETrue if aNextState contains another System State to head for, or
    *         EFalse if there is no further transitions to do.
	*/
    TBool GetNextState(
        TSsmState aCurrentTransition,
        TInt aReason,
        TInt aError,
        TInt aSeverity,
        TSsmState& aNextState );

private: // From CSsmPolicyBase

    void GetCommandListPathL( TDes& aCmdListPath );

private: // From CSsmStatePolicyBase

    TBool IsAllowedTargetState( const TSsmStateTransition& aRequest ) const;
    TUint16 TargetSubState( const TUint16 aRequestedSubState );

private:

    void StoreStartupReasonL( const TInt aReasonCode );

private:

    /**
    * First phase constructor.
    */
    CSsmShutdownPolicy();

    };

#endif // C_SSMSHUTDOWNPOLICY_H
