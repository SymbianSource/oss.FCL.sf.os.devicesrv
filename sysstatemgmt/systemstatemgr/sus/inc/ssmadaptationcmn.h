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

#ifndef __SSMADAPTATIONCMN_H__
#define __SSMADAPTATIONCMN_H__

#include <e32cmn.h>
#include "susadaptionclisrv.h"

class CAdaptationMessage : public CBase
	{
public:
	CAdaptationMessage(const RMessage2 &aMessage);
	void Complete(TInt aReason);
	CSession2 *Session() const;
	void WriteL(TInt aParam, const TDesC8 &aDes);
	TInt Function() const;
	TInt Int0() const;
	void ReadL(TInt aParamNumber,TPtr8 aParam);
	TInt GetDesLength(TInt aParam);
private:
	RMessage2 iMessage;
	TBool iMessageAvailable;
public :
	TSusAdaptionServerRequests iRequestType;
	//To know whether the object is created using reserved heap or not.
	TBool iUsingReservedHeap;
	};

class RSsmAdaptationRequestQueue
	{
public:
	void Close();
	TBool IsEmpty();
	void Dequeue(CAdaptationMessage *&aCurrentMessage);
	void RemoveFromQueueAndComplete(const RMessage2 &aMessage);
	TInt Queue(CAdaptationMessage *aPendingRequest);
	void NotifyAndRemoveAll();
	void RemoveFromQueueAndComplete(const RMessage2 &aMessage, RHeap *aReservedHeap);	
	void NotifyAndRemoveAll(RHeap *aReservedHeap);
	TInt Reserve(TInt aReserverCount);
	TInt Count();
	
private:
	RPointerArray<CAdaptationMessage > iQueue;
		
	};

#endif	__SSMADAPTATIONCMN_H__
