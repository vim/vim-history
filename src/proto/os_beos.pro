/* os_beos.c */
void check_for_bebox __ARGS((void));
void beos_cleanup_read_thread __ARGS((void));
int beos_select __ARGS((int nbits, struct fd_set *rbits, struct fd_set *wbits, struct fd_set *ebits, struct timeval *timeout));
