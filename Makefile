SRC     = ./src/
BIN     = ./bin/
INC     = ./inc/
LIB     = ./lib/
DOC	= ./doc/

CC      = $(CCOM)

FITSLIB = -L$(LIB) -lcfitsio
STDLIBS = -lm -lc 
OPTLIB  = -L$(LIB) -lopt
GSLLIB  = -lgsl -lgslcblas
FFTWLIB = -lrfftw -lfftw3
PGPLIB  =  -L/usr/X11/lib -lX11 -lgfortran -L$(LIB) -lcpgplot -lpgplot
PTHRLIB = -lpthread
RDLNLIB = -lreadline -lncurses
#OMPLIB  = -fopenmp
ALLLIB  = $(FITSLIB) $(OPTLIB) $(GSLLIB) $(FFTWLIB) $(STDLIBS) $(PGPLIB)

CASOBJ  = $(SRC)CaSPORT.o
VISOBJ  = $(SRC)genFITSmain.o
EPSOBJ  = $(SRC)genEPSmapsmain.o
MDLVOBJ = $(SRC)mdl2FITSmain.o
F2MDLOBJ= $(SRC)FITS2modelmain.c
PSOBJ   = $(SRC)psFITSmain.o
MKMDLOBJ= $(SRC)makemodelmain.o
MKPRBOBJ= $(SRC)makeprimarybeam.o
C2FOBJ  = $(SRC)corr2FITSmain.o
FLOGOBJ = $(SRC)FITSlogcmain.o
LOGOBJ  = $(SRC)logcalmain.o
FLINOBJ = $(SRC)FITSlincmain.o
LINOBJ  = $(SRC)lincalmain.o
ISOLOBJ = $(SRC)itersolmain.o
IGAINOBJ= $(SRC)itergainmain.o
SECLOBJ = $(SRC)selfcalmain.o
FISOLOBJ= $(SRC)FITSitersolmain.o
FIGNOBJ = $(SRC)FITSitergainmain.o
FINFOBJ = $(SRC)FITSinfomain.o
FISCLOBJ= $(SRC)FITSselfcalmain.o
FBEAMOBJ= $(SRC)FITSbeamformmain.o
FBMBDOBJ= $(SRC)FITSbeambandmain.o
FBSOLOBJ= $(SRC)FITSbandpassmain.o
FULCLOBJ= $(SRC)fullcalibmain.o
CALIBOBJ= $(SRC)FITScalibmain.o
SBMDLOBJ= $(SRC)subtractmodelmain.o
ADMDLOBJ= $(SRC)addmodelmain.o
FTSUBOBJ= $(SRC)FITSsubtractmain.o
FAPCLOBJ= $(SRC)FITSapplcalmain.o
RELTAOBJ= $(SRC)reltaFITSmain.o
RECHNOBJ= $(SRC)rechnFITSmain.o
PLOTOBJ = $(SRC)plotallmain.o
PERROBJ = $(SRC)ploterrmain.o
PSOLOBJ = $(SRC)plotsolmain.o
PCMPOBJ = $(SRC)plotcmpmain.o
PTRAJOBJ= $(SRC)plottrajmain.o
PHESOBJ = $(SRC)plotHessmain.o
PCRBOBJ = $(SRC)plotCRBmain.o
PMDLOBJ = $(SRC)plotmodelmain.o
PSPOBJ	= $(SRC)psplotmain.o
IMTFOBJ = $(SRC)imagtimefreqmain.o
PLTBLOBJ= $(SRC)plottimebaselinemain.o
PLFBLOBJ= $(SRC)plotfreqbaselinemain.o
IMFBLOBJ= $(SRC)imagfreqbaselinemain.o
IMTBLOBJ= $(SRC)imagtimebaselinemain.o
IMMDLOBJ= $(SRC)imagmodelvismain.o
FVOBJ   = $(SRC)FITSviewmain.o
FCHAVOBJ= $(SRC)FITSchavmain.o
FITPBOBJ= $(SRC)fitpbeammain.o
FITALOBJ= $(SRC)fitalphamain.o
STRECOBJ= $(SRC)striprecmain.o
NKRECOBJ= $(SRC)nukerecmain.o
PBCOROBJ= $(SRC)pbcorrectmain.o
FTCATOBJ= $(SRC)FITSconcatmain.o
AVGV2OBJ= $(SRC)avgv2main.o
V2COROBJ= $(SRC)v2corrmain.o
QB2FTOBJ= $(SRC)qb2FITSmain.o
PD2HIOBJ= $(SRC)pdtoHIqbmain.o
FLAGOBJ = $(SRC)flagdatamain.o
REMEZ   = $(SRC)remezsubs.o
SYSOBJ  = $(SRC)syssubs.o
SKYOBJ  = $(SRC)skysubs.o
MATHOBJ = $(SRC)matsubs.o
FITSOBJ = $(SRC)fitssubs.o
COROBJ  = $(SRC)corrsubs.o
PGPOBJ  = $(SRC)plotsubs.o
XTOBJ   = $(SRC)xtract.o
DEPOBJ  = $(REMEZ) $(SYSOBJ) $(SKYOBJ) $(MATHOBJ) $(FITSOBJ) $(PGPOBJ) 

FITSHDR = $(INC)fitsdefs.h
MATHDR  = $(INC)matdefs.h
SYSHDR  = $(INC)sysdefs.h
CORRHDR = $(INC)corrdefs.h
SKYHDR  = $(INC)skydefs.h
PGPHDR  = $(INC)plotdefs.h
REMEZHDR= $(INC)remezdefs.h

DEPHDR  = $(REMEZHDR) $(MATHDR) $(SYSHDR) $(CORRHDR) $(SKYHDR) $(FITSHDR) $(PGPHDR)

FITS	= dep genFITS genEPS mdl2FITS psFITS mkmodel reltaFITS rechnFITS FITSinfo FITSlogc FITSlinc FITSsol FITSgain FITSsecl FITSbeam FITSbmbd FITSbsol fullcal corr2FITS pbcorr fitpbeam fitalpha striprecs nukerecs v2corr avgv2 FITScat FITScal submodel addmodel FITS2mdl FITSsub flagdata mkprbeam avgv2 qb2FITS pd2HI
PLOT	= dep plotall ploterr plottraj plotsol plotcmp plotHess plotCRB psplot imagtifr imagmodl plottibl plotfrbl plotmodl imagfrbl imagtibl FITSview FITSchav
SIM	= dep logcal lincal itersol itergain selfcal 
ALL     = dep fits sim plot xtract CaSPORT


all      : $(ALL)
sim	 : $(SIM)
plot	 : $(PLOT)
fits	 : $(FITS)
CaSPORT	 : $(CASOBJ)
dep      : $(DEPOBJ)
remez	 : $(REMEZ)
syssubs  : $(SYSOBJ)
skysubs  : $(SKYOBJ)
matsubs  : $(MATHOBJ)
plotsubs : $(PGPOBJ)
fitsubs  : $(FITSOBJ)
genFITS  : $(VISOBJ)
genEPS   : $(EPSOBJ)
mdl2FITS : $(MDLVOBJ)
FITS2mdl : $(F2MDLOBJ)
psFITS   : $(PSOBJ)
mkmodel	 : $(MKMDLOBJ)
mkprbeam : $(MKPRBOBJ)
corr2FITS: $(C2FOBJ)
reltaFITS: $(RELTAOBJ)
rechnFITS: $(RECHNOBJ)
FITSinfo : $(FINFOBJ)
FITSlogc : $(FLOGOBJ)
logcal   : $(LOGOBJ)
FITSlinc : $(FLINOBJ)
lincal   : $(LINOBJ)
itersol  : $(ISOLOBJ)
itergain : $(IGAINOBJ)
FITSsol  : $(FISOLOBJ)
FITSgain : $(FIGNOBJ)
selfcal  : $(SECLOBJ)
FITSsecl : $(FISCLOBJ)
imagtifr : $(IMTFROBJ)
imagfrbl : $(IMFBLOBJ)
imagtibl : $(IMTBLOBJ)
imagmodl : $(IMMDLOBJ)
FITSview : $(FVOBJ)
FITSchav : $(FCHAVOBJ)
plottibl : $(PLTBLOBJ)
plotfrbl : $(PLFBLOBJ)
plotmodl : $(PMDLOBJ)
FITSbeam : $(FBEAMOBJ)
FITSbmbd : $(FBMBDOBJ)
FITSbsol : $(FBSOLOBJ)
fullcal  : $(FULCLOBJ)
FITScal  : $(CALIBOBJ)
submodel : $(SBMDLOBJ)
addmodel : $(ADMDLOBJ)
FITSsub  : $(FTSUBOBJ)
plotall  : $(PLOTOBJ)
plottraj : $(PTRAJOBJ)
plotsol  : $(PSOLOBJ)
ploterr  : $(PERROBJ)
plotcmp  : $(PCMPOBJ)
plotHess : $(PHESOBJ)
plotCRB  : $(PCRBOBJ)
psplot	 : $(PSPOBJ)
xtract   : $(XTOBJ)
pbcorr   : $(PBCOROBJ)
fitpbeam : $(FITPBOBJ)
fitalpha : $(FITALOBJ)
striprecs: $(STRECOBJ)
nukerecs : $(NKRECOBJ)
v2corr   : $(V2COROBJ)
qb2FITS  : $(QB2FTOBJ)
pd2HI    : $(PD2HIOBJ)
avgv2    : $(AVGV2OBJ)
flagdata : $(FLAGOBJ)
FITScat  : $(FTCATOBJ)

CaSPORT  : $(CASOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)CaSPORT $(CASOBJ) $(DEPOBJ) $(ALLLIB) $(RDLNLIB)

genFITS  : $(VISOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)genFITS $(VISOBJ) $(DEPOBJ) $(ALLLIB) 

genEPS   : $(EPSOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)genEPS $(EPSOBJ) $(DEPOBJ) $(ALLLIB) 

mdl2FITS : $(MDLVOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)mdl2FITS $(MDLVOBJ) $(DEPOBJ) $(ALLLIB) )

FITS2mdl : $(F2MDLOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)FITS2mdl $(F2MDLOBJ) $(DEPOBJ) $(ALLLIB) )

psFITS   : $(PSOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)psFITS $(PSOBJ) $(DEPOBJ) $(ALLLIB)

mkmodel  : $(MKMDLOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)mkmodel $(MKMDLOBJ) $(DEPOBJ) $(ALLLIB)

mkprbeam : $(MKPRBOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)mkprbeam $(MKPRBOBJ) $(DEPOBJ) $(ALLLIB)

corr2FITS: $(C2FOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)corr2FITS $(C2FOBJ) $(DEPOBJ) $(ALLLIB)

reltaFITS: $(RELTAOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)reltaFITS $(RELTAOBJ) $(DEPOBJ) $(ALLLIB)

rechnFITS: $(RECHNOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)rechnFITS $(RECHNOBJ) $(DEPOBJ) $(ALLLIB)

FITSinfo : $(FINFOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)FITSinfo $(FINFOBJ) $(DEPOBJ) $(ALLLIB)

FITSlogc : $(FLOGOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)FITSlogc $(FLOGOBJ) $(DEPOBJ) $(ALLLIB)

FITSlinc : $(FLINOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)FITSlinc $(FLINOBJ) $(DEPOBJ) $(ALLLIB)

FITSsol  : $(FISOLOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)FITSsol $(FISOLOBJ) $(DEPOBJ) $(ALLLIB)

FITSgain : $(FIGNOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)FITSgain $(FIGNOBJ) $(DEPOBJ) $(ALLLIB)

FITSsecl : $(FISCLOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)FITSsecl $(FISCLOBJ) $(DEPOBJ) $(ALLLIB)

FITSbeam : $(FBEAMOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)FITSbeam $(FBEAMOBJ) $(DEPOBJ) $(ALLLIB)

FITSbmbd : $(FBMbdOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)FITSbmbd $(FBMBDOBJ) $(DEPOBJ) $(ALLLIB)

FITSbsol : $(FBSOLOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)FITSbsol $(FBSOLOBJ) $(DEPOBJ) $(ALLLIB)

fullcal  : $(FULCLOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)fullcal $(FULCLOBJ) $(DEPOBJ) $(ALLLIB)

FITScal  : $(CALIBOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)FITScal $(CALIBOBJ) $(DEPOBJ) $(ALLLIB)

submodel : $(SBMDLOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)submodel $(SBMDLOBJ) $(DEPOBJ) $(ALLLIB)

addmodel : $(ADMDLOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)addmodel $(ADMDLOBJ) $(DEPOBJ) $(ALLLIB)

FITSsub  : $(FTSUBOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)FITSsub $(FTSUBOBJ) $(DEPOBJ) $(ALLLIB)

logcal   : $(LOGOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)logcal $(LOGOBJ) $(DEPOBJ) $(ALLLIB)

lincal   : $(LINOBJ)  $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)lincal $(LINOBJ) $(DEPOBJ) $(ALLLIB) 

itersol	 : $(ISOLOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)itersol $(ISOLOBJ) $(DEPOBJ) $(ALLLIB)

itergain : $(IGAINOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)itergain $(IGAINOBJ) $(DEPOBJ) $(ALLLIB)

selfcal	 : $(SECLOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)selfcal $(SECLOBJ) $(DEPOBJ) $(ALLLIB)

plotall	 : $(PLOTOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)plotall $(PLOTOBJ) $(DEPOBJ) $(ALLLIB)

ploterr	 : $(PERROBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)ploterr $(PERROBJ) $(DEPOBJ) $(ALLLIB)

plottraj : $(PTRAJOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)plottraj $(PTRAJOBJ) $(DEPOBJ) $(ALLLIB)

plotsol	 : $(PSOLOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)plotsol $(PSOLOBJ) $(DEPOBJ) $(ALLLIB)

plotcmp	 : $(PCMPOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)plotcmp $(PCMPOBJ) $(DEPOBJ) $(ALLLIB)

plotHess : $(PHESOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)plotHess $(PHESOBJ) $(DEPOBJ) $(ALLLIB)

plotCRB  : $(PCRBOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)plotCRB $(PCRBOBJ) $(DEPOBJ) $(ALLLIB)

plotmodl : $(PMDLOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)plotmodl $(PMDLOBJ) $(DEPOBJ) $(ALLLIB)

psplot   : $(PSPOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)psplot $(PSPOBJ) $(DEPOBJ) $(ALLLIB)

imagtifr : $(IMTFOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)imagtifr $(IMTFOBJ) $(DEPOBJ) $(ALLLIB)

plottibl : $(PLTBLOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)plottibl $(PLTBLOBJ) $(DEPOBJ) $(ALLLIB)

plotfrbl : $(PLFBLOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)plotfrbl $(PLFBLOBJ) $(DEPOBJ) $(ALLLIB)

imagfrbl : $(IMFBLOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)imagfrbl $(IMFBLOBJ) $(DEPOBJ) $(ALLLIB)

imagtibl : $(IMTBLOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)imagtibl $(IMTBLOBJ) $(DEPOBJ) $(ALLLIB)

imagmodl : $(IMMDLOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)imagmodl $(IMMDLOBJ) $(DEPOBJ) $(ALLLIB)

FITSview : $(FVOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)FITSview $(FVOBJ) $(DEPOBJ) $(ALLLIB)

FITSchav : $(FCHAVOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)FITSchav $(FCHAVOBJ) $(DEPOBJ) $(ALLLIB)

xtract   : $(XTOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)xtract $(XTOBJ) $(DEPOBJ) $(ALLLIB)

pbcorr   : $(PBCOROBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)pbcorr $(PBCOROBJ) $(DEPOBJ) $(ALLLIB)

fitpbeam : $(FITPBOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)fitpbeam $(FITPBOBJ) $(DEPOBJ) $(ALLLIB)

fitalpha : $(FITALOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)fitalpha $(FITALOBJ) $(DEPOBJ) $(ALLLIB)

striprecs: $(STRECOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)striprecs $(STRECOBJ) $(DEPOBJ) $(ALLLIB)

nukerecs : $(NKRECOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)nukerecs $(NKRECOBJ) $(DEPOBJ) $(ALLLIB)

v2corr   : $(V2COROBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)v2corr $(V2COROBJ) $(DEPOBJ) $(ALLLIB)

qb2FITS  : $(QB2FTOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)qb2FITS $(QB2FTOBJ) $(DEPOBJ) $(ALLLIB)

pd2HI    : $(PD2HIOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)pd2HI $(PD2HIOBJ) $(DEPOBJ) $(ALLLIB)

avgv2    : $(AVGV2OBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)avgv2  $(AVGV2OBJ) $(DEPOBJ) $(ALLLIB)

flagdata : $(FLAGOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)flagdata  $(FLAGOBJ) $(DEPOBJ) $(ALLLIB)

FITScat  : $(FTCATOBJ) $(DEPOBJ) $(DEPHDR)
	   $(CC) $(CFLAGS) -o $(BIN)FITScat $(FTCATOBJ) $(DEPOBJ) $(ALLLIB)

clean    :
	  /bin/rm $(SRC)*.o

install	 :
	  cp bin/* ~/astro/bin/.
	  cp bin/* ~/bin/.
