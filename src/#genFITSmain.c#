#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"

char *FITSFile=NULL;
char Usage[256];
long recs = 0;
int nsrc = 0;
int coupling = OFF;
int galfg_swtch = OFF;
int pntsrc_swtch = OFF;
int pbeam_swtch = OFF;
int h1_swtch = OFF;
int BigEndian    = 0 ;
int LittleEndian = 0 ;
char *EPSmap=NULL;
char *HImap=NULL;

int genFITS(int argc, char **argv )
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

 proj.nsrc = nsrc;
 proj.preint = preint;
 init_proj(&proj); 

 /*
 if( strstr(EPSmap,"/0") == NULL ) pntsrc_swtch = OFF;
 else
 { pntsrc_swtch = ON;
   strcpy(proj.eps.filename, EPSmap);
 }
 */
 if( EPSmap == NULL ) pntsrc_swtch = OFF;
 else 
 { pntsrc_swtch = ON;
   strcpy(proj.eps.filename, EPSmap);
 }

 if( HImap == NULL ) h1_swtch = OFF;
 else 
 { h1_swtch = ON;
   strcpy(proj.HI.filename, HImap);
 }

 if( proj.nsrc > -1 && ! galfg_swtch && ! pntsrc_swtch )
 { fprintf(ferr, "%s Specify at least one switch -p or -g\n", err); exit(-1);}

 proj.galfg_swtch = galfg_swtch;
 proj.pntsrc_swtch = pntsrc_swtch;
 proj.pbeam_swtch = pbeam_swtch;
 proj.h1_swtch = h1_swtch;

 init_sky_signal( &proj );
 write_maps2FITS( &proj );
 sky_to_model( &proj );
 //if(coupling) coupling_affected_model( &proj );
 //newline();
 write_model_visibilities( &proj, modelfile );

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
 free_sky_signal( &proj );
 kill_proj( &proj );
 newline();
 done();
 filesize(FITSFile);


 return 0;

}

int main(int argc, char **argv )
{
  optrega(&FITSFile, OPT_STRING, 'o', "out" , "Output FITS File"             );
  optrega(&recs    , OPT_ULONG,  't', "time", "Observing duration in seconds");
  optrega(&nsrc    , OPT_INT,   'n', "sources", "no. of sources");
  optrega(&galfg_swtch, OPT_BOOL, 'g', "galfg", "Gal FG only [ default : true ]");
  optrega(&pbeam_swtch, OPT_BOOL, 'b', "pbeam", "Input Primary beam file: default on the fly");
  optrega(&coupling, OPT_BOOL, 'c', "cplg", "switch on EM coupling; [default : OFF]");
  optrega(&EPSmap, OPT_STRING, 'p', "pntsrc", "Point source only [ default : true ]");
  optrega(&HImap, OPT_STRING, 'h', "HImap", "HI cube [ default : true ]");
  sprintf(Usage, "\n-o OutputFITSFile \n-t ObservingDuration_Seconds \n-n no. of sources\n-p point sources only\n-g Galactic foreground only -h HI Cube FITS -b read primary beam cube -c coupling switch");
  optUsage(Usage);
  optTitle("Generates UV FITS for a given radio sky observed with the Ooty Radio Telescope.\n");
  optMain(genFITS);
  opt(&argc,&argv);

  return genFITS(argc, argv);
}
