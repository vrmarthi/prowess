#ifndef _PLOTDEFS_H_
#define _PLOTDEFS_H_

#include "cpgplot.h"
#include "matdefs.h"
#include "sysdefs.h"

#ifdef _LAPTOP
#define VWPSIDE 4.1
#define XSIDE 3.3
#define YSIDE 0.508
#endif

#ifdef _WS
#define VWPSIDE 6.0
#define XSIDE 3.23
#define YSIDE 0.52

#endif

#define LIN 0
#define LOG 1
#define SQR 2

#define BLACK 0
#define WHITE 1
#define RED 2
#define GREEN 3
#define BLUE 4
#define CYAN 5
#define MAGENTA 6
#define YELLOW 7

#define OCHRE 8
#define PARROT 9
#define OLIVE 10
#define INDIGO 11
#define PURPLE 12
#define CRIMSON 13
#define MINIMAL_GREY 14
#define MEDIUM_GREY 15

#define XSCOLOR MEDIUM_GREY
#define PSCOLOR WHITE

#define GAMMA 1.0
#define HUE   1.0

#define ROWS 3
#define COLS 5

#define NCLEV 30

typedef struct
{
 float r,g,b;
}colormap;

typedef struct 
{
 char driver;
 int DSboard;
 int dashboard;
 int gcmplxboard;
 int Mcmplxboard;
 int MSEboard;
 int chisqboard;
 int SNRboard;
 int covarboard;
 int Hessianboard;
 int M_board;
 int G_board;
 int scale_board;
 int antennas;
 int redundant_baselines;
 int nsol;
 int MSE_opened;
 int chisq_opened;
 int SNR_opened;
 int covar_opened;
 int Hessian_opened;
 int iter;
 int monochrome;
 int colour;
 int itf;
 float g_rad;
 float V_rad;
 float scale;
 float g_amp_min;
 float g_amp_max;
 float g_phs_min;
 float g_phs_max;
 float V_amp_min;
 float V_amp_max;
 float V_phs_min;
 float V_phs_max;
 float MSE_min;
 float MSE_max;
 float chisq_min;
 float chisq_max;
 float SNRmax;
 float covarmax;
 float covarmin;
 float Hessianmax;
 CmplxType **gain;
 CmplxType **vis;
 CmplxType *g_true;
 CmplxType *V_true;
 CmplxType *g_err;
 CmplxType *M_err;
 float **g_amp, **g_phs;
 float **V_amp, **V_phs;
 float *g_true_amp, *g_true_phs;
 float *V_true_amp, *V_true_phs;
 float *MSE;
 float *chisq;
 float *SNR;
 float *covar;
 float *Hessian;
 float DS_amp_vwp[4];
 float DS_phs_vwp[4];
 float g_amp_vwp[4];
 float g_phs_vwp[4];
 float V_amp_vwp[4];
 float V_phs_vwp[4];
 float g_cmplx_vwp[4];
 float V_cmplx_vwp[4];
 float MSE_vwp[4];
 float chisq_vwp[4];
 float SNR_vwp[4];
 float covar_vwp[4];
 float Hessian_vwp[4];
 float M_amp_vwp[4];
 float M_phs_vwp[4];
 float G_amp_vwp[4];
 float G_phs_vwp[4];
 float ampscale_vwp[4];
 float phoffset_vwp[4];
}PlotParType;
void set_color_index( int, int * );
void init_DSscope( PlotParType * );
void init_trajscope( PlotParType * );
void init_errscope( PlotParType * );
void init_SNRscope( PlotParType * );
void init_covarscope( PlotParType * );
void init_Hessianscope( PlotParType * );
void init_gscope( PlotParType * );
void init_Mscope( PlotParType * );
void init_scalescope( PlotParType * );
void open_DSscope( PlotParType * );
void open_trajscope( PlotParType * );
void open_gcmplxscope( PlotParType * );
void open_Mcmplxscope( PlotParType * );
void open_MSEscope( PlotParType * );
void open_chisqscope( PlotParType * );
void open_errscope( PlotParType * );
void open_SNRscope( PlotParType * );
void open_covarscope( PlotParType * );
void open_Hessianscope( PlotParType * );
void open_gscope( PlotParType * );
void open_Mscope( PlotParType * );
void open_scalescope( PlotParType * );
void parse_solfile( PlotParType *, char * );
void parse_MSE_file (PlotParType * );
void parse_chisq_file (PlotParType * );
void read_solutions( PlotParType *, char * );
void read_MSE( PlotParType * );
void read_chisq( PlotParType * );
void read_SNR_matrix( PlotParType * );
void read_covar_matrix( PlotParType * );
void read_CRB_matrix( PlotParType * );
PolarType compute_errorbar( CmplxType, CmplxType );
void read_Hessian_matrix( PlotParType * );
void read_Fisher_matrix( PlotParType * );
void psplot_gain_and_vis( PlotParType *, char * );
void xsplot_gain_and_vis( PlotParType *, char * );
void xsplot_all( PlotParType *, char * );
void psplot_all( PlotParType *, char * );
void psplottraj( PlotParType *, char * );
void xsplottraj( PlotParType *, char * );
void psplotsol( PlotParType *, char * );
void xsplotsol( PlotParType *, char * );
int  mvps_to( char * );
void xsplot_all_but_errors( PlotParType *, char * );
void psplot_all_but_errors( PlotParType *, char * );
void xsplot_solcompare( PlotParType *, char * );
void psplot_solcompare( PlotParType *, char * );
void xsplot_Hessian_and_covar( PlotParType *, char * );
void psplot_Hessian_and_covar( PlotParType *, char * );
void xsplot_Fisher_and_CRB( PlotParType *, char * );
void psplot_Fisher_and_CRB( PlotParType *, char * );
void plot_itergain( PlotParType * );
void plot_itervis( PlotParType * );
void plot_gaincompare( PlotParType * );
void plot_viscompare( PlotParType * );
void plot_MSE( PlotParType * );
void plot_chisq( PlotParType * );
void plot_SNR_matrix( PlotParType * );
void plot_covar_matrix( PlotParType * );
void plot_Hessian_matrix( PlotParType * );
void plot_model_visibilities( PlotParType * );
void plot_scaled_model_visibilities( PlotParType * );
void plot_scaled_par_w_errorbar( PlotParType * );
void plot_scaled_par_w_CRB( PlotParType * );
void plot_gain( PlotParType * );
void plot_scaled_gain( PlotParType * );
void plot_gain_errorbar( PlotParType * );
void plot_vis_errorbar( PlotParType * );
void plot_scale( PlotParType * );
int  wedgeplot_dynspec( ProjParType *, PlotParType * );
int  wedgeplot_instspec( ProjParType *, PlotParType * );
int  wedgeplot_map( ProjParType *, PlotParType * );
int  imag_dynspec( ProjParType *, PlotParType * );
int  imag_instspec( ProjParType *, PlotParType * );
int  xs_nplot_timevis( ProjParType *);
int  ps_nplot_timevis( ProjParType *);
int  xs_nplot_chanvis( ProjParType *);
int  ps_nplot_chanvis( ProjParType *);

#endif
