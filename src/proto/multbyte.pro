/* multbyte.c */
int IsLeadByte __ARGS((int c));
int IsTrailByte __ARGS((char_u *base, char_u *p));
int AdjustCursorForMultiByteCharacter __ARGS((void));
int MultiStrLen __ARGS((char_u *str));
int han_dec __ARGS((FPOS *lp));
