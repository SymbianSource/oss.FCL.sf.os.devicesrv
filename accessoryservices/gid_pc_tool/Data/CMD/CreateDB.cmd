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


set ACCPOL_PATH=%1
set GIDFILE_PATH=%2

goto defined

:defined

IF EXIST "%ACCPOL_PATH%" ( goto copy_defined ) ELSE (goto error_end )

goto export_first

:copy_defined

cd\

goto export_first

:second_export
cd\

cd%ACCPOL_PATH%\group
echo export second
xcopy \epoc32\winscw\c\System\Data\DBS_10205030_accpolicydb.db ..\data\
call abld.bat cleanexport
call abld.bat export

goto end

:export_first

cd%ACCPOL_PATH%\group
echo export first
call bldmake bldfiles
call abld cleanexport
call abld export
goto importer


:importer
echo importer
cd\
cd\epoc32\release\winscw\udeb
call AccPolicyDBImp.exe
goto second_export

:error_end
@echo on
echo Please set the Nokia Accessory Policy to the root of the epoc drive. 
goto end

:end
cd\
cd omat koodit
echo Files successfully copied
