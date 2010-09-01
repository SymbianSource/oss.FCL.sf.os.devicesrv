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
* Description:
* Declaration of SsmLanguageLoader class.
*
*/

#ifndef SSMLANGUAGELOADER_H
#define SSMLANGUAGELOADER_H

#include <e32cmn.h>

/**
*  Static function managing operations related to loading locale based on
*  language code.
*
*  @lib None
*  @since S60 3.2
*/
class SsmLanguageLoader
    {

public:

    /**
    * Store language code to HAL and load corresponding locale DLL.
    *
    * @since S60 3.2
    * @param aLanguage Language code identifying which locale DLL to load.
    * @return KErrNone if loading was successful, one of Symbian error codes
    * otherwise.
    */
    static TInt LoadLanguage( const TInt aLanguage );

private:

    /**
    * Store language code to HAL and load corresponding locale DLL.
    *
    * @since S60 3.2
    * @param aLanguage Language code identifying which locale DLL to load.
    * @return KErrNone if successful, one of Symbian error codes otherwise.
    */
    static TInt StoreLanguageToHal( const TInt aLanguage );

    /**
    * Find the correct locale DLL to use by language code and take it to
    * use.
    *
    * @param aLanguageCode The language code to find a locale dll for.
    * @return KErrNone if successful, one of Symbian error codes otherwise.
    */
    static TInt LoadLocaleDll( const TInt aLanguage );

    /**
    * Take a locale DLL to use by name.
    *
    * @param aLocaleDllName The name of the locale DLL to load.
    * @return KErrNone if successful, one of Symbian error codes otherwise.
    */
    static TInt ChangeLocale( const TDesC& aLocaleDllName );

    };


#endif // SSMLANGUAGELOADER_H
