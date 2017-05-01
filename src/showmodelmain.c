#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char *FITSFile=NULL;
char *modelfile=NULL;
int nsrc=-2;
char Usage[256];
char dev = 'X';
int mode=0;
char itf = 'l';

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int showmodel(int argc, char **argv )
{
 int status = 0;
 static fitsfile *fmap;
 char outfilename[80];
 ProjParType proj;
 PlotParType plot;
 FILE *fmodel;
 int loop;
 int algo;

 plot.driver = dev;
 if( itf == 'g' ) plot.itf = LOG;
 else plot.itf = LIN;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 if( FITSFile == NULL)
 { fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
   fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
   exit(-1);
 }
 else if (FITSFile != NULL )
 { proj.preint = 0;
   init_proj(&proj); 
   
   if(strstr(FITSFile, "FITS")){
   strcpy(outfilename, FITSFile);
   strtok(outfilename, ".");
   strcat(outfilename, ".model");
   if( FITSmap_to_model( &proj, FITSFile ) ) 
   { fprintf(ferr, "%s %s is likely a UVFITS file \n", err, FITSFile);
     fprintf(ferr, "%s Please supply a FITS map\n", info);
     kill_proj( &proj );
     done();
     return 0;
   }
   newline();
   write_model_visibilities( &proj, outfilename );
   }
   
 else if(strstr(FITSFile, "model"))
   read_model_visibilities(&proj, FITSFile);
 }

 if( plot.driver == 'X' || plot.driver == 'x' )     plot.driver = 'X' ;
 if( plot.driver == 'P' || plot.driver == 'p' )     plot.driver = 'P' ;
 if(mode != 1) mode = 0;
 proj.dispBuf.altswtch = mode;

 do{
 loop = plot_sky_model(&proj, &plot);
 }while(loop);

 kill_proj( &proj );
 done(); 
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile, OPT_STRING, 'i', "in" , "Input FITS map/model file"             );
  optrega(&mode , OPT_UINT, 'm', "mode"   ,  "Plot mode 0 for Amp-Phase, 1 for Real/Imag" );
  optrega(&dev    , OPT_CHAR,   'd', "device", "X / PS");
  optrega(&itf , OPT_CHAR, 'f', "axf"   ,  "Axis scaling Function : default 'l'- lin, 'g'- log" );
  sprintf(Usage, "\n\t-i InputFITSmap \n\t-m 0/1 for amp-phase / real-imag\n\t-d default x/X for Xwindow, p/P for postscript.");
  optUsage(Usage);
  optTitle("Plots the model visibilities from the model vsibility file or for a given map observed by the Ooty Radio Telescope.\n");
  optMain(plotmodel);
  opt(&argc,&argv);

  return plotmodel(argc, argv);
}
