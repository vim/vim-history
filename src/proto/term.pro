/* term.c */
void set_term __PARMS((char *term));
char *tgoto __PARMS((char *cm, int x, int y));
void termcapinit __PARMS((char *term));
void outstrn __PARMS((char *s));
void outstr __PARMS((char *s));
void windgoto __PARMS((int row, int col));
void setcursor __PARMS((void));
void ttest __PARMS((int pairs));
int inchar __PARMS((int async));
void outnum __PARMS((int n));
void check_winsize __PARMS((void));
void set_winsize __PARMS((int width, int height, int mustset));
void settmode __PARMS((int raw));
