/* charset.c */
char *transchar __PARMS((unsigned int c));
int outtrans __PARMS((char *str, int len));
int charsize __PARMS((int c));
int strsize __PARMS((char *s));
int chartabsize __PARMS((int c, int col));
int isidchar __PARMS((int c));
