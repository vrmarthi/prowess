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
/* $Id: opt.h,v 3.6 1998/07/26 22:23:36 jt Exp $ */
/* opt.h */
/*
	User Include File for options package
*/

#ifndef _OPT_H
#define	_OPT_H 	/* Signal that this header file has been included */


typedef enum    {
    OPT_NUL,
    OPT_INT,        OPT_SHORT,      OPT_LONG,       OPT_CHAR,
    OPT_UINT,       OPT_USHORT,     OPT_ULONG,      OPT_UCHAR,
    OPT_FLOAT,      OPT_DOUBLE,
    OPT_FLAG,       OPT_NEGFLAG,    OPT_ABSFLAG,    OPT_ABSNEGFLAG,
    OPT_INTLEVEL,       
    OPT_STRING,     OPT_UNDELIM,
    OPT_CSTRING,    OPT_UNDELIMC,
    /* now deprecated */
    OPT_UNSINT,     OPT_VSTRING,    OPT_UNDELIMV
} opt_TYPE;

/* OPT_PFI is the ever popular: Pointer to Function returning an Integer */
typedef	int (*OPT_PFI)();
typedef	int (*OPT_PFI_ARG)(int,char**);

#define	OPT_EXT	".opt"	  /* standard options file extension */
#define OPT_MAXSTRLEN 80  /* CSTRING's should be no longer than this */

#define OPT_TRUE 1
#define OPT_FALSE 0

/* Return values from hook functions */
#define OPT_OK    0
#define OPT_ERROR 1
#define OPT_ABORT 2
#define OPT_QUIT  3
#define OPT_EXIT  4

/* opt(&argc,&argv) is the main function call that does all the work.
 * it processes the options on the command line, setting variables,
 * calling hooks, etc. 
 */

extern void opt(int *,char ***);

/* But before opt() can be called, variables/hooks/etc must be
 * registered.  Various ways are provided to register variables; one
 * identifies the variable by its (void *) pointer.  To register a
 * variable, an opt_TYPE must be specified, and either a character or
 * a string name (or both), and optionally a brief description.
 */
/*  optrega: "all"
 *  optreg:  standard
 *  optregc: just sets character name
 *  optregs: just sets string name
 *  optregcb: sets character name and brief description
 *  optregsb: sets string name and brief description
 *  optexec: registers an 'exec function' (somthing that is run, then exits)
 */

/*    ARGUMENT LIST:  &var   OPT_INT   'c'   "name"  "Descript" */
extern int optrega(  void *, opt_TYPE, char,  char *, char * );
extern int optreg(   void *, opt_TYPE, char,          char * );
extern int optregc(  void *, opt_TYPE, char                  );
extern int optregcb( void *, opt_TYPE, char,          char * );
extern int optregs(  void *, opt_TYPE,        char *         );
extern int optregsb( void *, opt_TYPE,        char *, char * );
/*    ARGUMENT LIST:  &var   OPT_INT   'c'   "name"  "Descript" */

/*    ARGUMENTS:    "name"  hook "Descript" */
extern int optexec( char *, OPT_PFI, char * );

/* Using the following routines, one can specify features (such as
 * char, longname, descript) of the variable that were left out in the
 * registration process, or one can add features (such as help, hook,
 * exec) that the registration routines do not permit.
 */
/* These functions use &var to identify which option */
extern void optchar(     void *,char);
extern void optlongname( void *,char *);
extern void optdescript( void *,char *);
extern void opthelp(     void *,char *);
extern void opthook(     void *,OPT_PFI);
/* These functions use option number to identify which option */
extern void optchar_n(     int,char);
extern void optlongname_n( int,char *);
extern void optdescript_n( int,char *);
extern void opthelp_n(     int,char *);
extern void opthook_n(     int,OPT_PFI);

/* optinvoked(&var) returns the number of times the option was invoked
 * on the command line.  This function is not used in the registering
 * stage, but is used during execution of the code to see whether or
 * not a value was actually set (or if it's just using the default).
 */
extern int  optinvoked(void *);

/* The following routines are used to set features (such as usage),
 * which are not specific to single variables, but apply to the whole
 * program.  In general, I try to use the convention optUpperCase()
 * for the registration of these more global features, while
 * optlowercase() refers to functions that apply to a specific
 * registered variable.
 */
extern void optUsage(char *);
extern void optTitle(char *);
extern void optEnvVarName(char *);
extern void optDefaultString(char *);
extern void optDefaultFile(char *);

extern void optAdditionalUsage(OPT_PFI);
extern void optQuit(OPT_PFI);
extern void optRun(OPT_PFI);
extern void optMain(OPT_PFI_ARG);
extern void optExitNumber(int);

/* The following prototypes arguably belong in opt_p.h because it is
 * not clear that the functions would ever be used in a program that
 * uses opt.  Certainly, opt itself makes great use of these, behind
 * the scenes.
 */
extern void optAbortRun(void);
extern  int optBeginRun(OPT_PFI);
extern void optAbortMain(void);
extern  int optBeginMain(OPT_PFI_ARG, int, char **);

extern void opt_message(char *);
extern void opt_warning(char *);
extern void opt_fatal(char *);



/* BEGIN_AUTOGENERATED */
#define UINT OPT_UINT
extern int optrega_UINT(unsigned int *v, char c, char *n, char *B);
extern int optreg_UINT(unsigned int *v, char c, char *B);
extern int optregc_UINT(unsigned int *v, char c);
extern int optregs_UINT(unsigned int *v, char *n);
extern int optregcb_UINT(unsigned int *v, char c, char *B);
extern int optregsb_UINT(unsigned int *v, char *s, char *B);
#define UNSINT OPT_UNSINT
extern int optrega_UNSINT(unsigned int *v, char c, char *n, char *B);
extern int optreg_UNSINT(unsigned int *v, char c, char *B);
extern int optregc_UNSINT(unsigned int *v, char c);
extern int optregs_UNSINT(unsigned int *v, char *n);
extern int optregcb_UNSINT(unsigned int *v, char c, char *B);
extern int optregsb_UNSINT(unsigned int *v, char *s, char *B);
#define INTLEVEL OPT_INTLEVEL
extern int optrega_INTLEVEL(int *v, char c, char *n, char *B);
extern int optreg_INTLEVEL(int *v, char c, char *B);
extern int optregc_INTLEVEL(int *v, char c);
extern int optregs_INTLEVEL(int *v, char *n);
extern int optregcb_INTLEVEL(int *v, char c, char *B);
extern int optregsb_INTLEVEL(int *v, char *s, char *B);
#define UNDELIMV OPT_UNDELIMV
extern int optrega_UNDELIMV(char **v, char c, char *n, char *B);
extern int optreg_UNDELIMV(char **v, char c, char *B);
extern int optregc_UNDELIMV(char **v, char c);
extern int optregs_UNDELIMV(char **v, char *n);
extern int optregcb_UNDELIMV(char **v, char c, char *B);
extern int optregsb_UNDELIMV(char **v, char *s, char *B);
#define ULONG OPT_ULONG
extern int optrega_ULONG(unsigned long *v, char c, char *n, char *B);
extern int optreg_ULONG(unsigned long *v, char c, char *B);
extern int optregc_ULONG(unsigned long *v, char c);
extern int optregs_ULONG(unsigned long *v, char *n);
extern int optregcb_ULONG(unsigned long *v, char c, char *B);
extern int optregsb_ULONG(unsigned long *v, char *s, char *B);
#define USHORT OPT_USHORT
extern int optrega_USHORT(unsigned short *v, char c, char *n, char *B);
extern int optreg_USHORT(unsigned short *v, char c, char *B);
extern int optregc_USHORT(unsigned short *v, char c);
extern int optregs_USHORT(unsigned short *v, char *n);
extern int optregcb_USHORT(unsigned short *v, char c, char *B);
extern int optregsb_USHORT(unsigned short *v, char *s, char *B);
#define ABSNEGFLAG OPT_ABSNEGFLAG
extern int optrega_ABSNEGFLAG(int *v, char c, char *n, char *B);
extern int optreg_ABSNEGFLAG(int *v, char c, char *B);
extern int optregc_ABSNEGFLAG(int *v, char c);
extern int optregs_ABSNEGFLAG(int *v, char *n);
extern int optregcb_ABSNEGFLAG(int *v, char c, char *B);
extern int optregsb_ABSNEGFLAG(int *v, char *s, char *B);
#define INT OPT_INT
extern int optrega_INT(int *v, char c, char *n, char *B);
extern int optreg_INT(int *v, char c, char *B);
extern int optregc_INT(int *v, char c);
extern int optregs_INT(int *v, char *n);
extern int optregcb_INT(int *v, char c, char *B);
extern int optregsb_INT(int *v, char *s, char *B);
#define VSTRING OPT_VSTRING
extern int optrega_VSTRING(char **v, char c, char *n, char *B);
extern int optreg_VSTRING(char **v, char c, char *B);
extern int optregc_VSTRING(char **v, char c);
extern int optregs_VSTRING(char **v, char *n);
extern int optregcb_VSTRING(char **v, char c, char *B);
extern int optregsb_VSTRING(char **v, char *s, char *B);
#define UNDELIM OPT_UNDELIM
extern int optrega_UNDELIM(char **v, char c, char *n, char *B);
extern int optreg_UNDELIM(char **v, char c, char *B);
extern int optregc_UNDELIM(char **v, char c);
extern int optregs_UNDELIM(char **v, char *n);
extern int optregcb_UNDELIM(char **v, char c, char *B);
extern int optregsb_UNDELIM(char **v, char *s, char *B);
#define NUL OPT_NUL
extern int optrega_NUL(void *v, char c, char *n, char *B);
extern int optreg_NUL(void *v, char c, char *B);
extern int optregc_NUL(void *v, char c);
extern int optregs_NUL(void *v, char *n);
extern int optregcb_NUL(void *v, char c, char *B);
extern int optregsb_NUL(void *v, char *s, char *B);
#define SHORT OPT_SHORT
extern int optrega_SHORT(short *v, char c, char *n, char *B);
extern int optreg_SHORT(short *v, char c, char *B);
extern int optregc_SHORT(short *v, char c);
extern int optregs_SHORT(short *v, char *n);
extern int optregcb_SHORT(short *v, char c, char *B);
extern int optregsb_SHORT(short *v, char *s, char *B);
#define LONG OPT_LONG
extern int optrega_LONG(long *v, char c, char *n, char *B);
extern int optreg_LONG(long *v, char c, char *B);
extern int optregc_LONG(long *v, char c);
extern int optregs_LONG(long *v, char *n);
extern int optregcb_LONG(long *v, char c, char *B);
extern int optregsb_LONG(long *v, char *s, char *B);
#define ABSFLAG OPT_ABSFLAG
extern int optrega_ABSFLAG(int *v, char c, char *n, char *B);
extern int optreg_ABSFLAG(int *v, char c, char *B);
extern int optregc_ABSFLAG(int *v, char c);
extern int optregs_ABSFLAG(int *v, char *n);
extern int optregcb_ABSFLAG(int *v, char c, char *B);
extern int optregsb_ABSFLAG(int *v, char *s, char *B);
#define UNDELIMC OPT_UNDELIMC
extern int optrega_UNDELIMC(char *v, char c, char *n, char *B);
extern int optreg_UNDELIMC(char *v, char c, char *B);
extern int optregc_UNDELIMC(char *v, char c);
extern int optregs_UNDELIMC(char *v, char *n);
extern int optregcb_UNDELIMC(char *v, char c, char *B);
extern int optregsb_UNDELIMC(char *v, char *s, char *B);
#define UCHAR OPT_UCHAR
extern int optrega_UCHAR(unsigned char *v, char c, char *n, char *B);
extern int optreg_UCHAR(unsigned char *v, char c, char *B);
extern int optregc_UCHAR(unsigned char *v, char c);
extern int optregs_UCHAR(unsigned char *v, char *n);
extern int optregcb_UCHAR(unsigned char *v, char c, char *B);
extern int optregsb_UCHAR(unsigned char *v, char *s, char *B);
#define NEGFLAG OPT_NEGFLAG
extern int optrega_NEGFLAG(int *v, char c, char *n, char *B);
extern int optreg_NEGFLAG(int *v, char c, char *B);
extern int optregc_NEGFLAG(int *v, char c);
extern int optregs_NEGFLAG(int *v, char *n);
extern int optregcb_NEGFLAG(int *v, char c, char *B);
extern int optregsb_NEGFLAG(int *v, char *s, char *B);
#define FLOAT OPT_FLOAT
extern int optrega_FLOAT(float *v, char c, char *n, char *B);
extern int optreg_FLOAT(float *v, char c, char *B);
extern int optregc_FLOAT(float *v, char c);
extern int optregs_FLOAT(float *v, char *n);
extern int optregcb_FLOAT(float *v, char c, char *B);
extern int optregsb_FLOAT(float *v, char *s, char *B);
#define STRING OPT_STRING
extern int optrega_STRING(char **v, char c, char *n, char *B);
extern int optreg_STRING(char **v, char c, char *B);
extern int optregc_STRING(char **v, char c);
extern int optregs_STRING(char **v, char *n);
extern int optregcb_STRING(char **v, char c, char *B);
extern int optregsb_STRING(char **v, char *s, char *B);
#define CSTRING OPT_CSTRING
extern int optrega_CSTRING(char *v, char c, char *n, char *B);
extern int optreg_CSTRING(char *v, char c, char *B);
extern int optregc_CSTRING(char *v, char c);
extern int optregs_CSTRING(char *v, char *n);
extern int optregcb_CSTRING(char *v, char c, char *B);
extern int optregsb_CSTRING(char *v, char *s, char *B);
#define FLAG OPT_FLAG
extern int optrega_FLAG(int *v, char c, char *n, char *B);
extern int optreg_FLAG(int *v, char c, char *B);
extern int optregc_FLAG(int *v, char c);
extern int optregs_FLAG(int *v, char *n);
extern int optregcb_FLAG(int *v, char c, char *B);
extern int optregsb_FLAG(int *v, char *s, char *B);
#define DOUBLE OPT_DOUBLE
extern int optrega_DOUBLE(double *v, char c, char *n, char *B);
extern int optreg_DOUBLE(double *v, char c, char *B);
extern int optregc_DOUBLE(double *v, char c);
extern int optregs_DOUBLE(double *v, char *n);
extern int optregcb_DOUBLE(double *v, char c, char *B);
extern int optregsb_DOUBLE(double *v, char *s, char *B);
#define CHAR OPT_CHAR
extern int optrega_CHAR(char *v, char c, char *n, char *B);
extern int optreg_CHAR(char *v, char c, char *B);
extern int optregc_CHAR(char *v, char c);
extern int optregs_CHAR(char *v, char *n);
extern int optregcb_CHAR(char *v, char c, char *B);
extern int optregsb_CHAR(char *v, char *s, char *B);
/* END_AUTOGENERATED */

/* The following prototypes are deprecated; they are defined for
 * compatibility with versions of opt prior to v3.0, but they are not
 * recommended.
 */
extern  int getopts(int,char **);
#define	optregister(val,typ,c,str) optreg(&val,typ,c,str)
#define optrunset(r) do {int r(); opt_run_set(r);} while(0)


#endif /* _OPT_H */


