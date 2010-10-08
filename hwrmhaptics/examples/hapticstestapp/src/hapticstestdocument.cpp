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
* Description:  Document class implementation.
*
*/


#include "hapticstestappui.h"
#include "hapticstestdocument.h"

// ---------------------------------------------------------
// Standard Symbian OS construction sequence
// ---------------------------------------------------------
//
CHapticsTestDocument* CHapticsTestDocument::NewL(CEikApplication& aApp)
    {
    CHapticsTestDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// Standard Symbian OS construction sequence
// ---------------------------------------------------------
//
CHapticsTestDocument* CHapticsTestDocument::NewLC(CEikApplication& aApp)
    {
    CHapticsTestDocument* self = new (ELeave) CHapticsTestDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------
//
void CHapticsTestDocument::ConstructL()
    {
    // no implementation required
    }    

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
//
CHapticsTestDocument::CHapticsTestDocument(CEikApplication& aApp) 
    : CAknDocument(aApp) 
    {
    // no implementation required
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
//
CHapticsTestDocument::~CHapticsTestDocument()
    {
    // no implementation required
    }

// ---------------------------------------------------------
// Creates AppUi component.
// ---------------------------------------------------------
//
CEikAppUi* CHapticsTestDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new (ELeave) CHapticsTestAppUi;
    return appUi;
    }

