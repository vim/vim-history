/* regexp.c */
unsigned char *skip_regexp __PARMS((unsigned char *p, int dirc));
struct regexp *regcomp __PARMS((unsigned char *exp));
int regexec __PARMS((struct regexp *prog, unsigned char *string, int at_bol));
int cstrncmp __PARMS((unsigned char *s1, unsigned char *s2, int n));
unsigned char *cstrchr __PARMS((unsigned char *s, int c));
