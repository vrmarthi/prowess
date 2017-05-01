#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char Usage[256];
char *FITSFile = NULL;
int chan2disp = -1;
char dev='X';
int mode = 0;
int nrow = ROWS;
int ncol = COLS;
int BigEndian    = 0 ;
int LittleEndian = 0 ;

int xtract(int argc, char **argv )
{
 static  fitsfile *fvis;
 PlotParType plot;
 ProjParType proj;
 int recnum = 0, nrecs = 0, bl = 0;
 int loop = 1;
 int i;
 char foutname[80];
 FILE *fout;

 plot.driver = dev;
 proj.preint = 1;

 if( FITSFile == NULL || (plot.driver != 'x' && plot.driver != 'X' && plot.driver != 'p' && plot.driver != 'P') )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 open_fits( &fvis, FITSFile );
 filesize( FITSFile ); 
 proj.nrecs =  nrecs = fill_proj(fvis, &proj);

 if(chan2disp == -1 )
     proj.dispBuf.chan2blank = proj.chans / 2;
 else
     proj.dispBuf.chan2blank = chan2disp;

 getpar_timevis( &proj );

 strcpy(foutname, argv[0]);
 strcat(foutname, ".dat");
 fout = fopen(foutname, "w");
 while(nrecs-- > 0)
 { recnum = readFITS2Vis( fvis, &proj );
   extract_vis( &proj );
   writevis( &proj, fout );
   if(!(recnum%10)) fprintf(finfo, "%s Read [ %d / %d ] records\r", info, recnum, proj.nrecs);
 }
 fprintf(finfo, "%s Read [ %d / %d ] records\r", info, recnum, proj.nrecs);
 fclose(fout);
 close_fits(fvis);
 free_display_databuf( &proj );
 kill_proj( &proj );
 done();

 return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile , OPT_STRING, 'i', "in"   , "Input FITS File" );
  optrega(&chan2disp , OPT_UINT, 'n', "channel"   ,  "Channel to display" );
  sprintf(Usage, "\n-i InputFITSFile\n[-n=N/2] Display chan.");
  optUsage(Usage);
  optTitle("Extracts the visibilities of all baselines, a given channel. \n");
  optMain(xtract);
  opt(&argc,&argv);

  return xtract(argc, argv);
}
