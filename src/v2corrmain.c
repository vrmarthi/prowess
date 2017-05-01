#include <stdio.h>
#include <string.h>
#include "../inc/fitsdefs.h"
#include "../inc/sysdefs.h"
#include "../inc/skydefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char *FITSFile=NULL;
char Usage[256];

int BigEndian    = 0 ;
int LittleEndian = 0 ;
int showants = 0, showbase = 0;

int corrl_v2(int argc, char **argv)
{
 int nrecs;
 static  fitsfile *fvis;
 ProjParType proj;
 PlotParType plot;
 int V2_init = 0;

 plot.driver = 'X';
 plot.itf = LIN;

 if( FITSFile == NULL )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 open_fits ( &fvis, FITSFile );
 proj.preint = 1;
 nrecs =  fill_proj ( fvis, &proj );
 if( nrecs == -1 ) 
 { fprintf(ferr, "%s %s is a FITS map file \n", err, FITSFile);
   done();
   filesize( FITSFile );
   return 0;
 }
 fprintf(finfo, "%s No. of recs in data : %d\n", info, nrecs);
 if( showants ) show_antennas( &proj );
 if( showbase ) show_baselines( &proj );

 /****** V2 correlation ******/
 
 /**** Mean V2 ****/
 init_V2corr( &proj );
 while( nrecs-- )
 { readFITS2Vis( fvis, &proj );
   prep_V2( &proj );
 }
/*
 proj.preint = nrecs;
 readFITS2Vis( fvis, &proj );
 prep_V2( &proj );
*/
 corrl_V2( &proj );
 V2_deltanu_from_v2corr( &proj );
 //plot_V2_deltanu( &proj, &plot);
 write_V2corr2FITS( &proj );

 /***************************/
 
 fits_rewind( fvis, &proj );
 nrecs = proj.nrecs;

 /******** V2 error bars ********/
 while( nrecs-- )
 { readFITS2Vis( fvis, &proj );
   cleanup_V_sum_rec( &proj );
   prep_V2_errbar( &proj );
   corrl_V2_rec( &proj );
 }

 estimate_V2_err( &proj );
 write_V2_err2FITS( &proj );

 free_V2( &proj );

 kill_proj ( &proj );
 close_fits( fvis );

 done();
 filesize( FITSFile );

 return 0;
}


int main(int argc, char **argv )
{
  optrega(&FITSFile, OPT_STRING, 'i', "in"    , "Input FITS File");
  optrega(&showants, OPT_BOOL, 'a', "", "Show antenna list");
  optrega(&showbase, OPT_BOOL, 'b', "", "Show baseline list");
  sprintf(Usage, " -i InputFITSFile [-a -b]");
  optUsage(Usage);
  optTitle("Prints information about the UVFITS file\n");
  optMain(corrl_v2);
  opt(&argc,&argv);

  return corrl_v2(argc, argv);
}
