/* os_beos.c */
extern void beos_cleanup_read_thread __ARGS((void));
extern int beos_select __ARGS((int nbits, struct fd_set *rbits, struct fd_set *wbits, struct fd_set *ebits, struct timeval *timeout));
/* vim: set ft=c : */
