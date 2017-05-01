#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char Usage[256];
char *FITSFile1 = NULL;
char *FITSFile2 = NULL;
char dev='X';
char itf = 'l';
int BigEndian    = 0 ;
int LittleEndian = 0 ;
int colour = 0 ;
//int publish = 0;
int chan2show = -1;
double threshold = 0.0;

int FITSpolview(int argc, char **argv )
{
 PlotParType plot;
 ProjParType proj;

 proj.threshold = threshold;
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

 read_FITSmap( &proj, FITSFile1 );
 newline();
 if(chan2show <= 0 || chan2show > proj.chans ) proj.chan2cal = 1;
 else proj.chan2cal = chan2show;
 filesize( FITSFile ); 
 //if(publish && plot.driver =='p') wedgeplot_map_for_paper( &proj, &plot );
 //else 
 wedgeplot_map( &proj, &plot );

 read_FITSmap( &proj, FITSFile2 );
 newline();
 if(chan2show <= 0 || chan2show > proj.chans ) proj.chan2cal = 1;
 else proj.chan2cal = chan2show;
 filesize( FITSFile ); 
 //if(publish && plot.driver =='p') wedgeplot_map_for_paper( &proj, &plot );
 //else 
 overlay_vector_map( &proj, &plot );


 free_display_databuf( &proj );
 done();

 return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile1 , OPT_STRING, 'i', "in"   , "Input P FITS File" );
  optrega(&FITSFile2 , OPT_STRING, 'j', "jn"   , "Input PA FITS File" );
  optrega(&chan2show , OPT_INT, 'n', "chan", "Channel to show");
  optrega(&threshold, OPT_DOUBLE, 't', "threshold", "Lower flux limit for visualization");
  optrega(&dev , OPT_CHAR, 'd', "dev"   ,  "Plot device : x/X for XWindow, p/P for PS file, no space after -d" ); 
  optrega(&itf , OPT_CHAR, 'f', "itf"   ,  "Image Transfer Function : default 'l'- lin, 'g'- log, 's' - sqrt" );
  optrega(&colour, OPT_BOOL, 'c', "", "Show colour map [ default: monochrome ]");
  optrega(&publish, OPT_BOOL, 'p', "", "[produce publishable ps map [ default: no ]");
  sprintf(Usage, "\n-i InputFITSFile\n[-d='X'] Plot device\n[-p] to publish; default no\n[-c : colour map ; default monochrome ]\nITF: default 'l'- lin, 'g'- log, 's'- sqrt\n[-n : channel to show ; default N/2]\n-t threshold mJy [default 0]");
  optUsage(Usage);
  optTitle("Displays the FITS map in false colour");
  optMain(FITSpolview);
  opt(&argc,&argv);

  return FITSpolview(argc, argv);
}
