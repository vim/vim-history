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
#include "globals.h"
#include "proto.h"
#include "param.h"


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

static struct buffheader stuffbuff = {{NULL, {NUL}}, NULL, 0, 0};
static struct buffheader redobuff = {{NULL, {NUL}}, NULL, 0, 0};
static struct buffheader recordbuff = {{NULL, {NUL}}, NULL, 0, 0};

	/*
	 * when block_redo is TRUE redo buffer will not be changed
	 * used by edit() to repeat insertions and 'V' command for redoing
	 */
static int		block_redo = FALSE;

struct mapblock
{
	struct mapblock *m_next;		/* next mapblock */
	char			*m_keys;		/* mapped from */
	char			*m_str; 		/* mapped to */
	int 			 m_mode;		/* valid mode */
	int				 m_noremap;		/* no re-mapping for this one */
};

/* variables used by vgetorpeek() and flush_buffers */
#define MAXMAPLEN 10	/* maximum length of key sequence to be mapped */
static char		typeahead[MAXMAPLEN + 2];
static int		typelen = 0;	/* number of characters in typeahead[] */
static char		*mapstr = NULL;	/* mapped characters */
static int		maplen = 0;		/* number of characters in mapstr */

static void		free_buff __ARGS((struct buffheader *));
static void		add_buff __ARGS((struct buffheader *, char *));
static void		add_num_buff __ARGS((struct buffheader *, long));
static u_char	read_stuff __ARGS((int));
static int		start_stuff __ARGS((void));
static int		read_redo __ARGS((int));
static u_char	vgetorpeek __ARGS((int));
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
		unsigned		count = 0;
		u_char			*p = NULL;
		struct bufblock	*bp;

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
 *	and clear the record buffer
 */
	u_char *
get_recorded()
{
		u_char *p;

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
				emsg("Add to read buffer");
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
	int			advance;
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
	static int
start_stuff()
{
	if (stuffbuff.bh_first.b_next == NULL)
		return FALSE;
	stuffbuff.bh_curr = &(stuffbuff.bh_first);
	stuffbuff.bh_space = 0;
	return TRUE;
}

/*
 * check if the stuff buffer is empty
 */
	int
stuff_empty()
{
	if (stuffbuff.bh_first.b_next == NULL)
		return TRUE;
	return FALSE;
}

/*
 * remove all typeahead characters (used in case of an error).
 */
	void
flush_buffers()
{
	start_stuff();
	while (read_stuff(TRUE) != NUL)
		;
	typelen = 0;
	maplen = 0;
	if (mapstr)
		*mapstr = 0;
}

	void
ResetBuffers()
{
	if (!block_redo)
		free_buff(&redobuff);
}

	void
AppendToRedobuff(s)
	char		   *s;
{
	if (!block_redo)
		add_buff(&redobuff, s);
}

	void
AppendNumberToRedobuff(n)
	long 			n;
{
	if (!block_redo)
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
	int			init;
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

	while ((c = read_redo(FALSE)) != NUL)
		stuffReadbuff(mkstr(c));
}

extern int redo_Quote_busy;		/* this is in normal.c */

/*
 * Stuff the redo buffer into the stuffbuff.
 * Insert the redo count into the command.
 */
	int
start_redo(count)
	long count;
{
		register int c;

		if (read_redo(TRUE))	/* init the pointers; return if nothing to redo */
				return FALSE;

		c = read_redo(FALSE);

/* copy the buffer name, if present */
		if (c == '"')
		{
				add_buff(&stuffbuff, "\"");
				c = read_redo(FALSE);

		/* if a numbered buffer is used, increment the number */
				if (c >= '1' && c < '9')
						++c;
				add_buff(&stuffbuff, mkstr(c));
				c = read_redo(FALSE);
		}

		if (c == 'q')	/* redo Quoting */
		{
			Quote = Curpos;
			redo_Quote_busy = TRUE;
			c = read_redo(FALSE);
		}

/* try to enter the count (in place of a previous count) */
		if (count)
		{
				while (isdigit(c))		/* skip "old" count */
						c = read_redo(FALSE);
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
	int
start_redo_ins()
{
		register u_char c;

		if (read_redo(TRUE))
				return FALSE;
		start_stuff();

/* skip the count and the command character */
		while ((c = read_redo(FALSE)) != NUL)
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
		block_redo = TRUE;
		return TRUE;
}

	void
set_redo_ins()
{
		block_redo = TRUE;
}

	void
stop_redo_ins()
{
		block_redo = FALSE;
}

struct mapblock maplist = {NULL, NULL, NULL}; /* first dummy entry in maplist */

/*
 * insert a string in front of the map-buffer (for '@' command and vgetorpeek)
 */
	int
ins_mapbuf(str)
	char *str;
{
	register char *s;
	register int newlen;

	newlen = maplen + strlen(str) + 1;
	if (newlen < 0)				/* string is getting too long */
	{
		emsg(e_toocompl);		/* also calls flush_buffers */
		setcursor();
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

extern int arrow_used;		/* this is in edit.c */

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
	int		advance;
{
	register int	c;
	int				n = 0;		/* init for GCC */
	char			*str;
	int				len;
	struct mapblock *mp;
	int				mode = State;
	static int		nomapping = 0;		/* number of characters that should
											not be mapped */
	int				timedout = FALSE;	/* waited for more than 1 second
												for mapping to complete */
	int				mapdepth = 0;		/* check for recursive mapping */

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
				if (got_int)
				{
					typelen = 0;			/* flush all typeahead */
					maplen = 0;
					len = 0;
				}
				else if (len > 0)	/* see if we have a mapped key sequence */
				{
					/*
					 * walk through the maplist until we find an
					 * entry that matches (if not timed out).
					 */
					mp = NULL;
					if (!timedout && (str != mapstr || (p_remap && nomapping == 0)))
					{
						for (mp = maplist.m_next; mp; mp = mp->m_next)
						{
							if (mp->m_mode != mode)
								continue;
							n = strlen(mp->m_keys);
							if (!strncmp(mp->m_keys, str, (size_t)(n > len ? len : n)))
								break;
						}
					}
					if (mp == NULL || (str == mapstr && n > len))
					{								/* no match found */
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
							if (nomapping)
								--nomapping;
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
						 * The depth check catches ":map x y" and ":map y x".
						 */
						if (++mapdepth == 1000)
						{
							emsg("recursive mapping");
							setcursor();
							maplen = 0;
							c = -1;
							break;
						}
						if (ins_mapbuf(mp->m_str) < 0)
						{
							c = -1;
							break;
						}
						if (mp->m_noremap)
							nomapping += strlen(mp->m_str);
						str = mapstr;
						len = maplen;
						continue;
					}
				}
											/* inchar() will reset got_int */
				c = inchar(!advance, len == 0 || !p_timeout);
				if (c <= NUL || !advance)	/* no character available or async */
				{
					if (!advance)
						break;
					if (len)				/* timed out */
					{
						timedout = TRUE;
						continue;
					}
				}
				else
				{
					typeahead[typelen++] = c;
					updatescript(c);
					if (Recording)
						add_buff(&recordbuff, mkstr(c));

							/* do not sync in insert mode, unless cursor key has
							 * been used */
					if (mode != INSERT || arrow_used)		
						u_sync();
				}
				len = typelen;
				str = typeahead;
			}
		}
	} while (c < 0 || (advance && c == NUL));
						/* if advance is FALSE don't loop on NULs */

	return (u_char) c;
}

	u_char
vgetc()
{
	return (vgetorpeek(TRUE));
}

	u_char
vpeekc()
{
	return (vgetorpeek(FALSE));
}

/*
 * unmap[!] {lhs}			: remove key mapping for {lhs}
 * map[!]					: show all key mappings
 * map[!] {lhs}				: show key mapping for {lhs}
 * map[!] {lhs} {rhs}		: set key mapping for {lhs} to {rhs}
 * noremap[!] {lhs} {rhs}	: same, but no remapping for {rhs}
 *
 * maptype == 1 for unmap command, 2 for noremap command.
 * arg is pointer to any arguments.
 * mode is INSERT if [!] is present.
 * 
 * Return 0 for success
 *		  1 for invalid arguments
 *		  2 for no match
 *		  3 for ambiguety
 *		  4 for out of mem
 */
	int
domap(maptype, keys, mode)
	int		maptype;
	char	*keys;
	int		mode;
{
		struct mapblock		*mp, *mprev;
		char				*arg;
		char				*p;
		int					n = 0;			/* init for GCC */
		int					len = 0;		/* init for GCC */
		char				*newstr;
		int					hasarg;

/*
 * find end of keys and remove CTRL-Vs in it
 */
		p = keys;
		while (*p && *p != ' ' && *p != '\t')
		{
			if (*p == Ctrl('V') && p[1] != NUL)
				strcpy(p, p + 1);			/* remove CTRL-V */
			++p;
		}
		if (*p != NUL)
			*p++ = NUL;
		skipspace(&p);
		hasarg = (*p != NUL);
		arg = p;
/*
 * remove CTRL-Vs from argument
 */
		while (*p)
		{
			if (*p == Ctrl('V') && p[1] != NUL)
				strcpy(p, p + 1);			/* remove CTRL-V */
			++p;
		}

/*
 * check arguments and translate function keys
 */
		if (*keys != NUL)
		{
				if (maptype == 1 && hasarg)			/* unmap has no arguments */
					return 1;
				if (*keys == '#' && isdigit(*(keys + 1)))	/* function key */
				{
					if (*++keys == '0')
						*(u_char *)keys = K_F10;
					else
						*keys += K_F1 - '1';
				}
				len = strlen(keys);
				if (len > MAXMAPLEN)			/* maximum lenght of 10 chars */
					return 2;
		}

/*
 * Find an entry in the maplist that matches.
 */
#ifdef AMIGA
		if (*keys == NUL || (maptype != 1 && !hasarg))
			settmode(0);				/* set cooked mode so output can be halted */
#endif
		for (mp = maplist.m_next, mprev = &maplist; mp; mprev = mp, mp = mp->m_next)
		{
			if (mp->m_mode != mode)
				continue;
			n = strlen(mp->m_keys);
			if (*keys == NUL)
				showmap(mp);
			else if (!strncmp(mp->m_keys, keys, (size_t)(n < len ? n : len)))
			{
				if (maptype != 1 && !hasarg)
					showmap(mp);
				else
					break;
			}
		}
		if (*keys == NUL || (maptype != 1 && !hasarg))
		{
#ifdef AMIGA
				settmode(1);
#endif
				wait_return(TRUE);
				return 0;				/* listing finished */
		}

		if (mp == NULL) 		/* new entry or nothing to remove */
		{
				if (maptype == 1)
						return 2;		/* no match */

				/* allocate a new entry for the maplist */
				mp = (struct mapblock *)alloc((unsigned)sizeof(struct mapblock));
				if (mp == NULL)
						return 4;		/* no mem */
				mp->m_keys = strsave(keys);
				mp->m_str = strsave(arg);
				if (mp->m_keys == NULL || mp->m_str == NULL)
				{
						free(mp->m_keys);
						free(mp->m_str);
						free(mp);
						return 4;		/* no mem */
				}
				mp->m_noremap = maptype;

				/* add the new entry in front of the maplist */
				mp->m_next = maplist.m_next;
				mp->m_mode = mode;
				maplist.m_next = mp;
				return 0;				/* added OK */
		}
		if (n != len)
			return 3;					/* ambigious */

		if (maptype == 1)
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
		newstr = strsave(arg);
		if (newstr == NULL)
				return 4;				/* no mem */
		free(mp->m_str);
		mp->m_str = newstr;
		mp->m_noremap = maptype;

		return 0;						/* replaced OK */
}

	static void
showmap(mp)
	struct mapblock *mp;
{
	int len;

	len = outtrans(mp->m_keys, -1);	/* get length of what we have written */
	while (len < MAXMAPLEN)
	{
		outchar(' ');				/* padd with blanks */
		++len;
	}
	if (mp->m_noremap)
		outchar('*');
	else
		outchar(' ');
	outtrans(mp->m_str, -1);
	outchar('\n');
	flushbuf();
}

/*
 * Write map commands for the current mapping to an .exrc file.
 * Return 1 on error.
 */
	int
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
