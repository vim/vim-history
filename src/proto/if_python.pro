/* if_python.c */
int python_enabled __ARGS((void));
void python_end __ARGS((void));
void ex_python __ARGS((exarg_t *eap));
void ex_pyfile __ARGS((exarg_t *eap));
void python_buffer_free __ARGS((buf_t *buf));
void python_window_free __ARGS((win_t *win));
