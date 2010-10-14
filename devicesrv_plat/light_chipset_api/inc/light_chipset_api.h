/*
  light_chipset_api.h

  Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
  All rights reserved.

  This program and the accompanying materials are made available 
  under the terms of the Eclipse Public License v1.0 which accompanies 
  this distribution, and is available at 
  http://www.eclipse.org/legal/epl-v10.html

  Initial Contributors:
  Nokia Corporation - initial contribution.

  Contributors:
*/

/** @file
@brief Light Chipset API H

This is the header file for Light Chipset API that is used to access 
the light service provided by the third party chipset vendor.

@publishedDeviceAbstraction
*/

#ifndef __LIGHT_CHIPSET_API_H__
#define __LIGHT_CHIPSET_API_H__

// Include files

#include <kern_priv.h>


// Constants


// Macros


// Data types

/**  
* RGB values.
*
*/

struct TLightColor
{
    TUint8 iRed;
    TUint8 iGreen;
    TUint8 iBlue;
};

// Function prototypes


// Class declaration

/**
This class defines the Chipset API that is used to access 
the light service provided by the third party chipset vendor.
*/
class LightService
    {
    public:

        /**
        This method is used to do all the necessary initialization so 
        that the chipset is ready to provide the light service.

        @return Symbian OS system wide error code, and the followings are required: 
                KErrNone – the operation is successfully executed
                KErrGeneral – error condition
                KErrAlreadyExists – if trying to initialize twice

        @pre Pre-condition, it should be called first before the other methods of this class are used 
        */
        IMPORT_C static TInt Init();

        /**
        This function is used to set the specified output to the desired intensity.

        @param aOutputId The identity of the output line that shall be controlled
        @param aIntensity	The desired intensity of the light with range 0 – 10000 [1/100%]

        @return Symbian OS system wide error code, and the followings are required: 
                KErrNone – the operation is successfully executed
                KErrGeneral – error condition
                KErrNotReady –if trying to access uninitialized device
                KErrNotSupported – if the feature is not supported by the HW

        @pre Pre-condition, the method Init() has been executed successfully
        */
        IMPORT_C static TInt SetIntensity(TUint aOutputId, TUint16 aIntensity);

        /**
        This function is used to set the specified output to the desired PWM frequency.

        @param aOutputId The identity of the output line that shall be controlled
        @param aFrequency	The PWM frequency from 0 to the maximum frequency available from the chipset (range 0 to 65535Hz)

        @return Symbian OS system wide error code, and the followings are required: 
                KErrNone – the operation is successfully executed
                KErrGeneral – error condition
                KErrNotReady – if trying to access uninitialized device
                KErrNotSupported – if the feature is not supported by the HW

        @pre Pre-condition, the method Init() has been executed successfully
        */
        IMPORT_C static TInt SetFrequency(TUint aOutputId, TUint16 aFrequency);
        /**
        This function is used to set the color of the LED target.

        @param aOutputId The identity of the output line that shall be controlled
        @param TLightColor	 	   The Light color structure to specify RGB values

        @return Symbian OS system wide error code, and the followings are required: 
                KErrNone – the operation is successfully executed
                KErrGeneral – error condition
                KErrNotReady – if trying to access uninitialized device
                KErrNotSupported – if the feature is not supported by the HW

        @pre Pre-condition, the method Init() has been executed successfully
        */
        IMPORT_C static TInt SetColor(TUint aOutputId, TLightColor aRGB);

    };

#endif // __LIGHT_CHIPSET_API_H__
