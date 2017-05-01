#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"

#define DELIM ' '

int infiles = 0;
char **inFITSFile=NULL;
char *outFITSFile=NULL;
char Usage[256];
char delim=DELIM;

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int FITScat(int argc, char **argv )
{
 int status = 0;
 fitsfile *fvisin, *fvisout;
 long int nrecs;
 long recnum = 0;
 long gcount = 0;
 ProjParType proj;
 int i=0;
 char inFITSname[64];
 int scan;

 if( outFITSFile == NULL || inFITSFile == NULL)
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 fprintf(finfo, "%s %d files to read\n", info, infiles);
 create_fits( &fvisout, outFITSFile );
 proj.preint = 1;

 for( i = 0 ; i < infiles ; i++)
 { fprintf(finfo, "%s File %d : %s\n", info, i+1, inFITSFile[i]);
   strcpy( inFITSname, inFITSFile[i] );
   open_fits( &fvisin, inFITSname );
   filesize(inFITSname);
   fill_proj( fvisin, &proj );
   scan = update_scaninfo( &proj );
   if(i == 0)  init_hdr( fvisout, &proj );
   for( nrecs = 0 ; nrecs < proj.nrecs; nrecs++ )
   { readFITS2Vis( fvisin, &proj );
     update_sid( &proj, scan );
     gcount = writeVis2FITS(fvisout, &proj);
     fprintf(finfo, "%s [%5ld / %5d] recs copied from %s\r", info, nrecs+1, proj.nrecs, inFITSname);
   }
   newline();
   close_fits( fvisin );
 }

 fits_update_key_lng(fvisout, "GCOUNT", gcount, "number of random groups", &status);
 write_EXtabs( fvisout, &proj );

 fits_rewind( fvisout, &proj );
 fill_proj( fvisout, &proj );

 kill_proj( &proj );
 close_fits( fvisout );
 done();

 filesize(outFITSFile);

 return 0;

}

int main(int argc, char **argv )
{
  optrega( &outFITSFile, OPT_STRING, 'o', "out" , "Output FITS File" );
  optrega_array( &infiles, &inFITSFile, OPT_STRING, 'i', "in", "Input FITS File(s)");
  sprintf(Usage, "-i InputFITSFiles separated by commas -o OutputFITSFile");
  optUsage(Usage);
  optTitle("Concatenates many FITS files.\n");
  optMain(FITScat);
  opt(&argc,&argv);

  return FITScat(argc, argv);
}
