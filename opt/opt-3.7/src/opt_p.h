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
/* $Id: opt_p.h,v 3.5 1998/07/01 16:56:34 jt Exp $ */
/* opt_p.h */
/*
 *	Private header file for OPT package.
 */

#ifndef _OPT_P_H
#define _OPT_P_H

#ifndef	OPT_TRUE
#define	OPT_TRUE	1
#define	OPT_FALSE	0
#endif

#ifndef DEBUG
#define DEBUG 0
#endif

typedef enum { False=OPT_FALSE, True=OPT_TRUE } flag;

/* 
 * These are delimiter characters 
 */
#define DELIM		'-'		/* option delimiter character */
#define	ALTDELIM	'/'		/* alternate delimiter character */
#define	OPTFROMFILE	'@'		/* denotes options are in a file */
#define	OPTTOFILE	'%'		/* says to put options in a file */
#define	DOCUMENT	'-'		/* write document to file */
#define	INTERACT	'$'		/* Flags interactive menu */
#define	HELPCH		'?'		/* Help character */
/*
 * These are not available on command line 
 * But may be invoked from a file
 */
#define IGNOREEOL       ';'		/* Ignore until the end of line */
#define	RUN             '='		/* Says to just run to completion */
#define	QUITCH		'.'		/* Quit character */
/*
 * These are not available on command line or from a file
 * But may be invoked from the menu
 */
#define	BANG		'!'		/* Shell escape character */
#define	ADDITIONAL_OPTS	'+'		/* Additional options */

#define ISEMPTYSTRING(s) (!((s) && *(s)))

/******************
 * Global variables
 */
extern	int	opt_nreg;
extern	flag	menuflag;
extern	flag	fileflag;
extern  int     opt_exit_number;
/*********************
 * Function Prototypes
 */

extern	void	opt_get_help(char);
extern  char   *optgetTitle(void);
extern  int     opt_char_number(char);

extern  int	opt_fprint(FILE *,int);
extern  void	opt_tofile(FILE *);
extern	char	*optstrval(int);
extern	char	*optstrtyp(int);
extern  int     opt_undelim(ARGVECTOR *);
extern  int     opt_delim(ARGVECTOR *);
extern  int     opt_longdelim(ARGVECTOR *);

extern void short_usage(void);
extern void long_usage(void);


		/* --------------- */
		/* Process Options */
		/* --------------- */

extern	char	  *opt_program_name;
extern  OPT_PFI    opt_additional_usage_fcn;
extern  OPT_PFI    opt_quit_fcn, opt_help_fcn, opt_run_fcn;

extern	void       opt_wr_title(void);
extern  int        opt_fromfname(char *);
extern	int	   opt_tofname(char *);
extern  int        opt_getline(char *,FILE *);
extern	char	  *opt_mstring(int);
extern  ARGVECTOR *opt_process(int,char **);
extern  char      *short_progname(char *);
extern  int        opt_undelim(ARGVECTOR *);
extern  int        opt_delim(ARGVECTOR *);
extern  int        opt_lineprocess(char *);
extern	void	   opt_menu(void);
extern  void       opt_menu_getresponse(char *, char *, int);

extern	void	opt_help(char *);
extern	void	opt_quit(void);
extern	void	opt_usage(void);
extern  void    opt_readline_init(char *);  

#define opt_warn_1(fmt,var1) do { \
    char gstr[180]; sprintf(gstr,fmt,var1); \
        opt_warning(gstr); } while(0)
#define opt_warn_2(fmt,var1,var2) do { \
    char gstr[180]; sprintf(gstr,fmt,var1,var2); \
	opt_warning(gstr); } while(0)
#define opt_warn_3(fmt,var1,var2,var3) do { \
    char gstr[180]; sprintf(gstr,fmt,var1,var2,var3); \
	opt_warning(gstr); } while(0)

#define opt_mess_1(fmt,var1) do { \
    char gstr[180]; sprintf(gstr,fmt,var1); \
	opt_message(gstr); } while(0)
#define opt_mess_2(fmt,var1,var2) do { \
    char gstr[180]; sprintf(gstr,fmt,var1,var2); \
	opt_message(gstr); } while(0)
#define opt_mess_3(fmt,var1,var2,var3) do { \
    char gstr[180]; sprintf(gstr,fmt,var1,var2,var3); \
	opt_message(gstr); } while(0)
#define opt_mess_4(fmt,var1,var2,var3,var4) do { \
    char gstr[180]; sprintf(gstr,fmt,var1,var2,var3,var4); \
	opt_message(gstr); } while(0)

#define opt_fatal_1(fmt,var1) do { \
    char gstr[180]; sprintf(gstr,fmt,var1); \
	opt_fatal(gstr); } while(0)
#define opt_fatal_2(fmt,var1,var2) do { \
    char gstr[180]; sprintf(gstr,fmt,var1,var2); \
	opt_fatal(gstr); } while(0)
#define opt_fatal_3(fmt,var1,var2,var3) do { \
    char gstr[180]; sprintf(gstr,fmt,var1,var2,var3); \
	opt_fatal(gstr); } while(0)


#ifndef HAVE_STRDUP
#define HAVE_STRDUP 0
#endif

#if !HAVE_STRDUP
char *strdup(char *s);
#endif

#endif /* _OPT_P_H */
