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

#ifndef __SSMSTATEAWARESESSION_H__
#define __SSMSTATEAWARESESSION_H__

#include <e32base.h>
#include <domaindefs.h>
#include <domainmember.h>

#include <ssm/ssmstate.h>

class CSsmStateMonitor;
class CSsmDeferralMonitor;


/**
Read-only client interface to receive notifications when the
System State changes.

CSsmStateAwareSession wraps this class in an active object.

@see CSsmStateAwareSession 
@publishedPartner
@released
*/
NONSHARABLE_CLASS(RSsmStateAwareSession)
	{
public:
	IMPORT_C RSsmStateAwareSession();
	IMPORT_C TInt Connect(TDmDomainId aId);
	IMPORT_C void Close();
	IMPORT_C TSsmState State() const;
	IMPORT_C void RequestStateNotification(TRequestStatus& aStatus);
	IMPORT_C void RequestStateNotificationCancel();
	IMPORT_C void AcknowledgeStateNotification(TInt aError);
	IMPORT_C void AcknowledgeAndRequestStateNotification(TInt aError, TRequestStatus& aStatus);
	IMPORT_C void DeferAcknowledgement(TRequestStatus& aStatus);
	IMPORT_C void CancelDeferral();

private:
	RSsmStateAwareSession(const RSsmStateAwareSession& aState);
	
private:
	//The main reason for this class is to hide implementation details that might change
	class RPrivateImpl;
	RPrivateImpl* iPimpl;
	};

/**
Interface to implement for clients that want to use the @c CSsmStateAwareSession 
utility class.

@publishedPartner
@released
*/
class MStateChangeNotificationSubscriber
    {
public:
	/** 
	@c StateChanged is called when the System State have changed
	@param aSsmState contains the new System State
	@see TSsmMainSystemStates
	*/
	virtual void StateChanged(TSsmState aSsmState) = 0;
	};	

/**
Utility class for monitoring the System State. Maintains a list of 
subscribers that share a single active object. When the System State
changes the subscribers will be notified in the order
they were added to the list of subscribers.

@see TSsmMainSystemStates
@see RSsmStateAwareSession
@publishedPartner
@released
*/
NONSHARABLE_CLASS(CSsmStateAwareSession) : public CBase
	{
public:
	IMPORT_C static CSsmStateAwareSession* NewL(TDmDomainId aId);
	IMPORT_C static CSsmStateAwareSession* NewLC(TDmDomainId aId);
	IMPORT_C ~CSsmStateAwareSession();
	IMPORT_C TSsmState State() const;
	IMPORT_C void AddSubscriberL(MStateChangeNotificationSubscriber& aSubscriber);	
	IMPORT_C void RemoveSubscriber(const MStateChangeNotificationSubscriber& aSubscriber);
	// for CSsmStateMonitor
	void NotifySubscribers(TSsmState aNewState);
	
private:
	CSsmStateAwareSession();
	void ConstructL(TDmDomainId aId);
	
private:
	RPointerArray<MStateChangeNotificationSubscriber> iSubscribers; // Elements of the array are not owned.
	CSsmStateMonitor* iMonitor;
	TInt iSpare[4];
	};


/**
Interface to implement for clients that want to use the @c CExtendedSsmStateAwareSession 
utility class.

@publishedPartner
@released
*/
class MStateChangeNotificationSubscriber2
    {
public:
    /*HandleTransition() will be called when the transition notification
    comes in. Thereafter, the active object will contineouslly defer the transition.
    
    The implementation of this function should be used first to call
    RequestTransitionNotification() again if required, and then to initiate the
    response to the transition. It should be kept as quick as possible.
    
    Once the Domain Member's transition operations are complete, it should call
    AcknowledgeLastState() on this active object, to indicate it is ready to be
    transitioned*/
    
    virtual void HandleTransition(TInt aError) = 0;
    
    virtual TInt HandleDeferralError(TInt aError) = 0;
    };

/**
This class automatically deferrs transitions as long as possible after the original notification
is received.

To make use of this class, one has to implement the implement the HandleTransition()
in MStateChangeNotificationSubscriber2. HandleTransition() will be called when the transition notification
comes in. Thereafter, the active object will continually defer the transition.

This object is intended to simplify the handling of notifications and
deferrals. The member must ensure that other active objects do not block or
have long-running RunL()s; this is to ensure that the Active Scheduler will
remain responsive to the completion of deadline deferrals.

The capabilities needed are the same as those needed for RDmDomain::DeferAcknowledgement()
@capability WriteDeviceData
@capability ProtServ
@see 
*/
NONSHARABLE_CLASS(CSsmStateAwareSession2):public CActive 
	{
public:	
	IMPORT_C static CSsmStateAwareSession2* NewL(TDmDomainId aDomainId, MStateChangeNotificationSubscriber2& aSubscriber );
	IMPORT_C ~CSsmStateAwareSession2();
	IMPORT_C void RequestStateNotification();
    IMPORT_C void RequestStateNotificationCancel();
    IMPORT_C void AcknowledgeStateNotification(TInt aError);
    IMPORT_C void AcknowledgeAndRequestStateNotification(TInt aError);	  
	IMPORT_C TSsmState GetState();    
	TInt HandleDeferralError(TInt aError);
    
private:
	CSsmStateAwareSession2(MStateChangeNotificationSubscriber2& aSubscriber);
    void RunL();
    void DoCancel();
	void ConstructL(TDmDomainId aDomainId);  
	
	
private:
	RSsmStateAwareSession iSsmStateAwareSession;
	//For deferring the notification.
	CSsmDeferralMonitor* iDeferNotification;	
	MStateChangeNotificationSubscriber2* iSubscriber;
	};
	
#endif
