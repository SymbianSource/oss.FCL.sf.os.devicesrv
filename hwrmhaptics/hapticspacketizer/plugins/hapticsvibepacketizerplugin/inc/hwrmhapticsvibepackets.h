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
* Description:  VibeTonz API protocol packet definition header file
*
*/

#ifndef _HWRMHAPTICSVIBEPACKETS_H_
#define _HWRMHAPTICSVIBEPACKETS_H_

/** 
 * Byte alignment is set to 1 byte boundary with this pragma.
 */
#pragma pack( push, 1 )

/**
 * Const for largest allowable packet size.  It is used
 * by packetizer functions that piggyback IVT or streaming 
 * data in their packets.
 */
const TInt KVibePacketMaxSize = 0xFFFF;

/**
 * Haptics command codes (VibeTonz API command codes)
 */
const TUint16 KVibeCmdProtocolVersion             = 0xFF;
const TUint16 KVibeCmdApiVersion                  = 0x00;
const TUint16 KVibeCmdInitialize                  = 0x01;
const TUint16 KVibeCmdTerminate                   = 0x02;
const TUint16 KVibeCmdPlayBasisEffect             = 0x03;
const TUint16 KVibeCmdPlayIVTEffectIncludeData    = 0x04;
const TUint16 KVibeCmdPlayIVTEffectNoData         = 0x05;
const TUint16 KVibeCmdModifyBasisEffect           = 0x06;
const TUint16 KVibeCmdStopEffect                  = 0x07;
const TUint16 KVibeCmdStopAllEffects              = 0x08;
const TUint16 KVibeCmdGetDeviceCapabilities       = 0x09;
const TUint16 KVibeCmdDebugBuffer                 = 0x0A;
const TUint16 KVibeCmdResetDebugBuffer            = 0x0B;
const TUint16 KVibeCmdStopDesignedBridge          = 0x0C;
const TUint16 KVibeCmdGetDeviceState              = 0x0D;
const TUint16 KVibeCmdSetKernelParameter          = 0x0E;
const TUint16 KVibeCmdGetKernelParameter          = 0x0F;
const TUint16 KVibeCmdSetDeviceProperty           = 0x10;
const TUint16 KVibeCmdGetDeviceProperty           = 0x11;
const TUint16 KVibeCmdOpenDevice                  = 0x12;
const TUint16 KVibeCmdCloseDevice                 = 0x13;
const TUint16 KVibeCmdStartDesignerBridge         = 0x14;
const TUint16 KVibeCmdDeviceCount                 = 0x15;
const TUint16 KVibeCmdPlayStreamingSample         = 0x16;
const TUint16 KVibeCmdCreateStreamingEffect       = 0x17;
const TUint16 KVibeCmdDestroyStreamingEffect      = 0x18;
const TUint16 KVibeCmdPausePlayingEffect          = 0x19;
const TUint16 KVibeCmdResumePausedEffect          = 0x1A;
const TUint16 KVibeCmdGetEffectState              = 0x1B;
const TUint16 KVibeCmdPlayMagSweepEffect          = 0x1C;
const TUint16 KVibeCmdPlayPeriodicEffect          = 0x1D;
const TUint16 KVibeCmdModifyPlayingMagSweepEffect = 0x1E;
const TUint16 KVibeCmdModifyPlayingPeriodicEffect = 0x1F;
const TUint16 KVibeCmdSetLicense                  = 0x20;

/** 
 * Simple request struct contains just command code
 */
struct TVibePacketSimpleRequest
    {
    TUint16  iCmdCode;
    };

/**
 * Simple response struct contains just command code and status.
 */
struct TVibePacketSimpleResponse
    {
    TUint16 iCmdCode;
    TInt8   iVibeStatus;
    };

/**
 * Packet structures for Protocol Version request and response
 * (Implemented since Mobile 2.0 protocol)
 */
typedef TVibePacketSimpleRequest TVibePacketProtocolVersionRequest;

struct TVibePacketProtocolVersionResponse
    {
    TUint16 iCmdCode;
    TUint8  iVersionMinor;
    TUint8  iVersionMajor;
    };


/**
 * Packet structures for API Version request and response
 */
typedef TVibePacketSimpleRequest TVibePacketApiVersionRequest;

struct TVibePacketApiVersionResponse
    {
    TUint16 iCmdCode;
    TUint8  iVersionMajor;
    TUint8  iVersionMinor;
    TUint8  iVersionBuild_7_0;
    TUint8  iVersionBuild_15_8;
    };

/**
 * Packet structures for Initialize request and response
 */
struct TVibePacketInitializeRequest
    {
    TUint16 iCmdCode;
    TUint8  iVersionMajor;
    TUint8  iVersionMinor;
    TUint8  iVersionBuild_7_0;
    TUint8  iVersionBuild_15_8;
    };

typedef TVibePacketSimpleResponse TVibePacketInitializeResponse;

/**
 * Packet structures for Terminate request and response
 */
typedef TVibePacketSimpleRequest TVibePacketTerminateRequest;

typedef TVibePacketSimpleResponse TVibePacketTerminateResponse;

/** 
 * Packet structures for PlayBasisEffect request and response.
 * Used with following commands:
 *    - KVibeCmdPlayBasisEffect
 *    - KVibeCmdPlayMagSweepEffect 
 *    - KVibeCmdPlayPeriodicEffect
 */
struct TVibePacketPlayBasisEffectRequest
    {
    TUint16 iCmdCode;
    TUint8  iDeviceHandle_7_0;
    TUint8  iDeviceHandle_15_8;
    TUint8  iDeviceHandle_23_16;
    TUint8  iDeviceHandle_31_24;
    TUint8  iDuration_7_0;
    TUint8  iDuration_15_8;
    TUint8  iDuration_23_16;
    TUint8  iDuration_31_24;
    TUint8  iMagnitude_7_0;
    TUint8  iMagnitude_15_8;
    TUint8  iMagnitude_23_16;
    TUint8  iMagnitude_31_24;
    TUint8  iEffectTypeStyle;
    TUint8  iPeriod_7_0;
    TUint8  iPeriod_15_8;
    TUint8  iPeriod_23_16;
    TUint8  iPeriod_31_24;
    TUint8  iImpulseTime_7_0;
    TUint8  iImpulseTime_15_8;
    TUint8  iImpulseTime_23_16;
    TUint8  iImpulseTime_31_24;
    TUint8  iImpulseLevel_7_0;
    TUint8  iImpulseLevel_15_8;
    TUint8  iImpulseLevel_23_16;
    TUint8  iImpulseLevel_31_24;
    TUint8  iFadeTime_7_0;
    TUint8  iFadeTime_15_8;
    TUint8  iFadeTime_23_16;
    TUint8  iFadeTime_31_24;
    TUint8  iFadeLevel_7_0;
    TUint8  iFadeLevel_15_8;
    TUint8  iFadeLevel_23_16;
    TUint8  iFadeLevel_31_24;
    };
    
struct TVibePacketPlayBasisEffectResponse
    {
    TUint16 iCmdCode;
    TInt8   iVibeStatus;
    TUint8  iEffectHandle_7_0;
    TUint8  iEffectHandle_15_8;
    TUint8  iEffectHandle_23_16;
    TUint8  iEffectHandle_31_24;
    };

/**  
 * Packet structures for PlayIVTEffect requests and responses.
 * Used with following commands:
 *    - KVibeCmdPlayIVTEffectIncludeData
 *    - KVibeCmdPlayIVTEffectNoData
 */
struct TVibePacketPlayIVTEffectRequest
    {
    TUint16 iCmdCode;
    TUint16 iIvtDataSize;
    TUint8  iDeviceHandle_7_0;
    TUint8  iDeviceHandle_15_8;
    TUint8  iDeviceHandle_23_16;
    TUint8  iDeviceHandle_31_24;
    TUint8  iIvtIndex_7_0;
    TUint8  iIvtIndex_15_8;
    TUint8  iRepeat;
    TUint8  iIvtData[1];
    };

struct TVibePacketPlayIVTEffectResponse
    {
    TUint16 iCmdCode;
    TInt8   iVibeStatus;
    TUint8  iEffectHandle_7_0;
    TUint8  iEffectHandle_15_8;
    TUint8  iEffectHandle_23_16;
    TUint8  iEffectHandle_31_24;
    };

/** 
 * Packet structures for ModifyBasisEffect request and response.
 * Used with following commands:
 *    - KVibeCmdModifyBasisEffect
 *    - KVibeCmdModifyPlayingMagSweepEffect
 *    - KVibeCmdModifyPlayingPeriodicEffect
 */
struct TVibePacketModifyBasisEffectRequest
    {
    TUint16 iCmdCode;
    TUint8  iDeviceHandle_7_0;
    TUint8  iDeviceHandle_15_8;
    TUint8  iDeviceHandle_23_16;
    TUint8  iDeviceHandle_31_24;
    TUint8  iEffectHandle_7_0;
    TUint8  iEffectHandle_15_8;
    TUint8  iEffectHandle_23_16;
    TUint8  iEffectHandle_31_24;
    TUint8  iDuration_7_0;
    TUint8  iDuration_15_8;
    TUint8  iDuration_23_16;
    TUint8  iDuration_31_24;
    TUint8  iMagnitude_7_0;
    TUint8  iMagnitude_15_8;
    TUint8  iMagnitude_23_16;
    TUint8  iMagnitude_31_24;
    TUint8  iEffectTypeStyle;
    TUint8  iPeriod_7_0;
    TUint8  iPeriod_15_8;
    TUint8  iPeriod_23_16;
    TUint8  iPeriod_31_24;
    TUint8  iImpulseTime_7_0;
    TUint8  iImpulseTime_15_8;
    TUint8  iImpulseTime_23_16;
    TUint8  iImpulseTime_31_24;
    TUint8  iImpulseLevel_7_0;
    TUint8  iImpulseLevel_15_8;
    TUint8  iImpulseLevel_23_16;
    TUint8  iImpulseLevel_31_24;
    TUint8  iFadeTime_7_0;
    TUint8  iFadeTime_15_8;
    TUint8  iFadeTime_23_16;
    TUint8  iFadeTime_31_24;
    TUint8  iFadeLevel_7_0;
    TUint8  iFadeLevel_15_8;
    TUint8  iFadeLevel_23_16;
    TUint8  iFadeLevel_31_24;
    };

typedef TVibePacketSimpleResponse TVibePacketModifyBasisEffectResponse;

/** 
 * Packet structures for StopEffect request and response
 */
struct TVibePacketStopEffectRequest
    {
    TUint16 iCmdCode;
    TUint8  iEffectHandle_7_0;
    TUint8  iEffectHandle_15_8;
    TUint8  iEffectHandle_23_16;
    TUint8  iEffectHandle_31_24;
    TUint8  iDeviceHandle_7_0;
    TUint8  iDeviceHandle_15_8;
    TUint8  iDeviceHandle_23_16;
    TUint8  iDeviceHandle_31_24;
    };

typedef TVibePacketSimpleResponse TVibePacketStopEffectResponse;

/**
 * Packet structures for StopAllEffects request and response
 */
struct TVibePacketStopAllEffectsRequest
    {
    TUint16 iCmdCode;
    TUint8  iDeviceHandle_7_0;
    TUint8  iDeviceHandle_15_8;
    TUint8  iDeviceHandle_23_16;
    TUint8  iDeviceHandle_31_24;
    };

typedef TVibePacketSimpleResponse TVibePacketStopAllEffectsResponse;

/**
 * Packet structures for GetDeviceCapabilities request and response
 */
struct TVibePacketGetDeviceCapabilitiesRequest
    {
    TUint16 iCmdCode;
    TUint8  iCapabilityValueType;
    TUint8  iDeviceIndex_7_0;
    TUint8  iDeviceIndex_15_8;
    TUint8  iDeviceIndex_23_16;
    TUint8  iDeviceIndex_31_24;
    TUint8  iCapabilityType_7_0;
    TUint8  iCapabilityType_15_8;
    TUint8  iCapabilityType_23_16;
    TUint8  iCapabilityType_31_24;
    };

struct TVibePacketGetDeviceCapabilitiesResponse
    {
    TUint16 iCmdCode;
    TInt8   iVibeStatus;
    TUint8  iCapabilityType_7_0;
    TUint8  iCapabilityType_15_8;
    TUint8  iCapabilityType_23_16;
    TUint8  iCapabilityType_31_24;
    TUint8  iCapabilityValueType;
    TUint8  iSize;
    TUint8  iCapabilityValue[1];
    };

/**
 * Packet structures for Get DebugBuffer request and response
 */
typedef TVibePacketSimpleRequest TVibePacketGetDebugBufferRequest;

struct TVibePacketGetDebugBufferResponse
    {
    TUint16 iCmdCode;
    TUint8  iBufferSize_7_0;
    TUint8  iBufferSize_15_8;
    TUint8  iBuffer[1];
    };

/**
 * Packet structures for ResetDebugBuffer request and response
 */
typedef TVibePacketSimpleRequest TVibePacketResetDebugBufferRequest;

typedef TVibePacketSimpleResponse TVibePacketResetDebugBufferResponse;

/**
 * Packet structures for Stop designer Bridge request and response
 */
typedef TVibePacketSimpleRequest TVibePacketStopDesignerBridgeRequest;

typedef TVibePacketSimpleResponse TVibePacketStopDesignerBridgeResponse;

/**
 * Packet structures for GetDeviceState request and response
 */
struct TVibePacketGetDeviceStateRequest
    {
    TUint16 iCmdCode;
    TUint8  iDeviceIndex_7_0;
    TUint8  iDeviceIndex_15_8;
    TUint8  iDeviceIndex_23_16;
    TUint8  iDeviceIndex_31_24;
    };

struct TVibePacketGetDeviceStateResponse
    {
    TUint16 iCmdCode;
    TInt8   iVibeStatus;
    TUint8  iDeviceState_7_0;
    TUint8  iDeviceState_15_8;
    TUint8  iDeviceState_23_16;
    TUint8  iDeviceState_31_24;
    };

/**
 * Packet structures for Set Device Kernel Parameter request and response
 */
struct TVibePacketSetKernelParameterRequest
    {
    TUint16 iCmdCode;
    TUint8  iDeviceIndex_7_0;
    TUint8  iDeviceIndex_15_8;
    TUint8  iDeviceIndex_23_16;
    TUint8  iDeviceIndex_31_24;
    TUint8  iKernelParameterID_7_0;
    TUint8  iKernelParameterID_15_8;
    TUint8  iKernelParameterValue_7_0;
    TUint8  iKernelParameterValue_15_8;
    };

struct TVibePacketSetKernelParameterResponse
    {
    TUint16 iCmdCode;
    TInt8   iVibeStatus;
    TUint8  iKernelParameterID_7_0;
    TUint8  iKernelParameterID_15_8;
    };

/**
 * Get Device Kernel Parameter request and response
 */
 struct TVibePacketGetKernelParameterRequest
    {
    TUint16 iCmdCode;
    TUint8  iDeviceIndex_7_0;
    TUint8  iDeviceIndex_15_8;
    TUint8  iDeviceIndex_23_16;
    TUint8  iDeviceIndex_31_24;
    TUint8  iKernelParameterID_7_0;
    TUint8  iKernelParameterID_15_8;
    };

struct TVibePacketGetKernelParameterResponse
    {
    TUint16 iCmdCode;
    TInt8   iVibeStatus;
    TUint8  iKernelParameterID_7_0;
    TUint8  iKernelParameterID_15_8;
    TUint8  iKernelParameterValue_7_0;
    TUint8  iKernelParameterValue_15_8;
    };

/** 
 * Packet structures for SetDeviceProperty request and response
 */
struct TVibePacketSetDevicePropertyRequest
    {
    TUint16 iCmdCode;
    TUint8  iDeviceHandle_7_0;
    TUint8  iDeviceHandle_15_8;
    TUint8  iDeviceHandle_23_16;
    TUint8  iDeviceHandle_31_24;
    TUint8  iPropertyType_7_0;
    TUint8  iPropertyType_15_8;
    TUint8  iPropertyType_23_16;
    TUint8  iPropertyType_31_24;
    TUint8  iPropertyValueType;
    TUint8  iSize;
    TUint8  iPropertyValue[1];
    };

struct TVibePacketSetDevicePropertyResponse
    {
    TUint16 iCmdCode;
    TInt8   iVibeStatus;
    TUint8  iPropertyType_7_0;
    TUint8  iPropertyType_15_8;
    TUint8  iPropertyType_23_16;
    TUint8  iPropertyType_31_24;
    TUint8  iPropertyValueType;
    };

/**
 * Packet structures for GetDeviceProperty request and response
 */
struct TVibePacketGetDevicePropertyRequest
    {
    TUint16 iCmdCode;
    TUint8  iPropertyValueType;
    TUint8  iDeviceHandle_7_0;
    TUint8  iDeviceHandle_15_8;
    TUint8  iDeviceHandle_23_16;
    TUint8  iDeviceHandle_31_24;
    TUint8  iPropertyType_7_0;
    TUint8  iPropertyType_15_8;
    TUint8  iPropertyType_23_16;
    TUint8  iPropertyType_31_24;
    };

struct TVibePacketGetDevicePropertyResponse
    {
    TUint16 iCmdCode;
    TInt8   iVibeStatus;
    TUint8  iPropertyType_7_0;
    TUint8  iPropertyType_15_8;
    TUint8  iPropertyType_23_16;
    TUint8  iPropertyType_31_24;
    TUint8  iPropertyValueType;
    TUint8  iSize;
    TUint8  iPropertyValue[1];
    };

/**
 * Packet structures for OpenDevice 
 */
struct TVibePacketOpenDeviceRequest
    {
    TUint16 iCmdCode;
    TUint8  iDeviceIndex_7_0;
    TUint8  iDeviceIndex_15_8;
    TUint8  iDeviceIndex_23_16;
    TUint8  iDeviceIndex_31_24;
    };

struct TVibePacketOpenDeviceResponse
    {
    TUint16 iCmdCode;
    TInt8   iVibeStatus;
    TUint8  iDeviceHandle_7_0;
    TUint8  iDeviceHandle_15_8;
    TUint8  iDeviceHandle_23_16;
    TUint8  iDeviceHandle_31_24;
    };

/**
 * Packet structures for CloseDevice request and response
 */
struct TVibePacketCloseDeviceRequest
    {
    TUint16 iCmdCode;
    TUint8  iDeviceHandle_7_0;
    TUint8  iDeviceHandle_15_8;
    TUint8  iDeviceHandle_23_16;
    TUint8  iDeviceHandle_31_24;
    };

typedef TVibePacketSimpleResponse TVibePacketCloseDeviceResponse;

/**
 * Packet structures for Start designer Bridge request and response
 */
typedef TVibePacketSimpleRequest TVibePacketStartDesignedBridgeRequest;

typedef TVibePacketSimpleResponse TVibePacketStartDesignedBridgeResponse;

/**
 * Packet structures for DeviceCount request and response
 */
typedef TVibePacketSimpleRequest TVibePacketDeviceCountRequest;

// Uses simple response packet struct where vibe_status contains 
// the device count (or negative error value)
typedef TVibePacketSimpleResponse TVibePacketDeviceCountResponse;

/** 
 * Packet structures for Upload SPE data request and response
 */
struct TVibePacketPlayStreamingSampleRequest
    {
    TUint16 iCmdCode;
    TUint8  iDeviceHandle_7_0;
    TUint8  iDeviceHandle_15_8;
    TUint8  iDeviceHandle_23_16;
    TUint8  iDeviceHandle_31_24;
    TUint8  iEffectHandle_7_0;
    TUint8  iEffectHandle_15_8;
    TUint8  iEffectHandle_23_16;
    TUint8  iEffectHandle_31_24;
    TUint8  iSize;
    TUint8  iEffectOffsetTime_7_0;
    TUint8  iEffectOffsetTime_15_8;
    TUint8  iStreamingData[1];
    };

typedef TVibePacketSimpleResponse TVibePacketPlayStreamingSampleResponse;

/**
 * Packet structures for Create streaming effect request and response
 */
struct TVibePacketCreateStreamingEffectRequest
    {
    TUint16 iCmdCode;
    TUint8  iDeviceHandle_7_0;
    TUint8  iDeviceHandle_15_8;
    TUint8  iDeviceHandle_23_16;
    TUint8  iDeviceHandle_31_24;
    };

struct TVibePacketCreateStreamingEffectResponse
    {
    TUint16 iCmdCode;
    TInt8   iVibeStatus;
    TUint8  iEffectHandle_7_0;
    TUint8  iEffectHandle_15_8;
    TUint8  iEffectHandle_23_16;
    TUint8  iEffectHandle_31_24;
    };

/**
 * Packet structures for Destroy streaming effect request and response
 */
struct TVibePacketDestroyStreamingEffectRequest
    {
    TUint16 iCmdCode;
    TUint8  iEffectHandle_7_0;
    TUint8  iEffectHandle_15_8;
    TUint8  iEffectHandle_23_16;
    TUint8  iEffectHandle_31_24;
    TUint8  iDeviceHandle_7_0;
    TUint8  iDeviceHandle_15_8;
    TUint8  iDeviceHandle_23_16;
    TUint8  iDeviceHandle_31_24;
    };

typedef TVibePacketSimpleResponse TVibePacketDestroyStreamingEffectResponse;

/**
 * Packet structures for Pause Playing Effect request and response
 */
struct TVibePacketPausePlayingEffectRequest
    {
    TUint16 iCmdCode;
    TUint8  iEffectHandle_7_0;
    TUint8  iEffectHandle_15_8;
    TUint8  iEffectHandle_23_16;
    TUint8  iEffectHandle_31_24;
    TUint8  iDeviceHandle_7_0;
    TUint8  iDeviceHandle_15_8;
    TUint8  iDeviceHandle_23_16;
    TUint8  iDeviceHandle_31_24;
    };
    
typedef TVibePacketSimpleResponse TVibePacketPausePlayingEffectResponse;

/**
 * Packet structures for Resume Paused Effect request and response
 */
typedef TVibePacketPausePlayingEffectRequest TVibePacketResumePlayingEffectRequest;

typedef TVibePacketSimpleResponse TVibePacketResumePlayingEffectResponse;

/**
 * Packet structures for Get effect state request and response
 */
struct TVibePacketGetEffectStateRequest
    {
    TUint16 iCmdCode;
    TUint8  iEffectHandle_7_0;
    TUint8  iEffectHandle_15_8;
    TUint8  iEffectHandle_23_16;
    TUint8  iEffectHandle_31_24;
    TUint8  iDeviceHandle_7_0;
    TUint8  iDeviceHandle_15_8;
    TUint8  iDeviceHandle_23_16;
    TUint8  iDeviceHandle_31_24;
    };

struct TVibePacketGetEffectStateResponse
    {
    TUint16 iCmdCode;
    TInt8   iVibeStatus;
    TInt8   iEffectState;
    };

/**
 * Packet structures for license request and response
 * (Uses currently same packet structs as SetProperty)
 */
typedef TVibePacketSetDevicePropertyRequest TVibePacketSetLicenseRequest;

typedef TVibePacketSetDevicePropertyResponse TVibePacketSetLicenseResponse;

/**
 * Packet union definition, useful for decoder function
 */
union TVibePacket
    {
    TVibePacketSimpleRequest                  iSimpleReq;
    TVibePacketSimpleResponse                 iSimpleRsp;
    TVibePacketProtocolVersionRequest         iProtocolVersionReq;
    TVibePacketProtocolVersionResponse        iProtocolVersionRsp;
    TVibePacketApiVersionRequest              iAPIVersionReq;
    TVibePacketApiVersionResponse             iAPIVersionRsp;
    TVibePacketInitializeRequest              iInitializeReq;
    TVibePacketInitializeResponse             iInitializeRsp;
    TVibePacketTerminateRequest               iTerminateReq;
    TVibePacketTerminateResponse              iTerminateRsp;
    TVibePacketPlayBasisEffectRequest         iPlayBasisEffectReq;
    TVibePacketPlayBasisEffectResponse        iPlayBasisEffectRsp;
    TVibePacketPlayIVTEffectRequest           iPlayIVTEffectReq;
    TVibePacketPlayIVTEffectResponse          iPlayIVTEffectRsp;
    TVibePacketModifyBasisEffectRequest       iModifyBasisEffectReq;
    TVibePacketModifyBasisEffectResponse      iModifyBasisEffectRsp;
    TVibePacketStopEffectRequest              iStopEffectReq;
    TVibePacketStopEffectResponse             iStopEffectRsp;
    TVibePacketStopAllEffectsRequest          iStopAllEffectsReq;
    TVibePacketStopAllEffectsResponse         iStopAllEffectsRsp;
    TVibePacketGetDeviceCapabilitiesRequest   iGetDeviceCapsReq;
    TVibePacketGetDeviceCapabilitiesResponse  iGetDeviceCapsRsp;
    TVibePacketGetDebugBufferRequest          iDebugBufferReq;
    TVibePacketGetDebugBufferResponse         iDebugBufferRsp;
    TVibePacketResetDebugBufferRequest        iResetDebugBufferReq;
    TVibePacketResetDebugBufferResponse       iResetDebugBufferRsp;
    TVibePacketStopDesignerBridgeRequest      iStopDesignerBridgeReq;
    TVibePacketStopDesignerBridgeResponse     iStopDesignerBridgeRsp;
    TVibePacketGetDeviceStateRequest          iGetDeviceStateReq;
    TVibePacketGetDeviceStateResponse         iGetDeviceStateRsp;
    TVibePacketSetKernelParameterRequest      iSetKernelParamsReq;
    TVibePacketSetKernelParameterResponse     iSetKernelParamsRsp;
    TVibePacketGetKernelParameterRequest      iGetKernelParamsReq;
    TVibePacketGetKernelParameterResponse     iGetKernelParamsRsp;
    TVibePacketOpenDeviceRequest              iOpenDeviceReq;
    TVibePacketOpenDeviceResponse             iOpenDeviceRsp;
    TVibePacketCloseDeviceRequest             iCloseDeviceReq;
    TVibePacketCloseDeviceResponse            iCloseDeviceRsp;
    TVibePacketSetDevicePropertyRequest       iSetDevicePropertyReq;
    TVibePacketSetDevicePropertyResponse      iSetDevicePropertyRsp;
    TVibePacketGetDevicePropertyRequest       iGetDevicePropertyReq;
    TVibePacketGetDevicePropertyResponse      iGetDevicePropertyRsp;
    TVibePacketDeviceCountRequest             iGetDeviceCountReq;
    TVibePacketDeviceCountResponse            iGetDeviceCountRsp;
    TVibePacketStartDesignedBridgeRequest     iStartDesignerBridgeReq;
    TVibePacketStartDesignedBridgeResponse    iStartDesignerBridgeRsp;
    TVibePacketPlayStreamingSampleRequest     iPlayStreamingSampleReq;
    TVibePacketPlayStreamingSampleResponse    iPlayStreamingSampleRsp;
    TVibePacketCreateStreamingEffectRequest   iCreateStreamingEffectReq;
    TVibePacketCreateStreamingEffectResponse  iCreateStreamingEffectRsp;
    TVibePacketDestroyStreamingEffectRequest  iDestroyStreamingEffectReq;
    TVibePacketDestroyStreamingEffectResponse iDestroyStreamingEffectRsp;
    TVibePacketPausePlayingEffectRequest      iPausePlayingEffectReq;
    TVibePacketPausePlayingEffectResponse     iPausePlayingEffectRsp;
    TVibePacketResumePlayingEffectRequest     iResumePausedEffectReq;
    TVibePacketResumePlayingEffectResponse    iResumePausedEffectRsp;
    TVibePacketGetEffectStateRequest          iGetEffectStateReq;
    TVibePacketGetEffectStateResponse         iGetEffectStateRsp;
    TVibePacketSetLicenseRequest              iSetLicenseReq;
    TVibePacketSetLicenseResponse             iSetLicenseRsp;
    };

/** 
 * The 1-byte alignment pack pragma definitions pop'ed out of stack.
 */
#pragma pack( pop )

#endif /* _HWRMHAPTICSVIBEPACKETS_H_ */

