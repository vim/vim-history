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
 * Unix system-dependent filenames
 */
#define BACKUPDIR "~/"

#ifdef TMPNAME1
# undef TMPNAME1
# undef TMPNAME2
# undef TMPNAMELEN
#endif /* TMPNAME1 */
#define TMPNAME1 "/tmp/viXXXXXX"
#define TMPNAME2 "/tmp/voXXXXXX"
#define TMPNAMELEN	15

#ifdef MAX_COLUMNS
# undef MAX_COLUMNS
#endif /* MAX_COLUMNS */
#define MAX_COLUMNS 1024

#define stricmp vim_stricmp

void	flushbuf __ARGS((void));
void	outchar __ARGS((unsigned));
void	outstr __ARGS((char *));
int 	GetChars __ARGS((char *, int, int));
void	vim_delay __ARGS((void));
void	mch_suspend __ARGS((void));
void	mch_windinit __ARGS((void));
void	check_win __ARGS((int, char **));
void	fname_case __ARGS((char *));
void	settitle __ARGS((char *));
void	resettitle __ARGS((void));
void	mch_settmode __ARGS((int));
int 	dirname __ARGS((char *, int));
int		FullName __ARGS((char *, char *, int));
long	getperm __ARGS((char *));
int		setperm __ARGS((char *, int));
int		isdir __ARGS((char *));
void	mch_windexit __ARGS((int));
int		mch_get_winsize __ARGS((void));
void	mch_set_winsize __ARGS((void));
int		call_shell __ARGS((char *, int));
void	breakcheck __ARGS((void));
#ifndef linux
int		remove __ARGS((char *));

/* generic functions, not in unix.c */
void	sleep __ARGS((int));
int		rename __ARGS((char *, char *));
#endif

int		stricmp __ARGS((char *, char *));

#ifdef WILD_CARDS
int		has_wildcard __ARGS((char *));
int		ExpandWildCards __ARGS((int, char **, int *, char ***, int, int));
void	FreeWild __ARGS((int, char **));
#endif

/* memmove is not present on all systems, use bcopy or memcpy */
#ifdef SYSV
#define memmove(to, from, len) memcpy(to, from, len)
#else
#define memmove(to, from, len) bcopy(from, to, len);
#endif
