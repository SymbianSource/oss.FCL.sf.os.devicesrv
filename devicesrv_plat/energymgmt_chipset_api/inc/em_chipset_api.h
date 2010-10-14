/*
 Nokia/Devices/R&D/CT/Adaptation SW/BB
 CONFIDENTIAL
 Deliverable software



 em_chipset_api.h
 --------------------------------

 SW Include Document - Symbian OS



 Document identification and location in Synergy/CM

 Database:           co1core

 Object (this information is substituted by Synergy/CM):
 %name:           em_chipset_api.h %
 %cvtype:         incl %
 %instance:       bh1dsgas_1 %
 %version:        1 %
 %date_modified:  Wed Oct 06 17:16:17 2010 %

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

#ifndef EM_CHIPSET_API_H
#define EM_CHIPSET_API_H

#include <kernel.h>
#include <platform.h>

/* ---------------------------------------------------------------------------
 *
 * TYPES
 *
 */

/*
 * Type definition for the call-back function used writing to
 *  asynchronous em callback. Callback is executed in the EM chipset's context.
 */
typedef void (*HAL_EM_CALLBACK)(TUint event, TInt p1, TAny* p2);

/* ---------------------------------------------------------------------------
 *
 * FUNCTIONAL PROTOTYPES AND MACROS
 *
 */

class TEmHal
    {
    // Class declaration

    /**
     Implements EmHal chipset
     */
public:

    /* ---------------------------------------------------------------------------
     *
     * CONSTANTS
     *
     */

    enum THalEmChaEvtType
        {
        EHalEMChaChargeStarted = 1,
        EHalEmChaChargeStopped,
        EHalEamChaChargeReady,
        EHalEamChaChargeChargerType,
        EHalEamChaChargeFull,
        EHalEamChaChargeError
        };

    enum THalEmChaChargerType
        {
        EHalEmChaChargerNone = 1,
        EHalEmChaChargerSpecial,
        EHalEmChaChargerDynamo
        };

    enum THalEmChaSource
        {
        EHalEmChaSourceDynamo = 1, 
        EHalEmChaSourceUsb
        };

    enum THalEmIbatEvtType
        {
        EHalEmChaIbatStarted = 1,
        EHalEmChaIbatStopped,
        EHalEmChaIbatSampling,
        EHalEmChaIbatError
        };

    enum THalEmVbatEvtType
        {
        EHalEmVbatTxOn = 1,
        EHalEmVbatTxOff,
        EHalEmVbatChOn,
        EHalEmVbatChOff,
        EHalEmVbatError
        };

    enum THalEmBsiEvtType
        {
        EHalEmBsiFloating = 1
        };

    enum THalEmDbiEvtType
        {
        EHalEmDbiAuthenticationOk = 1,
        EHalEmDbiAuthenticationFailed,
        EHalEmDbiReadReady,
        EHalEmDbiReadError,
        EHalEmDbiWriteReady,
        EHalEmDbiWriteError
        };

    /*
     * Constructor:
     */

    TEmHal();

    /*
     * Extension entrypoint
     */

    IMPORT_C static TEmHal* hal_em_entry_point();

    /*
     * INIT (setup):
     */

    virtual void hal_em_init(HAL_EM_CALLBACK cha_handler,
            HAL_EM_CALLBACK ibat_handler, HAL_EM_CALLBACK vbat_handler,
            HAL_EM_CALLBACK bsi_handler
			
			);

    /*
     * CHARGING:
     */
    virtual TInt hal_em_cha_charge(TInt action);
    virtual TInt hal_em_cha_charge_pause(TInt action);
    virtual TInt hal_em_cha_set_target_voltage(TInt mV);
    virtual TInt hal_em_cha_set_termination_current(TInt mA);
    virtual TInt hal_em_cha_set_in_current(TInt mA);
    virtual TInt hal_em_cha_set_out_current(TInt mA);
    virtual TInt hal_em_cha_set_min_vbus_volt(TInt mv);
    virtual TInt hal_em_cha_set_charging_source(THalEmChaSource source);

    /*
     * IBAT
     */
    virtual TInt hal_em_ibat_read();
    virtual TUint hal_em_ibat_sample_time();
    virtual TUint hal_em_ibat_avg_time_get();
    virtual TInt hal_em_ibat_avg_time_set(TUint number);
    virtual TInt hal_em_ibat_avg_get();
    virtual TInt hal_em_ibat_avg_start();
    virtual TInt hal_em_ibat_avg_stop();
    virtual TInt hal_em_ibat_avg_state_get();

    /*
     * VBAT
     */

    virtual TInt hal_em_vbat_initial();
    virtual TInt hal_em_vbat_read(TUint *value);
    virtual TInt hal_em_vbat_sync(TInt action);

    /*
     * BTEMP
     */
    virtual TInt hal_em_btemp_read(TUint *value);

    /*
     * BSI
     */
    virtual TInt hal_em_bsi_read(TUint *value);

    /* WATCHDOG */
    virtual void hal_em_watchdog();

    /* DBI */

    virtual TInt hal_em_dbi_init(HAL_EM_CALLBACK dbi_handler);
    virtual TInt hal_em_dbi_authenticate(TUint8 *challenge,
            TUint8 challengelength, TUint8 *response, TUint8 responselength);
    virtual TInt
            hal_em_dbi_read(TUint address, TUint length, TUint8* readbuf);
    virtual TInt hal_em_dbi_write(TUint address, TUint length,
            TUint8* writebuf);
    };

#endif /* EM_CHIPSET_API_H */
/* End of Include File */
