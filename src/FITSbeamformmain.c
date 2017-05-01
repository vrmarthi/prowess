#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"
#include "../inc/plotdefs.h"
#define MAXITER 1000

char *inFITSFile=NULL;
char Usage[256];
char dev='X';
int preint = 1;
int N = -1;
int chan2blank = 4;
float dec = 0;
float ra = 0;
char *modelFile = NULL;
int BigEndian    = 0 ;
int LittleEndian = 0 ;

int FITSbeamform(int argc, char **argv )
{
 int status = 0;
 int ch, del_iter = 0;
 long int solno = 0, gcount;
 fitsfile *fin;
 FILE *fbeam = fopen("beam.dat", "w");
 double tgt_ra=0.0, tgt_dec=0.0;
 ProjParType proj;
 PlotParType plot;
 int loop = 1;
 long recnum;
 plot.driver = dev;

 if( inFITSFile == NULL || preint < 1  || (plot.driver != 'x' && plot.driver != 'X' && plot.driver != 'p' && plot.driver != 'P') )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 open_fits( &fin, inFITSFile );
 proj.preint = preint;
 fill_proj(fin, &proj);
 if( N == -1 ) N = proj.chans / 2;
 fprintf(finfo, "%s Recs avg. for calib : %d\n", info, preint);
 
 setup_itersol( &proj );
 if( modelFile != NULL )
 { read_model_visibilities( &proj, modelFile );
   force_model( &proj );
 }

 init_gain_table( &proj );
 init_bandpass_table( &proj );

 /*Gain calibration */
 del_iter = 0;
 proj.chan2cal = N;
 proj.preint = preint;
 reinit_itersol( &proj );
 recnum = readFITS2Vis( fin, &proj );
 while( ++del_iter <= MAXITER )
 { ++proj.iter;
   iterate_solution( &proj );
   if( modelFile != NULL ) force_model( &proj );
   update_solutions( &proj );
   if( test_convergence( &proj) ) break;
   if(del_iter == MAXITER) fprintf(finfo, "\n%s Reached max. no. of iterations.\n", info); 
 }
 write_gaintab( &proj );

 tgt_dec = (double)dec;
 init_beamformer( &proj, tgt_ra, dec ) ;

 /* Looping over all records for BP calibration and beamforming */
 fprintf(finfo, "%s # of solution sets  : %d\n", info, proj.nrecs); 
 fits_rewind( fin, &proj );
 proj.preint = 1;
 while( solno++ < proj.nrecs )
 { /* Gain Calibration */
   recnum = readFITS2Vis( fin, &proj );
   proj.chan2cal = N;
   reinit_itersol( &proj );
   proj.iter = 0;
   while( ++proj.iter <= MAXITER )
   { iterate_solution( &proj );
       //if( modelFile != NULL ) force_model( &proj );
     update_solutions( &proj );
     if( test_convergence( &proj) ) break;
     if(proj.iter == MAXITER) fprintf(finfo, "\n%s Reached max. no. of iterations.\n", info); 
   }
   write_gaintab( &proj );
   apply_model( &proj );

   /*Bandpass calibration */
   for( proj.chan2cal = 0; proj.chan2cal < proj.chans ; proj.chan2cal++)
   { del_iter = 0;
     proj.iter = 0;
     reinit_itersol( &proj );
     while( ++del_iter <= MAXITER )
     { ++proj.iter;
       iterate_solution( &proj );
       //if( modelFile != NULL ) force_model( &proj );
       force_model( &proj );
       update_solutions( &proj );
       if( test_convergence( &proj) ) break;
       if(del_iter == MAXITER) fprintf(finfo, "\n%s Reached max. no. of iterations.\n", info); 
     }
     write_bpasstab( &proj );
   }
   fprintf(finfo, "%s Bandpass [ %ld / %d ] converged in %d iterations\r", info, solno, proj.nrecs, proj.iter);

   /*Apply calibration solutions and form the beam */
   apply_solutions( &proj );
   beamform( &proj );
   write_beam_to_file( fbeam, &proj );
 }

 close_fits (fin);

 fclose(fbeam);

#ifdef _PLOT
 proj.dispBuf.altswtch = 0;
 proj.dispBuf.chan2blank = chan2blank;
 getpar_beamdisp( &proj );
 copy_beam_to_dispBuf( &proj );

 newline();
 if(plot.driver == 'X' || plot.driver == 'x' )
 fprintf(finfo, "%s Press any key to toggle / Press 'q' to quit", info);
 else if ( plot.driver == 'P' || plot.driver == 'p' )
 fprintf(finfo, "%s ======= Plots in pgplot.ps =======", info);
 do{
 buffer_range( &(proj.dispBuf), proj.chans, (int)(proj.nrecs/proj.preint) );
 loop = imag_beamdata( &proj, &plot );
 }while(loop);
 free_display_databuf( &proj );
#endif

 close_gain_table( &proj );
 close_bandpass_table( &proj );
 kill_proj(&proj);
 done();
 filesize(inFITSFile);
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&inFITSFile, OPT_STRING, 'i', "in"    , "Input FITS File");
  optrega(&preint  , OPT_INT,    't', "preint", "Pre-integration in seconds >= 1" );
  optrega(&dev , OPT_CHAR, 'd', "dev"   ,  "Plot device : x/X for XWindow, p/P for PS file, no space after -d" ); 
  optrega(&N  , OPT_INT,    'n', "chan", "Channel to calibrate : [1 - MAX_CHAN], default MAX_CHAN/2" );
  optrega(&chan2blank  , OPT_INT,    'k', "blank", "Channels to blank [ default 4 ]" );
  optrega(&dec, OPT_FLOAT, 'c', "dec", "Declination towards which to form beam in sexagesimal units");
  optrega(&modelFile , OPT_STRING,   'm', "model", "model file for the visibilities");
  sprintf(Usage, "\n-i InputFITSFile \n-t Pre-integration in seconds >= 1\n-c Dec. for beam[sxg]\n-m model visibility file\n-k Channels to blank[default 4]\n-d device to plot [default X/x ; p/P for psplot ]");
  optUsage(Usage);
  optTitle("Reads UVFITS visibilites and solves for antenna gain and true visbility phasors.\n Writes the calibrated visibilities into FITS file\n. Forms a beam towards the given declination.");
  optMain(FITSbeamform);
  opt(&argc,&argv);

  return FITSbeamform(argc, argv);
}
