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
/* $Id: ag.h,v 3.2 1997/10/26 15:44:55 jt Exp $ */
/* ag.h */


#ifndef _AG_H
#define _AG_H

#ifndef	TRUE
#define	TRUE	1
#define	FALSE	0
#endif

/*************************************
 * ARGVECTOR structure
 * Basically, an (argc,argv) construct
 * with indices to which character of 
 * which word is the current position
 */

typedef struct {
    char    **v;            /* argument vector */
    int     c;              /* argument count  */
    int     iw;             /* current word    */
    int     ic;             /* current character */
}   ARGVECTOR;

/* 
 * Function prototypes 
 */


extern  ARGVECTOR *ag_new(int,char **);
extern  void       ag_free(ARGVECTOR *);
extern  void       ag_fprint(FILE *, ARGVECTOR *);
extern  int        ag_enstring(char *,ARGVECTOR *,int);
extern  void       ag_reset(ARGVECTOR *);
extern  int        ag_w_number(ARGVECTOR *);
extern  void       ag_w_advance(ARGVECTOR *);
extern  int        ag_eow(ARGVECTOR *);
extern  int        ag_end(ARGVECTOR *);
extern  char       ag_c(ARGVECTOR *);
extern  char       ag_c_next(ARGVECTOR *);
extern  char       ag_cnn_next(ARGVECTOR *);
extern  char       ag_c_advance(ARGVECTOR *);
extern  char       ag_backspace(ARGVECTOR *);
extern  void       ag_backword(ARGVECTOR *);
extern  char      *ag_s(ARGVECTOR *);
extern  char      *ag_s_next(ARGVECTOR *);
extern  char      *ag_s_advance(ARGVECTOR *);
extern  int        ag_clear(ARGVECTOR *);
extern  int        ag_argc(ARGVECTOR *);
extern  char     **ag_argv(ARGVECTOR *);
extern  ARGVECTOR *ag_copy(ARGVECTOR *);
extern  void       ag_freeall(ARGVECTOR *);
extern  ARGVECTOR *ag_prepend_argv0(ARGVECTOR *, char *);
extern  char      *argnext(ARGVECTOR *);
extern  double     argnextnum(ARGVECTOR *);


#endif /* _AG_H */








