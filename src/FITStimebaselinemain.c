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
int nplot = 5;
int BigEndian    = 0 ;
int LittleEndian = 0 ;

int FITStvsb(int argc, char **argv )
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
 proj.dispBuf.nplot = nplot;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 open_fits( &fvis, FITSFile );
 filesize( FITSFile ); 
 proj.nrecs =  nrecs = fill_proj(fvis, &proj);

 if(chan2disp == -1 )
     proj.dispBuf.chan2blank = proj.chans / 2;
 else
     proj.dispBuf.chan2blank = chan2disp;

 getpar_timevis( &proj );

 while(nrecs-- > 0)
 { recnum = readFITS2Vis( fvis, &proj );
   extract_vis( &proj );
   if(!(recnum%10)) fprintf(finfo, "%s Read [ %d / %d ] records\r", info, recnum, proj.nrecs);
 }
 fprintf(finfo, "%s Read [ %d / %d ] records\r", info, recnum, proj.nrecs);

 if ( plot.driver == 'P' || plot.driver == 'p' )
 { fprintf(finfo, "\n%s ======= Plots in pgplot.ps =======\n", info);
   ps_nplot_timevis( & proj, &plot );
 }
 else if( plot.driver == 'x' || plot.driver == 'X') 
   xs_nplot_timevis( & proj, &plot );

 close_fits(fvis);
 free_display_databuf( &proj );
 done();

 return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile , OPT_STRING, 'i', "in"   , "Input FITS File" );
  optrega(&chan2disp , OPT_UINT, 'c', "channel"   ,  "Channel to display" );
  optrega(&nplot , OPT_UINT, 'n', "nplot"   ,  "Number of baselines per page" );
  optrega(&dev , OPT_CHAR, 'd', "dev"   ,  "Plotting Device" ); 
  optrega(&mode , OPT_UINT, 'm', "mode"   ,  "Plotting Mode" );
  strcat(Usage, "\n-i InputFITSFile");
  strcat(Usage, "\n-c Channel to display");
  strcat(Usage, "\n-d Plot Device : x/X for XWindow, p/P for PS file, no space after -d");
  strcat(Usage, "\n-m Plot mode 0 for Amp-Phase, 1 for Real/Imag");
  optUsage(Usage);
  optTitle("Plots of time vs the visibilities of all baseline. \n");
  optMain(FITStvsb);
  opt(&argc,&argv);

  return FITStvsb(argc, argv);
}
