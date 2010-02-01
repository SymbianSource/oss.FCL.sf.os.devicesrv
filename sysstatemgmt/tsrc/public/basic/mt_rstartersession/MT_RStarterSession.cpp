/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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


//  CLASS HEADER
#include "MT_RStarterSession.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES
#include <starterclient.h>
#include <starter.hrh>

// CONSTRUCTION
MT_RStarterSession* MT_RStarterSession::NewL()
    {
    MT_RStarterSession* self = MT_RStarterSession::NewLC();
    CleanupStack::Pop();

    return self;
    }

MT_RStarterSession* MT_RStarterSession::NewLC()
    {
    MT_RStarterSession* self = new( ELeave ) MT_RStarterSession();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
MT_RStarterSession::~MT_RStarterSession()
    {
    Teardown();
    }

// Default constructor
MT_RStarterSession::MT_RStarterSession()
    {
    }

// Second phase construct
void MT_RStarterSession::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void MT_RStarterSession::SetupEmptyL(  )
    {
    }
    
void MT_RStarterSession::SetupL(  )
    {
    iRStarterSession.Connect();
    }
    
void MT_RStarterSession::Teardown(  )
    {
    iRStarterSession.Close();
    }
    
void MT_RStarterSession::TeardownEmpty(  )
    {
    }
    
void MT_RStarterSession::T_RStarterSession_ConnectL(  )
    {
    RStarterSession session;
    TInt err = session.Connect();
    
    EUNIT_PRINT( _L("Connect() err = %d"), err );
    EUNIT_ASSERT_DESC( err == KErrNone, "Incorrect return value");
    
    session.Close();
    }
    
void MT_RStarterSession::T_RStarterSession_SetStateL(  )
    {
    // Perhaps test also other values...
    TInt err = iRStarterSession.SetState( RStarterSession::ENormal );
    EUNIT_PRINT( _L("SetState() err = %d"), err );
    EUNIT_ASSERT_DESC( err == KErrNone, "Incorrect return value");
    }
    
void MT_RStarterSession::T_RStarterSession_ResetL(  )
    {
    // Perhaps test also other values...
    iRStarterSession.Reset( (RStarterSession::TResetReason)-1 );
    }
    
void MT_RStarterSession::T_RStarterSession_ShutdownL(  )
    {
    EUNIT_PRINT( _L("Calling Shutdown()") );
    iRStarterSession.Shutdown( );
    }
    
void MT_RStarterSession::T_RStarterSession_ResetNetworkL(  )
    {
    TInt err = iRStarterSession.ResetNetwork( );
    EUNIT_PRINT( _L("ResetNetwork() err = %d"), err );
    
    EUNIT_ASSERT_DESC( err == KErrNone, "Incorrect return value");
    }
    
void MT_RStarterSession::T_RStarterSession_IsRTCTimeValidL(  )
    {
    TBool valid = iRStarterSession.IsRTCTimeValid();
    EUNIT_PRINT( _L("IsRTCTimeValid() returned = %d"), valid );
    }
    
void MT_RStarterSession::T_RStarterSession_ActivateRfForEmergencyCallL(  )
    {
    TInt err = iRStarterSession.ActivateRfForEmergencyCall( );
    EUNIT_PRINT( _L("ActivateRfForEmergencyCall() err = %d"), err );
    // Do not check the return value since in some cases it is normal that
    // KErrGeneral(-2) is returned
    //EUNIT_ASSERT_DESC( err == KErrNone, "Incorrect return value" );
    }
    
void MT_RStarterSession::T_RStarterSession_DeactivateRfAfterEmergencyCallL(  )
    {
    TInt err = iRStarterSession.DeactivateRfAfterEmergencyCall( );
    EUNIT_PRINT( _L("DeactivateRfAfterEmergencyCall() err = %d"), err );
    EUNIT_ASSERT_DESC( err == KErrNone, "Incorrect return value" );

    // Activate RF again
    err = iRStarterSession.ActivateRfForEmergencyCall( );
    EUNIT_PRINT( _L("ActivateRfForEmergencyCall() err = %d"), err );
    // Do not check the return value since in some cases it is normal that
    // KErrGeneral(-2) is returned
    //EUNIT_ASSERT_DESC( err == KErrNone, "Incorrect return value" );
    }
    
void MT_RStarterSession::T_RStarterSession_EndSplashScreenL(  )
    {
    TInt err = iRStarterSession.EndSplashScreen( );
    EUNIT_PRINT( _L("EndSplashScreen() err = %d"), err );
    EUNIT_ASSERT_DESC( err == KErrNone, "Incorrect return value" );
    }
    
void MT_RStarterSession::T_RStarterSession_AddItemInStartListLL(  )
    {
    TPtrC8 md5hash( KNullDesC8 );
    TInt err = iRStarterSession.AddItemInStartListL(
        KNullDesC,
        EMonNone,
        EExecutableType,
        0,
        md5hash );
    EUNIT_PRINT( _L("AddItemInStartListL() err = %d"), err );
    EUNIT_ASSERT_DESC( err == KErrPermissionDenied, "Incorrect return value. SID check has failed" );
    }
    
void MT_RStarterSession::T_RStarterSession_AddItemInStartListL2L(  )
    {
    TInt err = iRStarterSession.AddItemInStartListL(
        KNullDesC,
        EMonNone,
        EExecutableType,
        0 );
    EUNIT_PRINT( _L("AddItemInStartListL() err = %d"), err );
    EUNIT_ASSERT_DESC( err == KErrPermissionDenied, "Incorrect return value. SID check has failed" );
    }
    
void MT_RStarterSession::T_RStarterSession_RemoveItemFromStartListLL(  )
    {
    TInt err = iRStarterSession.RemoveItemFromStartListL( KNullDesC );
    EUNIT_PRINT( _L("RemoveItemFromStartListL() err = %d"), err );
    EUNIT_ASSERT_DESC( err == KErrPermissionDenied, "Incorrect return value. SID check has failed" );
    }
    
void MT_RStarterSession::T_RStarterSession_SearchStartlistLL(  )
    {
    TInt err = iRStarterSession.SearchStartlistL( KNullDesC );
    EUNIT_PRINT( _L("SearchStartlistL() err = %d"), err );
    EUNIT_ASSERT_DESC( err == KErrPermissionDenied, "Incorrect return value. SID check has failed" );
    }
    

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    MT_RStarterSession,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "Connect - test0",
    "RStarterSession",
    "Connect - test0",
    "FUNCTIONALITY",
    SetupEmptyL, T_RStarterSession_ConnectL, TeardownEmpty)
    
EUNIT_TEST(
    "SetState - test1",
    "RStarterSession",
    "SetState - test1",
    "FUNCTIONALITY",
    SetupL, T_RStarterSession_SetStateL, Teardown)
    
EUNIT_TEST(
    "Reset - test2",
    "RStarterSession",
    "Reset - test2",
    "FUNCTIONALITY",
    SetupL, T_RStarterSession_ResetL, Teardown)
    
EUNIT_TEST(
    "ResetNetwork - test4",
    "RStarterSession",
    "ResetNetwork - test4",
    "FUNCTIONALITY",
    SetupL, T_RStarterSession_ResetNetworkL, Teardown)
    
EUNIT_TEST(
    "IsRTCTimeValid - test5",
    "RStarterSession",
    "IsRTCTimeValid - test5",
    "FUNCTIONALITY",
    SetupL, T_RStarterSession_IsRTCTimeValidL, Teardown)
    
EUNIT_TEST(
    "ActivateRfForEmergencyCall - test6",
    "RStarterSession",
    "ActivateRfForEmergencyCall - test6",
    "FUNCTIONALITY",
    SetupL, T_RStarterSession_ActivateRfForEmergencyCallL, Teardown)
    
EUNIT_TEST(
    "DeactivateRfAfterEmergencyCall - test7",
    "RStarterSession",
    "DeactivateRfAfterEmergencyCall - test7",
    "FUNCTIONALITY",
    SetupL, T_RStarterSession_DeactivateRfAfterEmergencyCallL, Teardown)
    
EUNIT_TEST(
    "EndSplashScreen - test8",
    "RStarterSession",
    "EndSplashScreen - test8",
    "FUNCTIONALITY",
    SetupL, T_RStarterSession_EndSplashScreenL, Teardown)
    
EUNIT_TEST(
    "AddItemInStartListL - test9",
    "RStarterSession",
    "AddItemInStartListL - test9",
    "FUNCTIONALITY",
    SetupL, T_RStarterSession_AddItemInStartListLL, Teardown)
    
EUNIT_TEST(
    "AddItemInStartListL - test10",
    "RStarterSession",
    "AddItemInStartListL - test10",
    "FUNCTIONALITY",
    SetupL, T_RStarterSession_AddItemInStartListL2L, Teardown)
    
EUNIT_TEST(
    "RemoveItemFromStartListL - test11",
    "RStarterSession",
    "RemoveItemFromStartListL - test11",
    "FUNCTIONALITY",
    SetupL, T_RStarterSession_RemoveItemFromStartListLL, Teardown)
    
EUNIT_TEST(
    "SearchStartlistL - test12",
    "RStarterSession",
    "SearchStartlistL - test12",
    "FUNCTIONALITY",
    SetupL, T_RStarterSession_SearchStartlistLL, Teardown)
/*    
EUNIT_TEST(
    "Shutdown - test3",
    "RStarterSession",
    "Shutdown - test3",
    "FUNCTIONALITY",
    SetupL, T_RStarterSession_ShutdownL, Teardown)*/
    
EUNIT_END_TEST_TABLE

//  END OF FILE
