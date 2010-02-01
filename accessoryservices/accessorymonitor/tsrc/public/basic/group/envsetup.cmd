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


@echo Cleaning accfwuinotifier...
pushd \sf\mw\appsupport\coreapplicationuis\accfwuinotifier\group\
call bldmake bldfiles
call abld reallyclean
popd
@echo Cleaning accfwuinotifier... Finished

@echo Cleaning accessoryadaptation...
pushd \ncp_sw\corecom\modemadaptation_ext\adaptationlayer_ext\systemadaptation\group\
call bldmake bldfiles
call abld clean armv5 accessoryadaptation
popd
@echo Cleaning accessoryadaptation... Finished

@echo Setting up stubs...
pushd ..\stubs\group\
call bldmake bldfiles
call abld test reallyclean
call abld test build
popd
@echo Setting up stubs... Finished
