/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Panic code definitions.
*
*/


#ifndef __HAPTICSTEST_PAN__
#define __HAPTICSTEST_PAN__

/** hapticstest application panic codes */
enum THapticsTestPanics 
    {
    EHapticsTestBasicUi = 1
    // add further panics here
    };

inline void Panic(THapticsTestPanics aReason)
    {
	_LIT(applicationName,"hapticstest");
    User::Panic(applicationName, aReason);
    }

#endif // __HAPTICSTEST_PAN__
