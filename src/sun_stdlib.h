/*
 * sun_stdlib.h: declararions used on a sun
 */

#ifndef __stdlib_h
extern void free(void *);
extern void *malloc(unsigned);
extern void *realloc(void *, unsigned);
extern void *calloc(unsigned, unsigned);
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

#ifndef __sys_fcntlcom_h
extern int open(char *, int, ...);
#endif
extern int close(int);
extern int read(int, char *, unsigned);
extern int write(int, char *, unsigned);
extern int ioctl(int, int, ...);
extern int unlink(char *);

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
extern char *getenv(char *);

extern char *getcwd(char *, int);
extern char *getwd(char *);

extern int chdir(char *);

extern int atoi (char *);
extern long atol(char *);
extern long strtol(char * , char **, int);

extern void bcopy(char *, char *, int);
extern int bcmp(char *, char *, int);
extern void bzero(char *, int);

extern char *memccpy(char *, char *, int, int);
extern char *memchr(char *, int, int);
extern int memcmp(char *, char *, int);
extern char *memcpy(char *, char *, int);
extern char *memset(char *, int, int);

extern int strcmp(char *, char *);
extern int strncmp(char *, char *, int);
extern int strcasecmp(char *, char *);

extern int toupper(int);
extern int tolower(int);
extern int isatty(int);
