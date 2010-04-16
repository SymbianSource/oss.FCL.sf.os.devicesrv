/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: PSKey definition.
*
*/
#ifndef __SSMLOCALEPSKEYS_H__
#define __SSMLOCALEPSKEYS_H__
#include <e32base.h>

/** Publish and Subscribe key used for observing Collation code changes. */
static const TInt KSSMCollationPSKey = 501;

/** Publish and Subscribe key used for observing Region code changes. */
static const TInt KSSMRegionPSKey = 502;

/** Publish and Subscribe key used for observing UI Language code changes. */
static const TInt KSSMUILanguagePSKey = 503;

#endif // __SSMLOCALEPSKEYS_H__
