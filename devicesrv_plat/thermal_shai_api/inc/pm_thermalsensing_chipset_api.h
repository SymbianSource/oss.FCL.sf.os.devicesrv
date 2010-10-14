/*
Nokia
CONFIDENTIAL
Deliverable software

            pm_thermalsensing_chipset_api.h
            --------------------------------

            SW Include Document - Symbian OS

* ============================================================================
*  Name     : pm_thermalsensing_chipset_api.h
*  Part of  : TP/PP/CoreSW/SASW, System Infra 
*  Origin   : System Power Knowledge Center, Nokia Corporation
*  Created  : Aditya Singh (16.05.2007)
*  Description:
*    This describes the class that determines the thermal sensor driver from a
*  hardware dependent way for battery temperature.
*
*  Continuus version  :
*    %version: 1 %,
*    %date_modified: Tue Oct 05 17:58:12 2010 %
*    by %derived_by: s95kumar %
*
*  Copyright (c) Nokia. This material, including documentation and any related
*  computer programs, is protected by copyright controlled by Nokia. All rights
*  are reserved. Copying, including reproducing, storing, adapting or
*  translating, any or all of this material requires the prior written consent
*  of Nokia. This material also contains confidential information which may
*  not be disclosed to others without the prior written consent of Nokia.
*  ---------------------------------------------------------------------------
*  Version history:
*  Template version: 1.0, 05.10.2004 by pkahonen
*  <ccm_history>
*
*  Version: 1
*  Ref: 417-41499
*  Initial Version of Thermal Sensing HW Adaptation Driver
*
*  </ccm_history>
* ============================================================================
*/

/** @file
@brief Chipset API H Template

This is a header template for Chipset API work. This template should not be
used as a normal code template because the location of where the API is
documented is different in normal coding style! Doxygen commenting style tries
to be similar what Symbian is using for their kernel side development.

You can define here also the scope of the whole API see below, or you can
define the scope separately before the class or even before the function if
that is needed.

@publishedDeviceAbstraction
 */

#ifndef __PM_THERMALSENSING_CHIPSET_API_H__
#define __PM_THERMALSENSING_CHIPSET_API_H__


// Include files

#include <kern_priv.h>

/**
Description of the function pointer

@note Description Callback Function which is being executed from DFC

@param TAny* Any type of parameter passed to the callback function
 */
typedef void (*TthermalSensorCbFn)(TAny* aParam);

// Forward declarations

class TthermalSensorCb;
class ChipsetThermalSensorHWPool;
class DThermalSensorHW;


// Class declaration
/**
 *
 * Thermal Sensor Callback function
 *
 */
class TthermalSensorCb : public TDfc
    {

public :
    /**
        Description of TthermalSensorCb
        This is the Thermal Sensor Callback Function
        @param aFn        Callback Function Pointer
        @param aParam     Any type of parameter passed to the callback function
        @param aPriority  Priority Assigned
        @return           KErrNone or a system wide error code to specify the effect of the method call.
        @pre              This can be called only after the ChipsetThermalSensorHWPool has been initialized 
                          in the system. Preferable way to call this function is from a thread context with 
                          interrupts enabled after all the kernel extensions have been initialized in the boot up.
     */
    IMPORT_C TthermalSensorCb(TthermalSensorCbFn aFn, TAny *aParam, TInt aPriority);
    /**
        Description of TthermalSensorCb
        This is the Thermal Sensor Callback Function
        @param aFn        Callback Function Pointer
        @param aParam     Any type of parameter passed to the callback function
        @param aPriority  Priority Assigned
        @param aQue       Interrupt Handling DFC queue. The same queue is used by the HW specific interface
        @return           KErrNone or a system wide error code to specify the effect of the method call.
        @pre              This can be called only after the ChipsetThermalSensorHWPool has been initialized 
                          in the system. Preferable way to call this function is from a thread context with 
                          interrupts enabled after all the kernel extensions have been initialized in the boot up.
     */
    IMPORT_C TthermalSensorCb(TthermalSensorCbFn aFn, TAny *aParam, TInt aPriority, TDfcQue* aQue);

private :

    static void CallbackFunc( TAny *);

private :
    /*** Any type of parameter passed to the callback function */
    TAny* iParam;

    /*** Callback Function Pointer */
    TthermalSensorCbFn iCallback;

    };

/**
@brief Class acting as a container class for several sensor objects

This class acts as a repository using which the references to the specific thermal sensor
interface can be accessed. The recommendation is to implement this as a Singleton class. 
 */
class ChipsetThermalSensorHWPool
    {
    // Methods
public :
    enum TthermalSensorID
        {
        /// Invalid Sensor ID to be used if system design does not have any thermal sensor.
        EInvalidThermalSensor = 0x0,
        /// Battery Specific Sensing
        EBatteryThermalSensor,
        /// DRAM Specific Sensing
        EDRAMThermalSensor,
        /// RF ASIC Specific Sensors
        ERFThermalSensor,
        /// Memory Card Specific Sensors
        EMemoryCardThermalSensor,
        /// SOC Specific Sensors
        EAPEThermalSensor,
        /// Camera Module Specific Sensors
        ECameraThermalSensor,
        /// GPS Module Specific Sensors
        EGPSThermalSensor,
        /// Display Module Specific Sensors
        EDisplayThermalSensor,
        /// Chipset Specific Sensors
        EChipsetCustom1ThermalSensor,
        EChipsetCustom2ThermalSensor,
        EChipsetCustom3hermalSensor,
        EChipsetCustom4ThermalSensor,
        EChipsetCustom5ThermalSensor,
        EChipsetCustom6ThermalSensor
        };

    /**
        Description of GetThermalSensorHW
        This is the method to get the pointer of the DThermalSensorHW object from the 
        ChipsetThermalSensorHWPool.
        @param aId     input parameter specifying the ID of the thermal sensor whose pointer we are 
                       trying to reference.
        @param aSensor output parameter giving the pointer of the thermal sensor object representing 
                       the ID requested or NULL if the thermal sensor do not exist in the system.
        @return        KErrNone or a system wide error code to specify the effect of the method call.
        @pre           This can be called only after the ChipsetThermalSensorHWPool has been initialized 
                       in the system. Preferable way to call this function is from a thread context with 
                       interrupts enabled after all the kernel extensions have been initialized in the boot up.
     */

    IMPORT_C  static TInt GetThermalSensorHW(TthermalSensorID aId, DThermalSensorHW** aSensor);

    /**
        Description of GetNumberOfSensors
        This is the method to get the number of sensors supported by the chipset adaptation software.  
        In addition it should give the highest and lowest identifier for the sensors to optimize the search 
        possibilities for the client software.
        @param aHighestID output parameter that gives the ID of the thermal sensor with the most positive value 
                          or 0 if no sensor is present.
        @param aLowestID  output parameter that gives the ID of the thermal sensor with the least positive value
                          or 0 if no sensor is present.
        @return           number of sensors in the system (+ve value or 0) or 
                          a system wide error code to specify the effect of the method call.
        @pre              This can be called only after the ChipsetThermalSensorHWPool has been initialized 
                          in the system.
     */
    IMPORT_C  static TInt GetNumberOfSensors(TthermalSensorID& aHighestID, TthermalSensorID& aLowestID);

    };


/**
 *
 * This class represents a generic thermal sensor that would be found in the HW. 
 * The pointer of the thermal sensor object would be passed to the client who can then call the virtual methods that are exposed by this interface. 
 *
 */
NONSHARABLE_CLASS( DThermalSensorHW ) : public DBase
    {
    public :
        /**
         Constructor
         Defined as a private to prevent clients to create own instances
         from this class
         @since  ver 1
         @param  none
         @return none
         @panic  none
         */
        DThermalSensorHW();

        /**
         Virtual Destructor
         @since  ver 1
         @param  none
         @return none
         @panic  none
         */
        virtual ~DThermalSensorHW();

        /**
         Description of InitHw
         This is the method by which the HW is initialized in the system. 
         The implementation has to take care that the HW is initialized only once even though this method can be called multiple times. 
         The main motivation for this method is that the client becomes sure about the functional state of the HW after it calls this method. 
         There are several scenarios possible when the interface is called. For example, 
                • If the HW cannot be initialized then the implementation should return KErrNotReady or KErrNotSupported depending on the scenario. 
                • If the HW can be initialized then the call would do the necessary initializations and reply with KErrNone. 
                • If the HW is already initialized, then the call should return immediately without changing the HW state and return value should also be KErrNone.
         @since  ver 1
         @param  none
         @return KErrNone if success
                 otherwise system wide error code to let the client know about the status of the HW
         @panic  none
         @ pre   This can be called at any time after the kernel extensions have been loaded. 
                 The preferable way to call this function is from a thread context with interrupts enabled.
         */
        virtual TInt InitHw() = 0;

        /**
         Description of SetupAlert
         This is the method to set up a callback mechanism when temperature moves outside the range 
         [alerts should occur when temperature is higher than high temperature limit (aTmpHigh) and lower than the low temperature limit (aTmpHigh – aTmpDiff)] 
         presented in the arguments of this method. Only one client is permitted to call this API because this controls the functional behavior of the thermal sensor. 
         If the boundary condition is already set and another client calls this method the call would not have any functional effect and the callback would not be registered.
         @since  ver 1
         @param  aTmpHigh The higher boundary of the temperature range in Kelvin. 
                 The callback specified would be triggered if the temperature of the sensor becomes greater than this level
         @param  aTmpDiff difference between THIGH and TLOW
         @param  aCb  this is the DFC that should be queued when the temperature of the sensor moves outside the range specified by the boundaries
         @return KErrNone if success
                 KErrAlreadyExists if the boundary values are already specified
                 otherwise system wide error code 
         @panic  none
         @pre    the preferable way to call this function is from a thread context with interrupts enabled after all the kernel extensions have been initialized in the boot up.
         */
        virtual TInt SetupAlert(TInt16 aTmpHigh, TInt16 aTmpDiff,TthermalSensorCb *aCb) = 0;

        /**
         Description of SetupCallBack
         This is the method to register callbacks for the current prevailing boundary conditions for a particular sensor. 
         This function is needed because only the controlling client is allowed to call the SetupAlert API, hence this API needs to be used by all other observer clients
         @since  ver 1
         @param  aCb This is the DFC that should be queued when the temperature of the sensor moves outside the range specified by the boundaries set before in the SetAlert method.
         @return KErrNone if success
                 otherwise system wide error code
         @panic  none
         @pre    The preferable way to call this function is from a thread context with interrupts enabled after all the kernel extensions have been initialized in the boot up.	
         */
        virtual TInt SetupCallBack(TthermalSensorCb *aCb) = 0;

        /**
         Description of StopAlert
         This is the method to cancel callbacks or the entire alert mechanism for a particular sensor.
         @since  ver 1
         @param  stopAll  If this true then the Alert mechanism is stopped altogether and the sensing HW is disabled until the next InitHW is called. 
                          It is recommended that only the client who has called SetupAlert successfully before calls this method. 
                          In other cases only the callback function is removed. If the callback function was the one used in the SetupAlert call before 
                          then the Alert mechanism is disabled also before the next SetupAlert is called.
         @param  aCb      This is the DFC that should be removed from the list of callbacks.
         @return KErrNone if success
                 otherwise system wide error code
         @panic  none
         @pre    This can only be called after SetupAlert API or SetupCallback API
                 The preferable way to call this function is from a thread context with interrupts enabled after all the kernel extensions have been initialized in the boot up.
         */
        virtual TInt StopAlert(TBool stopAll, TthermalSensorCb *aCb) = 0;


        /**
         Description of ReadTempData
         This is the method to read temperature data from a particular sensor
         @since  ver 2
         @param  tmpData stores the current temperature reading
         @param  aNewRead checks whether a new read is required for temperature
         @return KErrNone if success
                 otherwise system wide error code
         @panic  none
         @pre    The preferable way to call this function is from a thread context with interrupts enabled after all the kernel extensions have been initialized in the boot up.
         */
        virtual TInt ReadTempData(TInt16& tmpData, TBool aNewRead)   = 0;

        /**
         Description of PeformHWSelfTest
         This is the method to performing self test on the underlying thermal sensor HW. This method should be self contained meaning it should be able to save the context , 
         perform the self test and then restore the context back exactly to the state before the self test.
         @since  ver 2
         @param  none
         @return KErrNone if success
                 otherwise system wide error code
         @panic  none
         @pre    The preferable way to call this function is from a thread context with interrupts enabled after all the kernel extensions have been initialized in the boot up.
         */
        virtual TInt PerformHWSelfTest(void) = 0;
    };

#endif // __PM_THERMALSENSING_CHIPSET_API_H__

// End Of File
