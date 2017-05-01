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
/****************************************
*	Show up a weirdness in opt hooks.
*       Written by Mark Muldoon 21 Oct 97
*       The weirdness was in fact a bug in opt,
*       which was fixed in going from v3.0 to v3.1
*/
# include <stdio.h>
# include <opt.h>

/****************************************
*	Prototypes for hooks.
*/
int check_hook(void *a_ptr);

/****************************************
*	Global variables.
*/
int		a_global ;
/* _______________________________________ */

void main( int argc, char *argv[] ) 
/****************************************
*	Register a single command line 
* option and give it a hook.
*/
{
	optrega( &a_global, OPT_INT, 'a', 
			"set_a_global", "Set a global variable" ) ;

	opthook( &a_global, check_hook ) ;

	opt( &argc, &argv ) ;
}
/* ____________________________________ */

int check_hook( void *a_ptr )
/****************************************
*	Check whether the hook recieves the
* correct value.
*/
{
	int		a_local, verdict ;

	a_local = *((int *) a_ptr) ;
	if( a_local == a_global ) { 
		printf( "All is well.\n" ) ;

		verdict = OPT_OK ;
	}
	else {
		printf( "Confusion reigns: *a_ptr = %d, ", a_local ) ;
		printf( "but a_global = %d.\n", a_global ) ;

		verdict = OPT_ERROR ;
	}

	return( verdict ) ;
}
