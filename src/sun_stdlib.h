/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * sun_stdlib.h: declararions used on a sun
 */

#ifndef __stdlib_h
extern int atoi (char *);
extern long atol(char *);
extern void free(void *);
extern char *getenv(char *);
extern void *malloc(unsigned);
extern void *realloc(void *, unsigned);
extern void *calloc(unsigned, unsigned);
extern int read(int, char *, unsigned);
extern int write(int, char *, unsigned);
extern int unlink(char *);
#endif

#ifdef __sys_types_h
extern off_t lseek(int, off_t, int);

# ifdef _sys_time_h
extern int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
# endif _sys_time_h

#else
extern long lseek(int, long, int);
#endif

extern long tell(int);
extern void perror(char *);

#include <fcntl.h>

extern int close(int);

#ifdef FILE
extern int _filbuf(FILE *);
extern int _flsbuf(unsigned char, FILE *);
extern int fclose(FILE *);
extern int fprintf(FILE *, char *, ...);
extern int fscanf(FILE *, char *, ...);
extern int fseek(FILE *, long, int);
extern int fflush(FILE *);
extern int fread(char *, int, int, FILE *);
#else
extern char *sprintf(char *, char *, ...);
#endif

extern int printf(char *, ...);

extern int scanf(char *, ...);
extern int sscanf(char *, char *, ...);

extern int system(char *);

#ifndef __sys_unistd_h
extern char *getcwd(char *, int);
extern int chdir(char *);
extern int getuid(void);
extern int getgid(void);
#endif /* __sys_unistd_h */

extern long strtol(char * , char **, int);

extern char *memccpy(char *, char *, int, int);
extern char *memchr(char *, int, int);
extern char *memset(char *, int, int);

#include <string.h>
extern int strcasecmp(char *, char *);

extern int toupper(int);
extern int tolower(int);
extern int isatty(int);
