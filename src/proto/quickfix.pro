/* quickfix.c */
int qf_init __ARGS((char_u *efile, char_u *errorformat));
void qf_jump __ARGS((int dir, int errornr, int forceit));
void qf_list __ARGS((exarg_t *eap));
void qf_age __ARGS((exarg_t *eap));
void qf_mark_adjust __ARGS((linenr_t line1, linenr_t line2, long amount, long amount_after));
void ex_cwindow __ARGS((exarg_t *eap));
int bt_quickfix __ARGS((buf_t *buf));
int bt_nofile __ARGS((buf_t *buf));
int bt_scratch __ARGS((buf_t *buf));
