/* cmdline.c */
int getcmdline __PARMS((int firstc, unsigned char *buff));
void redrawcmdline __PARMS((void));
void redrawcmd __PARMS((void));
void docmdline __PARMS((unsigned char *cmdline));
int doarglist __PARMS((char *str));
void gotocmdline __PARMS((int clr, int firstc));
int getfile __PARMS((char *fname, int setpm));
int samealtfile __PARMS((int n));
int getaltfile __PARMS((int n, long lnum, int setpm));
char *getaltfname __PARMS((int n));
int dosource __PARMS((char *fname));
