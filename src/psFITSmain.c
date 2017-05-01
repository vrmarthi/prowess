#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"

char *FITSFile=NULL;
char Usage[256];
char Title[1024];
long recs = 0 ;

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int psFITS(int argc, char **argv )
{
int preint = 1;
int status = 0;
static fitsfile *fvis;
long recnum = 0;
long  gcount = 0;
ProjParType proj;
char modelfile[80] = "vis.model";

if( FITSFile == NULL || recs < 0 )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 proj.nsrc = -1;
 proj.preint = preint;
 init_proj(&proj); 

 create_fits( &fvis, FITSFile );
 init_hdr(fvis, &proj);
 write_model_visibilities( &proj, modelfile );

 while( recnum < recs )
 { recnum = genVis( &proj );
   gcount = writeVis2FITS(fvis, &proj);
   fprintf(finfo, "%s ([ %ld / %ld ] recs, [ %ld / %ld ] rgps) written\r", 
	   info, recnum, recs, gcount, recs*proj.baselines); 
 }

 fits_update_key_lng(fvis, "GCOUNT", gcount, "number of random groups", &status);
 write_EXtabs( fvis, &proj );

 kill_proj( &proj );
 close_fits( fvis );
 done();
 filesize(FITSFile);
 return 0;

}

int main(int argc, char **argv )
{
  optrega(&FITSFile, OPT_STRING, 'o', "out" , "Output FITS File"             );
  optrega(&recs    , OPT_ULONG,  't', "time", "Observing duration in seconds");
  sprintf(Usage, "\n-o OutputFITSFile -t ObservingDuration_Seconds");
  optUsage(Usage);
  strcat(Title, "\nGenerates UVFITS for a flux calibrator from the VLA Cal Manual,\n");
  strcat(Title, "observed with the ORT at the phase centre of the primary beam.\n");
  strcat(Title, "Does not write a sky map.\n\n");
  optTitle(Title);
  optMain(psFITS);
  opt(&argc,&argv);

  return psFITS(argc, argv);
}
