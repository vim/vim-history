/* tag.c */
void do_tag __PARMS((char_u *tag, int type, int count));
void do_tags __PARMS((void));
int find_tags __PARMS((char_u *tag, regexp *prog, int *num_file, char_u ***file, int help_only));
