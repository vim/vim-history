/* vi:ts=8:sw=4
 *
 * VIM - Vi IMitation
 *
 * Code Contributions By:	Bram Moolenaar		mool@oce.nl
 *				Tim Thompson		twitch!tjt
 *				Tony Andrews		onecom!wldrdg!tony
 *				G. R. (Fred) Walter	watmath!watcgl!grwalter
 */

/*
 * MSDOS Machine-dependent filenames.
 */
#ifdef SYSEXRC_FILE
# undef SYSEXRC_FILE
# undef SYSVIMRC_FILE
#endif /* SYSEXRC_FILE */
#define SYSEXRC_FILE "$VIM\\_exrc"
#define SYSVIMRC_FILE "$VIM\\_vimrc"

#ifdef EXRC_FILE
# undef EXRC_FILE
# undef VIMRC_FILE
#endif /* EXRC_FILE */
#define EXRC_FILE "_exrc"
#define VIMRC_FILE "_vimrc"

#ifdef VIM_HLP
# undef VIM_HLP
#endif /* VIM_HLP */
#define VIM_HLP "$VIM\\vim.hlp"

#ifdef TMPNAME1
# undef TMPNAME1
# undef TMPNAME2
# undef TMPNAMELEN
#endif /* TMPNAME1 */
#define TMPNAME1 "viXXXXXX"		/* put it in current dir */
#define TMPNAME2 "voXXXXXX"		/*  is there a better place? */
#define TMPNAMELEN	10

/*
 * MSDOS Machine-dependent routines.
 */

#undef remove                   /* MSDOS remove()s when not readonly */

/* msdos.c */
void	vim_delay __ARGS((void));
int	remove __ARGS((char *));
void	flushbuf __ARGS((void));
void	outchar __ARGS((unsigned));
void	outstr __ARGS((char *));
int 	GetChars __ARGS((char *, int, int));
void    textfile __ARGS((int));
void	mch_suspend __ARGS((void));
void	mch_windinit __ARGS((void));
void	check_win __ARGS((int, char **));
void	fname_case __ARGS((char *));
void	settitle __ARGS((char *));
void	resettitle __ARGS((void));
int	dirname __ARGS((char *, int));
int	FullName __ARGS((char *, char *, int));
long	getperm __ARGS((char *));
int	setperm __ARGS((char *, int));
int	isdir __ARGS((char *));
void	mch_windexit __ARGS((int));
void	mch_settmode __ARGS((int));
int	mch_get_winsize __ARGS((void));
void	mch_set_winsize __ARGS((void));
int	call_shell __ARGS((char *, int));
void	breakcheck __ARGS((void));
char	*modname __ARGS((char *, char *));
int	ExpandWildCards __ARGS((int, char **, int *, char ***, int, int));
void	FreeWild __ARGS((int, char **));
