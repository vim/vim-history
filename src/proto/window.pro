/* window.c */
void do_window __PARMS((int nchar, long Prenum));
int win_split __PARMS((long new_height, int redraw));
int make_windows __PARMS((int count));
void win_equal __PARMS((struct window *next_curwin, int redraw));
void close_window __PARMS((int free_buf));
void close_others __PARMS((int message));
void win_init __PARMS((struct window *wp));
void win_enter __PARMS((struct window *wp, int undo_sync));
struct window *win_alloc __PARMS((struct window *after));
void win_free __PARMS((struct window *wp));
int win_alloc_lsize __PARMS((struct window *wp));
void win_free_lsize __PARMS((struct window *wp));
void screen_new_rows __PARMS((void));
void win_setheight __PARMS((int height));
void win_comp_scroll __PARMS((struct window *wp));
void command_height __PARMS((void));
void last_status __PARMS((void));
unsigned char *file_name_at_cursor __PARMS((void));
