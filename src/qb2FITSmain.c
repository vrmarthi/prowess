#include <stdio.h>
#include <string.h>
#include "../inc/fitsdefs.h"
#include "../inc/sysdefs.h"
#include "../inc/skydefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char Usage[256];
char *cubefile=NULL;
char *FITSFile=NULL;
int BigEndian    = 0 ;
int LittleEndian = 0 ;
int showants = 0, showbase = 0;

int qb2fits(int argc, char **argv)
{
 int nrecs;
 ProjParType proj;
 int V2_init = 0;

 if( FITSFile == NULL || cubefile == NULL )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 init_proj(&proj);
 /****** V2 correlation ******/
 init_V2corr( &proj );
 write_qb2FITS( &proj, cubefile, FITSFile );
 /***************************/
 kill_proj ( &proj );
 done();

 return 0;
}


int main(int argc, char **argv )
{
  optrega(&cubefile , OPT_STRING, 'i', "in"   , "Input cube file" );
  optrega(&FITSFile , OPT_STRING, 'o', "out"   , "Output FITS File" );
  sprintf(Usage, "\n-i Input cube file -o Output FITS File");
  optUsage(Usage);
  optTitle("Writes a linear cube data file into a FITS cube");
  optMain(qb2fits);
  opt(&argc,&argv);

  return qb2fits(argc, argv);
}
