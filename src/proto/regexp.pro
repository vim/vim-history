/* regexp.c */
int re_multiline __ARGS((regprog_t *prog));
char_u *skip_regexp __ARGS((char_u *p, int dirc, int magic));
regprog_t *vim_regcomp __ARGS((char_u *expr, int magic));
int vim_regcomp_had_eol __ARGS((void));
int vim_regexec __ARGS((regmatch_t *rmp, char_u *line, colnr_t col));
long vim_regexec_multi __ARGS((regmmatch_t *rmp, win_t *win, buf_t *buf, linenr_t lnum, colnr_t col));
reg_extmatch_t *ref_extmatch __ARGS((reg_extmatch_t *em));
void unref_extmatch __ARGS((reg_extmatch_t *em));
char_u *regtilde __ARGS((char_u *source, int magic));
int vim_regsub __ARGS((regmatch_t *rmp, char_u *source, char_u *dest, int copy, int magic, int backslash));
int vim_regsub_multi __ARGS((regmmatch_t *rmp, linenr_t lnum, char_u *source, char_u *dest, int copy, int magic, int backslash));
char_u *reg_submatch __ARGS((int no));
/* vim: set ft=c : */
