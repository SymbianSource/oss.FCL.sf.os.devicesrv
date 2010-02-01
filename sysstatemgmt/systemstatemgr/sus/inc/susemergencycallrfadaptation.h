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

#ifndef __SUSEMERGENCYCALLRFADAPTATION_H__
#define __SUSEMERGENCYCALLRFADAPTATION_H__

#include <e32base.h>
#include "ssmadaptation.h"
#include "ssmdebug.h"
#include "ssmadaptationcmn.h"
#include "susadaptionsession.h"


/**
CEmergencyCallRfAdaptation

This class implements the EmergencyCallRf Adaptation related functionality as part of 
Adaptation server.This class implements an active object to route the requests
to Adaptation plugins loaded by CSsmAdaptationServer.When requests are received
from multiple clients this class queues the requests and processes them in order.
Adaptation server  handles only one request from each session and uses Cancel()
methods to cancel any of the session's requests made before.
The queueing mechanism is to handle multiple clients at a time not to handle multiple
requests from one particular clients session.

@internalComponent
*/


class CEmergencyCallRfAdaptation : public CActive
{
public:
	static CEmergencyCallRfAdaptation* NewL(MEmergencyCallRfAdaptation& aAdaptation);
	~CEmergencyCallRfAdaptation();

	void Release();
	void DoActivateRfForEmergencyCallL(const RMessage2& aMessage);
	void DoDeactivateRfForEmergencyCallL(const RMessage2& aMessage);
	void DoEmergencyCallRfAdaptationCancelL(const RMessage2& aMessage);
	void SetPriorityClientSession(CSsmAdaptationSession* aPriorityClientSession);
	void RemovePriorityClientSession();
	void ReserveMemoryL();
protected:
	void RunL();
	TInt RunError( TInt aError );
	void DoCancel();

private:

	CEmergencyCallRfAdaptation(MEmergencyCallRfAdaptation& aAdaptation);
	void SubmitOrQueueL(const RMessage2 &aMessage);
	void Submit(CAdaptationMessage*& aMessage);
	
private:
	CAdaptationMessage *iCurrentMessage;
	RSsmAdaptationRequestQueue iPendingRequestsQueue;
	
	MEmergencyCallRfAdaptation& iEmergencyCallRfAdaptation;
	//Reserving Heap to create CAdaptationMessage in OOM condition for priority clients
	RHeap* iReservedHeap;
	//For storing PriorityClientsession
    CSsmAdaptationSession* iPriorityClientSession;
    };

/**
 * Used for pushing the CAdaptationMessage object in to cleanup stack. This is needed as
 * the message is created using the reserved heap should be freed back to reserved heap
 * instead of deleting.
 * @internalComponent
 */
struct TStoreAdaptationMessage 
    {
    CAdaptationMessage* iAdaptationMessage;
    RHeap* iReservedHeap;
    };

#endif // __SUSEMERGENCYCALLRFADAPTATION_H__
