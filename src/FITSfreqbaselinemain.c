#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char Usage[256];
char *FITSFile = NULL;
char dev='X';
int mode = 0;
int chan2blank=4;
int rec2disp = 1;
int BigEndian    = 0 ;
int LittleEndian = 0 ;

int FITSbvsf(int argc, char **argv )
{
 static  fitsfile *fvis;
 PlotParType plot;
 ProjParType proj;
 int recnum = 0, nrecs = 0, bl = 0;
 int loop = 1, next = 1;
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
 proj.dispBuf.chan2blank = chan2blank;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 open_fits( &fvis, FITSFile );
 filesize( FITSFile ); 
 proj.nrecs =  nrecs = fill_proj(fvis, &proj);

 getpar_instspec( &proj );

 do{
 fprintf(fprompt, "%s Record to quit / display [ 0 / 1 - %d ] : ", prompt, proj.nrecs );
 scanf("%d", &rec2disp);
 if(rec2disp < 1 ) 
 { fprintf(finfo, "%s Quitting", info);
    break;
 }
 if( rec2disp > proj.nrecs )
 { fprintf(fwarn, "%s Record number outside range [1 -%d] records\n", warn, proj.nrecs);
   continue;
 }
 else if(plot.driver == 'X' || plot.driver == 'x' )
 fprintf(finfo, "%s Press any key to toggle / Press 'q' to quit record", info);
 else if ( plot.driver == 'P' || plot.driver == 'p' )
 fprintf(finfo, "%s ======= Plots in pgplot.ps =======", info);

 proj.chan2cal = rec2disp;
 readFITSrec( fvis, &proj );
 extract_rec( &proj );
 buffer_range( &(proj.dispBuf), proj.chans, proj.baselines );
 do loop = imag_instspec( &proj, &plot );
 while(loop);
 fprintf(stderr,"\n");
 }while(next);

 close_fits(fvis);
 free_display_databuf( &proj );
 done();
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile , OPT_STRING, 'i', "in"   , "Input FITS File" );
  optrega(&dev , OPT_CHAR, 'd', "dev"   ,  "Plotting Device" ); 
  optrega(&mode , OPT_UINT, 'm', "mode"   ,  "Plotting Mode" );
  optrega(&chan2blank , OPT_UINT, 'k', "blank"   ,  "Channels to blank" );
  strcat(Usage, "\n-i InputFITSFile");
  strcat(Usage, "\n-r Record number");
  strcat(Usage, "\n-d Plot Device : x/X for XWindow, p/P for PS file, no space after -d");
  strcat(Usage, "\n-m Plot mode 0 for Amp-Phase, 1 for Real/Imag");
  strcat(Usage, "\n-k Channels 0 through n-1 to blank");
  optUsage(Usage);
  optTitle("Plots of time vs freq of the visibilities of a baseline. \n");
  optMain(FITSbvsf);
  opt(&argc,&argv);

  return FITSbvsf(argc, argv);
}
