/* fileio.c */
void filemess __PARMS((unsigned char *name, unsigned char *s));
int readfile __PARMS((unsigned char *fname, unsigned char *sfname, long from, int newfile, long skip_lnum, long nlines));
int buf_write __PARMS((struct buffer *buf, unsigned char *fname, unsigned char *sfname, long start, long end, int append, int forceit, int reset_changed));
unsigned char *modname __PARMS((unsigned char *fname, unsigned char *ext));
unsigned char *buf_modname __PARMS((struct buffer *buf, unsigned char *fname, unsigned char *ext));
int vim_fgets __PARMS((unsigned char *buf, int size, struct __stdio *fp, int *lnum));
