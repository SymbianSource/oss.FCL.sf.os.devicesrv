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
// Name        : conditiononcentrep.cpp
// Part of     : System Startup / Condition
// Implementation of CConditionOnCentRep and
// CConditionOnCentRepFlag classes.
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
// File renamed from conditiononcentrep.cpp to cndcentralrepository.cpp as part of Core OS transfer.
//



#include "cndcentralrepository.h"
#include <centralrepository.h>

CCndCentralRepository::CCndCentralRepository(const TConditionCheckType aConditionCheckType, const TInt aRepositoryId,
        			const TUint aKey, const TInt aCndValue): iRepositoryId(aRepositoryId),
        		    iKey(aKey),
        		    iCndValue(aCndValue),
        		    iConditionCheckType(aConditionCheckType)
	{

	}

CCndCentralRepository* CCndCentralRepository::NewL(const TConditionCheckType aConditionCheckType,
								const TInt aRepositoryId, const TUint aKey, const TInt aCndValue)
	{
	CCndCentralRepository* self = new (ELeave)CCndCentralRepository(aConditionCheckType,
																aRepositoryId, aKey, aCndValue);
	return self;
	}

TBool CCndCentralRepository::EvaluateL() const
    {
    CRepository* repository = CRepository::NewLC(TUid::Uid(iRepositoryId));
    TInt value = 0;
    User::LeaveIfError(repository->Get(iKey, value));
    TBool retVal = EFalse;
    if (iConditionCheckType == ECompareValue)
        {
        retVal = (value == iCndValue);
        }
    else
        {
        retVal = ((value & iCndValue) == iCndValue);
        }

    CleanupStack::PopAndDestroy(repository);
    return retVal;
    }

CCndCentralRepository::~CCndCentralRepository()
	{
	}

