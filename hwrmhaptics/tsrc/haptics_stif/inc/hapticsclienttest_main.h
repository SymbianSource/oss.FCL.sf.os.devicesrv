/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  ?Description
*
*/



#ifndef HAPTICSCLIENTTESTMAIN_H
#define HAPTICSCLIENTTESTMAIN_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <hwrmhaptics.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KHapticsClientTestLogPath, "\\logs\\HapticsClientTest\\" ); 
// Log file
_LIT( KHapticsClientTestLogFile, "HapticsClientTest.txt" ); 

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class MT_HapticsTestAsyncPlaySender;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  HpaticsClient test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CHapticsClientTest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CHapticsClientTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CHapticsClientTest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
        
		/**
		 * Method, which is called by the status callback, when a status
		 * has been received. Stops the wait loop waiting for a new status.
		 */    
		void HapticsStatusReceived();

    private:

        /**
        * C++ default constructor.
        */
        CHapticsClientTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();
		
		/**
        * Build resources needed for test methods.
        * @since ?Series60_version
        */
		void BuildL ();
		
		/**
        * Test methods are listed below. 
        */
		/**
		* Test construction; Creates haptics client instance.
		*/
		TInt SetupL();

		/**
		 * Test teardown; Deletes haptics client instance.
		 */
		TInt Teardown();

		/**
		 * Helpers for creating effects
		 */
		void CreateMagSweepEffect( CHWRMHaptics::THWRMHapticsMagSweepEffect& aEffect );
		void CreatePeriodicEffect( CHWRMHaptics::THWRMHapticsPeriodicEffect& aEffect );
		void CreateIVTEffect( RBuf8& aBuffer, CHWRMHaptics::THWRMHapticsEffectTypes aType );

		/**
		 * Helper for such cases where it is first needed to load IVT data
		 */
		TInt LoadIVTDataInternal( TInt& aFileHandle, CHWRMHaptics::THWRMHapticsEffectTypes aType,
								  TBool aOpenActuator = ETrue );

		/**
		 * Helper for such cases where it is first needed to load IVT data
		 */
		TInt LoadIVTDataInternal( TInt& aFileHandle, 
								  const TUint8 aIvtData[],
								  TInt aIvtDataSize,
								  TBool aOpenActuator = ETrue );

		/**
		 * Helper for such cases where contents of stream sample
		 * is not relevant.
		 */
		void CreateDummyStreamSample( TInt aSize, RBuf8& aBuffer );
		
		TInt T_CreateAndDeleteL();
		TInt T_SeveralSessionsL();
		TInt T_ConsecutiveSessionsL();
		TInt T_SupportedActuators();
		TInt T_OpenSupportedActuator();
		TInt T_OpenNotSupportedActuator();
		TInt T_OpenAllActuators();
		TInt T_SeveralClientsWithOpenActuatorsL();
		TInt T_PlayMagSweepEffectSynchronous();
		TInt T_PlayMagSweepEffectAsynchronous();
		TInt T_PlayPeriodicEffectSynchronous();
		TInt T_PlayPeriodicEffectAsynchronous();
		TInt T_PlayIVTEffectWithDirectDataSynchronous();
		TInt T_PlayIVTEffectWithDirectDataAsynchronous();
		TInt T_PlayIVTEffectRepeatWithDirectDataSynchronous();
		TInt T_PlayIVTEffectRepeatWithDirectDataAsynchronous();
		TInt T_GetEffectState();
		TInt T_PlayNonTimelineIVTEffectRepeatWithDirectDataSynchronous();
		TInt T_PlayNonTimelineIVTEffectRepeatWithDirectDataAsynchronous();
		TInt T_PlayIVTEffectRepeatWithDirectDataWithZeroRepeatValueSynchronous();
		TInt T_PlayIVTEffectRepeatWithDirectDataWithZeroRepeatValueAsynchronous();  
		TInt T_LoadMultipleIVTData();
		TInt T_PlayIVTEffectsFromMultipleIVTDataSynchronous();
		TInt T_PlayIVTEffectsFromMultipleIVTDataAsynchronous();
		TInt T_PlayIVTEffectRepeatFromMultipleIVTDataSynchronous();
		TInt T_PlayIVTEffectRepeatFromMultipleIVTDataAsynchronous();
		TInt T_LoadIVTData();
		TInt T_DeleteIVTData();
		TInt T_DeleteAllIVTData();
		TInt T_LoadAndDeleteIVTData();
		TInt T_LoadAndDeleteIVTDataMixed();
		TInt T_LoadDeletePlaySync();
		TInt T_LoadDeletePlayAsync();
		TInt T_LoadDeletePlayRepeatSync();
		TInt T_LoadDeletePlayRepeatAsync();
		TInt T_PlayIVTEffectWithLoadedDataSynchronous();
		TInt T_PlayIVTEffectWithLoadedDataAsynchronous();
		TInt T_PlayIVTEffectRepeatWithLoadedDataSynchronous();
		TInt T_PlayIVTEffectRepeatWithLoadedDataAsynchronous();
		TInt T_ReserveAndReleaseHaptics();
		TInt T_ReserveHapticsSeveralTimes();
		TInt T_ReserveHapticsNoReleaseL();
		TInt T_ReleaseHapticsWithoutReservingFirst();
		TInt T_ReserveNoAutomaticFocusNotTrusted();
		TInt T_ReserveAlreadyReservedHigherL();
		TInt T_ReserveAlreadyReservedLowerL();
		TInt T_PlayWhenReservedL();
		TInt T_GetIVTEffectCount();
		TInt T_GetIVTEffectDuration();
		TInt T_GetIVTEffectIndexFromName();
		TInt T_GetIVTEffectType();
		TInt T_GetIVTEffectName();
		TInt T_GetIVTMagSweepEffectDefinition();
		TInt T_GetIVTPeriodicEffectDefinition();
		TInt T_CreateAndDeleteAsynchronousL();
		TInt T_SeveralSessionsAsynchronousL();
		TInt T_ConsecutiveSessionsAsynchronousL();
		TInt T_PauseEffect();
		TInt T_ResumeEffect();
		TInt T_StopEffect();
		TInt T_StopAllEffects();
		TInt T_PlayPauseResumeStop();
		TInt T_ConstantGetters();
		TInt T_ModifyMagSweepEffectSynchronous();
		TInt T_ModifyMagSweepEffectAsynchronous();
		TInt T_ModifyPeriodicEffectSynchronous();
		TInt T_ModifyPeriodicEffectAsynchronous();
		TInt T_VibeStatusToS60StatusConversions();
		TInt T_GetDeviceProperty_TInt_TInt();
		TInt T_GetDeviceProperty_TInt_TDesC();
		TInt T_SetDeviceProperty_TInt_TInt();
		TInt T_SetDeviceProperty_TInt_TDesC();    
		TInt T_GetDeviceCapability_TInt_TInt();
		TInt T_GetDeviceCapability_TInt_TDesC();
		TInt T_GetHapticsStatus();
		TInt T_ObserveHapticsStatus();
		TInt T_GetHapticsStatusWithReservations();
		TInt T_ObserveHapticsStatusWithReservations();
		TInt T_GetHapticsStatusWithSeveralReservations();
		TInt T_ObserveHapticsStatusWithSeveralReservations();
		TInt T_GetHapticsStatusWithSeveralClientsOneReservation();
		TInt T_ObserveHapticsStatusWithSeveralClientsOneReservations();
		TInt T_CreateStreamingEffect();
		TInt T_PlayStreamingSample();
		TInt T_PlayStreamingSampleAsync();
		TInt T_PlayStreamingSampleWithOffset();
		TInt T_PlayStreamingSampleWithOffsetAsync();
		TInt T_DestroyStreamingEffect();
		TInt T_ObserveActuatorStatus();
		TInt T_ObserveActuatorStatusAndHapticsStatus();
		TInt T_PluginMgrTransTimerExpires();
		
    private:    // Data
        /**
		 * Haptics client instance. Owned.
		 */
		CHWRMHaptics* iHaptics;

		/**
		 * Used to control tests, which are observing haptics status using
		 * status callback.
		 */
		CActiveSchedulerWait iWait;
		
		/**
		 * Async play-with-handle methods sender.
		 * Owned.
		 */ 
		MT_HapticsTestAsyncPlaySender* iSender;
			   
    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;   
    };

#endif      // HAPTICSCLIENTTEST_H

// End of File
