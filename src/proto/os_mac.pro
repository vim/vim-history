/* os_mac.c */
void mch_setmouse __PARMS((int on));

void mch_windexit __PARMS((int r));
int mch_check_win __PARMS((int argc, char **argv));
int mch_input_isatty __PARMS((void));

void fname_case __PARMS((char_u *name));
void mch_settitle __PARMS((char_u *title, char_u *icon));
void mch_restore_title __PARMS((int which));
#define mch_can_restore_title() TRUE
#define mch_can_restore_icon()  TRUE

int mch_get_user_name __PARMS((char_u *s, int len));
void mch_get_host_name __PARMS((char_u *s, int len));
long mch_get_pid __PARMS((void));

int mch_dirname __PARMS((char_u *buf, int len));
int mch_FullName __PARMS((char_u *fname, char_u *buf, int len, int force));
int mch_isFullName __PARMS((char_u *fname));
void slash_adjust __PARMS((char_u *p));
long mch_getperm __PARMS((char_u *name));
int mch_setperm __PARMS((char_u *name, long perm));
void mch_hide __ARGS((char_u *name));
int mch_isdir __PARMS((char_u *name));
int mch_can_exe __ARGS((char_u *name));
int mch_nodetype __ARGS((char_u *name));
void mch_init __ARGS((void));
void mch_settmode __PARMS((int raw));

#define mch_get_winsize()		gui_get_winsize()
#define mch_set_winsize()		gui_set_winsize(FALSE)
#define mch_suspend			gui_mch_iconify
#if defined(__MRC__) || defined(__SC__)
int mch_chdir __ARGS((char *p_name));
int stat __ARGS((char *p, struct stat *p_st));
#endif

int mch_call_shell __PARMS((char_u *cmd, int options));
int mch_has_wildcard __PARMS((char_u *s));
int mch_expandpath __PARMS((struct growarray *gap, char_u *path, int flags));
int mac_expandpath __PARMS((struct growarray *gap, char_u *path, int flags, short start_at, short as_full));
/*int vim_chdir __PARMS((char *path));*/
void mch_delay __PARMS((long msec, int ignoreinput));
void mch_breakcheck __PARMS((void));
long_u mch_avail_mem __ARGS((int special));
int mch_screenmode __PARMS((char_u *arg));

#define mch_break_check			gui_mch_update

void slash_n_colon_adjust __PARMS((char_u *buf));
void GetFullPathFromFSSpec (char_u *fname, FSSpec file);
int mch_copy_file(char_u *from, char_u *to);

int C2PascalString (char_u *CString, Str255 *PascalString);
int GetFSSpecFromPath ( char_u *file, FSSpec *fileFSSpec);
int mch_has_resource_fork (char_u *file);
int mch_copy_file_attribute(char_u *from, char_u *to);

void mch_shellinit __ARGS((void));
int mch_get_shellsize __ARGS((void));
void mch_set_shellsize __ARGS((void));
void mch_new_shellsize __ARGS((void));

/* vim: set ft=c : */
