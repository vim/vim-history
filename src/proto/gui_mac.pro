/* gui_mac.c */

/*
 * Mac specific prototypes
 */

pascal Boolean WaitNextEventWrp __PARMS((EventMask eventMask, EventRecord
*theEvent, UInt32 sleep, RgnHandle mouseRgn));
pascal void gui_mac_scroll_action __PARMS((ControlHandle theControl, short partCode));
pascal void gui_mac_drag_thumb __PARMS((ControlHandle theControl));
void gui_mac_handle_event __PARMS((EventRecord *event));
void gui_mac_doMouseDown __PARMS((EventRecord *theEvent));
void gui_mac_do_key __PARMS((EventRecord *theEvent));
void gui_mac_handle_menu __PARMS((long menuChoice));
void gui_mac_focus_change __PARMS((EventRecord *event));
void gui_mac_update __PARMS((EventRecord *event));
short gui_mch_get_mac_menu_item_index __PARMS((VimMenu *menu, VimMenu *parent));
void gui_mch_set_blinking __PARMS((long wait, long on, long off));
void gui_mch_stop_blink __PARMS((void));
void gui_mch_start_blink __PARMS((void));
int gui_mch_get_mouse_x __PARMS((void));
int gui_mch_get_mouse_y __PARMS((void));
void gui_mch_setmouse __PARMS((int x, int y));
void gui_mch_prepare __PARMS((int *argc, char **argv));
int gui_mch_init __PARMS((void));
void gui_mch_new_colors __PARMS((void));
int gui_mch_open __PARMS((void));
void gui_mch_exit __PARMS((int));
void gui_mch_set_winsize __PARMS((int width, int height, int min_width, int min_height, int base_width, int base_height));
int gui_mch_get_winpos __ARGS((int *x, int *y));
void gui_mch_set_winpos __ARGS((int x, int y));
void gui_mch_get_screen_dimensions __PARMS((int *screen_w, int *screen_h));
void gui_mch_set_text_area_pos __PARMS((int x, int y, int w, int h));
void gui_mch_enable_scrollbar __PARMS((GuiScrollbar *sb, int flag));
void gui_mch_set_scrollbar_thumb __PARMS((GuiScrollbar *sb, int val, int size, int max));
void gui_mch_set_scrollbar_pos __PARMS((GuiScrollbar *sb, int x, int y, int w, int h));
void gui_mch_create_scrollbar __PARMS((GuiScrollbar *sb, int orient));
void gui_mch_destroy_scrollbar __PARMS((GuiScrollbar *sb));
int gui_mch_init_font __PARMS((char_u *font_name));
GuiFont gui_mch_get_font __PARMS((char_u *name, int giveErrorIfMissing));
void gui_mch_set_font __PARMS((GuiFont font));
int gui_mch_same_font __PARMS((GuiFont f1, GuiFont f2));
void gui_mch_free_font __PARMS((GuiFont font));
GuiColor gui_mch_get_color __PARMS((char_u *name));
void gui_mch_set_fg_color __PARMS((GuiColor color));
void gui_mch_set_bg_color __PARMS((GuiColor color));
void gui_mch_draw_string __PARMS((int row, int col, char_u *s, int len, int flags));
int gui_mch_haskey __PARMS((char_u *name));
void gui_mch_beep __PARMS((void));
void gui_mch_flash __PARMS((void));
void gui_mch_invert_rectangle __PARMS((int r, int c, int nr, int nc));
void gui_mch_iconify __PARMS((void));
void gui_mch_settitle __PARMS((char_u *title, char_u *icon));
void gui_mch_draw_hollow_cursor __PARMS((GuiColor color));
void gui_mch_draw_part_cursor __PARMS((int w, int h, GuiColor color));
void gui_mch_update __PARMS((void));
int gui_mch_wait_for_chars __PARMS((int wtime));
void gui_mch_flush __PARMS((void));
void gui_mch_clear_block __PARMS((int row1, int col1, int row2, int col2));
void gui_mch_clear_all __PARMS((void));
void gui_mch_delete_lines __PARMS((int row, int num_lines));
void gui_mch_insert_lines __PARMS((int row, int num_lines));
void gui_mch_enable_menu __PARMS((int flag));
void gui_mch_set_menu_pos __PARMS((int x, int y, int w, int h));
void gui_mch_add_menu __PARMS((VimMenu *menu, VimMenu *parent, int idx));
void gui_mch_add_menu_item __PARMS((VimMenu *menu, VimMenu *parent, int idx));
void gui_mch_show_popupmenu __PARMS((VimMenu *menu));
void gui_mch_destroy_menu __PARMS((VimMenu *menu));
void gui_mch_menu_grey __PARMS((VimMenu *menu, int grey));
void gui_mch_menu_hidden __PARMS((VimMenu *menu, int hidden));
void gui_mch_draw_menubar __PARMS((void));
int gui_mch_get_lightness __ARGS((GuiColor pixel));
char_u *gui_mch_get_rgb __ARGS((GuiColor pixel));
int gui_mch_get_mouse_x __ARGS((void));
int gui_mch_get_mouse_y __ARGS((void));
void gui_mch_setmouse __ARGS((int x, int y));
void gui_mch_show_popupmenu __ARGS((VimMenu *menu));
int gui_mch_dialog __ARGS((int type, char_u *title, char_u *message, char_u *buttons, int dfltbutton));
char_u *gui_mch_browse __ARGS((int saving, char_u *title, char_u *dflt, char_u *ext, char_u *initdir, char_u *filter));




void gui_request_selection __PARMS((void));
void gui_mch_lose_selection __PARMS((void));
int gui_mch_own_selection __PARMS((void));
void gui_mch_clear_selection __PARMS((void));

void gui_win_new_height __PARMS((WIN *wp));
void gui_win_comp_pos __PARMS((void));
void gui_win_free __PARMS((WIN *wp));
void gui_win_alloc __PARMS((WIN *wp));
