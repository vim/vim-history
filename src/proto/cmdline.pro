/* cmdline.c */
int getcmdline __PARMS((int firstc, unsigned char *buff));
void redrawcmdline __PARMS((void));
void compute_cmdrow __PARMS((void));
void redrawcmd __PARMS((void));
int docmdline __PARMS((unsigned char *cmdline));
void autowrite_all __PARMS((void));
void gotocmdline __PARMS((int clr, int firstc));
void gotocmdend __PARMS((void));
int check_fname __PARMS((void));
int getfile __PARMS((unsigned char *fname, unsigned char *sfname, int setpm));
unsigned char *ExpandOne __PARMS((unsigned char *str, int list_notfound, int mode));
int dosource __PARMS((unsigned char *fname));
