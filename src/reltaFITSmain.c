#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"

char *inFITSFile=NULL;
char *outFITSFile=NULL;
char Usage[256];
unsigned int lta;

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int reltaFITS(int argc, char **argv )
{
 int status = 0;
 static fitsfile *fvisin, *fvisout;
 long int nrecs;
 long recnum = 0;
 long gcount = 0;
 ProjParType proj;


 if( outFITSFile == NULL || inFITSFile == NULL || lta < 1 )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 open_fits( &fvisin, inFITSFile );
 create_fits( &fvisout, outFITSFile );
 proj.preint = lta;
 nrecs = fill_proj( fvisin, &proj );
 init_hdr( fvisout, &proj );
 nrecs = abs(nrecs/lta)*lta;

 while( recnum < nrecs )
 { recnum = readFITS2Vis( fvisin, &proj );
   gcount = writeVis2FITS(fvisout, &proj);
   fprintf(finfo, "%s ([ %ld / %ld ] recs read, [ %ld / %ld ] recs) written\r", 
	   info, recnum, nrecs, recnum/lta, nrecs/lta); 
 }

 fits_update_key_lng(fvisout, "GCOUNT", gcount, "number of random groups", &status);
 write_EXtabs( fvisout, &proj );
 kill_proj( &proj );
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
  optrega(&lta        , OPT_UINT,   'l', "lta", "LTA factor" );
  sprintf(Usage, "-i InputFITSFile -o OutputFITSFile -l LTA Factor");
  optUsage(Usage);
  optTitle("Modifies the LTA of the recorded FITS file by factor l(>1).\n");
  optMain(reltaFITS);
  opt(&argc,&argv);

  return reltaFITS(argc, argv);
}
