/* param.c */
void set_init __PARMS((void));
int doset __PARMS((unsigned char *arg));
void paramchanged __PARMS((unsigned char *arg));
int makeset __PARMS((struct __stdio *fd));
void clear_termparam __PARMS((void));
void comp_col __PARMS((void));
void win_copy_options __PARMS((struct window *wp_from, struct window *wp_to));
void buf_copy_options __PARMS((struct buffer *bp_from, struct buffer *bp_to));
void set_context_in_set_cmd __PARMS((unsigned char *arg));
int ExpandSettings __PARMS((struct regexp *prog, int *num_file, unsigned char ***file));
