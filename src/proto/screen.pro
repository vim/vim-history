/* screen.c */
void updateline __PARMS((void));
void updateScreen __PARMS((int type));
void prt_line __PARMS((char *s));
void screenalloc __PARMS((void));
void screenclear __PARMS((void));
void cursupdate __PARMS((void));
void curs_columns __PARMS((void));
int getvcol __PARMS((struct fpos *pos, int type));
void scrolldown __PARMS((long nlines));
void scrollup __PARMS((long nlines));
int s_ins __PARMS((int row, int nlines, int invalid));
int s_del __PARMS((int row, int nlines, int invalid));
void showmode __PARMS((void));
void delmode __PARMS((void));
void showruler __PARMS((int always));
void clear_line __PARMS((void));
