/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Haptics adaptation test plugin ECom implementation proxy definition file
*
*/


#include <e32std.h>
#include <ecom/ImplementationProxy.h>

#include "hwrmhapticstestplugin.h"

/**
 * Mapping of the interface implementation UIDs to implementation factory functions
 */
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x2001CB6E, CHWRMHapticsTestPlugin::NewL )
    };

/**
 * Exported proxy for instantiation method resolution.
 */
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }
