/* if_python.c */
int python_enabled __ARGS((void));
void python_end __ARGS((void));
void ex_python __ARGS((exarg_T *eap));
void ex_pyfile __ARGS((exarg_T *eap));
void python_buffer_free __ARGS((buf_T *buf));
void python_window_free __ARGS((win_T *win));
char *Py_GetProgramName __ARGS((void));
/* vim: set ft=c : */
