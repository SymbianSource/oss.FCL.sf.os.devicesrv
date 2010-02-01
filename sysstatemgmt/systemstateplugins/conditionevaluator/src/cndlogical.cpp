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
// Name        : resourcecondition.cpp
// Part of     : System Startup / Condition
// Implementation of ResourceCondition class
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
// File renamed from resourcecondition.cpp to cndlogical.cpp as part of Core OS transfer.
//



#include "cndlogical.h"
#include "ssmpanic.h"

CLogicalNot::CLogicalNot(CConditionEvaluateBase* aExpr) : iExpr(aExpr)
    {
    }

CLogicalNot::~CLogicalNot()
    {
    delete iExpr;
    }

TBool CLogicalNot::EvaluateL() const
    {
    __ASSERT_DEBUG(NULL != iExpr, PanicNow(KPanicCLogicalNot, ENullPtr));
    TBool retVal = !(iExpr->EvaluateL());
    return retVal;
    }

CLogicalAnd::CLogicalAnd(CConditionEvaluateBase* aLhs, CConditionEvaluateBase* aRhs)
  : iLhs(aLhs),
    iRhs(aRhs)
    {
    }

CLogicalAnd::~CLogicalAnd()
    {
    delete iLhs;
    delete iRhs;
    }

TBool CLogicalAnd::EvaluateL() const
    {
    __ASSERT_DEBUG(NULL != iLhs, PanicNow(KPanicCLogicalAnd, ENullPtr));
    __ASSERT_DEBUG(NULL != iRhs, PanicNow(KPanicCLogicalAnd, ENullPtr));

    TBool retVal = (iLhs->EvaluateL() && iRhs->EvaluateL());
    return retVal;
    }

CLogicalOr::CLogicalOr(CConditionEvaluateBase* aLhs, CConditionEvaluateBase* aRhs)
  : iLhs(aLhs),
    iRhs(aRhs)
    {
    }

CLogicalOr::~CLogicalOr()
    {
    delete iLhs;
    delete iRhs;
    }

TBool CLogicalOr::EvaluateL() const
    {
    __ASSERT_DEBUG(NULL != iLhs, PanicNow(KPanicCLogicalOr, ENullPtr));
    __ASSERT_DEBUG(NULL != iRhs, PanicNow(KPanicCLogicalOr, ENullPtr));

    TBool retVal = (iLhs->EvaluateL() || iRhs->EvaluateL());
    return retVal;
    }
