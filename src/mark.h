/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
 */

/*
 * mark.h: definitions shared between tag.c and mark.c 
 */

struct mark
{
	char		   *ptr;
	colnr_t 		col;
};

struct filemark
{
	struct mark		mark;			/* cursor position */
	linenr_t		lnum;			/* last known line number */
	int				fnum;			/* file number */
};

char *fm_getname __ARGS((struct filemark *));
