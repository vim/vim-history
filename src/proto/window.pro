/* window.c */
void do_window __PARMS((int nchar, long Prenum));
int win_split __PARMS((long new_height, int redraw));
void win_equal __PARMS((void));
void close_window __PARMS((int free_buf));
void win_init __PARMS((struct window *wp));
void win_enter __PARMS((struct window *wp, int undo_sync));
struct window *win_alloc __PARMS((struct window *prev, struct window *next));
void win_free __PARMS((struct window *wp));
int win_alloc_lsize __PARMS((struct window *wp));
void win_free_lsize __PARMS((struct window *wp));
void screen_new_rows __PARMS((void));
void win_setheight __PARMS((int height));
void win_comp_scroll __PARMS((struct window *wp));
void command_height __PARMS((void));
void last_status __PARMS((void));
