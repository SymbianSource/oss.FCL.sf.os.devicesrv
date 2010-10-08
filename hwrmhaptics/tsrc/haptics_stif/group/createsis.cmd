@echo off
REM Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
REM All rights reserved.
REM This component and the accompanying materials are made available
REM under the terms of the License "Symbian Foundation License v1.0"
REM which accompanies this distribution, and is available
REM at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
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

echo.
call makesis hapticsclienttest.pkg hapticsclienttest.sis
call signsis hapticsclienttest.sis hapticsclienttest.sisx rd.cer rdkey.pem
