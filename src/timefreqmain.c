#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char Usage[256];
char *FITSFile = NULL;
int ant1 = 0, ant2 = 1;
char dev='X';
int mode = 0;
int BigEndian    = 0 ;
int LittleEndian = 0 ;

int timefreq(int argc, char **argv )
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
 proj.dynspec.altswtch = mode;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 open_fits( &fvis, FITSFile );
 filesize( FITSFile ); 
 proj.nrecs =  nrecs = fill_proj(fvis, &proj);

 if( ! strcmp(proj.Ant[ant1].name, "") ) 
     { fprintf(ferr, "%s Antenna %d not found. Quitting.\n", err, ant1);
   exit(-1);
 }
 if( ! strcmp(proj.Ant[ant2].name, "") ) 
     { fprintf(ferr, "%s Antenna %d not found. Quitting.\n", err, ant2);
   exit(-1);
 }

 bl = getpar_dynspect( &proj, ant1, ant2 );

 while(proj.nrecs-- > 0)
 { recnum = readFITS2Vis( fvis, &proj );
   extract_baseline( &proj, bl );
   fprintf(finfo, "%s Bl : %d (%s-%s) read [ %d / %d ] records\r", info, bl, proj.Ant[ant1].name, proj.Ant[ant2].name, recnum, nrecs);
 }

 if ( plot.driver == 'X' )fprintf(finfo, "\n%s Press any key to toggle display / Press 'q' to quit", info);

 do{
 buffer_range( &(proj.dynspec) );
 loop = plot_dynspec( &proj, &plot );
 } while(loop);

 close_fits(fvis);
 free_dynspect_databuf( &proj );
 done();

 return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile , OPT_STRING, 'i', "in"   , "Input FITS File" );
  optrega(&ant1 , OPT_UINT, 'a', "ant"   ,  "Antenna 1" );
  optrega(&ant2 , OPT_UINT, 'b', "ant"   ,  "Antenna 2" );
  optrega(&dev , OPT_CHAR, 'd', "dev"   ,  "Plotting Device" ); 
  optrega(&mode , OPT_UINT, 'm', "mode"   ,  "Plotting Mode" );
  strcat(Usage, "\n-i InputFITSFile");
  strcat(Usage, "\n-a Antenna 1");
  strcat(Usage, "\n-b Antenna 2");
  strcat(Usage, "\n-d Plot Device : x/X for XWindow, p/P for PS file, no space after -d");
  strcat(Usage, "\n-m Plot mode 0 for Amp-Phase, 1 for Real/Imag");
  optUsage(Usage);
  optTitle("Plots of time vs freq of the visibilities of a baseline. \n");
  optMain(timefreq);
  opt(&argc,&argv);

  return timefreq(argc, argv);
}
