/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation for asynchronous play commands sender object.
*
*/


#include "hapticstestappview.h"
#include "hapticstestappui.h"
#include "hapticstestasyncplaysender.h"
#include "hapticstesteffectdatahandler.h"

// ---------------------------------------------------------------------------
// Simple constructor.
// ---------------------------------------------------------------------------
//
CHapticsTestAsyncPlaySender::CHapticsTestAsyncPlaySender( 
    CHapticsTestAppUi* aUi )
    : CActive( EPriorityStandard ), iUi( aUi )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHapticsTestAsyncPlaySender::~CHapticsTestAsyncPlaySender()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Method for requesting the playing of an effect repeatedly.
// ---------------------------------------------------------------------------
//
void CHapticsTestAsyncPlaySender::PlayRepeat( CHWRMHaptics* aHaptics,
                                              TInt  aFileHandle,
                                              TInt aEffectIndex,
                                              TUint8 aRepeat,
                                              TInt& aEffectHandle )
    {
    iIndex = aEffectIndex;
    aHaptics->PlayEffectRepeat( aFileHandle, 
                                aEffectIndex,
                                aRepeat,
                                aEffectHandle,
                                iStatus );
    SetActive();                            
    }

// ---------------------------------------------------------------------------
// Method for requesting the playing of an effect.
// ---------------------------------------------------------------------------
//
void CHapticsTestAsyncPlaySender::Play( CHWRMHaptics* aHaptics,
                                        TInt  aFileHandle,
                                        TInt aEffectIndex,
                                        TInt& aEffectHandle )
    {
    iIndex = aEffectIndex;
    aHaptics->PlayEffect( aFileHandle, 
                          aEffectIndex,
                          aEffectHandle,
                          iStatus );
    SetActive();                      
    }

// ---------------------------------------------------------------------------
// This active object's RunL.
// ---------------------------------------------------------------------------
//
void CHapticsTestAsyncPlaySender::RunL()
    {
    TInt status = iStatus.Int();
    iUi->PrintPlayEffectStatus( status );
    iUi->GetCurrentEffectState();
    if ( !status )
        {
        iUi->iEffectData->AddEffectInfo( 
            iUi->iEffectHandle, iUi->iIVTFileHandle, iIndex );
        }
    }
    
// ---------------------------------------------------------------------------
// Dummy DoCancel(). Required due to inheritance from CActive.
// ---------------------------------------------------------------------------
//    
void CHapticsTestAsyncPlaySender::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
// Dummy RunError(). Required due to inheritance from CActive.
// ---------------------------------------------------------------------------
//
TInt CHapticsTestAsyncPlaySender::RunError( TInt /* aError */ )
    {
    return KErrNone;
    }

// end of file
