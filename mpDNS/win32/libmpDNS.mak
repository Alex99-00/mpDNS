# Microsoft Developer Studio Generated NMAKE File, Based on libmpDNS.dsp
!IF "$(CFG)" == ""
CFG=libmpDNS - Win32 Debug
!MESSAGE No configuration specified. Defaulting to libmpDNS - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "libmpDNS - Win32 Release" && "$(CFG)" != "libmpDNS - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libmpDNS.mak" CFG="libmpDNS - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libmpDNS - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libmpDNS - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libmpDNS - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\libmpDNS.dll"


CLEAN :
	-@erase "$(INTDIR)\alloc.obj"
	-@erase "$(INTDIR)\configparser.obj"
	-@erase "$(INTDIR)\db.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\dns.obj"
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\mpdnsmysql.obj"
	-@erase "$(INTDIR)\queue.obj"
	-@erase "$(INTDIR)\udp.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libmpDNS.dll"
	-@erase "$(OUTDIR)\libmpDNS.exp"
	-@erase "$(OUTDIR)\libmpDNS.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libmpDNS.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib mysqlclient.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\libmpDNS.pdb" /machine:I386 /out:"$(OUTDIR)\libmpDNS.dll" /implib:"$(OUTDIR)\libmpDNS.lib" 
LINK32_OBJS= \
	"$(INTDIR)\configparser.obj" \
	"$(INTDIR)\db.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\dns.obj" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\alloc.obj" \
	"$(INTDIR)\udp.obj" \
	"$(INTDIR)\queue.obj" \
	"$(INTDIR)\mpdnsmysql.obj"

"$(OUTDIR)\libmpDNS.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libmpDNS - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\libmpDNS.dll"


CLEAN :
	-@erase "$(INTDIR)\alloc.obj"
	-@erase "$(INTDIR)\configparser.obj"
	-@erase "$(INTDIR)\db.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\dns.obj"
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\mpdnsmysql.obj"
	-@erase "$(INTDIR)\queue.obj"
	-@erase "$(INTDIR)\udp.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\libmpDNS.dll"
	-@erase "$(OUTDIR)\libmpDNS.exp"
	-@erase "$(OUTDIR)\libmpDNS.ilk"
	-@erase "$(OUTDIR)\libmpDNS.lib"
	-@erase "$(OUTDIR)\libmpDNS.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libmpDNS.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib mysqlclient.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\libmpDNS.pdb" /debug /machine:I386 /out:"$(OUTDIR)\libmpDNS.dll" /implib:"$(OUTDIR)\libmpDNS.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\configparser.obj" \
	"$(INTDIR)\db.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\dns.obj" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\alloc.obj" \
	"$(INTDIR)\udp.obj" \
	"$(INTDIR)\queue.obj" \
	"$(INTDIR)\mpdnsmysql.obj"

"$(OUTDIR)\libmpDNS.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("libmpDNS.dep")
!INCLUDE "libmpDNS.dep"
!ELSE 
!MESSAGE Warning: cannot find "libmpDNS.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "libmpDNS - Win32 Release" || "$(CFG)" == "libmpDNS - Win32 Debug"
SOURCE=..\mpDNSlib\src\memory\alloc.c

!IF  "$(CFG)" == "libmpDNS - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\alloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libmpDNS - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\alloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\mpDNSlib\src\config\configparser.c

!IF  "$(CFG)" == "libmpDNS - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\configparser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libmpDNS - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\configparser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\mpDNSlib\src\db\db.c

!IF  "$(CFG)" == "libmpDNS - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\db.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libmpDNS - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\db.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\mpDNSlib\src\debug\debug.c

!IF  "$(CFG)" == "libmpDNS - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\debug.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libmpDNS - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\debug.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\mpDNSlib\src\dns\dns.c

!IF  "$(CFG)" == "libmpDNS - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\dns.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libmpDNS - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\dns.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\mpDNSlib\src\error\error.c

!IF  "$(CFG)" == "libmpDNS - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\error.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libmpDNS - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\error.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\mpDNSlib\src\db\mpdnsmysql.c

"$(INTDIR)\mpdnsmysql.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\mpDNSlib\src\queue\queue.c

!IF  "$(CFG)" == "libmpDNS - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\queue.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libmpDNS - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\queue.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\mpDNSlib\src\net\udp.c

!IF  "$(CFG)" == "libmpDNS - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\udp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libmpDNS - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I ".\..\mpDNSlib\include" /I ".\mysql\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBMPDNS_EXPORTS" /Fp"$(INTDIR)\libmpDNS.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\udp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

