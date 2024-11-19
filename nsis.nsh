#!Nsis Installer Command Script
#
# This is an NSIS Installer Command Script generated automatically
# by the Fedora nsiswrapper program.  For more information see:
#
#   http://fedoraproject.org/wiki/MinGW
#
# To build an installer from the script you would normally do:
#
#   makensis this_script
#
# which will generate the output file 'installer.exe' which is a Windows
# installer containing your program.
;Include Modern UI

  !include "MUI2.nsh"


Name "Amp Rack 5"
OutFile "/home/djshaji/projects/alana/releases/amprack-setup.exe"
InstallDir "$ProgramFiles\AmpRack"
InstallDirRegKey HKLM SOFTWARE\Amprack "Install_Dir"

ShowInstDetails hide
ShowUninstDetails hide

# Uncomment this to enable BZip2 compression, which results in
# slightly smaller files but uses more memory at install time.
#SetCompressor bzip2

XPStyle on

;~ Page components
;~ Page directory
;~ Page instfiles

!define MUI_WELCOMEPAGE_TITLE "Amp Rack Guitar Effects"
!define MUI_WELCOMEPAGE_TEXT "Welcome to Amp Rack Guitar Effects Setup. This will install Amp Rack Guitar Effects on your computer"
!define MUI_FINISHPAGE_TITLE "Amp Rack Guitar Effects"
!define MUI_FINISHPAGE_TEXT "Thank you for installing Amp Rack Guitar Effects. $\r$\n $\r$\n Amp Rack is also available for Linux and Android. Download from amprack.in $\r$\n $\r$\n Note: $\r$\n 1. You need Jack Audio Connection Kit for Windows to run Amp Rack $\r$\n 2. Run both Amp Rack and Jack as Administrators $\r$\n 3. Jack needs to be started before Amp Rack"
!define MUI_UNFINISHPAGE_TITLE "Amp Rack Guitar Effects"
!define MUI_UNFINISHPAGE_TEXT "Amp Rack Guitar Effects has been removed $\r$\n $\r$\n Amp Rack is also available for Linux and Android. Download from amprack.in"
!define MUI_UNWELCOMEPAGE_TITLE "Uninstall Amp Rack Guitar Effects"
!define MUI_UNWELCOMEPAGE_TEXT "This will remove Amp Rack Guitar Effects from your computer"
!define MUI_HEADERIMAGE "/usr/share/nsis/Contrib/Graphics/Header/orange.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "/usr/share/nsis/Contrib/Graphics/Wizard/orange.bmp"
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "LICENSE"

!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

;~ !insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
;~ !insertmacro MUI_UNPAGE_FINISH

ComponentText "Select which optional components you want to install."

DirText "Please select the installation folder."

Section "Amprack"
  SectionIn RO

  SetOutPath "$INSTDIR\."
  File /r "/home/djshaji/projects/alana/win/*"
SectionEnd

Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\AmpRack"
  CreateShortCut "$SMPROGRAMS\AmpRack\Uninstall Amprack.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\AmpRack\AmpRack.lnk" "$INSTDIR\Amprack.exe" "" "$INSTDIR\Amprack.exe" 0
SectionEnd

Section "Desktop Icons"
SectionEnd

Section "Uninstall"
  Delete /rebootok "$SMPROGRAMS\Amprack\Uninstall Amprack.lnk"
  Delete /rebootok "$SMPROGRAMS\Amprack\Amprack.lnk"
  RMDir "$SMPROGRAMS\AmpRack"

  RMDir /r "$INSTDIR\.\"

  RMDir "$INSTDIR"
SectionEnd

Section -post
  WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd
