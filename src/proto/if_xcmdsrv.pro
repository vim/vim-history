/* if_xcmdsrv.c */
int serverRegisterName __ARGS((Display *dpy, char_u *name));
void serverChangeRegisteredWindow __ARGS((Display *dpy, Window newwin));
int serverSendToVim __ARGS((Display *dpy, char_u *name, char_u *cmd));
char_u *serverGetVimNames __ARGS((Display *dpy));
void serverEventProc __ARGS((Display *dpy, XEvent *eventPtr));
/* vim: set ft=c : */
