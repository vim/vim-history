/* message.c */
void msg __PARMS((char *s));
void emsg __PARMS((char *s));
void emsg2 __PARMS((char *s, char *a1));
void wait_return __PARMS((int redraw));
void start_msg __PARMS((void));
void end_msg __PARMS((void));
void check_msg __PARMS((void));
