#include <stdio.h>
#include <string.h>
#include "../inc/fitsdefs.h"
#include "../inc/sysdefs.h"
#include "../inc/skydefs.h"
#include "../inc/opt.h"

char *FITSFile=NULL;
char Usage[256];

int ant1=-1, ant2=-1;
int base1=-1, base2=-1;
int chan1=-1, chan2=-1;
int rec1=-1, rec2=-1;
int undo = 0;

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int flagdata(int argc, char **argv)
{
 int nrecs;
 static  fitsfile *fvis;
 ProjParType proj;

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
 fprintf(finfo, "%s No. of recs in data : %d", info, nrecs);

 proj.flagpar.a1 = ant1; proj.flagpar.a2 - ant2;

 while( nrecs -- > 0 )
{ readFITS2Vis ( fvis, &proj );
  flagit( &proj);
  writeVis2FITS( fvis, &proj );
 }

 kill_proj ( &proj );
 close_fits( fvis );
 newline();
 done();
 filesize( FITSFile );

 return 0;
}


int main(int argc, char **argv )
{
  optrega(&FITSFile, OPT_STRING, 'i', "in"    , "Input FITS File");
  optrega(&ant1, OPT_INT, 'a', "ant1", "Start antenna");
  optrega(&ant2, OPT_INT, 'A', "ant2", "Stop antenna");
  optrega(&base1, OPT_INT, 'b', "base1", "Start baseline");
  optrega(&base2, OPT_INT, 'B', "base2", "Stop baseline");
  optrega(&chan1, OPT_INT, 'c', "chan1", "Start channel");
  optrega(&chan2, OPT_INT, 'C', "chan2", "Stop channel");
  sprintf(Usage, " -i InputFITSFile \n\t\t -a Ant1      [-A Ant2] \n\t\t -b baseline1 [-B baseline2] \n\t\t -c channel1  [-C channel2] \n\t\t -n rec1      [-N rec2]");
  optUsage(Usage);
  optTitle("Turns visibility weight off in the UVFITS file\n");
  optMain(flagdata);
  opt(&argc,&argv);

  return flagdata(argc, argv);
}
