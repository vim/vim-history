/* regsub.c */
unsigned char *regtilde __PARMS((unsigned char *source, int magic));
int regsub __PARMS((struct regexp *prog, unsigned char *source, unsigned char *dest, int copy, int magic));
