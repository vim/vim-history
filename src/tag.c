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
 * Code to handle tags and the tag stack
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"
#include "mark.h"

#define TAGSTACKSIZE 20

/*
 * the taggy struct is used to store the information about a :tag command:
 *	the tag name and the cursor position BEFORE the :tag command
 */
struct taggy
{
	char			*tagname;			/* tag name */
	struct filemark fmark;				/* cursor position */
};

/*
 * the tagstack grows from 0 upwards:
 * entry 0: older
 * entry 1: newer
 * entry 2: newest
 */
static struct taggy tagstack[TAGSTACKSIZE];	/* the tag stack */
static int tagstackidx = 0;				/* index just below active entry */
static int tagstacklen = 0;				/* number of tags on the stack */

static int findtag __ARGS((char *));

static char bottommsg[] = "at bottom of tag stack";
static char topmsg[] = "at top of tag stack";

/*
 * Jump to tag; handling of tag stack
 *
 * *tag != NUL (:tag): jump to new tag, add to tag stack
 * type == 1 (:pop) || type == 2 (CTRL-T): jump to old position
 * type == 0 (:tag): jump to old tag
 */
	void
dotag(tag, type, count)
	char	*tag;
	int		type;
	int		count;
{
	int 			i;
	struct taggy	temptag;

	if (*tag != NUL)						/* new pattern, add to the stack */
	{
		/*
		 * if last used entry is not at the top, put it at the top by rotating
		 * the stack until it is (the newer entries will be at the bottom)
		 */
		while (tagstackidx < tagstacklen)
		{
			temptag = tagstack[tagstacklen - 1];
			for (i = tagstacklen - 1; i > 0; --i)
				tagstack[i] = tagstack[i - 1];
			tagstack[0] = temptag;
			++tagstackidx;
		}
				/* if tagstack is full: remove oldest entry */
		if (++tagstacklen > TAGSTACKSIZE)
		{
			tagstacklen = TAGSTACKSIZE;
			free(tagstack[0].tagname);
			for (i = 1; i < tagstacklen; ++i)
				tagstack[i - 1] = tagstack[i];
			--tagstackidx;
		}
	/*
	 * remember the tag and the position before the jump
	 */
		tagstack[tagstackidx].tagname = strsave(tag);
		tagstack[tagstackidx].fmark.lnum = Curpos.lnum;
		tagstack[tagstackidx].fmark.mark.col = Curpos.col;
		tagstack[tagstackidx].fmark.mark.ptr = nr2ptr(Curpos.lnum);
		tagstack[tagstackidx].fmark.fnum = 0;
	}
	else if (tagstacklen == 0)					/* empty stack */
	{
		emsg("tag stack empty");
		return;
	}
	else if (type)								/* go to older position */
	{
		if ((tagstackidx -= count) < 0)
		{
			tagstackidx = 0;
			emsg(bottommsg);
		}
		else if (tagstackidx >= tagstacklen)	/* must have been count == 0 */
		{
			emsg(topmsg);
			return;
		}
		if (tagstack[tagstackidx].fmark.mark.ptr == NULL)	/* jump to other file */
		{
			if (getaltfile(tagstack[tagstackidx].fmark.fnum - 1, tagstack[tagstackidx].fmark.lnum, TRUE))
			{
				/* emsg(e_notopen); */
				return;
			}
			/* "refresh" this position, so we will not fall off the altfile array */
			tagstack[tagstackidx].fmark.fnum = 0;
			tagstack[tagstackidx].fmark.mark.ptr = nr2ptr(Curpos.lnum);
		}
		else
			Curpos.lnum = ptr2nr(tagstack[tagstackidx].fmark.mark.ptr, (linenr_t)1);
		Curpos.col = tagstack[tagstackidx].fmark.mark.col;
		return;
	}
	else									/* go to newer pattern */
	{
		if ((tagstackidx += count - 1) >= tagstacklen)
		{
			tagstackidx = tagstacklen - 1;
			emsg(topmsg);
		}
		else if (tagstackidx < 0)			/* must have been count == 0 */
		{
			emsg(bottommsg);
			tagstackidx = 0;
			return;
		}
	}
	if (findtag(tagstack[tagstackidx].tagname) > 0)
		++tagstackidx;
	else if (bufempty())		/* "vim -t tag" failed, start script now */
		startscript();
}

/*
 * invalidate the line pointer for all tags
 * called when abandoning the current file
 */
	void
clrtags()
{
	int			i;

	for (i = 0; i < tagstacklen; ++i)
		tagstack[i].fmark.mark.ptr = NULL;
}

/*
 * increment the file number for all tags
 * called when adding a file to the file stack
 */
	void
incrtags()
{
	int			i;

	for (i = 0; i < tagstacklen; ++i)
	{
#if 0		/* this would take too much time */
		if (tagstack[i].fmark.fnum == 0)	/* current file */
			tagstack[i].fmark.lnum = ptr2nr(tagstack[i].fmark.mark.ptr, 1);
#endif
		++tagstack[i].fmark.fnum;
	}
}

/*
 * decrement the file number for the tags of the current file
 * called when not adding the current file name to the file stack
 */
	void
decrtags()
{
	int			i;

	for (i = 0; i < tagstacklen; ++i)
		if (tagstack[i].fmark.fnum == 1)
			tagstack[i].fmark.fnum = 0;
}

/*
 * Print the tag stack (use the occasion to update the line numbers)
 */
	void
dotags()
{
	int			i;
	char		*name;

#ifdef AMIGA
	settmode(0);		/* set cooked mode so output can be halted */
#endif
	outstrn("\n  # TO tag      FROM line in file\n");
	for (i = 0; i < tagstacklen; ++i)
	{
		if (tagstack[i].tagname != NULL)
		{
			name = fm_getname(&(tagstack[i].fmark));
			if (name == NULL)		/* file name not available */
				continue;

			sprintf(IObuff, "%c%2d %-15s %4ld  %s\n",
				i == tagstackidx ? '>' : ' ',
				i + 1,
				tagstack[i].tagname,
				tagstack[i].fmark.lnum,
				name);
			outstrn(IObuff);
		}
		flushbuf();
	}
	if (tagstackidx == tagstacklen)		/* idx at top of stack */
		outstrn(">\n");
#ifdef AMIGA
	settmode(1);
#endif
	wait_return(TRUE);
}

/*
 * findtag(tag) - goto tag
 *   return 0 for failure, 1 for success
 */
	static int
findtag(tag)
	char		   *tag;
{
	FILE	   *tp, *fopen();
	char		lbuf[LSIZE];
	char		pbuf[LSIZE];			/* search pattern buffer */
	char	   *fname, *str;
	int			cmplen;
	char		*m = "No tags file";
	register char	*p;
	char		*np;					/* pointer into file name string */
	char		sbuf[CMDBUFFSIZE + 1];	/* tag file name */
	int			i;
	int			save_secure;

	if (tag == NULL)		/* out of memory condition */
		return 0;

	if ((cmplen = p_tl) == 0)
		cmplen = 999;

	/* get stack of tag file names from tags option */
	for (np = p_tags; *np; )
	{
		for (i = 0; i < CMDBUFFSIZE && *np; ++i)	/* copy next file name into lbuf */
		{
			if (*np == ' ')
			{
				++np;
				break;
			}
			sbuf[i] = *np++;
		}
		sbuf[i] = 0;
		if ((tp = fopen(sbuf, "r")) == NULL)
			continue;
		while (fgets(lbuf, LSIZE, tp) != NULL)
		{
			m = "Format error in tags file %s";	/* default error message */

		/* find start of file name, after first white space */
			fname = lbuf;
			skiptospace(&fname);	/* skip tag */
			if (*fname == NUL)
				goto erret;
			*fname++ = '\0';

			if (strncmp(lbuf, tag, (size_t)cmplen) == 0)	/* Tag found */
			{
				fclose(tp);
				skipspace(&fname);

			/* find start of search command, after second white space */
				str = fname;
				skiptospace(&str);
				if (*str == NUL)
					goto erret;
				*str++ = '\0';
				skipspace(&str);

				/*
				 * If the command is a string like "/^function fname"
				 * scan through the search string. If we see a magic
				 * char, we have to quote it. This lets us use "real"
				 * implementations of ctags.
				 */
				if (*str == '/' || *str == '?')
				{
					p = pbuf;
					*p++ = *str++;			/* copy the '/' or '?' */
					if (*str == '^')
						*p++ = *str++;			/* copy the '^' */

					while (*str)
					{
						switch (*str)
						{
						case '\\':	if (str[1] == '(')	/* remove '\' before '(' */
										++str;
									else
										*p++ = *str++;
									break;

						case '\r':
						case '\n':	*p++ = pbuf[0];	/* copy '/' or '?' */
									*str = 'n';		/* no setpcmark() for search */
									str[1] = NUL;	/* delete NL after CR */
									break;

									/*
									 * if string ends in search character: skip it
									 * else escape it with '\'
									 */
						case '/':
						case '?':	if (*str != pbuf[0])	/* not the search char */
										break;
															/* last char */
									if (str[1] == '\n' || str[1] == '\r')
									{
										++str;
										continue;
									}
						case '[':
									if (!p_magic)
										break;
						case '^':
						case '*':
						case '.':	*p++ = '\\';
									break;
						}
						*p++ = *str++;
					}
				}
				else		/* not a search command, just copy it */
					for (p = pbuf; *str && *str != '\n'; )
						*p++ = *str++;
				*p = NUL;

				RedrawingDisabled = TRUE;
				/* expand filename (for environment variables) */
				if ((p = ExpandOne((u_char *)fname, 1, -1)) != NULL)
					fname = p;
				i = getfile(fname, NULL, TRUE);
				if (p)
					free(p);
				if (i <= 0)
				{
					set_want_col = TRUE;

					RedrawingDisabled = FALSE;
					save_secure = secure;
					secure = 1;
					docmdline((u_char *)pbuf);
					if (secure == 2)		/* done something that is not allowed */
						wait_return(TRUE);
					secure = save_secure;

						/* print the file message after redraw */
					if (p_im && i == -1)
						stuffReadbuff("\033\007i");	/* ESC CTRL-G i */
					else
						stuffcharReadbuff('\007');		/* CTRL-G */
					return 1;
				}
				RedrawingDisabled = FALSE;
				return 0;
			}
		}
		m = NULL;

erret:
		fclose(tp);
		if (m)
		{
			emsg2(m, sbuf);
			sleep(1);
		}
	}
	if (m == NULL)
		emsg("tag not found");
	else if (*np == NUL)
		emsg(m);
	return 0;
}
