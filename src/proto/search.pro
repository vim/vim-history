/* search.c */
struct regexp *myregcomp __PARMS((unsigned char *pat, int sub_cmd, int which_pat));
int searchit __PARMS((struct fpos *pos, int dir, unsigned char *str, long count, int end, int message));
int dosearch __PARMS((int dirc, unsigned char *str, int reverse, long count, int echo, int message));
int searchc __PARMS((int c, int dir, int type, long count));
struct fpos *showmatch __PARMS((int initc));
int findfunc __PARMS((int dir, int what, long count));
int findsent __PARMS((int dir, long count));
int findpar __PARMS((int dir, long count, int what, int both));
int startPS __PARMS((long lnum, int para, int both));
int fwd_word __PARMS((long count, int type, int eol));
int bck_word __PARMS((long count, int type));
int end_word __PARMS((long count, int type, int stop));
int skip_chars __PARMS((int class, int dir));
