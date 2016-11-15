;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

;--------------------------------
;Configuration

  ;General
  Name "MAIET entertainment - 3ds Max 2008 Export Plugins ${__DATE__}"
  OutFile "..\..\Game\Runtime\Dev\MAIETExporters\MAIET3dsmax2008Exporters.exe"
  CRCCheck on

  ;Folder selection page
  InstallDir "$PROGRAMFILES\Autodesk\3ds Max 2008"

  ;Get install folder from registry if available
  InstallDirRegKey HKLM "Software\Autodesk\3dsmax\10.0\MAX-1:409" "Installdir"

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
  File "Release2008\MCPlug3.dle"
  SetOutPath "$INSTDIR"
  File "Dll\d3dx9_42.dll"
  File "Dll\D3DCompiler_42.dll"
  SetOutPath "$INSTDIR\Scripts\Startup"
  File "Scripts\MaietProperty.ms"
  SetOutPath "$INSTDIR\Scripts\Startup"
  File "Scripts\Gunz2Property.ms"
  SetOutPath "$INSTDIR\Scripts\Startup"
  File "Scripts\EasyExporter.ms"
  SetOutPath "$INSTDIR\maps\fx"
  File "Scripts\StandardFX_Maiet.fx"
  
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
  Delete "$INSTDIR\d3dx9_42.dll"
  Delete "$INSTDIR\D3DCompiler_42.dll"
  Delete "$INSTDIR\Scripts\Startup\MaietProperty.ms"
  Delete "$INSTDIR\Scripts\Startup\Gunz2Property.ms"
  Delete "$INSTDIR\Scripts\Startup\EasyExporter.ms"
  Delete "$INSTDIR\maps\fx\StandardFX_Maiet.fx"
  
SectionEnd
