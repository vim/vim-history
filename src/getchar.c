/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * getchar.c
 *
 * functions related with getting a character from the user/mapping/redo/...
 *
 * manipulations with redo buffer and stuff buffer
 * mappings and abbreviations
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
		char_u			b_str[1];		/* contents (actually longer) */
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
	char_u			*m_keys;		/* mapped from */
	int				 m_keylen;		/* strlen(m_keys) */
	char_u			*m_str; 		/* mapped to */
	int 			 m_mode;		/* valid mode */
	int				 m_noremap;		/* if non-zero no re-mapping for m_str */
};

static struct mapblock maplist = {NULL, NULL, 0, NULL, 0, 0};
									/* first dummy entry in maplist */

/*
 * variables used by vgetorpeek() and flush_buffers()
 *
 * typestr contains all characters that are not consumed yet.
 * The part in front may contain the result of mappings, abbreviations and
 * @a commands. The lenght of this part is typemaplen.
 * After it are characters that come from the terminal.
 * no_abbr_cnt is the number of characters in typestr that should not be
 * considered for abbreviations.
 * Some parts of typestr may not be mapped. These parts are remembered in
 * the noremaplist. 
 */
#define MAXMAPLEN 50				/* maximum length of key sequence to be mapped */
									/* must be able to hold an Amiga resize report */
static char_u	*typestr = NULL;	/* NUL-terminated buffer for typeahead characters */
static char_u	typebuf[MAXMAPLEN + 3]; /* initial typestr */

static int		typemaplen = 0;		/* number of mapped characters in typestr */
static int		no_abbr_cnt = 0;	/* number of chars without abbrev. in typestr */

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
static char_u	*get_bufcont __ARGS((struct buffheader *, int));
static void		add_buff __ARGS((struct buffheader *, char_u *));
static void		add_num_buff __ARGS((struct buffheader *, long));
static void		add_char_buff __ARGS((struct buffheader *, int));
static int		read_stuff __ARGS((int));
static void		start_stuff __ARGS((void));
static int		read_redo __ARGS((int));
static void		init_typestr __ARGS((void));
static void		gotchars __ARGS((char_u *, int));
static int		vgetorpeek __ARGS((int));
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
				free(p);
		}
		buf->bh_first.b_next = NULL;
}

/*
 * return the contents of a buffer as a single string
 */
	static char_u *
get_bufcont(buffer, dozero)
	struct buffheader	*buffer;
	int					dozero;		/* count == zero is not an error */
{
		long_u			count = 0;
		char_u			*p = NULL;
		struct bufblock	*bp;

/* compute the total length of the string */
		for (bp = buffer->bh_first.b_next; bp != NULL; bp = bp->b_next)
				count += STRLEN(bp->b_str);

		if ((count || dozero) && (p = lalloc(count + 1, TRUE)) != NULL)
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
	char_u *
get_recorded()
{
	char_u *p;

	p = get_bufcont(&recordbuff, TRUE);
	free_buff(&recordbuff);
	return (p);
}

/*
 * return the contents of the redo buffer as a single string
 */
	char_u *
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
	char_u						*s;
{
	struct bufblock *p;
	long_u 			n;
	long_u 			len;

	if ((n = STRLEN(s)) == 0)				/* don't add empty strings */
		return;

	if (buf->bh_first.b_next == NULL)		/* first add to list */
	{
		buf->bh_space = 0;
		buf->bh_curr = &(buf->bh_first);
	}
	else if (buf->bh_curr == NULL)			/* buffer has already been read */
	{
		EMSG("Add to read buffer");
		return;
	}
	else if (buf->bh_index != 0)
		STRCPY(buf->bh_first.b_next->b_str, buf->bh_first.b_next->b_str + buf->bh_index);
	buf->bh_index = 0;

	if (buf->bh_space >= n)
	{
		strcat((char *)buf->bh_curr->b_str, (char *)s);
		buf->bh_space -= n;
	}
	else
	{
		if (n < MINIMAL_SIZE)
			len = MINIMAL_SIZE;
		else
			len = n;
		p = (struct bufblock *)lalloc((long_u)(sizeof(struct bufblock) + len), TRUE);
		if (p == NULL)
			return; /* no space, just forget it */
		buf->bh_space = len - n;
		STRCPY(p->b_str, s);

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
		char_u	number[32];

		sprintf((char *)number, "%ld", n);
		add_buff(buf, number);
}

	static void
add_char_buff(buf, c)
	struct buffheader *buf;
	int 			  c;
{
		char_u	temp[2];

		temp[0] = c;
		temp[1] = NUL;
		add_buff(buf, temp);
}

/*
 * get one character from the stuff buffer
 * If advance == TRUE go to the next char.
 */
	static int
read_stuff(advance)
	int			advance;
{
	register char_u c;
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
			free(curr);
			stuffbuff.bh_index = 0;
		}
	}
	return c;
}

/*
 * prepare stuff buffer for reading (if it contains something)
 */
	static void
start_stuff()
{
	if (stuffbuff.bh_first.b_next != NULL)
	{
		stuffbuff.bh_curr = &(stuffbuff.bh_first);
		stuffbuff.bh_space = 0;
	}
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
		STRCPY(typestr, typestr + typemaplen);
	typemaplen = 0;
	no_abbr_cnt = 0;
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
	char_u		   *s;
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
	char_u		   *s;
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
 * if init is TRUE, prepare for redo, return FAIL if nothing to redo, OK otherwise
 */
	static int
read_redo(init)
	int			init;
{
	static struct bufblock	*bp;
	static char_u			*p;
	int						c;

	if (init)
	{
		if ((bp = redobuff.bh_first.b_next) == NULL)
			return FAIL;
		p = bp->b_str;
		return OK;
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
 * return FAIL for failure, OK otherwise
 */
	int
start_redo(count)
	long count;
{
	register int c;

	if (read_redo(TRUE) == FAIL)	/* init the pointers; return if nothing to redo */
		return FAIL;

	c = read_redo(FALSE);

/* copy the buffer name, if present */
	if (c == '"')
	{
		add_buff(&stuffbuff, (char_u *)"\"");
		c = read_redo(FALSE);

/* if a numbered buffer is used, increment the number */
		if (c >= '1' && c < '9')
			++c;
		add_char_buff(&stuffbuff, c);
		c = read_redo(FALSE);
	}

	if (c == 'v')	/* redo Visual */
	{
		VIsual = curwin->w_cursor;
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
	return OK;
}

/*
 * Repeat the last insert (R, o, O, a, A, i or I command) by stuffing
 * the redo buffer into the stuffbuff.
 * return FAIL for failure, OK otherwise
 */
	int
start_redo_ins()
{
	register int c;

	if (read_redo(TRUE) == FAIL)
		return FAIL;
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
	return OK;
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

/*
 * insert a string in front of the typeahead buffer (for '@' command and vgetorpeek)
 * return FAIL for failure, OK otherwise
 */
	int
ins_typestr(str, noremap)
	char_u	*str;
	int		noremap;
{
	register char_u	*s;
	register int	newlen;
	register int	addlen;

	init_typestr();

	/*
	 * In typestr there must always be room for MAXMAPLEN + 3 characters
	 */
	addlen = STRLEN(str);
	newlen = STRLEN(typestr) + addlen + MAXMAPLEN + 3;
	if (newlen < 0)				/* string is getting too long */
	{
		emsg(e_toocompl);		/* also calls flush_buffers */
		setcursor();
		return FAIL;
	}
	s = alloc(newlen);
	if (s == NULL)				/* out of memory */
		return FAIL;

	STRCPY(s, str);
	STRCAT(s, typestr);
	if (typestr != typebuf)
		free(typestr);
	typestr = s;
	typemaplen += addlen;		/* the inserted string is not typed */
	if (no_abbr_cnt)			/* and not used for abbreviations */
		no_abbr_cnt += addlen;
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
	return OK;
}

/*
 * remove "len" characters from the front of typestr
 */
	void
del_typestr(len)
	int	len;
{
	struct noremap *p;

	STRCPY(typestr, typestr + len);
										/* remove chars from the buffer */
	if ((typemaplen -= len) < 0)		/* adjust typemaplen */
		typemaplen = 0;
	if ((no_abbr_cnt -= len) < 0)		/* adjust no_abbr_cnt */
		no_abbr_cnt = 0;

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
	char_u	*s;
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
 * open new script file for ":so!" command
 * return OK on success, FAIL on error
 */
	int
openscript(name)
	char_u *name;
{
	int oldcurscript;

	if (curscript + 1 == NSCRIPT)
	{
		emsg(e_nesting);
		return FAIL;
	}
	else
	{
		if (scriptin[curscript] != NULL)	/* already reading script */
			++curscript;
		if ((scriptin[curscript] = fopen((char *)name, READBIN)) == NULL)
		{
			emsg2(e_notopen, name);
			if (curscript)
				--curscript;
			return FAIL;
		}
		/*
		 * With command ":g/pat/so! file" we have to execute the
		 * commands from the file now.
		 */
		if (global_busy)
		{
			State = NORMAL;
			oldcurscript = curscript;
			do
			{
				normal();
				vpeekc();			/* check for end of file */
			}
			while (scriptin[oldcurscript]);
			State = CMDLINE;
		}
	}
	return OK;
}

/*
 * updatescipt() is called when a character can be written into the script file
 * or when we have waited some time for a character (c == 0)
 *
 * All the changed memfiles are synced if c == 0 or when the number of typed
 * characters reaches 'updatecount'.
 */
	void
updatescript(c)
	int c;
{
	static int		count = 0;

	if (c && scriptout)
		putc(c, scriptout);
	if (c == 0 || ++count >= p_uc)
	{
		ml_sync_all(c == 0);
		count = 0;
	}
}

#define NEEDMORET 9999		/* value for incomplete mapping or key-code */

/*
 * get a character: 1. from the stuffbuffer
 *					2. from the typeahead buffer
 *					3. from the user
 *
 * KeyTyped is set to TRUE in the case the user typed the key.
 * vgetc() (advance is TRUE): really get the character.
 * vpeekc() (advance is FALSE): just look whether there is a character available.
 */
	int
vgetc()
{
	return (vgetorpeek(TRUE));
}

	int
vpeekc()
{
	return (vgetorpeek(FALSE));
}

	static int
vgetorpeek(advance)
	int		advance;
{
	register int	c;
	int				n = 0;		/* init for GCC */
	int				len;
#ifdef AMIGA
	char_u			*s;
#endif
	register struct mapblock *mp;
	int				timedout = FALSE;		/* waited for more than 1 second
												for mapping to complete */
	int				mapdepth = 0;			/* check for recursive mapping */
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
				len = STRLEN(typestr);
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
					 * entry that matches.
					 *
					 * Don't look for mappings if:
					 * - timed out
					 * - typestr[0] should not be remapped
					 * - in insert or cmdline mode and 'paste' option set
					 * - waiting for "hit return to continue" and CR or SPACE typed
					 */
					mp = NULL;
					if (!timedout && (typemaplen == 0 || (p_remap &&
							(noremaplist.nr_len == 0 || noremaplist.nr_off != 0)))
							&& !((State & (INSERT + CMDLINE)) && p_paste)
							&& !(State == HITRETURN && (typestr[0] == CR || typestr[0] == ' ')))
					{
						for (mp = maplist.m_next; mp; mp = mp->m_next)
						{
							if ((mp->m_mode & ABBREV) || !(mp->m_mode & State))
								continue;
							n = mp->m_keylen;
							if (noremaplist.nr_off != 0 && n > noremaplist.nr_off)
								continue;
							if (!STRNCMP(mp->m_keys, typestr, (size_t)(n > len ? len : n)))
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
								/* typemaplen == 0 && */ /* allow mapped keys anyway */
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
									STRCPY(typestr, s + 1);
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
							EMSG("recursive mapping");
							if (State == CMDLINE)
								redrawcmdline();
							else
								setcursor();
							flush_buffers(FALSE);
							mapdepth = 0;		/* for next one */
							c = -1;
							break;
						}
						if (ins_typestr(mp->m_str, mp->m_noremap) == FAIL)
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
					if (curwin->w_cursor.col != 0)	/* move cursor one left if possible */
					{
						if (curwin->w_col)
						{
							if (did_ai)
							{
								if (curwin->w_p_nu)
									curwin->w_col = 8;
								else
									curwin->w_col = 0;
							}
							else
								--curwin->w_col;
						}
						else if (curwin->w_p_wrap && curwin->w_row)
						{
								--curwin->w_row;
								curwin->w_col = Columns - 1;
						}
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
			}		/* for (;;) */
		}		/* if (!character from stuffbuf) */

						/* if advance is FALSE don't loop on NULs */
	} while (c < 0 || (advance && c == NUL));

	/*
	 * The "INSERT" message is taken care of here:
	 *   if we return an ESC the message is deleted
	 *   if we don't return an ESC but deleted the message before, redisplay it
	 */
	if (p_smd && (State & INSERT))
	{
		if (c == ESC && !mode_deleted)
			delmode();
		else if (c != ESC && mode_deleted)
			showmode();
	}

	return c;
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
	char_u	*keys;
	int		mode;
{
	struct mapblock		*mp, *mprev;
	char_u				*arg;
	char_u				*p;
	int					n = 0;			/* init for GCC */
	int					len = 0;		/* init for GCC */
	char_u				*newstr;
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
 * with :unmap white space is included in the keys, no argument possible
 */
	p = keys;
	while (*p && (maptype == 1 || !iswhite(*p)))
	{
		if (*p == Ctrl('V') && p[1] != NUL)
			STRCPY(p, p + 1);			/* remove CTRL-V */
		++p;
	}
	if (*p != NUL)
		*p++ = NUL;
	skipspace(&p);
	arg = p;
	hasarg = (*arg != NUL);
	haskey = (*keys != NUL);

		/* check for :unmap without argument */
	if (maptype == 1 && !haskey)	
		return 1;

/*
 * remove CTRL-Vs from argument
 */
	while (*p)
	{
		if (*p == Ctrl('V') && p[1] != NUL)
			STRCPY(p, p + 1);			/* remove CTRL-V */
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
				*keys = K_F10;
			else
				*keys += K_F1 - '1';
		}
		len = STRLEN(keys);
		if (len > MAXMAPLEN)			/* maximum lenght of MAXMAPLEN chars */
			return 1;

		/*
		 * abbreviation must end in id-char
		 * rest must be all id-char or all non-id-char
		 */
		if (abbrev)
		{
			if (!isidchar(*(keys + len - 1)))		/* does not end in id char */
				return 1;
			for (n = 0; n < len - 2; ++n)
				if (isidchar(*(keys + n)) != isidchar(*(keys + len - 2)))
					return 1;
		}
	}

	if (haskey && hasarg && abbrev)		/* if we will add an abbreviation */
		no_abbr = FALSE;				/* reset flag that indicates there are
															no abbreviations */

	if (!haskey || (maptype != 1 && !hasarg))
		msg_start();
/*
 * Find an entry in the maplist that matches.
 * For :unmap we may loop two times: once to try to unmap an entry with a
 * matching 'from' part, a second time, if the first fails, to unmap an
 * entry with a matching 'to' part. This was done to allow ":ab foo bar" to be
 * unmapped by typing ":unab foo", where "foo" will be replaced by "bar" because
 * of the abbreviation.
 */
	for (round = 0; (round == 0 || maptype == 1) && round <= 1 && !did_it && !got_int; ++round)
	{
		for (mp = maplist.m_next, mprev = &maplist; mp && !got_int; mprev = mp, mp = mp->m_next)
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
					n = STRLEN(mp->m_str);
					p = mp->m_str;
				}
				else
				{
					n = mp->m_keylen;
					p = mp->m_keys;
				}
				if (!STRNCMP(p, keys, (size_t)(n < len ? n : len)))
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
						if (abbrev)				/* for abbreviations that's ok */
							continue;
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
		if (did_it)
			msg_end();
		else if (abbrev)
			MSG("No abbreviation found");
		else
			MSG("No mapping found");
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
	mp->m_keylen = STRLEN(mp->m_keys);
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

	msg_pos(-1, 0);						/* always start in column 0 */
	if ((mp->m_mode & (INSERT + CMDLINE)) == INSERT + CMDLINE)
		msg_outstr((char_u *)"! ");
	else if (mp->m_mode & INSERT)
		msg_outstr((char_u *)"i ");
	else if (mp->m_mode & CMDLINE)
		msg_outstr((char_u *)"c ");
	len = msg_outtrans(mp->m_keys, -1);	/* get length of what we have written */
	do
	{
		msg_outchar(' ');				/* padd with blanks */
		++len;
	} while (len < 12);
	if (mp->m_noremap)
		msg_outchar('*');
	else
		msg_outchar(' ');
	msg_outtrans(mp->m_str, -1);
	msg_outchar('\n');
	flushbuf();							/* show one line at a time */
}

/*
 * Check for an abbreviation.
 * Cursor is at ptr[col]. When inserting, mincol is where insert started.
 * "c" is the character typed before check_abbr was called.
 *
 * Historic vi practice: The last character of an abbreviation must be an id
 * character ([a-zA-Z0-9_]). The characters in front of it must be all id
 * characters or all non-id characters. This allows for abbr. "#i" to "#include".
 *
 * return TRUE if there is an abbreviation, FALSE if not
 */
	int
check_abbr(c, ptr, col, mincol)
	int		c;
	char_u	*ptr;
	int		col;
	int		mincol;
{
	int				len;
	int				j;
	char_u			tb[3];
	struct mapblock *mp;
	int				is_id = TRUE;

	if (no_abbr_cnt)		/* abbrev. are not recursive */
		return FALSE;

	if (col == 0 || !isidchar(ptr[col - 1]))	/* cannot be an abbr. */
		return FALSE;

	if (col > 1)
		is_id = isidchar(ptr[col - 2]);
	for (len = col - 1; len > 0 && !isspace(ptr[len - 1]) &&
								is_id == isidchar(ptr[len - 1]); --len)
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
						!STRNCMP(mp->m_keys, ptr, (size_t)len))
				break;
		}
		if (mp)								/* found a match */
		{
			j = 0;
			if (c < 0x100 && (c < ' ' || c > '~'))
				tb[j++] = Ctrl('V');		/* special char needs CTRL-V */
			tb[j++] = c;
			tb[j] = NUL;
			(void)ins_typestr(tb, TRUE);					/* insert the last typed char */
			(void)ins_typestr(mp->m_str, mp->m_noremap);	/* insert the to string */
			no_abbr_cnt += STRLEN(mp->m_str) + j;	/* no abbrev. for these chars */
			while (len--)
				(void)ins_typestr((char_u *)"\b", TRUE);	/* delete the from string */
			return TRUE;
		}
	}
	return FALSE;
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
	int
makemap(fd)
	FILE *fd;
{
	struct mapblock *mp;
	char_u			c1;
	char_u 			*p;

	for (mp = maplist.m_next; mp; mp = mp->m_next)
	{
		c1 = NUL;
		p = (char_u *)"map";
		switch (mp->m_mode)
		{
		case NORMAL:
			break;
		case CMDLINE + INSERT:
			p = (char_u *)"map!";
			break;
		case CMDLINE:
			c1 = 'c';
			break;
		case INSERT:
			c1 = 'i';
			break;
		case INSERT + CMDLINE + ABBREV:
			p = (char_u *)"abbr";
			break;
		case CMDLINE + ABBREV:
			c1 = 'c';
			p = (char_u *)"abbr";
			break;
		case INSERT + ABBREV:
			c1 = 'i';
			p = (char_u *)"abbr";
			break;
		default:
			EMSG("makemap: Illegal mode");
			return FAIL;
		}
		if (c1 && putc(c1, fd) < 0)
			return FAIL;
		if (mp->m_noremap && fprintf(fd, "nore") < 0)
			return FAIL;
		if (fprintf(fd, (char *)p) < 0)
			return FAIL;

		if (	putc(' ', fd) < 0 || putescstr(fd, mp->m_keys, FALSE) == FAIL ||
				putc(' ', fd) < 0 || putescstr(fd, mp->m_str, FALSE) == FAIL ||
#ifdef MSDOS
				putc('\r', fd) < 0 ||
#endif
				putc('\n', fd) < 0)
			return FAIL;
	}
	return OK;
}

/*
 * write escape string to file
 *
 * return FAIL for failure, OK otherwise
 */
	int
putescstr(fd, str, set)
	FILE		*fd;
	char_u		*str;
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
				return FAIL;
		}
		else if ((set && *str == ' ') || *str == '|')
		{
			if (putc('\\', fd) < 0)
				return FAIL;
		}
		if (putc(*str, fd) < 0)
			return FAIL;
	}
	return OK;
}
