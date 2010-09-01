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
* Description: Declaration of CSsmLocaleObserverSup class.
*
*/

#ifndef C_SSMLOCALEOBSERVERSUP_H
#define C_SSMLOCALEOBSERVERSUP_H

#include <e32base.h>
#include <e32property.h>
#include <f32file.h>
#include <ssm/ssmutility.h>

class CEnvironmentChangeNotifier;

/**
* SSM Utility plugin which stores locale data to file when it changes and
* after data has been restored from a backup file.
*/
NONSHARABLE_CLASS( CSsmLocaleObserverSup ) : public CActive, public MSsmUtility
    {

public:

    /**
    * Two-phase constructor.
    */
	IMPORT_C static MSsmUtility* NewL();

    /**
    * Destructor.
    */
    virtual ~CSsmLocaleObserverSup();

    /**
    * Call-back method, called when there is a change in locale data.
    */
    void EnvChangeOccurredL();

private: // From CActive

	void RunL();
	void DoCancel();

private: // From MSsmUtility

    void InitializeL();
	void StartL();
	void Release();

private:

    /**
    * First phase constructor.
    */
    CSsmLocaleObserverSup();

    /**
    * Activate the active object.
    */
    void Activate();

    /**
    * Copied fron SysLocale.
    * SaveLocaleL saves TLocale class to the given directory
    * and initiates missing independent data, if any.
    * Note, that independent data is created only once and it is
    * shared by all the locales. Therefore, it is recommended
    * to call this method upon early boot phase of a device.
    * @param aPath Directory path containing locale data files.
    *              (e.g. "c:\dir" or c:\dir\")
    */
    void SaveLocaleL( const TDesC& aPath );

    /**
    * Copied fron SysLocale.
    * LoadLocaleL restores TLocale class from the given directory.
    * If locale independent data (see note)
    * exists, its data overrides the restored TLocale data.
    * @param aPath Directory path containing locale data files
    *              (e.g. "c:\dir" or c:\dir\")
    */
    void LoadLocaleL( const TDesC& aPath );

    /**
    * Stores locale independent data.
    */
    void SaveIndependentDataL( const TLocale& aLocale, const TDesC& aPath );

    /**
    * Loads locale independent data.
    */
    void LoadIndependentDataL( TLocale& aLocale, const TDesC& aPath );

private: // data

    /**
    * Publish & subscribe interface for KUidBackupRestoreKey key.
    */
    RProperty iProperty;

    /**
    * Indicates if restore activity is currently taking place.
    */
    TBool iRestoreActive;

    /**
    * An environment change notifier that notifies us when any change
    * happens in the environment.
    */
    CEnvironmentChangeNotifier* iEnvChangeNotifier;

    /**
    * File server session for writing the locale data files.
    */
    RFs iFs;

    };

#endif // C_SSMLOCALEOBSERVERSUP_H
