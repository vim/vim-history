/* help.c */
void do_help __PARMS((char_u *arg));
int help_heuristic __PARMS((char_u *matched_string, int offset));
int find_help_tags __PARMS((char_u *arg, int *num_matches, char_u ***matches));
