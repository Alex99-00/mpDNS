
LIBNAME=libmpDNS

SHELL=sh
TOPDIR=$(shell /bin/pwd)

SUBDIRS=src

LIBDIR=$(TOPDIR)/lib
OBJDIR=$(LIBDIR)/obj
OFILES=$(shell ls $(OBJDIR))
OFILES2=$(patsubst %.o,$(OBJDIR)/%.o,$(OFILES))


#include $(TOPDIR)/Rules.make



INCLUDEDIRS=-I. -I$(TOPDIR)/include
LIBINCLUDEDIRS=-L.

LIBBUILDER=ar
LIBFLAGS=-vrls
LIBLINK=$(LIBBUILDER) $(LIBFLAGS)


COMPILER=gcc
OPT=

CFLAGS=$(OPT) -c -Wall -Werror -pedantic -ansi

ifeq ($(MAKECMDGOALS),dynamic)
CFLAGS+=-fPIC -g
endif

ifneq ($(MAKECMDGOALS),debug)
CFLAGS+=-DNDEBUG
else
CFLAGS+=-ggdb
endif

CC=$(COMPILER) $(INCLUDEDIRS) $(LIBINCLUDEDIRS)




all: clean subdirs $(LIBNAME).a

debug: clean subdirs $(LIBNAME).a

dynamic: clean subdirs $(LIBNAME).so


.PHONY: subdirs $(SUBDIRS)
subdirs: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@



$(LIBNAME).a: $(OFILES2)
	$(LIBLINK) $(LIBNAME).a $(OFILES2) ; mv $(TOPDIR)/$(LIBNAME).a $(LIBDIR)/$(LIBNAME).a


$(LIBNAME).so: $(OFILES2)
	$(CC) -shared -Wl,-soname,$(LIBNAME).so -o $(LIBNAME).so $(OFILES2) -lc ; mv $(TOPDIR)/$(LIBNAME).so $(LIBDIR)/$(LIBNAME).so 
  


clean:
	rm -f $(OFILES2) $(LIBDIR)/$(LIBNAME).a $(LIBDIR)/$(LIBNAME).so


export TOPDIR SHELL CFLAGS CC LIBDIR OBJDIR
