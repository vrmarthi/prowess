#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"

char *inFITSFile=NULL;
char *outFITSFile=NULL;
char Usage[256];

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int modsu(int argc, char **argv )
{
 int status = 0;
 fitsfile *fvisin, *fvisout;
 long int nrecs;
 long recnum = 0;
 long gcount = 0, bigcount = 0;
 ProjParType proj;
 int bl=0;
 char inFITSname[64];
 RGParType *uvw;

 if(inFITSFile == NULL)
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 //create_fits( &fvisout, outFITSFile );
 proj.preint = 1;

 open_fits( &fvisin, inFITSFile );
 fill_proj( fvisin, &proj );
 for( nrecs = 0 ; nrecs < proj.nrecs; nrecs++ )
 { readFITS2Vis( fvisin, &proj );
   for(bl = 0 ; bl < proj.baselines ; bl++)
   { uvw = (RGParType *) (proj.RecBuf + bl*(proj.recwords) );
     uvw->su = 1.0;
   }
   gcount = writeVis2FITS(fvisin, &proj);
   fprintf(finfo, "%s [%5d / %5d] recs modified in %s\r", info, nrecs+1, proj.nrecs, inFITSFile);
   }
 newline();
 close_fits( fvisin );
 

//fits_update_key_lng(fvisout, "GCOUNT", bigcount, "number of random groups", &status);
//write_EXtabs( fvisout, &proj );
 kill_proj( &proj );
 //close_fits( fvisout );
 done();
 filesize(inFITSFile);
 //filesize(outFITSFile);

 return 0;

}

int main(int argc, char **argv )
{
    //optrega( &outFITSFile, OPT_STRING, 'o', "out" , "Output FITS File" );
  optrega( &inFITSFile, OPT_STRING, 'i', "in", "Input FITS File");
  sprintf(Usage, "-i InputFITSFile");
  optUsage(Usage);
  optTitle("MOdifies source ID in FITS files.\n");
  optMain(modsu);
  opt(&argc,&argv);

  return modsu(argc, argv);
}
