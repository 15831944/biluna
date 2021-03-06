; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "Biluna"
!define PRODUCT_VERSION "0.9.16"
!define PRODUCT_PUBLISHER "Biluna"
!define PRODUCT_WEB_SITE "http://www.biluna.com"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\Biluna.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "license.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\Biluna.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"
; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "Setup_Biluna_0_9_16.exe"
InstallDir "$PROGRAMFILES\Biluna"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "BilunaMainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "..\bil\release\Biluna.exe"
  CreateDirectory "$SMPROGRAMS\Biluna"
  CreateShortCut "$SMPROGRAMS\Biluna\Biluna.lnk" "$INSTDIR\Biluna.exe"
  CreateShortCut "$DESKTOP\Biluna.lnk" "$INSTDIR\Biluna.exe"
  File "..\bil\release\db.dll"
  File "..\bil\release\cad.dll"
SectionEnd

Section "LibraryAndPluginSection" SEC10
  ; Visual Studio VC 2013
  File "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\redist\x86\Microsoft.VC120.CRT\msvcp120.dll"
  File "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\redist\x86\Microsoft.VC120.CRT\msvcr120.dll"
  File "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\redist\1033\vcredist_x86.exe"
  ; Database drivers
  File "C:\Users\rutger\Documents\mysql-5.6.14-win32\lib\libmySQL.dll"
  ; OpenSSL
  File "C:\OpenSSL-Win32\libeay32.dll"
  ; File "C:\OpenSSL-Win32\libssl32.dll"
  ; old now libssl32.dll but required for webkit and webengine too?
  File "C:\OpenSSL-Win32\ssleay32.dll" 
  ; Qt library
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\assistant.exe"
  File "C:\Qt\Qt5.6.0\Tools\QtCreator\bin\d3dcompiler_47.dll"
  File "C:\Qt\Qt5.6.0\Tools\QtCreator\bin\icudt54.dll"
  File "C:\Qt\Qt5.6.0\Tools\QtCreator\bin\icuin54.dll"
  File "C:\Qt\Qt5.6.0\Tools\QtCreator\bin\icuuc54.dll"
  File "C:\Qt\Qt5.6.0\Tools\QtCreator\bin\libEGL.dll"
  File "C:\Qt\Qt5.6.0\Tools\QtCreator\bin\libGLESV2.dll"
  File "C:\Qt\Qt5.6.0\Tools\QtCreator\bin\opengl32sw.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\resources\icudtl.dat"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\resources\qtwebengine_resources.pak"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\resources\qtwebengine_resources_100p.pak"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\resources\qtwebengine_resources_200p.pak"
  
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5Charts.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5Core.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5Gui.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5Help.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5MultimediaWidgets.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5Multimedia.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5Network.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5OpenGL.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5Positioning.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5PrintSupport.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5Qml.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5Quick.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5Script.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5Sensors.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5Sql.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5Svg.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5WebChannel.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5WebEngine.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5WebEngineCore.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5WebEngineWidgets.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5WebView.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5Widgets.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5Xml.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\Qt5XmlPatterns.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\QtWebEngineProcess.exe"
  SetOutPath "$INSTDIR\bearer"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\bearer\qgenericbearer.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\bearer\qnativewifibearer.dll"
  SetOutPath "$INSTDIR\iconengines"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\iconengines\qsvgicon.dll"
  SetOutPath "$INSTDIR\imageformats"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\imageformats\qdds.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\imageformats\qgif.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\imageformats\qicns.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\imageformats\qico.dll"
  ; File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\imageformats\qjp2.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\imageformats\qjpeg.dll"
  ; File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\imageformats\qmng.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\imageformats\qsvg.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\imageformats\qtga.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\imageformats\qtiff.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\imageformats\qwbmp.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\imageformats\qwebp.dll"
  SetOutPath "$INSTDIR\platforms"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\platforms\qminimal.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\platforms\qwindows.dll"
  SetOutPath "$INSTDIR\position"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\position\qtposition_positionpoll.dll"
  SetOutPath "$INSTDIR\printsupport"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\printsupport\windowsprintersupport.dll"
  SetOutPath "$INSTDIR\qtwebengine"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\qtwebengine\ffmpegsumo.dll"
  SetOutPath "$INSTDIR\sqldrivers"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\sqldrivers\qsqlite.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\sqldrivers\qsqlmysql.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\sqldrivers\qsqlodbc.dll"
  File "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins\sqldrivers\qsqlpsql.dll"
  ; SetOutPath "$INSTDIR\translations"
  ; ...
SectionEnd

Section "BilunaPlugins" SEC50
  SetOutPath "$INSTDIR\plugins"
  File "..\bil\release\plugins\acc_actionplugin.dll"
  File "..\bil\release\plugins\crm_actionplugin.dll"
  File "..\bil\release\plugins\mrp_actionplugin.dll"
  File "..\bil\release\plugins\pcalc_actionplugin.dll"
  File "..\bil\release\plugins\peng_actionplugin.dll"
  File "..\bil\release\plugins\sail_actionplugin.dll"
  File "..\bil\release\plugins\scan_actionplugin.dll"
  File "..\bil\release\plugins\srm_actionplugin.dll"
SectionEnd

Section "BilunaDocumentation" SEC60
  SetOutPath "$INSTDIR\doc"
  File "..\scripts\help\doc\doc.qch"
  File "..\scripts\help\doc\doc.qhc"
SectionEnd

Section "BilunaData" SEC70
  SetOutPath "$INSTDIR\data"
  File "..\acc\data\acc_chartmaster_syssetting_nl.xml"
  ; ...
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\Biluna\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\Biluna\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\bil.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\bil.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\Biluna.exe"
  Delete "$INSTDIR\db.dll"
  Delete "$INSTDIR\cad.dll"
  ; Visual Studio VC 2013
  Delete "$INSTDIR\msvcp120.dll"
  Delete "$INSTDIR\msvcr120.dll"
  Delete "$INSTDIR\vcredist_x86.exe"
  ; Database drivers
  Delete "$INSTDIR\libmySQL.dll"
  ; OpenSSL
  Delete "$INSTDIR\libeay32.dll"
  ; Delete "$INSTDIR\libssl32.dll"
  ; old now libssl32.dll but required for webkit and webengine too?
  Delete "$INSTDIR\ssleay32.dll" 
  ; Qt library
  Delete "$INSTDIR\assistant.exe"
  Delete "$INSTDIR\d3dcompiler_47.dll"
  Delete "$INSTDIR\icudt54.dll"
  Delete "$INSTDIR\icuin54.dll"
  Delete "$INSTDIR\icuuc54.dll"
  Delete "$INSTDIR\libEGL.dll"
  Delete "$INSTDIR\libGLESV2.dll"
  Delete "$INSTDIR\opengl32sw.dll"
  Delete "$INSTDIR\icudtl.dat"
  Delete "$INSTDIR\qtwebengine_resources.pak"
  Delete "$INSTDIR\qtwebengine_resources_100p.pak"
  Delete "$INSTDIR\qtwebengine_resources_200p.pak"
  Delete "$INSTDIR\Qt5Charts.dll"
  Delete "$INSTDIR\Qt5Core.dll"
  Delete "$INSTDIR\Qt5Gui.dll"
  Delete "$INSTDIR\Qt5Help.dll"
  Delete "$INSTDIR\Qt5MultimediaWidgets.dll"
  Delete "$INSTDIR\Qt5Multimedia.dll"
  Delete "$INSTDIR\Qt5Network.dll"
  Delete "$INSTDIR\Qt5OpenGL.dll"
  Delete "$INSTDIR\Qt5Positioning.dll"
  Delete "$INSTDIR\Qt5PrintSupport.dll"
  Delete "$INSTDIR\Qt5Qml.dll"
  Delete "$INSTDIR\Qt5Quick.dll"
  Delete "$INSTDIR\Qt5Script.dll"
  Delete "$INSTDIR\Qt5Sensors.dll"
  Delete "$INSTDIR\Qt5Sql.dll"
  Delete "$INSTDIR\Qt5Svg.dll"
  Delete "$INSTDIR\Qt5WebChannel.dll"
  Delete "$INSTDIR\Qt5WebEngine.dll"
  Delete "$INSTDIR\Qt5WebEngineCore.dll"
  Delete "$INSTDIR\Qt5WebEngineWidgets.dll"
  Delete "$INSTDIR\Qt5WebView.dll"
  Delete "$INSTDIR\Qt5Widgets.dll"
  Delete "$INSTDIR\Qt5Xml.dll"
  Delete "$INSTDIR\Qt5XmlPatterns.dll"
  Delete "$INSTDIR\QtWebEngineProcess.exe"
  ; SetOutPath "$INSTDIR\bearer"
  Delete "$INSTDIR\bearer\qgenericbearer.dll"
  Delete "$INSTDIR\bearer\qnativewifibearer.dll"
  ; SetOutPath "$INSTDIR\iconengines"
  Delete "$INSTDIR\iconengines\qsvgicon.dll"
  ; SetOutPath "$INSTDIR\imageformats"
  Delete "$INSTDIR\imageformats\qdds.dll"
  Delete "$INSTDIR\imageformats\qgif.dll"
  Delete "$INSTDIR\imageformats\qicns.dll"
  Delete "$INSTDIR\imageformats\qico.dll"
  ; Delete "$INSTDIR\imageformats\qjp2.dll"
  Delete "$INSTDIR\imageformats\qjpeg.dll"
  ; Delete "$INSTDIR\imageformats\qmng.dll"
  Delete "$INSTDIR\imageformats\qsvg.dll"
  Delete "$INSTDIR\imageformats\qtga.dll"
  Delete "$INSTDIR\imageformats\qtiff.dll"
  Delete "$INSTDIR\imageformats\qwbmp.dll"
  Delete "$INSTDIR\imageformats\qwebp.dll"
  ; SetOutPath "$INSTDIR\platforms"
  Delete "$INSTDIR\platforms\qminimal.dll"
  Delete "$INSTDIR\platforms\qwindows.dll"
  ; SetOutPath "$INSTDIR\position"
  Delete "$INSTDIR\position\qtposition_positionpoll.dll"
  ; SetOutPath "$INSTDIR\printsupport"
  Delete "$INSTDIR\printsupport\windowsprintersupport.dll"
  ; SetOutPath "$INSTDIR\qtwebengine"
  Delete "$INSTDIR\qtwebengine\ffmpegsumo.dll"
  ; SetOutPath "$INSTDIR\sqldrivers"
  Delete "$INSTDIR\sqldrivers\qsqlite.dll"
  Delete "$INSTDIR\sqldrivers\qsqlmysql.dll"
  Delete "$INSTDIR\sqldrivers\qsqlodbc.dll"
  Delete "$INSTDIR\sqldrivers\qsqlpsql.dll"
  ; SetOutPath "$INSTDIR\translations"
  ; ...
  ; SetOutPath "$INSTDIR\plugins"
  Delete "$INSTDIR\plugins\acc_actionplugin.dll"
  Delete "$INSTDIR\plugins\crm_actionplugin.dll"
  Delete "$INSTDIR\plugins\mrp_actionplugin.dll"
  Delete "$INSTDIR\plugins\pcalc_actionplugin.dll"
  Delete "$INSTDIR\plugins\peng_actionplugin.dll"
  Delete "$INSTDIR\plugins\sail_actionplugin.dll"
  Delete "$INSTDIR\plugins\scan_actionplugin.dll"
  Delete "$INSTDIR\plugins\srm_actionplugin.dll"
  ; SetOutPath "$INSTDIR\doc"
  Delete "$INSTDIR\doc\doc.qch"
  Delete "$INSTDIR\doc\doc.qhc"
  ; SetOutPath "$INSTDIR\data"
  Delete "$INSTDIR\data\acc_chartmaster_syssetting_nl.xml"
  
  ; Installation files
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"

  Delete "$SMPROGRAMS\Biluna\Uninstall.lnk"
  Delete "$SMPROGRAMS\Biluna\Website.lnk"
  Delete "$DESKTOP\Biluna.lnk"
  Delete "$SMPROGRAMS\Biluna\Biluna.lnk"

  RMDir "$INSTDIR\bearer"
  RMDir "$INSTDIR\iconengines"
  RMDir "$INSTDIR\imageformats"
  RMDir "$INSTDIR\platforms"
  RMDir "$INSTDIR\plugins"
  RMDir "$INSTDIR\position"
  RMDir "$INSTDIR\printsupport"
  RMDir "$INSTDIR\qtwebengine"
  RMDir "$INSTDIR\sqldrivers"
  ; RMDir "$INSTDIR\translations"

  RMDir "$INSTDIR\doc"
  RMDir "$INSTDIR\data"
  RMDir "$SMPROGRAMS\Biluna"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd