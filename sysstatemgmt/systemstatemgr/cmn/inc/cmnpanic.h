// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//

#if !defined (__CMNPANICCODES_H__)
#define __CMNPANICCODES_H__

#include <e32std.h>

_LIT(KPanicSsmCmn, "Ssm-Cmn");

enum TCmnPanicCodes
	{
	//CSsmPropertyMonitor errors
	ECmnPropMonError1 = 1,
	
	//CSsmStateMonitor errors
	ECmnStateMonError1 = 2,
	
	//CSsmSystemWideProperty errors
	ECmnSwpSession1 = 3,
	
	//SsmStateAwareSession errors
	ECmnErrState = 4,
	ECmnErrRqstStateNotif = 5,
	ECmnErrRqstStateNotifCancel = 6,
	ECmnErrAcknldgStateNotif = 7,
	ECmnErrAcknldgRqstStateNotif = 8,
	ECmnErrStateMon = 9,
	ECmnErrDeferAcknNotif = 10,
	ECmnErrCancelDeferNotif = 11,
	
	//TSsmState errors
	ECmnStateMaxValue1 = 12,		//Out of range value supplied to the main state in TSsmState::TSsmState()
	ECmnStateMaxValue2 = 13,		//Out of range value supplied to the main state in TSsmState::TSsmState()
	ECmnStateMaxValue3 = 14,		//Out of range value supplied to the main state in TSsmState::Set()
	ECmnStateMaxValue4 = 15,		//Out of range value supplied to the main state in TSsmState::SetFromInt()
	
	//CSsmDeferralMonitor
	ECmnErrDeferNotif = 16
	};

#endif // __CMNPANICCODES_H__
