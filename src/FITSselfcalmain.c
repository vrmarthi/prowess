#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"
#include "../inc/plotdefs.h"
#define MAXITER 1000

char *FITSFile=NULL;
char Usage[256];
char *solfile = NULL;
char *modelFile = NULL;
int preint = 1;

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int FITSselfcal(int argc, char **argv )
{
 int del_iter = 0;
 long int nrecs, nsol, solno = 0;
 static  fitsfile *fvis;
 FILE *fsol;
 FILE *fMSE = fopen("MSE.dat", "w");
 FILE *fchisq = fopen("chisq.dat", "w");
 ProjParType proj;
 PlotParType plot;
 char filename[80];

 if( FITSFile == NULL || solfile == NULL || preint < 1 )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 strcpy(filename, solfile);
 strcat(filename, ".");
 strcat(filename, argv[0]);
 fsol = fopen(filename, "w");

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 open_fits( &fvis, FITSFile );

 proj.preint = preint;
 nrecs =  fill_proj(fvis, &proj);
 nsol = nrecs / preint ;
 fprintf(finfo, "%s # of recs averaged  : %d\n", info, preint);
 fprintf(finfo, "%s # of solution sets  : %ld\n", info, nsol);
 fprintf(finfo, "%s Max iter per solset : %d\n", info, MAXITER);

 setup_itersol( &proj );
 /* if( modelFile != NULL )
 { read_model_visibilities( &proj, modelFile );
   force_model( &proj );
 }
 */
 write_sol( &proj, fsol);

 proj.iter = 0;
 proj.chan2cal = proj.chans / 2;
 while( solno++ < nsol )
 { readFITS2Vis( fvis, &proj );
   compute_SNR( &proj );
   del_iter = 0;
#ifndef _NO_VERBOSE
     find_load(START_COUNT);
#endif
   while( ++del_iter <= MAXITER )
   { ++proj.iter;
     iterate_solution( &proj );
     //if( modelFile != NULL || solno > 1) force_model( &proj );
     compute_MSE( &proj, fMSE );
     compute_chisq( &proj, fchisq );
     update_solutions( &proj );
     if( test_convergence( &proj) ) break;
     fprintf(finfo, "%s Solset %5ld converged in %4d / %4d iterations\r", info, solno, del_iter, proj.iter);
   }
   //if( modelFile != NULL || solno == 1) apply_model( &proj );
#ifndef _NO_VERBOSE
     find_load(STOP_COUNT);
#endif
   if(del_iter == MAXITER)
   fprintf(finfo, "\n%s Reached max. no. of iterations.\n", info); 
   else
   fprintf(finfo, "%s Solset %5ld converged in %4d / %4d iterations\r", info, solno, del_iter, proj.iter);
   write_sol( &proj, fsol );
 }

 close_fits (fvis);
 fclose(fsol);
 fclose(fMSE);
 fclose(fchisq);

 write_baseline_SNR_matx( &proj );
 compute_error_matrix( &proj );
 kill_proj(&proj);
 
#ifdef _PLOT
 plot.driver = 'X';
 fprintf(finfo, "\n%s Begin plots...\n", info);
 xsplot_all( &plot, filename ); 
#endif

 done();
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile, OPT_STRING, 'i', "in"    , "Input FITS File");
  optrega(&solfile , OPT_STRING, 'o', "out"   , "Output Solution File" );
  optrega(&modelFile , OPT_STRING,   'm', "model", "model file for the visibilities");
  optrega(&preint  , OPT_INT,    't', "preint", "Acquisition time in seconds" );
  sprintf(Usage, "\n-i InputFITSFile \n-o OutputSolFile \n-t Pre-integration in seconds\n-m Model visibility file");
  optUsage(Usage);
  optTitle("Reads UVFITS visibilites and solves for antenna gain and true visbility phasors.\n");
  optMain(FITSselfcal);
  opt(&argc,&argv);

  return FITSselfcal(argc, argv);
}
