/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/* prototypes from msdos.c */
long	mch_avail_mem __ARGS((int));
void	vim_delay __ARGS((void));
int		vim_remove __ARGS((char_u *));
void	mch_write __ARGS((char_u *, int));
int 	GetChars __ARGS((char_u *, int, int));
int		mch_char_avail __ARGS((void));
void	mch_suspend __ARGS((void));
void	mch_windinit __ARGS((void));
void	check_win __ARGS((int, char **));
void	fname_case __ARGS((char_u *));
void	mch_settitle __ARGS((char_u *, char_u *));
void	mch_restore_title __PARMS((int which));
int		vim_dirname __ARGS((char_u *, int));
int		FullName __ARGS((char_u *, char_u *, int));
int		isFullName __ARGS((char_u *));
long	getperm __ARGS((char_u *));
int		setperm __ARGS((char_u *, long));
int		isdir __ARGS((char_u *));
void	mch_windexit __ARGS((int));
void	mch_settmode __ARGS((int));
int		mch_screenmode __ARGS((char_u *));
int		mch_get_winsize __ARGS((void));
void	set_window __ARGS((void));
void	mch_set_winsize __ARGS((void));
int		call_shell __ARGS((char_u *, int, int));
void	breakcheck __ARGS((void));
char_u	*modname __ARGS((char_u *, char_u *));
int		has_wildcard __ARGS((char_u *));
int		ExpandWildCards __ARGS((int, char_u **, int *, char_u ***, int, int));
void	FreeWild __ARGS((int, char_u **));
int		vim_chdir __ARGS((char_u *));
