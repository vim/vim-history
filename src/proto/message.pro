/* message.c */
int msg __PARMS((unsigned char *s));
int emsg __PARMS((unsigned char *s));
int emsg2 __PARMS((unsigned char *s, unsigned char *a1));
void wait_return __PARMS((int redraw));
void msg_start __PARMS((void));
void msg_pos __PARMS((int row, int col));
void msg_outchar __PARMS((int c));
void msg_outnum __PARMS((long n));
int msg_outtrans __PARMS((unsigned char *str, int len));
void msg_prt_line __PARMS((unsigned char *s));
void msg_outstr __PARMS((unsigned char *s));
void msg_ceol __PARMS((void));
int msg_end __PARMS((void));
int msg_check __PARMS((void));
