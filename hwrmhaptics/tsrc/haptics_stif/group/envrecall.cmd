@echo off
REM Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
REM All rights reserved.
REM This component and the accompanying materials are made available
REM under the terms of "Eclipse Public License v1.0"
REM which accompanies this distribution, and is available
REM at the URL "http://www.eclipse.org/legal/epl-v10.html".
REM
REM Initial Contributors:
REM Nokia Corporation - initial contribution.
REM
REM Contributors:
REM
REM Description:  ?Description
REM
REM
@echo on


@echo Cleaning Haptics Test stubs...
pushd ..\..\hapticstestplugins\group\
call sbs -c armv5.test reallyclean
call sbs -c winscw.test reallyclean
popd
@echo Cleaning Haptics Test stubs... Finished


@echoBuild Haptics SystemSW Adaptation Stub plugins...
pushd \sf\adaptation\stubs\systemswstubs\hwrmhapticsstubplugin\group
call sbs -c winscw
popd
@echo Build Haptics SystemSW Adaptation Stub plugins... Finished


@echo Build Haptics NCP Adaptation plugins...
pushd \ncp_sw\corecom\modemadaptation_ext\adaptationlayer_ext\hapticsadaptation\group
call sbs -c armv5
call sbs -c armv5.atlantis
call sbs -c armv5.atlantis102
call sbs -c armv5.atlantisry21x
call sbs -c armv5.gekko79
call sbs -c armv5.platsim79
call sbs -c armv5.tasmania
call sbs -c armv5.vasco
REM NOTE!! Please keep updating the above list as per the variant's available in the Product SDK
popd
@echo Build Haptics NCP Adaptation plugins... Finished
