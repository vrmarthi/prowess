#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
//#include <omp.h>
#include "../inc/sysdefs.h"
#include "../inc/matdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/remezdefs.h"
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_permutation.h>

char err[32];
char info[32];
char warn[32];
char prompt[32];

int preint;

double RefFreq =  BAND_CENTRE;// - BANDWIDTH/2 ;

/*****************************************************************
 * FUNCTION: int subtract_timeval(struct timeval *tv1, struct timeval *tv2, struct timeval *result)
 *  Subtracts two time instants tv1 and tv2 and returns the absolute 
 *  difference between two. result is stored in the pointer result.
******************************************************************/

int subtract_timeval(struct timeval *tv1, struct timeval *tv2, struct timeval *result)
{
   if(tv2->tv_usec < tv1-> tv_usec)
   {
      int nsec = (tv1->tv_usec - tv2->tv_usec)/1000000 + 1;
      tv1->tv_usec -= 1000000 * nsec;
      tv1->tv_sec += nsec;
   }

   if(tv2->tv_usec - tv1->tv_usec > 1000000)
   {
      int nsec = (tv2->tv_usec - tv1->tv_usec)/1000000;
      tv1->tv_usec +=1000000 * nsec;
      tv1->tv_sec -= nsec;
   }

   result->tv_sec  = tv2->tv_sec - tv1->tv_sec;
   result->tv_usec = tv2->tv_usec - tv1->tv_usec;

   return 0;
}

/*****************************************************************
 * FUNCTION: struct timeval find_load(int val)
 *
 *   Finds the time (with us resolution) for execution for instruction 
 *   between the calls to this function.
 *   Call this function two times. 
 *    First: Point at which you want to start measuring the time 
 *     call:
 *        find_load(START_COUNT);
 *    Second: Point till which you want to measure the time elapsed.
 *    call:
 *        find_load(STOP_COUNT);
 *       
 *    To find the time taken for execution of this function.
 *    call:
 *        find_load(TEST);
 *
 *   return value: returns time difference between START_COUNT and
 *                 STOP_COUNT times, in struct timeval
******************************************************************/

struct timeval find_load(int val)
{
   static struct timeval tv1;
   struct timeval tv2, result;
   static int indic=STOP_COUNT;

   if((val == START_COUNT)|| val == TEST)
   {
      if(indic == START_COUNT)  fprintf(fwarn, "%s Counting already initialized, initializing again.\n", warn);
      else indic = START_COUNT;
      gettimeofday(&tv1, NULL);
   }

   if((val == STOP_COUNT && indic == START_COUNT)|| val == TEST)
   {
      indic = STOP_COUNT;
      gettimeofday(&tv2, NULL);
      subtract_timeval(&tv1, &tv2, &result);
      fprintf(finfo, "\n%s took %ld.%06ld sec\n", info, result.tv_sec, result.tv_usec);
   }
   else
   {
      if((indic == STOP_COUNT) && (val != START_COUNT))
	  fprintf(fwarn, "%s Not initialized.\n", warn);
   }
   return result;
}

void newline(void)
{
    fprintf(stderr, "\n");
}

void done( void )
{
 fprintf(finfo, "%s Done. \n", info);
}

void filesize( char *filename )
{
  char filesz[80];
  sprintf(filesz, "ls -lh %s | awk '{print $5}'", filename);
  fprintf(finfo, "%s Size of file in disk: %s => ", info, filename);
  system(filesz);
}

void getPrompt(char *msg, char *call, char option)
{
 int len = 9 - strlen(call);
 switch( option ) { 
 case 'W' : 
     strcpy(msg, call) ;
     while( len-- > 0) strcat(msg, " ");
     strcat(msg, WARN) ;
     break;
     
 case 'E' :
     strcpy(msg, call) ;
     while( len-- > 0) strcat(msg, " ");
     strcat(msg, ERR);
     break;

 case 'I' :
     strcpy(msg, call);
     while( len-- > 0) strcat(msg, " ");
     strcat(msg, INFO);
     break;

 case 'P' :
     strcpy(msg, call);
     while( len-- > 0) strcat(msg, " ");
     strcat(msg, PROMPT);
     break;

 default  :
     strcpy(msg, call);
     while( len-- > 0) strcat(msg, " ");
     strcat(msg, PROMPT);
     }
}

void define_prompts(char *call)
{
 getPrompt(    err, call, 'E' ) ;
 getPrompt(   info, call, 'I' ) ;
 getPrompt(   warn, call, 'W' ) ;
 getPrompt( prompt, call, 'P' ) ;
}

void getMachineEndianness(int *BigEndian, int *LittleEndian)
{
union
{ unsigned short int u;
  char c[2];
} __attribute__((packed)) temp;

 temp.u = 0x0001;
 
 if ( temp.c[0] )
      { *LittleEndian = 1;
        *BigEndian = 0;
      }
 else { *LittleEndian = 0;
        *BigEndian = 1;
      }
}

void flash_obs_settings (ProjParType *proj )
{
 int i;
 fprintf(finfo, "%s Proj Code           : %s\n", info, proj->code);
 fprintf(finfo, "%s Observer            : %s\n", info, proj->observer);
 fprintf(finfo, "%s ORT Elements        : %d\n", info, proj->ORTelements);
 fprintf(finfo, "%s Receiver            : %s\n", info, proj->receiver);
 fprintf(finfo, "%s Obs Mode            : %s\n", info, proj->obsmode);
 fprintf(finfo, "%s Sidebands           : %d\n", info, proj->sidebands);
 fprintf(finfo, "%s Band Edge           : %5.2lf MHz\n", info, RefFreq/1e6 - proj->bandwidth/2e6);
 fprintf(finfo, "%s Band Center         : %5.2lf MHz\n", info, proj->bandcentre/1e6);
 fprintf(finfo, "%s Bandwidth           : %6.2f MHz\n", info, proj->bandwidth/1e6);
 fprintf(finfo, "%s Channels            : %d\n", info, proj->chans);
 fprintf(finfo, "%s Feed pols           : %d\n", info, proj->pols);
 for ( i = 0 ; i < proj->scans ; i++ )
 { fprintf(finfo, "%s ---------------------------------------\n", info);
   fprintf(finfo, "%s -----------> Source : %3d <------------\n", info, proj->scan[i].sid);
   fprintf(finfo, "%s Target              : %s\n", info, proj->scan[i].src);
   fprintf(finfo, "%s Pointing RA         : %2.0fh %2.0fm %2.0fs\n", info, proj->scan[i].ra.h, proj->scan[i].ra.m, proj->scan[i].ra.s);
   fprintf(finfo, "%s Pointing Dec        : %2.0fd %2.0f' %2.0f\"\n", info, proj->scan[i].dec.d, proj->scan[i].dec.m, proj->scan[i].dec.s);
   fprintf(finfo, "%s ---------------------------------------\n", info);
 }
}

int get_RxVer( char antstr[MAX_ANTS][80] )
{
 char str[80];
 int total_ants = 0;
 char AntDef[80];
 FILE *fAntDef;
 char *pwd, *pkgroot;

 pkgroot = getenv("PKGROOT");
 pwd = getenv("PWD");
 strcpy(AntDef, "Antenna.Def");
 if( ( fAntDef = fopen(AntDef, "r") ) == NULL )
 { fprintf(fwarn, "%s get_RxVer: Antenna.Def not found in %s.\n", warn, pwd);
   fprintf(fwarn,"%s get_RxVer: Copy from %s and edit!\n", warn, pkgroot);
   strcpy(AntDef, pkgroot);
   strcat(AntDef, "/Antenna.Def");
   if( ( fAntDef = fopen(AntDef, "r") ) == NULL )
   { fprintf(ferr, "%s get_RxVer: Antenna.Def not found in %s. Stop!\n", err, pkgroot);
     exit(-1);
   }
   fprintf(fwarn, "%s get_RxVer: Using default Antenna.Def\n", warn);
   fprintf(fwarn, "%s from %s!\n", warn, pkgroot);
 }
 else
 { fprintf(finfo, "%s get_RxVer           : Found Antenna.Def!\n", info);
 }

 do {fgets( str, sizeof(str), fAntDef ); }
 while( ! strstr(str,"#------------------") );
 
 do { fgets( str, sizeof(str), fAntDef );
    total_ants++;
 }while( ! strstr(str, "#END") );

 fclose(fAntDef);

 return --total_ants;
}

int get_num_ants( char antstr[MAX_ANTS][80] )
{
 char str[80];
 int ants = 0; 
 char AntDef[80];
 FILE *fAntDef;
 char *pwd, *pkgroot;
 
 pkgroot = getenv("PKGROOT");
 pwd = getenv("PWD");
 strcpy(AntDef, "Antenna.Def");
 if( ( fAntDef = fopen(AntDef, "r") ) == NULL )
 { fprintf(fwarn, "%s get_N(ants): Antenna.Def not found in %s.\n", warn, pwd);
   fprintf(fwarn,"%s get_N(ants): Copy from %s and edit!\n", warn, pkgroot);
   strcpy(AntDef, pkgroot);
   strcat(AntDef, "/Antenna.Def");
   if( ( fAntDef = fopen(AntDef, "r") ) == NULL )
   { fprintf(ferr, "%s get_N(ants):Antenna.Def not found in %s. Stop!\n", err, pkgroot);
     exit(-1);
   }
   fprintf(fwarn, "%s get_N(ants): Using default Antenna.Def\n", warn);
   fprintf(fwarn, "%s get_N(ants): from %s!\n", warn, pkgroot);
 }
 else
 { fprintf(finfo, "%s get_N(ants)         : Found Antenna.Def!\n", info);
 }

 do {fgets( str, sizeof(str), fAntDef ); }
 while( ! strstr(str,"#------------------") );
  do { fgets( str, sizeof(str), fAntDef );
    if( (!strstr(str,"#ANT")) && strstr(str,"ANT") )
        strcpy( antstr[ants++], str );
    }while( ! feof(fAntDef) );

 fclose(fAntDef);
 return ants;
}

void setpar_ant(ProjParType *proj,  char antstr[MAX_ANTS][80])
{
 FILE *flog;
 AntParType *ant = proj->Ant;
 SysParType system = proj->system;
 int i;
 char str[8], *token;

 flog = fopen("ant-init.info", "w");
 fprintf(finfo, "%s Antennas used and their initial gains in\n", info);
 fprintf(finfo, "%s %s/ant-init.info\n", info, getenv("PWD") );
 fprintf(flog, "###  Antenna\tRe(gain)\tIm(gain)\tabs(gain)\targ(gain)\n###\n");
 
 for( i = 0 ; i < proj->ants ; i++)
 { sscanf(antstr[i], "%s %s %lf %lf %lf %lf %lf", (str), (ant[i].name), 
	  &(ant[i].bx), &(ant[i].by), &(ant[i].bz), &(ant[i].ddly), &(ant[i].fdly) );
   token = strtok(str, "T");
   token = strtok(NULL, "T ");
   //sscanf(token, "%d",  &(ant[i].id) );
   ant[i].id = i;
   ant[i].gain.abs = 1.0;//2.0*(1+uniform_noise());
   ant[i].gain.arg = 0.0;//deg2rad(360*uniform_noise()) ;
   ant[i].g.r = ant[i].gain.abs * cos( ant[i].gain.arg );
   ant[i].g.i = ant[i].gain.abs * sin( ant[i].gain.arg );
   fprintf(flog, "%2d\t%s\t%lf\t%lf\t%lf\t%lf\n", ant[i].id, ant[i].name, ant[i].g.r, ant[i].g.i,
	   ant[i].gain.abs, rad2deg(ant[i].gain.arg) );
   //fprintf(stderr, "%2d\t%s\t%lf\t%lf\t%lf\t%lf\n", ant[i].id, ant[i].name, ant[i].g.r, ant[i].g.i,
   //ant[i].gain.abs, rad2deg(ant[i].gain.arg) );
 }
 fclose(flog);
}


void init_primary_beam( ProjParType *proj )
{
 double x_res, y_res;
 int xside, yside, zside;;
 double ptg_dec, dec_fan;
 double ptg_ra, ra_fan;
 double b, d;
 int i, j;
 PrimaryBeamParType *pb = &(proj->pbeam);

 ptg_ra = deg2rad( ra2sxg(&(proj->scan[0].ra)) );
 ptg_dec = deg2rad( dec2sxg(&(proj->scan[0].dec)) );
 dec_fan = deg2rad(4.5)*(proj->ORTelements/PHASE_I) / cos(ptg_dec);
 
  if( proj->ORTelements == PHASE_I ) 
  { y_res = CELLSIZE;  
    x_res = CELLSIZE;
    xside = MAPSIDE;
    yside = MAPSIDE;
    d = 11.5;
    b = 30.0;
  }

  else if( proj->ORTelements == PHASE_II)
  { y_res = CELLSIZE*6.0;
    x_res = CELLSIZE;
    xside = MAPSIDE;
    yside = MAPSIDE;
    d = 1.92;
    b = 30.0;
  }
  
  else
  {
  SkyMapType *sky = &(proj->skymap);
  xside = sky->xside;
  yside = sky->yside;
  x_res = deg2rad(sky->ra_res);
  y_res = deg2rad(sky->dec_res);
  }
 
 zside = proj->chans;

 pb->ra = (RAType*)calloc(sizeof(RAType), xside);
 pb->dec = (DecType*)calloc(sizeof(DecType), yside);
 pb->ra_res = rad2deg(x_res);
 pb->dec_res = rad2deg(y_res);
 pb->xside = xside;
 pb->yside = yside;
 pb->zside = zside;
 pb->b = b;
 pb->d = d;

 pb->shape = (double***)calloc(sizeof(double**),zside);
 for( i = 0 ; i < zside ; i++ ){
 pb->shape[i] = (double**)calloc(sizeof(double*), yside);
 for( j = 0; j < yside ; j++) pb->shape[i][j] =  (double*)calloc(sizeof(double), xside);
 }

}

void compute_primary_beam( ProjParType *proj )
{
 double x_res, y_res;
 int xside, yside, zside;
 double ptg_dec, dec_fan;
 double ptg_ra, ra_fan;
 int i, j, ch;
 PrimaryBeamParType *pb = &(proj->pbeam);
 double ra, dec, lambda;
 double rabeam, decbeam;
 double rabeam0, decbeam0;
 double nu, deltanu = proj->bandwidth/proj->chans;
 int tid;
 double b, d;
 double lambda_0 = LIGHTVEL / proj->bandcentre;
 FILE *fpb = fopen("PBEAM.dat", "w");

 ptg_ra = deg2rad( ra2sxg(&(proj->scan[0].ra)) );
 ptg_dec = deg2rad( dec2sxg(&(proj->scan[0].dec)) );
 xside = pb->xside;
 yside = pb->yside;
 zside = pb->zside;
 x_res = deg2rad(pb->ra_res);
 y_res = deg2rad(pb->dec_res);
 b = pb->b;
 d = pb->d;

 dec_fan = deg2rad(4.5)*(proj->ORTelements/PHASE_I) / cos(ptg_dec);
 ra_fan = deg2rad(1.75);

 // Fill the RA range
 j = 0;
 for(i = -xside/2 ; i < xside/2 ; i++)
 { ra = rad2deg(ptg_ra + i*x_res);
   pb->ra[j++] = sxg2ra(ra);
 }

 // Fill the Dec range
 j = 0;
 for( i  = -yside/2 ; i < yside/2 ; i++ )
 { dec = rad2deg(ptg_dec + i*y_res);
   pb->dec[j++] = sxg2dec(dec);
 }

 //#pragma omp parallel for private(tid, nu, lambda, dec, decbeam, i, j, ra, rabeam, pb->shape)
 {
 //Computing the primary beam shape for the given observing co-ordinates
 for( ch = 0 ; ch < zside ; ch++ ) {
 // tid = omp_get_thread_num();
 nu = proj->bandcentre - proj->bandwidth/2 + ch*deltanu + deltanu/2;

 // Making the primary beam frequency-statis
 // Remove the line below
 nu = proj->bandcentre;// - proj->bandwidth/2 + ch*deltanu + deltanu/2;
 // Remove the line above

 lambda = (double)LIGHTVEL/nu;
 
 for( i = 0 ; i < yside ; i++ )
 { dec = (deg2rad(dec2sxg(pb->dec+i)) - ptg_dec);
   //decbeam = pow(sin(M_PI*d*dec/lambda)/(M_PI*d*dec/lambda), 2);
     //decbeam = pow(sin(M_PI*d*cos(ptg_dec)*sin(dec)/lambda)/(M_PI*d*sin(dec)/lambda), 2);
   decbeam = pow(sin(M_PI*d*cos(ptg_dec)*dec/lambda)/(M_PI*d*cos(ptg_dec)*dec/lambda), 2);
   if(i == yside/2) decbeam = 1.0;
   for(j = 0 ; j < xside ; j++ )
   { ra =  (deg2rad(ra2sxg(pb->ra+j)) - ptg_ra) ;
   //rabeam = pow(sin(M_PI*b*cos(dec)*sin(ra)/lambda)/(M_PI*b*cos(dec)*sin(ra)/lambda), 2);
   rabeam = pow(sin(M_PI*b*ra/lambda)/(M_PI*b*ra/lambda), 2);
   //rabeam = exp( - pow(ra/(sqrt(2)*0.3*ra_fan*lambda/lambda_0),2) );
   if(j == xside/2) rabeam = 1.0;
   pb->shape[ch][i][j] = (rabeam*decbeam);
   fprintf(fpb, "%f %f %f\n", dec2sxg(pb->dec+i), ra2sxg(pb->ra+j), pb->shape[ch][i][j]);
      }
   fprintf(fpb, "\n");
 }
 fprintf(finfo, "%s Making primary beam : [ %2d / %2d ] chans\r", info, ch+1, proj->chans);
 }
fprintf(finfo, "%s Making primary beam : [ %2d / %2d ] chans ..done\n", info, ch, proj->chans);
 }
 fclose(fpb);
}


void reconstruct_primary_beam( ProjParType *proj, double *radec )
{
 double x_res, y_res;
 int xside, yside, zside;
 double ptg_dec, dec_fan;
 double ptg_ra, ra_fan;
 int i, j, ch;
 PrimaryBeamParType *pb = &(proj->pbeam);
 double ra, dec, ra_0, dec_0;;

 ra_0 = (double)radec[0];
 dec_0 = (double)radec[1];

 fprintf(finfo, "%s Reconstucting primary beam... ",info);

 ptg_ra = deg2rad( ra2sxg(&(proj->scan[0].ra)) );
 ptg_dec = deg2rad( dec2sxg(&(proj->scan[0].dec)) );
 xside = pb->xside;
 yside = pb->yside;
 zside = pb->zside;

 x_res = deg2rad(pb->ra_res);
 y_res = deg2rad(pb->dec_res);

 dec_fan = deg2rad(dec_0)*(proj->ORTelements/PHASE_I) / cos(ptg_dec);
 ra_fan = deg2rad(ra_0);

 // Fill the RA range
 j = 0;
 for(i = -xside/2 ; i < xside/2 ; i++)
 { ra = rad2deg(ptg_ra + i*x_res);
   pb->ra[j++] = sxg2ra(ra);
 }

 // Fill the Dec range
 j = 0;
 for( i  = -yside/2 ; i < yside/2 ; i++ )
 { dec = rad2deg(ptg_dec + i*y_res);
   pb->dec[j++] = sxg2dec(dec);
 }

 //Computing the primary beam shape for the given observing co-ordinates
 for( ch = 0 ; ch < proj->chans ; ch++ )
 { for( i = 0 ; i < yside ; i++ )
   { for(j = 0 ; j < xside ; j++ )
     { ra =  (deg2rad(ra2sxg(pb->ra+j)) - ptg_ra) ;
       dec = (deg2rad(dec2sxg(pb->dec+i)) - ptg_dec);
       //pb->shape[i][j] = exp( - (ra/(ra_fan/3))*(ra/(ra_fan/3))  - (dec/(dec_fan/3))*(dec/(dec_fan/3)));
       pb->shape[ch][i][j] = cos(M_PI * ra/ra_fan)*cos(M_PI * ra/ra_fan) * cos(M_PI * dec/dec_fan)*cos(M_PI * dec/dec_fan);
     }
   }
 }

 fprintf(finfo, "done\n");
}


int copy_skymap_to_pbeam( ProjParType *proj)
{
 SkyMapType *sky = &(proj->skymap);
 PrimaryBeamParType *pb = &(proj->pbeam);
 int i, j, ch;
 double x_res, y_res;
 int xside, yside;
 double ptg_ra, ptg_dec;

 ptg_ra = deg2rad( ra2sxg(&(proj->scan[0].ra)) );
 ptg_dec = deg2rad( dec2sxg(&(proj->scan[0].dec)) );
 xside = sky->xside;
 yside = sky->yside;
 x_res = deg2rad(sky->ra_res);
 y_res = deg2rad(sky->dec_res);
 pb->xside = sky->xside;
 pb->yside = sky->yside;
 pb->ra_res = sky->ra_res;
 pb->dec_res = sky->dec_res;

 // Copy RA, dec
 for( i = 0 ; i < sky->xside ; i++ ) pb->ra[i] = sky->ra[i];
 for( j = 0 ; j < sky->yside ; j++ ) pb->dec[j] = sky->dec[j];
 //Copy the pbeam
 for(ch = 0 ; ch<proj->chans ; ch++)
 { for ( i = 0 ; i < sky->yside ; i++ )
   { for ( j = 0 ; j < sky->xside ; j++ )  
     pb->shape[ch][i][j] = sky->map[ch][i][j]; 
   }
 }
 return 0;
}


void compute_ionospheric_phase_screen( ProjParType *proj )
{
 double x_res, y_res;
 int xside, yside;
 double ptg_dec, dec_fan;
 double ptg_ra, ra_fan;
 int i, j, ch;
 PrimaryBeamParType *pb = &(proj->pbeam);
 double ra, dec;
 FILE *ftmp = fopen("primary.beam", "w");

 fprintf(finfo, "%s Computing primary beam... ",info);

 ptg_ra = deg2rad( ra2sxg(&(proj->scan[0].ra)) );
 ptg_dec = deg2rad( dec2sxg(&(proj->scan[0].dec)) );
 dec_fan = deg2rad(4.5)*(proj->ORTelements/PHASE_I) / cos(ptg_dec);
 y_res = 2e-4;
 yside = (int) (dec_fan/y_res);
 xside = yside;
 ra_fan = deg2rad(1.75);
 x_res = -ra_fan / xside;

 pb->ra = (RAType*)calloc(sizeof(RAType), xside);
 pb->dec = (DecType*)calloc(sizeof(DecType), yside);
 pb->ra_res = rad2deg(x_res);
 pb->dec_res = rad2deg(y_res);
 pb->xside = xside;
 pb->yside = yside;
 pb->zside = proj->chans;

 // Fill the RA range
 j = 0;
 for(i = -xside/2 ; i < xside/2 ; i++)
 { ra = rad2deg(ptg_ra + i*x_res);
   pb->ra[j++] = sxg2ra(ra);
 }

 // Fill the Dec range
 j = 0;
 for( i  = -yside/2 ; i < yside/2 ; i++ )
 { dec = rad2deg(ptg_dec + i*y_res);
   pb->dec[j++] = sxg2dec(dec);
 }

 //Computing the primary beam shape for the given observing co-ordinates
 pb->shape = (double***)calloc(sizeof(double**),proj->chans);
 for(ch = 0 ; ch < proj->chans ; ch++ )
 { pb->shape[ch] = (double**)calloc(sizeof(double*),xside);
   for( i = 0 ; i < yside ; i++ )
   { pb->shape[ch][i] = (double*)calloc(sizeof(double), xside);
     for(j = 0 ; j < xside ; j++ )
     { ra =  (deg2rad(ra2sxg(pb->ra+j)) - ptg_ra) ;
     dec = (deg2rad(dec2sxg(pb->dec+i)) - ptg_dec);
     //pb->shape[i][j] = exp( - (ra/(ra_fan/3))*(ra/(ra_fan/3))  - (dec/(dec_fan/3))*(dec/(dec_fan/3)));
     pb->shape[ch][i][j] = cos(M_PI * ra/ra_fan)*cos(M_PI * ra/ra_fan) * cos(M_PI * dec/dec_fan)*cos(M_PI * dec/dec_fan);     
     fprintf(ftmp, "%f ", pb->shape[ch][i][j]);
     }
   fprintf(ftmp, "\n");
   //fprintf(finfo, "%s ra %f ra_0 %f", info, ra, ra_fan); getchar();
   }
 }
 fprintf(finfo, "done\n");

}

void free_primary_beam( ProjParType *proj )
{
 PrimaryBeamParType *pb = &(proj->pbeam);

 free(pb->ra);
 free(pb->dec);
 free(pb->shape);
}


void look_through_primary_beam( ProjParType *proj )
{
 int i, j, ch;
 PrimaryBeamParType *pb = &(proj->pbeam);
 SkyMapType *sky = &(proj->skymap);

 fprintf(finfo, "%s Weighting with PRBEAM...", info);
 for(ch = 0; ch < proj->chans; ch++)
 { for ( i = 0 ; i < pb->yside ; i++ )
   { for ( j = 0 ; j < pb->xside ; j++ ) 
     { //if( (i == MAPSIDE/2 + 1 || i == MAPSIDE/2 - 1) && j == MAPSIDE/2) sky->map[i][j] = 5.0; else sky->map[i][j] = 0.0;
	 //if( i == MAPSIDE/2  && j == MAPSIDE/2 ) sky->map[i][j] = 5.0; else sky->map[i][j] = 0.0;
       sky->map[ch][i][j] *= pb->shape[ch][i][j]; 
     }
   }
 }
 fprintf(finfo,"done\n");
}

void correct_primary_beam( ProjParType *proj )
{
 int i, j, ch;
 PrimaryBeamParType *pb = &(proj->pbeam);
 SkyMapType *sky = &(proj->skymap);

 for(ch = 0 ; ch < proj->chans ; ch++)
 { for ( i = 0 ; i < pb->yside ; i++ )
   { for ( j = 0 ; j < pb->xside ; j++ ) sky->map[ch][i][j] /= pb->shape[ch][i][j]; }
 }

}

void coupling_affected_model( ProjParType *proj )
{
 CmplxType alpha;// = ret_z(0.0, 0.5);
 PolarType al;
 double aa;
 double kernel[5];
 int bl, j, ch, M;
 int N = proj->redundant_baselines;
 int convlen = 5;
 double *conv_fn;
 double *h;
 CmplxType tmp;

 al.abs = 0.01; al.arg = M_PI/2;
 alpha = Polar_to_Cmplx( al );
 aa = z_abs(alpha);
 
 fprintf(finfo, "\n%s EM Coupling switched on\n", info);

 kernel[0] = kernel[4] = pow(aa,2);
 kernel[1] = kernel[3] = 2 * real(alpha);
 kernel[2] = 1 + 2*pow(aa,2);

 M = 2*N - 1;

 conv_fn = (double*)calloc(M, sizeof(double));

 conv_fn[N-3] = kernel[0];
 conv_fn[N-2] = kernel[1];
 conv_fn[N-1] = kernel[2];
 conv_fn[N]   = kernel[3];
 conv_fn[N+1] = kernel[4];


 for( bl = 0 ; bl < N ; bl++ )
 { h = conv_fn + N - 1 - bl;
   for( ch = 0 ; ch < proj->chans ; ch++ )
   { tmp = ret_z(0.0, 0.0);
     for(j = 0 ; j < N ; j++ )
      tmp = z_add(tmp, scale_z(h[j], proj->model_vis[ch][j]));
     proj->model_vis[ch][bl] = tmp;
   }
   fprintf(finfo, "%s Coupled Model computed for [ %d / %d ] baselines\r", info, bl+1, proj->redundant_baselines );
 }  
 free(conv_fn);
}

void sky_to_model ( ProjParType *proj )
{

 SkyMapType *sky = &(proj->skymap);
 BaseParType *baseline   = proj->Base;
 double c = LIGHTVEL;
 double v, nu, deltanu=proj->bandwidth/proj->chans;
 int idx;
 long *blindex = proj->blindex;
 double ptg_dec, ra_res, dec_res;
 double m, l;
 int ii, i, j, ch;
 double x1, x2, x3, x4;
 double u;
 double U[proj->redundant_baselines];
 static int NN = 0;
 FILE *fU;
 char Uname[32];
 char num[3];
 double profile = 0.0;
 //double *profile = (double*)calloc(MAPSIDE,sizeof(double));

 fU = fopen("U.dat", "w");

 ra_res = sky->ra_res;
 dec_res = sky->dec_res;
 ptg_dec = dec2sxg( &(proj->scan[0].dec) );

 {
 for( ii = 1; ii < proj->ants ; ii++ )
 { for(ch = 0 ; ch < proj->chans ; ch++)
   { nu = proj->bandcentre - proj->bandwidth/2 + ch*deltanu + deltanu/2;
     v = baseline[blindex[ii]].v * nu / c ;
     idx = blindex[ii];// * proj->chans + ch;
     proj->ULambda[ch][idx] = sqrt(pow(v*cos(deg2rad(ptg_dec)),2));
     u = 0.0;
     for(i = 0; i < sky->yside ; i++)
     { m = sin( deg2rad(dec_res* (i - sky->yside/2)));
       m *= cos( deg2rad(ptg_dec) );
       profile = 0.0;
       for(j=0; j < sky->xside ; j++)
       { //if(sky->map[ch][i][j] > proj->threshold/1000.0) continue;
         profile += sky->map[ch][i][j];
       }
       //l = sin( deg2rad(ra_res* (j - sky->xside/2)));
	 
       /* proj->model_vis[ch][ idx ].r += sky->map[ch][i][j] * cos( 2*M_PI*(l*u + m*v) ); 
	  proj->model_vis[ch][ idx ].i += sky->map[ch][i][j] * sin( 2*M_PI*(l*u + m*v) ); */
       //fprintf(stderr, "%e\n", profile);
       proj->model_vis[ch][idx].r += profile * cos(2*M_PI*m*v);
       proj->model_vis[ch][idx].i += profile * sin(2*M_PI*m*v);
     }
     fprintf(finfo, "%s Model computed for [ %d / %d ] baselines [ %2d / %2d ] chans\r", info, ii, proj->redundant_baselines+1,ch+1,proj->chans );
    }
 }
 }
 //free(profile);
 fclose(fU);
}

int countlines(char *filename)
{
  // count the number of lines in the file called filename                                    
  FILE *fp = fopen(filename,"r");
  int ch=0;
  char line[80];
  int lines=0;

  while( ch != EOF )
  { ch = fgetc(fp);
   if ( ch == '\n') lines++;
  }

  fclose(fp);
  return --lines;
}

void catalog_to_sky( ProjParType *proj, char *catalog)
{
 FILE *fcat;
 char line[200];
 int i;
 double ra, dec, S;
 double src_ra, ptg_ra, src_dec, ptg_dec;

 init_skymap( proj );

 proj->nsrc = countlines(catalog);
 fprintf(finfo, "%s %ld sources in %s\n", info, proj->nsrc, catalog);

 proj->skysrc = (SkySrcType*)calloc(proj->nsrc, sizeof(SkySrcType));
 ptg_ra = deg2rad( ra2sxg(&(proj->scan[0].ra)) );
 ptg_dec = deg2rad( dec2sxg(&(proj->scan[0].dec)) );

 fcat = fopen(catalog, "r");

 for(i = 0; i < proj->nsrc; i++)
 {
   fgets(line, sizeof(line), fcat);
   sscanf(line, "%lf %lf %lf", &ra,  &dec, &S);
   proj->skysrc[i].ra = sxg2ra( ra );
   proj->skysrc[i].dec = sxg2dec( dec );
   src_dec = deg2rad( dec2sxg( &proj->skysrc[i].dec ) );

   proj->skysrc[i].S = 0.001*S;
   proj->skysrc[i].m = sin(src_dec - ptg_dec);     
   proj->skysrc[i].alpha = uniform_noise() + 0.3;
 }

 fclose(fcat);

 proj->pntsrc_swtch = ON;
 proj->galfg_swtch = OFF;

 add_pntsrc_to_galfg( proj );
 look_through_primary_beam( proj );

}

void coupling_effect_on_model( ProjParType *proj )
{
 CmplxType alpha;
 CmplxType q[proj->redundant_baselines];

}


/*
void time_evolve_gains( ProjParType *proj, CmplxType *g )
{
 int i, test, N = proj->ants;
 double eps=0.01;
 static CmplxType propagator[proj->ORTelements];
 
 for(i = 0; i < N; i++)
 { test = signum(uniform_noise());
   propagator[i] = z_add( propagator[i], ret_z(1.0, test*eps) );
   g[i] = z_mul( proj->Ant[i].g, propagator[i] );
 }
}
*/
int count_redundant_baselines( ProjParType *proj)
{
 int i, rbl = 0;
 for( i = 1; i < proj->ORTelements; i++)
 { if(proj->unique_baseline[i] > 1) rbl++;
#ifndef _NO_VERBOSE
   if (proj->unique_baseline[i] == 1) 
   fprintf(finfo, "%s (U,V,W) = (0,%3d,0) : %3d copy\n", info, i, proj->unique_baseline[i]);
   else
   fprintf(finfo, "%s (U,V,W) = (0,%3d,0) : %3d copies\n", info, i, proj->unique_baseline[i]);
#endif
 }
 return rbl;
}

int antid_to_baseline_number(int ant1, int ant2, ProjParType *proj)
{
 AntParType *ant = proj->Ant;
 int baseline_num_start;
 int temp;
 if(ant1 > ant2) 
 { temp = ant1;
   ant1 = ant2;
   ant2 = temp;
 }

 if( !strcmp(proj->receiver, PHASE_I_RX)) baseline_num_start = FITS_BASELINE_START;
 else if( !strcmp(proj->receiver, PHASE_II_RX)) baseline_num_start = FITS_BASELINE_START*2;

 return baseline_num_start*(ant[ant1].id + 1) + (ant[ant2].id) ; 
}

void show_antennas( ProjParType *proj )
{ 
 int ant;
 AntParType *Ant = proj->Ant;
 for( ant = 0 ; ant < proj->ants ; ant++ ) fprintf(finfo, "\n%s Antenna %6d      : %s", info, ant, Ant[ant].name);
}

void show_baselines( ProjParType *proj )
{ 
 int bl;
 AntParType *Ant = proj->Ant;
 BaseParType *base = proj->Base;
 for( bl = 0 ; bl < proj->baselines ; bl++ )
 fprintf(finfo, "\n%s Baseline %5d      : %s - %s", info, bl+1, base[bl].ant1, base[bl].ant2);

}

void make_blofst_lookup_table( ProjParType *proj, double frac_tsys )
{


}

void setpar_baseline( ProjParType *proj )
{
 SysParType system = proj->system;
 int ant1, ant2 ;
 int i = 0 ;
 BaseParType *base = proj->Base;
 AntParType *ant = proj->Ant;
 int num_ants = proj->ants;
 int baseline_num_start;
 double stddev;
 PolarType offset;
 double deltanu = proj->bandwidth / proj->chans;
 const gsl_rng_type *T;
 const gsl_rng *r;
 double frac_tsys = FRAC_TSYS;
 /*
 gsl_rng_default_seed = gen_seed();;
 T = (gsl_rng_type*)gsl_rng_ranlxs2;
 r = (gsl_rng*)gsl_rng_alloc(T);*/
 //stddev     = sqrt( 0.5 * frac_tsys * proj->system.Tsys/(sqrt(deltanu)*system.gain) ) ;
 stddev = sqrt( frac_tsys * proj->system.Tsys / proj->chans );
 if( !strcmp(proj->receiver, PHASE_I_RX)) baseline_num_start = FITS_BASELINE_START;
 else if( !strcmp(proj->receiver, PHASE_II_RX)) baseline_num_start = FITS_BASELINE_START*2;

 proj->unique_baseline = (int*)calloc(proj->ORTelements, sizeof(int) );

 FILE *flog;
 flog = fopen("baselines.info", "w");
 fprintf(finfo, "%s Baselines in %s/baselines.info\n", info, getenv("PWD") );
 fprintf(flog, "###\tAnt1\tAnt2\tFITSbl\tAnt1\tAnt2\n###\n");
 i = 0;
 for(ant1 = 0 ; ant1 < num_ants ; ant1++)
 { for(ant2 = ant1+1 ; ant2 < num_ants ; ant2++)
   { base[i].id1 = ant[ant1].id; base[i].id2 = ant[ant2].id;
     base[i].number = antid_to_baseline_number(ant1, ant2, proj); ; 
     strcpy( (base[i].ant1), (ant[ant1].name) );
     strcpy( (base[i].ant2), (ant[ant2].name) );
     base[i].u = 0.0;
     base[i].v = ant[ant1].bz - ant[ant2].bz;
     base[i].w = 0.0;
     offset.abs = stddev; offset.arg = 2*M_PI*uniform_noise();
     base[i].b = Polar_to_Cmplx( offset ) ;
     fprintf(flog, "%3d\t%2d\t%2d\t%6d\t%s\t%s\t%f\t%f\n", i+1, base[i].id1, base[i].id2, base[i].number, base[i].ant1, base[i].ant2, offset.abs, rad2deg(offset.arg));
     if( proj->unique_baseline[ base[i].id2 - base[i].id1 ] == 0 )   proj->unique_baseline[0] += 1 ;
     proj->unique_baseline[ base[i].id2 - base[i].id1 ] += 1; 
     i++ ;
   }
 }
 fprintf(finfo, "%s Antennas            : %d\n", info, proj->ants);
 proj->redundant_baselines = count_redundant_baselines( proj );
 fprintf(finfo, "%s Baselines           : %ld\n", info, proj->baselines);
 fprintf ( finfo, "%s Unique baselines    : %d\n", info, proj->unique_baseline[0]);
 fprintf ( finfo, "%s Redundant baselines : %d\n", info, proj->redundant_baselines );
 set_baseline_index( proj );

 fclose(flog);
}

void set_baseline_index( ProjParType *proj )
{
 long i, j;
 
 i = 1; 
 j = 0;
 while(i < proj->ORTelements )
     { //if( proj->unique_baseline[i] > 1)
   { proj->blindex[i] = j; j = j+1; }
     //else proj->blindex[i] = 0;
   //fprintf(finfo, "%s blindex [%2d] : %2d\n", info, i, proj->blindex[i]);
   i += 1;
 }
}

void setpar_station(ProjParType *proj)
{
 StationParType *station = &(proj->station);
 station->latitude  = ORT_LAT;
 station->longitude = ORT_LNG;
 station->altitude  = ORT_ALT;
}

void setpar_dipole(ProjParType *proj)
{
 double RF = 326500000.00;
 DipoleParType *dipole = &(proj->dipole);
 dipole->lambda = (float)LAMBDA;
 dipole->length = dipole->lambda/2.0;
 dipole->sep = 0.57 * dipole->lambda;
}

void setpar_system(ProjParType *proj)
{
 SysParType *system = &(proj->system);
 system->gain = GAIN;
 system->bias = 0.0;
 system->Tsys = TSYS;
 system->HPbandwidth = 0.0; /* not used now*/
}

void parse_ProjParFile(ProjParType *proj)
{
 FILE *fProjPar;
 char  str[80];
 char *pwd, *pkgroot;
 char *token;
 char ProjPar[80];
 int num;
 RAType pntg_ra; 
 DecType pntg_dec;

 pwd = getenv("PWD");
 if( (pkgroot = getenv("PKGROOT")) == NULL)
 { fprintf(ferr, "%s PKGROOT undefined. \"source pkgrc.sh\" from package directory.\n", err);
   fprintf(ferr, "%s Ditching altogether.\n", err);
   exit(-1);
 }
 strcpy(ProjPar, "Project.Par"); 
 if( ( fProjPar = fopen(ProjPar, "r") ) == NULL )
   { fprintf(fwarn, "%s Project.Par not found in %s.\n", warn, pwd);
     fprintf(fwarn,"%s Copy from %s and edit!\n", warn, pkgroot);
     strcpy(ProjPar, pkgroot);
     strcat(ProjPar, "/Project.Par");
     if( ( fProjPar = fopen(ProjPar, "r") ) == NULL )
     { fprintf(ferr, "%s Project.Par not found in %s. Stop!\n", err, pkgroot);
       exit(-1);
     }
     fprintf(fwarn, "%s Using default Project.Par\n", warn);
     fprintf(fwarn, "%s from %s!\n",warn, pkgroot);
   }
 else
 { fprintf(finfo, "%s Found Project.Par!\n", info);
 }

 do { fgets ( str, sizeof(str), fProjPar );
 }while( ! (strstr(str, "#PROJPAR")) );

  fgets ( str, sizeof(str), fProjPar );
  token = strtok(str, ":");
  token = strtok(NULL, ": ");
  sscanf(token, "%s", (proj->code) );

  fgets ( str, sizeof(str), fProjPar );
  token = strtok(str, ":");
  token = strtok(NULL, ": ");
  sscanf(token, "%s", (proj->observer) );
 
  fgets ( str, sizeof(str), fProjPar );
  token = strtok(str, ":");
  token = strtok(NULL, ": ");
  sscanf(token, "%s", (proj->obsmode) );
  
  fgets ( str, sizeof(str), fProjPar );
  token = strtok(str, ":");
  token = strtok(NULL, ": ");
  sscanf(token, "%d", &(proj->chans) );
  
  do { fgets ( str, sizeof(str), fProjPar );
     } while( ! (strstr(str, "#SCANPAR")) );
  
  fgets ( str, sizeof(str), fProjPar );
  token = strtok(str, ":");
  token = strtok(NULL, ": ");
  sscanf(token, "%d", &(proj->scans) );
  
  for(num = 0 ; num < proj->scans ; num++ )
  { do { fgets ( str, sizeof(str), fProjPar );
       } while( ! (strstr(str, "#SCAN")) || strstr(str, "#END") );
       
    fgets ( str, sizeof(str), fProjPar );
    token = strtok(str, ":");
    token = strtok(NULL, ": ");
    sscanf(token, "%d", &(proj->scan[num].scan_num) );
    
    fgets ( str, sizeof(str), fProjPar );
    token = strtok(str, ":");
    token = strtok(NULL, ": ");
    sscanf(token, "%s", (proj->scan[num].src) );

    fgets ( str, sizeof(str), fProjPar );
    token = strtok(str, ":");
    token = strtok(NULL, ": ");
    sscanf(token, "%lf", &(proj->scan[num].flux) );

    fgets ( str, sizeof(str), fProjPar );
    token = strtok(str, ":");
    token = strtok(NULL, "h");
    sscanf(token, "%lf", &(pntg_ra.h));
    token = strtok(NULL, "m");
    sscanf(token, "%lf", &(pntg_ra.m));
    token = strtok(NULL, "s");
    sscanf(token, "%lf", &(pntg_ra.s) );
    
    fgets ( str, sizeof(str), fProjPar );
    token = strtok(str, ":");
    token = strtok(NULL, "d");
    sscanf(token, "%lf", &(pntg_dec.d));
    token = strtok(NULL, "'");
    sscanf(token, "%lf", &(pntg_dec.m) );
    token = strtok(NULL, "\"");
    sscanf(token, "%lf", &(pntg_dec.s) );

    proj->scan[0].ra = pntg_ra;
    proj->scan[0].dec = pntg_dec;

  }
 fclose(fProjPar);
}

void parse_VLACalManual(ProjParType *proj)
{
 FILE *fCal;
 char  *calpath, calfile[80];
 char *token, str[80];
 int  gotsrc=0;
 char  name[10], epoch[6], array[2], RA[20], Dec[20];
 RAType ra; 
 DecType dec;

 if( (calpath = getenv("CALPATH")) == NULL)
 { fprintf(fwarn, "%s Set CALPATH to point to <csource.mas>\n", warn);
   fprintf(ferr, "%s Ditching altogether!", err);
 }

 strcpy(calfile, calpath);
 strcat(calfile, "csource.mas");
 if( (fCal = fopen(calfile, "r") ) == NULL )
 { fprintf(ferr, "%s csource.mas not found in %s \n%s Stop!\n", err, calpath, err );
   exit(-1);
 }

 while( ! feof(fCal) )
 { fgets(str, sizeof(str), fCal);
   if( strstr(str, proj->scan[0].src) ) 
   { sscanf(str,"%s %s %s %s %s", name, epoch, array, RA, Dec);
     gotsrc = 1;
     fprintf(finfo, "%s Found source --->%s<--- in VLA Cal Manual\n", info, name);
   }
 }

 if( ! gotsrc ) 
 { fprintf(ferr, "%s %s not found in VLA Cal Manual; quitting\n", err, proj->scan[0].src);
   fprintf(fwarn, "%s Edit source %s in Project.Par\n", warn, proj->scan[0].src);
   fclose(fCal);
   exit(-1);
 }

 token = strtok(RA, "h");
 sscanf(token, "%lf", &(ra.h));
 token = strtok(NULL, "m");
 sscanf(token, "%lf", &(ra.m));
 token = strtok(NULL, "s");
 sscanf(token, "%lf", &(ra.s) );
 
 token = strtok(Dec, "d");
 sscanf(token, "%lf", &(dec.d));
 token = strtok(NULL, "'");
 sscanf(token, "%lf", &(dec.m) );
 token = strtok(NULL, "\"");
 sscanf(token, "%lf", &(dec.s) );
 
 token = strtok(epoch, "J");
 sscanf( token, "%lf",&(proj->scan[0].epoch) );

 proj->nsrc = 1;
 proj->skysrc = (SkySrcType*)calloc( proj->nsrc, sizeof(SkySrcType) );
 proj->scan[0].ra = ra;
 proj->scan[0].dec = dec;
 
 if( within_beam(proj->scan[0].ra, proj->scan[0].dec, ra, dec) )
 { assign(&(proj->skysrc[0]), ra, dec); 
   fprintf(finfo, "%s Flux assigned -->%5.1lf Jy \n", info, proj->skysrc[0].S);
 }
 else
 { fprintf(ferr, "%s Source not inside the beam\n", err);
   fprintf(ferr, "%s Fatal. Stop!\n", err);
   exit(-1);
 }

 fclose(fCal);
}


void set_Cal_flux(ProjParType *proj)
{
 
 proj->nsrc = 1;
 proj->skysrc = (SkySrcType*)calloc( proj->nsrc, sizeof(SkySrcType) );
 assign(&(proj->skysrc[0]), proj->scan[0].ra, proj->scan[0].dec); 
 proj->skysrc[0].S *= proj->scan[0].flux;
 proj->skysrc[0].alpha = 0.0;
 fprintf(finfo, "%s Flux assigned -->%5.1f Jy \n", info, proj->skysrc[0].S);
 
}

void set_skysrc( ProjParType *proj )
{
 double fov, dec_fan;
 double ptg_dec, src_dec;
 int i=0, j, bins=1000;
 int bin=0, cnt = 211234;
 int S_src[211234];
 char temp[20];
 int nelem = proj->ORTelements;
 double wenss_area = 3.8152; //steradian
 double cellsize, mapside;
 char *pkgroot; 
 char wensscat[80];
 FILE *wcat;
 double temp1;
 struct timeval tv;

 if(proj->nsrc == 0 )
 {

 pkgroot = getenv("PKGROOT");
 strcpy(wensscat, pkgroot);
 strcat(wensscat, "/inc/wenss.fluxes");
 wcat = fopen(wensscat, "r");

 for (j = 0; j < cnt ; j++) fscanf(wcat, "%d", S_src+j);
 fclose(wcat);

 ptg_dec = deg2rad( dec2sxg(&(proj->scan[0].dec)) );
 dec_fan = deg2rad(4.5)*(proj->ORTelements/PHASE_I) / cos(ptg_dec);


  if( proj->ORTelements == PHASE_I ) 
  { cellsize = CELLSIZE;  
    mapside = MAPSIDE;
  }

  else if( proj->ORTelements == PHASE_II)
  { cellsize = CELLSIZE*6.0;
    mapside = MAPSIDE;
  }

  fov = cellsize * mapside * cellsize * mapside / (wenss_area*cos(ptg_dec));

 gettimeofday( &tv, NULL);
 srand( tv.tv_usec*1e6  );
  
 proj->nsrc = (long)(cnt*fov);
 proj->skysrc = (SkySrcType*)calloc(proj->nsrc, sizeof(SkySrcType));

 if(proj->pntsrc_swtch) {
 for(i = 0; i < proj->nsrc; i++)
 { src_dec = ptg_dec + cellsize*MAPSIDE*uniform_noise();
   temp1 = rad2deg(cellsize)*MAPSIDE * uniform_noise();
   proj->skysrc[i].ra = sxg2ra( ra2sxg(&(proj->scan[0].ra)) + temp1 ) ;
   proj->skysrc[i].dec = sxg2dec(rad2deg(src_dec));
   
   j = (int)((uniform_noise() + 0.5)*cnt) - 1;
   proj->skysrc[i].S = 0.001*(double)S_src[j];
   proj->skysrc[i].m = sin(ptg_dec - src_dec);     
   proj->skysrc[i].alpha = 0.0;//uniform_noise() + 0.3;
 } }

 }
 else
 {
  if(proj->nsrc == 1) 
       fprintf(finfo, "%s Seeding %ld source randomly within the primary beam\n",    info, proj->nsrc);
  else fprintf(finfo, "%s Seeding %ld sources randomly within the primary beam\n", info, proj->nsrc);
  proj->skysrc = (SkySrcType*)calloc(proj->nsrc, sizeof(SkySrcType));
  if( proj->pntsrc_swtch ) {
  for( i = 0; i < proj->nsrc; i++){ associate( &(proj->skysrc[i]), proj->scan[0].ra, proj->scan[0].dec, nelem );
      proj->skysrc[i].alpha = 0.7; }
                           }
 }
}

int init_proj(ProjParType *proj)
{
 char antstr[MAX_ANTS][80];
 int Rx_Ver = get_RxVer ( antstr );
 int i;

 if( Rx_Ver == PHASE_I ) strcpy(proj->receiver, PHASE_I_RX);
 else if ( Rx_Ver == PHASE_II ) strcpy(proj->receiver, PHASE_II_RX);

 proj->ORTelements = Rx_Ver;
 proj->ants = get_num_ants( antstr ) ;
 proj->baselines = proj->ants * (proj->ants - 1) / 2;
 proj->Ant = (AntParType*) calloc( proj->ants, AntParSize ) ;
 proj->Base = (BaseParType*) calloc( proj->baselines, BaseParSize );
 proj->pols    = 1;
 proj->sidebands = 1;
 proj->bandwidth = BANDWIDTH;
 proj->bandcentre = BAND_CENTRE;
 proj->mjd_start = mjdnow();
 proj->tick = 1.0; /*seconds */

 parse_ProjParFile(proj);

 setpar_dipole( proj );
 setpar_system( proj );
 setpar_station( proj );

 setpar_ant( proj, antstr ) ;
 setpar_baseline( proj );

 proj->ULambda = (double**)calloc(proj->chans, sizeof(double*));
 proj->model_vis = (CmplxType**)calloc(proj->chans, sizeof(CmplxType*) );
 proj->MV_errbar = (CmplxType**)calloc(proj->chans, sizeof(CmplxType*) );
 for(i = 0 ; i < proj->chans ; i++)  
 { proj->model_vis[i] = (CmplxType*)calloc( proj->redundant_baselines+1, sizeof(CmplxType) );
   proj->ULambda[i] = (double*)calloc(proj->redundant_baselines+1, sizeof(double));
   proj->MV_errbar[i] = (CmplxType*)calloc( proj->redundant_baselines+1, sizeof(CmplxType) );
 }

 //make_bandpass_filter( proj, proj->chans );
 flash_obs_settings( proj );

 proj->pcount = RGParSize / sizeof(double);
 proj->recwords = proj->pcount + 3 * ( proj->pols * proj->sidebands * proj->chans );
 proj->RecBuf = (double*)calloc( proj->baselines * proj->recwords, sizeof(double) ) ;
 proj->SNR = (double*)calloc(proj->baselines, sizeof(double));
 proj->sigma = (double*)calloc(proj->baselines, sizeof(double));

 init_SNR_compute( proj );

 /*Dummy alloc, otherwise kill_proj(proj) will crib if 
   solvers have not been called earlier. 
   Now genFITSmain, which doesn't call "_solve()", runs peacefully*/
 dummy_gsl_alloc(proj);

 proj->window = (double*)calloc(proj->chans, sizeof(double));

 blackman_nuttall( proj->window, proj->chans );

 proj->delphi = (double*)calloc(proj->ants, sizeof(double) ) ;
 for( i = 0 ; i < proj->ants ; i++ ) proj->delphi[i] = 0.0;//10*M_PI*uniform_noise()/proj->chans;
 return 1;
}


void add_HI_signal( ProjParType *proj )
{
 HIMapType *HI = &(proj->HI);
 SkyMapType *sky = &(proj->skymap);
 int i, j, ch;
 fprintf(finfo, "%s Embedding the point source foreground map ...", info);
 for(ch = 0; ch < proj->chans; ch++)
 { for ( i = 0 ; i < HI->yside ; i++ )
   { for ( j = 0 ; j < HI->xside ; j++ ) 
     { sky->map[ch][i][j] += HI->map[ch][i][j]; 
     }
   }
 }
 fprintf(finfo,"done\n");

}

void embed_EPS_in_galfg( ProjParType *proj )
{
 int i, j, ch;
 EPSFGType *eps = &(proj->eps);
 SkyMapType *sky = &(proj->skymap);
 GalFGType *gal = &(proj->fg);
 double deltanu = proj->bandwidth / proj->chans;
 double nu;
 // Copy FG Map within primary beam to Sky Map
 if( proj->galfg_swtch ) { 
 fprintf(finfo, "%s Laying the diffuse foreground map ...", info);
 for( ch = 0 ; ch < proj->chans ; ch++ ){
 for ( i = 0 ; i < sky->yside ; i++ )
 { for ( j = 0 ; j < sky->xside ; j++ ) sky->map[ch][i][j] = gal->map[ch][i][j]; }
 }                      
 fprintf(finfo,"done\n");
 }


 //Add point source map to diffuse FG map
 if( proj->pntsrc_swtch ) { 
 fprintf(finfo, "%s Embedding the point source foreground map ...", info);
 for(ch = 0; ch < proj->chans; ch++)
 { for ( i = 0 ; i < eps->yside ; i++ )
   { for ( j = 0 ; j < eps->xside ; j++ ) 
     { sky->map[ch][i][j] += eps->map[ch][i][j]; 
     }
   }
 }
 fprintf(finfo,"done\n");
 }
 /*
 if( proj->nsrc == 1 )
 { 
  for(ch = 0; ch < proj->chans; ch++)
 {  nu = proj->bandcentre - proj->bandwidth/2 + ch*deltanu + deltanu/2;
    sky->map[ch][sky->yside/2][sky->xside/2] = pow(proj->bandcentre/nu,proj->skysrc[0].alpha) * proj->skysrc[0].S;
 }
  }
 */
}

void init_sky_signal( ProjParType *proj )
{
 char filename[80];

 if( proj->preint > 0 )
 { if(proj->nsrc == -1)
   { set_Cal_flux( proj );
       //gen_V_sky( proj );
   }
     /*
   else
   { set_skysrc( proj );
       //gen_pntsrc( proj );
       //gen_V_sky( proj );
       }*/
 }

 // Compute the primary beam
 init_primary_beam( proj );
 if( proj->pbeam_swtch )
 { 
   strcpy(filename, "PRBEAM.FITS\0");
   read_FITSmap( proj, filename);
   newline();
   copy_skymap_to_pbeam( proj );
 }
 else
  compute_primary_beam( proj );
 
 // Generate the Galactic Foreground
if( proj->galfg_swtch ) {
     //gen_gal_pol_map( proj );
     //gen_pol_PA( proj );
     //gen_galfg_pol_map(proj);
    gen_gal_fg(proj);
     //resize_fgmap( proj );    
    /*
     getchar();
     write_pol_Qmap2FITS( proj );
     write_pol_Umap2FITS( proj );
    */
 }
 
 if( proj->pntsrc_swtch )
 {
  read_EPS_map( proj );
 }
 
 // Generate the Ionospheric TEC screen
 //gen_TEC_scr(proj);
 //resize_tecmap( proj );

 // Integrated sky Map
 init_skymap( proj );
 embed_EPS_in_galfg( proj );
 add_pntsrc_to_galfg( proj );
 
 if( proj->h1_swtch )
 {   read_HI_map( proj );
     add_HI_signal( proj );
 }
 
 // Weight with primary beam
 look_through_primary_beam( proj );
}

void free_sky_signal( ProjParType *proj )
{
 free_primary_beam( proj );
 if( proj->galfg_swtch) free_gal_fgmap( proj );
 free_skymap( proj );
}

void init_gain_table( ProjParType *proj )
{
 GainTableType *gaintab = &(proj->gtab);
 int row;

 gaintab->flag = 0; /*not used */
 gaintab->nrow = proj->nrecs+3; // First row is for absolute gain
 gaintab->nant = proj->ants ;
 gaintab->nstokes = proj->pols;
 gaintab->rowdone = 0;
 gaintab->row = (GainTableRow*)calloc( gaintab->nrow, sizeof(GainTableRow) );
 for(row = 0 ; row < gaintab->nrow ; row++ )
 { gaintab->row[row].gain = (Cmplx3Type*)calloc( gaintab->nant * gaintab->nstokes, sizeof(Cmplx3Type) ); }

}

void close_gain_table( ProjParType *proj )
{
 GainTableType *gaintab = &(proj->gtab);
 free(gaintab->row);
}


void bloat_gaintab( ProjParType *proj )
{
 GainTableType *gaintab = &(proj->gtab);
 GainTableRow *gtrow = gaintab->row;
 int row, ant;
 int start_row = gaintab->rowdone;
 for(row = start_row; row < proj->preint ; row++) 
 { for(ant = 0 ; ant < gaintab->nant ; ant++) 
   { gtrow[row].gain[ant].r = gtrow[0].gain[ant].r;
     gtrow[row].gain[ant].i = gtrow[0].gain[ant].i;
   }
 gaintab->rowdone++;
 if( row % 100 == 0 ) fprintf(finfo, "%s Gain Table          : %d records bloated\r", info, row);
 }
 fprintf(finfo, "%s Gain Table          : %d records bloated\n", info, row);
}

void write_gaintab( ProjParType *proj )
{
 int ant = 0;
 GainTableType *gaintab = &(proj->gtab);
 GainTableRow *gtrow = gaintab->row;
 CmplxType gain;
 int row = gaintab->rowdone;

 for(ant = 0 ; ant < gaintab->nant ; ant++)
 { gtrow[row].gain[ant].r  = proj->sol_re->data[ant];
   gtrow[row].gain[ant].i  = proj->sol_im->data[ant];
   gain = ret_z(gtrow[row].gain[ant].r, gtrow[row].gain[ant].i);
   gtrow[row].gain[ant].wt = 1.0;
   //fprintf(finfo, "%s Ant %d : %lf + i %lf\n", info, ant, gain.r, gain.i);
 }
  gaintab->rowdone++;
}

void write_gainfile( ProjParType *proj, FILE *fgain )
{
 GainTableType *gaintab = &(proj->gtab);
 GainTableRow *gtrow = gaintab->row;
 int ant, row;

 for( row = 0 ; row < gaintab->nrow ; row++ ) 
 { for(ant = 0 ; ant < gaintab->nant ; ant++)
   fprintf(fgain, "%f %f ",hypotf(gtrow[row].gain[ant].r, gtrow[row].gain[ant].i), rad2deg(atan2f(gtrow[row].gain[ant].i, gtrow[row].gain[ant].r)) );
   fprintf(fgain, "\n");
 }
}


void write_bpassfile( ProjParType *proj, FILE *fbpass )
{
 BPassTableType *bpasstab = &(proj->bptab);
 BPassTableRow *bptrow = bpasstab->row;
 int ant, row, ch;

 for( row = 0 ; row < bpasstab->nrow ; row++ ) 
 { for(ch = 0 ; ch < bpasstab->nchan; ch++ )
   { for(ant = 0 ; ant < bpasstab->nant ; ant++)
     fprintf(fbpass, "%f %f ",hypotf(bptrow[row].bpass[ant*bpasstab->nchan + ch].r, bptrow[row].bpass[ant*bpasstab->nchan + ch].i), rad2deg(atan2f(bptrow[row].bpass[ant*bpasstab->nchan + ch].i, bptrow[row].bpass[ant*bpasstab->nchan + ch].r)) );
     fprintf(fbpass, "\n");
   }
     fprintf(fbpass, "\n");
 }
}

void init_bandpass_table( ProjParType *proj )
{
 BPassTableType *bpasstab = &(proj->bptab);
 int row;
 bpasstab->flag = 0; /*not used */
 bpasstab->nrow = proj->nrecs + 1;
 bpasstab->nchan = proj->chans ;
 bpasstab->nant = proj->ants;
 bpasstab->nstokes = proj->pols;
 bpasstab->rowdone = 0;
 bpasstab->row = (BPassTableRow*)calloc( bpasstab->nrow, sizeof(BPassTableRow) );
 for(row = 0 ; row < bpasstab->nrow ; row++ )
 { bpasstab->row[row].bpass = (Cmplx3Type*)calloc( bpasstab->nant * bpasstab->nstokes * bpasstab->nchan, sizeof(Cmplx3Type) ); }
}

void close_bandpass_table( ProjParType *proj )
{
 BPassTableType *bpasstab = &(proj->bptab);
 free( bpasstab->row );
}

void bloat_bpasstab( ProjParType *proj )
{
 BPassTableType *bpasstab = &(proj->bptab);
 BPassTableRow *bptrow = bpasstab->row;
 int row, ant, ch;
 int start_row = bpasstab->rowdone;
 for( row = start_row ; row < proj->preint ; row++ ) 
 { for ( ch = 0 ; ch < proj->chans ; ch++ )
    { for( ant = 0 ; ant < bpasstab->nant ; ant++ )
      { bptrow[row].bpass[ant*bpasstab->nchan + ch].r  = bptrow[0].bpass[ant*bpasstab->nchan + ch].r;
        bptrow[row].bpass[ant*bpasstab->nchan + ch].i  = bptrow[0].bpass[ant*bpasstab->nchan + ch].i;
	bptrow[row].bpass[ant*bpasstab->nchan + ch].wt = bptrow[0].bpass[ant*bpasstab->nchan + ch].wt;
      }
    }
     bpasstab->rowdone++;
     if( row % 100 == 0 ) fprintf(finfo, "%s Bandpass Table      : %d records bloated\r", info, row);
 }
  fprintf(finfo, "%s Bandpass Table      : %d records bloated\n", info, row);
}

void write_bpasstab( ProjParType *proj )
{
 static int chan = 0;
 int ch = proj->chan2cal;
 int ant = 0;
 GainTableType *gaintab = &(proj->gtab);
 GainTableRow *gtrow = gaintab->row;
 BPassTableType *bpasstab = &(proj->bptab);
 BPassTableRow *bptrow = bpasstab->row;
 CmplxType bpass, gain, z;
 int row = bpasstab->rowdone;

 for(ant = 0 ; ant < bpasstab->nant ; ant++)
 { gain = ret_z(gtrow[row].gain[ant].r, gtrow[row].gain[ant].i);
   z = ret_z(proj->sol_re->data[ant], proj->sol_im->data[ant]);

   bpass = z_div(z, gain);
   
   bptrow[row].bpass[ant*bpasstab->nchan + chan].r  = real(bpass);
   bptrow[row].bpass[ant*bpasstab->nchan + chan].i  = imag(bpass);
   bptrow[row].bpass[ant*bpasstab->nchan + chan].wt = 1.0;
 }
 chan++;
 if( (chan % bpasstab->nchan) == 0){ bpasstab->rowdone++; chan = 0;}
}

 /*
void write_bpasstab( ProjParType *proj )
{
 static int row = 0, ch = 0;
 int ant = 0;
 GainTableType *gaintab = &(proj->gtab);
 GainTableRow *gtrow = gaintab->row;
 BPassTableType *bpasstab = &(proj->bptab);
 BPassTableRow *bptrow = bpasstab->row;
 CmplxType bpass, gain, z;

 for(ant = 0 ; ant < bpasstab->nant ; ant++)
 { gain = ret_z(gtrow[row].gain[ant].r, gtrow[row].gain[ant].i);
   z = ret_z(proj->sol_re->data[ant], proj->sol_im->data[ant]);
   bpass = z_div(z, gain);
   bptrow[row].bpass[ant*bpasstab->nchan + ch].r  = real(bpass);
   bptrow[row].bpass[ant*bpasstab->nchan + ch].i  = imag(bpass);
   bptrow[row].bpass[ant*bpasstab->nchan + ch].wt = 1.0;
   fprintf(finfo, "%s Row: %3d Ant : %2d Channel  0  gain    : %3.1f /_ %3.1f\n", info, row, ant, z_abs(gain), rad2deg(z_arg(gain)) );
   fprintf(finfo, "%s Row: %3d Ant : %2d Channel %2d gain    : %3.1f /_ %3.1f\n", info, row, ant, ch, z_abs(z), rad2deg(z_arg(z)) );
   fprintf(finfo, "%s Row: %3d Ant : %2d Chan: %2d Bandpass : %3.1f /_ %3.1f\n", info, row, ant, ch, z_abs(bpass), rad2deg(z_arg(bpass)) );
 }
 getchar();
 ch++;
 if( (ch % bpasstab->nchan) == 0){ row++; ch = 0; }
}
*/
void reproj( ProjParType *outproj, ProjParType *inproj )
{
 int ch;

 outproj->ORTelements = inproj->ORTelements;
 outproj->ants = inproj->ants;
 outproj->baselines = inproj->baselines;
 outproj->Ant = (AntParType*) calloc( outproj->ants, AntParSize ) ;
 outproj->Base = (BaseParType*) calloc( outproj->baselines, BaseParSize );
 outproj->pols    = inproj->pols;
 outproj->sidebands = inproj->sidebands;
 outproj->bandwidth = inproj->bandwidth;
 outproj->bandcentre = inproj->bandcentre;
 outproj->mjd_start = inproj->mjd_start;
 outproj->tick = inproj->tick;
 strcpy( outproj->receiver, inproj->receiver );
 strcpy( outproj->code, inproj->code );
 strcpy( outproj->observer, inproj->observer );
 strcpy(outproj->obsmode,inproj->obsmode );

 memcpy( &(outproj->system), &(inproj->system), sizeof(SysParType) );
 memcpy( &(outproj->station), &(inproj->station), sizeof(StationParType) );
 memcpy( &(outproj->dipole), &(inproj->dipole), sizeof(DipoleParType) );
 memcpy( outproj->Ant, inproj->Ant, outproj->ants*sizeof(AntParType) );
 setpar_baseline( outproj );

 outproj->pcount = RGParSize / sizeof(double);
 outproj->recwords = outproj->pcount + 3 * ( outproj->pols * outproj->sidebands * outproj->chans );
 outproj->RecBuf = (double*)calloc( outproj->baselines * outproj->recwords, sizeof(double) ) ;
 outproj->SNR = (double*)calloc(outproj->baselines, sizeof(double));
 outproj->sigma = (double*)calloc(outproj->baselines, sizeof(double));
 outproj->nsrc = inproj->nsrc;
 outproj->scans = inproj->scans;
 memcpy(outproj->scan, inproj->scan, outproj->scans*sizeof(ScanParType) );
 /* Dummy filter allocation*/
 outproj->H_w = (CmplxType *)calloc(1, sizeof(CmplxType));
 outproj->model_vis = (CmplxType**)calloc(outproj->chans, sizeof(CmplxType*));
 for(ch = 0 ; ch < outproj->chans; ch++)
 outproj->model_vis[ch] = (CmplxType*)calloc( outproj->redundant_baselines /* outproj->chans*/, sizeof(CmplxType) );

 init_SNR_compute( outproj );

 dummy_gsl_alloc(outproj);
 flash_obs_settings( outproj );
}

void reband( ProjParType *outproj, ProjParType *inproj )
{
 int i, bl, ch;
 RGParType *inuvw, *outuvw;
 Cmplx3Type *invis, *outvis;
 int navg = inproj->chans / outproj->chans ;

 for ( bl = 0 ; bl < inproj->baselines ; bl++ )
 { inuvw = (RGParType *) (inproj->RecBuf + bl*(inproj->recwords) );
   outuvw = (RGParType *) (outproj->RecBuf + bl*(outproj->recwords) );

   memcpy(outuvw, inuvw, RGParSize);

   invis = (Cmplx3Type *) (inproj->RecBuf + bl*(inproj->recwords) + inproj->pcount);
   outvis = (Cmplx3Type *) (outproj->RecBuf + bl*(outproj->recwords) + outproj->pcount);

   ch = 0;

   while( ch < outproj->chans )
   { outvis[ch].r  = 0.0;
     outvis[ch].i  = 0.0;
     outvis[ch].wt = 0.0;

     for ( i = 0 ; i < navg ; i++ )
     { outvis[ch].r  += invis[navg*ch + i].r;
       outvis[ch].i  += invis[navg*ch + i].i;
       outvis[ch].wt += invis[navg*ch + i].wt;
     }
     outvis[ch].r  /= navg;
     outvis[ch].i  /= navg;
     outvis[ch].wt /= navg;
     ch++;
   }
 }
  
}

void make_bandpass_filter( ProjParType *proj, int taps )
{

 double *weights, *desired, *bands, *h, *temp;
 CmplxType *H;
 int i, fwd = 1;
 int num_bands = 3;
 FILE *fBP = fopen("bandpass.filter", "w");
 
 bands = (double *)calloc(2*num_bands, sizeof(double));
 weights = (double *)calloc(num_bands, sizeof(double));
 desired = (double *)calloc(num_bands, sizeof(double));
 
 h = (double *)calloc(2*proj->chans, sizeof(double));
 temp = (double *)calloc(2*proj->chans, sizeof(double));
 proj->H_w = (CmplxType *)calloc(proj->chans, sizeof(CmplxType));
 H = proj->H_w;
 
 desired[0] = 0;
 desired[1] = 1; 
 desired[2] = 0;
 
 weights[0] = 10;
 weights[1] = 1;
 weights[2] = 10;
 
 bands[0] = 0;
 bands[1] = 0.01;
 bands[2] = 0.05;
 bands[3] = 0.45;
 bands[4] = 0.49;
 bands[5] = 0.5;
 
 remez((float*)h, taps, num_bands, (float*)bands, (float*)desired, (float*)weights, BANDPASS);
 memcpy(temp, h, 2*proj->chans*sizeof(double));
 realft((float*)temp - 1, (unsigned long)(2 * proj->chans), fwd);
 fprintf(finfo, "%s Spectral Shape in %s/bandpass.filter\n", info, getenv("PWD") );
 fprintf( fBP, "#\tEven(h)\tOdd(h)\tReal(H)\tImag(H)\tAbs(H)\targ(H)\n#\n" );
 for(i = 0; i < proj->chans; i++)
 { H[i].r = temp[2*i + 0]; H[i].i = temp[2*i + 1];  H[0].i = 0.0;
   fprintf( fBP, "%d\t%4.3f\t%4.3f\t%4.3f\t%4.3f\t%4.3f\t%4.3f\n", 
	    i, h[2*i + 0], h[2*i + 1], H[i].r, H[i].i, z_abs(H[i]), z_arg(H[i]) );
 }
 free(bands);
 free(weights);
 free(desired);
 free(temp);
 free(h);
 fclose(fBP);
}

void baseline_number_to_ant_ids(BaseParType *baseline, int baseline_num_start)
{
 int ant1 = 0, ant2 = 0;
 int num = baseline->number - baseline_num_start;
 
 while(num >baseline_num_start)
 { ant1 += 1 ;
   num = num - baseline_num_start;
 }
 ant2 = num ;

 baseline->id1 = ant1;
 baseline->id2 = ant2;

}

void index_ants( ProjParType *proj )
{
 int i, ant, bl;
 AntParType *Ant = proj->Ant;
 BaseParType *baseline = proj->Base;

 proj->Ant[0].id = proj->Base[0].id1;
 proj->Ant[1].id = proj->Base[0].id2;
 for( i = 2 ; i < proj->ants ; i++ ) proj->Ant[i].id = proj->Base[i-1].id2;

 for( bl = 0 ; bl < proj->baselines ; bl++ )
 { 
   for(ant = 0; ant < proj->ants; ant++)
   { if( baseline[bl].id1 == Ant[ant].id ) strcpy(baseline[bl].ant1, Ant[ant].name);
     if( baseline[bl].id2 == Ant[ant].id ) strcpy(baseline[bl].ant2, Ant[ant].name);
   }
 }
}

void dummy_gsl_alloc( ProjParType *proj )
{
 proj->A           = gsl_matrix_calloc(1,1);
 proj->B           = gsl_matrix_calloc(1,1);

 proj->QR          = gsl_matrix_calloc(1,1);
 proj->tau         = gsl_vector_calloc(1);
 proj->QRres       = gsl_vector_calloc(1);

 proj->U           = gsl_matrix_calloc(1,1);
 proj->V           = gsl_matrix_calloc(1,1);
 proj->S           = gsl_vector_calloc(1);

 proj->sol_re      = gsl_vector_calloc(1);
 proj->sol_im      = gsl_vector_calloc(1);

 proj->prev_sol_re = gsl_vector_calloc(1);
 proj->prev_sol_im = gsl_vector_calloc(1);

 proj->d_sol_re    = gsl_vector_calloc(1);
 proj->d_sol_im    = gsl_vector_calloc(1);

 proj->R_re        = gsl_vector_calloc(1);
 proj->R_im        = gsl_vector_calloc(1);

 proj->Q_re        = gsl_vector_calloc(1);
 proj->Q_im        = gsl_vector_calloc(1);

 proj->delta_sol   = gsl_vector_calloc(1);
 proj->delta_V_obs = gsl_vector_calloc(1);
}

void all_gsl_free( ProjParType *proj)
{
 gsl_matrix_free(proj->A);
 gsl_matrix_free(proj->B);

 gsl_matrix_free(proj->QR);
 gsl_vector_free(proj->tau);
 gsl_vector_free(proj->QRres);

 gsl_matrix_free(proj->U);
 gsl_matrix_free(proj->V);
 gsl_vector_free(proj->S);

 gsl_vector_free(proj->sol_re);
 gsl_vector_free(proj->sol_im);

 gsl_vector_free(proj->prev_sol_re);
 gsl_vector_free(proj->prev_sol_im);

 gsl_vector_free(proj->d_sol_re);
 gsl_vector_free(proj->d_sol_im);

 gsl_vector_free(proj->R_re);
 gsl_vector_free(proj->R_im);

 gsl_vector_free(proj->Q_re);
 gsl_vector_free(proj->Q_im);

 gsl_vector_free(proj->delta_sol);
 gsl_vector_free(proj->delta_V_obs);

 gsl_matrix_free(proj->V_re);
 gsl_matrix_free(proj->V_im);

}

void kill_proj( ProjParType *proj )
{
 free(proj->Ant);
 free(proj->Base);
 free(proj->RecBuf);
 free(proj->SNR);
 free(proj->sigma);
 //free(proj->H_w);
 free(proj->model_vis);
 free(proj->unique_baseline);
 //if(proj->V2_init_done) free_V2( proj );
 //free(proj->delphi);
 //free(proj->skysrc);
 all_gsl_free(proj);
}

void extract_channel( ProjParType *proj, int recno )
{
 int ch = proj->chans / 2;
 int bl;
 Cmplx3Type *vis;

 for ( bl = 0 ; bl < proj->baselines ; bl++ )
 { vis = (Cmplx3Type*) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);
   gsl_matrix_set(proj->V_re, bl, recno, vis[ch].r);
   gsl_matrix_set(proj->V_im, bl, recno, vis[ch].i);
 }
}

void init_SNR_compute( ProjParType *proj )
{
 proj->preint = max(proj->preint, 1);
 proj->V_re = gsl_matrix_calloc( proj->baselines, proj->preint );
 proj->V_im = gsl_matrix_calloc( proj->baselines, proj->preint );
}

/*
void compute_RMS( ProjParType *proj, int integ )
{
 int bl;
 int str = 1;
 gsl_vector *real, *imag;
 double s_re, s_im;
 int i = 0;

 real = gsl_vector_calloc(integ);
 imag = gsl_vector_calloc(integ);

 for(bl = 0 ; bl < proj->baselines ; bl++)
 { gsl_matrix_get_row( real, proj->V_re, bl);
   gsl_matrix_get_row( imag, proj->V_im, bl);
   if(integ != 1)
   { proj->sigma_re->data[bl] = gsl_stats_sd( real->data, str, real->size);
     proj->sigma_im->data[bl] = gsl_stats_sd( imag->data, str, imag->size);
   }
   else
   { proj->sigma_re->data[bl] = real->data[0];
     proj->sigma_im->data[bl] = imag->data[0];
   }
 }
 gsl_vector_free( real );
 gsl_vector_free( imag );
}
*/
void compute_SNR( ProjParType *proj )
{
 int bl;
 int str = 1;
 gsl_vector *real, *imag;
 int i = 0;
 gsl_vector *V;
 double mean, rms;
 int integ = proj->preint;

 real = gsl_vector_calloc(integ);
 imag = gsl_vector_calloc(integ);

 V = gsl_vector_calloc(integ);

 for(bl = 0 ; bl < proj->baselines ; bl++)
 { gsl_matrix_get_row( real, proj->V_re, bl);
   gsl_matrix_get_row( imag, proj->V_im, bl);

   for( i = 0 ; i < integ ; i++ )  
   { V->data[i] = hypotf( real->data[i], imag->data[i] ); 
     //if ( bl == 25 ) fprintf(finfo, "%d %f\n", i, V->data[i]);
   }
   rms = gsl_stats_sd(V->data, str, V->size);
   mean = gsl_stats_mean(V->data, str, V->size);
   proj->sigma[bl] = (double)rms / sqrt ( (double) integ );
   //fprintf(finfo, "%s %d %f %f\n", info, bl, mean, rms);
   //proj->sigma[bl] = (double)rms;
   proj->SNR[bl] = mean / proj->sigma[bl] ;
 }

 gsl_vector_free( real );
 gsl_vector_free( imag );
 gsl_vector_free( V );

}

int genVis( ProjParType *proj )
{
 int bl, ch, i=0, j=0, n, count = proj->preint;
 int len = proj->recwords * proj->baselines;
 long *blindex = proj->blindex;
 double stddev;
 double iatutc = IATUTC;
 double c = LIGHTVEL;
 double deltanu = proj->bandwidth / proj->chans;
 BaseParType *baseline;
 SkyMapType *sky = &(proj->skymap);
 SysParType system = proj->system;
 RGParType *uvw;
 Cmplx3Type *vis;
 double phi;
 double JD;
 double date1, date2;
 double *accBuf, *snrBuf;
 CmplxType V_ij, gain, signal, V_s, noise, H_w;
 double ptg_dec, ra_res, dec_res;
 double m, nu, v;
 PolarType phasor;
 //CmplxType alpha = ret_z(0.0070710678118654752440, -0.0070710678118654752440);
 CmplxType alpha = ret_z(0.0, 0.0);
 CmplxType g_i, g_j, blofst;
 CmplxType gain1, gain2;
 const gsl_rng_type *T;
 const gsl_rng *r;

 static long recnum;

 accBuf = (double*)calloc(len, sizeof(double));
 snrBuf = (double*)calloc(proj->baselines, sizeof(double));
 
 gsl_rng_default_seed = gen_seed();;
 T = (gsl_rng_type*)gsl_rng_ranlxs2;
 r = (gsl_rng*)gsl_rng_alloc(T);

 baseline   = proj->Base;
 //stddev     = sqrt( 0.5 * proj->system.Tsys/(sqrt(deltanu)*system.gain) ) ;
 stddev     = proj->system.Tsys/(sqrt(2*deltanu)*system.gain);
 JD         = (proj->mjd_start + (proj->tick)*recnum/86400.0 + 2400000.5) ;
 date1      = floor(JD);
 date2      = JD - date1 + (iatutc / 86400.0);

 while(count > 0){
 for ( bl = 0 ; bl < proj->baselines ; bl++ )
 { uvw = (RGParType *) (proj->RecBuf + bl*(proj->recwords) );
   vis = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);
   uvw->u        = baseline[bl].u / c;
   uvw->v        = baseline[bl].v / c;
   uvw->w        = baseline[bl].w / c;
   uvw->baseline = baseline[bl].number;
   uvw->date1    = (double)date1;
   uvw->date2    = (double)date2;
   uvw->su       = 1.0;
   uvw->freq     = 1.0;

   for ( ch = 0 ; ch < proj->chans ; ch++ )
    {
   
   /******** Accounting for the longest baseline *********
   if( baseline[bl].id2 - baseline[bl].id1 == proj->ORTelements-1 )
   { ra_res = sky->ra_res;
     dec_res = sky->dec_res;
     ptg_dec = dec2sxg( &(proj->scan[0].dec) );
     nu = proj->bandcentre - proj->bandwidth/2 + ch*deltanu + deltanu/2;
     v = baseline[proj->ORTelements-1].v * nu / c ;

     for(i = 0; i < sky->yside ; i++)
     { for(j=0; j < sky->xside ; j++)
       { m = sin( deg2rad(dec_res* (i - sky->yside/2)));
	 V_s.r += 1.0 * sky->map[ch][i][j] * cos( 2*M_PI*m*v ); 
	 V_s.i += 1.0 * sky->map[ch][i][j] * sin( 2*M_PI*m*v ); 
       }
     }
   }
   
   ***** Regular baselines, redundant ********
   else*/
   V_s = proj->model_vis[ch][ (blindex[ baseline[bl].id2 - baseline[bl].id1 ]) ];

   //fprintf(finfo, "%s Model_vis : %f %f\n", info, V_s.r, V_s.i);

   for(n = 0; n < proj->ants; n++)
   { if( baseline[bl].id1 == proj->Ant[n].id ) i = n;
     if( baseline[bl].id2 == proj->Ant[n].id ) j = n;
   }

     noise.r = gsl_ran_gaussian(r, stddev);
     noise.i = gsl_ran_gaussian(r, stddev);
     noise = ret_z(0.0, 0.0);
     
     /*
     H_w.r = proj->H_w[ch].r;
     H_w.i = proj->H_w[ch].i;
     */

     /*quick and dirty*/
     H_w.r = 1.0;//z_abs(H_w);
     H_w.i = 0.0;
     
     /*         
     if (i == proj->ants-1) gain1 = z_add(z_mul(alpha, proj->Ant[i-1].g), proj->Ant[i].g);
     else if (i == 0)       gain1 = z_add(z_mul(alpha, proj->Ant[i+1].g), proj->Ant[i].g);
     else                   gain1 = z_add( z_mul(alpha, z_add(proj->Ant[i-1].g, proj->Ant[i+1].g)), proj->Ant[i].g);

     if (j == proj->ants-1) gain2 = z_add(z_mul(alpha, proj->Ant[j-1].g), proj->Ant[j].g);
     else if (j == 0)       gain2 = z_add(z_mul(alpha, proj->Ant[j+1].g), proj->Ant[j].g);
     else                   gain2 = z_add( z_mul(alpha, z_add(proj->Ant[j-1].g, proj->Ant[j+1].g)), proj->Ant[j].g);
     */

     //phi = ch*(proj->delphi[j] - proj->delphi[i]);
     //phi = 0.0;
     
     
     phasor.abs = 1.0; phasor.arg = (ch-proj->chans/2)*proj->delphi[i] ;
     g_i = z_mul( proj->Ant[i].g, Polar_to_Cmplx(phasor) );

     phasor.abs = 1.0; phasor.arg = (ch-proj->chans/2)*proj->delphi[j] ;
     g_j = z_mul( proj->Ant[j].g, Polar_to_Cmplx(phasor) );
     
     
     //gain = z_mul( proj->Ant[i].g, z_conj(proj->Ant[j].g) );
     //fprintf(finfo, "%s Gain %d: %5.3f /_ %5.3f Gain %d: %5.3f /_ %5.3f\n",info, i, z_abs(g_i), rad2deg(z_arg(g_i)), j, z_abs(g_j), rad2deg(z_arg(g_j)));
     /*gain = z_mul( gain, ret_z( cos(phi), sin(phi)) );
     */

     gain = z_mul(g_i, z_conj(g_j));
     //gain = z_mul(gain1, z_conj(gain2));
     //fprintf(finfo, "%s Gain %d : %f + i%f Gain %d : %f + i%f\n", info, i, gain1.r, gain1.i, j, gain2.r, gain2.i);
     signal = z_add( noise, V_s );
     V_ij = z_mul( z_mul(signal, gain ), H_w );
     
     /*
     blofst = proj->Base[bl].b ;
     V_ij = z_add(V_ij, blofst);
     */
     
     vis[ch].r   = real(V_ij);// * proj->window[ch];
     vis[ch].i   = imag(V_ij);// * proj->window[ch];
     vis[ch].wt  = 1.0;

     //fprintf(finfo, "%s Vis[%d][%d] = %5.1f + i %5.1f\n", info, bl, ch, vis[ch].r, vis[ch].i);
    if ( ch == proj->chans/2 ) snrBuf[bl] += z_abs(signal)/z_abs(noise);
    }
 }
 extract_channel( proj, (proj->preint-count)) ;
 ++recnum;
 count--;
 accumulate(accBuf, proj->RecBuf, len);
 }

 scale(snrBuf, proj->SNR, proj->preint, proj->baselines);
 scale(accBuf, proj->RecBuf, proj->preint, len);
 free(snrBuf);
 free(accBuf);

 return recnum;
}


void write_model_visibilities( ProjParType *proj, char *modelfile )
{
 int i;
 CmplxType M, ME;
 FILE* fMV = fopen(modelfile, "w");
 double arg;
 int ch = proj->chans / 2;

 fprintf(fMV, "#Baseline  \tUL\t\tReal(M)\t\tErr.r(M)\tImag(M)\t\tErr.i(M)\tAmpl(M)\t\tPhase(M)\n");
 fprintf(stderr, "#Baseline  \tUL\t\tReal(M)\t\tErr.r(M)\tImag(M)\t\tErr.i(M)\tAmpl(M)\t\tPhase(M)\n");
 for(ch = 0 ; ch < proj->chans ; ch++)
 { for(i = 0; i < proj->redundant_baselines+1; i++)
   { M = proj->model_vis[ch][i];
     ME = proj->MV_errbar[ch][i];
     arg = rad2deg(z_arg(M));
     proper_angle( &arg );
     fprintf(fMV, "%7d\t\t%lf\t%e\t%lf\t%e\t%lf\t%lf\t%lf\n", i+1, proj->ULambda[ch][i], real(M), real(ME), imag(M), imag(ME), z_abs(M), arg  );
     fprintf(stderr, "%7d\t\t%lf\t%e\t%lf\t%e\t%lf\t%lf\t%lf\n", i+1, proj->ULambda[ch][i], real(M), real(ME), imag(M), imag(ME), z_abs(M), arg  );
   }
     //fprintf(fMV,"\n");
     //fprintf(stderr,"\n");
 }
 fclose(fMV);
 fprintf(finfo, "%s Model visibilities written to %s\n", info, modelfile);
}

void compose_A( ProjParType *proj )
{
 int bl, rbl = 0, i=0, j=0, n; 
 long nrows = proj->baselines - (proj->unique_baseline[0] - proj->redundant_baselines) + 2 ;
 BaseParType *base = proj->Base;
 int ncols = proj->ants + proj->redundant_baselines;
 long *blindex = proj->blindex;

 double wt = 1.0;
 FILE *fwmatx;

 set_baseline_index( proj );

 proj->A = gsl_matrix_calloc(nrows, ncols);
 fprintf(finfo, "%s Composing the array signature matrix\n", info);

 for ( bl = 0 ; bl < proj->baselines ; bl++ )
 { if ( proj->unique_baseline [ base[bl].id2 - base[bl].id1 ] > 1 ) /*Is the baseline bl redundant?*/
   {/*Compacting the matrix index elements*/
     for(n = 0; n < proj->ants; n++)
     { if( base[bl].id1 == proj->Ant[n].id ) i = n;
       if( base[bl].id2 == proj->Ant[n].id ) j = n;
     }
     /*Weights for the gains*/
     gsl_matrix_set(proj->A, rbl, i, wt);
     gsl_matrix_set(proj->A, rbl, j, wt);

     /*Weights for the redundant true visibilities*/
     gsl_matrix_set(proj->A, rbl, proj->ants + blindex[ base[bl].id2 - base[bl].id1 ], wt);

     rbl++;
    }
  }
 /*constraint on the gains */
 for(i = 0; i<proj->ants; i++) gsl_matrix_set(proj->A, rbl, i, wt);
 /*constraint on the visibilities */
 for(i = proj->ants; i<proj->ants + proj->redundant_baselines; i++) gsl_matrix_set(proj->A, rbl, i, wt/proj->redundant_baselines);

 fprintf(finfo, "%s Array signature matrix in %s/arrsign.matx\n", info, getenv("PWD") );
 fwmatx = fopen("arrsign.matx", "w"); 
 for(i = 0; i < nrows ; i++ )
 { for(j = 0; j < ncols ; j++) fprintf(fwmatx, "%f ", proj->A->data[i*ncols + j] ) ;
     fprintf(fwmatx, "\n"); 
 }
 fclose(fwmatx); 
 fflush(finfo);
}

void log_QR_decomp(ProjParType *proj)
{
 int i, j; 
 int nrows = proj->A->size1;
 int ncols = proj->A->size2;

 FILE *fQR;

 proj->sol_re = gsl_vector_calloc(ncols);
 proj->sol_im = gsl_vector_calloc(ncols);

 proj->QR = gsl_matrix_calloc(nrows, ncols);
 proj->tau = gsl_vector_calloc( min(nrows,ncols) );

 gsl_matrix_memcpy(proj->QR, proj->A);

 fprintf(finfo, "%s QR decomposition...", info); fflush(finfo);
 gsl_linalg_QR_decomp( proj->QR, proj->tau);
 fprintf(finfo, "done\n"); fflush(finfo);

 fprintf(finfo, "%s Gen. inv. matrix in %s/wtinv.matx\n", info, getenv("PWD") );
 fQR = fopen("wtinv.matx","w");
 for(i = 0; i < nrows ; i++ )
 { for(j = 0; j < ncols ; j++)  fprintf(fQR, "%2.2f ", proj->QR->data[i*ncols + j] ) ;
   fprintf(fQR, "\n");
 }
  fclose(fQR);

}

void log_QR_solve( ProjParType *proj )
{
 int nrows = proj->QR->size1; 
 Cmplx3Type *vis;
 BaseParType *base;
 CmplxType rd_vis[nrows];
 gsl_vector *V_re, *V_im;  
 gsl_vector *resmag, *resphi;
 int bl, ch=proj->chans / 2, rbl = 0;

 V_re =  gsl_vector_calloc(nrows);
 V_im =  gsl_vector_calloc(nrows);
 resmag = gsl_vector_calloc(nrows);
 resphi =  gsl_vector_calloc(nrows);

 base = proj->Base;

 for ( bl = 0 ; bl < proj->baselines ; bl++ )
 { vis = (Cmplx3Type*) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);
   if ( proj->unique_baseline [ base[bl].id2 - base[bl].id1 ] > 1 ) 
   { rd_vis[rbl].r = vis[ch].r;
     rd_vis[rbl].i = vis[ch].i;
     rbl++;
   }
 }

 ComplexCartesian_to_GSLPolar(rd_vis, V_re, V_im);
 GSL_Ln(V_re);

 V_re->data[rbl] = V_im->data[rbl] = 0.0;
 rbl++;
 V_re->data[rbl] = V_im->data[rbl] = 0.0;

 gsl_linalg_QR_lssolve(proj->QR, proj->tau, V_re, proj->sol_re, resmag);
 gsl_linalg_QR_lssolve(proj->QR, proj->tau, V_im, proj->sol_im, resphi);

 gsl_vector_free(V_re);
 gsl_vector_free(V_im);
 gsl_vector_free(resmag);
 gsl_vector_free(resphi);
}

void convert_sol_log2lin( ProjParType *proj )
{ 
 int i = 0, N = proj->sol_re->size;
 double real, imag;

 for( i = 0 ; i < N ; i++ )
 { real = proj->sol_re->data[i];
   imag = proj->sol_im->data[i];

   proj->sol_re->data[i] = exp(real)*cos(imag);
   proj->sol_im->data[i] = exp(real)*sin(imag);
 }

}

void log_SV_decomp( ProjParType *proj )
{
 int i, j; 
 int nrows = proj->A->size1;
 int ncols = proj->A->size2;
 gsl_vector *work;
 FILE *fS, *fU, *fV;

 work = (gsl_vector*)gsl_vector_alloc ( ncols );

 proj->U = gsl_matrix_calloc ( nrows, ncols );
 proj->V = gsl_matrix_calloc ( ncols, ncols );
 proj->S = gsl_vector_calloc ( ncols );

 proj->sol_re = gsl_vector_calloc(ncols);
 proj->sol_im = gsl_vector_calloc(ncols);

 gsl_matrix_memcpy(proj->U, proj->A);

 fprintf(finfo, "%s Singular Value Decomposition...", info); fflush(finfo);

 if(nrows >= ncols)  gsl_linalg_SV_decomp_jacobi( proj->U, proj->V, proj->S );
 else gsl_linalg_SV_decomp(proj->U, proj->V, proj->S, work);
 fprintf(finfo, "done\n"); fflush(finfo);
 
 fprintf(finfo, "%s U, S and V in %s/.\n", info, getenv("PWD") );
 fU = fopen("U","w");
 fS = fopen("S","w");
 fV = fopen("V","w");
 for(i = 0; i < nrows ; i++ )
 { for(j = 0; j < ncols ; j++)  fprintf(fU, "%2.2f ", proj->U->data[i*ncols + j] ) ;
   fprintf(fU, "\n");
 }
 for(i = 0; i < ncols ; i++ )
 { fprintf(fS, "%2.2f\n", proj->S->data[i] ) ;
   { for(j = 0; j < ncols ; j++)  fprintf(fV, "%2.2f ", proj->V->data[i*ncols + j] ) ;
     fprintf(fV, "\n");
   }
 }
 fclose(fU); fclose(fS); fclose(fV);

 gsl_vector_free(work);
}

void log_SV_solve( ProjParType * proj)
{
 int nrows = proj->A->size1;
 Cmplx3Type *vis;
 BaseParType *base;
 CmplxType rd_vis[nrows];

 gsl_vector *V_re, *V_im;  
 int bl, ch=proj->chans / 2, rbl = 0;

 V_re =  (gsl_vector*)gsl_vector_calloc(nrows);
 V_im =  (gsl_vector*)gsl_vector_calloc(nrows);

 base = proj->Base;

 for ( bl = 0 ; bl < proj->baselines ; bl++ )
 { vis = (Cmplx3Type*) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);
   if ( proj->unique_baseline [ base[bl].id2 - base[bl].id1 ] > 1 ) 
   { rd_vis[rbl].r = vis[ch].r;
     rd_vis[rbl].i = vis[ch].i;
     rbl++;
   }
 }
 ComplexCartesian_to_GSLPolar(rd_vis, V_re, V_im);
 GSL_Ln(V_re);

 V_re->data[rbl] = V_im->data[rbl] = 0.0;

 gsl_linalg_SV_solve(proj->U, proj->V, proj->S, V_re, proj->sol_re);
 gsl_linalg_SV_solve(proj->U, proj->V, proj->S, V_im, proj->sol_im);

 gsl_vector_free(V_re);
 gsl_vector_free(V_im);
}

void supply_initial_solutions( ProjParType *proj )
{
 int i, n = proj->ants + proj->redundant_baselines;
 double g, S;

 proj->sol_re = gsl_vector_calloc(n);
 proj->sol_im = gsl_vector_calloc(n);

 g = 1.0;
 S = 1.0;
 
 for( i = 0; i < proj->ants; i++ )
 { proj->sol_re->data[i] = log(g);
   proj->sol_im->data[i] = 0.0;
 }

 for( i = 0; i < proj->redundant_baselines ; i++ )
 { proj->sol_re->data[ proj->ants + i ] = log(S); 
   proj->sol_im->data[ proj->ants + i ] = 0.0;
 }

}

void setup_lincal( ProjParType *proj )
{
 int i, n = proj->sol_re->size;
 CmplxType temp;

 for ( i = 0; i < proj->ants; i++ )
 { temp.r = proj->sol_re->data[i];
   temp.i = proj->sol_im->data[i];

   proj->sol_re->data[i] = GAIN * expf( temp.r ) * cos( temp.i );
   proj->sol_im->data[i] = GAIN * expf( temp.r ) * sin( temp.i );
 }
 
 for ( i = proj->ants; i < n; i++ )
 { temp.r = proj->sol_re->data[i];
   temp.i = proj->sol_im->data[i];

   proj->sol_re->data[i] = expf( temp.r ) * cos( temp.i );
   proj->sol_im->data[i] = expf( temp.r ) * sin( temp.i );
 }

}

void compose_B( ProjParType *proj )
{
 int bl, rbl = 0, i=0, j=0, n; 
 long nrows = proj->baselines - (proj->unique_baseline[0] - proj->redundant_baselines) + 2;
 BaseParType *base = proj->Base;
 int ncols = proj->ants + proj->redundant_baselines;
 long *blindex = proj->blindex;
 FILE *fwmatx;
 CmplxType g_i_guess, g_j_guess, G, V_sky_guess, V_meas_guess, temp;
 Cmplx3Type *V_meas;
 int ch = proj->chans / 2;
 double one = 1.0;

 proj->B = gsl_matrix_calloc(2*nrows, 2*ncols);
 proj->delta_V_obs = gsl_vector_calloc(2*nrows);
 proj->delta_sol = gsl_vector_calloc(2*ncols);

 set_baseline_index( proj );

 fprintf(finfo, "%s Composing matrix B\n", info);

 for ( bl = 0 ; bl < proj->baselines ; bl++ )
 { if ( proj->unique_baseline [ base[bl].id2 - base[bl].id1 ] > 1 ) /*Is the baseline bl redundant?*/
   {/*Compacting the matrix index elements*/
     for(n = 0; n < proj->ants; n++)
     { if( base[bl].id1 == proj->Ant[n].id ) i = n;
       if( base[bl].id2 == proj->Ant[n].id ) j = n;
     }
     /*Initial guess antenna gains assigned from Wieringa's intial gain solutions*/
     g_i_guess = ret_z(proj->sol_re->data[i], proj->sol_im->data[i]);
     g_j_guess = ret_z(proj->sol_re->data[j], proj->sol_im->data[j]);
     G = z_mul( g_i_guess, z_conj(g_j_guess) );

     /*Initial guess sky visibilities assigned from Wieringa's intial gain solutions*/
     temp.r = proj->sol_re->data[ proj->ants + blindex[ base[bl].id2 - base[bl].id1 ] ];
     temp.i = proj->sol_im->data[ proj->ants + blindex[ base[bl].id2 - base[bl].id1 ] ];
     V_sky_guess = temp;

     /*define V_meas_guess = G.V_sky_guess*/
     V_meas_guess = z_mul(G, V_sky_guess);

     /*Weights for the gains*/
     /*Equal to the real part of the vis. correction*/
     gsl_matrix_set(proj->B, 2*rbl + 0, 2*i + 0,  real(V_meas_guess) );
     gsl_matrix_set(proj->B, 2*rbl + 0, 2*i + 1, -imag(V_meas_guess) );
     gsl_matrix_set(proj->B, 2*rbl + 0, 2*j + 0,  real(V_meas_guess) );
     gsl_matrix_set(proj->B, 2*rbl + 0, 2*j + 1,  imag(V_meas_guess) );
     
     /*Weights for the real part of the redundant sky visibilities*/
     gsl_matrix_set(proj->B, 2*rbl + 0, 2*proj->ants + 2*blindex[ base[bl].id2 - base[bl].id1 ] + 0,  real(V_meas_guess));
     gsl_matrix_set(proj->B, 2*rbl + 0, 2*proj->ants + 2*blindex[ base[bl].id2 - base[bl].id1 ] + 1, -imag(V_meas_guess));

     /*Weights for the gains*/
     /*Equal to the imag part of the vis. correction*/
     gsl_matrix_set(proj->B, 2*rbl + 1, 2*i + 0,  imag(V_meas_guess) );
     gsl_matrix_set(proj->B, 2*rbl + 1, 2*i + 1,  real(V_meas_guess) );
     gsl_matrix_set(proj->B, 2*rbl + 1, 2*j + 0,  imag(V_meas_guess) );
     gsl_matrix_set(proj->B, 2*rbl + 1, 2*j + 1, -real(V_meas_guess) );
 
     /*Weights for the imag part of the redundant sky visibilities*/
     gsl_matrix_set(proj->B, 2*rbl + 1, 2*proj->ants + 2*blindex[ base[bl].id2 - base[bl].id1 ] + 0, imag(V_meas_guess));
     gsl_matrix_set(proj->B, 2*rbl + 1, 2*proj->ants + 2*blindex[ base[bl].id2 - base[bl].id1 ] + 1, real(V_meas_guess));

     V_meas = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);

     proj->delta_V_obs->data[ 2*rbl + 0 ] = V_meas[ch].r - real(V_meas_guess);
     proj->delta_V_obs->data[ 2*rbl + 1 ] = V_meas[ch].i - imag(V_meas_guess);

     rbl += 1;
    }
 }

 /*  Constraint on gains */
 for( i = 0; i < 2*proj->ants; i += 2) 
 { gsl_matrix_set(proj->B, 2*rbl + 0, i, one);
   gsl_matrix_set(proj->B, 2*rbl + 1, i+1, one);
 }
 proj->delta_V_obs->data[ 2*rbl + 0 ] = 0.0 ;
 proj->delta_V_obs->data[ 2*rbl + 1 ] = 0.0 ;
 rbl += 1;

 /*Constraints on the visibilities*/
 for( i = proj->ants; i < ncols; i += 2) 
 { gsl_matrix_set(proj->B, 2*rbl + 0, i, one);
   gsl_matrix_set(proj->B, 2*rbl + 1, i+1, one);
 }
 proj->delta_V_obs->data[ 2*rbl + 0 ] = 0.0 ;
 proj->delta_V_obs->data[ 2*rbl + 1 ] = 0.0 ;
 
 fprintf(finfo, "%s Matrix B in %s/B.matx\n", info, getenv("PWD") );
 fwmatx = fopen("B.matx", "w"); 
 for(i = 0; i < 2*nrows ; i++ )
 { for(j = 0; j < 2*ncols ; j++) fprintf(fwmatx, "%lf ", proj->B->data[i*2*ncols + j] ) ;
     fprintf(fwmatx, "\n"); 
 }
 fclose(fwmatx);

}

void B_index( ProjParType *proj)
{
 int nrows = proj->B->size1;
 int ncols = proj->B->size2;
 int i = 0, j = 0;
 FILE *fAidx = fopen("A.idx", "w");
 FILE *fBidx = fopen("B.idx", "w");

 for( i = 0 ; i < nrows ; i++ )
 { for( j = 0; j < ncols; j++ ) if( (gsl_matrix_get(proj->B, i, j)) != 0)  fprintf(fBidx, "%d %d\n", i, j ); 
 }
 fclose(fBidx);

 nrows = proj->A->size1;
 ncols = proj->A->size2;

 for( i = 0 ; i < nrows ; i++ )
 { for( j = 0; j < ncols; j++ ) if( (gsl_matrix_get(proj->A, i, j)) != 0)  fprintf(fAidx, "%d %d\n", i, j ); 
 }
 fclose(fAidx);
}

void assign_keymap_B( ProjParType *proj )
{
 /* This function is required only if the wt. matrix 
 is to be updated frequently; it indexes the non-zero elements
 in "ProjParType->B" */

 int nrows = proj->B->size1;
 int ncols = proj->B->size2;
 int keymap_elements = (nrows-2)*6 + 2*proj->ants;
 int i = 0, j = 0;

 proj->keymap = (int*) calloc ( keymap_elements, sizeof(int) );
 for( i = 0 ; i < nrows*ncols ; i++ )
 { if ( proj->B->data[i] != 0.0 ) proj->keymap[j++] = i; }

}

void setup_QRD( ProjParType *proj )
{
 int nrows = proj->B->size1;
 int ncols = proj->B->size2;

 proj->QR = gsl_matrix_calloc(nrows, ncols);
 proj->tau = gsl_vector_calloc( min(nrows,ncols) );
 proj->QRres = gsl_vector_calloc(nrows);
}

void setup_SVD( ProjParType *proj )
{
 int nrows = proj->B->size1;
 int ncols = proj->B->size2;

 proj->U = gsl_matrix_calloc ( nrows, ncols );
 proj->V = gsl_matrix_calloc ( ncols, ncols );
 proj->S = gsl_vector_calloc ( ncols );
}

void lin_QRD_solve( ProjParType *proj )
{
 gsl_matrix_memcpy(proj->QR, proj->B);
 gsl_linalg_QR_decomp( proj->QR, proj->tau);
 gsl_linalg_QR_lssolve(proj->QR, proj->tau, proj->delta_V_obs, proj->delta_sol, proj->QRres);
}


void lin_SVD_solve( ProjParType *proj )
{
 int nrows = proj->B->size1;
 int ncols = proj->B->size2;
 gsl_vector *work;
 
 work = (gsl_vector*)gsl_vector_alloc ( ncols );
 gsl_matrix_memcpy(proj->U, proj->B);
 if(nrows >= ncols)  gsl_linalg_SV_decomp_jacobi( proj->U, proj->V, proj->S );
 else  gsl_linalg_SV_decomp(proj->U, proj->V, proj->S, work);
 gsl_linalg_SV_solve(proj->U, proj->V, proj->S, proj->delta_V_obs, proj->delta_sol);
 gsl_vector_free(work);
}


void write_sol( ProjParType *proj, FILE *fsol)
{
 static int hdr_written = 0;
 int i, ncols = proj->ants + proj->redundant_baselines;
 double t;
 RGParType *uvw = (RGParType*)proj->RecBuf;

 if(! hdr_written )
 { fprintf(fsol, "## This solution file was generated at time %s\n", mjd2iau_date( mjdnow() ) );
   fprintf(fsol, "## It has 1 + %d columns.\n", 2*ncols);
   fprintf(fsol, "## Column 1 is time in minutes since UTC 0h.\n" );
   fprintf(fsol, "## The next %d columns are arranged as \"Re(g) Im(g)\"\n", 2*proj->ants);
   fprintf(fsol, "## for the gains of the %d antennas.\n",  proj->ants);
   fprintf(fsol, "## The remaining %d columns are arranged as \"Re(V) Im(V)\"\n", 2*proj->redundant_baselines);
   fprintf(fsol, "## for the unknown visibilities at the %d redundant baselines of\n", proj->redundant_baselines);
   fprintf(fsol, "## unique length each.\n");
   fprintf(fsol, "##\n");
   fprintf(fsol, "## Mean System Gain        : %5.3f K/Jy \n", proj->system.gain);
   fprintf(fsol, "## Mean System Temperature : %5.0f K\n", proj->system.Tsys);
   fprintf(fsol, "##\n");
   hdr_written = 1;
 }
 
 t = (uvw->date2 * 86400 - IATUTC) / 60;

 fprintf(fsol, "%f\t", t);
 for(i = 0; i < ncols; i++)  fprintf(fsol, "%f\t%f\t",  proj->sol_re->data[i], proj->sol_im->data[i] );
 fprintf(fsol, "\n");

}

void write_baseline_SNR_matx( ProjParType *proj )
{
 BaseParType *baseline = proj->Base;
 float SNRmatx[proj->ORTelements][proj->ORTelements];
 int i, j, bl;
 int ant1, ant2;
 FILE *fSNR = fopen("SNR.matx", "w");
 
 for(i = 0; i < proj->ORTelements; i++)
 { for(j = 0; j < proj->ORTelements; j++)  SNRmatx[i][j] = 0.0; }

 for(bl = 0; bl < proj->baselines; bl++)
 { ant1 = baseline[bl].id1;
   ant2 = baseline[bl].id2;

   SNRmatx[ant1][ant2] = proj->SNR[bl]; 
   SNRmatx[ant2][ant1] = proj->SNR[bl]; 
 }
 fprintf(finfo, "\n%s SNR matrix in %s/SNR.matx", info, getenv("PWD") );
 for(i = 0 ; i < proj->ORTelements ; i++)
 { for(j = 0 ; j < proj->ORTelements ; j++) fprintf(fSNR, "%f ", SNRmatx[i][j] );
   fprintf(fSNR, "\n");
 }
 fclose(fSNR);
}


void update_B( ProjParType *proj )
{
 BaseParType *base = proj->Base;
 long *blindex = proj->blindex; 
 CmplxType g_i_guess, g_j_guess, G, V_sky_guess, V_meas_guess,  temp;
 Cmplx3Type *V_meas;
 int ch = proj->chans / 2;
 int i=0, j=0, bl=0, n=0, rbl = 0; 
 FILE *fwmatx;

 set_baseline_index( proj );

 for ( bl = 0 ; bl < proj->baselines ; bl++ )
 { if ( proj->unique_baseline [ base[bl].id2 - base[bl].id1 ] > 1 ) /*Is the baseline bl redundant?*/
   {/*Compacting the matrix index elements*/
     for(n = 0; n < proj->ants; n++)
     { if( base[bl].id1 == proj->Ant[n].id ) i = n;
       if( base[bl].id2 == proj->Ant[n].id ) j = n;
     }

     /*Initial guess antenna gains assigned from Wieringa's intial gain solutions*/
     g_i_guess = ret_z(proj->sol_re->data[i], proj->sol_im->data[i]);
     g_j_guess = ret_z(proj->sol_re->data[j], proj->sol_im->data[j]);
     G = z_mul( g_i_guess, z_conj(g_j_guess) );
     
     /*Initial guess sky visibilities assigned from Wieringa's intial gain solutions*/
     temp.r = proj->sol_re->data[ proj->ants + blindex[ base[bl].id2 - base[bl].id1 ] ];
     temp.i = proj->sol_im->data[ proj->ants + blindex[ base[bl].id2 - base[bl].id1 ] ];
     V_sky_guess = temp;

     /*define V_meas_guess = G.V_sky_guess*/
     V_meas_guess = z_mul(G, V_sky_guess);

     /*Weights for the gains*/
     /*Equal to the real part of the vis. correction*/
     gsl_matrix_set(proj->B, 2*rbl + 0, 2*i + 0,  real(V_meas_guess) );
     gsl_matrix_set(proj->B, 2*rbl + 0, 2*i + 1, -imag(V_meas_guess) );
     gsl_matrix_set(proj->B, 2*rbl + 0, 2*j + 0,  real(V_meas_guess) );
     gsl_matrix_set(proj->B, 2*rbl + 0, 2*j + 1,  imag(V_meas_guess) );
     
     /*Weights for the real part of the redundant sky visibilities*/
     gsl_matrix_set(proj->B, 2*rbl + 0, 2*proj->ants + 2*blindex[ base[bl].id2 - base[bl].id1 ] + 0,  real(V_meas_guess));
     gsl_matrix_set(proj->B, 2*rbl + 0, 2*proj->ants + 2*blindex[ base[bl].id2 - base[bl].id1 ] + 1, -imag(V_meas_guess));

     /*Weights for the gains*/
     /*Equal to the imag part of the vis. correction*/
     gsl_matrix_set(proj->B, 2*rbl + 1, 2*i + 0,  imag(V_meas_guess) );
     gsl_matrix_set(proj->B, 2*rbl + 1, 2*i + 1,  real(V_meas_guess) );
     gsl_matrix_set(proj->B, 2*rbl + 1, 2*j + 0,  imag(V_meas_guess) );
     gsl_matrix_set(proj->B, 2*rbl + 1, 2*j + 1, -real(V_meas_guess) );
 
     /*Weights for the imag part of the redundant sky visibilities*/
     gsl_matrix_set(proj->B, 2*rbl + 1, 2*proj->ants + 2*blindex[ base[bl].id2 - base[bl].id1 ] + 0, imag(V_meas_guess));
     gsl_matrix_set(proj->B, 2*rbl + 1, 2*proj->ants + 2*blindex[ base[bl].id2 - base[bl].id1 ] + 1, real(V_meas_guess));

     V_meas = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);

     proj->delta_V_obs->data[ 2*rbl + 0 ] = V_meas[ch].r - real(V_meas_guess);
     proj->delta_V_obs->data[ 2*rbl + 1 ] = V_meas[ch].i - imag(V_meas_guess);

     rbl += 1;
    }
  }

 fwmatx = fopen("B.matx", "w"); 
 for(i = 0; i < proj->B->size1 ; i++ )
 { for(j = 0; j < proj->B->size2 ; j++) fprintf(fwmatx, "%lf ", proj->B->data[i*proj->B->size2 + j] ) ;
     fprintf(fwmatx, "\n"); 
 }
 fclose(fwmatx);
}



void update_sol( ProjParType *proj )
{
 int i;
 int ncols = proj->B->size2;
 CmplxType temp, temp1, temp2;
 for ( i = 0; i < ncols; i += 2 )
 { 
  temp1.r = 1 + proj->delta_sol->data[i];
  temp1.i = proj->delta_sol->data[i+1];

  temp2.r = proj->sol_re->data[i/2];
  temp2.i = proj->sol_im->data[i/2];

  temp = z_mul(temp1, temp2);

  proj->sol_re->data[i/2] = real(temp);
  proj->sol_im->data[i/2] = imag(temp);
 }

}
/*
void setup_kalman_filter( ProjParType *proj )
{
 int nrows, ncols;

 nrows = proj->B->size2;
 ncols = proj->B->size2;
 proj->kalman->cov_err_sol =  gsl_matrix_calloc( nrows, ncols );

 nrows = proj->B->size2;
 ncols = proj->B->size1;
 proj->kalman->kal_gain = gsl_matrix_calloc( nrows, ncols );

 nrows = proj->B->size1;
 ncols = proj->B->size1;
 proj->kalman->cov_err_meas = gsl_matrix_calloc( nrows, ncols );

}

void get_covar_measurement_noise( ProjParType *proj )
{
 int i, j;
 for(i = 0; i < proj->kalman->cov_err_meas->size1; i++)
 { for(j = 0; j < proj->kalman->cov_err_meas->size2; j++)
   gsl_matrix_set(proj->kalman->cov_err_meas, i, j, proj->delta_V_obs->data[i], proj->delta_V_obs->data[j]);
 }
}

void get_covar_solution_error( ProjParType *proj )
{
 int i, j;
 for(i = 0; i < proj->kalman->cov_err_sol->size1; i++)
 { for(j = 0; j < proj->kalman->cov_err_sol->size2; j++)
   gsl_matrix_set(proj->kalman->cov_err_sol, i, j, proj->delta_sol->data[i], proj->delta_sol->data[j]);
 }
}

void get_kalman_gain( ProjParType *proj )
{
 double alpha, beta;
 gsl_vector *tau;
 gsl_permutation *p;
 int nrows, ncols;

 alpha = 1.0; beta = 0.0;
 gsl_blas_dgemm(CblasNoTrans, CblasTrans, alpha, proj->kalman->cov_err_sol, proj->B, beta, proj->kalman->temp1);

 alpha = 1.0; beta = 1.0;
 gsl_matrix_memcpy(proj->kalman->temp2, proj->kalman->cov_err_meas);
 gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, alpha, proj->B, proj->kalman->temp1, beta, proj->kalman->temp2);

 nrows = proj->kalman->temp2->size1;
 ncols = proj->kalman->temp2->size2;
 p = gsl_permutation_calloc(
 gsl_linalg_LU_decomp( proj->kalman->temp2, p );


}

void update_covar_solution_error( ProjParType *proj )
 {

}

void kalman_predict( ProjParType *proj )
{

}

void kalman_update (projParType *proj )
{

}

*/

void setup_itersol ( ProjParType *proj )
{
 int n = proj->ants + proj->redundant_baselines;
 int i, j;
 double g_amp, g_phs;
 double V_amp, V_phs;
 CmplxType g, V;

 proj->sol_re = gsl_vector_calloc(n);
 proj->sol_im = gsl_vector_calloc(n);

 proj->d_sol_re = gsl_vector_calloc(n);
 proj->d_sol_im = gsl_vector_calloc(n);

 proj->prev_sol_re = gsl_vector_calloc(n);
 proj->prev_sol_im = gsl_vector_calloc(n);

 g_amp = 1.0;
 g_phs = 0.0;

 V_amp = 1.0;
 V_phs = 0.0;

 g.r = g_amp * cos(g_phs);
 g.i = g_amp * sin(g_phs);

 V.r = V_amp * cos(V_phs);
 V.i = V_amp * sin(V_phs);

 for ( i = 0; i < proj->ants; i++ )
 { proj->sol_re->data[i] = real(g);
   proj->sol_im->data[i] = imag(g);
 }
 
 for ( i = 0; i < proj->redundant_baselines; i++ )
 { 
   j = proj->ants + i;
     
   proj->sol_re->data[j] = real(V);
   proj->sol_im->data[j] = imag(V);
 }

}

void reinit_itersol( ProjParType *proj )
{
 int n = proj->ants + proj->redundant_baselines;
 int i, j;
 double g_amp, g_phs;
 double V_amp, V_phs;
 CmplxType g, V;

 g_amp = 1.0;
 g_phs = 0.0;

 V_amp = 1.0;
 V_phs = 0.0;

 g.r = g_amp * cos(g_phs);
 g.i = g_amp * sin(g_phs);

 V.r = V_amp * cos(V_phs);
 V.i = V_amp * sin(V_phs);

 for ( i = 0; i < proj->ants; i++ )
 { proj->sol_re->data[i] = real(g);
   proj->sol_im->data[i] = imag(g);
 }
 
 for ( i = 0; i < proj->redundant_baselines; i++ )
 { 
   j = proj->ants + i;
     
   proj->sol_re->data[j] = real(V);
   proj->sol_im->data[j] = imag(V);
 }

}

void resize_fgmap ( ProjParType *proj )
{
 int pbeam_xpix, pbeam_ypix;
 double ptg_ra, ptg_dec, dec_fan;
 GalFGType *gal = &(proj->fg);
 PrimaryBeamParType *pb = &(proj->pbeam); 
 int i,j, ch;

 fprintf(finfo, "%s Resizing the diffuse foreground map ...", info);

 ptg_ra  = ra2sxg( &(proj->scan[0].ra) );
 ptg_dec = dec2sxg( &(proj->scan[0].dec) );
 dec_fan = (4.5)*(proj->ORTelements/PHASE_I) / cos(deg2rad(ptg_dec));
 /*
 pbeam_xpix = STERAD * 1.75/rad2deg(1.0);
 pbeam_ypix = STERAD * dec_fan/rad2deg(1.0);
 */

 pbeam_xpix = pb->xside;
 pbeam_ypix = pb->yside;

 gal->pbside = pbeam_ypix;
 gal->pbmap = (double***)calloc(proj->chans, sizeof(double**) );
 for(ch = 0 ; ch<proj->chans;ch++)
 { gal->pbmap[ch] = (double**)calloc(gal->pbside, sizeof(double*) );
   for (i = 0 ; i < gal->pbside ; i++ )
   { gal->pbmap[ch][i] = (double*)calloc(gal->pbside,sizeof(double));
     for(j = 0 ; j < gal->pbside ; j++)
     gal->pbmap[ch][i][j] = gal->map[ch][i][j] * pb->shape[ch][i][j];
   }
 }

 gal->ra_res = -gal->angres;
 gal->dec_res = gal->angres;

 fprintf(finfo, "done\n");


 /* Resize image in RA to make it square *
 {
  double x[pbeam_xpix], y[pbeam_xpix];
  double xi, yi;
  gsl_interp_accel *acc = gsl_interp_accel_alloc();
  gsl_interp *interp = gsl_interp_alloc (gsl_interp_linear, pbeam_xpix);
  int x_0 = gal->fullsize / 2,   y_0 = gal->fullsize / 2 ;

  for ( j = 0 ; j < pbeam_ypix ; j++ )
  { for ( i = 0 ; i < pbeam_xpix ; i++ )
    { x[i] = (double)i / (double)pbeam_xpix;
      y[i] = (double)gal->map[y_0 - pbeam_ypix/2 + j][x_0 - pbeam_xpix/2 + i];
    }
    gsl_interp_init (interp, x, y, pbeam_xpix);
    for (i = 0; i < pbeam_ypix; i++)
    { xi = (double)i/(double)pbeam_ypix;
      yi = gsl_interp_eval (interp, x, y, xi, acc);
      gal->pbmap[j][i] = (double)yi;
    }
  }
  gsl_interp_free (interp);
  gsl_interp_accel_free (acc);
 }
 */

}


void resize_tecmap ( ProjParType *proj )
{
 int pbeam_xpix, pbeam_ypix;
 double ptg_ra, ptg_dec, dec_fan;
 TECScrType *tec = &(proj->tec);
 int i,j;

 fprintf(finfo, "%s Resizing the ionospheric TEC screen map ...", info);

 ptg_ra  = ra2sxg( &(proj->scan[0].ra) );
 ptg_dec = dec2sxg( &(proj->scan[0].dec) );
 dec_fan = (4.5)*(proj->ORTelements/PHASE_I) / cos(deg2rad(ptg_dec));

 pbeam_xpix = STERAD * 1.75/rad2deg(1.0);
 pbeam_ypix = STERAD * dec_fan/rad2deg(1.0);

 tec->pbside = pbeam_ypix;
 tec->pbmap = (double**)calloc(tec->pbside, sizeof(double*));
 for (i = 0 ; i < tec->pbside ; i++ ) tec->pbmap[i] = (double*)calloc(tec->pbside,sizeof(double));

 /* Resize image in RA to make it square */
 {
  double x[pbeam_xpix], y[pbeam_xpix];
  double xi, yi;
  gsl_interp_accel *acc = gsl_interp_accel_alloc();
  gsl_interp *interp = gsl_interp_alloc (gsl_interp_linear, pbeam_xpix);
  int x_0 = tec->fullsize / 2,   y_0 = tec->fullsize / 2 ;

  for ( j = 0 ; j < pbeam_ypix ; j++ )
  { for ( i = 0 ; i < pbeam_xpix ; i++ )
    { x[i] = (double)i / (double)pbeam_xpix;
      y[i] = (double)tec->map[y_0 - pbeam_ypix/2 + j][x_0 - pbeam_xpix/2 + i];
    }
    gsl_interp_init (interp, x, y, pbeam_xpix);
    for (i = 0; i < pbeam_ypix; i++)
    { xi = (double)i/(double)pbeam_ypix;
      yi = gsl_interp_eval (interp, x, y, xi, acc);
      tec->pbmap[j][i] = (double)yi;
    }
  }
  gsl_interp_free (interp);
  gsl_interp_accel_free (acc);
 }

 tec->ra_res = -tec->angres*(double)pbeam_xpix/(double)pbeam_ypix;
 tec->dec_res = tec->angres;

 fprintf(finfo, "done\n");

}

void free_gal_fgmap( ProjParType *proj )
{
 GalFGType *gal = &(proj->fg);

 free(gal->map);
 free(gal->pbmap);
}


void free_tec_map( ProjParType *proj )
{
 TECScrType *tec = &(proj->tec);

 free(tec->map);
 free(tec->pbmap);
}

void init_skymap( ProjParType *proj )
{
    int i,ch;
 PrimaryBeamParType *pb = &(proj->pbeam);
 SkyMapType *sky = &(proj->skymap);

 sky->xside = pb->xside;
 sky->yside = pb->yside;
 sky->zside = pb->zside;

 sky->ra = (RAType*)calloc(sizeof(RAType), sky->xside);
 sky->dec = (DecType*)calloc(sizeof(DecType), sky->yside);

 sky->ra_res = pb->ra_res;
 sky->dec_res = pb->dec_res;

 for(i = 0 ; i < sky->xside ; i++) sky->ra[i] = pb->ra[i];
 for(i = 0 ; i < sky->yside ; i++) sky->dec[i] = pb->dec[i];

 sky->map =  (double***)calloc(sizeof(double**), sky->zside);
 for( ch = 0 ; ch < proj->chans ; ch++ )
 { sky->map[ch] = (double**)calloc(sizeof(double*), sky->yside);
   for ( i = 0 ; i < sky->yside ; i++ ) sky->map[ch][i] = (double*)calloc(sizeof(double), sky->xside);
 }

}

void free_skymap( ProjParType *proj )
{
 SkyMapType *sky = &(proj->skymap);

 free( sky->ra );
 free( sky->dec );
 free( sky->map );
}

void add_pntsrc_to_galfg( ProjParType *proj )
{
 int i, j, ch;
 int xofst, yofst;
 double ptg_ra, ptg_dec;
 double src_ra, src_dec;
 GalFGType *gal = &(proj->fg);
 SkyMapType *sky = &(proj->skymap);
 PrimaryBeamParType *pb = &(proj->pbeam);
 double nu, deltanu = proj->bandwidth/proj->chans;


 ptg_ra = ra2sxg(&(proj->scan[0].ra));
 ptg_dec = dec2sxg(&(proj->scan[0].dec));

 //memcpy(sky->map, gal->map, proj->chans*sky->yside*sky->xside*sizeof(double));


 // Copy FG Map within primary beam to Sky Map
 if( proj->galfg_swtch ) { 
 fprintf(finfo, "%s Laying the diffuse foreground map ...", info);
 for( ch = 0 ; ch < proj->chans ; ch++ ){
 for ( i = 0 ; i < sky->yside ; i++ )
 { for ( j = 0 ; j < sky->xside ; j++ ) sky->map[ch][i][j] = gal->map[ch][i][j]; }
 }                      }

 if(proj->pntsrc_swtch) {
 newline();
 fprintf(finfo, "%s Embedding pnt srcs in the map ...", info);
 // Add extragalactic point sources
 for(i = 0; i < proj->nsrc; i++)
 { src_ra = ra2sxg( &(proj->skysrc[i].ra) );
   src_dec = dec2sxg( &(proj->skysrc[i].dec) );
   xofst = ( (ptg_ra  - src_ra)  / pb->ra_res) + MAPSIDE / 2 ;
   yofst = ( (ptg_dec - src_dec) / pb->dec_res) + MAPSIDE / 2 ;
   if ( yofst >= MAPSIDE ) yofst = MAPSIDE-1;
   if ( yofst <= 0 ) yofst = 0;
   for(ch = 0; ch < proj->chans ; ch++)
   { nu = proj->bandcentre - proj->bandwidth/2 + ch*deltanu + deltanu/2;
     sky->map[ch][yofst][xofst] += pow(proj->bandcentre/nu,proj->skysrc[i].alpha) * proj->skysrc[i].S;
   }
 }                       
}

 fprintf(finfo,"done\n");
}

void read_model_visibilities( ProjParType *proj, char *modelFile )
{
 int i, ch;
 CmplxType M, ME;
 int n;
 double n1, n2;
 double UL;
 FILE* fMV = fopen(modelFile, "r");
 double arg;
 char str[100];
 fgets( str, sizeof(str), fMV );

 for(ch = 0 ; ch < proj->chans;ch++)
 { for(i = 0; i < proj->redundant_baselines+1; i++)
   { fgets( str, sizeof(str), fMV );
     sscanf(str, "%d %lf %lf %lf %lf %lf %lf %lf\n", &n, &UL, &(M.r), &(ME.r), &(M.i), &(ME.i), &n1, &n2 );
     proj->model_vis[ch][i].r = M.r;
     proj->model_vis[ch][i].i = M.i;
     proj->MV_errbar[ch][i].r = ME.r;
     proj->MV_errbar[ch][i].i = ME.i;
     proj->ULambda[ch][i] = UL;
     //fprintf(finfo, "%s Bl : %d Ch : %d U : %lf Re : %lf Im : %lf\n", info, i,  ch, proj->ULambda[ch][i], proj->model_vis[ch][i].r, proj->model_vis[ch][i].i);
   }
 }
 fclose(fMV);

 fprintf(finfo, "%s Model visibilities read from %s \n", info, modelFile);
}

void getpar_modelvis( ProjParType *proj)
{
  DisplayBufParType *ds = &(proj->dispBuf);
  ds->databuf = (double*)calloc(4*(proj->redundant_baselines+1)*proj->chans, sizeof(double) );
  ds->xaxis = (double*)calloc(proj->redundant_baselines+1, sizeof(double) );
}

void prep_modelvis_display( ProjParType *proj )
{
 DisplayBufParType *ds = &(proj->dispBuf);
 int ch, bl;
 int chans = proj->chans;
 int baselines = proj->redundant_baselines+1;
 int offset = chans * baselines;

 for(ch = 0 ; ch < chans ; ch++ )
 { for(bl = 0 ; bl < baselines ; bl++)
   { ds->databuf[ch*baselines + bl + 0*offset] = z_abs(proj->model_vis[ch][bl]);
     ds->databuf[ch*baselines + bl + 1*offset] = rad2deg(z_arg(proj->model_vis[ch][bl]));
     ds->databuf[ch*baselines + bl + 2*offset] = proj->model_vis[ch][bl].r;
     ds->databuf[ch*baselines + bl + 3*offset] = proj->model_vis[ch][bl].i;
   }
 }

}


void seed_solutions( ProjParType *proj )
{
 static int row = 1;
 int ant;
 GainTableType *gaintab = &(proj->gtab);
 GainTableRow *gtrow = gaintab->row;

 for(ant = 0 ; ant < gaintab->nant ; ant++)
 { proj->sol_re->data[ant] = gtrow[row].gain[ant].r;
   proj->sol_im->data[ant] = gtrow[row].gain[ant].i;
 }
 if( proj->chan2cal == proj->chans) row++;
}

void universal_model( ProjParType *proj )
{
 int i,j;
 for ( i = 0; i < proj->redundant_baselines; i++ )
 { 
   j = proj->ants + i;
     
   proj->sol_re->data[j] = 1.0;
   proj->sol_im->data[j] = 0.0;
   
   proj->d_sol_re->data[j] = 1.0;
   proj->d_sol_im->data[j] = 0.0;
 }
 
}

void force_model( ProjParType *proj )
{
 int i,j;
 int ch = proj->chan2cal;
 for ( i = 0; i < proj->redundant_baselines+1; i++ )
 { 
   j = proj->ants + i;
     
   proj->sol_re->data[j] = proj->model_vis[ch][i].r;
   proj->sol_im->data[j] = proj->model_vis[ch][i].i;
   
   proj->d_sol_re->data[j] = proj->model_vis[ch][i].r;
   proj->d_sol_im->data[j] = proj->model_vis[ch][i].i;

   //fprintf(finfo, "%s Ch : %d Re : %f Im : %f Re : %f Im : %f\n", info, ch, proj->sol_re->data[j],  proj->sol_im->data[j], proj->d_sol_re->data[j], proj->d_sol_im->data[j]);
 }
 
}

void apply_model( ProjParType *proj )
{
 int i,j;
 int ch = proj->chan2cal;
for( i = 0 ; i < proj->redundant_baselines ; i++)
 { j = proj->ants + i;
   proj->model_vis[ch][i].r = proj->sol_re->data[j];
   proj->model_vis[ch][i].i = proj->sol_im->data[j];
 }
}

void setup_itergain ( ProjParType *proj )
{
 int n = proj->ants + proj->redundant_baselines;
 int i, j;
 double g_amp, g_phs;
 CmplxType g; 
 int ch;

 proj->sol_re = gsl_vector_calloc(n);
 proj->sol_im = gsl_vector_calloc(n);

 proj->d_sol_re = gsl_vector_calloc(n);
 proj->d_sol_im = gsl_vector_calloc(n);

 proj->prev_sol_re = gsl_vector_calloc(n);
 proj->prev_sol_im = gsl_vector_calloc(n);

 //proj->model_vis = (CmplxType**)calloc( proj->redundant_baselines /* proj->chans*/, sizeof(CmplxType*) );
 //for(ch = 0; ch<proj->chans;ch++)
 //proj->model_vis[ch] = (CmplxType*)calloc( proj->redundant_baselines /* proj->chans*/, sizeof(CmplxType) );

 g_amp = 1.0;
 g_phs = 0.0;

 g.r = g_amp * cos(g_phs);
 g.i = g_amp * sin(g_phs);

 for ( i = 0; i < proj->ants; i++ )
 { proj->sol_re->data[i] = real(g);
   proj->sol_im->data[i] = imag(g);
 }
 /*
 parse_VLACalManual( proj );
 gen_V_sky( proj );

 for ( i = 0; i < proj->redundant_baselines; i++ )
 { 
   j = proj->ants + i;
     
   proj->sol_re->data[j] = proj->model_vis[ch][i].r;
   proj->sol_im->data[j] = proj->model_vis[ch][i].i;
 }
 */
}

void iterate_solution( ProjParType * proj )
{
 int i, j, len;
 CmplxType g_i, G_i, g_j, G_j, V_true, C_true,  V_obs;
 Cmplx3Type Vis; 
 int unique_lengths = proj->redundant_baselines;

 CmplxType g_num, M_num;
 float g_dnm, M_dnm, wt;

 double *R_re, *R_im, *Q_re, *Q_im;

 R_re = proj->d_sol_re->data;
 R_im = proj->d_sol_im->data;

 Q_re = proj->d_sol_re->data+(proj->ants);
 Q_im = proj->d_sol_im->data+(proj->ants);

 for ( i = 0 ; i < proj->ants ; i++ )
 { 
 g_num = ret_z(0.0, 0.0);
 g_dnm = 0.0;

  for ( j = 0 ; j < proj->ants ; j++ )
   { if( i == j ) continue;
     
     g_j     = ret_z(proj->sol_re->data[j], proj->sol_im->data[j]);
     G_j     = z_mul(g_j, z_conj(g_j) );
     
     Vis     = ret_V_obs( proj, i, j );
     V_obs   = ret_z(Vis.r, Vis.i);
     wt      = Vis.wt;

     V_true  = ret_V_true( proj , i, j);
     C_true  = z_mul(V_true, z_conj(V_true));

     g_num   = z_add( g_num, scale_z( wt, z_mul(g_j, z_mul(V_true, V_obs)) ) );
     g_dnm   =        g_dnm + (wt * real(G_j) * real(C_true) );

   }
     R_re[i] =  real(g_num) / g_dnm ;
     R_im[i] =  imag(g_num) / g_dnm ;
 }

 for( len = 1 ; len <= unique_lengths ; len++)
 {  M_num = ret_z(0.0, 0.0);
    M_dnm = 0.0;
    for(i = 0 ; i < proj->ants - len ; i++)
    { j = i + len;
      g_i     = ret_z(proj->sol_re->data[i], proj->sol_im->data[i]);
      G_i     = z_mul(g_i, z_conj(g_i) );

      g_j     = ret_z(proj->sol_re->data[j], proj->sol_im->data[j]);
      G_j     = z_mul(g_j, z_conj(g_j) );
     
      Vis     = ret_V_obs(proj, i, j);
      V_obs   = ret_z(Vis.r, Vis.i);
      wt      = Vis.wt;
       
      M_num   = z_add( M_num, scale_z( wt, z_mul( z_mul(g_j, z_conj(g_i)), V_obs ) ) ) ;
      M_dnm   =        M_dnm + (wt * real(G_i) * real(G_j) );

    }
     Q_re[len - 1] =  real(M_num) / M_dnm ;
     Q_im[len - 1] =  imag(M_num) / M_dnm ;
 }
} 

/*
void iterate_solution( ProjParType * proj )
{
 int i, j, len;
 CmplxType g_i, G_i, g_j, G_j, V_true, C_true,  V_obs;
 Cmplx3Type Vis; 
 int unique_lengths = proj->redundant_baselines+1;
 int sign;
 char sgn;
 CmplxType g_num, M_num;
 double g_dnm, M_dnm, wt;
 int rec = proj->recnow-1;
 double *R_re, *R_im, *Q_re, *Q_im;

 R_re = proj->d_sol_re->data;
 R_im = proj->d_sol_im->data;

 Q_re = proj->d_sol_re->data+(proj->ants);
 Q_im = proj->d_sol_im->data+(proj->ants);

 for ( i = 0 ; i < proj->ants ; i++ )
 { 
 g_num = ret_z(0.0, 0.0);
 g_dnm = 0.0;

 if( proj->ant2flag[i] ) 
 { R_re[i] = R_im[i] = 1.0;
   continue;
 }
 //fprintf( finfo, "%s Antenna : %d\n", info, i);
  for ( j = 0 ; j < proj->ants ; j++ )
   { if( i == j ) continue;
       //if( proj->flagmask[rec][i][j] == proj->chans ) continue;
     g_j     = ret_z(proj->sol_re->data[j], proj->sol_im->data[j]);
     G_j     = z_mul(g_j, z_conj(g_j) );
     
     Vis     = ret_V_obs( proj, i, j );
     V_obs   = ret_z(Vis.r, Vis.i);
     wt      = Vis.wt;

     /**** Added this line to consider flags || VRM 3-2-2015 ***
     if( wt < 0.0 ) continue;
     /*{ wt = 0.0;
       V_obs = ret_z(0.0, 0.0);
     }
     *******

     V_true  = ret_V_true( proj , i, j);
     C_true  = z_mul(V_true, z_conj(V_true));

     g_num   = z_add( g_num, scale_z( wt, z_mul(g_j, z_mul(V_true, V_obs)) ) );
     g_dnm   =        g_dnm + (wt * real(G_j) * real(C_true) );

     //fprintf(finfo, " %f %f %f %f\n",  wt, g_dnm, V_true.r, V_true.i);

   }
  /*
  sign = signum(g_num.i);
  if(sign == 1) sgn = '+';
  else if (sign == -1) sgn = '-';
  *
  //fprintf(finfo, "%s g_num : %f %c i %f  g_dnm : %f\n", info, g_num.r, sgn, fabsf(g_num.i), g_dnm);
  
     R_re[i] =  real(g_num) / g_dnm ;
     R_im[i] =  imag(g_num) / g_dnm ;
     if(g_dnm == 0.0) R_re[i] = R_im[i] = 1e6;
 }

 //getchar();

 for( len = 1 ; len <= unique_lengths ; len++)
 {  M_num = ret_z(0.0, 0.0);
    M_dnm = 0.0;
    for(i = 0 ; i < proj->ants - len ; i++)
    { j = i + len;
      g_i     = ret_z(proj->sol_re->data[i], proj->sol_im->data[i]);
      G_i     = z_mul(g_i, z_conj(g_i) );

      g_j     = ret_z(proj->sol_re->data[j], proj->sol_im->data[j]);
      G_j     = z_mul(g_j, z_conj(g_j) );
     
      Vis     = ret_V_obs(proj, i, j);
      V_obs   = ret_z(Vis.r, Vis.i);
      wt      = Vis.wt;

      /**** Added this line to consider flags || VRM 3-2-2015 ***
      if( wt < 0.0 ) continue;
      /*{ wt = 0.0;
        V_obs = ret_z(0.0, 0.0);
      }
      *******

      M_num   = z_add( M_num, scale_z( wt, z_mul( z_mul(g_j, z_conj(g_i)), V_obs ) ) ) ;
      M_dnm   =        M_dnm + (wt * real(G_i) * real(G_j) );
      
    }
     Q_re[len - 1] =  real(M_num) / M_dnm ;
     Q_im[len - 1] =  imag(M_num) / M_dnm ;
     if(M_dnm == 0.0) Q_re[len-1] = Q_im[len-1] = 1e6;
 }
} 

*/
void compute_MSE( ProjParType *proj, FILE *fMSE)
{
 int i, N = proj->d_sol_re->size;
 gsl_vector *res_re, *res_im;
 double energy = 0.0;
 CmplxType temp;

 res_re = gsl_vector_calloc(N);
 res_im = gsl_vector_calloc(N);

 gsl_vector_add(res_re, proj->sol_re);
 gsl_vector_add(res_im, proj->sol_im);

 gsl_vector_sub(res_re, proj->d_sol_re);
 gsl_vector_sub(res_im, proj->d_sol_im);
 
 for(i = 0; i < proj->ants; i++)  
 { temp = ret_z(res_re->data[i], res_im->data[i] );
   energy += real( z_mul(temp, z_conj(temp) ) );
 }
 
 energy /= N;

 fprintf(fMSE, "%d %lf\n", proj->iter, energy);

 gsl_vector_free(res_re);
 gsl_vector_free(res_im);

}

double compute_chisq( ProjParType *proj, FILE *fchisq)
{
 int bl, n, i, j;
 int ch = proj->chans / 2 ;
 long *blindex = proj->blindex;
 int rbl;
 CmplxType V[proj->baselines];
 CmplxType g[proj->ants];
 CmplxType M[proj->redundant_baselines];
 Cmplx3Type *vis;
 RGParType *uvw;
 BaseParType *baseline = proj->Base;
 CmplxType res[proj->baselines];
 double chisq = 0.0;

 for(bl = 0; bl < proj->baselines; bl++)
 { vis = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);
   uvw = (RGParType *)  (proj->RecBuf + bl*(proj->recwords));

   for(n = 0; n < proj->ants; n++)
   { if( baseline[bl].id1 == proj->Ant[n].id ) i = n;
     if( baseline[bl].id2 == proj->Ant[n].id ) j = n;
   }

   rbl = blindex[ baseline[bl].id2 - baseline[bl].id1 ] ;

   V[bl] = ret_z( vis[ch].r, vis[ch].i );
   g[i] = ret_z( proj->sol_re->data[i], proj->sol_im->data[i] );
   g[j] = ret_z( proj->sol_re->data[j], proj->sol_im->data[j] );
   M[ rbl ] = ret_z( proj->sol_re->data[ proj->ants + rbl ], proj->sol_im->data[ proj->ants + rbl ] );
   
   /* V_ij - (g_i . g_J* . M_|i-j|) */
   res[bl] = z_sub( V[bl], z_mul( z_mul( g[i], z_conj( g[j] ) ), M[rbl] ) );
   chisq += powf( z_abs( scale_z( proj->sigma[bl], res[bl] ) ), 2 );
 }
 
 fprintf(fchisq, "%d %lf\n", proj->iter, chisq);

 return chisq;
}

int test_convergence( ProjParType *proj )
{
 int i, N = proj->d_sol_re->size;
 gsl_vector *diff_re, *diff_im;
 double relative_change[N];
 double largest_change = 0.0;
 double eps = 0.005;

 diff_re = gsl_vector_calloc(N);
 diff_im = gsl_vector_calloc(N);

 gsl_vector_add(diff_re, proj->sol_re);
 gsl_vector_add(diff_im, proj->sol_im);

 gsl_vector_sub(diff_re, proj->prev_sol_re);
 gsl_vector_sub(diff_im, proj->prev_sol_im);

 for(i = 0; i < N; i++)
 { relative_change[i] = z_abs( ret_z(diff_re->data[i], diff_im->data[i]) ) / z_abs( ret_z(proj->sol_re->data[i], proj->sol_im->data[i]) );
   if( relative_change[i] >= largest_change ) largest_change = relative_change[i];
 }

 gsl_vector_free(diff_re);
 gsl_vector_free(diff_im);

 if ( largest_change < eps ) return 1;
 else return 0;

}

void update_solutions( ProjParType *proj )
{
 double step = STEP;

 gsl_vector_memcpy(proj->prev_sol_re, proj->sol_re);
 gsl_vector_memcpy(proj->prev_sol_im, proj->sol_im);

 gsl_vector_scale (proj->d_sol_re, step);
 gsl_vector_scale (proj->sol_re, (1-step) );
 gsl_vector_add   (proj->sol_re, proj->d_sol_re);

 gsl_vector_scale (proj->d_sol_im, step);
 gsl_vector_scale (proj->sol_im, (1-step) );
 gsl_vector_add   (proj->sol_im, proj->d_sol_im);

}


void compute_step_size ( ProjParType *proj )
{

}


Cmplx3Type ret_V_obs( ProjParType *proj, int i, int j)
 {
 int bl, l = 0, k = 0, ch = proj->chan2cal;
 Cmplx3Type *V;
 Cmplx3Type V_obs;

 l = abs( i-j ) - 1;
 bl = 0;
 for( k = 1 ; k <= min(i,j) ; k++ ) bl += (proj->ants-1) - k;
 bl += l + min(i,j); 
 //fprintf(finfo, "%s Baseline : %d %d %d ", info, i, j, bl);
 V = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);

 memcpy(&V_obs, V+ch, sizeof(Cmplx3Type) );
 V_obs.i = (i>j) ? (-V_obs.i) : (V_obs.i) ;

 return V_obs;

 }

CmplxType ret_V_true( ProjParType *proj, int i, int j)
{
 CmplxType V_true;
 long *blindex = proj->blindex;
 int ant1, ant2;

 ant1 = proj->Ant[i].id;
 ant2 = proj->Ant[j].id;

 V_true.r = proj->sol_re->data[ proj->ants + blindex[abs(ant2 - ant1)] ] ;
 V_true.i = proj->sol_im->data[ proj->ants + blindex[abs(ant2 - ant1)] ] ;

 V_true.i = (i>j) ? (V_true.i) : (-V_true.i) ;

 return V_true;
}

void gen_V_sky( ProjParType *proj )
{
 int ii=0, isrc=0;
 long *blindex = proj->blindex;
 double c = LIGHTVEL;
 double nu, v;
 BaseParType *baseline;
 int idx;
 int ch;

 baseline   = proj->Base;

 for( ii = 1; ii < proj->ORTelements ; ii++ )
 { if( proj->unique_baseline[ii] > 1 ) 
  { for ( ch = 0 ; ch < proj->chans ; ch++ )
   { nu = proj->bandcentre;// - bandwidth/2 + ch*deltanu + deltanu/2;
     v = baseline[blindex[ii]].v * nu / c ;
     idx = blindex[ii];// * proj->chans + ch;
       for (isrc = 0; isrc < proj->nsrc; isrc++ )
       { proj->model_vis[ch][ idx ].r += pow(proj->bandcentre/nu, proj->skysrc[isrc].alpha) * proj->skysrc[isrc].S * cos( 2*M_PI*proj->skysrc[isrc].m*v ); 
	 proj->model_vis[ch][ idx ].i += pow(proj->bandcentre/nu, proj->skysrc[isrc].alpha)*proj->skysrc[isrc].S * sin( 2*M_PI*proj->skysrc[isrc].m*v ); 
       }
       fprintf(finfo, "%s Model computed for [ %d / %d ] redundant baselines \r", info, ii, proj->redundant_baselines );
   }
  }
 }
 fprintf(stderr, "\n");
}

void compute_error_matrix( ProjParType *proj )
{
 int k,l;
 int bl;
 long *blindex = proj->blindex;
 int N = proj->sol_re->size;
 int signum[N];
 BaseParType *baseline = proj->Base;
 CmplxType pH;
 double sigma;
 CmplxType alpha[N][N];
 gsl_complex cov, std, H;
 gsl_permutation *p;
 FILE *fCov; 
 gsl_matrix_complex *a, *C;

 a = gsl_matrix_complex_calloc(N, N);
 C = gsl_matrix_complex_calloc(N, N);
 p = gsl_permutation_calloc(N);

 /* Computing the Hessian */
 fCov = fopen( "Hessian.matx", "w" );
 for( k = 0; k < N; k++)
 { for( l = 0; l < N; l++ )
   { alpha[k][l] = ret_z( 0.0, 0.0 );
     if( proj->ants > PHASE_I && k != l ) continue;
     for( bl = 0 ; bl < proj->baselines ; bl++ )
     { if( proj->unique_baseline[ blindex[ baseline[bl].id2 - baseline[bl].id1 ] ] <= 1 ) continue;
       pH = pseudo_Hessian( proj, bl, k, l );
       sigma = proj->sigma[bl];
       alpha[k][l] = z_add( alpha[k][l], scale_z( (-1.0/(sigma*sigma)), pH ) );
     }
#ifdef _DEBUG_
     fprintf(finfo, "\n%s %d %d", info, k, l);
#endif
     fprintf(fCov, "%f %f ", alpha[k][l].r, alpha[k][l].i);
     H.dat[0] = alpha[k][l].r;
     H.dat[1] = alpha[k][l].i;
     gsl_matrix_complex_set( a, k, l, H );
   }
     fprintf(fCov, "\n");
 }
 fclose( fCov );

 if( proj->ants > PHASE_I )
 {
 /* Writing the covariance matrix to file */
 fCov = fopen( "cov.matx", "w" );
 
 for(k = 0 ; k < N ; k++ )
 { cov = gsl_matrix_complex_get( a, k, k );
   std = gsl_complex_sqrt( cov );
   std = gsl_complex_inverse(std);
   fprintf(fCov, "%f %f ", std.dat[0], std.dat[1]);
#ifdef _DEBUG_
   fprintf(finfo, "\n%s Par %d : %lf", info, k, hypot(std.dat[0], std.dat[1]) );
#endif
 }
   fprintf(fCov, "\n");
 }

 else
 { gsl_linalg_complex_LU_decomp( a, p, signum );
   gsl_linalg_complex_LU_invert( a, p, C );
 
 /* Writing the covariance matrix to file */
 fCov = fopen( "cov.matx", "w" );
 
 for(k = 0 ; k < N ; k++ )
 { for(l = 0 ; l < N ; l++ )
   { cov = gsl_matrix_complex_get( C, k, l );
     std = gsl_complex_sqrt( cov );
    fprintf(fCov, "%f %f ", std.dat[0], std.dat[1]);
#ifdef _DEBUG_
    if ( k == l) fprintf(finfo, "\n%s Par %d : %lf", info, k, hypot(std.dat[0], std.dat[1]) );
#endif
   }
   fprintf(fCov, "\n");
  }
 }

 fclose(fCov);
 fprintf(finfo, "\n%s Error matrix written to cov.matx", info);
 
 gsl_permutation_free(p);
 gsl_matrix_complex_free(a);
 gsl_matrix_complex_free(C);
}


void compute_CRLB( ProjParType *proj )
{
 int k,l;
 int bl;
 long *blindex = proj->blindex;
 int N = proj->sol_re->size;
 int signum[N];
 BaseParType *baseline = proj->Base;
 CmplxType pH;
 double sigma;
 CmplxType alpha[N][N];
 gsl_complex cov, std, H;
 gsl_permutation *p;
 FILE *fCov; 
 gsl_matrix_complex *a, *C;
 int i, ch=proj->chans/2;

 /* Assigning the true values to the solution vector*/
 for( i = 0 ; i < proj->ants; i++ )
 { proj->sol_re->data[i] = proj->Ant[i].g.r;
   proj->sol_im->data[i] = proj->Ant[i].g.i;
 }

 for( i = 0 ; i < proj->redundant_baselines ; i++ )
 { proj->sol_re->data[ i + proj->ants ] = proj->model_vis[ch][i].r;
   proj->sol_im->data[ i + proj->ants ] = proj->model_vis[ch][i].i;
 }

 a = gsl_matrix_complex_calloc(N, N);
 C = gsl_matrix_complex_calloc(N, N);
 p = gsl_permutation_calloc(N);

 /* Computing the Fisher matrix */
 fCov = fopen( "Fisher.matx", "w" );
 for( k = 0; k < N; k++)
 { for( l = 0; l < N; l++ )
   { alpha[k][l] = ret_z( 0.0, 0.0 );
     if( proj->ants > PHASE_I && k != l ) continue;
     for( bl = 0 ; bl < proj->baselines ; bl++ )
     { if( proj->unique_baseline[ blindex[ baseline[bl].id2 - baseline[bl].id1 ] ] <= 1 ) continue;
       pH = pseudo_Hessian( proj, bl, k, l );
       sigma = proj->sigma[bl];
       alpha[k][l] = z_add( alpha[k][l], scale_z( (-1.0/(sigma*sigma)), pH ) );
     }
#ifdef _DEBUG_
     fprintf(finfo, "\n%s %d %d", info, k, l);
#endif
     fprintf(fCov, "%f %f ", alpha[k][l].r, alpha[k][l].i);
     H.dat[0] = alpha[k][l].r;
     H.dat[1] = alpha[k][l].i;
     gsl_matrix_complex_set( a, k, l, H );
   }
     fprintf(fCov, "\n");
 }
 fclose( fCov );

 if( proj->ants > PHASE_I )
 {
 /* Writing the CRB matrix to file */
 fCov = fopen( "CRB.matx", "w" );
 
 for(k = 0 ; k < N ; k++ )
 { cov = gsl_matrix_complex_get( a, k, k );
   std = gsl_complex_sqrt( cov );
   std = gsl_complex_inverse(std);
   fprintf(fCov, "%f %f ", std.dat[0], std.dat[1]);
#ifdef _DEBUG_
   fprintf(finfo, "\n%s Par %d : %lf", info, k, hypot(std.dat[0], std.dat[1]) );
#endif
 }
   fprintf(fCov, "\n");
 }

 else
 { gsl_linalg_complex_LU_decomp( a, p, signum );
   gsl_linalg_complex_LU_invert( a, p, C );

 /* Writing the CRB matrix to file */
 fCov = fopen( "CRB.matx", "w" );
 
 for(k = 0 ; k < N ; k++ )
 { for(l = 0 ; l < N ; l++ )
   {
       cov = gsl_matrix_complex_get( C, k, l );
       std = gsl_complex_sqrt( cov );
       fprintf(fCov, "%f %f ", std.dat[0], std.dat[1]);
#ifdef _DEBUG_
       if ( k == l) fprintf(finfo, "\n%s Par %d : %lf", info, k, hypot(std.dat[0], std.dat[1]) );
#endif
   }
   fprintf(fCov, "\n");
 }
 }

 fclose(fCov);
 fprintf(finfo, "\n%s CRB matrix written to CRB.matx", info);
 
 gsl_permutation_free(p);
 gsl_matrix_complex_free(a);
 gsl_matrix_complex_free(C);
}

CmplxType pseudo_Hessian( ProjParType *proj, int bl, int k, int l )
{
 BaseParType *baseline = proj->Base;
 int i, j, n, rbl;
 CmplxType residue, pH, pH1, pH2, pH3, pH4;
 CmplxType d_y_gk, d_y_gl, d_ystar_gk, d_ystar_gl, d_ystar_gstark, d_ystar_gstarl, d_y_gstark, d_y_gstarl, d2y_gk_gl;
 long *blindex = proj->blindex;
 CmplxType g_i, g_j, M_ij;
 Cmplx3Type *V;
 int ch = proj->chans / 2;

 d_y_gk = ret_z(0.0, 0.0);
 d_y_gl =  ret_z(0.0, 0.0);
 d_ystar_gk =  ret_z(0.0, 0.0);
 d_ystar_gl =  ret_z(0.0, 0.0);
 d_ystar_gstark =  ret_z(0.0, 0.0);
 d_ystar_gstarl =  ret_z(0.0, 0.0);
 d_y_gstark =  ret_z(0.0, 0.0);
 d_y_gstarl =  ret_z(0.0, 0.0);
 d2y_gk_gl =  ret_z(0.0, 0.0);

 for(n = 0; n < proj->ants; n++)
   { if( baseline[bl].id1 == proj->Ant[n].id ) i = n;
     if( baseline[bl].id2 == proj->Ant[n].id ) j = n;
   }

 V = (Cmplx3Type*)(proj->RecBuf + bl*(proj->recwords) + proj->pcount);
 rbl = blindex[ baseline[bl].id2 - baseline[bl].id1 ] + proj->ants ;
 g_i = ret_z(  proj->sol_re->data[i], proj->sol_im->data[i] );
 g_j = ret_z(  proj->sol_re->data[j], proj->sol_im->data[j] );
 M_ij = ret_z( proj->sol_re->data[ rbl ], proj->sol_im->data[ rbl ] );
 residue = z_sub( ret_z(V[ch].r, V[ch].i), z_mul(z_mul(g_i, z_conj(g_j)), M_ij));

 if ( k < proj->ants )
 { 
   if ( k == i )
   { d_y_gk = z_mul( z_conj(g_j), M_ij );
     d_ystar_gstark = z_mul( g_j, z_conj(M_ij) );
   }
   else
   { d_y_gk = ret_z( 0.0, 0.0 );
     d_ystar_gstark = ret_z(0.0, 0.0);
   }

   if ( k == j )
   { d_ystar_gk = z_mul( z_conj(g_i), z_conj(M_ij) );
     d_y_gstark = z_mul( g_i, M_ij );
   }
   else 
   { d_ystar_gk = ret_z( 0.0, 0.0 );
     d_y_gstark = ret_z( 0.0, 0.0 );
   }
 }


 if ( l < proj->ants )
 { 
   if ( l == i )
   { d_y_gl = z_mul( z_conj(g_j), M_ij );
     d_ystar_gstarl = z_mul( g_j, z_conj(M_ij) );
   }
   else
   { d_y_gl = ret_z( 0.0, 0.0 );
     d_ystar_gstarl = ret_z(0.0, 0.0);
   }

   if ( l == j )
   { d_ystar_gl = z_mul( z_conj(g_i), z_conj(M_ij) );
     d_y_gstarl = z_mul( g_i, M_ij );
   }
   else 
   { d_ystar_gl = ret_z( 0.0, 0.0 );
     d_y_gstarl = ret_z( 0.0, 0.0 );
   }
 }

 if( k >= proj->ants )
 { 
   if( k == rbl )
   { d_y_gk = z_mul( g_i, z_conj( g_j ) );
     d_ystar_gstark = z_mul( z_conj(g_i), g_j );
   }
   else
   { d_y_gk = ret_z(0.0, 0.0);
     d_ystar_gstark = ret_z(0.0, 0.0);
   }

   d_ystar_gk = ret_z(0.0, 0.0);
   d_y_gstark = ret_z(0.0, 0.0);
  
 }

 if( l >= proj->ants )
 { 
   if( l == rbl )
   { d_y_gl = z_mul( g_i, z_conj( g_j ) );
     d_ystar_gstarl = z_mul( z_conj(g_i), g_j );
   }
   else
   { d_y_gl = ret_z(0.0, 0.0);
     d_ystar_gstarl = ret_z(0.0, 0.0);
   }

   d_ystar_gl = ret_z(0.0, 0.0);
   d_y_gstarl = ret_z(0.0, 0.0);
  
 }
 
 if( k < proj->ants && l < proj->ants )
 { d2y_gk_gl = ret_z( 0.0, 0.0 );
   if( k == i && l == j ) d2y_gk_gl = z_mul( M_ij, z_conj(residue) );
   if( k == j && l == i ) d2y_gk_gl = z_mul( z_conj(M_ij), residue );
 }

 if( (k < proj->ants && l >= proj->ants) )
 { if ( k != j ) d2y_gk_gl = ret_z(0.0, 0.0);
   else if( k == j && l == rbl ) d2y_gk_gl = z_mul( g_i, z_conj(residue) );
 }

 if( (l < proj->ants && k >= proj->ants) )
 { if ( l != j ) d2y_gk_gl = ret_z(0.0, 0.0);
   else if( l == j && k == rbl ) d2y_gk_gl = z_mul( z_conj(g_i), residue ) ;
 }
 
 pH1 = z_mul( d_y_gk, scale_z(-1.0, d_ystar_gstarl) );
 pH2 = z_mul( d_ystar_gk, scale_z(-1.0, d_y_gstarl) );

 pH3 = z_mul( d_y_gl, scale_z(-1.0, d_ystar_gstark) );
 pH4 = z_mul( d_ystar_gl, scale_z(-1.0, d_y_gstark) );

 pH = z_add( z_add(z_add( pH1, pH2), z_add(pH3, pH4) ), d2y_gk_gl );

 return pH;
}


CmplxType Jacobian( ProjParType *proj, int bl, int k )
{
 BaseParType *baseline = proj->Base;
 int i, j, n, rbl;
 CmplxType residue, pH, pH1, pH2, pH3, pH4;
 CmplxType d_y_gk;
 long *blindex = proj->blindex;
 CmplxType g_i, g_j, M_ij;
 Cmplx3Type *V;
 int ch = proj->chans / 2;

 d_y_gk = ret_z(0.0, 0.0);

 for(n = 0; n < proj->ants; n++)
   { if( baseline[bl].id1 == proj->Ant[n].id ) i = n;
     if( baseline[bl].id2 == proj->Ant[n].id ) j = n;
   }

 V = (Cmplx3Type*)(proj->RecBuf + bl*(proj->recwords) + proj->pcount);
 rbl = blindex[ baseline[bl].id2 - baseline[bl].id1 ] + proj->ants ;
 g_i = ret_z(  proj->sol_re->data[i], proj->sol_im->data[i] );
 g_j = ret_z(  proj->sol_re->data[j], proj->sol_im->data[j] );
 M_ij = ret_z( proj->sol_re->data[ rbl ], proj->sol_im->data[ rbl ] );

 if ( k < proj->ants )
 { 
   if ( k == i )
   d_y_gk = z_mul( z_conj(g_j), M_ij );
 }

 if( k >= proj->ants )
 { 
   if( k == rbl )
   d_y_gk = z_mul( g_i, z_conj( g_j ) );
 }

 return d_y_gk;
}



int getpar_dynspect( ProjParType *proj, int ant1, int ant2)
{
 DisplayBufParType *ds = &(proj->dispBuf);
 int bl = 0;
 BaseParType *base = proj->Base;
 AntParType *Ant = proj->Ant;
 int found = 0;
 int temp;

 ds->tick = proj->tick * proj->preint;
 ds->deltanu = proj->bandwidth / proj->chans;

 if(ant1 > ant2) 
 { temp = ant1;
   ant1 = ant2;
   ant2 = temp;
 }

 ds->ant1 = ant1;
 ds->ant2 = ant2;

 ds->nrec = ds->stop_rec - ds->start_rec + 1;
 ds->nchan = ds->stop_chan - ds->start_chan + 1;

 ds->databuf = (double*)calloc(ds->nrec*ds->nchan*4, sizeof(double) );
 ds->xaxis = (double*)calloc(1, sizeof(double));

 do{
 if( (!strcmp(Ant[ds->ant1].name, base[bl].ant1)) && !(strcmp(Ant[ds->ant2].name, base[bl].ant2)) ) found = 1;
 bl++;
 } while(!found);

 return --bl;
}

void free_display_databuf( ProjParType *proj )
{ DisplayBufParType *ds = &(proj->dispBuf);
  free(ds->databuf);
  free(ds->xaxis);
}

int extract_baseline( ProjParType *proj, int bl )
{
 static int recnum = 0;
 Cmplx3Type *V;
 DisplayBufParType *ds = &(proj->dispBuf);
 int ch, chans, recs, offset;
 int chstr = 0;

 chans = ds->nchan;
 recs = ds->nrec;
 offset = chans * recs;

 V = (Cmplx3Type*)(proj->RecBuf + bl*(proj->recwords) + proj->pcount);
 
  for(ch = ds->start_chan-1 ; ch < ds->stop_chan; ch++ )
  { 
    chstr = ch - ds->start_chan + 1;

    if ( V[ch].wt < 0.0 ) continue;

    ds->databuf[recnum*chans + chstr +    0  ] = hypotf(V[ch].r, V[ch].i);
    ds->databuf[recnum*chans + chstr + offset] = rad2deg(atan2f(V[ch].i, V[ch].r)); 

    ds->databuf[recnum*chans + chstr + 0      + 2*offset] = V[ch].r;
    ds->databuf[recnum*chans + chstr + offset + 2*offset] = V[ch].i;
  }
  recnum++;
 if (recnum >= recs) recnum = 0;
 return bl;
}

void buffer_range( DisplayBufParType *ds, int X, int Y )
{
 int offset = X * Y, start = 2*(X*Y)*(ds->altswtch);
 int i;
 double min[2], max[2];

 min[0] = max[0] = ds->databuf[start];
 for(i = start ; i < offset+start; i++)
 { if(ds->databuf[i] > max[0]) max[0] = ds->databuf[i];
   if(ds->databuf[i] < min[0]) min[0] = ds->databuf[i];
 }

 min[1] = max[1] = ds->databuf[start+offset];
 for(i = start ; i < offset+start; i++)
 { if(ds->databuf[offset + i] > max[1]) max[1] = ds->databuf[offset + i];
   if(ds->databuf[offset + i] < min[1]) min[1] = ds->databuf[offset + i];
 }

 ds->min[0] = min[0];
 ds->min[1] = min[1];
 ds->max[0] = max[0];
 ds->max[1] = max[1];
}


void apply_solutions( ProjParType *proj )
{
 static int row = 0;
 int i=0, j=0, n, count = proj->preint;
 Cmplx3Type *vis, *bpass;
 CmplxType V_ij, M_ij, g_i, g_j;
 BaseParType *baseline = proj->Base;
 GainTableType *gaintab = &(proj->gtab);
 GainTableRow *gtrow = gaintab->row;
 BPassTableType *bpasstab = &(proj->bptab);
 BPassTableRow *bptrow = bpasstab->row;
 CmplxType bandpass, g_t0_p0, g_t_p0, gain;
 PolarType propagator;
 int bl;
 int ch;

  for ( bl = 0 ; bl < proj->baselines ; bl++ )
  { vis = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);
    for(n = 0; n < proj->ants; n++)
    { if( baseline[bl].id1 == proj->Ant[n].id ) i = n;
      if( baseline[bl].id2 == proj->Ant[n].id ) j = n;
    }
    for( ch = 0 ; ch < proj->chans ; ch++ )
    { 
      g_t0_p0 = ret_z(gtrow[0].gain[i].r, gtrow[0].gain[i].i);
      g_t_p0  = ret_z(gtrow[row].gain[i].r, gtrow[row].gain[i].i);
      propagator.abs = z_abs(g_t0_p0)/z_abs(g_t_p0);
      //propagator.abs = 1.0;
      propagator.arg = z_arg(g_t0_p0) - z_arg(g_t_p0);
      gain = z_mul(g_t_p0, Polar_to_Cmplx( propagator ) );
      bandpass = ret_z(bptrow[row].bpass[i*bpasstab->nchan + ch].r, bptrow[row].bpass[i*bpasstab->nchan + ch].i);
      g_i = z_mul(gain, bandpass);

      g_t0_p0 = ret_z(gtrow[0].gain[j].r, gtrow[0].gain[j].i);
      g_t_p0  = ret_z(gtrow[row].gain[j].r, gtrow[row].gain[j].i);
      propagator.abs =  z_abs(g_t0_p0)/z_abs(g_t_p0);
      //propagator.abs =  1.0;
      propagator.arg = z_arg(g_t0_p0) - z_arg(g_t_p0);
      gain = z_mul(g_t_p0, Polar_to_Cmplx( propagator ) );
      bandpass = ret_z(bptrow[row].bpass[j*bpasstab->nchan + ch].r, bptrow[row].bpass[j*bpasstab->nchan + ch].i);
      g_j = z_mul(gain, bandpass);

      V_ij = ret_z(vis[ch].r, vis[ch].i);
      M_ij = z_div( V_ij, z_mul( g_i, z_conj(g_j)) );
      if(vis[ch].wt < 0.0) M_ij = ret_z(0.0, 0.0);
      vis[ch].r   = real(M_ij);
      vis[ch].i   = imag(M_ij);
    }
  }
 row++;
 if ( row % 100 == 0 )fprintf(finfo, "%s %d solutions applied \r", info, row);
}

void apply_gain( ProjParType *proj )
{
  int bl, n, i, j, ch;
  Cmplx3Type *vis;
  CmplxType V_ij, M_ij, g_i, g_j;
  BaseParType *baseline = proj->Base;

  ch = proj->chan2cal;

  for ( bl = 0 ; bl < proj->baselines ; bl++ )
  { vis = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);
    for(n = 0; n < proj->ants; n++)
    { if( baseline[bl].id1 == proj->Ant[n].id ) i = n;
      if( baseline[bl].id2 == proj->Ant[n].id ) j = n;
    }
    
    g_i = ret_z(proj->sol_re->data[i], proj->sol_im->data[i]);
    g_j = ret_z(proj->sol_re->data[j], proj->sol_im->data[j]);

    V_ij = ret_z(vis[ch].r, vis[ch].i);
    M_ij = z_div( V_ij, z_mul( g_i, z_conj(g_j)) );
    if(vis[ch].wt < 0.0) M_ij = ret_z(0.0, 0.0);
    vis[ch].r   = real(M_ij);
    vis[ch].i   = imag(M_ij);
    //fprintf(finfo, "%s Bl : %d Chan : %d Real : %3.1lf Imag : %3.1lf\n", info, bl+1, ch+1, vis[ch].r, vis[ch].i);
  }
  //getchar();
}

 /*
void apply_solutions( ProjParType *proj )
{
 static int row = 0;
 int i=0, j=0, n, count = proj->preint;
 Cmplx3Type *vis, *bpass;
 CmplxType V_ij, M_ij, g_i, g_j;
 BaseParType *baseline = proj->Base;
 GainTableType *gaintab = &(proj->gtab);
 GainTableRow *gtrow = gaintab->row;
 BPassTableType *bpasstab = &(proj->bptab);
 BPassTableRow *bptrow = bpasstab->row;
 CmplxType bandpass, g_t0_p0, g_t_p0, gain;
 PolarType propagator;
 int bl;
 int ch;

  for ( bl = 0 ; bl < proj->baselines ; bl++ )
  { vis = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);
    for(n = 0; n < proj->ants; n++)
    { if( baseline[bl].id1 == proj->Ant[n].id ) i = n;
      if( baseline[bl].id2 == proj->Ant[n].id ) j = n;
    }
    for( ch = 0 ; ch < proj->chans ; ch++ )
    { 
      g_t0_p0 = ret_z(gtrow[0].gain[i].r, gtrow[0].gain[i].i);
      bandpass = ret_z(bptrow[0].bpass[i*bpasstab->nchan + ch].r, bptrow[0].bpass[i*bpasstab->nchan + ch].i);
      g_i = z_mul(g_t0_p0, bandpass);

      g_t0_p0 = ret_z(gtrow[0].gain[j].r, gtrow[0].gain[j].i);
      bandpass = ret_z(bptrow[0].bpass[j*bpasstab->nchan + ch].r, bptrow[0].bpass[j*bpasstab->nchan + ch].i);
      g_j = z_mul(g_t0_p0, bandpass);

      V_ij = ret_z(vis[ch].r, vis[ch].i);
      M_ij = z_div( V_ij, z_mul( g_i, z_conj(g_j)) );
      if(vis[ch].wt < 0.0) M_ij = ret_z(0.0, 0.0);
      vis[ch].r   = real(M_ij);
      vis[ch].i   = imag(M_ij);
    }
  }
 row++;
}
*/

void copyBuf_bpass( ProjParType *proj )
{
 memcpy(proj->RecBuf, proj->bpBuf, proj->recwords*proj->baselines*sizeof(double) );
}

void get_bpass( ProjParType *proj )
{
 int bl, ch2cal = proj->chans/2, ch;
 Cmplx3Type *band, *vis;
 CmplxType V_ij, B_ij, V_ch2cal;

 for ( bl = 0 ; bl < proj->baselines ; bl++ )
 { vis  = (Cmplx3Type *)(proj->RecBuf + bl*(proj->recwords) + proj->pcount);
   band = (Cmplx3Type *)(proj->bpBuf + bl*(proj->recwords) + proj->pcount);

   V_ch2cal = ret_z(vis[ch2cal].r, vis[ch2cal].i);

   for(ch = 0 ; ch < proj->chans ; ch++ )
   { V_ij = ret_z(vis[ch].r, vis[ch].i);
     B_ij = z_div(V_ij, V_ch2cal);
     band[ch].r = real(B_ij);
     band[ch].i = imag(B_ij);
   }
 }
}

void getpar_timevis( ProjParType *proj)
{
  DisplayBufParType *ds = &(proj->dispBuf);
  ds->databuf = (double*)calloc(4*proj->baselines*proj->nrecs, sizeof(double) );
  ds->xaxis = (double*)calloc(proj->nrecs, sizeof(double) );
}

void getpar_instspec( ProjParType *proj)
{
  DisplayBufParType *ds = &(proj->dispBuf);
  ds->databuf = (double*)calloc(4*proj->baselines*proj->chans, sizeof(double) );
  ds->xaxis = (double*)calloc(proj->chans, sizeof(double) );
}

void getpar_beamdisp( ProjParType *proj)
{
  DisplayBufParType *ds = &(proj->dispBuf);
  ds->databuf = (double*)calloc(4*((int)(proj->nrecs/proj->preint))*proj->chans, sizeof(double) );
  ds->xaxis = (double*)calloc(proj->chans, sizeof(double) );
}

void copy_beam_to_dispBuf( ProjParType *proj )
{  DisplayBufParType *ds = &(proj->dispBuf);
   BeamParType *beam = &(proj->beam);
   long offset = proj->chans * (int)(proj->nrecs/proj->preint);
   int row, ch;
   for( row = 0 ; row < proj->nrecs/proj->preint ; row++ )
   {for ( ch = ds->chan2blank ; ch < proj->chans ; ch++ )
    {ds->xaxis[ch] = (double)ch;
     ds->databuf[row*proj->chans + ch ] = hypotf(beam->data[row*proj->chans + ch].r, beam->data[row*proj->chans + ch].i);
     ds->databuf[row*proj->chans + ch + offset] = rad2deg(atan2f( beam->data[row*proj->chans + ch].i,  beam->data[row*proj->chans + ch].r));
     ds->databuf[row*proj->chans + ch + 2*offset] = beam->data[row*proj->chans + ch].r;
     ds->databuf[row*proj->chans + ch + 3*offset] = beam->data[row*proj->chans + ch].i;
     }
   }
}



void extract_bandpass( ProjParType *proj )
{
  BaseParType *baseline = proj->Base;
  DisplayBufParType *ds = &(proj->dispBuf);
  BPassTableType *bpasstab = &(proj->bptab);
  BPassTableRow *bptrow = bpasstab->row;
  int bl, ch, i, j, n;
  Cmplx3Type *ant1_band, *ant2_band;
  CmplxType a1, a2, b;
  int row = proj->chan2cal;
  long offset = proj->baselines * proj->chans;

  for( bl = 0 ; bl < proj->baselines ; bl++ )
  { 
   for(n = 0; n < proj->ants; n++)
    { if( baseline[bl].id1 == proj->Ant[n].id ) i = n;
      if( baseline[bl].id2 == proj->Ant[n].id ) j = n;
    }

   ant1_band = (Cmplx3Type*)&(bptrow[row].bpass[i*bpasstab->nchan]);
   ant2_band = (Cmplx3Type*)&(bptrow[row].bpass[j*bpasstab->nchan]);

   for( ch = ds->chan2blank ; ch < proj->chans ; ch++ )
   { a1 = ret_z(ant1_band[ch].r, ant1_band[ch].i);
     a2 = ret_z(ant2_band[ch].r, ant2_band[ch].i);
     b = z_mul(a1,a2);
     if ( ch == proj->chans / 2) 
	 fprintf(finfo, "%s %4s : %4.1f   %4s : %4.1f \n", info, baseline[bl].ant1, rad2deg(z_arg(a1)), baseline[bl].ant2, rad2deg(z_arg(a2)));
     ds->xaxis[ch] = (double)ch;
     ds->databuf[bl*proj->chans + ch           ] = hypotf(b.r, b.i);
     ds->databuf[bl*proj->chans + ch + offset  ] = rad2deg(atan2f(b.i, b.r));
     ds->databuf[bl*proj->chans + ch + 2*offset] = b.r;
     ds->databuf[bl*proj->chans + ch + 3*offset] = b.i;
   }
  }

}

void extract_rec( ProjParType *proj )
{
  DisplayBufParType *ds = &(proj->dispBuf);
  int bl, ch;
  Cmplx3Type *V;
  long offset = proj->baselines * proj->chans;
  for( bl = 0 ; bl < proj->baselines ; bl++ )
  { V = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);
    for( ch = ds->chan2blank ; ch < proj->chans ; ch++ )
    { ds->xaxis[ch] = (double)ch;
      ds->databuf[bl*proj->chans + ch           ] = hypotf(V[ch].r, V[ch].i);
      ds->databuf[bl*proj->chans + ch + offset  ] = rad2deg(atan2f(V[ch].i, V[ch].r));
      //fprintf(finfo, "%s Bl %d Ch %d Abs : %lf Arg : %lf\n", info, bl, ch,  ds->databuf[bl*proj->chans + ch],  ds->databuf[bl*proj->chans + ch + offset]);
      ds->databuf[bl*proj->chans + ch + 2*offset] = V[ch].r;
      ds->databuf[bl*proj->chans + ch + 3*offset] = V[ch].i;
    }
    //getchar();
  }
  
}

void extract_vis( ProjParType *proj )
{
  static int recnum = 0;
  DisplayBufParType *ds = &(proj->dispBuf);
  int bl, ch = ds->chan2blank;
  Cmplx3Type *V;
  RGParType *uvw = (RGParType*)proj->RecBuf;
  double t = (uvw->date2 * 86400 - IATUTC) / 60;  
  long offset;

  ds->xaxis[recnum] = (double)recnum;//t;
  offset = proj->baselines*proj->nrecs;

  for ( bl = 0 ; bl < proj->baselines ; bl++ )
  { V = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);
    if( V[ch].wt < 0.0) continue;
    ds->databuf[bl*proj->nrecs + recnum           ] = hypotf(V[ch].r, V[ch].i);
    ds->databuf[bl*proj->nrecs + recnum +   offset] = rad2deg(atan2f(V[ch].i, V[ch].r)); 
    ds->databuf[bl*proj->nrecs + recnum + 2*offset] = V[ch].r;
    ds->databuf[bl*proj->nrecs + recnum + 3*offset] = V[ch].i;
  }
  recnum++;
}

void writevis( ProjParType *proj, FILE *fout )
{
 static int recnum = 0;
 DisplayBufParType *ds = &(proj->dispBuf);
 int bl, ch = ds->chan2blank;
 Cmplx3Type *V;
 RGParType *uvw = (RGParType*)proj->RecBuf;
 double t = (uvw->date2 * 86400 - IATUTC) / 60;  
 long offset;
 
 ds->xaxis[recnum] = t;
 offset = proj->baselines*proj->nrecs;
 
 for ( bl = 0 ; bl < proj->baselines ; bl++ )
   fprintf(fout, "%f %f ", ds->databuf[bl*proj->nrecs + recnum + 2*offset], ds->databuf[bl*proj->nrecs + recnum + 3*offset] );
 fprintf(fout, "\n");

 recnum++;
}

void init_beamformer( ProjParType *proj, double tgt_ra, double tgt_dec )
{
  proj->beam.ptg_ra    = ra2sxg( &(proj->scan[0].ra) );
  proj->beam.ptg_dec   = dec2sxg( &(proj->scan[0].dec) );
  proj->beam.tgt_ra    = tgt_ra ;
  proj->beam.tgt_dec   = tgt_dec;
  proj->beam.data = (CmplxType*)calloc( proj->nrecs*proj->chans, sizeof(CmplxType) );
}

void beamform( ProjParType *proj )
{
 static int row = 0;
 double c = LIGHTVEL;
 double bl_length = 0.0;
 BeamParType *beam = &(proj->beam);
 StationParType *station = &(proj->station);
 double deltanu = proj->bandwidth / proj->chans ;
 double nu;
 double phase_correction;
 BaseParType *baseline = proj->Base;
 Cmplx3Type *vis;
 RGParType *uvw;
 CmplxType V, phasor;

 int bl, ch, i, j, n;

 for ( bl = 0 ; bl < proj->baselines ; bl++ )
 { uvw = (RGParType *) (proj->RecBuf + bl*(proj->recwords) );
   vis = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);

   bl_length = uvw->v * cos(deg2rad(beam->ptg_dec)) ;

   for(n = 0; n < proj->ants; n++)
   { if( baseline[bl].id1 == proj->Ant[n].id ) i = n;
     if( baseline[bl].id2 == proj->Ant[n].id ) j = n;
   }

   for( ch = 0 ; ch < proj->chans ; ch++ )
   { nu = proj->bandcentre - proj->bandwidth/2 + ch*deltanu ;
     phase_correction = 2 * M_PI * nu * bl_length * sin( deg2rad(beam->tgt_dec - beam->ptg_dec) );
     V.r = vis[ch].r; V.i = vis[ch].i;
     phasor = scale_z(1.0/proj->baselines, ret_z( cos(phase_correction), sin(phase_correction) ));
     beam->data[row*proj->chans + ch] = z_add( beam->data[row*proj->chans + ch], z_mul(V, phasor) );
   }
 }     

 row++;
}


void write_beam_to_file( FILE *fbeam, ProjParType *proj)
{ static int row = 0;
  int ch; 
  BeamParType *beam = &(proj->beam);
  for( ch = 0 ; ch < proj->chans ; ch++ ) 
  { fprintf(fbeam, "%f %f ", beam->data[row*proj->chans + ch].r, beam->data[row*proj->chans + ch].i);
  }
  fprintf(fbeam, "\n");
  row++;
}

double f( const gsl_vector *x, void *prj )
{
 ProjParType *proj = prj;
 SkyMapType *sky = &(proj->skymap);
 int i, j, n = proj->redundant_baselines, rbl;
 double sigma[proj->redundant_baselines];
 double l, m;
 double u = 0 , v = 0;
 CmplxType M,  phsr, V[proj->redundant_baselines];
 double ptg_ra, ptg_dec, ra_res, dec_res;
 int xx, yy;
 double S[sky->yside][sky->xside], B[sky->yside][sky->xside];
 BaseParType *baseline   = proj->Base;
 long *blindex = proj->blindex;
 double nu;
 double c = LIGHTVEL;
 double d= 30.0, b = 12.56968, lambda = 0.91884;
 double rabeam, decbeam;
 double xside, yside, x_res, y_res;
 double chisq = 0.0;
 double ra, dec, ra_0 = gsl_vector_get (x, 0), dec_0 = gsl_vector_get(x,1), A = 1.0;
 int bl;
 Cmplx3Type *vis;
 RGParType *uvw;
 PrimaryBeamParType *pb = &(proj->pbeam);
 int ch = proj->chans / 2;

 ra_res = sky->ra_res;
 dec_res = sky->dec_res;
 ptg_ra = deg2rad( ra2sxg(&(proj->scan[0].ra)) );
 ptg_dec = deg2rad( dec2sxg(&(proj->scan[0].dec)) );
 xside = pb->xside;
 yside = pb->yside;
 x_res = deg2rad(pb->ra_res);
 y_res = deg2rad(pb->dec_res);

 // Fill the RA range
 j = 0;
 for(i = -xside/2 ; i < xside/2 ; i++)
 { ra = rad2deg(ptg_ra + i*x_res);
   pb->ra[j++] = sxg2ra(ra);
 }

 // Fill the Dec range
 j = 0;
 for( i  = -yside/2 ; i < yside/2 ; i++ )
 { dec = rad2deg(ptg_dec + i*y_res);
   pb->dec[j++] = sxg2dec(dec);
 }


 for(i = proj->uvmin ; i < proj->redundant_baselines; i++)
 { nu = proj->bandcentre;
   v = baseline[i].v * nu/c;
   M = ret_z(0.0, 0.0);
   V[i] = proj->model_vis[ch][i];
   for(yy = 0; yy < sky->yside ; yy++)
   { //dec = dec_res  * (yy - sky->yside/2);
     //m = sin( deg2rad(dec));
       m = 0.1;
       dec = (deg2rad(dec2sxg(pb->dec+i)) - ptg_dec);
      decbeam = pow(sin(M_PI*b*dec/lambda)/(M_PI*b*dec/lambda), 2);
      if(dec == 0.0) decbeam = 1.0;
       for(xx=0; xx < sky->xside ; xx++)
       { ra = ra_res  * (xx - sky->xside/2);
	 l = sin( deg2rad(ra));
	 ra =  (deg2rad(ra2sxg(pb->ra+j)) - ptg_ra) ;
	 rabeam = pow(sin(M_PI*d*ra/lambda)/(M_PI*d*ra/lambda), 2);
	 if(ra == 0.0) rabeam = 1.0;
	 B[yy][xx] = rabeam*decbeam;
	 //B[yy][xx] = A * cos(M_PI * ra/ra_0)*cos(M_PI * ra/ra_0) * cos(M_PI * dec/dec_0)*cos(M_PI * dec/dec_0);
	 M.r += sky->map[ch][yy][xx] * B[yy][xx] * cos( 2*M_PI*m*v ); 
	 M.i += sky->map[ch][yy][xx] * B[yy][xx] * sin( 2*M_PI*m*v ); 
       }
     }
   //fprintf(finfo, "%s V[%d].r %f V[%d].i, %f M.r %f M.i %f\n", info, i+1, V[i].r, i+1, V[i].i, M.r, M.i);
   chisq += real(z_mul( z_sub(V[i], M), z_conj(z_sub(V[i], M)) ) );
   //chisq += (double)pow(z_abs(V[i]) - z_abs(M), 2);
 }
return chisq;
}

double ff( const gsl_vector *x, void *prj )
{
 ProjParType *proj = prj;
 int i, j, n;
 BaseParType *baseline   = proj->Base;
 long *blindex = proj->blindex;
 double nu;
 double c = LIGHTVEL;
 double chisq = 0.0;
 double alpha_real = gsl_vector_get (x, 0), alpha_imag = gsl_vector_get(x,1), A = 1.0;
 int bl;
 Cmplx3Type *vis;
 RGParType *uvw;
 int ch = proj->chans / 2;
 CmplxType alpha, gain1, gain2, gain;
 CmplxType model, V_s, V_ij;

 alpha = ret_z(alpha_real, alpha_imag);

 for ( bl = 0 ; bl < proj->baselines ; bl++ )
 { uvw = (RGParType *) (proj->RecBuf + bl*(proj->recwords) );
   vis = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);
   uvw->u        = baseline[bl].u / c;
   uvw->v        = baseline[bl].v / c;
   uvw->w        = baseline[bl].w / c;
   uvw->baseline = baseline[bl].number;

   for(n = 0; n < proj->ants; n++)
   { if( baseline[bl].id1 == proj->Ant[n].id ) i = n;
     if( baseline[bl].id2 == proj->Ant[n].id ) j = n;
   }

   if (i == proj->ants-1) gain1 = z_add(z_mul(alpha, proj->Ant[i-1].g), proj->Ant[i].g);
   else if (i == 0)       gain1 = z_add(z_mul(alpha, proj->Ant[i+1].g), proj->Ant[i].g);
   else                   gain1 = z_add( z_mul(alpha, z_add(proj->Ant[i-1].g, proj->Ant[i+1].g)), proj->Ant[i].g);
   
   if (j == proj->ants-1) gain2 = z_add(z_mul(alpha, proj->Ant[j-1].g), proj->Ant[j].g);
   else if (j == 0)       gain2 = z_add(z_mul(alpha, proj->Ant[j+1].g), proj->Ant[j].g);
   else                   gain2 = z_add( z_mul(alpha, z_add(proj->Ant[j-1].g, proj->Ant[j+1].g)), proj->Ant[j].g);

   gain = z_mul( gain1, z_conj(gain2));
   
   V_s = proj->model_vis[ch][ (blindex[ baseline[bl].id2 - baseline[bl].id1 ]) ];

   V_ij = ret_z(vis[ch].r, vis[ch].i);

   chisq += real(z_mul(z_sub(V_ij, z_mul(gain, V_s)), z_conj(z_sub(V_ij, z_mul(gain, V_s) ) ) ) );
 }

 return chisq;

}


int solve_prbeam_par_nelder_mead_simplex( ProjParType *proj )
{
 const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex;
 gsl_multimin_fminimizer *s = NULL;
 gsl_vector *ss, *x;
 gsl_multimin_function minex_func;
 double ra_0, dec_0, A = 1.0;
 int iter = 0;
 int status;
 double size;

 /* Starting point */
 x = gsl_vector_alloc(2);
 gsl_vector_set(x, 0, 1.5);
 gsl_vector_set(x, 1, 4.0);

 /* Set initial step sizes  */
 ss = gsl_vector_alloc (2);
 gsl_vector_set_all (ss, 0.05);

 /* Initialize method and iterate */
 minex_func.n = 2;
 minex_func.f = f;
 minex_func.params = (void*)proj;

 s = gsl_multimin_fminimizer_alloc(T,2);
 gsl_multimin_fminimizer_set(s, &minex_func, x, ss);

 fprintf(finfo,   "%s ============ Solving for primary beam shape ============", info);
 fprintf(finfo, "\n%s ============ Nelder-Mead Simplex Algorithm =============\n", info);
do
{ iter++;
  status = gsl_multimin_fminimizer_iterate(s);

  if (status) break;

  size = gsl_multimin_fminimizer_size(s);
  status = gsl_multimin_test_size(size, 1e-2);

  if (status == GSL_SUCCESS) fprintf(finfo, "%s Error energy converged to minimum at \n", info);

  ra_0 = gsl_vector_get(s->x,0);
  dec_0 = gsl_vector_get(s->x,1);

  fprintf(finfo, "%s It: %3d   ra_0= %10.3e  dec_0= %10.3e  f()= %7.3f area= %4.2e\n", 
	 info,
	 iter,
         ra_0,
         dec_0,
	 s->fval,
         ra_0*dec_0);
 }

 while (status == GSL_CONTINUE && iter < 100);

 gsl_vector_free(x);
 gsl_vector_free(ss);
 gsl_multimin_fminimizer_free(s);
 return status;

}


int solve_dipole_coupling_nelder_mead_simplex( ProjParType *proj )
{
 const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex;
 gsl_multimin_fminimizer *s = NULL;
 gsl_vector *ss, *x;
 gsl_multimin_function minex_func;
 double alpha_real, alpha_imag, A = 1.0;
 int iter = 0;
 int status;
 double size;

 /* Starting point */
 x = gsl_vector_alloc(2);
 gsl_vector_set(x, 0, 0.1);
 gsl_vector_set(x, 1, 0.1);

 /* Set initial step sizes  */
 ss = gsl_vector_alloc (2);
 gsl_vector_set_all (ss, 0.05);

 /* Initialize method and iterate */
 minex_func.n = 2;
 minex_func.f = ff;
 minex_func.params = (void*)proj;

 s = gsl_multimin_fminimizer_alloc(T,2);
 gsl_multimin_fminimizer_set(s, &minex_func, x, ss);

 fprintf(finfo,   "%s ============ Solving for primary beam shape ============", info);
 fprintf(finfo, "\n%s ============ Nelder-Mead Simplex Algorithm =============\n", info);
do
{ iter++;
  status = gsl_multimin_fminimizer_iterate(s);

  if (status) break;

  size = gsl_multimin_fminimizer_size(s);
  status = gsl_multimin_test_size(size, 1e-2);

  if (status == GSL_SUCCESS) fprintf(finfo, "%s Error energy converged to minimum at \n", info);

  alpha_real = gsl_vector_get(s->x,0);
  alpha_imag = gsl_vector_get(s->x,1);

  fprintf(finfo, "%s It: %3d   alpha.re_0= %10.3e  alpha.im= %10.3e  f()= %7.3e\n", 
	 info,
	 iter,
         alpha_real,
         alpha_imag,
	 s->fval);
 }

 while (status == GSL_CONTINUE && iter < 100);

 gsl_vector_free(x);
 gsl_vector_free(ss);
 gsl_multimin_fminimizer_free(s);
 return status;

}

void df( const gsl_vector *x, void *prj, gsl_vector *del_f )
{
 ProjParType *proj = prj;
 SkyMapType *sky = &(proj->skymap);
 int i, n = proj->redundant_baselines;
 double sigma[proj->redundant_baselines];
 double  l, m;
 double u = 0 , v = 0;
 CmplxType **V = proj->model_vis, phsr, conj_phsr;
 double ptg_ra, ptg_dec, ra_res, dec_res;
 int xx, yy;
 double S[sky->yside][sky->xside], B[sky->yside][sky->xside];
 BaseParType *baseline   = proj->Base;
 long *blindex = proj->blindex;
 double nu;
 double c = LIGHTVEL;
 double chisq = 0.0;
 double ra, dec, ra_0 = gsl_vector_get (x, 0), dec_0 = gsl_vector_get(x,1);
 double SB, SBr_inv, SBd_inv;
 double r_inv, d_inv;
 CmplxType z11, z21, z31, z41, z51, z61, gg1;
 CmplxType z12, z22, z32, z42, z52, z62, gg2;
 CmplxType z53, z63, gg3;
 double A = 1.0, A_inv = 1.0;
 int ch = proj->chans/2;

 ra_res = sky->ra_res;
 dec_res = sky->dec_res;


 gg1 = ret_z(0.0, 0.0); gg2 = ret_z(0.0, 0.0); gg3 = ret_z(0.0, 0.0);

 for( i = 0 ; i < proj->redundant_baselines ; i++ ) 
 { sigma[i] = 1.0;
   nu = proj->bandcentre;// - bandwidth/2 + ch*deltanu + deltanu/2;
   v = baseline[blindex[i+1]].v * nu / c ;
   
   z11 = ret_z(0.0, 0.0); z12 = ret_z(0.0, 0.0);
   z21 = ret_z(0.0, 0.0); z22 = ret_z(0.0, 0.0);
   z31 = ret_z(0.0, 0.0); z32 = ret_z(0.0, 0.0);
   z41 = ret_z(0.0, 0.0); z42 = ret_z(0.0, 0.0);
   z51 = ret_z(0.0, 0.0); z52 = ret_z(0.0, 0.0); z53 = ret_z(0.0, 0.0);
   z61 = ret_z(0.0, 0.0); z62 = ret_z(0.0, 0.0); z63 = ret_z(0.0, 0.0);
   

   for(yy = 0 ; yy < sky->yside ; yy++)
   { dec = dec_res  * (yy - sky->yside/2);
     //d_inv = 2.0 * pow(dec,2)/pow(dec_0,3);
     d_inv = 2.0*M_PI*dec / pow(dec_0,2) * tan( M_PI * dec/dec_0 );
     m = sin(deg2rad(dec));
     for(xx = 0 ; xx < sky->xside ; xx++)
     {     
      ra = ra_res  * (xx - sky->xside/2);
      //r_inv = 2.0 * pow(ra,2)/pow(ra_0,3);
      r_inv = 2.0*M_PI*ra / pow(ra_0,2) * tan( M_PI * ra/ra_0 );
      l = sin(deg2rad(ra));

      //B[xx][yy] =exp( -pow(ra/ra_0, 2) - pow(dec/dec_0, 2) );
      B[yy][xx] = A * cos(M_PI * ra/ra_0)*cos(M_PI * ra/ra_0) * cos(M_PI * dec/dec_0)*cos(M_PI * dec/dec_0);
      S[yy][xx] = sky->map[ch][yy][xx];
   
      SB =  S[yy][xx]*B[yy][xx];
      SBr_inv = SB*r_inv;
      SBd_inv = SB*d_inv;

      phsr = ret_z( cos( 2*M_PI*( l*u + m*v ) ), sin( 2*M_PI*( l*u + m*v ) ) );
      conj_phsr = z_conj(phsr);
      
      z11 = z_add( z11, scale_z( SB, phsr ) );
      z21 = z_add( z21, scale_z( SB, conj_phsr) );
      z31 = z_add( z31, scale_z( SBr_inv, phsr ) );
      z41 = z_add( z41, scale_z( SBr_inv, conj_phsr ) );
      
      z12 = z_add( z12, scale_z( SB, phsr ) );
      z22 = z_add( z22, scale_z( SB, conj_phsr) );
      z32 = z_add( z32, scale_z( SBd_inv, phsr ) );
      z42 = z_add( z42, scale_z( SBd_inv, conj_phsr ) );
     }       
   }
   z51 = z_add( z_mul(z_neg(V[ch][i]), z41), z_mul(z_neg(z_conj(V[ch][i])), z31));
   z61 = z_add( z_mul(z11, z41), z_mul(z21, z31));

   z52 = z_add( z_mul(z_neg(V[ch][i]), z42), z_mul(z_neg(z_conj(V[ch][i])), z32));
   z62 = z_add( z_mul(z12, z42), z_mul(z22, z32));
   /*
   z53 = z_add( z_mul(z_neg(scale_z(A_inv, V[ch][i])), z11), z_mul(z_neg(scale_z(A_inv, V[ch][i])), z21) );
   z63 = scale_z(2.0*A_inv, z_mul(z11, z21) ) ;
   */
   gg1 = z_add( gg1, z_add(z51, z61));
   gg2 = z_add( gg2, z_add(z52, z62));
   //gg2 = z_add( gg3, z_add(z53, z63));
 }

 //fprintf(finfo, "\n%s %f %f %f %f\n", info, gg1.r, gg1.i, gg2.r, gg2.i);

 gsl_vector_set(del_f, 0, real(gg1));
 gsl_vector_set(del_f, 1, real(gg2));
 //gsl_vector_set(del_f, 2, real(gg3));

}

void fdf( const gsl_vector *x, void *proj, double *ff, gsl_vector *del_f )
{
 *ff = f(x, proj);
 df(x, proj, del_f);
}

int solve_prbeam_par_conj_grad_fletcher_reeves( ProjParType *proj )
{
 const gsl_multimin_fdfminimizer_type *T = gsl_multimin_fdfminimizer_conjugate_fr;
 gsl_multimin_fdfminimizer *s = gsl_multimin_fdfminimizer_alloc(T,2);
 gsl_vector *x;
 gsl_multimin_function_fdf minex_func;
 double A = 1.0;
 int iter = 0;
 int status;
 double size;

 /* Starting point */
 x = gsl_vector_alloc(2);
 gsl_vector_set(x, 0, 1.5);
 gsl_vector_set(x, 1, 4.5);

 /* Initialize method and iterate */
 minex_func.n = 2;
 minex_func.f = (void*)&f;
 minex_func.df = (void*)&df;
 minex_func.fdf = (void*)&fdf;
 minex_func.params = (void*)proj;

 gsl_multimin_fdfminimizer_set(s, &minex_func, x, 0.01, 1e-4);

 fprintf(finfo,   "%s ============ Solving for primary beam shape ============", info);
 fprintf(finfo, "\n%s ===== Fletcher-Reeves Conjugate Gradient Algorithm =====\n", info);
do
{ iter++;

  status = gsl_multimin_fdfminimizer_iterate(s);

  if (status) break;

  status = gsl_multimin_test_gradient(s->gradient, 1e-3);

  if (status == GSL_SUCCESS) fprintf(finfo, "%s Error energy converged to minimum at \n", info);

  fprintf(finfo, "%s It: %3d   ra_0= %10.3e  dec_0= %10.3e  f()= %7.3e area= %4.2f\n", 
	 info,
	 iter,
	 gsl_vector_get(s->x, 0),
	 gsl_vector_get(s->x, 1), 
         s->f,
         gsl_vector_get(s->x, 0)*
	 gsl_vector_get(s->x, 1));
  
  gsl_multimin_fdfminimizer_restart(s);
 }

 while (status == GSL_CONTINUE && iter < 100);

 gsl_vector_free(x);
 gsl_multimin_fdfminimizer_free(s);
 return status;

}

int solve_prbeam_par_conj_grad_polak_ribiere( ProjParType *proj )
{
 const gsl_multimin_fdfminimizer_type *T = gsl_multimin_fdfminimizer_conjugate_pr;
 gsl_multimin_fdfminimizer *s = gsl_multimin_fdfminimizer_alloc(T,2);
 gsl_vector *x;
 gsl_multimin_function_fdf minex_func;
 double A = 1.0;
 int iter = 0;
 int status;
 double size;

 /* Starting point */
 x = gsl_vector_alloc(2);
 gsl_vector_set(x, 0, 1.5);
 gsl_vector_set(x, 1, 4.0);

 /* Initialize method and iterate */
 minex_func.n = 2;
 minex_func.f = (void*)&f;
 minex_func.df = (void*)&df;
 minex_func.fdf = (void*)&fdf;
 minex_func.params = (void*)proj;

 gsl_multimin_fdfminimizer_set(s, &minex_func, x, 0.01, 1e-4);

 fprintf(finfo,   "%s ============ Solving for primary beam shape ============", info);
 fprintf(finfo, "\n%s ====== Polak-Ribiere Conjugate Gradient Algorithm ======\n", info);
do
{ iter++;
  status = gsl_multimin_fdfminimizer_iterate(s);

  if (status) break;

  status = gsl_multimin_test_gradient(s->gradient, 1e-3);

  if (status == GSL_SUCCESS) fprintf(finfo, "%s Error energy converged to minimum at \n", info);

  fprintf(finfo, "%s It: %3d   ra_0= %10.3e  dec_0= %10.3e  f()= %7.3e area= %4.2f\n", 
	 info,
	 iter,
	 gsl_vector_get(s->x, 0),
	 gsl_vector_get(s->x, 1), 
	 s->f,
         gsl_vector_get(s->x, 0)*
         gsl_vector_get(s->x, 1) );

 }

 while (status == GSL_CONTINUE && iter < 100);

 gsl_vector_free(x);
 gsl_multimin_fdfminimizer_free(s);
 return status;

}

int solve_prbeam_par_vector_BFGS( ProjParType *proj )
{
 const gsl_multimin_fdfminimizer_type *T = gsl_multimin_fdfminimizer_vector_bfgs2;
 gsl_multimin_fdfminimizer *s = gsl_multimin_fdfminimizer_alloc(T,2);;
 gsl_vector *x;
 gsl_multimin_function_fdf minex_func;
 double A = 1.0;
 int iter = 0;
 int status;
 double size;

 /* Starting point */
 x = gsl_vector_alloc(2);
 gsl_vector_set(x, 0, 1.5);
 gsl_vector_set(x, 1, 4.0);

 /* Initialize method and iterate */
 minex_func.n = 2;
 minex_func.f = (void*)&f;
 minex_func.df = (void*)&df;
 minex_func.fdf = (void*)&fdf;
 minex_func.params = (void*)proj;

 gsl_multimin_fdfminimizer_set(s, &minex_func, x, 0.01, 1e-4);

 fprintf(finfo,   "%s ============ Solving for primary beam shape ============", info);
 fprintf(finfo, "\n%s ====== Broyden-Fletcher-Goldfarb-Shanno Algorithm ======\n", info);
do
{ iter++;
  status = gsl_multimin_fdfminimizer_iterate(s);

  if (status) break;

  status = gsl_multimin_test_gradient(s->gradient, 1e-3);

  if (status == GSL_SUCCESS) fprintf(finfo, "%s Error energy converged to minimum at \n", info);

  fprintf(finfo, "%s It: %3d   ra_0= %10.3e  dec_0= %10.3e f()= %7.3e area= %4.2f\n", 
	 info,
	 iter,
	 gsl_vector_get(s->x, 0),
	 gsl_vector_get(s->x, 1), 
	 s->f,
         gsl_vector_get(s->x, 0)*
         gsl_vector_get(s->x, 1));

 }

 while (status == GSL_CONTINUE && iter < 100);

 gsl_vector_free(x);
 gsl_multimin_fdfminimizer_free(s);
 return status;

}

int solve_prbeam_par_steepest_descent( ProjParType *proj )
{
 const gsl_multimin_fdfminimizer_type *T = gsl_multimin_fdfminimizer_steepest_descent;
 gsl_multimin_fdfminimizer *s = gsl_multimin_fdfminimizer_alloc(T,2);
 gsl_vector *x;
 gsl_multimin_function_fdf minex_func;
 double A = 1.0;
 int iter = 0;
 int status;
 double size;

 /* Starting point */
 x = gsl_vector_alloc(2);
 gsl_vector_set(x, 0, 1.5);
 gsl_vector_set(x, 1, 4.0);

 /* Initialize method and iterate */
 minex_func.n = 2;
 minex_func.f = (void*)&f;
 minex_func.df = (void*)&df;
 minex_func.fdf = (void*)&fdf;
 minex_func.params = (void*)proj;

 gsl_multimin_fdfminimizer_set(s, &minex_func, x, 1e-6, 5e-3);
 fprintf(finfo,   "%s ============ Solving for primary beam shape ============", info);
 fprintf(finfo, "\n%s ============== Steepest Descent Algorithm  =============\n", info);
do
{ iter++;
  status = gsl_multimin_fdfminimizer_iterate(s);

  if (status) break;

  status = gsl_multimin_test_gradient(s->gradient, 1e-6);

  if (status == GSL_SUCCESS) fprintf(finfo, "%s Error energy converged to minimum at \n", info);

  fprintf(finfo, "%s It: %3d   ra_0= %10.3e  dec_0= %10.3e  f()= %7.3e area= %4.2f\n", 
	 info,
	 iter,
	 gsl_vector_get(s->x, 0),
	 gsl_vector_get(s->x, 1), 
	 s->f,
         gsl_vector_get(s->x, 0)*
         gsl_vector_get(s->x, 1));

 }

 while (status == GSL_CONTINUE && iter < 100);

 gsl_vector_free(x);
 gsl_multimin_fdfminimizer_free(s);
 return status;

}
/*
double F( double *beampar, void *prj )
{
 ProjParType *proj = prj;
 SkyMapType *sky = &(proj->skymap);
 int i, j, n = proj->redundant_baselines, rbl;
 double sigma[proj->redundant_baselines];
 double l, m;
 double u = 0 , v = 0;
 CmplxType M,  phsr, V[proj->redundant_baselines];
 double ra_res, dec_res, x_res, y_res;
 double ptg_ra = 0.0, ptg_dec = 0.0;
 int xside, yside;
 double d= 30.0, b = 12.56968, lambda = 0.91884;
 double rabeam, decbeam;
 int xx, yy;
 double S[sky->yside][sky->xside], B[sky->yside][sky->xside];
 BaseParType *baseline   = proj->Base;
 int *blindex = proj->blindex;
 double nu;
 double c = LIGHTVEL;
 double chisq = 0.0;
 double ra, ra_0 = beampar[0], dec_0 = beampar[1], A = 1.0;
 double dec;
 int bl;
 //PrimaryBeamParType *pb = &(proj->pbeam);
 Cmplx3Type *vis;
 RGParType *uvw;
 int ch = proj->chans / 2;
 double dec_rad;

 ra_res = sky->ra_res;
 dec_res = sky->dec_res;
 xside = sky->xside;
 yside = sky->yside;

 for(i = proj->uvmin ; i < proj->redundant_baselines; i++)
 { nu = proj->bandcentre;
   v = baseline[i].v * nu/c;
   //M.r = 0.0; M.i = 0.0;
   M = ret_z(0.0, 0.0);
   V[i] = proj->model_vis[ch][i];
   for(yy = 0; yy < sky->yside ; yy++)
     { dec = dec_res  * (yy - sky->yside/2);
	 dec_rad = M_PI*dec/180.0;
	 //deg2rad(dec);
	 m = sin(dec_rad);
	 //m = 0.1;
	//m = sin( deg2rad(dec));
	//dec = (deg2rad(dec2sxg(sky->dec+yy)) - ptg_dec);
	decbeam = pow(sin(M_PI*b*dec/lambda)/(M_PI*b*dec/lambda), 2);
	//decbeam = 0.5;
       if(dec == 0.0) decbeam = 1.0;
       for(xx=0; xx < sky->xside ; xx++)
       { ra = ra_res  * (xx - sky->xside/2);
	   l = sin( deg2rad(ra));
	   //l = 0.2;
	 //ra =  (deg2rad(ra2sxg(sky->ra+xx)) - ptg_ra) ;
	 rabeam = 0.5;
	 //rabeam = pow(sin(M_PI*d*ra/lambda)/(M_PI*d*ra/lambda), 2);
	 rabeam = 0.5;
	 if(ra == 0.0) rabeam = 1.0;
	 B[yy][xx] = rabeam*decbeam;

	 M.r += 0.5 * sky->map[yy][xx] * B[yy][xx] * cos( 2*M_PI*m*v ); 
	 M.i += 0.5 * sky->map[yy][xx] * B[yy][xx] * sin( 2*M_PI*m*v ); 
       }
     }
   chisq += real(z_mul( z_sub(V[i], M), z_conj(z_sub(V[i], M)) ) );

 }
return chisq;
}

void solve_prbeam_par_Sweep_Zoom_Minimum_search( ProjParType *proj, double *radec )
{
 int bins = 7;
 double Fn;
 double beampar[2];
 int iter = 0;
 double eps = 1e-4;
 double error[2][bins];
 int i, j;
 FILE *ftmp = fopen("error.surface", "w");
 double ra_0, dec_0;
 double ra_last, dec_last;
 double ra[bins], dec[bins];
 double stride = 0.1;

 ra_0 = radec[0]; dec_0 = radec[1];
 ra_last = ra_0; dec_last = dec_0;

 fprintf(finfo,   "%s ============ Solving for primary beam shape ============", info);
 fprintf(finfo, "\n%s ========= Sweep-Zoom Minimum Search Algorithm  =========\n", info);

 beampar[0] = ra_last;
 beampar[1] = dec_last;

 fprintf(finfo, "%s It= %3d  ra_0= %7.5e  dec_0= %7.5e  F()= %7.5e\n", info, iter, ra_0, dec_0, F(beampar, proj) );

 do{
 beampar[1] = dec_last;
 for( i = 0 ; i < bins ; i++ )
 { ++iter;
   ra[i] = ra_last + stride*(i - bins/2);
   beampar[0] = ra[i];
   error[0][i] = (double)F( beampar, proj );
   fprintf(finfo, "%s It= %3d  ra_0= %7.5e  dec_0= %7.5e  F()= %7.5e\n", info, iter, ra[i], dec_last, error[0][i]);
 }

 ra_last = ra[ indexmin(error[0], bins) ];
 beampar[0] = ra_last;
 for( i = 0 ; i < bins ; i++ )
 { iter++;
   dec[i] = dec_last + stride*(i - bins/2);
   beampar[1] = dec[i];
   error[1][i] = (double)F( beampar, proj );
   fprintf(finfo, "%s It= %3d  ra_0= %7.5e  dec_0= %7.5e  F()= %7.5e\n", info, iter, ra_last, dec[i], error[1][i]);
 }
 dec_last = dec[ indexmin(error[1], bins) ];

 if( fabs(ra_0 - ra_last) > eps || fabs(dec_0 - dec_last) > eps )
 {
   stride *= 0.5;
   ra_0 = ra_last;
   dec_0 = dec_last;
 }
 else break;

}
while(stride > 5e-4);

 beampar[0] = ra_last;
 beampar[1] = dec_last;

 fprintf(finfo, "%s Error energy converged to minimum at \n", info);
 fprintf(finfo, "%s It= %3d  ra_0= %7.5e  dec_0= %7.5e  F()= %7.5e\n", info, iter, ra_last, dec_last, F(beampar, proj) );

 radec[0] = ra_last;
 radec[1] = dec_last;
}*/
/*
void solve_prbeam_paraboloid( ProjParType *proj )
{
 int bins = 392;
 double Fn;
 double beampar[2];
 int iter = 0;
 double eps = 1e-4;
 double error[bins][bins];
 int i, j;
 FILE *ftmp = fopen("error.surface", "w");
 double ra_0, dec_0;
 double ra_last, dec_last;
 double ra[bins], dec[bins];
 double stride = 0.1;
 SkyMapType *sky = &(proj->skymap);

 fprintf(finfo, "%s ========= Constructing error paraboloid  =========\n", info);

 for( i = 0 ; i < bins ; i++ )
 {ra[i] = 0.83 + i*0.005;
  beampar[0] = ra[i];
  for( j = 0 ; j < bins ; j++ )
  { error[i][j] = 0.0; 
    dec[j] = 3.58 + j*0.005;
    beampar[1] = dec[j];
    error[i][j] = (double)F(beampar, proj);
    fprintf(ftmp, "%f ", error[i][j]);
    fprintf(stderr, "%s ra[%d] = %lf dec[%d] = %lf error = %f\n", info, i+1, ra[i], j+1, dec[j], error[i][j]);
  }
  fprintf(ftmp, "\n");

 }

 for( i = 0 ; i < bins ; i++ )
 { for( j = 0 ; j < bins ; j++ )
   sky->map[j][i] = error[i][j];
 }
 write_sky2FITS( proj, "ERROR.FITS" );

}
*/


double F( double *beampar, void *prj )
{
 ProjParType *proj = prj;
 SkyMapType *sky = &(proj->skymap);
 int i, n = proj->redundant_baselines, rbl;
 double sigma[proj->redundant_baselines];
 double l, m;
 double u = 0 , v = 0;
 CmplxType M,  phsr, V[proj->redundant_baselines];
 double ptg_ra, ptg_dec, ra_res, dec_res;
 int xx, yy;
 double S[sky->yside][sky->xside], B[sky->yside][sky->xside];
 BaseParType *baseline   = proj->Base;
 long *blindex = proj->blindex;
 double nu;
 double c = LIGHTVEL;
 double chisq = 0.0;
 double ra, dec, ra_0 = beampar[0], dec_0 = beampar[1], A = 1.0;
 int bl;
 Cmplx3Type *vis;
 RGParType *uvw;
 int ch = proj->chans / 2;

 ra_res = sky->ra_res;
 dec_res = sky->dec_res;
 ptg_dec = 0.0;
 ptg_ra = 0.0;

 for(i = proj->uvmin ; i < proj->redundant_baselines; i++)
 { nu = proj->bandcentre;
   v = baseline[i].v * nu/c;
   M.r = 0.0;
   M.i = 0.0;
   //M = ret_z(0.0, 0.0);
   V[i] = proj->model_vis[ch][i];
   for(yy = 0; yy < sky->yside ; yy++)
     { dec = dec_res  * (yy - sky->yside/2);
       m = sin( deg2rad(dec));
       for(xx=0; xx < sky->xside ; xx++)
       { ra = ra_res  * (xx - sky->xside/2);
	 l = sin( deg2rad(ra));
	 B[yy][xx] = A * cos(M_PI * ra/ra_0)*cos(M_PI * ra/ra_0) * cos(M_PI * dec/dec_0)*cos(M_PI * dec/dec_0);
	 M.r += 0.5 * sky->map[ch][yy][xx] * B[yy][xx] * cos( 2*M_PI*m*v ); 
	 M.i += 0.5 * sky->map[ch][yy][xx] * B[yy][xx] * sin( 2*M_PI*m*v ); 
       }
     }
   //fprintf(finfo, "%s V[%d].r %f V[%d].i, %f M.r %f M.i %f\n", info, i+1, V[i].r, i+1, V[i].i, M.r, M.i);
   chisq += real(z_mul( z_sub(V[i], M), z_conj(z_sub(V[i], M)) ) );
   //chisq += (double)pow(z_abs(V[i]) - z_abs(M), 2);
 }
return chisq;
}

void solve_prbeam_par_Sweep_Zoom_Minimum_search( ProjParType *proj, double ra_init, double dec_init )
{
 int bins = 7;
 double Fn;
 double beampar[2];
 int iter = 0;
 double eps = 1e-4;
 double error[2][bins];
 int i, j;
 FILE *ftmp = fopen("error.surface", "w");
 double ra_0, dec_0;
 double ra_last, dec_last;
 double ra[bins], dec[bins];
 double stride = 0.1;

 ra_0 = ra_init; dec_0 = dec_init;
 ra_last = ra_0; dec_last = dec_0;

 fprintf(finfo,   "%s ============ Solving for primary beam shape ============", info);
 fprintf(finfo, "\n%s ========= Sweep-Zoom Minimum Search Algorithm  =========\n", info);

 beampar[0] = ra_last;
 beampar[1] = dec_last;

 fprintf(finfo, "%s It= %3d  ra_0= %7.5e  dec_0= %7.5e  F()= %7.5e\n", info, iter, ra_0, dec_0, F(beampar, proj) );

 do{
 beampar[1] = dec_last;
 for( i = 0 ; i < bins ; i++ )
 { ++iter;
   ra[i] = ra_last + stride*(i - bins/2);
   beampar[0] = ra[i];
   error[0][i] = (double)F( beampar, proj );
   fprintf(finfo, "%s It= %3d  ra_0= %7.5e  dec_0= %7.5e  F()= %7.5e\n", info, iter, ra[i], dec_last, error[0][i]);
 }

 ra_last = ra[ indexmin(error[0], bins) ];
 beampar[0] = ra_last;
 for( i = 0 ; i < bins ; i++ )
 { iter++;
   dec[i] = dec_last + stride*(i - bins/2);
   beampar[1] = dec[i];
   error[1][i] = (double)F( beampar, proj );
   fprintf(finfo, "%s It= %3d  ra_0= %7.5e  dec_0= %7.5e  F()= %7.5e\n", info, iter, ra_last, dec[i], error[1][i]);
 }
 dec_last = dec[ indexmin(error[1], bins) ];

 if( fabs(ra_0 - ra_last) > eps || fabs(dec_0 - dec_last) > eps )
 {
   stride *= 0.5;
   ra_0 = ra_last;
   dec_0 = dec_last;
 }
 else break;

 }
 while(stride > 5e-4);

 beampar[0] = ra_last;
 beampar[1] = dec_last;

 fprintf(finfo, "%s Error energy converged to minimum at \n", info);
 fprintf(finfo, "%s It= %3d  ra_0= %7.5e  dec_0= %7.5e  F()= %7.5e\n", info, iter, ra_last, dec_last, F(beampar, proj) );

}

void init_V2corr( ProjParType *proj )
{
 int bl, ch;

 proj->V_sum = (CmplxType**)calloc(proj->redundant_baselines+1, sizeof(CmplxType*));
 proj->V_sum_rec = (CmplxType**)calloc(proj->redundant_baselines+1, sizeof(CmplxType*));
 proj->V_ofst = (double**)calloc(proj->redundant_baselines+1, sizeof(double*));
 proj->V_count = (int**)calloc(proj->redundant_baselines+1, sizeof(int*) );
 proj->V2 = (double***)calloc(proj->redundant_baselines, sizeof(double**) );
 proj->V2_rec = (double***)calloc(proj->redundant_baselines, sizeof(double**) );
 proj->V2_err = (double***)calloc(proj->redundant_baselines, sizeof(double**) );
 proj->V2_dnu = (double*)calloc(proj->redundant_baselines*proj->chans, sizeof(double) );
 for( bl = 0 ; bl < proj->redundant_baselines; bl++ ) 
 { proj->V2[bl] = (double**)calloc(proj->chans, sizeof(double*) );
   proj->V2_rec[bl] = (double**)calloc(proj->chans, sizeof(double*) );
   proj->V2_err[bl] = (double**)calloc(proj->chans, sizeof(double*) );
   for( ch = 0 ; ch < proj->chans ; ch++ )
   { proj->V2[bl][ch] = (double*)calloc(proj->chans, sizeof(double) ); 
     proj->V2_rec[bl][ch] = (double*)calloc(proj->chans, sizeof(double) ); 
     proj->V2_err[bl][ch] = (double*)calloc(proj->chans, sizeof(double) ); 
   }
 }

 for( bl = 0 ; bl < proj->redundant_baselines+1 ; bl++ )
     { proj->V_sum[bl] = (CmplxType*)calloc(proj->chans, sizeof(CmplxType) );
       proj->V_sum_rec[bl] = (CmplxType*)calloc(proj->chans, sizeof(CmplxType) );
       proj->V_ofst[bl] = (double*)calloc(proj->chans, sizeof(double) );
       proj->V_count[bl] = (int*)calloc(proj->chans, sizeof(int) );
     }

 proj->V2_init_done = 1;

}


void prep_V2( ProjParType *proj )
{
 int bl, ch, ch1, ch2;
 Cmplx3Type *vis;
 long *idx = proj->blindex;
 BaseParType *baseline = proj->Base;
 double signv;
 CmplxType V;
 static long int count = 0;

 for ( bl = 0 ; bl < proj->baselines ; bl++ )
 { vis = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);
   for( ch = 0; ch < proj->chans ; ch++ )
   {
     if( vis[ch].wt < 0.0 ) continue;

     V.r = vis[ch].r; V.i = vis[ch].i;
     //fprintf(finfo, "%s B : %d C : %d V.r : %f V.i : %f\n", info, bl, ch, V.r, V.i);
     
     signv = baseline[bl].v < 0.0 ? -1.0 : 1.0 ;
     //signv = 1.0;

     proj->V_sum[ idx[ baseline[bl].id2 - baseline[bl].id1 ] ][ ch ].r += V.r;
     proj->V_sum[ idx[ baseline[bl].id2 - baseline[bl].id1 ] ][ ch ].i += signv * V.i;

     proj->V_ofst[idx[ baseline[bl].id2 - baseline[bl].id1 ] ][ ch ] += V.r*V.r + V.i*V.i;
     
     proj->V_count[ idx[ baseline[bl].id2 - baseline[bl].id1 ] ][ ch ] += 1;
   }
 }
 fprintf(finfo, "%s Processed %4ld /  %4ld records\r", info, ++count, proj->nrecs );
}

void cleanup_V_sum_rec( ProjParType *proj )
{
 int bl, ch;

 for(bl = 0 ; bl < proj->redundant_baselines+1; bl++)
  for(ch = 0 ; ch < proj->chans ; ch++ )
      proj->V_sum_rec[bl][ch] = ret_z(0.0, 0.0);
}


void prep_V2_errbar( ProjParType *proj )
{

 int bl, ch, ch1, ch2;
 Cmplx3Type *vis;
 long *idx = proj->blindex;
 BaseParType *baseline = proj->Base;
 double signv;
 CmplxType V;

 for ( bl = 0 ; bl < proj->baselines ; bl++ )
 { vis = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);
   for( ch = 0; ch < proj->chans ; ch++ )
   {
     if( vis[ch].wt < 0.0 ) continue;

     V.r = vis[ch].r; V.i = vis[ch].i;
     
     signv = baseline[bl].v < 0.0 ? -1.0 : 1.0 ;

     proj->V_sum_rec[ idx[ baseline[bl].id2 - baseline[bl].id1 ] ][ ch ].r += V.r;
     proj->V_sum_rec[ idx[ baseline[bl].id2 - baseline[bl].id1 ] ][ ch ].i += signv * V.i;
   }
 }

}


void corrl_V2_rec( ProjParType *proj )
{
  CmplxType **V = proj->V_sum_rec;
  int **N = proj->V_count;
  double ***V2 = proj->V2;
  double ***V2_err = proj->V2_err;
  double ***V2_rec = proj->V2_rec;
  double adj_contrib = 0.0;
  int bl, ch1, ch2;
  double dd;
  static int count = 0;

  for( bl = 0 ; bl < proj->redundant_baselines ; bl++ )
  { for(ch1 = 0 ; ch1 < proj->chans ; ch1++ )
    { for( ch2 = 0 ; ch2 < proj->chans ; ch2++ )
      { /*if( ch1 == ch2 ) V2_rec[bl][ch1][ch2] = ( V[bl][ch1].r * V[bl][ch2].r + V[bl][ch1].i * V[bl][ch2].i  - proj->V_ofst[bl][ch1]) / (N[bl][ch1]*(N[bl][ch2]-1) ) ;
	  else if (ch1 != ch2)*/
          V2_rec[bl][ch1][ch2] = ( V[bl][ch1].r * V[bl][ch2].r + V[bl][ch1].i * V[bl][ch2].i ) / ( N[bl][ch1]*N[bl][ch2] );
	  if ( N[bl][ch1] == 0 || N[bl][ch2] == 0 ) V2_rec[bl][ch1][ch2] = 0.0;
	  if(proj->nrecs == 1) V2[bl][ch1][ch2] = 0.0;
	  V2_err[bl][ch1][ch2] += pow((V2_rec[bl][ch1][ch2] - V2[bl][ch1][ch2]), 2);
	  /*V2_err[bl][ch1][ch2] = (V2_rec[bl][ch1][ch2] - V2[bl][ch1][ch2]);
	  fprintf(finfo, "%s bl : %d ch1 : %d ch2 : %d V2 : %5.2e V2_rec : %5.2e V2_err : %5.2e \n", info, bl, ch1, ch2, V2[bl][ch1][ch2], V2_rec[bl][ch1][ch2], V2_err[bl][ch1][ch2]);
	  V2_err[bl][ch1][ch2] += pow((V2_rec[bl][ch1][ch2] - V2[bl][ch1][ch2]), 2);*/
      }
    }

  }
 fprintf(finfo, "%s Processed %d /  %d records\r", info, ++count, proj->nrecs );
}

void estimate_V2_err( ProjParType *proj )
{
  double ***V2_err = proj->V2_err;
  int bl, ch1, ch2;

  for( bl=0; bl < proj->redundant_baselines; bl++ ){
      for(ch1 = 0 ; ch1 < proj->chans ; ch1++){
	  for(ch2 = 0 ; ch2 < proj->chans ; ch2++){
          V2_err[bl][ch1][ch2] = sqrt(V2_err[bl][ch1][ch2])/proj->nrecs;
	  }}}
}

 void corrl_V2( ProjParType *proj )
 {
  CmplxType **V = proj->V_sum;
  int **N = proj->V_count;
  double ***V2 = proj->V2;
  FILE *fpspec = fopen("pspec.dat", "w");
  double mean[proj->redundant_baselines+1], var[proj->redundant_baselines+1];
                                                double adj_contrib = 0.0;
  int bl, ch1, ch2;
  double dd;

  for( bl = 0 ; bl < proj->redundant_baselines ; bl++ )
  { mean[bl] = 0.0; var[bl] = 0.0;
    for(ch1 = 0 ; ch1 < proj->chans ; ch1++ )
    { for( ch2 = 0 ; ch2 < proj->chans ; ch2++ )
      { /*if( ch1 == ch2 ) V2[bl][ch1][ch2] = ( V[bl][ch1].r * V[bl][ch2].r + V[bl][ch1].i * V[bl][ch2].i  - proj->V_ofst[bl][ch1]) / (N[bl][ch1]*(N[bl][ch2]-1) ) ;
	  else if (ch1 != ch2)*/
          V2[bl][ch1][ch2] = ( V[bl][ch1].r * V[bl][ch2].r + V[bl][ch1].i * V[bl][ch2].i ) / ( N[bl][ch1]*N[bl][ch2] );
	  if ( N[bl][ch1] == 0 || N[bl][ch2] == 0 ) V2[bl][ch1][ch2] = 0.0;

	  /*     
	adj_contrib = 0.0;  
        if( bl == 0 ) 
	{ if( N[bl][ch1] == 0 || N[bl+1][ch2] == 0 ) adj_contrib = 0.0;
	  else adj_contrib = ( V[bl][ch1].r * V[bl+1][ch2].r + V[bl][ch1].i * V[bl+1][ch2].i )/(N[bl][ch1] * N[bl+1][ch2]);
	  V2[bl][ch1][ch2] += adj_contrib;
	}
	else if( bl == proj->redundant_baselines ) 
        { if( N[bl][ch1] == 0 || N[bl-1][ch2] == 0 ) adj_contrib = 0.0;
	  else adj_contrib = ( V[bl][ch1].r * V[bl-1][ch2].r + V[bl][ch1].i * V[bl-1][ch2].i )/(N[bl][ch1] * N[bl-1][ch2]);
	  V2[bl][ch1][ch2] += adj_contrib;
	}
	else 
	{ if( N[bl][ch1] == 0 ) 
	  adj_contrib = 0.0;
          else if ( N[bl-1][ch2] == 0 && N[bl+1][ch2] > 0 ) 
          adj_contrib = ( V[bl][ch1].r * V[bl+1][ch2].r + V[bl][ch1].i * V[bl+1][ch2].i )/(N[bl][ch1] * N[bl+1][ch2]);
	  else if ( N[bl+1][ch2] == 0 && N[bl-1][ch2] > 0 ) 
	  adj_contrib = ( V[bl][ch1].r * V[bl-1][ch2].r + V[bl][ch1].i * V[bl-1][ch2].i )/(N[bl][ch1] * N[bl-1][ch2]);
	  else adj_contrib = ( V[bl][ch1].r * V[bl+1][ch2].r + V[bl][ch1].i * V[bl+1][ch2].i )/(N[bl][ch1] * N[bl+1][ch2]) + \
		             ( V[bl][ch1].r * V[bl-1][ch2].r + V[bl][ch1].i * V[bl-1][ch2].i )/(N[bl][ch1] * N[bl-1][ch2]);
          V2[bl][ch1][ch2] += adj_contrib;
	}
	  */	  
	//fprintf(finfo, "%d  %f\n", bl, adj_contrib);

	
	if(ch1 == ch2)
	    { mean[bl] += V2[bl][ch1][ch2] / proj->chans;
              var[bl] += pow(V2[bl][ch1][ch2],2.0) /  proj->chans;
	}
      }
    }
    //getchar();
    var[bl] = var[bl] - mean[bl]*mean[bl];
    fprintf(fpspec, "%f %f", (bl+1)*12.5, 16798*pow((bl+1)*12.5, -2.34));
    fprintf(fpspec, " %f %f %f\n", V2[bl][proj->chans / 2][proj->chans / 2], mean[bl], sqrt(var[bl]) );
  }
  fclose(fpspec); 
 }

	/*
void init_V2corr( ProjParType *proj )
{
 int bl, ch;

 proj->V_sum = (CmplxType**)calloc(proj->redundant_baselines+1, sizeof(CmplxType*));
 proj->V_count = (int**)calloc(proj->redundant_baselines+1, sizeof(int*) );
 proj->V2 = (double***)calloc(proj->redundant_baselines, sizeof(double**) );
 proj->V2_dnu = (double*)calloc(proj->redundant_baselines*proj->chans, sizeof(double) );
 for( bl = 0 ; bl < proj->redundant_baselines; bl++ ) 
 { proj->V2[bl] = (double**)calloc(proj->chans, sizeof(double*) );
   for( ch = 0 ; ch < proj->chans ; ch++ )
    proj->V2[bl][ch] = (double*)calloc(proj->chans, sizeof(double) ); 
 }

 for( bl = 0 ; bl < proj->redundant_baselines+1 ; bl++ )
     { proj->V_sum[bl] = (CmplxType*)calloc(proj->chans, sizeof(CmplxType) );
       proj->V_count[bl] = (int*)calloc(proj->chans, sizeof(int) );
     }

 proj->V2_init_done = 1;

}


void prep_V2( ProjParType *proj )
{
 int bl, ch, ch1, ch2;
 Cmplx3Type *vis;
 int *idx = proj->blindex;
 BaseParType *baseline = proj->Base;
 double signv;

 for ( bl = 0 ; bl < proj->baselines ; bl++ )
 { vis = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);
   for( ch = 0; ch < proj->chans ; ch++ )
   {
     if( vis[ch].wt < 0.0 ) continue;

     signv = baseline[bl].v < 0.0 ? -1.0 : 1.0 ;

     proj->V_sum[ idx[ baseline[bl].id2 - baseline[bl].id1 ] ][ ch ].r += vis[ch].r;
     proj->V_sum[ idx[ baseline[bl].id2 - baseline[bl].id1 ] ][ ch ].i += signv * vis[ch].i;

     proj->V_count[ idx[ baseline[bl].id2 - baseline[bl].id1 ] ][ ch ]++;
   }
 }

}

 void corrl_V2( ProjParType *proj )
 {
  CmplxType **V = proj->V_sum;
  int **N = proj->V_count;
  double ***V2 = proj->V2;
  FILE *fpspec = fopen("pspec.dat", "w");
  double mean[proj->redundant_baselines+1], var[proj->redundant_baselines+1];
  double adj_contrib = 0.0;

  int bl, ch1, ch2;
  for( bl = 0 ; bl < proj->redundant_baselines ; bl++ )
  { mean[bl] = 0.0; var[bl] = 0.0;
    for(ch1 = 0 ; ch1 < proj->chans ; ch1++ )
    { for( ch2 = 0 ; ch2 < proj->chans ; ch2++ )
      { if ( N[bl][ch1] == 0 || N[bl][ch2] == 0 ) V2[bl][ch1][ch2] = 0.0;
	else V2[bl][ch1][ch2]= ( V[bl][ch1].r * V[bl][ch2].r + V[bl][ch1].i * V[bl][ch2].i )/(N[bl][ch1] * N[bl][ch2]);
	  	  
	adj_contrib = 0.0;  
        if( bl == 0 ) 
	{ if( N[bl][ch1] == 0 || N[bl+1][ch2] == 0 ) adj_contrib = 0.0;
	  else adj_contrib = ( V[bl][ch1].r * V[bl+1][ch2].r + V[bl][ch1].i * V[bl+1][ch2].i )/(N[bl][ch1] * N[bl+1][ch2]);
	  V2[bl][ch1][ch2] += adj_contrib;
	}
	else if( bl == proj->redundant_baselines ) 
        { if( N[bl][ch1] == 0 || N[bl-1][ch2] == 0 ) adj_contrib = 0.0;
	  else adj_contrib = ( V[bl][ch1].r * V[bl-1][ch2].r + V[bl][ch1].i * V[bl-1][ch2].i )/(N[bl][ch1] * N[bl-1][ch2]);
	  V2[bl][ch1][ch2] += adj_contrib;
	}
	else 
	{ if( N[bl][ch1] == 0 ) 
	  adj_contrib = 0;
          else if ( N[bl-1][ch2] == 0 && N[bl+1][ch2] > 0 ) 
          adj_contrib = ( V[bl][ch1].r * V[bl+1][ch2].r + V[bl][ch1].i * V[bl+1][ch2].i )/(N[bl][ch1] * N[bl+1][ch2]);
	  else if ( N[bl+1][ch2] == 0 && N[bl-1][ch2] > 0 ) 
	  adj_contrib = ( V[bl][ch1].r * V[bl-1][ch2].r + V[bl][ch1].i * V[bl-1][ch2].i )/(N[bl][ch1] * N[bl-1][ch2]);
	  else adj_contrib = ( V[bl][ch1].r * V[bl+1][ch2].r + V[bl][ch1].i * V[bl+1][ch2].i )/(N[bl][ch1] * N[bl+1][ch2]) + \
		             ( V[bl][ch1].r * V[bl-1][ch2].r + V[bl][ch1].i * V[bl-1][ch2].i )/(N[bl][ch1] * N[bl-1][ch2]);
          V2[bl][ch1][ch2] += adj_contrib;
	}
	//fprintf(finfo, "%d  %f\n", bl, adj_contrib);
	
	if(ch1 == ch2)
	    { mean[bl] += V2[bl][ch1][ch2] / proj->chans;
              var[bl] += pow(V2[bl][ch1][ch2],2.0) /  proj->chans;
	}
      }
    }
    var[bl] = var[bl] - mean[bl]*mean[bl];
    fprintf(fpspec, "%f %f", (bl+1)*12.5, 16798*pow((bl+1)*12.5, -2.34));
    fprintf(fpspec, " %f %f %f\n", V2[bl][proj->chans / 2][proj->chans / 2], mean[bl], sqrt(var[bl]) );
  }
  fclose(fpspec); 
}
*/
void V2_deltanu_from_cube( ProjParType *proj )
{ SkyMapType *sky = &(proj->skymap);
  int U = sky->zside, channels = sky->xside;
  double deltanu = sky->ra_res;
  int *count;
  int u, ch1, ch2, deltach;
  FILE *fV2 = fopen("V2.delta", "w");

  count = (int*)calloc(channels, sizeof(int));

  for(ch1 = 0 ; ch1 < channels ; ch1++ )
  for(ch2 = 0 ; ch2 < channels ; ch2++ ) count[abs(ch1-ch2)]++;
  

  proj->V2_dnu = (double*)calloc(U*channels, sizeof(double));
  for(u = 0 ; u < U ; u++)
  { for(ch1 = 0 ; ch1 < channels ; ch1++ )
    { for(ch2 = 0 ; ch2 < channels ; ch2++ )
      proj->V2_dnu[u*channels + abs(ch1-ch2)] += sky->map[u][ch1][ch2]/count[abs(ch1-ch2)];
    }
    
  }

  for(deltach = 0 ; deltach < channels ; deltach++ )
  { fprintf(fV2, "%f ", deltach*deltanu/1e6);
    for(u = 0 ; u < U ; u++ ) fprintf(fV2, "%f ", proj->V2_dnu[u*channels + deltach]);
    fprintf(fV2, "\n");
  }

  fclose(fV2);
  free(count);
}

void free_V2( ProjParType *proj )
{
    free(proj->V2);
    free(proj->V2_rec);
    free(proj->V2_err);
    free(proj->V2_dnu);
    free(proj->V_count);
    free(proj->V_sum);
    free(proj->V_sum_rec);
}



void V2_deltanu_from_v2corr( ProjParType *proj )
{ int U = proj->redundant_baselines;
  int channels = proj->chans;
  double *V2_deltanu = proj->V2_dnu;
  double ***V2 = proj->V2;
  double deltanu = proj->bandwidth / proj->chans;
  int *count;
  int u, ch1, ch2, deltach;
  FILE *fV2 = fopen("V2.delta", "w");
  double V2_0[U];

  count = (int*)calloc(channels, sizeof(int));

  for(ch1 = 0 ; ch1 < channels ; ch1++ )
  for(ch2 = 0 ; ch2 < channels ; ch2++ ) count[abs(ch1-ch2)]++;
  
  for(u = 0 ; u < U ; u++)
  { for(ch1 = 0 ; ch1 < channels ; ch1++ )
    { for(ch2 = 0 ; ch2 < channels ; ch2++ )
	    V2_deltanu[u*channels + abs(ch1-ch2)] += V2[u][ch1][ch2] / (double) count[abs(ch1-ch2)];
    }
      V2_0[u] = arraymax(V2_deltanu + u*channels, channels);
  }

  for(deltach = 0 ; deltach < channels ; deltach++ )
  { fprintf(fV2, "%e ", deltach*deltanu/1e6);
      for(u = 0 ; u < U ; u++ )
      {  V2_deltanu[u*channels + deltach];
	 fprintf(fV2, "%e ", V2_deltanu[u*channels + deltach]);
	 //fprintf(stderr, "U : %d dnu : %d V2 : %e \n", u, deltach, V2_deltanu[u*channels + deltach]);
      }
      //getchar();
    fprintf(fV2, "\n");
  }

  fclose(fV2);

  free(count);
}
 
void init_flagmask( ProjParType *proj )
{

 int ant, rec;

 proj->flagmask = (int***)calloc(proj->nrecs, sizeof(int**) );
 for(rec = 0; rec < proj->nrecs; rec++)
 { proj->flagmask[rec] = (int**)calloc(proj->ants, sizeof(int*) );
   for( ant = 0 ; ant < proj->ants ; ant++ )
   proj->flagmask[rec][ant] = (int*)calloc(proj->ants, sizeof(int) );
 }
 proj->ant2flag = (int*)calloc(proj->ants, sizeof(int) );

}

void make_flagmask( fitsfile *fvis, ProjParType *proj )
{
 int rec = 0;
 RGParType *uvw;
 Cmplx3Type *V;
 BaseParType *baseline = proj->Base;
 int ant, bl, ch, n, i, j;
 long int cnt = 0;
 unsigned long int data;

 fits_rewind( fvis, proj );
 while( rec < proj->nrecs )
 { readFITS2Vis( fvis, proj );
   for ( bl = 0 ; bl < proj->baselines ; bl++ )
   { uvw = (RGParType *) (proj->RecBuf + bl*proj->recwords );
     V = (Cmplx3Type *)  (proj->RecBuf + bl*proj->recwords + proj->pcount );
     //fprintf(finfo, "%s Baseline %d : id1 : %d id2 : %d\n", info, bl, baseline[bl].id1, baseline[bl].id2);
     for(ch = 0 ; ch < proj->chans ; ch++ )
     { if( V[ch].wt < 0.0 )  
       { proj->flagmask[rec][baseline[bl].id1][baseline[bl].id2]++;
	 proj->flagmask[rec][baseline[bl].id2][baseline[bl].id1]++;
	 cnt++;
       }
     }
   }
   rec++;
   fprintf(finfo, "%s Making flag information mask: %4d/%4d recs done\r", info, rec, proj->nrecs);
 }
 fits_rewind( fvis, proj );
 fprintf(finfo, "\n%s =========================================\n",info);
 write_fgmask2FITS( proj );
 data = proj->nrecs*proj->baselines*proj->chans;
 fprintf(finfo, "%s [ %ld / %ld ] visibilities flagged\n", info, cnt, data );
 fprintf(finfo, "%s ========> %4.1f %c data flagged <========\n", info, 100.0*(double)cnt/(double)data, '%');
 fprintf(finfo, "%s =========================================\n",info);
}

void check_chan2cal( fitsfile *fvis, ProjParType *proj )
{
 RGParType *uvw;
 Cmplx3Type *V;
 BaseParType *baseline = proj->Base;
 int bl, ant, rec=0, a1, a2;
 long int cnt = 0, data;
 int ***ch0mask;
 int flagged;

 ch0mask = (int***)calloc(proj->nrecs, sizeof(int**) );
 for( rec = 0 ; rec < proj->nrecs ; rec++ )
 { ch0mask[rec] = (int**)calloc(proj->ants, sizeof(int*) );
   for( ant = 0 ; ant < proj->ants ; ant++ ) ch0mask[rec][ant] = (int*)calloc(proj->ants, sizeof(int) );
 }

 fprintf(finfo, "%s Checking 'Channel 0' sanity ...\n", info);
 fits_rewind( fvis, proj );
 rec = 0;
 while( rec < proj->nrecs )
 { readFITS2Vis( fvis, proj );
   for ( bl = 0 ; bl < proj->baselines ; bl++ )
   { uvw = (RGParType *) (proj->RecBuf + bl*(proj->recwords) );
     V = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);
     ch0mask[rec][baseline[bl].id1][baseline[bl].id2] = 0;
     if( V[proj->chan2cal].wt < 0.0 )
     { ch0mask[rec][baseline[bl].id1][baseline[bl].id2] = 1;
       ch0mask[rec][baseline[bl].id2][baseline[bl].id1] = 1;
       cnt++;
     }
   }

   fprintf(finfo, "%s Rec : %4d Channel %d Antennas flagged : ", info, rec+1, proj->chan2cal);
   for(a1 = 0 ; a1 < proj->ants ; a1++ )
   { flagged = 0;
     for( a2 = 0 ; a2 < proj->ants ; a2++ ) flagged += ch0mask[rec][a1][a2];
     if( flagged == proj->ants - 1)
     { proj->ant2flag[a1] = 1;
       fprintf(finfo, "%s ", proj->Ant[a1].name);
     }
   }
   rec++;
   fprintf(stderr, "\r");
 }
 fprintf(stderr,"\n");
}

int update_scaninfo( ProjParType *proj )
{
 static int scan = 1, sid = 1, rpt = 0;
 int j; 
 int ret;

 proj->scans = scan;
 strcpy( proj->scan[scan].src, proj->scan[0].src);
 proj->scan[scan].scan_num = scan;
 proj->scan[scan].ra    = proj->scan[0].ra;
 proj->scan[scan].dec   = proj->scan[0].dec;
 proj->scan[scan].sid   = sid - rpt;
 proj->scan[scan].epoch = proj->scan[0].epoch;
 proj->scan[scan].recs  = proj->nrecs;

 for(j = 1 ; j < scan; j++)
    { if( (strcmp(proj->scan[scan].src, proj->scan[j].src)) == 0) 
     { proj->scan[scan].sid = proj->scan[j].sid;
       rpt += 1;
     }
    }
 fprintf(finfo, "%s Scan : %3d Records : %4d Source : %s Source ID : %d \n", info, proj->scan[scan].scan_num, proj->scan[scan].recs, proj->scan[scan].src, proj->scan[scan].sid);

 ++sid;
 ++scan;

 return  scan-1;
}


void update_sid( ProjParType *proj, int scan )
{
 int bl;
 RGParType *uvw;

 for ( bl = 0 ; bl < proj->baselines ; bl++ )
 { uvw = (RGParType *) (proj->RecBuf + bl*(proj->recwords) );
   uvw->su       = (double)proj->scan[scan].sid;
 }

}



void show_scaninfo( fitsfile *fvis, ProjParType *proj )
{
 RGParType *uvw;
 int i;
 int recs;
 int scanrecs[MAX_SCANS] = {0};
 int sid = 0;
 int scan = 0;
 char src[MAX_SCANS][10];
 int suid[MAX_SCANS];

 fits_rewind( fvis, proj );
 readFITS2Vis( fvis, proj );
 uvw = (RGParType *) proj->RecBuf;
 suid[scan] = (int)uvw->su;
 scanrecs[scan] = 1;

 for(recs = 1 ; recs < proj->nrecs ; recs++ )
 { readFITS2Vis( fvis, proj );
   uvw = (RGParType *) proj->RecBuf;
   for( sid = 0 ; sid < proj->scans ; sid++ )
   { if ( (int)uvw->su == proj->scan[sid].sid )
      strcpy( src[scan], proj->scan[sid].src);
     if( (int)uvw->su != suid[scan] ) 
      suid[++scan] = (int)uvw->su;
   }   
   ++scanrecs[scan];
 }
   
 for( i = 0 ; i <= scan ; i++ )
     fprintf(finfo, "%s      Scan %2d   Recs : %4d   Source : %s\n", info, i+1, scanrecs[i], proj->scan[suid[i]-1].src);
 fits_rewind( fvis, proj );
 }
/*
void flagit( ProjParType *proj )
{
 RGParType *uvw;
 Cmplx3Type *vis;
 FlagParType fgpar = proj->flagpar;
 BaseParType *baseline = proj->Base;
 int bl, ch, i, j, n;

 /*
 if( fgpar.a1 == -1 && fgpar.a2 == -1 ) break;
 if( fgpar.b1 == -1 && fgpar.b2 == -1) break;
 if( fgpar.c1 == -1 && fgpar.c2 == -1) break;
 if( fgpar.d1 <= proj->recnow || fgpar.d2 >= proj->recnow || (fgpar.d1 == -1 && fgpar.d2 == -1)) break;
 */
 //fprintf(finfo, "%s Antenna to flag : %5s\n", info, proj->Ant[fgpar.a1].name);
//if(fgpar.a1 > -1 && fgpar.a2 == -1) fgpar.a2 = fgpar.a1;
 //fprintf(finfo, "%s Antenna to flag : %5s\n", info, proj->Ant[fgpar.a2].name);
 //getchar();
 /*if(fgpar.b1 > -1 && fgpar.b2 == -1) fgpar.b2 = fgpar.b1;
   if(fgpar.c1 > -1 && fgpar.c2 == -1) fgpar.c2 = fgpar.c1;*/
/*
 for( bl = 0 ; bl < proj->baselines ; bl++ )
 { //if(bl >= fgpar.b1 && bl <= fgpar.b2) continue;
   uvw = (RGParType*) (proj->RecBuf + bl*proj->recwords);
   vis = (Cmplx3Type*)(uvw + proj->pcount);
   for(n = 0; n < proj->ants; n++)
   { if( baseline[bl].id1 == proj->Ant[n].id ) i = n;
     if( baseline[bl].id2 == proj->Ant[n].id ) j = n;
   }
   if( proj->recnow == 1) 
     fprintf(finfo, "\n%s Baseline %3d : %4s-%4s flagged", info, bl+1, proj->Ant[i].name, proj->Ant[j].name);
   //if(( i >= fgpar.a1 && i <= fgpar.a2) ||( j >= fgpar.a1 && j <= fgpar.a2) )
   if( fgpar.a1 == i || fgpar.a1 == j)
   { 
     for( ch = 0 ; ch < proj->chans ; ch++ ) vis[ch].wt = -1.0;  
   }
 }
 
}
*/


void flagit( ProjParType *proj )
{
 RGParType *uvw;
 Cmplx3Type *vis;
 FlagParType fgpar = proj->flagpar;
 BaseParType *baseline = proj->Base;
 int bl, ch, i, j, n;

 if(fgpar.a1 > -1 && fgpar.a2 == -1) fgpar.a2 = fgpar.a1;
 for( bl = 0 ; bl < proj->baselines ; bl++ )
 { uvw = (RGParType*) (proj->RecBuf + bl*proj->recwords);
   vis = (Cmplx3Type*)(proj->RecBuf + bl*proj->recwords + proj->pcount);
   for(n = 0; n < proj->ants; n++)
   { if( baseline[bl].id1 == proj->Ant[n].id ) i = n;
     if( baseline[bl].id2 == proj->Ant[n].id ) j = n;
   }
   if( fgpar.a1 == i || fgpar.a1 == j)
   { if( proj->recnow == 1) 
     fprintf(finfo, "\n%s Baseline %3d : %4s-%4s flagged", info, bl+1, proj->Ant[i].name, proj->Ant[j].name); 
     for( ch = 0 ; ch < proj->chans ; ch++ ) vis[ch].wt = -1.0;  
   }
 }
 
}


void submodel( ProjParType *inprj, ProjParType *mdlprj)
{
 int bl, ch;
 Cmplx3Type *vis;
 CmplxType mdl;
 BaseParType *baseline = inprj->Base;
 long *blindex = inprj->blindex;
 int idx;

 for( bl = 0 ; bl < inprj->baselines ; bl++ )
 { vis = (Cmplx3Type *) (inprj->RecBuf + bl*(inprj->recwords) + inprj->pcount);
   idx = blindex[ baseline[bl].id2 - baseline[bl].id1 ];

   for( ch = 0 ; ch < inprj->chans ; ch++ )
   { mdl.r = mdlprj->model_vis[ch][idx].r;
     mdl.i = mdlprj->model_vis[ch][idx].i;

     vis[ch].r -= mdl.r;
     vis[ch].i -= mdl.i;
   }
 }

}

void addmodel( ProjParType *proj1, ProjParType *proj2)
{
 int bl, ch;

 for( bl = 0 ; bl < proj1->redundant_baselines ; bl++ )
 { for( ch = 0 ; ch < proj1->chans ; ch++ )
   { proj1->model_vis[ch][bl].r += proj2->model_vis[ch][bl].r;
     proj1->model_vis[ch][bl].i += proj2->model_vis[ch][bl].i;
   }
 }

}

 void getmodel( ProjParType *proj )
 {
 int bl, ch;
 int copies;
 Cmplx3Type *vis;
 BaseParType *baseline = proj->Base;
 long *blindex = proj->blindex;
 int idx;
 double nu, v, c = LIGHTVEL;
 double deltanu = proj->bandwidth/proj->chans;

  for( bl = 0 ; bl < proj->baselines ; bl++ )
  { vis = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);
    idx = blindex[ baseline[bl].id2 - baseline[bl].id1 ];
    copies = proj->unique_baseline[ baseline[bl].id2 - baseline[bl].id1 ];
    fprintf(finfo, "%s BL : %d copies : %d\n", info, baseline[bl].id2-baseline[bl].id1, copies);
    for( ch = 0 ; ch < proj->chans ; ch++ )
    { nu = proj->bandcentre - proj->bandwidth/2 + ch*deltanu + deltanu/2;
      v = baseline[idx].v * nu / c ;
      proj->ULambda[ch][idx] = sqrt(pow(v,2));
      proj->model_vis[ch][idx].r += vis[ch].r / copies;
      proj->model_vis[ch][idx].i += vis[ch].i / copies;
    }
  }

  for( bl = 0 ; bl < proj->baselines ; bl++ )
  { vis = (Cmplx3Type *) (proj->RecBuf + bl*(proj->recwords) + proj->pcount);
    idx = blindex[ baseline[bl].id2 - baseline[bl].id1 ];
    copies = proj->unique_baseline[ baseline[bl].id2 - baseline[bl].id1 ];
    for( ch = 0 ; ch < proj->chans ; ch++ )
    { if(copies==1)
      { proj->MV_errbar[ch][idx].r += proj->model_vis[ch][idx].r;
	proj->MV_errbar[ch][idx].i += proj->model_vis[ch][idx].i;
      }
      else
      {
	proj->MV_errbar[ch][idx].r += pow((vis[ch].r - proj->model_vis[ch][idx].r),2);
	proj->MV_errbar[ch][idx].i += pow((vis[ch].i - proj->model_vis[ch][idx].i),2);
      }
    }
  }


  for( bl = 0 ; bl < proj->redundant_baselines+1 ; bl++ )
  { 
    copies = proj->unique_baseline[ baseline[bl].id2 - baseline[bl].id1 ];
    for(ch = 0 ; ch < proj->chans ; ch++ )
    { proj->MV_errbar[ch][bl].r = sqrt(proj->MV_errbar[ch][bl].r/copies);
      proj->MV_errbar[ch][bl].i = sqrt(proj->MV_errbar[ch][bl].i/copies);
    }
  }

 }

void subrec( ProjParType *inprj, ProjParType *mdlprj)
{
 int bl, ch;
 Cmplx3Type *vis;
 Cmplx3Type *mdl;
 BaseParType *baseline = inprj->Base;
 long *blindex = inprj->blindex;
 int idx;

 for( bl = 0 ; bl < inprj->baselines ; bl++ )
 { vis = (Cmplx3Type *) (inprj->RecBuf + bl*(inprj->recwords) + inprj->pcount);
   mdl = (Cmplx3Type *) (mdlprj->RecBuf + bl*(mdlprj->recwords) + mdlprj->pcount);
   idx = blindex[ baseline[bl].id2 - baseline[bl].id1 ];

   for( ch = 0 ; ch < inprj->chans ; ch++ )
   { vis[ch].r -= mdl[inprj->chans/2].r;
     vis[ch].i -= mdl[inprj->chans/2].i;
   }
 }
}

