#include <stdio.h>

#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"

char *inFITSFile=NULL;
char *outFITSFile=NULL;
char Usage[256];
unsigned int navg;

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int rechnFITS(int argc, char **argv )
{
 int preint = 1;
 static fitsfile *fvisin, *fvisout;
 long int nrecs;
 int status=0;
 long recnum = 0;
 long  gcount = 0;
 ProjParType inproj, outproj;

 if( outFITSFile == NULL || inFITSFile == NULL || navg < 1 )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 open_fits( &fvisin, inFITSFile );
 create_fits( &fvisout, outFITSFile );

 inproj.preint = preint;
 outproj.preint = preint;

 nrecs = fill_proj( fvisin, &inproj );
 outproj.chans = inproj.chans / navg ;
 reproj( &outproj, &inproj );
 init_hdr( fvisout, &outproj );

 while( recnum < nrecs )
 { recnum = readFITS2Vis( fvisin, &inproj );
   reband( &outproj, &inproj );
   gcount = writeVis2FITS(fvisout, &outproj);
   fprintf(finfo, "%s ([ %ld / %ld ] recs done\r", info, recnum, nrecs);
 }

 fits_update_key_lng(fvisout, "GCOUNT", gcount, "number of random groups", &status);
 write_EXtabs( fvisout, &outproj  );
 kill_proj( &inproj );
 kill_proj( &outproj );
 close_fits( fvisin );
 close_fits( fvisout );
 newline();
 done();
 filesize(inFITSFile);
 filesize(outFITSFile);
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&inFITSFile,  OPT_STRING, 'i', "in" , "Input FITS File" );
  optrega(&outFITSFile, OPT_STRING, 'o', "out" , "Output FITS File" );
  optrega(&navg        , OPT_UINT,  'n', "navg", "Chans to collapse" );
  sprintf(Usage, "-i InputFITSFile -o OutputFITSFile -n Chans to collapse");
  optUsage(Usage);
  optTitle("Modifies the LTA of the recorded FITS file by factor l(>1).\n");
  optMain(rechnFITS);
  opt(&argc,&argv);

  return rechnFITS(argc, argv);
}
