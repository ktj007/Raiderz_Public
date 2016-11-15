;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

;--------------------------------
;Configuration

  ;General
  Name "MAIET entertainment - 3ds Max 8 Export Plugins ${__DATE__}"
  OutFile "..\..\Game\Runtime\Dev\MAIETExporters\MAIET3dsmax8Exporters.exe"
  CRCCheck on

  ;Folder selection page
  InstallDir "$PROGRAMFILES\Autodesk\3dsMax8"

  ;Get install folder from registry if available
  InstallDirRegKey HKLM "Software\Autodesk\3dsmax\8.0" "Installdir"
;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "License.txt"
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "Korean"
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections
Section "Installation" secInstallation

  SetOutPath "$INSTDIR\plugins"
  File "Release8\MCPlug3.dle"
;  File "RsSceneExporter\Release\rsexp3.dle"
;  SetOutPath "$INSTDIR\UI\MacroScripts"
;  File "RsSceneExporter\Render-MAIETBake.mcr"
  SetOutPath "$INSTDIR\Scripts\Startup"
  File "Scripts\MaietProperty.ms"
  SetOutPath "$INSTDIR\Scripts\Startup"
  File "Scripts\Gunz2Property.ms"
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\MAIETUninstall.exe"
SectionEnd

;--------------------------------
;Descriptions

  LangString DESC_secInstallation ${LANG_KOREAN} "¼³Ä¡"
  LangString DESC_secInstallation ${LANG_ENGLISH} "Installation"

  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${secInstallation} $(DESC_secInstallation)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall" SecUninstall

  Delete "$INSTDIR\plugins\MCPlug3.dle"
;  Delete "$INSTDIR\plugins\rsexp3.dle"
;  Delete "$INSTDIR\UI\MacroScripts\Render-MAIETBake.mcr"
  Delete "$INSTDIR\Scripts\Startup\MaietProperty.ms"
  Delete "$INSTDIR\Scripts\Startup\Gunz2Property.ms"

SectionEnd
