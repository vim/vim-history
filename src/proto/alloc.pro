/* alloc.c */
char *alloc __PARMS((unsigned int size));
char *lalloc __PARMS((unsigned long size, int message));
char *strsave __PARMS((char *string));
char *strnsave __PARMS((char *string, int len));
void copy_spaces __PARMS((char *ptr, unsigned long count));
char *mkstr __PARMS((unsigned int c));
