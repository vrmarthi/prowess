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
/* testopt.c */

#include <stdio.h>
#include <stdlib.h>
#include <opt.h>

int month=4;
int day=24;
int year=1993;
int julian=0;
char *who=NULL;

int go(int argc, char **argv)
{
    if (argc>1) {
        printf("In program %s, Extra option: %s\n",argv[0],argv[1]);
    }

    if (optinvoked(&month)) {
        printf("User set month...\n");
    }

    if (month == 9 && day == 11 && year == 1989) {
        printf("Happy birthday, Max\n");
    } else {
        printf("Hello, %s: %4d/%02d/%02d %s\n",(who==NULL ? "world" : who),
               year,month,day,(julian ? "(Julian)" : ""));
    }
    return OPT_OK;
}

int checkyear(void *v)
{
    if (year == 2000) {
        printf("Watch out for that year=2000 bug!\n");
        return OPT_ERROR;
    }
    return OPT_OK;
}
int quit()
{
    printf("Bye...\n");
    return OPT_OK;
}
int write_version()
{
    printf("Version XXX\n");
    optExitNumber(12);
    return OPT_EXIT;
}
int fix_mon(void *v)
{
    int m;
    /* fix whatever int variable v is pointing to */
    m = *((int *)v);
    if (m < 1 || m > 12) 
        m=1;
    *((int *)v) = m;
    return OPT_OK;
} 

main(int argc, char **argv)
{
    optreg(&month,OPT_INT,'m',"Month");
    optlongname(&month,"month");
    opthook(&month,fix_mon);

    optrega(&day,OPT_INT,'d',"day","Day");
    opthelp(&day,"Use day of month, should be less than 32");
    
    optreg_INT(&year,'y',"Year");
    optreg(&year,OPT_INT,'Y',"Year");
    optdescript(&year,"What Year");
    opthook(&year,checkyear);

    optregs(&julian,OPT_FLAG,"julian");

    optreg(&who,OPT_UNDELIM,'\0',"Who to say hello to");

    optexec("version",write_version,"Write version number and exit");

    optEnvVarName( "OPT" );
    optDefaultFile( "testoptrc" ); /* in practice, you do ~/.testoptrc */
    
    optMain(go);
    optQuit(quit);

    opt(&argc,&argv);
    return go(argc,argv);
}



