#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"

#define MAXITER 1000

char Usage[256];
char *solfile = NULL;
int preint;

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int itergain(int argc, char **argv )
{
 FILE *fsol;
 FILE *fMSE = fopen("MSE.dat", "w");
 FILE *fchisq = fopen("chisq.dat", "w");
 ProjParType proj;
 PlotParType plot;
 char modelfile[80] = "vis.model";

 if( solfile == NULL || preint < 0 )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 proj.nsrc = -1;
 proj.preint = preint;
 init_proj(&proj); 
 init_sky_signal( &proj );
 sky_to_model( &proj );
 write_model_visibilities( &proj, modelfile );
 write_maps2FITS( &proj );

 fprintf(finfo, "%s # of recs averaged  : %d\n", info, preint);
 fprintf(finfo, "%s Max # of iterations : %d\n", info, MAXITER);
 
 strcat(solfile, ".");
 strcat(solfile, argv[0]);
 fsol = fopen(solfile, "w");

 fprintf(finfo, "%s Acquiring data ...\n", info);
 genVis( &proj );
 compute_SNR( &proj );
 setup_itersol( &proj );
 force_model( &proj );
 write_sol( &proj, fsol);

 proj.iter = 0;
 proj.chan2cal = proj.chans / 2;
 while( proj.iter++ < MAXITER )
 { iterate_solution( &proj );
   force_model( &proj );
   compute_MSE( &proj, fMSE );
   compute_chisq( &proj, fchisq );
   update_solutions( &proj );
   if( test_convergence( &proj) ) break;
   fprintf(finfo, "%s [ %d / %d ] iterations done\r", info, proj.iter, MAXITER);
   write_sol( &proj, fsol );
 }
 if(--proj.iter == MAXITER)
 fprintf(finfo, "\n%s Reached max. no. of iterations.", info);
 else
 fprintf(finfo, "\n%s Solution converged in %d iterations.", info, proj.iter);

 fclose(fsol);
 fclose (fMSE);
 fclose(fchisq);

 write_baseline_SNR_matx( &proj );
 compute_error_matrix ( &proj );

 compute_CRLB( &proj );
 kill_proj(&proj);

#ifdef _PLOT
 plot.driver = 'X';
 fprintf(finfo, "\n%s Begin plots...\n", info);
 xsplot_all( &plot, solfile );
#endif

 done();

 return 0;
}

int main(int argc, char **argv )
{
  optrega(&solfile, OPT_STRING, 'o', "out"   , "Output Solution File" );
  optrega(&preint , OPT_INT,    't', "preint", "Acquisition time in seconds" );
  sprintf(Usage, "-o OutputSolFile -t Acquisition time in seconds");
  optUsage(Usage);
  optTitle("Observes a point sources at phase centre of the primary beam and solves for antenna gain phasors.\n");
  optMain(itergain);
  opt(&argc,&argv);

  return itergain(argc, argv);
}
