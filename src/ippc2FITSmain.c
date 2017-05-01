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
#include "../inc/fitsdefs.h"
#include "../inc/corrdefs.h"
#include "../inc/opt.h"

void printpar_baseline(ProjParType *proj)
{
 int ant1, ant2 ;
 int i = 0 ;
 BaseParType *base = proj->Base;
 AntParType *ant = proj->Ant;
 int num_ants = proj->ants;

 FILE *flog;
 flog = fopen("baselines.info", "w");
 fprintf(finfo, "%s Baselines in %s/baselines.info\n", info, getenv("PWD") );
 fprintf(flog, "###\tAnt1\tAnt2\tFITSbl\tAnt1\tAnt2\n###\n");
 i = 0;
 for(ant1 = 0 ; ant1 < num_ants ; ant1++)
 { for(ant2 = ant1+1 ; ant2 < num_ants ; ant2++)
   { fprintf(flog, "%3d\t%2d\t%2d\t%6d\t%s\t%s\n", i+1, base[i].id1, base[i].id2, base[i].number, base[i].ant1, base[i].ant2);
     i++ ;
   }
 }
 fprintf(finfo, "%s Antennas            : %d\n", info, proj->ants);
 proj->redundant_baselines = count_redundant_baselines( proj );
 fprintf(finfo, "%s Baselines           : %d\n", info, proj->baselines);
 fprintf ( finfo, "%s Unique baselines    : %d\n", info, proj->unique_baseline[0]);
 fprintf ( finfo, "%s Redundant baselines : %d\n", info, proj->redundant_baselines );


 fclose(flog);
}

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
int get_next_ippcrec(FILE *fpkt, int corrpktsize, CorrHdrType *corrhdr,  
		 ProjParType *proj, int *fits2ippmap){

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

char *FITSFile=NULL;
char *IPPCFile=NULL;
char Usage[256];


int BigEndian    = 0 ;
int LittleEndian = 0 ;

int ippcmap(CorrHdrType *corrhdr, MetaPktType *mpkt,   
	    PackBlineHdrType *packblinehdr, ProjParType *proj,
	    int *fits2ippcmap)
{ int    a,b,b1;
  char   name1[5],name2[5];
  char   ipp_name1[5],ipp_name2[5];
  int    ipp_ants;
  int    found,conjugate;

  if (mpkt->ort.arrmode != HalfMod){
    fprintf(stderr,"Unknown ort.arrmode, can only handle HalfMod!\n");
    return 1;
  }

  ipp_ants=NChans[corrhdr->chans];
  /* check that the total number of baselines match! (SELFS HANDLED???)*/
  if(corrhdr->blines-ipp_ants != proj->baselines){
    fprintf(stderr,"Baseline number mismatch! IPPCORR(%d) FITSWRITE(%d)\n",
	    corrhdr->blines-ipp_ants,proj->baselines);
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
      fprintf(stderr,"Can't find antennas corresponding to baseline %d!\n",b);
      return 1;
    }
    found=0;
    for (b1=0; b1<corrhdr->blines; b1++){
      strcpy(ipp_name1,
	     HalfModule_Names[mpkt->ort.mod2chan[packblinehdr[b1].a0]]);
      strcpy(ipp_name2,
	     HalfModule_Names[mpkt->ort.mod2chan[packblinehdr[b1].astar1]]);
      if(!strcmp(name1,ipp_name1) && !strcmp(name2,ipp_name2))
	{ found=1;conjugate=0;}
      if(!strcmp(name1,ipp_name2) && !strcmp(name2,ipp_name1))
	{ found=1;conjugate=1;}
      if(found) /* baseline map along with conjugation information */
	{ fits2ippcmap[b]=2*b1+conjugate; break;}
    }
    if(!found){
      fprintf(stderr,"Cannot find IPP baseline corresponding to %s- %s\n",
	      name1,name2);
      return 1;
    }
  }

  /* all done */
  return 0;
}


int ippc2fits(int argc, char **argv )
{ int              preint = 1;
  int              status = 0;
  FILE            *fpkt;
  fitsfile        *fvis;
  ProjParType      proj;
  CorrHdrType      *corrhdr;
  MetaPktType      *mpkt;
  PackBlineHdrType *packblinehdr;
  int             *fits2ippcmap;
  long             gcount = 0,corrpktsize=0, rec;

  if( FITSFile == NULL || IPPCFile==NULL ){
   fprintf(stderr, "Usage : %s %s\n", argv[0], Usage );
   return 1;
 }

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 create_fits( &fvis, FITSFile);
 if((fpkt=fopen(IPPCFile,"rb"))==NULL){perror(IPPCFile);return 1;}

 proj.nsrc = 1;
 proj.preint = preint;
 init_proj(&proj); 
 init_hdr(fvis, &proj);
 
 if((fits2ippcmap=(int*)malloc(proj.baselines*sizeof(int)))==NULL){
   perror("malloc"); return 1;
 }

 corrpktsize=get_ippc_metadata (fpkt, &corrhdr,&mpkt,&packblinehdr);
 if(corrpktsize<0)  return 1;
 if(ippcmap(corrhdr,mpkt, packblinehdr, &proj,fits2ippcmap))
   return 1;
 
 genVis( &proj );
 rec=0;
 while(get_next_ippcrec(fpkt,corrpktsize,corrhdr,&proj,fits2ippcmap)==0){
   gcount = writeVis2FITS(fvis, &proj);
   genVis(&proj);
   rec++;
 }

 fits_update_key_lng(fvis, "GCOUNT", gcount, "number of random groups", &status);
 write_EXtabs( fvis, &proj );
 
 close_fits( fvis );
 kill_proj( &proj );
 fprintf(finfo,"%s Converted %ld records\n", info, rec);
 done();

 return 0;

}

int main(int argc, char **argv )
{
  optrega(&FITSFile, OPT_STRING, 'o', "out" , "Output FITS File"             );
  optrega(&IPPCFile, OPT_STRING, 'i', "in" ,  "Input IPPCorr File"           );
  sprintf(Usage, "\n-i Input IPPCorrFile -o OutputFITSFile");
  optUsage(Usage);
  optTitle("Generates UV FITS from an IPPCorr file\n");
  optMain(ippc2fits);
  opt(&argc,&argv);

  return ippc2fits(argc, argv);
}
