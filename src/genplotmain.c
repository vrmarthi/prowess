#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char Usage[256];
char *solfilename = NULL;

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int genplot(int argc, char **argv )
{
 PlotParType plot;
 plot.driver = 'X';

 if( solfilename == NULL )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);

 if( ! (strstr(solfilename, "lin") || strstr(solfilename, "log")) )
 xsplot_all( &plot, solfilename );
 else
 xsplot_all_but_errors( &plot, solfilename );
 done();

 return 0;
}

int main(int argc, char **argv )
{
  optrega(&solfilename , OPT_STRING, 'i', "in"   , "Input Solution File" );
  optrega(&solfilename , OPT_CHAR, 'd', "dev"   ,  "Plotting Device" );
  sprintf(Usage, "\n-i InputSolFile");
  sprintf(Usage, "\n-d Plot Device : x/X for XWindow, p/P for PS file, no space after -d");
  optUsage(Usage);
  optTitle("Generates plots of calibrated complex antenna gains and model sky visibilities. \n");
  optMain(genplot);
  opt(&argc,&argv);

  return genplot(argc, argv);
}
