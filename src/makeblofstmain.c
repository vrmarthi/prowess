#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"

char *FITSFile=NULL;
int nsrc=-2;
char Usage[256];

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int mkblofst(int argc, char **argv )
{
int status = 0;
float frac_tsys = 0.0;
ProjParType proj;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 
 init_proj( &proj );
 make_blofst_lookup_table( &proj, frac_tsys);

 if ( strstr(FITSFile, "FITS") != NULL)
 { strcpy(outfilename, FITSFile );
   strtok(outfilename, ".");
   strcat(outfilename, ".model");
   proj.preint = 0;
   init_proj(&proj); 
   read_FITSmap( &proj, "PRBEAM.FITS");
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
   catalog_to_sky( &proj, FITSFile);
   write_maps2FITS( &proj );
   sky_to_model( &proj );
   write_model_visibilities( &proj, outfilename );
 }


 kill_proj( &proj );

 done();
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&frac_tsys, OPT_FLOAT, 'f', "offset" , "Offset as a fraction of T_sys");
  sprintf(Usage, "\n-f offset");
  optUsage(Usage);
  optTitle("Generates a look up table for baseline based offsets\n");
  optMain(mkblofst);
  opt(&argc,&argv);

  return mkblofst(argc, argv);
}
