/* script.c */
void startscript __PARMS((void));
int stopscript __PARMS((void));
int openscript __PARMS((char *name));
void updatescript __PARMS((int c));
void openrecover __PARMS((void));
void scriptfullpath __PARMS((void));
char *modname __PARMS((char *fname, char *ext));
void script_winsize __PARMS((void));
void script_winsize_pp __PARMS((void));
