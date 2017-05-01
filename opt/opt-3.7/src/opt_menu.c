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
/* $Id: opt_menu.c,v 3.1 1997/10/24 22:35:08 jt Exp $ */
/* opt_menu.c */


#ifndef	SINGLEFILE
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef convex
#include <strings.h>
#else
#include <string.h>
#endif
#ifdef __TURBOC__
#include <alloc.h>
#endif
#include "opt.h"
#include "ag.h"
#include "opt_p.h"
#endif /* SINGLEFILE */

/* --------------------- menu flag ------------------- */

flag menuflag=False;

static char mgstring[160];	/* global menu string */

#define menu_wr_string(str)       opt_message(str)
#define menu_getresponse(respond) opt_getline(respond,stdin)

static void write_the_menu(int iopfrom, int iopto);
static int auto_prefix_delim(char *r);


/* ----------------------------------------------------	*/
/*  opt_menu:	Get options from an interactive menu	*/
/* ----------------------------------------------------	*/

#define	MAXRESPONDLINE	280
#ifndef	MAXOPTSINMENU
#define	MAXOPTSINMENU	20
#endif
#define MENUPROMPT "-> "

void
opt_menu(void)
{
    char respond[MAXRESPONDLINE+2];
    static int maxoptsinmenu=MAXOPTSINMENU;
    int	iopfrom,iopto;

    menuflag=True;	/* turn on MENUFLAG in case it is not set already */
    
    iopfrom = 0;
    iopto = ( opt_nreg < maxoptsinmenu ? opt_nreg : maxoptsinmenu );
    
    respond[0]='\0';

    opt_mess_1("%s\n",optgetTitle());
    write_the_menu(iopfrom,iopto);

    while( menuflag ) {
        opt_menu_getresponse(MENUPROMPT,respond,MAXRESPONDLINE);
		
        switch(*respond) {
        case ADDITIONAL_OPTS:
            if( respond[1] != '\0' && respond[1] != ' ' ) {
                maxoptsinmenu = atoi(respond+1);
                if(maxoptsinmenu < 1)
                    maxoptsinmenu = opt_nreg;
                sprintf(mgstring,"Scroll %d options\n",maxoptsinmenu);
                menu_wr_string(mgstring);
                iopfrom = 0;
                iopto = 
                    ( opt_nreg < maxoptsinmenu ? opt_nreg : maxoptsinmenu );
            } else {
                iopfrom += maxoptsinmenu;
                if( iopfrom > opt_nreg)
                    iopfrom = 0;
                iopto = iopfrom + maxoptsinmenu;
                if( iopto > opt_nreg )
                    iopto = opt_nreg;
            }
            write_the_menu(iopfrom,iopto);
            break;
        case INTERACT:
            menuflag=False;
            break;
        case BANG:
            system( respond+1 );
            break;
        case '\0':
            write_the_menu(iopfrom,iopto);
            break;
        case QUITCH:
            /* Only quit if the QUITCH is followed by whitespace.  In
             * other words, if respond = '.m=5', don't quit.  However,
             * note that respond = '. m=5' will cause a quit.
             */
            if ( respond[1]=='\0' || respond[1]==' ' )
                opt_quit();
            else
                opt_mess_1("Invalid line: [%s]\n",respond);
            break;
        case DELIM:
#ifdef PERMIT_ALTDELIM            
        case ALTDELIM:
#endif            
            opt_lineprocess(respond);
            break;
        default:
            auto_prefix_delim(respond);
            opt_lineprocess(respond);
            break;
        }
    }
}/* opt_menu */


/**********
 * write_the_menu:
 *	write the menu including options from iopfrom to iopto.
 */
static void
write_the_menu(int iopfrom, int iopto)
{
    int iop;
    flag fullmenu;

    fullmenu = ((iopfrom==0 && iopto==opt_nreg) ? True : False );

    if( !fullmenu ) {
        sprintf(mgstring,"menu: %d->%d [%d]\n",iopfrom,iopto,opt_nreg);
        menu_wr_string(mgstring);
    }

    for(iop=iopfrom; iop<iopto; ++iop) {
        char *s;
        s = opt_mstring(iop);
        if (s!=NULL) {
            strcpy( mgstring, s );
            menu_wr_string(mgstring);
            menu_wr_string("\n");
        }
    }
    if (!fullmenu) {
        sprintf(mgstring,"%c Additional options\n",ADDITIONAL_OPTS);
        menu_wr_string(mgstring);
    }
    sprintf(mgstring,"(Type %c for Help)\n",HELPCH);
    menu_wr_string(mgstring);
}

/*	auto_prefix_delim:	
 *		this is a fru-fru piece of code that automatically
 *		sticks a DELIM character onto the front of a string
 *		in cases where it imagines that that is what the user
 *		really meant.  Thus
 *		-> m4
 *		gives the same effect as
 *		-> -m4
 *		But be warned that this only applies in limited cases.
 *		Eg.,
 *		-> m4 b3
 *		is not the same as
 *		-> -m4 -b3
 *
 *		a '-' will be prepended in the case that 
 *		the first character is a registered name
 */
static int
auto_prefix_delim(char *r)
{
    if( opt_char_number( *r ) != -1 ) {
        int len;
        len = strlen(r)+1;	/* +1 since double terminated */
        while(len>=0) {
            r[len+1]=r[len];
            --len;
        }
        r[0]=DELIM;
        return(1);
    }
    else
        return(0);
}/* auto_prefix_delim */


