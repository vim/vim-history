/* regexp.c */
struct regexp *regcomp __PARMS((char *exp));
int regexec __PARMS((struct regexp *prog, char *string, int at_bol));
char *cstrchr __PARMS((char *s, int c));
