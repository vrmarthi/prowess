/* OPT v3.7: options parsing tool */
/*
 * Copyright (C) 1998, James Theiler (jt@lanl.gov)
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 * 
 */
/* testtypes.c */

#include <stdio.h>
#include <opt.h>

int aint = 1;
unsigned int buint = 2;
short cshort = 3;
long dlong = 4;
char echar = '5';
int flevel = 6;
float ffloat = 7.0;
double gdouble = 8.0;
int hflag = OPT_TRUE;
int inegflag = OPT_FALSE;
int jabsflag = OPT_TRUE;
int kabsnegflag = OPT_FALSE;
char *lvstring = NULL;
char mcstring[OPT_MAXSTRLEN];

unsigned short nushort=32769;      /* == 2^15+1 */
unsigned long oulong=2150000000UL; /* just larger than 2^31 */

int oldmain(int argc, char **argv)
{
    printf("a=%d, b=%u, c=%hd, d=%ld, e=[%c]\n",
           aint,buint,cshort,dlong,echar);
    printf("n=%hu, o=%lu\n",nushort,oulong);
    return OPT_OK;
}

main(int argc, char **argv)
{
    optreg_INT(&aint,'a',"int");
    optreg_UINT(&buint,'b',"unsigned int");
    optreg_SHORT(&cshort,'c',"short");
    optreg_LONG(&dlong,'d',"long");
    optreg_CHAR(&echar,'e',"char");
    optreg_INTLEVEL(&flevel,'F',"int level");
    optreg_FLOAT(&ffloat,'f',"float");
    optreg_DOUBLE(&gdouble,'g',"double");
    optreg_FLAG(&hflag,'h',"flag");
    optreg_ABSFLAG(&jabsflag,'j',"abs flag");
    optreg_ABSNEGFLAG(&kabsnegflag,'k',"abs neg flag");
    /* Note: VSTRING is now deprecated in favor of STRING */
    optreg_STRING(&lvstring,'l',"string");

    /* NOTE: the CSTRING is the only type in which the
     * actual variable name is used instead of a pointer
     * to the variable name!  However, on many systems,
     * &mcstring and mcstring are identical, so it doesn't
     * really matter.!
     */
    optreg_CSTRING(mcstring,'m',"c-string");

    optreg_USHORT(&nushort,'n',"unsigned short");
    optreg_ULONG(&oulong,'o',"unsigned long");
    
    optMain(oldmain);

    opt(&argc,&argv);

    return oldmain(argc,argv);
}
