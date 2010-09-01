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


#ifndef __MT_SYSLANGUTIL_H__
#define __MT_SYSLANGUTIL_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>
#include <f32file.h>

//  CLASS DEFINITION
/**
 * SysLangUtil API test
 */
NONSHARABLE_CLASS( MT_SysLangUtil )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors
        /**
         * Two phase construction
         */
        static MT_SysLangUtil* NewL();
        static MT_SysLangUtil* NewLC();
        /**
         * Destructor
         */
        ~MT_SysLangUtil();

    private:    // Constructors and destructors
        MT_SysLangUtil();
        void ConstructL();

    private:    // New methods
         void SetupL();        
         void Teardown();        
         void T_SysLangUtil_IsValidLanguageL();        
         void T_SysLangUtil_GetDefaultLanguageL();        
         void T_SysLangUtil_GetInstalledLanguagesL();        
         void T_SysLangUtil_RestoreSIMLanguageL();
        
    private:    // Data		
        EUNIT_DECLARE_TEST_TABLE;
        
        // Own: file server session
        RFs iFs;
    };

#endif      //  __MT_SYSLANGUTIL_H__
