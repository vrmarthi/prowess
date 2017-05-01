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
/* $Id: opt_util.c,v 3.7 1998/07/01 16:56:39 jt Exp $ */
/**************************
 *  some useful utilities *
 **************************/
/*
 *  *** NO LONGER AVAILABLE!!
 *  *** 1) Variable argument lists: for error messages
 *  ***   opt_errmess(), gives message and then aborts run
 *     
 *  2) Long Jump: for aborting run() without losing the entire job
 *     optBeginRun()
 *     optAbortRun()
 *     2a) Signal: trapping ^C so that it aborts run()
 *  4) Readline: interface to GNU readline
 *     
 */
/*  Can enable longjmp with: #define HAVE_LONGJMP 1
 *  Only if longjmp is enabled can signal be enabled, using
 *                           #define HAVE_SIGNAL 1
 *  Finally, readline can be enabled with
 *                           #define HAVE_READLINE 1
 *  If this version of opt came with a configure script, then those
 *  values should be set automatically by the system at configure time.
 */
#ifndef SINGLEFILE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if 0                           /* Currently, config.h is not working */
#include "config.h"             /* supplies HAVE_VPRINTF + RETSIGTYPE */
#endif
#include "ag.h"
#include "opt.h"
#include "opt_p.h"
#endif /* SINGLEFILE */

/* If these are not defined on the compile command line, then assume
 * the worst, that you don't have anything!
 */
#ifndef HAVE_LONGJMP
#define HAVE_LONGJMP 0
#endif
#ifndef HAVE_SIGNAL
#define HAVE_SIGNAL 0
#endif
#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif

/* Provide message/warning/fatal functions */
/* Ordinary messages go to stdout, but warn/fatal messages to stderr;
 * (This convention adopted at suggestion of Jason V. Morgan) */
void
opt_message(char *s)
{
    if (s) fputs(s,stdout);
}
void
opt_warning(char *s)
{
    fputs("OPT Warning: ",stderr);
    if (s) fputs(s,stderr);
    fputs("\n",stderr);
}
void
opt_fatal(char *s)
{
    fputs("OPT Fatal error: ",stderr);
    if (s) fputs(s,stderr);
    exit(opt_exit_number);
}

#if HAVE_LONGJMP
#include <setjmp.h>
static jmp_buf	opt_jumpstart;
static int	opt_jump_set=FALSE;

/* prototypes */
static RETSIGTYPE set_signal(void);
static RETSIGTYPE unset_signal(void);

/**********
 * SIGNAL *
 **********/
#if HAVE_SIGNAL
#include <signal.h>

static void
catch_signal(int sig
#if 0                           /* someday make this portable! */
             ,int code, struct sigcontext *scp, char *addr
#endif                         
             )
{
    opt_message("\nOPT Interrupted:\n");
    longjmp(opt_jumpstart,1);
    perror("Did not long-jump");
    exit(0);
}

static RETSIGTYPE (*old_catcher)(int) = NULL;
static RETSIGTYPE
set_signal(void)
{
    old_catcher = signal(SIGINT,catch_signal);
}
static RETSIGTYPE
unset_signal(void)
{
    signal(SIGINT,old_catcher);
}
#else

static RETSIGTYPE   set_signal(void) {}
static RETSIGTYPE unset_signal(void) {}

#endif /* end SIGNAL */


/* optBeginRun, optAbortRun: these functions are for running
 * the run hook defined by the programmer in optRun().
 */

int
optBeginRun(OPT_PFI run)
{
    int value;

    opt_jump_set=OPT_TRUE;
    set_signal();

    value = setjmp(opt_jumpstart);
    if(value!=0)
        opt_message("OPT: Run aborted...try again\n");
    else
        value = (*run)();
	
    unset_signal();
    opt_jump_set=OPT_FALSE;

    return value;
}

void
optAbortRun(void)
{
    if(opt_jump_set)
        longjmp(opt_jumpstart,1);
    else
        /** should this be some opt_exit() ?? **/
        exit(opt_exit_number);
}

int optBeginMain(OPT_PFI_ARG run_args,int argc, char **argv)
{
    int value;

    opt_jump_set=OPT_TRUE;
    set_signal();

    value = setjmp(opt_jumpstart);
    if(value!=0)
        opt_message("OPT: Run aborted...try again\n");
    else
        value = (*run_args)(argc,argv);
	
    unset_signal();
    opt_jump_set=OPT_FALSE;

    return value;
}    

#else  /* If LONGJMP is unavailable, or disabled, then these functions
        * are rather trivial.
        */
int  optBeginRun(OPT_PFI run) { return (*run)(); }
void optAbortRun() { exit(opt_exit_number); }
int  optBeginMain(OPT_PFI_ARG run_args,int argc, char **argv)
{
    return (*run_args)(argc,argv);
}

#endif /* end LONGJMP */

void optAbortMain() { optAbortRun(); }


/************
 * Readline *
 ************/

#ifdef HAVE_READLINE
#define READLINE_LIBRARY
#include <readline/readline.h>
#include <readline/history.h>
#endif

void
opt_readline_init(char *s)
{
#ifdef HAVE_READLINE
    rl_readline_name = s;
#endif
    return;
}

void 
opt_menu_getresponse(char *prompt, char *respond, int maxlen)
{
#ifdef HAVE_READLINE
    char *line;
    line = readline(prompt);
    if (line && *line) {
	add_history(line);
	strncpy(respond,line,maxlen);
	free(line);
    } else
	respond[0] = '\0';
	/* double terminate the line */
    respond[strlen(respond)+1] = '\0';
#else
    opt_message(prompt);
    opt_getline(respond,stdin);
#endif
}

/* ...well, assume that you do have strdup, since if you do and you
 * say you don't you can get into trouble.
 */
#ifndef HAVE_STRDUP
#define HAVE_STRDUP 1
#endif

#if !HAVE_STRDUP
/* Provided here in case it is not available on system */     
char *strdup(char *s)
{
    char *sdup;
    int i,n;
    n = strlen(s);
    sdup = (char *)malloc(n+1);
    for (i=0; i<n; ++i)
        sdup[i] = s[i];
    sdup[n]='\0';
    return sdup;
}
#endif
    
