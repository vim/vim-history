/* eval.c */
void set_internal_string_var __ARGS((char_u *name, char_u *value));
int eval_to_bool __ARGS((char_u *arg, int *error, char_u **nextcmd));
char_u *eval_to_string __ARGS((char_u *arg, char_u **nextcmd));
void do_let __ARGS((EXARG *eap));
void do_unlet __ARGS((char_u *arg));
char_u *get_env_string __ARGS((char_u **arg));
void var_init __ARGS((struct growarray *gap));
void var_clear __ARGS((struct growarray *gap));
void do_echo __ARGS((EXARG *eap, int echo));
void do_execute __ARGS((EXARG *eap, char_u *(*getline)(int, void *, int), void *cookie));
