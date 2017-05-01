#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char *FITSFile=NULL;
char Usage[256];
double alpha_real = -1.0, alpha_imag= -1.0;
int BigEndian    = 0 ;
int LittleEndian = 0 ;

int fitalpha(int argc, char **argv )
{
 int status = 0;
 static fitsfile *fvis;
 char outfilename[80];
 ProjParType proj;
 PlotParType plot;
 FILE *fmodel;
 int loop;
 int algo;
 int nrecs;
 double alpha[2];

 alpha[0] = alpha_real; alpha[1] = alpha_imag;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 if( FITSFile == NULL || alpha[0] < 0.0 || alpha[1] < 0.0 )
 { fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
   fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
   exit(-1);
 }
 else if (FITSFile != NULL )
 { open_fits( &fvis, FITSFile );
   nrecs =  fill_proj(fvis, &proj);
   proj.preint = nrecs;
   readFITS2Vis( fvis, &proj ); 
   read_model_visibilities(&proj, "vis.model");
   solve_dipole_coupling_nelder_mead_simplex( &proj );
 }   
   kill_proj( &proj );
   done(); 
   return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile, OPT_STRING, 'i', "in" , "Input UVFITS file"             );
  optrega(&alpha_real, OPT_DOUBLE, 'r', "real"    , "Real part of coupling");
  optrega(&alpha_imag, OPT_DOUBLE, 'm', "imag"    , "Imag part of coupling");
  sprintf(Usage, "\n\t-i Input UVFITS file \n\t-r Real part of coupling coeff\n\t-m Imag part of coupling\n");
  optUsage(Usage);
  optTitle("Estimates the coupling coefficient for the Ooty Radio Telescope from the observed visibilities.\n");
  optMain(fitalpha);
  opt(&argc,&argv);

  return fitalpha(argc, argv);
}
