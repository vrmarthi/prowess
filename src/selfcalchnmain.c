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
int nrecs, navg, nsrc, cavg;

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int selfcal(int argc, char **argv )
{
 int del_iter = 0;
 long int nsol, solno = 0;
 FILE *fsol;
 FILE *fMSE = fopen("MSE.dat", "w");
 FILE *fchisq = fopen("chisq.dat", "w");
 ProjParType inproj, outproj;
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

 inproj.nsrc = nsrc;
 inproj.preint = navg;

 ouproj.nsrc = inproj.nsrc;
 outproj.preint = inproj.preint;
 outproj.chans = inproj.chans / cavg;

 init_proj( &inproj );
 reproj( &outproj, &inproj );

 write_model_visibilities( &inproj );
 write_map2FITS( &inproj );
 nsol =  nrecs / navg ;
 fprintf(finfo, "%s Records to generate : %d\n", info, nrecs);
 fprintf(finfo, "%s Records to average  : %d\n", info, navg);
 fprintf(finfo, "%s Solution sets       : %ld\n", info, nsol);
 fprintf(finfo, "%s Max iter per solset : %d\n", info, MAXITER);
 fprintf(finfo, "%s Acquiring data ...\n", info);

 genVis( &inproj );
 reband( &outproj, &inproj );
 setup_itersol( &outproj );
 write_sol( &outproj, fsol);

 outproj.iter=0;
 while( solno++ < nsol )
 { del_iter = 0;
   genVis( &inproj );
   reband( &outproj, &inproj );
   compute_SNR( &outproj );
   while( ++del_iter <= MAXITER )
   { ++outproj.iter;
     iterate_solution( &outproj );
     compute_MSE( &outproj, fMSE );
     compute_chisq( &outproj, fchisq );
     update_solutions( &outproj );
     //write_sol( &proj, fsol );
     if( test_convergence( &outproj) ) break;
     fprintf(finfo, "%s Solset %5ld converged in %4d / %4d iterations\r", info, solno, del_iter, outproj.iter);
   }
   if(del_iter == MAXITER)
   fprintf(finfo, "\n%s Reached max. no. of iterations.\n", info); 
   else
   fprintf(finfo, "%s Solset %5ld converged in %4d / %4d iterations\r", info, solno, del_iter, outproj.iter);
   write_sol( &outproj, fsol );
 }
 
 fclose(fsol);
 fclose(fMSE);
 fclose(fchisq);

 write_baseline_SNR_matx( &outproj );
 compute_error_matrix( &outproj );
 kill_proj(&outproj);
 kill_proj(&inproj);

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
  optrega(&navg  , OPT_INT,    'c', "cavg",   "Channels to average" );
  optrega(&nsrc  , OPT_INT,    'n', "nsrc",   "Number of sources" );
  sprintf(Usage, "\n-o OutputSolFile \n-t Acquisition time in seconds\n-a Records to average \n-n No. of sources");
  optUsage(Usage);
  optTitle("Acquires visibilites and solves for antenna gain and true visbility phasors.\n");
  optMain(selfcal);
  opt(&argc,&argv);

  return selfcal(argc, argv);
}
