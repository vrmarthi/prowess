#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

#define MAXITER 1000

char *FITSFile=NULL;
char Usage[256];
char *solfile = NULL;
char *modelFile = NULL;
int  preint;

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int FITSgain(int argc, char **argv )
{
 long int nrecs;
 static  fitsfile *fvis;
 static FILE *fsol;
 static FILE *fMSE;
 static FILE *fchisq;
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
 fMSE = fopen("MSE.dat", "w");
 fchisq = fopen("chisq.dat", "w");

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 open_fits( &fvis, FITSFile );

 proj.nsrc = -1;
 proj.preint = preint;
 nrecs =  fill_proj(fvis, &proj);
 if(nrecs < preint) preint = nrecs;

 fprintf(finfo, "%s # of recs averaged  : %d\n", info, preint);
 fprintf(finfo, "%s Max iter per solset : %d\n", info, MAXITER);

 setup_itersol( &proj );
 if( modelFile != NULL )
 { read_model_visibilities( &proj, modelFile );
   force_model( &proj );
 }
 write_sol( &proj, fsol);
 fprintf(finfo, "%s Reading UVFITS data ...\n", info);
 readFITS2Vis( fvis, &proj );
 compute_SNR( &proj );

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
 fprintf(finfo, "\n%s Reached max. no. of allowed iterations.", info); 
 else
 fprintf(finfo, "\n%s Solution converged in %d iterations.", info, proj.iter);
 
 fclose(fsol);
 fclose(fMSE);
 fclose(fchisq);

 write_baseline_SNR_matx( &proj );
 compute_error_matrix( &proj );

 kill_proj(&proj);
 close_fits(fvis);

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
  optrega(&preint  , OPT_INT,    't', "preint", "Integration time in seconds" );
  optrega(&modelFile , OPT_STRING,   'm', "model", "model file for the visibilities");
  sprintf(Usage, "\n-i InputFITSFile \n-o OutputSolFile \n-t Integration time in seconds\n-m Model visibilty file");
  optUsage(Usage);
  optTitle("Reads UVFITS visibilites and solves for antenna gain phasors for a point source at phase centre.\n");
  optMain(FITSgain);
  opt(&argc,&argv);

  return FITSgain(argc, argv);
}
