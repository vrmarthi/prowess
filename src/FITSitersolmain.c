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
int  preint;

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int FITSsol(int argc, char **argv )
{
 long int nrecs;
 static fitsfile *fvis;
 static FILE *fsol;
 static FILE *fMSE; 
 static FILE *fchisq;
 char filename[32];
 ProjParType proj;
 PlotParType plot;

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

 proj.preint = preint;
 nrecs =  fill_proj(fvis, &proj);
 if(nrecs < preint) preint = nrecs;
 fprintf(finfo, "%s # of recs averaged  : %d\n", info, preint);
 fprintf(finfo, "%s Max iter per solset : %d\n", info, MAXITER);

 fprintf(finfo, "%s Reading UVFITS data ...\n", info);
 readFITS2Vis( fvis, &proj );
 compute_SNR( & proj );
 setup_itersol( &proj );
 write_sol( &proj, fsol);

 proj.iter = 0;
 proj.chan2cal = proj.chans / 2;
#ifndef _NO_VERBOSE
  find_load( START_COUNT);
#endif
 while( proj.iter++ < MAXITER )
 { /*get_bpass( &proj );
     copyBuf_bpass( &proj );*/
   iterate_solution( &proj );
   compute_MSE( &proj, fMSE );
   compute_chisq( &proj, fchisq );
   update_solutions( &proj );
   if( test_convergence( &proj) ) break;
   fprintf(finfo, "%s [ %d / %d ] iterations done\r", info, proj.iter, MAXITER);
   write_sol( &proj, fsol );
 }
#ifndef _NO_VERBOSE
  find_load( STOP_COUNT);
#endif

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
  optrega(&preint  , OPT_INT,    't', "preint", "Acquisition time in seconds" );
  sprintf(Usage, "\n-i InputFITSFile \n-o OutputSolFile \n-t Data duration in seconds\n");
  optUsage(Usage);
  optTitle("Reads UVFITS visibilites and solves for antenna gain and true visbility phasors.\n");
  optMain(FITSsol);
  opt(&argc,&argv);

  return FITSsol(argc, argv);
}
