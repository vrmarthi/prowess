#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char *FITSFile=NULL;
char *InFile=NULL;
int nsrc=-2;
char Usage[256];
char dev='X';
char itf = 'l';
int colour = 0 ;
int BigEndian    = 0 ;
int LittleEndian = 0 ;

int HItoFITS(int argc, char **argv )
{
int status = 0;
static fitsfile *fmap;
char outfilename[80];
ProjParType proj;
PlotParType plot;
FILE *fmodel;

 plot.driver = dev;
 plot.colour = colour;
 if( itf == 'g' ) plot.itf = LOG;
 else if( itf == 's' ) plot.itf = SQR;
 else plot.itf = LIN;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 
 if( FITSFile == NULL || InFile == NULL )
 { fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
   fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
   exit(-1);
 }

   strcpy(proj.HI.filename, FITSFile);
   
   init_proj(&proj); 
   write_HImap2FITS( &proj, InFile );
   kill_proj( &proj );

 done();
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&InFile, OPT_STRING, 'i', "in" , "Input HI native data"             );
  optrega(&FITSFile, OPT_STRING, 'o', "out" , "Output HI FITS cube"             );
  sprintf(Usage, "\n-i Input data file -o OutputFITSmap");
  optUsage(Usage);
  optTitle("Writes particle density to HI intensity FITS cube.\n");
  optMain(HItoFITS);
  opt(&argc,&argv);

  return HItoFITS(argc, argv);
}
