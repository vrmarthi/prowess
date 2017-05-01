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
/* birthday.c */

#include <stdio.h>
#include <opt.h>                /* part of opt package */

/* Parameters that user has access to via opt package;
 * They are typically (but not necessarily) global variables.
 * Their default values are provided in the assignement statements.
 */
int month=9;
int day=11;
int year=1989;
int verb=0;
int pade=0;
int greg=0;

/* All of what the program itself does is in the birthday() function;
 * This function does what a non-options parsing main() might do.
 */
int birthday(int argc, char **argv)
{
    if (month == 9 && day == 11 && year == 1989)
        printf("Happy birthday, Max\n");
    else if (month == 4 && day == 24 && year == 1993)
        printf("Happy birthday, Sky\n");

    if (verb)
        printf("Hello, world: %4d/%02d/%02d\n",year,month,day);

    return OPT_OK;
}

/* all of the options parsing is in the new main() function */

int main(int argc, char **argv)
{
    /* optrega() registers short name '-m' and long name '--month' to
     * variable 'month', and provides brief description "Month"
     */
    optrega(&month,OPT_INT,'m',"month","Month");
    optrega(&day,  OPT_INT,'d',"day",  "Day of month");
    /* optreg() only provides short name '-y' */
    optreg(&year,OPT_INT,'y',"Year");
    /* register some flag variables... */
    optreg(&verb,OPT_FLAG,'v',"Verbose");
    optreg(&pade,OPT_FLAG,'p',"Use Pade Approximants");
    optreg(&greg,OPT_FLAG,'g',"Gregorian");

    /* the function birthday() is registered with opt */
    optMain(birthday);

    /* opt() is the routine that actually parses the argc/argv
     * variables
     */
    opt(&argc,&argv);
    /* and when it's done, argc/argv will contain the leftover
     * argc/argv variables, including the same argv[0] as in
     * the original argc/argv
     */

    /* Now that variables are parsed, run birthday() */
    return birthday(argc,argv);
}
