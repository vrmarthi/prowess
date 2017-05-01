#ifndef SKYDEFS_H
#define SKYDEFS_H

#define BEAMWIDTH_RA  1.755
#define BEAMWIDTH_DEC 4.1886

//#define CELLSIZE 1.3075e-4 // Original in the simulation
//#define CELLSIZE 6.0e-4 // for wide-field images
//#define CELLSIZE 1.6069e-4 // matched to HI N-body output
//#define CELLSIZE 4e-5
//#define CELLSIZE 3.22e-4
#define CELLSIZE 4.20e-4
//#define MAPSIDE 2048
#define MAPSIDE 1024
//#define MAPSIDE 8192
#define STERAD 5000
#define T_SKY 50
#define k_B 1.38e3 /* in Jy/K*/
#define A_DEFINED_NU 5.13e-4


#define ALPHA 2.52
#define BETA  2.34

typedef struct 
{
 double h,m,s;
} RAType;

typedef struct
{
 double d,m,s;
} DecType;

typedef struct
{
 char *filename;
 int fullsize;
 double angres;
 double ***map;
 int pbside;
 int zside;
 double ra_res;
 double dec_res;
 double ***pbmap;
} GalFGType;

typedef struct
{
 char *filename;
 int fullsize;
 double angres;
 double ***Qmap;
 double ***Umap;
 int pbside;
 int zside;
 double ra_res;
 double dec_res;
 double ***pbmap;
} PolGalFGType;

typedef struct
{
 char filename[256];
 int xside;
 int yside;
 int zside;
 double ra_res;
 double dec_res;
 RAType *ra;
 DecType *dec;
 double ***map;
} HIMapType;

typedef struct
{
 char filename[256];
 int xside;
 int yside;
 int zside;
 double ra_res;
 double dec_res;
 RAType *ra;
 DecType *dec;
 double ***map;
} EPSFGType;

typedef struct
{
 int fullsize;
 double angres;
 double **map;
 int pbside;
 double ra_res;
 double dec_res;
 double **pbmap;
} TECScrType;

typedef struct
{
 RAType ra;
 DecType dec;
 double m;
 double S;
 double alpha; // S_nu = ( nu_c / nu ) ^ alpha // Added on 12-Jan-15 - VRM
}SkySrcType;

typedef struct
{
 double nu;
 double U;
 double defined_nu;
 double A_defined_nu;
 double A;
 double nu_0;
 double alpha;
 double beta;
}PowerSpecParType;

double ra2sxg ( RAType * );
double dec2sxg ( DecType * );
RAType sxg2ra ( double );
DecType sxg2dec ( double );
int  find_bin( double, double * );
double genSrc( int, double *, double * );
double  P_I( double, double, double, double);
double D_C( double, double );

int  within_beam ( RAType, DecType, RAType, DecType );
void assign ( SkySrcType *, RAType, DecType );
void associate ( SkySrcType *, RAType, DecType, int );

#endif
