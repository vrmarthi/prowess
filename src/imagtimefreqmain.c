#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char Usage[512];
char *FITSFile = NULL;
int ant1 = 0, ant2 = 1;
int mode = 0;
char dev='X';
char itf = 'l';

int BigEndian    = 0 ;
int LittleEndian = 0 ;


int imagtifr(int argc, char **argv )
{
 static  fitsfile *fvis;
 PlotParType plot;
 ProjParType proj;
 int recnum = 0, nrecs = 0, bl = 0;
 int loop = 1;
 int i;
 int bigloop = 1;

 plot.driver = dev;
 proj.preint = 1;

 if( FITSFile == NULL || (plot.driver != 'x' && plot.driver != 'X' && plot.driver != 'p' && plot.driver != 'P') )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 proj.dispBuf.altswtch = mode;
 if( itf == 'g' ) plot.itf = LOG;
 else if( itf == 's' ) plot.itf = SQR;
 else plot.itf = LIN;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 open_fits( &fvis, FITSFile );
 filesize( FITSFile ); 
 proj.nrecs =  nrecs = fill_proj(fvis, &proj);

 proj.dispBuf.start_rec = 1;
 proj.dispBuf.stop_rec = proj.nrecs;
 proj.dispBuf.start_chan = 1;
 proj.dispBuf.stop_chan = proj.chans;

 if( ant1 >= proj.ants )
 { fprintf(ferr, "%s Antenna %d not found. Quitting.\n", err, ant1);
   exit(-1);
 }
 if( ant2 >= proj.ants )
 { fprintf(ferr, "%s Antenna %d not found. Quitting.\n", err, ant2);
    exit(-1);
 }

 else if (ant1 == ant2)
{ fprintf(ferr, "%s Self-correlation of baseline %d-%d not found.\n%s Quitting.\n", err, ant1, ant2, err);
  exit(-1);
 }

 do{
 bl = getpar_dynspect( &proj, ant1, ant2);
 fits_rewind( fvis, &proj );

 for( i = 0 ; i < proj.dispBuf.start_rec-1 ; i++ ) readFITS2Vis( fvis, &proj );

 for ( i = proj.dispBuf.start_rec-1 ; i < proj.dispBuf.stop_rec ; i++ )
 { recnum = readFITS2Vis( fvis, &proj );
   extract_baseline( &proj, bl );
   nrecs = (recnum - proj.dispBuf.start_rec + 1) % (proj.dispBuf.nrec+1);
   fprintf(finfo, "%s Bl : %d (%s-%s) read [ %d / %d ] records\r", info, bl, proj.Ant[ant1].name, proj.Ant[ant2].name, nrecs, proj.dispBuf.nrec);
 }
fprintf(finfo, "%s Bl : %d (%s-%s) read [ %d / %d ] records", info, bl, proj.Ant[ant1].name, proj.Ant[ant2].name, nrecs, proj.dispBuf.nrec);

 if ( plot.driver == 'P' || plot.driver == 'p' )
 fprintf(finfo, "\n%s ======= Plots in pgplot.ps =======", info);

 do{
   buffer_range( &(proj.dispBuf), proj.dispBuf.nchan, proj.dispBuf.nrec );
   loop = imag_dynspec( &proj, &plot );
 } while(loop);

 free_display_databuf( &proj );

 newline();
 fprintf(fprompt, "%s Start record        : ", prompt); scanf("%d", &(proj.dispBuf.start_rec)  );
 fprintf(fprompt, "%s Stop record         : ", prompt); scanf("%d", &(proj.dispBuf.stop_rec)   );
 fprintf(fprompt, "%s Start channel       : ", prompt); scanf("%d", &(proj.dispBuf.start_chan) );
 fprintf(fprompt, "%s Stop channel        : ", prompt); scanf("%d", &(proj.dispBuf.stop_chan)  );

 if(proj.dispBuf.stop_rec == 0) bigloop = 0;

 }while(bigloop);


 close_fits(fvis);
 kill_proj( &proj );
 done();
 filesize(FITSFile);
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile , OPT_STRING, 'i', "in"   , "Input FITS File" );
  optrega(&ant1 , OPT_UINT, 'a', "ant"   ,  "Antenna 1" );
  optrega(&ant2 , OPT_UINT, 'b', "ant"   ,  "Antenna 2" );
  optrega(&mode , OPT_UINT, 'm', "mode"   ,  "Plot mode 0 for Amp-Phase, 1 for Real/Imag" );
  optrega(&dev , OPT_CHAR, 'd', "dev"   ,  "Plot device : x/X for XWindow, p/P for PS file, no space after -d" ); 
  optrega(&itf , OPT_CHAR, 'f', "itf"   ,  "Image Transfer Function : default 'l'- lin, 'g'- log, 's' - sqrt" );
  sprintf(Usage, "\n-i InputFITSFile \n[-a=0] Ant 1\n[-b=1] Ant 2\n[-d='X'] Plot device\n ITF: default 'l'- lin, 'g'- log, 's'- sqrt " );
  optUsage(Usage);
  optTitle("Plots of time vs freq of the visibilities of a baseline. \n");
  optMain(imagtifr);
  opt(&argc,&argv);

  return imagtifr(argc, argv);
}
