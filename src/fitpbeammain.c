#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

char *FITSFile=NULL;
char *inFITSMap=NULL;
char *modelfile=NULL;
char Usage[256];
double ra_in = -1.0, dec_in = -1.0;
int uvmin = 0;
int BigEndian    = 0 ;
int LittleEndian = 0 ;

int fitpbeam(int argc, char **argv )
{
 int status = 0;
 static fitsfile *fmap;
 char outfilename[80];
 ProjParType proj;
 PlotParType plot;
 FILE *fmodel;
 int loop;
 int algo;
 double radec[2];

 radec[0] = ra_in; radec[1] = dec_in;
 proj.uvmin = uvmin;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 if( FITSFile == NULL || radec[0] < 0.0 || radec[1] < 0.0 )
 { fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
   fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
   exit(-1);
 }
 else if (FITSFile != NULL )
 { proj.preint = 0;
   init_proj(&proj); 
   
   if(strstr(FITSFile, "FITS")){
   strcpy(outfilename, FITSFile);
   strtok(outfilename, ".");
   strcat(outfilename, ".model");
   if( FITSmap_to_model( &proj, FITSFile ) ) 
   { fprintf(ferr, "%s %s is likely a UVFITS file \n", err, FITSFile);
     fprintf(ferr, "%s Please supply a FITS map\n", info);
     kill_proj( &proj );
     done();
     return 0;
   }
   newline();
   write_model_visibilities( &proj, outfilename );
   }
   
 else if(strstr(FITSFile, "model"))
   read_model_visibilities(&proj, FITSFile);
 }

   /* Solve for primary beam using one of the algorithms below */
   if(inFITSMap != NULL )
   { read_FITSmap( &proj, inFITSMap);
     plot.itf = LIN; 
     plot.driver = 'X';
     plot.colour = 1;
     //wedgeplot_map( &proj, &plot);
     fprintf(finfo, "\n%s Choose from one of the methods listed below\n", info);
     fprintf(finfo,   "%s ===========================================\n", info);
     fprintf(finfo,   "%s === Skip primary beamshape parameter estimation          : 0\n", info);
     fprintf(finfo,   "%s === Nelder-Mead Simplex Minimization Algorithm           : 1\n", info);
     fprintf(finfo,   "%s === Fletcher-Reeves Conjugate Gradient Algorithm         : 2\n", info);
     fprintf(finfo,   "%s === Polak-Ribiere Conjugate Gradient Algorithm           : 3\n", info);
     fprintf(finfo,   "%s === Broyden-Fletcher-Goldfarb-Shanno Algorithm           : 4\n", info);
     fprintf(finfo,   "%s === Steepest Descent Algorithm                           : 5\n", info);  
     fprintf(finfo,   "%s === Sweep-Zoom Minimum Search Algorithm (default, enter) : 6\n", info);  
     fprintf(fprompt, "%s === ", prompt);
     //algo = getchar();
     algo = 6;
     switch(algo) 
     { case '0' : break;
       case '1' : solve_prbeam_par_nelder_mead_simplex( &proj );
                  break;
       case '2' : solve_prbeam_par_conj_grad_fletcher_reeves( &proj );
	          break;
       case '3' : solve_prbeam_par_conj_grad_polak_ribiere( &proj );
	          break;
       case '4' : solve_prbeam_par_vector_BFGS( &proj );
	          break;
       case '5' : solve_prbeam_par_steepest_descent( &proj );
	          break;
       case '6' : solve_prbeam_par_Sweep_Zoom_Minimum_search( &proj, radec );
	          break;
       default  : solve_prbeam_par_Sweep_Zoom_Minimum_search( &proj, radec );
	          break;
     }
   }
 /******************************************************************/

 kill_proj( &proj );
 done(); 
 return 0;
}

int main(int argc, char **argv )
{
  optrega(&FITSFile, OPT_STRING, 'i', "in" , "Input FITS map/model file"             );
  optrega(&inFITSMap, OPT_STRING, 'p', "map"    , "Input FITS Map");
  optrega(&ra_in, OPT_DOUBLE, 'r', "ra"    , "Intial value for RA");
  optrega(&dec_in, OPT_DOUBLE, 'd', "dec"    , "Initial value for Dec");
  optrega(&uvmin, OPT_INT, 'u', "uvmin"    , "UVMin Cutoff");
  sprintf(Usage, "\n\t-i Observed map/model file \n\t-p Input Map FITS\n\t-r Initial RA value\n\t-d Initial Dec value\n\t-u UV Min cutoff: default 0");
  optUsage(Usage);
  optTitle("Estimates the primary beam for the Ooty Radio Telescope from the observed model.\n");
  optMain(fitpbeam);
  opt(&argc,&argv);

  return fitpbeam(argc, argv);
}
