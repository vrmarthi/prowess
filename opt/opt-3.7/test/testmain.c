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
/* testmain.c */
/* $Id: testmain.c,v 3.1 1997/10/26 15:46:06 jt Exp $ */
/* Test and/or Show-off how the new optMain() works.
 */

#include <stdio.h>
#include <opt.h>

long N=1;

int oldmain(int argc, char **argv)
{
    int i;

    if (N < argc) {
        printf("Truncated to %ld words:\n",N);
    }
    printf("<%s> ",argv[0]);
    for (i=1; i<argc && i<=N; ++i) {
            printf(" [%s]",argv[i]);
    }
    printf("\n");
    return OPT_OK;
}

main(int argc, char **argv)
{
    optreg(&N,OPT_LONG,'N',"Max number of arguments echoed");
    optMain(oldmain);
    opt(&argc,&argv);
    return oldmain(argc,argv);
}



