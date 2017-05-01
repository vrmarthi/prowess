#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"

char *FITSFile=NULL;
char PBEAMFile[80] = "PRBEAM.FITS";
int nsrc=-2;
char Usage[256];
int galfg_swtch = OFF;
int pntsrc_swtch = OFF;
float threshold = 0.0;

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int mkmodel(int argc, char **argv )
{
int status = 0;
static fitsfile *fmap;
char outfilename[80];
char pbeamfilename[80];
ProjParType proj;
FILE *fmodel;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 
 if( FITSFile == NULL )
 { fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
   fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
   exit(-1);
 }

 if ( strstr(FITSFile, "FITS") != NULL)
 { strcpy(outfilename, FITSFile );
   strtok(outfilename, ".");
   strcat(outfilename, ".model");
   proj.preint = 0;
   init_proj(&proj); 
   strcpy(pbeamfilename, PBEAMFile);
   read_FITSmap( &proj, pbeamfilename);
   init_primary_beam( &proj );
   copy_skymap_to_pbeam( &proj );
   newline();
   if(read_FITSmap( &proj, FITSFile) )
   { fprintf(ferr, "%s %s is likely a UVFITS file \n", err, FITSFile);
     fprintf(ferr, "%s Please supply a FITS map\n", info);
     kill_proj( &proj );
     done();
     return 0;
   }
   newline();
   proj.threshold = threshold;
   look_through_primary_beam( &proj );
   sky_to_model( &proj );
   newline();
   write_model_visibilities(&proj, outfilename);
 }
 
 else if ( strstr(FITSFile, "cat") != NULL )
 { strcpy(outfilename, FITSFile );
   strtok(outfilename, ".");
   strcat(outfilename, ".model");
   init_proj(&proj); 
   strcpy(pbeamfilename, PBEAMFile);
   read_FITSmap( &proj, pbeamfilename);
   init_primary_beam( &proj );
   copy_skymap_to_pbeam( &proj );
   newline();
   catalog_to_sky( &proj, FITSFile);
   write_maps2FITS( &proj );
   proj.threshold = threshold;
   sky_to_model( &proj );
   write_model_visibilities( &proj, outfilename );
 }


 kill_proj( &proj );

 done();
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile, OPT_STRING, 'i', "in" , "Input FITS map or source catalog"             );
  optrega(&PBEAMFile, OPT_STRING, 'p', "pbeam" , "Input PBEAM FITS map"             );
  optrega(&threshold, OPT_FLOAT, 't', "thresh" , "threshold(mJy)"             );
  sprintf(Usage, "\n-i InputFITSmap/catalog -p [Primary Beam FITS File] -t [threshold(mJy)]");
  optUsage(Usage);
  optTitle("Generates model visibilities for a given map or for a source catalog as observed by the Ooty Radio Telescope.\n");
  optMain(mkmodel);
  opt(&argc,&argv);

  return mkmodel(argc, argv);
}
