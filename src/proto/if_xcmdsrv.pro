/* if_xcmdsrv.c */
int serverRegisterName __ARGS((Display *dpy, char_u *name));
void serverChangeRegisteredWindow __ARGS((Display *dpy, Window newwin));
int serverSendToVim __ARGS((Display *dpy, char_u *name, char_u *cmd));
char_u *serverGetVimNames __ARGS((Display *dpy));
void serverEventProc __ARGS((Display *dpy, XEvent *eventPtr));
void ex_serversend __ARGS((exarg_T *eap));
void ex_serverlist __ARGS((exarg_T *eap));
void cmdsrv_main __ARGS((int argc, char **argv, char_u *cmdTarget));
/* vim: set ft=c : */
