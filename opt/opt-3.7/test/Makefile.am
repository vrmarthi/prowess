# $Id: Makefile.am,v 3.4 1998/07/01 16:59:17 jt Exp $

AUTOMAKE_OPTIONS = no-dependencies

TESTS = checkopt

noinst_PROGRAMS = testopt birthday hooktest iloop testmain testtypes

EXTRA_DIST= checkopt
##I would have thought checkopt was a noinst_SCRIPT, but it doesn't
##even get included in the distribution if I do that
##noinst_SCRIPTS= checkopt

## Uncomment the appropriate line depending on whether you want to
## test libopt.a or opt.o
##TESTLIB = ../src/opt.o
TESTLIB= ../src/libopt.a
LDADD = $(TESTLIB)

INCLUDES = -I$(top_srcdir)/src

DEPENDENCIES = $(TESTLIB)

## This remakes the library if it doesn't exist

RANLIB = @RANLIB@

../src/libopt.a:
	cd ../src; $(MAKE) libopt.a; $(RANLIB) libopt.a

../src/opt.o:
	cd ../src; $(MAKE) opt.o



