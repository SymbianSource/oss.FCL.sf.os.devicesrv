/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : conditiononpubsub.h
* Part of     : System Startup / Condition
* Interface   : None
* Declaration of CConditionOnPubSub and CConditionOnPubSubFlag
* classes
* Version     : %version: 1 %
* This material, including documentation and any related computer
* programs, is protected by copyright controlled by Nokia.  All
* rights are reserved.  Copying, including reproducing, storing,
* adapting or translating, any or all of this material requires the
* prior written consent of Nokia.  This material also contains
* confidential information which may not be disclosed to others
* without the prior written consent of Nokia.
* Template version: 4.0
* Nokia Core OS *
* File renamed from conditiononpubsub.h to conditionevaluatebase.h as part of Core OS transfer.
*
*/




/**
 @file
 @internalComponent
 @released
*/

#ifndef __CONDITIONEVALUATEBASE_H__
#define __CONDITIONEVALUATEBASE_H__

#include <e32std.h>
#include <e32base.h>

enum TConditionCheckType
    {
    ECompareNone,
    ECompareValue = 1,		// value comparison
    ECompareBitwiseAnd,		// Check if a bit is set
    ECompareFeatureData		// compare feature data value
    };

class CConditionEvaluateBase : public CBase
	{
public:
	virtual TBool EvaluateL() const = 0;

	};

#endif // __CONDITIONEVALUATEBASE_H__
