/* cmdcmds.c */
void do_align __PARMS((long start, long end, int width, int type));
int do_move __PARMS((long line1, long line2, long n));
void do_copy __PARMS((long line1, long line2, long n));
void dobang __PARMS((int addr_count, long line1, long line2, int forceit, unsigned char *arg));
void doshell __PARMS((unsigned char *cmd));
void dofilter __PARMS((long line1, long line2, unsigned char *buff, int do_in, int do_out));
