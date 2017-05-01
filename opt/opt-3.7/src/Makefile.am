#AUTOMAKE_OPTIONS = no-dependencies

lib_LIBRARIES    = libopt.a

include_HEADERS  = opt.h

## Order is important here, opt_sf.h should be first
noinst_HEADERS   = opt_sf.h ag.h opt_p.h

libopt_a_SOURCES = opt_util.c opt_reg.c opt_regt.c opt_menu.c opt_proc.c ag.c

EXTRA_DIST= makeopth.pl

## The following is used only if --with-readline is invoked
## xrlo is list of object files in libreadline.a
## xtco is list of object files in libtermcap.a
libopt_a_LIBADD  = @xrlo@ @xtco@
libopt_a_DEPENDENCIES = @xrlo@ @xtco@

readline_PATH = @readline_PATH@
termcap_PATH =  @termcap_PATH@

@xrlo@: $(readline_PATH) 
	$(AR) x $(readline_PATH)

@xtco@: $(termcap_PATH)
	$(AR) x $(termcap_PATH)



####################
##
##      Make a single source file; this is useful for attaching
##      opt source to other packages without a lot of overhead
##

opt.c:  $(noinst_HEADERS) $(libopt_a_SOURCES)
	echo "#define PACKAGE \"@PACKAGE@\""  > opt.c
	echo "#define VERSION \"@VERSION@\"" >> opt.c
	echo "#include" \"opt.h\"            >> opt.c
	cat $(noinst_HEADERS)                >> opt.c
	cat $(libopt_a_SOURCES)              >> opt.c

opt.o: opt.c
	$(CC) $(CFLAGS) -c opt.c

###
  






