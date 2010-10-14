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


@echo Cleaning Haptics NCP Adaptation plugins...
pushd \ncp_sw\corecom\modemadaptation_ext\adaptationlayer_ext\hapticsadaptation\group
call sbs -c armv5 reallyclean
call sbs -c armv5.atlantis reallyclean
call sbs -c armv5.atlantis102 reallyclean
call sbs -c armv5.atlantisry21x reallyclean
call sbs -c armv5.gekko79 reallyclean
call sbs -c armv5.platsim79 reallyclean
call sbs -c armv5.tasmania reallyclean
call sbs -c armv5.vasco reallyclean
REM NOTE!! Please keep updating the above list as per the variant's available in the Product SDK
popd
@echo Cleaning Haptics NCP Adaptation plugins... Finished


@echo Cleaning Haptics SystemSW Adaptation Stub plugins...
pushd \sf\adaptation\stubs\systemswstubs\hwrmhapticsstubplugin\group
call sbs -c winscw reallyclean
popd
@echo Cleaning Haptics SystemSW Adaptation Stub plugins... Finished


@echo Setting up Haptics Test stubs...
pushd ..\..\hapticstestplugins\group\
call sbs reallyclean
call sbs
popd
@echo Setting up Haptics Test stubs... Finished
