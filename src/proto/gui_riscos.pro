/* Prototypes.
 * Stolen from gui_x11_pro.h
 */
void gui_mch_prepare (int *argc, char **argv);
int gui_mch_init (void);
void gui_mch_new_colors (void);
int gui_mch_open (void);
void gui_mch_exit (int rc);
void gui_mch_set_winsize (int width, int height, int min_width, int min_height, int base_width, int base_height);
void gui_mch_get_winpos __ARGS((int *x, int *y));
void gui_mch_set_winpos __ARGS((int x, int y));
void gui_mch_get_screen_dimensions (int *screen_w, int *screen_h);
int gui_mch_init_font (char_u *font_name);
GuiFont gui_mch_get_font (char_u *name, int giveErrorIfMissing);
void gui_mch_set_font (GuiFont font);
int gui_mch_same_font (GuiFont f1, GuiFont f2);
void gui_mch_free_font (GuiFont font);
GuiColor gui_mch_get_color (char_u *name);
void gui_mch_set_colors (GuiColor gf, GuiColor bg);
void gui_mch_draw_string (int row, int col, char_u *s, int len, int flags);
int gui_mch_haskey (char_u *name);
void gui_mch_beep (void);
void gui_mch_flash (int msec);
void gui_mch_invert_rectangle (int r, int c, int nr, int nc);
void gui_mch_iconify (void);
void gui_mch_draw_hollow_cursor (GuiColor color);
void gui_mch_draw_part_cursor (int w, int h, GuiColor color);
void gui_mch_update (void);
int gui_mch_wait_for_chars (long wtime);
void gui_mch_flush (void);
void gui_mch_clear_block (int row1, int col1, int row2, int col2);
void gui_mch_clear_all (void);
void gui_mch_delete_lines (int row, int num_lines);
void gui_mch_insert_lines (int row, int num_lines);
void clip_mch_request_selection (void);
void clip_mch_lose_selection (void);
int clip_mch_own_selection (void);
void clip_mch_set_selection (void);
void gui_mch_menu_grey (GuiMenu *menu, int grey);
void gui_mch_menu_hidden (GuiMenu *menu, int hidden);
void gui_mch_draw_menubar (void);
void gui_mch_enable_scrollbar (GuiScrollbar *sb, int flag);
void gui_mch_set_blinking (long waittime, long on, long off);
void gui_mch_stop_blink (void);
void gui_mch_start_blink (void);
int gui_mch_get_lightness (GuiColor pixel);
long_u gui_mch_get_rgb (GuiColor pixel);
void gui_mch_show_popupmenu(GuiMenu *menu);

void process_event(int event, int *block);
void plot_rectangle(int plot, int col, int minx, int miny, int maxx, int maxy);
void ro_redraw_title(int window);
char_u *gui_mch_browse (int saving, char_u *title, char_u *dflt, char_u *ext, char_u *initdir, char_u *filter);
int ro_ok_to_quit(void);
