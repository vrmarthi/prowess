#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/skydefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"

char Usage[256];
int nmaps = 0;
int BigEndian    = 0 ;
int LittleEndian = 0 ;

int genEPSmaps(int argc, char **argv )
{
ProjParType proj;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 if( nmaps <= 0 )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 init_proj(&proj); 
 gen_mock_EPS_maps( &proj, nmaps );
 kill_proj( &proj );
  done();

 return 0;

}

int main(int argc, char **argv )
{
  optrega(&nmaps    , OPT_INT,   'n', "maps", "no. of maps");
  sprintf(Usage, "\n-n no. of maps \n");
  optUsage(Usage);
  optTitle("Generates confusion-limited maps of extragalactic radio point sources.\n");
  optMain(genEPSmaps);
  opt(&argc,&argv);

  return genEPSmaps(argc, argv);
}
