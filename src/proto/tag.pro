/* tag.c */
void dotag __PARMS((unsigned char *tag, int type, int count));
void dotags __PARMS((void));
int ExpandTags __PARMS((struct regexp *prog, int *num_file, unsigned char ***file));
