/* param.c */
void set_init __PARMS((void));
int doset __PARMS((char *arg));
void paramchanged __PARMS((char *arg));
int makeset __PARMS((struct __stdio *fd));
void clear_termparam __PARMS((void));
void comp_col __PARMS((void));
