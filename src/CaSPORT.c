#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/errno.h>

#include <readline/readline.h>
#include <readline/history.h>


#include "../inc/sysdefs.h"
#include "../inc/fitsdefs.h"
#include "../inc/plotdefs.h"
#include "../inc/opt.h"

#include "../inc/version.h"

int BigEndian    = 0 ;
int LittleEndian = 0 ;
char Usage[256];
int v = 0;

char *keywords[] = {"list         : List all keywords supported at the prompt.", 
		    "prog         : List all executables available in this package.", 
		    "readme       : Open \"README\", the how-to-build file for viewing.", 
		    "explain(arg) : Open the help page for the program given as argument",
		    "version      : Show what version is currently running.", 
		    "clear        : Clear page, take prompt to top of page.", 
		    "quit / exit  : Exit this package.", 
		    NULL};

char *progs[] = { "psFITS", 
		  "genFITS", 
		  "corr2FITS", 
		  "FITSinfo",
		  "FITSview",
		  "reltaFITS", 
		  "rechnFITS", 
		  "FITSgain", 
		  "FITSsol", 
		  "FITSapcl", 
		  "FITSsecl", 
		  "FITSlogc", 
		  "FITSlinc", 
		  "logcal", 
		  "lincal", 
		  "itergain", 
		  "itersol", 
		  "selfcal", 
		  "plotsol", 
		  "plotall", 
		  "ploterr", 
		  "plottraj", 
		  "plotcmp", 
		  "plotHess", 
		  "plotCRB", 
		  "psplot", 
		  "imagtifr", 
		  "plottibl", 
		  "plotfrbl",
		  "imagfrbl", 
		  "mkmodel", 
		  "plotmodl",
		  "FITSbsol",
		  "FITSbeam",
		  "FITSbmbd",
		  NULL};
		  
float show_version()
{
  float version = VERSION;
  return version;
}

int show_readme(void)
{
 char README[80];
 char *path;
 path = getenv("PKGROOT");
 strcpy(README, "less ");
 strcat(README, path);
 strcat(README, "/README");
 system(README);
 return 0;
}

int show_explain(void)
{
 char explain[80];
 char *path;
 path = getenv("PKGROOT");
 strcpy(explain, "less ");
 strcat(explain, path);
 strcat(explain, "/doc/explain");
 system(explain);
 return 0;
}

int show_keywords(void)
{
 int got = 0, len = 0;
 char header[120]= "";
 char underline[120]= "";

 strcpy(header, "Keywords available :");
 len = strlen(header);
 while(len-- > 0) strcat(underline, "~");
 fprintf(finfo, "%s %s\n", info, header);
 fprintf(finfo, "%s %s\n", info, underline);
 while ( keywords[got] != NULL )
 fprintf(finfo, "%s %s\n", info, keywords[got++]);
 return got;
}

int show_programs(void)
{
  int got = 0, len = 0;
 char header[120]= "";
 char underline[120]= "";

 strcpy(header, "Executables available :");
 len = strlen(header);
 while(len-- > 0) strcat(underline, "~");
 fprintf(finfo, "%s %s\n", info, header);
 fprintf(finfo, "%s %s\n", info, underline);
 while ( progs[got] != NULL )
     fprintf(finfo, "%s %s\n", info, progs[got++]);
 return got;
}

int explain(char *program)
{
 char helppage[1024];
 int n = 0, got = 0;
 char *path;
 path = getenv("PKGROOT");
 while( !got && progs[n] != NULL )
 { if( !strcmp(progs[n++],program) ) 
   { got = 1;
     strcpy(helppage, "less ");
     strcat(helppage, path);
     strcat(helppage, "/doc/");
     strcat(helppage, program);
     system(helppage);
   }
   else if (progs[n] == NULL )
     fprintf(ferr, "%s\"%s\" is not a valid program\n", err, program);
 }
 return 0;
}

int lookup( char *command)
{
 int got = 0;
 float ver;
 char *token;
 char program[80];

 if ( !strcmp(command, "quit") ) got = 0;
 else if( !strcmp(command, "exit") ) got = 0;
 else if( !strcmp(command, "prog") ) got = show_programs();
 else if( !strcmp(command,"list") ) got = show_keywords();
 else if( !strcmp(command,"clear") ) got = system("clear") + 1;
 else if( !strcmp(command, "readme") ) got = show_readme() + 1;
 else if( !strcmp(command, "explain") ) got = show_explain() + 1;
 else if( strstr(command, "explain") ) 
 { token = strtok(command, " ");
   token = strtok(NULL, " ");
   strcpy(program, token);
   got = explain(program) + 1;
 }
 else if( !strcmp(command, "version") ) 
 { fprintf(fprompt, " Version : %3.2f\n", show_version());
   got = 1;
 }
 else if( !strcmp(command, "") ) got = 2;
 else 
 {system(command); 
  got = 1;
 }
 return got;
}

void greeting(char *progname)
{
    char about[120];
    fprintf( stderr, " %s: Calibration System for the Programmable ORT\n", progname);
    fprintf( stderr, " Version %3.2f\n", show_version());
    fprintf(stderr, " Last build : ");
    sprintf(about, "ls -lrt `which %s`|awk '{print $6, $7, $8}'", progname);
    system(about);
    if (!v) fprintf(stderr, "\n Type \"list\" at the prompt for a list of keywords.\n\n");
}


int CaSPORT(int argc, char **argv )
{
 char *command;
 int ret;
 float ver;

 define_prompts(argv[0]);
 getMachineEndianness(&BigEndian, &LittleEndian);

 if ( !v ) system("clear");
 greeting(argv[0]);

 if( v ) exit(-1);

 do
 { command = readline(prompt);
   add_history(command);
   ret = lookup(command);
   if(ret == -1) fprintf(ferr, "%s Unknown command \"%s\"\n", err, command);
 } while( ret != 0);

 return ret;
}

int main(int argc, char **argv )
{
  optrega(&v, OPT_BOOL, 'v', "ver", "Show version");
  sprintf(Usage, " [-v]");
  optUsage(Usage);
  optTitle("Calibration System for the Programmable Ooty Radio Telescope\n");
  optMain(CaSPORT);
  opt(&argc,&argv);

  return CaSPORT(argc, argv);
}
