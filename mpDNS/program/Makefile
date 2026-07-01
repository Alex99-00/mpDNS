#
# C Makefile für mpDNS.
#

# Zu benutzende Shell.
SHELL=sh


# Basis Verzeichnis der mpDNS Bibliothek.
mpDNSlibBASEPATH=./../mpDNSlib


# OS-Typ bestimmen. Linux oder Windows(cygwin).
OS=$(OSTYPE)
ifeq ($(OS),linux)
  OS=LINUX
else
  OS=WINDOWS
endif


# Programmname.
WINDOWS_PROGRAM=main.exe
LINUX_PROGRAM=main
WINDOWS_PROGRAM_TARGET=mpdns.exe
LINUX_PROGRAM_TARGET=mpDNS
PROGRAM=$($(OS)_PROGRAM)
PROGRAM_TARGET=$($(OS)_PROGRAM_TARGET)


# Programm Module.
OFILES=main.o inThread.o outThread.o processingThread.o processing.o

# C Compiler.
COMPILER=gcc

# Pfad zu den Header Dateien der mpDNS Bibliothek.
INCLUDEDIRS=-I. -I$(mpDNSlibBASEPATH)/include

# Comiler Optimierung.
OPT=

# Compiler Optionen.
CFLAGS=$(OPT) -c -pedantic -Wall -Werror -ansi

# Statische Bibliotheken, voller Pfad.
LIBSTATIC=

# Pfad zu statischen Bibliotheken.
LIBDIRS=-L.

# Zu bindende Bibliotheken, es können je nach OS und Bautyp(statisch,dynamisch)
# noch welche dazukommen.
LIBS=-lpthread -lmysqlclient 

# Je nach dem ob dynamisch oder statisch, die entsprechende mpDNS Bibliothek binden
ifeq ($(MAKECMDGOALS),dynamic)
LIBS+=-lmpDNS
LIBDIRS+=-L$(mpDNSlibBASEPATH)/lib
else
LIBSTATIC+=$(mpDNSlibBASEPATH)/lib/libmpDNS.a
endif

# Debug(nur statisch möglich) 
ifneq ($(MAKECMDGOALS),debug)
CFLAGS+=-DNDEBUG
else
CFLAGS+=-ggdb
# Zum finden von Speicherlöschern
LIBS+=-lmcheck
endif


# cygwin hat statische mysqlclient libs, libz muß manuell dazugebunden werden.
WINDOWS_LIBS=-lz
LINUX_LIBS=
LIBS+=$($(OS)_LIBS)



# Compiler.
CC=$(COMPILER) $(INCLUDEDIRS)

# Linker.
LINK=$(CC) $(LIBDIRS)


# Dummys für all Ziel. Nur zum abfragen für $(MAKECMDGOALS) Variable.
dynamic: all
debug: all


# Alles alte löschen(Programm ist nicht so groß...) und neu bauen.
# Danach die $(PROGRAM) Datei nach ./../install/$(PROGRAM_TARGET) kopieren.
all: clean $(PROGRAM) ; cp ./$(PROGRAM) ./../install/$(PROGRAM_TARGET)


# .c nach .o Regel.
.c.o:
	$(CC) $(CFLAGS) $<


# Programm bauen.
$(PROGRAM): $(OFILES)
	$(LINK) $(OFILES) $(LIBSTATIC) $(LIBS) -o $(PROGRAM)


# Alles löschen, was zum Programm gehört.
clean:
	rm -f *.o $(PROGRAM)

