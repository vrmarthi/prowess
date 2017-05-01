#ifndef FITSDEFS_H
#define FITSDEFS_H

#include "../cfitsio/fitsio.h"
#include "../cfitsio/fitsio2.h"
#include "sysdefs.h"

#define RGParSize sizeof(RGParType)
#define ANTabSize sizeof(AntennaTableType)
#define FQTabSize sizeof(FrequencyTableType)
#define SUTabSize sizeof(SourceTableType)

typedef struct
{
 double u;
 double v;
 double w;
 double baseline;
 double date1;
 double date2;
 double su;
 double freq;
}__attribute__((packed)) RGParType;

typedef struct
{
 char     anname[8] ;
 double   x,y,z ;
 int      nosta,mntsta ;
 float    staxof ;
 char     poltya[4] ;
 float    polaa, polcala[4] ;
 char     poltyb[4]  ;
 float    polab, polcalb[4] ;
} __attribute__((packed)) AntennaTableType;

typedef struct
{
 int      id ;
 char     src[16] ;
 int      qual ;
 char     calcode[4] ;
 double   iflux[2],qflux[2],uflux[2],vflux[2];
 double   freq_off[2],bandwidth, ra,dec,epoch,ra_app,dec_app ;
 double   lsrvel[2], rest_freq[2], pmra,pmdec ;
} __attribute__((packed)) SourceTableType;

typedef struct
{
 int      id ;
 double   iffreq;
 double    chwidth;
 double    bandwidth;
 int      sideband;
} __attribute__((packed)) FrequencyTableType;


void printerror( int );
void open_fits( fitsfile **, char * );
void create_fits( fitsfile **, char * );
void close_fits( fitsfile * );

void init_hdr(fitsfile *, ProjParType *);
int fill_proj (fitsfile *, ProjParType * );

void getpar_baseline( fitsfile *, ProjParType * );

int writeVis2FITS(fitsfile *, ProjParType * );
int readFITS2Vis(fitsfile *, ProjParType * );
int readFITSrec(fitsfile *, ProjParType * );

void write_maps2FITS( ProjParType * );
int read_FITSmap( ProjParType *, char * );
int FITSmap_to_model( ProjParType *, char * );


void write_EXtabs ( fitsfile *, ProjParType * );
void read_EXtabs ( fitsfile *, ProjParType * );

int writeANtab(fitsfile *, ProjParType *);
int readANtab( fitsfile *, ProjParType * );

int writeFQtab(fitsfile *, ProjParType *);
int readFQtab( fitsfile *, ProjParType * );

int writeSUtab(fitsfile *, ProjParType *);
int readSUtab( fitsfile *, ProjParType * );

#endif
