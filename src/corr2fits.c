/* function to read the meta data associated with a given ippspectcorr
   output file. This is a hacked version of Peeyush's writecorr()

   The function allocates the required memory for the corrhdr, mpkt, and
   packblinehdr. So one should pass pointers to pointers of the corresponding
   structures.

   After a call to this routine, the input file is positioned just before
   the start of data.

   jnc 13/dec/12 RAC

   Modified to conform with SkySim - VRM 16/Dec/12

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../inc/sysdefs.h"
#include "../inc/corrdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/opt.h"


int get_corr_metadata (FILE *fpkt, CorrHdrType **corrhdrp,  MetaPktType **mpktp, PackBlineHdrType **packblinehdrp)
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
      fprintf (finfo, "%s Found Packed spectral correlations data.\n", info);
  }else{
    fprintf (ferr, "%s Data is of unrecognized type %s. Quitting.\n", err, DataType[corrhdr->datatype]); 
    exit(-1);
  }

  if (!((corrhdr->bits2pix == _32fcmplx)|| (corrhdr->bits2pix == _16scmplx))){
      fprintf (ferr, "%s Unknown datatype! Quitting!\n", err); 
      exit(-1);
  }
  
  fprintf (finfo, "%s BITS2PIX            : %-16s\n", info, BppDesc[corrhdr->bits2pix]);
  fprintf (finfo, "%s Datatype            : %-16s\n", info, DataType[corrhdr->datatype]);
  fprintf (finfo, "%s PktSize             : %d bytes\n", info, corrpktsize);
  fprintf (finfo, "%s RF signals          : %-16d\n", info, NChans[corrhdr->chans]);
  fprintf (finfo, "%s All baselines       : %-16d\n", info, corrhdr->blines);
  fprintf (finfo, "%s Taps                : %-16d\n", info, corrhdr->taps);
  fprintf (finfo, "%s Sampling frequency  : %f MHz\n", info, corrhdr->f_samp/1e6);

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
int get_next_corrrec(FILE *fpkt, int corrpktsize, CorrHdrType *corrhdr, ProjParType *proj, int *fits2ippmap){

  unsigned char *dbuf = NULL;
  Cmplx3Type    *out;
  float         *bline32fc,*in;
  int            b,b1,c,conjugate;    
  int            got;

  if ((dbuf=calloc (corrpktsize, 1)) == NULL){
    perror ("calloc"); 
    return -1; 
  }
  
  if ((got=fread (dbuf, corrpktsize, 1, fpkt)) != 1){
      fprintf(finfo, "%s Read all packets from corr file\n", info);
      //perror ("fread"); 
    return -1;
  }

  if (corrhdr->bits2pix != _32fcmplx){
      fprintf(ferr,"%s Can only handle corrhdr->bits2pix = _32fcmplx not %d\n", err, corrhdr->bits2pix);
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
    in = bline32fc + 2*b1*proj->chans;
    if(conjugate){
      for(c=0;c<proj->chans;c++){
	out->r=in[0];out->i=-in[1];
	in +=2; out++;
      }
    }else{
      for(c=0;c<proj->chans;c++){
	out->r=in[0];out->i=in[1];
	in +=2; out++;
      }
    }
  }

  return 0;
}

int corrmap(CorrHdrType *corrhdr, MetaPktType *mpkt, PackBlineHdrType *packblinehdr, ProjParType *proj,
	    int *fits2corrmap)
{ int    a,b,b1;
  char   name1[5],name2[5];
  char   cor_name1[5],cor_name2[5];
  int    cor_ants;
  int    found,conjugate;

  if (mpkt->ort.arrmode != HalfMod){
    fprintf(ferr,"%s Unknown ORT.arrmode, can only handle HalfMod! Quitting.\n", err);
    return 1;
  }

  cor_ants=NChans[corrhdr->chans];
  /* check that the total number of baselines match! (SELFS HANDLED???)*/
  if(corrhdr->blines-cor_ants != proj->baselines){
      fprintf(ferr,"%s Baseline number mismatch! CORR(%d) FITSWRITE(%d)\n", err, corrhdr->blines-cor_ants,proj->baselines);
    return 1;
  }

  for(b=0;b<proj->baselines;b++){
    name1[0]=name2[0]='\0';
    for(a=0;a<proj->ants;a++){
      if(proj->Ant[a].id==proj->Base[b].id1)
	strcpy(name1,proj->Ant[a].name);
      if(proj->Ant[a].id==proj->Base[b].id2)
	strcpy(name2,proj->Ant[a].name);
    }
    if(!strlen(name1)||!strlen(name2)){
    fprintf(ferr,"%s Cannot find antennas corresponding to baseline %d!\n",err, b);
    return 1;
    }
    found=0;
    for (b1=0; b1<corrhdr->blines; b1++){
      strcpy(cor_name1,
	     HalfModule_Names[mpkt->ort.mod2chan[packblinehdr[b1].a0]]);
      strcpy(cor_name2,
	     HalfModule_Names[mpkt->ort.mod2chan[packblinehdr[b1].astar1]]);
      if(!strcmp(name1,cor_name1) && !strcmp(name2,cor_name2))
	{ found=1;conjugate=0;}
      if(!strcmp(name1,cor_name2) && !strcmp(name2,cor_name1))
	{ found=1;conjugate=1;}
      if(found) /* baseline map along with conjugation information */
	{ fits2corrmap[b]=2*b1+conjugate; break;}
    }
    if(!found){
	fprintf(ferr,"%s Cannot find CORR baseline corresponding to %s- %s\n", err, name1, name2);
      return 1;
    }
  }

  /* all done */
  return 0;
}









char *FITSFile=NULL;
char *CorrFile=NULL;
char Usage[256];
int BigEndian    = 0 ;
int LittleEndian = 0 ;


int corr2fits(int argc, char **argv )
{ int               preint = 1;
  int               status = 0;
  FILE             *fpkt;
  fitsfile         *fvis;
  ProjParType       proj;
  CorrHdrType      *corrhdr;
  MetaPktType      *mpkt;
  PackBlineHdrType *packblinehdr;
  int              *fits2corrmap;
  long              gcount = 0,corrpktsize=0, rec;

  if( FITSFile == NULL || CorrFile==NULL )
  {
  fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
  fprintf(stderr, "\"%s --help\" for self-explanatory documentation.\n", argv[0]);
  exit(-1);
  }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 create_fits( &fvis, FITSFile);
 if((fpkt=fopen(CorrFile,"rb"))==NULL)
 { fprintf(ferr, "%s Cannot open %s!\n", err, CorrFile);
   exit(-1);
 }

 proj.nsrc = 1;
 proj.preint = preint;
 init_proj(&proj); 
 init_hdr(fvis, &proj);
 
 if((fits2corrmap=(int*)malloc(proj.baselines*sizeof(int)))==NULL)
 { fprintf(ferr, "%s Cannot find memory for FITS2Corr map\n", err);
   exit(-1);
 }

 corrpktsize=get_corr_metadata (fpkt, &corrhdr,&mpkt,&packblinehdr);
 if(corrpktsize<0)  return 1;
 if(corrmap(corrhdr,mpkt, packblinehdr, &proj, fits2corrmap))
   return 1;
 
 genVis( &proj );
 rec=0;

 while(get_next_corrrec(fpkt,corrpktsize,corrhdr,&proj,fits2corrmap)==0){
   gcount = writeVis2FITS(fvis, &proj);
   genVis(&proj);
   rec++;
 }

 fits_update_key_lng(fvis, "GCOUNT", gcount, "number of random groups", &status);
 write_EXtabs( fvis, &proj );
 
 close_fits( fvis );
 kill_proj( &proj );
 fprintf(finfo,"%s Converted %ld records", info, rec);
 done();

 return 0;

}

int main(int argc, char **argv )
{
  optrega(&FITSFile, OPT_STRING, 'o', "out" , "Output FITS File"             );
  optrega(&CorrFile, OPT_STRING, 'i', "in" ,  "Input IPPCorr File"           );
  sprintf(Usage, "\n-i Input IPPCorrFile -o OutputFITSFile");
  optUsage(Usage);
  optTitle("Generates UV FITS from an IPPCorr file\n");
  optMain(corr2fits);
  opt(&argc,&argv);

  return corr2fits(argc, argv);
}
