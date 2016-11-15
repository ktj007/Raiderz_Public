@echo off

if defined ProgramFiles(x86) (set nsis_path="%ProgramFiles(x86)%\NSIS\makensis" ) else (set nsis_path="%ProgramFiles%\NSIS\makensis" )

@echo %nsis_path%

%nsis_path% %1
