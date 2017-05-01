#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"

char *FITSFile=NULL;
char Usage[256];
int BigEndian    = 0 ;
int LittleEndian = 0 ;

int FITS2model(int argc, char **argv )
{
 int status = 0;
 static fitsfile *fin, *fout;
 char modelfile[80];
 ProjParType proj;
 int recnum = 0;
 long int gcount = 0;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 
 if( FITSFile == NULL)
 { fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
   fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
   exit(-1);
 }

 if ( strstr(FITSFile, "FITS") != NULL)
 { strcpy(modelfile, FITSFile );
   strtok(modelfile, ".");
   strcat(modelfile, ".model");
   open_fits( &fin, FITSFile );
   proj.preint = 0;
   fill_proj(fin, &proj);
   proj.preint = proj.nrecs;
 }

 readFITS2Vis( fin, &proj );
 getmodel( &proj );
 write_model_visibilities( &proj, modelfile );

 close_fits( fin );

 kill_proj( &proj );

 done();
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile, OPT_STRING, 'i', "in" , "Input UVFITS File"             );
  optUsage(Usage);
  optTitle("Extracts the underlying model from the input UV FITS file.\n");
  optMain(FITS2model);
  opt(&argc,&argv);

  return FITS2model(argc, argv);
}
