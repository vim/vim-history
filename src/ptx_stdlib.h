/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * ptx_stdlib.h: declarations which are needed for sequent
 */

extern void free(void *);
#ifdef SIZE_T /* sys/types.h */
extern void *malloc(size_t);
#else
extern void *malloc(unsigned);
#endif

#ifndef _FCNTL_H_
extern int open(char *, int, ...);
#endif
extern int close(int);
extern int read(int, char *, unsigned);
extern int write(int, char *, unsigned);
extern int ioctl(int, int, ...);
extern int unlink(char *);

extern char *getenv(char *);

#ifdef _NFILE /* stdio.h */
extern int _filbuf(FILE *);
extern int _flsbuf(unsigned char, FILE *);
#endif

#ifdef _POLL_H_
extern int poll(struct pollfd[], unsigned long, int);
#endif /* _POLL_H_ */

extern char *getcwd(char *, int);

extern int chdir(char *);

extern int atoi (char *);
extern long atol(char *);
extern long strtol(char *, char **, int);

extern int isatty(int);
