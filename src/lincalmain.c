#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char Usage[256];
char *solfile = NULL;
int nrecs, preint, nsrc;
char method;

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int lincal(int argc, char **argv )
{
 int nsol, recnum = 0;
 static FILE *flinsol;//, *flogsol;
 FILE *fchisq = fopen("chisq.dat", "w");
 ProjParType proj;
 PlotParType plot;
 char linsol[32];//, logsol[32];
 char modelfile[80] = "vis.model";

 if( solfile == NULL || nrecs < 1 || &method == NULL )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 strcpy(linsol, solfile);
 strcat(linsol, ".lin");
 flinsol = fopen(linsol, "w");

 nsol = nrecs / preint ;
 fprintf(finfo, "%s No. of recs in data : %d\n", info, nrecs);
 fprintf(finfo, "%s No. of solutions    : %d\n", info, nsol);
 fprintf(finfo, "%s One solution set per: %d rec[s].\n", info, preint);
 proj.nsrc = nsrc;
 proj.iter = 0;
 proj.preint = preint;
 init_proj(&proj); 
 init_sky_signal( &proj );
 sky_to_model( &proj );
 write_model_visibilities( &proj, modelfile );
 write_maps2FITS( &proj );
  
 /* 
 strcpy(logsol, solfile);
 strcat(logsol, ".log");
 flogsol = fopen(logsol, "w");
 */
{compose_A( &proj );
 recnum = genVis ( &proj);
 if     (method == 'Q' || method == 'q') log_QR_decomp( &proj ); 
 else if(method == 'S' || method == 's') log_SV_decomp( &proj );
 else  
 { fprintf(fwarn, "\a%s Unknown method %c: forcing SVD\n", warn, method);
   log_SV_decomp( &proj );
 }
   
 if (method == 'Q' || method == 'q') log_QR_solve( &proj ); 
 else                                log_SV_solve( &proj );
 // write_sol( &proj, flogsol );

 //fclose(flogsol);
}

/*  proj.nsrc = nsrc;
  proj.iter = 0;
  proj.preint = preint;
  init_proj(&proj); 
  write_model_visibilities( &proj );
  write_map2FITS( &proj );*/
//supply_initial_solutions( &proj );
  setup_lincal( &proj );
  write_sol( &proj, flinsol );
  genVis( &proj );

{ compose_B( &proj );
  B_index( &proj );
  if( method == 'Q' || method == 'q' ) setup_QRD( &proj );
  else                                 setup_SVD( &proj );

  while( recnum < nrecs )
  {recnum = genVis( &proj );
   proj.iter = recnum;
   if (method == 'Q' || method == 'q') lin_QRD_solve( &proj ); 
   else                                lin_SVD_solve( &proj );
   update_sol( &proj );
   update_B( &proj );
   compute_chisq( &proj, fchisq );
   fprintf(finfo, "%s [ %d / %d ] recs solved\r", info, recnum, nrecs);
   write_sol( &proj, flinsol );
  }
  fclose(flinsol);
}

 fclose( fchisq );

 compute_SNR( &proj );
 write_baseline_SNR_matx( &proj );
 compute_error_matrix( &proj );
 kill_proj(&proj);

#ifdef _PLOT
 plot.driver = 'X';
 fprintf(finfo, "\n%s Begin plots...\n", info);
 xsplot_all_but_errors( &plot, linsol ); 
#endif

 done();

 return 0;
}

int main(int argc, char **argv )
{
  optrega(&solfile, OPT_STRING, 'o', "out"   , "Output Solution File" );
  optrega(&nrecs  , OPT_INT,    't', "time"  , "Observing duration in seconds" );
  optrega(&preint , OPT_INT,    'a', "preint", "Pre-integration in seconds" );
  optrega(&nsrc   , OPT_INT,    'n', "nsrc"  , "No. of sources" );
  optrega(&method , OPT_CHAR,   'm', "method", "QR ('Q/q') or SVD('S/s') for the LLSE without space after -m" );
  sprintf(Usage, "\n-o OutputSolFile \n-t ObservingDuration_Seconds \n-a PreInt_Seconds \n-n No. of sources \n-m Method_LLSE: no space between -m and option");
  optUsage(Usage);
  optTitle("Generates visibilites and LLSq. solves for antenna gain phasors.\n");
  optMain(lincal);
  opt(&argc,&argv);

  return lincal(argc, argv);
}
