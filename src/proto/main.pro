/* main.c */
void main_loop __ARGS((int cmdwin));
void getout __ARGS((int exitval));
int process_env __ARGS((char_u *env, int is_viminit));
void mainerr_arg_missing __ARGS((char_u *str));
void time_push __ARGS((void *tv_rel, void *tv_start));
void time_pop __ARGS((void *tp));
void time_msg __ARGS((char *msg, void *tv_start));
void server_to_input_buf __ARGS((char_u *str));
int toF_TyA __ARGS((int c));
int fkmap __ARGS((int c));
void conv_to_pvim __ARGS((void));
void conv_to_pstd __ARGS((void));
char_u *lrswap __ARGS((char_u *ibuf));
char_u *lrFswap __ARGS((char_u *cmdbuf, int len));
char_u *lrF_sub __ARGS((char_u *ibuf));
int cmdl_fkmap __ARGS((int c));
int F_isalpha __ARGS((int c));
int F_isdigit __ARGS((int c));
int F_ischar __ARGS((int c));
void farsi_fkey __ARGS((cmdarg_T *cap));
/* vim: set ft=c : */
