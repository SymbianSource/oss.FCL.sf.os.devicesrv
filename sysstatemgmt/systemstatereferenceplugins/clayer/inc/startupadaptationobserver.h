/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : startupadaptationobserver.h
* Part of     : System Startup / Starter
* Interface   : Domain, Startup Adaptation API
* This header file defines the startup adaptation API observer
* interface used to return responses and deliver events.
* Version     : %version: ou1s60rt#5 %
* This material, including documentation and any related computer
* programs, is protected by copyright controlled by Nokia.  All
* rights are reserved.  Copying, including reproducing, storing,
* adapting or translating, any or all of this material requires the
* prior written consent of Nokia.  This material also contains
* confidential information which may not be disclosed to others
* without the prior written consent of Nokia.
* Template version: 4.1
* Nokia Core OS *
*
*/



#ifndef __STARTUPADAPTATIONOBSERVER_H__
#define __STARTUPADAPTATIONOBSERVER_H__

/*****************************************************
*       Series 60 Customer
*       Needs customer implementation
*****************************************************/

#include <ssm/startupadaptationcommands.h>
#include <ssm/startupadaptationevents.h>

/*
* @publishedPartner
* @released
*/

/**
*  An observer interface used by the adaptation
*  to return responses and deliver events.
*
*  @since S60 3.2
*/
class MStartupAdaptationObserver
    {

public:

    /**
    * A response to a command.
    *
    * @since S60 3.2
    *
    * @param aCommandId A command identifier.
    * @param aData      Data associated with the response.
    */
    virtual void ResponseL(
        const StartupAdaptation::TCommand aCommandId,
        TDesC8& aData ) = 0;

    /**
    * An event from Adaptation.
    *
    * @since S60 3.2
    *
    * @param aEventId A command identifier.
    * @param aData    Data associated with the response.
    */
    virtual void EventL(
        const StartupAdaptation::TEvent aEventId,
        TDesC8& aData ) = 0;
    };

#endif // __STARTUPADAPTATIONOBSERVER_H__
