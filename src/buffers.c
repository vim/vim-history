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
 * buffers.c
 *
 * manipulations with redo buffer and stuff buffer
 */

#include "vim.h"

/*
 * structure used to store one block of the stuff/redo/macro buffers
 */
struct bufblock
{
		struct bufblock *b_next;		/* pointer to next bufblock */
		u_char			b_str[1];		/* contents (actually longer) */
};

#define MINIMAL_SIZE 20 				/* minimal size for b_str */

/*
 * header used for the stuff buffer and the redo buffer
 */
struct buffheader
{
		struct bufblock bh_first;		/* first (dummy) block of list */
		struct bufblock *bh_curr;		/* bufblock for appending */
		int 			bh_index;		/* index for reading */
		int 			bh_space;		/* space in bh_curr for appending */
};

static struct buffheader stuffbuff = {{NULL, NUL}, NULL, 0, 0};
static struct buffheader redobuff = {{NULL, NUL}, NULL, 0, 0};
static struct buffheader recordbuff = {{NULL, NUL}, NULL, 0, 0};

bool_t	redo_ins_busy = FALSE;	/* when TRUE redo buffer will not be changed
									(used by edit() to repeat insertions) */

/* variables used by vgetorpeek() and flush_buffers */
#define MAXMAPLEN 10	/* maximum length of key sequence to be mapped */
static char		typeahead[MAXMAPLEN + 2];
static int		typelen = 0;	/* number of characters in typeahead[] */
static char		*mapstr = NULL;	/* mapped characters */
static int		maplen = 0;		/* number of characters in mapstr */

static void		free_buff __ARGS((struct buffheader *));
static void		add_buff __ARGS((struct buffheader *, char *));
static void		add_num_buff __ARGS((struct buffheader *, long));
static u_char	read_stuff __ARGS((bool_t));
static bool_t	start_stuff __ARGS((void));
static int		read_redo __ARGS((bool_t));
static u_char	vgetorpeek __ARGS((bool_t));
static void		showmap __ARGS((struct mapblock *));

/*
 * free and clear a buffer
 */
	static void
free_buff(buf)
	struct buffheader *buf;
{
		register struct bufblock *p, *np;

		for (p = buf->bh_first.b_next; p != NULL; p = np)
		{
				np = p->b_next;
				free((char *)p);
		}
		buf->bh_first.b_next = NULL;
}

/*
 * return the contents of a buffer as a single string
 */
	u_char *
get_bufcont(buffer)
	struct buffheader *buffer;
{
		int count = 0;
		u_char *p = NULL;
		struct bufblock *bp;

/* compute the total length of the string */
		for (bp = buffer->bh_first.b_next; bp != NULL; bp = bp->b_next)
				count += strlen((char *)bp->b_str);

		if (count != 0 && (p = (u_char *)alloc(count + 1)) != NULL)
		{
				*p = NUL;
				for (bp = buffer->bh_first.b_next; bp != NULL; bp = bp->b_next)
						strcat((char *)p, (char *)bp->b_str);
		}
		return (p);
}

/*
 * return the contents of the record buffer as a single string
 */
	u_char *
get_recorded()
{
		u_char *p = NULL;

		p = get_bufcont(&recordbuff);
		free_buff(&recordbuff);
		return (p);
}

/*
 * return the contents of the redo buffer as a single string
 */
	u_char *
get_inserted()
{
		return(get_bufcont(&redobuff));
}

/*
 * add string "s" after the current block of buffer "buf"
 */
	static void
add_buff(buf, s)
	register struct buffheader	*buf;
	char						*s;
{
		struct bufblock *p;
		int 			n;
		int 			len;

		if ((n = strlen(s)) == 0)		/* don't add empty strings */
				return;

		if (buf->bh_first.b_next == NULL)		/* first add to list */
		{
				buf->bh_space = 0;
				buf->bh_curr = &(buf->bh_first);
		}
		else if (buf->bh_curr == NULL)	/* buffer has already been read */
		{
				emsg("add to read buffer");
				return;
		}
		else if (buf->bh_index != 0)
				strcpy((char *)buf->bh_first.b_next->b_str, (char *)buf->bh_first.b_next->b_str + buf->bh_index);
		buf->bh_index = 0;

		if (buf->bh_space >= n)
		{
				strcat((char *)buf->bh_curr->b_str, s);
				buf->bh_space -= n;
		}
		else
		{
				if (n < MINIMAL_SIZE)
						len = MINIMAL_SIZE;
				else
						len = n;
				p = (struct bufblock *)alloc((unsigned)(sizeof(struct bufblock) + len));
				if (p == NULL)
						return; /* no space, just forget it */
				buf->bh_space = len - n;
				strcpy((char *)p->b_str, s);

				p->b_next = buf->bh_curr->b_next;
				buf->bh_curr->b_next = p;
				buf->bh_curr = p;
		}
		return;
}

	static void
add_num_buff(buf, n)
	struct buffheader *buf;
	long 			  n;
{
		char	number[32];

		sprintf(number, "%ld", n);
		add_buff(buf, number);
}

/*
 * get one character from the stuff buffer
 * If advance == TRUE go to the next char.
 */
	static u_char
read_stuff(advance)
	bool_t			advance;
{
		register u_char c;
		register struct bufblock *curr;


		if (stuffbuff.bh_first.b_next == NULL)	/* buffer is empty */
				return NUL;

		curr = stuffbuff.bh_first.b_next;
		c = curr->b_str[stuffbuff.bh_index];

		if (advance)
		{
				if (curr->b_str[++stuffbuff.bh_index] == NUL)
				{
						stuffbuff.bh_first.b_next = curr->b_next;
						free((char *)curr);
						stuffbuff.bh_index = 0;
				}
		}
		return c;
}

/*
 * prepare stuff buffer for reading (if it contains something)
 */
	static bool_t
start_stuff()
{
		if (stuffbuff.bh_first.b_next == NULL)
				return FALSE;
		stuffbuff.bh_curr = &(stuffbuff.bh_first);
		stuffbuff.bh_space = 0;
		return TRUE;
}

/*
 * remove all typeahead characters (used in case of an error).
 */
	void
flush_buffers()
{
	start_stuff();
	while (read_stuff((bool_t)TRUE) != NUL)
		;
	typelen = 0;
	maplen = 0;
	if (mapstr)
		*mapstr = 0;
}

	void
ResetBuffers()
{
	if (!redo_ins_busy)
		free_buff(&redobuff);
}

	void
AppendToRedobuff(s)
	char		   *s;
{
	if (!redo_ins_busy)
		add_buff(&redobuff, s);
}

	void
AppendNumberToRedobuff(n)
	long 			n;
{
	if (!redo_ins_busy)
		add_num_buff(&redobuff, n);
}

	void
stuffReadbuff(s)
	char		   *s;
{
	add_buff(&stuffbuff, s);
}

	void
stuffnumReadbuff(n)
	long	n;
{
	add_num_buff(&stuffbuff, n);
}

/*
 * Read a character from the redo buffer.
 * The redo buffer is left as it is.
 */
	static int
read_redo(init)
	bool_t		init;
{
	static struct bufblock	*bp;
	static u_char			*p;
	int						c;

	if (init)
	{
		if ((bp = redobuff.bh_first.b_next) == NULL)
			return TRUE;
		p = bp->b_str;
		return FALSE;
	}
	if ((c = *p) != NUL)
	{
		if (*++p == NUL && bp->b_next != NULL)
		{
			bp = bp->b_next;
			p = bp->b_str;
		}
	}
	return c;
}

/*
 * copy the rest of the redo buffer into the stuff buffer (could be done faster)
 */
	void
copy_redo()
{
	register int c;

	while ((c = read_redo((bool_t)FALSE)) != NUL)
		stuffReadbuff(mkstr(c));
}

/*
 * Stuff the redo buffer into the stuffbuff.
 * Insert the redo count into the command.
 */
	bool_t
start_redo(count)
	long count;
{
		register int c;

		if (read_redo((bool_t)TRUE))	/* init the pointers; return if nothing to redo */
				return FALSE;

		c = read_redo((bool_t)FALSE);

/* copy the buffer name, if present */
		if (c == '"')
		{
				add_buff(&stuffbuff, "\"");
				c = read_redo((bool_t)FALSE);

		/* if a numbered buffer is used, increment the number */
				if (c >= '1' && c < '9')
						++c;
				add_buff(&stuffbuff, mkstr(c));
				c = read_redo((bool_t)FALSE);
		}

/* try to enter the count (in place of a previous count) */
		if (count)
		{
				while (isdigit(c))		/* skip "old" count */
						c = read_redo((bool_t)FALSE);
				add_num_buff(&stuffbuff, count);
		}

/* copy from the redo buffer into the stuff buffer */
		add_buff(&stuffbuff, mkstr(c));
		copy_redo();
		return TRUE;
}

/*
 * Repeat the last insert (R, o, O, a, A, i or I command) by stuffing the redo buffer
 * into the stuffbuff.
 */
	bool_t
start_redo_ins()
{
		register u_char c;

		if (read_redo((bool_t)TRUE))
				return FALSE;
		start_stuff();

/* skip the count and the command character */
		while ((c = read_redo((bool_t)FALSE)) != NUL)
		{
			c = toupper(c);
			if (strchr("AIRO", c) != NULL)
			{
				if (c == 'O')
					stuffReadbuff(NL_STR);
				break;
			}
		}

/* copy the typed text from the redo buffer into the stuff buffer */
		copy_redo();
		redo_ins_busy = TRUE;
		return TRUE;
}

	void
stop_redo_ins()
{
		redo_ins_busy = FALSE;
}

struct mapblock
{
		struct mapblock *m_next;		/* next mapblock */
		char			*m_keys;		/* mapped from */
		char			*m_str; 		/* mapped to */
		int 			 m_mode;		/* valid mode */
};

struct mapblock maplist = {NULL, NULL, NULL}; /* first dummy entry in maplist */

/*
 * insert a string in front of the map-buffer (for '@' command and vgetorpeek)
 */
ins_mapbuf(str)
	char *str;
{
	register char *s;
	register int newlen;

	newlen = maplen + strlen(str) + 1;
	if (newlen < 0)		/* string is getting too long */
	{
		emsg("command too complex");	/* also calls flush_buffers */
		return -1;
	}
	s = alloc(newlen);
	if (s == NULL)
		return -1;
	strcpy(s, str);
	if (mapstr)
	{
		strcat(s, mapstr);
		free(mapstr);
	}
	mapstr = s;
	maplen = strlen(mapstr);
	return 0;
}

/*
 * get a character: 1. from the stuffbuffer
 *					2. from the user
 *
 * KeyTyped is set to TRUE in the case the user typed the key.
 * If advance is TRUE, we really get the character. Otherwise we just look
 * whether there is a character available.
 */
	static u_char
vgetorpeek(advance)
	bool_t advance;
{
	register int	c;
	int				n;
	char			*str;
	int				len;
	struct mapblock *mp;
	int				mode = State;

	if (mode == REPLACE || mode == CMDLINE)
		mode = INSERT;			/* treat replace mode just like insert mode */
	else if (mode == NORMAL_BUSY)
		mode = NORMAL;

	start_stuff();
	do
	{
		c = read_stuff(advance);
		if (c != NUL)
			KeyTyped = FALSE;
		else
		{
			/*
			 * Loop until we either find a matching mapped key, or we
			 * are sure that it is not a mapped key.
			 * We do this first for mapstr and then for typeahead.
			 * If a mapped key sequence is found we go back to mapstr to
			 * try re-mapping.
			 */
			if (maplen)		/* first try mapstr */
			{
				str = mapstr;
				len = maplen;
			}
			else			/* no mapped chars, try typeahead[] */
			{
				str = typeahead;
				len = typelen;
			}

			for (;;)		/* loop until we got a character */
			{
				breakcheck();				/* check for CTRL-C */
				if (!got_int && len > 0)	/* see if we have a mapped key sequence */
				{
					/*
					 * walk through the maplist until we find an
					 * entry that matches.
					 */
					for (mp = maplist.m_next; mp; mp = mp->m_next)
					{
						if (mp->m_mode != mode)
							continue;
						n = strlen(mp->m_keys);
						if (!strncmp(mp->m_keys, str, (size_t)(n > len ? len : n)))
							break;
					}
					if (mp == NULL || str == mapstr && (n > len ||
								P(P_REMAP) == FALSE)) /* no match found */
					{
						c = str[0] & 255;
						if (str == mapstr)
							KeyTyped = FALSE;
						else
							KeyTyped = TRUE;
						if (advance)
						{
							strncpy(&str[0], &str[1], (size_t)len);
							if (str == mapstr)
								--maplen;
							else
								--typelen;
						}
						break;
					}
					if (n <= len)	/* complete match */
					{
							/* remove the mapped keys */
						len -= n;
						strncpy(&str[0], &str[n], (size_t)(len + 1));
						if (str == mapstr)
							maplen = len;
						else
							typelen = len;

						/*
						 * Put the replacement string in front of mapstr.
						 */
						if (ins_mapbuf(mp->m_str) < 0)
						{
							c = -1;
							break;
						}
						str = mapstr;
						len = maplen;
						continue;
					}
				}
				c = inchar(!advance);
				if (c <= NUL)	/* no character available */
				{
					if (!advance)
						break;
				}
				else
				{
					typeahead[typelen++] = c;
					updatescript(c);
					if (Recording)
						add_buff(&recordbuff, mkstr(c));
					if (mode != INSERT)
						u_sync();
				}
				len = typelen;
				str = typeahead;
			}
			if (got_int)		/* interrupted: remove all chars */
			{
				c = -1;
				continue;
			}
		}
	} while (c < 0 || advance && c == NUL);
						/* if advance is FALSE don't loop on NULs */

	return (u_char) c;
}

	u_char
vgetc()
{
	return (vgetorpeek((bool_t)TRUE));
}

	u_char
vpeekc()
{
	return (vgetorpeek((bool_t)FALSE));
}

/*
 * unmap[!] {lhs}		: remove key mapping for {lhs}
 * map[!]				: show all key mappings
 * map[!] {lhs}			: show key mapping for {lhs}
 * map[!] {lhs} {rhs}	: set key mapping for {lhs} to {rhs}
 *
 * unmap == 1 for unmap command.
 * arg is pointer to any arguments.
 * mode is INSERT if [!] is present.
 * 
 * Return 0 for success
 *		  1 for invalid arguments
 *		  2 for no match
 *		  3 for ambiguety
 *		  4 for out of mem
 */
domap(unmap, arg, mode)
	int		unmap;
	char	*arg;
	int		mode;
{
		struct mapblock *mp, *mprev;
		char *p;
		int n;
		int len;
		char *newstr;

/*
 * find end of keys
 */
		for (p = arg; *p != ' ' && *p != '\t' && *p != NUL; ++p)
			;
		if (*p != NUL)
			*p++ = NUL;
		skipspace(&p);

/*
 * check arguments and translate function keys
 */
		if (*arg != NUL)
		{
				if (unmap && *p != NUL)				/* unmap has no arguments */
					return 1;
				if (*arg == '#' && isdigit(*(arg + 1)))	/* funcion key */
				{
					if (*++arg == '0')
						*arg = K_F10;
					else
						*arg += K_F1 - '1';
				}
				len = strlen(arg);
				if (len > MAXMAPLEN)			/* maximum lenght of 10 chars */
					return 2;
		}

/*
 * Find an entry in the maplist that matches.
 */
		if (*arg == NUL || !unmap && *p == NUL)
			setmode(0);				/* set cooked mode so output can be halted */
		for (mp = maplist.m_next, mprev = &maplist; mp; mprev = mp, mp = mp->m_next)
		{
			if (mp->m_mode != mode)
				continue;
			n = strlen(mp->m_keys);
			if (*arg == NUL)
				showmap(mp);
			else if (!strncmp(mp->m_keys, arg, (size_t)(n < len ? n : len)))
			{
				if (!unmap && *p == NUL)
					showmap(mp);
				else
					break;
			}
		}
		if (*arg == NUL || !unmap && *p == NUL)
		{
				setmode(1);
				wait_return((bool_t)TRUE);
				return 0;				/* listing finished */
		}

		if (mp == NULL) 		/* new entry or nothing to remove */
		{
				if (unmap)
						return 2;		/* no match */

				/* allocate a new entry for the maplist */
				mp = (struct mapblock *)alloc((unsigned)sizeof(struct mapblock));
				if (mp == NULL)
						return 4;		/* no mem */
				mp->m_keys = strsave(arg);
				mp->m_str = strsave(p);
				if (mp->m_keys == NULL || mp->m_str == NULL)
				{
						free(mp->m_keys);
						free(mp->m_str);
						free(mp);
						return 4;		/* no mem */
				}

				/* add the new entry in front of the maplist */
				mp->m_next = maplist.m_next;
				mp->m_mode = mode;
				maplist.m_next = mp;
				return 0;				/* added OK */
		}
		if (n != len)
			return 3;					/* ambigious */

		if (unmap)
		{
				free(mp->m_keys);
				free(mp->m_str);
				mprev->m_next = mp->m_next;
				free(mp);
				return 0;				/* removed OK */
		}

/*
 * replace existing entry
 */
		newstr = strsave(p);
		if (newstr == NULL)
				return 4;				/* no mem */
		free(mp->m_str);
		mp->m_str = newstr;

		return 0;						/* replaced OK */
}

	static void
showmap(mp)
	struct mapblock *mp;
{
	char *p;
	int len;

	outtrans(mp->m_keys, -1);
	len = 0;
	for (p = mp->m_keys; *p; ++p)
		len += charsize(*p);		/* count length of what we have written */
	while (len < MAXMAPLEN)
	{
		outchar(' ');				/* padd with blanks */
		++len;
	}
	outtrans(mp->m_str, -1);
	outchar('\n');
	flushbuf();
}

/*
 * Write map commands for the current mapping to an .exrc file.
 * Return 1 on error.
 */
makemap(fd)
	FILE *fd;
{
	struct mapblock *mp;

	for (mp = maplist.m_next; mp; mp = mp->m_next)
	{
		if (fprintf(fd, "map%c %s %s\n", mp->m_mode == INSERT ? '!' : ' ',
									mp->m_keys, mp->m_str) < 0)
			return 1;
	}
	return 0;
}
