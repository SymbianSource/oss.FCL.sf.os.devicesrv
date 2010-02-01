/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


//  CLASS HEADER
#include "MT_SysLangUtil.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <syslangutil.h>

// CONSTRUCTION
MT_SysLangUtil* MT_SysLangUtil::NewL()
    {
    MT_SysLangUtil* self = MT_SysLangUtil::NewLC();
    CleanupStack::Pop();
    return self;
    }

MT_SysLangUtil* MT_SysLangUtil::NewLC()
    {
    MT_SysLangUtil* self = new( ELeave ) MT_SysLangUtil();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
MT_SysLangUtil::~MT_SysLangUtil()
    {
    Teardown();
    }

// Default constructor
MT_SysLangUtil::MT_SysLangUtil()
    {
    }

// Second phase construct
void MT_SysLangUtil::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS
void MT_SysLangUtil::SetupL()
    {
    User::LeaveIfError( iFs.Connect() );    
    }

void MT_SysLangUtil::Teardown()
    {
    iFs.Close();
    }
    
void MT_SysLangUtil::T_SysLangUtil_IsValidLanguageL()
    {
    // Get default language 
    TInt language = 0;
    TInt result = SysLangUtil::GetDefaultLanguage( language );
    EUNIT_ASSERT_EQUALS( result, KErrNone );
    
    // Default language should be valid language
    TBool valid = SysLangUtil::IsValidLanguage( language );
    EUNIT_ASSERT( valid );
    
    // Default language should be valid language
    valid = SysLangUtil::IsValidLanguage( language, &iFs );
    EUNIT_ASSERT( valid );        
    }
    
void MT_SysLangUtil::T_SysLangUtil_GetDefaultLanguageL()
    {
    // Get default language 
    TInt language = 0;
    TInt result = SysLangUtil::GetDefaultLanguage( language );
    EUNIT_ASSERT_EQUALS( result, KErrNone );
    
    result = SysLangUtil::GetDefaultLanguage( language, &iFs );
    EUNIT_ASSERT_EQUALS( result, KErrNone );    
    }
    
void MT_SysLangUtil::T_SysLangUtil_GetInstalledLanguagesL()
    {
    // Create array for languages
    CArrayFixFlat<TInt>* array = new( ELeave ) CArrayFixFlat<TInt>( 3 );
    CleanupStack::PushL( array );

    // Get installed languages
    TInt res = SysLangUtil::GetInstalledLanguages( array );
    EUNIT_ASSERT_EQUALS( res, KErrNone );
    
    array->Reset();
    res = SysLangUtil::GetInstalledLanguages( array, &iFs );
    EUNIT_ASSERT_EQUALS( res, KErrNone );    
    
    TInt count = array->Count();
    EUNIT_ASSERT(  count > 0 );
        
    // Assert that each received language is a valid language
    for( TInt i = 0; i < count; ++i )
    	{
    	EUNIT_ASSERT( SysLangUtil::IsValidLanguage( array->At( i ) ) );
    	}
    
    CleanupStack::PopAndDestroy( array );
    }

void MT_SysLangUtil::T_SysLangUtil_RestoreSIMLanguageL()
    {
    // Get default language 
    TInt language = 0;
    TInt result = SysLangUtil::GetDefaultLanguage( language );
    EUNIT_ASSERT_EQUALS( result, KErrNone );
    
    // Default language should be valid language
    TInt res = SysLangUtil::RestoreSIMLanguage( language );    
    EUNIT_ASSERT_EQUALS( res, KErrNone );

    // Default language should be valid language
    res = SysLangUtil::RestoreSIMLanguage( language, &iFs );    
    EUNIT_ASSERT_EQUALS( res, KErrNone );    
    }
    
//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    MT_SysLangUtil,
    "SysLangUtil API Test",
    "MODULE" )

EUNIT_TEST(
    "#1 Test IsValidLanguage()",
    "SysLangUtil",
    "IsValidLanguage",
    "FUNCTIONALITY",
    SetupL, T_SysLangUtil_IsValidLanguageL, Teardown)
    
EUNIT_TEST(
    "#2 Test GetDefaultLanguage()",
    "SysLangUtil",
    "GetDefaultLanguage",
    "FUNCTIONALITY",
    SetupL, T_SysLangUtil_GetDefaultLanguageL, Teardown)
    
EUNIT_TEST(
    "#3 Test GetInstalledLanguages()",
    "SysLangUtil",
    "GetInstalledLanguages",
    "FUNCTIONALITY",
    SetupL, T_SysLangUtil_GetInstalledLanguagesL, Teardown)
    
EUNIT_TEST(
    "#4 Test RestoreSIMLanguage()",
    "SysLangUtil",
    "RestoreSIMLanguage",
    "FUNCTIONALITY",
    SetupL, T_SysLangUtil_RestoreSIMLanguageL, Teardown)    
EUNIT_END_TEST_TABLE
