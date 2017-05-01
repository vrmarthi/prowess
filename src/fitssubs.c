#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/matdefs.h"

void printerror(int status)
{ fits_report_error(stderr, status); }


void open_fits( fitsfile **fits, char *filename )
{
int status = 0;
if( fits_open_file(fits, filename, READWRITE, &status) ) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 fprintf(finfo, "%s %s found on disk\n", info, filename);
}

void create_fits( fitsfile **fits, char *filename )
{
 int status = 0;
 remove(filename);
 if(fits_create_file( fits, filename, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 fprintf(finfo, "%s %s opened on disk\n", info, filename);
}

void close_fits ( fitsfile *fits )
{
 int status = 0;
 if( fits_close_file(fits, &status) ) printerror(status) ;
}

void read_EXtabs( fitsfile *fits, ProjParType *proj)
{
 int status = 0;
 if( (status = readANtab(fits, proj) ) ) printerror(status);
 if( (status = readFQtab(fits, proj) ) ) printerror(status);
 if( (status = readSUtab(fits, proj) ) ) printerror(status);
}

void write_EXtabs( fitsfile *fits, ProjParType *proj)
{
 int status = 0;
 if( ( status = writeANtab(fits, proj) ) ) printerror(status);
 if( ( status = writeFQtab(fits, proj) ) ) printerror(status);
 if( ( status = writeSUtab(fits, proj) ) ) printerror(status);
}

int readFITS2Vis(fitsfile *fvis, ProjParType *proj )
{
 int status = 0, i, recnum;
 static long gcount = 0;
 int integ = proj->preint;

 unsigned long int len = proj->recwords * proj->baselines;
 double *accBuf ;
 
 if ( gcount >= proj->nrecs*proj->baselines ) gcount = 0;

 if( proj->fits_rewind == 1 ) gcount = 0;

 proj->fits_rewind = 0;


 accBuf = (double*)calloc(len, sizeof(double));

 for( i = 0; i < integ; i++ )
 { fits_read_tblbytes(fvis, 
		      1+gcount, 
		      1, 
		      len*sizeof(double), 
		      (unsigned char*)proj->RecBuf, 
		      &status); 
  if(LittleEndian) ffswap8( (double*)proj->RecBuf, len );
  extract_channel( proj, i );
  accumulate(accBuf, proj->RecBuf, len);
  gcount += (proj->baselines);
 }
 scale(accBuf, proj->RecBuf, integ, len);
 free(accBuf);

 recnum = gcount/proj->baselines;
 proj->recnow = recnum;
 return recnum;
}


int readFITSrec(fitsfile *fvis, ProjParType *proj )
{
 int status = 0;
 long gcount = 0;
 int rec2pull = proj->chan2cal;
 unsigned long int len = proj->recwords * proj->baselines;

 gcount = (rec2pull-1)*proj->baselines;

 fits_read_tblbytes(fvis, 
		      1+gcount, 
		      1, 
		      len*sizeof(double), 
		      (unsigned char*)proj->RecBuf, 
		      &status); 
  if(LittleEndian) ffswap8( (double*)proj->RecBuf, len );

  return status;
}

int writeFITSrec(fitsfile *fvis, ProjParType *proj )
{
 int status = 0;
 long gcount = 0;
 int rec2pull = proj->chan2cal;
 unsigned long int len = proj->recwords * proj->baselines;

 gcount = (rec2pull-1)*proj->baselines;

 fits_write_tblbytes(fvis, 
		      1+gcount, 
		      1, 
		      len*sizeof(double), 
		      (unsigned char*)proj->RecBuf, 
		      &status); 
  if(LittleEndian) ffswap8( (double*)proj->RecBuf, len );

  return status;
}

int writeVis2FITS(fitsfile *fvis, ProjParType *proj)
{
 int  status = 0;
 static long gcount = 0;
 static long rec = 0;

 if(LittleEndian) ffswap8( (double*)proj->RecBuf, proj->baselines * proj->recwords );
 fits_write_tblbytes(fvis, 
		     1+gcount, 
		     1, 
		     (long)( (proj->baselines)*proj->recwords*sizeof(double)), 
		     (unsigned char*)proj->RecBuf, 
		     &status); 
 printerror(status);
 gcount = (proj->baselines) * (++rec) ;
 return gcount;
}

void init_hdr(fitsfile *fvis, ProjParType *proj)
{ int bitpix, naxis, simple, extend, blocked, groups;
  long naxes[7], gcount = 1, parcount = RGParSize/sizeof(double);
  int status=0;
  int j;
  char *ctype[] = {"COMPLEX", "STOKES", "FREQ", "IF","RA","DEC"};
  double crval[] = {0.0,1.0,-1.0, 3.265E8,1.0,0.0,0.0};
  double cdelt[] = {0.0,1.0,-1.0, 1.6E7, 1.0,1.0E0,1.0E0};
  double crpix[] = {0.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
  double crota[] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  char *ptype[] = {"UU---SIN", "VV---SIN", "WW---SIN", "BASELINE", "DATE", "DATE","SOURCE","FREQSEL"};
  double pscal[] = {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
  double pzero[] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  char history[64];
  char keynew[16];
  char ref_date[32];
  double epoch = 2000.0;
  if (parcount !=8) { fprintf (stderr, "INVALID PARCOUNT = %ld\n", parcount) ; exit(-1); }
  
  strncpy(ref_date , mjd2iau_date(proj->mjd_start), 10) ;
  ref_date[10] = 0 ;
  crval[3] = RefFreq;
  crpix[3] = 0.5;
  cdelt[3] = proj->bandwidth / proj->chans ;
  crval[2] = -1.0;
  cdelt[2] = -1.0;
  simple   = TRUE;
  bitpix   = DOUBLE_IMG;
  naxis    = 7;
  naxes[0] = 0;
  naxes[1] = 3;
  naxes[2] = proj->pols;
  naxes[3] = proj->chans;
  naxes[4] = proj->sidebands;
  naxes[5] = 1;
  naxes[6] = 1;
  extend   = TRUE;
  blocked  = TRUE;
  groups   = TRUE;
  gcount   = 1;

  if( fits_write_grphdr(fvis, simple, bitpix, naxis, naxes, parcount, gcount, extend, &status) ) printerror(status);
  if ( fits_write_key_str(fvis,"OBJECT","MULTI", "",&status) ) printerror(status) ;
  if ( fits_write_key_str(fvis,"TELESCOP","ORT", "",&status) ) printerror(status) ;
  if ( fits_write_key_str(fvis,"INSTRUME", proj->receiver, "",&status) ) printerror(status) ;
  if ( fits_write_key_str(fvis,"OBSERVER", proj->observer, "",&status) ) printerror(status) ;
  if ( fits_write_key_str(fvis,"EXTNAME", "Primary", "",&status) ) printerror(status) ;
  if ( fits_write_history(fvis,"AIPS SORT ORDER = 'TB' ",&status) ) printerror(status);
  sprintf(history,"Created by SkySim version %s", "1.0");
  if ( fits_write_history(fvis,history,&status) ) printerror(status) ;
  sprintf(history,"ORT PRx CORR Version : DAS1.0");
  if ( fits_write_history(fvis,history,&status) ) printerror(status) ;
  if ( fits_write_key_str(fvis,"PROJECT", proj->code, "",&status) ) printerror(status) ;
  if ( fits_write_key_str(fvis,"OBSMODE", proj->obsmode, "",&status) ) printerror(status) ;
  if ( fits_write_key_str(fvis,"DATE-OBS",ref_date, "",&status) ) printerror(status) ;
  if ( fits_write_key_str(fvis,"DATE-MAP",ref_date, "",&status) ) printerror(status) ; 
  if ( fits_write_key_dbl(fvis,"BSCALE",1.0E+00 ,11,"",&status) ) printerror(status) ;
  if ( fits_write_key_dbl(fvis,"BZERO",0.0E+00 ,11,"",&status) ) printerror(status) ;
  if ( fits_write_key_str(fvis,"BUNIT","UNCALIB", "",&status) ) printerror(status) ;
  if ( fits_write_key_dbl (fvis,"EPOCH",epoch, 9, "",&status) ) printerror(status) ;
  if ( fits_write_key_flt (fvis,"ALTRPIX",1.0, 9 , "",&status) ) printerror(status) ;
  for (j=2; j <= naxis; j++)
    {
      if ( fits_make_keyn ("CTYPE",j,keynew, &status) ) printerror(status) ;
      if (fits_write_key_str(fvis,keynew,ctype[j-2], "",&status)) printerror(status) ;
      if ( fits_make_keyn ("CRVAL",j,keynew, &status) ) printerror(status) ;
      if (fits_write_key_dbl(fvis,keynew,crval[j-1],10,"" ,&status) ) printerror(status) ;
      if ( fits_make_keyn ("CDELT",j,keynew, &status) ) printerror(status) ;
      if ( fits_write_key_dbl (fvis,keynew,cdelt[j-1],9,"" ,&status) ) printerror(status) ;
      if ( fits_make_keyn ("CRPIX",j,keynew,&status) ) printerror(status) ;
      if ( fits_write_key_dbl (fvis,keynew,crpix[j-1],9,"" ,&status) ) printerror(status) ;
      if ( fits_make_keyn ("CROTA",j,keynew, &status) ) printerror(status) ; 
      if ( fits_write_key_dbl (fvis,keynew,crota[j-1],10,"" ,&status) ) printerror(status) ;

    }
  for ( j=1; j <= parcount ; j++)
    {
      if (ffkeyn ("PTYPE",j,keynew,&status) ) printerror(status) ;
      if (ffpkys (fvis,keynew,ptype[j-1],"", &status) ) printerror(status) ;
      if (ffkeyn ("PSCAL", j,keynew, &status) ) printerror(status) ;
      if ( ffpkye(fvis,keynew,pscal[j-1],11,"",&status)) printerror(status) ;
      if (ffkeyn ("PZERO",j,keynew,&status)) printerror(status) ;
      if (ffpkye (fvis,keynew,pzero[j-1],11,"",&status)) printerror(status) ;
    }

  printerror(status);
}

int writeANtab(fitsfile *fvis, ProjParType *proj)
{ double gstia0 ;
  double iatutc = IATUTC;
  long REFANT = 1 ;
  int row=0, status=0;
  int tfields =12;
  long nrows;
  char ref_date[24] ;
  char *ttype[]={ "ANNAME","STABXYZ","ORBPARM","NOSTA","MNTSTA","STAXOF","POLTYA","POLAA"  ,"POLCALA","POLTYB",  "POLAB", "POLCALB"};
  char *tform[]={ "8a"    ,"3d"     ,"0d"     ,"1j"   ,"1j"    ,"1e"    ,"4a"    ,"1e"     ,"4e"     ,    "4a",     "1e",      "4e"};
  char *tunit[]={ " "     ,"METERS" , " "     ," "    ," "     ,"METERS", " "    ,"DEGREES", " "     ,     " ","DEGREES",       " "};

  AntParType *ANT;
  AntennaTableType antab;

  nrows = proj->ants ;
  ANT = proj->Ant;

  strncpy(ref_date , mjd2iau_date(proj->mjd_start), 10) ;
  ref_date[10] = 0 ;
  { double mjd_iat0 = proj->mjd_start +5.5/24.0 + iatutc/86400.0 ;
    gstia0 = jd2gst(mjd_iat0+2400000.5) ;
  }

  if( fits_create_tbl(fvis, BINARY_TBL, row, tfields, ttype, tform, tunit, "AIPS AN", &status) ) return status;
  if (fits_write_key_lng(fvis,"EXTVER", 1,"Version number of table",&status))
    return status ;
  if (fits_write_key_dbl(fvis,"ARRAYX", 0.0,12,"",&status))
    return status ;
  if (fits_write_key_dbl(fvis,"ARRAYY", 0.0,12,"",&status))
    return status ;
  if (fits_write_key_dbl(fvis,"ARRAYZ", 0.0,12,"",&status))
    return status ;
  if (fits_write_key_dbl(fvis,"GSTIA0", gstia0,12,"GST at IAT=0 (degrees) on ref. date",&status))
    return status ;
  if (fits_write_key_dbl(fvis,"DEGPDY",0.36098564497436661e3 ,20,
	     "Earth rotation rate (deg/IAT day",&status))
    return status ;
  if (fits_write_key_dbl(fvis,"FREQ",RefFreq,12,"Reference frequency for array",&status))
    return status ;
  if (fits_write_key_str(fvis,"RDATE", ref_date,"Reference date 'YYYY-MM-DD'",&status))
    return status ;
  if (fits_write_key_dbl(fvis,"POLARX", 0.0,12,"Polar position X (meters) on Ref. date",&status))
    return status ;
  if (fits_write_key_dbl(fvis,"POLARY", 0.0,12,"Polar position Y (meters) on Ref. date",&status))
    return status ;
  if (fits_write_key_dbl(fvis,"UT1UTC", 0.0,12,"UT1-UTC (time sec.)",&status))
    return status ;
  if (fits_write_key_dbl(fvis,"DATUTC", 0.0,12," ",&status))
    return status ;
  if (fits_write_key_str(fvis,"TIMSYS", "IAT","Time system, 'IAT' or 'UTC' ",&status))
    return status ;
  if (fits_write_key_str(fvis,"ARRNAM", "ORT","Array name",&status))
    return status ;
  if (fits_write_key_lng(fvis,"NUMORB", 0,"Number of orbital parameters",&status))
    return status ;
  if (fits_write_key_lng(fvis,"NOPCAL", 4,"Number of pol. cal constants",&status))
    return status ;
  if (fits_write_key_lng(fvis,"FREQID",1 ,"The ref freq of the uv data",&status))
    return status ;
  if (fits_write_key_dbl(fvis,"IATUTC", iatutc,12,"IAT-UTC (time sec) ",&status))
    return status ;
  if (fits_write_key_str(fvis,"POLTYPE", "APPROX","Feed polarization parameterization" ,&status))
    return status ;
  if (fits_write_key_lng(fvis,"P_REFANT" , REFANT, "Reference antenna" ,&status))
    return status ;
  if (fits_write_key_dbl(fvis,"P_DIFF01", 0.0,12," ???",&status))
    return status ;
  if (fits_write_key_dbl(fvis,"P_DIFF02", 0.0,12," ???",&status))
    return status ;

  for(row=1; row <= nrows; row++ )
  { AntParType *ant = ANT + row - 1;
    strncpy(antab.anname, ant->name,4) ;
    sprintf(antab.anname+4,":%02d", row) ;
    antab.x = ant->bx;
    antab.y = ant->by;
    antab.z = ant->bz;
    antab.nosta=row;
    antab.mntsta=0;
    antab.staxof=0.0;

    antab.poltya[0] = 'R' ; antab.poltyb[0] = 'L' ;
    antab.poltya[1] = ' ' ; antab.poltyb[1] = ' ' ;
    antab.poltya[2] = ' ' ; antab.poltyb[2] = ' ' ;
    antab.poltya[3] = ' ' ; antab.poltyb[3] = ' ' ;

    antab.polaa=0.0      ;  antab.polab=0.0      ;
    antab.polcala[0]=0.0 ;  antab.polcalb[0]=0.0 ;
    antab.polcala[1]=0.0 ;  antab.polcalb[1]=0.0 ;
    antab.polcala[2]=0.0 ;  antab.polcalb[2]=0.0 ;
    antab.polcala[3]=0.0 ;  antab.polcalb[3]=0.0 ;
    if(LittleEndian) 
	{  ffswap8(&antab.x, 3);
	   ffswap4((INT32BIT*)&antab.nosta, 3);
	   ffswap4((INT32BIT*)&antab.polaa, 5);
	   ffswap4((INT32BIT*)&antab.polab, 5);
	}
    if (fits_write_tblbytes(fvis, row, 1, ANTabSize, (unsigned char *) &antab, &status)) return status ;
  }
  return 0;
}

int readANtab(fitsfile *fvis, ProjParType *proj)
{ int maxdim = 8, row=0, status=0, tfields =12;
  double iatutc;
  long pcount, nrows;

  AntennaTableType antab;
  int extver = 1;

  if ( fits_movnam_hdu(fvis, ANY_HDU, "AIPS AN", extver, &status )) printerror(status);
  if( fits_read_btblhdr(fvis, maxdim, &nrows, &tfields, NULL, NULL, NULL, NULL, &pcount, &status) ) 
      {
	  printerror(status);
	  return status;
      }
  if (fits_read_key_dbl(fvis,"IATUTC", &iatutc, NULL,&status))  return status ;

  proj->ants = (int)nrows;
  proj->Ant = (AntParType*)calloc(proj->ants, AntParSize );

  for(row=1; row <= nrows; row++ )
  { 
    if (fits_read_tblbytes(fvis, row, 1, ANTabSize, (unsigned char *) &antab, &status)) return status ;
    if(LittleEndian) 
	{  ffswap8(&antab.x, 3);
	   ffswap4((INT32BIT*)&antab.nosta, 3);
	   ffswap4((INT32BIT*)&antab.polaa, 5);
	   ffswap4((INT32BIT*)&antab.polab, 5);
	}

    AntParType *ant = proj->Ant + row - 1;
    strncpy(ant->name, antab.anname, 4) ;
    ant->id = atoi(antab.anname+5);
    ant->bx = antab.x;
    ant->by = antab.y;
    ant->bz = antab.z;
  }
 fprintf(finfo, "%s Antennas            : %d\n", info, proj->ants);
  return 0;
}


int writeFQtab(fitsfile *fvis, ProjParType *proj)
{
  enum {tfields = 5 } ;
  int status = 0, row = 1;
  FrequencyTableType fqtab;

  char *ttype[tfields] =
      { "FRQSEL", "IF FREQ", "CH WIDTH", "TOTAL BANDWIDTH", "SIDEBAND" };
  char *tform[tfields] =
      {   "1J" ,    "1D" ,      "1E" ,          "1E" ,          "1J" } ;
  char *tunit[tfields] = {"  ", "Hz", "Hz", "Hz" , "  " } ;

  status=0;
  if( fits_create_tbl(fvis, BINARY_TBL, row-1, tfields, ttype, tform, tunit, "AIPS FQ", &status) ) return status;
    if (ffpkyj(fvis,"EXTVER", 1,"Version number of table",&status)) return status ;
  if (ffpkyj(fvis,"NO_IF" , proj->sidebands, "The number of IFs" ,&status)) return status ;

  fqtab.id = 1 ;
  fqtab.iffreq = (proj->bandcentre - proj->bandwidth/2) - RefFreq ;
  fqtab.chwidth = proj->bandwidth / proj->chans;
  fqtab.bandwidth = proj->bandwidth;
  fqtab.sideband = proj->sidebands ;

  if(LittleEndian)
  {  ffswap4( (INT32BIT*)&fqtab.id, 1);
     ffswap8( &fqtab.iffreq, 1); 
     ffswap8( &fqtab.chwidth,1);
     ffswap8( &fqtab.bandwidth,1);
     ffswap4( (INT32BIT*)&fqtab.sideband, 1);
  }

  fits_write_tblbytes(fvis, row, 1, FQTabSize, (unsigned char *)&fqtab, &status) ;
  return status ;
}

int readFQtab( fitsfile *fvis, ProjParType *proj )
{
 int status = 0;
 int extver = 1;
 int maxdim = 8;
 long pcount;
 int tfields = 5;
 long nrows, row;

 FrequencyTableType fqtab;

 //fprintf(stderr, "RefFreq : %e", RefFreq); getchar();

 if ( fits_movnam_hdu(fvis, ANY_HDU, "AIPS FQ", extver, &status )) printerror(status);
 if( fits_read_btblhdr(fvis, maxdim, &nrows, &tfields, NULL, NULL, NULL, NULL, &pcount, &status) ) 
 { printerror(status);
   return status;
 }
 nrows = 1;
 for(row=1; row<=nrows; row++)
 { if( fits_read_tblbytes(fvis, row, 1, FQTabSize, (unsigned char *)&fqtab, &status) ) return status;
   if(LittleEndian)
   { ffswap4( (INT32BIT*)&fqtab.id, 1);
     ffswap8(&fqtab.iffreq, 1); 
     ffswap8(&fqtab.chwidth,1);
     ffswap8(&fqtab.bandwidth,1);
     ffswap4( (INT32BIT*)&fqtab.sideband, 1);
   }
 proj->bandwidth = fqtab.bandwidth;
 proj->bandcentre = RefFreq + fqtab.iffreq + fqtab.bandwidth/2;
 }

 return status;
}

int writeSUtab(fitsfile *fvis, ProjParType *proj) 
{
  enum {tfields=19};
  SourceTableType sutab;
  int status ;
  int i,j;
  int sid = 0;
  char *ttype[tfields] = 
       { "ID. NO. ", "SOURCE  ", "QUAL    ", "CALCODE ", "IFLUX   ",
         "QFLUX   ", "UFLUX   ", "VFLUX   ", "FREQOFF ", "BANDWIDTH",
         "RAEPO   ", "DECEPO  ", "EPOCH   ", "RAAPP   ", "DECAPP  ",
         "LSRVEL  ", "RESTFREQ", "PMRA    ", "PMDEC   " 
       } ;

  char *tform[tfields] = 
     { "1J   ", "16A", "1J ", "4A ", "2E ", "2E ", "2E ", "2E ", "2D ", 
       "1D ", "1D ", "1D ", "1D ", "1D ", "1D ", "2D ", "2D ", "1D ", "1D "
     } ;

  char *tunit[tfields] =
     { "   ", "   ", "   ", "   ", "JY ", "JY ", "JY ", "JY ", 
       "HZ ", "HZ ", "DEGREES", "DEGREES", "YEARS  ", "DEGREES", 
       "DEGREES", "M/SEC  ", "HZ     ", "DEG/DAY", "DEG/DAY"
     } ;

  status=0;
  if( fits_create_tbl(fvis, BINARY_TBL, sid, tfields, ttype, tform, tunit, "AIPS SU", &status) ) return status;
  if ( fits_write_key_lng (fvis,"EXTVER", 1,"Version number of table",&status)) return status ;
  if ( fits_write_key_lng (fvis,"NO_IF" ,proj->sidebands, "num_IF" ,&status)) return status ;
  if ( fits_write_key_str (fvis,"VELTYP" ,"TOPOCENT", "Velocity type" ,&status)) return status ;
  if ( fits_write_key_str (fvis,"VELDEF" ,"RADIO", " Velocity definition" ,&status))
    return status ;
  if ( fits_write_key_lng (fvis,"FREQID" ,1, "Frequency ID" ,&status)) return status ;

  /****** Single source file source table *****/
  if( proj->scans == 1)
  {
  strcpy(sutab.src, proj->scan[0].src);
  strcpy(sutab.calcode, " ");
  sutab.id           = ++sid;
  sutab.qual         = 0;
  sutab.iflux[0]     = 0.00;
  sutab.iflux[1]     = 0.00;
  sutab.qflux[0]     = 0.00;
  sutab.qflux[1]     = 0.00;
  sutab.uflux[0]     = 0.00; 
  sutab.uflux[1]     = 0.00;
  sutab.vflux[0]     = 0.00;
  sutab.vflux[1]     = 0.00;
  sutab.freq_off[0]  = 0.00;
  sutab.freq_off[1]  = 0.00;
  sutab.bandwidth    = proj->bandwidth;
  sutab.ra           = ra2sxg ( &(proj->scan[0].ra) );
  sutab.dec          = dec2sxg( &(proj->scan[0].dec) );
  sutab.epoch        = proj->scan[0].epoch;
  sutab.ra_app       = sutab.ra;
  sutab.dec_app      = sutab.dec;
  sutab.lsrvel[0]    = 0.00;
  sutab.lsrvel[1]    = 0.00;
  sutab.rest_freq[0] = 0.00;
  sutab.rest_freq[1] = 0.00;
  sutab.pmra         = 0.00;
  sutab.pmdec        = 0.00;

  if(LittleEndian)
  {  ffswap4( (INT32BIT*)&sutab.id, 1);
     ffswap4( (INT32BIT*)&sutab.qual, 1);
     ffswap8(sutab.iflux, 4*2);
     ffswap8(sutab.freq_off, 14);
  }

 fits_write_tblbytes (fvis, sid, 1, SUTabSize, (unsigned char *)&sutab, &status) ;
  }
  /****** Multi source file source table ******/
  else
  {/***** First source ******/
    i = 1;
    strcpy(sutab.src, proj->scan[i].src);
    strcpy(sutab.calcode, " ");
    sutab.id           = proj->scan[i].sid;
    
    sutab.qual         = 0;
    sutab.iflux[0]     = 0.00;
    sutab.iflux[1]     = 0.00;
    sutab.qflux[0]     = 0.00;
    sutab.qflux[1]     = 0.00;
    sutab.uflux[0]     = 0.00; 
    sutab.uflux[1]     = 0.00;
    sutab.vflux[0]     = 0.00;
    sutab.vflux[1]     = 0.00;
    sutab.freq_off[0]  = 0.00;
    sutab.freq_off[1]  = 0.00;
    sutab.bandwidth    = proj->bandwidth;
    sutab.ra           = ra2sxg ( &(proj->scan[i].ra) );
    sutab.dec          = dec2sxg( &(proj->scan[i].dec) );
    sutab.epoch        = proj->scan[i].epoch + (proj->mjd_start - 51544.5)/365.25 ;
    sutab.ra_app       = sutab.ra;
    sutab.dec_app      = sutab.dec;
    sutab.lsrvel[0]    = 0.00;
    sutab.lsrvel[1]    = 0.00;
    sutab.rest_freq[0] = 0.00;
    sutab.rest_freq[1] = 0.00;
    sutab.pmra         = 0.00;
    sutab.pmdec        = 0.00;

    if(LittleEndian)
    {  ffswap4( (INT32BIT*)&sutab.id, 1);
       ffswap4( (INT32BIT*)&sutab.qual, 1);
       ffswap8( sutab.iflux, 4*2);
       ffswap8( sutab.freq_off, 14 );
    }
    fits_write_tblbytes (fvis, proj->scan[i].sid, 1, SUTabSize, (unsigned char *)&sutab, &status) ;

    /*** Subsequent sources in a multi source file ****/
  for(i = 2; i <= proj->scans; i++)
  { for( j = 1 ; j < i ; j++ )
    { if( proj->scan[i].sid == proj->scan[j].sid ) continue;
      strcpy(sutab.src, proj->scan[i].src);
      strcpy(sutab.calcode, " ");
      sutab.id           = proj->scan[i].sid;

      sutab.qual         = 0;
      sutab.iflux[0]     = 0.00;
      sutab.iflux[1]     = 0.00;
      sutab.qflux[0]     = 0.00;
      sutab.qflux[1]     = 0.00;
      sutab.uflux[0]     = 0.00; 
      sutab.uflux[1]     = 0.00;
      sutab.vflux[0]     = 0.00;
      sutab.vflux[1]     = 0.00;
      sutab.freq_off[0]  = 0.00;
      sutab.freq_off[1]  = 0.00;
      sutab.bandwidth    = proj->bandwidth;
      sutab.ra           = ra2sxg ( &(proj->scan[i].ra) );
      sutab.dec          = dec2sxg( &(proj->scan[i].dec) );
      sutab.epoch        = proj->scan[i].epoch + (proj->mjd_start - 51544.5)/365.25 ;
      sutab.ra_app       = sutab.ra;
      sutab.dec_app      = sutab.dec;
      sutab.lsrvel[0]    = 0.00;
      sutab.lsrvel[1]    = 0.00;
      sutab.rest_freq[0] = 0.00;
      sutab.rest_freq[1] = 0.00;
      sutab.pmra         = 0.00;
      sutab.pmdec        = 0.00;

    if(LittleEndian)
    {  ffswap4( (INT32BIT*)&sutab.id, 1);
       ffswap4( (INT32BIT*)&sutab.qual, 1);
       ffswap8( sutab.iflux, 4*2);
       ffswap8( sutab.freq_off, 14 );
    }
    fits_write_tblbytes (fvis, proj->scan[i].sid, 1, SUTabSize, (unsigned char *)&sutab, &status) ;
    }
  }
  }
 return status ;
}
/*
int readSUtab( fitsfile *fvis, ProjParType *proj )
{
 int status = 0;
 int extver = 1;
 int maxdim = 8;
 int tfields = 19;
 long nrows, row;
 long pcount;
 SourceTableType sutab;

 if ( fits_movnam_hdu(fvis, ANY_HDU, "AIPS SU", extver, &status )) printerror(status);
 if( fits_read_btblhdr(fvis, maxdim, &nrows, &tfields, NULL, NULL, NULL, NULL, &pcount, &status) ) 
 { printerror(status);
   return status;
 }

 for(row=1; row<=nrows; row++)
{ if( fits_read_tblbytes(fvis, row, 1, SUTabSize, (unsigned char *)&sutab, &status) ) {printerror(status); return status;}
  if(LittleEndian)
  {  ffswap8( (double*)&sutab.id, 1);
     ffswap8( (double*)&sutab.qual, 1);
     ffswap8( (double*)&sutab.iflux, 4*2);
     ffswap8(sutab.freq_off, 14);
  }
  proj->scan[row-1].scan_num = row-1;
  strcpy(proj->scan[row-1].src, sutab.src);
  proj->scan[row-1].ra = sxg2ra(sutab.ra);
  proj->scan[row-1].dec = sxg2dec(sutab.dec);
  proj->scan[row-1].epoch = sutab.epoch;
 }
 return status;
}
*/

int readSUtab( fitsfile *fvis, ProjParType *proj )
{
 int status = 0;
 int extver = 1;
 int maxdim = 8;
 int tfields = 19;
 long nrows, row;
 long pcount;
 SourceTableType sutab;

 if ( fits_movnam_hdu(fvis, ANY_HDU, "AIPS SU", extver, &status )) printerror(status);
 if( fits_read_btblhdr(fvis, maxdim, &nrows, &tfields, NULL, NULL, NULL, NULL, &pcount, &status) ) 
 { printerror(status);
   return status;
 }

 proj->scans = nrows;
 nrows=1;
 for(row=1; row<=nrows; row++)
 { if( fits_read_tblbytes(fvis, row, 1, SUTabSize, (unsigned char *)&sutab, &status) ) {printerror(status); return status;}
     if(LittleEndian)
    {  ffswap4( (INT32BIT*)&sutab.id, 1);
       ffswap4( (INT32BIT*)&sutab.qual, 1);
       ffswap8( sutab.iflux, 4*2);
       ffswap8( sutab.freq_off, 14 );
    }
  proj->scan[row-1].scan_num = row-1;
  proj->scan[row-1].sid = sutab.id;
  strcpy(proj->scan[row-1].src, sutab.src);
  proj->scan[row-1].ra = sxg2ra(sutab.ra);
  proj->scan[row-1].dec = sxg2dec(sutab.dec);
  proj->scan[row-1].epoch = sutab.epoch;
 }
 return status;
}

void fits_rewind( fitsfile *fvis, ProjParType *proj )
{
 int status = 0;
 if ( fits_movabs_hdu(fvis, 1, NULL, &status) ) printerror(status);
 proj->fits_rewind = 1;
}

int fill_proj(fitsfile *fvis, ProjParType *proj)
{
 int maxdim = 8, nrecs, naxis;
 long naxes[7];
 char ref_date[32];
 long gcount, pcount;
 int status = 0;
 double epoch;
 int particlesize;
 int ch;

 if( fits_read_imghdr(fvis, maxdim, NULL, NULL, &naxis, naxes, &pcount, &gcount, NULL, &status) ) return status;
 if( naxes[0] != 0 ) 
 {
   fprintf(finfo, "%s Image dimensions            : %ld x %ld\n", info, naxes[0], naxes[1]);
   fprintf(finfo, "%s Channels                    : %ld\n", info, naxes[2]);
   return -1;
 }

 fits_rewind( fvis, proj );
 read_EXtabs( fvis, proj );
 proj->baselines = ( proj->ants * (proj->ants - 1) ) / 2 ;
 proj->Base = (BaseParType*)calloc(proj->baselines, BaseParSize);
 
 fits_rewind( fvis, proj );

 proj->tick = 1.0; //seconds
 proj->pcount = pcount;
 particlesize = naxes[1];
 proj->pols = naxes[2];
 proj->chans = naxes[3];
 proj->sidebands = naxes[4];
 proj->recwords = proj->pcount + particlesize * ( proj->pols * proj->sidebands * proj->chans );
 proj->RecBuf = (double*)calloc( proj->baselines * proj->recwords, sizeof(double) ) ;
 
 if ( fits_read_key_str(fvis, "OBSERVER", proj->observer, NULL, &status) )  return status;
 if ( fits_read_key_str(fvis, "PROJECT", proj->code, NULL, &status) ) return status ;
 if ( fits_read_key_str(fvis, "OBSMODE", proj->obsmode, NULL, &status) ) return status ;
 if ( fits_read_key_str(fvis, "DATE-OBS",ref_date, NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fvis, "EPOCH", &epoch, NULL, &status) ) return status ;
 if ( fits_read_key_str(fvis, "INSTRUME", proj->receiver, NULL, &status) ) return status ;

 getpar_baseline( fvis, proj );
 index_ants( proj );
 fits_rewind( fvis, proj );
 
 flash_obs_settings( proj );
 dummy_gsl_alloc( proj );
 proj->H_w = (CmplxType*)calloc(1, sizeof(CmplxType));
 proj->SNR = (double*)calloc(proj->baselines, sizeof(double));
 proj->sigma = (double*)calloc(proj->baselines, sizeof(double));
 proj->ULambda = (double**)calloc(proj->chans, sizeof(double*));
 proj->model_vis = (CmplxType**)calloc(proj->chans, sizeof(CmplxType*));
 proj->MV_errbar = (CmplxType**)calloc(proj->chans, sizeof(CmplxType*));
 for(ch = 0 ; ch < proj->chans; ch++)  
 { proj->model_vis[ch] = (CmplxType*)calloc(proj->redundant_baselines+1, sizeof(CmplxType));
   proj->MV_errbar[ch] = (CmplxType*)calloc(proj->redundant_baselines+1, sizeof(CmplxType));
   proj->ULambda[ch] = (double*)calloc(proj->redundant_baselines+1, sizeof(double));
 }
 nrecs = gcount/proj->baselines;
 proj->nrecs = nrecs;

 if ( proj->preint > nrecs ) proj->preint = nrecs;
 init_SNR_compute( proj );

 if(proj->nsrc == -1)
 { parse_VLACalManual(proj); 
   gen_V_sky( proj );
 }

 return proj->nrecs;
}
/*
void writeBPtab( fitsfile *fvis, ProjParType *proj )
{
  enum {tfields=12};
  BPassTableType bptab;
  int status ;
  int i,j;
  int row = 0;
  char *ttype[tfields] = 
       { "TIME    ", "INTERVAL", "SOURCE ID", "SUBARRAY", 
	 "ANTENNA",  "BANDWIDTH","CHN_SHIFT", "FREQ ID", 
	 "REFANT 1", "WEIGHT 1","REAL 1", "IMAG 1"
       } ;

  char *tform[tfields] = 
     { "1D ", "1E", "1J ", "1J ", 
       "1J ", "1J ", "1E ", "D ", 
       "1J ", "64E ", "64E ", "64E "
     } ;

  char *tunit[tfields] =
     { "DAYS", "DAYS", "   ", "   ",
       "  ",   "HZ ", "  ", "  ", 
       "   ", "  ", " ", " "
     } ;

  status=0;
  if( fits_create_tbl(fvis, BINARY_TBL, row, tfields, ttype, tform, tunit, "AIPS BP", &status) ) return status;
  if ( fits_write_key_lng (fvis,"EXTVER", 1,"Version number of table",&status)) return status ;
  if ( fits_write_key_lng (fvis,"NO_IF" ,proj->sidebands, "num_IF" ,&status)) return status ;
  if ( fits_write_key_str (fvis,"VELTYP" ,"TOPOCENT", "Velocity type" ,&status)) return status ;
  if ( fits_write_key_str (fvis,"VELDEF" ,"RADIO", " Velocity definition" ,&status))
    return status ;
  if ( fits_write_key_lng (fvis,"FREQID" ,1, "Frequency ID" ,&status)) return status ;
  

}
*/
void getpar_baseline( fitsfile *fvis, ProjParType *proj)
{
 RGParType *uvw;
 BaseParType *baseline;
 AntParType *Ant = proj->Ant;
 long len = proj->recwords * proj->baselines;
 int bl, ant, status = 0;
 double c = LIGHTVEL;
 int baseline_num_start;

 baseline = proj->Base;

 if(! strcmp(proj->receiver, PHASE_I_RX) )
 { proj->ORTelements = PHASE_I;
   baseline_num_start = FITS_BASELINE_START;
 }
 if(! strcmp(proj->receiver, PHASE_II_RX) ) 
 { proj->ORTelements = PHASE_II;
   baseline_num_start = FITS_BASELINE_START*2;
 }

 proj->unique_baseline = (int*)calloc(proj->ORTelements, sizeof(int) );
 
 fits_read_tblbytes(fvis, 
		    1, 
		    1, 
		    len*sizeof(double), 
		    (unsigned char*)proj->RecBuf, 
		    &status); 
 if(LittleEndian) ffswap8( (double*)proj->RecBuf, len );
      
 for(bl = 0; bl < proj->baselines; bl++)
 { uvw = (RGParType*)(proj->RecBuf + bl*proj->recwords);
   if(bl == 0 ) proj->mjd_start = uvw->date1 - 2400000.5;
   baseline[bl].u =  uvw->u * c;
   baseline[bl].v =  uvw->v * c;
   baseline[bl].w =  uvw->w * c;
   baseline[bl].number = uvw->baseline;
   baseline_number_to_ant_ids( baseline+bl, baseline_num_start );

   if( proj->unique_baseline[ baseline[bl].id2 - baseline[bl].id1 ]  == 0 )   proj->unique_baseline[0] += 1 ;
   proj->unique_baseline[ baseline[bl].id2 - baseline[bl].id1 ] += 1; 
 }
 proj->redundant_baselines = count_redundant_baselines(proj);
 set_baseline_index( proj );
 index_ants( proj );

 fprintf(finfo, "%s Baselines           : %ld\n", info, proj->baselines);
 fprintf(finfo, "%s Unique baselines    : %d\n", info, proj->unique_baseline[0]);
 fprintf(finfo, "%s Redundant baselines : %d\n", info, proj->redundant_baselines);
}

int write_pntsrc2FITS( ProjParType *proj )
{
 PrimaryBeamParType *pb = &(proj->pbeam);
 int XPIX = pb->xside, YPIX = XPIX; int ZPIX = proj->chans;
 fitsfile *fmap;
 FILE *fsrc;
 int i, x, y;
 double ra_res, dec_res, dec_fan;
 //int XPIX = 392, YPIX = 1000;
 //int XPIX = 117, YPIX = 300;
 //int XPIX = 60, YPIX = 60;
 double ***map;
 double ptg_ra, ptg_dec;
 double src_ra, src_dec;
 char FITSFile[32], srcfile[32];
 int bitpix, naxis=3, simple, extend;
 long naxes[naxis];
 int status=0;
 int j, ch;
 char *ctype[] = { "Right Ascension - deg", "Declination - deg", "Frequency - MHz" };
 double crval[] = { ra2sxg(&(proj->scan[0].ra)), dec2sxg(&(proj->scan[0].dec)), proj->bandcentre};
 double cdelt[] = { 0.0, 0.0, 0.0};
 double crpix[] = { XPIX/2, YPIX/2, ZPIX/2 };
 double crota[] = { 0.0, 0.0, 0.0 };
 char keynew[16];
 char ref_date[32];
 double epoch = 2000.0;
 double datamax = 0.0, datamin = 0.0;
 double *data;
 long int cnt;
 int xpix, ypix, zpix;
 double deltanu = proj->bandwidth / proj->chans;
 double nu[proj->chans];

 strcpy(FITSFile, "PNTSRC.FITS\0");

 remove(FITSFile);
 if(fits_create_file(&fmap,  FITSFile, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }

 fprintf(finfo, "%s %s opened on disk", info, FITSFile);

 bitpix = DOUBLE_IMG;
 simple = TRUE;
 extend = TRUE;
 naxes[0] = XPIX, naxes[1] = YPIX; naxes[2] = ZPIX;

 if( fits_create_img( fmap, bitpix, naxis, naxes, &status ) )
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 
 if ( fits_write_key_str(fmap,"OBJECT",proj->scan[0].src, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"TELESCOP","ORT", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"INSTRUME", proj->receiver, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSERVER", proj->observer, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"EXTNAME", "Primary", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"PROJECT", proj->code, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSMODE", proj->obsmode, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-OBS",ref_date, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-MAP",ref_date, "",&status) ) return status ; 
 if ( fits_write_key_dbl(fmap,"BSCALE",1.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"BZERO",0.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_str(fmap,"BUNIT","Jy/beam", "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"EQUINOX",epoch, 9, "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSRA", ra2sxg(&(proj->scan[0].ra)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSDEC",dec2sxg(&(proj->scan[0].dec)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSFREQ",proj->bandcentre, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMAX",datamax, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMIN",datamin, 9 , "",&status) ) return status ;

 cdelt[0] = pb->ra_res;
 cdelt[1] = pb->dec_res;
 cdelt[2] = deltanu;

 for (j=1; j <= naxis; j++)
    {
      if ( fits_make_keyn ("CTYPE",j,keynew, &status) ) return status ;
      if ( fits_write_key_str(fmap,keynew,ctype[j-1], "",&status)) return status ;
      if ( fits_make_keyn ("CRVAL",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl(fmap,keynew,crval[j-1],10,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CDELT",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,cdelt[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CRPIX",j,keynew,&status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,crpix[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CROTA",j,keynew, &status) ) return status ; 
      if ( fits_write_key_dbl (fmap,keynew,crota[j-1],10,"" ,&status) ) return status ;
    }
  
 map = (double***)calloc(ZPIX, sizeof(double**) );
 for( ch = 0 ; ch < ZPIX ; ch++)
 { map[ch] = (double**)calloc(YPIX, sizeof(double*));
   for(i=0; i < YPIX ; i++) map[ch][i] = (double*)calloc(XPIX, sizeof(double));
   nu[ch] = proj->bandcentre - proj->bandwidth/2 + ch*deltanu + deltanu/2;
 }

 ptg_ra = ra2sxg(&(proj->scan[0].ra));
 ptg_dec = dec2sxg(&(proj->scan[0].dec));

  
 for(i = 0; i < proj->nsrc; i++)
 {  src_ra = ra2sxg( &(proj->skysrc[i].ra) );
    src_dec = dec2sxg( &(proj->skysrc[i].dec) );
    x = ( (src_ra  - ptg_ra)  / pb->ra_res) + MAPSIDE / 2 ;
    y = ( (src_dec - ptg_dec) / pb->dec_res) + MAPSIDE / 2 ;
    for(ch = 0 ; ch < ZPIX ; ch++ )
    { map[ch][y][x] = proj->skysrc[i].S *  pow(proj->bandcentre/nu[ch],proj->skysrc[i].alpha) ;
      if( map[ch][y][x] > datamax) datamax = map[ch][y][x];
    }
 }


 data = (double*)calloc(XPIX * YPIX * ZPIX, sizeof(double) );
 cnt = 0;
 for( zpix = 0 ; zpix < ZPIX ; zpix++ )
 { for(ypix = 0 ; ypix < YPIX ; ypix++ )
   { for( xpix = 0 ; xpix < XPIX ; xpix++ )
     { data[cnt] = map[zpix][ypix][xpix];
       cnt++;
     }
   }
 }
  
 datamax = arraymax(data, cnt-1);

 if( fits_update_key_dbl(fmap, "DATAMAX", datamax, 9, "", &status)) printerror(status);

 fits_write_3d_dbl ( fmap, 0, XPIX, YPIX, XPIX, YPIX, ZPIX, data, &status );
 fprintf(finfo, "...written!\n");
 if( fits_close_file(fmap, &status) ) printerror(status) ;

 free(data);
 free(map);

 //strcpy(srcfile, proj->scan[0].src);
 strcpy(srcfile, "PNTSRC.cat");
 fprintf(finfo, "%s Writing %s: sources within primary beam ...", info, srcfile);
 fsrc = fopen(srcfile, "w");
 for(i = 0 ; i < proj->nsrc ; i++ )
 fprintf(fsrc, "%d %f %f %f\n", i+1, ra2sxg(&(proj->skysrc[i].ra)), dec2sxg(&(proj->skysrc[i].dec)), proj->skysrc[i].S*1000.0);
 fclose(fsrc);
 fprintf(stderr, "written!\n");

 return status;
}

int read_FITSmap( ProjParType *proj, char *FITSfile )
{
 SkyMapType *sky = &(proj->skymap);
 fitsfile *fmap;
 int XPIX, YPIX, ZPIX = 1;
 DisplayBufParType *ds = &(proj->dispBuf);
 int nullval = 0, anynul = 0;
 int status=0;
 double ra_res, dec_res, dec_fan;
 double ptg_ra, ptg_dec;
 int bitpix, naxis=3, simple, extend;
 long naxes[naxis];
 char keynew[16];
 char ref_date[32];
 double datamax = 0.0, datamin = 0.0;
 int xx, yy, i;
 int ch, cnt;
 double centre_freq, deltanu;
 double nullpix = 0.0;
 long group = 0;
 char label[naxis][80];

 if(fits_open_image( &fmap, FITSfile, READONLY, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 else
 fprintf(finfo, "%s %s found on disk", info, FITSfile);
 
 if ( fits_movabs_hdu(fmap, 1, NULL, &status) ) printerror(status);;
 fits_get_img_param( fmap, 3, &bitpix, &naxis, naxes, &status );
 XPIX = naxes[0], YPIX = naxes[1]; if(naxis == 3) ZPIX = naxes[2];

 //if ( fits_read_key_dbl(fmap,"DATAMAX",&datamax, NULL, &status) ) return status ;
 //if ( fits_read_key_dbl(fmap,"DATAMIN",&datamin, NULL, &status) ) return status ;
 if ( fits_read_key_str(fmap,"CTYPE1", label[0], NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"CRVAL1",&ptg_ra , NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"CDELT1",&ra_res, NULL, &status) ) return status ;
 if ( fits_read_key_str(fmap,"CTYPE2", label[1], NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"CRVAL2",&ptg_dec, NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"CDELT2",&dec_res, NULL, &status) ) return status ;
 if(naxis == 3) 
 { 
  if ( fits_read_key_str(fmap,"CTYPE3", label[2], NULL, &status) ) return status ;
  if ( fits_read_key_dbl(fmap,"CRVAL3",&centre_freq , NULL, &status) ) return status ;
  if ( fits_read_key_dbl(fmap,"CDELT3",&deltanu, NULL, &status) ) return status ;
 }
 ds->min[0] = ptg_ra - ra_res*XPIX/2;
 ds->max[0] = ptg_ra + ra_res*XPIX/2;
 ds->min[1] = ptg_dec - dec_res*YPIX/2;
 ds->max[1] = ptg_dec + dec_res*YPIX/2;

 for(i = 0 ; i < naxis ; i++ ) strcpy(ds->axlbl[i], label[i]);


 ds->databuf = (double*)calloc(XPIX*YPIX*ZPIX, sizeof(double));
 ds->xaxis = (double*)calloc(1, sizeof(double));

 ds->ncol = XPIX;
 ds->nrow = YPIX;

 naxes[0] = 1;
 naxes[1] = 1;
 if(naxis == 3) naxes[2] = 1;
 
 fits_read_3d_dbl(fmap, group, nullpix, XPIX, YPIX, XPIX, YPIX, ZPIX, ds->databuf, &anynul, &status);

 datamax = arraymax(ds->databuf, XPIX*YPIX*ZPIX);
 datamin = arraymin(ds->databuf, XPIX*YPIX*ZPIX);
 //fits_read_pix(fmap, TDOUBLE, naxes, XPIX*YPIX, &nullval, ds->databuf, &anynul, &status);

 if( fits_close_file(fmap, &status) ) printerror(status) ;

 proj->chans = ZPIX;
 sky->xside = XPIX;
 sky->yside = YPIX;
 sky->zside = ZPIX;
 sky->ra_res = ra_res;
 sky->dec_res = dec_res;

 sky->ra = (RAType*)calloc(sky->xside, sizeof(RAType) );
 sky->dec = (DecType*)calloc(sky->yside, sizeof(DecType) );

 for(i = 0 ; i < sky->xside ; i++) sky->ra[i] = sxg2ra(ptg_ra + (i - sky->xside/2)*ra_res);
 for(i = 0 ; i < sky->yside ; i++) sky->dec[i] = sxg2dec(ptg_dec + (i - sky->yside/2)*dec_res);

 cnt = 0;
 sky->map = (double***)calloc(ZPIX, sizeof(double**));
 for(ch = 0; ch<ZPIX;ch++) 
 { sky->map[ch] = (double**)calloc(sky->yside, sizeof(double*) );
   for( yy = 0 ; yy < sky->yside ; yy++ )
   { sky->map[ch][yy] = (double*)calloc(sky->xside, sizeof(double) );
     for( xx = 0 ; xx < sky->xside ; xx++ )
     { //sky->map[ch][yy][xx] = ds->databuf[yy*sky->xside + xx];
        sky->map[ch][yy][xx] = (double)ds->databuf[cnt];
	cnt++;
     }
   }
 }

 //V2_deltanu_from_cube( proj );

 return status; 
}

void FITSchan_avg( ProjParType *proj, int kk)
{
 SkyMapType *sky = &(proj->skymap);
 int ch, xx, yy;
 int k;

 for(ch = 0; ch<sky->zside;ch += kk) 
   for( yy = 0 ; yy < sky->yside ; yy++ )
     for( xx = 0 ; xx < sky->xside ; xx++ )
	 for(k=1; k < kk; k++)
	     sky->map[ch][yy][xx] += sky->map[ch+k][yy][xx];

}

int read_EPS_map( ProjParType *proj )
{
 fitsfile *fmap;
 int XPIX, YPIX, ZPIX = 1;
 DisplayBufParType *ds = &(proj->dispBuf);
 int nullval = 0, anynul = 0;
 int status=0;
 double ra_res, dec_res, dec_fan;
 double ptg_ra, ptg_dec;
 int bitpix, naxis=3, simple, extend;
 long naxes[naxis];
 char keynew[16];
 char ref_date[32];
 double datamax = 0.0, datamin = 0.0;
 int xx, yy, i;
 int ch, cnt;
 double centre_freq, deltanu;
 double nullpix = 0.0;
 long group = 0;
 char label[naxis][80];
 EPSFGType *eps = &(proj->eps);
 char FITSfile[256];

 strcpy(FITSfile, eps->filename);

 if(fits_open_image( &fmap, FITSfile, READONLY, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 else
 fprintf(finfo, "%s %s found on disk", info, FITSfile);
 newline();

 if ( fits_movabs_hdu(fmap, 1, NULL, &status) ) printerror(status);;
 fits_get_img_param( fmap, 3, &bitpix, &naxis, naxes, &status );
 XPIX = naxes[0], YPIX = naxes[1]; if(naxis == 3) ZPIX = naxes[2];

 //if ( fits_read_key_dbl(fmap,"DATAMAX",&datamax, NULL, &status) ) return status ;
 //if ( fits_read_key_dbl(fmap,"DATAMIN",&datamin, NULL, &status) ) return status ;
 if ( fits_read_key_str(fmap,"CTYPE1", label[0], NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"CRVAL1",&ptg_ra , NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"CDELT1",&ra_res, NULL, &status) ) return status ;
 if ( fits_read_key_str(fmap,"CTYPE2", label[1], NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"CRVAL2",&ptg_dec, NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"CDELT2",&dec_res, NULL, &status) ) return status ;
 if(naxis == 3) 
 { 
  if ( fits_read_key_str(fmap,"CTYPE3", label[2], NULL, &status) ) return status ;
  if ( fits_read_key_dbl(fmap,"CRVAL3",&centre_freq , NULL, &status) ) return status ;
  if ( fits_read_key_dbl(fmap,"CDELT3",&deltanu, NULL, &status) ) return status ;
 }
 ds->min[0] = ptg_ra - ra_res*XPIX/2;
 ds->max[0] = ptg_ra + ra_res*XPIX/2;
 ds->min[1] = ptg_dec - dec_res*YPIX/2;
 ds->max[1] = ptg_dec + dec_res*YPIX/2;

 for(i = 0 ; i < naxis ; i++ ) strcpy(ds->axlbl[i], label[i]);

 ds->databuf = (double*)calloc(XPIX*YPIX*ZPIX, sizeof(double));
 ds->xaxis = (double*)calloc(1, sizeof(double));

 ds->ncol = XPIX;
 ds->nrow = YPIX;

 naxes[0] = 1;
 naxes[1] = 1;
 if(naxis == 3) naxes[2] = 1;
 
 fits_read_3d_dbl(fmap, group, nullpix, XPIX, YPIX, XPIX, YPIX, ZPIX, ds->databuf, &anynul, &status);

 datamax = arraymax(ds->databuf, XPIX*YPIX*ZPIX);
 datamin = arraymin(ds->databuf, XPIX*YPIX*ZPIX);
 //fits_read_pix(fmap, TFLOAT, naxes, XPIX*YPIX, &nullval, ds->databuf, &anynul, &status);

 if( fits_close_file(fmap, &status) ) printerror(status) ;

 proj->chans = ZPIX;
 eps->xside = XPIX;
 eps->yside = YPIX;
 eps->zside = ZPIX;
 eps->ra_res = ra_res;
 eps->dec_res = dec_res;

 eps->ra = (RAType*)calloc(eps->xside, sizeof(RAType) );
 eps->dec = (DecType*)calloc(eps->yside, sizeof(DecType) );

 for(i = 0 ; i < eps->xside ; i++) eps->ra[i] = sxg2ra(ptg_ra + (i - eps->xside/2)*ra_res);
 for(i = 0 ; i < eps->yside ; i++) eps->dec[i] = sxg2dec(ptg_dec + (i - eps->yside/2)*dec_res);

 cnt = 0;
 eps->map = (double***)calloc(ZPIX, sizeof(double**));
 for(ch = 0; ch<ZPIX;ch++) 
 { eps->map[ch] = (double**)calloc(eps->yside, sizeof(double*) );
   for( yy = 0 ; yy < eps->yside ; yy++ )
   { eps->map[ch][yy] = (double*)calloc(eps->xside, sizeof(double) );
     for( xx = 0 ; xx < eps->xside ; xx++ )
     {  eps->map[ch][yy][xx] = (double)ds->databuf[cnt];
	cnt++;
     }
   }
 }

 return 0;
}
int read_HI_map( ProjParType *proj )
{
 fitsfile *fmap;
 int XPIX, YPIX, ZPIX = 1;
 DisplayBufParType *ds = &(proj->dispBuf);
 int nullval = 0, anynul = 0;
 int status=0;
 double ra_res, dec_res, dec_fan;
 double ptg_ra, ptg_dec;
 int bitpix, naxis=3, simple, extend;
 long naxes[naxis];
 char keynew[16];
 char ref_date[32];
 double datamax = 0.0, datamin = 0.0;
 int xx, yy, i;
 int ch, cnt;
 double centre_freq, deltanu;
 double nullpix = 0.0;
 long group = 0;
 char label[naxis][80];
 HIMapType *HI = &(proj->HI);
 char FITSfile[256];

 strcpy(FITSfile, HI->filename);

 if(fits_open_image( &fmap, FITSfile, READONLY, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 else
 fprintf(finfo, "%s %s found on disk", info, FITSfile);
 newline();

 if ( fits_movabs_hdu(fmap, 1, NULL, &status) ) printerror(status);;
 fits_get_img_param( fmap, 3, &bitpix, &naxis, naxes, &status );
 XPIX = naxes[0], YPIX = naxes[1]; if(naxis == 3) ZPIX = naxes[2];

 //if ( fits_read_key_dbl(fmap,"DATAMAX",&datamax, NULL, &status) ) return status ;
 //if ( fits_read_key_dbl(fmap,"DATAMIN",&datamin, NULL, &status) ) return status ;
 if ( fits_read_key_str(fmap,"CTYPE1", label[0], NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"CRVAL1",&ptg_ra , NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"CDELT1",&ra_res, NULL, &status) ) return status ;
 if ( fits_read_key_str(fmap,"CTYPE2", label[1], NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"CRVAL2",&ptg_dec, NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"CDELT2",&dec_res, NULL, &status) ) return status ;
 if(naxis == 3) 
 { 
  if ( fits_read_key_str(fmap,"CTYPE3", label[2], NULL, &status) ) return status ;
  if ( fits_read_key_dbl(fmap,"CRVAL3",&centre_freq , NULL, &status) ) return status ;
  if ( fits_read_key_dbl(fmap,"CDELT3",&deltanu, NULL, &status) ) return status ;
 }
 ds->min[0] = ptg_ra - ra_res*XPIX/2;
 ds->max[0] = ptg_ra + ra_res*XPIX/2;
 ds->min[1] = ptg_dec - dec_res*YPIX/2;
 ds->max[1] = ptg_dec + dec_res*YPIX/2;

 for(i = 0 ; i < naxis ; i++ ) strcpy(ds->axlbl[i], label[i]);

 ds->databuf = (double*)calloc(XPIX*YPIX*ZPIX, sizeof(double));
 ds->xaxis = (double*)calloc(1, sizeof(double));

 ds->ncol = XPIX;
 ds->nrow = YPIX;

 naxes[0] = 1;
 naxes[1] = 1;
 if(naxis == 3) naxes[2] = 1;
 
 fits_read_3d_dbl(fmap, group, nullpix, XPIX, YPIX, XPIX, YPIX, ZPIX, ds->databuf, &anynul, &status);

 datamax = arraymax(ds->databuf, XPIX*YPIX*ZPIX);
 datamin = arraymin(ds->databuf, XPIX*YPIX*ZPIX);
 //fits_read_pix(fmap, TFLOAT, naxes, XPIX*YPIX, &nullval, ds->databuf, &anynul, &status);

 if( fits_close_file(fmap, &status) ) printerror(status) ;

 proj->chans = ZPIX;
 HI->xside = XPIX;
 HI->yside = YPIX;
 HI->zside = ZPIX;
 HI->ra_res = ra_res;
 HI->dec_res = dec_res;

 HI->ra = (RAType*)calloc(HI->xside, sizeof(RAType) );
 HI->dec = (DecType*)calloc(HI->yside, sizeof(DecType) );

 for(i = 0 ; i < HI->xside ; i++) HI->ra[i] = sxg2ra(ptg_ra + (i - HI->xside/2)*ra_res);
 for(i = 0 ; i < HI->yside ; i++) HI->dec[i] = sxg2dec(ptg_dec + (i - HI->yside/2)*dec_res);

 cnt = 0;
 HI->map = (double***)calloc(ZPIX, sizeof(double**));
 for(ch = 0; ch<ZPIX;ch++) 
 { HI->map[ch] = (double**)calloc(HI->yside, sizeof(double*) );
   for( yy = 0 ; yy < HI->yside ; yy++ )
   { HI->map[ch][yy] = (double*)calloc(HI->xside, sizeof(double) );
     for( xx = 0 ; xx < HI->xside ; xx++ )
     {  HI->map[ch][yy][xx] = (double)ds->databuf[cnt];
	cnt++;
     }
   }
 }

 return 0;
}

int FITSmap_to_model( ProjParType *proj, char *FITSfile )
{
 fitsfile *fmap;
 FILE *fmodel;
 int ii, i, j, x, y;
 double ra_res, dec_res, dec_fan;
 double m;
 double ptg_ra, ptg_dec;
 int bitpix, naxis=3, simple, extend;
 long naxes[naxis];
 int nullval = 0, anynul = 0;
 int status=0;
 int XPIX, YPIX, ZPIX;
 FILE *fsrc = fopen("test.src", "w");
 BaseParType *baseline   = proj->Base;
 DisplayBufParType *ds = &(proj->dispBuf);
 double c = LIGHTVEL;
 double v, nu;
 int idx;
 int ch;
 long *blindex = proj->blindex;
 SkyMapType *sky = &proj->skymap;
 long int cnt = 0;
 double deltanu = proj->bandwidth / proj->chans;
 double nullpix = 0.0;
 long group = 0;

 /*
 char *ctype[] = { "Right Ascension - deg", "Declination - deg" };
 double crval[] = { ra2sxg(&(proj->scan[0].ra)), dec2sxg(&(proj->scan[0].dec))};
 double cdelt[] = { 0.0,0.0};
 double crpix[] = { XPIX/2, YPIX/2 };
 double crota[] = { 0.0, 0.0 };
 */
 char keynew[16];
 char ref_date[32];
 double datamax = 0.0, datamin = 0.0;
 double bzero, bscale;
 char bunit[8];

 if(fits_open_image( &fmap, FITSfile, READONLY, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 else
 fprintf(finfo, "%s %s found on disk\n", info, FITSfile);
 if ( fits_movabs_hdu(fmap, 1, NULL, &status) ) printerror(status);;
 fits_get_img_param( fmap, 3, &bitpix, &naxis, naxes, &status );
 if( naxes[0] == 0 ) return 1;
 if(naxis == 3) naxes[2] = 1;

 XPIX = naxes[0]; YPIX = naxes[1];
 ZPIX = naxes[2];
 if ( fits_read_key_str(fmap,"OBJECT",proj->scan[0].src, NULL, &status) ) return status ;
 /*if ( fits_read_key_str(fmap,"INSTRUME", proj->receiver, NULL, &status) ) return status ;
 if ( fits_read_key_str(fmap,"OBSERVER", proj->observer, NULL, &status) ) return status ;
 if ( fits_read_key_str(fmap,"PROJECT", proj->code, NULL, &status) ) return status ;
 if ( fits_read_key_str(fmap,"OBSMODE", proj->obsmode, NULL, &status) ) return status ;*/
 if ( fits_read_key_str(fmap,"DATE-OBS",ref_date, NULL, &status) ) return status ;
 if ( fits_read_key_str(fmap,"DATE-MAP",ref_date, NULL, &status) ) return status ; 
 if ( fits_read_key_dbl(fmap,"BSCALE", &bscale, NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"BZERO",&bzero, NULL, &status) ) return status ;
 if ( fits_read_key_str(fmap,"BUNIT",bunit, NULL, &status) ) return status ;
 /*if ( fits_read_key_dbl(fmap,"OBSFREQ",&proj->bandcentre, NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"DATAMAX",&datamax, NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"DATAMIN",&datamin, NULL, &status) ) return status ;*/
 if ( fits_read_key_dbl(fmap,"CRVAL1",&ptg_ra , NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"CDELT1",&ra_res, NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"CRVAL2",&ptg_dec, NULL, &status) ) return status ;
 if ( fits_read_key_dbl(fmap,"CDELT2",&dec_res, NULL, &status) ) return status ;

 proj->scan[0].ra = sxg2ra(ptg_ra);
 proj->scan[0].dec = sxg2dec(ptg_dec);
 //dec_fan = (4.5)*(proj->ORTelements/PHASE_I) / cos(deg2rad(dec2sxg(&ptg_dec)));

 ds->databuf = (double*)calloc(XPIX*YPIX*ZPIX, sizeof(double));
 
 fits_read_3d_dbl(fmap, group, nullpix, XPIX, YPIX, XPIX, YPIX, ZPIX, ds->databuf, &anynul, &status);

 if( fits_close_file(fmap, &status) ) printerror(status) ;

 proj->chans = ZPIX;
 sky->xside = XPIX;
 sky->yside = YPIX;
 sky->zside = ZPIX;
 sky->ra_res = ra_res;
 sky->dec_res = dec_res;

 sky->ra = (RAType*)calloc(sky->xside, sizeof(RAType) );
 sky->dec = (DecType*)calloc(sky->yside, sizeof(DecType) );

 for(i = 0 ; i < sky->xside ; i++) sky->ra[i] = sxg2ra(ptg_ra + (i - sky->xside/2)*ra_res);
 for(i = 0 ; i < sky->yside ; i++) sky->dec[i] = sxg2dec(ptg_dec + (i - sky->yside/2)*dec_res);

 cnt = 0;
 sky->map = (double***)calloc(ZPIX, sizeof(double**));
 for(ch = 0; ch<ZPIX;ch++) 
     { sky->map[ch] = (double**)calloc(sky->yside, sizeof(double*) );
   for( j = 0 ; j < sky->yside ; j++ )
       { sky->map[ch][j] = (double*)calloc(sky->xside, sizeof(double) );
     for( i = 0 ; i < sky->xside ; i++ )
     { sky->map[ch][j][i] = ds->databuf[cnt];
       cnt++;
     }
       }
     }

if( fits_close_file(fmap, &status) ) printerror(status) ;


 for( ii = 1; ii < proj->ORTelements ; ii++ )
 { if( proj->unique_baseline[ii] > 1 ) 
   { for(ch = 0 ; ch < proj->chans ; ch++)
     { nu = proj->bandcentre - proj->bandwidth/2 + ch*deltanu + deltanu/2;
       v = baseline[blindex[ii]].v * nu / c ;
       idx = blindex[ii];
       for(i = 0; i < YPIX ; i++)
        { m = sin( deg2rad(dec_res* (i - YPIX/2)));
          for(j=0; j < XPIX ; j++)
          { proj->model_vis[ch][ idx ].r += 1.0 * sky->map[ch][i][j] * cos( 2*M_PI*m*v ); 
	    proj->model_vis[ch][ idx ].i += 1.0 * sky->map[ch][i][j] * sin( 2*M_PI*m*v ); 
	  }
	}
       fprintf(finfo, "%s Model computed for [ %d / %d ] redundant baselines \r", info, ii, proj->redundant_baselines );
     }
   }
 }
 
 return status;
}


int write_fgmap2FITS( ProjParType *proj)
{
 fitsfile *fmap;
 FILE *fsrc;
 int i, x, y;
 double ra_res, dec_res, dec_fan;
 int XPIX = proj->fg.fullsize, YPIX = proj->fg.fullsize;
 int ZPIX = proj->chans;
 RAType ptg_ra;
 DecType ptg_dec;
 char FITSFile[32];
 int bitpix, naxis=3, simple, extend;
 long naxes[naxis];
 int status=0;
 int j;
 char *ctype[] = { "Right Ascension - deg", "Declination - deg", "Frequency - MHz" };
 double crval[] = { ra2sxg(&(proj->scan[0].ra)), dec2sxg(&(proj->scan[0].dec)), proj->bandcentre};
 double cdelt[] = { 0.0, 0.0, 0.0};
 double crpix[] = { XPIX/2, YPIX/2, ZPIX/2 };
 double crota[] = { 0.0, 0.0, 0.0 };
 char keynew[16];
 char ref_date[32];
 double epoch = 2000.0;
 float datamax = 0.0, datamin = 0.0;
 double *data;
 long int cnt;
 int xpix, ypix, zpix;
 double deltanu = proj->bandwidth / proj->chans;

 strcpy(FITSFile, "GALFG.FITS\0");

 remove(FITSFile);
 if(fits_create_file(&fmap,  FITSFile, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }

 fprintf(finfo, "%s %s opened on disk", info, FITSFile);

 bitpix = DOUBLE_IMG;
 simple = TRUE;
 extend = TRUE;
 naxes[0] = XPIX, naxes[1] = YPIX; naxes[2] = ZPIX;

 if( fits_create_img( fmap, bitpix, naxis, naxes, &status ) )
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 
 if ( fits_write_key_str(fmap,"OBJECT",proj->scan[0].src, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"TELESCOP","ORT", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"INSTRUME", proj->receiver, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSERVER", proj->observer, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"EXTNAME", "Primary", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"PROJECT", proj->code, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSMODE", proj->obsmode, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-OBS",ref_date, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-MAP",ref_date, "",&status) ) return status ; 
 if ( fits_write_key_dbl(fmap,"BSCALE",1.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"BZERO",0.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_str(fmap,"BUNIT","Jy/beam", "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"EQUINOX",epoch, 9, "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSRA", ra2sxg(&(proj->scan[0].ra)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSDEC",dec2sxg(&(proj->scan[0].dec)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSFREQ",proj->bandcentre, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMAX",datamax, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMIN",datamin, 9 , "",&status) ) return status ;

 ptg_ra = proj->scan[0].ra;
 ptg_dec = proj->scan[0].dec;
 dec_fan = (4.5)*(proj->ORTelements/PHASE_I) / cos(deg2rad(dec2sxg(&ptg_dec)));
 ra_res = proj->fg.angres;
 dec_res = proj->fg.angres;
 cdelt[0] = -ra_res; cdelt[1] = dec_res; cdelt[2] = deltanu;

 for (j=1; j <= naxis; j++)
    {
      if ( fits_make_keyn ("CTYPE",j,keynew, &status) ) return status ;
      if ( fits_write_key_str(fmap,keynew,ctype[j-1], "",&status)) return status ;
      if ( fits_make_keyn ("CRVAL",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl(fmap,keynew,crval[j-1],10,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CDELT",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,cdelt[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CRPIX",j,keynew,&status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,crpix[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CROTA",j,keynew, &status) ) return status ; 
      if ( fits_write_key_dbl (fmap,keynew,crota[j-1],10,"" ,&status) ) return status ;
    }


 data = (double*)calloc(XPIX * YPIX * ZPIX, sizeof(double) );
 cnt = 0;
 for( zpix = 0 ; zpix < ZPIX ; zpix++ )
 { for(ypix = 0 ; ypix < YPIX ; ypix++ )
   { for( xpix = 0 ; xpix < XPIX ; xpix++ )
     { data[cnt] = proj->fg.map[zpix][ypix][xpix];
       cnt++;
     }
   }
 }
  
 datamax = arraymax(data, cnt-1);

 if( fits_update_key_dbl(fmap, "DATAMAX", datamax, 9, "", &status)) printerror(status);

 fits_write_3d_dbl ( fmap, 0, XPIX, YPIX, XPIX, YPIX, ZPIX, data, &status );
 fprintf(finfo, "...written!\n");
 if( fits_close_file(fmap, &status) ) printerror(status) ;

 free(data);
 return status;

}

int write_pol_Qmap2FITS( ProjParType *proj)
{
 fitsfile *fmap;
 FILE *fsrc;
 int i, x, y;
 double ra_res, dec_res, dec_fan;
 int XPIX = proj->polfg.fullsize, YPIX = proj->polfg.fullsize;
 int ZPIX = proj->chans;
 RAType ptg_ra;
 DecType ptg_dec;
 char FITSFile[32];
 int bitpix, naxis=3, simple, extend;
 long naxes[naxis];
 int status=0;
 int j;
 char *ctype[] = { "Right Ascension - deg", "Declination - deg", "Frequency - MHz" };
 double crval[] = { ra2sxg(&(proj->scan[0].ra)), dec2sxg(&(proj->scan[0].dec)), proj->bandcentre};
 double cdelt[] = { 0.0, 0.0, 0.0};
 double crpix[] = { XPIX/2, YPIX/2, ZPIX/2 };
 double crota[] = { 0.0, 0.0, 0.0 };
 char keynew[16];
 char ref_date[32];
 double epoch = 2000.0;
 float datamax = 0.0, datamin = 0.0;
 double *data;
 long int cnt;
 int xpix, ypix, zpix;
 double deltanu = proj->bandwidth / proj->chans;

 strcpy(FITSFile, "QMAP.FITS\0");

 remove(FITSFile);
 if(fits_create_file(&fmap,  FITSFile, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }

 fprintf(finfo, "%s %s opened on disk", info, FITSFile);

 bitpix = DOUBLE_IMG;
 simple = TRUE;
 extend = TRUE;
 naxes[0] = XPIX, naxes[1] = YPIX; naxes[2] = ZPIX;

 if( fits_create_img( fmap, bitpix, naxis, naxes, &status ) )
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 
 if ( fits_write_key_str(fmap,"OBJECT",proj->scan[0].src, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"TELESCOP","ORT", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"INSTRUME", proj->receiver, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSERVER", proj->observer, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"EXTNAME", "Primary", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"PROJECT", proj->code, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSMODE", proj->obsmode, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-OBS",ref_date, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-MAP",ref_date, "",&status) ) return status ; 
 if ( fits_write_key_dbl(fmap,"BSCALE",1.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"BZERO",0.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_str(fmap,"BUNIT","Jy/beam", "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"EQUINOX",epoch, 9, "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSRA", ra2sxg(&(proj->scan[0].ra)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSDEC",dec2sxg(&(proj->scan[0].dec)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSFREQ",proj->bandcentre, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMAX",datamax, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMIN",datamin, 9 , "",&status) ) return status ;

 ptg_ra = proj->scan[0].ra;
 ptg_dec = proj->scan[0].dec;
 dec_fan = (4.5)*(proj->ORTelements/PHASE_I) / cos(deg2rad(dec2sxg(&ptg_dec)));
 ra_res = proj->polfg.angres;
 dec_res = proj->polfg.angres;
 cdelt[0] = -ra_res; cdelt[1] = dec_res; cdelt[2] = deltanu;

 for (j=1; j <= naxis; j++)
    {
      if ( fits_make_keyn ("CTYPE",j,keynew, &status) ) return status ;
      if ( fits_write_key_str(fmap,keynew,ctype[j-1], "",&status)) return status ;
      if ( fits_make_keyn ("CRVAL",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl(fmap,keynew,crval[j-1],10,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CDELT",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,cdelt[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CRPIX",j,keynew,&status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,crpix[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CROTA",j,keynew, &status) ) return status ; 
      if ( fits_write_key_dbl (fmap,keynew,crota[j-1],10,"" ,&status) ) return status ;
    }


 data = (double*)calloc(XPIX * YPIX * ZPIX, sizeof(double) );
 cnt = 0;
 for( zpix = 0 ; zpix < ZPIX ; zpix++ )
 { for(ypix = 0 ; ypix < YPIX ; ypix++ )
   { for( xpix = 0 ; xpix < XPIX ; xpix++ )
     { data[cnt] = proj->polfg.Qmap[zpix][ypix][xpix];
       cnt++;
     }
   }
 }
  
 datamax = arraymax(data, cnt-1);

 if( fits_update_key_dbl(fmap, "DATAMAX", datamax, 9, "", &status)) printerror(status);

 fits_write_3d_dbl ( fmap, 0, XPIX, YPIX, XPIX, YPIX, ZPIX, data, &status );
 fprintf(finfo, "...written!\n");
 if( fits_close_file(fmap, &status) ) printerror(status) ;

 free(data);
 return status;

}


int write_pol_Umap2FITS( ProjParType *proj)
{
 fitsfile *fmap;
 FILE *fsrc;
 int i, x, y;
 double ra_res, dec_res, dec_fan;
 int XPIX = proj->polfg.fullsize, YPIX = proj->polfg.fullsize;
 int ZPIX = proj->chans;
 RAType ptg_ra;
 DecType ptg_dec;
 char FITSFile[32];
 int bitpix, naxis=3, simple, extend;
 long naxes[naxis];
 int status=0;
 int j;
 char *ctype[] = { "Right Ascension - deg", "Declination - deg", "Frequency - MHz" };
 double crval[] = { ra2sxg(&(proj->scan[0].ra)), dec2sxg(&(proj->scan[0].dec)), proj->bandcentre};
 double cdelt[] = { 0.0, 0.0, 0.0};
 double crpix[] = { XPIX/2, YPIX/2, ZPIX/2 };
 double crota[] = { 0.0, 0.0, 0.0 };
 char keynew[16];
 char ref_date[32];
 double epoch = 2000.0;
 float datamax = 0.0, datamin = 0.0;
 double *data;
 long int cnt;
 int xpix, ypix, zpix;
 double deltanu = proj->bandwidth / proj->chans;

 strcpy(FITSFile, "UMAP.FITS\0");

 remove(FITSFile);
 if(fits_create_file(&fmap,  FITSFile, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }

 fprintf(finfo, "%s %s opened on disk", info, FITSFile);

 bitpix = DOUBLE_IMG;
 simple = TRUE;
 extend = TRUE;
 naxes[0] = XPIX, naxes[1] = YPIX; naxes[2] = ZPIX;

 if( fits_create_img( fmap, bitpix, naxis, naxes, &status ) )
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 
 if ( fits_write_key_str(fmap,"OBJECT",proj->scan[0].src, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"TELESCOP","ORT", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"INSTRUME", proj->receiver, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSERVER", proj->observer, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"EXTNAME", "Primary", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"PROJECT", proj->code, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSMODE", proj->obsmode, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-OBS",ref_date, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-MAP",ref_date, "",&status) ) return status ; 
 if ( fits_write_key_dbl(fmap,"BSCALE",1.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"BZERO",0.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_str(fmap,"BUNIT","Jy/beam", "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"EQUINOX",epoch, 9, "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSRA", ra2sxg(&(proj->scan[0].ra)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSDEC",dec2sxg(&(proj->scan[0].dec)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSFREQ",proj->bandcentre, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMAX",datamax, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMIN",datamin, 9 , "",&status) ) return status ;

 ptg_ra = proj->scan[0].ra;
 ptg_dec = proj->scan[0].dec;
 dec_fan = (4.5)*(proj->ORTelements/PHASE_I) / cos(deg2rad(dec2sxg(&ptg_dec)));
 ra_res = proj->polfg.angres;
 dec_res = proj->polfg.angres;
 cdelt[0] = -ra_res; cdelt[1] = dec_res; cdelt[2] = deltanu;

 for (j=1; j <= naxis; j++)
    {
      if ( fits_make_keyn ("CTYPE",j,keynew, &status) ) return status ;
      if ( fits_write_key_str(fmap,keynew,ctype[j-1], "",&status)) return status ;
      if ( fits_make_keyn ("CRVAL",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl(fmap,keynew,crval[j-1],10,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CDELT",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,cdelt[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CRPIX",j,keynew,&status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,crpix[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CROTA",j,keynew, &status) ) return status ; 
      if ( fits_write_key_dbl (fmap,keynew,crota[j-1],10,"" ,&status) ) return status ;
    }


 data = (double*)calloc(XPIX * YPIX * ZPIX, sizeof(double) );
 cnt = 0;
 for( zpix = 0 ; zpix < ZPIX ; zpix++ )
 { for(ypix = 0 ; ypix < YPIX ; ypix++ )
   { for( xpix = 0 ; xpix < XPIX ; xpix++ )
     { data[cnt] = proj->polfg.Umap[zpix][ypix][xpix];
       cnt++;
     }
   }
 }
  
 datamax = arraymax(data, cnt-1);

 if( fits_update_key_dbl(fmap, "DATAMAX", datamax, 9, "", &status)) printerror(status);

 fits_write_3d_dbl ( fmap, 0, XPIX, YPIX, XPIX, YPIX, ZPIX, data, &status );
 fprintf(finfo, "...written!\n");
 if( fits_close_file(fmap, &status) ) printerror(status) ;

 free(data);
 return status;

}

int write_tecmap2FITS( ProjParType *proj)
{
 fitsfile *fmap;
 FILE *fsrc;
 int i, x, y;
 double ra_res, dec_res, dec_fan;
 int XPIX = proj->tec.fullsize, YPIX = proj->tec.fullsize;
 double **map;
 RAType ptg_ra;
 DecType ptg_dec;
 char FITSFile[32];
 int bitpix, naxis=2, simple, extend;
 long naxes[naxis];
 int status=0;
 int j;
 char *ctype[] = { "Right Ascension - deg", "Declination - deg" };
 double crval[] = { ra2sxg(&(proj->scan[0].ra)), dec2sxg(&(proj->scan[0].dec))};
 double cdelt[] = { 0.0,0.0};
 double crpix[] = { XPIX/2, YPIX/2 };
 double crota[] = { 0.0, 0.0 };
 char keynew[16];
 char ref_date[32];
 double epoch = 2000.0;
 double datamax = 0.0, datamin = 0.0;
 double xarr[XPIX];

 strcpy(FITSFile, "TEC.FITS\0");

 remove(FITSFile);
 if(fits_create_file(&fmap,  FITSFile, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }

 fprintf(finfo, "%s %s opened on disk", info, FITSFile);

 bitpix = DOUBLE_IMG;
 simple = TRUE;
 extend = TRUE;
 naxes[0] = XPIX, naxes[1] = YPIX;

 if( fits_create_img( fmap, bitpix, naxis, naxes, &status ) )
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 
 if ( fits_write_key_str(fmap,"OBJECT",proj->scan[0].src, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"TELESCOP","ORT", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"INSTRUME", proj->receiver, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSERVER", proj->observer, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"EXTNAME", "Primary", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"PROJECT", proj->code, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSMODE", proj->obsmode, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-OBS",ref_date, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-MAP",ref_date, "",&status) ) return status ; 
 if ( fits_write_key_dbl(fmap,"BSCALE",1.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"BZERO",0.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_str(fmap,"BUNIT","Jy/beam", "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"EQUINOX",epoch, 9, "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSRA", ra2sxg(&(proj->scan[0].ra)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSDEC",dec2sxg(&(proj->scan[0].dec)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSFREQ",proj->bandcentre, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMAX",datamax, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMIN",datamin, 9 , "",&status) ) return status ;

 ptg_ra = proj->scan[0].ra;
 ptg_dec = proj->scan[0].dec;
 dec_fan = (4.5)*(proj->ORTelements/PHASE_I) / cos(deg2rad(dec2sxg(&ptg_dec)));
 ra_res = proj->tec.angres;
 dec_res = proj->tec.angres;
 cdelt[0] = -ra_res; cdelt[1] = dec_res;

 for (j=1; j <= naxis; j++)
    {
      if ( fits_make_keyn ("CTYPE",j,keynew, &status) ) return status ;
      if ( fits_write_key_str(fmap,keynew,ctype[j-1], "",&status)) return status ;
      if ( fits_make_keyn ("CRVAL",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl(fmap,keynew,crval[j-1],10,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CDELT",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,cdelt[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CRPIX",j,keynew,&status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,crpix[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CROTA",j,keynew, &status) ) return status ; 
      if ( fits_write_key_dbl (fmap,keynew,crota[j-1],10,"" ,&status) ) return status ;
    }

 for(i = 0 ; i < XPIX ; i++)
 xarr[i] = arraymax(proj->tec.map[i], YPIX);

 datamax = arraymax(xarr, XPIX);

 if( fits_update_key_dbl(fmap, "DATAMAX", datamax, 9, "", &status)) printerror(status);

 for(y = 1; y <= YPIX ; y++)
 { naxes[0] = 1; naxes[1] = y; 
   fits_write_pix(fmap, TDOUBLE, naxes, XPIX, proj->tec.map[y-1], &status);
 }
 fprintf(finfo, "...written!\n");
 if( fits_close_file(fmap, &status) ) printerror(status) ;

 return status;
}


int write_sky2FITS( ProjParType *proj, char *FITSFile )
{
 fitsfile *fmap;
 int i, j, y;
 int XPIX = proj->skymap.xside;
 int YPIX = proj->skymap.yside;
 int ZPIX = proj->chans;
 //char FITSFile[32];
 int bitpix, naxis=3, simple, extend;
 long naxes[naxis];
 int status=0;
 char *ctype[] = { "Right Ascension - deg", "Declination - deg", "Frequency - MHz" };
 double crval[] = { ra2sxg(&(proj->scan[0].ra)), dec2sxg(&(proj->scan[0].dec)), proj->bandcentre};
 double cdelt[] = { 0.0, 0.0, 0.0};
 double crpix[] = { XPIX/2, YPIX/2, ZPIX/2 };
 double crota[] = { 0.0, 0.0, 0.0 };
 char keynew[16];
 char ref_date[32];
 double epoch = 2000.0;
 double datamax = 0.0, datamin = 0.0;
 double *data;
 long int cnt;
 int xpix, ypix, zpix;
 double deltanu = proj->bandwidth / proj->chans;

 //strcpy(FITSFile, "SKYMAP.FITS\0");

 remove(FITSFile);
 if(fits_create_file(&fmap,  FITSFile, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }

 fprintf(finfo, "%s %s opened on disk", info, FITSFile);

 bitpix = DOUBLE_IMG;
 simple = TRUE;
 extend = TRUE;
 naxes[0] = XPIX, naxes[1] = YPIX; naxes[2] = ZPIX;

 if( fits_create_img( fmap, bitpix, naxis, naxes, &status ) )
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 
 if ( fits_write_key_str(fmap,"OBJECT",proj->scan[0].src, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"TELESCOP","ORT", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"INSTRUME", proj->receiver, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSERVER", proj->observer, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"EXTNAME", "Primary", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"PROJECT", proj->code, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSMODE", proj->obsmode, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-OBS",ref_date, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-MAP",ref_date, "",&status) ) return status ; 
 if ( fits_write_key_dbl(fmap,"BSCALE",1.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"BZERO",0.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_str(fmap,"BUNIT","Jy/beam", "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"EQUINOX",epoch, 9, "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSRA", ra2sxg(&(proj->scan[0].ra)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSDEC",dec2sxg(&(proj->scan[0].dec)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSFREQ",proj->bandcentre, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMAX",datamax, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMIN",datamin, 9 , "",&status) ) return status ;

 cdelt[0] = proj->skymap.ra_res; cdelt[1] = proj->skymap.dec_res; cdelt[2] = deltanu;

 for (j=1; j <= naxis; j++)
    {
      if ( fits_make_keyn ("CTYPE",j,keynew, &status) ) return status ;
      if ( fits_write_key_str(fmap,keynew,ctype[j-1], "",&status)) return status ;
      if ( fits_make_keyn ("CRVAL",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl(fmap,keynew,crval[j-1],10,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CDELT",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,cdelt[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CRPIX",j,keynew,&status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,crpix[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CROTA",j,keynew, &status) ) return status ; 
      if ( fits_write_key_dbl (fmap,keynew,crota[j-1],10,"" ,&status) ) return status ;
    }

 data = (double*)calloc(XPIX * YPIX * ZPIX, sizeof(double) );
 cnt = 0;
 for( zpix = 0 ; zpix < ZPIX ; zpix++ )
 { for(ypix = 0 ; ypix < YPIX ; ypix++ )
   { for( xpix = 0 ; xpix < XPIX ; xpix++ )
     { data[cnt] = proj->skymap.map[zpix][ypix][xpix];
       cnt++;
     }
   }
 }
  
 datamax = arraymax(data, cnt-1);

 if( fits_update_key_dbl(fmap, "DATAMAX", datamax, 9, "", &status)) printerror(status);

 fits_write_3d_dbl ( fmap, 0, XPIX, YPIX, XPIX, YPIX, ZPIX, data, &status );
 fprintf(finfo, "...written!\n");
 if( fits_close_file(fmap, &status) ) printerror(status) ;

 free(data);
 return status;
}



int write_fginpb2FITS( ProjParType *proj )
{
 fitsfile *fmap;
 int i, j, y;
 int XPIX = proj->fg.pbside;
 int YPIX = proj->fg.pbside;
 int ZPIX = proj->chans;
 char FITSFile[32];
 int bitpix, naxis=3, simple, extend;
 long naxes[naxis];
 int status=0;
 char *ctype[] = { "Right Ascension - deg", "Declination - deg", "Frequency - MHz" };
 double crval[] = { ra2sxg(&(proj->scan[0].ra)), dec2sxg(&(proj->scan[0].dec)), proj->bandcentre};
 double cdelt[] = { 0.0,0.0, 0.0};
 double crpix[] = { XPIX/2, YPIX/2, ZPIX/2 };
 double crota[] = { 0.0, 0.0, 0.0 };
 char keynew[16];
 char ref_date[32];
 double epoch = 2000.0;
 double datamax = 0.0, datamin = 0.0;
 double *data;
 double deltanu = proj->bandwidth / proj->chans;
 long int cnt;
 int xpix, ypix, zpix;

 strcpy(FITSFile, "GALFG.PRBEAM.FITS\0");

 remove(FITSFile);
 if(fits_create_file(&fmap,  FITSFile, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }

 fprintf(finfo, "%s %s opened on disk", info, FITSFile);

 bitpix = DOUBLE_IMG;
 simple = TRUE;
 extend = TRUE;
 naxes[0] = XPIX, naxes[1] = YPIX; naxes[2] = ZPIX;

 if( fits_create_img( fmap, bitpix, naxis, naxes, &status ) )
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 
 if ( fits_write_key_str(fmap,"OBJECT",proj->scan[0].src, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"TELESCOP","ORT", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"INSTRUME", proj->receiver, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSERVER", proj->observer, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"EXTNAME", "Primary", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"PROJECT", proj->code, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSMODE", proj->obsmode, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-OBS",ref_date, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-MAP",ref_date, "",&status) ) return status ; 
 if ( fits_write_key_dbl(fmap,"BSCALE",1.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"BZERO",0.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_str(fmap,"BUNIT","Jy/beam", "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"EQUINOX",epoch, 9, "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSRA", ra2sxg(&(proj->scan[0].ra)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSDEC",dec2sxg(&(proj->scan[0].dec)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSFREQ",proj->bandcentre, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMAX",datamax, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMIN",datamin, 9 , "",&status) ) return status ;

 cdelt[0] = proj->fg.ra_res; cdelt[1] = proj->fg.dec_res; cdelt[2] = deltanu;

 for (j=1; j <= naxis; j++)
    {
      if ( fits_make_keyn ("CTYPE",j,keynew, &status) ) return status ;
      if ( fits_write_key_str(fmap,keynew,ctype[j-1], "",&status)) return status ;
      if ( fits_make_keyn ("CRVAL",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl(fmap,keynew,crval[j-1],10,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CDELT",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,cdelt[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CRPIX",j,keynew,&status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,crpix[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CROTA",j,keynew, &status) ) return status ; 
      if ( fits_write_key_dbl (fmap,keynew,crota[j-1],10,"" ,&status) ) return status ;
    }


 data = (double*)calloc(XPIX * YPIX * ZPIX, sizeof(double) );
 cnt = 0;
 for( zpix = 0 ; zpix < ZPIX ; zpix++ )
 { for(ypix = 0 ; ypix < YPIX ; ypix++ )
   { for( xpix = 0 ; xpix < XPIX ; xpix++ )
     { data[cnt] = proj->fg.pbmap[zpix][ypix][xpix];
       cnt++;
     }
   }
 }
  
 datamax = arraymax(data, cnt-1);

 if( fits_update_key_dbl(fmap, "DATAMAX", datamax, 9, "", &status)) printerror(status);

 fits_write_3d_dbl ( fmap, 0, XPIX, YPIX, XPIX, YPIX, ZPIX, data, &status );

 fprintf(finfo, "...written!\n");
 if( fits_close_file(fmap, &status) ) printerror(status) ;

 free(data);
 return status;
}


int write_tecinpb2FITS( ProjParType *proj )
{
 fitsfile *fmap;
 int i, j, y;
 int XPIX = proj->tec.pbside;
 int YPIX = proj->tec.pbside;
 char FITSFile[32];
 int bitpix, naxis=2, simple, extend;
 long naxes[naxis];
 int status=0;
 char *ctype[] = { "Right Ascension - deg", "Declination - deg" };
 double crval[] = { ra2sxg(&(proj->scan[0].ra)), dec2sxg(&(proj->scan[0].dec))};
 double cdelt[] = { 0.0,0.0};
 double crpix[] = { XPIX/2, YPIX/2 };
 double crota[] = { 0.0, 0.0 };
 char keynew[16];
 char ref_date[32];
 double epoch = 2000.0;
 double datamax = 0.0, datamin = 0.0;
 double xarr[XPIX];

 strcpy(FITSFile, "TEC.PRBEAM.FITS\0");

 remove(FITSFile);
 if(fits_create_file(&fmap,  FITSFile, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }

 fprintf(finfo, "%s %s opened on disk", info, FITSFile);

 bitpix = DOUBLE_IMG;
 simple = TRUE;
 extend = TRUE;
 naxes[0] = XPIX, naxes[1] = YPIX;

 if( fits_create_img( fmap, bitpix, naxis, naxes, &status ) )
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 
 if ( fits_write_key_str(fmap,"OBJECT",proj->scan[0].src, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"TELESCOP","ORT", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"INSTRUME", proj->receiver, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSERVER", proj->observer, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"EXTNAME", "Primary", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"PROJECT", proj->code, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSMODE", proj->obsmode, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-OBS",ref_date, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-MAP",ref_date, "",&status) ) return status ; 
 if ( fits_write_key_dbl(fmap,"BSCALE",1.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"BZERO",0.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_str(fmap,"BUNIT","Jy/beam", "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"EQUINOX",epoch, 9, "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSRA", ra2sxg(&(proj->scan[0].ra)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSDEC",dec2sxg(&(proj->scan[0].dec)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSFREQ",proj->bandcentre, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMAX",datamax, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMIN",datamin, 9 , "",&status) ) return status ;

 cdelt[0] = proj->tec.ra_res; cdelt[1] = proj->tec.dec_res;

 for (j=1; j <= naxis; j++)
    {
      if ( fits_make_keyn ("CTYPE",j,keynew, &status) ) return status ;
      if ( fits_write_key_str(fmap,keynew,ctype[j-1], "",&status)) return status ;
      if ( fits_make_keyn ("CRVAL",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl(fmap,keynew,crval[j-1],10,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CDELT",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,cdelt[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CRPIX",j,keynew,&status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,crpix[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CROTA",j,keynew, &status) ) return status ; 
      if ( fits_write_key_dbl (fmap,keynew,crota[j-1],10,"" ,&status) ) return status ;
    }

 for(i = 0 ; i < XPIX ; i++)
 xarr[i] = arraymax(proj->tec.pbmap[i], YPIX);

 datamax = arraymax(xarr, XPIX);

 if( fits_update_key_dbl(fmap, "DATAMAX", datamax, 9, "", &status)) printerror(status);

 for(y = 1; y <= YPIX ; y++)
 { naxes[0] = 1; naxes[1] = y; 
   fits_write_pix(fmap, TDOUBLE, naxes, XPIX, proj->tec.pbmap[y-1], &status);
 }
 fprintf(finfo, "...written!\n");
 if( fits_close_file(fmap, &status) ) printerror(status) ;

 return status;
}

int write_prbeam2FITS( ProjParType *proj, char *filename )
{
 fitsfile *fmap;
 int i, j, y;
 int XPIX = proj->pbeam.xside;
 int YPIX = proj->pbeam.yside;
 int ZPIX = proj->chans;
 char FITSFile[32];
 int bitpix, naxis=3, simple, extend;
 long naxes[naxis];
 int status=0;
 char *ctype[] = { "Right Ascension - deg", "Declination - deg", "Frequency - MHz" };
 double crval[] = { ra2sxg(&(proj->scan[0].ra)), dec2sxg(&(proj->scan[0].dec)), proj->bandcentre};
 double cdelt[] = { 0.0,0.0, 0.0};
 double crpix[] = { XPIX/2, YPIX/2, ZPIX/2 };
 double crota[] = { 0.0, 0.0, 0.0 };
 char keynew[16];
 char ref_date[32];
 double epoch = 2000.0;
 double datamax = 0.0, datamin = 0.0;
 //double xarr[XPIX];
 double *data;
 double deltanu = proj->bandwidth / proj->chans ;
 int xpix, ypix, zpix;
 long int cnt;
 FILE *fp = fopen("prbeam.dat","w");

 if(filename == NULL ) strcpy(FITSFile, "PRBEAM.FITS\0");
 else strcpy(FITSFile, filename );

 remove(FITSFile);
 if(fits_create_file(&fmap,  FITSFile, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }

 fprintf(finfo, "%s %s opened on disk", info, FITSFile);

 bitpix = DOUBLE_IMG;
 simple = TRUE;
 extend = TRUE;
 naxes[0] = XPIX, naxes[1] = YPIX; naxes[2] = ZPIX;

 if( fits_create_img( fmap, bitpix, naxis, naxes, &status ) )
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 
 if ( fits_write_key_str(fmap,"OBJECT",proj->scan[0].src, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"TELESCOP","ORT", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"INSTRUME", proj->receiver, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSERVER", proj->observer, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"EXTNAME", "Primary", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"PROJECT", proj->code, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSMODE", proj->obsmode, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-OBS",ref_date, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-MAP",ref_date, "",&status) ) return status ; 
 if ( fits_write_key_dbl(fmap,"BSCALE",1.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"BZERO",0.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_str(fmap,"BUNIT","Jy/beam", "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"EQUINOX",epoch, 9, "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSRA", ra2sxg(&(proj->scan[0].ra)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSDEC",dec2sxg(&(proj->scan[0].dec)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSFREQ",proj->bandcentre, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMAX",datamax, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMIN",datamin, 9 , "",&status) ) return status ;

 cdelt[0] = proj->pbeam.ra_res; cdelt[1] = - proj->pbeam.dec_res; cdelt[2] = deltanu;

 for (j=1; j <= naxis; j++)
    {
      if ( fits_make_keyn ("CTYPE",j,keynew, &status) ) return status ;
      if ( fits_write_key_str(fmap,keynew,ctype[j-1], "",&status)) return status ;
      if ( fits_make_keyn ("CRVAL",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl(fmap,keynew,crval[j-1],10,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CDELT",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,cdelt[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CRPIX",j,keynew,&status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,crpix[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CROTA",j,keynew, &status) ) return status ; 
      if ( fits_write_key_dbl (fmap,keynew,crota[j-1],10,"" ,&status) ) return status ;
    }

 data = (double*)calloc(XPIX * YPIX * ZPIX, sizeof(double) );
 cnt = 0;
 for( zpix = 0 ; zpix < ZPIX ; zpix++ )
 { for(ypix = 0 ; ypix < YPIX ; ypix++ )
   { for( xpix = 0 ; xpix < XPIX ; xpix++ )
     { data[cnt] = proj->pbeam.shape[zpix][ypix][xpix];
	 //fprintf(fp, "%e\n", data[cnt]);	 
       cnt++;
     }
   }
 }
 fclose(fp);
 datamax = arraymax(data, cnt-1);

 if( fits_update_key_dbl(fmap, "DATAMAX", datamax, 9, "", &status)) printerror(status);

 fits_write_3d_dbl ( fmap, 0, XPIX, YPIX, XPIX, YPIX, ZPIX, data, &status );
 /*
 for(i = 0 ; i < XPIX ; i++)
 xarr[i] = arraymax(proj->pbeam.shape[proj->chans/2][i], YPIX);

 datamax = arraymax(xarr, XPIX);

 if( fits_update_key_dbl(fmap, "DATAMAX", datamax, 9, "", &status)) printerror(status);

 for(y = 1; y <= YPIX ; y++)
 { naxes[0] = 1; naxes[1] = y; 
   fits_write_pix(fmap, TDOUBLE, naxes, XPIX, proj->pbeam.shape[y-1], &status);
 }
 */
 fprintf(finfo, "...written!\n");
 if( fits_close_file(fmap, &status) ) printerror(status) ;

 free(data);
 return status;
}
int write_HImap2FITS( ProjParType *proj, char *filename )
{
 fitsfile *fmap;
 int i, j, y;
 int XPIX = MAPSIDE;
 int YPIX = MAPSIDE;
 int ZPIX = proj->chans;
 char FITSFile[32];
 int bitpix, naxis=3, simple, extend;
 long naxes[naxis];
 int status=0;
 char *ctype[] = { "Right Ascension - deg", "Declination - deg", "Frequency - MHz" };
 double crval[] = { ra2sxg(&(proj->scan[0].ra)), dec2sxg(&(proj->scan[0].dec)), proj->bandcentre};
 double cdelt[] = { 0.0,0.0, 0.0};
 double crpix[] = { XPIX/2, YPIX/2, ZPIX/2 };
 double crota[] = { 0.0, 0.0, 0.0 };
 char keynew[16];
 char ref_date[32];
 double epoch = 2000.0;
 double datamax = 0.0, datamin = 0.0;
 //double xarr[XPIX];
 double *data;
 double deltanu = proj->bandwidth / proj->chans ;
 int xpix, ypix, zpix;
 long int cnt;
 FILE *fp = fopen(filename,"r");
 int pd;
 double bias = 2.0;
 double I0  = 1.13 * CELLSIZE * CELLSIZE *bias / (1./8.0);

 strcpy(FITSFile, proj->HI.filename );

 remove(FITSFile);
 if(fits_create_file(&fmap,  FITSFile, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }

 fprintf(finfo, "%s %s opened on disk", info, FITSFile);
 newline();

 bitpix = DOUBLE_IMG;
 simple = TRUE;
 extend = TRUE;
 naxes[0] = XPIX, naxes[1] = YPIX; naxes[2] = ZPIX;

 if( fits_create_img( fmap, bitpix, naxis, naxes, &status ) )
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 
 if ( fits_write_key_str(fmap,"OBJECT",proj->scan[0].src, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"TELESCOP","ORT", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"INSTRUME", proj->receiver, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSERVER", proj->observer, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"EXTNAME", "Primary", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"PROJECT", proj->code, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSMODE", proj->obsmode, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-OBS",ref_date, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-MAP",ref_date, "",&status) ) return status ; 
 if ( fits_write_key_dbl(fmap,"BSCALE",1.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"BZERO",0.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_str(fmap,"BUNIT","Jy/beam", "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"EQUINOX",epoch, 9, "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSRA", ra2sxg(&(proj->scan[0].ra)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSDEC",dec2sxg(&(proj->scan[0].dec)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSFREQ",proj->bandcentre, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMAX",datamax, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMIN",datamin, 9 , "",&status) ) return status ;

 cdelt[0] = rad2deg(CELLSIZE); cdelt[1] = rad2deg(CELLSIZE); cdelt[2] = deltanu;

 for (j=1; j <= naxis; j++)
    {
      if ( fits_make_keyn ("CTYPE",j,keynew, &status) ) return status ;
      if ( fits_write_key_str(fmap,keynew,ctype[j-1], "",&status)) return status ;
      if ( fits_make_keyn ("CRVAL",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl(fmap,keynew,crval[j-1],10,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CDELT",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,cdelt[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CRPIX",j,keynew,&status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,crpix[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CROTA",j,keynew, &status) ) return status ; 
      if ( fits_write_key_dbl (fmap,keynew,crota[j-1],10,"" ,&status) ) return status ;
    }

 data = (double*)calloc(XPIX * YPIX * ZPIX, sizeof(double) );
 cnt = 0;
 for( zpix = 0 ; zpix < ZPIX ; zpix++ )
 { for(ypix = 0 ; ypix < YPIX ; ypix++ )
   { for( xpix = 0 ; xpix < XPIX ; xpix++ )
     { fscanf(fp, "%d\n", &pd);
       data[cnt] = pd * I0;
       cnt++;
     }
   }
     fprintf(finfo, "%s Converted %3d/%3d channels\r", info, zpix+1, proj->chans);
 }
 fclose(fp);
 datamax = arraymax(data, cnt-1);

 if( fits_update_key_dbl(fmap, "DATAMAX", datamax, 9, "", &status)) printerror(status);

 fits_write_3d_dbl ( fmap, 0, XPIX, YPIX, XPIX, YPIX, ZPIX, data, &status );

 newline();
 fprintf(finfo, "%s...written!\n", info);
 if( fits_close_file(fmap, &status) ) printerror(status) ;

 free(data);
 return status;
}

void write_maps2FITS( ProjParType *proj )
{
 char *filename=NULL;
 write_prbeam2FITS( proj, filename );
 if( proj->galfg_swtch) write_fgmap2FITS( proj );
 //write_pol_Qmap2FITS( proj );
 //write_pol_Umap2FITS( proj );
 //if( proj->galfg_swtch) write_fginpb2FITS( proj );
 /*write_tecmap2FITS( proj );
   write_tecinpb2FITS( proj );*/
 if( proj->pntsrc_swtch) write_pntsrc2FITS( proj );
 //write_sky2FITS( proj, "SKYMAP.FITS" );
}


int write_V2corr2FITS( ProjParType *proj )
{
 fitsfile *fmap;
 int i, j, y;
 int XPIX = proj->chans;
 int YPIX = proj->chans;
 int ZPIX = proj->redundant_baselines;
 char FITSFile[32];
 int bitpix, naxis=3, simple, extend;
 long naxes[naxis];
 int status=0;
 char *ctype[] = { "\\fn\\(2139) MHz", "\\fn\\(2139) MHz", "Baseline" };
 double crval[] = { proj->bandcentre/1e6, proj->bandcentre/1e6, (double)(proj->redundant_baselines+1)/2};
 double cdelt[] = { 0.0,0.0,0.0};
 double crpix[] = { XPIX/2, YPIX/2, ZPIX/2 };
 double crota[] = { 0.0, 0.0, 0.0 };
 char keynew[16];
 char ref_date[32];
 double epoch = 2000.0;
 double datamax = 0.0, datamin = 0.0;
 double xarr[XPIX];
 double deltanu = proj->bandwidth / proj->chans/1e3;
 int bl, ch1, ch2, cnt;
 double *data;
 FILE *fv2corr = fopen("V2CORR.dat", "w");
 strcpy(FITSFile, "V2CORR.FITS\0");

 remove(FITSFile);
 if(fits_create_file(&fmap,  FITSFile, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }

 fprintf(finfo, "%s %s opened on disk", info, FITSFile);

 bitpix = DOUBLE_IMG;
 simple = TRUE;
 extend = TRUE;
 naxes[0] = XPIX, naxes[1] = YPIX; naxes[2] = ZPIX;

 if( fits_create_img( fmap, bitpix, naxis, naxes, &status ) )
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 
 if ( fits_write_key_str(fmap,"OBJECT",proj->scan[0].src, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"TELESCOP","ORT", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"INSTRUME", proj->receiver, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSERVER", proj->observer, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"EXTNAME", "Primary", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"PROJECT", proj->code, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSMODE", proj->obsmode, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-OBS",ref_date, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-MAP",ref_date, "",&status) ) return status ; 
 if ( fits_write_key_dbl(fmap,"BSCALE",1.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"BZERO",0.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_str(fmap,"BUNIT","Jy/beam", "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"EQUINOX",epoch, 9, "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSRA", ra2sxg(&(proj->scan[0].ra)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSDEC",dec2sxg(&(proj->scan[0].dec)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSFREQ",proj->bandcentre, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMAX",datamax, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMIN",datamin, 9 , "",&status) ) return status ;

 cdelt[0] = deltanu/1e3; cdelt[1] = deltanu/1e3; cdelt[2] = 1.0;

 for (j=1; j <= naxis; j++)
    {
      if ( fits_make_keyn ("CTYPE",j,keynew, &status) ) return status ;
      if ( fits_write_key_str(fmap,keynew,ctype[j-1], "",&status)) return status ;
      if ( fits_make_keyn ("CRVAL",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl(fmap,keynew,crval[j-1],10,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CDELT",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,cdelt[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CRPIX",j,keynew,&status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,crpix[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CROTA",j,keynew, &status) ) return status ; 
      if ( fits_write_key_dbl (fmap,keynew,crota[j-1],10,"" ,&status) ) return status ;
    }

 data = (double*)calloc((proj->redundant_baselines+1)*proj->chans*proj->chans, sizeof(double) );
  cnt = 0;
  for( bl = 0 ; bl < proj->redundant_baselines ; bl++ )
  { for(ch1 = 0 ; ch1 < proj->chans ; ch1++ )
    { for( ch2 = 0 ; ch2 < proj->chans ; ch2++ )
      { data[cnt] = proj->V2[bl][ch1][ch2];
	fprintf(fv2corr, "%e\n", data[cnt]);
	cnt++;
      }
    }
  }
  
  fclose(fv2corr);
 datamax = arraymax(data, cnt-1);

 if( fits_update_key_dbl(fmap, "DATAMAX", datamax, 9, "", &status)) printerror(status);

 fits_write_3d_dbl ( fmap, 0, XPIX, YPIX, XPIX, YPIX, ZPIX, data, &status );

 fprintf(finfo, "...written!\n");
 if( fits_close_file(fmap, &status) ) printerror(status) ;

 free(data);

 return status;
}

int write_V2_err2FITS( ProjParType *proj )
{
 fitsfile *fmap;
 int i, j, y;
 int XPIX = proj->chans;
 int YPIX = proj->chans;
 int ZPIX = proj->redundant_baselines;
 char FITSFile[32];
 int bitpix, naxis=3, simple, extend;
 long naxes[naxis];
 int status=0;
 char *ctype[] = { "\\fn\\(2139) MHz", "\\fn\\(2139) MHz", "Baseline" };
 double crval[] = { proj->bandcentre/1e6, proj->bandcentre/1e6, (double)(proj->redundant_baselines+1)/2};
 double cdelt[] = { 0.0,0.0,0.0};
 double crpix[] = { XPIX/2, YPIX/2, ZPIX/2 };
 double crota[] = { 0.0, 0.0, 0.0 };
 char keynew[16];
 char ref_date[32];
 double epoch = 2000.0;
 double datamax = 0.0, datamin = 0.0;
 double xarr[XPIX];
 double deltanu = proj->bandwidth / proj->chans/1e3;
 int bl, ch1, ch2, cnt;
 double *data;
 FILE *fv2corr = fopen("V2ERR.dat", "w");
 strcpy(FITSFile, "V2ERR.FITS\0");

 remove(FITSFile);
 if(fits_create_file(&fmap,  FITSFile, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }

 fprintf(finfo, "%s %s opened on disk", info, FITSFile);

 bitpix = DOUBLE_IMG;
 simple = TRUE;
 extend = TRUE;
 naxes[0] = XPIX, naxes[1] = YPIX; naxes[2] = ZPIX;

 if( fits_create_img( fmap, bitpix, naxis, naxes, &status ) )
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 
 if ( fits_write_key_str(fmap,"OBJECT",proj->scan[0].src, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"TELESCOP","ORT", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"INSTRUME", proj->receiver, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSERVER", proj->observer, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"EXTNAME", "Primary", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"PROJECT", proj->code, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSMODE", proj->obsmode, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-OBS",ref_date, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-MAP",ref_date, "",&status) ) return status ; 
 if ( fits_write_key_dbl(fmap,"BSCALE",1.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"BZERO",0.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_str(fmap,"BUNIT","Jy/beam", "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"EQUINOX",epoch, 9, "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSRA", ra2sxg(&(proj->scan[0].ra)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSDEC",dec2sxg(&(proj->scan[0].dec)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSFREQ",proj->bandcentre, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMAX",datamax, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMIN",datamin, 9 , "",&status) ) return status ;

 cdelt[0] = deltanu/1e3; cdelt[1] = deltanu/1e3; cdelt[2] = 1.0;

 for (j=1; j <= naxis; j++)
    {
      if ( fits_make_keyn ("CTYPE",j,keynew, &status) ) return status ;
      if ( fits_write_key_str(fmap,keynew,ctype[j-1], "",&status)) return status ;
      if ( fits_make_keyn ("CRVAL",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl(fmap,keynew,crval[j-1],10,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CDELT",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,cdelt[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CRPIX",j,keynew,&status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,crpix[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CROTA",j,keynew, &status) ) return status ; 
      if ( fits_write_key_dbl (fmap,keynew,crota[j-1],10,"" ,&status) ) return status ;
    }

 data = (double*)calloc((proj->redundant_baselines+1)*proj->chans*proj->chans, sizeof(double) );
  cnt = 0;
  for( bl = 0 ; bl < proj->redundant_baselines ; bl++ )
  { for(ch1 = 0 ; ch1 < proj->chans ; ch1++ )
    { for( ch2 = 0 ; ch2 < proj->chans ; ch2++ )
      { data[cnt] = proj->V2_err[bl][ch1][ch2];
        fprintf(fv2corr, "%e\n", data[cnt]);
	cnt++;
      }
    }
  }
  
  fclose(fv2corr);
 datamax = arraymax(data, cnt-1);

 if( fits_update_key_dbl(fmap, "DATAMAX", datamax, 9, "", &status)) printerror(status);

 fits_write_3d_dbl ( fmap, 0, XPIX, YPIX, XPIX, YPIX, ZPIX, data, &status );

 fprintf(finfo, "...written!\n");
 if( fits_close_file(fmap, &status) ) printerror(status) ;

 free(data);

 return status;
}

int write_qb2FITS( ProjParType *proj, char *filename, char *FITSFile )
{
 fitsfile *fmap;
 int i, j, y;
 int XPIX = proj->chans;
 int YPIX = proj->chans;
 int ZPIX = proj->redundant_baselines;
 int bitpix, naxis=3, simple, extend;
 long naxes[naxis];
 int status=0;
 char *ctype[] = { "\\fn\\(2139) MHz", "\\fn\\(2139) MHz", "Baseline" };
 double crval[] = { proj->bandcentre/1e6, proj->bandcentre/1e6, (double)(proj->redundant_baselines+1)/2};
 double cdelt[] = { 0.0,0.0,0.0};
 double crpix[] = { XPIX/2, YPIX/2, ZPIX/2 };
 double crota[] = { 0.0, 0.0, 0.0 };
 char keynew[16];
 char ref_date[32];
 double epoch = 2000.0;
 double datamax = 0.0, datamin = 0.0;
 double xarr[XPIX];
 double deltanu = proj->bandwidth / proj->chans/1e3;
 int bl, ch1, ch2, cnt;
 double *data;
 FILE *fv2res = fopen(filename, "r");
 char line[80];

 data = (double*)calloc((proj->redundant_baselines+1)*proj->chans*proj->chans, sizeof(double) );
 cnt = 0;
  for( bl = 0 ; bl < proj->redundant_baselines ; bl++ )
  { for(ch1 = 0 ; ch1 < proj->chans ; ch1++ )
    { for( ch2 = 0 ; ch2 < proj->chans ; ch2++ )
     { fgets(line, sizeof(line), fv2res);
        sscanf(line, "%lf", data+cnt);
       proj->V2[bl][ch1][ch2] = data[cnt];
       cnt++;
     }
    }
  }
  
  fclose(fv2res);

 remove(FITSFile);
 if(fits_create_file(&fmap,  FITSFile, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }

 fprintf(finfo, "%s %s opened on disk", info, FITSFile);

 bitpix = DOUBLE_IMG;
 simple = TRUE;
 extend = TRUE;
 naxes[0] = XPIX, naxes[1] = YPIX; naxes[2] = ZPIX;

 if( fits_create_img( fmap, bitpix, naxis, naxes, &status ) )
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 
 if ( fits_write_key_str(fmap,"OBJECT",proj->scan[0].src, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"TELESCOP","ORT", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"INSTRUME", proj->receiver, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSERVER", proj->observer, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"EXTNAME", "Primary", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"PROJECT", proj->code, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSMODE", proj->obsmode, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-OBS",ref_date, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-MAP",ref_date, "",&status) ) return status ; 
 if ( fits_write_key_dbl(fmap,"BSCALE",1.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"BZERO",0.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_str(fmap,"BUNIT","Jy/beam", "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"EQUINOX",epoch, 9, "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSRA", ra2sxg(&(proj->scan[0].ra)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSDEC",dec2sxg(&(proj->scan[0].dec)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSFREQ",proj->bandcentre, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMAX",datamax, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMIN",datamin, 9 , "",&status) ) return status ;

 cdelt[0] = deltanu/1e3; cdelt[1] = deltanu/1e3; cdelt[2] = 1.0;

 for (j=1; j <= naxis; j++)
    {
      if ( fits_make_keyn ("CTYPE",j,keynew, &status) ) return status ;
      if ( fits_write_key_str(fmap,keynew,ctype[j-1], "",&status)) return status ;
      if ( fits_make_keyn ("CRVAL",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl(fmap,keynew,crval[j-1],10,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CDELT",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,cdelt[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CRPIX",j,keynew,&status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,crpix[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CROTA",j,keynew, &status) ) return status ; 
      if ( fits_write_key_dbl (fmap,keynew,crota[j-1],10,"" ,&status) ) return status ;
    }

 datamax = arraymax(data, cnt-1);
 if( fits_update_key_dbl(fmap, "DATAMAX", datamax, 9, "", &status)) printerror(status);

 fits_write_3d_dbl ( fmap, 0, XPIX, YPIX, XPIX, YPIX, ZPIX, data, &status );

 fprintf(finfo, "...written!\n");
 if( fits_close_file(fmap, &status) ) printerror(status) ;

 free(data);

 return status;
}

int write_fgmask2FITS( ProjParType *proj )
{
 fitsfile *fmap;
 int i, j, y;
 int XPIX = proj->ants;
 int YPIX = proj->ants;
 int ZPIX = proj->nrecs;
 char FITSFile[32];
 int bitpix, naxis=3, simple, extend;
 long naxes[naxis];
 int status=0;
 char *ctype[] = { "Antenna", "Antenna", "Record" };
 double crval[] = { (double)proj->ants/2, (double)proj->ants/2, (double)proj->nrecs/2};
 double cdelt[] = { 0.0,0.0,0.0};
 double crpix[] = { XPIX/2, YPIX/2, ZPIX/2 };
 double crota[] = { 0.0, 0.0, 0.0 };
 char keynew[16];
 char ref_date[32];
 double epoch = 2000.0;
 double datamax = 0.0, datamin = 0.0;
 double xarr[XPIX];
 int rec, ant1, ant2, cnt;
 double *data;

 strcpy(FITSFile, "FGMASK.FITS\0");

 remove(FITSFile);
 if(fits_create_file(&fmap,  FITSFile, &status)) 
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }

 fprintf(finfo, "%s %s opened on disk", info, FITSFile);

 bitpix = DOUBLE_IMG;
 simple = TRUE;
 extend = TRUE;
 naxes[0] = XPIX, naxes[1] = YPIX; naxes[2] = ZPIX;

 if( fits_create_img( fmap, bitpix, naxis, naxes, &status ) )
 { fprintf(ferr, "%s ", err); 
   printerror(status); 
   exit(1); 
 }
 
 if ( fits_write_key_str(fmap,"OBJECT",proj->scan[0].src, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"TELESCOP","ORT", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"INSTRUME", proj->receiver, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSERVER", proj->observer, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"EXTNAME", "Primary", "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"PROJECT", proj->code, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"OBSMODE", proj->obsmode, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-OBS",ref_date, "",&status) ) return status ;
 if ( fits_write_key_str(fmap,"DATE-MAP",ref_date, "",&status) ) return status ; 
 if ( fits_write_key_dbl(fmap,"BSCALE",1.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"BZERO",0.0E+00 ,11,"",&status) ) return status ;
 if ( fits_write_key_str(fmap,"BUNIT","Jy/beam", "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"EQUINOX",epoch, 9, "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSRA", ra2sxg(&(proj->scan[0].ra)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSDEC",dec2sxg(&(proj->scan[0].dec)), 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"OBSFREQ",proj->bandcentre, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMAX",datamax, 9 , "",&status) ) return status ;
 if ( fits_write_key_dbl(fmap,"DATAMIN",datamin, 9 , "",&status) ) return status ;

 cdelt[0] = 1.0; cdelt[1] = 1.0; cdelt[2] = 1.0;

 for (j=1; j <= naxis; j++)
    {
      if ( fits_make_keyn ("CTYPE",j,keynew, &status) ) return status ;
      if ( fits_write_key_str(fmap,keynew,ctype[j-1], "",&status)) return status ;
      if ( fits_make_keyn ("CRVAL",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl(fmap,keynew,crval[j-1],10,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CDELT",j,keynew, &status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,cdelt[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CRPIX",j,keynew,&status) ) return status ;
      if ( fits_write_key_dbl (fmap,keynew,crpix[j-1],9,"" ,&status) ) return status ;
      if ( fits_make_keyn ("CROTA",j,keynew, &status) ) return status ; 
      if ( fits_write_key_dbl (fmap,keynew,crota[j-1],10,"" ,&status) ) return status ;
    }

 data = (double*)calloc(proj->nrecs*proj->ants*proj->ants, sizeof(double) );
  cnt = 0;
  for( rec = 0 ; rec < proj->nrecs ; rec++ )
  { for(ant1 = 0 ; ant1 < proj->ants ; ant1++ )
    { for( ant2 = 0 ; ant2 < proj->ants ; ant2++ )
      { data[cnt] = (double)proj->flagmask[rec][ant1][ant2];
	cnt++;
      }
    }
  }
  
 datamax = arraymax(data, cnt-1);

 if( fits_update_key_dbl(fmap, "DATAMAX", datamax, 9, "", &status)) printerror(status);

 fits_write_3d_dbl ( fmap, 0, XPIX, YPIX, XPIX, YPIX, ZPIX, data, &status );

 fprintf(finfo, "...written!\n");
 if( fits_close_file(fmap, &status) ) printerror(status) ;

 free(data);

 return status;
}
