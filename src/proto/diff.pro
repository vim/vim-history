/* diff.c */
void diff_new_buffer __ARGS((void));
void diff_buf_delete __ARGS((buf_t *buf));
void diff_buf_add __ARGS((buf_t *buf));
void diff_invalidate __ARGS((void));
void diff_mark_adjust __ARGS((linenr_t line1, linenr_t line2, long amount, long amount_after));
void diff_redraw __ARGS((void));
void ex_diffupdate __ARGS((exarg_t *eap));
void ex_diffpatch __ARGS((exarg_t *eap));
void ex_diffsplit __ARGS((exarg_t *eap));
void diff_win_options __ARGS((win_t *wp, int addbuf));
int diff_check __ARGS((win_t *wp, linenr_t lnum));
int diff_check_fill __ARGS((win_t *wp, linenr_t lnum));
void diff_set_topline __ARGS((win_t *fromwin, win_t *towin));
int diffopt_changed __ARGS((void));
void diff_find_change __ARGS((win_t *wp, linenr_t lnum, int *startp, int *endp));
int diff_infold __ARGS((win_t *wp, linenr_t lnum));
/* vim: set ft=c : */
