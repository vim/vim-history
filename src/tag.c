/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * Code to handle tags and the tag stack
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

#define TAGSTACKSIZE 20

/*
 * the taggy struct is used to store the information about a :tag command:
 *	the tag name and the cursor position BEFORE the :tag command
 */
struct taggy
{
	char_u			*tagname;			/* tag name */
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

static int findtag __ARGS((char_u *));

static char_u *bottommsg = (char_u *)"at bottom of tag stack";
static char_u *topmsg = (char_u *)"at top of tag stack";

/*
 * Jump to tag; handling of tag stack
 *
 * *tag != NUL (:tag): jump to new tag, add to tag stack
 * type == 1 (:pop) || type == 2 (CTRL-T): jump to old position
 * type == 0 (:tag): jump to old tag
 */
	void
dotag(tag, type, count)
	char_u	*tag;
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
		tagstack[tagstackidx].fmark.mark = curwin->w_cursor;
		tagstack[tagstackidx].fmark.fnum = curbuf->b_fnum;
	}
	else if (tagstacklen == 0)					/* empty stack */
	{
		EMSG("tag stack empty");
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
		if (tagstack[tagstackidx].fmark.fnum != curbuf->b_fnum)	/* jump to other file */
		{
			if (filelist_getfile(tagstack[tagstackidx].fmark.fnum, tagstack[tagstackidx].fmark.mark.lnum, TRUE) == FAIL)
			{
				/* emsg(e_notopen); */
				return;
			}
		}
		else
			curwin->w_cursor.lnum = tagstack[tagstackidx].fmark.mark.lnum;
		curwin->w_cursor.col = tagstack[tagstackidx].fmark.mark.col;
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
}

/*
 * Print the tag stack
 */
	void
dotags()
{
	int			i;
	char_u		*name;

	gotocmdline(TRUE, NUL);
	mch_start_listing();	/* may set cooked mode, so output can be halted */
	msg_outstr((char_u *)"\n  # TO tag      FROM line in file\n");
	for (i = 0; i < tagstacklen; ++i)
	{
		if (tagstack[i].tagname != NULL)
		{
			name = fm_getname(&(tagstack[i].fmark));
			if (name == NULL)		/* file name not available */
				continue;

			sprintf((char *)IObuff, "%c%2d %-15s %4ld  %s\n",
				i == tagstackidx ? '>' : ' ',
				i + 1,
				tagstack[i].tagname,
				tagstack[i].fmark.mark.lnum,
				name);
			msg_outstr(IObuff);
		}
		flushbuf();					/* show one line at a time */
	}
	if (tagstackidx == tagstacklen)		/* idx at top of stack */
		msg_outstr((char_u *)">\n");
	mch_stop_listing();
	wait_return(FALSE);
}

/*
 * findtag(tag) - goto tag
 *   return 0 for failure, 1 for success
 */
	static int
findtag(tag)
	char_u		   *tag;
{
	FILE	   *tp;
	char_u		lbuf[LSIZE];
	char_u		pbuf[LSIZE];			/* search pattern buffer */
	char_u	   *fname, *str;
	int			cmplen;
	char_u		*m = (char_u *)"No tags file";
	char_u		*marg = NULL;
	register char_u	*p;
	char_u		*np;					/* pointer into file name string */
	char_u		sbuf[CMDBUFFSIZE + 1];	/* tag file name */
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
		if ((tp = fopen((char *)sbuf, "r")) == NULL)
			continue;
		reg_ic = p_ic;										/* for cstrncmp() */
		while (fgets((char *)lbuf, LSIZE, tp) != NULL)
		{
			m = (char_u *)"Format error in tags file %s";	/* default error message */
			marg = sbuf;

		/* find start of file name, after first white space */
			fname = lbuf;
			skiptospace(&fname);	/* skip tag */
			if (*fname == NUL)
				goto erret;
			*fname++ = '\0';

			if (cstrncmp(lbuf, tag, cmplen) == 0)	/* Tag found */
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
						case '\n':	*str = pbuf[0];	/* copy '/' or '?' */
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

				/* expand filename (for environment variables) */
				if ((p = ExpandOne((char_u *)fname, 1, -1)) != NULL)
					fname = p;
				/*
				 * check if file for tag exists before abandoning current file
				 */
				if (getperm(fname) < 0)
				{
					m = (char_u *)"File \"%s\" does not exist";
					marg = fname;
					goto erret;
				}

				RedrawingDisabled = TRUE;
				/*
				 * if it was a CTRL-W CTRL-] command split window now
				 */
				if (postponed_split)
					win_split(0L, FALSE);
				i = getfile(fname, NULL, TRUE);
				if (p)
					free(p);
				if (i <= 0)
				{
					curwin->w_set_curswant = TRUE;
					postponed_split = FALSE;

					RedrawingDisabled = FALSE;
					save_secure = secure;
					secure = 1;
					tag_busy = TRUE;			/* don't set marks for this search */
					curwin->w_cursor.lnum = 1;	/* start search in line 1 */
					docmdline(pbuf);
					tag_busy = FALSE;
					if (secure == 2)			/* done something that is not allowed */
						wait_return(TRUE);
					secure = save_secure;

						/* print the file message after redraw */
					if (p_im && i == -1)
						stuffReadbuff((char_u *)"\033\007i");	/* ESC CTRL-G i */
					else
						stuffcharReadbuff('\007');		/* CTRL-G */
					return 1;
				}
				RedrawingDisabled = FALSE;
				if (postponed_split)			/* close the window */
				{
					close_window(FALSE);
					postponed_split = FALSE;
				}
				return 0;
			}
		}
		m = NULL;

erret:
		fclose(tp);
		if (m)
			emsg2(m, marg);
	}
	if (m == NULL)
		EMSG("tag not found");
	else if (marg == NULL)
		emsg(m);
	return 0;
}
