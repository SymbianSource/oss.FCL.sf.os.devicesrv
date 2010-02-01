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
//

/**
 @internalComponent
 @released
*/

#include "ssmuiproviderdll.h"
#include "ssmpanic.h"
#include <e32property.h>
#include <f32file.h>

const TUid KPSStartupUid = {0x2000E65E};
const TUid KSecurityPinNotifierUid = {0x2000E667};
const TUid KScreenOutputChannel = {0x10009D48};
const TUid KEmergencyCallPropertyCategory = {0x2001032C};

const TUint KEmergencyCallPropertyKey = 0x0101;
const TUint KSimStatusPropertyKey = 0x0102;

const TUid KSecurityStatusPropertyCategory =  {0x2000E664};

const TUid KRFStatusPropertyCategory = {0x2000D75B};
const TUint KRFStatusPropertyKey = 0x2001D2A9;
const TUid KValidateRTCPropertyCategory = {0x2000D75B};
const TUint KValidateRTCPropertyKey = 0x2001D2AB;

_LIT(KTsyModuleName, "mm.tsy");
_LIT(KTsyPhoneName, "GsmPhone1");

CSsmUiSpecific::CSsmUiSpecific()
: iReferenceCount(1)
	{
	}

EXPORT_C CSsmUiSpecific::~CSsmUiSpecific()
	{
	}

EXPORT_C TUid CSsmUiSpecific::StartupPSUid()
	{
	return KPSStartupUid;
	}

EXPORT_C TUid CSsmUiSpecific::SecurityPinNotifierUid()
	{
	return KSecurityPinNotifierUid;
	}

EXPORT_C TUint CSsmUiSpecific::EmergencyCallPropertyKey()
	{
	return KEmergencyCallPropertyKey;
	}

EXPORT_C TUid CSsmUiSpecific::EmergencyCallPropertyCategory()
	{
	return KEmergencyCallPropertyCategory;
	}

EXPORT_C TBool CSsmUiSpecific::IsSimSupported()
	{
	return ETrue;
	}

EXPORT_C void CSsmUiSpecific::SetSecurityStatus(const TStrtSecurityStatus& aSecurityStatus)
	{
	iStrtSecurityStatus = aSecurityStatus;
	}

EXPORT_C TStrtSecurityStatus CSsmUiSpecific::SecurityStatus() const
	{
	return iStrtSecurityStatus;
	}

EXPORT_C CSsmUiSpecific* CSsmUiSpecific::InstanceL()
	{
	CSsmUiSpecific* self;

	//Check Tls data
	if (NULL == Dll::Tls())
		{
		//Instantiate CSsmUiSpecific if TLS is null
		self = new (ELeave) CSsmUiSpecific();
		CleanupStack::PushL(self);
		
		//Copy CSsmUiSpecific pointer in TLS
		User::LeaveIfError(Dll::SetTls(self));
		CleanupStack::Pop(self);
		}
	else
		{
		//CSsmUiSpecific has already been instantiated
		self = static_cast<CSsmUiSpecific*>(Dll::Tls());
		++self->iReferenceCount;
		}
	return self;
	}

EXPORT_C void CSsmUiSpecific::Release()
	{
	   TAny* tlsPtr = Dll::Tls();
	__ASSERT_DEBUG(NULL != tlsPtr, User::Panic(KPanicSsmUiSpecific, KErrNotFound));
 
    CSsmUiSpecific* self = static_cast<CSsmUiSpecific*>(tlsPtr);
    if (0 == --self->iReferenceCount)
        {
        Dll::FreeTls();
        delete self;
        }
	}

EXPORT_C TUid CSsmUiSpecific::ScreenOutputChannelUid()
	{
	return KScreenOutputChannel;
	}

EXPORT_C TUint CSsmUiSpecific::SimStatusPropertyKey()
	{
	return KSimStatusPropertyKey;
	}

EXPORT_C TBool CSsmUiSpecific::IsSimStateChangeAllowed()
	{
	return ETrue;
	}

EXPORT_C TBool CSsmUiSpecific::IsAmaStarterSupported()
	{
	return ETrue;
	}

EXPORT_C HBufC* CSsmUiSpecific::GetTsyModuleNameL()
	{
	HBufC* tstModuleName = KTsyModuleName().AllocL();
	return tstModuleName;
	}

EXPORT_C TUid CSsmUiSpecific::StarterPSUid()
	{
	return KSecurityStatusPropertyCategory;
	}

EXPORT_C HBufC* CSsmUiSpecific::PhoneTsyNameL()
	{
	HBufC* tsyPhoneName = KTsyPhoneName().AllocL();
	return tsyPhoneName;
	}

EXPORT_C TBool CSsmUiSpecific::IsSimPresent()
	{
	return ETrue;
	}

EXPORT_C TBool CSsmUiSpecific::IsSimlessOfflineSupported()
	{
	return ETrue;
	}

EXPORT_C TBool CSsmUiSpecific::IsNormalBoot()
	{
	return ETrue;
	}

EXPORT_C TBool CSsmUiSpecific::IsSimChangedReset()
	{
	return ETrue;
	}

EXPORT_C TUint CSsmUiSpecific::RFStatusPropertyKey()
	{
	return KRFStatusPropertyKey;
	}

EXPORT_C TUid CSsmUiSpecific::RFStatusPropertyCategory()
	{
	return KRFStatusPropertyCategory;
	}
EXPORT_C TUint CSsmUiSpecific::ValidateRTCPropertyKey()
    {
    return KValidateRTCPropertyKey;
    }

EXPORT_C TUid CSsmUiSpecific::ValidateRTCPropertyCategory()
    {
    return KValidateRTCPropertyCategory;
    }

EXPORT_C TInt CSsmUiSpecific::PhoneMemoryRootDriveId()
	{
	return EDriveC;
	}

