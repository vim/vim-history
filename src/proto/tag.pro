/* tag.c */
void do_tag __ARGS((char_u *tag, int type, int count, int forceit));
void do_tags __ARGS((void));
int find_tags __ARGS((char_u *pat, int *num_matches, char_u ***matchesp, int flags, int mincount));
void simplify_filename __ARGS((char_u *filename));
