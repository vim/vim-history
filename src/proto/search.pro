/* search.c */
vim_regexp *search_regcomp __ARGS((char_u *pat, int pat_save, int pat_use, int options));
void save_search_patterns __ARGS((void));
void restore_search_patterns __ARGS((void));
void set_reg_ic __ARGS((char_u *pat));
vim_regexp *last_pat_prog __ARGS((void));
int searchit __ARGS((BUF *buf, FPOS *pos, int dir, char_u *str, long count, int options, int pat_use));
int do_search __ARGS((OPARG *oap, int dirc, char_u *str, long count, int options));
int search_for_exact_line __ARGS((BUF *buf, FPOS *pos, int dir, char_u *pat));
int searchc __ARGS((int c, int dir, int type, long count));
FPOS *findmatch __ARGS((OPARG *oap, int initc));
FPOS *findmatchlimit __ARGS((OPARG *oap, int initc, int flags, int maxtravel));
void showmatch __ARGS((void));
int findsent __ARGS((int dir, long count));
int findpar __ARGS((OPARG *oap, int dir, long count, int what, int both));
int startPS __ARGS((linenr_t lnum, int para, int both));
int fwd_word __ARGS((long count, int type, int eol));
int bck_word __ARGS((long count, int type, int stop));
int end_word __ARGS((long count, int type, int stop, int empty));
int bckend_word __ARGS((long count, int type, int eol));
int current_word __ARGS((OPARG *oap, long count, int include, int type));
int current_sent __ARGS((OPARG *oap, long count, int include));
int current_block __ARGS((OPARG *oap, long count, int include, int what));
int current_par __ARGS((OPARG *oap, long count, int include, int type));
int linewhite __ARGS((linenr_t lnum));
void find_pattern_in_path __ARGS((char_u *ptr, int dir, int len, int whole, int skip_comments, int type, long count, int action, linenr_t start_lnum, linenr_t end_lnum));
int read_viminfo_search_pattern __ARGS((char_u *line, FILE *fp, int force));
void write_viminfo_search_pattern __ARGS((FILE *fp));
