/* quickfix.c */
int qf_init __PARMS((void));
void qf_jump __PARMS((int dir, int errornr));
void qf_list __PARMS((void));
void qf_mark_adjust __PARMS((long line1, long line2, long inc));
