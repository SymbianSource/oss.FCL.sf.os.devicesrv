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
* Description:  This API provides access to the display driver.
*
*/


#ifndef __DDC_ACCESS_H__
#define __DDC_ACCESS_H__


//- Include Files  ----------------------------------------------------------

#include <e32base.h>

//- Enumerations and consts -------------------------------------------------

/** API version number:
 1:  First version */
const TUint KDdcAccessVersion = 1;

///One DDC segment size
const TUint KSizeOfSegment = 256;

/** One data block array, See that EDID block size is just 128 bytes, so one read contains one EDID extension (if exists) */
typedef TUint8 TDataBlock[ KSizeOfSegment ];

///DDC port enumerations
enum TDdcPort
    {
    EUndefinedPort = 0x00, //!<Initialisation value, don't use
    EMonitorPort   = 0xA0, //!<0xA0/0xA1, DDC monitor port. Normally for EDID, but if EDID is not supported here is DisplayID
    EDisplayIdPort = 0xA4, //!<0xA4/0xA5, if EDID is supported, here may exists DisplayID
    };

//- Namespace ---------------------------------------------------------------


//- Using -------------------------------------------------------------------


//- Data Types --------------------------------------------------------------


//- Constants ---------------------------------------------------------------


//- Macros ------------------------------------------------------------------


//- External Data -----------------------------------------------------------


//- Variables ---------------------------------------------------------------


//- Forward Declarations ----------------------------------------------------


//- Class Definitions -------------------------------------------------------

/**
Adaptation API for VESA's E-DDC (Enhanced Display Data Channel) raw read
*/
class CDdcPortAccess : public CBase
    {
    public:
        /** Gets API version number. Compare this to KDdcAccessVersion, must match.
            @return  KDdcAccessVersion which is used for implementation */
        IMPORT_C static TUint ApiVersion();

        /** Creates object.
            Method requires ECapabilityReadDeviceData capability.
            @return  new CDdcPortAccess, NULL if there is error, otherwise CDdcPortAccess */
        IMPORT_C static CDdcPortAccess* NewL();
        ///Destructor
        virtual ~CDdcPortAccess(){};

        /** Starts DDC read from wanted port.

            User should first read EMonitorPort, if there is EDID found, then read also EDisplayIdPort.
            If DisplayID is found from EMonitorPort, then it is not needed to read EDisplayIdPort.

            DisplayID is "second generation" of EDID, and it is proposed to be used.

            See the method reads one TDataBlock, and it size is double of one EDID block,
            so the first EDID read might get base-EDID and first extension, if exists.
            With variable size DisplayID, method reads anyway whole size of TDataBlock,
            even DisplayID section is smaller.

            @param  aDdcPort            DDC port which to be read
            @param  aBlockNumber        Datablock which to be read (parameter maps to segment-register 0x60)
            @param  aDataBlock          Method fills this with read values. Ensure that this data exists all the time, be careful if local variable.
            @param  aCompletedWhenRead  Will be completed when ready, if no errors, then aDataBlock contains read values
            @return KErrNone if success */
        virtual TInt Read(TDdcPort aDdcPort, TUint aBlockNumber, TDataBlock& aDataBlock, TRequestStatus& aCompletedWhenRead) = 0;

        /** Cancels all TRequestStatuses.
            @return KErrNone if success */
        virtual void CancelAll() = 0;

    protected:
        ///Constructor
        CDdcPortAccess(){};
        ///Second phase constructor
        virtual void ConstructL() = 0;
    };


//- Global Function Prototypes ----------------------------------------------


//- Inline Functions --------------------------------------------------------


#endif //__DDC_ACCESS_H__

// End of File

