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
/* $Id: opt_reg.c,v 3.12 1998/07/26 22:25:03 jt Exp $ */
/* opt_reg.c */

/************************ register options ********************************
 *
 *    options are stored in an array,
 *    each element of which is a structure
 *
 *    The members of the structure contain information about
 *        1) the variable which is altered
 *        2) the type of variable
 *        3) a character by which the variable is invoked
 *        4) a string by which the variable can be invoked
 *        5) a brief description of the variable's role in the program
 *        6) a longer help message
 *        7) a flag indicating that an option has been invoked
 *        7) a "hook", a function that is called when variable is invoked
 *            
 *    The structure elements are not assigned directly, but are
 *    "registered" with a call to the function optreg().
 *        
 *    In this file are the routines which access the structure of
 *    registered options.
 *
 *          I) Register options 
 *         II) Get number corresponding to name of option
 *        III) Print options out
 *             a) to a file which can be used as an input command line
 *             b) to the screen as part of a usage message
 *             c) to the screen as part of an interactive menu
 *         IV) Process single delimited and undelimited options
 */

#ifndef	SINGLEFILE
#include <stdio.h>
#include <stdlib.h>
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
#endif

/* Default is to indicate whether a flag is true or false by using + or -
 * But, by #define'ing FLAG_ZERO_ONE, we will use 1 or 0 instead
 */
#ifdef FLAG_ZERO_ONE
#define FLAG_TRUE '1'
#define FLAG_FALSE '0'        
#else
#define FLAG_TRUE '+'
#define FLAG_FALSE '-'        
#endif

/* Prototypes for static (local) functions */
static void opt_fileheader(FILE *);
static int opt_action(int, ARGVECTOR *);
static int opt_number(void *v);
static int opt_longname_number(char *);
#if 0
static int opt_valid_longname(char *);
#endif

static char blankstringval = '\0';
#define BLANKSTRING (&blankstringval) /* non-null blank string */
#define ISVALIDNUMBER(n) ((n)>=0 && (n)<opt_nreg)

int opt_exit_number=0;          /* if we have to exit(), then exit
                                 * with this number.
                                 */

        /* -------------------------- */
        /* List of Registered Options */
        /* -------------------------- */

#define MAXOPTS 256             /* sort of an arbitrary limitation! */

typedef struct {
    void      *value;         /* pointer to value of option */
    opt_TYPE   type;          /* will be cast according to type */
    char       name;          /* name by which option is invoked */
    char      *longname;      /* long version of the name */
    char      *descript;      /* a brief description */
    char      *help;          /* longer help message */
    int        invoked;       /* flag is set to 1 if option is invoked */
    OPT_PFI    hook;          /* code to evaluate if option is invoked */
}   EACH_OPT;

static	EACH_OPT optlist[MAXOPTS];               /* array of options */
int              opt_nreg=0;                     /* number of registered opts */

    /* ---------- */
    /* OPT macros */
    /* ---------- */

#define	OPT_isflagtype(o) \
	((o)==OPT_FLAG ||    (o)==OPT_NEGFLAG || \
         (o)==OPT_ABSFLAG || (o)==OPT_ABSNEGFLAG)
#define	OPT_isundelimtype(o) ((o)==OPT_UNDELIM ||  \
                              (o)==OPT_UNDELIMC || \
                              (o)==OPT_UNDELIMV )

	/* -------------------------------------------------- */
	/*    OPTVALUE: a macro to get the value of an option */
	/* SETOPTVALUE: a macro to set the value of an option */
	/* -------------------------------------------------- */

#define	OPTVALUE(typ,i)		((typ)(*((typ *)(optlist[i].value))))

#define SETOPTVALUE(typ,i,val)  { typ *xptr;                     \
                                  xptr = (typ *)(optlist[i].value); \
                                  *xptr = val; }

        /* ------------------------------- */
        /* Routine to register each option */
        /* ------------------------------- */

int optreg(void *val, opt_TYPE otype, char name, char *descript) {
    return optrega(val,otype,name,NULL,descript);
}
int optregc(void *val, opt_TYPE otype, char name) {
    return optrega(val,otype,name,NULL,NULL);
}
int optregcb(void *val, opt_TYPE otype, char name, char *descript) {
    return optrega(val,otype,name,NULL,descript);
}
int optregs(void *val, opt_TYPE otype, char *longname) {
    return optrega(val,otype,'\0',longname,longname);
}
int optregsb(void *val, opt_TYPE otype, char *longname, char *descript) {
    return optrega(val,otype,'\0',longname,descript);
}
int optexec(char *longname, OPT_PFI fcn, char *descript)
{
    int n;
    n = optrega(NULL,OPT_NUL,'\0',longname,descript);
    opthook_n(n,fcn);
    return n;
}
     
int
optrega(void *val, opt_TYPE otype, char name, char *longname, char *descript)
{
    char *thename=BLANKSTRING;

    if (name != '\0') {
        thename = strdup(".");
        thename[0] = name;
        thename[1] = '\0';
    } else if (!ISEMPTYSTRING(longname)) {
        thename = strdup(longname);
    } else if (!ISEMPTYSTRING(descript)) {
        thename = strdup(descript);
    }
    
    if( opt_nreg >= MAXOPTS )
        opt_fatal("optrega: too many options");
	
    if( name != '\0' && opt_char_number(name) != -1 ) {
        opt_warn_1("optrega: Duplicate option name \'%c\'",name);
    }
    /* This is probably a mistake to print out a warning message,
       since it will be printed every time the program is run.
       Perhaps it should only be run if the user types 'run --testopt'
       */
#if 0    
    if( val != NULL  && opt_val_number(val) != -1) {
        opt_warn_1("optrega: This variable [%s] has already been registered",
                   thename);
    }
#endif    
    
    /* Programmer may specify that an option is to be an
     * undelimted option by setting the name to '\0'
     */
    if(name=='\0' && ISEMPTYSTRING(longname)) {
        if(otype==OPT_STRING)  otype=OPT_UNDELIM;
        if(otype==OPT_VSTRING) otype=OPT_UNDELIM;
        if(otype==OPT_CSTRING) otype=OPT_UNDELIMC;
    }

    /* Having checked for various warnings, now register the options */

    optlist[opt_nreg].value         = val;
    optlist[opt_nreg].type          = otype;
    optlist[opt_nreg].name          = name;
    optlist[opt_nreg].longname      = longname;
    optlist[opt_nreg].descript      = descript;
    optlist[opt_nreg].help          = NULL;
    optlist[opt_nreg].hook          = NULL;
    optlist[opt_nreg].invoked       = 0;
    
    /* UN-delimited strings are set to NULL when registered */
    /* This enables them to be invoked on the command line  */
    
    if (val != NULL) {
        if(otype==OPT_UNDELIMC) *((char *)optlist[opt_nreg].value)='\0';
        if(otype==OPT_UNDELIM ||
           otype==OPT_UNDELIMV) SETOPTVALUE(char *,opt_nreg,NULL);
    }
    ++opt_nreg;
    return opt_nreg-1;

}
void optchar_n(int n, char name)
{
    if (ISVALIDNUMBER(n)) optlist[n].name = name;
}    
void optdescript_n(int n, char *descript)
{
    if (ISVALIDNUMBER(n)) optlist[n].descript = strdup(descript);
}
void opthelp_n(int n, char *help)
{
    if (ISVALIDNUMBER(n)) optlist[n].help = strdup(help);
}
void optlongname_n(int n, char *longname)
{
    if (ISVALIDNUMBER(n)) optlist[n].longname = strdup(longname);
}
void opthook_n(int n, OPT_PFI hook)
{
    if (ISVALIDNUMBER(n)) optlist[n].hook = hook;
}
int optinvoked_n(int n)
{
    if (ISVALIDNUMBER(n))
        return optlist[n].invoked;
    return 0;
}

int optinvoked(void *v) { return optinvoked_n( opt_number(v) ); }

void optchar(void *v,  char name) { optchar_n( opt_number(v),   name ); }
void opthelp(void *v,  char *help) { opthelp_n( opt_number(v),  help ); }
void opthook(void *v,  OPT_PFI hook) { opthook_n( opt_number(v), hook ); }      
void optlongname(void *v, char *longname)
{
    optlongname_n( opt_number(v), longname );
}
void optdescript(void *v, char *descript)
{
    optdescript_n( opt_number(v), descript );
}
void optExitNumber(int n) {
    opt_exit_number = n;
}


#if 0
/* opt_valid_longname: currently not used, but we should make sure
 * there are no whitespaces or funny characters
 */
static int opt_valid_longname(char *name)
{
    
    return 1;
}
#endif

void
opt_get_help(char c)
{
    int n;
    n = opt_char_number(c);
    if (ISVALIDNUMBER(n) && optlist[n].help != NULL)
        opt_mess_2("%c: %s\n",c,optlist[n].help);
    else
        opt_mess_1("Help unavailable for \'%c\'\n",c);
}
	
	

/* opt_XXX_number(): Get number corresponding to option name; return
 * a value of -1 if the option does not exist.
 *  opt_char_number(c) takes as input the single-character name;
 *  opt_longname_number(longname) takes as input the long name
 *  opt_number(void *) takes as input pointer to variable
 */

int
opt_char_number(char c)
{
    int i;                         /* see which registered option */
    for(i=0; i<opt_nreg; ++i) {
        if( c == optlist[i].name )
            return i;
    }
    return -1;
}
    
static int
opt_number(void *v)
{
    int i;                         /* see which registered option */
    for(i=0; i<opt_nreg; ++i) {
        if( v == optlist[i].value )
            return i;
    }
    return -1;                     /* to signify not an option */
}
static int
opt_longname_number(char *s)
{
    int i;                         /* see which registered option */
    if ( ISEMPTYSTRING(s) ) return -1;
    for(i=0; i<opt_nreg; ++i) {
        if ( !ISEMPTYSTRING(optlist[i].longname) ) {
            if (0==strcmp(s,optlist[i].longname)) {
                return i;
            }
        }
    }
    return -1;                     /* to signify not an option */
}




	/* -------------------------------------------- */
	/* Print value of registered option to a string */
	/* -------------------------------------------- */

/* optstrval:
 * returns the value of the ith registered option as a string
 * Thus if x = 12.6, this returns the string "12.6"
 * The return value is a pointer to a static string which is
 * overwritten with each call
 * FLAG values are returned as "+" or "-"
 * INTLEVEL values are returned as "- -xxxx..." depending on value
 */
char *
optstrval(int i)
{
    int maybe;
    static char stval_buf[80];
    
    switch( optlist[i].type ) {
    case OPT_INT:
        sprintf(stval_buf,"%d", OPTVALUE(int,i) );
        break;
    case OPT_UINT:
    case OPT_UNSINT:
        sprintf(stval_buf,"%u", OPTVALUE(unsigned int,i) );
        break;
    case OPT_SHORT:
        sprintf(stval_buf,"%d", OPTVALUE(short,i) );
        break;
    case OPT_USHORT:
        sprintf(stval_buf,"%u", OPTVALUE(unsigned short,i) );
        break;
    case OPT_LONG:
        sprintf(stval_buf,"%ld", OPTVALUE(long,i) );
        break;
    case OPT_ULONG:
        sprintf(stval_buf,"%lu", OPTVALUE(unsigned long,i) );
        break;
    case OPT_CHAR:
        sprintf(stval_buf,"%c", OPTVALUE(char,i));
        break;
    case OPT_UCHAR:
        sprintf(stval_buf,"%c", OPTVALUE(unsigned char,i));
        break;
    case OPT_FLOAT:
        sprintf(stval_buf,"%g", OPTVALUE(float,i) );
        break;
    case OPT_DOUBLE:
        sprintf(stval_buf,"%g", OPTVALUE(double,i) );
        break;
    case OPT_INTLEVEL:
        if (1) {
            int j;
            char buff[80];
            strcpy(stval_buf,"-"); 		/* Begin with level==OFF */
            for(j=0; j< OPTVALUE(int,i); ++j) {
                sprintf(buff,(j==0?" -%c":"%c"),optlist[i].name);
                strcat(stval_buf,buff);
            }
        }
        break;
    case OPT_FLAG:
    case OPT_ABSFLAG:
        maybe = OPTVALUE(int,i);
        sprintf(stval_buf,"%c", (maybe==OPT_TRUE ? FLAG_TRUE : FLAG_FALSE) );
        break;
    case OPT_NEGFLAG:
    case OPT_ABSNEGFLAG:
        maybe = OPTVALUE(int,i);
        sprintf(stval_buf,"%c", (maybe==OPT_TRUE ? FLAG_FALSE : FLAG_TRUE) );
        break;
    case OPT_CSTRING:    
    case OPT_UNDELIMC:
        /* OPTVALUE(char *,i) macro doesn't work for CSTRING's */
        sprintf(stval_buf,"\"%s\"",(char *)optlist[i].value);
        break;
    case OPT_STRING:
    case OPT_VSTRING:
    case OPT_UNDELIM:
    case OPT_UNDELIMV:
        if( optlist[i].value == NULL )	/* this should never happen */
            sprintf(stval_buf,"\"\"");
        else 
            if( OPTVALUE(char *,i) == NULL )
                sprintf(stval_buf,"\"\"");
            else
                sprintf(stval_buf,"\"%s\"",OPTVALUE(char *,i));
        break;
    case OPT_NUL:
        break;
    default:
        opt_fatal("optstrval: Undefined o-type");
        break;
    }
    /* ---- Return value is static buffer ---- */
    return stval_buf;
}/*optstrval*/


/* optstrtyp:
 * returns the type of the ith registered option as a string
 * Thus if x = 12.6 is a float , this returns the string "<real>"
 * The return value is a pointer to a static string which is
 * overwritten with each call
 */
char *
optstrtyp(int i)
{
    static char	sttyp_buf[80];

    switch( optlist[i].type ) {
        /* Integers */
    case OPT_INT:         strcpy(sttyp_buf,"<INT>"); break;
    case OPT_UNSINT:      /* deprecated, same as UINT */
    case OPT_UINT:        strcpy(sttyp_buf,"<UINT>"); break;
    case OPT_SHORT:       strcpy(sttyp_buf,"<SHORT>"); break;
    case OPT_LONG:        strcpy(sttyp_buf,"<LONG>"); break;
    case OPT_USHORT:      strcpy(sttyp_buf,"<USHORT>"); break;
    case OPT_ULONG:       strcpy(sttyp_buf,"<ULONG>"); break;
        /* Floating point */
    case OPT_FLOAT:       strcpy(sttyp_buf,"<FLOAT>"); break;
    case OPT_DOUBLE:      strcpy(sttyp_buf,"<DOUBLE>"); break;
        /* Char */
    case OPT_CHAR:        strcpy(sttyp_buf,"<CHAR>"); break;
    case OPT_UCHAR:       strcpy(sttyp_buf,"<UCHAR>"); break;
        /* Flag */
    case OPT_INTLEVEL:    strcpy(sttyp_buf,"<INTLEVEL>"); break;
    case OPT_FLAG:        strcpy(sttyp_buf,"<FLAG>"); break;
    case OPT_NEGFLAG:     strcpy(sttyp_buf,"<NEGFLAG>"); break;
    case OPT_ABSFLAG:     strcpy(sttyp_buf,"<ABSFLAG>"); break;
    case OPT_ABSNEGFLAG:  strcpy(sttyp_buf,"<ABSNEGFLAG>"); break;
        /* String */
    case OPT_STRING:      strcpy(sttyp_buf,"<STRING>"); break;
    case OPT_VSTRING:     strcpy(sttyp_buf,"<STRING>"); break;
    case OPT_CSTRING:     strcpy(sttyp_buf,"<STRING>"); break;
        /* Undelimited */
    case OPT_UNDELIM:     strcpy(sttyp_buf,"<UNDELIM>"); break;
    case OPT_UNDELIMV:    strcpy(sttyp_buf,"<UNDELIM>"); break;
    case OPT_UNDELIMC:    strcpy(sttyp_buf,"<UNDELIM>"); break;
        /* Nul */
    case OPT_NUL:         strcpy(sttyp_buf,"<NUL>"); break;
        
    default:
        opt_fatal("usage: undefined o-type");
    }
    return sttyp_buf;
}

		/* ------------------------------ */
		/* Put registered options to file */
		/* ------------------------------ */

void
opt_tofile(FILE *fp)
{
    int	i;
    char *format;
    /* Short Option Names */
    char *fnumstr="-%c %-38s %c%s\n";	/* numbers and strings  */
    char *fflgchr="-%c%-38s  %c%s\n";	/* flags and characters */
    
    opt_fileheader(fp);
    /* Short Option Names First */
    for(i=0; i<opt_nreg; ++i) {
        if (optlist[i].name == '\0')
            continue;
        if( OPT_isundelimtype( optlist[i].type ) )
            continue;
        if (optlist[i].type == OPT_NUL)
            continue;
        if( OPT_isflagtype( optlist[i].type )
            || optlist[i].type == OPT_INTLEVEL
            || optlist[i].type == OPT_CHAR )
            format = fflgchr;
        else
            format = fnumstr;
        fprintf(fp,format,optlist[i].name,
                optstrval(i),IGNOREEOL,optlist[i].descript);
        /* If an option has both a short and a long option name
         * then write it as a comment '    ; -b == --option-b
         */
        if (optlist[i].longname) {
            fprintf(fp,"%-42s%c -%c == --%s\n","",IGNOREEOL,
                    optlist[i].name,optlist[i].longname);
        }
    }
    /* Now Long Option Names (thanks to Jason V. Morgan) */
    for(i=0; i<opt_nreg; ++i) {
        /* do not output the long name if a short name exists */
        if (optlist[i].name != '\0')
           continue;
        if (!optlist[i].longname) 
           continue;

        /* The following arguably applies to short options too ?? */
        /* don't want anything with a hook (do we?) */
        if (optlist[i].type == OPT_NUL)
            continue;
        if (optlist[i].hook != NULL)
            continue;
        
        fprintf(fp, "--%s=%-*s %c%s\n", optlist[i].longname, 
                38 - strlen(optlist[i].longname), optstrval(i), 
                IGNOREEOL, optlist[i].descript);
    }
}
static void
opt_fileheader(FILE *fp)
{
    extern char *opt_program_name;
    int	i,n;
    char buf[80];

    sprintf(buf,"Options file created by: [%.30s]",opt_program_name);
    n = strlen(buf);

    fputc(IGNOREEOL,fp);
    fprintf(fp,"%s",buf);
    fputc(IGNOREEOL,fp); fputc('\n',fp);

    fputc(IGNOREEOL,fp);
    for(i=0;i<n;++i) fprintf(fp,"-");
	fputc(IGNOREEOL,fp); fputc('\n',fp);
}
		/* ----------------------------------------- */
		/* write options out to the interactive menu */
		/* ----------------------------------------- */

char *
opt_mstring(int i)
{
    static char mstring[180];
    char	flgstr[10];
    char	*s;
    char	*optstrval(int i);
    char	*format = "%c %-40s %s";
    char        *lformat = "  --%s %*s%s";

    if( OPT_isundelimtype( optlist[i].type ) )
		return NULL;

    s = optstrval(i);
    if( optlist[i].type == OPT_INTLEVEL ) {
        int f;
        f = OPTVALUE(int,i);
        if(f==0)
            strcpy(flgstr,"OFF");
        else if (f==1)
            strcpy(flgstr,"ON");
        else
            sprintf(flgstr,"ON:%d",f);
        s=flgstr;
    } else if( optlist[i].type == OPT_CHAR ) {
        switch (s[0]) {
        case '\0':	sprintf(flgstr,"\'\\0\'"); break;
        case '\n':	sprintf(flgstr,"\'\\n\'"); break;
        case '\t':	sprintf(flgstr,"\'\\t\'"); break;
        case ' ':	sprintf(flgstr,"\'  \'");  break;
        default:	sprintf(flgstr,"%c",s[0]); break;
        }
        s = flgstr;
    } else if( OPT_isflagtype( optlist[i].type ) ) {
        if( *s == '+' || *s == '1')
            strcpy(flgstr,"TRUE");
        if( *s == '-' || *s == '0')
            strcpy(flgstr,"FALSE");
        s=flgstr;
    }

    /* This needs to be fancier, since we now have many combinarions
     * of long/short name'd options
     */
    if (optlist[i].name) {
        sprintf(mstring,format,optlist[i].name,optlist[i].descript,s);
    } else if (optlist[i].longname) {
        sprintf(mstring,lformat,optlist[i].longname,
                38 - strlen(optlist[i].longname),"",s);
    }
            
    return mstring;
}
int
opt_fprint(FILE *fp, int i)
{
    fprintf(fp,"-%c%-17s %c%s\n",optlist[i].name,
            optstrval(i),IGNOREEOL,optlist[i].descript);
    return i;
}

/* opt_usage: Write a usage statement describing registered options.
 * This is what the program writes in response to '--help' on the
 * command line.
 */

#define OPTUSAGEDEBUG 0

void
opt_usage(void)
{
    static char *dformat = " -%c                 %-12s    %s\n";
    static char *lformat=         " -%c, --%-12s %-12s    %s\n";
    static char *uformat =  "                    %-12s    %s\n";
    static char *aformat =         "     --%-12s %-12s    %s\n";
    int  i;
    
    for(i=0; i<opt_nreg; ++i) {
        if(OPTUSAGEDEBUG) fprintf(stderr,"%2d ",i);
        if( OPT_isundelimtype( optlist[i].type ) ) {
            if(OPTUSAGEDEBUG) fprintf(stderr,"u ");
            opt_mess_2(uformat,optstrtyp(i),optlist[i].descript);
        } else {
            if (optlist[i].name == '\0') {
                    if(OPTUSAGEDEBUG) fprintf(stderr,"a ");
                    opt_mess_3(aformat,optlist[i].longname,
                               optstrtyp(i),optlist[i].descript);
            } else {
                if (!ISEMPTYSTRING(optlist[i].longname)) {
                    if(OPTUSAGEDEBUG) fprintf(stderr,"l ");
                    opt_mess_4(lformat,optlist[i].name,optlist[i].longname,
                               optstrtyp(i),optlist[i].descript);
                } else {
                    if(OPTUSAGEDEBUG) fprintf(stderr,"d ");
                    opt_mess_3(dformat,optlist[i].name,
                               optstrtyp(i),optlist[i].descript);
                }
            }
        }
    }
    if (opt_additional_usage_fcn != NULL) {
        /* precede additional usage by -- to indicate that we
         * are finished with the options
         */
        opt_message(" --\n");
        /* return value is ignored */
        (*opt_additional_usage_fcn)();
    }
}

	/* ----------------------- */
	/* Get and process options */
	/* ----------------------- */

	/*
	 * The routines below make use of the ARGVECTOR structure
	 * defined in "ag.h" to process the command line options
	 */
		
	/* -------------------------------------- */
	/* get and process an UN-delimited option */
	/* -------------------------------------- */

int
opt_undelim(ARGVECTOR *ag)
{
    int     i;
    char    *s;
 
    /* Find the first undelimited option that has not been invoked and
     * set that to the argument string.  Note that once an undelimited
     * option has been invoked, it cannot be un-invoked.
     */

    s = strdup( ag_s_advance(ag) );
    for (i=0; i<opt_nreg; ++i) {
        if (optlist[i].invoked) continue;
        if (optlist[i].type==OPT_UNDELIMC ) {
            strncpy( optlist[i].value, s , OPT_MAXSTRLEN);
            optlist[i].invoked += 1;
            return 1; /* success */
        }
        if (optlist[i].type==OPT_UNDELIM ||
            optlist[i].type==OPT_UNDELIMV) {
            /** optlist[i].value = strdup(s); <-- this fails! **/
            SETOPTVALUE(char *,i,strdup(s));
            optlist[i].invoked += 1;
            return 1; /* success */
        }
    }
    return 0; /* failed to find an uninvoked option */
}

		/* ---------------------------------- */
		/* get and process a delimited option */
		/* ---------------------------------- */

int
opt_delim(ARGVECTOR *ag)
{
    int      i;
    char     c;
 
    c = ag_c_advance(ag);   /* first character gives name of option */
    i = opt_char_number(c);      /* this is number of option w/ name 'c' */
 
    if(! ISVALIDNUMBER(i)) { /* if invalid option name */
        opt_warn_1("%c not a registered option name\n",c);
        return 0;
    }
    return opt_action(i,ag);
}


int
opt_longdelim(ARGVECTOR *ag)
{
    int      i;
    char     c;
    char     *sc,*scptr;

    /* this code parses input options of the form --var=value */
    
    sc = strdup(ag_s(ag));      /* long option name is a string */
    scptr = sc;
    while( !ag_eow(ag) ) {
        c=ag_c_advance(ag);
        if (c == '=') {
            *scptr = '\0';
            break;
        }
        ++scptr;
    }
    
    i  = opt_longname_number(sc);   /* this is number of option w/ name `sc' */
    if(! ISVALIDNUMBER(i)) {
        opt_warn_1("%s not a registered option name\n",sc);
        return 0;
    }

    return opt_action(i,ag);
}

static int
opt_action(int i, ARGVECTOR *ag)
{
    /* ------------- */
    /* act on option */
    /* ------------- */
    opt_TYPE  o;
    char      c;
    char      *s;
    int       yes,no,maybe,toggle;    /* flag values */

    if (!ISVALIDNUMBER(i)) {
        opt_warn_1("opt_action: invalid number %d\n",i);
        return 0; /* failure */
    }

    switch( o=optlist[i].type ) {
    case OPT_INT:
        SETOPTVALUE(int,i, argnextnum(ag)); 
        break;
    case OPT_UNSINT: /* deprecated */
    case OPT_UINT:
        SETOPTVALUE(unsigned int,i, argnextnum(ag));
        break;
    case OPT_SHORT:
        SETOPTVALUE(short,i, argnextnum(ag));
        break;
    case OPT_LONG:
        SETOPTVALUE(long,i, argnextnum(ag));
        break;
    case OPT_USHORT:
        SETOPTVALUE(unsigned short,i, argnextnum(ag));
        break;
    case OPT_ULONG:
        SETOPTVALUE(unsigned long,i, argnextnum(ag));
        break;
    case OPT_FLOAT:
        SETOPTVALUE(float,i, argnextnum(ag));
        break;
    case OPT_DOUBLE:
        SETOPTVALUE(double,i, argnextnum(ag));
        break;
    case OPT_CHAR:
        SETOPTVALUE(char,i, ag_c_advance(ag));
        break;
    case OPT_UCHAR:
        SETOPTVALUE(unsigned char,i, ag_c_advance(ag));
        break;

    case OPT_FLAG:
    case OPT_NEGFLAG:
    case OPT_ABSFLAG:
    case OPT_ABSNEGFLAG:
        /* define terms:
         * yes: value of flag when  ON
         * no:  value of flag when OFF
         * maybe:  present value of flag
         * toggle: negative of maybe
         */
        maybe  = OPTVALUE(int,i);
        toggle = (maybe==OPT_TRUE ? OPT_FALSE : OPT_TRUE);
        if( o==OPT_FLAG || o==OPT_ABSFLAG )
            yes=OPT_TRUE;
        else
            yes=OPT_FALSE;
        no = (yes==OPT_TRUE ? OPT_FALSE : OPT_TRUE);
        c = ag_c(ag);           /* character following 'c' */
        switch(c) {
        case '+':
        case '1':
            SETOPTVALUE(int,i,yes);
            ag_c_advance(ag);       /* eat the '+' */
            break;
        case '-':
        case '0':
            SETOPTVALUE(int,i,no);
            ag_c_advance(ag);       /* eat the '-' */
            break;
        default:
            if( o==OPT_ABSFLAG || o==OPT_ABSNEGFLAG ) {
                SETOPTVALUE(int,i,yes);
            } else {    /* otherwise toggle value */
                SETOPTVALUE(int,i,toggle);
            }
            break;
        }
        break;

    case OPT_INTLEVEL:
        SETOPTVALUE(int,i,OPTVALUE(int,i) + 1);  /* default is to increment */
        c = ag_c(ag);
        switch(c) {
        case '+':
            /* we've already incremented */
            ag_c_advance(ag);
            break;
        case '-':
            SETOPTVALUE(int,i,0);
            ag_c_advance(ag);
            break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            SETOPTVALUE(int,i,c-'0');
            ag_c_advance(ag);
        default:
            break;
        }
        break;
        
    case OPT_STRING:
    case OPT_VSTRING:
        s = argnext(ag);
        SETOPTVALUE(char *,i,strdup(s));
        break;
    case OPT_CSTRING:
        strncpy( (char *)optlist[i].value , argnext(ag), OPT_MAXSTRLEN );
        break;
        
    case OPT_UNDELIMC:
    case OPT_UNDELIMV:
        opt_fatal("opt_action: can't process UNDELIM type");
        break;

    case OPT_NUL:
        break;
        
    default:
        opt_fatal("opt_delim: don't know this type");
        break;
    }


    /* Indicate that the option was invoked */
    optlist[i].invoked += 1;

    /* Run the hook if its defined */
    if (optlist[i].hook) {
        int retval;
        retval = optlist[i].hook(optlist[i].value);

        if (retval == OPT_ERROR) {
            /* do nothing for now */;
        }
        if (retval == OPT_ABORT && !menuflag)
            exit(opt_exit_number);
        if (retval == OPT_QUIT)
            opt_quit();
        if (retval == OPT_EXIT)
            exit(opt_exit_number);
    }
    
    return 1;                  /* success */
}



