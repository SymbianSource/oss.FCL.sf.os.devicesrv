/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation of CSsmLangSelCmd class.
*
*/

#include "ssmlangselcmd.h"
#include "ssmmapperutility.h"
#include "ssmalternativelanguages.h"
#include "ssmlanguageloader.h"
#include "syslangutilprivatecrkeys.h"
#include "trace.h"

#include <syslangutil.h>
#include <CommonEngineDomainCRKeys.h>
#include <centralrepository.h>


/** Default granularity for list of installed languages. */
const TInt KLanguageListGranularity( 4 );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::NewL
// ---------------------------------------------------------------------------
//
CSsmLangSelCmd* CSsmLangSelCmd::NewL()
	{
    FUNC_LOG;
    CSsmLangSelCmd* self = new ( ELeave ) CSsmLangSelCmd();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
	return self;	
	}


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::~CSsmLangSelCmd
// ---------------------------------------------------------------------------
//
CSsmLangSelCmd::~CSsmLangSelCmd()
    {
    FUNC_LOG;
    delete iLangList;
    delete iMapperUtility;
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::RunL
//
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::RunL()
    {
    FUNC_LOG;
    TInt errorCode = iStatus.Int();
    
    ERROR( errorCode, "CSsmLangSelCmd::RunL with error" );

    if ( iState == EQueryListSize )
        {
        if ( errorCode == KErrNone )
            {
            TRAP( errorCode, FetchLanguageListL() );
            ERROR( errorCode, "Failed to fetch language list" );
            }
        else
            {
            TryNextList();
            }
        }
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::DoCancel
//
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::DoCancel()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::Initialize
// ---------------------------------------------------------------------------
//
TInt CSsmLangSelCmd::Initialize( CSsmCustomCommandEnv* aCmdEnv )
    {
    FUNC_LOG;

    iEnv = aCmdEnv;
    TRAPD( errorCode, InitializeL() );
    return errorCode;
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::Execute
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::Execute(
    const TDesC8& /*aParams*/,
    TRequestStatus& aRequest )
    {
    FUNC_LOG;
    
    ASSERT_TRACE( !iClientStatus );
    ASSERT_TRACE( !IsActive() );
    ASSERT_TRACE( iEnv );

    aRequest = KRequestPending;
    iClientStatus = &aRequest;
    

    RFs* fsSession = &( const_cast<RFs&>( iEnv->Rfs() ) );
    TInt errorCode = SysLangUtil::GetDefaultLanguage( iDefaultLanguage,
                                                      fsSession );
    ERROR( errorCode, "Failed to get default language" );

    TInt lastSelectedLang = GetLastSelectedLang();
    // Revert to automatic selection if fetching last used language
    // fails, last used language is not valid (and obviously when last
    // selection is automatic).
    if ( lastSelectedLang == 0 )
        {
        PrepareNextList();
        }
    else if ( IsValid( lastSelectedLang ) )
        {
        UseLanguage( lastSelectedLang );
        }
    else
        {
        UseLanguage( iDefaultLanguage );
        }    
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::ExecuteCancel
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::ExecuteCancel()
    {
    FUNC_LOG;

    Cancel();
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::Close
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::Close()
    {
    FUNC_LOG;
    delete iValidLanguages;
    iValidLanguages = NULL;
    
    iAdaptation.Close();
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::Release
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::Release()
    {
    FUNC_LOG;

	delete this;
    }
// ---------------------------------------------------------------------------
// CSsmLangSelCmd::CSsmLangSelCmd
// ---------------------------------------------------------------------------
//

CSsmLangSelCmd::CSsmLangSelCmd()
  : CActive( EPriorityNormal )
    {
    FUNC_LOG;

    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::ConstructL
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::ConstructL()
    {
    iMapperUtility = CSsmMapperUtility::NewL();
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::InitializeL
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::InitializeL()
    {

    FUNC_LOG;
    ASSERT_TRACE( iEnv );
    
    iValidLanguages = new ( ELeave ) CArrayFixFlat<TInt>( 
                                                    KLanguageListGranularity );
    RFs* fsSession = &( const_cast<RFs&>( iEnv->Rfs() ) );
    TInt errorCode = SysLangUtil::GetInstalledLanguages( iValidLanguages,
                                                         fsSession );
    
    ERROR( errorCode, "Failed to get installed languages" );
    User::LeaveIfError( errorCode );
    
    errorCode = iAdaptation.Connect();
    ERROR( errorCode, "Failed to connect to RSsmMiscAdaptation" );
    User::LeaveIfError( errorCode );
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::GetLastSelectedLang
//
// ---------------------------------------------------------------------------
//
TInt CSsmLangSelCmd::GetLastSelectedLang()
    {
    FUNC_LOG;

    TInt value( 0 );
    TInt errorCode = iMapperUtility->CrValue( KCRUidCommonEngineKeys, 
                                              KGSDisplayTxtLang,
                                              value );
    
    ERROR( errorCode, "Failed read KGSDisplayTxtLang CenRep key" );
    INFO_1( "Last selected language = %d", value );
    return value;
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::IsValid
//
// ---------------------------------------------------------------------------
//
TBool CSsmLangSelCmd::IsValid( const TInt aLanguage ) const
    {
    FUNC_LOG;
    ASSERT_TRACE( iValidLanguages );

    TBool valid( EFalse );
    TKeyArrayFix keyProp( 0, ECmpTInt );
    TInt index( KErrNotFound );
    // Returns zero if element is found.
    if ( 0 == iValidLanguages->Find( aLanguage, keyProp, index ) )
        {
        valid = ETrue;
        }
    return valid;
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::PrepareNextList
//
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::PrepareNextList()
    {
    FUNC_LOG;
    ASSERT_TRACE( iLangListPref >= EPrimaryLanguages &&
                  iLangListPref <= ETertiaryLanguages );
    INFO_1( "Trying preferred language list %d", iLangListPref );

    iState = EQueryListSize;
    iAdaptation.PrepareSimLanguages(
        static_cast<TSsmLanguageListPriority>( iLangListPref++ ),
        iLangListSize, iStatus );
    SetActive();
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::FetchLanguageListL
//
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::FetchLanguageListL()
    {
    FUNC_LOG;
    INFO_1( "Required language list size = %d", iLangListSize() );

    iState = EQueryListContent;
    delete iLangList;
    iLangList = NULL;
    
    iLangList = iAdaptation.SimLanguagesArrayL( iLangListSize() );    
    
    TInt selectionResult = InterpretResult();

    if ( selectionResult > KErrNone )
        {
        StoreLanguageToCentRep( selectionResult );
        UseLanguage( selectionResult );
        }
    else
        {
        TryNextList();
        }
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::InterpretResult
//
// ---------------------------------------------------------------------------
//
TInt CSsmLangSelCmd::InterpretResult()
    {
    FUNC_LOG;

    TInt selectedLanguage = KErrNotFound;
    TInt langCount( iLangListSize() );
    INFO_1( "Number of languages: %d", langCount );

    for ( TInt i = 0; i < langCount; i++ )
        {
        TInt lang = ( *iLangList )[i];
        INFO_1( "Language: %d", lang );

        TInt nearestMatch = ValidAlternative( lang );
        if ( nearestMatch > 0 ) // Not an error code
            {
            selectedLanguage = nearestMatch;
            break;
            }
        }

    INFO_1( "Selected language %d from list", selectedLanguage );
    return selectedLanguage;
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::ValidAlternative
//
// ---------------------------------------------------------------------------
//
TInt CSsmLangSelCmd::ValidAlternative( const TInt aLanguage ) const
    {
    FUNC_LOG;

    TInt retVal = KErrNotFound;

    if ( IsValid( aLanguage ) )
        {
        retVal = aLanguage;
        }
    else
        {
        for ( TInt i = 0; i < KSsmAlternativeLanguageTableSize; i++ )
            {
            if ( KAlternativeLanguageTable[i].iLang1 == aLanguage )
                {
                INFO_2( "Alternative %d found for language %d",
                    KAlternativeLanguageTable[i].iLang2, aLanguage );

                if ( IsValid( KAlternativeLanguageTable[i].iLang2 ) )
                    {
                    retVal = KAlternativeLanguageTable[i].iLang2;
                    break;
                    }
                }
            else if ( KAlternativeLanguageTable[i].iLang2 == aLanguage )
                {
                INFO_2( "Alternative %d found for language %d",
                    KAlternativeLanguageTable[i].iLang1, aLanguage );

                if ( IsValid( KAlternativeLanguageTable[i].iLang1 ) )
                    {
                    retVal = KAlternativeLanguageTable[i].iLang1;
                    break;
                    }
                }
            }
        }

    return retVal;
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::TryNextList
//
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::TryNextList()
    {
    FUNC_LOG;

    if ( iLangListPref <= ETertiaryLanguages )
        {
        PrepareNextList();
        }
    else // All lists exhausted
        {
        TInt prevLang = GetLanguageFromCentRep();
        if ( IsValid( prevLang ) )
            {
            UseLanguage( prevLang );
            }
        else
            {
            INFO( "Previous language not valid, reverting to default" );
            UseLanguage( iDefaultLanguage );
            }
        }
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::StoreLanguageToCentRep
//
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::StoreLanguageToCentRep( const TInt aLanguage )
    {
    FUNC_LOG;
    TUid mappedUid = iMapperUtility->CrUid( KCRUidSysLangUtil );
    CRepository* cenrep = NULL;
    TRAPD( errorCode, cenrep = CRepository::NewL( mappedUid ) );
    ERROR_1( errorCode, "Failed to initialize cen rep %d", errorCode );
    
    if ( KErrNone == errorCode )
        {
        errorCode = cenrep->Set( KSysLangUtilSimLanguage, aLanguage );
        ERROR_1( errorCode, "Failed to store language code to CentRep, %d", 
                                                                  errorCode );
        delete cenrep;
        }
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::GetLanguageFromCentRep
//
// ---------------------------------------------------------------------------
//
TInt CSsmLangSelCmd::GetLanguageFromCentRep()
    {
    FUNC_LOG;

    TInt lang = iDefaultLanguage;
    TInt errorCode = iMapperUtility->CrValue( KCRUidSysLangUtil,
                                              KSysLangUtilSimLanguage,
                                              lang );
    
    ERROR_1( errorCode, "Failed to read language code from CenRep %d", 
             errorCode );
    return lang;
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::UseLanguage
//
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::UseLanguage( const TInt aLang )
    {
    FUNC_LOG;

    TInt errorCode = aLang;
    if ( aLang > KErrNone )
        {
        errorCode = SsmLanguageLoader::LoadLanguage( aLang );
        ERROR( errorCode, "Failed to load language" );
        }

    ERROR( errorCode, "Language selection failed" );

    if ( iClientStatus )
        {
        User::RequestComplete( iClientStatus, errorCode );
        }
    }

