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
// susemergencycallrfadaptationref.cpp
// 
//

#include <e32debug.h>
#include <ssm/ssmadaptation.h>
#include "susemergencycallrfadaptation.h"


/**
 * Function used to cleanup the CAdaptationMessage object which is pushed to cleanup stack incase of Leave
 * CAdaptationMessage message will be freed back to reserved heap incase of it is created using reserved heap.
 * or else message will be deleted.
 */
static void DoCleanUp(TAny* aAdaptationMessage)
    {
    TStoreAdaptationMessage* storeMessage = static_cast <TStoreAdaptationMessage*>(aAdaptationMessage);
    if(storeMessage->iAdaptationMessage->iUsingReservedHeap)
        {
        storeMessage->iReservedHeap->Free(storeMessage->iAdaptationMessage);
        }
    else
        {
        delete storeMessage->iAdaptationMessage;
        }
    }

/**
@publishedPartner
*/
void CEmergencyCallRfAdaptation::SubmitOrQueueL(const RMessage2 &aMessage)
	{
	CAdaptationMessage *messageCopy = NULL;	
	TRAPD(err , messageCopy = new(ELeave) CAdaptationMessage(aMessage));
	//Use preallocated heap for creating CAdaptationMessage under OOM condition, if it is a priority client
    if (KErrNoMemory == err && aMessage.Session() == iPriorityClientSession)
        {
        DEBUGPRINT1A("CAdaptationMessage will be created using Reserved Heap");
        TAny* messagePtr = iReservedHeap->AllocL(sizeof(CAdaptationMessage));
        messageCopy = new (messagePtr)CAdaptationMessage(aMessage);
        messageCopy->iUsingReservedHeap = ETrue;
        }
    else
        {
        User::LeaveIfError(err);
        }
	  
	if(!IsActive())
		{
		Submit(messageCopy);
		}
	else 
		{
		//Store the CAdaptationMessage pointer and iReservedHeap in a struct inorder to cleanup 
		//depending on the reserved heap/normal heap used.
		TStoreAdaptationMessage storeMessage;
		storeMessage.iAdaptationMessage = messageCopy;
		storeMessage.iReservedHeap = iReservedHeap;
		CleanupStack::PushL(TCleanupItem(DoCleanUp, &storeMessage ));
		DEBUGPRINT2A("CEmergencyCallRfAdaptationRequests queueing request with function id: %d", aMessage.Function());
		//Reserve heap only in non OOM condition
		if(messageCopy->iUsingReservedHeap == EFalse)
		    {
		    //Always reserve 2 slots in queue for Emergency call requests. Slots will be reserved if count
		    //to request memory(RPointerArray.Reserve(count)) is greater than the existing reserved memory in
		    //RPonterArray. So there will be memory allocation only when
		    //iPendingRequestsQueue.Count()+ reserveCount + 1(for the present message))
		    // > already reserved memory.
		    const TInt reserveCount = 2;
		    err = iPendingRequestsQueue.Reserve(iPendingRequestsQueue.Count() + reserveCount + 1 );
		    }
		if(KErrNone == err || (KErrNoMemory == err && aMessage.Session() == iPriorityClientSession))
		    {	 
		    User::LeaveIfError(iPendingRequestsQueue.Queue(messageCopy));
		    }
		else
		    {
		    User::Leave(err);
		    }
		CleanupStack::Pop(&storeMessage);
		}	
	}

void CEmergencyCallRfAdaptation::Submit(CAdaptationMessage*& aMessage)
	{
	DEBUGPRINT2A("CEmergencyCallRfAdaptationRequests immediate submission of request with function id: %d", aMessage->Function());
	iCurrentMessage = aMessage;
	switch(aMessage->Function())
		{
		case EActivateRfForEmergencyCall :
			{
			iEmergencyCallRfAdaptation.ActivateRfForEmergencyCall(iStatus);
			break;	
			}
		case EDeactivateRfForEmergencyCall :
			{
			iEmergencyCallRfAdaptation.DeactivateRfForEmergencyCall(iStatus);
			break;	
			}
		default :
			{
			break;
			}
		}
	SetActive();
	}
/**
CEmergencyCallRfAdaptation implements EmergencyCallRf Adaptation related functionality as part of 
CSsmAdaptationServer.CSsmAdaptationServer loads EmergencyCallRf Adaptation plugin and creates
CEmergencyCallRfAdaptation using the NewL. From then the loaded EmergencyCallRf Adaptation plugin
will be owned by CEmergencyCallRfAdaptation.

@internalComponent
*/

CEmergencyCallRfAdaptation* CEmergencyCallRfAdaptation::NewL(MEmergencyCallRfAdaptation& aAdaptation)
	{
	CEmergencyCallRfAdaptation* self = new(ELeave) CEmergencyCallRfAdaptation(aAdaptation);
	return self;	
	}
/**
 * Function to reserve memory to make emergency call during OOM condition
 */
void CEmergencyCallRfAdaptation :: ReserveMemoryL()
    {
    //Reserve space for one Activate Rf call + one Deactive Rf message in Queue.
    const TInt reserveCount = 2;
    // heap requested for one Active Rf call + one Deactive Rf
    const TInt reservedHeap = reserveCount * sizeof(CAdaptationMessage);    
    //heap is reserved for storing CAdaptationMessage during OOM condition.
    iReservedHeap = UserHeap::ChunkHeap(NULL, reservedHeap, reservedHeap); 
    //Leave with KErrNoMemory if iReservedHeap is NULL    
    if(iReservedHeap == NULL)
        {
        User::Leave(KErrNoMemory);
        }
    User::LeaveIfError(iPendingRequestsQueue.Reserve(reserveCount));   
    }


CEmergencyCallRfAdaptation::~CEmergencyCallRfAdaptation()
	{
	iPendingRequestsQueue.NotifyAndRemoveAll(iReservedHeap);
	Cancel();
	iPendingRequestsQueue.Close();
	if(iReservedHeap != NULL)
	    {
        iReservedHeap->Reset();
        iReservedHeap->Close();
	    }
	Release();
	}

CEmergencyCallRfAdaptation::CEmergencyCallRfAdaptation(MEmergencyCallRfAdaptation& aAdaptation) : CActive(EPriorityStandard), iEmergencyCallRfAdaptation(aAdaptation)
, iReservedHeap(NULL)
	{
	CActiveScheduler::Add(this);
	}

void CEmergencyCallRfAdaptation::Release()
	{
	iEmergencyCallRfAdaptation.Release();
	}

void CEmergencyCallRfAdaptation::DoActivateRfForEmergencyCallL(const RMessage2& aMessage)
	{
	SubmitOrQueueL(aMessage);	
	}

void CEmergencyCallRfAdaptation::DoDeactivateRfForEmergencyCallL(const RMessage2& aMessage)
	{
	SubmitOrQueueL(aMessage);	
	}

void CEmergencyCallRfAdaptation::DoEmergencyCallRfAdaptationCancelL(const RMessage2& aMessage)
	{

	if(iCurrentMessage != NULL)	
		{
		if(aMessage.Session() == iCurrentMessage->Session())
			{
			DEBUGPRINT1A("CEmergencyCallRfAdaptationRequests cancelling current request as requested");
			iEmergencyCallRfAdaptation.Cancel();
			}
		iPendingRequestsQueue.RemoveFromQueueAndComplete(aMessage, iReservedHeap);
		
		aMessage.Complete(KErrNone);
		}
	else
		{
		DEBUGPRINT1A("CEmergencyCallRfAdaptationRequests nothing to cancel, but cancel requested");
		aMessage.Complete(KErrNone);				
		}		
	}


void CEmergencyCallRfAdaptation::RunL()
	{
	
	DEBUGPRINT2A("CEmergencyCallRfAdaptationRequests processed the request with funtion id: %d", iCurrentMessage->Function());
	iCurrentMessage->Complete(iStatus.Int());
	if(iCurrentMessage->iUsingReservedHeap)
        {
        iReservedHeap->Free(iCurrentMessage);
        }
    else
        {
        delete iCurrentMessage;
        }
	iCurrentMessage = NULL;  

	if( (iPendingRequestsQueue.IsEmpty()) == EFalse )
		{
		CAdaptationMessage *messageCopy = NULL;
		iPendingRequestsQueue.Dequeue(messageCopy);		
		Submit(messageCopy);
		} 
	}

TInt CEmergencyCallRfAdaptation::RunError( TInt aError )
	{
	
	if(iCurrentMessage != NULL)	
		{
		iCurrentMessage->Complete(aError);
        if(iCurrentMessage->iUsingReservedHeap)
            {
            iReservedHeap->Free(iCurrentMessage);
            }
        else
            {
            delete iCurrentMessage;
            }
		iCurrentMessage = NULL;
		}
	
	while( (iPendingRequestsQueue.IsEmpty()) == EFalse )
		{
		iPendingRequestsQueue.Dequeue(iCurrentMessage);
		iCurrentMessage->Complete(aError);
		if(iCurrentMessage->iUsingReservedHeap)
            {
            iReservedHeap->Free(iCurrentMessage);
            }
        else
            {
            delete iCurrentMessage;
            }
		iCurrentMessage = NULL;
		}
	
	return KErrNone;
		
	}

void CEmergencyCallRfAdaptation::DoCancel()
	{
	if(iCurrentMessage != NULL)	
		{
		iCurrentMessage->Complete(KErrCancel);
		if(iCurrentMessage->iUsingReservedHeap)
            {
            iReservedHeap->Free(iCurrentMessage);
            }
        else
            {
            delete iCurrentMessage;
            }
		iCurrentMessage = NULL;
		}
		
	while( (iPendingRequestsQueue.IsEmpty()) == EFalse )
		{
		iPendingRequestsQueue.Dequeue(iCurrentMessage);
		iCurrentMessage->Complete(KErrCancel);
		if(iCurrentMessage->iUsingReservedHeap)
            {
            iReservedHeap->Free(iCurrentMessage);
            }
        else
            {
            delete iCurrentMessage;
            }
		iCurrentMessage = NULL;
		}
	}

/**
 * Sets the Priority Client Session
 */
void CEmergencyCallRfAdaptation::SetPriorityClientSession(CSsmAdaptationSession* aPriorityClientSession)
    {
    iPriorityClientSession = aPriorityClientSession;
    }

/**
 * Unsets the Priority Client session
 */
void CEmergencyCallRfAdaptation::RemovePriorityClientSession()
    {
    iPriorityClientSession = NULL;
    }

