/*
 * debug.h -- my debug header for just any program.
 * use:
 * place the statement OPENDEBUG("/tmp/programdebug"); inside main.
 * Nothing will happen, unless you compile the source with -DDEBUG.
 * 
 * jw. 13.4.91.
 */
#ifndef FILE
# include <stdio.h>
#endif

#ifdef DEBUG
#	define debug(x) {fprintf(debugfp,x);fflush(debugfp);}
#	define debug1(x,a) {fprintf(debugfp,x,a);fflush(debugfp);}
#	define debug2(x,a,b) {fprintf(debugfp,x,a,b);fflush(debugfp);}
#	define debug3(x,a,b,c) {fprintf(debugfp,x,a,b,c);fflush(debugfp);}
#	define OPENDEBUG(file)\
	if ((debugfp = fopen(file,"w")) == NULL)\
	{	debugfp = stderr;\
		debug1("OPENDEBUG: sorry, cannot open '%s'\n", file);\
		debug("           beware, using stderr!\n");\
		sleep(3);\
	}
#else
#	define debug(x)
#	define debug1(x,a)
#	define debug2(x,a,b)
#	define debug3(x,a,b,c)
#	define OPENDEBUG(file)
#endif
