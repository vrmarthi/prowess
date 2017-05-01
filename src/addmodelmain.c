#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"

char *modelFile1=NULL;
char *modelFile2=NULL;
char *opmodelfile=NULL;
char Usage[256];
int BigEndian    = 0 ;
int LittleEndian = 0 ;

int add_model(int argc, char **argv )
{

 ProjParType proj1, proj2;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);
 
 if( modelFile1 == NULL || modelFile2 == NULL)
 { fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
   fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
   exit(-1);
 }

 init_proj( &proj1 );
 init_proj( &proj2 );

 if ( strstr(modelFile1, "model") != NULL )
  read_model_visibilities( &proj1, modelFile1 );
 
 if ( strstr(modelFile2, "model") != NULL )
  read_model_visibilities( &proj2, modelFile2 );
 
 
 addmodel( &proj1, &proj2 );

 write_model_visibilities( &proj1, opmodelfile );

 kill_proj( &proj1 );
 kill_proj( &proj2 );

 done();
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&modelFile1, OPT_STRING, 'i', "in1" , "Input Model File 1"             );
  optrega(&modelFile2, OPT_STRING, 'j', "in2" , "Input Model File 1"             );
  optrega(&opmodelfile, OPT_STRING, 'o', "out" , "Output Model File"             );
  optUsage(Usage);
  optTitle("Adds the two model visibilities.\n");
  optMain(add_model);
  opt(&argc,&argv);

  return add_model(argc, argv);
}
