// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


//Test Step header
#include "HWRMFmTxSetRdsPty_CStep.h"
#include "HWRMFmTxClientServer.h"


//TO BE SAFE
IMPORT_C TInt StartDialogThread();

CHWRMFmTxSetRdsPtyStep::CHWRMFmTxSetRdsPtyStep()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	//DEF iTestStepName = _L("CPARAM_MESS_NAMEStep");

	const TUint32 KCapabilityWriteUserDataBitMask  = 0x10000; // 65536LL;
	const TUint32 KCapabilityCapabilityMultimediaDDBitMask  = 0x8; //8LL;
	
	//The server name and IPC number is obtained and all messages are checked Sync
	SR_ServerName		= KServerProcessName;
	SR_MESSAGE_TYPE		= KSyncMessage;	//Is it Async or sync?
	SR_MESSAGE_ID		= EHWRMFmTxSetRdsPty ;	//It holds the IPC number
	SR_MESSAGE_MASK		= KCapabilityCapabilityMultimediaDDBitMask + KCapabilityWriteUserDataBitMask; //Holds the cap mask for the message
	
	

	//The iServer_Panic is a unique name from Server,but always truncated to KMaxExitCategoryName

	iServer_Panic		=	_L("HWRM Server");

	//TCapability cap[] = {ECapabilityPARAM_MESS_NAMECAP, ECapability_Limit};
	TCapability cap[] = {ECapabilityMultimediaDD, ECapabilityWriteUserData, ECapability_Limit};

	TSecurityInfo info;
	info.Set(RProcess());
	TBool result = EFalse;

	for (TInt i = 0; cap[i] != ECapability_Limit; i++)
	{

		if (!(info.iCaps.HasCapability(cap[i])))
		{
			result=ETrue;

		}

	}
	
#ifdef HWRM_FMTX_POLICY_CHECKING_DISABLED
            
    result = EFalse;
            
#endif
            
                
    iExpect_Rejection = result;


	iStepCap            = KCapabilityCapabilityMultimediaDDBitMask + KCapabilityWriteUserDataBitMask;

	//Get a unique thread name
	ChildThread_SR.Format(_L("ChildThread_%S_%d"),&SR_ServerName,SR_MESSAGE_ID);
	}

/*
Exec_SendReceive():
	This Fn is called by the Child Thread
1.	Create a session with the server
2.	Test an SendReceive call
3.	Informs the main thread about the status of the call using
	a.	iSessionCreated, if the a connection is established
	b.	iResult_Server, holds the return value for connection
	c.	iResult_SR, the return value of SendReceive	call
*/

TInt CHWRMFmTxSetRdsPtyStep::Exec_SendReceive()
	{
	iResult_Server = CreateSession(SR_ServerName,Version(),2);

	if (iResult_Server!=KErrNone)
		{

		iResult_Server=StartServer();
		if (iResult_Server!=KErrNone)
			return(iResult_Server);

		iResult_Server = CreateSession(SR_ServerName,TVersion(),2);
		}
	if(iResult_Server == KErrNone)
		{
		iSessionCreated = ETrue;
		
		/*
		if(SR_MESSAGE_ID == EEtelDbgMarkHeap ||
			SR_MESSAGE_ID == EEtelDbgCheckHeap ||
			SR_MESSAGE_ID == EEtelDbgMarkEnd ||
			SR_MESSAGE_ID == EEtelDbgFailNext)
				{
 				iResult_SR = TestDebugHeap(SR_MESSAGE_ID);
				}
		else
			*/
			if(SR_MESSAGE_ID >= 0)
				{
			 	TIpcArgs args(0,0,0,0);
				iResult_SR	=	SendReceive(SR_MESSAGE_ID,args);
				}
		}

	return iResult_Server;
	}
