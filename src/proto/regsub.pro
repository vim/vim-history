/* regsub.c */
char *regtilde __PARMS((char *source, int magic));
int regsub __PARMS((struct regexp *prog, char *source, char *dest, int copy, int magic));
