/* alloc.c */
unsigned char *alloc __PARMS((unsigned int size));
unsigned char *lalloc __PARMS((unsigned long size, int message));
unsigned char *strsave __PARMS((unsigned char *string));
unsigned char *strnsave __PARMS((unsigned char *string, int len));
void copy_spaces __PARMS((unsigned char *ptr, unsigned long count));
void del_spaces __PARMS((unsigned char *ptr));
int vim_strnicmp __PARMS((unsigned char *s1, unsigned char *s2, unsigned long len));
