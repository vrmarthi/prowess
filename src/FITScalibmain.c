#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"
#include "../inc/plotdefs.h"
#define MAXITER 1000

char *inFITSFile = NULL;
char *outFITSFile = NULL;
char *inFITSMap = NULL;
char Usage[256];
char Title[256];
unsigned int preint = 1;
unsigned int N = -1;
unsigned int chan2blank = 4;
char *modelFile = NULL;
int BigEndian    = 0 ;
int LittleEndian = 0 ;
unsigned int nrow = ROWS;
unsigned int ncol = COLS;

int FITSapplcal(int argc, char **argv )
{
 int status = 0;
 int ch, del_iter = 0;
 long int nrecs, nsol, solno = 0, gcount;
 fitsfile *fin, *fout;
 FILE *fMSE = fopen("MSE.dat", "w");
 FILE *fchisq = fopen("chisq.dat", "w");
 ProjParType proj;
 int algo;
 PlotParType plot;
 int rec2disp = 0;
 int loop = 1, next = 1;

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
 if( N == -1 ) N = proj.chans / 2;

 create_fits( &fout, outFITSFile );
 init_hdr(fout, &proj);

 fprintf(finfo, "%s Recs avg for calib  : %d\n", info, preint);
 fprintf(finfo, "%s # of solution sets  : %d\n", info, proj.nrecs/preint);

 setup_itersol( &proj );
 if( modelFile != NULL )
 { read_model_visibilities( &proj, modelFile );
   force_model( &proj );
 }

 init_gain_table( &proj );
 init_bandpass_table( &proj );

 /* Gain calibration g(t_0,p_0) */
 proj.preint = preint;
 readFITS2Vis( fin, &proj );
 del_iter = 0;
 proj.chan2cal = N;
 reinit_itersol( &proj );
 while( ++del_iter <= MAXITER )
 { ++proj.iter;
   iterate_solution( &proj );
   if( modelFile != NULL ) force_model( &proj );
   update_solutions( &proj );
   if( test_convergence( &proj) ) break;
   if(del_iter == MAXITER) fprintf(finfo, "\n%s Reached max. no. of iterations.\n", info); 
 }
 
 /* Write gain table */
 write_gaintab( &proj );
 fits_rewind( fin, &proj );
 proj.nrecs /= proj.preint;

 while( solno++ < proj.nrecs )
 { /*Gain calibration g(t,p) */
   readFITS2Vis( fin, &proj );
   del_iter = 0;
   proj.chan2cal = N;
   reinit_itersol( &proj );
   while( ++del_iter < MAXITER )
   { ++proj.iter;
     iterate_solution( &proj );
     update_solutions( &proj );
     if( test_convergence( &proj) ) break;
     if(del_iter == MAXITER) fprintf(finfo, "\n%s Reached max. no. of iterations.\n", info); 
   }
   write_gaintab( &proj );
   apply_model( &proj );

   /*Write out model visibilities */
   if( solno == 1) write_model_visibilities( &proj, "estimated.model" );

   /*Bandpass calibration b(t,p) = g(t,p)/g(t,p_0)*/
   for( proj.chan2cal = 0; proj.chan2cal < proj.chans ; proj.chan2cal++)
   { del_iter = 0;
     proj.iter = 0;
     while( ++del_iter < MAXITER )
     { ++proj.iter;
       iterate_solution( &proj );
       force_model( &proj );
       update_solutions( &proj );
       if( test_convergence( &proj) ) break;
       if(del_iter == MAXITER) fprintf(finfo, "\n%s Reached max. no. of iterations.\n", info); 
     }
     write_bpasstab( &proj );
   }
   fprintf(finfo, "%s Bandpass [ %ld / %d ] converged in %d iterations\r", info, solno, proj.nrecs, proj.iter);

   /*Apply calibration solutions and write to FITS */
   apply_solutions( &proj );

   gcount = writeVis2FITS( fout, &proj );
 }

 fits_update_key_lng(fout, "GCOUNT", gcount, "number of random groups", &status);
 write_EXtabs( fout, &proj );
 newline();

 proj.dispBuf.nrow = nrow;
 proj.dispBuf.ncol = ncol;
 proj.dispBuf.altswtch = 0;
 proj.dispBuf.chan2blank = chan2blank;
 plot.driver = 'X';
 getpar_instspec( &proj );

 do{
   fprintf(fprompt, "%s BSOL # to quit / display [ 0 / 1 - %d ] : ", prompt, proj.nrecs );
   scanf("%d", &rec2disp);
   if(rec2disp < 1 ) 
   { fprintf(finfo, "%s Quitting", info);
      break;
   }
   if( rec2disp > proj.nrecs )
   { fprintf(fwarn, "%s BSOL number outside range [1 -%d] solutions!\n", warn, proj.nrecs);
     continue;
   }

   proj.chan2cal = rec2disp;
   extract_bandpass( &proj );
   buffer_range( &(proj.dispBuf), proj.chans, proj.baselines );
   if ( plot.driver == 'P' || plot.driver == 'p' )
   { fprintf(finfo, "%s ======= Plots in pgplot.ps =======\n", info);
     ps_nplot_chanvis( & proj );
     newline();
   }
   else if( plot.driver == 'x' || plot.driver == 'X') 
   xs_nplot_chanvis( & proj );

 } while(next);


 close_fits (fin);
 close_fits (fout);

 fclose(fMSE);
 fclose(fchisq);

 close_gain_table( &proj );
 close_bandpass_table( &proj );
 kill_proj(&proj);
 newline();
 done();
 filesize(inFITSFile);
 filesize(outFITSFile);
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&inFITSFile, OPT_STRING, 'i', "in"    , "Input FITS File");
  optrega(&outFITSFile , OPT_STRING, 'o', "out"   , "Output FITS File" );
  optrega(&inFITSMap, OPT_STRING, 'p', "map"    , "Input FITS Map");
  optrega(&preint  , OPT_UINT,    't', "preint", "Pre-integration in seconds >= 1" );
  optrega(&N  , OPT_UINT,    'n', "chan", "Channel to calibrate : [1 - MAX_CHAN], default MAX_CHAN/2" );
  optrega(&chan2blank  , OPT_UINT,    'k', "blank", "Channels to blank for BPASS plots" );
  optrega(&nrow  , OPT_UINT,    'r', "rows", "No. of rows per plot page");
  optrega(&ncol  , OPT_UINT,    'c', "cols", "No. of columns per plot page");
  optrega(&modelFile , OPT_STRING,   'm', "model", "model file for the visibilities");

  strcpy(Usage, "\n-i InputFITSFile");
  strcat(Usage, "\n-o OutputFITSFile"); 
  strcat(Usage, "\n-t Pre-integration in seconds >= 1");
  strcat(Usage, "\n-n [Channel to calibrate[1-MAX_CHAN]]");
  strcat(Usage, "\n-k [Channels to blank for BPASS plots]");
  strcat(Usage, "\n-r [No. of rows per plot page]");
  strcat(Usage, "\n-c [No. of columns per plot page]");
  strcat(Usage, "\n-m [model visibility file]\n");
  optUsage(Usage);

  strcpy(Title, "Reads UVFITS visibilites and solves for antenna gain and true visbility phasors.\n");
  strcat(Title, "Writes the calibrated visibilities into FITS file\n");
  optTitle(Title);

  optMain(FITSapplcal);
  opt(&argc,&argv);

  return FITSapplcal(argc, argv);
}
