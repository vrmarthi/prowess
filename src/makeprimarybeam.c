#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char *FITSFile=NULL;
int nsrc=-2;
char Usage[256];
int galfg_swtch = OFF;
int pntsrc_swtch = OFF;
char dev='X';
char itf = 'l';
int colour = 0 ;
int BigEndian    = 0 ;
int LittleEndian = 0 ;

int mkprbeam(int argc, char **argv )
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
 
 if( FITSFile == NULL )
 { fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
   fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
   exit(-1);
 }

   init_proj(&proj); 
   init_primary_beam( &proj );
   compute_primary_beam( &proj );
   write_prbeam2FITS( &proj, FITSFile );
   read_FITSmap( &proj, FITSFile);
   proj.chan2cal = proj.chans / 2 ;
   wedgeplot_map( &proj, &plot );
   free_display_databuf( &proj );
   kill_proj( &proj );

 done();
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile, OPT_STRING, 'o', "out" , "Output Primary Beam FITS map" );
  sprintf(Usage, "\n-o OutputFITSmap");
  optUsage(Usage);
  optTitle("Generates the primary beam of the Ooty Radio Telescope.\n");
  optMain(mkprbeam);
  opt(&argc,&argv);

  return mkprbeam(argc, argv);
}
