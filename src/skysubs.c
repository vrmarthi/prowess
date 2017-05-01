#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <fftw3.h>
#include "../inc/skydefs.h"
#include "../inc/sysdefs.h"
#include "../inc/matdefs.h"
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_permutation.h>


double ra2sxg ( RAType *ra )
{
 double sxgra = 15*( (ra->h) + (ra->m)/60 + (ra->s)/3600 );
 return sxgra;
}

double dec2sxg ( DecType *dec )
{
 double sxgdec = ( (dec->d) + (dec->m)/60 + (dec->s)/3600 );
 return sxgdec;
}

RAType sxg2ra ( double sxg )
{
 RAType ra;
 ra.h = floor(sxg / 15);
 ra.m = floor( ((sxg / 15 ) - ra.h)*60 );
 ra.s = floor( ((sxg/15) - (ra.h + ra.m/60))*3600 );
 return ra;
}

DecType sxg2dec ( double sxg )
{
 DecType dec;
 dec.d = floor( sxg );
 dec.m = floor( ( (sxg) - dec.d ) * 60 );
 dec.s = floor( ( (sxg) - (dec.d + dec.m/60) )*3600 );
 return dec;
}

int within_beam(RAType pra, DecType pdec, RAType ra, DecType dec)
{
 double pntgra, srcra, pntgdec, srcdec;
 double ra_dist, dec_dist;
 double BW_RA = BEAMWIDTH_RA * 3600;
 double BW_DEC = BEAMWIDTH_DEC * 3600;

 pntgra = 15.0*pra.h*3600.0 + 15.0*pra.m*60.0 + 15.0*pra.s;
 pntgdec = pdec.d*3600 + pdec.m*60 + pdec.s;
 
 srcra = 15.0*ra.h*3600.0 + 15.0*ra.m*60.0 + 15.0*ra.s;
 srcdec = dec.d*3600 + dec.m*60 + dec.s;

 ra_dist = sqrt( (pntgra - srcra)*(pntgra - srcra) );
 dec_dist = sqrt( (pntgdec - srcdec)*(pntgdec - srcdec) );

 if( (ra_dist < BW_RA/2) && (dec_dist < BW_DEC/2) )
 { fprintf(finfo , "%s Found cal source within beam\n", info);
   return 1;
 }
 else return 0;

}

void associate( SkySrcType *skysrc, RAType ra, DecType dec, int nelem )
{
 double ptg_dec, src_dec, dec_fan;
 //static int i = 0, j = 0;
 ptg_dec = deg2rad( dec2sxg(&dec) );
 dec_fan = deg2rad(4.5)*(nelem/PHASE_I) / cos(ptg_dec);
 src_dec = ptg_dec + dec_fan*uniform_noise();

 srand( rand() );
 skysrc->ra = sxg2ra( ra2sxg(&ra) + 1.75*uniform_noise() ) ;
 skysrc->dec = sxg2dec(rad2deg(src_dec));
 /*skysrc->ra = sxg2ra( ra2sxg(&ra) + 0.175*(i-5)) ;
 j++; 
 if ( j == 11 ) { i++; j = 0; }
 if ( i == 11 ) i = 0;
 skysrc->dec = sxg2dec(rad2deg(ptg_dec) + 0.45*(j-5));*/
 src_dec = deg2rad(dec2sxg( &(skysrc->dec)));
 skysrc->S = 1.0;//1.0*(uniform_noise() + 0.5);
 skysrc->m = sin( ptg_dec - src_dec);//uniform_noise()*deg2rad(4.5);
}

void assign ( SkySrcType *skysrc, RAType ra, DecType dec)
{
 srand( time(NULL) );
 skysrc->ra  = ra;
 skysrc->dec = dec;
 skysrc->S = 1.0; //(int)( rand() % 100 ) ;
 skysrc->m = 0.0;
 skysrc->alpha = 0.7;
}

int find_bin( double RV, double *n_S)
{
 int i = 0;
 while(n_S[i] < RV) i++;
 return i;
 }

double genSrc( int bin, double *S_lft, double *S_rgt )
{
 double flux = S_lft[bin] + (S_rgt[bin] - S_lft[bin])*(uniform_noise() + 0.5 );
 return flux;
}


// Power spectrum of diffuse foreground specific intensity fluctuation 

double  P_I(double nu,double U, double alpha, double beta)
{
 double pu;
 double A_150=5.13e-4;
 double c = LIGHTVEL;
 pu=pow(2.*k_B*nu*nu*1.0e12/(c*c),2.)*A_150*pow(150.0/nu,2.0*alpha)*pow(1000./(2.*M_PI*U),beta);
 return(pu);
}

double  P_P(double nu,double U)
{
 double pu;
 double A_150=0.0025*5.13e-4,alpha=2.52, betav=1.65;
 double c = LIGHTVEL;
 pu=pow(2.*k_B*nu*nu*1.0e12/(c*c),2.)*A_150*pow(150.0/nu,2.0*alpha)*pow(1000./(2.*M_PI*U),betav);

 return(pu);
}

double  P_PPA(double nu,double U)
{
 double pu;
 double A = 45.0,  betav=4.5;
 double c = LIGHTVEL;
 pu = pow(2.*k_B*nu*nu*1.0e12/(c*c),2.) * A * pow(1000./(2.*M_PI*U),betav);

 return(pu);
}

double  P_I_PS(double nu,double U)
{
 double pu = 0.0;
 double A,alpha=2.7,beta=0.9;
 double nu0 = 326.5;
 double c = LIGHTVEL;
 double S_c = 0.180; //mJy
 
 A = 453*pow(S_c/1000.0, 0.72) - 112*pow(S_c/1000.0, 0.36) + 161.0;
 pu=pow(2.*k_B*nu*nu*1.0e12/(c*c),2.)*A*pow(nu0/nu,2.0*alpha)*pow(1000./(2.*M_PI*U),beta);
 

 A = 288*pow(S_c/1000.0, 1.36) + 0.01;
 beta = 0.0;
 pu += pow(2.*k_B*nu*nu*1.0e12/(c*c),2.)*A*pow(nu0/nu,2.0*alpha)*pow(1000./(2.*M_PI*U),beta);

 return(pu);
}


//This generates the dc part of the diffuse radiation

double D_C(double nu,double temp)
{
  double dc;
  double c = LIGHTVEL;
  dc=2.*k_B*temp*nu*nu*1.0e12/(c*c);
  return(dc);
}
/*
void read_diffuse_map( ProjParType *proj )
{
 FILE *fp = fopen("de.dat", "r");
 GalFGType *gal = &(proj->fg);
 int i, j;
 int mapside = MAPSIDE;

  gal->fullsize = mapside;
  gal->angres = rad2deg(5.23e-4);
  gal->map = (double**)calloc(sizeof(double*), proj->chans);
  gal->map = (double**)calloc(sizeof(double*), mapside);

  for(i=0;i<mapside;++i)
   { gal->map[i] = (double*)calloc(sizeof(double), mapside);
     for(j=0;j<mapside;++j) fscanf(fp, "%f\n", &gal->map[i][j]);
   }

  fclose(fp);
}
*/


void power_spec(ProjParType *proj, double *usum, double *pksum, int *no, fftw_complex* in)
{
  double Umax,Umin,binsiz,u;
  int i,j,index,ia,tmp;
  int no_of_bins = 20;
  int mapside;
  double L;//Resolution in radian
  double length; 

  if( proj->ORTelements == PHASE_I ) 
  { L = CELLSIZE;  
    mapside = MAPSIDE;
  }

  else if( proj->ORTelements == PHASE_II)
  { L = CELLSIZE*6;
    mapside = MAPSIDE;
  }

  length = L*mapside;

  for(i=0;i<no_of_bins;++i)
    {
      no[i]=0;
      usum[i]=0.0;
    }

  Umax=sqrt(2.)*(mapside/2.)/length;
  Umin=1./(length);
  binsiz=(log10(1.*mapside/sqrt(2.))/(1.*no_of_bins));
  
   for(i=0;i<mapside;++i)
    for(j=0;j<mapside/2+1;++j)
      {
  	index=i*(mapside/2+1)+j;
  	ia=(i>mapside/2) ? (mapside-i) : i ;
  	u=sqrt(1.*(ia*ia+j*j))/length;
  	if((u>Umin)&&(u<Umax))
  	  {
  	    tmp=(int) floor(log10(u/Umin)/binsiz);
  	    no[tmp]++;
	    pksum[tmp]+=( (in[index][0]*in[index][0]) + (in[index][1]*in[index][1]) );
  	    usum[tmp]+=u;
  	  }
      }
  for(i=0;i<no_of_bins;++i)
    if(no[i] != 0)
      {
	usum[i]=usum[i]/no[i];
	pksum[i]=pksum[i]/no[i];
      }
}

void gen_pol_amp( ProjParType *proj)
{
  fftw_complex *in;
  double *out;
  fftw_plan p, q;
  //double **fgmap;
  int i,j,k,index,index1,xdim,ydim,ia,nchan;
  double L,nu, nu0,fac,length,u,amp,rms;
  double thetax,thetay,theta,be;
  long seed;
  //rfftwnd_plan p;
  FILE *fp_ps;
  const gsl_rng_type *RNG_T;
  const gsl_rng *r;
  CmplxType noise;
  double stddev = 1.0;
  PolGalFGType *pol = &(proj->polfg);
  DipoleParType *dp = &(proj->dipole);
  gsl_rng_default_seed = 2;//gen_seed();
  RNG_T = (gsl_rng_type*)gsl_rng_ranlxs2;
  r = (gsl_rng*)gsl_rng_alloc(RNG_T);
  int mapside;
  int no_of_bins = 39;
  int *num;
  double *binu,*ps_beam;
  double p11=0.,pstot=0.,pstot2=0.;
  int tot=0;
  double cont=1.;
  double AA;
  double lambda = dp->lambda, b, d;
  PrimaryBeamParType *pb = &(proj->pbeam);
  int ch;
  double deltanu = proj->bandwidth/proj->chans;
  double alpha=2.52,  scale=0.0;

  fprintf(finfo, "%s Computing GALFG power spectrum...", info);
  fflush(finfo);

  if( proj->ORTelements == PHASE_I ) 
  { L = CELLSIZE;  
    mapside = MAPSIDE;
    b = 30.0; 
    d = 11.5;

  }

  else if( proj->ORTelements == PHASE_II)
  { L = CELLSIZE*6;
    mapside = MAPSIDE;
    b = 30.0; 
    d = 1.92;
  }

  nu0 = 326.5;//in MHz

  ydim=(mapside/2+1);
  xdim=mapside;
  fac=L/(sqrt(2.)*mapside);
  length=L*mapside;

  out=(double*)calloc ( (mapside*(mapside+2)), sizeof(double));
  in=(fftw_complex*)out;

  p = fftw_plan_dft_c2r_2d(mapside, mapside, in, out, FFTW_ESTIMATE);
  q = fftw_plan_dft_r2c_2d(mapside, mapside, out, in, FFTW_ESTIMATE);

  num = (int*) calloc(no_of_bins, sizeof(int));
  binu = (double*) calloc(no_of_bins, sizeof(double));
  ps_beam = (double*) calloc(no_of_bins, sizeof(double));

  // Fill Fourier components 
  //along x axis (j = 0 and j = N/2)
  for(j=0; j<ydim; j=j+mapside/2){
  for(i=1;i<mapside/2;++i)
  { noise.r = gsl_ran_gaussian(r, stddev);
    noise.i = gsl_ran_gaussian(r, stddev);
    // along + x 
    u=sqrt(1.*(i*i+j*j))/length;
    amp=fac*sqrt(P_P(nu0,u));
    index=i*ydim+j;
    in[index][0]=amp*noise.r;
    in[index][1]=amp*noise.i;
    
    // along -x 
    index1=(mapside-i)*ydim+j;
    in[index1][0]=in[index][0];
    in[index1][1]=-in[index][1];
  }
  }
  // upper half plane excluding x axis

  for(i=0;i<xdim;++i)
  for(j=1;j<mapside/2;++j)
  { noise.r = gsl_ran_gaussian(r, stddev);
    noise.i = gsl_ran_gaussian(r, stddev);
    ia= (i>mapside/2) ? (mapside-i) : i ;
    u=sqrt(1.*(ia*ia+j*j))/length;
    amp=fac*sqrt(P_P(nu0,u));
    index=i*ydim+j;
    in[index][0] = pow(-1., i+j)*amp*noise.r;
    in[index][1] = pow(-1., i+j)*amp*noise.i;
  }

  //4 points remain 
  for(i=0;i<2;++i)
  for(j=0;j<2;++j)
  { if(i+j==0) 
    { in[0][0]=0.0;//D_C(nu0, 5e-7);
      in[0][1]=0.0;
    }
    else
    { u=(mapside/2.)*sqrt(1.*(i*i+j*j))/length;
      amp=fac*sqrt(P_P(nu0,u));
      index=i*(mapside/2)*ydim+j*(mapside/2);
      
      in[index][0] = pow(-1., (i*mapside/2 + j*mapside/2))*amp*gsl_ran_gaussian(r, stddev);
      in[index][1] = 0.0;
    }
  }
  
  //power_spec(proj, binu, ps_nobeam+k*no_of_bins, num, in);

  // finished filling Fourier components
  //fprintf(stderr, "inverting...");
  fftw_execute(p);

  //p= rfftw2d_create_plan ( mapside, mapside, FFTW_COMPLEX_TO_REAL, FFTW_ESTIMATE | FFTW_IN_PLACE );
  //rfftwnd_one_complex_to_real ( p, in, NULL);
  pol->fullsize = mapside;
  pol->angres = rad2deg(L);
  pol->pbside = mapside;

  pol->Qmap = (double***)calloc(sizeof(double**), proj->chans);
  for(ch = 0; ch < proj->chans; ch++)
  { pol->Qmap[ch] = (double**)calloc(sizeof(double*), mapside);
    nu = (proj->bandcentre - proj->bandwidth/2 + ch*deltanu + deltanu/2) / 1e6;
    scale = pow(nu0/nu, alpha-2);
    for(i=0;i<mapside;++i)
    { pol->Qmap[ch][i] = (double*)calloc(sizeof(double), mapside);
      for(j=0;j<mapside;++j)
      { index=i*(mapside+2)+j;
	thetax= (i-mapside/2)*L;
	thetay= (j-mapside/2)*L;
	pol->Qmap[ch][i][j] = scale * out[index];
      }
    }
  }
  fprintf(stderr, "done.\n");

  fftw_destroy_plan(p);
  fftw_destroy_plan(q);
  fftw_free(out);
  //rfftwnd_destroy_plan ( p);

}


//Generate Galactic foreground
void gen_pol_PA(ProjParType *proj)
{
  fftw_complex *in;
  double *out;
  fftw_plan p, q;
  //double **fgmap;
  int i,j,k,index,index1,xdim,ydim,ia,nchan;
  double L,nu, nu0,fac,length,u,amp,rms;
  double thetax,thetay,theta,be;
  long seed;
  //rfftwnd_plan p;
  FILE *fp_ps;
  const gsl_rng_type *RNG_T;
  const gsl_rng *r;
  CmplxType noise;
  double stddev = 1.0;
  PolGalFGType *pol = &(proj->polfg);
  DipoleParType *dp = &(proj->dipole);
  gsl_rng_default_seed = 1;//gen_seed();
  RNG_T = (gsl_rng_type*)gsl_rng_ranlxs2;
  r = (gsl_rng*)gsl_rng_alloc(RNG_T);
  int mapside;
  int no_of_bins = 39;
  int *num;
  double *binu,*ps_beam;
  double p11=0.,pstot=0.,pstot2=0.;
  int tot=0;
  double cont=1.;
  double AA;
  double lambda = dp->lambda, b, d;
  PrimaryBeamParType *pb = &(proj->pbeam);
  int ch;
  double deltanu = proj->bandwidth/proj->chans;
  double alpha=2.52, scale=0.0;

  fprintf(finfo, "%s Computing GALFG power spectrum...", info);
  fflush(finfo);

  if( proj->ORTelements == PHASE_I ) 
  { L = CELLSIZE;  
    mapside = MAPSIDE;
    b = 30.0; 
    d = 11.5;

  }

  else if( proj->ORTelements == PHASE_II)
  { L = CELLSIZE*6;
    mapside = MAPSIDE;
    b = 30.0; 
    d = 1.92;
  }

  nu0 = 326.5;//in MHz

  ydim=(mapside/2+1);
  xdim=mapside;
  fac=L/(sqrt(2.)*mapside);
  length=L*mapside;

  out=(double*)calloc ( (mapside*(mapside+2)), sizeof(double));
  in=(fftw_complex*)out;

  p = fftw_plan_dft_c2r_2d(mapside, mapside, in, out, FFTW_ESTIMATE);
  q = fftw_plan_dft_r2c_2d(mapside, mapside, out, in, FFTW_ESTIMATE);

  num = (int*) calloc(no_of_bins, sizeof(int));
  binu = (double*) calloc(no_of_bins, sizeof(double));
  ps_beam = (double*) calloc(no_of_bins, sizeof(double));

  // Fill Fourier components 
  //along x axis (j = 0 and j = N/2)
  for(j=0; j<ydim; j=j+mapside/2){
  for(i=1;i<mapside/2;++i)
  { noise.r = gsl_ran_gaussian(r, stddev);
    noise.i = gsl_ran_gaussian(r, stddev);
    // along + x 
    u=sqrt(1.*(i*i+j*j))/length;
    amp=fac*sqrt(P_PPA(nu0,u));
    index=i*ydim+j;
    in[index][0]=amp*noise.r;
    in[index][1]=amp*noise.i;
    
    // along -x 
    index1=(mapside-i)*ydim+j;
    in[index1][0]=in[index][0];
    in[index1][1]=-in[index][1];
  }
  }
  // upper half plane excluding x axis

  for(i=0;i<xdim;++i)
  for(j=1;j<mapside/2;++j)
  { noise.r = gsl_ran_gaussian(r, stddev);
    noise.i = gsl_ran_gaussian(r, stddev);
    ia= (i>mapside/2) ? (mapside-i) : i ;
    u=sqrt(1.*(ia*ia+j*j))/length;
    amp=fac*sqrt(P_PPA(nu0,u));
    index=i*ydim+j;
    in[index][0] = pow(-1., i+j)*amp*noise.r;
    in[index][1] = pow(-1., i+j)*amp*noise.i;
  }

  //4 points remain 
  for(i=0;i<2;++i)
  for(j=0;j<2;++j)
  { if(i+j==0) 
    { in[0][0]=0.0;//DC(nu0, 5.0e-7);
      in[0][1]=0.0;
    }
    else
    { u=(mapside/2.)*sqrt(1.*(i*i+j*j))/length;
      amp=fac*sqrt(P_PPA(nu0,u));
      index=i*(mapside/2)*ydim+j*(mapside/2);
      
      in[index][0] = pow(-1., (i*mapside/2 + j*mapside/2))*amp*gsl_ran_gaussian(r, stddev);
      in[index][1] = 0.0;
    }
  }
  
  //power_spec(proj, binu, ps_nobeam+k*no_of_bins, num, in);

  // finished filling Fourier components
  //fprintf(stderr, "inverting...");
  fftw_execute(p);

  //p= rfftw2d_create_plan ( mapside, mapside, FFTW_COMPLEX_TO_REAL, FFTW_ESTIMATE | FFTW_IN_PLACE );
  //rfftwnd_one_complex_to_real ( p, in, NULL);
  pol->fullsize = mapside;
  pol->angres = rad2deg(L);
  pol->pbside = mapside;

  pol->Umap = (double***)calloc(sizeof(double**), proj->chans);
  for(ch = 0; ch < proj->chans; ch++)
  { pol->Umap[ch] = (double**)calloc(sizeof(double*), mapside);
    nu = (proj->bandcentre - proj->bandwidth/2 + ch*deltanu + deltanu/2) / 1e6;
    scale = pow(nu0/nu, alpha-2);
    for(i=0;i<mapside;++i)
    { pol->Umap[ch][i] = (double*)calloc(sizeof(double), mapside);
      for(j=0;j<mapside;++j)
      { index=i*(mapside+2)+j;
	thetax= (i-mapside/2)*L;
	thetay= (j-mapside/2)*L;
	pol->Umap[ch][i][j] = scale * out[index];// + gsl_ran_gaussian(r, stddev);
      }
    }
  }
  fprintf(stderr, "done.\n");

  fftw_destroy_plan(p);
  fftw_destroy_plan(q);
  fftw_free(out);
  //rfftwnd_destroy_plan ( p);
  
}

void gen_gal_pol_map( ProjParType *proj )
{
  int ch, i, j;
  int mapside;
  double P, PA;  
  double Q, U;
  PolGalFGType *pol = &(proj->polfg);


  gen_pol_amp( proj );
  gen_pol_PA( proj );

  write_pol_Qmap2FITS( proj );
  write_pol_Umap2FITS( proj );


  for(ch = 0; ch < proj->chans; ch++)
  { for(i=0;i<mapside;++i)
   { for(j=0;j<mapside;++j)
     { P = pol->Qmap[ch][i][j];
       PA = pol->Umap[ch][i][j];
       Q = P * sin(deg2rad(PA));
       U = P * cos(deg2rad(PA));
       pol->Qmap[ch][i][j] = Q; 
       pol->Umap[ch][i][j] = U;
     }
   }
  }

}

//Generate Galactic foreground
void gen_gal_fg(ProjParType *proj)
{
  fftw_complex *in;
  double *out;
  fftw_plan p, q;
  //double **fgmap;
  int i,j,k,index,index1,xdim,ydim,ia,nchan;
  double L,nu, nu0,fac,length,u,amp,rms;
  double thetax,thetay,theta,be;
  long seed;
  //rfftwnd_plan p;
  FILE *fp_ps;
  const gsl_rng_type *RNG_T;
  const gsl_rng *r;
  CmplxType noise;
  double stddev = 1.0;
  GalFGType *gal = &(proj->fg);
  DipoleParType *dp = &(proj->dipole);
  gsl_rng_default_seed = 1;//gen_seed();
  RNG_T = (gsl_rng_type*)gsl_rng_ranlxs2;
  r = (gsl_rng*)gsl_rng_alloc(RNG_T);
  int mapside;
  int no_of_bins = 20;
  double temp[20];
  int *num;
  double *binu,*ps_beam;
  double p11=0.,pstot=0.,pstot2=0.;
  int tot=0;
  double cont=1.;
  double AA;
  double lambda = dp->lambda, b, d;
  PrimaryBeamParType *pb = &(proj->pbeam);
  int ch;
  double deltanu = proj->bandwidth/proj->chans;
  double alpha=ALPHA, beta=BETA, scale=0.0;
  double err;

  SkyMapType *sky = &(proj->skymap);

  fprintf(finfo, "%s Computing GALFG power spectrum...", info);
  fflush(finfo);

  if( proj->ORTelements == PHASE_I ) 
  { L = CELLSIZE;  
    mapside = MAPSIDE;
    b = 30.0; 
    d = 11.5;

  }

  else if( proj->ORTelements == PHASE_II)
  { L = CELLSIZE*6;
    mapside = MAPSIDE;
    b = 30.0; 
    d = 1.92;
  }

  nu0 = 326.5;//in MHz

  ydim=(mapside/2+1);
  xdim=mapside;
  fac=L/(sqrt(2.)*mapside);
  length=L*mapside;

  out=(double*)calloc ( (mapside*(mapside+2)), sizeof(double));
  in=(fftw_complex*)out;

  p = fftw_plan_dft_c2r_2d(mapside, mapside, in, out, FFTW_ESTIMATE);
  q = fftw_plan_dft_r2c_2d(mapside, mapside, out, in, FFTW_ESTIMATE);

  num = (int*) calloc(no_of_bins, sizeof(int));
  binu = (double*) calloc(no_of_bins, sizeof(double));
  ps_beam = (double*) calloc(no_of_bins, sizeof(double));

  // Fill Fourier components 
  //along x axis (j = 0 and j = N/2)
  for(j=0; j<ydim; j=j+mapside/2){
  for(i=1;i<mapside/2;++i)
  { noise.r = gsl_ran_gaussian(r, stddev);
    noise.i = gsl_ran_gaussian(r, stddev);
    // along + x 
    u=sqrt(1.*(i*i+j*j))/length;
    amp=fac*sqrt(P_I(nu0,u, alpha, beta));
    index=i*ydim+j;
    in[index][0]=amp*noise.r;
    in[index][1]=amp*noise.i;
    
    // along -x 
    index1=(mapside-i)*ydim+j;
    in[index1][0]=in[index][0];
    in[index1][1]=-in[index][1];
  }
  }
  // upper half plane excluding x axis

  for(i=0;i<xdim;++i)
  for(j=1;j<mapside/2;++j)
  { noise.r = gsl_ran_gaussian(r, stddev);
    noise.i = gsl_ran_gaussian(r, stddev);
    ia= (i>mapside/2) ? (mapside-i) : i ;
    u=sqrt(1.*(ia*ia+j*j))/length;
    amp=fac*sqrt(P_I(nu0,u, alpha,beta));
    index=i*ydim+j;
    in[index][0] = pow(-1., i+j)*amp*noise.r;
    in[index][1] = pow(-1., i+j)*amp*noise.i;
  }

  //4 points remain 
  for(i=0;i<2;++i)
  for(j=0;j<2;++j)
  { if(i+j==0) 
    { in[0][0]=0.0;//DC(nu0, 5.0e-7);
      in[0][1]=0.0;
    }
    else
    { u=(mapside/2.)*sqrt(1.*(i*i+j*j))/length;
	amp=fac*sqrt(P_I(nu0,u, alpha,beta));
      index=i*(mapside/2)*ydim+j*(mapside/2);
      
      in[index][0] = pow(-1., (i*mapside/2 + j*mapside/2))*amp*gsl_ran_gaussian(r, stddev);
      in[index][1] = 0.0;
    }
  }
  
  //power_spec(proj, binu, ps_nobeam+k*no_of_bins, num, in);

  // finished filling Fourier components
  //fprintf(stderr, "inverting...");
  fftw_execute(p);

  //p= rfftw2d_create_plan ( mapside, mapside, FFTW_COMPLEX_TO_REAL, FFTW_ESTIMATE | FFTW_IN_PLACE );
  //rfftwnd_one_complex_to_real ( p, in, NULL);
  gal->fullsize = mapside;
  gal->angres = rad2deg(L);
  gal->pbside = mapside;
  /**/
  gal->map = (double***)calloc(sizeof(double**), proj->chans);
  for(ch = 0; ch < proj->chans; ch++)
  { gal->map[ch] = (double**)calloc(sizeof(double*), mapside);
    nu = (proj->bandcentre - proj->bandwidth/2 + ch*deltanu + deltanu/2) / 1e6;
    scale = pow(nu0/nu, alpha-2);
    for(i=0;i<mapside;++i)
    { gal->map[ch][i] = (double*)calloc(sizeof(double), mapside);
      for(j=0;j<mapside;++j)
      { index=i*(mapside+2)+j;
	thetax= (i-mapside/2)*L;
	thetay= (j-mapside/2)*L;
	gal->map[ch][i][j] = scale * out[index];
      }
    }
  }
  fprintf(stderr, "done.\n");
 /*/
  gal->map = (double***)calloc(sizeof(double**), proj->chans);
  for(ch = 0; ch < proj->chans; ch++)
  { gal->map[ch] = (double**)calloc(sizeof(double*), mapside);
    nu = (proj->bandcentre - proj->bandwidth/2 + ch*deltanu + deltanu/2) / 1e6;
    scale = 1.0;
    for(i=0;i<mapside;++i)
    { gal->map[ch][i] = (double*)calloc(sizeof(double), mapside);
      for(j=0;j<mapside;++j)
      { index=i*(mapside+2)+j;
	thetax= (i-mapside/2)*L;
	thetay= (j-mapside/2)*L;
	gal->map[ch][i][j] = scale * sky->map[ch][i][j];;
      }
    }
  }
  fprintf(stderr, "done.\n");
*/

  /******* When the primary beam is switched off, the constant of multiplication is 1/(length*length)
Now, when the primary beam is switched on, the constant of multiplication is

        1
-----------------     =     919.44 for ORT Phase-I

 4            1
--- lambda^2  ---
 9           (bd)

where b = 30 m, d = 11.5 m, lambda = 0.918 m.

  */

  /******* Primary beam part ****/
 for ( i = 0 ; i < pb->yside ; i++ )
 { for ( j = 0 ; j < pb->xside ; j++ ) 
   {  index=i*(mapside+2)+j;      
       out[index] = gal->map[proj->chans/2][i][j];// * pb->shape[proj->chans/2][i][j] ; 
   }
 }

 /* Integral of the square of the FT of the primary beam, a sinc function in RA and dec */
 AA = (4.0/9.0) * lambda*lambda/(b*d);
 //AA = 1.0;
 k = 0;

 fftw_execute(q);
 fprintf(finfo, "%s Diffuse emission scaled by primary beam ...done!\n", info);
 power_spec(proj, binu, ps_beam+k*no_of_bins, num, in);
 

 fp_ps=fopen("powerspec.dat","w");

  for(i=0;i<no_of_bins;i++)
    if(num[i]>0)
      {
        fprintf(fp_ps,"%e\t%d\t%e\t",binu[i], num[i], P_I(nu0, binu[i], alpha, beta)*AA/cont);
	tot=0;
	pstot=0.;
	pstot2=0.;
	j=0;
	index=j*no_of_bins+i;
	p11=ps_beam[index];
	pstot+=p11;
	pstot2 += (p11*p11);
	tot++;

	pstot=pstot/tot;

	err = sqrt(pstot/num[i]);

	pstot2=pstot2/tot;
	fprintf(fp_ps,"%e\t%e\n", pstot/cont, err);
      }
  fclose(fp_ps);

  fftw_destroy_plan(p);
  fftw_destroy_plan(q);
  fftw_free(out);
  //rfftwnd_destroy_plan ( p);
  
  }

//Generate Ionospheric TEC screen
void gen_TEC_scr(ProjParType *proj)
{
  fftw_complex *in;
  double *out;
  fftw_plan p;
  //double **fgmap;
  int i,j,k,index,index1,xdim,ydim,ia,nchan;
  double L,nu0,deltanu,fac,length,u,amp,rms,alpha=0.8;
  double thetax,thetay,theta,be;
  long seed;
  //rfftwnd_plan p;
  //FILE *fp,*fp1;
  const gsl_rng_type *RNG_T;
  const gsl_rng *r;
  CmplxType noise;
  double stddev = 1.0;
  TECScrType *tec = &(proj->tec);
  gsl_rng_default_seed = gen_seed();
  RNG_T = (gsl_rng_type*)gsl_rng_ranlxs2;
  r = (gsl_rng*)gsl_rng_alloc(RNG_T);
  int mapside;
  int limit = 6;

  fprintf(finfo, "%s Computing ionospheric TEC...", info);
  fflush(finfo);

  if( proj->ORTelements == PHASE_I ) mapside = MAPSIDE;
  else if( proj->ORTelements == PHASE_II ) mapside = 8*MAPSIDE;

  L = 2e-4;//Resolution in radian
  nu0 = 326.0;//in MHz

  ydim=(mapside/2+1);
  xdim=mapside;
  length=L*mapside;

  out=(double*)calloc ( (mapside*(mapside+2)), sizeof (double));
  in=(fftw_complex*)&out[0];

  p = fftw_plan_dft_c2r_2d(mapside, mapside, in, out, FFTW_ESTIMATE);
    
  // Fill Fourier components 
  //along x axis (j = 0)
  j=0;
  for(i=1;i<limit/2;i++)
  { noise.r = gsl_ran_gaussian(r, stddev);
    noise.i = gsl_ran_gaussian(r, stddev);

    // along + x 
    index=i*ydim+j;
    in[index][0] = noise.r;
    in[index][1] = noise.i;
    
    // along -x 
    index1=(mapside-i)*ydim+j;
    in[index1][0] = in[index][0];
    in[index1][1] = -in[index][1];
    
  }

  // upper half plane excluding x axis
  for(i=0;i<limit;i++)
  for(j=1;j<limit;j++)
  { noise.r = gsl_ran_gaussian(r, stddev);
    noise.i = gsl_ran_gaussian(r, stddev);

    index=i*ydim+j;
    in[index][0] = noise.r;
    in[index][1] = noise.i;
  }
  
  //4 points remain 
  for(i=0;i<2;++i)
  for(j=0;j<2;++j)
  { if(i+j==0) 
    { in[0][0] = 100.0;
      in[0][1] = 0.0;
    }
    else
    { index=i*(mapside/2)*ydim+j*(mapside/2);
      in[index][0] = 0.0;//gsl_ran_gaussian(r, stddev);
      in[index][1] = 0.0;
    }
  }
  // finished filling Fourier components
  fprintf(stderr, "inverting...");
  fftw_execute(p);
  //p= rfftw2d_create_plan ( mapside, mapside, FFTW_COMPLEX_TO_REAL, FFTW_ESTIMATE | FFTW_IN_PLACE );

  //rfftwnd_one_complex_to_real ( p, in, NULL);
  tec->fullsize = mapside;
  tec->angres = rad2deg(L);
  tec->map = (double**)calloc(sizeof(double*), mapside);

  for(i=0;i<mapside;++i)
   { tec->map[i] = (double*)calloc(sizeof(double), mapside);
     for(j=0;j<mapside;++j)
    { index=i*(mapside+2)+j;
      tec->map[i][j] = 1e12*out[index];
    }
  }
  fprintf(stderr, "done.\n");
  fftw_free(out);
  fftw_destroy_plan(p);
  //rfftwnd_destroy_plan ( p);

}

/*
void gen_spidx( ProjParType *proj )
{
 double A = 40, mu = 0.4, sigma = 1.2;
 double alpha_min = -0.3, alpha_max = 0.7;

 if ( proj->nsrc == 0 )
 {

 }
}
*/

void gen_mock_EPS_maps( ProjParType *proj, int nmaps )
{

 int i, j, n, ch;
 int bin;
 double ***EPSmap;
 double **pmod;
 char command[80];
 long int nsrcs;
 double fov, dec_fan;
 double ptg_dec, src_dec;
 FILE *fpmod, *fdNdS; 
 FILE *fmap = fopen("EPS.map", "w");
 FILE *falpha = fopen("alpha.dat", "w");
 double theta;
 double deltanu = proj->bandwidth / proj->chans;
 double nu0 = 326.5;
 int mapside;
 GalFGType *eps;
 double dS;
 double x, dx;
 double *n_S, N_S;
 double nu;
 double S_low, S_high;
 int bins = 30;
 double S_lft[bins], S_rgt[bins], S_ctr[bins], N[bins], N_cum[bins];
 char *pkgroot, dNdSfile[80], pmodfile[80];
 char str[120];

 double stddev = 0.25;
 double **alpha;

 const gsl_rng_type *T;
 const gsl_rng *r;
 gsl_rng_default_seed = gen_seed();;
 T = (gsl_rng_type*)gsl_rng_ranlxs2;
 r = (gsl_rng*)gsl_rng_alloc(T);

 
 theta = CELLSIZE;
 mapside = MAPSIDE;
 fov = nmaps * pow(theta * mapside, 2);

 alpha = (double**)calloc(mapside, sizeof(double*));
 for( i = 0 ; i < mapside ; i++)
  { alpha[i] = (double*)calloc(mapside, sizeof(double));
    for( j = 0 ; j < mapside ; j++)
    { alpha[i][j] = -0.7 + gsl_ran_gaussian(r, stddev);
      fprintf(falpha, "%f\n", alpha[i][j]);
    }
  }

 fclose(falpha);

 pkgroot = getenv("PKGROOT");
 sprintf(dNdSfile, "%s/inc/325.source.counts.50Jy.modified", pkgroot);
 sprintf(pmodfile, "%s/inc/pmod.dat", pkgroot);
 if( (fdNdS = fopen(dNdSfile, "r")) == NULL ) exit(-1);

 N_S = 0.0;
 do 
 {fgets( str, sizeof(str), fdNdS ); }
 while( ! strstr(str,"##################") );
 for( i = 0 ; i < bins ; i++ )
 {  fgets( str, sizeof(str), fdNdS );
    sscanf(str, "%lf %lf %lf %lf", S_lft+i, S_rgt+i, S_ctr+i, N+i);
    N[i] *= fov;
    N_S += N[i];
    N_cum[i] = N_S;
    fprintf(finfo, "%s %lf %lf %lf %lf\n", info, S_lft[i], S_rgt[i], S_ctr[i], N[i]);
 }

 fclose(fdNdS);

 eps = &(proj->fg);
 fprintf(finfo, "%s Making %d realizations of mock EPS maps\n", info, nmaps);

 EPSmap = (double***)calloc(nmaps, sizeof(double**));
 for (n = 0 ; n < nmaps ; n++ )
 { EPSmap[n] = (double**)calloc(mapside, sizeof(double*));
   for ( i = 0 ; i < mapside ; i++ ) 
     EPSmap[n][i] = (double*)calloc(mapside, sizeof(double) );
 }

 pmod = (double**)calloc(mapside, sizeof(double*));
 for(i = 0; i<mapside;i++) pmod[i] = (double*)calloc(mapside,sizeof(double));

 ptg_dec = deg2rad( dec2sxg(&(proj->scan[0].dec)) );
 dec_fan = deg2rad(4.5)*(proj->ORTelements/PHASE_I) / cos(ptg_dec);

 fpmod = fopen(pmodfile, "r");
 
 for ( i = 0 ; i < mapside ; i++ )
 { for ( j = 0 ; j < mapside ; j++ )
   { fscanf(fpmod, "%lf\n", &pmod[i][j]);
     nsrcs = floor(nmaps * pmod[i][j]);
     while( nsrcs-- > 0 )
     { n = floor(nmaps * (uniform_noise()+0.5));
       if(n == nmaps) n -= 1;
       bin = find_bin( N_S * (uniform_noise() + 0.5), N_cum ); // uniform RV in [0,1]
       EPSmap[n][i][j] += genSrc( bin, S_lft, S_rgt );
     }
   }
     fprintf(finfo, "%s |-------------->  %3.1f %c done <--------------|\r", info,  (double)((i+1)*(j+1)*100.0/(mapside*mapside)),'%' );
 }
 
 free(pmod);
 fclose(fpmod);
 //newline();

  eps->fullsize = mapside;
  eps->angres = rad2deg(theta);
  eps->pbside = mapside;

  eps->map = (double***)calloc(proj->chans, sizeof(double**));
  for(ch = 0; ch < proj->chans; ch++)
  { eps->map[ch] = (double**)calloc(mapside, sizeof(double*));
    for(i=0;i<mapside;i++) eps->map[ch][i] = (double*)calloc(mapside, sizeof(double));
  }
  newline();
  for( n = 0 ; n < nmaps ; n++ )
  { for(ch = 0; ch < proj->chans; ch++)
    { nu = (proj->bandcentre - proj->bandwidth/2 + ch*deltanu + deltanu/2) / 1e6;
     for ( i = 0 ; i < mapside ; i++ )
     { for ( j = 0 ; j < mapside ; j++ )
      {  eps->map[ch][i][j] = pow(nu0/nu, alpha[i][j]) * EPSmap[n][i][j];
	  if( n == 0 )fprintf(fmap, "%lf ", eps->map[0][i][j] );
      }
	  if(n == 0) fprintf(fmap, "\n");
     }
    }
  newline();
  write_fgmap2FITS( proj );
  fprintf(finfo, "%s |-------------->  [%3d / %3d] maps done <--------------|\r", info, n+1, nmaps );
  sprintf(command, "mv GALFG.FITS EPS%d.FITS", n+1);
  fprintf(finfo, "%s %s\n", info, command);
  system(command);
  }

 fclose(fmap);
 free(pmod);
 free(EPSmap);
 
}
