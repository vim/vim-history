/* buffer.c */
int open_buffer __PARMS((unsigned char *fname, int *oldbuf, int free_buf));
struct buffer *buf_alloc __PARMS((void));
void close_buffer __PARMS((struct buffer *buf, int free_buf));
void buf_init __PARMS((struct buffer *buf));
void buf_freeall __PARMS((struct buffer *buf));
void do_buffers __PARMS((void));
