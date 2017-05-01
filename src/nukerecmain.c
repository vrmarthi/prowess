#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"

char *inFITSFile=NULL;
char *outFITSFile=NULL;
char Usage[256];
unsigned int start, stop;

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int nukerecs(int argc, char **argv )
{
 int status = 0;
 static fitsfile *fvisin, *fvisout;
 long int nrecs;
 long recnum = 0;
 long gcount = 0;
 ProjParType proj;
 int i;

 if( outFITSFile == NULL || inFITSFile == NULL)
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 open_fits( &fvisin, inFITSFile );
 create_fits( &fvisout, outFITSFile );
 proj.preint = 1;
 nrecs = fill_proj( fvisin, &proj );
 init_hdr( fvisout, &proj );

 for ( i = 1 ; i < start ; i++ )
 { readFITS2Vis( fvisin, &proj );
   gcount = writeVis2FITS(fvisout, &proj);
 }

 for( i = start ; i <= stop ; i++ ) readFITS2Vis( fvisin, &proj );

 for ( i = stop+1 ; i <= nrecs ; i++ )
 { readFITS2Vis( fvisin, &proj );
   gcount = writeVis2FITS(fvisout, &proj);
 }
 
 fits_update_key_lng(fvisout, "GCOUNT", gcount, "number of random groups", &status);
 write_EXtabs( fvisout, &proj );
 kill_proj( &proj );
 close_fits( fvisin );
 close_fits( fvisout );
 done();
 filesize(inFITSFile);
 filesize(outFITSFile);

 return 0;

}

int main(int argc, char **argv )
{
  optrega(&inFITSFile,  OPT_STRING, 'i', "in" , "Input FITS File" );
  optrega(&outFITSFile, OPT_STRING, 'o', "out" , "Output FITS File" );
  optrega(&start        , OPT_UINT,   't', "start", "start record" );
  optrega(&stop        , OPT_UINT,   'p', "stop", "stop record" );
  sprintf(Usage, "-i InputFITSFile -o OutputFITSFile -t start -p stop");
  optUsage(Usage);
  optTitle("Strips out records from a FITS file.\n");
  optMain(nukerecs);
  opt(&argc,&argv);

  return nukerecs(argc, argv);
}
