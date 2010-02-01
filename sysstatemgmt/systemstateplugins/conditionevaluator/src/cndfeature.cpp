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
// Name        : conditiononfeature.cpp
// Part of     : System Startup / Starter
// Implementation of CConditionOnFeature class
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
// File renamed from conditiononfeature.cpp to cndfeature.cpp as part of Core OS transfer.
//



#include "cndfeature.h"
#include <featmgr/featurecontrol.h>
#include <featmgr/featmgr.h> 

CCndFeature::CCndFeature(const TConditionCheckType aConditionCheckType, const TInt aFeatureId, const TUint32 aCndValue)
						: iConditionCheckType( aConditionCheckType ), iFeatureId ( aFeatureId ),
	        		    iCndValue( aCndValue )
	{
	}

CCndFeature* CCndFeature::NewL(const TConditionCheckType aConditionCheckType, const TInt aFeatureId)
	{
	const TUint nullFeatureData = 0;

	//Passing the condition value as 0 as it is only required while checking the feature data.
	return NewL(aConditionCheckType, aFeatureId, nullFeatureData);
	}

CCndFeature* CCndFeature::NewL(const TConditionCheckType aConditionCheckType, const TInt aFeatureId, const TUint32 aCndValue)
	{
	CCndFeature* self = new (ELeave)CCndFeature(aConditionCheckType, aFeatureId, aCndValue);
	return self;
	}

TBool CCndFeature::EvaluateL() const
    {
    RFeatureControl featureControl;
    User::LeaveIfError(featureControl.Open());

    TUid featureUid = {iFeatureId};
    TBool retVal = EFalse;
	TFeatureEntry featureEntry(featureUid);
	TInt result = featureControl.FeatureSupported(featureUid);

	if (KFeatureSupported  == result)
		{
		if (ECompareFeatureData == iConditionCheckType)
	    	{
	    	if(iCndValue == featureEntry. FeatureData())
	    		{
	    		retVal = ETrue;
	    		}
	    	}
		else
			{
			retVal = ETrue;
			}
		}

    featureControl.Close();
    return retVal;
    }

CCndFeature::~CCndFeature()
	{
	}

