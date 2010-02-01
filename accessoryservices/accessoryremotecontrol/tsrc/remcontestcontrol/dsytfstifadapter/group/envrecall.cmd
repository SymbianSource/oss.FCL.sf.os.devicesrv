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


@echo Cleaning stubs
pushd ..\..\..\stub\group\
call bldmake bldfiles
call abld test reallyclean
popd
@echo Cleaning stubs done

@echo build remconadaptation...
pushd \ncp_sw\corecom\modemadaptation_ext\adaptationlayer_ext\systemadaptation\accessoryadaptation\group\
call bldmake bldfiles
call abld build remconadaptation
popd
@echo build remconadaptation... Finished

@echo build phoneclient...
pushd \sf\mw\phonesrv\phoneclientserver\phoneclient\group
call bldmake bldfiles
call abld build
popd
@echo build phoneclient... Finished

@echo build phonecmdhandler
pushd \sf\mw\phonesrv\phonecmdhandler\group\
call bldmake bldfiles
call abld makefile 
call abld reallyclean
call abld build
popd
@echo build phonecmdhandler done
