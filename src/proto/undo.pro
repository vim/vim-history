/* undo.c */
int u_save_cursor __PARMS((void));
int u_save __PARMS((linenr_t top, linenr_t bot));
int u_savesub __PARMS((linenr_t lnum));
int u_inssub __PARMS((linenr_t lnum));
int u_savedel __PARMS((linenr_t lnum, long nlines));
void u_undo __PARMS((int count));
void u_redo __PARMS((int count));
void u_sync __PARMS((void));
void u_unchanged __PARMS((BUF *buf));
void u_clearall __PARMS((BUF *buf));
void u_saveline __PARMS((linenr_t lnum));
void u_clearline __PARMS((void));
void u_undoline __PARMS((void));
void u_blockfree __PARMS((BUF *buf));
