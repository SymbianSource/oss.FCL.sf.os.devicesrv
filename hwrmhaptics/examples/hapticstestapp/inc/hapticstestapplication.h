/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Application class definition.
*
*/


#ifndef __HAPTICSTEST_APPLICATION_H__
#define __HAPTICSTEST_APPLICATION_H__

#include <aknapp.h>

/**
 * An instance of CHapticsTestApplication is the application part of the AVKON
 * application framework for the hapticstest example application
 */
class CHapticsTestApplication : public CAknApplication
    {
public:  // from CAknApplication

   /**
    *
    * Returns the application DLL UID value
    * @return the UID of this Application/Dll
    */
    TUid AppDllUid() const;

protected: // from CAknApplication

   /** 
    * Create a CApaDocument object and return a pointer to it
    * @return a pointer to the created document
    */
    CApaDocument* CreateDocumentL();
    };

#endif // __HAPTICSTEST_APPLICATION_H__
