/*
 Nokia/Devices/R&D/CT/Adaptation SW/BB
 CONFIDENTIAL
 Deliverable software



 adc_chipset_api.h
 --------------------------------

 SW Include Document - Symbian OS



 Document identification and location in Synergy/CM

 Database:           co1core

 Object (this information is substituted by Synergy/CM):
 %name:           adc_chipset_api.h %
 %cvtype:         incl %
 %instance:       bh1dsgas_1 %
 %version:        1 %
 %date_modified:  Wed Oct 06 17:16:07 2010 %

 Copyright (c) Nokia. This material, including documentation and any related
 computer programs, is protected by copyright controlled by Nokia.
 All rights are reserved. Copying, including reproducing, storing, adapting or
 translating, any or all of this material requires the prior written consent
 of Nokia. This material also contains confidential information, which may not
 be disclosed to others without the prior written consent of Nokia.
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

#ifndef ADC_CHIPSET_API_H
#define ADC_CHIPSET_API_H

/* ---------------------------------------------------------------------------
 *
 * CONSTANTS
 *
 */

/* Battery Voltage (VBAT) */
#define HAL_ADC_CHIPSET_VBAT 0x01

/* Battery Size Identification (BSI) */
#define HAL_ADC_CHIPSET_BSI 0x04

/* Battery TEMPerature (BTEMP) */
#define HAL_ADC_CHIPSET_BTEMP 0x05

/* Power Amplifier TEMPerature (PATEMP) */
#define HAL_ADC_PATEMP 0x09

/* Battery current (positive: current from battery to phone, negative: current running to the battery) */
/* Ibat can be implemented as a physical or logical adc channel, in case of logical this channel is still used in calibration */
#define HAL_ADC_CHIPSET_IBAT 0x0E

/* PWB Temperature */
#define HAL_ADC_CHIPSET_PWBTEMP 0x22

/* Main channel  */
#define HAL_ADC_MAIN_CAL 0xFE

#include <kernel.h>
#include <platform.h>

/* ---------------------------------------------------------------------------
 *
 * TYPES
 *
 */

/*
 * Type definition for the call-back function used writing to
 *  asynchronous em callback.
 */
typedef void (*HAL_ADC_CALLBACK)(TUint event, TInt p1, TAny* p2);

/* ---------------------------------------------------------------------------
 *
 * FUNCTIONAL PROTOTYPES AND MACROS
 *
 */
class TAdcHal
    {

    // Class declaration

    /**
     Implements AdcHal chipset
     */

public:

    enum THalAdcCalType
        {
        EHalAdcCalAppliedVoltage = 1,
        EHalAdcCalAppliedCurrent,
        EHalAdcCalSensor
        };

    /* ---------------------------------------------------------------------------
     *
     * DATA STRUCTURES
     *
     */

    class HAL_ADC_CAL_DATA_ELEMENT_TYPE
        {
    public:
        TUint8 Channel;
        TUint16 RawReading;
        THalAdcCalType Type;
        TInt32 ReferenceValue;
        TInt32 Identification;
        };

    class HAL_ADC_CAL_DATA_TYPE
        {
    public:
        TUint16 NumberOfElements;
        HAL_ADC_CAL_DATA_ELEMENT_TYPE *Element;
        };

    /*
     * Constructor:
     */

    TAdcHal();

    /*
     * Extension entrypoint
     */
    IMPORT_C static TAdcHal* hal_adc_entry_point();

    /* Initialisation */
    virtual void hal_adc_init();

    /* Read/convert raw values */
    virtual TInt hal_adc_raw_read(TUint channel, TUint* value,
            HAL_ADC_CALLBACK adc_handler, TAny* context);
    virtual TInt hal_adc_unit_convert(TUint channel, TUint value);

    /* Calibration depending function */
    virtual TInt hal_adc_unit_read(TUint channel, TInt* value,
            HAL_ADC_CALLBACK adc_handler, TAny* context);
    virtual TInt hal_adc_cal_unit_conv(TUint channel, TUint rawval,
            TInt *unitval);
    virtual TInt hal_adc_cal_get(TUint8 channel, TUint *identification,
            TInt *value1, TInt *value2, TInt *value3);
    virtual TInt hal_adc_cal_set(HAL_ADC_CAL_DATA_TYPE *data);
    virtual TInt hal_adc_cal_store();
    virtual TInt hal_adc_unit_lock_channel(TUint channel, TBool action,
            TInt value);
    };

#endif /* ADC_CHIPSET_API_H */
/* End of Include File */
