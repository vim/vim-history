/* fileio.c */
void filemess __PARMS((char *name, char *s));
int readfile __PARMS((char *fname, long from, int newfile));
int writeit __PARMS((char *fname, long start, long end, int append, int forceit));
