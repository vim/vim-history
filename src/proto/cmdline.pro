/* cmdline.c */
int getcmdline __PARMS((int firstc, unsigned char *buff));
void redrawcmdline __PARMS((void));
void compute_cmdrow __PARMS((void));
void redrawcmd __PARMS((void));
int docmdline __PARMS((unsigned char *cmdline));
int autowrite __PARMS((struct buffer *buf));
void autowrite_all __PARMS((void));
int doecmd __PARMS((unsigned char *fname, unsigned char *sfname, unsigned char *command, int hide, long newlnum));
void gotocmdline __PARMS((int clr, int firstc));
void gotocmdend __PARMS((void));
int check_fname __PARMS((void));
int getfile __PARMS((unsigned char *fname, unsigned char *sfname, int setpm, long lnum));
unsigned char *ExpandOne __PARMS((unsigned char *str, int list_notfound, int mode));
int dosource __PARMS((unsigned char *fname));
