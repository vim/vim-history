/* fileio.c */
void filemess __PARMS((char *name, char *s));
int readfile __PARMS((char *fname, char *sfname, long from, int newfile));
int writeit __PARMS((char *fname, char *sfname, long start, long end, int append, int forceit, int whole));
