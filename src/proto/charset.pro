/* charset.c */
unsigned char *transchar __PARMS((int c));
int charsize __PARMS((int c));
int strsize __PARMS((unsigned char *s));
int chartabsize __PARMS((int c, long col));
int isidchar __PARMS((int c));
