#include <stdio.h>
#include <string.h>
#include "../inc/fitsdefs.h"
#include "../inc/sysdefs.h"
#include "../inc/skydefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char Usage[256];
int BigEndian    = 0 ;
int LittleEndian = 0 ;
int showants = 0, showbase = 0;

int res2fits(int argc, char **argv)
{
 int nrecs;
 ProjParType proj;
 int V2_init = 0;


 system("./fgrm.m");

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 init_proj(&proj);
 /****** V2 correlation ******/
 init_V2corr( &proj );
 write_V2res2FITS( &proj );
 /***************************/
 kill_proj ( &proj );
 done();


 return 0;
}


int main(int argc, char **argv )
{
  optMain(res2fits);
  opt(&argc,&argv);

  return res2fits(argc, argv);
}
