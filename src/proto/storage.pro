/* storage.c */
char *m_blockalloc __PARMS((unsigned long size, int message));
void m_blockfree __PARMS((void));
void free_line __PARMS((char *ptr));
char *alloc_line __PARMS((unsigned int size));
char *save_line __PARMS((char *src));
void filealloc __PARMS((void));
void freeall __PARMS((void));
char *nr2ptr __PARMS((long nr));
char *pos2ptr __PARMS((struct fpos *pos));
char *Curpos2ptr __PARMS((void));
void setmarked __PARMS((long lnum));
long firstmarked __PARMS((void));
void clearmarked __PARMS((void));
long ptr2nr __PARMS((char *ptr, long start));
int appendline __PARMS((long after, char *s));
char *delsline __PARMS((long nr, int delmarks));
char *replaceline __PARMS((long lnum, char *new));
int canincrease __PARMS((int n));
