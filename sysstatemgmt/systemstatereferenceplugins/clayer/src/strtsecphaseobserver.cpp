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
// Name        : strtsecphaseobserver.cpp
// Part of     : System Startup / StrtSecObs
// Implementation of CStrtSecPhaseObserver class
// Version     : %version: 1 % << Don't touch! Updated by Synergy at check-out.
// This material, including documentation and any related computer
// programs, is protected by copyright controlled by Nokia.  All
// rights are reserved.  Copying, including reproducing, storing,
// adapting or translating, any or all of this material requires the
// prior written consent of Nokia.  This material also contains
// confidential information which may not be disclosed to others
// without the prior written consent of Nokia.
// Template version: 4.1.1
// Nokia Core OS *
//



#include "strtsecphaseobserver.h"
#include "ssmdebug.h"
#include "ssmrefcustomcmdcommon.h"
#include <ssm/ssmuiproviderdll.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CStrtSecPhaseObserver::NewL
//
// ---------------------------------------------------------------------------
//
CStrtSecPhaseObserver* CStrtSecPhaseObserver::NewL()
    {
    CStrtSecPhaseObserver* self = new( ELeave ) CStrtSecPhaseObserver;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CStrtSecPhaseObserver::~CStrtSecPhaseObserver
//
// ---------------------------------------------------------------------------
//
CStrtSecPhaseObserver::~CStrtSecPhaseObserver()
    {
    Cancel();
    iProperty.Close();
    }

// -----------------------------------------------------------------------------
// CStrtSecPhaseObserver::IsSecurityPhaseSimOk
//
// -----------------------------------------------------------------------------
//
TBool CStrtSecPhaseObserver::IsSecurityPhaseSimOk() const
    {
    return ( iValue == EStarterSecurityPhaseSimOk ||
             iValue == EStarterSecurityPhaseSecOk ||
             iValue == EStarterSecurityPhaseSecNok );
    }

// -----------------------------------------------------------------------------
// CStrtSecPhaseObserver::IsSecurityPhaseSecOk
//
// -----------------------------------------------------------------------------
//
TBool CStrtSecPhaseObserver::IsSecurityPhaseSecOk() const
    {
    return iValue == EStarterSecurityPhaseSecOk;
    }

// -----------------------------------------------------------------------------
// CStrtSecPhaseObserver::DoCancel
//
// -----------------------------------------------------------------------------
//
void CStrtSecPhaseObserver::DoCancel()
    {
    iProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CStrtSecPhaseObserver::RunL
//
// -----------------------------------------------------------------------------
//
void CStrtSecPhaseObserver::RunL()
    {
    TInt errorCode = iStatus.Int();
    if ( errorCode == KErrNone )
        {
        IssueRequest();

        TInt val( 0 );
        errorCode = iProperty.Get( CSsmUiSpecific::StarterPSUid(), KStarterSecurityPhase, val );
        if (KErrNone == errorCode)
            {
            iValue = val;
            }
        else
        	{
        	DEBUGPRINT2A("Failed to get value of Starter Security Phase property: %d", errorCode);
        	}
        }
    }

// -----------------------------------------------------------------------------
// CStrtSecPhaseObserver::CStrtSecPhaseObserver
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CStrtSecPhaseObserver::CStrtSecPhaseObserver() : CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CStrtSecPhaseObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStrtSecPhaseObserver::ConstructL()
    {
    // Attach can only return error in oom-situation.
    User::LeaveIfError( iProperty.Attach( CSsmUiSpecific::StarterPSUid(), KStarterSecurityPhase ) );
    IssueRequest();
    }

// -----------------------------------------------------------------------------
// CStrtSecPhaseObserver::IssueRequest
//
// -----------------------------------------------------------------------------
//
void CStrtSecPhaseObserver::IssueRequest()
    {
    iProperty.Subscribe( iStatus );
    SetActive();
    }
