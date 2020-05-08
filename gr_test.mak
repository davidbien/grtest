# Microsoft Developer Studio Generated NMAKE File, Based on gr_test.dsp
!IF "$(CFG)" == ""
CFG=gr_test - Win32 Debug
!MESSAGE No configuration specified. Defaulting to gr_test - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "gr_test - Win32 Release" && "$(CFG)" != "gr_test - Win32 Debug" && "$(CFG)" != "gr_test - Win32 Release debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gr_test.mak" CFG="gr_test - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gr_test - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "gr_test - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "gr_test - Win32 Release debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gr_test - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\gr_test.exe"


CLEAN :
	-@erase "$(INTDIR)\_gr_outp.obj"
	-@erase "$(INTDIR)\_gr_test.obj"
	-@erase "$(INTDIR)\dbgthrw.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\gr_test.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "__HAVE_ALLOC_TRAITS" /D "__GR_DEFINEOLEIO" /D "__DEBUG_THROW" /Fp"$(INTDIR)\gr_test.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gr_test.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\gr_test.pdb" /machine:I386 /out:"$(OUTDIR)\gr_test.exe" 
LINK32_OBJS= \
	"$(INTDIR)\_gr_outp.obj" \
	"$(INTDIR)\_gr_test.obj" \
	"$(INTDIR)\dbgthrw.obj"

"$(OUTDIR)\gr_test.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gr_test - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\gr_test.exe"


CLEAN :
	-@erase "$(INTDIR)\_gr_outp.obj"
	-@erase "$(INTDIR)\_gr_test.obj"
	-@erase "$(INTDIR)\dbgthrw.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\gr_test.exe"
	-@erase "$(OUTDIR)\gr_test.ilk"
	-@erase "$(OUTDIR)\gr_test.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "c:\dvnew\stl32" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "__HAVE_ALLOC_TRAITS" /D "__GR_DEFINEOLEIO" /D "__DEBUG_THROW" /Fp"$(INTDIR)\gr_test.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Qwd985 /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gr_test.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\gr_test.pdb" /debug /machine:I386 /out:"$(OUTDIR)\gr_test.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\_gr_outp.obj" \
	"$(INTDIR)\_gr_test.obj" \
	"$(INTDIR)\dbgthrw.obj"

"$(OUTDIR)\gr_test.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gr_test - Win32 Release debug"

OUTDIR=.\gr_test___Win32_Release_debug
INTDIR=.\gr_test___Win32_Release_debug
# Begin Custom Macros
OutDir=.\gr_test___Win32_Release_debug
# End Custom Macros

ALL : "$(OUTDIR)\gr_test.exe"


CLEAN :
	-@erase "$(INTDIR)\_gr_outp.obj"
	-@erase "$(INTDIR)\_gr_test.obj"
	-@erase "$(INTDIR)\dbgthrw.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\gr_test.exe"
	-@erase "$(OUTDIR)\gr_test.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /Zi /O2 /Ob2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "__HAVE_ALLOC_TRAITS" /D "__GR_DEFINEOLEIO" /D "__NDEBUG_THROW" /Fp"$(INTDIR)\gr_test.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gr_test.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\gr_test.pdb" /debug /machine:I386 /out:"$(OUTDIR)\gr_test.exe" 
LINK32_OBJS= \
	"$(INTDIR)\_gr_outp.obj" \
	"$(INTDIR)\_gr_test.obj" \
	"$(INTDIR)\dbgthrw.obj"

"$(OUTDIR)\gr_test.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("gr_test.dep")
!INCLUDE "gr_test.dep"
!ELSE 
!MESSAGE Warning: cannot find "gr_test.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "gr_test - Win32 Release" || "$(CFG)" == "gr_test - Win32 Debug" || "$(CFG)" == "gr_test - Win32 Release debug"
SOURCE=.\_gr_outp.cpp

"$(INTDIR)\_gr_outp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\_gr_test.cpp

"$(INTDIR)\_gr_test.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\_gr_todo.cpp
SOURCE=..\stdlib\dbgthrw.cpp

"$(INTDIR)\dbgthrw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

