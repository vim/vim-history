/* linefunc.c */
void coladvance __PARMS((unsigned int wcol));
int inc __PARMS((struct fpos *lp));
int incCurpos __PARMS((void));
int incl __PARMS((struct fpos *lp));
int dec __PARMS((struct fpos *lp));
int decCurpos __PARMS((void));
int decl __PARMS((struct fpos *lp));
void adjustCurpos __PARMS((void));
