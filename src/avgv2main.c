#include <stdio.h>
#include <string.h>
#include "../inc/fitsdefs.h"
#include "../inc/sysdefs.h"
#include "../inc/skydefs.h"
#include "../inc/opt.h"

int files=0;
char Usage[256];

int BigEndian    = 0 ;
int LittleEndian = 0 ;

int avgv2(int argc, char **argv)
{
 int nrecs;
 static  fitsfile *fvis;
 ProjParType proj;
 int i;
 int bl, ch1, ch2;
 char FITSFile[32];
 double ***V2;

 if( files == 0 )
 {fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
 }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);


 strcpy(FITSFile, "D1.FITS\0");
 open_fits ( &fvis, FITSFile );
 proj.preint = 1;
 nrecs =  fill_proj ( fvis, &proj );

 V2 = (double***)calloc(proj.redundant_baselines+1, sizeof(double**) );
 for( bl = 0 ; bl < proj.redundant_baselines+1; bl++ ) 
 { V2[bl] = (double**)calloc(proj.chans, sizeof(double*) );
   for( ch1 = 0 ; ch1 < proj.chans ; ch1++ )
    V2[bl][ch1] = (double*)calloc(proj.chans, sizeof(double) ); 
 }

 for(i = 1 ; i <= files ; i++ )
 { sprintf(FITSFile, "D%d.FITS\0", i);
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
   filesize( FITSFile );
   /****** V2 correlation ******/
   init_V2corr( &proj );
   while( nrecs-- )
   { readFITS2Vis( fvis, &proj );
     prep_V2( &proj );
   }
   corrl_V2( &proj );

   /***************************/

  for( bl = 0 ; bl < proj.redundant_baselines ; bl++ )
  { for(ch1 = 0 ; ch1 < proj.chans ; ch1++ )
    { for( ch2 = 0 ; ch2 < proj.chans ; ch2++ )
       V2[bl][ch1][ch2] += proj.V2[bl][ch1][ch2]/files;
    }
  }

  free_V2( &proj );
  close_fits( fvis );
 }

 init_V2corr( &proj );
 proj.V2 = V2;

 write_V2corr2FITS( &proj );
 
 kill_proj ( &proj );
 done();

 free(V2);  

   return 0;
 }


int main(int argc, char **argv )
{
  optrega(&files, OPT_INT, 'n', ""    , "Number");
  sprintf(Usage, " -n number");
  optUsage(Usage);
  optTitle("Prints information about the UVFITS file\n");
  optMain(avgv2);
  opt(&argc,&argv);

  return avgv2(argc, argv);
}
