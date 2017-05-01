# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.2 from Makefile.am

# Copyright (C) 1994, 1995, 1996, 1997 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

#AUTOMAKE_OPTIONS = no-dependencies


SHELL = /bin/sh

srcdir = .
top_srcdir = ..
prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

pkgdatadir = $(datadir)/opt
pkglibdir = $(libdir)/opt
pkgincludedir = $(includedir)/opt

top_builddir = ..

ACLOCAL = /home/chengalu/skysym/build/opt/opt-3.7/missing aclocal
AUTOCONF = /home/chengalu/skysym/build/opt/opt-3.7/missing autoconf
AUTOMAKE = /home/chengalu/skysym/build/opt/opt-3.7/missing automake
AUTOHEADER = /home/chengalu/skysym/build/opt/opt-3.7/missing autoheader

INSTALL = /opt/imake/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = true
PRE_INSTALL = true
POST_INSTALL = true
NORMAL_UNINSTALL = true
PRE_UNINSTALL = true
POST_UNINSTALL = true
CC = cc
MAKEINFO = /home/chengalu/skysym/build/opt/opt-3.7/missing makeinfo
PACKAGE = opt
RANLIB = ranlib
VERSION = 3.7
testdir = test
xrlo = 
xtco = 

lib_LIBRARIES    = libopt.a

include_HEADERS  = opt.h

noinst_HEADERS   = opt_sf.h ag.h opt_p.h

libopt_a_SOURCES = opt_util.c opt_reg.c opt_regt.c opt_menu.c opt_proc.c ag.c

EXTRA_DIST= makeopth.pl

libopt_a_LIBADD  =  
libopt_a_DEPENDENCIES =  

readline_PATH = 
termcap_PATH =  
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_CLEAN_FILES = 
LIBRARIES =  $(lib_LIBRARIES)


DEFS =  -DPACKAGE=\"opt\" -DVERSION=\"3.7\" -DHAVE_STRDUP=1 -DHAVE_LONGJMP=1 -DHAVE_SIGNAL=1 -DRETSIGTYPE=void  -I. -I$(srcdir) 
CPPFLAGS = 
LDFLAGS = 
LIBS = 
libopt_a_OBJECTS =  opt_util.o opt_reg.o opt_regt.o opt_menu.o \
opt_proc.o ag.o
AR = ar
CFLAGS = -g -Wall -D_PLOT -DGAIN=1.0 -DTSYS=10.0
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS)
LINK = $(CC) $(CFLAGS) $(LDFLAGS) -o $@
HEADERS =  $(include_HEADERS) $(noinst_HEADERS)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP = --best
SOURCES = $(libopt_a_SOURCES)
OBJECTS = $(libopt_a_OBJECTS)

default: all

.SUFFIXES:
.SUFFIXES: .c .o
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps src/Makefile

Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-libLIBRARIES:

clean-libLIBRARIES:
	test -z "$(lib_LIBRARIES)" || rm -f $(lib_LIBRARIES) libopt.sl
	

distclean-libLIBRARIES:

maintainer-clean-libLIBRARIES:

install-libLIBRARIES: $(lib_LIBRARIES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(libdir)
	list='$(lib_LIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(INSTALL_DATA) $$p $(libdir)/$$p"; \
	    $(INSTALL_DATA) $$p $(libdir)/$$p; \
	  else :; fi; \
	done
	$(INSTALL_PROGRAM) -m 0755 libopt.sl $(libdir)
	@$(POST_INSTALL)
	@list='$(lib_LIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(RANLIB) $(libdir)/$$p"; \
	    $(RANLIB) $(libdir)/$$p; \
	  else :; fi; \
	done

uninstall-libLIBRARIES:
	$(NORMAL_UNINSTALL)
	list='$(lib_LIBRARIES)'; for p in $$list; do \
	  rm -f $(libdir)/$$p; \
	done

.c.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	rm -f *.o core

clean-compile:

distclean-compile:
	rm -f *.tab.c

maintainer-clean-compile:

libopt.a: $(libopt_a_OBJECTS) $(libopt_a_DEPENDENCIES)
	rm -f libopt.a
	$(AR) cru libopt.a $(libopt_a_OBJECTS) $(libopt_a_LIBADD)
	$(RANLIB) libopt.a
	ld -b -o libopt.sl $(libopt_a_OBJECTS) $(libopt_a_LIBADD)

install-includeHEADERS: $(include_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(includedir)
	@list='$(include_HEADERS)'; for p in $$list; do \
	  echo " $(INSTALL_DATA) $(srcdir)/$$p $(includedir)/$$p"; \
	  $(INSTALL_DATA) $(srcdir)/$$p $(includedir)/$$p; \
	done

uninstall-includeHEADERS:
	$(NORMAL_UNINSTALL)
	list='$(include_HEADERS)'; for p in $$list; do \
	  rm -f $(includedir)/$$p; \
	done

tags: TAGS

ID: $(HEADERS) $(SOURCES)
	here=`pwd` && cd $(srcdir) && mkid -f$$here/ID $(SOURCES) $(HEADERS)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES)
	tags=; \
	here=`pwd`; \
	test -z "$(ETAGS_ARGS)$(SOURCES)$(HEADERS)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $(SOURCES) $(HEADERS) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = src

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  test -f $(distdir)/$$file \
	  || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	  || cp -p $$d/$$file $(distdir)/$$file; \
	done
ag.o ag.lo: ag.c opt.h ag.h
opt_menu.o opt_menu.lo: opt_menu.c opt.h ag.h opt_p.h
opt_proc.o opt_proc.lo: opt_proc.c opt.h ag.h opt_p.h
opt_reg.o opt_reg.lo: opt_reg.c opt.h ag.h opt_p.h
opt_regt.o opt_regt.lo: opt_regt.c opt.h
opt_util.o opt_util.lo: opt_util.c ag.h opt.h opt_p.h

info:
dvi:
check: all
	$(MAKE)
installcheck:
install-exec: install-libLIBRARIES
	@$(NORMAL_INSTALL)

install-data: install-includeHEADERS
	@$(NORMAL_INSTALL)

install: install-exec install-data all
	@:

uninstall: uninstall-libLIBRARIES uninstall-includeHEADERS

all: Makefile $(LIBRARIES) $(HEADERS)

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' INSTALL_SCRIPT='$(INSTALL_PROGRAM)' install
installdirs:
	$(mkinstalldirs)  $(libdir) $(includedir)


mostlyclean-generic:
	test -z "$(MOSTLYCLEANFILES)" || rm -f $(MOSTLYCLEANFILES)

clean-generic:
	test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean-generic:
	rm -f Makefile $(DISTCLEANFILES)
	rm -f config.cache config.log stamp-h stamp-h[0-9]*
	test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)

maintainer-clean-generic:
	test -z "$(MAINTAINERCLEANFILES)" || rm -f $(MAINTAINERCLEANFILES)
	test -z "$(BUILT_SOURCES)" || rm -f $(BUILT_SOURCES)
mostlyclean:  mostlyclean-libLIBRARIES mostlyclean-compile \
		mostlyclean-tags mostlyclean-generic

clean:  clean-libLIBRARIES clean-compile clean-tags clean-generic \
		mostlyclean

distclean:  distclean-libLIBRARIES distclean-compile distclean-tags \
		distclean-generic clean
	rm -f config.status

maintainer-clean:  maintainer-clean-libLIBRARIES \
		maintainer-clean-compile maintainer-clean-tags \
		maintainer-clean-generic distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

.PHONY: default mostlyclean-libLIBRARIES distclean-libLIBRARIES \
clean-libLIBRARIES maintainer-clean-libLIBRARIES uninstall-libLIBRARIES \
install-libLIBRARIES mostlyclean-compile distclean-compile \
clean-compile maintainer-clean-compile uninstall-includeHEADERS \
install-includeHEADERS tags mostlyclean-tags distclean-tags clean-tags \
maintainer-clean-tags distdir info dvi installcheck install-exec \
install-data install uninstall all installdirs mostlyclean-generic \
distclean-generic clean-generic maintainer-clean-generic clean \
mostlyclean distclean maintainer-clean


xrlo: $(readline_PATH) 
	$(AR) x $(readline_PATH)

xtco: $(termcap_PATH)
	$(AR) x $(termcap_PATH)

####################

opt.c:  $(noinst_HEADERS) $(libopt_a_SOURCES)
	echo "#define PACKAGE \"opt\""  > opt.c
	echo "#define VERSION \"3.7\"" >> opt.c
	echo "#include" \"opt.h\"            >> opt.c
	cat $(noinst_HEADERS)                >> opt.c
	cat $(libopt_a_SOURCES)              >> opt.c

opt.o: opt.c
	$(CC) $(CFLAGS) -c opt.c

###

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
