/* vi:ts=4:sw=4
 *
 * VIM - Vi IMitation
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
 */

/*
 * Amiga Machine-dependent routines.
 */

/* amiga.c */
void	flushbuf();
void	outchar __ARGS((unsigned));
void	outstr __ARGS((char *));
int 	inchar __ARGS((bool_t));
void	outnum __ARGS((unsigned));
void	beep();
void	sleep __ARGS((int));
void	delay();
void	windinit();
void	check_win __ARGS((int, char **));
void	fname_case __ARGS((char *));
void	settitle __ARGS((char *));
void	resettitle();
void	setmode __ARGS((bool_t));
int		dirname __ARGS((char *, int));
int		FullName __ARGS((char *, char *, int));
int		getperm __ARGS((char *));
int		setperm __ARGS((char *, int));
int		isdir __ARGS((char *));
void	windexit __ARGS((int));
void	windgoto __ARGS((int, int));
void	setmode __ARGS((bool_t));
int		set_winsize __ARGS((int, int));
int		call_shell __ARGS((char *));
int		breakcheck();
long	Chk_Abort();
int		ExpandWildCards __ARGS((int, char **, int *, char ***, bool_t, bool_t));
void	FreeWild __ARGS((int, char **));
