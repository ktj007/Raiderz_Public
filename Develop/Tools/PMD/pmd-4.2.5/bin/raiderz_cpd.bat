del RaiderzResults.xml

@echo off
set TOPDIR=%~dp0/..
set VERSION=4.2.5
set PMDJAR=%TOPDIR%/lib/pmd-%VERSION%.jar
set JARPATH=%TOPDIR%/lib/asm-3.1.jar;%TOPDIR%/lib/jaxen-1.1.1.jar
set OPTS=-Xmx512m
set MAIN_CLASS=net.sourceforge.pmd.cpd.CPD

java %OPTS% -cp "%PMDJAR%;%JARPATH%;%CLASSPATH%" %MAIN_CLASS% --minimum-tokens 150 --files D:\Develop\CSCommon --files D:\Develop\Game --files D:\Develop\Server\AppServer --files D:\Develop\Server\GameServer --files D:\Develop\Server\LoginServer --files D:\Develop\Server\MasterServer --files D:\Develop\Server\ServerCommon --files D:\Develop\Server\ServerCommonUnitTest --files D:\Develop\Server\ServerResourceValidator --files D:\Develop\Server\Tools --language cpp --skip-duplicate-files --format xml > RaiderzResults.xml

