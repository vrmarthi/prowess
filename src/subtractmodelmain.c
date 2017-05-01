#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"

char *FITSFile=NULL;
char *modelFile=NULL;
char Usage[256];
float threshold=0.0;
int BigEndian    = 0 ;
int LittleEndian = 0 ;

int subtractmodel(int argc, char **argv )
{
 int status = 0;
 static fitsfile *fin, *fout;
 char outfilename[80];
 ProjParType proj, mdlproj;
 int recnum = 0;
 long int gcount = 0;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 
 if( FITSFile == NULL || modelFile == NULL)
 { fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
   fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
   exit(-1);
 }

 if ( strstr(FITSFile, "FITS") != NULL)
 { strcpy(outfilename, FITSFile );
   strtok(outfilename, ".");
   strcat(outfilename, ".UVSUB.FITS");
   open_fits( &fin, FITSFile );
   proj.preint = 0;
   fill_proj(fin, &proj);
   create_fits( &fout, outfilename );
   init_hdr(fout, &proj);
 }

 if ( strstr(modelFile, "model") != NULL )
 { read_model_visibilities( &proj, modelFile );
 }

 while( recnum++ < proj.nrecs )
 { readFITS2Vis( fin, &proj );
   submodel( &proj, &proj );
   gcount = writeVis2FITS(fout, &proj);
   fprintf(finfo, "%s Model subtracted for [ %4d / %4d ] records\r", info, recnum, proj.nrecs);
 }
 fprintf(finfo, "%s Model subtracted for [ %4d / %4d ] records\n", info, recnum-1, proj.nrecs);

 fits_update_key_lng(fout, "GCOUNT", gcount, "number of random groups", &status);
 write_EXtabs( fout, &proj );

 close_fits( fin );
 close_fits( fout );

 kill_proj( &proj );

 done();
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile, OPT_STRING, 'i', "in" , "Input UVFITS File"             );
  optrega(&modelFile, OPT_STRING, 'm', "model" , "Model File"             );
  optrega(&threshold, OPT_FLOAT, 't', "thresh" , "Threshold flux density"             );
  optUsage(Usage);
  optTitle("Subtracts the provided model from the input UV FITS file.\n");
  optMain(subtractmodel);
  opt(&argc,&argv);

  return subtractmodel(argc, argv);
}
