#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char Usage[512];
char *FITSFile = NULL;
char dev='X';
int chan2disp = 0;
int mode = 0;
int BigEndian    = 0 ;
int LittleEndian = 0 ;

int imagtibl(int argc, char **argv )
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

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 open_fits( &fvis, FITSFile );
 filesize( FITSFile ); 
 proj.nrecs =  nrecs = fill_proj(fvis, &proj);

 if( chan2disp == 0) proj.dispBuf.chan2blank = proj.chans / 2;
 else proj.dispBuf.chan2blank = chan2disp;

 getpar_timevis( &proj );

 while(nrecs-- > 0)
 { recnum = readFITS2Vis( fvis, &proj );
   extract_vis( &proj );
   if ( !(recnum % 50) )
   fprintf(finfo, "%s Read [ %d / %d ] records\r", info, recnum, proj.nrecs);
 }
 fprintf(finfo, "%s Read [ %d / %d ] records\r", info, recnum, proj.nrecs);
 newline(); 
 if ( plot.driver == 'X' || plot.driver == 'x' )
 fprintf(finfo, "%s Press any key to toggle display / Press 'q' to quit", info);
 else if ( plot.driver == 'P' || plot.driver == 'p' )
 fprintf(finfo, "%s ======= Plots in pgplot.ps =======", info);

 do{
   buffer_range( &(proj.dispBuf), proj.nrecs, proj.baselines );
   loop = imag_dynvis( &proj, &plot );
 } while(loop);

 close_fits(fvis);
 free_display_databuf( &proj );
 kill_proj( &proj );
 done();
 filesize(FITSFile);
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile , OPT_STRING, 'i', "in"   , "Input FITS File" );
  optrega(&dev , OPT_CHAR, 'd', "dev"   ,  "Plot device : x/X for XWindow, p/P for PS file, no space after -d" ); 
  optrega(&mode , OPT_UINT, 'm', "mode"   ,  "Plot mode 0 for Amp-Phase, 1 for Real/Imag" );
  optrega(&chan2disp , OPT_UINT, 'n', "chan"   ,  "Channel number to display" );
  sprintf(Usage, "\n-i InputFITSFile \n[-dX ] Plot device \n[-m=0] Plot mode \n[-n=N/2] Channel number" );
  optUsage(Usage);
  optTitle("Plots of time vs baselines of the visibilities. \n");
  optMain(imagtibl);
  opt(&argc,&argv);

  return imagtibl(argc, argv);
}
