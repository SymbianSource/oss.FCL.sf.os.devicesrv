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
* Description: Implementation of CSsmLocaleObserverSup class.
*
*/

#include <bacntf.h>
#include <bautils.h>
#include <connect/sbdefs.h>
#include <s32file.h>

#include "ssmmapperutilitystatic.h"
#include "ssmlocaleobserversup.h"
#include "trace.h"

/**
* Directory to store user-specific locale data files.
* Private directory of SSM Utility Server.
*/
_LIT( KLocaleDataDir, "\\private\\2000d766\\localedata\\" );

/** Name is LocaleData.Dnn or CommonData.D00 */
_LIT( KFileName, "LocaleData.D" );
_LIT( KCommonFileName, "CommonData.D" );

/**
* Version number from 1..255 to indicate data
* format of the file. To make dynamic data format fixes possible we need
* version ID.
*/
const TInt KCurrentVersionNumber = 2; // For fixing TChar r/w

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// EnvChangeNotifierCallbackL
// ---------------------------------------------------------------------------
//
static TInt EnvChangeNotifierCallback( TAny* aThis )
    {
    CSsmLocaleObserverSup* self = static_cast<CSsmLocaleObserverSup*>( aThis );
    ASSERT_TRACE( self );
    TRAPD( err, self->EnvChangeOccurredL() );
    ERROR( err, "Failed to handle environment change" );
    return err;
    }


// ---------------------------------------------------------------------------
// MakeFileNameL
// ---------------------------------------------------------------------------
//
static HBufC* MakeFileNameL( const TDesC& aPath, const TDesC& aName, const TInt aCode )
    {
    FUNC_LOG;
    INFO_3( "Locale data dir: %S, file name base: %S, lang code: %d", &aPath, &aName, aCode );

    TInt length = aPath.Length();

    // Reserve spave for language code also
    HBufC* buffer = HBufC::NewL( length + aName.Length() + 10 );
    TPtr ptr = buffer->Des();
    ptr.Append( aPath );
    if ( length > 0 && aPath[ length - 1 ] != KPathDelimiter )
        {
        ptr.Append( KPathDelimiter );
        }

    ptr.Append( aName );
    if ( aCode < 10 )
        {
        ptr.AppendNum( 0 ); // add leading zero to the extention
        }
    ptr.AppendNum( aCode );

    INFO_1( "Locale data file: %S", buffer );

    return buffer;
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmLocaleObserverSup::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmUtility* CSsmLocaleObserverSup::NewL()
    {
    CSsmLocaleObserverSup* self = new ( ELeave ) CSsmLocaleObserverSup;
    return self;
    }


// ---------------------------------------------------------------------------
// CSsmLocaleObserverSup::~CSsmLocaleObserverSup
// ---------------------------------------------------------------------------
//
CSsmLocaleObserverSup::~CSsmLocaleObserverSup()
    {
    FUNC_LOG;

    Cancel();
    delete iEnvChangeNotifier;
    iFs.Close();
    iProperty.Close();
    }


// ---------------------------------------------------------------------------
// CSsmLocaleObserverSup::EnvChangeOccurredL
// ---------------------------------------------------------------------------
//
void CSsmLocaleObserverSup::EnvChangeOccurredL()
    {
    FUNC_LOG;

    const TInt changes( iEnvChangeNotifier->Change() );
    INFO_2( "Env data changed: 0x%08x, restore active: %d",
        changes, iRestoreActive );

    if ( !iRestoreActive && ( changes & EChangesLocale ) )
        {
        TParsePtrC parse( KLocaleDataDir );
        SaveLocaleL( parse.FullName() );
        }

    if( changes & EChangesSystemTime )
        {
        INFO( "Persisting HAL settings" );
        // Changes by writing to HAL
        TInt err = BaflUtils::PersistHAL();
        ERROR( err, "BaflUtils::PersistHAL failed" );
        }
    }


// ---------------------------------------------------------------------------
// CSsmLocaleObserverSup::RunL
// ---------------------------------------------------------------------------
//
void CSsmLocaleObserverSup::RunL()
    {
    FUNC_LOG;

    TInt err = iStatus.Int();
    ERROR( err, "RSsmStateAdaptation::NotifyCoopSysEvent completed with error" );

    if ( err != KErrServerTerminated )
        {
        Activate();
        }

    if ( err == KErrNone )
        {
        TInt status( 0 );
        err = iProperty.Get( status );
        ERROR( err, "Failed to get backup status from P&S" );

        if ( err == KErrNone )
            {
            TInt type = status & conn::KBURPartTypeMask;
            if ( type == conn::EBURRestoreFull ||
                 type == conn::EBURRestorePartial )
                {
                INFO( "Restore started" );

                iRestoreActive = ETrue;
                }
            else if ( type == conn::EBURNormal && iRestoreActive )
                {
                INFO( "Restore finished" );

                iRestoreActive = EFalse;

                TParsePtrC parse( KLocaleDataDir );
                TRAP( err, LoadLocaleL( parse.FullName() ) );
                ERROR( err, "Failed to load locale" );
                // Can not do anything about the error - just continue.
                }
            }
        }
    }


// ---------------------------------------------------------------------------
// CSsmLocaleObserverSup::DoCancel()
// ---------------------------------------------------------------------------
//
void CSsmLocaleObserverSup::DoCancel()
    {
    FUNC_LOG;

    iProperty.Cancel();
    }


// ---------------------------------------------------------------------------
// CSsmLocaleObserverSup::InitializeL()
// ---------------------------------------------------------------------------
//
void CSsmLocaleObserverSup::InitializeL()
    {
    FUNC_LOG;

    TInt err = iFs.Connect();
    ERROR( err, "Failed to connect to file server" );
    User::LeaveIfError( err );

    TRAP( err, iEnvChangeNotifier = CEnvironmentChangeNotifier::NewL(
        CActive::EPriorityStandard,
        TCallBack( EnvChangeNotifierCallback, this ) ) );
    ERROR( err, "Failed to create environment change notifier" );
    User::LeaveIfError( err );
    }


// ---------------------------------------------------------------------------
// CSsmLocaleObserverSup::StartL()
// ---------------------------------------------------------------------------
//
void CSsmLocaleObserverSup::StartL()
    {
    FUNC_LOG;

    TParsePtrC parse( KLocaleDataDir );
    TRAPD_ERR( err, LoadLocaleL( parse.FullName() ) );
    ERROR( err, "Failed to load locale data the first time" );

    if ( iEnvChangeNotifier )
        {
        iEnvChangeNotifier->Start();
        }

    TInt err1 = iProperty.Attach( SsmMapperUtility::PsUid( KUidSystemCategory ), conn::KUidBackupRestoreKey );
    ERROR( err1, "Failed to attach to KUidBackupRestoreKey" );

    Activate();
    }


// ---------------------------------------------------------------------------
// CSsmLocaleObserverSup::Release()
// ---------------------------------------------------------------------------
//
void CSsmLocaleObserverSup::Release()
    {
    FUNC_LOG;

    delete this;
    }


// ---------------------------------------------------------------------------
// CSsmLocaleObserverSup::CSsmLocaleObserverSup
// ---------------------------------------------------------------------------
//
CSsmLocaleObserverSup::CSsmLocaleObserverSup()
  : CActive( EPriorityNormal ),
    iRestoreActive( EFalse )
    {
    FUNC_LOG;

    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// CSsmLocaleObserverSup::Activate
// ---------------------------------------------------------------------------
//
void CSsmLocaleObserverSup::Activate()
    {
    FUNC_LOG;
    ASSERT_TRACE( !IsActive() );

    iProperty.Subscribe( iStatus );
    SetActive();
    }


// ---------------------------------------------------------------------------
// CSsmLocaleObserverSup::SaveLocaleL
// ---------------------------------------------------------------------------
//
void CSsmLocaleObserverSup::SaveLocaleL( const TDesC& aPath )
    {
    FUNC_LOG;
    INFO( "Saving locale data" );

    HBufC* fName = MakeFileNameL( aPath, KFileName, User::Language() );
    CleanupStack::PushL( fName );

    RFileWriteStream fStream;
    CleanupClosePushL( fStream );
    // According to documentation RFileWriteStream
    // keeps written data in RAM until CommitL has been
    // called.

    TInt err = iFs.MkDirAll( *fName ); // Ignore errors
    err = fStream.Create( iFs, *fName, EFileWrite );
    if ( err == KErrAlreadyExists )
        {
        // Override
        err = fStream.Open( iFs, *fName, EFileWrite );
        ERROR_1( err, "Failed to open stream %S", fName );
        }
    User::LeaveIfError( err );

    // Write first the version number to enable
    // support for file format changes.
    fStream.WriteInt8L( KCurrentVersionNumber );

    TLocale locale; // copy current values
    fStream.WriteInt16L( locale.CountryCode() );
    fStream.WriteInt16L( 0 ); // reserved
    fStream.WriteInt8L( locale.DateFormat() );
    fStream.WriteInt8L( locale.TimeFormat() );
    fStream.WriteInt8L( locale.CurrencySymbolPosition() );
    fStream.WriteInt8L( locale.CurrencySpaceBetween() );
    fStream.WriteInt8L( locale.CurrencyDecimalPlaces() );
    fStream.WriteInt8L( locale.CurrencyNegativeInBrackets() );
    fStream.WriteInt8L( locale.CurrencyTriadsAllowed() );
    fStream.WriteUint32L( locale.ThousandsSeparator() );
    fStream.WriteUint32L( locale.DecimalSeparator() );
    fStream.WriteInt8L( KMaxDateSeparators ); // restoring this constant. It might chance.
    TInt counter( 0 );
    for ( counter = 0; counter < KMaxDateSeparators; counter++ )
        {
        fStream.WriteUint32L( locale.DateSeparator( counter ) );
        }
    fStream.WriteInt8L( KMaxTimeSeparators ); // restoring this constant. It might chance.
    for ( counter = 0; counter < KMaxTimeSeparators; counter++ )
        {
        fStream.WriteUint32L( locale.TimeSeparator( counter ) );
        }
    fStream.WriteInt8L( locale.AmPmSpaceBetween() );
    fStream.WriteInt8L( locale.AmPmSymbolPosition() );

    // DaylightSaving and HomeDaylightSavingZone are not part of TLocale
    // any more, zeros are written to LocaleData.Dxx to keep file structure
    // as it was.
    fStream.WriteInt32L( 0 );
    fStream.WriteUint32L( 0 );

    fStream.WriteUint8L( locale.WorkDays() );
    fStream.WriteInt8L( locale.StartOfWeek() );
    fStream.WriteInt8L( locale.ClockFormat() );
    fStream.WriteInt8L( locale.UnitsGeneral() );
    fStream.WriteInt8L( locale.UnitsDistanceShort() );
    fStream.WriteInt8L( locale.UnitsDistanceLong() );
    fStream.WriteInt8L( locale.NegativeCurrencyFormat() );
    fStream.WriteInt8L( locale.NegativeLoseSpace() );
    fStream.WriteInt8L( locale.NegativeCurrencySymbolOpposite() );
    fStream.WriteInt16L( locale.DigitType() );

    // Then save display language independent data
    SaveIndependentDataL( locale, aPath );

    // If CommitL leaves it means probably full disk.
    // It is here assumed that data has not been changed if this leaves.
    fStream.CommitL();

    CleanupStack::PopAndDestroy( &fStream );
    CleanupStack::PopAndDestroy( fName );
    }


// ---------------------------------------------------------------------------
// CSsmLocaleObserverSup::LoadLocaleL
// ---------------------------------------------------------------------------
//
void CSsmLocaleObserverSup::LoadLocaleL( const TDesC& aPath )
    {
    FUNC_LOG;
    INFO( "Loading locale data" );

    HBufC* fName = MakeFileNameL( aPath, KFileName, User::Language() );
    CleanupStack::PushL( fName );

    TLocale locale; // copy current values

    RFileReadStream fStream;
    CleanupClosePushL( fStream );
    TInt err = fStream.Open( iFs, *fName, EFileRead );
    if ( err != KErrNotFound && err != KErrPathNotFound )
        {
        ERROR_1( err, "Failed to open stream %S", fName );
        // Data file is missing upon the first boot or when switching into a
        // language for the first time
        }

    if ( err == KErrNone  )
        {
        TInt version = fStream.ReadInt8L();
        INFO_1( "Locale file version: %d", version );

        locale.SetCountryCode( fStream.ReadInt16L() );
        fStream.ReadInt16L(); // obsolete
        locale.SetDateFormat( static_cast< TDateFormat >( fStream.ReadInt8L() ) );
        locale.SetTimeFormat( static_cast< TTimeFormat >( fStream.ReadInt8L() ) );
        locale.SetCurrencySymbolPosition( static_cast< TLocalePos >( fStream.ReadInt8L() ) );
        locale.SetCurrencySpaceBetween( fStream.ReadInt8L() );
        locale.SetCurrencyDecimalPlaces( fStream.ReadInt8L() );
        locale.SetCurrencyNegativeInBrackets( fStream.ReadInt8L() );
        locale.SetCurrencyTriadsAllowed( fStream.ReadInt8L() );
        locale.SetThousandsSeparator( fStream.ReadUint32L() );
        locale.SetDecimalSeparator( fStream.ReadUint32L() );
        TInt dateSeparators = fStream.ReadInt8L();
        TInt counter( 0 );
        for ( counter = 0; counter < dateSeparators ; counter++ )
            {
            locale.SetDateSeparator( fStream.ReadUint32L(), counter );
            }
        TInt timeSeparators = fStream.ReadInt8L();
        for ( counter = 0; counter < timeSeparators; counter++ )
            {
            locale.SetTimeSeparator( fStream.ReadUint32L(), counter );
            }
        locale.SetAmPmSpaceBetween( fStream.ReadInt8L() );
        locale.SetAmPmSymbolPosition( static_cast< TLocalePos >( fStream.ReadInt8L() ) );

        // DaylightSaving and HomeDaylightSavingZone are not part of TLocale
        // any more, zeros are written to LocaleData.Dxx to keep file structure
        // as it was.
        fStream.ReadUint32L();
        fStream.ReadUint32L();

        locale.SetWorkDays( fStream.ReadUint8L() );
        locale.SetStartOfWeek( static_cast< TDay >( fStream.ReadInt8L() ) );
        locale.SetClockFormat( static_cast< TClockFormat>( fStream.ReadInt8L() ) );
        locale.SetUnitsGeneral( static_cast< TUnitsFormat >( fStream.ReadInt8L() ) );
        locale.SetUnitsDistanceShort( static_cast< TUnitsFormat >( fStream.ReadInt8L() ) );
        locale.SetUnitsDistanceLong( static_cast< TUnitsFormat >( fStream.ReadInt8L() ) );
        locale.SetNegativeCurrencyFormat(
            static_cast< TLocale::TNegativeCurrencyFormat >( fStream.ReadInt8L() ) );
        locale.SetNegativeLoseSpace( fStream.ReadInt8L() );
        locale.SetNegativeCurrencySymbolOpposite( fStream.ReadInt8L() );
        locale.SetDigitType( static_cast< TDigitType >( fStream.ReadInt16L() ) );
        }

    // Then patch data with locale independent data (code 00)
    // No changes to locale if no independent data can be found (the very first boot)
    LoadIndependentDataL( locale, aPath );

    // Save changes to the system.
    locale.Set();

    CleanupStack::PopAndDestroy( &fStream );
    CleanupStack::PopAndDestroy( fName );
    }


// ---------------------------------------------------------------------------
// CSsmLocaleObserverSup::SaveIndependentDataL
// ---------------------------------------------------------------------------
//
void CSsmLocaleObserverSup::SaveIndependentDataL(
    const TLocale& aLocale,
    const TDesC& aPath )
    {
    FUNC_LOG;

    // Get old independent data, if any.
    TLocale savedLoc;
    TRAPD( err, LoadIndependentDataL( savedLoc, aPath ) );
    ERROR( err, "Failed to load locale independent data" );

    HBufC* fName = MakeFileNameL( aPath, KCommonFileName, 0 );
    CleanupStack::PushL( fName );
    RFileWriteStream fStream;
    CleanupClosePushL( fStream );

    err = iFs.MkDirAll( *fName ); // Ignore errors
    err = fStream.Create( iFs, *fName, EFileWrite );
    if ( err == KErrAlreadyExists )
        {
        // Override
        err = fStream.Open( iFs, *fName, EFileWrite );
        ERROR_1( err, "Failed to create stream %S", fName );
        }
    User::LeaveIfError( err );

    // Write first the version number to enable support for file format changes.
    fStream.WriteInt8L( KCurrentVersionNumber );
    fStream.WriteInt32L( 0 ); // Universal time offset is not part of TLocale
                              // any more. Write zero here to keep file structure.
    // Clock format is also common.
    fStream.WriteUint32L( aLocale.ClockFormat() );
    fStream.WriteUint32L( 0 );  // reserved 2
    fStream.WriteUint32L( 0 );  // reserved 3

    fStream.CommitL();

    CleanupStack::PopAndDestroy( &fStream );
    CleanupStack::PopAndDestroy( fName );
    }


// ---------------------------------------------------------------------------
// CSsmLocaleObserverSup::LoadIndependentDataL
// ---------------------------------------------------------------------------
//
void CSsmLocaleObserverSup::LoadIndependentDataL(
    TLocale& aLocale,
    const TDesC& aPath )
    {
    FUNC_LOG;

    HBufC* fName = MakeFileNameL( aPath, KCommonFileName, 0 );
    CleanupStack::PushL( fName );

    RFileReadStream fStream;
    CleanupClosePushL( fStream );
    TInt err = fStream.Open( iFs, *fName, EFileRead );
    if ( err != KErrNotFound && err != KErrPathNotFound )
        {
        ERROR_1( err, "Failed to open stream %S", fName );
        // Data file is missing upon the first boot or when switching into a
        // language for the first time
        }

    if ( err == KErrNotFound || err == KErrPathNotFound )
        {
        // File not found --> Not an error because
        // this is a new file and older builds do not have this file.
        INFO( "No locale data found" );
        }
    else
        {
        User::LeaveIfError( err );

        fStream.ReadInt8L();    // Version.
        fStream.ReadInt32L();   // Universal time offset was stored here.
        aLocale.SetClockFormat( ( TClockFormat ) fStream.ReadUint32L() );
        fStream.ReadUint32L(); // reserved 2
        fStream.ReadUint32L(); // reserved 3
        }

    CleanupStack::PopAndDestroy( &fStream );
    CleanupStack::PopAndDestroy( fName );
    }
