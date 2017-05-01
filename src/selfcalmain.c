#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

#define MAXITER 1000

char Usage[256];
char *solfile = NULL;
char *modelFile = NULL;
//char method = 'u';
int nrecs=1, navg=1, nsrc = -1;

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int selfcal(int argc, char **argv )
{
 int del_iter = 0;
 long int nsol, solno = 0;
 FILE *fsol;
 FILE *fMSE = fopen("MSE.dat", "w");
 FILE *fchisq = fopen("chisq.dat", "w");
 ProjParType proj;
 PlotParType plot;

 if( solfile == NULL || nrecs < 0 )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 
 strcat(solfile, ".");
 strcat(solfile, argv[0]);
 fsol = fopen(solfile, "w");
 
 proj.nsrc = nsrc;
 proj.preint = navg;

 init_proj( &proj );
 init_sky_signal( &proj );
 sky_to_model( &proj );
 write_model_visibilities( &proj, modelFile );
 write_maps2FITS( &proj );
 nsol =  nrecs / navg ;
 fprintf(finfo, "%s Records to generate : %d\n", info, nrecs);
 fprintf(finfo, "%s Records to average  : %d\n", info, navg);
 fprintf(finfo, "%s Solution sets       : %ld\n", info, nsol);
 fprintf(finfo, "%s Max iter per solset : %d\n", info, MAXITER);
 fprintf(finfo, "%s Acquiring data ...\n", info);

 genVis( &proj );
 setup_itersol( &proj );
 if( modelFile != NULL )
 { //read_model_visibilities( &proj, modelFile );
   force_model( &proj );
 }
 write_sol( &proj, fsol);

 proj.iter=0;
 proj.chan2cal = proj.chans / 2;
 while( solno++ < nsol )
 { del_iter = 0;
   genVis( &proj );
   compute_SNR( &proj );
   while( ++del_iter <= MAXITER )
   { ++proj.iter;
     iterate_solution( &proj );
     if( modelFile != NULL || solno > 1) force_model( &proj );
     compute_MSE( &proj, fMSE );
     compute_chisq( &proj, fchisq );
     update_solutions( &proj );
     //write_sol( &proj, fsol );
     if( test_convergence( &proj) ) break;
     fprintf(finfo, "%s Solset %5ld converged in %4d / %4d iterations\r", info, solno, del_iter, proj.iter);
   }
   if( modelFile != NULL || solno == 1) apply_model( &proj );
   if(del_iter == MAXITER)
   fprintf(finfo, "\n%s Reached max. no. of iterations.\n", info); 
   else
   fprintf(finfo, "%s Solset %5ld converged in %4d / %4d iterations\r", info, solno, del_iter, proj.iter);
   write_sol( &proj, fsol );
 }
 
 fclose(fsol);
 fclose(fMSE);
 fclose(fchisq);

 write_baseline_SNR_matx( &proj );
 compute_error_matrix( &proj );
 compute_CRLB( &proj );
 kill_proj(&proj);

#ifdef _PLOT
 plot.driver = 'X';
 fprintf(finfo, "\n%s Begin plots ...\n", info);
 xsplot_all( &plot, solfile );
#endif

 done();

 return 0;
}

int main(int argc, char **argv )
{
  optrega(&solfile , OPT_STRING, 'o', "out"   , "Output Solution File" );
  optrega(&nrecs  , OPT_INT,    't', "acq",    "Acquisition time in seconds" );
  optrega(&navg  , OPT_INT,    'a', "navg",   "Records to average" );
  optrega(&nsrc  , OPT_INT,    'n', "nsrc",   "Number of sources" );
  optrega(&modelFile , OPT_STRING,   'm', "model", "model visibility file");
  sprintf(Usage, "\n-o OutputSolFile \n-t Acquisition time in seconds\n-a Records to average \n-n No. of sources \n-m model visibility file");
  optUsage(Usage);
  optTitle("Acquires visibilites and solves for antenna gain and true visbility phasors.\n");
  optMain(selfcal);
  opt(&argc,&argv);

  return selfcal(argc, argv);
}
