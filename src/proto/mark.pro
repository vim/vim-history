/* mark.c */
int setmark __PARMS((int c));
void setpcmark __PARMS((void));
void checkpcmark __PARMS((void));
struct fpos *movemark __PARMS((int count));
struct fpos *getmark __PARMS((int c, int changefile));
void clrallmarks __PARMS((struct buffer *buf));
unsigned char *fm_getname __PARMS((struct filemark *fmark));
void domarks __PARMS((void));
void dojumps __PARMS((void));
void mark_adjust __PARMS((long line1, long line2, long inc));
