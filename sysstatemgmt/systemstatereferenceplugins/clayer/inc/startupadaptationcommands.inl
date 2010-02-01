/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name        : startupadaptationcommands.inl
* Part of     : System Startup / Starter
* Interface   : Domain, Startup Adaptation API
* Implementation for inline functions of classes defined in
* startupadaptationcommands.h.
* Version     : %version: 2 %
* This material, including documentation and any related computer
* programs, is protected by copyright controlled by Nokia.  All
* rights are reserved.  Copying, including reproducing, storing,
* adapting or translating, any or all of this material requires the
* prior written consent of Nokia.  This material also contains
* confidential information which may not be disclosed to others
* without the prior written consent of Nokia.
* Template version: 4.0
* Nokia Core OS *
*
*/



#ifndef __STARTUPADAPTATIONCOMMANDS_INL__
#define __STARTUPADAPTATIONCOMMANDS_INL__

/*
* @publishedPartner
* @released
*/

// ============================ MEMBER FUNCTIONS ===============================

namespace StartupAdaptation
    {
    inline RLanguageListResponse::RLanguageListResponse()
      : iErrorCode( KErrNone )
        {
        }

    inline void RLanguageListResponse::Close()
        {
        iLanguages.Close();
        }
    }

#endif	// __STARTUPADAPTATIONCOMMANDS_INL__
