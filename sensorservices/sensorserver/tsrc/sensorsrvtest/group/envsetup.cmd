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


@echo Setting up SensorTestModuleStubs
pushd ..\SensorTestModule\group\
call sbs REALLYCLEAN -c winscw.test -c armv5.generic.test
call sbs -b bld.inf -c winscw.test -c armv5.generic.test
popd
@echo Setting up SensorTestModuleStubs Finished
