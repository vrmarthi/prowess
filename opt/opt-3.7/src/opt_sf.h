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
/*
    Copyright (C) 1996,1997 James Theiler 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    You can find the latest version of OPT on the web
    http://nis-www.lanl.gov/~jt/Software
    Or you can email the author: jt@lanl.gov

*/
/* $Id: opt_sf.h,v 3.5 1997/10/31 23:49:28 jt Exp $ */
#define SINGLEFILE 1

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


/** You may want to edit the #define's below **/

#ifndef HAVE_STRDUP
#define HAVE_STRDUP 1
#endif

#ifndef HAVE_UNISTD_H
#define HAVE_UNISTD_H 0
#endif

#ifndef HAVE_LONGJMP
#define HAVE_LONGJMP 0
#endif

#ifndef HAVE_SIGNAL
#define HAVE_SIGNAL 0
#endif

#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif

/** end opt_sf.h **/

