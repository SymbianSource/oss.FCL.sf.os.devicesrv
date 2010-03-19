/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/


#ifndef __MT_RSTARTERSESSION_H__
#define __MT_RSTARTERSESSION_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES
#include <starterclient.h>

//  FORWARD DECLARATIONS


//  CLASS DEFINITION
/**
 *
 */
NONSHARABLE_CLASS( MT_RStarterSession )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static MT_RStarterSession* NewL();
        static MT_RStarterSession* NewLC();
        /**
         * Destructor
         */
        ~MT_RStarterSession();

    private:    // Constructors and destructors

        MT_RStarterSession();
        void ConstructL();

    private:    // New methods

        void SetupL();
        void SetupEmptyL();
        void Teardown();
        void TeardownEmpty();

        void T_RStarterSession_ConnectL();
        void T_RStarterSession_SetStateL();
        void T_RStarterSession_ResetL();
        void T_RStarterSession_ShutdownL();
        void T_RStarterSession_ResetNetworkL();
        void T_RStarterSession_IsRTCTimeValidL();
        void T_RStarterSession_ActivateRfForEmergencyCallL();
        void T_RStarterSession_DeactivateRfAfterEmergencyCallL();
        void T_RStarterSession_EndSplashScreenL();

    private:    // Data
		
        RStarterSession iRStarterSession;
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __MT_RSTARTERSESSION_H__

// End of file
