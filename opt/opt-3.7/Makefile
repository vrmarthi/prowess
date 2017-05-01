# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.2 from Makefile.am

# Copyright (C) 1994, 1995, 1996, 1997 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.


SHELL = /bin/sh

srcdir = .
top_srcdir = .
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

top_builddir = .

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
readline_PATH = 
termcap_PATH = 
testdir = test
xrlo = 
xtco = 

SUBDIRS = src ext

info_TEXINFOS = opt.texi

EXTRA_DIST = opt-RPM.spec.in makeRPM.in opt.lgpl

TAR = tar
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_CLEAN_FILES =  makeRPM opt-RPM.spec
TEXI2DVI = texi2dvi
TEXINFO_TEX = $(srcdir)/texinfo.tex
INFO_DEPS = opt.info
DVIS = opt.dvi
TEXINFOS = opt.texi
DIST_COMMON =  README AUTHORS COPYING ChangeLog INSTALL Makefile.am \
Makefile.in NEWS aclocal.m4 configure configure.in install-sh \
makeRPM.in mdate-sh missing mkinstalldirs opt-RPM.spec.in stamp-vti \
texinfo.tex version.texi


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

GZIP = --best
default: all

.SUFFIXES:
.SUFFIXES: .dvi .info .ps .texi .texinfo
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps Makefile

Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$@ CONFIG_HEADERS= $(SHELL) ./config.status

$(ACLOCAL_M4):  configure.in 
	cd $(srcdir) && $(ACLOCAL)

config.status: $(srcdir)/configure
	$(SHELL) ./config.status --recheck
$(srcdir)/configure: $(srcdir)/configure.in $(ACLOCAL_M4) $(CONFIGURE_DEPENDENCIES)
	cd $(srcdir) && $(AUTOCONF)
makeRPM: $(top_builddir)/config.status makeRPM.in
	cd $(top_builddir) && CONFIG_FILES=$@ CONFIG_HEADERS= ./config.status
opt-RPM.spec: $(top_builddir)/config.status opt-RPM.spec.in
	cd $(top_builddir) && CONFIG_FILES=$@ CONFIG_HEADERS= ./config.status

version.texi: stamp-vti
	cp $(srcdir)/stamp-vti $(srcdir)/version.texi

stamp-vti: opt.texi $(top_srcdir)/configure.in
	@echo "@set UPDATED `cd $(srcdir) \
	  && $(SHELL) ./mdate-sh opt.texi`" > vti.tmp
	@echo "@set EDITION $(VERSION)" >> vti.tmp
	@echo "@set VERSION $(VERSION)" >> vti.tmp
	@cmp -s vti.tmp $(srcdir)/stamp-vti \
	  || (echo "Updating $(srcdir)/stamp-vti"; \
	      cp vti.tmp $(srcdir)/stamp-vti)
	@rm -f vti.tmp

mostlyclean-vti:
	rm -f vti.tmp

clean-vti:

distclean-vti:

maintainer-clean-vti:
	rm -f stamp-vti version.texi

opt.info: opt.texi version.texi
opt.dvi: opt.texi version.texi


DVIPS = dvips

.texi.info:
	@cd $(srcdir) && rm -f $@ $@-[0-9] $@-[0-9][0-9]
	cd $(srcdir) \
	  && $(MAKEINFO) `echo $< | sed 's,.*/,,'`

.texi.dvi:
	TEXINPUTS=$(srcdir):$$TEXINPUTS \
	  MAKEINFO='$(MAKEINFO) -I $(srcdir)' $(TEXI2DVI) $<

.texi:
	@cd $(srcdir) && rm -f $@ $@-[0-9] $@-[0-9][0-9]
	cd $(srcdir) \
	  && $(MAKEINFO) `echo $< | sed 's,.*/,,'`

.texinfo.info:
	@cd $(srcdir) && rm -f $@ $@-[0-9] $@-[0-9][0-9]
	cd $(srcdir) \
	  && $(MAKEINFO) `echo $< | sed 's,.*/,,'`

.texinfo:
	@cd $(srcdir) && rm -f $@ $@-[0-9] $@-[0-9][0-9]
	cd $(srcdir) \
	  && $(MAKEINFO) `echo $< | sed 's,.*/,,'`

.texinfo.dvi:
	TEXINPUTS=$(srcdir):$$TEXINPUTS \
	  MAKEINFO='$(MAKEINFO) -I $(srcdir)' $(TEXI2DVI) $<
.dvi.ps:
	$(DVIPS) $< -o $@

install-info-am: $(INFO_DEPS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(infodir)
	@for file in $(INFO_DEPS); do \
	  d=$(srcdir); \
	  for ifile in `cd $$d && echo $$file $$file-[0-9] $$file-[0-9][0-9]`; do \
	    if test -f $$d/$$ifile; then \
	      echo " $(INSTALL_DATA) $$d/$$ifile $(infodir)/$$ifile"; \
	      $(INSTALL_DATA) $$d/$$ifile $(infodir)/$$ifile; \
	    else : ; fi; \
	  done; \
	done
	@$(POST_INSTALL)
	@if $(SHELL) -c 'install-info --version | sed 1q | fgrep -s -v -i debian' >/dev/null 2>&1; then \
	  for file in $(INFO_DEPS); do \
	    echo " install-info --info-dir=$(infodir) $(infodir)/$$file";\
	    install-info --info-dir=$(infodir) $(infodir)/$$file || :;\
	  done; \
	else : ; fi

uninstall-info:
	$(PRE_UNINSTALL)
	@if $(SHELL) -c 'install-info --version | sed 1q | fgrep -s -v -i debian' >/dev/null 2>&1; then \
	  ii=yes; \
	else ii=; fi; \
	for file in $(INFO_DEPS); do \
	  test -z $ii || install-info --info-dir=$(infodir) --remove $$file; \
	done
	$(NORMAL_UNINSTALL)
	for file in $(INFO_DEPS); do \
	  (cd $(infodir) && rm -f $$file $$file-[0-9] $$file-[0-9][0-9]); \
	done

dist-info: $(INFO_DEPS)
	for base in $(INFO_DEPS); do \
	  d=$(srcdir); \
	  for file in `cd $$d && eval echo $$base*`; do \
	    test -f $(distdir)/$$file \
	    || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	    || cp -p $$d/$$file $(distdir)/$$file; \
	  done; \
	done

mostlyclean-aminfo:
	rm -f opt.aux opt.cp opt.cps opt.dvi opt.fn opt.fns opt.ky opt.log \
	  opt.pg opt.toc opt.tp opt.tps opt.vr opt.vrs opt.op opt.tr \
	  opt.cv

clean-aminfo:

distclean-aminfo:

maintainer-clean-aminfo:
	for i in $(INFO_DEPS); do rm -f `eval echo $$i*`; done

# This directory's subdirectories are mostly independent; you can cd
# into them and run `make' without going through this Makefile.
# To change the values of `make' variables: instead of editing Makefiles,
# (1) if the variable is set in `config.status', edit `config.status'
#     (which will cause the Makefiles to be regenerated when you run `make');
# (2) otherwise, pass the desired values on the `make' command line.



all-recursive install-data-recursive install-exec-recursive \
installdirs-recursive install-recursive uninstall-recursive  \
check-recursive installcheck-recursive info-recursive dvi-recursive:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	for subdir in $(SUBDIRS); do \
	  target=`echo $@ | sed s/-recursive//`; \
	  echo "Making $$target in $$subdir"; \
	  (cd $$subdir && $(MAKE) $$target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"

mostlyclean-recursive clean-recursive distclean-recursive \
maintainer-clean-recursive:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	rev=''; for subdir in $(SUBDIRS); do rev="$$rev $$subdir"; done; \
	for subdir in $$rev; do \
	  target=`echo $@ | sed s/-recursive//`; \
	  echo "Making $$target in $$subdir"; \
	  (cd $$subdir && $(MAKE) $$target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"
tags-recursive:
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  (cd $$subdir && $(MAKE) tags); \
	done

tags: TAGS

ID: $(HEADERS) $(SOURCES)
	here=`pwd` && cd $(srcdir) && mkid -f$$here/ID $(SOURCES) $(HEADERS)

TAGS: tags-recursive $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES)
	tags=; \
	here=`pwd`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  test -f $$subdir/TAGS && tags="$$tags -i $$here/$$subdir/TAGS"; \
	done; \
	test -z "$(ETAGS_ARGS)$(SOURCES)$(HEADERS)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $(SOURCES) $(HEADERS) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	rm -f TAGS ID

maintainer-clean-tags:

distdir = $(PACKAGE)-$(VERSION)
top_distdir = $(distdir)

# This target untars the dist file and tries a VPATH configuration.  Then
# it guarantees that the distribution is self-contained by making another
# tarfile.
distcheck: dist
	rm -rf $(distdir)
	GZIP=$(GZIP) $(TAR) zxf $(distdir).tar.gz
	mkdir $(distdir)/=build
	mkdir $(distdir)/=inst
	dc_install_base=`cd $(distdir)/=inst && pwd`; \
	cd $(distdir)/=build \
	  && ../configure --srcdir=.. --prefix=$$dc_install_base \
	  && $(MAKE) \
	  && $(MAKE) dvi \
	  && $(MAKE) check \
	  && $(MAKE) install \
	  && $(MAKE) installcheck \
	  && $(MAKE) dist
	rm -rf $(distdir)
	@echo "========================"; \
	echo "$(distdir).tar.gz is ready for distribution"; \
	echo "========================"
dist: distdir
	-chmod -R a+r $(distdir)
	GZIP=$(GZIP) $(TAR) chozf $(distdir).tar.gz $(distdir)
	rm -rf $(distdir)
dist-all: distdir
	-chmod -R a+r $(distdir)
	GZIP=$(GZIP) $(TAR) chozf $(distdir).tar.gz $(distdir)
	rm -rf $(distdir)
distdir: $(DISTFILES)
	rm -rf $(distdir)
	mkdir $(distdir)
	-chmod 777 $(distdir)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  test -f $(distdir)/$$file \
	  || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	  || cp -p $$d/$$file $(distdir)/$$file; \
	done
	for subdir in $(SUBDIRS); do		\
	  test -d $(distdir)/$$subdir		\
	  || mkdir $(distdir)/$$subdir		\
	  || exit 1;				\
	  chmod 777 $(distdir)/$$subdir;	\
	  (cd $$subdir && $(MAKE) top_distdir=../$(top_distdir) distdir=../$(distdir)/$$subdir distdir) \
	    || exit 1; \
	done
	$(MAKE) top_distdir="$(top_distdir)" distdir="$(distdir)" dist-info
info: $(INFO_DEPS) info-recursive
dvi: $(DVIS) dvi-recursive
check: all-am
	$(MAKE) check-recursive
installcheck: installcheck-recursive
all-am: Makefile $(INFO_DEPS)

install-data-am: install-info-am

uninstall-am: uninstall-info

install-exec: install-exec-recursive
	@$(NORMAL_INSTALL)

install-data: install-data-recursive install-data-am
	@$(NORMAL_INSTALL)

install: install-recursive install-data-am
	@:

uninstall: uninstall-recursive uninstall-am

all: all-recursive all-am

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' INSTALL_SCRIPT='$(INSTALL_PROGRAM)' install
installdirs: installdirs-recursive
	$(mkinstalldirs)  $(infodir)


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
mostlyclean-am:  mostlyclean-vti mostlyclean-aminfo mostlyclean-tags \
		mostlyclean-generic

clean-am:  clean-vti clean-aminfo clean-tags clean-generic \
		mostlyclean-am

distclean-am:  distclean-vti distclean-aminfo distclean-tags \
		distclean-generic clean-am

maintainer-clean-am:  maintainer-clean-vti maintainer-clean-aminfo \
		maintainer-clean-tags maintainer-clean-generic \
		distclean-am

mostlyclean:  mostlyclean-recursive mostlyclean-am

clean:  clean-recursive clean-am

distclean:  distclean-recursive distclean-am
	rm -f config.status

maintainer-clean:  maintainer-clean-recursive maintainer-clean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."
	rm -f config.status

.PHONY: default mostlyclean-vti distclean-vti clean-vti \
maintainer-clean-vti install-info-am uninstall-info mostlyclean-aminfo \
distclean-aminfo clean-aminfo maintainer-clean-aminfo \
install-data-recursive uninstall-data-recursive install-exec-recursive \
uninstall-exec-recursive installdirs-recursive uninstalldirs-recursive \
all-recursive check-recursive installcheck-recursive info-recursive \
dvi-recursive mostlyclean-recursive distclean-recursive clean-recursive \
maintainer-clean-recursive tags tags-recursive mostlyclean-tags \
distclean-tags clean-tags maintainer-clean-tags distdir info dvi \
installcheck all-am install-data-am uninstall-am install-exec \
install-data install uninstall all installdirs mostlyclean-generic \
distclean-generic clean-generic maintainer-clean-generic clean \
mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
