/* linefunc.c */
void coladvance __PARMS((unsigned int wcol));
int inc_cursor __PARMS((void));
int inc __PARMS((struct fpos *lp));
int incl __PARMS((struct fpos *lp));
int dec_cursor __PARMS((void));
int dec __PARMS((struct fpos *lp));
int decl __PARMS((struct fpos *lp));
void adjust_cursor __PARMS((void));
