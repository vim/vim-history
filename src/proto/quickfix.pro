/* quickfix.c */
int qf_init __ARGS((char_u *efile));
void qf_jump __ARGS((int dir, int errornr, int forceit));
void qf_list __ARGS((int all));
void qf_mark_adjust __ARGS((linenr_t line1, linenr_t line2, long amount, long amount_after));
