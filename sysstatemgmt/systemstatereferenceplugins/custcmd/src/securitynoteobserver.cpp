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
// Name        : strtsecuritynoteobserver.cpp
// Part of     : System Startup / Starter
// Implementation of CStrtSecurityNoteObserver class.
// Version     : %version: 3 %
// This material, including documentation and any related computer
// programs, is protected by copyright controlled by Nokia.  All
// rights are reserved.  Copying, including reproducing, storing,
// adapting or translating, any or all of this material requires the
// prior written consent of Nokia.  This material also contains
// confidential information which may not be disclosed to others
// without the prior written consent of Nokia.
// Template version: 4.1
// Nokia Core OS *
// File renamed from strtsecuritynoteobserver.cpp to securitynoteobserver.cpp as part of Core OS transfer.
//




/**
 @file
 @internalComponent
 @released
*/

#include "startupdomainpskeys.h"
#include "securitynoteobserver.h"
#include "ssmuiproviderdll.h"
#include <e32property.h>

CSecurityNoteObserver* CSecurityNoteObserver::NewL()
	{
	CSecurityNoteObserver* self = new (ELeave) CSecurityNoteObserver(CSsmUiSpecific::StartupPSUid(), KStartupSecurityCodeQueryStatus);

	CleanupStack::PushL( self );
	self->StartL();
	CleanupStack::Pop( self );

	return self;
	}

CSecurityNoteObserver::CSecurityNoteObserver(const TUid& aCategory,
						const TUint aKey ): CActive( EPriorityStandard ),
						iCategory( aCategory ), iKey( aKey )
	{
	CActiveScheduler::Add(this);
	}

CSecurityNoteObserver::~CSecurityNoteObserver()
	{
	if(IsAdded())
		{
		Deque();
		}
	iProperty.Close();
	}

void CSecurityNoteObserver::StartL()
	{
    // Attach can only return error in oom-situation.
    User::LeaveIfError(iProperty.Attach(iCategory, iKey));
    Activate();
    }

void CSecurityNoteObserver::DoCancel()
	{
	iProperty.Cancel();
	}

TBool CSecurityNoteObserver::PropertyChanged(const TInt aCurrentValue )
	{
	TBool continueObserving = EFalse;
	if (aCurrentValue == ESecurityQueryActive)
		{
		RProperty::Set(CSsmUiSpecific::StartupPSUid(),KPSSplashShutdown, ESplashShutdown );
		continueObserving = ETrue;
		}
	return continueObserving;
	}

void CSecurityNoteObserver::RunL()
	{
	if (KErrNone  == iStatus.Int())
		{
		TInt currentValue = 0;
		TInt errorCode = iProperty.Get( iCategory, iKey, currentValue );
		if ( errorCode == KErrNone )
			{
			TBool continueObserving = PropertyChanged( currentValue );
			if ( continueObserving )
				{
				Activate();
				}
			else
				{
				iProperty.Close();
				}
			}
		else
			{
			Activate();
			}
		}
	else
		{
		Activate();
		}
	}

void CSecurityNoteObserver::Activate()
	{
	iProperty.Subscribe(iStatus);
	SetActive();
	}
