;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

;--------------------------------
;Configuration

  ;General
  Name "MAIET entertainment - 3ds Max 2011 PhysX Plugin ${__DATE__}"
  OutFile "..\..\Game\Runtime\Dev\MAIETExporters\MAIETMax2011PluginGunz2.exe"
  CRCCheck on

  ;Folder selection page
  InstallDir "$PROGRAMFILES\Autodesk\3ds Max 2011"

  ;Get install folder from registry if available
  InstallDirRegKey HKLM "Software\Autodesk\3dsmax\13.0\MAX-1:409" "Installdir"

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
  Delete "$INSTDIR\plugins\pxplugin.dlm"
  File "Dll\pxplugin2011.dlm"
  SetOutPath "$INSTDIR\Scripts\physx"
  File "Scripts\physx\*.ms"
  SetOutPath "$INSTDIR\Scripts\startup"
  File "Scripts\startup\px_startup.ms"
  SetOutPath "$INSTDIR\Scripts\Startup"
  File "Scripts\Gunz2SoundGeometry.ms"
  SetOutPath "$INSTDIR\Scripts\physx"
  File "Scripts\physx\px_rigidbodyproperties.ms"
  
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

  Delete "$INSTDIR\pxplugin2011.dlm"
  Delete "$INSTDIR\Scripts\physx\*.ms"
  Delete "$INSTDIR\Scripts\physx\px_rigidbodyproperties.ms"
  Delete "$INSTDIR\Scripts\startup\px_startup.ms"
  Delete "$INSTDIR\Scripts\Startup\Gunz2SoundGeometry.ms"
   
SectionEnd
