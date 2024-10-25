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

#ifndef __TIMEOUTWAITERNOTIFICATION_H__
#define __TIMEOUTWAITERNOTIFICATION_H__


class MTimeoutWaiterNotification
	{
public:
	virtual void NotifyTimeout() = 0;
	};


#endif // __TIMEOUTWAITERNOTIFICATION_H__
