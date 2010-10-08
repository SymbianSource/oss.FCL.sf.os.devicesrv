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

#ifndef __SSMSTATEMONITOR_H_
#define __SSMSTATEMONITOR_H_

#include <e32base.h>
#include <domaindefs.h>

class CSsmStateAwareSession;

/**
 @internalComponent
 @released 
 */
NONSHARABLE_CLASS(CSsmStateMonitor) : public CActive
	{
public:
	static CSsmStateMonitor* NewL(CSsmStateAwareSession& aObserver, TDmDomainId aId);
	~CSsmStateMonitor();
	
	TSsmState State() const;
	
	//from CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
private:
	CSsmStateMonitor(CSsmStateAwareSession& aObserver);
	void ConstructL(TDmDomainId aId);
	
private:
	CSsmStateAwareSession& iObserver;
	RSsmStateAwareSession iSas;
	TSsmState iState;
	};

/**
This active object will, once activated, repeatedly attempt to defer
a transition deadline.
It will stop once an attempt to defer fails eg. because deferral
was cancelled or the transition was acknowledged or the deferral limit was reached.
@internalComponent
@released
*/
NONSHARABLE_CLASS(CSsmDeferralMonitor) : public CActive
    {
public:
    CSsmDeferralMonitor(RSsmStateAwareSession& aStateAwareSession, CSsmStateAwareSession2& aOwnerActiveObject);
    ~CSsmDeferralMonitor();

    /**
    Request deadline deferral for the last transition
    notification
    */
    void DeferNotification();
    void NotifyOfAcknowledgement();

protected:
    /**
    Re-call DeferNotification(), unless the previous
    call completed with an error.
    */
    void RunL();

    /**
    Handle errors thrown from RunL() - call HandleDeferralErrror()    
    */
    TInt RunError(TInt aError);
    void DoCancel();
private:
    RSsmStateAwareSession& iSsmStateAwareSession;
    CSsmStateAwareSession2& iOwnerActiveObject;
    TBool iCeaseDeferral;
    };

#endif

