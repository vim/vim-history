/* os_mswin.c */
void mch_init __ARGS((void));
int mch_input_isatty __ARGS((void));
void mch_settitle __ARGS((char_u *title, char_u *icon));
void mch_restore_title __ARGS((int which));
int mch_can_restore_title __ARGS((void));
int mch_can_restore_icon __ARGS((void));
int mch_FullName __ARGS((char_u *fname, char_u *buf, int len, int force));
int mch_isFullName __ARGS((char_u *fname));
void slash_adjust __ARGS((char_u *p));
int vim_stat __ARGS((const char *name, struct stat *stp));
void mch_suspend __ARGS((void));
void mch_errmsg __ARGS((char *str));
void mch_display_error __ARGS((void));
int mch_has_wildcard __ARGS((char_u *s));
int mch_chdir __ARGS((char *path));
int can_end_termcap_mode __ARGS((int give_msg));
int mch_screenmode __ARGS((char_u *arg));
void clip_mch_lose_selection __ARGS((void));
int clip_mch_own_selection __ARGS((void));
void DumpPutS __ARGS((const char *psz));
/* vim: set ft=c : */
