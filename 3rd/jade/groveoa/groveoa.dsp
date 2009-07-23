# Microsoft Developer Studio Project File - Name="groveoa" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=groveoa - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "groveoa.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "groveoa.mak" CFG="groveoa - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "groveoa - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "groveoa - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "groveoa - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir "."
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\include" /I "..\grove" /I "..\spgrove" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /D "_WINDLL" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D SP_NAMESPACE=James_Clark_SP /D GROVE_NAMESPACE=James_Clark_GROVE /D "SP_MULTI_BYTE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\bin\groveoa.dll"
# Begin Special Build Tool
TargetPath=\home\work\SP\bin\groveoa.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=regsvr32 /s /c "$(TargetPath)" 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "groveoa - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir "."
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\grove" /I "..\spgrove" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /D "_WINDLL" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D SP_NAMESPACE=James_Clark_SP /D GROVE_NAMESPACE=James_Clark_GROVE /D "SP_MULTI_BYTE" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\dbgbin\groveoa.dll"
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "groveoa - Win32 Release"
# Name "groveoa - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\CGroveBuilder.cxx
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\GroveNode.cxx
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\groveoa.cxx
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\groveoa.def
# End Source File
# Begin Source File

SOURCE=.\groveoa.idl
# ADD MTL /tlb "groveoa.tlb" /h "groveoa.h" /iid "groveoa_i.c" /Oicf
# SUBTRACT MTL /mktyplib203
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cxx
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\CGroveBuilder.h
# End Source File
# Begin Source File

SOURCE=.\GroveNode.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\groveoa.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\GroveBuilder.rgs
# End Source File
# End Target
# End Project
