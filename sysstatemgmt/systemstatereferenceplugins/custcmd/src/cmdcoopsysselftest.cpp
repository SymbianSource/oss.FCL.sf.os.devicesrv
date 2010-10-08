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

/**
 @file
 @internalComponent
 @released
*/

#include "cmdcoopsysselftest.h"
#include "ssmdebug.h"

#ifdef SYMBIAN_INCLUDE_APP_CENTRIC
_LIT( KSecEnvLogicalDeviceDriverName, "secldd" ); 
#endif //SYMBIAN_INCLUDE_APP_CENTRIC

CCustomCmdCoopSysSelfTest* CCustomCmdCoopSysSelfTest::NewL()
	{
	CCustomCmdCoopSysSelfTest* self = new (ELeave) CCustomCmdCoopSysSelfTest();
	return self;
	}

CCustomCmdCoopSysSelfTest::CCustomCmdCoopSysSelfTest()
#ifdef SYMBIAN_INCLUDE_APP_CENTRIC
	:	iSsmLoadSecLdd(EFalse)
#endif //SYMBIAN_INCLUDE_APP_CENTRIC
	{	
	}

CCustomCmdCoopSysSelfTest::~CCustomCmdCoopSysSelfTest()
	{
	}

TInt CCustomCmdCoopSysSelfTest::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
    TInt err = KErrNone;
#ifdef SYMBIAN_INCLUDE_APP_CENTRIC
	//Load the logical device.
	err = User::LoadLogicalDevice(KSecEnvLogicalDeviceDriverName);
	DEBUGPRINT2A("CCustomCmdCoopSysSelfTest: LoadLogicalDevice returned error %d", err);
	iSsmLoadSecLdd = (KErrNone == err) ? ETrue : EFalse;
	if ( KErrNone == err || KErrAlreadyExists == err )
	    { 
	    err = iSecEnvLogicalChannel.Open();
	    DEBUGPRINT2A("CCustomCmdCoopSysSelfTest: Opening the Logical channel retured error %d", err);
	    }
#else
	//Connect RSsmStateAdaptation
    err = iSsmStateAdaptation.Connect();	  
#endif  //SYMBIAN_INCLUDE_APP_CENTRIC  
    return err;
	}

void CCustomCmdCoopSysSelfTest::Close()
	{
#ifdef SYMBIAN_INCLUDE_APP_CENTRIC
	//Cancel the request if there is any
	iSecEnvLogicalChannel.CancelRequest(KErrCancel);  
	//Close the channel.
	iSecEnvLogicalChannel.Close();
	//Free the ldd if it is loaded by SSM selftest custom command.
	if (iSsmLoadSecLdd)
		{
		//Ignore the error returned by unloading the logical device.	
		TInt err = User::FreeLogicalDevice(KSecEnvDriverName);
		DEBUGPRINT2A("CCustomCmdCoopSysSelfTest: FreeLogicalDevice() returned error %d", err);
  		}  
#else
	//Cancel the request if there is any
	iSsmStateAdaptation.RequestCancel();

	//	Close RSsmStateAdaptation
	iSsmStateAdaptation.Close();
#endif	//SYMBIAN_INCLUDE_APP_CENTRIC
	}

void CCustomCmdCoopSysSelfTest::Release()
	{
	delete this;
	}

void CCustomCmdCoopSysSelfTest::Execute(const TDesC8& /*aParams*/, TRequestStatus& aStatus)
	{
#ifdef SYMBIAN_INCLUDE_APP_CENTRIC	
	iParam.iSelfTest = TSecSelfTest::ESelfTestSecurity;
	iSecEnvLogicalChannel.ServiceRequest(aStatus, iParam);
#else
//Request coop system to perform self test
	iSsmStateAdaptation.RequestCoopSysSelfTest(aStatus);
#endif //SYMBIAN_INCLUDE_APP_CENTRIC
  
	}

void CCustomCmdCoopSysSelfTest::ExecuteCancel()
	{
#ifdef SYMBIAN_INCLUDE_APP_CENTRIC
	iSecEnvLogicalChannel.CancelRequest(KErrCancel);
#else
	//Cancel the request if there is any
	iSsmStateAdaptation.RequestCancel();
#endif 	//SYMBIAN_INCLUDE_APP_CENTRIC
	}
