/* mark.c */
int setmark __PARMS((int c));
void setpcmark __PARMS((void));
struct fpos *movemark __PARMS((int count));
struct fpos *getmark __PARMS((int c, int changefile));
void clrallmarks __PARMS((void));
void incrmarks __PARMS((void));
void decrmarks __PARMS((void));
void adjustmark __PARMS((char *old, char *new));
char *fm_getname __PARMS((struct filemark *fmark));
void domarks __PARMS((void));
void dojumps __PARMS((void));
