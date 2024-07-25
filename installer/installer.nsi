;--------------------------------
;Include Modern UI

	!include "MUI2.nsh"

;--------------------------------
;General

	Name "OpenVR-DisplayDevices"
	OutFile "OpenVR-DisplayDevices.exe"
	InstallDir "$PROGRAMFILES64\OpenVR-DisplayDevices"
	InstallDirRegKey HKLM "Software\OpenVR-DisplayDevices\Main" ""
	RequestExecutionLevel admin
	ShowInstDetails show
	
;--------------------------------
;Variables

VAR upgradeInstallation

;--------------------------------
;Interface Settings

	!define MUI_ABORTWARNING

;--------------------------------
;Pages

	!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
	!define MUI_PAGE_CUSTOMFUNCTION_PRE dirPre
	!insertmacro MUI_PAGE_DIRECTORY
	!insertmacro MUI_PAGE_INSTFILES
  
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
	!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Macros

;--------------------------------
;Functions

Function dirPre
	StrCmp $upgradeInstallation "true" 0 +2 
		Abort
FunctionEnd

Function .onInit
	StrCpy $upgradeInstallation "false"
 
	ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenVRDisplayDevices" "UninstallString"
	StrCmp $R0 "" done
	
	
	; If SteamVR is already running, display a warning message and exit
	FindWindow $0 "Qt5QWindowIcon" "SteamVR Status"
	StrCmp $0 0 +3
		MessageBox MB_OK|MB_ICONEXCLAMATION \
			"SteamVR is still running. Cannot install this software.$\nPlease close SteamVR and try again."
		Abort
 
	
	MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
		"OpenVR-DisplayDevices is already installed. $\n$\nClick `OK` to upgrade the \
		existing installation or `Cancel` to cancel this upgrade." \
		IDOK upgrade
	Abort
 
	upgrade:
		StrCpy $upgradeInstallation "true"
	done:
FunctionEnd

;--------------------------------
;Installer Sections

Section "Install" SecInstall
	
	StrCmp $upgradeInstallation "true" 0 noupgrade 
		DetailPrint "Uninstall previous version..."
		ExecWait '"$INSTDIR\Uninstall.exe" /S _?=$INSTDIR'
		Delete $INSTDIR\Uninstall.exe
		Goto afterupgrade
		
	noupgrade:

	afterupgrade:

	SetOutPath "$INSTDIR"

	File "..\LICENSE"
	File "..\x64\Release\OpenVR Display Devices.exe"
	File "..\3rdparty\openvr\bin\win64\openvr_api.dll"
	File "..\OpenVR Display Devices\manifest.vrmanifest"

	ExecWait '"$INSTDIR\vcredist_x64.exe" /install /quiet'
	
	Var /GLOBAL vrRuntimePath
	nsExec::ExecToStack '"$INSTDIR\OpenVR Display Devices.exe" -openvrpath'
	Pop $0
	Pop $vrRuntimePath
	DetailPrint "VR runtime path: $vrRuntimePath"
	
	WriteRegStr HKLM "Software\OpenVRDisplayDevices\Main" "" $INSTDIR
  
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenVRDisplayDevices" "DisplayName" "OpenVR-DisplayDevices"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenVRDisplayDevices" "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""

	CreateShortCut "$SMPROGRAMS\OpenVR-DisplayDevices.lnk" "$INSTDIR\OpenVR Display Devices.exe"
	
	SetOutPath "$INSTDIR"
	nsExec::ExecToLog '"$INSTDIR\OpenVR Display Devices.exe" -installmanifest'

SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"
	; If SteamVR is already running, display a warning message and exit
	FindWindow $0 "Qt5QWindowIcon" "SteamVR Status"
	StrCmp $0 0 +3
		MessageBox MB_OK|MB_ICONEXCLAMATION \
			"SteamVR is still running. Cannot uninstall this software.$\nPlease close SteamVR and try again."
		Abort
	
	SetOutPath "$INSTDIR"
	nsExec::ExecToLog '"$INSTDIR\OpenVR Display Devices.exe" -removemanifest'

	Delete "$INSTDIR\LICENSE"
	Delete "$INSTDIR\OpenVR Display Devices.exe"
	Delete "$INSTDIR\openvr_api.dll"
	Delete "$INSTDIR\manifest.vrmanifest"
	
	DeleteRegKey HKLM "Software\OpenVRDisplayDevices\Main"
	DeleteRegKey HKLM "Software\OpenVRDisplayDevices"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenVRDisplayDevices"

	Delete "$SMPROGRAMS\OpenVR-DisplayDevices.lnk"
SectionEnd
