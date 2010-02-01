// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __SSMMAXBOOTATTEMPTS_PATCH_H__
#define __SSMMAXBOOTATTEMPTS_PATCH_H__

/**
 * Patchable constant.
 * 
 * This value determines the maximum number of unsuccessful device reboots allowed before powering off the device.
 * The default value for KSsmMaxBootAttempts is 3, ie., after 3 unsuccessful attempts to boot the device will be powered off.
 * 
 * If this value is set to '0xFFFFFFFF' boot failure will be ignored and attempts will be made to reboot forever.
 * 
 * Bootup information can be found in '<System drive>/private/<SID of SSM>/bootupinfo/bootupcount.log'.
 * 
 * To patch these values, add a line to an iby or oby file that is included in the rom being built using the following format:
 * 
 * "patchdata <dll> @ <symbol> <newvalue>"
 * 
 * e.g. to enable maximum attempts for device reboot, use the following line:
 * "patchdata ssm.state.policy.0000.dll @ KSsmMaxBootAttempts 3"
 * 
 * @SYMPatchable
 * @publishedPartner
 * @released
 * 
 */
IMPORT_C extern const TInt KSsmMaxBootAttempts;

#endif	// __SSMMAXBOOTATTEMPTS_PATCH_H__
