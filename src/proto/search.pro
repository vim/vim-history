/* search.c */
struct regexp *myregcomp __PARMS((char *pat));
int searchit __PARMS((struct fpos *pos, int dir, char *str, long count, int end));
int dosearch __PARMS((int dirc, char *str, int reverse, long count, int echo));
int searchc __PARMS((int c, int dir, int type, long count));
struct fpos *showmatch __PARMS((void));
int findfunc __PARMS((int dir, int what, long count));
int findsent __PARMS((int dir, long count));
int findpar __PARMS((int dir, long count, int what));
int startPS __PARMS((long lnum, int para));
int fwd_word __PARMS((long count, int type, int eol));
int bck_word __PARMS((long count, int type));
int end_word __PARMS((long count, int type, int stop));
int skip_chars __PARMS((int class, int dir));
