/* cmdline.c */
int getcmdline __PARMS((int firstc, unsigned char *buff));
void redrawcmdline __PARMS((void));
void redrawcmd __PARMS((void));
void docmdline __PARMS((unsigned char *cmdline));
void gotocmdline __PARMS((int clr, int firstc));
void gotocmdend __PARMS((void));
int check_fname __PARMS((void));
int getfile __PARMS((char *fname, char *sfname, int setpm));
int samealtfile __PARMS((int n));
int getaltfile __PARMS((int n, long lnum, int setpm));
char *getaltfname __PARMS((int n));
char *ExpandOne __PARMS((unsigned char *str, int list_notfound, int mode));
int dosource __PARMS((char *fname));
