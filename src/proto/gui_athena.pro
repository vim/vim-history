/* gui_athena.c */
void gui_x11_create_widgets __ARGS((void));
void gui_x11_destroy_widgets __ARGS((void));
void gui_mch_set_toolbar_pos __ARGS((int x, int y, int w, int h));
void gui_mch_set_text_area_pos __ARGS((int x, int y, int w, int h));
void gui_x11_set_back_color __ARGS((void));
void gui_mch_enable_menu __ARGS((int flag));
void gui_mch_set_menu_pos __ARGS((int x, int y, int w, int h));
void gui_mch_add_menu __ARGS((vimmenu_T *menu, int idx));
void gui_mch_new_menu_font __ARGS((void));
void gui_mch_add_menu_item __ARGS((vimmenu_T *menu, int idx));
void gui_mch_show_toolbar __ARGS((int showit));
int gui_mch_compute_toolbar_height __ARGS((void));
void gui_mch_get_toolbar_colors __ARGS((Pixel *bgp, Pixel *fgp));
void gui_mch_toggle_tearoffs __ARGS((int enable));
void gui_mch_new_menu_colors __ARGS((void));
void gui_mch_destroy_menu __ARGS((vimmenu_T *menu));
void gui_mch_show_popupmenu __ARGS((vimmenu_T *menu));
void gui_mch_set_scrollbar_thumb __ARGS((scrollbar_T *sb, long val, long size, long max));
void gui_mch_set_scrollbar_pos __ARGS((scrollbar_T *sb, int x, int y, int w, int h));
void gui_mch_enable_scrollbar __ARGS((scrollbar_T *sb, int flag));
void gui_mch_create_scrollbar __ARGS((scrollbar_T *sb, int orient));
void gui_mch_destroy_scrollbar __ARGS((scrollbar_T *sb));
void gui_mch_set_scrollbar_colors __ARGS((scrollbar_T *sb));
Window gui_x11_get_wid __ARGS((void));
char_u *gui_mch_browse __ARGS((int saving, char_u *title, char_u *dflt, char_u *ext, char_u *initdir, char_u *filter));
int gui_mch_dialog __ARGS((int type, char_u *title, char_u *message, char_u *buttons, int dfltbutton));
/* vim: set ft=c : */
