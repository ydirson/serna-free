# Microsoft Developer Studio Project File - Name="jade" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=jade - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jade.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jade.mak" CFG="jade - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jade - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "jade - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jade - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\include" /I "..\style" /I "..\grove" /I "..\spgrove" /D "_CONSOLE" /D "NDEBUG" /D DSSSL_NAMESPACE=James_Clark_DSSSL /D "WIN32" /D SP_NAMESPACE=James_Clark_SP /D GROVE_NAMESPACE=James_Clark_GROVE /D "SP_MULTI_BYTE" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"..\bin\jade.exe"
# SUBTRACT LINK32 /profile /map

!ELSEIF  "$(CFG)" == "jade - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\include" /I "..\style" /I "..\grove" /I "..\spgrove" /D "_CONSOLE" /D "_DEBUG" /D DSSSL_NAMESPACE=James_Clark_DSSSL /D "WIN32" /D SP_NAMESPACE=James_Clark_SP /D GROVE_NAMESPACE=James_Clark_GROVE /D "SP_MULTI_BYTE" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\dbgbin\jade.exe"
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "jade - Win32 Release"
# Name "jade - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\dsssl\fot.dtd
# End Source File
# Begin Source File

SOURCE=.\HtmlFOTBuilder.cxx
# End Source File
# Begin Source File

SOURCE=.\HtmlFOTBuilder_inst.m4

!IF  "$(CFG)" == "jade - Win32 Release"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\HtmlFOTBuilder_inst.m4
InputName=HtmlFOTBuilder_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "jade - Win32 Debug"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\HtmlFOTBuilder_inst.m4
InputName=HtmlFOTBuilder_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HtmlMessages.msg

!IF  "$(CFG)" == "jade - Win32 Release"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\HtmlMessages.msg
InputName=HtmlMessages

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	perl -w ..\msggen.pl $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "jade - Win32 Debug"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\HtmlMessages.msg
InputName=HtmlMessages

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	perl -w ..\msggen.pl $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jade.cxx
# ADD CPP /D "JADE_MIF"
# End Source File
# Begin Source File

SOURCE=.\JadeMessages.msg

!IF  "$(CFG)" == "jade - Win32 Release"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\JadeMessages.msg
InputName=JadeMessages

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	perl -w ..\msggen.pl $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "jade - Win32 Debug"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\JadeMessages.msg
InputName=JadeMessages

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	perl -w ..\msggen.pl $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MifFOTBuilder.cxx
# ADD CPP /D "JADE_MIF"
# End Source File
# Begin Source File

SOURCE=.\MifFOTBuilder_inst.m4

!IF  "$(CFG)" == "jade - Win32 Release"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\MifFOTBuilder_inst.m4
InputName=MifFOTBuilder_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "jade - Win32 Debug"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\MifFOTBuilder_inst.m4
InputName=MifFOTBuilder_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MifMessages.msg

!IF  "$(CFG)" == "jade - Win32 Release"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\MifMessages.msg
InputName=MifMessages

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	perl -w ..\msggen.pl $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "jade - Win32 Debug"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\MifMessages.msg
InputName=MifMessages

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	perl -w ..\msggen.pl $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RtfFOTBuilder.cxx
# End Source File
# Begin Source File

SOURCE=.\RtfFOTBuilder_inst.m4

!IF  "$(CFG)" == "jade - Win32 Release"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\RtfFOTBuilder_inst.m4
InputName=RtfFOTBuilder_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "jade - Win32 Debug"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\RtfFOTBuilder_inst.m4
InputName=RtfFOTBuilder_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RtfMessages.msg

!IF  "$(CFG)" == "jade - Win32 Release"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\RtfMessages.msg
InputName=RtfMessages

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	perl -w ..\msggen.pl $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "jade - Win32 Debug"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\RtfMessages.msg
InputName=RtfMessages

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	perl -w ..\msggen.pl $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RtfOle.cxx
# End Source File
# Begin Source File

SOURCE=.\SgmlFOTBuilder.cxx
# End Source File
# Begin Source File

SOURCE=.\TeXFOTBuilder.cxx
# End Source File
# Begin Source File

SOURCE=.\TeXFOTBuilder_inst.m4

!IF  "$(CFG)" == "jade - Win32 Release"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\TeXFOTBuilder_inst.m4
InputName=TeXFOTBuilder_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "jade - Win32 Debug"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\TeXFOTBuilder_inst.m4
InputName=TeXFOTBuilder_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TeXMessages.msg

!IF  "$(CFG)" == "jade - Win32 Release"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\TeXMessages.msg
InputName=TeXMessages

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	perl -w ..\msggen.pl $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "jade - Win32 Debug"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\TeXMessages.msg
InputName=TeXMessages

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	perl -w ..\msggen.pl $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TransformFOTBuilder.cxx
# End Source File
# Begin Source File

SOURCE=.\TransformFOTBuilder_inst.m4

!IF  "$(CFG)" == "jade - Win32 Release"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\TransformFOTBuilder_inst.m4
InputName=TransformFOTBuilder_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "jade - Win32 Debug"

# Begin Custom Build - Processing $(InputPath)
InputDir=.
InputPath=.\TransformFOTBuilder_inst.m4
InputName=TransformFOTBuilder_inst

"$(InputDir)\$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f $(InputDir)\$(InputName).cxx 
	m4 ..\lib\instmac.m4 $(InputPath) >$(InputDir)\$(InputName).cxx 
	attrib +r $(InputDir)\$(InputName).cxx 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\HtmlMessages.h
# End Source File
# Begin Source File

SOURCE=.\JadeMessages.h
# End Source File
# Begin Source File

SOURCE=.\MifFOTBuilder.h
# End Source File
# Begin Source File

SOURCE=.\MifMessages.h
# End Source File
# Begin Source File

SOURCE=.\RtfFOTBuilder.h
# End Source File
# Begin Source File

SOURCE=.\RtfMessages.h
# End Source File
# Begin Source File

SOURCE=.\RtfOle.h
# End Source File
# Begin Source File

SOURCE=.\TeXFOTBuilder.h
# End Source File
# Begin Source File

SOURCE=.\TeXMessages.h
# End Source File
# Begin Source File

SOURCE=.\TmpOutputByteStream.h
# End Source File
# Begin Source File

SOURCE=.\TransformFOTBuilder.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\style\DssslAppMessages.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\HtmlMessages.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\style\InterpreterMessages.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\jade.rc
# ADD BASE RSC /l 0x809 /i "jade"
# ADD RSC /l 0x809 /i "." /i "..\style" /i "jade" /d "JADE_MIF"
# End Source File
# Begin Source File

SOURCE=.\JadeMessages.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\RtfMessages.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\TeXMessages.rc
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=.\HtmlFOTBuilder_inst.cxx
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\MifFOTBuilder_inst.cxx
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\RtfFOTBuilder_inst.cxx
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\TeXFOTBuilder_inst.cxx
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\TransformFOTBuilder_inst.cxx
# PROP Ignore_Default_Tool 1
# End Source File
# End Target
# End Project
