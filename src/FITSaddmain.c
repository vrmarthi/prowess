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
char itf = 'l';
int BigEndian    = 0 ;
int LittleEndian = 0 ;
int colour = 0 ;
int chan2show = -1;

int FITSadd(int argc, char **argv )
{
 PlotParType plot;
 ProjParType proj;

 plot.driver = dev;
 plot.colour = colour;
 if( itf == 'g' ) plot.itf = LOG;
 else if( itf == 's' ) plot.itf = SQR;
 else plot.itf = LIN;

 if( FITSFile == NULL || (plot.driver != 'x' && plot.driver != 'X' && plot.driver != 'p' && plot.driver != 'P') )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }


 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 read_FITSmap( &proj, FITSFile );
 newline();
 if(chan2show <= 0 || chan2show > proj.chans ) proj.chan2cal = 1;
 else proj.chan2cal = chan2show;
 filesize( FITSFile ); 
 wedgeplot_map( &proj, &plot );

 free_display_databuf( &proj );
 done();

 return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile , OPT_STRING, 'i', "in"   , "Input FITS File" );
  optrega(&chan2show , OPT_INT, 'n', "chan", "Channel to show");
  optrega(&dev , OPT_CHAR, 'd', "dev"   ,  "Plot device : x/X for XWindow, p/P for PS file, no space after -d" ); 
  optrega(&itf , OPT_CHAR, 'f', "itf"   ,  "Image Transfer Function : default 'l'- lin, 'g'- log, 's' - sqrt" );
  optrega(&colour, OPT_BOOL, 'c', "", "Show colour map [ default: monochrome ]");
  sprintf(Usage, "\n-i InputFITSFile\n[-d='X'] Plot device\n[-c : colour map ; default monochrome ]\nITF: default 'l'- lin, 'g'- log, 's'- sqrt\n[-n : channel to show ; default N/2]");
  optUsage(Usage);
  optTitle("Displays the FITS map in false colour");
  optMain(FITSview);
  opt(&argc,&argv);

  return FITSadd(argc, argv);
}
