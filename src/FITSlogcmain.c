#include <stdio.h>
#include <string.h>
#include "../inc/fitsdefs.h"
#include "../inc/sysdefs.h"
#include "../inc/skydefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char *FITSFile=NULL;
char *solfile=NULL;
char Usage[256];
int  preint;
char method;

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int FITSlogc(int argc, char **argv)
{
 int nrecs, nsol, recnum;
 static  fitsfile *fvis;
 static FILE *fsol;
 ProjParType proj;
 PlotParType plot;
 char filename[80];

 if( FITSFile == NULL || solfile == NULL || preint < 1 || &method == NULL )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 strcpy(filename, solfile);
 strcat(filename,".");
 strcat(filename, argv[0]);
 fsol = fopen(filename, "w");

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 open_fits( &fvis, FITSFile );
 proj.preint = preint;
 nrecs =  fill_proj(fvis, &proj);
 nsol = nrecs / preint ;

 fprintf(finfo, "%s No. of recs in data : %d\n", info, nrecs);
 fprintf(finfo, "%s No. of solutions    : %d\n", info, nsol);
 fprintf(finfo, "%s One solution set per: %d rec[s].\n", info, preint);

 compose_A( &proj );

 if     (method == 'Q' || method == 'q')  log_QR_decomp( &proj ); 
 else if(method == 'S' || method == 's')  log_SV_decomp( &proj );
 else  
 { fprintf(fwarn, "%s Unkown method %c: forcing SVD\n", warn, method);
   log_SV_decomp( &proj );
 }

 while ( nsol-- )
 {recnum = readFITS2Vis( fvis, &proj );
  if (method == 'Q' || method == 'q') log_QR_solve( &proj ); 
  else                                log_SV_solve( &proj );
  convert_sol_log2lin( &proj );
  write_sol( &proj, fsol );
  if(! (recnum % preint) )
  fprintf(finfo, "%s Solved [ %d / %d ] recs\r", info, recnum, nrecs );
 }
 fclose(fsol);

 compute_SNR(&proj );
 write_baseline_SNR_matx( &proj );
 compute_error_matrix( &proj );
 kill_proj ( &proj );
 close_fits( fvis );

#ifdef _PLOT
 plot.driver = 'X';
 fprintf(finfo, "\n%s Begin plots...\n", info);
 xsplot_all_but_errors( &plot, filename ); 
#endif

 done();

 return 0;
}


int main(int argc, char **argv )
{
  optrega(&FITSFile, OPT_STRING, 'i', "in"    , "Input FITS File");
  optrega(&solfile , OPT_STRING, 'o', "out"   , "Output Solution File");
  optrega(&preint  , OPT_INT   , 'n', "preint", "Pre-integration time in seconds");
  optrega(&method  , OPT_CHAR  , 'm', "method", "QR (Q) or SVD(S) for the LLSE");
  sprintf(Usage, "\n-i InputFITSFile \n-o OutputSolFile \n-n PreInt_Seconds \n-m Method_LLSE: no space between -m and option");
  optUsage(Usage);
  optTitle("Solves for the gains and phases of the ORT antennas every <int> seconds.\n");
  optMain(FITSlogc);
  opt(&argc,&argv);

  return FITSlogc(argc, argv);
}
