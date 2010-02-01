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
* Description:  Implementation of SysLangUtil class.
*
*/


#include <s32file.h>
#include <centralrepository.h>

#include "syslangutil.h"
#include "syslangutiltrace.h"
#include "ssmmapperutilitystatic.h"
#include "syslangutilprivatecrkeys.h"

const TInt KReadBufSize = 10;
const TInt KLangArraySize = 20;

// R&D support: Use language file in Starters internal directory for module
// testing purposes.
#ifdef __STARTER_MODULE_TEST_SUPPORT__
  _LIT( KLanguagesIni, "C:\\private\\100059C9\\languages.txt" );
#else // __STARTER_MODULE_TEST_SUPPORT__
  _LIT( KLanguagesIni, "z:\\resource\\bootdata\\languages.txt" );
#endif // __STARTER_MODULE_TEST_SUPPORT__

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// SysLangUtil::IsValidLanguage
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool SysLangUtil::IsValidLanguage(
    const TInt& aLanguage,
    RFs* aFileServerSession )
    {
    API_FUNC_ENTRY_TRACE;

    TBool hadFS;
    TInt err;
    aFileServerSession = CheckFS( aFileServerSession, hadFS, err );

    if ( err )
        {
        return EFalse;
        }

    RFile file;
    err = file.Open(
        *aFileServerSession,
        KLanguagesIni,
        EFileStream | EFileRead | EFileShareReadersOnly );

    if ( !err )
        {
        // Prepare Reader
        TFileText reader;
        reader.Set(file);
        if ( reader.Seek(ESeekStart) )
            {
            file.Close();
            if ( !hadFS )
                {
                aFileServerSession->Close();
                delete aFileServerSession;
                aFileServerSession = NULL;
                }
            return EFalse;
            }

        TBuf<KReadBufSize> readBuf;
        err = EFalse; // This solution is not very elegant but saves ROM :)

        while ( !reader.Read( readBuf ) && readBuf.Length() )
            {
            if ( readBuf.Length() > 1 )
                {
                TLex lex( readBuf );
                lex.SkipSpace();
                TInt language;
                err = lex.Val( language );

                if ( language == aLanguage )
                    {
                     err = ETrue;
                     break;
                    }
                }
            }
        file.Close();
        }
    else
        {
        err = EFalse;
        ERROR_TRACE_1( "ERROR: file '%S' is missing", &KLanguagesIni );
        }

    if ( !hadFS )
        {
        aFileServerSession->Close();
        delete aFileServerSession;
        aFileServerSession = NULL;
        }

    FUNC_EXIT_RET_TRACE( err );
    return err;
    }


// -----------------------------------------------------------------------------
// SysLangUtil::GetDefaultLanguage
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SysLangUtil::GetDefaultLanguage(
    TInt& aLanguage,
    RFs* aFileServerSession )
    {
    API_FUNC_ENTRY_TRACE;

    TBool hadFS;
    TInt err;
    aFileServerSession = CheckFS( aFileServerSession, hadFS, err );

    if ( err )
        {
        return err;
        }

    RFile file;
    err = file.Open(
        *aFileServerSession,
        KLanguagesIni,
        EFileStream | EFileRead | EFileShareReadersOnly );

    if ( !err )
        {
        // Prepare Reader
        TFileText reader;
        reader.Set(file);
        err = reader.Seek( ESeekStart );
        if ( !err )
            {
            TBuf<KReadBufSize> readBuf;

            while ( !reader.Read( readBuf ) && readBuf.Length() )
                {
                if ( readBuf.Locate('d') != KErrNotFound )
                   {
                        TLex lex( readBuf );
                        lex.SkipSpace();
                        err = lex.Val( aLanguage );
                        break;
                   }

                readBuf.Zero();
                }

            if ( !readBuf.Length() )
                {
                ERROR_TRACE_1( "ERROR: file '%S' is missing", &KLanguagesIni );
                file.Close();
                return KErrNotFound;
                }
            }
        file.Close();
        }

    if ( !hadFS )
        {
        aFileServerSession->Close();
        delete aFileServerSession;
        aFileServerSession = NULL;
        }

    FUNC_EXIT_RET_TRACE( err );
    return err;
    }


// -----------------------------------------------------------------------------
// SysLangUtil::GetInstalledLanguages
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SysLangUtil::GetInstalledLanguages(
    CArrayFixFlat<TInt>*& aLanguages,
    RFs* aFileServerSession )
    {
    API_FUNC_ENTRY_TRACE;

    if ( aLanguages )
        {
        delete aLanguages;
        }

    aLanguages = new CArrayFixFlat<TInt>( KLangArraySize );

    if ( !aLanguages )
        {
        return KErrNoMemory;
        }


    TBool hadFS;
    TInt err;
    aFileServerSession = CheckFS( aFileServerSession, hadFS, err );

    if ( err )
        {
        return err;
        }

    RFile file;
    err = file.Open(
        *aFileServerSession,
        KLanguagesIni,
        EFileStream | EFileRead | EFileShareReadersOnly );
    if ( !err )
        {
        // Prepare Reader
        TFileText reader;
        reader.Set( file );
        err = reader.Seek( ESeekStart );
        if ( !err )
            {
            TBuf<KReadBufSize> readBuf;

            for ( TInt i = 0; !reader.Read( readBuf ) && readBuf.Length(); i++ )
                {
                if ( readBuf.Length() > 0 )
                    {
                    TLex lex( readBuf );
                    lex.SkipSpace();
                    TInt language;
                    err = lex.Val( language );
                    if ( err ) break;
                    // Will not leave if array size (KLangArraySize) not exceeded
                    TRAP( err, aLanguages->AppendL( language ) );
                    err = KErrNone; // Ignore.
                    }
                readBuf.Zero();
                }
            }
        file.Close();
        }
    else
        {
        ERROR_TRACE_1( "ERROR: file '%S' is missing", &KLanguagesIni );
        }

    if ( !hadFS )
        {
        aFileServerSession->Close();
        delete aFileServerSession;
        aFileServerSession = NULL;
        }

    FUNC_EXIT_RET_TRACE( err );
    return err;
    }


// -----------------------------------------------------------------------------
// SysLangUtil::RestoreSIMLanguage
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SysLangUtil::RestoreSIMLanguage(
    TInt &aLanguage,
    RFs* /*aFileServerSession*/ )
    {
    API_FUNC_ENTRY_TRACE;
    
    TUid mappedUid = SsmMapperUtility::CrUid( KCRUidSysLangUtil );

    CRepository* repository = NULL;
    TRAPD( errorCode, repository = CRepository::NewL( mappedUid ) );

    if ( errorCode == KErrNone )
        {
        errorCode = repository->Get( KSysLangUtilSimLanguage, aLanguage );

        if ( errorCode != KErrNone )
            {
            ERROR_TRACE_1( "Failed to get value from CentRep, error code %d.", errorCode );
            }
        }
    else
        {
        ERROR_TRACE_1( "Failed to open repository, error code %d.", errorCode );
        }

    delete repository;

    FUNC_EXIT_RET_TRACE( errorCode );
    return errorCode;
    }


// -----------------------------------------------------------------------------
// SysLangUtil::CheckFS
//
// -----------------------------------------------------------------------------
//
RFs* SysLangUtil::CheckFS( RFs* aRFs, TBool& aExist, TInt& aErr )
    {
    FUNC_ENTRY_TRACE;

    if ( !aRFs )
        {
        aExist = EFalse;
        aRFs = new RFs;
        if ( aRFs )
            {
            aErr = aRFs->Connect();
            if ( aErr != KErrNone)
                {
                delete aRFs;
                return NULL;
                }
            }
        else
            {
            aErr = KErrNoMemory;
            return NULL;
            }
        }
    else
        {
        // File server session given as argument MUST be already connected
        ASSERT_TRACE( aRFs->Handle() );

        aExist = ETrue;
        }

    aErr = KErrNone;

    FUNC_EXIT_TRACE;
    return aRFs;
    }
