#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char Usage[256];
char *solfilename = NULL;
char dev = 'X';

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int plotcmp(int argc, char **argv )
{
 PlotParType plot;
 plot.driver = dev;

 if( solfilename == NULL || (plot.driver != 'x' && plot.driver != 'X' && plot.driver != 'p' && plot.driver != 'P') )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);

 if ( plot.driver == 'X' || plot.driver == 'x' )
 { plot.driver = 'X';
   xsplot_solcompare( &plot, solfilename );
 }

 else if ( plot.driver == 'P' || plot.driver == 'p' )
 { plot.driver = 'P';
   psplot_solcompare( &plot, solfilename );
 }
 done();

 return 0;
}

int main(int argc, char **argv )
{
  optrega(&solfilename , OPT_STRING, 'i', "in"   , "Input Solution File" );
  optrega(&dev , OPT_CHAR, 'd', "dev"   ,  "Plotting Device" );
  sprintf(Usage, "\n-i InputSolFile\n-d Plot Device : x/X for XWindow, p/P for PS file, no space after -d");
  optUsage(Usage);
  optTitle("Generates plots of calibrated complex antenna gains and model sky visibilities. \n");
  optMain(plotcmp);
  opt(&argc,&argv);

  return plotcmp(argc, argv);
}
