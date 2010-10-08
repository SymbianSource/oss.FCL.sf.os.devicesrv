// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: Test Server to test transition monitoring feature incase of state change to shutdown state
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


 
#include "tcmn_transitionmonitor_server.h"
#include "t_stateawaresessionwrapper.h"
#include "t_ssmstatemanager.h"
#include "t_stateawaresession2.h"

_LIT(KTransitionMonitorServer,        "TransitionMonitorServer");
_LIT(KStateAwareSession1,             "RSsmStateAwareSession1");
_LIT(KStateAwareSession2,             "RSsmStateAwareSession2");
_LIT(KStateManager,                   "RSsmStateManager");
_LIT(KStateAwareSession3,             "CSsmStateAwareSession2");

CAwareSessionTestServer* CAwareSessionTestServer::NewL( )
	{
	CAwareSessionTestServer* server = new (ELeave)CAwareSessionTestServer();
	CleanupStack::PushL(server);
	server->ConstructL();
	CleanupStack::Pop(server);
	return server;
	}

static void MainL()
	{
	CActiveScheduler* sched=new(ELeave) CActiveScheduler;	
	CActiveScheduler::Install(sched);
	CAwareSessionTestServer* server = NULL;
	TRAPD(err, server = CAwareSessionTestServer::NewL());
	if(!err)
		{
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

/**
 Process entry point. Called by client using RProcess API
 @return - Standard Epoc error code on process exit
 */
TInt E32Main(void)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());	
	if (err)
		{
		_LIT(KMainErrorStatement, "KTransitionMonitorServer::MainL - Error: %d");
		RDebug::Print(KMainErrorStatement, err);
	   	User::Panic(KTransitionMonitorServer, err);
		}		
	delete cleanup;
	return KErrNone;
	}

CTestBlockController* CAwareSessionTestServer::CreateTestBlock()
	{
	CTestBlockController* controller = NULL;
	TRAPD(err, (controller = new (ELeave) CAwareSessionTestBlock()));
	if(KErrNone != err)
		{
		User::Panic(KTransitionMonitorServer, err);
		}
	return controller;
	}

CDataWrapper* CAwareSessionTestBlock::CreateDataL(const TDesC& aData)
	{
	// Print out the parameters for debugging
	CDataWrapper* wrapper = NULL;
	if ( KStateAwareSession1() == aData )
		{
		wrapper = CTestRStateAwareSession1::NewL();
		}
	else if ( KStateAwareSession2() == aData )
        {
        wrapper = CTestRStateAwareSession2::NewL();
        }
	else if ( KStateManager() == aData )
	    {
	    wrapper = CTestRSsmStateManager::NewL();
	    }
	else if ( KStateAwareSession3() == aData)
	    {
	    wrapper = new (ELeave) CTestCStateAwareSession();
	    }
	return wrapper;
	}
