#ifndef MATDEFS_H
#define MATDEFS_H

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

#include <math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

typedef struct
{
 double r;
 double i;
}__attribute__((packed)) CmplxType;

typedef struct
{
 double r;
 double i;
 double wt;
}__attribute__((packed)) Cmplx3Type;

typedef struct
{
 double abs;
 double arg;
}__attribute__((packed)) PolarType;

int min ( int , int );
int max ( int , int );

double minf( double, double );
double maxf( double, double );
double arraymax( double *, int );
double arraymin( double *, int );
float arraymaxf( float *, int );
float arrayminf( float *, int );
void proper_angle( double * );
void accumulate( double *, double *, int ) ;
void scale( double *, double *, int, int ) ;
void Cmplx_to_Polar( CmplxType *, PolarType * );
CmplxType Polar_to_Cmplx( PolarType );
void ComplexCartesian_to_GSLPolar( CmplxType *, gsl_vector *, gsl_vector * ) ;
void GSL_Ln( gsl_vector * ) ;
void unwrap_phase( double *, int );
void realft(float *, unsigned long, int );
void four1(float *, unsigned long, int );
unsigned long int seed(void);

double uniform_noise( void ) ;
int signum( double );
CmplxType ret_z(double, double) ;
CmplxType scale_z(double, CmplxType) ;
CmplxType z_mul(CmplxType, CmplxType) ;
CmplxType z_div(CmplxType, CmplxType) ;
CmplxType z_add(CmplxType, CmplxType) ;
CmplxType z_sub(CmplxType, CmplxType) ;
CmplxType z_conj(CmplxType) ;
CmplxType z_neg(CmplxType) ;
double z_abs(CmplxType) ;
double z_arg(CmplxType) ;
double real(CmplxType) ;
double imag(CmplxType) ;
double deg2rad( double );
double rad2deg( double );
double mjdnow(void);
char* mjd2iau_date(double) ;
unsigned long gen_seed( void );
double jd2gst(double );  // gst in degrees corresponding to jd

#endif
