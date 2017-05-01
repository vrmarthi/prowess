/* function to read the meta data associated with a given ippspectcorr
   output file. This is a hacked version of Peeyush's writecorr()

   The function allocates the required memory for the corrhdr, mpkt, and
   packblinehdr. So one should pass pointers to pointers of the corresponding
   structures.

   After a call to this routine, the input file is positioned just before
   the start of data.

   jnc 13/dec/12 RAC
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../inc/sysdefs.h"
#include "hdr.h"


int get_ippc_metadata (FILE *fpkt, CorrHdrType **corrhdrp,  MetaPktType **mpktp,
		       PackBlineHdrType **packblinehdrp)
{ DataHdrType       hdr;
  int               corrpktsize = 0;
  unsigned char    *dbuf = NULL;
  CorrHdrType      *corrhdr;
  MetaPktType      *mpkt;
  PackBlineHdrType *packblinehdr;

  fread (&hdr, sizeof (DataHdrType), 1, fpkt);
  if (hdr.words == 0xfff)
    corrpktsize = hdr.tick*8;
  else 
    corrpktsize = hdr.words*8;

  if ((dbuf=calloc (corrpktsize, 1)) == NULL)
    { perror ("calloc"); return -1; }
  if ((*corrhdrp=calloc (corrpktsize, 1)) == NULL)
    { perror ("calloc"); return -1; } 
  if ((*mpktp=calloc (corrpktsize, 1)) == NULL)
    { perror ("calloc"); return -1; } 

  mpkt=*mpktp;
  rewind (fpkt);
  fread (dbuf, corrpktsize, 1, fpkt);
  memcpy (mpkt, dbuf,corrpktsize);
  corrhdr=(CorrHdrType*)dbuf;

  // Get rid of Meta information packets.
  while (corrhdr->datatype == DataMeta)
    fread (dbuf, corrpktsize, 1, fpkt);
  // put back the last read packet to fstream
  fseek (fpkt, -corrpktsize, SEEK_CUR); 
  
  if (corrhdr->datatype == DataSpectCorrPack){
    fprintf (stderr, "## NOTE: Found Packed spectral correlations data.\n");
  }else{
    fprintf (stderr, "# Data is of unrecognized type %s. Quitting.\n", 
	     DataType[corrhdr->datatype]); 
    return -1; 
  }

  if (!((corrhdr->bits2pix == _32fcmplx)|| (corrhdr->bits2pix == _16scmplx))){
    fprintf (stderr, "### Unknown datatype! Quitting!\n"); 
    return -1;
  }
  
  fprintf (stdout, "# BITS2PIX : %-16s\n", BppDesc[corrhdr->bits2pix]);
  fprintf (stdout, "# Datatype : %-16s\n", DataType[corrhdr->datatype]);
  fprintf (stdout, "# PktSize  : %-16d\n", corrpktsize);
  fprintf (stdout, "# Channels : %-16d\n", NChans[corrhdr->chans]);
  fprintf (stdout, "# Baselines: %-16d\n", corrhdr->blines);
  fprintf (stdout, "# Taps     : %-16d\n", corrhdr->taps);
  fprintf (stdout, "# Fsamp    : %-16.3f\n", corrhdr->f_samp);

  packblinehdr = (PackBlineHdrType*) (dbuf + (int) (16 * 
                    ceil (sizeof (CorrHdrType)/16.)));
  if ((*packblinehdrp=calloc (corrhdr->blines*sizeof(PackBlineHdrType), 1))
      == NULL){
    perror ("calloc"); 
    return -1; 
  } 
  
  memcpy(*corrhdrp,corrhdr,corrpktsize);
  memcpy(*packblinehdrp,packblinehdr,corrhdr->blines*sizeof(PackBlineHdrType));

  free(dbuf);
  return corrpktsize;
}
/* copies over a single record of data from the ippspectcorr file
   into the proj structure

   jnc 13/dec/12
*/
get_next_ippcrec(FILE *fpkt, int corrpktsize, CorrHdrType *corrhdr,  
		 ProjParType *proj, int *fits2ippmap){

  unsigned char *dbuf = NULL;
  Cmplx3Type    *out;
  float         *bline32fc,*in;
  int            b,b1,c,conjugate;    

  if ((dbuf=calloc (corrpktsize, 1)) == NULL){
    perror ("calloc"); 
    return -1; 
  }
  
  if (fread (dbuf, corrpktsize, 1, fpkt) != 1){
    perror ("fread"); 
    return -1;
  }

  if (corrhdr->bits2pix != _32fcmplx){
    fprintf(stderr,"Can only handle corrhdr->bits2pix = _32fcmplx not %d\n",
	    corrhdr->bits2pix);
    return -1;
  }
  /* offset to the start of the data */
  bline32fc = (float*) (dbuf + 
			(int) (16 * ceil (sizeof (CorrHdrType)/16.))
	+ (int) (16 * ceil (sizeof (PackBlineHdrType)*corrhdr->blines/16.)));
      

  /* copy over the baseline data from the buffer into the proj structure */
  for(b=0;b<proj->baselines;b++){
    out = (Cmplx3Type *) (proj->RecBuf + b*(proj->recwords) + proj->pcount);
    b1=fits2ippmap[b]/2;
    if(fits2ippmap[b]%2) conjugate=1;
    else conjugate=0;
    float *in = bline32fc + 2*b1*proj->chans;
    if(conjugate){
      for(c=0;c>proj->chans;c++){
	out->r=in[0];out->i=in[1];
	in +=2; out++;
      }
    }else{
      for(c=0;c>proj->chans;c++){
	out->r=in[0];out->i=-in[1];
	in +=2; out++;
      }
    }
  }

  return 0;
}
