#include <stdio.h>
#include <stdlib.h>
#include "/home/viswesh/prokect/ORT/code/skysim/build/inc/cpgplot.h"

int main()
{
 FILE *fcon;
 float x[22], y[22];
 char line[20];
 int i = 0;

 fcon = fopen("convergence.dat","r");

 for(i = 0; i < 22; i++)
 {
   fscanf(fcon, "%f %f", &x[i], &y[i]);
   fprintf(stderr, "%d %f %f\n", i, x[i], y[i]);
 }

 fclose(fcon);

 cpgopen("/xs");
 open_scope();


 return 0;
}

void open_scope( void )
{
 float SIDE = 6.0, SQUARE = 1.0;
 float XMIN, XMAX, YMIN, YMAX;
 int C;
 float vwp[4];

 vwp[0] = 0.08;
 vwp[1] = 0.92;
 vwp[2] = 0.08;
 vwp[3] = 0.92;
 /*
 if( plot->driver == 'P' )
 { C = PSCOLOR;
   plot->MSEboard = cpgopen("/cps");
 }
 else if ( plot->driver == 'X' )
 { C = COLOR;
   plot->MSEboard = cpgopen("/xs");
 }
 */
 cpgpap(SIDE, SQUARE);
 cpgsch(0.84);
 cpgsci(C);
 cpgsvp(vwp[0], vwp[1], vwp[2], vwp[3]);
 XMIN =  log10f(0.005);
 XMAX =  log10f(1.0);
 YMIN =  log10f(10);
 YMAX =  log10f(2000);
 cpgswin(XMIN, XMAX, YMIN, YMAX);
 cpgbox("BCGNST", 0.0, 0, "BCGLNST", 0.0, 0);
 cpgsch(1.2);
 cpgmtxt("B",1.8, 0.45, 0.0, "Step size");
 cpgmtxt("L",1.8, 0.40, 0.0, " Iterations ");
}
