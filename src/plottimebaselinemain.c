#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char Usage[256];
char *FITSFile = NULL;
int chan2disp = -1;
char dev='X';
int mode = 0;
int scl = 0;
int nrow = ROWS;
int ncol = COLS;
int BigEndian    = 0 ;
int LittleEndian = 0 ;
int start=0, stop=0;

int plottibl(int argc, char **argv )
{
 static  fitsfile *fvis;
 PlotParType plot;
 ProjParType proj;
 int recnum = 0, nrecs = 0, bl = 0;
 int loop = 1;
 int i;

 plot.driver = dev;
 proj.preint = 1;

 if( FITSFile == NULL || (plot.driver != 'x' && plot.driver != 'X' && plot.driver != 'p' && plot.driver != 'P') )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 if(mode != 1) mode = 0;
 proj.dispBuf.altswtch = mode;
 proj.dispBuf.nrow = nrow;
 proj.dispBuf.ncol = ncol;
 proj.scale = scl;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 open_fits( &fvis, FITSFile );
 filesize( FITSFile ); 
 proj.nrecs =  nrecs = fill_proj(fvis, &proj);

 if( chan2disp >= proj.chans || chan2disp < 0 )
     proj.dispBuf.chan2blank = proj.chans / 2;
 else
     proj.dispBuf.chan2blank = chan2disp;

 getpar_timevis( &proj );

 while(nrecs-- > 0)
 { recnum = readFITS2Vis( fvis, &proj );
   extract_vis( &proj );
   fprintf(finfo, "%s Read [ %d / %d ] records\r", info, recnum, proj.nrecs);
 }

 if ( plot.driver == 'P' || plot.driver == 'p' )
 { fprintf(finfo, "\n%s ======= Plots in pgplot.ps =======\n", info);
   ps_nplot_timevis( & proj );
 }
 else if( plot.driver == 'x' || plot.driver == 'X') 
   xs_nplot_timevis( & proj );

 close_fits(fvis);
 free_display_databuf( &proj );
 kill_proj( &proj );
 newline();
 done();

 return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile , OPT_STRING, 'i', "in"   , "Input FITS File" );
  optrega(&chan2disp , OPT_UINT, 'n', "channel"   ,  "Channel to display" );
  optrega(&start, OPT_UINT, 't', "start", "Start record");
  optrega(&stop, OPT_UINT, 'p', "stop", "Stop record");
  optrega(&nrow , OPT_UINT, 'r', "rows"   ,  "Number of rows per page" );
  optrega(&ncol , OPT_UINT, 'c', "cols"   ,  "Number of cols per page" );
  optrega(&dev , OPT_CHAR, 'd', "dev"   ,  " Plot device : x/X for XWindow, p/P for PS file, no space after -d" ); 
  optrega(&mode , OPT_UINT, 'm', "mode"   ,  "\n[-m=0] Plot mode 0 for Amp-Phase, 1 for Real/Imag" );
  optrega(&scl, OPT_BOOL, 's', "scale" , "Scale reset to absolute");
  sprintf(Usage, "\n-i InputFITSFile\n[-n=N/2] Display chan.\n[-r=%d] plot rows\n[-c=%d] plot cols\n[-d='X'] Device\n[m=0] mode\n[-s] Scale reset", nrow, ncol);
  optUsage(Usage);
  optTitle("Plots of time vs the visibilities of all baseline. \n");
  optMain(plottibl);
  opt(&argc,&argv);

  return plottibl(argc, argv);
}
