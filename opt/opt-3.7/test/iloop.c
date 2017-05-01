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
/* iloop.c */
/* This routine shows how the interrupt works, when, for example, you
 * are in an infinite loop.  To try this out run it in the menu mode,
 * ie
 *
 *    iloop --menu
 *    ->
 *
 * If you enter 'N -1 =', meaning run with N=-1 (no, this isn't
 * postfix!)  then after 100 times through the loop, it will interrupt
 * itself with optAbortRun() which will bring you back to the menu.
 * If you enter any other negative N (or just a VERY large positive
 * N), then you will get into an infinite loop.  Hit ^C to get out of
 * the loop.  If the signal trapping works, then you should be
 * returned to the prompt instead of exiting the whole program.
 * 
 */
#include <stdio.h>
#include <opt.h>

long N=1;

int run()
{
    long n;
    /** if N is negative, this gives an infinite loop **/
    for (n=0; n!=N; ++n) {
        printf("Type ^C to exit\n");
        if (n==10 && N == -1) optAbortRun();
    }
    return OPT_OK;
}

main(int argc, char **argv)
{
    optreg(&N,OPT_LONG,'N',"Number of times through the loop");
    optRun(run);
    opt(&argc,&argv);
    run();
}



