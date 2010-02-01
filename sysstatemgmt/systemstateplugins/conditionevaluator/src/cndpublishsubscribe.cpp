// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : conditiononpubsub.cpp
// Part of     : System Startup / Condition
// Implementation of CConditionOnPubSub and CConditionOnPubSubFlag
// classes
// Version     : %version: 1 %
// This material, including documentation and any related computer
// programs, is protected by copyright controlled by Nokia.  All
// rights are reserved.  Copying, including reproducing, storing,
// adapting or translating, any or all of this material requires the
// prior written consent of Nokia.  This material also contains
// confidential information which may not be disclosed to others
// without the prior written consent of Nokia.
// Template version: 4.1
// Nokia Core OS *
// File renamed from conditiononpubsub.cpp to cndpublishsubscribe.cpp as part of Core OS transfer.
//



#include "cndpublishsubscribe.h"
#include <e32property.h> 

CCndPublishSubscribe::CCndPublishSubscribe(const TConditionCheckType aConditionCheckType, const TInt aCategory,
        			const TUint aKey, const TInt aCndValue): iConditionCheckType(aConditionCheckType),
        		    iCategory(aCategory),
        		    iKey(aKey),
        		    iCndValue(aCndValue)
	{

	}

CCndPublishSubscribe::~CCndPublishSubscribe()
	{
	
	}

CCndPublishSubscribe* CCndPublishSubscribe::NewL(const TConditionCheckType aConditionCheckType,
								const TInt aCategory, const TUint aKey, const TInt aCndValue)
	{
	CCndPublishSubscribe* self = new (ELeave)CCndPublishSubscribe(aConditionCheckType,
																aCategory, aKey, aCndValue);
	return self;
	}

TBool CCndPublishSubscribe::EvaluateL() const
    {
    TInt value = 0;
    User::LeaveIfError(RProperty::Get(TUid::Uid(iCategory), iKey, value));
    TBool retVal = EFalse;
    if (iConditionCheckType == ECompareValue)
        {
        retVal = (value == iCndValue);
        }
    else
        {
        retVal = ((value & iCndValue) == iCndValue);
        }
    return retVal;
    }
