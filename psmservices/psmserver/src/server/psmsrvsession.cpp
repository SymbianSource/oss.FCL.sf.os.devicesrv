/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PSM Server session
*
*/

#include <e32property.h>
#include <connect/sbdefs.h>
#include <psmsettingsprovider.h>
#include "psmmanager.h"
#include "psmsrvserver.h"
#include "psmsrvsession.h"
#include "psmsrvmessage.h"
#include "psmclientserver.h"
#include "psmtrace.h"

// -----------------------------------------------------------------------------
// CPsmSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmSession* CPsmSession::NewL( CPsmManager& aPsmManager, CPsmSrvServer& aServer )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::NewL()" ) ) );

    CPsmSession* self = CPsmSession::NewLC( aPsmManager, aServer );
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::NewL - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmSession::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmSession* CPsmSession::NewLC( CPsmManager& aPsmManager, CPsmSrvServer& aServer )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::NewLC()" ) ) );

    CPsmSession* self = new( ELeave ) CPsmSession( aPsmManager, aServer );

    CleanupStack::PushL( self );
    self->ConstructL();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::NewLC - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmSession::CPsmSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsmSession::CPsmSession( CPsmManager& aPsmManager, CPsmSrvServer& aServer ) :
    iPsmServer( aServer ),
    iPsmManager( aPsmManager )
    {
    // Nothing to do
    }

// -----------------------------------------------------------------------------
// CPsmSession::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPsmSession::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::ConstructL()" ) ) );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::ConstructL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CPsmSession::~CPsmSession()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::~CPsmSession()" ) ) );

    // Notify server that session is closed
    iPsmServer.SessionClosed();

    // Complete notify message if it is not null
    if ( iNotifyModeMessage )
        {
        iNotifyModeMessage->Complete( KErrCancel );
        // Unregister observer from manager
        iPsmManager.UnregisterObserver( iNotifyModeMessage );
        // Finally delete message
        delete iNotifyModeMessage;
        }

    else
        {
        COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::~CPsmSession - No pending requests") ) );
        }

    // Reset and close config array
    iConfigArray.Reset();
    iConfigArray.Close();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::~CPsmSession - return") ) );
    }

// -----------------------------------------------------------------------------
// CPsmSession::ServiceL
// -----------------------------------------------------------------------------
//
void CPsmSession::ServiceL( const RMessage2& aMessage )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::ServiceL()" ) ) );

    TRAPD( error, DispatchL( aMessage ) );

    // Complete message with error code if there were errors
    if ( KErrNone != error )
        {
        COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::ServiceL() - Error: %i" ), error ) );
        aMessage.Complete( error );
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::ServiceL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmSession::DispatchL
// -----------------------------------------------------------------------------
//
void CPsmSession::DispatchL( const RMessage2& aMessage )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::DispatchL()" ) ) );

    switch ( aMessage.Function() )
        {
        case EPsmServerNotifyModeChange: // For power save mode change
            {
            TInt mode = aMessage.Int0();
            if ( !iNotifyModeMessage )
                {
                // Create message wrapper if not yet created
                iNotifyModeMessage = CPsmMessage::NewL( iPsmManager );
                iPsmManager.RegisterObserver( iNotifyModeMessage );
                }

            // Check that mode to be set is valid, otherwise error code.
            if( mode == iPsmManager.SettingsProvider().Mode() )
                {
                User::Leave( KErrAlreadyExists );
                }

            if( ( mode > EPsmsrvPartialMode ) || ( mode < EPsmsrvModeNormal ) )
                {
                User::Leave( KErrArgument );
                }
			
            TInt keyVal = -1;
            User::LeaveIfError( RProperty::Get( KUidSystemCategory, conn::KUidBackupRestoreKey, keyVal ) );
			//if backup or restore is in progress, change in power save mode
			//is not allowed as it will not be possible to do write
			//operation in cenrep and it would leave the device in unstable power saving mode state
            if( (( keyVal & conn::KBURPartTypeMask ) != conn::EBURNormal ) 
				&& (( keyVal & conn::KBURPartTypeMask ) != conn::EBURUnset ))
				{
				User::Leave( KErrNotSupported );
				}
			
            iNotifyModeMessage->Initialize( aMessage );
            iPsmManager.NotifyPowerSaveModeChangeL( mode );
            break;
            }
        case EPsmServerChangeSettings: // For PSM settings
        case EPsmServerGetSettings: // For PSM settings
        case EPsmServerBackupSettings: // For PSM settings
            {
            // Check previous config
            if ( iConfigArray.Count() > 0 )
                {
                iConfigArray.Reset();
                }

            // Handle settings requests in different function
            HandleSettingsRequestL( aMessage );
            break;
            }

        case EPsmServerCancelModeChange: // For cancelling PSM change
            {
            // Cancel PSM change and return previous mode
            if ( iNotifyModeMessage )
                {
                // Unregister observer from manager
                iPsmManager.UnregisterObserver( iNotifyModeMessage );
                // Cancel pending message
                iNotifyModeMessage->Complete( KErrCancel );
                // Finally delete message
                delete iNotifyModeMessage;
                iNotifyModeMessage = NULL;
                }

            // Call PsmManager to stop mode change and to go back to previous
            iPsmManager.CancelPowerSaveModeChangeL();
            // Complete message
            aMessage.Complete( KErrNone );
            break;
            }
        case EPsmServerModeChangeNotificationRequest: // PSM Change notification request
            {
            if ( !iNotifyModeMessage )
                {
                // Create message wrapper if not yet created
                iNotifyModeMessage = CPsmMessage::NewL( iPsmManager );
                iPsmManager.RegisterObserver( iNotifyModeMessage );
                }
            iNotifyModeMessage->Initialize( aMessage );
            break;
            }
        case EPsmServerCancelModeChangeNotificationRequest: // For cancelling PSM change notification
            {
            // Cancel PSM change notification request
            if ( iNotifyModeMessage )
                {
                // Unregister observer from manager
                iPsmManager.UnregisterObserver( iNotifyModeMessage );
                // Cancel pending message
                iNotifyModeMessage->Complete( KErrCancel );
                // Finally delete message
                delete iNotifyModeMessage;
                iNotifyModeMessage = NULL;
                }
            // Complete message
            aMessage.Complete( KErrNone );
            break;
            }
        default:
            {
            COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::ServiceL() - unknown request: %i - ERROR" ), aMessage.Function() ) );
            User::Leave( KErrUnknown );
            }
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::DispatchL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmSession::HandleSettingsRequestL
// -----------------------------------------------------------------------------
//
void CPsmSession::HandleSettingsRequestL( const RMessage2& aMessage )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::HandleSettingsRequestL()" ) ) );

    // read config array from message
    ReadConfigArrayL( iConfigArray, aMessage );
    // Get storage UID
    TUint32 storage = aMessage.Int2();

    TInt err( KErrNone );

    if ( aMessage.Function() == EPsmServerBackupSettings )
        {
        iPsmManager.SettingsProvider().BackupSettingsL( iConfigArray, storage );
        }
    else 
        {
        if ( aMessage.Function() == EPsmServerGetSettings )
            {
            iPsmManager.SettingsProvider().GetSettingsL( iConfigArray, storage );
            }
        else
            {
            // EPsmServerChangeSettings
            iPsmManager.SettingsProvider().BackupAndGetSettingsL( iConfigArray, storage );
            }

        // We have to write changed values back to message
        TInt arraySize( iConfigArray.Count() * sizeof( TPsmsrvConfigInfo ) );
        TPtr8 arrayPtr( reinterpret_cast<TUint8*>(&iConfigArray[0]), arraySize, arraySize );
        err = aMessage.Write( 0, arrayPtr );
        ERROR_TRACE( ( _L( "PSM Server - CPsmSession::HandleSettingsRequestL - New data wrote to message: %i" ), err ) );
        }
    // Complete message before destroying local config array
    aMessage.Complete( err );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::HandleSettingsRequestL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmSession::ReadConfigArrayL
// -----------------------------------------------------------------------------
//
void CPsmSession::ReadConfigArrayL( RConfigInfoArray& aArray, const RMessage2& aMessage )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::ReadConfigArrayL()" ) ) );

    const TInt configCount( aMessage.Int1() );
    TInt arrayLength( configCount * sizeof( TPsmsrvConfigInfo ) );

    // There is no ResizeL() for RArray
    // ReserveL() does not change iCount, which will
    // result array[0] failure (access beyond array index)
    // That is why we fill the array with dummy items
    for(TInt x = 0; x < configCount; ++x )
    	{
        // Append empty config infos to array
    	aArray.AppendL( TPsmsrvConfigInfo() );
    	}

    TPtr8 arrayPtr( ( TUint8* )&aArray[0], arrayLength, arrayLength );
    aMessage.ReadL( 0, arrayPtr, 0 );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSession::ReadConfigArrayL - return" ) ) );
    }

// End of file
