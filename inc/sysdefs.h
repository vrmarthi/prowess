#ifndef SYSDEFS_H
#define SYSDEFS_H

#include <time.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_multimin.h>
#include "skydefs.h"
#include "matdefs.h"
#include "remezdefs.h"

enum { START_COUNT, STOP_COUNT, TEST };

#define ON               1
#define OFF              0

#define IATUTC           34

#define MAX_SCANS        1000
#define MAX_SRC          1000

#define LIGHTVEL         299792458
#define SAMPFREQ         78000000
#define BAND_CENTRE      326500000
#define BANDWIDTH        SAMPFREQ/2
#define LAMBDA           LIGHTVEL/BAND_CENTRE
#define PHASE_I          40
#define PHASE_I_RX       "PRx1.0"
#define PHASE_II         264
#define PHASE_II_RX      "PRx2.0"
#define MAX_ANTS         264

#define FITS_BASELINE_START 256

#define FRAC_TSYS        0.5
//#define GAIN 
//#define TSYS 
//define STEP
#define CHANS            256

#define ORT_LAT          11.38056
#define ORT_LNG          76.66667
#define ORT_ALT          2150

#define AntParSize       sizeof(AntParType)
#define BaseParSize      sizeof(BaseParType)

#define WARN             "|WARN>\0"
#define INFO             "|INFO>\0"
#define ERR              "| ERR>\0"
#define PROMPT           "|    >\0"


#define finfo            stderr
#define fwarn            stderr
#define ferr             stderr
#define fprompt          stderr

extern char err[32];
extern char info[32];
extern char warn[32];
extern char prompt[32];

extern int BigEndian    ;
extern int LittleEndian ;

extern double RefFreq;

typedef struct
{
 double latitude;
 double longitude;
 double altitude;
}StationParType;

typedef struct
{
 double  lambda;
 double  length;
 double  sep;
}DipoleParType;

typedef struct
{
 double gain;
 double bias;
 double Tsys;
 double HPbandwidth;
}SysParType;

typedef struct 
{
 int id;
 char name[5];
 double bx, by, bz;
 double ddly, fdly;
 PolarType gain;
 CmplxType g;
}AntParType;

typedef struct
{ 
 int number;
 int id1, id2;
 char ant1[5];
 char ant2[5];
 double u;
 double v;
 double w;
 CmplxType b;
}BaseParType;

typedef struct
{
 int scan_num;
 int recs;
 int sid;
 char src[16];
 RAType ra;
 DecType dec;
 double flux;
 double epoch;
}ScanParType;

typedef struct
{
 int xside;
 int yside;
 int zside;
 double b;
 double d;
 double ra_res;
 double dec_res;
 RAType *ra;
 DecType *dec;
 double ***shape;
}PrimaryBeamParType;

typedef struct
{
 int a1,a2; //antenna
 int b1, b2; //baseline
 int c1, c2; //channel
 int d1, d2; //record
}FlagParType;

typedef struct
{
 int xside;
 int yside;
 int zside;
 double ra_res;
 double dec_res;
 RAType *ra;
 DecType *dec;
 double ***map;
}SkyMapType;

typedef struct
{
 gsl_matrix *QR;
 gsl_vector *tau;
 gsl_vector *x;
 gsl_vector *b;
}QRDType;

typedef struct
{
 gsl_matrix *U;
 gsl_matrix *V;
 gsl_vector *S;

}SVDType;

typedef struct
{
 gsl_matrix *time_updater;
 gsl_matrix *cov_err_sol;
 gsl_matrix *Q;
 gsl_matrix *kal_gain;
 gsl_matrix *H;
 gsl_matrix *cov_err_meas;
}KalmanParType;

typedef struct
{
 int    ant1, ant2;
 int    baseline;
 int    start_chan, stop_chan, nchan;
 int    start_rec, stop_rec, nrec;
 int    nplot;
 int    nrow, ncol;
 double  tick;
 double  deltanu;
 double *xaxis;
 double *databuf;
 double  min[2], max[2];
 int    altswtch;
 int    chan2blank;
 char   axlbl[10][80];
}DisplayBufParType;

typedef struct
{
 double     ptg_ra;
 double     ptg_dec;
 double     tgt_ra;
 double     tgt_dec; 
 CmplxType *data;
}BeamParType;

typedef struct 
{ Cmplx3Type  *bpass;       /* dimension nchan*nstokes*nant                   */
  double     date;        /* center of the soln block [randpar date1+date2] */
} __attribute__((packed)) BPassTableRow;
typedef struct 
{ int            flag;        /* flag for the bpass table                    */
  int            nrow;        /* number of rows in the bpass table           */
  int            nant;        /* number of antennas in the bpass table       */
  int            nchan;       /* number of channels in the bpass table       */
  int            nstokes;     /* number of stokes in the gain table          */
  BPassTableRow *row;         /* array of actual solutions, dimension nrow   */
  char           source[32];  /* calibration source name                     */
  double         flux[4];     /* stokes parameters of the source             */
  int            rowdone;     /* extraneous parameter, not part of standard GTable */
                              /* Added on 15 Apr-15 VRM */          
} __attribute__((packed)) BPassTableType;

typedef struct 
{ Cmplx3Type  *gain;       /* dimension nstokes*nant                          */
  double     date;       /* center of the gain block [randpar date1+date2]  */
}  __attribute__((packed)) GainTableRow;
typedef struct 
{ int            flag;        /* is this gain table flagged?            */
  int            nrow;        /* number of rows in the gain table       */
  int            nant;        /* number of antennas in the gain table   */
  int            nstokes;     /* number of stokes in the gain table     */
  GainTableRow  *row;         /* array of actual gains, dimension nrow  */
  double         scale_fac[4];/* per stokes scalefactor to convert to Jy*/
  char           source[32];  /* calibration source name                */
  double         freq;        /* frequency of the visibilities from which
				this gain table was derived            */
  double         flux[4];     /* stokes parameters of the source        */
  int            rowdone;     /* extraneous parameter, not part of standard GTable */
                              /* Added on 15 Apr-15 VRM */          
} __attribute__((packed)) GainTableType;


typedef struct
{
 StationParType station;
 DipoleParType  dipole;
 SysParType     system;
 DisplayBufParType dispBuf;
 BeamParType    beam;
 int            fits_rewind;

 char           obsmode[10]; 
 char           code[16];
 char           observer[40];
 char           receiver[8];
 int            ORTelements;

 int            sidebands; 
 double         bandcentre;
 double          bandwidth;
 int            chans;
 int            chan2cal;
 int            pols;
 
 double         mjd_start;
 double         tick;
 int            preint;

 int            ants;
 double         *delphi;
 AntParType    *Ant;
 long           baselines;
 int           *unique_baseline;
 int            redundant_baselines;
 long           blindex[MAX_ANTS];
 BaseParType   *Base;

 int            uvmin;
 int            uvmax;
 double          threshold;

 int            scans;
 long           nsrc;
 ScanParType    scan[MAX_SCANS];
 SkySrcType    *skysrc;
 GalFGType      fg;
 PolGalFGType   polfg;
 EPSFGType      eps;
 HIMapType      HI;
 TECScrType     tec;
 SkyMapType     skymap;
 PrimaryBeamParType pbeam;
 FlagParType     flagpar;
 int            pntsrc_swtch;
 int            galfg_swtch;
 int            pbeam_swtch;
 int            h1_swtch;
 int            scale;
 long           pcount;
 long           recwords;
 int            nrecs;
 int            recnow;
 int         ***flagmask;
 int           *ant2flag;
 double         *RecBuf;
 double         *bpBuf;
 double         *window;
 double         *SNR;
 double         *sigma;
 gsl_matrix    *V_re;
 gsl_matrix    *V_im;
 CmplxType    **V_sum;
 CmplxType    **V_sum_rec;
 double        **V_ofst;
 int          **V_count;
 double      ***V2;
 double      ***V2_rec;
 double      ***V2_err;
 double        *V2_dnu;
 int            V2_init_done;

 CmplxType     *H_w;

 double       **ULambda;
 CmplxType    **model_vis;
 CmplxType    **MV_errbar;

 int            iter;
 gsl_matrix    *A;
 gsl_matrix    *B;
 int           *keymap;

 gsl_vector    *tau;
 gsl_matrix    *QR;
 gsl_vector    *QRres;

 gsl_matrix    *U, *V;
 gsl_vector    *S;

 gsl_vector    *sol_re;
 gsl_vector    *sol_im;

 gsl_vector    *delta_sol;
 gsl_vector    *delta_V_obs;

 gsl_vector    *grad_re;
 gsl_vector    *grad_im;

 gsl_vector    *R_re;
 gsl_vector    *R_im;

 gsl_vector    *Q_re;
 gsl_vector    *Q_im;

 gsl_vector    *prev_sol_re;
 gsl_vector    *prev_sol_im;

 gsl_vector    *d_sol_re;
 gsl_vector    *d_sol_im;

 KalmanParType *kalman;

 GainTableType  gtab;
 BPassTableType bptab;

}ProjParType;

void done ( void );
void getPrompt( char *, char*, char );
void define_prompts( char * );
void getMachineEndianness( int *, int * );
char* suffix( char*, char* );

void setpar_station( ProjParType * );
void setpar_dipole ( ProjParType * );
void setpar_system ( ProjParType * );
int  get_RxVer ( char antstr[MAX_ANTS][80] );
int  get_num_ants( char antstr[MAX_ANTS][80] );
int  count_redundant_baselines( ProjParType * );
void setpar_ant    ( ProjParType *, char antstr[MAX_ANTS][80] );
void time_evolve_gains( ProjParType *, CmplxType * );
void setpar_baseline( ProjParType * );
void show_antennas ( ProjParType * );
void show_baselines ( ProjParType * );
void baseline_number_to_ant_ids( BaseParType *, int );
int  antid_to_baseline_number(int, int, ProjParType * );
void set_baseline_index( ProjParType * );
void index_ants( ProjParType * );
void parse_ProjParFile( ProjParType * );
void parse_VLACalManual( ProjParType * );
void flash_obs_settings( ProjParType * );
int  getpar_dynspect( ProjParType *, int, int );
void getpar_instspec( ProjParType * );
void getpar_timevis( ProjParType * );
int  init_proj  ( ProjParType * );
void reproj ( ProjParType *, ProjParType * );
void reband ( ProjParType *, ProjParType * );
void kill_proj( ProjParType * );
void supply_initial_solutions( ProjParType * );
void dummy_gsl_alloc( ProjParType * );
void all_gsl_free( ProjParType * );
void free_display_databuf( ProjParType * );
void buffer_range( DisplayBufParType *, int, int );
void init_gain_table( ProjParType * );
void close_gain_table( ProjParType * );
//void write_gaintab( ProjParType * );
void init_bandpass_table( ProjParType * );
void close_bandpass_table( ProjParType * );
void write_bpasstab( ProjParType * );
int  genVis( ProjParType * );
void extract_channel( ProjParType *, int );
void extract_vis( ProjParType * );
void writevis( ProjParType *, FILE * );
int  extract_baseline( ProjParType *, int );
void set_skysrc( ProjParType * );
void gen_V_sky( ProjParType * );
void write_map( ProjParType * );
void resize_fgmap( ProjParType * );
void add_galfg_to_model( ProjParType * );
void compute_primary_beam( ProjParType * );
void free_primary_beam( ProjParType * );
void free_gal_fgmap( ProjParType * );
void free_skymap( ProjParType * );
void free_sky_signal( ProjParType * );
void look_through_primary_beam( ProjParType * );
void correct_primary_beam( ProjParType * );
void init_skymap( ProjParType * );
void add_pntsrc_to_galfg( ProjParType * );
void init_sky_signal( ProjParType * );
void make_bandpass_filter( ProjParType *, int );
void compose_A( ProjParType * );
void log_QR_decomp( ProjParType *);
void log_QR_solve ( ProjParType *);
void convert_sol_log2lin( ProjParType * );
void log_SV_decomp( ProjParType *);
void log_SV_solve ( ProjParType *);
void compose_B( ProjParType * );
void B_index( ProjParType * );
void assign_keymap_B( ProjParType *);
void setup_QRD( ProjParType * );
void setup_SVD( ProjParType * );
void setup_gradient_search( ProjParType * );
void setup_itersol( ProjParType * );
void reinit_itersol( ProjParType * );
void force_model( ProjParType * );
void setup_itergain( ProjParType * );
void setup_lincal( ProjParType * );
void lin_QRD_solve( ProjParType * );
void lin_SVD_solve( ProjParType * );
void update_sol ( ProjParType * );
void update_B( ProjParType * );
void compute_gradient( ProjParType * );
void descend_along_gradient( ProjParType * );
void iterate_solution( ProjParType * );
void iterate_gains( ProjParType * );
void update_solutions( ProjParType * );
int  test_convergence( ProjParType * );
void compute_MSE( ProjParType *, FILE * );
void compute_SNR( ProjParType * );
void init_SNR_compute( ProjParType * );
double compute_chisq( ProjParType *, FILE * );
CmplxType pseudo_Hessian( ProjParType *, int, int, int );
void compute_error_matrix( ProjParType * );
void compute_CRLB( ProjParType * );
void compute_step_size( ProjParType *);
void write_sol( ProjParType *, FILE * );
void write_model_visibilities( ProjParType *, char *);
void read_model_visibilities( ProjParType *, char *);
void write_baseline_SNR_matx( ProjParType * );
Cmplx3Type ret_V_obs( ProjParType *, int, int );
CmplxType ret_V_true( ProjParType *, int, int );
#endif 

