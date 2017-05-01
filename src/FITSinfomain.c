#include <stdio.h>
#include <string.h>
#include "../inc/fitsdefs.h"
#include "../inc/sysdefs.h"
#include "../inc/skydefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char *FITSFile=NULL;
char Usage[256];
char dev='X';
char itf = 'l';
int colour = 0 ;
int BigEndian    = 0 ;
int LittleEndian = 0 ;
int showants = 0, showbase = 0, showflag=0;

int FITSinfo(int argc, char **argv)
{
 int nrecs;
 static  fitsfile *fvis;
 ProjParType proj;
 PlotParType plot;

 plot.driver = dev;
 plot.colour = colour;
 if( itf == 'g' ) plot.itf = LOG;
 else if( itf == 's' ) plot.itf = SQR;
 else plot.itf = LIN;

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
 if( showants ) show_antennas( &proj );
 if( showbase ) show_baselines( &proj );

 show_scaninfo( fvis, &proj );
 fprintf(finfo, "%s  All scans =>  Recs : %4d\n", info, nrecs);

 if(showflag)
 { init_flagmask( &proj );
   make_flagmask( fvis, &proj );
   read_FITSmap( &proj, "FGMASK.FITS" );
   newline();
   proj.chan2cal = 1;
   wedgeplot_map( &proj, &plot );
   free_display_databuf( &proj );
 }

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
  optrega(&showflag, OPT_BOOL, 'f', "", "Show flag mask");
  sprintf(Usage, " -i InputFITSFile [-a -b -f]\n -a : Show antennas \n -b : Show baselines \n -f : Show flagmask");
  optUsage(Usage);
  optTitle("Prints information about the UVFITS file\n");
  optMain(FITSinfo);
  opt(&argc,&argv);

  return FITSinfo(argc, argv);
}
