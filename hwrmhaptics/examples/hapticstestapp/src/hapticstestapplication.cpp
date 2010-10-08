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
* Description:  Application class implementation.
*
*/


#include "hapticstestdocument.h"
#include "hapticstestapplication.h"

// UID for the application, this should correspond to the uid defined in the mmp file
static const TUid KUidhapticstestApp = {0x0AD0595A};

// ---------------------------------------------------------
// Creates the document component of the application.
// ---------------------------------------------------------
//
CApaDocument* CHapticsTestApplication::CreateDocumentL()
    {  
    // Create an hapticstest document, and return a pointer to it
    CApaDocument* document = CHapticsTestDocument::NewL(*this);
    return document;
    }

// ---------------------------------------------------------
// Returns application uid.
// ---------------------------------------------------------
//
TUid CHapticsTestApplication::AppDllUid() const
    {
    // Return the UID for the hapticstest application
    return KUidhapticstestApp;
    }

