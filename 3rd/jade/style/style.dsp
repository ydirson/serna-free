# Microsoft Developer Studio Project File - Name="style" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=style - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "style.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "style.mak" CFG="style - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "style - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "style - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "style - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\spgrove" /I "..\grove" /I "..\include" /D "_WINDOWS" /D "NDEBUG" /D DSSSL_NAMESPACE=James_Clark_DSSSL /D "WIN32" /D SP_NAMESPACE=James_Clark_SP /D GROVE_NAMESPACE=James_Clark_GROVE /D "SP_MULTI_BYTE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x20000000" /subsystem:windows /dll /machine:I386 /out:"..\bin\style.dll"
# SUBTRACT LINK32 /profile /map

!ELSEIF  "$(CFG)" == "style - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\spgrove" /I "..\grove" /I "..\include" /D "_WINDOWS" /D "_DEBUG" /D DSSSL_NAMESPACE=James_Clark_DSSSL /D "WIN32" /D SP_NAMESPACE=James_Clark_SP /D GROVE_NAMESPACE=James_Clark_GROVE /D "SP_MULTI_BYTE" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x20000000" /subsystem:windows /dll /debug /machine:I386 /out:"..\dbgbin\style.dll"
# SUBTRACT LINK32 /incremental:no /map

!ENDIF 

# Begin Target

# Name "style - Win32 Release"
# Name "style - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Collector.cxx
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\common_inst.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\common_inst.m4

!IF  "$(CFG)" == "style - Win32 Release"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\common_inst.m4
InputName=common_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "style - Win32 Debug"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\common_inst.m4
InputName=common_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DssslApp.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\DssslAppMessages.msg

!IF  "$(CFG)" == "style - Win32 Release"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\DssslAppMessages.msg
InputName=DssslAppMessages

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	perl -w ..\msggen.pl $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "style - Win32 Debug"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\DssslAppMessages.msg
InputName=DssslAppMessages

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	perl -w ..\msggen.pl $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DssslSpecEventHandler.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\ELObj.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\ELObjMessageArg.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\Expression.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\FlowObj.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\FlowObj_inst.m4

!IF  "$(CFG)" == "style - Win32 Release"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\FlowObj_inst.m4
InputName=FlowObj_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "style - Win32 Debug"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\FlowObj_inst.m4
InputName=FlowObj_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FOTBuilder.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\GroveManager.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\InheritedC.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\Insn.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\Interpreter.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\InterpreterMessages.cxx
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\InterpreterMessages.msg

!IF  "$(CFG)" == "style - Win32 Release"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\InterpreterMessages.msg
InputName=InterpreterMessages

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	perl -w ..\msggen.pl $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "style - Win32 Debug"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\InterpreterMessages.msg
InputName=InterpreterMessages

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	perl -w ..\msggen.pl $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MacroFlowObj.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\NumberCache.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\Pattern.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\primitive.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\primitive_inst.m4

!IF  "$(CFG)" == "style - Win32 Release"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\primitive_inst.m4
InputName=primitive_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "style - Win32 Debug"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\primitive_inst.m4
InputName=primitive_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ProcessContext.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\ProcessingMode.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\SchemeParser.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\Style.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\style_inst.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\style_inst.m4

!IF  "$(CFG)" == "style - Win32 Release"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\style_inst.m4
InputName=style_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "style - Win32 Debug"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\style_inst.m4
InputName=style_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StyleEngine.cxx
# ADD CPP /Yu"stylelib.h"
# End Source File
# Begin Source File

SOURCE=.\stylelib.cxx
# ADD CPP /Yc"stylelib.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\charNames.h
# End Source File
# Begin Source File

SOURCE=.\Collector.h
# End Source File
# Begin Source File

SOURCE=.\dsssl_ns.h
# End Source File
# Begin Source File

SOURCE=.\DssslApp.h
# End Source File
# Begin Source File

SOURCE=.\DssslAppMessages.h
# End Source File
# Begin Source File

SOURCE=.\DssslSpecEventHandler.h
# End Source File
# Begin Source File

SOURCE=.\ELObj.h
# End Source File
# Begin Source File

SOURCE=.\ELObjMessageArg.h
# End Source File
# Begin Source File

SOURCE=.\EvalContext.h
# End Source File
# Begin Source File

SOURCE=.\Expression.h
# End Source File
# Begin Source File

SOURCE=.\FOTBuilder.h
# End Source File
# Begin Source File

SOURCE=.\GroveManager.h
# End Source File
# Begin Source File

SOURCE=.\Insn.h
# End Source File
# Begin Source File

SOURCE=.\Insn2.h
# End Source File
# Begin Source File

SOURCE=.\Interpreter.h
# End Source File
# Begin Source File

SOURCE=.\InterpreterMessages.h
# End Source File
# Begin Source File

SOURCE=.\jade_version.h
# End Source File
# Begin Source File

SOURCE=.\MacroFlowObj.h
# End Source File
# Begin Source File

SOURCE=.\NumberCache.h
# End Source File
# Begin Source File

SOURCE=.\Pattern.h
# End Source File
# Begin Source File

SOURCE=.\primitive.h
# End Source File
# Begin Source File

SOURCE=.\ProcessContext.h
# End Source File
# Begin Source File

SOURCE=.\ProcessingMode.h
# End Source File
# Begin Source File

SOURCE=.\SchemeParser.h
# End Source File
# Begin Source File

SOURCE=.\sdata.h
# End Source File
# Begin Source File

SOURCE=.\SosofoObj.h
# End Source File
# Begin Source File

SOURCE=.\Style.h
# End Source File
# Begin Source File

SOURCE=.\style_pch.h
# End Source File
# Begin Source File

SOURCE=.\StyleEngine.h
# End Source File
# Begin Source File

SOURCE=.\stylelib.h
# End Source File
# Begin Source File

SOURCE=.\VM.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\FlowObj_inst.cxx
# PROP BASE Exclude_From_Build 1
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\primitive_inst.cxx
# PROP BASE Exclude_From_Build 1
# PROP Ignore_Default_Tool 1
# End Source File
# End Target
# End Project
