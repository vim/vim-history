/* quickfix.c */
int qf_init __PARMS((char *fname));
void qf_jump __PARMS((int errornr));
void qf_list __PARMS((void));
void qf_clrallmarks __PARMS((void));
void qf_adjustmark __PARMS((char *old, char *new));
