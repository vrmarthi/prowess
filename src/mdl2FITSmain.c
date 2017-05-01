#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"

char *FITSFile=NULL;
char Usage[256];
long recs = 0;
char *modelFile=NULL;
int BigEndian    = 0 ;
int LittleEndian = 0 ;

int mdl2FITS(int argc, char **argv )
{
int preint = 1;
int status = 0;
static fitsfile *fvis;
long recnum = 0;
long  gcount = 0;
ProjParType proj;
//char modelfile[80] = "vis.model";

if( FITSFile == NULL || modelFile == NULL)
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 init_proj(&proj); 
 read_model_visibilities( &proj, modelFile );

 create_fits( &fvis, FITSFile );
 init_hdr(fvis, &proj);

 while( recnum < recs )
 { recnum = genVis( &proj );
   compute_SNR( &proj );
   gcount = writeVis2FITS(fvis, &proj);
   if( recnum%10 == 0) fprintf(finfo, "%s ([ %ld / %ld ] recs, [ %ld / %ld ] rgps) written\r", info, recnum, recs, gcount, recs*proj.baselines); 
 }
 fprintf(finfo, "%s ([ %ld / %ld ] recs, [ %ld / %ld ] rgps) written\r", 
	   info, recnum, recs, gcount, recs*proj.baselines); 
 fits_update_key_lng(fvis, "GCOUNT", gcount, "number of random groups", &status);
 write_EXtabs( fvis, &proj );

 close_fits( fvis );

 kill_proj( &proj );
 newline();
 done();
 filesize(FITSFile);


 return 0;

}

int main(int argc, char **argv )
{
  optrega(&modelFile, OPT_STRING, 'i', "in" , "Input Model File"             );
  optrega(&FITSFile, OPT_STRING, 'o', "out" , "Output FITS File"             );
  optrega(&recs    , OPT_ULONG,  't', "time", "Observing duration in seconds");
  sprintf(Usage, "\n-i Input model visibility file \n-o OutputFITSFile \n-t ObservingDuration_Seconds");
  optUsage(Usage);
  optTitle("Generates UV FITS for a given radio sky observed with the Ooty Radio Telescope.\n");
  optMain(mdl2FITS);
  opt(&argc,&argv);

  return mdl2FITS(argc, argv);
}
