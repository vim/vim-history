/* undo.c */
int u_save_cursor __ARGS((void));
int u_save __ARGS((linenr_t top, linenr_t bot));
int u_savesub __ARGS((linenr_t lnum));
int u_inssub __ARGS((linenr_t lnum));
int u_savedel __ARGS((linenr_t lnum, long nlines));
void u_undo __ARGS((int count));
void u_redo __ARGS((int count));
void u_sync __ARGS((void));
void u_unchanged __ARGS((buf_t *buf));
void u_clearall __ARGS((buf_t *buf));
void u_saveline __ARGS((linenr_t lnum));
void u_clearline __ARGS((void));
void u_undoline __ARGS((void));
void u_blockfree __ARGS((buf_t *buf));
int bufIsChanged __ARGS((buf_t *buf));
int curbufIsChanged __ARGS((void));
