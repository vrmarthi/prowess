#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"

char *FITSFile=NULL;
char *offsetFITSFile=NULL;
char Usage[256];

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int subtractrecord(int argc, char **argv )
{
 int status = 0;
 static fitsfile *fin, *fout, *fsub;
 char outfilename[80];
 ProjParType proj, mdlproj;
 int recnum = 0;
 long int gcount = 0;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 
 if( FITSFile == NULL || offsetFITSFile == NULL)
 { fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
   fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
   exit(-1);
 }

 if ( strstr(FITSFile, "FITS") != NULL && strstr(offsetFITSFile, "FITS") != NULL)
 { strcpy(outfilename, FITSFile );
   strtok(outfilename, ".");
   strcat(outfilename, ".SUB.FITS");
   open_fits( &fin, FITSFile );
   open_fits( &fsub, offsetFITSFile );
   proj.preint = 0;
   fill_proj(fin, &proj);
   fill_proj(fsub, &mdlproj);
   create_fits( &fout, outfilename );
   init_hdr(fout, &proj);
 }

 while( recnum++ < proj.nrecs )
 { readFITS2Vis( fin, &proj );
   readFITS2Vis( fsub, &mdlproj );
   subrec( &proj, &mdlproj );
   gcount = writeVis2FITS(fout, &proj);
   fprintf(finfo, "%s Model subtracted for [ %4d / %4d ] records\r", info, recnum, proj.nrecs);
 }
 fprintf(finfo, "%s Model subtracted for [ %4d / %4d ] records\n", info, recnum-1, proj.nrecs);

 fits_update_key_lng(fout, "GCOUNT", gcount, "number of random groups", &status);
 write_EXtabs( fout, &proj );

 close_fits( fin );
 close_fits( fsub );
 close_fits( fout );

 kill_proj( &proj );
 kill_proj( &mdlproj );

 done();
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile, OPT_STRING, 'i', "in" , "Input UVFITS File"             );
  optrega(&offsetFITSFile, OPT_STRING, 's', "sub" , "UVFITS File to subtract"             );
  sprintf(Usage, "\n-i InputUVFITSFile -s OFfsetUVFITSFile");
  optUsage(Usage);
  optTitle("Subtracts the provided model from the input UV FITS file.\n");
  optMain(subtractrecord);
  opt(&argc,&argv);

  return subtractrecord(argc, argv);
}
