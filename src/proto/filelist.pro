/* filelist.c */
int filelist_getfile __PARMS((int n, long lnum, int setpm));
void filelist_getlnum __PARMS((void));
unsigned char *filelist_nr2name __PARMS((int n));
void filelist_list __PARMS((void));
int filelist_name_nr __PARMS((int fnum, unsigned char **fname, long *lnum));
void setfname __PARMS((unsigned char *s, unsigned char *ss));
void setaltfname __PARMS((unsigned char *fname, unsigned char *sfname, long lnum));
int filelist_add __PARMS((unsigned char *fname));
void filelist_altlnum __PARMS((void));
int otherfile __PARMS((unsigned char *fname));
unsigned char *fix_fname __PARMS((unsigned char *fname));
void fileinfo __PARMS((int fullname));
void maketitle __PARMS((void));
void resettitle __PARMS((void));
