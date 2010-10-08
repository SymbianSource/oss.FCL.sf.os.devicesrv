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


copy %EPOCROOT%epoc32\release\armv5\urel\mt_hapticsclient.dll .

echo.
call makesis mt_hapticsclient.pkg mt_hapticsclient.sis
call signsis mt_hapticsclient.sis mt_hapticsclient.sisx rd.cer rdkey.pem

del MT_HapticsClient.dll /Q
