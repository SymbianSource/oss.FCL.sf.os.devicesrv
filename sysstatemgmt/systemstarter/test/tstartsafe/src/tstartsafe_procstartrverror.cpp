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
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <startupproperties.h>

#include "startsafe.h"
#include "tstartsafe_procstartrverror.h"
#include "testapps.h"


const TInt KStartSafeProcStartRvErrorNominalTimeout = 300; // milliseconds
const TInt KStartSafeProcStartRvErrorRetries = 3;

/**
Old Test CaseID 		APPFWK-STARTSAFE-0017
New Test CaseID 		DEVSRVS-SYSSTART-STARTSAFE-0017
 */

TVerdict CAppFwkStartSafeTestStepProcStartRvError::doTestStepL()
	{
	TInt threadHandles_Before;
	TInt processHandles_Before;
	TInt threadHandles_After;
	TInt processHandles_After;
		
	RThread().HandleCount(processHandles_Before, threadHandles_Before);
		
	CStartSafe* startSafe = CStartSafe::NewL();	
	CleanupStack::PushL( startSafe );
	
	CStartupProperties* prop = CStartupProperties::NewL();
	CleanupStack::PushL( prop );
	
	RProcess proc;
	CleanupClosePushL( proc );
	
	prop->SetFileParamsL( KTestAppRvError, KNullDesC );
	prop->SetStartupType( EStartProcess );
	prop->SetStartMethod( EWaitForStart );
	prop->SetTimeout( KStartSafeProcStartRvErrorNominalTimeout );
	prop->SetNoOfRetries( KStartSafeProcStartRvErrorRetries );
	
	TInt tried = 0;
	
	INFO_PRINTF1( _L("Start the Test-App process") );
	TRAPD( err, startSafe->StartL(*prop, proc, tried) );
	
	// Ensure that the process-name of the test-app is not extant.
	TPtrC procNamePtr = proc.FileName().Right( KTestAppRvError().Length() );
	TInt nameComparison = procNamePtr.Compare( KTestAppRvError() );
	

	if( (0 == nameComparison) )
		{
		ERR_PRINTF2( _L("Application whose process-name is\"%S\" was found, but should not have been"), &procNamePtr );
		
		SetTestStepResult( EFail );
		// Dispose of the erroneously extant test-app.
		proc.Kill( KErrNone );
		
		ERR_PRINTF1( _L( "Test Failed") );
		}
	else if( (KStartSafeProcStartRvErrorRetries +1) != tried )
		{
		ERR_PRINTF1( _L("Process-name (correctly) not found, but...") );
		ERR_PRINTF3( _L("Total tries = %d  but should have been %d"), tried, (KStartSafeProcStartRvErrorRetries +1) );	
		
		SetTestStepResult( EFail );
		}
	else
		{
		INFO_PRINTF1( _L("Process was (correctly) not found") );	
		INFO_PRINTF2( _L("Error code %d"), err );
		
		// The 'erroneous' rendezvous code
		TESTE( KErrGeneral, err );
		}
		
		
	CleanupStack::PopAndDestroy( 3, startSafe );
	
	RThread().HandleCount(processHandles_After, threadHandles_After);
	TEST(processHandles_After == processHandles_Before);
	TEST(threadHandles_After == threadHandles_Before);
	
	return TestStepResult();	
	}



CAppFwkStartSafeTestStepProcStartRvError::CAppFwkStartSafeTestStepProcStartRvError()
	{
	SetTestStepName( KCTestCaseProcStartRvError );
	}
