// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
//Contributors:
//
// Description: Test wrapper for RSsmStateAwareSession

#include <e32std.h>
#include <e32capability.h>
#include <test/datawrapper.h>
#include <ssm/ssmdomaindefs.h> 
#include <ssm/ssmsubstates.hrh>
#include <ssm/startupdomainpskeys.h>
#include <e32debug.h>

#include "t_stateawaresessionwrapper.h"

//
_LIT(KDomainId,                  "domainId"); 
_LIT(KAcknowledge,               "acknowledge");
_LIT(KError,                     "error");
_LIT(KCancelDeferral,            "cancelDeferral");
_LIT(KCount,                     "count"); 
_LIT(KAckLastState,              "ackLastState");
_LIT(KDeferTwice,                "deferTwice"); 

//commands
_LIT(KCmdNewL,                                      "NewL");
_LIT(KCmdConnect,                                   "Connect");
_LIT(KCmdReadIniData,                               "ReadIniData");
_LIT(KCmdRequestStateNotification,                  "RequestStateNotification");
_LIT(KCmdAcknowledgeStateNotification,              "AcknowledgeStateNotification");
_LIT(KCmdClose,                                     "Close");
_LIT(KCmdRequestStateNotificationCancel,            "RequestStateNotificationCancel");
_LIT(KCmdAcknowledgeAndRequestStateNotification,    "AcknowledgeAndRequestStateNotification");
_LIT(KCmdDeferAcknowledgement,                      "DeferAcknowledgement");
_LIT(KCmdReturnCount,                               "ReturnCount");
_LIT(KCmdDestructor,                                "~");

/**
 * Helper method to map the DomainIds.
 */
static TBool MapToDomainId(TPtrC& aDomainIdName, TDmDomainId& aDomainId);

/**
 * Constructor
 */
TIniData1::TIniData1()
        : iCancelDeferral(EFalse)
        , iAckLastState(EFalse)
        , iDeferTwice(EFalse)
        , iCount(0)
        , iAcknowledge(ETrue)
        {}

/**
 * Constructor
 */
TIniData2::TIniData2()
        : iCount(0)
        , iAcknowledge(ETrue)
        {}

/**
 * Two phase constructor
 */
CTestRStateAwareSession1* CTestRStateAwareSession1::NewL()
    {
    CTestRStateAwareSession1*  testStateAwareSession = new (ELeave) CTestRStateAwareSession1();
    CleanupStack::PushL(testStateAwareSession);
    testStateAwareSession->ConstructL();
    CleanupStack::Pop(testStateAwareSession);
    return testStateAwareSession; 
    }

/**
* @return pointer to the object that the data wraps
*/
TAny*  CTestRStateAwareSession1::GetObject()
    {
    return iSsmStateAwareSession;
    }

/**
 * SetObjectL
 */
void CTestRStateAwareSession1::SetObjectL(TAny* aAny)
    {
    DoCleanup();
    iSsmStateAwareSession=static_cast<RSsmStateAwareSession*> (aAny);
    }

/**
 * DisownObjectL
 */
void CTestRStateAwareSession1::DisownObjectL()
    {
    iSsmStateAwareSession=NULL;
    }

/**
 * Protected constructor. First phase construction
 */
CTestRStateAwareSession1::CTestRStateAwareSession1():
CDataWrapper()
    {
    }

/**
 * Protected constructor. Second phase construction
 */
void CTestRStateAwareSession1::ConstructL()
    {
    iActiveNotifyOnChange = CActiveCallback::NewL(*this);
    iActiveNotifyOnChange2 = CActiveCallback::NewL(*this);
    }

/**
 * Destructor.
 */
CTestRStateAwareSession1::~CTestRStateAwareSession1()
    {
    DoCleanup();
    delete iActiveNotifyOnChange;
    delete iActiveNotifyOnChange2;
    }

/**
 * Process a command read from the ini file
 *
 * @param aCommand  the command to process
 * @param aSection      the entry in the ini file requiring the command to be processed
 *
 * @return ETrue if the command is processed
 */
TBool CTestRStateAwareSession1::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
    {
    TBool retVal = ETrue;
    if (aCommand == KCmdNewL)
        {
        DoCmdNew();
        }
    else if (aCommand == KCmdConnect)
        {
        DoCmdConnect(aSection);
        }
    else if (aCommand == KCmdReadIniData)
        {
        DoCmdReadIniData(aSection);
        }
    else if (aCommand == KCmdRequestStateNotification)
        {
        DoCmdRequestStateNotification(aAsyncErrorIndex);
        }
    else if (aCommand == KCmdAcknowledgeStateNotification)
        {
        DoCmdAcknowledgeStateNotification(aSection);
        }
    else if (aCommand == KCmdClose)
        {
        DoCmdClose();
        }
    else if (aCommand == KCmdRequestStateNotificationCancel)
        {
        DoCmdRequestStateNotificationCancel();
        }
    else if (aCommand == KCmdAcknowledgeAndRequestStateNotification)
        {
        DoCmdAcknowledgeAndRequestStateNotification(aSection, aAsyncErrorIndex);
        }
    else if (aCommand == KCmdDeferAcknowledgement)
        {
        DoCmdDeferAcknowledgement( aAsyncErrorIndex);
        }
    else if (aCommand == KCmdReturnCount)
        {
        DoCmdReturnCount(aSection);
        }
    else if (aCommand == KCmdDestructor)
        {
        DoCmdDestructor();
        }
    else
        {
        retVal = EFalse;
        }    
   
    return retVal;
    }

/**
 * Creates RSSmStateAwareSession class instance
 */
void CTestRStateAwareSession1::DoCmdNew()
    {
    INFO_PRINTF1(_L("CTestRStateAwareSession1: Create RSsmStateAwareSession class instance"));
    DoCleanup();
    
    TRAPD(err, iSsmStateAwareSession = new (ELeave)RSsmStateAwareSession());
    
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("CTestRStateAwareSession1: new error %d"), err);
        SetError(err);
        }      
    }

/**
 * Close RSsmStateAwareSession handle
 */
void CTestRStateAwareSession1::DoCmdClose()
    {
    INFO_PRINTF1(_L("CTestRStateAwareSession1: Close RSsmStateAwareSession"));
    iSsmStateAwareSession->Close();
    }

/**
 * Contains cleanup implementation
 */
void CTestRStateAwareSession1::DoCleanup()
    {
    if(iSsmStateAwareSession != NULL)
        {
        INFO_PRINTF1(_L("CTestRStateAwareSession1: Deleting current RSsmStateAwareSession"));
        delete iSsmStateAwareSession;
        iSsmStateAwareSession = NULL;
        }
    }

/**
 * Destroy RSsmStateAwareSession object
 */
void CTestRStateAwareSession1::DoCmdDestructor()
    {
    INFO_PRINTF1(_L("CTestRStateAwareSession1: Destroying the RStateAwareSession object"));
    DoCleanup();
    }

/**
 * Check whether the client is deferred expected no. of times 
 */
void CTestRStateAwareSession1::DoCmdReturnCount(const TDesC& aSection)
    {
    TInt expectedCount = 0;
    if ( !GetIntFromConfig(aSection, KCount(), expectedCount))
        {
        ERR_PRINTF1(_L("CTestRStateAwareSession1: Error reading ini file data"));
        SetError(KErrNotFound);        
        }
    if (iCount == expectedCount)
        return;
    ERR_PRINTF3(_L("CTestRStateAwareSession1: Actual count = %d, Expected count = %d"), iCount, expectedCount);
    SetError(KErrGeneral);
    }

/*
 * Helper function to read ini file data
 */
void CTestRStateAwareSession1::DoCmdReadIniData(const TDesC& aSection)
    {
    if( !GetIntFromConfig(aSection, KCount(), iIniData.iCount) ||
        !GetBoolFromConfig(aSection, KCancelDeferral(), iIniData.iCancelDeferral) ||
        !GetBoolFromConfig(aSection, KAckLastState(), iIniData.iAckLastState) ||
        !GetBoolFromConfig(aSection, KDeferTwice(), iIniData.iDeferTwice) )
        {
        ERR_PRINTF1(_L("CTestRStateAwareSession1: Error reading ini file data"));
        SetError(KErrNotFound);
        }
    }

/**
 * Connects a client to RSsmStateAwareSession with Domain Id
 */
void CTestRStateAwareSession1::DoCmdConnect(const TDesC& aSection)
    {
    TPtrC domainIdName;
    if (!GetStringFromConfig(aSection, KDomainId(), domainIdName))
        {
        //set default domainId if value is not provided in .ini file
        domainIdName.Set(_L("KSM2UiServicesDomain1"));        
        }
    TDmDomainId domainId;
    if(MapToDomainId(domainIdName, domainId ))
       {
       INFO_PRINTF2(_L("CTestRStateAwareSession1: Calling RSsmStateAwareSession->Connect with domain id %S"), &domainIdName );
       TInt  err = iSsmStateAwareSession->Connect(domainId);          
       if (KErrNone != err)
            {
            ERR_PRINTF2(_L("CTestRStateAwareSession1: Connect() error %d"), err);
            SetError(err);  
            }       
       } 
    else
        {
        ERR_PRINTF2(_L("CTestRStateAwareSession1: Could not map %S to domain Id"), &domainIdName );
        SetBlockResult(EFail);
        }
    }

/**
 * DoCmdRequestStateNotification 
 */
void CTestRStateAwareSession1::DoCmdRequestStateNotification(const TInt aAsyncErrorIndex)
    {
    INFO_PRINTF2(_L("CTestRStateAwareSession1: RequestStateNotification with Async Error Index: %d"), aAsyncErrorIndex);
    iSsmStateAwareSession->RequestStateNotification(iActiveNotifyOnChange->iStatus);
    iActiveNotifyOnChange->Activate(aAsyncErrorIndex);
    IncOutstanding();    
    }

/**
 * DoCmdAcknowledgeStateNotification
 */
void CTestRStateAwareSession1::DoCmdAcknowledgeStateNotification(const TDesC& aSection)
    {
    INFO_PRINTF1(_L("CTestRStateAwareSession1: AcknowledgeStateNotification"));
    TInt error;
    if (!GetIntFromConfig(aSection, KError(), error ))
        {
        //set default error value if value is not provided in .ini file
        error = KErrNone;
        }
   iSsmStateAwareSession->AcknowledgeStateNotification(error);    
    }

/**
 * DoCmdAcknowledgeAndRequestStateNotification
 */
void CTestRStateAwareSession1::DoCmdAcknowledgeAndRequestStateNotification(const TDesC& aSection, const TInt aAsyncErrorIndex)
    {
    INFO_PRINTF1(_L("CTestRStateAwareSession1: AcknowledgeAndRequestStateNotification"));
    TInt error;
    if (!GetIntFromConfig(aSection, KError(), error ))
        {
        //set default error value if value is not provided in .ini file
        error = KErrNone;
        }
    iSsmStateAwareSession->AcknowledgeAndRequestStateNotification(error, iActiveNotifyOnChange->iStatus );
    iActiveNotifyOnChange->Activate(aAsyncErrorIndex);
    IncOutstanding();        
    }

/**
 * DoCmdRequestStateNotificationCancel
 */
void CTestRStateAwareSession1::DoCmdRequestStateNotificationCancel()
    {
    INFO_PRINTF1(_L("CTestRStateAwareSession1: RequestStateNotificationCancel is called "));
    iSsmStateAwareSession->RequestStateNotificationCancel();
    }

void CTestRStateAwareSession1::DoCmdDeferAcknowledgement(const TInt aAsyncErrorIndex)
    {
    INFO_PRINTF1(_L("CTestRStateAwareSession1: DeferAcknowledgement is called"));
    iSsmStateAwareSession->DeferAcknowledgement(iActiveNotifyOnChange->iStatus);
    iActiveNotifyOnChange->Activate(aAsyncErrorIndex);
    IncOutstanding();
    }

void CTestRStateAwareSession1::RunL(CActive* aActive, TInt aIndex)
    {
    TInt error = KErrNone;

    if ( aActive == iActiveNotifyOnChange )
        {
        error = iActiveNotifyOnChange->iStatus.Int();
        switch ( error )
            {
            case KErrNone:
                {  
                //Request has completed i.e. The member must either defer again or acknowledge.
                TSsmState currentState = iSsmStateAwareSession->State(); 
                // This component only responds to Shutdown state events.
                if ( (currentState.MainState() == ESsmShutdown) &&
                     (currentState.SubState() == ESsmShutdownSubStateCritical || currentState.SubState() == ESsmShutdownSubStateNonCritical) )
                    {
                    if( iCount < iIniData.iCount )
                        {
                        iCount++;
                        iSsmStateAwareSession->DeferAcknowledgement(iActiveNotifyOnChange->iStatus);
                        iActiveNotifyOnChange->Activate(aIndex);
                        IncOutstanding();                    
                        if( iIniData.iDeferTwice )
                            {
                            iCount++;
                            iSsmStateAwareSession->DeferAcknowledgement(iActiveNotifyOnChange2->iStatus);
                            iActiveNotifyOnChange2->Activate(aIndex);
                            IncOutstanding();
                            }
                        else if( iIniData.iAckLastState )
                            {
                            iSsmStateAwareSession->AcknowledgeStateNotification(KErrNone);
                            }
                        else if( iIniData.iCancelDeferral )
                            {
                            iSsmStateAwareSession->CancelDeferral();
                            }
                        }
                    else
                        {
                        //Acknowledge with success
                        if (iIniData.iAcknowledge)
                            {
                            iSsmStateAwareSession->AcknowledgeStateNotification(KErrNone);
                            }
                        }
                    }  
                break;
                }
            default:
                {
                // Error
                INFO_PRINTF2(_L("CTestRStateAwareSession1: Test returned with error = %d"),error);
                SetAsyncError(aIndex, error);
                }
            }   
        }
    else if ( aActive == iActiveNotifyOnChange2 )
        {
        error = iActiveNotifyOnChange2->iStatus.Int();
        SetAsyncError(aIndex, error);
        }
    DecOutstanding();
    }

/**
 Virtual DoCancel - Request to cancel the asynchronous command
 @see - MTPActiveCallback
 @param aActive Active Object that DoCancel has been called on
 @pre - N/A
 @post - N/A
 @leave system wide error code
*/
void CTestRStateAwareSession1::DoCancel(CActive* aActive, TInt aIndex)
    {
    INFO_PRINTF1(_L("CTestRStateAwareSession1: DoCancel"));
    if(aActive ==  iActiveNotifyOnChange) 
        {
        TInt err =  iActiveNotifyOnChange->iStatus.Int(); 
        if (err != KErrNone)
            {
            ERR_PRINTF2(_L("CTestRStateAwareSession1: DoCancel Error %d"), err);
            SetAsyncError(aIndex, err);
            }
        }
    else if(aActive == iActiveNotifyOnChange2)
        {
        TInt err =  iActiveNotifyOnChange2->iStatus.Int(); 
        if (err != KErrNone)
            {
            ERR_PRINTF2(_L("CTestRStateAwareSession1: DoCancel Error %d"), err);
            SetAsyncError(aIndex, err);
            }
        }
    DecOutstanding();   
    }

/**
 * Helper method to map the DomainIds. This can be extendable for other domain Ids
 */
static TBool MapToDomainId(TPtrC& aDomainIdName, TDmDomainId& aDomainId)
    {
    TBool ret = ETrue;
    if (!(aDomainIdName.Compare(_L("KSM2UiServicesDomain1"))))
        {
        aDomainId = KSM2UiServicesDomain1;
        }
    else if (!(aDomainIdName.Compare(_L("KSM2HALDomain2"))))
        {
        aDomainId = KSM2HALDomain2;
        }
    else if (!(aDomainIdName.Compare(_L("KSM2HALDomain3"))))
        {
        aDomainId = KSM2HALDomain3;  
        }
    else if (!(aDomainIdName.Compare(_L("KSM2AppServicesDomain1"))))
        {
        aDomainId = KSM2AppServicesDomain1;  
        }
    else if (!(aDomainIdName.Compare(_L("KSM2AppServicesDomain2"))))
        {
        aDomainId = KSM2AppServicesDomain2;  
        }
    else if (!(aDomainIdName.Compare(_L("KSM2AppServicesDomain3"))))
        {
        aDomainId = KSM2AppServicesDomain3;  
        } 
    else if (!(aDomainIdName.Compare(_L("KSM2AppServicesDomain4"))))
        {
        aDomainId = KSM2AppServicesDomain4;  
        } 
    else 
        {
        ret = EFalse;
        }   
    return ret;
    }


/**
 * Two phase constructor
 */
CTestRStateAwareSession2* CTestRStateAwareSession2::NewL()
    {
    CTestRStateAwareSession2*  testStateAwareSession = new (ELeave) CTestRStateAwareSession2();
    CleanupStack::PushL(testStateAwareSession);
    testStateAwareSession->ConstructL();
    CleanupStack::Pop(testStateAwareSession);
    return testStateAwareSession; 
    }

/**
* @return pointer to the object that the data wraps
*/
TAny*  CTestRStateAwareSession2::GetObject()
    {
    return iSsmStateAwareSession;
    }

/**
 * SetObjectL
 */
void CTestRStateAwareSession2::SetObjectL(TAny* aAny)
    {
    DoCleanup();
    iSsmStateAwareSession=static_cast<RSsmStateAwareSession*> (aAny);
    }

/**
 * DisownObjectL
 */
void CTestRStateAwareSession2::DisownObjectL()
    {
    iSsmStateAwareSession=NULL;
    }

/**
 * Protected constructor. First phase construction
 */
CTestRStateAwareSession2::CTestRStateAwareSession2():
CDataWrapper()
    {
    }

/**
 * Protected constructor. Second phase construction
 */
void CTestRStateAwareSession2::ConstructL()
    {
    iActiveNotifyOnChange = CActiveCallback::NewL(*this);
    }

/**
 * Destructor.
 */
CTestRStateAwareSession2::~CTestRStateAwareSession2()
    {
    DoCleanup();
    delete iActiveNotifyOnChange;
    }

/**
 * Process a command read from the ini file
 *
 * @param aCommand  the command to process
 * @param aSection      the entry in the ini file requiring the command to be processed
 *
 * @return ETrue if the command is processed
 */
TBool CTestRStateAwareSession2::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
    {
    TBool retVal = ETrue;
    if (aCommand == KCmdNewL)
        {
        DoCmdNew();
        }
    else if (aCommand == KCmdConnect)
        {
        DoCmdConnect(aSection);
        }
    else if (aCommand == KCmdReadIniData)
        {
        DoCmdReadIniData(aSection);
        }
    else if (aCommand == KCmdRequestStateNotification)
        {
        DoCmdRequestStateNotification(aAsyncErrorIndex);
        }
    else if (aCommand == KCmdAcknowledgeStateNotification)
        {
        DoCmdAcknowledgeStateNotification(aSection);
        }
    else if (aCommand == KCmdClose)
        {
        DoCmdClose();
        }
    else if (aCommand == KCmdRequestStateNotificationCancel)
        {
        DoCmdRequestStateNotificationCancel();
        }
    else if (aCommand == KCmdAcknowledgeAndRequestStateNotification)
        {
        DoCmdAcknowledgeAndRequestStateNotification(aSection, aAsyncErrorIndex);
        }
    else if (aCommand == KCmdDestructor)
        {
        DoCmdDestructor();
        }
    else
        {
        retVal = EFalse;
        }    
   
    return retVal;
    }

/**
 * Creates RSSmStateAwareSession class instance
 */
void CTestRStateAwareSession2::DoCmdNew()
    {
    INFO_PRINTF1(_L("CTestRStateAwareSession2: Create RSsmStateAwareSession class instance"));
    DoCleanup();
    
    TRAPD(err, iSsmStateAwareSession = new (ELeave)RSsmStateAwareSession());
    
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("CTestRStateAwareSession2: new error %d"), err);
        SetError(err);
        }      
    }

/**
 * Close RSsmStateAwareSession handle
 */
void CTestRStateAwareSession2::DoCmdClose()
    {
    INFO_PRINTF1(_L("CTestRStateAwareSession2: Close RSsmStateAwareSession"));
    iSsmStateAwareSession->Close();
    }

/**
 * Contains cleanup implementation
 */
void CTestRStateAwareSession2::DoCleanup()
    {
    if(iSsmStateAwareSession != NULL)
        {
        INFO_PRINTF1(_L("CTestRStateAwareSession2: Deleting current RSsmStateAwareSession"));
        delete iSsmStateAwareSession;
        iSsmStateAwareSession = NULL;
        }
    }

/**
 * Destroy RSsmStateAwareSession object
 */
void CTestRStateAwareSession2::DoCmdDestructor()
    {
    INFO_PRINTF1(_L("CTestRStateAwareSession2: Destroying the RStateAwareSession object"));
    DoCleanup();
    }

/**
 * Connects a client to RSsmStateAwareSession with Domain Id
 */
void CTestRStateAwareSession2::DoCmdConnect(const TDesC& aSection)
    {
    TPtrC domainIdName;
    if (!GetStringFromConfig(aSection, KDomainId(), domainIdName))
        {
        //set default domainId if value is not provided in .ini file
        domainIdName.Set(_L("KSM2UiServicesDomain1"));        
        }
    TDmDomainId domainId;
    if(MapToDomainId(domainIdName, domainId ))
       {
       INFO_PRINTF2(_L("CTestRStateAwareSession2: Calling RSsmStateAwareSession->Connect with domain id %S"), &domainIdName );
       TInt  err = iSsmStateAwareSession->Connect(domainId);          
       if (KErrNone != err)
            {
            ERR_PRINTF2(_L("CTestRStateAwareSession2: Connect() error %d"), err);
            SetError(err);  
            }       
       } 
    else
        {
        ERR_PRINTF2(_L("CTestRStateAwareSession2: Could not map %S to domain Id"), &domainIdName );
        SetBlockResult(EFail);
        }
    }

/*
 * Helper function to read ini file data
 */
void CTestRStateAwareSession2::DoCmdReadIniData(const TDesC& aSection)
{
    if( !GetIntFromConfig(aSection,KCount(), iIniData.iCount) ||
        !GetBoolFromConfig(aSection,KAcknowledge(), iIniData.iAcknowledge) )
        {
        ERR_PRINTF1(_L("Error reading ini file data"));
        SetError(KErrNotFound);
        }
}

/**
 * DoCmdRequestStateNotification 
 */
void CTestRStateAwareSession2::DoCmdRequestStateNotification(const TInt aAsyncErrorIndex)
    {
    INFO_PRINTF2(_L("CTestRStateAwareSession2: RequestStateNotification with Asyc Error Index: %d"), aAsyncErrorIndex);
    iSsmStateAwareSession->RequestStateNotification(iActiveNotifyOnChange->iStatus);
    iActiveNotifyOnChange->Activate(aAsyncErrorIndex);
    IncOutstanding();    
    }

/**
 * DoCmdAcknowledgeStateNotification
 */
void CTestRStateAwareSession2::DoCmdAcknowledgeStateNotification(const TDesC& aSection)
    {
    TInt error;
    if (!GetIntFromConfig(aSection, KError(), error ))
        {
        //set default error value if value is not provided in .ini file
        error = KErrNone;
        }
   iSsmStateAwareSession->AcknowledgeStateNotification(error);    
    }

/**
 * DoCmdAcknowledgeAndRequestStateNotification
 */
void CTestRStateAwareSession2::DoCmdAcknowledgeAndRequestStateNotification(const TDesC& aSection, const TInt aAsyncErrorIndex)
    {
    TInt error;
    if (!GetIntFromConfig(aSection, KError(), error ))
        {
        //set default error value if value is not provided in .ini file
        error = KErrNone;
        }
    iSsmStateAwareSession->AcknowledgeAndRequestStateNotification(error, iActiveNotifyOnChange->iStatus );
    iActiveNotifyOnChange->Activate(aAsyncErrorIndex);
    IncOutstanding();        
    }

/**
 * DoCmdRequestStateNotificationCancel
 */
void CTestRStateAwareSession2::DoCmdRequestStateNotificationCancel()
    {
    INFO_PRINTF1(_L("CTestRStateAwareSession2: RequestStateNotificationCancel is called "));
    iSsmStateAwareSession->RequestStateNotificationCancel();
    }

/**
 Virtual RunL - Called on completion of an asynchronous command
 @see MTPActiveCallback
 @param aActive Active Object that RunL has been called on
 @pre N/A
 @post N/A
 @leave system wide error code
*/
void CTestRStateAwareSession2::RunL(CActive* aActive, TInt aIndex)
    {
    TInt error = KErrNone;

    if (aActive == iActiveNotifyOnChange)
        {
        error = iActiveNotifyOnChange->iStatus.Int();
        if (error == KErrNone)
            {
            // Request has completed i.e. The member must either defer again or acknowledge.
            TSsmState currentState = iSsmStateAwareSession->State(); 
            // This component only responds to Shutdown state events.
            if ( (currentState.MainState() == ESsmShutdown) &&
                 (currentState.SubState() == ESsmShutdownSubStateCritical || currentState.SubState() == ESsmShutdownSubStateNonCritical) )
                {
                if(iCount < iIniData.iCount)
                    {
                    iCount++;
                    iSsmStateAwareSession->DeferAcknowledgement(iActiveNotifyOnChange->iStatus);
                    iActiveNotifyOnChange->Activate(aIndex);
                    IncOutstanding();
                    }
                else
                    {
                    //Acknowledge with success/failure
                    if (iIniData.iAcknowledge)
                        {
                        iSsmStateAwareSession->AcknowledgeStateNotification(KErrNone);
                        }
                    }
                }
            else
                {
                INFO_PRINTF1(_L("CTestRStateAwareSession2: Deferring acknowledgement for a state that doesn't support transition monitoring"));
                iSsmStateAwareSession->DeferAcknowledgement(iActiveNotifyOnChange->iStatus);
                iActiveNotifyOnChange->Activate(aIndex);
                IncOutstanding();
                }
            }
        else
            {
            INFO_PRINTF2(_L("CTestRStateAwareSession2: RunL Error %d"), error);
            SetAsyncError(aIndex, error);
            }       
        }
    DecOutstanding();
    }

/**
 Virtual DoCancel - Request to cancel the asynchronous command
 @see - MTPActiveCallback
 @param aActive Active Object that DoCancel has been called on
 @pre - N/A
 @post - N/A
 @leave system wide error code
*/
void CTestRStateAwareSession2::DoCancel(CActive* aActive, TInt aIndex)
    {
    INFO_PRINTF1(_L("CTestRStateAwareSession2: DoCancel"));
    if(aActive ==  iActiveNotifyOnChange) 
        {
        TInt err =  iActiveNotifyOnChange->iStatus.Int(); 
        if (err!=KErrNone)
            {
            ERR_PRINTF2(_L("CTestRStateAwareSession2: DoCancel Error %d"), err);
            SetAsyncError(aIndex, err);
            }
        }   
    DecOutstanding();   
    }
