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
* Description:  Document class definition.
*
*/


#ifndef __HAPTICSTEST_DOCUMENT_H__
#define __HAPTICSTEST_DOCUMENT_H__

#include <akndoc.h>

// Forward references
class CHapticsTestAppUi;
class CEikApplication;


/**
 * An instance of class ChapticstestDocument is the Document part of the AVKON
 * application framework for the hapticstest example application
 */
class CHapticsTestDocument : public CAknDocument
    {
public:

   /**
    * Construct a ChapticstestDocument for the AVKON application aApp 
    * using two phase construction, and return a pointer to the created object
    *
    * @param aApp application creating this document
    * @return a pointer to the created instance of ChapticstestDocument
    */
    static CHapticsTestDocument* NewL(CEikApplication& aApp);

   /**
    * Construct a ChapticstestDocument for the AVKON application aApp 
    * using two phase construction, and return a pointer to the created object
    *
    * @param aApp application creating this document
    * @return a pointer to the created instance of ChapticstestDocument
    */
    static CHapticsTestDocument* NewLC(CEikApplication& aApp);

   /**
    * Destroy the object and release all memory objects
    */
    ~CHapticsTestDocument();

public: // from CAknDocument

   /**
    * Create a ChapticstestAppUi object and return a pointer to it
    *
    * @return a pointer to the created instance of the AppUi created
    */
    CEikAppUi* CreateAppUiL();

private:

   /**
    * Perform the second phase construction of a ChapticstestDocument object
    */
    void ConstructL();

   /**
    * Perform the first phase of two phase construction
    *
    * @param application creating this document
    */
    CHapticsTestDocument(CEikApplication& aApp);

    };


#endif // __HAPTICSTEST_DOCUMENT_H__
