/* undo.c */
int u_save_cursor __PARMS((void));
int u_save __PARMS((long top, long bot));
int u_savesub __PARMS((long lnum));
int u_inssub __PARMS((long lnum));
int u_savedel __PARMS((long lnum, long nlines));
void u_undo __PARMS((int count));
void u_redo __PARMS((int count));
void u_sync __PARMS((void));
void u_unchanged __PARMS((struct buffer *buf));
void u_clearall __PARMS((struct buffer *buf));
void u_saveline __PARMS((long lnum));
void u_clearline __PARMS((void));
void u_undoline __PARMS((void));
void u_blockfree __PARMS((struct buffer *buf));
