/* ops.c */
void doshift __PARMS((int op));
void shift_line __PARMS((int left));
int dorecord __PARMS((int c));
int doexecbuf __PARMS((int c));
int insertbuf __PARMS((int c));
void dodelete __PARMS((void));
void dotilde __PARMS((void));
void swapchar __PARMS((struct fpos *pos));
void dochange __PARMS((void));
void init_yank __PARMS((void));
int doyank __PARMS((int deleting));
void doput __PARMS((int dir, long count));
void dodis __PARMS((void));
void dodojoin __PARMS((long count, int flag, int redraw));
int dojoin __PARMS((int insert_space));
void doformat __PARMS((void));
void startinsert __PARMS((int initstr, int startln, long count));
