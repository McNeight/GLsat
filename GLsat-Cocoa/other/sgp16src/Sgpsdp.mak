# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=sgpsdp - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to sgpsdp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "sgpsdp - Win32 Release" && "$(CFG)" != "sgpsdp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Sgpsdp.mak" CFG="sgpsdp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sgpsdp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sgpsdp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "sgpsdp - Win32 Release"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "sgpsdp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Sgpsdp.dll"

CLEAN : 
	-@erase ".\Release\Sgpsdp.dll"
	-@erase ".\Release\astro.obj"
	-@erase ".\Release\observer.obj"
	-@erase ".\Release\math.obj"
	-@erase ".\Release\sgpsdp.obj"
	-@erase ".\Release\utils.obj"
	-@erase ".\Release\time.obj"
	-@erase ".\Release\Sgpsdp.lib"
	-@erase ".\Release\Sgpsdp.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)/Sgpsdp.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Sgpsdp.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/Sgpsdp.pdb" /machine:I386 /out:"$(OUTDIR)/Sgpsdp.dll"\
 /implib:"$(OUTDIR)/Sgpsdp.lib" 
LINK32_OBJS= \
	"$(INTDIR)/astro.obj" \
	"$(INTDIR)/observer.obj" \
	"$(INTDIR)/math.obj" \
	"$(INTDIR)/sgpsdp.obj" \
	"$(INTDIR)/utils.obj" \
	"$(INTDIR)/time.obj"

"$(OUTDIR)\Sgpsdp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "sgpsdp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\Sgpsdp.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\Sgpsdp.dll"
	-@erase ".\Debug\math.obj"
	-@erase ".\Debug\time.obj"
	-@erase ".\Debug\observer.obj"
	-@erase ".\Debug\sgpsdp.obj"
	-@erase ".\Debug\astro.obj"
	-@erase ".\Debug\utils.obj"
	-@erase ".\Debug\Sgpsdp.ilk"
	-@erase ".\Debug\Sgpsdp.lib"
	-@erase ".\Debug\Sgpsdp.exp"
	-@erase ".\Debug\Sgpsdp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/Sgpsdp.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Sgpsdp.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/Sgpsdp.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Sgpsdp.dll"\
 /implib:"$(OUTDIR)/Sgpsdp.lib" 
LINK32_OBJS= \
	"$(INTDIR)/math.obj" \
	"$(INTDIR)/time.obj" \
	"$(INTDIR)/observer.obj" \
	"$(INTDIR)/sgpsdp.obj" \
	"$(INTDIR)/astro.obj" \
	"$(INTDIR)/utils.obj"

"$(OUTDIR)\Sgpsdp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "sgpsdp - Win32 Release"
# Name "sgpsdp - Win32 Debug"

!IF  "$(CFG)" == "sgpsdp - Win32 Release"

!ELSEIF  "$(CFG)" == "sgpsdp - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\sgpsdp.cpp
DEP_CPP_SGPSD=\
	".\sgpsdp.h"\
	

"$(INTDIR)\sgpsdp.obj" : $(SOURCE) $(DEP_CPP_SGPSD) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\utils.cpp
DEP_CPP_UTILS=\
	".\sgpsdp.h"\
	

"$(INTDIR)\utils.obj" : $(SOURCE) $(DEP_CPP_UTILS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\observer.cpp
DEP_CPP_OBSER=\
	".\sgpsdp.h"\
	

"$(INTDIR)\observer.obj" : $(SOURCE) $(DEP_CPP_OBSER) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\astro.cpp
DEP_CPP_ASTRO=\
	".\sgpsdp.h"\
	".\vector.h"\
	

"$(INTDIR)\astro.obj" : $(SOURCE) $(DEP_CPP_ASTRO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\time.cpp
DEP_CPP_TIME_=\
	".\sgpsdp.h"\
	

"$(INTDIR)\time.obj" : $(SOURCE) $(DEP_CPP_TIME_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\math.cpp
DEP_CPP_MATH_=\
	".\sgpsdp.h"\
	".\vector.h"\
	

"$(INTDIR)\math.obj" : $(SOURCE) $(DEP_CPP_MATH_) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
