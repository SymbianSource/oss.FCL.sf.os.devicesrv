/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Plugin for reading effect information from IVT effect data.
*
*/

#ifndef C_HWRMHAPTICSIVTGETTER_H
#define C_HWRMHAPTICSIVTGETTER_H

#include <e32base.h>

#include <hwrmhapticseffectdatagetter.h>
#include "ivtinternal.h"

/**
 *  Ecom plugin for getting haptics ivt data.
 *
 *  @lib hapticsivtgetterplugin.lib
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CHWRMHapticsIVTGetter ): public CHWRMHapticsEffectDataGetter
    {
public:

    /**
     * 2-phased constructor.
     * @return An instance of haptics ivt getter.
     */
    static CHWRMHapticsIVTGetter* NewL();

    /**
     * Destructor.
     */
    virtual ~CHWRMHapticsIVTGetter();

public: // From CHWRMHapticsEffectDataGetter

    /**
     * @see CHWRMHapticsEffectDataGetter
     */
    TInt GetEffectCount( const TDesC8& aData, TInt& aCount );

    /**
     * @see CHWRMHapticsEffectDataGetter
     */
    TInt GetEffectName( const TDesC8& aData,
                        TInt aEffectIndex, 
                        TDes8& aEffectName );

    /**
     * @see CHWRMHapticsEffectDataGetter
     */
    TInt GetEffectIndexFromName( const TDesC8& aData,
                                 const TDesC8& aEffectName,
                                 TInt& aEffectIndex );

    /**
     * @see CHWRMHapticsEffectDataGetter
     */
    TInt GetEffectType( const TDesC8& aData, 
                        TInt aEffectIndex, 
                        TInt& aEffectType );

    /**
     * @see CHWRMHapticsEffectDataGetter
     */ 
    TInt GetMagSweepEffectDefinition( 
                      const TDesC8& aData, 
                      TInt aEffectIndex, 
                      CHWRMHaptics::THWRMHapticsMagSweepEffect& aEffect );
                                                 
    /**
     * @see CHWRMHapticsEffectDataGetter
     */
    TInt GetPeriodicEffectDefinition( 
                      const TDesC8& aData, 
                      TInt aEffectIndex, 
                      CHWRMHaptics::THWRMHapticsPeriodicEffect& aEffect );
                
    /**
     * @see CHWRMHapticsEffectDataGetter
     */
    TInt GetEffectDuration( const TDesC8& aData, 
                            TInt aEffectIndex, 
                            TInt& aEffectDuration );

private:

    /**
     * Constructor.
     */
    CHWRMHapticsIVTGetter();

    /**
     * 2nd phase constructor.
     */
    void ConstructL();
    
private:

    /**
     * Utility method for fetching the effect definition using effect data
     * and index.
     */
    TInt GetEffectByIndex( const TDesC8& aData,  
                           TInt aEffectIndex,  
                           TVibeEffectDefinition* aEffectDefinition );

    };


#endif // C_HWRMHAPTICSIVTGETTER_H
