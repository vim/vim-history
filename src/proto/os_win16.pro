/* os_win16.c */
void mch_setmouse __ARGS((int on));
void mch_init __ARGS((void));
int mch_check_win __ARGS((int argc, char **argv));
int mch_get_user_name __ARGS((char_u *s, int len));
void mch_get_host_name __ARGS((char_u *s, int len));
long mch_get_pid __ARGS((void));
int mch_dirname __ARGS((char_u *buf, int len));
long mch_getperm __ARGS((char_u *name));
int mch_setperm __ARGS((char_u *name, long perm));
void mch_hide __ARGS((char_u *name));
int mch_isdir __ARGS((char_u *name));
int mch_can_exe __ARGS((char_u *name));
int mch_nodetype __ARGS((char_u *name));
int mch_call_shell __ARGS((char_u *cmd, int options));
void mch_delay __ARGS((long msec, int ignoreinput));
int mch_remove __ARGS((char_u *name));
void mch_breakcheck __ARGS((void));
long_u mch_avail_mem __ARGS((int special));
int mch_rename __ARGS((const char *pszOldFile, const char *pszNewFile));
char_u *mch_getenv __ARGS((char_u *var));
char *default_shell __ARGS((void));
/* vim: set ft=c : */
