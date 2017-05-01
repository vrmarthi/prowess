#include <sys/time.h>
#include "../inc/matdefs.h"
#include "../inc/sysdefs.h"

int min( int m, int n )
{ int result = 0;
  if( m < n ) result = m;
  else if ( n < m ) result = n;
  else if( m == n ) result = m;
  return result;
}

int max( int m, int n )
{ int result = 0;
  if( m < n ) result = n;
  else if ( n < m ) result = m;
  else if( m == n ) result = n;
  return result;
}

double minf( double m, double n )
{ double result = 0;
  if( m < n ) result = m;
  else if ( n < m ) result = n;
  else if( m == n ) result = m;
  return result;
}

double maxf( double m, double n )
{ double result = 0;
  if( m < n ) result = n;
  else if ( n < m ) result = m;
  else if( m == n ) result = n;
  return result;
}

double arraymax( double *arr, int len)
{
 int i;
 double max = arr[0];

 for ( i = 1 ; i < len ; i++ )
     { if (arr[i] > max) max = arr[i];}

 return max;
}


double arraymin( double *arr, int len)
{
 int i;
 double min = arr[0];

 for ( i = 1 ; i < len ; i++ )
     {     if (arr[i] < min) min = arr[i];}

 return min;
}

float arraymaxf( float *arr, int len)
{
 long i;
 float max = arr[0];

 for ( i = 1 ; i < len ; i++ )
     { if (arr[i] > max) max = arr[i];}

 return max;
}


float arrayminf( float *arr, int len)
{
 int i;
 float min = arr[0];

 for ( i = 1 ; i < len ; i++ )
     {     if (arr[i] < min) min = arr[i];}

 return min;
}

int indexmin( double *arr, int len)
{
 int i;
 double min = arr[0];
 int index = 0;

 for ( i = 1 ; i < len ; i++ )
     { if (arr[i] < min) 
       { min = arr[i];
         index = i;
       }
     }
 return index;
}

void accumulate(double *accBuf, double *inBuf, int len)
{ int i;
  for(i = 0; i < len; i++) accBuf[i] += inBuf[i];
}

void scale( double *inBuf, double *outBuf, int N, int len)
{ int i;
 for(i = 0; i < len; i++) 
 {
     outBuf[i] = inBuf[i] / N ;
 }
}

void proper_angle( double *arg )
{ if( *arg <= -180.0 ) *arg += 360.0;
  if( *arg >=  180.0 ) *arg -= 360.0;
}


double uniform_noise(void)
{
 return  ( (double)rand() / (double)RAND_MAX  - 0.5 );
}

int signum( double num )
{
 if (num > 0) 
      return 1;
 else if (num < 0) 
      return -1;
 else return 0;
}

void realft(float data[], unsigned long n, int isign)
{
 void four1(float data[], unsigned long nn, int isign);
 unsigned long i,i1,i2,i3,i4,np3;
 float c1=0.5,c2,h1r,h1i,h2r,h2i;
 double wr,wi,wpr,wpi,wtemp,theta;
 
 theta=3.141592653589793/(double) (n>>1);
 if (isign == 1) {
     c2 = -0.5;
     four1(data,n>>1,1);
 } else {
     c2=0.5;
     theta = -theta;
 }
 wtemp=sin(0.5*theta);
 wpr = -2.0*wtemp*wtemp;
 wpi=sin(theta);
 wr=1.0+wpr;
 wi=wpi;
 np3=n+3;
 for (i=2;i<=(n>>2);i++) {
     i4=1+(i3=np3-(i2=1+(i1=i+i-1)));
     h1r=c1*(data[i1]+data[i3]);
     h1i=c1*(data[i2]-data[i4]);
     h2r = -c2*(data[i2]+data[i4]);
     h2i=c2*(data[i1]-data[i3]);
     data[i1]=h1r+wr*h2r-wi*h2i;
     data[i2]=h1i+wr*h2i+wi*h2r;
     data[i3]=h1r-wr*h2r+wi*h2i;
     data[i4] = -h1i+wr*h2i+wi*h2r;
     wr=(wtemp=wr)*wpr-wi*wpi+wr;
     wi=wi*wpr+wtemp*wpi+wi;
 }
 if (isign == 1) {
     data[1] = (h1r=data[1])+data[2];
     data[2] = h1r-data[2];
 } else {
     data[1]=c1*((h1r=data[1])+data[2]);
     data[2]=c1*(h1r-data[2]);
     four1(data,n>>1,-1);
 }
}

void blackman_nuttall( double *w, int N )
{
  double a_0, a_1, a_2, a_3;
  int n;

  a_0 = 0.3635819;
  a_1 = 0.4891775;
  a_2 = 0.1365995;
  a_3 = 0.0106411;

  for(n = 0 ; n < N ; n++ )
  w[n] = a_0 - a_1*cos(2*M_PI*n/(N-1)) + a_2*cos(4*M_PI*n/(N-1)) - a_3*cos(6*M_PI*n/(N-1));

}


void four1(float data[], unsigned long nn, int isign)
{
 unsigned long n,mmax,m,j,istep,i;
 double wtemp,wr,wpr,wpi,wi,theta;
 float tempr,tempi;
 
 n=nn << 1;
 j=1;
 for (i=1;i<n;i+=2) {
     if (j > i) {
	 SWAP(data[j],data[i]);
	 SWAP(data[j+1],data[i+1]);
     }
     m=nn;
     while (m >= 2 && j > m) {
	 j -= m;
	 m >>= 1;
     }
     j += m;
 }
 mmax=2;
 while (n > mmax) {
     istep=mmax << 1;
     theta=isign*(6.28318530717959/mmax);
     wtemp=sin(0.5*theta);
     wpr = -2.0*wtemp*wtemp;
     wpi=sin(theta);
     wr=1.0;
     wi=0.0;
     for (m=1;m<mmax;m+=2) {
	 for (i=m;i<=n;i+=istep) {
	     j=i+mmax;
	     tempr=wr*data[j]-wi*data[j+1];
	     tempi=wr*data[j+1]+wi*data[j];
	     data[j]=data[i]-tempr;
	     data[j+1]=data[i+1]-tempi;
	     data[i] += tempr;
	     data[i+1] += tempi;
	 }
	 wr=(wtemp=wr)*wpr-wi*wpi+wr;
	 wi=wi*wpr+wtemp*wpi+wi;
     }
     mmax=istep;
 }
}
#undef SWAP


char *mjd2iau_date( double mjd )
{
  double J = mjd + 2400000.5 ;  /*  is this correct ?  crs/june 2000 */
  static char date[32] ;
  int month, day;
  long year, a, c, d, x, y, jd;
  double dd;
  
  if( J < 1721425.5 ) return 0 ; /* January 1.0, 1 A.D.  dont accept BC ! */
  
  jd = J + 0.5; /* round Julian date up to integer */
  
  /* Find the number of Gregorian centuries
   * since March 1, 4801 B.C.
   */
  a = (100*jd + 3204500L)/3652425L;
  
  /* Transform to Julian calendar by adding in Gregorian century years
   * that are not leap years.
   * Subtract 97 days to shift origin of JD to March 1.
   * Add 122 days for magic arithmetic algorithm.
   * Add four years to ensure the first leap year is detected.
   */
  c = jd + 1486;
  if( jd >= 2299160.5 )
      c += a - a/4;
  else
      c += 38;
  /* Offset 122 days, which is where the magic arithmetic
   * month formula sequence starts (March 1 = 4 * 30.6 = 122.4).
   */
  d = (100*c - 12210L)/36525L;
  x = (36525L * d)/100L; /* Days in that many whole Julian years */
  
  /* Find month and day. */
  y = ((c-x)*100L)/3061L;
  day = c - x - ((306L*y)/10L);
  month = y - 1;
  if( y > 13 ) month -= 12;
  
  /* Get the year right. */
  year = d - 4715;
  if( month > 2 ) year -= 1;
  
  a = (jd + 1) % 7; /* Day of the week. */
  
  dd = day + J - jd + 0.5; /* Fractional part of day. */
  day = dd;
  { int h,m,s ;
      s = (dd-day)*24*3600 ;
      h = s/3600 ;
      m = (s-h*3600)/60 ;
      s -= (h*3600 + m*60) ;
      sprintf(date,"%04ld-%02d-%02dT%02d:%02d:%02d / UT",year,month,day,h,m,s) ;
  }
  return date ;
}

double jd2gst(double jd)  // gst in degrees corresponding to jd
{
  double c0,c1,c2,c3;
  double temp,t;
  c0=24110.54841;
  c1=8640184.812866;
  c2=0.093104;
  c3=-6.2E-6;
  t=(jd-2451545.0)/36525;
  temp=((c0+c1*t+c2*t*t+c3*t*t*t))/240.0;
  return (temp);
}

unsigned long int seed(void)
{
 struct timeval t;
 gettimeofday(&t, NULL);            /* Unix time*/
 return (unsigned long int)(t.tv_sec + t.tv_usec);
}

unsigned long gen_seed( void )
{
 unsigned long sd;
 struct timeval t;

 gettimeofday( &t, NULL );
 sd = t.tv_sec + t.tv_usec;

 return sd;
}
double mjdnow(void)
{
 double sec;
 struct timeval t;
 double jd, mjd;

 gettimeofday(&t, NULL);            /* Unix time*/
 sec = t.tv_sec + t.tv_usec*(1e-6); /* Seconds + miroseconds */
 jd = sec/86400.0 + 2440587.5;      /* UNIX-time Day to JD */
 mjd = jd - 2400000.5;              /* JD to MJD */

 return mjd;
}

CmplxType ret_z(double re, double im)
{
 CmplxType z;
 z.r = re;
 z.i = im;
 return z;
}

CmplxType z_mul(CmplxType z1, CmplxType z2)
{
 CmplxType z;
 z.r = z1.r * z2.r - z1.i * z2.i;
 z.i = z1.r * z2.i + z2.r * z1.i;
 return z;
}


CmplxType z_div( CmplxType z1, CmplxType z2)
{
 CmplxType temp;
 double den;
 temp = z_mul( z1, z_conj(z2) );
 den = real( z_mul( z2, z_conj(z2) ) );
 return scale_z( 1.0/den, temp );
}

CmplxType z_add(CmplxType z1, CmplxType z2)
{
 CmplxType z;
 z.r = z1.r + z2.r;
 z.i = z1.i + z2.i;
 return z;
}

CmplxType z_sub(CmplxType z1, CmplxType z2)
{
 CmplxType z;
 z.r = z1.r - z2.r;
 z.i = z1.i - z2.i;
 return z;
}

CmplxType z_conj(CmplxType z1)
{
 CmplxType z;
 z.r = z1.r;
 z.i = -z1.i;
 return z;
}

CmplxType z_neg(CmplxType z1)
{
 CmplxType z;
 z.r = -z1.r;
 z.i = -z1.i;
 return z;
}

double ddelta(int a, int b)
{
 double retval = 1.0;
 if(a != b) retval = 0.0;
 else retval = 1.0;
 return retval;
}

double z_abs(CmplxType z)
{
 return sqrt ( z.r * z.r + z.i * z.i );
}

double z_arg(CmplxType z)
{
 return atan2f ( z.i, z.r );
}

CmplxType scale_z(double a, CmplxType z1)
{
 CmplxType z;
 z.r = z1.r*a;
 z.i = z1.i*a;
 return z;
}

double real(CmplxType z)
{
 return z.r;
}

double imag(CmplxType z)
{
 return z.i;
}

CmplxType Polar_to_Cmplx( PolarType p )
{
  CmplxType c;
  c.r = p.abs * cos( p.arg );
  c.i = p.abs * sin( p.arg );
  return c;
}

void ComplexCartesian_to_GSLPolar(CmplxType *rd_vis, gsl_vector *vmag, gsl_vector *vphi)
{
 int i, n = (int)vmag->size;
 for(i = 0; i < n; i++)
 { vmag->data[i] = hypot( rd_vis[i].r, rd_vis[i].i );
   vphi->data[i] = atanf( rd_vis[i].i / rd_vis[i].r );
 }
}

void GSL_Ln(gsl_vector *vmag )
{
  int i, n = vmag->size;
  for(i = 0; i < n; i++)  vmag->data[i] = logf(vmag->data[i]);
}

double deg2rad( double deg )
{
 return (deg*M_PI/180);
}

double rad2deg( double rad )
{
 return (rad*180.0/M_PI);
}

void unwrap_phase( double *phase, int N )
{ int i;
  double d_ph;
  
  for( i = 1 ; i < N ; i++ )
  { d_ph = phase[i] - phase[i-1] ;
    if( (d_ph > 170.0) && (d_ph < 190.0) ) 
    phase[i] -= 180.0; 
    else if( (d_ph < -170.0) && (d_ph > -190) ) 
    phase[i] += 180.0;
  }
}

PolarType compute_errorbar( CmplxType par, CmplxType err )
{
 double abs, arg;
 double d_abs, d_arg;
 PolarType errorbar;

 abs = z_abs(par);
 arg = z_arg(par);

 d_abs = ( 1.0 / abs ) * sqrt( (par.r * err.r)*(par.r * err.r) + (par.i * err.i)*(par.i * err.i) );
 d_arg = ( 1.0 / arg ) * sqrt( (par.i * err.r)*(par.i * err.r) + (err.i/(par.r*par.r))*(err.i/(par.r*par.r)) );

 errorbar.abs = d_abs;
 errorbar.arg = rad2deg(d_arg);

 proper_angle( &(errorbar.arg) );

 return errorbar;
}
