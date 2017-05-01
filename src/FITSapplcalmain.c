#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"
#include "../inc/plotdefs.h"
#define MAXITER 1000

char *inFITSFile=NULL;
char *outFITSFile = NULL;
char Usage[256];
int preint = 1;
char *modelFile = NULL;
int BigEndian    = 0 ;
int LittleEndian = 0 ;

int FITSapplcal(int argc, char **argv )
{
 int status = 0;
 int del_iter = 0;
 long int nrecs, nsol, solno = 0, gcount;
 fitsfile *fin, *fout;
 FILE *fMSE = fopen("MSE.dat", "w");
 FILE *fchisq = fopen("chisq.dat", "w");
 ProjParType proj;

 if( inFITSFile == NULL || outFITSFile == NULL || preint < 1 )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 open_fits( &fin, inFITSFile );
 proj.preint = preint;
 nrecs =  fill_proj(fin, &proj);

 create_fits( &fout, outFITSFile );
 init_hdr(fout, &proj);

 nsol = nrecs / preint ;
 fprintf(finfo, "%s # of recs averaged  : %d\n", info, preint);
 fprintf(finfo, "%s # of solution sets  : %ld\n", info, nsol);

 setup_itersol( &proj );
 if( modelFile != NULL )
 { read_model_visibilities( &proj, modelFile );
   force_model( &proj );
 }

 init_gain_table( &proj );
 init_bandpass_table( &proj );

 while( solno++ < nsol )
 { readFITS2Vis( fin, &proj );
   proj.iter = 0;
   for( proj.chan2cal = 0; proj.chan2cal < proj.chans ; proj.chan2cal++)
   { del_iter = 0;
     reinit_itersol( &proj );
     while( ++del_iter <= MAXITER )
     { ++proj.iter;
       iterate_solution( &proj );
       if( modelFile != NULL ) force_model( &proj );
       compute_MSE( &proj, fMSE );
       compute_chisq( &proj, fchisq );
       update_solutions( &proj );
       if( test_convergence( &proj) ) break;
     }
     if(del_iter == MAXITER) fprintf(finfo, "\n%s Reached max. no. of iterations.\n", info); 
     if(proj.chan2cal == 0) write_gaintab( &proj );
     write_bpasstab( &proj );
     fprintf(finfo, "%s Channel [ %d / %d ] converged in %d iterations\r", info, proj.chan2cal+1, proj.chans, proj.iter);
    }
   fprintf(finfo, "%s Solset %5ld converged in %4.1f iterations/channel\n", info, solno, (float)proj.iter/(float)proj.chans);
   apply_solutions( &proj );
   
   gcount = writeVis2FITS( fout, &proj );
 }

 fits_update_key_lng(fout, "GCOUNT", gcount, "number of random groups", &status);
 write_EXtabs( fout, &proj );

 close_fits (fin);
 close_fits (fout);

 fclose(fMSE);
 fclose(fchisq);

 kill_proj(&proj);
 done();
 filesize(inFITSFile);
 filesize(outFITSFile);
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&inFITSFile, OPT_STRING, 'i', "in"    , "Input FITS File");
  optrega(&outFITSFile , OPT_STRING, 'o', "out"   , "Output FITS File" );
  optrega(&preint  , OPT_INT,    't', "preint", "Pre-integration in seconds >= 1" );
  optrega(&modelFile , OPT_STRING,   'm', "model", "model file for the visibilities");
  sprintf(Usage, "\n-i InputFITSFile \n-o OutputFITSFile \n-t Pre-integration in seconds >= 1\n-m model visibility file\n");
  optUsage(Usage);
  optTitle("Reads UVFITS visibilites and solves for antenna gain and true visbility phasors.\n Writes the calibrated visibilities into FITS file\n");
  optMain(FITSapplcal);
  opt(&argc,&argv);

  return FITSapplcal(argc, argv);
}
