/* undo.c */
int u_saveCurpos __PARMS((void));
int u_save __PARMS((long top, long bot));
int u_savesub __PARMS((long lnum, char *ptr));
int u_savedel __PARMS((long lnum, char *ptr));
void u_undo __PARMS((int count));
void u_redo __PARMS((int count));
void u_sync __PARMS((void));
void u_clearall __PARMS((void));
void u_saveline __PARMS((long lnum));
void u_clearline __PARMS((void));
void u_undoline __PARMS((void));
