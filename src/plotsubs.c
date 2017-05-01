#include "../inc/plotdefs.h"
#include "../inc/sysdefs.h"
#include <math.h>
#include <string.h>

#define True YELLOW
#define Estimated CYAN

#define GScale YELLOW
#define MScale CYAN
#define COLOR XSCOLOR

#define MAXCOL 64

void read_colormap(char *cmapfile, colormap *cmap)
{
 FILE *fcmap; 
 char *pkgroot,str[120];
 int i, N = MAXCOL;

 pkgroot = getenv("PKGROOT");
 sprintf(str, "%s/inc/%s", pkgroot, cmapfile);
 fcmap = fopen(str, "r");

 do 
 {fgets( str, sizeof(str), fcmap ); }
 while( ! strstr(str,"##################") );
 for( i = 0 ; i < N ; i++ )
 {  fgets( str, sizeof(str), fcmap );
    sscanf(str, "%f %f %f", &(cmap[i].r), &(cmap[i].g), &(cmap[i].b));
 }
 fclose(fcmap);
}


void init_plotvars( PlotParType *plot )
{
  int n;
  plot->MSE_opened = 0;
  plot->chisq_opened = 0;
  plot->SNR_opened = 0;
  plot->covar_opened = 0;
  plot->Hessian_opened = 0;

  plot->gain = (CmplxType**)calloc(plot->nsol, sizeof(CmplxType*) );
  plot->vis = (CmplxType**)calloc(plot->nsol, sizeof(CmplxType*) );
  plot->g_amp = (float**)calloc(plot->nsol, sizeof(float*) );
  plot->g_phs = (float**)calloc(plot->nsol, sizeof(float*) );
  plot->V_amp = (float**)calloc(plot->nsol, sizeof(float*) );
  plot->V_phs = (float**)calloc(plot->nsol, sizeof(float*) );
  for( n = 0 ; n < plot->nsol ; n++)
  { plot->gain[n] = (CmplxType*)calloc(plot->antennas, sizeof(CmplxType) );
    plot->vis[n] = (CmplxType*)calloc(plot->redundant_baselines, sizeof(CmplxType) );
    plot->g_amp[n] = (float*)calloc(plot->antennas, sizeof(float) );
    plot->g_phs[n] = (float*)calloc(plot->antennas, sizeof(float) );
    plot->V_amp[n] = (float*)calloc(plot->antennas, sizeof(float) );
    plot->V_phs[n] = (float*)calloc(plot->antennas, sizeof(float) );
  }
  plot->g_true = (CmplxType*)calloc(plot->antennas, sizeof(CmplxType) );
  plot->V_true = (CmplxType*)calloc(plot->redundant_baselines, sizeof(CmplxType) );
  plot->g_true_amp = (float*)calloc(plot->antennas, sizeof(float) );
  plot->g_true_phs = (float*)calloc(plot->antennas, sizeof(float) );
  plot->V_true_amp = (float*)calloc(plot->redundant_baselines, sizeof(float) );
  plot->V_true_phs = (float*)calloc(plot->redundant_baselines, sizeof(float) );
}

void free_plotvars( PlotParType *plot )
{
 free(plot->gain);
 free(plot->vis);
 free(plot->g_amp);
 free(plot->g_phs);
 free(plot->V_amp);
 free(plot->V_phs);
 free(plot->g_true);
 free(plot->V_true);
 free(plot->g_true_amp);
 free(plot->g_true_phs);
 free(plot->V_true_amp);
 free(plot->V_true_phs);
 if(plot->MSE_opened) free(plot->MSE);
 if(plot->chisq_opened) free(plot->chisq);
 if(plot->SNR_opened) free(plot->SNR);
 if(plot->covar_opened) free(plot->covar);
 if(plot->Hessian_opened) free(plot->Hessian);
}

void init_trajscope( PlotParType *plot )
{
/*  XLEFT, XRIGHT, YBOT, YTOP */
 plot->g_amp_vwp[0] = 0.05;
 plot->g_amp_vwp[1] = 0.45; 
 plot->g_amp_vwp[2] = 0.75;
 plot->g_amp_vwp[3] = 0.95 ;

 plot->g_phs_vwp[0] = 0.05;
 plot->g_phs_vwp[1] = 0.45; 
 plot->g_phs_vwp[2] = 0.55;
 plot->g_phs_vwp[3] = 0.75;

 plot->V_amp_vwp[0] = 0.55;
 plot->V_amp_vwp[1] = 0.95; 
 plot->V_amp_vwp[2] = 0.75;
 plot->V_amp_vwp[3] = 0.95 ;

 plot->V_phs_vwp[0] = 0.55;
 plot->V_phs_vwp[1] = 0.95; 
 plot->V_phs_vwp[2] = 0.55;
 plot->V_phs_vwp[3] = 0.75;

 plot->g_cmplx_vwp[0] = 0.05;
 plot->g_cmplx_vwp[1] = 0.45; 
 plot->g_cmplx_vwp[2] = 0.05;
 plot->g_cmplx_vwp[3] = 0.45; 

 plot->V_cmplx_vwp[0] = 0.55;
 plot->V_cmplx_vwp[1] = 0.95; 
 plot->V_cmplx_vwp[2] = 0.05;
 plot->V_cmplx_vwp[3] = 0.45; 

}

void init_errscope( PlotParType *plot )
{
 plot->MSE_vwp[0] = 0.08;
 plot->MSE_vwp[1] = 0.92; 
 plot->MSE_vwp[2] = 0.08;
 plot->MSE_vwp[3] = 0.92; 

 plot->chisq_vwp[0] = 0.08;
 plot->chisq_vwp[1] = 0.92; 
 plot->chisq_vwp[2] = 0.08;
 plot->chisq_vwp[3] = 0.92; 

}

void init_SNRscope( PlotParType *plot )
{
 plot->SNR_vwp[0] = 0.06;
 plot->SNR_vwp[1] = 0.88; 
 plot->SNR_vwp[2] = 0.10;
 plot->SNR_vwp[3] = 0.92; 
}

void init_covarscope( PlotParType *plot )
{
 plot->covar_vwp[0] = 0.06;
 plot->covar_vwp[1] = 0.88; 
 plot->covar_vwp[2] = 0.10;
 plot->covar_vwp[3] = 0.92; 
}

void init_Hessianscope( PlotParType *plot )
{
 plot->Hessian_vwp[0] = 0.06;
 plot->Hessian_vwp[1] = 0.88; 
 plot->Hessian_vwp[2] = 0.10;
 plot->Hessian_vwp[3] = 0.92; 
}

void init_gscope( PlotParType *plot )
{
 plot->G_amp_vwp[0] = 0.08;
 plot->G_amp_vwp[1] = 0.92; 
 plot->G_amp_vwp[2] = 0.50;
 plot->G_amp_vwp[3] = 0.92; 

 plot->G_phs_vwp[0] = 0.08;
 plot->G_phs_vwp[1] = 0.92; 
 plot->G_phs_vwp[2] = 0.08;
 plot->G_phs_vwp[3] = 0.50; 

}

void init_Mscope( PlotParType *plot )
{
 plot->M_amp_vwp[0] = 0.08;
 plot->M_amp_vwp[1] = 0.92; 
 plot->M_amp_vwp[2] = 0.50;
 plot->M_amp_vwp[3] = 0.92; 

 plot->M_phs_vwp[0] = 0.08;
 plot->M_phs_vwp[1] = 0.92; 
 plot->M_phs_vwp[2] = 0.08;
 plot->M_phs_vwp[3] = 0.50; 
}

void init_scalescope( PlotParType *plot )
{
 plot->ampscale_vwp[0] = 0.18;
 plot->ampscale_vwp[1] = 0.82; 
 plot->ampscale_vwp[2] = 0.55;
 plot->ampscale_vwp[3] = 0.92; 

 plot->phoffset_vwp[0] = 0.18;
 plot->phoffset_vwp[1] = 0.82; 
 plot->phoffset_vwp[2] = 0.18;
 plot->phoffset_vwp[3] = 0.55; 

}

void init_scopes( PlotParType *plot )
{
 init_trajscope( plot );
 init_errscope( plot );
 //init_SNRscope( plot );
 init_covarscope( plot );
 init_gscope( plot );
 init_Mscope( plot );
 init_scalescope( plot ); 
}

void init_DSscope( PlotParType *plot )
{
 plot->DS_amp_vwp[0] = 0.08;
 plot->DS_amp_vwp[1] = 0.48; 
 plot->DS_amp_vwp[2] = 0.08;
 plot->DS_amp_vwp[3] = 0.92; 

 plot->DS_phs_vwp[0] = 0.52;
 plot->DS_phs_vwp[1] = 0.92; 
 plot->DS_phs_vwp[2] = 0.08;
 plot->DS_phs_vwp[3] = 0.92; 
}

void open_DSscope( PlotParType *plot )
{
 float SIDE = 3.2*VWPSIDE, SQUARE = 0.50;
 float *vwp;
 float XMIN, XMAX, YMIN, YMAX;
 int C;

 if( plot->driver == 'P' || plot->driver == 'p' )
 { C = PSCOLOR;
   plot->DSboard = cpgopen("/cps");
 }
 else if ( plot->driver == 'X' || plot->driver == 'x' )
 { C = COLOR;
   plot->DSboard = cpgopen("/xs");
 }

 cpgpap(SIDE, SQUARE);
}

void open_trajscope ( PlotParType *plot )
{
 float SIDE = VWPSIDE, SQUARE = 1.0;
 float *vwp;
 float XMIN, XMAX, YMIN, YMAX;
 int C;

 if( plot->driver == 'P' )
 { C = PSCOLOR;
   plot->dashboard = cpgopen("/cps");
 }
 else if ( plot->driver == 'X' )
 { C = COLOR;
   plot->dashboard = cpgopen("/xs");
 }

 cpgpap(1.68*SIDE, SQUARE);

 vwp = plot->g_amp_vwp;
 cpgslw(3);
 cpgsch(0.5);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = 0.00 * plot->nsol; 
 XMAX = 1.10 * plot->nsol;
 YMIN = 0.00 * plot->g_amp_min;
 YMAX = 1.10 * plot->g_amp_max;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCMST", 0.0, 0, "BCNSTV", 0.0, 0);
 cpgsch(0.8);
 cpgmtxt("L",1.8, 0.5, 0.0, "|g|");

 vwp = plot->g_phs_vwp;
 cpgsch(0.5);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN =  0.00 * plot->nsol; 
 XMAX =  1.10 * plot->nsol;
 YMIN = -200.0;
 YMAX =  200.0;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCNST", 0.0, 0, "BCMSTV", 0.0, 0);
 cpgsch(0.8);
 cpgmtxt("L",1.8, 0.05, 0.0, "Phase - degrees");
 cpgmtxt("B",1.8, 0.35, 0.0, "Iterations");

 vwp = plot->V_amp_vwp;
 cpgsch(0.5);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = 0.00 * plot->nsol; 
 XMAX = 1.10 * plot->nsol;   
 YMIN =  log10f(0.5 * plot->V_amp_min);
 YMAX =  log10f(3.10 * plot->V_amp_max);
 //YMIN = 0.00 * plot->V_amp_min;
 //YMAX = 1.10 * plot->V_amp_max;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCMST", 0.0, 0, "BCNSLTV2", 0.0, 0);
 cpgsch(0.8);
 cpgmtxt("L",1.8, 0.25, 0.0, "|M| -  Jy");

 vwp = plot->V_phs_vwp;
 cpgsch(0.5);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN =  0.00 * plot->nsol; 
 XMAX =  1.10 * plot->nsol;
 YMIN = -200.0;
 YMAX =  200.0;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCNST", 0.0, 0, "BCMSTV", 0.0, 0);
 cpgsch(0.8);
 cpgmtxt("L",1.8, 0.05, 0.0, "Phase - degrees");
 cpgmtxt("B",1.8, 0.35, 0.0, "Iterations");

 vwp = plot->g_cmplx_vwp;
 cpgsch(0.5);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = -1.10 * plot->g_rad;
 XMAX =  1.10 * plot->g_rad;
 YMIN = -1.10 * plot->g_rad;
 YMAX =  1.10 * plot->g_rad;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCGNMST", 0.0, 0, "BCGNVMST", 0.0, 0);
 cpgsch(0.8);
 cpgmtxt("B",1.8, 0.45, 0.0, "real(g)");
 cpgmtxt("L",1.8, 0.45, 0.0, "imag(g)");

 vwp = plot->V_cmplx_vwp;
 cpgsch(0.5);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = -1.10 * plot->V_rad;
 XMAX =  1.10 * plot->V_rad;
 YMIN = -1.10 * plot->V_rad;
 YMAX =  1.10 * plot->V_rad;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCGNMST", 0.0, 0, "BCGNMVST", 0.0, 0);
 cpgsch(0.8);
 cpgmtxt("B",1.8, 0.45, 0.0, "real(M)");
 cpgmtxt("L",1.8, 0.45, 0.0, "imag(M)");

}

void open_MSEscope( PlotParType *plot )
{
 float SIDE = VWPSIDE, SQUARE = 1.0;
 float *vwp;
 float XMIN, XMAX, YMIN, YMAX;
 int C;

 if( plot->driver == 'P' )
 { C = PSCOLOR;
   plot->MSEboard = cpgopen("/cps");
 }
 else if ( plot->driver == 'X' )
 { C = COLOR;
   plot->MSEboard = cpgopen("/xs");
 }

 cpgpap(1.68*SIDE, SQUARE);
 vwp = plot->MSE_vwp;
 cpgsch(0.84);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = -0.05 * plot->iter;
 XMAX =  1.05 * plot->iter;
 YMIN =  log10f(0.5 * plot->MSE_min);
 YMAX =  log10f(1.5 * plot->MSE_max);
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCGNST", 0.0, 0, "BCGLNST", 0.0, 0);
 cpgsch(1.2);
 cpgmtxt("B",1.8, 0.45, 0.0, "Iterations");
 cpgmtxt("L",1.8, 0.40, 0.0, "|| g(n) - g(n-1) ||");
}

void open_chisqscope( PlotParType *plot )
{
 float SIDE = VWPSIDE, SQUARE = 1.0;
 float *vwp;
 float XMIN, XMAX, YMIN, YMAX;
 int C;

 if( plot->driver == 'P' )
 { C = PSCOLOR;
   plot->chisqboard = cpgopen("/cps");
 }
 else if ( plot->driver == 'X' )
 { C = COLOR;
   plot->chisqboard = cpgopen("/xs");
 }

 cpgpap(1.68*SIDE, SQUARE);
 vwp = plot->chisq_vwp;
 cpgsch(0.84);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = -0.05 * plot->iter;
 XMAX =  1.05 * plot->iter;
 YMIN =  log10f(0.5 * plot->chisq_min);
 YMAX =  log10f(1.5 * plot->chisq_max);
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCGNST", 0.0, 0, "BCGLNST", 0.0, 0);
 cpgsch(1.2);
 cpgmtxt("B",1.8, 0.45, 0.0, "Iterations");
 cpgmtxt("L",1.8, 0.40, 0.0, "chi_squared");

}

void open_errscope( PlotParType *plot )
{
 open_MSEscope( plot );
 open_chisqscope( plot );
}

void open_SNRscope( PlotParType *plot )
{
 float SIDE = VWPSIDE, SQUARE = 1.0;
 float *vwp;
 float XMIN, XMAX, YMIN, YMAX, N;
 int C;

 N = (float)plot->antennas;

 if( plot->driver == 'P' )
 { C = PSCOLOR;
   plot->SNRboard = cpgopen("/cps");
 }
 else if ( plot->driver == 'X' )
 { C = COLOR;
   plot->SNRboard = cpgopen("/xs");
 }

 cpgpap(1.68*SIDE, SQUARE);
 vwp = plot->SNR_vwp;
 cpgsch(1.0);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = 0.5;
 XMAX = (float)(N) + 0.5;
 YMIN = 0.5;
 YMAX = (float)(N) + 0.5;
 cpgswin(XMIN, XMAX, YMAX, YMIN);
 cpgbox("BCN", 0.0, 0, "BCN", 0.0, 0);
 cpgsch(1.0);
 cpgmtxt("L",1.8, 0.45, 0.0, "Antenna #");
 cpgmtxt("B",2.2, 0.42, 0.0, "Antenna #");
}

void open_covarscope( PlotParType *plot )
{
 float SIDE = VWPSIDE, SQUARE = 1.0;
 float *vwp;
 float XMIN, XMAX, YMIN, YMAX, N;
 int C;

 N = (float)(plot->antennas + plot->redundant_baselines);

 if( plot->driver == 'P' )
 { C = PSCOLOR;
   plot->covarboard = cpgopen("/cps");
 }
 else if ( plot->driver == 'X' )
 { C = COLOR;
   plot->covarboard = cpgopen("/xs");
 }

 cpgpap(1.68*SIDE, SQUARE);
 vwp = plot->covar_vwp;
 cpgsch(1.0);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = 0.5;
 XMAX = (float)(N) + 0.5;
 YMIN = 0.5;
 YMAX = (float)(N) + 0.5;
 cpgswin(XMIN, XMAX, YMAX, YMIN);
 cpgbox("BCN", 0.0, 0, "BCN", 0.0, 0);
 cpgsch(1.0);
 cpgmtxt("L",1.7, 0.5, 0.5, "Parameter index");
 cpgmtxt("B",2.2, 0.5, 0.5, "Parameter index");
 cpgmtxt("L",1.7, 0.5, 0.505, "Parameter index");
 cpgmtxt("B",2.2, 0.5, 0.505, "Parameter index");
}

void open_Hessianscope( PlotParType *plot )
{
 float SIDE = VWPSIDE, SQUARE = 1.0;
 float *vwp;
 float XMIN, XMAX, YMIN, YMAX, N;
 int C;

 N = (float)(plot->antennas + plot->redundant_baselines);

 if( plot->driver == 'P' )
 { C = PSCOLOR;
   plot->Hessianboard = cpgopen("/cps");
 }
 else if ( plot->driver == 'X' )
 { C = COLOR;
   plot->Hessianboard = cpgopen("/xs");
 }

 cpgpap(1.68*SIDE, SQUARE);
 vwp = plot->Hessian_vwp;
 cpgsch(1.0);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = 0.5;
 XMAX = (float)(N) + 0.5;
 YMIN = 0.5;
 YMAX = (float)(N) + 0.5;
 cpgswin(XMIN, XMAX, YMAX, YMIN);
 cpgbox("BCN", 0.0, 0, "BCN", 0.0, 0);
 cpgsch(1.0);
 cpgmtxt("L",1.7, 0.5, 0.5, "Parameter index");
 cpgmtxt("L",1.7, 0.5, 0.505, "Parameter index");
 cpgmtxt("B",2.2, 0.5, 0.5, "Parameter index");
 cpgmtxt("B",2.2, 0.5, 0.505 , "Parameter index");
}

void open_gscope( PlotParType *plot)
{
 float SIDE = VWPSIDE, SQUARE = 1.0;
 int C;

 if( plot->driver == 'P' )
 { C = PSCOLOR;
   plot->G_board = cpgopen("/cps");
 }
 else if ( plot->driver == 'X' )
 { C = COLOR;
   plot->G_board = cpgopen("/xs");
 }

 cpgpap(1.68*SIDE, SQUARE);
}

void open_Mscope( PlotParType *plot )
{
 float SIDE = VWPSIDE, SQUARE = 1.0;
 int C;

 if( plot->driver == 'P' )
 { C = PSCOLOR;
   plot->M_board = cpgopen("/cps");
 }
 else if ( plot->driver == 'X' )
 { C = COLOR;
   plot->M_board = cpgopen("/xs");
 }
 cpgpap(1.68*SIDE, SQUARE);
}


void open_scalescope( PlotParType *plot )
{
 float SIDE = VWPSIDE, SQUARE = 1.0;
 float *vwp;
 float XMIN, XMAX, YMIN, YMAX;
 int G, M, C;

 if( plot->driver == 'P' )
 { C = PSCOLOR;
   G = CRIMSON;
   M = BLUE;
   plot->scale_board = cpgopen("/cps");
 }
 else if ( plot->driver == 'X' )
 { G = GScale;
   M = MScale;
   C = COLOR;
   plot->scale_board = cpgopen("/xs");
 }
 cpgpap(1.68*SIDE, SQUARE);

 vwp = plot->ampscale_vwp;
 cpgsch(0.84);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN =  0.0;
 YMAX =  1.15 * plot->scale;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCST", 0.0, 0, "BCNST", 0.0, 0);
 cpgsch(1.0);
 cpgmtxt("L",1.8, 0.35, 0.0, "Scale factor");

 vwp = plot->phoffset_vwp;
 cpgsch(0.84);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN = -200.0;
 YMAX =  200.0;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCNST", 0.0, 0, "BCMST", 0.0, 0);
 cpgsch(1.0);
 cpgmtxt("L",1.8, 0.13, 0.0, "Phase offset -  degrees");
 cpgsci(G);
 cpgmtxt("B",1.9, 0.20, 0.0, " Antenna #");
 cpgsci(C);
 cpgmtxt("B",1.9, 0.35, 0.0, " / ");
 cpgsci(M);
 cpgmtxt("B",1.9, 0.39, 0.0, "Redundant baseline # ");
 //cpgmtxt("B",1.9, 0.20, 0.0, " Antenna # / Redundant baseline # ");
}

void open_gcmplxscope( PlotParType *plot )
{
 float SIDE = VWPSIDE, SQUARE = 1.0;
 float *vwp;
 float XMIN, XMAX, YMIN, YMAX;
 int C;

 if( plot->driver == 'P' )
 { C = PSCOLOR;
   plot->gcmplxboard = cpgopen("/cps");
 }
 else if ( plot->driver == 'X' )
 { C = COLOR;
   plot->gcmplxboard = cpgopen("/xs");
 }

 cpgpap(1.68*SIDE, SQUARE);
 vwp = plot->MSE_vwp;
 cpgsch(0.84);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = -1.10 * plot->g_rad;
 XMAX =  1.10 * plot->g_rad;
 YMIN = -1.10 * plot->g_rad;
 YMAX =  1.10 * plot->g_rad;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCGNMST", 0.0, 0, "BCGNMST", 0.0, 0);
 cpgsch(1.2);
 cpgmtxt("B",1.8, 0.45, 0.0, "real(g)");
 cpgmtxt("L",1.8, 0.45, 0.0, "imag(g)");

}

void open_Mcmplxscope( PlotParType *plot )
{
float SIDE = VWPSIDE, SQUARE = 1.0;
 float *vwp;
 float XMIN, XMAX, YMIN, YMAX;
 int C;

 if( plot->driver == 'P' )
 { C = PSCOLOR;
   plot->Mcmplxboard = cpgopen("/cps");
 }
 else if ( plot->driver == 'X' )
 { C = COLOR;
   plot->Mcmplxboard = cpgopen("/xs");
 }

 cpgpap(1.68*SIDE, SQUARE);
 vwp = plot->MSE_vwp;
 cpgsch(0.84);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = -1.10 * plot->V_rad;
 XMAX =  1.10 * plot->V_rad;
 YMIN = -1.10 * plot->V_rad;
 YMAX =  1.10 * plot->V_rad;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCGNMST", 0.0, 0, "BCGNMST", 0.0, 0);
 cpgsch(1.2);
 cpgmtxt("B",1.8, 0.45, 0.0, "real(g)");
 cpgmtxt("L",1.8, 0.45, 0.0, "imag(g)");

}

void open_scopes( PlotParType *plot )
{
 open_trajscope( plot );
 open_errscope( plot );
 //open_SNRscope( plot );
 open_covarscope( plot );
 open_gscope( plot );
 open_Mscope( plot );
 open_scalescope( plot );
}

void parse_solfile( PlotParType *plot, char *solfile )
{
 FILE *fsol; 
 char str[80];
 char *token;
 float utc_min;
 char line[16384];
 long size = 0;

 if ( (fsol = fopen(solfile, "r")) == NULL )
 { fprintf(ferr, "%s File \"%s\" not found.", err, solfile );
   fprintf(ferr, "\n%s Quit!\n", err);
   exit(-1);
 }
 fprintf(finfo, "%s Found %s", info, solfile );
 
 if(fsol)
  {fseek (fsol, 0, SEEK_END);
   size = ftell(fsol);
   if(size == 0)
   { fclose(fsol);
     fprintf(ferr, "\n%s %s is empty\n", err, solfile);
     exit(-1);
   }
   else rewind(fsol);
  }

  do { fgets ( str, sizeof(str), fsol );
  }while( ! (strstr(str, "## Column 1")) );

  fgets ( str, sizeof(str), fsol );
  token = strtok(str, "next");
  token = strtok(NULL, "next ");
  sscanf(token, "%d", &(plot->antennas) );
  plot->antennas /= 2;

  fgets ( str, sizeof(str), fsol );

  fgets ( str, sizeof(str), fsol );
  token = strtok(str, "remaining");
  token = strtok(NULL, "remaining ");
  sscanf(token, "%d", &(plot->redundant_baselines) );
  plot->redundant_baselines /= 2;

  do { fgets ( str, sizeof(str), fsol );
  }while( ! (strstr(str, "## Mean System Temperature")) );
  
  fgets ( str, sizeof(str), fsol );

  plot->nsol = 0;
  while(!feof(fsol))
  { fgets( line, sizeof(line), fsol);
    sscanf(line, "%f ", &utc_min);
    plot->nsol++;
  }
  plot->nsol--;
  fclose(fsol);
}

void read_solutions(PlotParType *plot, char *solfile)
{ 
 FILE *fsol = fopen(solfile, "r");
 FILE *ginit = fopen("ant-init.info", "r");
 FILE *Vinit = fopen("vis.model", "r");
 char str[80];
 char *token;
 float utc_min;
 int sol, rbl, ant;
 char line[16384];
 float g_amp_min = 1.0, g_amp_max = 1.0;
 float V_amp_min = 1.0, V_amp_max = 1.0;
 int sno;
 float tmp;
 float gscale = 0.0;
 float Mscale = 0.0;
 char antname[5];

  do { fgets ( str, sizeof(str), fsol );
  }while( ! (strstr(str, "## Mean System Temperature")) );
  fgets ( str, sizeof(str), fsol );

  for(sol = 0 ; sol < plot->nsol; sol++)
  { fgets( line, sizeof(line), fsol);

    for(ant = 0 ; ant < plot->antennas ; ant++) 
    { if(ant == 0) 
      { sscanf(line, "%f", &utc_min);
	token = strtok(line, "\t");
      }
      
      token = strtok(NULL, "\t");
      sscanf(token, "%lf", &(plot->gain[sol][ant].r) );
      token = strtok(NULL, "\t");
      sscanf(token, "%lf", &(plot->gain[sol][ant].i) );

      plot->g_amp[sol][ant] = z_abs( plot->gain[sol][ant] );
      plot->g_phs[sol][ant] = rad2deg( z_arg( plot->gain[sol][ant]) );
      proper_angle( (double*)&(plot->g_phs[sol][ant]) );

      g_amp_min = minf( g_amp_min, plot->g_amp[sol][ant] );
      g_amp_max = maxf( g_amp_max, plot->g_amp[sol][ant] );
    }

    for(rbl = 0 ; rbl < plot->redundant_baselines ; rbl++) 
    { token = strtok(NULL, "\t");
      sscanf(token, "%lf", &(plot->vis[sol][rbl].r) );
      token = strtok(NULL, "\t");
      sscanf(token, "%lf", &(plot->vis[sol][rbl].i) );

      plot->V_amp[sol][rbl] = z_abs( plot->vis[sol][rbl] );
      plot->V_phs[sol][rbl] = rad2deg( z_arg( plot->vis[sol][rbl]) );
      proper_angle( (double*)&(plot->V_phs[sol][rbl]) );

      V_amp_min = minf( V_amp_min, plot->V_amp[sol][rbl] );
      V_amp_max = maxf( V_amp_max, plot->V_amp[sol][rbl] );
    }
  }

  plot->g_amp_min = g_amp_min;
  plot->g_amp_max = g_amp_max;

  plot->V_amp_min = V_amp_min;
  plot->V_amp_max = V_amp_max;

  fclose(fsol);

  /*****************************/
  fgets ( str, sizeof(str), ginit );
  fgets ( str, sizeof(str), ginit );

  for(ant = 0 ; ant < plot->antennas ; ant++)
  { fgets ( str, sizeof(str), ginit );
    sscanf( str, "%d\t%s\t%lf\t%lf\t%f\t%f", &sno, antname, &(plot->g_true[ant].r), &(plot->g_true[ant].i), &(plot->g_true_amp[ant]), &(plot->g_true_phs[ant]) );
    proper_angle( (double*)&(plot->g_true_phs[ant]) );
    gscale = maxf(gscale, plot->g_amp[sol-1][ant] / plot->g_true_amp[ant] );
    plot->g_rad = maxf( plot->g_rad, maxf( plot->g_true_amp[ant], plot->g_amp[sol-1][ant] ) );
  }

  fclose(ginit);
  /*****************************/
  fgets ( str, sizeof(str), Vinit );
  for(rbl = 0 ; rbl < plot->redundant_baselines ; rbl++)
  { fgets ( str, sizeof(str), Vinit );
      sscanf( str, "%d\t%f\t%lf\t%f\t%lf\t%f\t%f\t%f", &sno, &tmp, &(plot->V_true[rbl].r), &tmp, &(plot->V_true[rbl].i), &tmp, &(plot->V_true_amp[rbl]), &(plot->V_true_phs[rbl]));
 fprintf(finfo, "%s RBL : %d Real : %f Imag : %f\n", info, rbl+1, plot->V_true[rbl].r, plot->V_true[rbl].i);
    proper_angle( (double*)&(plot->V_true_phs[rbl]) );
    Mscale = maxf(Mscale, plot->V_true_amp[rbl] / plot->V_amp[sol-1][rbl] );
    plot->V_rad = maxf( plot->V_rad, maxf( plot->V_true_amp[rbl], plot->V_amp[sol-1][rbl] ) );
  }
  fclose(Vinit);
  /*****************************/
  /*
  unwrap_phase( plot->V_phs[plot->nsol-1], plot->redundant_baselines );
  unwrap_phase( plot->V_true_phs, plot->redundant_baselines );
  */
  plot->scale = maxf(gscale, Mscale);
}

void parse_MSE_file ( PlotParType *plot )
{
  char str[80];
  float val, min, max;
  FILE *fMSE;
  long size = 0;

 if ( (fMSE = fopen("MSE.dat", "r")) == NULL )
 { fprintf(ferr, "\n%s File \"MSE.dat\" not found.", err);
   fprintf(ferr, "\n%s Quit!\n", err);
   exit(-1);
 }
 fprintf(finfo, "\n%s Found MSE.dat", info );

 if(fMSE)
  {fseek (fMSE, 0, SEEK_END);
   size = ftell(fMSE);
   if(size == 0)
   { fclose(fMSE);
     fprintf(ferr, "\n%s %s is empty\n", err, "MSE.dat");
     exit(-1);
   }
   else rewind(fMSE);
  }

  fgets( str, sizeof(str), fMSE );
  sscanf( str, "%d %f", &(plot->iter), &val);
  min = max = val;

  while( !feof(fMSE))
  { fgets( str, sizeof(str), fMSE );
      sscanf( str, "%d %f", &(plot->iter), &val);
    min = minf(min, val);
    max = maxf(max, val);
    if ( val <= 0.000001 ) val = 0.000001;
  }

  if( min <= 0.000001 ) min = 0.000001;
  //plot->iter = iter;
  plot->MSE_min = min;
  plot->MSE_max = max;

  plot->MSE = (float*)calloc( plot->iter, sizeof(float) );
  fclose(fMSE);

  plot->MSE_opened = 1;

}


void parse_chisq_file ( PlotParType *plot )
{
  int iter = 0;
  char str[80];
  float val, min, max;
  FILE *fchisq;
  long size = 0;

 if ( (fchisq = fopen("chisq.dat", "r")) == NULL )
 { fprintf(ferr, "\n%s File \"chisq.dat\" not found.", err);
   fprintf(ferr, "\n%s Quit!\n", err);
   exit(-1);
 }
 fprintf(finfo, "\n%s Found chisq.dat", info );
  
 if(fchisq)
  {fseek (fchisq, 0, SEEK_END);
   size = ftell(fchisq);
   if(size == 0)
   { fclose(fchisq);
     fprintf(ferr, "\n%s %s is empty\n", err, "chisq.dat");
     exit(-1);
   }
   else rewind(fchisq);
  }

  fgets( str, sizeof(str), fchisq );
  sscanf( str, "%d %f", &(plot->iter), &val);
  min = max = val;

  while( !feof(fchisq))
  { fgets( str, sizeof(str), fchisq );
    sscanf( str, "%d %f", &iter, &val);
    min = minf(min, val);
    max = maxf(max, val);
  }

  plot->iter = iter;
  plot->chisq_min = min;
  plot->chisq_max = max;

  plot->chisq = (float*)calloc( plot->iter, sizeof(float) );
  fclose(fchisq);

  plot->chisq_opened = 1;

}

void read_MSE( PlotParType *plot )
{
  FILE *fMSE = fopen("MSE.dat", "r");
  int  i, iter; 
  char str[80];

  for(iter = 0; iter < plot->iter ; iter++)
  { fgets( str, sizeof(str), fMSE );
    sscanf( str, "%d %f", &i, &( plot->MSE[iter] ) );
  }

  fclose(fMSE);
}


void read_chisq( PlotParType *plot )
{
  FILE *fchisq = fopen("chisq.dat", "r");
  int  i, iter; 
  char str[80];

  for(iter = 0; iter < plot->iter ; iter++)
  { fgets( str, sizeof(str), fchisq );
    sscanf( str, "%d %f", &i, &( plot->chisq[iter] ) );
  }

  fclose(fchisq);
}

void read_SNR_matrix( PlotParType *plot )
{
  FILE *fSNR;
  char line[8192];
  char *token;
  int row, col;
  int N = plot->antennas;
  
  plot->SNR_opened = 1;
  plot->SNRmax = 0.0;

  plot->SNR = (float*)calloc( N * N, sizeof(float) );

  if ( (fSNR = fopen("SNR.matx", "r")) == NULL )
  { fprintf(ferr, "%s File \"SNR.matx\" not found.", err );
    fprintf(ferr, "\n%s Quit!\n", err);
    exit(-1);
  }
  else fprintf(finfo, "\n%s Found SNR.matx", info );

  for( row = 0; row < N ; row++)
  { col = 0;
    fgets( line, sizeof(line), fSNR );
    token = strtok( line, " ");
    sscanf(token, "%f", &(plot->SNR[N*row + col]) );
    plot->SNRmax = maxf( plot->SNRmax, plot->SNR[N*row + col] );
    for(col = 1; col < N; col++)
    { token = strtok(NULL, " ");
      sscanf(token, "%f", &(plot->SNR[N*row + col]) );
      plot->SNRmax = maxf( plot->SNRmax, plot->SNR[N*row + col] );
    }
  }    
  fclose(fSNR);
}

void read_covar_matrix( PlotParType *plot )
{
  FILE *fcov;
  char line[8192];
  char *token;
  int row, col;
  int N = plot->antennas + plot->redundant_baselines;
  float real, imag;
  float eps = 0.0;

  plot->covar_opened = 1;
  plot->covarmax = -10000.0;
  plot->covarmin = 10000.0;
  plot->covar = (float*)calloc( N * N, sizeof(float) );
  plot->g_err = (CmplxType*)calloc( plot->antennas, sizeof(CmplxType) );
  plot->M_err = (CmplxType*)calloc( plot->redundant_baselines, sizeof(CmplxType) );
  
  if ( (fcov = fopen("cov.matx", "r")) == NULL )
  { fprintf(ferr, "\n%s File \"cov.matx\" not found.", err );
    fprintf(ferr, "\n%s Quit!\n", err);
    exit(-1);
  }
  else fprintf(finfo, "\n%s Found cov.matx", info );
  
  /*
  if ( (fcov = fopen("alpha.mtx", "r")) == NULL )
  { fprintf(ferr, "%s File \"alpha.mtx\" not found.", err );
    fprintf(ferr, "\n%s Quit!\n", err);
    exit(-1);
  }
  else fprintf(finfo, "\n%s Found alpha.mtx", info );
  */
  for( row = 0; row < N ; row++)
  { col = 0;
    fgets( line, sizeof(line), fcov );
    token = strtok( line, " ");
    sscanf(token, "%f ", &real);
    token = strtok( NULL, " ");
    sscanf(token, "%f ", &imag);
    if( row == col ) plot->g_err[row] = ret_z(real, imag);
    plot->covar[ N*row + col ] = 10*log10f(hypotf(real, imag));
    plot->covarmax = maxf( plot->covarmax, plot->covar[N*row + col] );
    plot->covarmin = minf( plot->covarmax, plot->covar[N*row + col] );
    for(col = 1; col < N; col++)
    { token = strtok( NULL, " ");
      sscanf(token, "%f ", &real);
      token = strtok( NULL, " ");
      sscanf(token, "%f ", &imag);
      if( (row < plot->antennas) && (row == col) ) plot->g_err[row] = ret_z(real, imag);
      if( (row >= plot->antennas) && (row == col) ) plot->M_err[row - plot->antennas] = ret_z(real, imag);
      plot->covar[ N*row + col ] = 10*log10f(hypotf(real, imag));
      plot->covarmax = maxf( plot->covarmax, plot->covar[N*row + col] );
      plot->covarmin = minf( plot->covarmin, plot->covar[N*row + col] );
    }
  }    
  fclose(fcov);
}


void read_CRB_matrix( PlotParType *plot )
{
  FILE *fCRB;
  char line[8192];
  char *token;
  int row, col;
  int N = plot->antennas + plot->redundant_baselines;
  float real, imag;

  plot->covar_opened = 1;
  plot->covarmax = 0.0;

  plot->covar = (float*)calloc( N * N, sizeof(float) );
  plot->g_err = (CmplxType*)calloc( plot->antennas, sizeof(CmplxType) );
  plot->M_err = (CmplxType*)calloc( plot->redundant_baselines, sizeof(CmplxType) );
  
  if ( (fCRB = fopen("CRB.matx", "r")) == NULL )
  { fprintf(ferr, "\n%s File \"CRB.matx\" not found.", err );
    fprintf(ferr, "\n%s Quit!\n", err);
    exit(-1);
  }
  else fprintf(finfo, "\n%s Found CRB.matx", info );
  
  /*
  if ( (fCRB = fopen("alpha.mtx", "r")) == NULL )
  { fprintf(ferr, "%s File \"alpha.mtx\" not found.", err );
    fprintf(ferr, "\n%s Quit!\n", err);
    exit(-1);
  }
  else fprintf(finfo, "\n%s Found alpha.mtx", info );
  */
  for( row = 0; row < N ; row++)
  { col = 0;
    fgets( line, sizeof(line), fCRB );
    token = strtok( line, " ");
    sscanf(token, "%f ", &real);
    token = strtok( NULL, " ");
    sscanf(token, "%f ", &imag);
    if( row == col ) plot->g_err[row] = ret_z(real, imag);
    plot->covar[ N*row + col ] = hypotf(real, imag);
    plot->covarmax = maxf( plot->covarmax, plot->covar[N*row + col] );
    for(col = 1; col < N; col++)
    { token = strtok( NULL, " ");
      sscanf(token, "%f ", &real);
      token = strtok( NULL, " ");
      sscanf(token, "%f ", &imag);
      if( (row < plot->antennas) && (row == col) ) plot->g_err[row] = ret_z(real, imag);
      if( (row >= plot->antennas) && (row == col) ) plot->M_err[row - plot->antennas] = ret_z(real, imag);
      plot->covar[ N*row + col ] = hypotf(real, imag);
      plot->covarmax = maxf( plot->covarmax, plot->covar[N*row + col] );
    }
  }    
  fclose(fCRB);

}


void read_Hessian_matrix( PlotParType *plot )
{
  FILE *fHes;
  char line[8192];
  char *token;
  int row, col;
  int N = plot->antennas + plot->redundant_baselines;
  float real, imag;
  float eps = 0.0;

  plot->Hessian_opened = 1;
  plot->Hessianmax = 0.0;

  plot->Hessian = (float*)calloc( N * N, sizeof(float) );
  
  if ( (fHes = fopen("Hessian.matx", "r")) == NULL )
      //if ( (fHes = fopen("Fisher.matx", "r")) == NULL )
  { fprintf(ferr, "\n%s File \"Hessian.matx\" not found.", err );
    fprintf(ferr, "\n%s Quit!\n", err);
    exit(-1);
  }
  else fprintf(finfo, "\n%s Found Hessian.matx", info );
  
  for( row = 0; row < N ; row++)
  { col = 0;
    fgets( line, sizeof(line), fHes );
    token = strtok( line, " ");
    sscanf(token, "%f ", &real);
    token = strtok( NULL, " ");
    sscanf(token, "%f ", &imag);
    plot->Hessian[ N*row + col ] = 10*log10f( hypotf(real, imag) + eps );
    plot->Hessianmax = maxf( plot->Hessianmax, plot->Hessian[N*row + col] );
    for(col = 1; col < N; col++)
    { token = strtok( NULL, " ");
      sscanf(token, "%f ", &real);
      token = strtok( NULL, " ");
      sscanf(token, "%f ", &imag);
      plot->Hessian[ N*row + col ] = 10*log10f( hypotf(real, imag) + eps );
      plot->Hessianmax = maxf( plot->Hessianmax, plot->Hessian[N*row + col] );
    }
  }    
  fclose(fHes);
}


void read_Fisher_matrix( PlotParType *plot )
{
  FILE *fFisher;
  char line[8192];
  char *token;
  int row, col;
  int N = plot->antennas + plot->redundant_baselines;
  float real, imag;
  float eps = 1e-9;

  plot->Hessian_opened = 1;
  plot->Hessianmax = 0.0;

  plot->Hessian = (float*)calloc( N * N, sizeof(float) );
  
  if ( (fFisher = fopen("Fisher.matx", "r")) == NULL )
  { fprintf(ferr, "\n%s File \"Fisher.matx\" not found.", err );
    fprintf(ferr, "\n%s Quit!\n", err);
    exit(-1);
  }
  else fprintf(finfo, "\n%s Found Fisher.matx", info );
  
  for( row = 0; row < N ; row++)
  { col = 0;
    fgets( line, sizeof(line), fFisher );
    token = strtok( line, " ");
    sscanf(token, "%f ", &real);
    token = strtok( NULL, " ");
    sscanf(token, "%f ", &imag);
    plot->Hessian[ N*row + col ] = 10*log10f( hypotf(real, imag) + eps );
    plot->Hessianmax = maxf( plot->Hessianmax, plot->Hessian[N*row + col] );
    for(col = 1; col < N; col++)
    { token = strtok( NULL, " ");
      sscanf(token, "%f ", &real);
      token = strtok( NULL, " ");
      sscanf(token, "%f ", &imag);
      plot->Hessian[ N*row + col ] = 10*log10f( hypotf(real, imag) + eps );
      plot->Hessianmax = maxf( plot->Hessianmax, plot->Hessian[N*row + col] );
    }
  }    
  fclose(fFisher);
}

void plot_gaincompare( PlotParType *plot )
{
 int n = plot->nsol;
 int ant;
 int symbol = -3;
 float *vwp;
 float XMIN, XMAX, YMIN, YMAX;
 int T, E, C, LC;

 T = True;
 E = Estimated;
 C = COLOR;
 LC = MINIMAL_GREY;

 if( plot->driver == 'P' )
 { T = CRIMSON;
   E = BLUE;
   C = PSCOLOR;
   LC = MEDIUM_GREY;
 }

 cpgslct(plot->gcmplxboard);

 /*Complex plane plot */
 vwp = plot->MSE_vwp;
 cpgsch(0.84);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = -1.10 * plot->g_rad;
 XMAX =  1.10 * plot->g_rad;
 YMIN = -1.10 * plot->g_rad;
 YMAX =  1.10 * plot->g_rad;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCGNMST", 0.0, 0, "BCGNMST", 0.0, 0);

 cpgsch(1.2);
 for(ant = 0; ant < plot->antennas ; ant++)
 { cpgsci(E);
   cpgpt1(plot->gain[n-1][ant].r, plot->gain[n-1][ant].i, symbol);
   cpgsci(T);
   cpgpt1(plot->g_true[ant].r, plot->g_true[ant].i, symbol);
 }

 /*Legend*/
 cpgsch(1.2);
 cpgsci(T);
 cpgmtxt("T",-1.8, 0.75, 0.0, "True");
 cpgsci(E);
 cpgmtxt("T",-3.0, 0.75, 0.0, "Estimated");

 cpgclos();
}


void plot_viscompare( PlotParType *plot )
{
 int n = plot->nsol;
 int rbl;
 int symbol = -3;
 float *vwp;
 float XMIN, XMAX, YMIN, YMAX;
 int T, E, C, LC;

 T = True;
 E = Estimated;
 C = COLOR;
 LC = MINIMAL_GREY;

 if( plot->driver == 'P' )
 { T = CRIMSON;
   E = BLUE;
   C = PSCOLOR;
   LC = MEDIUM_GREY;
 }

 cpgslct(plot->Mcmplxboard);
 /*Complex plane plot */
 vwp = plot->MSE_vwp;
 cpgsch(0.84);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = -1.10 * plot->V_rad;
 XMAX =  1.10 * plot->V_rad;
 YMIN = -1.10 * plot->V_rad;
 YMAX =  1.10 * plot->V_rad;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCGNMST", 0.0, 0, "BCGNMST", 0.0, 0);

 cpgsch(1.2);
 for(rbl = 0; rbl < plot->redundant_baselines ; rbl++)
 { cpgsci(E);
   cpgpt1(plot->vis[n-1][rbl].r, plot->vis[n-1][rbl].i, symbol);
   cpgsci(T);
   cpgpt1(plot->V_true[rbl].r, plot->V_true[rbl].i, symbol);
 }

 /*Legend*/
 cpgsch(1.2);
 cpgsci(T);
 cpgmtxt("T",-1.8, 0.75, 0.0, "True");
 cpgsci(E);
 cpgmtxt("T",-3.0, 0.75, 0.0, "Estimated");

 cpgclos();

}

void plot_itergain(PlotParType *plot )
{
 int n, ant;
 int symbol = -3;
 float *vwp;
 float XMIN, XMAX, YMIN, YMAX;
 float x[plot->antennas][plot->nsol];
 float amp[plot->antennas][plot->nsol];
 float phs[plot->antennas][plot->nsol];
 int T, E, C, LC;

 T = True;
 E = Estimated;
 C = COLOR;
 LC = MINIMAL_GREY;

 if( plot->driver == 'P' )
 { T = CRIMSON;
   E = BLUE;
   C = PSCOLOR;
   LC = MEDIUM_GREY;
 }

 cpgslct(plot->dashboard);

 /* Get amplitudes and phases in form for lineplot */
 for(n = 0; n < plot->nsol; n++)
 { for(ant = 0; ant < plot->antennas ; ant++)
   { x[ant][n] = (float)(n+1);
     amp[ant][n] = plot->g_amp[n][ant];
     phs[ant][n] = plot->g_phs[n][ant];
   }
 }

 /* Lineplot */
 for(ant = 0; ant < plot->antennas ; ant++)
 { vwp = plot->g_amp_vwp;
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   cpgsch(0.5);
   cpgsci(C);
   XMIN = 0.00 * plot->nsol; 
   XMAX = 1.10 * plot->nsol;
   YMIN = 0.00 * plot->g_amp_min;
   YMAX = 1.10 * plot->g_amp_max;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BGCMST", 0.0, 0, "BCGVNST", 0.0, 0);
   cpgsci(LC);

   cpgline(plot->nsol, x[ant], amp[ant]);

   vwp = plot->g_phs_vwp;
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   cpgsch(0.5);
   cpgsci(C);
   XMIN =  0.00 * plot->nsol; 
   XMAX =  1.10 * plot->nsol;
   YMIN = -200.0;
   YMAX =  200.0;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BGCNST", 0.0, 0, "BCGMVST", 0.0, 0);
   cpgsci(LC);

   cpgline(plot->nsol, x[ant], phs[ant]);
 }

 /*Symbol plot */
 for(n = 0; n < plot->nsol; n++)
 { vwp = plot->g_amp_vwp;
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   cpgsch(0.5);
   cpgsci(C);
   XMIN =  0.00 * plot->nsol; 
   XMAX =  1.10 * plot->nsol;
   YMIN =  0.00 * plot->g_amp_min;
   YMAX =  1.10 * plot->g_amp_max;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BGCMST", 0.0, 0, "BCGNSVT", 0.0, 0);
   
   for(ant = 0; ant < plot->antennas ; ant++)
   { cpgsci((C+ant) % 15 + 1);
     cpgpt1((float)(n+1), plot->g_amp[n][ant], symbol);
   }

   vwp = plot->g_phs_vwp;
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   cpgsch(0.5);
   cpgsci(C);
   XMIN =  0.00 * plot->nsol; 
   XMAX =  1.10 * plot->nsol;
   YMIN = -200.0;
   YMAX =  200.0;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCGNST", 0.0, 0, "BCGMSTV", 0.0, 0);

   for(ant = 0; ant < plot->antennas ; ant++)
   { cpgsci((C+ant) % 15 + 1);
     cpgpt1((float)(n+1), plot->g_phs[n][ant], symbol);
   }

 }
 /*Complex plane plot */
 vwp = plot->g_cmplx_vwp;
 cpgsch(0.5);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = -1.10 * plot->g_rad;
 XMAX =  1.10 * plot->g_rad;
 YMIN = -1.10 * plot->g_rad;
 YMAX =  1.10 * plot->g_rad;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCGNMST", 0.0, 0, "BCGNMSTV", 0.0, 0);

 for(ant = 0; ant < plot->antennas ; ant++)
 { cpgsci(E);
   cpgpt1(plot->gain[n-1][ant].r, plot->gain[n-1][ant].i, symbol);
   cpgsci(T);
   cpgpt1(plot->g_true[ant].r, plot->g_true[ant].i, symbol);
 }

 /*Legend*/
 cpgsch(0.6);
 cpgsci(T);
 cpgmtxt("T",-1.8, 0.75, 0.0, "True");
 cpgsci(E);
 cpgmtxt("T",-3.0, 0.75, 0.0, "Estimated");

}

void plot_itervis(PlotParType *plot )
{
 int n, rbl;
 int symbol = -3;
 float *vwp;
 float XMIN, XMAX, YMIN, YMAX;
 float x[plot->redundant_baselines][plot->nsol];
 float amp[plot->redundant_baselines][plot->nsol];
 float phs[plot->redundant_baselines][plot->nsol];
 int T, E, C, LC;

 T = True;
 E = Estimated;
 C = COLOR;
 LC = MINIMAL_GREY;

 if( plot->driver == 'P' )
 { T = CRIMSON;
   E = BLUE;
   C = PSCOLOR;
   LC = MEDIUM_GREY;
 }

 cpgslct(plot->dashboard);

 /* Get amplitudes and phases in form for lineplot */
 for(n = 0; n < plot->nsol; n++)
 { for(rbl = 0; rbl < plot->redundant_baselines ; rbl++)
   { x[rbl][n] = (float)(n+1);
     amp[rbl][n] = log10f(plot->V_amp[n][rbl]);
     phs[rbl][n] = plot->V_phs[n][rbl];
   }
 }
 /*Lineplot*/
 for(rbl = 0; rbl < plot->redundant_baselines ; rbl++)
 { vwp = plot->V_amp_vwp;
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   cpgsch(0.5);
   cpgsci(C);
   XMIN =  0.00 * plot->nsol; 
   XMAX =  1.10 * plot->nsol;
   //YMIN =  0.00 * plot->V_amp_min;
   YMIN =  log10f(0.5 * plot->V_amp_min);
   YMAX =  log10f(3.10 * plot->V_amp_max);
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BGCMST", 0.0, 0, "BCGLNSTV2", 0.0, 0);
   cpgsci(LC);

   cpgline(plot->nsol, x[rbl], amp[rbl]);

   vwp = plot->V_phs_vwp;
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   cpgsch(0.5);
   cpgsci(C);
   XMIN =  0.00 * plot->nsol; 
   XMAX =  1.10 * plot->nsol;
   YMIN = -200.0;
   YMAX =  200.0;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BGCNST", 0.0, 0, "BCGMSTV", 0.0, 0);
   cpgsci(LC);

   cpgline(plot->nsol, x[rbl], phs[rbl]);
 }

 /*Symbol plot*/
 for(n = 0; n < plot->nsol; n++)
 { vwp = plot->V_amp_vwp;
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   cpgsch(0.5);
   cpgsci(C);
   XMIN = 0.00 * plot->nsol; 
   XMAX = 1.10 * plot->nsol;
   //YMIN = 0.00 * plot->V_amp_min;
   //YMAX = 1.10 * plot->V_amp_max;
   YMIN =  log10f(0.5 * plot->V_amp_min);
   YMAX =  log10f(3.10 * plot->V_amp_max);
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCGMSTV", 0.0, 0, "BCGNLSTV2", 0.0, 0);
   cpgsci(C);
   
   for(rbl = 0; rbl < plot->redundant_baselines ; rbl++)
   { cpgsci((C+rbl) % 15 + 1);
	   //cpgsci(E);
       cpgpt1((float)(n+1), amp[rbl][n], symbol);
   }

   vwp = plot->V_phs_vwp;
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   cpgsch(0.5);
   cpgsci(C);
   XMIN =  0.00 * plot->nsol; 
   XMAX =  1.10 * plot->nsol;
   YMIN = -200.0;
   YMAX =  200.0;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCGNST", 0.0, 0, "BCGMSTV", 0.0, 0);
   cpgsci(C);

   for(rbl = 0; rbl < plot->redundant_baselines ; rbl++)
       { cpgsci((C+rbl) % 15 + 1);
	   //cpgsci(E);
     cpgpt1((float)(n+1), plot->V_phs[n][rbl], symbol);
   }

 }

 /*Complex plane plot */
 vwp = plot->V_cmplx_vwp;
 cpgsch(0.5);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = -1.10 * plot->V_rad;
 XMAX =  1.10 * plot->V_rad;
 YMIN = -1.10 * plot->V_rad;
 YMAX =  1.10 * plot->V_rad;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCGNMST", 0.0, 0, "BCGNMSTV", 0.0, 0);

 for(rbl = 0; rbl < plot->redundant_baselines ; rbl++)
 { fprintf(finfo, "%s RBL : %d Real : %f Imag : %f\n", info, rbl+1, plot->V_true[rbl].r, plot->V_true[rbl].i);
   cpgsci(E);
   cpgpt1(plot->vis[n-1][rbl].r, plot->vis[n-1][rbl].i, symbol);
   cpgsci(T);
   cpgpt1(plot->V_true[rbl].r, plot->V_true[rbl].i, symbol);
 }

 /*Legend*/
 cpgsch(0.6);
 cpgsci(T);
 cpgmtxt("T",-1.8, 0.75, 0.0, "True");
 cpgsci(E);
 cpgmtxt("T",-3.0, 0.75, 0.0, "Estimated");

 cpgclos();

}

void plot_MSE( PlotParType *plot )
{
 int iter;
 float XMIN, XMAX, YMIN, YMAX;
 float *vwp;
 float x[plot->iter];
 int symbol = -5;
 int ERROR, C, LC;

 ERROR = YELLOW;
 C = COLOR;
 LC = MINIMAL_GREY;

 if( plot->driver == 'P' )
 { ERROR = CRIMSON;
   C = PSCOLOR;
   LC = MEDIUM_GREY;
 }

 for(iter = 0; iter < plot->iter ; iter++)
 { x[iter] = (float)iter+1;
   plot->MSE[iter] = log10f(plot->MSE[iter]);
 }
   
 cpgslct(plot->MSEboard);

 cpgsci(LC);
 cpgline(plot->iter, x, plot->MSE);

 vwp = plot->MSE_vwp;
 cpgsch(0.84);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = -0.05 * plot->iter;
 XMAX =  1.05 * plot->iter;
 YMIN =  log10f(0.5 * plot->MSE_min);
 YMAX =  log10f(1.5 * plot->MSE_max);
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCGNST", 0.0, 0, "BCGLNST", 0.0, 0);
 
 for(iter = 0; iter < plot->iter ; iter++)
 { cpgsci(ERROR);
   cpgpt1((float)x[iter], plot->MSE[iter], symbol);
 }
 
 cpgclos();
}

void plot_chisq( PlotParType *plot )
{
 int iter;
 float XMIN, XMAX, YMIN, YMAX;
 float *vwp;
 float x[plot->iter];
 int symbol = -5;
 int ERROR, C, LC;

 ERROR = YELLOW;
 C = COLOR;
 LC = MINIMAL_GREY;
 
 if( plot->driver == 'P' )
 { ERROR = CRIMSON;
   C = PSCOLOR; 
   LC = MEDIUM_GREY;
 }
 
 for(iter = 0; iter < plot->iter ; iter++)
 { x[iter] = (float)iter+1;
   plot->chisq[iter] = log10f(plot->chisq[iter]);
 }
 
 cpgslct(plot->chisqboard);

 cpgsci(LC);
 cpgline(plot->iter, x, plot->chisq);
 
 vwp = plot->chisq_vwp;
 cpgsch(0.84);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN = -0.05 * plot->iter;
 XMAX =  1.05 * plot->iter;
 YMIN =  log10f(0.5 * plot->chisq_min);
 YMAX =  log10f(1.5 * plot->chisq_max);
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCGNST", 0.0, 0, "BCGLNST", 0.0, 0);
 
 for(iter = 0; iter < plot->iter ; iter++)
 { cpgsci(ERROR);
   cpgpt1((float)(iter+1), plot->chisq[iter], symbol);
 }
  
 cpgclos();
}

void plot_SNR_matrix( PlotParType *plot )
{ int i, N = plot->antennas;
  float TR[6] = { 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
  float min = 0.0;
  float max = plot->SNRmax;
  float XMIN, XMAX, YMIN, YMAX;
  float *vwp;
  float R, G, B;
  int C;
 
  if( plot->driver == 'P' )  
  C = PSCOLOR;
  else  
  C = COLOR;

  cpgslct( plot->SNRboard );

  for(i = 0 ; i < MAXCOL ; i++)
  { R = 0.75*(float)(i)/(float)(MAXCOL) + 0.25;
    G = maxf(0.0, 2.0*(float)(i-MAXCOL/2)/(float)(MAXCOL+1));
    B = 0.2 + 0.4*(float)(MAXCOL-i)/(float)(MAXCOL);
    cpgscr(16+i, R, G, B);
  }

  vwp = plot->SNR_vwp;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  cpgsci(C);
  XMIN = 0.5;
  XMAX = (float)(N) + 0.5;
  YMIN = 0.5;
  YMAX = (float)(N) + 0.5;
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  cpgbox("BCN", 0.0, 0, "BCN", 0.0, 0);
  cpgscir(16,15+MAXCOL);  
  cpgsitf(2);
  cpgimag( plot->SNR, N, N, 1, N, 1, N, min, max, TR );
  cpgwedg("RI", 1.5, 2.5, min, max, "");

  cpgclos();
}

void plot_covar_matrix( PlotParType *plot )
{ int i, N = plot->antennas + plot->redundant_baselines;
  float TR[6] = { 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
  float min = plot->covarmin;
  float max = plot->covarmax;
  float XMIN, XMAX, YMIN, YMAX;
  float *vwp;
  float R, G, B;
  int C;

  if( plot->driver == 'P' )  
  C = PSCOLOR;
  else  
  C = COLOR;

  cpgslct( plot->covarboard );

  for(i = 0 ; i < MAXCOL ; i++)
  { R = 0.75*(float)(i)/(float)(MAXCOL) + 0.25;
    G = maxf(0.0, 2.0*(float)(i-MAXCOL/2)/(float)(MAXCOL+1));
    B = 0.0 + 0.4*(float)(MAXCOL-i)/(float)(MAXCOL);
    cpgscr(16+i, R, G, B);
  }

  vwp = plot->covar_vwp;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  cpgsci(C);
  XMIN = 0.5;
  XMAX = (float)(N) + 0.5;
  YMIN = 0.5;
  YMAX = (float)(N) + 0.55;
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  cpgbox("BCN", 0.0, 0, "BCN", 0.0, 0);
  cpgscir(16,15+MAXCOL);  
  cpgsitf(2);
  cpgimag( plot->covar, N, N, 1, N, 1, N, min, max, TR );
  cpgwedg( "RI", 1.5, 2.5, min, max, "");

  cpgclos();
}

void plot_Hessian_matrix( PlotParType *plot )
{ int i, N = plot->antennas + plot->redundant_baselines;
  float TR[6] = { 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
  float min = 0.0;
  float max = plot->Hessianmax;
  float XMIN, XMAX, YMIN, YMAX;
  float *vwp;
  float R, G, B;
  int C;
 
  if( plot->driver == 'P' )  
  C = PSCOLOR;
  else  
  C = COLOR;

  cpgslct( plot->Hessianboard );

  for(i = 0 ; i < MAXCOL ; i++)
  { R = 0.75*(float)(i)/(float)(MAXCOL) + 0.25;
    G = maxf(0.0, 2.0*(float)(i-MAXCOL/2)/(float)(MAXCOL+1));
    B = 0.0 + 0.4*(float)(MAXCOL-i)/(float)(MAXCOL);
    cpgscr(16+i, R, G, B);
  }

  vwp = plot->Hessian_vwp;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  cpgsci(C);
  XMIN = 0.5;
  XMAX = (float)(N) + 0.5;
  YMIN = 0.5;
  YMAX = (float)(N) + 0.55;
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  cpgbox("BCN", 0.0, 0, "BCN", 0.0, 0);
  cpgscir(16,15+MAXCOL);  
  cpgsitf(2);
  cpgimag( plot->Hessian, N, N, 1, N, 1, N, min, max, TR );
  cpgwedg( "RI", 1.5, 2.5, min, max, "");

  cpgclos();
}

void plot_model_visibilities( PlotParType *plot )
{
 int rbl;
 int n = plot->nsol;
 int symbol = -4;
 float *vwp;
 float XMIN, XMAX, YMIN, YMAX;
 float x[plot->redundant_baselines];
 int T, E, C, LC;

 T = True;
 E = Estimated;
 C = COLOR;
 LC = MINIMAL_GREY;

 if( plot->driver == 'P' )
 { T = CRIMSON;
   E = BLUE;
   C = PSCOLOR;
   LC = MEDIUM_GREY;
 }

 for(rbl = 0; rbl < plot->redundant_baselines ; rbl++)
 {  x[rbl] = (float)(rbl+1); }

 cpgslct(plot->M_board);

 vwp = plot->M_amp_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->redundant_baselines + 1);
 YMIN =  -0.1 * plot->V_rad;
 YMAX =  1.15 * plot->V_rad; 
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsch(0.84);
 cpgbox("BCMST", 0.0, 0, "BCNST", 0.0, 0);
 cpgsch(1.0);
 cpgmtxt("L",1.8, 0.35, 0.0, "|M| - Jy");
 cpgsci(LC);
 cpgline(plot->redundant_baselines, x, plot->V_true_amp);
 cpgline(plot->redundant_baselines, x, plot->V_amp[n-1]);

 vwp = plot->M_amp_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(T);  
 cpgsch(0.84);
 cpgmtxt("T",-1.8, 0.75, 0.0, "True");
 cpgsci(E);
 cpgmtxt("T",-3.0, 0.75, 0.0, "Estimated");

 vwp = plot->M_phs_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->redundant_baselines + 1);
 YMIN = -200.0;
 YMAX =  200.0;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsch(0.84);
 cpgbox("BCNST", 0.0, 0, "BCMST", 0.0, 0);
 cpgsch(1.0);
 cpgmtxt("L",1.8, 0.20, 0.0, "Phase  -  degrees");
 cpgmtxt("B",1.9, 0.35, 0.0, "Redundant baseline #");
 cpgsci(LC);
 cpgline(plot->redundant_baselines, x, plot->V_true_phs);
 cpgline(plot->redundant_baselines, x, plot->V_phs[n-1]);

 for(rbl = 0; rbl < plot->redundant_baselines ; rbl++)
 { 
   vwp = plot->M_amp_vwp;
   cpgsch(0.84);
   cpgsci(C);
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   XMIN = -1.0;
   XMAX =  (float)(plot->redundant_baselines + 1);
   YMIN =  0.0;
   YMIN =  -0.1 * plot->V_rad;
   YMAX =  1.15 * plot->V_rad; 
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCMST", 0.0, 0, "BCNST", 0.0, 0);
   
   cpgsch(0.4);
   cpgsci(E);
   cpgpt1(x[rbl], plot->V_amp[n-1][rbl], symbol);
   cpgsch(0.4);
   cpgsci(T);
   cpgpt1(x[rbl], plot->V_true_amp[rbl], symbol);

   /*********************/

   vwp = plot->M_phs_vwp;
   cpgsch(0.84);
   cpgsci(C);
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   XMIN = -1.0;
   XMAX =  (float)(plot->redundant_baselines + 1);
   YMIN = -200.0;
   YMAX =  200.0;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCNST", 0.0, 0, "BCMST", 0.0, 0);

   cpgsch(0.4);
   cpgsci(E);
   cpgpt1(x[rbl], plot->V_phs[n-1][rbl], symbol);
   cpgsch(0.4);
   cpgsci(T);
   cpgpt1(x[rbl], plot->V_true_phs[rbl], symbol);

 }

 vwp = plot->M_phs_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(T);  cpgsch(0.84);
 cpgmtxt("T",-1.8, 0.75, 0.0, "True");
 cpgsci(E);
 cpgmtxt("T",-3.0, 0.75, 0.0, "Estimated");

 cpgclos();
}

void plot_scaled_model_visibilities( PlotParType *plot )
{
 int rbl, N = plot->redundant_baselines;
 int n = plot->nsol-1;
 int symbol = -4;
 float *vwp;
 PolarType err;
 float V_abs_plus[N], V_phs_plus[N];
 float V_abs_minus[N], V_phs_minus[N];
 float scale = 0.0, max = 0.0;
 float XMIN, XMAX, YMIN, YMAX;
 float x[N];
 int T, E, C, LC;

 T = True;
 E = Estimated;
 C = COLOR;
 LC = MINIMAL_GREY;

 if( plot->driver == 'P' )
 { T = CRIMSON;
   E = BLUE;
   C = PSCOLOR;
   LC = MEDIUM_GREY;
 }

 scale =  plot->V_true_amp[0] / plot->V_amp[n][0];

 for(rbl = 0; rbl < N ; rbl++)
 { x[rbl] = (float)(rbl+1); 
   
   plot->V_amp[n][rbl] *= scale;

   err = compute_errorbar( plot->vis[n][rbl], plot->M_err[rbl]  );

   V_abs_plus[rbl] = plot->V_amp[n][rbl] + err.abs;
   V_abs_minus[rbl] = plot->V_amp[n][rbl] - err.abs;

   if( max < V_abs_plus[rbl] ) max = V_abs_plus[rbl];
   if( V_abs_minus[rbl] < 0.0 ) V_abs_minus[rbl] = 0.0;

   V_phs_plus[rbl] = plot->V_phs[n][rbl] + err.arg;
   V_phs_minus[rbl] = plot->V_phs[n][rbl] - err.arg;
 }
 /*
 for(rbl = 0; rbl < N ; rbl++)
 { x[rbl] = (float)(rbl+1); 
   
   plot->V_amp[n][rbl] *= scale;

   upper = z_add( plot->vis[n][rbl], plot->M_err[rbl] );
   lower = z_sub( plot->vis[n][rbl], plot->M_err[rbl] );

   err =  z_abs( plot->M_err[rbl]) * scale;

   V_abs_plus[rbl] = plot->V_amp[n][rbl] + err;
   V_abs_minus[rbl] = plot->V_amp[n][rbl] - err;

   if( max < V_abs_plus[rbl] ) max = V_abs_plus[rbl];
   if( V_abs_minus[rbl] < 0.0 ) V_abs_minus[rbl] = 0.0;

   V_phs_plus[rbl] = rad2deg( z_arg(upper) );
   proper_angle( V_phs_plus + rbl );

   V_phs_minus[rbl] = rad2deg( z_arg(lower) );
   proper_angle( V_phs_minus + rbl );
 }
 */ 
 /*
 unwrap_phase( V_phs_plus, plot->redundant_baselines );
 unwrap_phase( V_phs_minus, plot->redundant_baselines );
 */
 cpgslct(plot->M_board);

 vwp = plot->M_amp_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(N + 1);
 YMIN =  -0.1 * max;
 YMAX =  1.15 * max; 
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsch(0.84);
 cpgbox("BCMST", 0.0, 0, "BCNST", 0.0, 0);
 cpgsch(1.0);
 cpgmtxt("L",1.8, 0.35, 0.0, "|M| - Jy");
 cpgsci(LC);
 cpgline(N, x, plot->V_true_amp);
 cpgline(N, x, plot->V_amp[n]);

 vwp = plot->M_amp_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(T);  
 cpgsch(0.84);
 cpgmtxt("T",-1.8, 0.75, 0.0, "True");
 cpgsci(E);
 cpgmtxt("T",-3.0, 0.75, 0.0, "Estimated");

 vwp = plot->M_phs_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(N + 1);
 YMIN = -200.0;
 YMAX =  200.0;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsch(0.84);
 cpgbox("BCNST", 0.0, 0, "BCMST", 0.0, 0);
 cpgsch(1.0);
 cpgmtxt("L",1.8, 0.20, 0.0, "Phase  -  degrees");
 cpgmtxt("B",1.9, 0.45, 0.0, " | UV |");
 cpgsci(LC);
 cpgline(N, x, plot->V_true_phs);
 cpgline(N, x, plot->V_phs[n]);

 for(rbl = 0; rbl < N ; rbl++)
 { 
   vwp = plot->M_amp_vwp;
   cpgsch(0.84);
   cpgsci(C);
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   XMIN = -1.0;
   XMAX =  (float)(N + 1);
   YMIN =  0.0;
   YMIN =  -0.1 * max;
   YMAX =  1.15 * max;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCMST", 0.0, 0, "BCNST", 0.0, 0);
   
   cpgsch(0.4);
   cpgsci(E);
   cpgpt1(x[rbl], plot->V_amp[n][rbl], symbol);
   cpgsch(0.4);
   cpgsci(T);
   cpgpt1(x[rbl], plot->V_true_amp[rbl], symbol);

   /*********************/

   vwp = plot->M_phs_vwp;
   cpgsch(0.84);
   cpgsci(C);
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   XMIN = -1.0;
   XMAX =  (float)(N + 1);
   YMIN = -200.0;
   YMAX =  200.0;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCNST", 0.0, 0, "BCMST", 0.0, 0);

   cpgsch(0.4);
   cpgsci(E);
   cpgpt1(x[rbl], plot->V_phs[n][rbl], symbol);
   cpgsch(0.4);
   cpgsci(T);
   cpgpt1(x[rbl], plot->V_true_phs[rbl], symbol);

 }

 vwp = plot->M_phs_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(T);  
 cpgsch(0.84);
 cpgmtxt("T",-1.8, 0.75, 0.0, "True");
 cpgsci(E);
 cpgmtxt("T",-3.0, 0.75, 0.0, "Estimated");

 vwp = plot->M_amp_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 cpgsch(0.84);
 XMIN = -1.0;
 XMAX =  (float)(plot->redundant_baselines + 1);
 YMIN =  0.0;
 YMIN =  -0.1 * max;
 YMAX =  1.15 * max;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsci(E);
 cpgerry( N, x, V_abs_plus, V_abs_minus, 1.0 );

 vwp = plot->M_phs_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->redundant_baselines + 1);
 YMIN =  0.0;
 YMAX =  1.15 * plot->V_rad; 
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsci(LC);
 cpgerry( N, x, V_phs_plus, V_phs_minus, 1.0 );

 cpgclos();
}


void plot_gain( PlotParType * plot )
{
 int ant;
 int n = plot->nsol;
 int symbol = -4;
 float *vwp;
 float XMIN, XMAX, YMIN, YMAX;
 float x[plot->antennas];
 int T, E, C, LC;

 T = True;
 E = Estimated;
 C = COLOR;
 LC = MINIMAL_GREY;

 if( plot->driver == 'P' )
 { T = CRIMSON;
   E = BLUE;
   C = PSCOLOR;
   LC = MEDIUM_GREY;
 }

 for(ant = 0; ant < plot->antennas ; ant++)
 { x[ant] = (float)(ant+1); }

 cpgslct(plot->G_board);
 
 vwp = plot->G_amp_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN =  0.0;
 YMAX =  1.15 * plot->g_rad; 
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsch(0.84);
 cpgbox("BCMST", 0.0, 0, "BCNST", 0.0, 0);
 cpgsch(1.0);
 cpgmtxt("L",1.8, 0.25, 0.0, "|G|");
 cpgsci(LC);
 cpgline(plot->antennas, x, plot->g_true_amp);
 cpgline(plot->antennas, x, plot->g_amp[n-1]);

 vwp = plot->G_amp_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(T);  
 cpgsch(0.84);
 cpgmtxt("T",-1.8, 0.75, 0.0, "True");
 cpgsci(E);
 cpgmtxt("T",-3.0, 0.75, 0.0, "Estimated");

 vwp = plot->G_phs_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN = -200.0;
 YMAX =  200.0;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsch(0.84); 
 cpgbox("BCNST", 0.0, 0, "BCMST", 0.0, 0);
 cpgsch(1.0);
 cpgmtxt("L",1.8, 0.20, 0.0, "Phase  -  degrees");
 cpgmtxt("B",1.9, 0.45, 0.0, " Antenna # ");
 cpgsci(LC);
 cpgline(plot->antennas, x, plot->g_true_phs);
 cpgline(plot->antennas, x, plot->g_phs[n-1]);

 for(ant = 0; ant < plot->antennas ; ant++)
 { vwp = plot->G_amp_vwp;
   cpgsch(0.84);
   cpgsci(C);
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   XMIN = -1.0;
   XMAX =  (float)(plot->antennas + 1);
   YMIN =  0.0;
   YMAX =  1.15 * plot->g_rad; 
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCMST", 0.0, 0, "BCNST", 0.0, 0);
   
   cpgsch(0.4);
   cpgsci(E);
   cpgpt1(x[ant], plot->g_amp[n-1][ant], symbol);
   cpgsch(0.4);
   cpgsci(T);
   cpgpt1(x[ant], plot->g_true_amp[ant], symbol);

   /*********************/

   vwp = plot->G_phs_vwp;
   cpgsch(0.84);
   cpgsci(C);
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   XMIN = -1.0;
   XMAX =  (float)(plot->antennas + 1);
   YMIN = -200.0;
   YMAX =  200.0;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCNST", 0.0, 0, "BCMST", 0.0, 0);

   cpgsch(0.4);
   cpgsci(E);
   cpgpt1(x[ant], plot->g_phs[n-1][ant], symbol);
   cpgsch(0.4);
   cpgsci(T);
   cpgpt1(x[ant], plot->g_true_phs[ant], symbol);
 }

 vwp = plot->G_phs_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(T);  cpgsch(0.84);
 cpgmtxt("T",-1.8, 0.75, 0.0, "True");
 cpgsci(E);
 cpgmtxt("T",-3.0, 0.75, 0.0, "Estimated");

 cpgclos();
}


void plot_scaled_par_w_errorbar( PlotParType *plot )
{
 int N = plot->antennas, M = plot->redundant_baselines;
 int ant, rbl;
 int n = plot->nsol - 1;
 int symbol = -4;
 float *vwp;
 float g_abs_plus[N];
 float g_abs_minus[N];
 float V_abs_plus[M];
 float V_abs_minus[M];
 PolarType err;
 float XMIN = 0.0, XMAX=0.0, YMIN=0.0, YMAX=0.0;
 float x[N];
 int T, E, C, LC;
 float scale = 0.0, max = 0.0;
#ifdef _DEBUG
 FILE* fcov = fopen("cov.dat", "w");
#endif
 T = True;
 E = Estimated;
 C = COLOR;
 LC = MINIMAL_GREY;

 if( plot->driver == 'P' )
 { T = CRIMSON;
   E = BLUE;
   C = PSCOLOR;
   LC = MEDIUM_GREY;
 }


 for(ant = 0; ant < plot->antennas ; ant++)
 { x[ant] = (float)(ant+1); 
   scale += plot->g_amp[n-1][ant] / plot->g_true_amp[ant] ;
 }
 /* mean scale factor */
 scale /= plot->antennas;

  /*scaling estimated gains */
 for( ant = 0 ; ant < plot->antennas ; ant++ )
 { plot->g_amp[n][ant] /= scale;
   err = compute_errorbar( plot->gain[n][ant], plot->g_err[ant]  );

   g_abs_plus[ant] =  plot->g_amp[n][ant] +  err.abs / scale ;
   g_abs_minus[ant] = plot->g_amp[n][ant] -  err.abs / scale ;

#ifdef _DEBUG
   fprintf(finfo, "\n%s g[%d] -/+ : %f to %f", info, ant+1, g_abs_minus[ant], g_abs_plus[ant]);
   fprintf(fcov, "%d  %f\n", ant+1, err.abs / scale);
#endif

   if( max < g_abs_plus[ant] ) max = g_abs_plus[ant];
   if( g_abs_minus[ant] < 0.0 ) g_abs_minus[ant] = 0.0;
 }

 cpgslct(plot->G_board);
 
 vwp = plot->G_amp_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN =  -0.1 * max;
 YMAX =  1.50 * max;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsch(0.84);
 cpgbox("BCMST", 0.0, 0, "BCNST", 0.0, 0);
 cpgsch(1.0); 
 cpgmtxt("L",1.8, 0.25, 0.0, "|G|");
 cpgsci(LC);
 cpgline(plot->antennas, x, plot->g_true_amp);
 cpgline(plot->antennas, x, plot->g_amp[n]);
 cpgsci(T);  
 cpgsch(0.84);
 cpgmtxt("T",-1.8, 0.75, 0.0, "True");
 cpgsci(E);
 cpgmtxt("T",-3.0, 0.75, 0.0, "Estimated");

for(ant = 0; ant < plot->antennas ; ant++)
 { vwp = plot->G_amp_vwp;
   cpgsch(0.84);
   cpgsci(C);
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   XMIN = -1.0;
   XMAX =  (float)(plot->antennas + 1);
   YMIN =  -0.1 * max;
   YMAX =  1.50 * max;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCMST", 0.0, 0, "BCNST", 0.0, 0);
   
   cpgsch(0.4);
   cpgsci(E);
   cpgpt1(x[ant], plot->g_amp[n][ant], symbol);
   cpgsch(0.4);
   cpgsci(T);
   cpgpt1(x[ant], plot->g_true_amp[ant], symbol);

   /*********************/
 }

 vwp = plot->G_amp_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN =  -0.1 * max;
 YMAX =  1.50 * max; 
 cpgsch(0.84);
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsci(E);
 cpgerry( N, x, g_abs_plus, g_abs_minus, 1.0 );
 cpgsci(C);
 cpgmtxt("T",1.9, 0.43, 0.0, " Antenna #");

 //scale =  plot->V_true_amp[0] / plot->V_amp[n][0];
 scale = scale * scale;
 max = 0.0;
 /*
 for(rbl = 0; rbl < M ; rbl++)
 { x[rbl] = (float)(rbl+1); 
   
   plot->V_amp[n][rbl] *= scale;

   err =  z_abs( plot->M_err[rbl]) * scale;
   
   V_abs_plus[rbl] = plot->V_amp[n][rbl] + err;
   V_abs_minus[rbl] = plot->V_amp[n][rbl] - err;
   
   if( max < V_abs_plus[rbl] ) max = V_abs_plus[rbl];
   if( V_abs_minus[rbl] < 0.0 ) V_abs_minus[rbl] = 0.0;
 }
 */
 for(rbl = 0; rbl < M ; rbl++)
 { x[rbl] = (float)(rbl+1); 
   
   plot->V_amp[n][rbl] *= scale;

   err = compute_errorbar( plot->vis[n][rbl], plot->M_err[rbl]  );

   V_abs_plus[rbl] = plot->V_amp[n][rbl] + err.abs*scale;
   V_abs_minus[rbl] = plot->V_amp[n][rbl] - err.abs*scale;

   if( max < V_abs_plus[rbl] ) max = V_abs_plus[rbl];
   if( V_abs_minus[rbl] < 0.0 ) V_abs_minus[rbl] = 0.0;

#ifdef _DEBUG
   fprintf(finfo, "\n%s M[%d] -/+ : %f to %f", info, rbl+1, V_abs_minus[rbl], V_abs_plus[rbl]);
   fprintf(fcov, "%d  %f\n", plot->antennas+rbl+1, err.abs *scale);
#endif
 }

 vwp = plot->G_phs_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(N + 1);
 YMIN =  -0.1 * max;
 YMAX =  1.15 * max; 
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsch(0.84);
 cpgbox("BCNST", 0.0, 0, "BCNST", 0.0, 0);
 cpgsch(1.0);
 cpgmtxt("L",1.8, 0.35, 0.0, "|M| - Jy");
 cpgsci(LC);
 cpgline(M, x, plot->V_true_amp);
 cpgline(M, x, plot->V_amp[n]);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(T);  
 cpgsch(0.84);
 cpgmtxt("T",-1.8, 0.75, 0.0, "True");
 cpgsci(E);
 cpgmtxt("T",-3.0, 0.75, 0.0, "Estimated");

 for(rbl = 0; rbl < M ; rbl++)
 { 
   vwp = plot->G_phs_vwp;
   cpgsch(0.84);
   cpgsci(C);
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   XMIN = -1.0;
   XMAX =  (float)(N + 1);
   YMIN =  0.0;
   YMIN =  -0.1 * max;
   YMAX =  1.15 * max;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCNST", 0.0, 0, "BCNST", 0.0, 0);
   
   cpgsch(0.4);
   cpgsci(E);
   cpgpt1(x[rbl], plot->V_amp[n][rbl], symbol);
   cpgsch(0.4);
   cpgsci(T);
   cpgpt1(x[rbl], plot->V_true_amp[rbl], symbol);
 }

 vwp = plot->G_phs_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 cpgsch(0.84);
 XMIN = -1.0;
 XMAX =  (float)(N + 1);
 YMIN =  0.0;
 YMIN =  -0.1 * max;
 YMAX =  1.15 * max;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsci(E);
 cpgerry( M, x, V_abs_plus, V_abs_minus, 1.0 );

 cpgsci(C);
 cpgmtxt("B",2.5, 0.35, 0.0, " Redundant baseline #");

 cpgclos();
}

void plot_scaled_par_w_CRB( PlotParType *plot )
{
 int N = plot->antennas, M = plot->redundant_baselines;
 int ant, rbl;
 int n = plot->nsol - 1;
 int symbol = -4;
 float *vwp;
 float g_abs_plus[N];
 float g_abs_minus[N];
 float V_abs_plus[M];
 float V_abs_minus[M];
 PolarType err;
 float XMIN = 0.0, XMAX=0.0, YMIN=0.0, YMAX=0.0;
 float x[N];
 int T, E, C, LC;
 float scale = 0.0, max = 0.0;
#ifdef _DEBUG
 FILE *fCRB = fopen("crb.dat", "w");
#endif
 T = True;
 E = Estimated;
 C = COLOR;
 LC = MINIMAL_GREY;

 if( plot->driver == 'P' )
 { T = CRIMSON;
   E = BLUE;
   C = PSCOLOR;
   LC = MEDIUM_GREY;
 }


 for(ant = 0; ant < plot->antennas ; ant++)
 { x[ant] = (float)(ant+1); 
   scale += plot->g_amp[n-1][ant] / plot->g_true_amp[ant] ;
 }
 /* mean scale factor */
 scale /= plot->antennas;

 /*
 for(ant = 0; ant < plot->antennas ; ant++)
 { plot->g_amp[n][ant] /= scale;

   err = scale * z_abs( plot->g_err[ant] );

   g_abs_plus[ant] =  plot->g_amp[n][ant] + err;
   g_abs_minus[ant] = plot->g_amp[n][ant] - err ;

   if( max < g_abs_plus[ant] ) max = g_abs_plus[ant];
   if( g_abs_minus[ant] < 0.0 ) g_abs_minus[ant] = 0.0;
 }
 */
  /*scaling estimated gains */
 for( ant = 0 ; ant < plot->antennas ; ant++ )
 { plot->g_amp[n][ant] /= scale;
   err = compute_errorbar( plot->gain[n][ant], plot->g_err[ant]  );
   
   g_abs_plus[ant] =  plot->g_amp[n][ant] +  err.abs ;
   g_abs_minus[ant] = plot->g_amp[n][ant] -  err.abs ;

#ifdef _DEBUG
   fprintf(finfo, "\n%s g[%d] -/+ : %f to %f", info, ant+1, g_abs_minus[ant], g_abs_plus[ant]);
   fprintf(fCRB, "%d  %f\n", ant+1, err.abs);
#endif

   if( max < g_abs_plus[ant] ) max = g_abs_plus[ant];
   if( g_abs_minus[ant] < 0.0 ) g_abs_minus[ant] = 0.0;
 }

 cpgslct(plot->G_board);
 
 vwp = plot->G_amp_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN =  -0.1 * max;
 YMAX =  1.50 * max;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsch(0.84);
 cpgbox("BCMST", 0.0, 0, "BCNST", 0.0, 0);
 cpgsch(1.0); 
 cpgmtxt("L",1.8, 0.25, 0.0, "|G|");
 cpgsci(LC);
 cpgline(plot->antennas, x, plot->g_true_amp);
 cpgline(plot->antennas, x, plot->g_amp[n]);
 cpgsci(T);  
 cpgsch(0.84);
 cpgmtxt("T",-1.8, 0.75, 0.0, "True");
 cpgsci(E);
 cpgmtxt("T",-3.0, 0.75, 0.0, "Estimated");

for(ant = 0; ant < plot->antennas ; ant++)
 { vwp = plot->G_amp_vwp;
   cpgsch(0.84);
   cpgsci(C);
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   XMIN = -1.0;
   XMAX =  (float)(plot->antennas + 1);
   YMIN =  -0.1 * max;
   YMAX =  1.50 * max;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCMST", 0.0, 0, "BCNST", 0.0, 0);
   
   cpgsch(0.4);
   cpgsci(E);
   cpgpt1(x[ant], plot->g_amp[n][ant], symbol);
   cpgsch(0.4);
   cpgsci(T);
   cpgpt1(x[ant], plot->g_true_amp[ant], symbol);

   /*********************/
 }

 vwp = plot->G_amp_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN =  -0.1 * max;
 YMAX =  1.50 * max; 
 cpgsch(0.84);
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsci(E);
 cpgerry( N, x, g_abs_plus, g_abs_minus, 1.0 );
 cpgsci(C);
 cpgmtxt("T",1.9, 0.43, 0.0, " Antenna #");

 //scale =  plot->V_true_amp[0] / plot->V_amp[n][0];
 scale = scale * scale;
 max = 0.0;
 /*
 for(rbl = 0; rbl < M ; rbl++)
 { x[rbl] = (float)(rbl+1); 
   
   plot->V_amp[n][rbl] *= scale;

   err =  z_abs( plot->M_err[rbl]) * scale;
   
   V_abs_plus[rbl] = plot->V_amp[n][rbl] + err;
   V_abs_minus[rbl] = plot->V_amp[n][rbl] - err;
   
   if( max < V_abs_plus[rbl] ) max = V_abs_plus[rbl];
   if( V_abs_minus[rbl] < 0.0 ) V_abs_minus[rbl] = 0.0;
 }
 */
 for(rbl = 0; rbl < M ; rbl++)
 { x[rbl] = (float)(rbl+1); 
   
   plot->V_amp[n][rbl] *= scale;

   err = compute_errorbar( plot->vis[n][rbl], plot->M_err[rbl]  );

   V_abs_plus[rbl] = plot->V_amp[n][rbl] + err.abs;
   V_abs_minus[rbl] = plot->V_amp[n][rbl] - err.abs;

   if( max < V_abs_plus[rbl] ) max = V_abs_plus[rbl];
   if( V_abs_minus[rbl] < 0.0 ) V_abs_minus[rbl] = 0.0;

#ifdef _DEBUG
   fprintf(finfo, "\n%s M[%d] -/+ : %f to %f", info, rbl+1, V_abs_minus[rbl], V_abs_plus[rbl]);
   fprintf(fCRB, "%d  %f\n", plot->antennas+rbl+1, err.abs );
#endif
 }

 vwp = plot->G_phs_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(N + 1);
 YMIN =  -0.1 * max;
 YMAX =  1.15 * max; 
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsch(0.84);
 cpgbox("BCNST", 0.0, 0, "BCNST", 0.0, 0);
 cpgsch(1.0);
 cpgmtxt("L",1.8, 0.35, 0.0, "|M| - Jy");
 cpgsci(LC);
 cpgline(M, x, plot->V_true_amp);
 cpgline(M, x, plot->V_amp[n]);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(T);  
 cpgsch(0.84);
 cpgmtxt("T",-1.8, 0.75, 0.0, "True");
 cpgsci(E);
 cpgmtxt("T",-3.0, 0.75, 0.0, "Estimated");

 for(rbl = 0; rbl < M ; rbl++)
 { 
   vwp = plot->G_phs_vwp;
   cpgsch(0.84);
   cpgsci(C);
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   XMIN = -1.0;
   XMAX =  (float)(N + 1);
   YMIN =  0.0;
   YMIN =  -0.1 * max;
   YMAX =  1.15 * max;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCNST", 0.0, 0, "BCNST", 0.0, 0);
   
   cpgsch(0.4);
   cpgsci(E);
   cpgpt1(x[rbl], plot->V_amp[n][rbl], symbol);
   cpgsch(0.4);
   cpgsci(T);
   cpgpt1(x[rbl], plot->V_true_amp[rbl], symbol);
 }

 vwp = plot->G_phs_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 cpgsch(0.84);
 XMIN = -1.0;
 XMAX =  (float)(N + 1);
 YMIN =  0.0;
 YMIN =  -0.1 * max;
 YMAX =  1.15 * max;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsci(E);
 cpgerry( M, x, V_abs_plus, V_abs_minus, 1.0 );

 cpgsci(C);
 cpgmtxt("B",2.5, 0.35, 0.0, " Redundant baseline #");

 cpgclos();
}

void plot_scaled_gain( PlotParType * plot )
{
 int N = plot->antennas;
 int ant;
 int n = plot->nsol - 1;
 int symbol = -4;
 float *vwp;
 float g_abs_plus[N], g_phs_plus[N];
 float g_abs_minus[N], g_phs_minus[N];
 PolarType err;
 float XMIN = 0.0, XMAX=0.0, YMIN=0.0, YMAX=0.0;
 float x[N];
 int T, E, C, LC;
 float scale = 0.0, max = 0.0;

 T = True;
 E = Estimated;
 C = COLOR;
 LC = MINIMAL_GREY;

 if( plot->driver == 'P' )
 { T = CRIMSON;
   E = BLUE;
   C = PSCOLOR;
   LC = MEDIUM_GREY;
 }
 /*
 for(ant = 0; ant < plot->antennas ; ant++)
 { x[ant] = (float)(ant+1); 
   scale += plot->g_amp[n-1][ant] / plot->g_true_amp[ant] ;
 }

 scale /= plot->antennas;

 for(ant = 0; ant < plot->antennas ; ant++)
 { plot->g_amp[n][ant] /= scale;

   upper = z_add( plot->gain[n][ant], plot->g_err[ant] );
   lower = z_sub( plot->gain[n][ant], plot->g_err[ant] );

   err = scale * z_abs( plot->g_err[ant] );

   g_abs_plus[ant] =  plot->g_amp[n][ant] + err;
   g_abs_minus[ant] = plot->g_amp[n][ant] - err ;

   if( max < g_abs_plus[ant] ) max = g_abs_plus[ant];
   if( g_abs_minus[ant] < 0.0 ) g_abs_minus[ant] = 0.0;

   g_phs_plus[ant] = rad2deg( z_arg(upper) );
   proper_angle( g_phs_plus + ant );

   g_phs_minus[ant] = rad2deg( z_arg(lower) );
   proper_angle( g_phs_minus + ant );
 }
 */

 for(ant = 0; ant < plot->antennas ; ant++)
 { x[ant] = (float)(ant+1); 
   scale += plot->g_amp[n-1][ant] / plot->g_true_amp[ant] ;
 }
 /* mean scale factor */
 scale /= plot->antennas;

  /*scaling estimated gains */
 for( ant = 0 ; ant < plot->antennas ; ant++ )
 { plot->g_amp[n][ant] /= scale;
   err = compute_errorbar( plot->gain[n][ant], plot->g_err[ant]  );
   
   g_abs_plus[ant] =  plot->g_amp[n][ant] + scale * err.abs;
   g_abs_minus[ant] = plot->g_amp[n][ant] - scale * err.abs ;

   if( max < g_abs_plus[ant] ) max = g_abs_plus[ant];
   if( g_abs_minus[ant] < 0.0 ) g_abs_minus[ant] = 0.0;

   g_phs_plus[ant] = plot->g_phs[n][ant] + err.arg;
   g_phs_minus[ant] = plot->g_phs[n][ant] - err.arg;
 }

 cpgslct(plot->G_board);
 
 vwp = plot->G_amp_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN =  -0.1 * max;
 YMAX =  1.50 * max;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsch(0.84);
 cpgbox("BCMST", 0.0, 0, "BCNST", 0.0, 0);
 cpgsch(1.0); 
 cpgmtxt("L",1.8, 0.25, 0.0, "|G|");
 cpgsci(LC);
 cpgline(plot->antennas, x, plot->g_true_amp);
 cpgline(plot->antennas, x, plot->g_amp[n]);
 vwp = plot->G_amp_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(T);  
 cpgsch(0.84);
 cpgmtxt("T",-1.8, 0.75, 0.0, "True");
 cpgsci(E);
 cpgmtxt("T",-3.0, 0.75, 0.0, "Estimated");

 vwp = plot->G_phs_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN = -200.0;
 YMAX =  200.0;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsch(0.84);
 cpgbox("BCNST", 0.0, 0, "BCMST", 0.0, 0);
 cpgsch(1.0);
 cpgmtxt("L",1.8, 0.20, 0.0, "Phase  -  degrees");
 cpgmtxt("B",1.9, 0.45, 0.0, " Antenna # ");
 cpgsci(LC);
 cpgline(plot->antennas, x, plot->g_true_phs);
 cpgline(plot->antennas, x, plot->g_phs[n]);
 
for(ant = 0; ant < plot->antennas ; ant++)
 { vwp = plot->G_amp_vwp;
   cpgsch(0.84);
   cpgsci(C);
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   XMIN = -1.0;
   XMAX =  (float)(plot->antennas + 1);
   YMIN =  -0.1 * max;
   YMAX =  1.50 * max;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCMST", 0.0, 0, "BCNST", 0.0, 0);
   
   cpgsch(0.4);
   cpgsci(E);
   cpgpt1(x[ant], plot->g_amp[n][ant], symbol);
   cpgsch(0.4);
   cpgsci(T);
   cpgpt1(x[ant], plot->g_true_amp[ant], symbol);

   /*********************/

   vwp = plot->G_phs_vwp;
   cpgsch(0.84);
   cpgsci(C);
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   XMIN = -1.0;
   XMAX =  (float)(plot->antennas + 1);
   YMIN = -200.0;
   YMAX =  200.0;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCNST", 0.0, 0, "BCMST", 0.0, 0);

   cpgsch(0.4);
   cpgsci(E);
   cpgpt1(x[ant], plot->g_phs[n][ant], symbol);
   cpgsch(0.4);
   cpgsci(T);
   cpgpt1(x[ant], plot->g_true_phs[ant], symbol);
 }

 vwp = plot->G_phs_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(T);  
 cpgsch(0.84);
 cpgmtxt("T",-1.8, 0.75, 0.0, "True");
 cpgsci(E);
 cpgmtxt("T",-3.0, 0.75, 0.0, "Estimated");

 vwp = plot->G_amp_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN =  -0.1 * max;
 YMAX =  1.50 * max; 
 cpgsch(0.84);
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsci(E);
 cpgerry( N, x, g_abs_plus, g_abs_minus, 1.0 );

 vwp = plot->G_phs_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN =  -200.0;
 YMAX =  200.0;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsci(LC);
 cpgerry( N, x, g_phs_plus, g_phs_minus, 1.0 );

 cpgclos();
}

void plot_scale( PlotParType *plot )
{
 int ant, rbl;
 int n = plot->nsol;
 float abs[plot->antennas], arg[plot->antennas];
 int symbol = -4;
 float *vwp;
 float XMIN, XMAX, YMIN, YMAX;
 float x[plot->antennas];
 int G, M, C, LC;

 G = GScale;
 M = MScale;
 C = COLOR;
 LC = MINIMAL_GREY;

 if( plot->driver == 'P' )
 { G = CRIMSON;
   M = BLUE;
   C = PSCOLOR;
   LC = MEDIUM_GREY;
 }

 for( ant = 0; ant < plot->antennas; ant++)
 { x[ant] = (float)(ant+1);
   abs[ant] = plot->g_amp[n-1][ant] / plot->g_true_amp[ant] ;
   arg[ant] = plot->g_phs[n-1][ant] - plot->g_true_phs[ant]  ;
   proper_angle( (double*)&(arg[ant]) );
 }

 unwrap_phase( (double*)arg, plot->antennas);

 cpgslct(plot->scale_board);
 vwp = plot->ampscale_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN =  0.0;
 YMAX =  1.15 * plot->scale; 
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsci(LC);
 cpgline(plot->antennas, x, abs);
 cpgsci(G);  cpgsch(0.84);
 cpgmtxt("T",-1.8, 0.75, 0.0, "|g_est|/|g_true|");
 
 vwp = plot->phoffset_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN = -200.0;
 YMAX =  200.0;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsci(LC);
 cpgline(plot->antennas, x, arg);
 cpgsci(G);  cpgsch(0.84);
 cpgmtxt("T",-1.8, 0.60, 0.0, "Phase(g_est) - phase(g_true)");

 for(ant = 0; ant < plot->antennas ; ant++)
 { vwp = plot->ampscale_vwp;
   cpgsch(0.84);
   cpgsci(C);
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   XMIN = -1.0;
   XMAX =  (float)(plot->antennas + 1);
   YMIN =  0.0;
   YMAX =  1.15 * plot->scale; 
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCMST", 0.0, 0, "BCNST", 0.0, 0);
   cpgsch(0.4);
   cpgsci(G);
   cpgpt1(x[ant], abs[ant], symbol);

   /*********************/

   vwp = plot->phoffset_vwp;
   cpgsch(0.84);
   cpgsci(C);
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   XMIN = -1.0;
   XMAX =  (float)(plot->antennas + 1);
   YMIN = -200.0;
   YMAX =  200.0;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCNST", 0.0, 0, "BCMST", 0.0, 0);
   cpgsch(0.4);
   cpgsci(G);
   cpgpt1(x[ant], arg[ant], symbol);

 }


 for( rbl = 0; rbl < plot->redundant_baselines; rbl++)
 { x[rbl] = (float)(rbl+1);
   abs[rbl] = plot->V_true_amp[rbl]/plot->V_amp[n-1][rbl];
   arg[rbl] = plot->V_true_phs[rbl] - plot->V_phs[n-1][rbl] ;
   proper_angle( (double*)&(arg[rbl]) );
 }

 unwrap_phase( (double*)arg, plot->redundant_baselines );
 /*
 for( rbl = 0; rbl < plot->redundant_baselines; rbl++)
 {arg[rbl] += 180.0;
  if (arg[rbl] >= 180.0) arg[rbl] -= 360.0;
 }
 */
 vwp = plot->ampscale_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN =  0.0;
 YMAX =  1.15 * plot->scale; 
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsci(LC);
 cpgline(plot->redundant_baselines, x, abs);
 cpgsci(M);  cpgsch(0.84);
 cpgmtxt("T",-3.1, 0.75, 0.0, "|M_true|/|M_est|");

 vwp = plot->phoffset_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN = -200.0;
 YMAX =  200.0;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsci(LC);
 cpgline(plot->redundant_baselines, x, arg);
 cpgsci(M);  cpgsch(0.84);
 cpgmtxt("T",-3.1, 0.60, 0.0, "Phase(M_true) - phase(M_est)");

 for(rbl = 0; rbl < plot->redundant_baselines ; rbl++)
 { vwp = plot->ampscale_vwp;
   cpgsch(0.84);
   cpgsci(C);
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   XMIN = -1.0;
   XMAX =  (float)(plot->antennas + 1);
   YMIN =  0.0;
   YMAX =  1.15 * plot->scale; 
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCMST", 0.0, 0, "BCNST", 0.0, 0);
   cpgsch(0.4);
   cpgsci(M);
   cpgpt1(x[rbl], abs[rbl], symbol);

   /*********************/

   vwp = plot->phoffset_vwp;
   cpgsch(0.84);
   cpgsci(C);
   cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
   XMIN = -1.0;
   XMAX =  (float)(plot->antennas + 1);
   YMIN = -200.0;
   YMAX =  200.0;
   cpgswin(XMIN, XMAX, YMIN, YMAX);
   cpgbox("BCNST", 0.0, 0, "BCMST", 0.0, 0);
   cpgsch(0.4);
   cpgsci(M);
   cpgpt1(x[rbl], arg[rbl], symbol);
 }

 cpgclos();
}

void plot_gain_errorbar( PlotParType *plot )
{
 int ant, N = plot->antennas;
 int n = plot->nsol - 1;
 CmplxType upper, lower;
 float g_abs_plus[N], g_phs_plus[N];
 float g_abs_minus[N], g_phs_minus[N];
 float *vwp, x[N];
 float XMIN, XMAX, YMIN, YMAX;
 int T, E, C, LC;

 T = True;
 E = Estimated;
 C = COLOR;
 LC = MINIMAL_GREY;

 if( plot->driver == 'P' )
 { T = CRIMSON;
   E = BLUE;
   C = PSCOLOR;
   LC = MEDIUM_GREY;
 }

 for( ant = 0 ; ant < N ; ant++ )
 { upper = z_add( plot->gain[n][ant], plot->g_err[ant] );
   lower = z_sub( plot->gain[n][ant], plot->g_err[ant] );

   g_abs_plus[ant] = z_abs(upper);
   g_abs_minus[ant] = z_abs(lower);

   g_phs_plus[ant] = rad2deg( z_arg(upper) );
   g_phs_minus[ant] = rad2deg( z_arg(lower) );

   x[ant] = (float)(ant+1);

   //fprintf(finfo, "\n%s %d %f %f %f %f", info, ant, g_abs_plus[ant], g_abs_minus[ant], g_phs_plus[ant], g_phs_minus[ant]);
 }

 cpgslct(plot->G_board);
 
 vwp = plot->G_amp_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN =  0.0;
 YMAX =  1.15 * plot->g_rad; 
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsci(LC);
 cpgerry( N, x, g_abs_plus, g_abs_minus, 1.0 );

 vwp = plot->G_phs_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->antennas + 1);
 YMIN =  0.0;
 YMAX =  1.15 * plot->g_rad; 
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsci(LC);
 cpgerry( N, x, g_phs_plus, g_phs_minus, 1.0 );

}

void plot_vis_errorbar( PlotParType *plot )
{
 int rbl, N = plot->redundant_baselines;
 int n = plot->nsol - 1;
 CmplxType upper, lower;
 float err;
 float V_abs_plus[N], V_phs_plus[N];
 float V_abs_minus[N], V_phs_minus[N];
 float *vwp, x[N];
 float XMIN, XMAX, YMIN, YMAX;
 int T, E, C, LC;

 T = True;
 E = Estimated;
 C = COLOR;
 LC = MINIMAL_GREY;

 if( plot->driver == 'P' )
 { T = CRIMSON;
   E = BLUE;
   C = PSCOLOR;
   LC = MEDIUM_GREY;
 }

 for( rbl = 0 ; rbl < N ; rbl++ )
 { upper = z_add( plot->vis[n][rbl], plot->M_err[rbl] );
   lower = z_sub( plot->vis[n][rbl], plot->M_err[rbl] );

   err = z_abs( plot->M_err[rbl]);

   V_abs_plus[rbl] = z_abs(upper);
   V_abs_minus[rbl] = z_abs(lower);
   /*
   V_abs_plus[rbl] = z_abs( plot->vis[n][rbl] ) + err;
   V_abs_minus[rbl] = z_abs( plot->vis[n][rbl] ) - err;
   */
   if( V_abs_minus[rbl] < 0.0 ) V_abs_minus[rbl] = 0.0;

   V_phs_plus[rbl] = rad2deg( z_arg(upper) );
   proper_angle( (double*)V_phs_plus + rbl );

   V_phs_minus[rbl] = rad2deg( z_arg(lower) );
   proper_angle( (double*)V_phs_minus + rbl );

   x[rbl] = (float)(rbl+1);

   fprintf(finfo, "\n%s %d %f %f %f", info, rbl, V_abs_plus[rbl], V_abs_minus[rbl], z_abs(plot->vis[n][rbl]) ) ;
 }
 /*
 unwrap_phase( V_phs_plus, plot->redundant_baselines );
 unwrap_phase( V_phs_minus, plot->redundant_baselines );
 */
 cpgslct(plot->M_board);
 
 vwp = plot->M_amp_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->redundant_baselines + 1);
 YMIN =  0.0;
 YMIN =  -0.1 * plot->V_rad;
 YMAX =  1.15 * plot->V_rad; 
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsci(LC);
 cpgerry( N, x, V_abs_plus, V_abs_minus, 1.0 );
 
 vwp = plot->M_phs_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = -1.0;
 XMAX =  (float)(plot->redundant_baselines + 1);
 YMIN =  0.0;
 YMAX =  1.15 * plot->V_rad; 
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsci(LC);
 cpgerry( N, x, V_phs_plus, V_phs_minus, 1.0 );
 
}

void xsplot_gain_and_vis( PlotParType *plot, char *solfilename )
{
  parse_solfile( plot, solfilename );
  init_plotvars( plot );
  read_solutions( plot, solfilename );

  read_covar_matrix( plot );

  init_gscope( plot );
  open_gscope( plot );
  init_covarscope( plot );
  open_covarscope( plot );
  plot_scaled_par_w_errorbar( plot );
  /*  
  init_Mscope( plot );
  open_Mscope( plot );
  init_covarscope( plot );
  open_covarscope( plot );
  plot_scaled_gain( plot );
  plot_scaled_model_visibilities( plot );
  */
  plot_covar_matrix( plot );

}

void psplot_gain_and_vis( PlotParType *plot, char *solfilename )
{
  parse_solfile( plot, solfilename );
  init_plotvars( plot );
  read_solutions( plot, solfilename );

  read_covar_matrix( plot );

  init_gscope( plot );
  open_gscope( plot );
  plot_scaled_par_w_errorbar( plot );
  //plot_scaled_gain( plot );
  mvps_to("par_errbar.eps");
  /*
  init_Mscope( plot );
  open_Mscope( plot );
  plot_scaled_model_visibilities( plot );
  mvps_to("vis_errbar.eps");
  */
  init_covarscope( plot );
  open_covarscope( plot );
  plot_covar_matrix( plot );
  mvps_to("covar.eps");
}


void xsplot_all ( PlotParType *plot, char *solfilename )
{
  parse_solfile( plot, solfilename );
  init_plotvars( plot );
  read_solutions( plot, solfilename );

  //parse_MSE_file( plot );
  //read_MSE( plot );

  //parse_chisq_file( plot );
  //read_chisq( plot );

  read_SNR_matrix( plot );

  read_covar_matrix( plot );

  init_scopes( plot );
  open_scopes( plot );

  plot_itergain( plot );
  plot_itervis( plot );
  //plot_MSE( plot );
  //plot_chisq( plot );
  plot_SNR_matrix( plot );
  plot_covar_matrix( plot );

  plot_gain( plot );
  plot_model_visibilities( plot );
  plot_scale( plot );
  
  free_plotvars( plot );
}

void xsplot_all_but_errors ( PlotParType *plot, char *solfilename )
{
  parse_solfile( plot, solfilename );
  //parse_chisq_file( plot );

  init_plotvars( plot );
  read_solutions( plot, solfilename );
  //read_chisq( plot );
  //read_SNR_matrix(plot);
  read_covar_matrix(plot);
  init_scopes( plot );
  open_trajscope( plot );

  //open_SNRscope( plot );  

  open_gscope( plot );
  open_Mscope( plot );
  open_scalescope( plot );
  open_covarscope( plot );
  plot_itergain( plot );
  plot_itervis( plot );
  plot_gain( plot );
  //plot_SNR_matrix( plot );
  plot_model_visibilities( plot );
  plot_scale( plot );
  plot_covar_matrix( plot );

  free_plotvars( plot );
}


int mvps_to( char *filename )
{
 int success;
 char command[80];
 sprintf(command, "mv pgplot.ps %s", filename);
 success = system(command);
 return success;
}

void xsplottraj( PlotParType *plot, char *solfilename )
{
 parse_solfile( plot, solfilename );
 init_plotvars( plot );
 read_solutions( plot, solfilename );

 init_scopes( plot );

 open_trajscope( plot );
 open_gscope( plot );
 open_Mscope( plot );
 open_scalescope( plot );

 plot_itergain( plot );
 plot_itervis( plot );
 plot_gain( plot );
 plot_model_visibilities( plot );
 plot_scale( plot );

 free_plotvars( plot );
}

void psplottraj( PlotParType *plot, char *solfilename )
{
 parse_solfile( plot, solfilename );
 init_plotvars( plot );
 read_solutions( plot, solfilename );

 init_scopes( plot );

 open_trajscope( plot );
 plot_itergain( plot );
 plot_itervis( plot );
 mvps_to("iterplots.eps");

 open_gscope( plot );
 plot_gain( plot );
 mvps_to("gain.eps");

 open_Mscope( plot );
 plot_model_visibilities( plot );
 mvps_to("modelvis.eps");

 open_scalescope( plot );
 plot_scale( plot );
 mvps_to("scale.eps");
  
 free_plotvars( plot );
}

void psplotsol ( PlotParType *plot, char *solfilename )
{
  parse_solfile( plot, solfilename );
  init_plotvars( plot );
  read_solutions( plot, solfilename );

  parse_MSE_file( plot );
  read_MSE( plot );

  parse_chisq_file( plot );
  read_chisq( plot );
  //read_SNR_matrix( plot );

  init_scopes( plot );

  open_trajscope( plot );
  plot_itergain( plot );
  plot_itervis( plot );
  mvps_to("iterplots.eps");

  open_MSEscope( plot );
  plot_MSE( plot );
  mvps_to("MSE.eps");

  open_chisqscope( plot );
  plot_chisq( plot );
  mvps_to("chisq.eps");
  /*
  open_SNRscope( plot );
  plot_SNR_matrix( plot );
  mvps_to("SNR.eps");
  */
  open_gscope( plot );
  plot_gain( plot );
  mvps_to("gain.eps");

  open_Mscope( plot );
  plot_model_visibilities( plot );
  mvps_to("modelvis.eps");

  open_scalescope( plot );
  plot_scale( plot );
  mvps_to("scale.eps");
  
  free_plotvars( plot );
}

void xsplotsol ( PlotParType *plot, char *solfilename )
{
  parse_solfile( plot, solfilename );
  init_plotvars( plot );
  read_solutions( plot, solfilename );

  //parse_MSE_file( plot );
  //read_MSE( plot );

  //parse_chisq_file( plot );
  //read_chisq( plot );
  //read_SNR_matrix( plot );

  init_scopes( plot );
  open_trajscope( plot );
  open_MSEscope( plot );
  open_chisqscope( plot );
  //open_SNRscope( plot );
  open_gscope( plot );
  open_Mscope( plot );
  open_scalescope( plot );

  plot_itergain( plot );
  plot_itervis( plot );
  //plot_MSE( plot );
  //plot_chisq( plot );
  //plot_SNR_matrix( plot );
  plot_gain( plot );
  plot_model_visibilities( plot );
  plot_scale( plot );
  
  free_plotvars( plot );
}

void psplot_all ( PlotParType *plot, char *solfilename )
{
  parse_solfile( plot, solfilename );
  init_plotvars( plot );
  read_solutions( plot, solfilename );

  /*  parse_MSE_file( plot );
  read_MSE( plot );

  parse_chisq_file( plot );
  read_chisq( plot );

  //read_SNR_matrix( plot );
  read_covar_matrix( plot );
  */
  init_scopes( plot );

  open_trajscope( plot );
  plot_itergain( plot );
  plot_itervis( plot );
  mvps_to("iterplots.eps");
  /*
  open_MSEscope( plot );
  plot_MSE( plot );
  mvps_to("MSE.eps");

  open_chisqscope( plot );
  plot_chisq( plot );
  mvps_to("chisq.eps");
  */
  /*
  open_SNRscope( plot );
  plot_SNR_matrix( plot );
  mvps_to("SNR.eps");
  */

  open_covarscope( plot );
  plot_covar_matrix( plot );
  mvps_to("covar.eps");

  open_gscope( plot );
  plot_gain( plot );
  mvps_to("gain.eps");

  open_Mscope( plot );
  plot_model_visibilities( plot );
  mvps_to("modelvis.eps");

  open_scalescope( plot );
  plot_scale( plot );
  mvps_to("scale.eps");
  
  free_plotvars( plot );
}

void psplot_all_but_errors ( PlotParType *plot, char *solfilename )
{
  parse_solfile( plot, solfilename );
  init_plotvars( plot );
  read_solutions( plot, solfilename );

  //read_SNR_matrix( plot );

  init_scopes( plot );

  open_trajscope( plot );
  plot_itergain( plot );
  plot_itervis( plot );
  mvps_to("iterplots.eps");

  /*
  open_SNRscope( plot );
  plot_SNR_matrix( plot );
  mvps_to("SNR.eps");
  */
  open_gscope( plot );
  plot_gain( plot );
  mvps_to("gain.eps");

  open_Mscope( plot );
  plot_model_visibilities( plot );
  mvps_to("modelvis.eps");

  open_scalescope( plot );
  plot_scale( plot );
  mvps_to("scale.eps");
  
  free_plotvars( plot );
}

void xsplot_solcompare( PlotParType *plot, char *solfilename )
{
  parse_solfile( plot, solfilename );
  init_plotvars( plot );
  read_solutions( plot, solfilename );

  init_errscope( plot );
  open_gcmplxscope( plot );
  open_Mcmplxscope( plot );

  plot_gaincompare( plot );

  plot_viscompare( plot );

  free_plotvars( plot );

}

void psplot_solcompare( PlotParType *plot, char *solfilename )
{
  parse_solfile( plot, solfilename );
  init_plotvars( plot );
  read_solutions( plot, solfilename );

  init_errscope( plot );
  open_gcmplxscope( plot );
  plot_gaincompare( plot );
  mvps_to("gcmplx.eps");

  open_Mcmplxscope( plot );
  plot_viscompare( plot );
  mvps_to("Mcmplx.eps");

  free_plotvars( plot );
}

void xsplot_Hessian_and_covar( PlotParType *plot, char *solfilename )
{
 parse_solfile( plot, solfilename );
 init_plotvars( plot );
 read_solutions( plot, solfilename );

 init_Hessianscope( plot );
 open_Hessianscope( plot );
 read_Hessian_matrix( plot );
 init_covarscope( plot );
 open_covarscope( plot );
 read_covar_matrix( plot );
 init_gscope( plot );
 open_gscope( plot );
 plot_Hessian_matrix( plot );
 plot_covar_matrix( plot );
 plot_scaled_par_w_errorbar( plot );

 free_plotvars( plot );

}

void psplot_Hessian_and_covar( PlotParType *plot, char *solfilename )
{
 parse_solfile( plot, solfilename );
 init_plotvars( plot );
 read_solutions( plot, solfilename );

 init_Hessianscope( plot );
 open_Hessianscope( plot );
 read_Hessian_matrix( plot );
 plot_Hessian_matrix( plot );
 if( plot->driver == 'P' ) mvps_to("Hessian.eps");

 init_covarscope( plot );
 open_covarscope( plot );
 read_covar_matrix( plot );
 plot_covar_matrix( plot );
 if( plot->driver == 'P' ) mvps_to("covar.eps");

 init_gscope( plot );
 open_gscope( plot );
 plot_scaled_par_w_errorbar( plot );
 if( plot->driver == 'P' ) mvps_to("par_errbar.eps");


 free_plotvars( plot );

}

void psplot_Fisher_and_CRB( PlotParType *plot, char *solfilename )
{
 parse_solfile( plot, solfilename );
 init_plotvars( plot );
 read_solutions( plot, solfilename );

 init_Hessianscope( plot );
 open_Hessianscope( plot );
 read_Fisher_matrix( plot );
 plot_Hessian_matrix( plot );
 if( plot->driver == 'P' ) mvps_to("Fisher.eps");

 init_covarscope( plot );
 open_covarscope( plot );
 read_CRB_matrix( plot );
 plot_covar_matrix( plot );
 if( plot->driver == 'P' ) mvps_to("CRB.eps");

 init_gscope( plot );
 open_gscope( plot );
 plot_scaled_par_w_CRB( plot );
 if( plot->driver == 'P' ) mvps_to("par_CRB.eps");

 free_plotvars( plot );

}

void xsplot_Fisher_and_CRB( PlotParType *plot, char *solfilename )
{
 parse_solfile( plot, solfilename );
 init_plotvars( plot );
 read_solutions( plot, solfilename );

 init_Hessianscope( plot );
 open_Hessianscope( plot );
 read_Fisher_matrix( plot );

 init_covarscope( plot );
 open_covarscope( plot );
 read_CRB_matrix( plot );

 init_gscope( plot );
 open_gscope( plot );

 plot_Hessian_matrix( plot );
 plot_covar_matrix( plot );
 plot_scaled_par_w_CRB( plot );

 free_plotvars( plot );
}

int wedgeplot_dynspec( ProjParType *proj, PlotParType *plot )
{ 
  DisplayBufParType *ds = &(proj->dispBuf);
  float TR[6] = { 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
  float XMIN, XMAX, YMIN, YMAX;
  float *vwp;
  float R, G, B;
  char colorscale[32];
  int C, STRC, SYMC;
  int i;
  float min, max;
  float x = 1.5, y = 1.5;
  char c;
  int N, M;
  int ret = 0;
  int offset = 2*ds->altswtch*(ds->nchan*ds->nrec);
  float l, h, a, g, s, r, phi, L;
  int itf = plot->itf;
  colormap *cmap;
  float *temp;
  cmap = (colormap*)calloc(MAXCOL,sizeof(colormap));

  N = ds->nchan;
  M = ds->nrec;
  
  temp = (float*)calloc(sizeof(float), 4*N*M);
  for(i = 0 ; i < 4*N*M; i++)
      temp[i] = (float)ds->databuf[i];


  if( plot->driver == 'P' || plot->driver == 'p')  
  { C = PSCOLOR;
    STRC = BLUE;
    SYMC = CRIMSON;
  }
  else  
  { C = WHITE;//COLOR;
    STRC = YELLOW;;
    SYMC = CYAN;
  }

  cpgslct( plot->DSboard );
  /*
  for(i = 0 ; i < MAXCOL ; i++)
  { R = 0.75*(float)(i)/(float)(MAXCOL) + 0.25;
    G = maxf(0.0, 2.0*(float)(i-MAXCOL/2)/(float)(MAXCOL+1));
    B = 0.0 + 0.4*(float)(MAXCOL-i)/(float)(MAXCOL);
    cpgscr(16+i, R, G, B);
  }
  */
  strcpy(colorscale, "Colour Scale : ");
  if(itf == LIN ) strcat(colorscale, "lin\0");
  else if(itf == LOG ) strcat(colorscale, "log\0");
  else if(itf == SQR ) strcat(colorscale, "sqrt\0");

  /* Dave Green's CubeHelix colour scheme 
  s = 3.0;
  r = 1.0;
  l = 1.0;
  h = HUE;
  g = GAMMA;

  for(i = 0 ; i < MAXCOL ; i++)
  { L = l*(i+1)/MAXCOL;
    a = h*pow(L,g)*(1 - pow(L,g))/2;
    phi = 2*M_PI*(s/3 + r*L);
    R = pow(L,g) + a*( -0.14861*cos(phi) + 1.78277*sin(phi) );
    G = pow(L,g) + a*( -0.29277*cos(phi) - 0.90649*sin(phi) );
    B = pow(L,g) + a*( 1.97294*cos(phi) + 0.0*sin(phi) );
    cpgscr(16+i, R, G, B);
  }
  */

  read_colormap("coolwarm.cmap", cmap);

  for(i = 0 ; i < MAXCOL ; i++)
  { R = cmap[i].r;
    G = cmap[i].g;
    B = cmap[i].b;
    cpgscr(16+i, R, G, B);
  }
  


  min = ds->min[0];
  max = ds->max[0];
  vwp = plot->DS_amp_vwp;

  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  cpgswin(0.5,N+0.5, M+0.5, 0.5);
  cpgscir(16,15+MAXCOL);  
  cpgsitf(itf);
  cpgimag( temp + offset, N, M, 1, N, 1, M, min, max, TR );
  XMIN = (float)(ds->start_chan) -0.5;
  XMAX = (float)(ds->stop_chan) + 0.5;
  YMIN = (float)(ds->start_rec) - 0.5;
  YMAX = (float)(ds->stop_rec) + 0.5 ;
  cpgsci(C);
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgbox("BCN", 0.0, 0, "BC", 0.0, 0);
  cpgwedg( "LI", 1.0, 2.5, min, max, "");
  if(!ds->altswtch)
  { cpgmtxt("L", 4.5, 0.4, 0.0, "Amplitude - Jy");
    cpgmtxt("B", 2.75, 0.42, 0.0, "Channel #");
  }
  else
  { cpgmtxt("L", 4.5, 0.42, 0.0, "Real - Jy");
    cpgmtxt("B", 2.75, 0.42, 0.0, "Channel #");
  }

  if( plot->driver == 'x' || plot->driver == 'X')
  { cpgsci(SYMC);
    cpgmtxt("T", 2.5, -0.15, 0.0, "l           g                s                  space                 q");
    cpgsci(STRC);
    cpgmtxt("T", 2.5, -0.15, 0.0, " : linear     : logarithmic     : square root           : toggle view      : quit");
  }
  min = ds->min[1];
  max = ds->max[1];
  vwp = plot->DS_phs_vwp;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  cpgswin(0.5,N+0.5, M+0.5, 0.5);
  cpgscir(16,15+MAXCOL);  
  cpgsitf(itf);
  cpgimag( temp + offset + N*M, N, M, 1, N, 1, M, min, max, TR );
  XMIN = (float)(ds->start_chan) - 0.5;
  XMAX = (float)(ds->stop_chan) + 0.5;
  YMIN = (float)(ds->start_rec) - 0.5;
  YMAX = (float)(ds->stop_rec) + 0.5 ;
  cpgsci(C);
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgbox("BCN", 0.0, 0, "BCVN", 0.0, 0);
  cpgwedg( "RI", 1.0, 2.5, min, max, "");
  if(!ds->altswtch)
  { cpgmtxt("R", 4.5, 0.4, 0.0, "Phase - deg");
    cpgmtxt("B", 2.75, 0.42, 0.0, "Channel #");
  }
  else
  { cpgmtxt("R", 4.5, 0.42, 0.0, "Imag - Jy");
    cpgmtxt("B", 2.75, 0.42, 0.0, "Channel #");
  }

  cpgmtxt("L", 1.4, 0.92, 0.0, "<--");
  cpgmtxt("T", 0.5, -0.09, 0.0, "Rec #");

  if( plot->driver == 'X')
  { cpgsci(STRC);
    cpgmtxt("T", 2.5, 0.85, 0.0, colorscale);

    ret = cpgcurs(&x, &y, &c);
    if( c == 'q' ) ret = 0;
    if( c == 'l' ) { itf = LIN;  ret = 1; }
    if( c == 'g' ) { itf = LOG;  ret = 1; }
    if( c == 's' ) { itf = SQR;  ret = 1; }
    if( c == ' ' ) { ds->altswtch = !(ds->altswtch); ret = 1; }
    plot->itf = itf;
  }
  
  cpgclos();  

  return ret;
}


int wedgeplot_instspec( ProjParType *proj, PlotParType *plot )
{ 
  DisplayBufParType *ds = &(proj->dispBuf);
  float TR[6] = { 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
  float XMIN, XMAX, YMIN, YMAX;
  float *vwp;
  float R, G, B;
  int C, SYMC, STRC;
  int i;
  float min, max;
  float x = 1.5, y = 1.5;
  char c;
  char colorscale[32];
  int N, M;
  int ret = 1;
  int offset = 2*ds->altswtch*(proj->baselines * proj->chans);
  float l, h, a, g, s, r, phi, L;
  int itf = plot->itf;
  colormap *cmap;
  float *temp;

  cmap = (colormap*)calloc(MAXCOL,sizeof(colormap));

  N = proj->chans;
  M = proj->baselines;

  temp = (float*)calloc(sizeof(float), 4*N*M);
  for(i = 0 ; i < 4*N*M; i++)
      temp[i] = (float)ds->databuf[i];
  
  if( plot->driver == 'P' || plot->driver == 'p')  
  { C = PSCOLOR;
    SYMC = MAGENTA;
    STRC = BLUE;
  }
  else  
  { C = WHITE;//COLOR;
    SYMC = CYAN;
    STRC = YELLOW;
  }

  cpgslct( plot->DSboard );
  /*
  for(i = 0 ; i < MAXCOL ; i++)
  { R = 0.75*(float)(i)/(float)(MAXCOL) + 0.25;
    G = maxf(0.0, 2.0*(float)(i-MAXCOL/2)/(float)(MAXCOL+1));
    B = 0.0 + 0.4*(float)(MAXCOL-i)/(float)(MAXCOL);
    cpgscr(16+i, R, G, B);
  }
  */
  /* Dave Green's CubeHelix colour scheme 
  s = 3.0;
  r = 1.0;
  l = 1.0;
  h = HUE;
  g = GAMMA;

  for(i = 0 ; i < MAXCOL ; i++)
  { L = l*(i+1)/MAXCOL;
    a = h*pow(L,g)*(1 - pow(L,g))/2;
    phi = 2*M_PI*(s/3 + r*L);
    R = pow(L,g) + a*( -0.14861*cos(phi) + 1.78277*sin(phi) );
    G = pow(L,g) + a*( -0.29277*cos(phi) - 0.90649*sin(phi) );
    B = pow(L,g) + a*( 1.97294*cos(phi) + 0.0*sin(phi) );
    cpgscr(16+i, R, G, B);
  }
  */

  read_colormap("coolwarm.cmap", cmap);
  //read_colormap("summerlilac.cmap", cmap);
  //read_colormap("winterbeige.cmap", cmap);

  for(i = 0 ; i < MAXCOL ; i++)
  { R = cmap[i].r;
    G = cmap[i].g;
    B = cmap[i].b;
    cpgscr(16+i, R, G, B);
  }
  

  min = ds->min[0];
  max = ds->max[0];
  vwp = plot->DS_amp_vwp;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  cpgsci(C);
  XMIN = 0.5;
  XMAX = (float)(N) + 0.5;
  YMIN = 0.5;
  YMAX = (float)(M) + 0.5;
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  cpgbox("BCN", 0.0, 0, "BC", 0.0, 0);
  cpgscir(16,15+MAXCOL);  
  cpgsitf(itf);
  cpgimag( temp + offset, N, M, 1, N, 1, M, min, max, TR );
  cpgwedg( "LI", 1.0, 2.5, min, max, "");
  if(!ds->altswtch)
  { cpgmtxt("L", 4.5, 0.4, 0.0, "Amplitude - Jy");
    cpgmtxt("B", 2.75, 0.42, 0.0, "Channel #");
  }
  else
  { cpgmtxt("L", 4.5, 0.42, 0.0, "Real - Jy");
    cpgmtxt("B", 2.75, 0.42, 0.0, "Channel #");
  }
 
if( plot->driver == 'x' || plot->driver == 'X')
  { cpgsci(SYMC);
    cpgmtxt("T", 2.5, -0.15, 0.0, "l           g                s                  space                 q");
    cpgsci(STRC);
    cpgmtxt("T", 2.5, -0.15, 0.0, " : linear     : logarithmic     : square root           : toggle view      : quit");
  } 

  min = ds->min[1];
  max = ds->max[1];
  vwp = plot->DS_phs_vwp;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  cpgsci(C);
  XMIN = 0.5;
  XMAX = (float)(N) + 0.5;
  YMIN = 0.5;
  YMAX = (float)(M) + 0.5;
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  cpgbox("BCN", 0.0, 0, "BCVN", 0.0, 0);
  cpgscir(16,15+MAXCOL);  
  cpgsitf(itf);
  cpgimag( temp + offset + N*M, N, M, 1, N, 1, M, min, max, TR );
  cpgwedg( "RI", 1.0, 2.5, min, max, "");
  if(!ds->altswtch)
  { cpgmtxt("R", 4.5, 0.4, 0.0, "Phase - deg");
    cpgmtxt("B", 2.75, 0.42, 0.0, "Channel #");
  }
  else
  { cpgmtxt("R", 4.5, 0.42, 0.0, "Imag - Jy");
    cpgmtxt("B", 2.75, 0.42, 0.0, "Channel #");
  }

  cpgmtxt("L", 1.4, 0.92, 0.0, "<--");
  cpgmtxt("T", 0.5, -0.125, 0.0, "Baseline #");

  cpgsch(0.8);
  strcpy(colorscale, "Colour Scale : ");
  if(itf == LIN ) strcat(colorscale, "lin\0");
  else if(itf == LOG ) strcat(colorscale, "log\0");
  else if(itf == SQR ) strcat(colorscale, "sqrt\0");
  cpgsci(STRC);
  cpgmtxt("T", 2.5, 0.8, 0.0, colorscale);

  if( ret == 1  && plot->driver == 'X')
  {
    ret = cpgcurs(&x, &y, &c);
    if( c == 'q' ) ret = 0;
    else if( c == 'l' ) { plot->itf = LIN;  ret = 1; }
    else if( c == 'g' ) { plot->itf = LOG;  ret = 1; }
    else if( c == 's' ) { plot->itf = SQR;  ret = 1; }
    else if( c == ' ' ) { ds->altswtch = !(ds->altswtch); ret = 1; }
  }

  if( plot->driver == 'P' || plot->driver == 'p') ret = 0;
  cpgclos();  

  return ret;
}



int wedgeplot_beamdata( ProjParType *proj, PlotParType *plot )
{ 
  DisplayBufParType *ds = &(proj->dispBuf);
  float TR[6] = { 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
  float XMIN, XMAX, YMIN, YMAX;
  float *vwp;
  float R, G, B;
  int C;
  int i;
  float min, max;
  float x = 1.5, y = 1.5;
  char c;
  int N, M;
  int ret = 0;
  int offset = 2*ds->altswtch*(proj->nrecs/proj->preint) * proj->chans;
  float l, h, a, g, s, r, phi, L;
  colormap *cmap;
  float *temp;

  cmap = (colormap*)calloc(MAXCOL, sizeof(colormap));

  N = proj->chans;
  M = proj->nrecs/proj->preint;

  temp = (float*)calloc(sizeof(float), 4*N*M);
  
  if( plot->driver == 'P' || plot->driver == 'p')  
  C = PSCOLOR;
  else  
  C = WHITE;//COLOR;
  cpgslct( plot->DSboard );
  /*
  for(i = 0 ; i < MAXCOL ; i++)
  { R = 0.75*(float)(i)/(float)(MAXCOL) + 0.25;
    G = maxf(0.0, 2.0*(float)(i-MAXCOL/2)/(float)(MAXCOL+1));
    B = 0.0 + 0.4*(float)(MAXCOL-i)/(float)(MAXCOL);
    cpgscr(16+i, R, G, B);
  }
  */
  /* Dave Green's CubeHelix colour scheme 
  s = 3.0;
  r = 1.0;
  l = 1.0;
  h = HUE;
  g = GAMMA;

  for(i = 0 ; i < MAXCOL ; i++)
  { L = l*(i+1)/MAXCOL;
    a = h*pow(L,g)*(1 - pow(L,g))/2;
    phi = 2*M_PI*(s/3 + r*L);
    R = pow(L,g) + a*( -0.14861*cos(phi) + 1.78277*sin(phi) );
    G = pow(L,g) + a*( -0.29277*cos(phi) - 0.90649*sin(phi) );
    B = pow(L,g) + a*( 1.97294*cos(phi) + 0.0*sin(phi) );
    cpgscr(16+i, R, G, B);
  }
  */

  read_colormap("coolwarm.cmap", cmap);

  for(i = 0 ; i < MAXCOL ; i++)
  { R = cmap[i].r;
    G = cmap[i].g;
    B = cmap[i].b;
    cpgscr(16+i, R, G, B);
  }
  

  min = ds->min[0];
  max = ds->max[0];
  vwp = plot->DS_amp_vwp;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  cpgsci(C);
  XMIN = 0.5;
  XMAX = (float)(N) + 0.5;
  YMIN = 0.5;
  YMAX = (float)(M) + 0.5;
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  cpgbox("BCN", 0.0, 0, "BC", 0.0, 0);
  cpgscir(16,15+MAXCOL);  
  cpgsitf(2);
  cpgimag( temp + offset, N, M, 1, N, 1, M, min, max, TR );
  cpgwedg( "LI", 1.0, 2.5, min, max, "");
  if(!ds->altswtch)
  { cpgmtxt("L", 4.5, 0.4, 0.0, "Amplitude - Jy");
    cpgmtxt("B", 2.75, 0.42, 0.0, "Channel #");
  }
  else
  { cpgmtxt("L", 4.5, 0.42, 0.0, "Real - Jy");
    cpgmtxt("B", 2.75, 0.42, 0.0, "Channel #");
  }
  
  if( plot->driver == 'x' || plot->driver == 'X')
  { cpgsci(YELLOW);
    cpgmtxt("T", 2.5, -0.15, 0.0, "any key to toggle / q to quit");
    cpgsci(C);
  }

  min = ds->min[1];
  max = ds->max[1];
  vwp = plot->DS_phs_vwp;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  cpgsci(C);
  XMIN = 0.5;
  XMAX = (float)(N) + 0.5;
  YMIN = 0.5;
  YMAX = (float)(M) + 0.5;
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  cpgbox("BCN", 0.0, 0, "BCVN", 0.0, 0);
  cpgscir(16,15+MAXCOL);  
  cpgsitf(2);
  cpgimag( temp + offset + N*M, N, M, 1, N, 1, M, min, max, TR );
  cpgwedg( "RI", 1.0, 2.5, min, max, "");
  if(!ds->altswtch)
  { cpgmtxt("R", 4.5, 0.4, 0.0, "Phase - deg");
    cpgmtxt("B", 2.75, 0.42, 0.0, "Channel #");
  }
  else
  { cpgmtxt("R", 4.5, 0.42, 0.0, "Imag - Jy");
    cpgmtxt("B", 2.75, 0.42, 0.0, "Channel #");
  }

  cpgmtxt("L", 1.4, 0.92, 0.0, "<--");
  cpgmtxt("T", 0.5, -0.09, 0.0, "Rec #");
  
  ds->altswtch = !(ds->altswtch);

  if( plot->driver == 'X' )
  { ret = cpgcurs(&x, &y, &c);
    if( c == 'q' )
    { ret = 0;
      ds->altswtch = !(ds->altswtch);
    }
  }

  cpgclos();  

  return ret;
}

int wedgeplot_dynvis( ProjParType *proj, PlotParType *plot )
{ 
  DisplayBufParType *ds = &(proj->dispBuf);
  float TR[6] = { 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
  float XMIN, XMAX, YMIN, YMAX;
  float *vwp;
  float R, G, B;
  int C;
  int i;
  float min, max;
  float x = 1.5, y = 1.5;
  char c;
  int N, M;
  int ret = 0;
  int offset = 2*ds->altswtch*(proj->baselines * proj->nrecs);
  float l, h, a, g, s, r, phi, L;
  colormap *cmap;
  float *temp;

  cmap = (colormap*)calloc(MAXCOL, sizeof(colormap));

  N = proj->nrecs;
  M = proj->baselines;

  temp = (float*) calloc(sizeof(float), 4*N*M);
    for(i = 0 ; i < 4*N*M; i++)
      temp[i] = (float)ds->databuf[i];

  if( plot->driver == 'P' || plot->driver == 'p')  
  C = PSCOLOR;
  else  
  C = WHITE;//COLOR;
  cpgslct( plot->DSboard );
  /*
  for(i = 0 ; i < MAXCOL ; i++)
  { R = 0.75*(float)(i)/(float)(MAXCOL) + 0.25;
    G = maxf(0.0, 2.0*(float)(i-MAXCOL/2)/(float)(MAXCOL+1));
    B = 0.0 + 0.4*(float)(MAXCOL-i)/(float)(MAXCOL);
    cpgscr(16+i, R, G, B);
  }
  */
  /* Dave Green's CubeHelix colour scheme 
  s = 3.0;
  r = 1.0;
  l = 1.0;
  h = HUE;
  g = GAMMA;

  for(i = 0 ; i < MAXCOL ; i++)
  { L = l*(i+1)/MAXCOL;
    a = h*pow(L,g)*(1 - pow(L,g))/2;
    phi = 2*M_PI*(s/3 + r*L);
    R = pow(L,g) + a*( -0.14861*cos(phi) + 1.78277*sin(phi) );
    G = pow(L,g) + a*( -0.29277*cos(phi) - 0.90649*sin(phi) );
    B = pow(L,g) + a*( 1.97294*cos(phi) + 0.0*sin(phi) );
    cpgscr(16+i, R, G, B);
  }
  */

  read_colormap("coolwarm.cmap", cmap);

  for(i = 0 ; i < MAXCOL ; i++)
  { R = cmap[i].r;
    G = cmap[i].g;
    B = cmap[i].b;
    cpgscr(16+i, R, G, B);
  }
  

  min = ds->min[0];
  max = ds->max[0];
  vwp = plot->DS_amp_vwp;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  cpgsci(C);
  XMIN = 0.5;
  XMAX = (float)(N) + 0.5;
  YMIN = 0.5;
  YMAX = (float)(M) + 0.5;
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  cpgbox("BCN", 0.0, 0, "BC", 0.0, 0);
  cpgscir(16,15+MAXCOL);  
  cpgsitf(0);
  cpgimag( temp + offset, N, M, 1, N, 1, M, min, max, TR );
  cpgwedg( "LI", 1.0, 2.5, min, max, "");
  if(!ds->altswtch)
  { cpgmtxt("L", 4.5, 0.4, 0.0, "Amplitude - Jy");
    cpgmtxt("B", 2.75, 0.5, 0.5, "Rec #");
  }
  else
  { cpgmtxt("L", 4.5, 0.42, 0.0, "Real - Jy");
    cpgmtxt("B", 2.75, 0.5, 0.5, "Rec #");
  }
  
  if( plot->driver == 'x' || plot->driver == 'X')
  { cpgsci(YELLOW);
    cpgmtxt("T", 2.5, -0.15, 0.0, "any key to toggle / q to quit");
    cpgsci(C);
  }

  min = ds->min[1];
  max = ds->max[1];
  vwp = plot->DS_phs_vwp;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  cpgsci(C);
  XMIN = 0.5;
  XMAX = (float)(N) + 0.5;
  YMIN = 0.5;
  YMAX = (float)(M) + 0.5;
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  cpgbox("BCN", 0.0, 0, "BCVN", 0.0, 0);
  cpgscir(16,15+MAXCOL);  
  cpgsitf(0);
  cpgimag( temp + offset + N*M, N, M, 1, N, 1, M, min, max, TR );
  cpgwedg( "RI", 1.0, 2.5, min, max, "");
  if(!ds->altswtch)
  { cpgmtxt("R", 4.5, 0.4, 0.0, "Phase - deg");
    cpgmtxt("B", 2.75, 0.5, 0.5, "Rec #");
  }
  else
  { cpgmtxt("R", 4.5, 0.42, 0.0, "Imag - Jy");
    cpgmtxt("B", 2.75, 0.5, 0.5, "Rec #");
  }

  cpgmtxt("L", 1.4, 0.92, 0.0, "<--");
  cpgmtxt("T", 0.5, -0.125, 0.0, "Baseline #");

  ds->altswtch = !(ds->altswtch);

  if( plot->driver == 'X' )
  { ret = cpgcurs(&x, &y, &c);
    if( c == 'q' )
    { ret = 0;
      ds->altswtch = !(ds->altswtch);
    }
  }

  cpgclos();  

  return ret;
}

int imag_dynspec( ProjParType *proj, PlotParType *plot )
{
 init_DSscope( plot );
 open_DSscope( plot );
 return wedgeplot_dynspec( proj, plot ) ;
}

int imag_instspec( ProjParType *proj, PlotParType *plot )
{
 init_DSscope( plot );
 open_DSscope( plot );
 return wedgeplot_instspec( proj, plot ) ;
}


int imag_modelvis( ProjParType *proj, PlotParType *plot )
{
 init_DSscope( plot );
 open_DSscope( plot );
 return wedgeplot_modelvis( proj, plot ) ;
}

int imag_beamdata( ProjParType *proj, PlotParType *plot )
{
 init_DSscope( plot );
 open_DSscope( plot );
 return wedgeplot_beamdata( proj, plot ) ;
}

int imag_dynvis( ProjParType *proj, PlotParType *plot )
{
 init_DSscope( plot );
 open_DSscope( plot );
 return wedgeplot_dynvis( proj, plot ) ;
}

int plot_sky_model( ProjParType *proj, PlotParType *plot )
{
 init_scalescope( plot );
 return sky_model( proj, plot );
}

int sky_model( ProjParType *proj, PlotParType *plot )
{
 DisplayBufParType *ds = &(proj->dispBuf);
 int bl;
 int baselines = proj->redundant_baselines + 1;
 int chans = proj->chans;
 float mag[chans*baselines], phase[chans*baselines];
 float re[chans*baselines], im[chans*baselines];
 float UL[chans*baselines];
 float x[baselines];
 float a,b;
 char c;
 char colorscale[32];
 int symbol = -2;
 float *vwp;
 CmplxType MV;
 float XMIN, XMAX, YMIN, YMAX;
 int G, M, C, LC;
 float SIDE = VWPSIDE, SQUARE = 1.0;
 int ret = 1;
 float min, max;
 int itf = plot->itf;
 int ch = chans/2;

 if( plot->driver == 'P' )
 { C = PSCOLOR;
   G = BLUE;
   M = MINIMAL_GREY;
   LC = MEDIUM_GREY;
   plot->scale_board = cpgopen("/cps");
 }
 else if ( plot->driver == 'X' )
 { G  = GScale;
   M  = MEDIUM_GREY;
   C  = PSCOLOR;
   LC = MINIMAL_GREY;
   plot->scale_board = cpgopen("/xs");
 }
 cpgpap(1.68*SIDE, SQUARE);

 
 for( bl = 0 ; bl < baselines ; bl++ )
 { for(ch = 0 ; ch < chans ; ch++ ) {
   MV = proj->model_vis[ch][bl];
   UL[ch*baselines + bl] = (float)proj->ULambda[ch][bl];
   re[ch*baselines + bl] = (float)real(MV);
   im[ch*baselines + bl] = (float)imag(MV);
   if (itf == LIN) mag[ch*baselines + bl] = (float)z_abs(MV);
   else if(itf == LOG) mag[ch*baselines + bl] = (float)log10(z_abs(MV));
   phase[ch*baselines + bl] = (float)rad2deg(z_arg(MV));
   }
   x[bl] = (float)(bl+1);
 }
 /************ Amplitude - line plot ***********/
 vwp = plot->ampscale_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 cpgsch(1.5);
 XMIN = 0.0;
 XMAX =  arraymaxf(UL, baselines*chans) + 10.0;;
 if(!ds->altswtch)
 { if(itf == LOG) 
   { min = arrayminf(mag, baselines*chans);
     max = arraymaxf(mag, baselines*chans);
     YMIN = min - signum(min)*0.15*min;
     YMAX = max + signum(max)*0.15*max;
   }
   else 
   { YMIN =  -0.15 * arraymaxf( mag, baselines*chans );
     YMAX =  1.15 * arraymaxf( mag, baselines*chans );
   }
 }

 else
 { min = arrayminf(re, baselines*chans);
   max = arraymaxf(re, baselines*chans);
   YMIN = min - signum(min)*0.35*min;
   YMAX = max + signum(max)*0.35*max;
 }
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgslw(5.0);
 if(itf == LOG && !ds->altswtch) cpgbox("BCST", 0.0, 0, "BCNSTVL2", 0.0, 0);
 else cpgbox("BCST", 0.0, 0, "BCNSTV", 0.0, 0);
 cpgslw(1.0);
 cpgsci(LC);
 /*
 for(ch = 0 ; ch < chans ; ch++ )
 { if(!ds->altswtch) cpgline(baselines, UL+ch*baselines, mag+ch*baselines);
   else cpgline(baselines, UL+ch*baselines, re+ch*baselines);
 }
 */
 cpgsci(M);  
 cpgsch(1.5);
 cpgsci(C);
 cpgslw(5.0);
 //cpgmtxt("T",2.0, 0.5, 0.5, "\\fn\\(2021)  \\fn\\(2137)");
 if(!ds->altswtch) cpgmtxt("R",1.6, 0.5, 0.5, "Amplitude - Jy");
 else cpgmtxt("R",1.6, 0.5, 0.5, "Real - Jy");
 /************** Phase - line plot *********/
 cpgslw(1.0);
 cpgsch(1.5);
 vwp = plot->phoffset_vwp;
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 cpgsci(C);
 XMIN = 0.0;
 XMAX =  arraymaxf(UL, baselines*chans) + 10.0;
 if(!ds->altswtch)
 { YMIN = -250.0;
   YMAX =  250.0;
 }
 else
 { min = arrayminf(im, baselines);
   max = arraymaxf(im, baselines);
   YMIN = min - signum(min)*0.35*min;
   YMAX = max + signum(max)*0.35*max;
 }
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgslw(5.0);
 cpgbox("BCNST", 0.0, 0, "BCNSTV", 0.0, 0);
 cpgslw(1.0);
 cpgsci(LC);
 /*
 for( ch = 0 ; ch < chans ; ch++ ) 
{ //if(!ds->altswtch) cpgline(baselines, UL+ch*baselines, phase+ch*baselines);
    if(ds->altswtch) cpgline(baselines, UL+ch*baselines, im+ch*baselines);
 }
 */
 cpgsci(M);  
 cpgsch(1.5);
 cpgsci(C);
 cpgslw(5.0);
 cpgmtxt("B",3.0, 0.50, 0.5, "\\fn\\(2021)  \\fn\\(2137)");
 if(!ds->altswtch) cpgmtxt("R", 1.6, 0.5, 0.5, "Phase - degrees");
 else cpgmtxt("R",1.6, 0.5, 0.5, "Imag - Jy");
 cpgslw(1.0);
 cpgsch(0.84);

 /************ Point plot ********/
  vwp = plot->ampscale_vwp;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  cpgsci(C);
  XMIN = 0.0;
  XMAX =  arraymaxf(UL, baselines*chans) + 10.0;
  if(!ds->altswtch)
  { if(itf == LOG) 
   { min = arrayminf(mag, baselines*chans);
     max = arraymaxf(mag, baselines*chans);
     YMIN = min - signum(min)*0.15*min;
     YMAX = max + signum(max)*0.15*max;
   }
   else 
   { YMIN =  -0.15 * arraymaxf( mag, baselines*chans );
     YMAX =  1.15 * arraymaxf( mag, baselines*chans );
   }
  }
  else
  { min = arrayminf(re, baselines*chans);
    max = arraymaxf(re, baselines*chans);
    YMIN = min - signum(min)*0.35*min;
    YMAX = max + signum(max)*0.35*max;
  }
  cpgswin(XMIN, XMAX, YMIN, YMAX);
  cpgsci(LC);
  cpgsci(M);  cpgsch(0.84);

  cpgslw(10);
  if(!ds->altswtch) cpgpt(chans*baselines, UL, mag, symbol);
  else cpgpt(chans*baselines, UL, re, symbol);
  cpgslw(1);

  /******** Phase - point plot **********/
  vwp = plot->phoffset_vwp;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  cpgsci(C);
  XMIN = 0.0;
  XMAX =  arraymaxf(UL, baselines*chans) + 10.0;
  if(!ds->altswtch)
  { YMIN = -250.0;
    YMAX =  250.0;
  }
  else
  { min = arrayminf(im, baselines*chans);
    max = arraymaxf(im, baselines*chans);
    YMIN = min - signum(min)*0.35*min;
    YMAX = max + signum(max)*0.35*max;
  }
  cpgswin(XMIN, XMAX, YMIN, YMAX);
  cpgsci(LC);
  cpgsci(M);  cpgsch(0.84);

  cpgslw(10);
  if(!ds->altswtch) cpgpt(chans*baselines, UL, phase, symbol);
  else cpgpt(chans*baselines, UL, im, symbol);
  cpgslw(1);
   /*********************/

  if( plot->driver == 'x' || plot->driver == 'X')
  { if( !ds->altswtch )
    { strcpy(colorscale, "Ampl Y Scale : ");
      if(itf == LIN ) strcat(colorscale, "lin\0");
      else if(itf == LOG ) strcat(colorscale, "log\0");
      cpgsci(YELLOW);
      cpgsch(0.6); 
      cpgsci(CYAN);
      cpgmtxt("B", 10.0, -0.08, 0.0, "l           g                q");
      cpgsci(G);
      cpgmtxt("B", 10.0, -0.08, 0.0, " : linear     : logarithmic     : quit / any other key to toggle");
      cpgsci(G);
      cpgmtxt("B", 10.0, 0.8, 0.0, colorscale);
      ret = cpgcurs(&a, &b, &c);
      if( c == 'l') plot->itf = LIN;
      else if( c == 'g') plot->itf = LOG;
      else if( c == 'q' ) ret = 0;
      else ds->altswtch = !ds->altswtch;
    }
    else if( ds->altswtch )
    { strcpy(colorscale, "Y Scale : lin");
      cpgsci(YELLOW);
      cpgsch(0.6); 
      cpgsci(CYAN);
      cpgmtxt("B", 10.0, -0.08, 0.0, "q");
      cpgsci(G);
      cpgmtxt("B", 10.0, -0.08, 0.0, "  : quit / any other key to toggle");
      cpgsci(G);
      cpgmtxt("B", 10.0, 0.8, 0.0, colorscale);

      ret = cpgcurs(&a, &b, &c);
      if( c == 'q' ) ret = 0;
      else ds->altswtch = !ds->altswtch;
    }
  }
  else if(plot->driver == 'P') ret = 0;

  cpgclos();  

  return ret;

}

int xs_nplot_chanvis( ProjParType *proj )
{ 
  float SIDE = XSIDE*VWPSIDE, SQUARE = YSIDE;
  DisplayBufParType *ds = &(proj->dispBuf);
  float XMIN, XMAX, YMIN, YMAX;
  float vwp[4];
  int C, LC, SYMC, STRC ;
  int i, j, npt;
  long ii;
  float min, max;
  float x = 1.5, y = 1.5;
  char c;
  int bl = 0;
  int nrow = ds->nrow, ncol = ds->ncol, nplots = nrow*ncol ;
  int ret = 1;
  int offset, start;
  int circnt = 0;
  char blstr[16];
  int symbol = 5;
  float R, G, B;
  static int scale = 0;
  int incr = 1;
  float *temp = (float*)calloc(4*proj->baselines*proj->chans, sizeof(float));
  float *axtemp = (float*)calloc(proj->chans, sizeof(float));

  for( ii = 0 ; ii < 4*proj->baselines*proj->chans; ii++ ) 
       temp[ii] = (float)ds->databuf[ii];

  for( ii = 0 ; ii < proj->chans; ii++ ) 
      axtemp[ii] = (float)ds->xaxis[ii];
  
  offset = proj->baselines*proj->chans;  
  start  = 2*ds->altswtch*proj->baselines*proj->chans;
  scale = proj->scale;

  cpgopen("/xs");
  C = YELLOW;
  LC = MINIMAL_GREY;
  SYMC = CYAN;
  STRC = YELLOW;
  
  cpgpap(SIDE,SQUARE);

  while(ret == 1)
  { 
    i = circnt % nrow;
    j = (circnt / nrow) % ncol;
    incr = 1;
    circnt++;
    if( i == 0 && j == 0) cpgeras();
    sprintf(blstr, "%5d : %s-%s", bl+1, proj->Base[bl].ant1, proj->Base[bl].ant2);

    /* Amplitude or real part */
    min = (float)arraymin(ds->databuf+start + bl*proj->chans, proj->chans);
    max = (float)arraymax(ds->databuf+start + bl*proj->chans, proj->chans);
    vwp[0] = 0.11 + j*(0.92-0.08)/ncol ;
    vwp[1] = 0.08 + (j+1)*(0.92-0.08)/ncol;
    vwp[2] = 0.99 - (i+0.5)*(0.99-0.08)/nrow;
    vwp[3] = 0.99 - i*(0.99-0.08)/nrow;

    XMIN = ds->xaxis[0];
    XMAX = ds->xaxis[proj->chans-1];
    YMIN = min - signum(min)*0.1*min;
    YMAX = max + signum(max)*0.1*max;
    if (!ds->altswtch && scale)
    { YMIN = 0;
      YMAX = 2*max;	
    }
    if( ds->altswtch && scale )
    { YMIN = min - signum(min)*3.0*min;
      YMAX = max + signum(max)*3.0*max;
    }
    cpgsci(C);
    cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
    cpgsch(0.95 - nrow*0.1);
    cpgswin(XMIN, XMAX, YMIN, YMAX);
    cpgbox("BC", 0.0, 0, "BCVN", 0.0, 0);
    cpgsci(LC);
    cpgline( proj->chans, axtemp, temp+start + bl*proj->chans);
    for( npt = 0 ; npt < proj->chans ; npt++ )
    { cpgsch(0.5); cpgsci(SYMC);
      cpgpt1( axtemp[npt], temp[start + bl*proj->chans + npt], symbol);
    }
    cpgsci(STRC);
    cpgsch(0.95 - nrow*0.1);
    cpgmtxt("T", -1.5, 0.02, 0.0, blstr);
    if(j == ncol-1 && !ds->altswtch) cpgmtxt("R", 1.5, 0.5, 0.5, "Ampl-Jy");
    else if( j == ncol-1 && ds->altswtch) cpgmtxt("R", 1.5, 0.5, 0.5, "Real - Jy");

    /* Phase or imaginary part */
    min = (float)arraymin(ds->databuf+start + bl*proj->chans + offset, proj->chans);
    max = (float)arraymax(ds->databuf+start + bl*proj->chans + offset, proj->chans);
    vwp[0] = 0.11 + j*(0.92-0.08)/ncol ;
    vwp[1] = 0.08 + (j+1)*(0.92-0.08)/ncol;
    vwp[2] = 0.99 - (i+1.0)*(0.99-0.08)/nrow;
    vwp[3] = 0.99 - (i+0.5)*(0.99-0.08)/nrow;

    XMIN = ds->xaxis[0];
    XMAX = ds->xaxis[proj->chans-1];
    YMIN = min - signum(min)*0.1*min;
    YMAX = max + signum(max)*0.1*max;
    if (!ds->altswtch && scale)
    { YMIN = -199.0;
      YMAX = 199.1;	
    }
    if( ds->altswtch && scale )
    { YMIN = min - signum(min)*3.0*min;
      YMAX = max + signum(max)*3.0*max;
    }
    cpgsch(0.95 - nrow*0.1);
    cpgsci(C);
    cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
    cpgswin(XMIN, XMAX, YMIN, YMAX);
    cpgsci(LC);
    cpgline( proj->chans, axtemp, temp+start + bl*proj->chans + offset);
    for( npt = 0 ; npt < proj->chans ; npt++ )
    { cpgsch(0.5); cpgsci(SYMC);
      cpgpt1( axtemp[npt], temp[start + bl*proj->chans + offset + npt], symbol);
    }
    cpgsci(STRC);
    cpgsch(0.95 - nrow*0.1);
    if(j == ncol-1 && !ds->altswtch) cpgmtxt("R", 1.5, 0.5, 0.5, "Phi-deg");
    else if( j == ncol-1 && ds->altswtch) cpgmtxt("R", 1.5, 0.5, 0.5, "Imag - Jy");
    if (i == nrow-1) 
    { cpgbox("BCN", 0.0, 0, "BCVN", 0.0, 0);
      cpgsch(0.95 - nrow*0.1); cpgmtxt("B", 3.0, 0.5, 0.5, "Channel #");
    }
    else cpgbox("BC", 0.0, 0, "BCVN", 0.0, 0);

    if( i == nrow-1 && j == 0)
    {cpgsch(0.6); 
     cpgsci(SYMC);
     cpgmtxt("B", 5.0, -0.08, 0.0, "n                 p                  s                   m                   q");
     cpgsci(STRC);
     cpgmtxt("B", 5.0, -0.08, 0.0, "  - next page      - prev. page      - toggle scale      - toggle mode     - quit");
    }
    /* Next page or quit for X-window */
    if( !(circnt%nplots) )
    {
     ret = cpgcurs(&x, &y, &c);
     if( c == 'q' ) ret = 0;
     if( c == 's' ) { scale = !scale; }
     if( c == 'm' ) { ds->altswtch = !ds->altswtch; 
                      start  = 2*ds->altswtch*proj->baselines*proj->chans;}
     if( c == 'p' ) { incr = -(nplots)+1; ret = 1;}
     if( c == 'n' ) { incr = 1; ret = 1;}
     else { bl = bl - nplots; }
    }
    bl = bl + incr;
    bl = (bl + proj->baselines) % proj->baselines;
    if(ret == 0) break;
  }
  cpgclos();  

  return ret;
}


int ps_nplot_chanvis( ProjParType *proj)
{ 
  float SIDE = XSIDE*VWPSIDE, SQUARE = YSIDE;
  DisplayBufParType *ds = &(proj->dispBuf);
  float XMIN, XMAX, YMIN, YMAX;
  float vwp[4];
  int C, LC, SYMC, STRC ;
  int i, j, npt;
  long ii;
  float min, max;
  float x = 1.5, y = 1.5;
  char c;
  int bl = 0;
  int nrow = ds->nrow, ncol = ds->ncol;
  int nplots = nrow * ncol;
  int offset, start;
  char blstr[16];
  int symbol = 5;
  float R, G, B;
  static int scale = 0;
  float *temp = (float*)calloc(sizeof(float), sizeof(ds->databuf)/sizeof(double));
  float *axtemp = (float*)calloc(sizeof(float), sizeof(ds->xaxis)/sizeof(double));
  for( ii = 0 ; ii < sizeof(ds->databuf)/sizeof(double); ii++ ) 
  { temp[ii] = (float)ds->databuf[ii];
  }

  for( ii = 0 ; ii < sizeof(ds->xaxis)/sizeof(double); ii++ ) 
  { axtemp[ii] = (float)ds->xaxis[ii];
  }

  offset = proj->baselines*proj->chans;
  start  = 2*ds->altswtch*proj->baselines*proj->chans;
  scale = proj->scale;

  cpgopen("/cps");
  C = BLUE;
  LC = MEDIUM_GREY;
  SYMC = CRIMSON;
  STRC = BLUE;

  cpgpap(SIDE,SQUARE);

  for( bl = 0 ; bl < proj->baselines; bl++ )
  { 
    i = bl % nrow;
    j = (bl / nrow) % ncol;    

    if( i == 0 && j == 0 ) cpgpage();
    sprintf(blstr, "%5d : %s-%s", bl+1, proj->Base[bl].ant1, proj->Base[bl].ant2);

    /* Amplitude or real part */
    min = arraymin(ds->databuf+start + bl*proj->chans, proj->chans);
    max = arraymax(ds->databuf+start + bl*proj->chans, proj->chans);
    vwp[0] = 0.11 + j*(0.92-0.08)/ncol ;
    vwp[1] = 0.08 + (j+1)*(0.92-0.08)/ncol;
    vwp[2] = 0.99 - (i+0.5)*(0.99-0.08)/nrow;
    vwp[3] = 0.99 - i*(0.99-0.08)/nrow;

    XMIN = ds->xaxis[0];
    XMAX = ds->xaxis[proj->chans-1];
    YMIN = min - signum(min)*0.1*min;
    YMAX = max + signum(max)*0.1*max;
    if (!ds->altswtch && scale)
    { YMIN = 0;
      YMAX = 2*max;	
    }
    if( ds->altswtch && scale )
    { YMIN = min - signum(min)*3.0*min;
      YMAX = max + signum(max)*3.0*max;
    }

    cpgsci(C);
    cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
    cpgsch(0.95 - nrow*0.1);
    cpgswin(XMIN, XMAX, YMIN, YMAX);
    cpgbox("BC", 0.0, 0, "BCVN", 0.0, 0);
    cpgsci(LC);
    cpgline( proj->chans, axtemp, temp+start + bl*proj->chans);
    for( npt = 0 ; npt < proj->chans ; npt++ )
    { cpgsch(0.5); cpgsci(SYMC);
      cpgpt1( axtemp[npt], temp[start + bl*proj->chans + npt], symbol);
    }
    cpgsci(STRC);
    cpgsch(0.95 - nrow*0.1);
    cpgmtxt("T", -1.5, 0.02, 0.0, blstr);
    if(j == ncol-1 && !ds->altswtch) cpgmtxt("R", 1.5, 0.5, 0.5, "Ampl-Jy");
    else if( j == ncol-1 && ds->altswtch) cpgmtxt("R", 1.5, 0.5, 0.5, "Real - Jy");

    /* Phase or imaginary part */
    min = arraymin(ds->databuf+start + bl*proj->chans + offset, proj->chans);
    max = arraymax(ds->databuf+start + bl*proj->chans + offset, proj->chans);
    vwp[0] = 0.11 + j*(0.92-0.08)/ncol ;
    vwp[1] = 0.08 + (j+1)*(0.92-0.08)/ncol;
    vwp[2] = 0.99 - (i+1.0)*(0.99-0.08)/nrow;
    vwp[3] = 0.99 - (i+0.5)*(0.99-0.08)/nrow;

    XMIN = ds->xaxis[0];
    XMAX = ds->xaxis[proj->chans-1];
    YMIN = min - signum(min)*0.1*min;
    YMAX = max + signum(max)*0.1*max;
    if (!ds->altswtch && scale)
    { YMIN = -199.0;
      YMAX = 199.0;	
    }
    if( ds->altswtch && scale )
    { YMIN = min - signum(min)*3.0*min;
      YMAX = max + signum(max)*3.0*max;
    }

    cpgsch(0.95 - nrow*0.1);
    cpgsci(C);
    cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
    cpgswin(XMIN, XMAX, YMIN, YMAX);
    cpgsci(LC);
    cpgline( proj->chans, axtemp, temp+start + bl*proj->chans + offset);
    for( npt = 0 ; npt < proj->chans ; npt++ )
    { cpgsch(0.5); cpgsci(SYMC);
      cpgpt1( axtemp[npt], temp[start + bl*proj->chans + offset + npt], symbol);
    }
    cpgsci(STRC);
    cpgsch(0.95 - nrow*0.1);
    if(j == ncol-1 && !ds->altswtch) cpgmtxt("R", 1.5, 0.5, 0.5, "Phi-deg");
    else if( j == ncol-1 && ds->altswtch) cpgmtxt("R", 1.5, 0.5, 0.5, "Imag - Jy");
    if (i == nrow-1) 
    { cpgbox("BCN", 0.0, 0, "BCVN", 0.0, 0);
      cpgsch(0.95 - nrow*0.1); cpgmtxt("B", 3.0, 0.5, 0.5, "Channel #");
    }
    else cpgbox("BC", 0.0, 0, "BCVN", 0.0, 0);

    /* Status for PS plots */
    if( !((bl+1)%5) )
    fprintf(finfo, "%s Plotted [ %5d / %5ld ] baselines\r", info, bl+1, proj->baselines );
  }

  cpgclos();  

  return 0;
}


int xs_nplot_timevis( ProjParType *proj)
{ 
  float SIDE = XSIDE*VWPSIDE, SQUARE = YSIDE;
  DisplayBufParType *ds = &(proj->dispBuf);
  float XMIN, XMAX, YMIN, YMAX;
  float vwp[4];
  int C, LC, SYMC, STRC ;
  int i, j, npt;
  long ii;
  float min, max;
  float x = 1.5, y = 1.5;
  char c;
  int bl = 0;
  int nrow = ds->nrow, ncol = ds->ncol, nplots = nrow*ncol ;
  int ret = 1;
  int offset, start;
  int circnt = 0;
  char blstr[16];
  int symbol = -3;
  float R, G, B;
  static int scale = 0;
  int incr = 1;
  float *temp = (float*)calloc(4*proj->baselines*proj->nrecs, sizeof(float));
  float *axtemp = (float*)calloc(proj->nrecs, sizeof(float));
  for( ii = 0 ; ii < 4*proj->baselines*proj->nrecs; ii++ ) 
   temp[ii] = (float)ds->databuf[ii];
  
  for( ii = 0 ; ii < proj->nrecs; ii++ ) 
       axtemp[ii] = (float)ds->xaxis[ii];
  
  offset = proj->baselines*proj->nrecs;  
  start  = 2*ds->altswtch*proj->baselines*proj->nrecs;
  scale = proj->scale;

  cpgopen("/xs");
  C = YELLOW;
  LC = MINIMAL_GREY;
  SYMC = CYAN;
  STRC = YELLOW;
  
  cpgpap(SIDE,SQUARE);

  while(ret == 1)
  { 
    i = circnt % nrow;
    j = (circnt / nrow) % ncol;
    incr = 1;
    circnt++;
    if( i == 0 && j == 0) cpgeras();
    sprintf(blstr, "%5d : %s-%s", bl+1, proj->Base[bl].ant1, proj->Base[bl].ant2);

    /* Amplitude or real part */
    min = arraymin(ds->databuf+start + bl*proj->nrecs, proj->nrecs);
    max = arraymax(ds->databuf+start + bl*proj->nrecs, proj->nrecs);
    vwp[0] = 0.11 + j*(0.92-0.08)/ncol ;
    vwp[1] = 0.08 + (j+1)*(0.92-0.08)/ncol;
    vwp[2] = 0.99 - (i+0.5)*(0.99-0.08)/nrow;
    vwp[3] = 0.99 - i*(0.99-0.08)/nrow;

    XMIN = ds->xaxis[0];
    XMAX = ds->xaxis[proj->nrecs-1];
    YMIN = min - signum(min)*0.1*min;
    YMAX = max + signum(max)*0.1*max;
    if (!ds->altswtch && scale)
    { YMIN = 0;
      YMAX = 2*max;	
    }
    if( ds->altswtch && scale )
    { YMIN = min - signum(min)*3.0*min;
      YMAX = max + signum(max)*3.0*max;
    }
    cpgsci(C);
    cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
    cpgsch(0.95 - nrow*0.1);
    cpgswin(XMIN, XMAX, YMIN, YMAX);
    cpgbox("BC", 0.0, 0, "BCVN", 0.0, 0);
    cpgsci(LC);
    cpgline( proj->nrecs, axtemp, temp+start + bl*proj->nrecs);
    for( npt = 0 ; npt < proj->nrecs ; npt++ )
    { cpgsch(0.5); cpgsci(SYMC);
      cpgpt1( axtemp[npt], temp[start + bl*proj->nrecs + npt], symbol);
    }
    cpgsci(STRC);
    cpgsch(0.95 - nrow*0.1);
    cpgmtxt("T", -1.5, 0.02, 0.0, blstr);
    if(j == ncol-1 && !ds->altswtch) cpgmtxt("R", 1.5, 0.5, 0.5, "Ampl-Jy");
    else if( j == ncol-1 && ds->altswtch) cpgmtxt("R", 1.5, 0.5, 0.5, "Real - Jy");

    /* Phase or imaginary part */
    min = arraymin(ds->databuf+start + bl*proj->nrecs + offset, proj->nrecs);
    max = arraymax(ds->databuf+start + bl*proj->nrecs + offset, proj->nrecs);

    vwp[0] = 0.11 + j*(0.92-0.08)/ncol ;
    vwp[1] = 0.08 + (j+1)*(0.92-0.08)/ncol;
    vwp[2] = 0.99 - (i+1.0)*(0.99-0.08)/nrow;
    vwp[3] = 0.99 - (i+0.5)*(0.99-0.08)/nrow;

    XMIN = ds->xaxis[0];
    XMAX = ds->xaxis[proj->nrecs-1];
    YMIN = min - signum(min)*0.1*min;
    YMAX = max + signum(max)*0.1*max;
    if (!ds->altswtch && scale)
    { YMIN = -199.0;
      YMAX = 199.0;	
    }
    if( ds->altswtch && scale )
    { YMIN = min - signum(min)*3.0*min;
      YMAX = max + signum(max)*3.0*max;
    }
    cpgsch(0.95 - nrow*0.1);
    cpgsci(C);
    cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
    cpgswin(XMIN, XMAX, YMIN, YMAX);
    cpgsci(LC);
    cpgline( proj->nrecs, axtemp, temp+start + bl*proj->nrecs + offset);
    for( npt = 0 ; npt < proj->nrecs ; npt++ )
    { cpgsch(0.5); cpgsci(SYMC);
      cpgpt1( axtemp[npt], temp[start + bl*proj->nrecs + offset + npt], symbol);
    }
    cpgsci(STRC);
    cpgsch(0.95 - nrow*0.1);
    if(j == ncol-1 && !ds->altswtch) cpgmtxt("R", 1.5, 0.5, 0.5, "Phi-deg");
    else if( j == ncol-1 && ds->altswtch) cpgmtxt("R", 1.5, 0.5, 0.5, "Imag - Jy");
    if (i == nrow-1) 
    { cpgbox("BCN", 0.0, 0, "BCVN", 0.0, 0);
      cpgsch(0.95 - nrow*0.1); cpgmtxt("B", 3.0, 0.5, 0.5, "Rec #");
    }
    else cpgbox("BC", 0.0, 0, "BCVN", 0.0, 0);

    if( i == nrow-1 && j == 0)
    {cpgsch(0.6); 
     cpgsci(SYMC);
     cpgmtxt("B", 5.0, -0.08, 0.0, "n                 p                  s                   m                   q");
     cpgsci(STRC);
     cpgmtxt("B", 5.0, -0.08, 0.0, "  - next page      - prev. page      - toggle scale      - toggle mode     - quit");
    }
    /* Next page or quit for X-window */
    if( !(circnt%nplots) )
    {
     ret = cpgcurs(&x, &y, &c);
     if( c == 'q' ) ret = 0;
     if( c == 's' ) { scale = !scale; }
     if( c == 'm' ) { ds->altswtch = !ds->altswtch; 
                      start  = 2*ds->altswtch*proj->baselines*proj->nrecs;}
     if( c == 'p' ) { incr = -(nplots)+1; ret = 1;}
     if( c == 'n' ) { incr = 1; ret = 1;}
     else { bl = bl - nplots; }
    }
    bl = bl + incr;
    bl = (bl + proj->baselines) % proj->baselines;
    if(ret == 0) break;
  }
  cpgclos();  

  return ret;
}

int ps_nplot_timevis( ProjParType *proj)
{ 
  float SIDE = XSIDE*VWPSIDE, SQUARE = YSIDE;
  DisplayBufParType *ds = &(proj->dispBuf);
  float XMIN, XMAX, YMIN, YMAX;
  float vwp[4];
  int C, LC, SYMC, STRC ;
  int i, j, npt;
  long ii;
  float min, max;
  float x = 1.5, y = 1.5;
  char c;
  int bl = 0;
  int nrow = ds->nrow, ncol = ds->ncol;
  int nplots = nrow * ncol;
  int offset, start;
  char blstr[16];
  int symbol = 5;
  float R, G, B;
  static int scale = 0;
  float *temp = (float*)calloc(sizeof(float), sizeof(ds->databuf)/sizeof(double));
  float *axtemp = (float*)calloc(sizeof(float), sizeof(ds->xaxis)/sizeof(double));
  for( ii = 0 ; ii < sizeof(ds->databuf)/sizeof(double); ii++ ) 
  { temp[ii] = (float)ds->databuf[ii];
  }

  for( ii = 0 ; ii < sizeof(ds->xaxis)/sizeof(double); ii++ ) 
  { axtemp[ii] = (float)ds->xaxis[ii];
  }

  offset = proj->baselines*proj->nrecs;
  start  = 2*ds->altswtch*proj->baselines*proj->nrecs;
  scale = proj->scale;

  cpgopen("/cps");
  C = BLUE;
  LC = MEDIUM_GREY;
  SYMC = CRIMSON;
  STRC = BLUE;

  cpgpap(SIDE,SQUARE);

  for( bl = 0 ; bl < proj->baselines; bl++ )
  { 
    i = bl % nrow;
    j = (bl / nrow) % ncol;    

    if( i == 0 && j == 0 ) cpgpage();
    sprintf(blstr, "%5d : %s-%s", bl+1, proj->Base[bl].ant1, proj->Base[bl].ant2);

    /* Amplitude or real part */
    min = (float)arraymin(ds->databuf+start + bl*proj->nrecs, proj->nrecs);
    max = (float)arraymax(ds->databuf+start + bl*proj->nrecs, proj->nrecs);
    vwp[0] = 0.11 + j*(0.92-0.08)/ncol ;
    vwp[1] = 0.08 + (j+1)*(0.92-0.08)/ncol;
    vwp[2] = 0.99 - (i+0.5)*(0.99-0.08)/nrow;
    vwp[3] = 0.99 - i*(0.99-0.08)/nrow;

    XMIN = ds->xaxis[0];
    XMAX = ds->xaxis[proj->nrecs-1];
    YMIN = min - signum(min)*0.1*min;
    YMAX = max + signum(max)*0.1*max;
    if (!ds->altswtch && scale)
    { YMIN = 0;
      YMAX = 2*max;	
    }
    if( ds->altswtch && scale )
    { YMIN = min - signum(min)*3.0*min;
      YMAX = max + signum(max)*3.0*max;
    }

    cpgsci(C);
    cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
    cpgsch(0.95 - nrow*0.1);
    cpgswin(XMIN, XMAX, YMIN, YMAX);
    cpgbox("BC", 0.0, 0, "BCVN", 0.0, 0);
    cpgsci(LC);
    cpgline( proj->nrecs, axtemp, temp+start + bl*proj->nrecs);
    for( npt = 0 ; npt < proj->nrecs ; npt++ )
    { cpgsch(0.5); cpgsci(SYMC);
      cpgpt1( axtemp[npt], temp[start + bl*proj->nrecs + npt], symbol);
    }
    cpgsci(STRC);
    cpgsch(0.95 - nrow*0.1);
    cpgmtxt("T", -1.5, 0.02, 0.0, blstr);
    if(j == ncol-1 && !ds->altswtch) cpgmtxt("R", 1.5, 0.5, 0.5, "Ampl-Jy");
    else if( j == ncol-1 && ds->altswtch) cpgmtxt("R", 1.5, 0.5, 0.5, "Real - Jy");

    /* Phase or imaginary part */
    min = (float)arraymin(ds->databuf+start + bl*proj->nrecs + offset, proj->nrecs);
    max = (float)arraymax(ds->databuf+start + bl*proj->nrecs + offset, proj->nrecs);
    vwp[0] = 0.11 + j*(0.92-0.08)/ncol ;
    vwp[1] = 0.08 + (j+1)*(0.92-0.08)/ncol;
    vwp[2] = 0.99 - (i+1.0)*(0.99-0.08)/nrow;
    vwp[3] = 0.99 - (i+0.5)*(0.99-0.08)/nrow;

    XMIN = (float)ds->xaxis[0];
    XMAX = (float)ds->xaxis[proj->nrecs-1];
    YMIN = min - signum(min)*0.1*min;
    YMAX = max + signum(max)*0.1*max;
    if (!ds->altswtch && scale)
    { YMIN = -199.0;
      YMAX = 199.0;	
    }
    if( ds->altswtch && scale )
    { YMIN = min - signum(min)*3.0*min;
      YMAX = max + signum(max)*3.0*max;
    }

    cpgsch(0.95 - nrow*0.1);
    cpgsci(C);
    cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
    cpgswin(XMIN, XMAX, YMIN, YMAX);
    cpgsci(LC);
    cpgline( proj->nrecs, axtemp, temp+start + bl*proj->nrecs + offset);
    for( npt = 0 ; npt < proj->nrecs ; npt++ )
    { cpgsch(0.5); cpgsci(SYMC);
      cpgpt1( axtemp[npt], temp[start + bl*proj->nrecs + offset + npt], symbol);
    }
    cpgsci(STRC);
    cpgsch(0.95 - nrow*0.1);
    if(j == ncol-1 && !ds->altswtch) cpgmtxt("R", 1.5, 0.5, 0.5, "Phi-deg");
    else if( j == ncol-1 && ds->altswtch) cpgmtxt("R", 1.5, 0.5, 0.5, "Imag - Jy");
    if (i == nrow-1) 
    { cpgbox("BCN", 0.0, 0, "BCVN", 0.0, 0);
      cpgsch(0.95 - nrow*0.1); cpgmtxt("B", 3.0, 0.5, 0.5, "UTC - min");
    }
    else cpgbox("BC", 0.0, 0, "BCVN", 0.0, 0);

    /* Status for PS plots */
    if( !((bl+1)%5) )
    fprintf(finfo, "%s Plotted [ %5d / %5ld ] baselines\r", info, bl+1, proj->baselines );
  }

  cpgclos();  

  return 0;
}

void pierce( float x, float y, ProjParType *proj)
{
    float res;
    float lb, ub;
    int i, ch1, ch2;;
    SkyMapType *sky = &(proj->skymap);
    PrimaryBeamParType *pb = &(proj->pbeam);
    DisplayBufParType *ds = &(proj->dispBuf);
    float pierce_point[sky->zside], log_pp[sky->zside];
    float pierce_point_err[sky->zside], log_pp_err[sky->zside];
    float err_pos[sky->zside], err_neg[sky->zside];
    float log_err_pos[sky->zside], log_err_neg[sky->zside];
    float u[sky->zside], log_u[sky->zside];
    float U = 12.5;
    float XMIN, XMAX, YMIN, YMAX;
    int pspecboard;
    char c;
    float xx = proj->bandcentre, yy = proj->bandcentre;
    float nu1, nu2;
    int ret;
    char chinfo1[32], chinfo2[32];
    char nuinfo[32];
    char filename[80];
    double  ptg_dec = dec2sxg( &(proj->scan[0].dec) );
    float inp[sky->zside];
    float nu = 326.5;

    ptg_dec = 0.0;

    U *= cos(deg2rad(ptg_dec));

    res = (ds->max[0] - ds->min[0])/ds->nrow;
    i = 0;
    while(i < ds->nrow) 
    {	lb = ds->min[0] + i*res;
	ub = lb + res;
	if(lb < x && x <= ub )
       {  ch1 = i;
          nu1 = (lb + ub) / 2.0;
	  break;
	}
	else i++;
    }

    res = (ds->max[1] - ds->min[1])/ds->ncol;
    i = 0;
    while(i < ds->ncol) 
    {	lb = ds->min[1] + i*res;
	ub = lb + res;
	if(lb < y && y <= ub )
	{ ch2 = i;
          nu2 = (lb + ub) / 2.0;
	  break;
	}
	else i++;
    }

    sprintf(nuinfo, "   %3.2f             %3.2f", nu1, nu2);
    sprintf(chinfo1, "Ch 1:             Ch 2:  ");
    sprintf(chinfo2, "      %d                %d", ch1+1, ch2+1);
    
    fprintf(finfo,"\n%s   --------------------------------------------\n", info);
    fprintf(finfo,  "%s    U \t\tS_2(U)\t\tsigma_S_2(U)\n", info);
    fprintf(finfo,  "%s   --------------------------------------------\n", info);
    
    for( i = 0 ; i < sky->zside ; i++ )
    { u[i] = U*(i+1);
      log_u[i] = log10f(U*(i+1));
      pierce_point[i] = sky->map[i][ch1][ch2];
      pierce_point_err[i] = pb->shape[i][ch1][ch2];
      log_pp[i] = log10f(sky->map[i][ch1][ch2]);
      log_pp_err[i] = log10f(pb->shape[i][ch1][ch2]);
      err_pos[i] = pierce_point[i] + pierce_point_err[i];
      err_neg[i] = pierce_point[i] - pierce_point_err[i];
      log_err_pos[i] = log10f(pierce_point[i] + pierce_point_err[i]);
      log_err_neg[i] = log10f(pierce_point[i] - pierce_point_err[i]);
      inp[i] = log10f(P_I(nu, u[i], 2.52, 2.34)/919.44);
      fprintf(finfo, "%s    %4.1f  \t%e\t%e\n", info, u[i], pierce_point[i], pierce_point_err[i]);
    }
    
 pspecboard = cpgopen("/xs");
 cpgslct(pspecboard);

 cpgpap(0.0, 1.0);
 cpgsci(WHITE);
 cpgsvp(0.1, 0.9, 0.1, 0.9);
 XMIN = arrayminf(log_u, sky->zside) - 0.1; 
 XMAX = arraymaxf(log_u, sky->zside) + 0.1; 
 YMIN = arrayminf(log_pp, sky->zside) - 0.1;
 YMAX = arraymaxf(log_err_pos, sky->zside) + 0.1;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCLNST", 0.0, 0, "BCLNSTV2", 0.0, 0);

 cpgsch(0.65);
 cpgsci(YELLOW);
 cpgmtxt("B", 5.5, 0.0, 0.0, "p :                      any other key : ");
 cpgsci(CYAN);
 cpgmtxt("B", 5.5, 0.0, 0.0, "    print ps file                         quit");
 cpgsch(1.2);
 cpgsci(WHITE);
 cpgmtxt("L",2.35, 0.5, 0.5, "\\fiS\\d2\\u\\fn(\\(2021))  Jy\\u2\\d");
 cpgmtxt("B",2.5, 0.5, 0.5, "\\(2021)  \\(2137)");
 //cpgmtxt("L",1.9, 0.5, 0.0, "\\fiV\\d2\\u\\fn(\\(2021)");
 //cpgmtxt("B",2.2, 0.5, 0.0, "\\(2021)");
 cpgsch(0.7);
 cpgsci(YELLOW);
 cpgmtxt("T",1.9, 0.0, 0.0, "\\(0639)\\d1\\u:        MHz    \\(0639)\\d2\\u :         MHz");
 cpgsci(CYAN);
 cpgmtxt("T", 1.9, 0.0, 0.0, nuinfo);
 cpgsch(0.7);
 cpgslw(3.5);
 cpgsci(YELLOW);
 cpgmtxt("T",1.7, 0.79, 0.0, chinfo1);
 cpgsci(CYAN);
 cpgmtxt("T",1.7, 0.79, 0.0, chinfo2);
 cpgslw(1.0);

 
 cpgsch(0.8);
 cpgsci(MEDIUM_GREY);
 cpgsls(2);
 cpgline(sky->zside, log_u, inp);
 cpgsci(PARROT);
 cpgsls(1);
 //cpgerry(sky->zside, log_u, log_err_pos, log_err_neg, 1.0);
 cpgsch(0.8); 

 for(i = 0; i<sky->zside; i++)
 {  
   if(pierce_point[i] > 0.0) 
 { cpgpt1(log_u[i], log_pp[i], -20);
   if(err_neg[i] == 0.0)
   { cpgsci(PARROT);
     cpgarro(log_u[i], log_pp[i], log_u[i], log_err_pos[i]);
   }
   else
   cpgerry(1, log_u+i, log_err_pos+i, log_err_neg+i, 1.0);
 }
 }
 
 ret = cpgcurs(&xx, &yy, &c);
 cpgslct(pspecboard);
 cpgclos();
 
 if( c == 'p' )
 {
 pspecboard = cpgopen("/cps");
 cpgslct(pspecboard);
 cpgslw(4);
 cpgpap(0.0, 1.0);
 cpgsci(WHITE);
 cpgsvp(0.25, 0.99, 0.25, 0.99);
 XMIN = arrayminf(log_u, sky->zside) - 0.1; 
 XMAX = arraymaxf(log_u, sky->zside) + 0.1; 
 YMIN = arrayminf(log_pp, sky->zside) - 0.1;
 YMAX = arraymaxf(log_err_pos, sky->zside) + 0.1;
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgsch(2.0);
 cpgslw(8);
 cpgbox("BCLNST", 0.0, 0, "BCLNSTV2", 0.0, 0);
 cpgmtxt("L",3.5, 0.5, 0.5, "\\fiS\\d2\\u\\fn(\\(2021))  Jy\\u2\\d");
 cpgmtxt("B", 3.5, 0.5, 0.5, "\\(2021)  \\(2137)");
 cpgslw(4);
 cpgsch(0.8);
 cpgsci(MINIMAL_GREY);
 cpgsls(2);
 cpgline(sky->zside, log_u, inp);
 cpgsch(0.8); 
 cpgsci(WHITE);
 cpgsls(1);
 //gpt( sky->zside, log_u, log_pp, -20);
 
 for(i = 0; i<sky->zside; i++)
 { if(pierce_point[i] > 0.0) 
   { cpgpt1(log_u[i], log_pp[i], -20);
   if(err_neg[i] == 0.0)
   { cpgsci(WHITE);
     cpgarro(log_u[i], log_pp[i], log_u[i], log_err_pos[i]);
   }
   else
   cpgerry(1, log_u+i, log_err_pos+i, log_err_neg+i, 1.0);
   }
 }
  /*
 cpgsch(0.8);
 cpgsci(MINIMAL_GREY);
 cpgline(sky->zside, log_u, log_pp);
 */
 cpgsch(1.0);

 cpgsci(WHITE);
 cpgmtxt("T",-1.9, 0.4, 0.0, "\\(0639)\\d1\\u:        MHz    \\(0639)\\d2\\u :         MHz");
 cpgsci(WHITE);
 cpgmtxt("T", -1.9, 0.4, 0.0, nuinfo);
 cpgsch(1.0);
 cpgslw(4.0);
 cpgsci(WHITE);
 cpgmtxt("T",-3.9, 0.4, 0.0, chinfo1);
 cpgsci(WHITE);
 cpgmtxt("T",-3.9, 0.4, 0.0, chinfo2);
 
 /*
 for(i = 0; i<sky->zside; i++)
 { cpgsci(MAGENTA);
   cpgpt1(log_u[i], log_pp[i], -3);
   }*/
 cpgslct(pspecboard);
 cpgclos();

 sprintf(filename, "mv pgplot.ps V2Corr.ch%d-ch%d.nu1_%3.2f_MHz-nu2_%3.2f_MHz.eps", ch1+1, ch2+1, nu1, nu2);
 system(filename);
 fprintf(finfo, "%s ----------------->\n", info);
 fprintf(finfo, "%s Shell did : %s\n", info, filename);
 fprintf(finfo, "%s ----------------->\n", info);
 }

}
int plot_V2_deltanu( ProjParType *proj, PlotParType *plot)
{
  float SIDE = 1.675*VWPSIDE;
  float TR[6] = { 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
  float XMIN, XMAX, YMIN, YMAX;
  int XPIX, YPIX;
  float vwp[4];
  float R, G, B;
  int C, SYMC, STRC;
  int i, u;
  float delnu[proj->chans];
  float min, max;
  int ret = 1;
  float l, h, a, g, s, r, phi, L;
  float SQUARE = 1.0;
  int itf = plot->itf;
  int board1, board2;
  float *temp = (float*)calloc(sizeof(proj->V2_dnu)/sizeof(double), sizeof(float));
  long ii;

  for( ii = 0 ; ii < sizeof(proj->V2_dnu)/sizeof(double) ; ii++ )
      temp[ii] = (float)proj->V2_dnu[ii];

  min = (float)arraymin(proj->V2_dnu, proj->redundant_baselines*proj->chans);
  max = (float)arraymax(proj->V2_dnu, proj->redundant_baselines*proj->chans);

  for(i = 0 ; i < proj->chans ; i++ ) delnu[i] = 1.0e-6 * i*proj->bandwidth/proj->chans;

  XPIX = proj->chans;
  YPIX = proj->redundant_baselines;

  if(plot->driver == 'x' || plot->driver == 'X') plot->driver = 'X';

  //SQUARE = (float)XPIX/(float)YPIX;
  SQUARE = 1.0;
  
  if( plot->driver == 'P' || plot->driver == 'p')  
  { plot->driver = 'P';
    board1 = cpgopen("/cps");
    C = WHITE; 
    STRC = BLUE;
    SYMC = CRIMSON;
  }
  else  
  { board1 = cpgopen("/xs");
    C = WHITE;
    STRC = YELLOW;;
    SYMC = CYAN;
  }

  //cpgslct(board1);
 
  cpgpap(SIDE, SQUARE);  

  for(i = 0 ; i < MAXCOL ; i++)
  { R = 0.75*(float)(i)/(float)(MAXCOL) + 0.25;
    G = maxf(0.0, 2.0*(float)(i-MAXCOL/2)/(float)(MAXCOL+1));
    B = 0.0 + 0.4*(float)(MAXCOL-i)/(float)(MAXCOL);
    cpgscr(16+i, R, G, B);
  }

  vwp[0] = 0.08;
  vwp[1] = 0.92;
  vwp[2] = 0.12;
  vwp[3] = 0.9;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  
  XMIN = 0.5;
  XMAX = XPIX + 0.5;
  YMIN = 0.5;
  YMAX = YPIX + 0.5;

  itf = LIN;

  cpgsch(1.0);
  cpgsci(C);
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  cpgbox("BCN", 0.0, 0, "BCN", 0.0, 0);
  cpgscir(16,15+MAXCOL);  
  cpgsitf(itf);
  cpgsch(1.0);
  cpgslw(3.0);
  
  cpgimag( temp, XPIX, YPIX, 1, XPIX, 1, YPIX, min, max, TR );
  cpgwedg( "TI", 1.0, 3.5, min, max, "");
   
  cpgsch(1.0);
  cpgsci(!C);
  cpgslw(35);
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgbox("BCN", 0.0, 0, "BCN", 0.0, 0);
  cpgslw(1);

  XMIN = -delnu[1];
  XMAX = proj->bandwidth/1.0e6;
  YMIN = 0;
  YMAX = 12.5 * (proj->redundant_baselines);
  cpgslw(3.0);  
  cpgsci(C);
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  cpgbox("BCNTS", 0.0, 0, "BCNMTS", 0.0, 0);

  cpgmtxt("B",3.0, 0.5, 0.5, "deltanu - MHz");
  cpgmtxt("L",2.5, 0.5, 0.5, "U - lambda");
  cpgslw(1);

  cpgclos();

  return 0;
  /*
  cpgslct(board2);
  cpgpap(0.0, 1.0);
  cpgsci(WHITE);
  cpgsvp(0.1, 0.9, 0.1, 0.9);
  XMIN = 0.0;
  XMAX = proj->bandwidth;
  YMIN = 0.0;
  YMAX = 1.1;
  cpgswin(XMIN, XMAX, YMIN, YMAX);
  cpgbox("BCLNST", 0.0, 0, "BCLNST", 0.0, 0);
  cpgsch(1.2);

  cpgmtxt("L",1.9, 0.5, 0.0, "\\fiV\\d2\\u\\fn(\\(2021))");
  cpgmtxt("B",2.2, 0.5, 0.0, "\\(2021)");

 cpgsch(0.8);
 cpgsci(MINIMAL_GREY);
 for( u = 0 ; u < proj->redundant_baselines ; u++ )
  cpgline(proj->chans, delnu, proj->V2_dnu+u*proj->chans);

 cpgsch(1.0);

 cpgslct(board2);
 cpgclos();
  */

}

int overlay_vector_map( ProjParType *proj, PlotParType *plot )
{ 
  float SIDE = 1.675*VWPSIDE;
  DisplayBufParType *ds = &(proj->dispBuf);
  float TR[6] = { 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
  float XMIN, XMAX, YMIN, YMAX;
  float vwp[4];
  float R, G, B;
  int C, SYMC, STRC;
  int i;
  double min, max;
  float x = (float)(ds->min[0] + ds->max[0]) / 2.0;
  float y = (float)(ds->min[1] + ds->max[1]) / 2.0;
  char c='\0';
  char colorscale[32], chaninfo[32];
  int XPIX = ds->ncol;
  int YPIX = ds->nrow;
  int ZPIX = proj->chans;
  int ret = 1;
  float l, h, a, g, s, r, phi, L;
  float SQUARE = 1.0;
  int itf = plot->itf;
  int ch = proj->chan2cal-1;
  float deltanu = proj->bandwidth / proj->chans;
  int mapboard;
  colormap *cmap;
  float *temp;
  float *clev;
  int nclev=NCLEV;

  temp = (float*)calloc(sizeof(float), XPIX*YPIX*ZPIX);
  clev = (float*)calloc(sizeof(float), nclev);

  for(i = 0 ; i < XPIX*YPIX*ZPIX; i++ ) 
  { temp[i] = (float)ds->databuf[i];
      if( proj->threshold == 0.0 ) continue;
    if( temp[i] < proj->threshold/1000.0 ) temp[i] = 0.0;
  }

  cmap = (colormap*)calloc(MAXCOL, sizeof(colormap));

  min = arraymin(ds->databuf, XPIX*YPIX*ZPIX);
  max = (1.0 + 1e-4)*arraymax(ds->databuf, XPIX*YPIX*ZPIX);

  if(plot->driver == 'x' || plot->driver == 'X') plot->driver = 'X';

  SQUARE = (float)XPIX/(float)YPIX;
  
  while (ret)
  {
  if( plot->driver == 'P' || plot->driver == 'p')  
  { plot->driver = 'P';
    mapboard = cpgopen("/cps");
    C = WHITE; 
    STRC = BLUE;
    SYMC = CRIMSON;
  }
  else  
  { mapboard = cpgopen("/xs");
    C = WHITE;
    STRC = YELLOW;;
    SYMC = CYAN;
  }

  cpgpap(SIDE, SQUARE);  
  
  /* Dave Green's CubeHelix colour scheme *
  s = 1.0;
  r = 0.175;
  l = 1.0;
  h = 1.0;//HUE;
  g = GAMMA;

  for(i = 0 ; i < MAXCOL ; i++)
  { L = l*(i+1)/MAXCOL;
    a = h*pow(L,g)*(1 - pow(L,g))/2;
    phi = 2*M_PI*(s/3 + r*L);
    R = pow(L,g) + a*( -0.14861*cos(phi) + 1.78277*sin(phi) );
    G = pow(L,g) + a*( -0.29277*cos(phi) - 0.90649*sin(phi) );
    B = pow(L,g) + a*( 1.97294*cos(phi) + 0.0*sin(phi) );
    cpgscr(16+i, R, G, B);
  }
  
  /*
  for(i = 0 ; i < MAXCOL ; i++)
  { R = 0.75*(float)(i)/(float)(MAXCOL) + 0.25;
    G = maxf(0.0, 2.0*(float)(i-MAXCOL/2)/(float)(MAXCOL+1));
    B = 0.0 + 0.4*(float)(MAXCOL-i)/(float)(MAXCOL);
    cpgscr(16+i, R, G, B);
  }
  /**/

  read_colormap("coolwarm.cmap", cmap);

  for(i = 0 ; i < MAXCOL ; i++)
  { R = cmap[i].r;
    G = cmap[i].g;
    B = cmap[i].b;
    cpgscr(16+i, R, G, B);
  }
  

  vwp[0] = 0.1;
  vwp[1] = 0.9;
  vwp[2] = 0.11;
  vwp[3] = 0.88;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  
  XMIN = 0.5;
  XMAX = XPIX + 0.5;
  YMIN = 0.5;
  YMAX = YPIX + 0.5;

  cpgsch(1.0);
  cpgsci(C);
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  cpgbox("BCN", 0.0, 0, "BCN", 0.0, 0);
  cpgscir(16,15+MAXCOL);  
  cpgsitf(itf);

  cpgsch(1.0);
  cpgslw(3.0);

  min = (1.0 - 1e-4)*arraymin(ds->databuf+ch*XPIX*YPIX, XPIX*YPIX);
  max = (1.0 + 1e-4)*arraymax(ds->databuf+ch*XPIX*YPIX, XPIX*YPIX);
  for(i = 0 ; i < nclev; i++) clev[i] = min + (max-min)*i/(nclev-1);

  if(plot->colour)
      { cpgimag( temp+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, 1, YPIX, (float)min, (float)max, TR );
	if( ! strcmp ( ds->axlbl[2], "Baseline" ) ) {
	//cpgimag( ds->databuf+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, YPIX, 1, min, max, TR );
	cpgwedg( "TI", 1.0, 3.5, (float)min, (float)max, "Jy\\u2\\d");
	}
	 else 
         cpgwedg( "TI", 1.0, 3.5, (float)min, (float)max, "");
        
	cpgcons(temp+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, 1, YPIX, clev, nclev, TR);
  }
  else if(!(plot ->colour) && plot->driver == 'P')
      { cpggray( temp+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, 1, YPIX, (float)min, (float)max, TR );
	  cpgwedg( "TG", 1.0, 3.5, (float)min, max, "");
        cpgcons(temp+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, 1, YPIX, clev, nclev, TR);
  }
  else
      { cpggray( temp+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, 1, YPIX, (float)max, (float)min, TR );
	  cpgwedg( "TG", 1.0, 3.5, (float)max, (float)min, "");
        cpgcons(temp+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, 1, YPIX, clev, nclev, TR);

  }
 
  cpgsch(1.0);
  cpgsci(!C);
  cpgslw(35);
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgbox("BCN", 0.0, 0, "BCN", 0.0, 0);
  cpgslw(1);

  XMIN = (float)ds->min[0];
  XMAX = (float)ds->max[0];
  YMIN = (float)ds->min[1];
  YMAX = (float)ds->max[1];
  cpgslw(3.0);  
  cpgsci(C);
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  if( ! strcmp ( ds->axlbl[2], "Baseline" ) ) cpgbox("BCNTS", 0.0, 0, "BCNTSV", 0.0, 0);
  else cpgbox("BCNTS", 0.0, 0, "BCNMTSV", 0.0, 0);

  cpgmtxt("B",3.0, 0.5, 0.5, ds->axlbl[0]);
  cpgmtxt("L",2.8, 0.5, 0.5, ds->axlbl[1]);
  cpgslw(1);

  strcpy(colorscale, "Colour Scale : ");
  if(itf == LIN ) strcat(colorscale, "lin\0");
  else if(itf == LOG ) strcat(colorscale, "log\0");
  else if(itf == SQR ) strcat(colorscale, "sqrt\0");
  
  if( ! strcmp ( ds->axlbl[2], "Frequency - MHz" ) )
  { fprintf(finfo, "%s Showing channel # %3d\r", info, ch+1);
    sprintf(chaninfo, "Channel # %d", ch+1);
  }
  else if( ! strcmp ( ds->axlbl[2], "Baseline" ) )
  { fprintf(finfo, "%s Showing baseline # %3d\r", info, ch+1);
    sprintf(chaninfo, "Baseline # %d", ch+1);
  }
  else if( ! strcmp ( ds->axlbl[2], "Record" ) )
  { fprintf(finfo, "%s Showing record # %3d\r", info, ch+1);
    sprintf(chaninfo, "Record # %d", ch+1);
  }

  /* Next page or quit for X-window */
  if( ret == 1  && plot->driver == 'X')
  { cpgsch(0.7); 
    cpgsci(SYMC);
    cpgmtxt("B", 6.0, -0.08, 0.0, "l      g       s         n            p             space                  q");
    cpgsci(STRC);
    cpgmtxt("B", 6.0, -0.08, 0.0, " : lin   : log    : sqrt     : chan++   : chan--          : toggle colour    : quit");
    cpgsci(STRC);
    cpgmtxt("B", 6.0, 0.85, 0.0, colorscale);
    cpgmtxt("B", 4.5, 0.85, 0.0, chaninfo);
    ret = cpgcurs(&x, &y, &c);
    if( c == 'q' ) { ret = 0; }
    if( c == 'l' ) { c = '\0';itf = LIN;  ret = 1; }
    if( c == 'g' ) { c = '\0';itf = LOG;  ret = 1; }
    if( c == 's' ) { c = '\0';itf = SQR;  ret = 1; }
    if( c == 'n' ) { c = '\0';ch = (ch+1)%proj->chans; ret = 1; }
    if( c == 'p' ) { c = '\0';ch = (proj->chans+ch-1)%proj->chans; ret = 1; }
    else if ( c == ' ' )
    { c = '\0';
      plot->colour = !(plot->colour); 
      ret = 1; 
    }
  }

  if( ! strcmp ( ds->axlbl[2], "Baseline" ) && ret == 1 && (c != '\0' )  )
  { cpgslct(mapboard);
    cpgclos();
    pierce(x, y, proj);
  }

  else
  { cpgslct(mapboard);
    cpgclos();  
  }  
  
  if(plot->driver == 'P')  ret = 0;

  }
  if(abs( strcmp(ds->axlbl[2],"")) ) newline();
  return ret;
  cpgclos();
  free(temp);
}


int wedgeplot_map( ProjParType *proj, PlotParType *plot )
{ 
  float SIDE = 1.675*VWPSIDE;
  DisplayBufParType *ds = &(proj->dispBuf);
  float TR[6] = { 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
  float XMIN, XMAX, YMIN, YMAX;
  float vwp[4];
  float R, G, B;
  int C, SYMC, STRC;
  int i;
  double min, max;
  float x = (float)(ds->min[0] + ds->max[0]) / 2.0;
  float y = (float)(ds->min[1] + ds->max[1]) / 2.0;
  char c='\0';
  char colorscale[32], chaninfo[32];
  int XPIX = ds->ncol;
  int YPIX = ds->nrow;
  int ZPIX = proj->chans;
  int ret = 1;
  float l, h, a, g, s, r, phi, L;
  float SQUARE = 1.0;
  int itf = plot->itf;
  int ch = proj->chan2cal-1;
  float deltanu = proj->bandwidth / proj->chans;
  int mapboard;
  colormap *cmap;
  float *temp;
  float *clev;
  int nclev=NCLEV;
  float axis[proj->chans], nues[proj->chans];
  temp = (float*)calloc(sizeof(float), XPIX*YPIX*ZPIX);
  clev = (float*)calloc(sizeof(float), nclev);


for(i = 0 ; i < proj->chans ; i++ )
{
axis[i] = proj->bandcentre/1e6 + deltanu*(proj->chans/2 - i);
nues[i] = proj->bandcentre / 1e6;
}
  for(i = 0 ; i < XPIX*YPIX*ZPIX; i++ ) 
  { temp[i] = (float)ds->databuf[i];
      if( proj->threshold == 0.0 ) continue;
    if( temp[i] < proj->threshold/1000.0 ) temp[i] = 0.0;
  }

  cmap = (colormap*)calloc(MAXCOL, sizeof(colormap));

  min = arraymin(ds->databuf, XPIX*YPIX*ZPIX);
  max = (1.0 + 1e-4)*arraymax(ds->databuf, XPIX*YPIX*ZPIX);

  if(plot->driver == 'x' || plot->driver == 'X') plot->driver = 'X';

  SQUARE = (float)XPIX/(float)YPIX;
  
  while (ret)
  {
  if( plot->driver == 'P' || plot->driver == 'p')  
  { plot->driver = 'P';
    mapboard = cpgopen("/cps");
    C = WHITE; 
    STRC = BLUE;
    SYMC = CRIMSON;
  }
  else  
  { mapboard = cpgopen("/xs");
    C = WHITE;
    STRC = YELLOW;;
    SYMC = CYAN;
  }

  cpgpap(SIDE, SQUARE);  
  
  /* Dave Green's CubeHelix colour scheme *
  s = 1.0;
  r = 0.175;
  l = 1.0;
  h = 1.0;//HUE;
  g = GAMMA;

  for(i = 0 ; i < MAXCOL ; i++)
  { L = l*(i+1)/MAXCOL;
    a = h*pow(L,g)*(1 - pow(L,g))/2;
    phi = 2*M_PI*(s/3 + r*L);
    R = pow(L,g) + a*( -0.14861*cos(phi) + 1.78277*sin(phi) );
    G = pow(L,g) + a*( -0.29277*cos(phi) - 0.90649*sin(phi) );
    B = pow(L,g) + a*( 1.97294*cos(phi) + 0.0*sin(phi) );
    cpgscr(16+i, R, G, B);
  }
  
  /*
  for(i = 0 ; i < MAXCOL ; i++)
  { R = 0.75*(float)(i)/(float)(MAXCOL) + 0.25;
    G = maxf(0.0, 2.0*(float)(i-MAXCOL/2)/(float)(MAXCOL+1));
    B = 0.0 + 0.4*(float)(MAXCOL-i)/(float)(MAXCOL);
    cpgscr(16+i, R, G, B);
  }
  /**/

  read_colormap("coolwarm.cmap", cmap);
  //read_colormap("summerlilac.cmap", cmap);
  //read_colormap("winterbeige.cmap", cmap);

  for(i = 0 ; i < MAXCOL ; i++)
  { R = cmap[i].r;
    G = cmap[i].g;
    B = cmap[i].b;
    cpgscr(16+i, R, G, B);
  }
  

  vwp[0] = 0.1;
  vwp[1] = 0.9;
  vwp[2] = 0.11;
  vwp[3] = 0.88;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  
  XMIN = 0.5;
  XMAX = XPIX + 0.5;
  YMIN = 0.5;
  YMAX = YPIX + 0.5;

  cpgsch(1.0);
  cpgsci(C);
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  cpgbox("BCN", 0.0, 0, "BCN", 0.0, 0);
  cpgscir(16,15+MAXCOL);  
  cpgsitf(itf);

  cpgsch(1.0);
  cpgslw(3.0);

  min = (1.0 - 1e-4)*arraymin(ds->databuf+ch*XPIX*YPIX, XPIX*YPIX);
  max = (1.0 + 1e-4)*arraymax(ds->databuf+ch*XPIX*YPIX, XPIX*YPIX);
  for(i = 0 ; i < nclev; i++) clev[i] = min + (max-min)*i/(nclev-1);

  if(plot->colour)
      { cpgimag( temp+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, 1, YPIX, (float)min, (float)max, TR );
	if( ! strcmp ( ds->axlbl[2], "Baseline" ) ) {
	//cpgimag( ds->databuf+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, YPIX, 1, min, max, TR );
	cpgwedg( "TI", 1.0, 3.5, (float)min, (float)max, "Jy\\u2\\d");
	}
	 else 
         cpgwedg( "TI", 1.0, 3.5, (float)min, (float)max, "");
        
	//cpgcons(temp+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, 1, YPIX, clev, nclev, TR);
  }
  else if(!(plot ->colour) && plot->driver == 'P')
      { cpggray( temp+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, 1, YPIX, (float)min, (float)max, TR );
	  cpgwedg( "TG", 1.0, 3.5, (float)min, max, "");
	  //cpgcons(temp+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, 1, YPIX, clev, nclev, TR);
  }
  else
      { cpggray( temp+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, 1, YPIX, (float)max, (float)min, TR );
	  cpgwedg( "TG", 1.0, 3.5, (float)max, (float)min, "");
	  //cpgcons(temp+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, 1, YPIX, clev, nclev, TR);

  }
 
  cpgsch(1.0);
  cpgsci(!C);
  cpgslw(35);
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgbox("BCN", 0.0, 0, "BCN", 0.0, 0);
  cpgslw(1);

  XMIN = (float)ds->min[0];
  XMAX = (float)ds->max[0];
  YMIN = (float)ds->min[1];
  YMAX = (float)ds->max[1];
  cpgslw(3.0);  
  cpgsci(C);
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  if( ! strcmp ( ds->axlbl[2], "Baseline" ) ) cpgbox("BCNTS", 0.0, 0, "BCNTSV", 0.0, 0);
  else cpgbox("BCNTS", 0.0, 0, "BCNMTSV", 0.0, 0);

  cpgmtxt("B",3.0, 0.5, 0.5, ds->axlbl[0]);
  cpgmtxt("L",2.8, 0.5, 0.5, ds->axlbl[1]);
  cpgslw(1);

  strcpy(colorscale, "Colour Scale : ");
  if(itf == LIN ) strcat(colorscale, "lin\0");
  else if(itf == LOG ) strcat(colorscale, "log\0");
  else if(itf == SQR ) strcat(colorscale, "sqrt\0");
  
  if( ! strcmp ( ds->axlbl[2], "Frequency - MHz" ) )
  { fprintf(finfo, "%s Showing channel # %3d\r", info, ch+1);
    sprintf(chaninfo, "Channel # %d", ch+1);
  }
  else if( ! strcmp ( ds->axlbl[2], "Baseline" ) )
  { fprintf(finfo, "%s Showing baseline # %3d\r", info, ch+1);
    sprintf(chaninfo, "Baseline # %d", ch+1);
cpgsci(WHITE);
cpgslw(3.0);
cpgsls(2);
cpgline(proj->chans, axis, nues);
cpgline(proj->chans, nues, axis);
  }
  else if( ! strcmp ( ds->axlbl[2], "Record" ) )
  { fprintf(finfo, "%s Showing record # %3d\r", info, ch+1);
    sprintf(chaninfo, "Record # %d", ch+1);
  }

  /* Next page or quit for X-window */
  if( ret == 1  && plot->driver == 'X')
  { cpgsch(0.7); 
    cpgsci(SYMC);
    cpgmtxt("B", 6.0, -0.08, 0.0, "l      g       s         n            p             space                  q");
    cpgsci(STRC);
    cpgmtxt("B", 6.0, -0.08, 0.0, " : lin   : log    : sqrt     : chan++   : chan--          : toggle colour    : quit");
    cpgsci(STRC);
    cpgmtxt("B", 6.0, 0.85, 0.0, colorscale);
    cpgmtxt("B", 4.5, 0.85, 0.0, chaninfo);
    ret = cpgcurs(&x, &y, &c);
    if( c == 'q' ) { ret = 0; }
    if( c == 'l' ) { c = '\0';itf = LIN;  ret = 1; }
    if( c == 'g' ) { c = '\0';itf = LOG;  ret = 1; }
    if( c == 's' ) { c = '\0';itf = SQR;  ret = 1; }
    if( c == 'n' ) { c = '\0';ch = (ch+1)%proj->chans; ret = 1; }
    if( c == 'p' ) { c = '\0';ch = (proj->chans+ch-1)%proj->chans; ret = 1; }
    else if ( c == ' ' )
    { c = '\0';
      plot->colour = !(plot->colour); 
      ret = 1; 
    }
  }

  if( ! strcmp ( ds->axlbl[2], "Baseline" ) && ret == 1 && (c != '\0' )  )
  { cpgslct(mapboard);
    cpgclos();
    pierce(x, y, proj);
  }

  else
  { cpgslct(mapboard);
    cpgclos();  
  }  
  
  if(plot->driver == 'P')  ret = 0;

  }
  if(abs( strcmp(ds->axlbl[2],"")) ) newline();
  return ret;
  cpgclos();
  free(temp);
}


int wedgeplot_map_for_paper( ProjParType *proj, PlotParType *plot )
{ 
  float SIDE = 1.675*VWPSIDE;
  DisplayBufParType *ds = &(proj->dispBuf);
  float TR[6] = { 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
  float XMIN, XMAX, YMIN, YMAX;
  float vwp[4];
  float R, G, B;
  int C, SYMC, STRC;
  int i;
  long ii;
  float min, max;
  float x = (ds->min[0] + ds->max[0]) / 2.0;
  float y = (ds->min[1] + ds->max[1]) / 2.0;
  char c='\0';
  char colorscale[32], chaninfo[32];
  int XPIX = ds->ncol;
  int YPIX = ds->nrow;
  int ZPIX = proj->chans;
  int ret = 1;
  float l, h, a, g, s, r, phi, L;
  float SQUARE = 1.0;
  int itf = plot->itf;
  float clev[NCLEV];
  int ch = proj->chan2cal-1;
  float deltanu = proj->bandwidth / proj->chans;
  int mapboard;
  colormap *cmap;
  float *temp = (float*)calloc(sizeof(float), XPIX*YPIX*ZPIX);
float axis[proj->chans], nues[proj->chans];
for(i = 0 ; i < proj->chans ; i++ )
{
axis[i] = proj->bandcentre/1e6 + deltanu*(proj->chans/2 - i);
nues[i] = proj->bandcentre / 1e6;
}

  for ( ii = 0 ; ii < XPIX*YPIX*ZPIX ; ii++ ) 
      temp[ii] = (float)ds->databuf[ii];

  cmap = (colormap*)calloc(MAXCOL, sizeof(colormap));

  min = (float)arraymin(ds->databuf, XPIX*YPIX*ZPIX);
  max = (1.0 + 1e-4)*(float)arraymax(ds->databuf, XPIX*YPIX*ZPIX);

  if(plot->driver == 'x' || plot->driver == 'X') plot->driver = 'X';

  SQUARE = (float)XPIX/(float)YPIX;
  
  while (ret)
  {
  if( plot->driver == 'P' || plot->driver == 'p')  
  { plot->driver = 'P';
    mapboard = cpgopen("/cps");
    C = WHITE; 
    STRC = BLUE;
    SYMC = CRIMSON;
  }
  else  
  { mapboard = cpgopen("/xs");
    C = WHITE;
    STRC = YELLOW;;
    SYMC = CYAN;
  }

  cpgpap(SIDE, SQUARE);  
  
  /* Dave Green's CubeHelix colour scheme *
  s = 1.0;
  r = 0.175;
  l = 1.0;
  h = 1.0;//HUE;
  g = GAMMA;

  for(i = 0 ; i < MAXCOL ; i++)
  { L = l*(i+1)/MAXCOL;
    a = h*pow(L,g)*(1 - pow(L,g))/2;
    phi = 2*M_PI*(s/3 + r*L);
    R = pow(L,g) + a*( -0.14861*cos(phi) + 1.78277*sin(phi) );
    G = pow(L,g) + a*( -0.29277*cos(phi) - 0.90649*sin(phi) );
    B = pow(L,g) + a*( 1.97294*cos(phi) + 0.0*sin(phi) );
    cpgscr(16+i, R, G, B);
  }
  
  /*
  for(i = 0 ; i < MAXCOL ; i++)
  { R = 0.75*(float)(i)/(float)(MAXCOL) + 0.25;
    G = maxf(0.0, 2.0*(float)(i-MAXCOL/2)/(float)(MAXCOL+1));
    B = 0.0 + 0.4*(float)(MAXCOL-i)/(float)(MAXCOL);
    cpgscr(16+i, R, G, B);
  }
  /**/

  read_colormap("coolwarm.cmap", cmap);

  for(i = 0 ; i < MAXCOL ; i++)
  { R = cmap[i].r;
    G = cmap[i].g;
    B = cmap[i].b;
    cpgscr(16+i, R, G, B);
  }
  

  vwp[0] = 0.2;
  vwp[1] = 0.8;
  vwp[2] = 0.21;
  vwp[3] = 0.77;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  
  XMIN = 0.5;
  XMAX = XPIX + 0.5;
  YMIN = 0.5;
  YMAX = YPIX + 0.5;

  cpgsch(1.0);
  cpgsci(C);
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  cpgbox("BCN", 0.0, 0, "BCN", 0.0, 0);
  cpgscir(16,15+MAXCOL);  
  cpgsitf(itf);


  cpgsch(2.0);
  cpgslw(10.0);

  min = (1.0 - 1e-4)*(float)arraymin(ds->databuf+ch*XPIX*YPIX, XPIX*YPIX);
  max = (1.0 + 1e-4)*(float)arraymax(ds->databuf+ch*XPIX*YPIX, XPIX*YPIX);
  
  if(plot->colour)
    { cpgimag( temp+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, 1, YPIX, min, max, TR );
     if( ! strcmp ( ds->axlbl[2], "Baseline" ) ) {
	 //cpgimag( ds->databuf+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, YPIX, 1, min, max, TR );
      cpgwedg( "TI", 0.5, 3.5, min, max, "Jy\\u2\\d");
       }
    else cpgwedg( "TI", 1.0, 3.5, min, max, "");
  }
  else if(!(plot ->colour) && plot->driver == 'P')
  { cpggray( temp+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, 1, YPIX, max, min, TR );
     if( ! strcmp ( ds->axlbl[2], "Baseline" ) ) {
	 //cpgimag( ds->databuf+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, YPIX, 1, min, max, TR );
      cpgwedg( "TG", 0.5, 3.5, max, min, "Jy\\u2\\d");
     }
else
    cpgwedg( "TG", 1.0, 3.5, min, max, "");
  }
  else
  { cpggray( temp+ch*XPIX*YPIX, XPIX, YPIX, 1, XPIX, 1, YPIX, max, min, TR );
    cpgwedg( "TG", 1.0, 3.5, max, min, "");
  }
 
  cpgsch(1.0);
  cpgsci(!C);
  cpgslw(35);
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgbox("BCN", 0.0, 0, "BCN", 0.0, 0);
  cpgslw(1);

  XMIN = (float)ds->min[0];
  XMAX = (float)ds->max[0];
  YMIN = (float)ds->min[1];
  YMAX = (float)ds->max[1];
  cpgsch(2.0);
  cpgslw(12.0);  
  cpgsci(C);
  cpgswin(XMIN, XMAX, YMAX, YMIN);

  cpgsci(C);

  if( ! strcmp ( ds->axlbl[2], "Baseline" ) ) cpgbox("BCNT", 0.0, 0, "BCNTV", 0.0, 0);
  else cpgbox("BCNTS", 0.0, 0, "BCNMTSV", 0.0, 0);

  cpgmtxt("B",3.0, 0.5, 0.5, ds->axlbl[0]);
  cpgmtxt("L",3.2, 0.5, 0.5, ds->axlbl[1]);
  cpgslw(1);

  strcpy(colorscale, "Colour Scale : ");
  if(itf == LIN ) strcat(colorscale, "lin\0");
  else if(itf == LOG ) strcat(colorscale, "log\0");
  else if(itf == SQR ) strcat(colorscale, "sqrt\0");
  
  if( ! strcmp ( ds->axlbl[2], "Frequency MHz" ) )
  { fprintf(finfo, "%s Showing channel # %3d\r", info, ch+1);
    sprintf(chaninfo, "Channel # %d", ch+1);
  }
  else if( ! strcmp ( ds->axlbl[2], "Baseline" ) )
  { fprintf(finfo, "%s Showing baseline # %3d\r", info, ch+1);
    sprintf(chaninfo, "Baseline # %d", ch+1);
cpgsci(WHITE);
cpgslw(3.0);
cpgsls(2);
cpgline(proj->chans, axis, nues);
cpgline(proj->chans, nues, axis);

  }
  else if( ! strcmp ( ds->axlbl[2], "Record" ) )
  { fprintf(finfo, "%s Showing record # %3d\r", info, ch+1);
    sprintf(chaninfo, "Record # %d", ch+1);
  }

  /* Next page or quit for X-window */
  if( ret == 1  && plot->driver == 'X')
  { cpgsch(0.7); 
    cpgsci(SYMC);
    cpgmtxt("B", 6.0, -0.08, 0.0, "l      g       s         n            p             space                  q");
    cpgsci(STRC);
    cpgmtxt("B", 6.0, -0.08, 0.0, " : lin   : log    : sqrt     : chan++   : chan--          : toggle colour    : quit");
    cpgsci(STRC);
    cpgmtxt("B", 6.0, 0.85, 0.0, colorscale);
    cpgmtxt("B", 4.5, 0.85, 0.0, chaninfo);
    ret = cpgcurs(&x, &y, &c);
    if( c == 'q' ) { ret = 0; }
    if( c == 'l' ) { c = '\0';itf = LIN;  ret = 1; }
    if( c == 'g' ) { c = '\0';itf = LOG;  ret = 1; }
    if( c == 's' ) { c = '\0';itf = SQR;  ret = 1; }
    if( c == 'n' ) { c = '\0';ch = (ch+1)%proj->chans; ret = 1; }
    if( c == 'p' ) { c = '\0';ch = (proj->chans+ch-1)%proj->chans; ret = 1; }
    else if ( c == ' ' )
    { c = '\0';
      plot->colour = !(plot->colour); 
      ret = 1; 
    }
  }

  if( ! strcmp ( ds->axlbl[2], "Baseline" ) && ret == 1 && (c != '\0' )  )
  { cpgslct(mapboard);
    cpgclos();
    pierce(x, y, proj);
  }
  
  else
  { cpgslct(mapboard);
    cpgclos();  
  }  
  
  if(plot->driver == 'P')  ret = 0;

  }
  if(abs( strcmp(ds->axlbl[2],"")) ) newline();
  return ret;
  cpgclos();
}

int wedgeplot_modelvis( ProjParType *proj, PlotParType *plot )
{ 
  DisplayBufParType *ds = &(proj->dispBuf);
  float TR[6] = { 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
  float XMIN, XMAX, YMIN, YMAX;
  float *vwp;
  float R, G, B;
  int C, SYMC, STRC;
  int i;
  long ii;
  float min, max;
  float x = 1.5, y = 1.5;
  char c;
  char colorscale[32];
  int N, M;
  int ret = 1;
  int offset = 2*ds->altswtch*((proj->redundant_baselines+1) * proj->chans);
  float l, h, a, g, s, r, phi, L;
  int itf = plot->itf;
  colormap *cmap;
  float *temp;


  cmap = (colormap*)calloc(MAXCOL, sizeof(colormap));

  M = proj->chans;
  N = proj->redundant_baselines+1;

  temp = (float*)calloc(4*N*M, sizeof(float));
  for(i = 0 ; i < 4*N*M; i++)
      temp[i] = (float)ds->databuf[i];
  
  if( plot->driver == 'P' || plot->driver == 'p')  
  { C = PSCOLOR;
    SYMC = MAGENTA;
    STRC = BLUE;
  }
  else  
  { C = WHITE;//COLOR;
    SYMC = CYAN;
    STRC = YELLOW;
  }

  cpgslct( plot->DSboard );
  /*
  for(i = 0 ; i < MAXCOL ; i++)
  { R = 0.75*(float)(i)/(float)(MAXCOL) + 0.25;
    G = maxf(0.0, 2.0*(float)(i-MAXCOL/2)/(float)(MAXCOL+1));
    B = 0.0 + 0.4*(float)(MAXCOL-i)/(float)(MAXCOL);
    cpgscr(16+i, R, G, B);
  }
  */
  /* Dave Green's CubeHelix colour scheme 
  s = 3.0;
  r = 1.0;
  l = 1.0;
  h = HUE;
  g = GAMMA;

  for(i = 0 ; i < MAXCOL ; i++)
  { L = l*(i+1)/MAXCOL;
    a = h*pow(L,g)*(1 - pow(L,g))/2;
    phi = 2*M_PI*(s/3 + r*L);
    R = pow(L,g) + a*( -0.14861*cos(phi) + 1.78277*sin(phi) );
    G = pow(L,g) + a*( -0.29277*cos(phi) - 0.90649*sin(phi) );
    B = pow(L,g) + a*( 1.97294*cos(phi) + 0.0*sin(phi) );
    cpgscr(16+i, R, G, B);
  }
  */

  read_colormap("coolwarm.cmap", cmap);

  for(i = 0 ; i < MAXCOL ; i++)
  { R = cmap[i].r;
    G = cmap[i].g;
    B = cmap[i].b;
    cpgscr(16+i, R, G, B);
  }
  
  min = (float)ds->min[0];
  max = (float)ds->max[0];
  vwp = plot->DS_amp_vwp;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  cpgsci(C);
  XMIN = 0.5;
  XMAX = (float)(N) + 0.5;
  YMIN = 0.5;
  YMAX = (float)(M) + 0.5;
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  cpgbox("BCN", 0.0, 0, "BC", 0.0, 0);
  cpgscir(16,15+MAXCOL);  
  cpgsitf(itf);
  cpgimag( temp + offset, N, M, 1, N, 1, M, min, max, TR );
  cpgwedg( "LI", 1.0, 2.5, min, max, "");
  if(!ds->altswtch)
  { cpgmtxt("L", 4.5, 0.4, 0.0, "Amplitude - Jy");
    cpgmtxt("B", 2.75, 0.42, 0.0, "Baseline");
  }
  else
  { cpgmtxt("L", 4.5, 0.42, 0.0, "Real - Jy");
    cpgmtxt("B", 2.75, 0.42, 0.0, "Baseline");
  }
 
if( plot->driver == 'x' || plot->driver == 'X')
  { cpgsci(SYMC);
    cpgmtxt("T", 2.5, -0.15, 0.0, "l           g                s                  space                 q");
    cpgsci(STRC);
    cpgmtxt("T", 2.5, -0.15, 0.0, " : linear     : logarithmic     : square root           : toggle view      : quit");
  } 

 min = (float)ds->min[1];
 max = (float)ds->max[1];
  vwp = plot->DS_phs_vwp;
  cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
  cpgsci(C);
  XMIN = 0.5;
  XMAX = (float)(N) + 0.5;
  YMIN = 0.5;
  YMAX = (float)(M) + 0.5;
  cpgswin(XMIN, XMAX, YMAX, YMIN);
  cpgsci(C);
  cpgbox("BCN", 0.0, 0, "BCVN", 0.0, 0);
  cpgscir(16,15+MAXCOL);  
  cpgsitf(itf);
  cpgimag( temp + offset + N*M, N, M, 1, N, 1, M, min, max, TR );
  cpgwedg( "RI", 1.0, 2.5, min, max, "");
  if(!ds->altswtch)
  { cpgmtxt("R", 4.5, 0.4, 0.0, "Phase - deg");
    cpgmtxt("B", 2.75, 0.42, 0.0, "Baseline");
  }
  else
  { cpgmtxt("R", 4.5, 0.42, 0.0, "Imag - Jy");
    cpgmtxt("B", 2.75, 0.42, 0.0, "Baseline");
  }

  cpgmtxt("L", 1.4, 0.92, 0.0, "<--");
  cpgmtxt("T", 0.5, -0.125, 0.0, "Channel #");

  cpgsch(0.8);
  strcpy(colorscale, "Colour Scale : ");
  if(itf == LIN ) strcat(colorscale, "lin\0");
  else if(itf == LOG ) strcat(colorscale, "log\0");
  else if(itf == SQR ) strcat(colorscale, "sqrt\0");
  cpgsci(STRC);
  cpgmtxt("T", 2.5, 0.8, 0.0, colorscale);

  if( ret == 1  && plot->driver == 'X')
  {
    ret = cpgcurs(&x, &y, &c);
    if( c == 'q' ) ret = 0;
    else if( c == 'l' ) { plot->itf = LIN;  ret = 1; }
    else if( c == 'g' ) { plot->itf = LOG;  ret = 1; }
    else if( c == 's' ) { plot->itf = SQR;  ret = 1; }
    else if( c == ' ' ) { ds->altswtch = !(ds->altswtch); ret = 1; }
  }

  if( plot->driver == 'P' || plot->driver == 'p') ret = 0;
  cpgclos();  

  return ret;
}
