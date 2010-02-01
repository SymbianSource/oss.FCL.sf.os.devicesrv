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


cd XML_FILES\COM

IF EXIST %EPOCROOT%s60\accessoryserver\Data\XML\nul ( goto copyOld ) ELSE ( goto copyNew )

:copyOld
xcopy *.XML %EPOCROOT%s60\accessoryserver\Data\XML\
cd..\..
cd Data\COM
xcopy *.dta %EPOCROOT%s60\accessoryserver\Data\XML\
goto end

:copyNew
xcopy *.XML %EPOCROOT%s60\OSExt\systemswextensions\accessoryservices\accessoryserver\Data\XML\
cd..\..
cd Data\COM
xcopy *.dta %EPOCROOT%s60\OSExt\systemswextensions\accessoryservices\accessoryserver\Data\XML\
goto end

:end