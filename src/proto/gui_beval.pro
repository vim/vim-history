/* gui_beval.c */
BalloonEval *gui_mch_create_beval_area __ARGS((Widget target, char_u *mesg, void (*mesgCB)(BalloonEval *, int), XtPointer clientData));
void gui_mch_destroy_beval_area __ARGS((BalloonEval *beval));
void gui_mch_enable_beval_area __ARGS((BalloonEval *beval));
void gui_mch_disable_beval_area __ARGS((BalloonEval *beval));
Boolean gui_mch_get_beval_info __ARGS((BalloonEval *beval, char_u **filename, int *line, char_u **text, int *idx));
void gui_mch_post_balloon __ARGS((BalloonEval *beval, char_u *mesg));
/* vim: set ft=c : */
