/* ex_cmds2.c */
void do_debug __ARGS((char_u *cmd));
void ex_debug __ARGS((exarg_T *eap));
void dbg_check_breakpoint __ARGS((exarg_T *eap));
int dbg_check_skipped __ARGS((exarg_T *eap));
void ex_breakadd __ARGS((exarg_T *eap));
void ex_debuggreedy __ARGS((exarg_T *eap));
void ex_breakdel __ARGS((exarg_T *eap));
void ex_breaklist __ARGS((exarg_T *eap));
linenr_T dbg_find_breakpoint __ARGS((int file, char_u *fname, linenr_T after));
void dbg_breakpoint __ARGS((char_u *name, linenr_T lnum));
int autowrite __ARGS((buf_T *buf, int forceit));
void autowrite_all __ARGS((void));
int check_changed __ARGS((buf_T *buf, int checkaw, int mult_win, int forceit, int allbuf));
void dialog_changed __ARGS((buf_T *buf, int checkall));
int can_abandon __ARGS((buf_T *buf, int forceit));
int check_changed_any __ARGS((int hidden));
int check_fname __ARGS((void));
int buf_write_all __ARGS((buf_T *buf, int forceit));
char_u *do_one_arg __ARGS((char_u *str));
void set_arglist __ARGS((char_u *str));
void check_arg_idx __ARGS((win_T *win));
void ex_args __ARGS((exarg_T *eap));
void ex_previous __ARGS((exarg_T *eap));
void ex_rewind __ARGS((exarg_T *eap));
void ex_last __ARGS((exarg_T *eap));
void ex_argument __ARGS((exarg_T *eap));
void do_argfile __ARGS((exarg_T *eap, int argn));
void ex_next __ARGS((exarg_T *eap));
void ex_argedit __ARGS((exarg_T *eap));
void ex_argadd __ARGS((exarg_T *eap));
void ex_argdelete __ARGS((exarg_T *eap));
void ex_listdo __ARGS((exarg_T *eap));
void ex_compiler __ARGS((exarg_T *eap));
void ex_runtime __ARGS((exarg_T *eap));
int cmd_runtime __ARGS((char_u *name, int all));
int do_in_runtimepath __ARGS((char_u *name, int all, void (*callback)(char_u *fname)));
void ex_options __ARGS((exarg_T *eap));
void ex_source __ARGS((exarg_T *eap));
linenr_T *source_breakpoint __ARGS((void *cookie));
int *source_dbg_tick __ARGS((void *cookie));
int source_level __ARGS((void *cookie));
int do_source __ARGS((char_u *fname, int check_other, int is_vimrc));
void ex_scriptnames __ARGS((exarg_T *eap));
void scriptnames_slash_adjust __ARGS((void));
char_u *get_scriptname __ARGS((scid_T id));
char *fgets_cr __ARGS((char *s, int n, FILE *stream));
char_u *getsourceline __ARGS((int c, void *cookie, int indent));
void ex_scriptencoding __ARGS((exarg_T *eap));
void ex_finish __ARGS((exarg_T *eap));
void do_finish __ARGS((exarg_T *eap, int reanimate));
int source_finished __ARGS((void *cookie));
void ex_checktime __ARGS((exarg_T *eap));
int get_printer_page_num __ARGS((void));
int prt_header_height __ARGS((void));
int prt_use_number __ARGS((void));
int prt_get_unit __ARGS((int idx));
void ex_hardcopy __ARGS((exarg_T *eap));
void mch_print_cleanup __ARGS((void));
int mch_print_init __ARGS((prt_settings_T *psettings, char_u *jobname, int forceit));
int mch_print_begin __ARGS((prt_settings_T *psettings));
void mch_print_end __ARGS((prt_settings_T *psettings));
int mch_print_end_page __ARGS((void));
int mch_print_begin_page __ARGS((char_u *str));
int mch_print_blank_page __ARGS((void));
void mch_print_start_line __ARGS((int margin, int page_line));
int mch_print_text_out __ARGS((char_u *p, int len));
void mch_print_set_font __ARGS((int iBold, int iItalic, int iUnderline));
void mch_print_set_bg __ARGS((long_u bgcol));
void mch_print_set_fg __ARGS((long_u fgcol));
void set_lang_var __ARGS((void));
void ex_language __ARGS((exarg_T *eap));
char_u *get_lang_arg __ARGS((expand_T *xp, int idx));
/* vim: set ft=c : */
