/*
 * ptx_stdlib.h: declarations which are needed for sequent
 */

extern void free(void *);
#ifdef SIZE_T /* sys/types.h */
extern void *malloc(size_t);
extern off_t lseek(int, off_t, int);
#else
extern void *malloc(unsigned);
extern int lseek(int, int, int);
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
extern int getuid(void);
extern int getgid(void);

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
