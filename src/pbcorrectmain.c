#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char *inFITSMap=NULL;
char *pb=NULL;
char dev = 'X';
char Usage[256];
int mode=0;
char itf = 'l';
int colour = 0 ;

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int pbcorr(int argc, char **argv )
{
 int status = 0;
 char pbFITSMap[80], outFITSMap[80];
 ProjParType proj;
 PlotParType plot;
 int loop;
 int algo;

 plot.driver = dev;
 plot.colour = colour;

 if( itf == 'g' ) plot.itf = LOG;
 else if( itf == 's' ) plot.itf = SQR;
 else plot.itf = LIN;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 if( inFITSMap == NULL || (plot.driver != 'x' && plot.driver != 'X' && plot.driver != 'p' && plot.driver != 'P') )
 { fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
   fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
   exit(-1);
 }
 else if (inFITSMap != NULL )
 { proj.preint = 0;
   init_proj(&proj); 
   
   if( pb == NULL ) strcpy(pbFITSMap, "PRBEAM.FITS");
   else strcpy(pbFITSMap, pb);

   if(strstr(inFITSMap, "FITS"))
   {
   strcpy(outFITSMap, inFITSMap);
   strtok(outFITSMap, ".");
   strcat(outFITSMap, ".PBCOR.FITS");
   }
   
   read_FITSmap( &proj, pbFITSMap );
   newline();
   filesize( pbFITSMap ); 

   init_primary_beam( &proj );
   copy_skymap_to_pbeam( &proj );

   read_FITSmap( &proj, inFITSMap );
   newline();
   filesize( inFITSMap ); 

   correct_primary_beam( &proj );
   wedgeplot_map( &proj, &plot );

   write_sky2FITS( &proj, outFITSMap );

   free_display_databuf( &proj );
   
   kill_proj( &proj );
   done(); 
  }
  return 0;

}

int main(int argc, char **argv )
{
  optrega(&inFITSMap, OPT_STRING, 'i', "in" , "Input FITS map file"             );
  optrega(&pb, OPT_STRING, 'p', "in" , "Input FITS PBEAM file"             );
  optrega(&dev    , OPT_CHAR,   'd', "device", "X / PS");
  optrega(&itf , OPT_CHAR, 'f', "itf"   ,  "Image Transfer Function : default 'l'- lin, 'g'- log, 's' - sqrt" );
  optrega(&colour, OPT_BOOL, 'c', "", "Show colour map [ default: monochrome ]");
  sprintf(Usage, "\n\t-i InputFITSmap \n\t-p InputFITSpb\n\t-d default x/X for Xwindow, p/P for postscript.");
  optUsage(Usage);
  optTitle("Plots the model visibilities from the model vsibility file or for a given map observed by the Ooty Radio Telescope.\n");
  optMain(pbcorr);
  opt(&argc,&argv);

  return pbcorr(argc, argv);
}
