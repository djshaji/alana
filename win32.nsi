# define installer name
OutFile "amprack-setup.exe"
 
# set desktop as install directory
InstallDir $PROGRAMFILES
 
# default section start
Section
 
# define output path
SetOutPath $INSTDIR
 
# specify file to go in output path
File amprack.exe
 
# define uninstaller name
WriteUninstaller $INSTDIR\uninstaller.exe
 
 
#-------
# default section end
SectionEnd
 
# create a section to define what the uninstaller does.
# the section will always be named "Uninstall"
Section "Uninstall"
 
# Delete installed file
Delete $INSTDIR\test.txt
 
# Delete the uninstaller
Delete $INSTDIR\uninstaller.exe
 
# Delete the directory
RMDir $INSTDIR
SectionEnd

