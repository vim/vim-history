/*
 * debug.h -- my debug header for just any program.
 * use:
 * place the statement OPENDEBUG; inside main.
 * nothing will happen, unless you compile the source with
 * -DDEBUG="/tmp/programdebug"
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
#	define OPENDEBUG
#endif

#undef NOASSERT

#ifndef NOASSERT
# if defined(__STDC__)
#  define ASSERT(lousy_cpp) {if (!(lousy_cpp)) {debug2("ASSERT("#lousy_cpp")ion failed file %s line %d\n", __FILE__, __LINE__);abort();}}
# else
#  define ASSERT(lousy_cpp) {if (!(lousy_cpp)) {debug2("ASSERT(lousy_cpp)ion failed file %s line %d\n", __FILE__, __LINE__);abort();}}
# endif
#else
# define ASSERT(lousy_cpp) {;}
#endif
