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

/*
 * structure used for mapping
 */
struct mapblock
{
	struct mapblock *m_next;		/* next mapblock */
	char			*m_keys;		/* mapped from */
	int				 m_keylen;		/* strlen(m_keys) */
	char			*m_str; 		/* mapped to */
	int 			 m_mode;		/* valid mode */
	int				 m_noremap;		/* if non-zero no re-mapping for m_str */
};

static struct mapblock maplist = {NULL, NULL, 0, NULL, 0, 0};
									/* first dummy entry in maplist */

/*
 * variables used by vgetorpeek() and flush_buffers()
 *
 * typestr contains all characters that are not consumed yet.
 * The part in front may contain the result of mappings and @a commands.
 * The lenght of this part is typemaplen.
 * After it are characters that come from the terminal.
 * Some parts of typestr may not be mapped. These parts are remembered in
 * the noremaplist. 
 */
#define MAXMAPLEN 50				/* maximum length of key sequence to be mapped */
									/* must be able to hold an Amiga resize report */
static char		*typestr = NULL;	/* NUL-terminated buffer for typeahead characters */
static char		typebuf[MAXMAPLEN + 3]; /* initial typestr */

static int		typemaplen = 0;		/* number of mapped characters in typestr */

/* 
 * parts int typestr that should not be mapped are remembered with a list
 * of noremap structs. Noremaplist is the first.
 */
struct noremap
{
	int				nr_off;			/* offset to not remappable chars */
	int				nr_len;			/* number of not remappable chars */
	struct noremap	*nr_next;		/* next entry in the list */
};

static struct noremap noremaplist = {0, 0, NULL};

static void		free_buff __ARGS((struct buffheader *));
static u_char	*get_bufcont __ARGS((struct buffheader *, int));
static void		add_buff __ARGS((struct buffheader *, char *));
static void		add_num_buff __ARGS((struct buffheader *, long));
static void		add_char_buff __ARGS((struct buffheader *, int));
static u_char	read_stuff __ARGS((int));
static int		start_stuff __ARGS((void));
static int		read_redo __ARGS((int));
static void		gotchars __ARGS((char *, int));
static void		init_typestr __ARGS((void));
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
	static u_char *
get_bufcont(buffer, dozero)
	struct buffheader	*buffer;
	int					dozero;		/* count == zero is not an error */
{
		u_long			count = 0;
		u_char			*p = NULL;
		struct bufblock	*bp;

/* compute the total length of the string */
		for (bp = buffer->bh_first.b_next; bp != NULL; bp = bp->b_next)
				count += strlen((char *)bp->b_str);

		if ((count || dozero) && (p = (u_char *)lalloc(count + 1, TRUE)) != NULL)
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

	p = get_bufcont(&recordbuff, TRUE);
	free_buff(&recordbuff);
	return (p);
}

/*
 * return the contents of the redo buffer as a single string
 */
	u_char *
get_inserted()
{
		return(get_bufcont(&redobuff, FALSE));
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
	u_long 			n;
	u_long 			len;

	if ((n = strlen(s)) == 0)				/* don't add empty strings */
		return;

	if (buf->bh_first.b_next == NULL)		/* first add to list */
	{
		buf->bh_space = 0;
		buf->bh_curr = &(buf->bh_first);
	}
	else if (buf->bh_curr == NULL)			/* buffer has already been read */
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
		p = (struct bufblock *)lalloc((u_long)(sizeof(struct bufblock) + len), TRUE);
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

	static void
add_char_buff(buf, c)
	struct buffheader *buf;
	int 			  c;
{
		char	temp[2];

		temp[0] = c;
		temp[1] = NUL;
		add_buff(buf, temp);
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
	return (stuffbuff.bh_first.b_next == NULL);
}

/*
 * Remove the contents of the stuff buffer and the mapped characters in the
 * typeahead buffer (used in case of an error). If 'typeahead' is true,
 * flush all typeahead characters (used when interrupted by a CTRL-C).
 */
	void
flush_buffers(typeahead)
	int typeahead;
{
	struct noremap *p;

	init_typestr();

	start_stuff();
	while (read_stuff(TRUE) != NUL)
		;

	if (typeahead)			/* remove all typeahead */
	{
			/*
			 * We have to get all characters, because we may delete the first
			 * part of an escape sequence.
			 * In an xterm we get one char at a time and we have to get them all.
			 */
		while (inchar(typestr, MAXMAPLEN, 10))	
			;
		*typestr = NUL;
	}
	else					/* remove mapped characters only */
		strcpy(typestr, typestr + typemaplen);
	typemaplen = 0;
	noremaplist.nr_len = 0;
	noremaplist.nr_off = 0;
	while (noremaplist.nr_next)
	{
		p = noremaplist.nr_next->nr_next;
		free(noremaplist.nr_next);
		noremaplist.nr_next = p;
	}
}

	void
ResetRedobuff()
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
AppendCharToRedobuff(c)
	int			   c;
{
	if (!block_redo)
		add_char_buff(&redobuff, c);
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
stuffcharReadbuff(c)
	int			   c;
{
	add_char_buff(&stuffbuff, c);
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
		stuffcharReadbuff(c);
}

extern int redo_Visual_busy;		/* this is in normal.c */

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
		add_char_buff(&stuffbuff, c);
		c = read_redo(FALSE);
	}

	if (c == 'v')	/* redo Visual */
	{
		Visual = Curpos;
		redo_Visual_busy = TRUE;
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
	add_char_buff(&stuffbuff, c);
	copy_redo();
	return TRUE;
}

/*
 * Repeat the last insert (R, o, O, a, A, i or I command) by stuffing
 * the redo buffer into the stuffbuff.
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
		c = TO_UPPER(c);
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

/*
 * insert a string in front of the typeahead buffer (for '@' command and vgetorpeek)
 */
	int
ins_typestr(str, noremap)
	char	*str;
	int		noremap;
{
	register char	*s;
	register int	newlen;
	register int	addlen;

	init_typestr();

	/*
	 * In typestr there must always be room for MAXMAPLEN + 3 characters
	 */
	addlen = strlen(str);
	newlen = strlen(typestr) + addlen + MAXMAPLEN + 3;
	if (newlen < 0)				/* string is getting too long */
	{
		emsg(e_toocompl);		/* also calls flush_buffers */
		setcursor();
		return -1;
	}
	s = alloc(newlen);
	if (s == NULL)				/* out of memory */
		return -1;

	strcpy(s, str);
	strcat(s, typestr);
	if (typestr != typebuf)
		free(typestr);
	typestr = s;
	typemaplen += addlen;		/* the inserted string is not typed */
	if (noremap)
	{
		if (noremaplist.nr_off == 0)
			noremaplist.nr_len += addlen;
		else
		{
			struct noremap *p;

			p = (struct noremap *)alloc((int)sizeof(struct noremap));
			if (p != NULL)
			{
				p->nr_next = noremaplist.nr_next;
				p->nr_off = noremaplist.nr_off;
				p->nr_len = noremaplist.nr_len;
				noremaplist.nr_next = p;
				noremaplist.nr_len = addlen;
				noremaplist.nr_off = 0;
			}
		}
	}
	else if (noremaplist.nr_len)
		noremaplist.nr_off += addlen;
	return 0;
}

/*
 * remove "len" characters from the front of typestr
 */
	void
del_typestr(len)
	int	len;
{
	struct noremap *p;

	strcpy(typestr, typestr + len);		/* remove chars from the buffer */
	if ((typemaplen -= len) <= 0)		/* adjust typemaplen */
		typemaplen = 0;
	while (len)							/* adjust noremaplist */
	{
		if (noremaplist.nr_off >= len)
		{
			noremaplist.nr_off -= len;
			break;
		}
		len -= noremaplist.nr_off;
		noremaplist.nr_off = 0;
		if (noremaplist.nr_len > len)
		{
			noremaplist.nr_len -= len;
			break;
		}
		len -= noremaplist.nr_len;
		p = noremaplist.nr_next;
		if (p == NULL)
		{
			noremaplist.nr_len = 0;
			break;
		}
		noremaplist.nr_next = p->nr_next;
		noremaplist.nr_len = p->nr_len;
		noremaplist.nr_off = p->nr_off;
		free(p);
	}
}

extern int arrow_used;			/* this is in edit.c */

/*
 * Write typed characters to script file.
 * If recording is on put the character in the recordbuffer.
 */
	static void
gotchars(s, len)
	char	*s;
	int		len;
{
	while (len--)
	{
		updatescript(*s & 255);

		if (Recording)
			add_char_buff(&recordbuff, (*s & 255));
		++s;
	}

			/* do not sync in insert mode, unless cursor key has been used */
	if (!(State & (INSERT + CMDLINE)) || arrow_used)		
		u_sync();
}

/*
 * Initialize typestr to point to typebuf.
 * Alloc() cannot be used here: In out-of-memory situations it would
 * be impossible to type anything.
 */
	static void
init_typestr()
{
	if (typestr == NULL)
	{
		typestr = typebuf;
		typebuf[0] = NUL;
	}
}

#define NEEDMORET 9999		/* value for incomplete mapping or key-code */

/*
 * get a character: 1. from the stuffbuffer
 *					2. from the typeahead buffer
 *					3. from the user
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
	int				len;
#ifdef AMIGA
	char			*s;
#endif
	register struct mapblock *mp;
	int				timedout = FALSE;	/* waited for more than 1 second
												for mapping to complete */
	int				mapdepth = 0;		/* check for recursive mapping */
	int				mode_deleted = FALSE;	/* set when mode has been deleted */

	init_typestr();
	start_stuff();
	if (typemaplen == 0)
		Exec_reg = FALSE;
	do
	{
		c = read_stuff(advance);
		if (c != NUL && !got_int)
			KeyTyped = FALSE;
		else
		{
			/*
			 * Loop until we either find a matching mapped key, or we
			 * are sure that it is not a mapped key.
			 * If a mapped key sequence is found we go back to the start to
			 * try re-mapping.
			 */

			for (;;)
			{
				len = strlen(typestr);
				breakcheck();				/* check for CTRL-C */
				if (got_int)
				{
					c = inchar(typestr, MAXMAPLEN, 0);	/* flush all input */
					/*
					 * If inchar returns TRUE (script file was active) or we are
					 * inside a mapping, get out of insert mode.
					 * Otherwise we behave like having gotten a CTRL-C.
					 * As a result typing CTRL-C in insert mode will
					 * really insert a CTRL-C.
					 */
					if ((c || typemaplen) && (State & (INSERT + CMDLINE)))
						c = ESC;
					else
						c = Ctrl('C');
					flush_buffers(TRUE);		/* flush all typeahead */
					break;
				}
				else if (len > 0)	/* see if we have a mapped key sequence */
				{
					/*
					 * walk through the maplist until we find an
					 * entry that matches (if not timed out).
					 */
					mp = NULL;
					if (!timedout && (typemaplen == 0 || (p_remap &&
							(noremaplist.nr_len == 0 || noremaplist.nr_off != 0)))
							&& !((State & (INSERT + CMDLINE)) && p_paste))
					{
						for (mp = maplist.m_next; mp; mp = mp->m_next)
						{
							if ((mp->m_mode & ABBREV) || !(mp->m_mode & State))
								continue;
							n = mp->m_keylen;
							if (noremaplist.nr_off != 0 && n > noremaplist.nr_off)
								continue;
							if (!strncmp(mp->m_keys, typestr, (size_t)(n > len ? len : n)))
								break;
						}
					}
					if (mp == NULL)					/* no match found */
					{
							/*
							 * check if we have a terminal code, when
							 *	mapping is allowed,
							 *  keys have not been mapped,
							 *	and not an ESC sequence, not in insert mode or
							 *		p_ek is on,
							 *	and when not timed out,
							 */
						if (State != NOMAPPING &&
								typemaplen == 0 &&
								(typestr[0] != ESC || p_ek || !(State & INSERT)) &&
								!timedout)
							n = check_termcode(typestr);
						else
							n = 0;
						if (n == 0)		/* no matching terminal code */
						{
#ifdef AMIGA					/* check for window bounds report */
							if (typemaplen == 0 && (typestr[0] & 0xff) == CSI)
							{
								for (s = typestr + 1; isdigit(*s) || *s == ';' || *s == ' '; ++s)
									;
								if (*s == 'r' || *s == '|')	/* found one */
								{
									strcpy(typestr, s + 1);
									set_winsize(0, 0, FALSE);		/* get size and redraw screen */
									continue;
								}
								if (*s == NUL)		/* need more characters */
									n = -1;
							}
							if (n != -1)			/* got a single character */
#endif
							{
								c = typestr[0] & 255;
								if (typemaplen)
									KeyTyped = FALSE;
								else
								{
									KeyTyped = TRUE;
									if (advance)	/* write char to script file(s) */
										gotchars(typestr, 1);
								}
								if (advance)		/* remove chars from typestr */
									del_typestr(1);
								break;		/* got character, break for loop */
							}
						}
						if (n > 0)		/* full matching terminal code */
							continue;	/* try mapping again */

						/* partial match: get some more characters */
						n = NEEDMORET;
					}
					if (n <= len)		/* complete match */
					{
						if (n > typemaplen)		/* write chars to script file(s) */
							gotchars(typestr + typemaplen, n - typemaplen);

						del_typestr(n);	/* remove the mapped keys */

						/*
						 * Put the replacement string in front of mapstr.
						 * The depth check catches ":map x y" and ":map y x".
						 */
						if (++mapdepth == 1000)
						{
							emsg("recursive mapping");
							if (State == CMDLINE)
								redrawcmdline();
							else
								setcursor();
							flush_buffers(FALSE);
							mapdepth = 0;		/* for next one */
							c = -1;
							break;
						}
						if (ins_typestr(mp->m_str, mp->m_noremap) < 0)
						{
							c = -1;
							break;
						}
						continue;
					}
				}
				/*
				 * special case: if we get an <ESC> in insert mode and there are
				 * no more characters at once, we pretend to go out of insert mode.
				 * This prevents the one second delay after typing an <ESC>.
				 * If we get something after all, we may have to redisplay the
				 * mode. That the cursor is in the wrong place does not matter.
				 */
				c = 0;
				if (advance && len == 1 && typestr[0] == ESC && typemaplen == 0 && (State & INSERT) && (p_timeout || (n == NEEDMORET && p_ttimeout)) && (c = inchar(typestr + len, 2, 0)) == 0)
				{
					if (p_smd)
					{
						delmode();
						mode_deleted = TRUE;
					}
					if (Curscol)		/* move cursor one left if possible */
						--Curscol;
					else if (p_wrap && Curpos.col != 0 && Cursrow)
					{
							--Cursrow;
							Curscol = Columns - 1;
					}
					setcursor();
					flushbuf();
				}
				len += c;

				if (len >= typemaplen + MAXMAPLEN)	/* buffer full, don't map */
				{
					timedout = TRUE;
					continue;
				}
				c = inchar(typestr + len, typemaplen + MAXMAPLEN - len, !advance ? 0 : ((len == 0 || !(p_timeout || (p_ttimeout && n == NEEDMORET))) ? -1 : (int)p_tm));
				if (c <= NUL)		/* no character available */
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
					if (mode_deleted)		/* character entered after ESC */
					{
						showmode();
						mode_deleted = FALSE;
					}
				}
			}		/* for (;;) */
		}		/* if (!character from stuffbuf) */

						/* if advance is FALSE don't loop on NULs */
	} while (c < 0 || (advance && c == NUL));

		/* delete "INSERT" message if we return an ESC */
	if (c == ESC && p_smd && !mode_deleted && (State & INSERT))
		delmode();

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
 * map[!]					: show all key mappings
 * map[!] {lhs}				: show key mapping for {lhs}
 * map[!] {lhs} {rhs}		: set key mapping for {lhs} to {rhs}
 * noremap[!] {lhs} {rhs}	: same, but no remapping for {rhs}
 * unmap[!] {lhs}			: remove key mapping for {lhs}
 * abbr						: show all abbreviations
 * abbr {lhs}				: show abbreviations for {lhs}
 * abbr {lhs} {rhs}			: set abbreviation for {lhs} to {rhs}
 * noreabbr {lhs} {rhs}		: same, but no remapping for {rhs}
 * unabbr {lhs}				: remove abbreviation for {lhs}
 *
 * maptype == 1 for unmap command, 2 for noremap command.
 *
 * keys is pointer to any arguments.
 *
 * for :map	  mode is NORMAL 
 * for :map!  mode is INSERT + CMDLINE
 * for :cmap  mode is CMDLINE
 * for :imap  mode is INSERT 
 * for :abbr  mode is INSERT + CMDLINE + ABBREV
 * for :iabbr mode is INSERT + ABBREV
 * for :cabbr mode is CMDLINE + ABBREV
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
	int					haskey;
	int					did_it = FALSE;
	int					abbrev = 0;
	int					round;

	if (mode & ABBREV)		/* not a mapping but an abbreviation */
	{
		abbrev = ABBREV;
		mode &= ~ABBREV;
	}
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
	arg = p;
	hasarg = (*arg != NUL);
	haskey = (*keys != NUL);

		/* check for :unmap with not one argument */
	if (maptype == 1 && (!haskey || hasarg))	
		return 1;

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
	if (haskey)
	{
		if (*keys == '#' && isdigit(*(keys + 1)))	/* function key */
		{
			if (*++keys == '0')
				*(u_char *)keys = K_F10;
			else
				*keys += K_F1 - '1';
		}
		len = strlen(keys);
		if (len > MAXMAPLEN)			/* maximum lenght of MAXMAPLEN chars */
			return 2;
	}

	if (haskey && hasarg && abbrev)		/* will add an abbreviation */
		no_abbr = FALSE;

#ifdef AMIGA
	if (!haskey || (maptype != 1 && !hasarg))
		settmode(0);				/* set cooked mode so output can be halted */
#endif
/*
 * Find an entry in the maplist that matches.
 * For :unmap we may loop two times: once to try to unmap an entry with a
 * matching 'from' part, a second time, if the first fails, to unmap an
 * entry with a matching 'to' part. This was done to allow ":ab foo bar" to be
 * unmapped by typing ":unab foo", where "foo" will be replaced by "bar" because
 * of the abbreviation.
 */
	for (round = 0; (round == 0 || maptype == 1) && round <= 1 && !did_it; ++round)
	{
		for (mp = maplist.m_next, mprev = &maplist; mp; mprev = mp, mp = mp->m_next)
		{
										/* skip entries with wrong mode */
			if (!(mp->m_mode & mode) || (mp->m_mode & ABBREV) != abbrev)
				continue;
			if (!haskey)						/* show all entries */
			{
				showmap(mp);
				did_it = TRUE;
			}
			else								/* do we have a match? */
			{
				if (round)		/* second round: try 'to' string for unmap */
				{
					n = strlen(mp->m_str);
					p = mp->m_str;
				}
				else
				{
					n = mp->m_keylen;
					p = mp->m_keys;
				}
				if (!strncmp(p, keys, (size_t)(n < len ? n : len)))
				{
					if (maptype == 1)			/* delete entry */
					{
						if (n != len)			/* not a full match */
							continue;
						/*
						 * We reset the indicated mode bits. If nothing is left the
						 * entry is deleted below.
						 */
						mp->m_mode &= (~mode | ABBREV);
						did_it = TRUE;			/* remember that we did something */
					}
					else if (!hasarg)			/* show matching entry */
					{
						showmap(mp);
						did_it = TRUE;
					}
					else if (n != len)			/* new entry is ambigious */
					{
						return 3;
					}
					else
					{
						mp->m_mode &= (~mode | ABBREV);		/* remove mode bits */
						if (!(mp->m_mode & ~ABBREV) && !did_it)	/* reuse existing entry */
						{
							newstr = strsave(arg);
							if (newstr == NULL)
								return 4;			/* no mem */
							free(mp->m_str);
							mp->m_str = newstr;
							mp->m_noremap = maptype;
							mp->m_mode = mode + abbrev;
							did_it = TRUE;
						}
					}
					if (!(mp->m_mode & ~ABBREV))		/* entry can be deleted */
					{
						free(mp->m_keys);
						free(mp->m_str);
						mprev->m_next = mp->m_next;
						free(mp);
						mp = mprev;					/* continue with next entry */
					}
				}
			}
		}
	}

	if (maptype == 1)						/* delete entry */
	{
		if (did_it)
			return 0;						/* removed OK */
		else
			return 2;						/* no match */
	}

	if (!haskey || !hasarg)					/* print entries */
	{
#ifdef AMIGA
		settmode(1);
#endif
		if (did_it)
			wait_return(TRUE);
		else if (abbrev)
			msg("No abbreviation found");
		else
			msg("No mapping found");
		return 0;							/* listing finished */
	}

	if (did_it)					/* have added the new entry already */
		return 0;
/*
 * get here when we have to add a new entry
 */
		/* allocate a new entry for the maplist */
	mp = (struct mapblock *)alloc((unsigned)sizeof(struct mapblock));
	if (mp == NULL)
		return 4;			/* no mem */
	mp->m_keys = strsave(keys);
	mp->m_str = strsave(arg);
	if (mp->m_keys == NULL || mp->m_str == NULL)
	{
			free(mp->m_keys);
			free(mp->m_str);
			free(mp);
			return 4;		/* no mem */
	}
	mp->m_keylen = strlen(mp->m_keys);
	mp->m_noremap = maptype;
	mp->m_mode = mode + abbrev;

	/* add the new entry in front of the maplist */
	mp->m_next = maplist.m_next;
	maplist.m_next = mp;

	return 0;				/* added OK */
}

	static void
showmap(mp)
	struct mapblock *mp;
{
	int len;

	if ((mp->m_mode & (INSERT + CMDLINE)) == INSERT + CMDLINE)
		outstr("! ");
	else if (mp->m_mode & INSERT)
		outstr("i ");
	else if (mp->m_mode & CMDLINE)
		outstr("c ");
	len = outtrans(mp->m_keys, -1);	/* get length of what we have written */
	do
	{
		outchar(' ');				/* padd with blanks */
		++len;
	} while (len < 12);
	if (mp->m_noremap)
		outchar('*');
	else
		outchar(' ');
	outtrans(mp->m_str, -1);
	outchar('\n');
	flushbuf();
}

/*
 * Check for an abbreviation.
 * Cursor is at ptr[col]. When inserting, mincol is where insert started.
 * "c" is the character typed before check_abbr was called.
 */
	int
check_abbr(c, ptr, col, mincol)
	int		c;
	char	*ptr;
	int		col;
	int		mincol;
{
	int				len;
	int				j;
	char			tb[3];
	struct mapblock *mp;

	for (len = col; len > 0 && isidchar(ptr[len - 1]); --len)
		;
	if (len < mincol)
		len = mincol;
	if (len < col)				/* there is a word in front of the cursor */
	{
		ptr += len;
		len = col - len;
		for (mp = maplist.m_next; mp; mp = mp->m_next)
		{
					/* find entries with right mode and keys */
			if ((mp->m_mode & ABBREV) == ABBREV &&
						(mp->m_mode & State) &&
						mp->m_keylen == len &&
						!strncmp(mp->m_keys, ptr, (size_t)len))
				break;
		}
		if (mp)								/* found a match */
		{
			j = 0;
			if (c < 0x100 && (c < ' ' || c > '~'))
				tb[j++] = Ctrl('V');		/* special char needs CTRL-V */
			tb[j++] = c;
			tb[j] = NUL;
			ins_typestr(tb, TRUE);			/* insert the last typed char */
			ins_typestr(mp->m_str, mp->m_noremap);	/* insert the to string */
			while (len--)
				ins_typestr("\b", TRUE);	/* delete the from string */
			return TRUE;
		}
	}
	return FALSE;
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return 1 on error.
 */
	int
makemap(fd)
	FILE *fd;
{
	struct mapblock *mp;
	char			c1;
	char 			*p;

	for (mp = maplist.m_next; mp; mp = mp->m_next)
	{
		c1 = NUL;
		p = "map";
		switch (mp->m_mode)
		{
		case NORMAL:
			break;
		case CMDLINE + INSERT:
			p = "map!";
			break;
		case CMDLINE:
			c1 = 'c';
			break;
		case INSERT:
			c1 = 'i';
			break;
		case INSERT + CMDLINE + ABBREV:
			p = "abbr";
			break;
		case CMDLINE + ABBREV:
			c1 = 'c';
			p = "abbr";
			break;
		case INSERT + ABBREV:
			c1 = 'i';
			p = "abbr";
			break;
		default:
			emsg("makemap: Illegal mode");
			return 1;
		}
		if (c1 && putc(c1, fd) < 0)
			return 1;
		if (mp->m_noremap && fprintf(fd, "nore") < 0)
			return 1;
		if (fprintf(fd, p) < 0)
			return 1;

		if (	putc(' ', fd) < 0 || putescstr(fd, mp->m_keys, FALSE) < 0 ||
				putc(' ', fd) < 0 || putescstr(fd, mp->m_str, FALSE) < 0 ||
#ifdef MSDOS
				putc('\r', fd) < 0 ||
#endif
				putc('\n', fd) < 0)
			return 1;
	}
	return 0;
}

	int
putescstr(fd, str, set)
	FILE		*fd;
	char		*str;
	int			set;		/* TRUE for makeset, FALSE for makemap */
{
	for ( ; *str; ++str)
	{
		/*
		 * some characters have to be escaped with CTRL-V to
		 * prevent them from misinterpreted in DoOneCmd().
		 * A space has to be escaped with a backslash to
		 * prevent it to be misinterpreted in doset().
		 */
		if (*str < ' ' || *str > '~' || (*str == ' ' && !set))
		{
			if (putc(Ctrl('V'), fd) < 0)
				return -1;
		}
		else if ((set && *str == ' ') || *str == '|')
		{
			if (putc('\\', fd) < 0)
				return -1;
		}
		if (putc(*str, fd) < 0)
			return -1;
	}
	return 0;
}
