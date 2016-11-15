@echo off

if exist ..\Results_ScreenShotTest.xml del ..\Results_ScreenShotTest.xml

set EXE_FILE=TestRS3_T.exe
if exist %EXE_FILE% (
			%EXE_FILE%
			echo %EXE_FILE% is excuted.
			) ELSE (
			echo %EXE_FILE% does not exist.
			goto fail
			)
			
set RETURN=%errorlevel%

echo %RETURN% Screenshotis broken.

rem 앞쪽에 10시 이전에는 공백+시간이 들어가서 파일이름으로 적합하지 않아서 0을 채워넣는다
set UNPADDEDHOURWITHSPACE=%TIME:~0,2%
set /a UNPADDEDHOUR=%UNPADDEDHOURWITHSPACE%
set EXTRAZERO=0%UNPADDEDHOUR%
set HOUR=%EXTRAZERO:~-2%

SET TARGET=..\history\%CCNETNUMERICLABEL%_%DATE%_%HOUR%_%TIME:~3,2%_%TIME:~6,2%

mkdir %TARGET%
copy ..\*.png %TARGET%
if exist ..\*.bmp copy ..\*.bmp %TARGET%
if exist TestRS3_TD.exe del TestRS3_TD.exe
if exist TestRS3_T.exe del TestRS3_T.exe

if not %RETURN%==0 (
		exit 1
		) ELSE (
		exit 0
		)
		
:fail		
exit 1
