/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of CXXXPlugin class 
*	             member functions.
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "remconplugin.h"

// CONSTANTS
const TImplementationProxy ImplementationTable[] = 
    {
        { { 0x10200c77 }, (TProxyNewLPtr) CRemConPlugin::NewL }
    };

// ================= OTHER EXPORTED FUNCTIONS ==============

// ---------------------------------------------------------
// ImplementationGroupProxy
// Returns: TImplementationProxy*: ?description
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof ( ImplementationTable ) / sizeof ( TImplementationProxy );

	return ImplementationTable;
	}
// End of File
