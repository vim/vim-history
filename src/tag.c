/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Code to handle tags and the tag stack
 */

#if defined MSDOS || defined WIN32
# include <io.h>	/* for lseek(), must be before vim.h */
#endif

#include "vim.h"

#ifdef HAVE_FCNTL_H
# include <fcntl.h>	/* for lseek() */
#endif

struct tag_pointers
{
    /* filled in by parse_tag_line(): */
    char_u	*tagname;	/* start of tag name (skip "file:") */
    char_u	*tagname_end;	/* char after tag name */
    char_u	*fname;		/* first char of file name */
    char_u	*fname_end;	/* char after file name */
    char_u	*command;	/* first char of command */
    /* filled in by parse_match(): */
    char_u	*command_end;	/* first char of command */
    char_u	*tag_fname;	/* file name of the tags file */
#ifdef EMACS_TAGS
    int		is_etag;	/* TRUE for emacs tag */
#endif
    char_u	*tagkind;	/* "kind:" value */
    char_u	*tagkind_end;	/* end of tagkind */
};

/*
 * The matching tags are first stored in ga_match[].  In which one depends on
 * the priority of the match.
 * At the end, the matches from ga_match[] are concatenated, to make a list
 * sorted on priority.
 */
#define MT_ST_CUR	0		/* static match in current file */
#define MT_GL_CUR	1		/* global match in current file */
#define MT_GL_OTH	2		/* global match in other file */
#define MT_ST_OTH	3		/* static match in other file */
#define MT_IC_ST_CUR	4		/* icase static match in current file */
#define MT_IC_GL_CUR	5		/* icase global match in current file */
#define MT_IC_GL_OTH	6		/* icase global match in other file */
#define MT_IC_ST_OTH	7		/* icase static match in other file */
#define MT_COUNT	8
#define MT_IC_OFF	4		/* add for icase match */

static char	*mt_names[MT_COUNT] =
		{"FSC", "F C", "F  ", "FS ", " SC", "  C", "   ", " S "};

#define NOTAGFILE	99		/* return value for jumpto_tag */
static char_u	*nofile_fname = NULL;	/* fname for NOTAGFILE error */

static int get_tagfname __ARGS((int first, char_u *buf));

static int jumpto_tag __ARGS((char_u *lbuf, int forceit));
#ifdef EMACS_TAGS
static int parse_tag_line __ARGS((char_u *lbuf, int is_etag, struct tag_pointers *tagp));
#else
static int parse_tag_line __ARGS((char_u *lbuf, struct tag_pointers *tagp));
#endif
static int test_for_static __ARGS((struct tag_pointers *));
static int parse_match __ARGS((char_u *lbuf, struct tag_pointers *tagp));
static char_u *expand_rel_name __ARGS((char_u *fname, char_u *tag_fname));
#ifdef EMACS_TAGS
static int test_for_current __ARGS((int, char_u *, char_u *, char_u *));
#else
static int test_for_current __ARGS((char_u *, char_u *, char_u *));
#endif
static int find_extra __ARGS((char_u **pp));

static char_u *bottommsg = (char_u *)"at bottom of tag stack";
static char_u *topmsg = (char_u *)"at top of tag stack";

/*
 * Jump to tag; handling of tag commands and tag stack
 *
 * *tag != NUL: ":tag {tag}", jump to new tag, add to tag stack
 *
 * type == DT_TAG:	":tag [tag]", jump to newer position or same tag again
 * type == DT_HELP:	like DT_TAG, but don't use wildcards.
 * type == DT_POP:	":pop" or CTRL-T, jump to old position
 * type == DT_NEXT:	jump to next match of same tag
 * type == DT_PREV:	jump to previous match of same tag
 * type == DT_FIRST:	jump to first match of same tag
 * type == DT_LAST:	jump to first match of same tag
 * type == DT_SELECT:	":tselect [tag]", select tag from a list of all matches
 */
    void
do_tag(tag, type, count, forceit)
    char_u	*tag;		/* tag (pattern) to jump to */
    int		type;
    int		count;
    int		forceit;	/* :ta with ! */
{
    struct taggy	*tagstack = curwin->w_tagstack;
    int			tagstackidx = curwin->w_tagstackidx;
    int			tagstacklen = curwin->w_tagstacklen;
    int			cur_match = 0;
    int			oldtagstackidx = tagstackidx;
    int			prev_num_matches;
    int			new_tag = FALSE;
    int			i;
    int			ic;
    char_u		*p;
    char_u		*name;
    int			no_wild = FALSE;
    int			error_cur_match = 0;
    char_u		*command_end;

    /* remember the matches for the last used tag */
    static int		num_matches = 0;
    static int		max_num_matches = 0;  /* limit used for match search */
    static char_u	**matches = NULL;
    static int		haswild = FALSE;
    static char_u	*matchname = NULL;

    if (type == DT_HELP)
    {
	type = DT_TAG;
	no_wild = TRUE;
    }

    prev_num_matches = num_matches;
    nofile_fname = NULL;

    /* new pattern, add to the tag stack */
    if ((type == DT_TAG || type == DT_SELECT) && *tag)
    {
	/*
	 * If the last used entry is not at the top, delete all tag stack
	 * entries above it.
	 */
	while (tagstackidx < tagstacklen)
	    vim_free(tagstack[--tagstacklen].tagname);

	/* if the tagstack is full: remove oldest entry */
	if (++tagstacklen > TAGSTACKSIZE)
	{
	    tagstacklen = TAGSTACKSIZE;
	    vim_free(tagstack[0].tagname);
	    for (i = 1; i < tagstacklen; ++i)
		tagstack[i - 1] = tagstack[i];
	    --tagstackidx;
	}

	/*
	 * put the tag name in the tag stack
	 * the position is added below
	 */
	if ((tagstack[tagstackidx].tagname = vim_strsave(tag)) == NULL)
	{
	    --tagstacklen;
	    goto end_do_tag;
	}
	new_tag = TRUE;
    }
    else
    {
	if (tagstacklen == 0)			/* empty stack */
	{
	    EMSG(e_tagstack);
	    goto end_do_tag;
	}

	if (type == DT_POP)		/* go to older position */
	{
	    if ((tagstackidx -= count) < 0)
	    {
		emsg(bottommsg);
		if (tagstackidx + count == 0)
		{
		    /* We did [num]^T from the bottom of the stack */
		    tagstackidx = 0;
		    goto end_do_tag;
		}
		/* We weren't at the bottom of the stack, so jump all the
		 * way to the bottom now.
		 */
		tagstackidx = 0;
	    }
	    else if (tagstackidx >= tagstacklen)    /* count == 0? */
	    {
		emsg(topmsg);
		goto end_do_tag;
	    }
	    if (tagstack[tagstackidx].fmark.fnum != curbuf->b_fnum)
	    {
		/*
		 * Jump to other file. If this fails (e.g. because the
		 * file was changed) keep original position in tag stack.
		 */
		if (buflist_getfile(tagstack[tagstackidx].fmark.fnum,
			tagstack[tagstackidx].fmark.mark.lnum,
			GETF_SETMARK, forceit) == FAIL)
		{
		    tagstackidx = oldtagstackidx;  /* back to old posn */
		    goto end_do_tag;
		}
	    }
	    else
		curwin->w_cursor.lnum = tagstack[tagstackidx].fmark.mark.lnum;
	    curwin->w_cursor.col = tagstack[tagstackidx].fmark.mark.col;
	    curwin->w_set_curswant = TRUE;
	    /* remove the old list of matches */
	    FreeWild(num_matches, matches);
	    num_matches = 0;
	    goto end_do_tag;
	}

	if (type == DT_TAG)		/* go to newer pattern */
	{
	    if ((tagstackidx += count - 1) >= tagstacklen)
	    {
		/*
		 * beyond the last one, just give an error message and go to
		 * the last one
		 */
		tagstackidx = tagstacklen - 1;
		emsg(topmsg);
	    }
	    else if (tagstackidx < 0)	    /* must have been count == 0 */
	    {
		emsg(bottommsg);
		tagstackidx = 0;
		goto end_do_tag;
	    }
	    cur_match = tagstack[tagstackidx].cur_match;
	    new_tag = TRUE;
	}
	else				/* go to other matching tag */
	{
	    cur_match = tagstack[tagstackidx].cur_match;
	    switch (type)
	    {
		case DT_FIRST: cur_match = count - 1; break;
		case DT_SELECT:
		case DT_LAST:  cur_match = MAXCOL - 1; break;
		case DT_NEXT:  cur_match += count; break;
		case DT_PREV:  cur_match -= count; break;
	    }
	    if (cur_match >= MAXCOL)
		cur_match = MAXCOL - 1;
	    else if (cur_match < 0)
		cur_match = 0;
	    if (--tagstackidx < 0)
		tagstackidx = 0;
	}
    }

    /*
     * For ":tag [arg]" or ":tselect" remember position before the jump.
     */
    if (type == DT_TAG || type == DT_SELECT)
    {
	tagstack[tagstackidx].fmark.mark = curwin->w_cursor;
	tagstack[tagstackidx].fmark.fnum = curbuf->b_fnum;
    }

    /* curwin will change in the call to jumpto_tag() if ":stag" was used */
    curwin->w_tagstackidx = tagstackidx;
    curwin->w_tagstacklen = tagstacklen;
    if (type != DT_SELECT)
	curwin->w_tagstack[tagstackidx].cur_match = cur_match;

    /*
     * Repeat searching for tags, when a file has not been found
     */
    for (;;)
    {
	/*
	 * When desired match not found yet, try to find it (and others).
	 */
	name = tagstack[tagstackidx].tagname;
	if (new_tag
		|| (cur_match >= num_matches && max_num_matches != MAXCOL)
		|| matchname == NULL
		|| STRCMP(matchname, name) != 0)
	{
	    FreeWild(num_matches, matches);
	    num_matches = 0;
	    matches = NULL;
	    vim_free(matchname);
	    matchname = vim_strsave(name);

	    if (type == DT_SELECT)
		cur_match = MAXCOL - 1;
	    max_num_matches = cur_match + 1;

	    haswild = 0;
	    if (!no_wild)
		for (p = name; *p; ++p)
		    if (vim_iswildc(*p))
		    {
			haswild = TAG_WILD;
			break;
		    }

	    if (find_tags(name, &num_matches, &matches,
					      haswild, max_num_matches) == OK
		    && num_matches < max_num_matches)
		/* If less than max_num_matches found: all matches found. */
		max_num_matches = MAXCOL;
	}

	/*
	 * Jump to the desired match.
	 */
	if (num_matches <= 0)
	    EMSG("tag not found");
	else
	{
	    if (type == DT_SELECT)
	    {
		struct tag_pointers tagp;

		/*
		 * List all the matching tags.
		 */
		MSG_PUTS_ATTR(" nr pri kind tag\t\tfile\n",
						       highlight_attr[HLF_T]);
		for (i = 0; i < num_matches; ++i)
		{
		    parse_match(matches[i], &tagp);
		    if (!new_tag && i == tagstack[tagstackidx].cur_match)
			*IObuff = '>';
		    else
			*IObuff = ' ';
		    sprintf((char *)IObuff + 1, "%2d %s ", i + 1,
						     mt_names[matches[i][0]]);
		    msg_puts(IObuff);
		    if (tagp.tagkind != NULL)
			msg_outtrans_len(tagp.tagkind,
				      (int)(tagp.tagkind_end - tagp.tagkind));
		    msg_advance(13);
		    msg_outtrans_len_attr(tagp.tagname,
				       (int)(tagp.tagname_end - tagp.tagname),
						       highlight_attr[HLF_D]);
		    msg_putchar(' ');
		    msg_advance(32);
		    msg_outtrans_len_attr(tagp.fname,
					  (int)(tagp.fname_end - tagp.fname),
						       highlight_attr[HLF_D]);
		    msg_putchar('\n');
		    msg_advance(15);

		    /* print any extra fields */
		    command_end = tagp.command_end;
		    if (command_end != NULL)
		    {
			p = command_end + 3;
			while (*p)
			{
			    /* skip "file:" without a value (static tag) */
			    if (STRNCMP(p, "file:", 5) == 0
							 && vim_isspace(p[5]))
			    {
				p += 6;
				continue;
			    }
			    /* skip "kind:<kind>" and "<kind>" */
			    if (p == tagp.tagkind
				    || (p + 5 == tagp.tagkind
					    && STRNCMP(p, "kind:", 5) == 0))
			    {
				p = tagp.tagkind_end;
				if (*p == TAB)
				    ++p;
				continue;
			    }

			    if (*p == '\r' || *p == '\n')
				break;

			    while (*p && *p != '\r' && *p != '\n' && *p != TAB)
			    {
				if (msg_col + charsize(*p) >= Columns)
				{
				    msg_putchar('\n');
				    msg_advance(15);
				}
				msg_puts_attr(transchar(*p),
						      highlight_attr[HLF_CM]);
				++p;
			    }
			}
			if (msg_col > 15)
			{
			    msg_putchar('\n');
			    msg_advance(15);
			}
		    }
		    else
		    {
			for (p = tagp.command;
					  *p && *p != '\r' && *p != '\n'; ++p)
			    ;
			command_end = p;
		    }

		    /*
		     * Put the info (in several lines) at column 15.
		     * Don't display "/^" and "?^".
		     */
		    p = tagp.command;
		    if (*p == '/' || *p == '?')
		    {
			++p;
			if (*p == '^')
			    ++p;
		    }
		    while (p != command_end)
		    {
			if (msg_col + (*p == TAB ? 1 : charsize(*p)) > Columns)
			    msg_putchar('\n');
			msg_advance(15);

			/* skip backslash used for escaping command char */
			if (*p == '\\' && *(p + 1) == *tagp.command)
			    ++p;

			if (*p == TAB)
			    msg_putchar(' ');
			else
			    msg_puts(transchar(*p));
			++p;

			/* don't display the "$/;\"" and "$?;\"" */
			if (p == command_end - 2 && *p == '$'
						 && *(p + 1) == *tagp.command)
			    break;
			if (p == command_end - 1 && *p == *tagp.command)
			    break;
		    }
		    if (msg_col)
			msg_putchar('\n');
		    ui_breakcheck();
		    if (got_int)
		    {
			got_int = FALSE;	/* only stop the listing */
			break;
		    }
		}

		/*
		 * Ask to select a tag from the list.
		 */
		MSG_PUTS("Enter nr of choice (<CR> to abort): ");
		i = get_number(TRUE);
		if (KeyTyped)		/* don't call wait_return() now */
		{
		    msg_putchar('\n');
		    dont_wait_return = TRUE;
		    need_wait_return = FALSE;
		    msg_didany = FALSE;
		    cmdline_row = msg_row;
		    msg_scrolled = 0;
		    redraw_all_later(NOT_VALID);
		}
		if (i <= 0 || i > num_matches || got_int)
		{
		    /* don't change the current match setting now */
		    if (!new_tag)
			cur_match = tagstack[tagstackidx].cur_match;
		    break;
		}
		cur_match = i - 1;
	    }

	    if (cur_match >= num_matches)
		cur_match = num_matches - 1;
	    curwin->w_tagstack[tagstackidx].cur_match = cur_match;
	    ++tagstackidx;

	    /*
	     * Only when going to try the next match, report that the previous
	     * file didn't exist.  Otherwise an EMSG() is given below.
	     */
	    if (nofile_fname != NULL && error_cur_match != cur_match)
		smsg((char_u *)"File \"%s\" does not exist", nofile_fname);


	    ic = (matches[cur_match][0] >= MT_IC_OFF);
	    if (type != DT_SELECT && (num_matches > 1 || ic))
	    {
		/* Give an indication of the number of matching tags */
		sprintf((char *)msg_buf, "tag %d of %c%d",
				cur_match + 1,
				max_num_matches != MAXCOL ? '>' : ' ',
				num_matches);
		if (ic)
		    STRCAT(msg_buf, "  Using tag with different case!");
		if ((num_matches > prev_num_matches || new_tag)
							   && num_matches > 1)
		{
		    if (ic)
			msg_attr(msg_buf, highlight_attr[HLF_W]);
		    else
			msg(msg_buf);
		    msg_scroll = TRUE;	/* don't overwrite this message */
		}
		else
		    give_warning(msg_buf, ic);
		if (ic && !msg_scrolled)
		{
		    out_flush();
		    ui_delay(1000L, TRUE);
		}
	    }

	    if (jumpto_tag(matches[cur_match], forceit) == NOTAGFILE)
	    {
		/* File not found: try again with another matching tag */
		if ((type == DT_PREV && cur_match > 0)
			|| ((type == DT_TAG || type == DT_NEXT
							  || type == DT_FIRST)
			    && (max_num_matches != MAXCOL
					     || cur_match < num_matches - 1)))
		{
		    error_cur_match = cur_match;
		    --tagstackidx;
		    if (type == DT_PREV)
			--cur_match;
		    else
		    {
			type = DT_NEXT;
			++cur_match;
		    }
		    continue;
		}
		EMSG2("File \"%s\" does not exist", nofile_fname);
	    }
	}
	break;
    }

end_do_tag:
    curwin->w_tagstackidx = tagstackidx;
    curwin->w_tagstacklen = tagstacklen;
}

/*
 * Print the tag stack
 */
    void
do_tags()
{
    int		    i;
    char_u	    *name;
    struct taggy    *tagstack = curwin->w_tagstack;
    int		    tagstackidx = curwin->w_tagstackidx;
    int		    tagstacklen = curwin->w_tagstacklen;

    /* Highlight title */
    MSG_PUTS_TITLE("\n  # TO tag         FROM line  in file");
    for (i = 0; i < tagstacklen; ++i)
    {
	if (tagstack[i].tagname != NULL)
	{
	    name = fm_getname(&(tagstack[i].fmark));
	    if (name == NULL)	    /* file name not available */
		continue;

	    msg_putchar('\n');
	    sprintf((char *)IObuff, "%c%2d %2d %-15s %5ld  %s",
		i == tagstackidx ? '>' : ' ',
		i + 1,
		tagstack[i].cur_match + 1,
		tagstack[i].tagname,
		tagstack[i].fmark.mark.lnum,
		name);
	    msg_outtrans(IObuff);
	    vim_free(name);
	}
	out_flush();		    /* show one line at a time */
    }
    if (tagstackidx == tagstacklen)	/* idx at top of stack */
	MSG_PUTS("\n>");
}

/*
 * find_tags() - search for tags in tags files
 *
 * Return FAIL if search completely failed (*num_matches will be 0, *matchesp
 * will be NULL), OK otherwise.
 *
 * There is a priority in which type of tag is recognized.
 *
 *  6.	A static or global tag with a full matching tag for the current file.
 *  5.	A global tag with a full matching tag for another file.
 *  4.	A static tag with a full matching tag for another file.
 *  3.	A static or global tag with an ignore-case matching tag for the
 *	current file.
 *  2.	A global tag with an ignore-case matching tag for another file.
 *  1.	A static tag with an ignore-case matching tag for another file.
 *
 * Tags in an emacs-style tags file are always global.
 *
 * flags:
 * TAG_HELP	only search for help tags
 * TAG_NAMES	only return name of tag
 * TAG_WILD	"pat" has wildcards
 */

    int
find_tags(pat, num_matches, matchesp, flags, mincount)
    char_u	*pat;			/* pattern to search for */
    int		*num_matches;		/* return: number of matches found */
    char_u	***matchesp;		/* return: array of matches found */
    int		flags;
    int		mincount;		/*  MAXCOL: find all matches
					     other: minimal number of matches */
{
    FILE       *fp;
    char_u     *lbuf;			/* line buffer */
    char_u     *tag_fname;		/* name of tag file */
    int		first_file;		/* trying first tag file */
    struct tag_pointers tagp;
    int		did_open = FALSE;	/* did open a tag file */
    int		stop_searching = FALSE;	/* stop when match found or error */
    int		retval = FAIL;		/* return value */
    int		is_static;		/* current tag line is static */
    int		is_current;		/* file name matches */
    int		eof;			/* found end-of-file */
    char_u	*p;
    char_u	*s;
    int		i;
    vim_regexp	*prog = NULL;		/* regexp program or NULL */
#ifdef BINARY_TAGS
    struct tag_search_info
    {
	long	low_offset;	/* Low file offset of search range */
	long	high_offset;	/* High file offset of search range */
	long	curr_offset;	/* Current file offset in search range */
	long	match_offset;	/* Where the binary search found a tag */
    }		search_info;	/* Binary search file offsets */
    off_t	filesize;
    int		tagcmp;
    long	offset;
#endif
    enum
    {
	TS_START,		/* at start of file */
	TS_LINEAR,		/* linear searching forward, till EOF */
#ifdef BINARY_TAGS
	TS_BINARY,		/* binary searching */
	TS_SKIP_BACK,		/* skipping backwards */
	TS_STEP_FORWARD		/* stepping forwards */
#endif
    }	state;			/* Current search state */

    int		cmplen;
    int		match;		/* matches */
    int		match_no_ic = 0;/* matches with reg_ic == FALSE */
    int		matchoff = 0;

#ifdef EMACS_TAGS
    /*
     * Stack for included emacs-tags file.
     * It has a fixed size, to truncate cyclic includes. jw
     */
# define INCSTACK_SIZE 42
    struct
    {
	FILE	*fp;
	char_u	*etag_fname;
    } incstack[INCSTACK_SIZE];

    int		incstack_idx = 0;	/* index in incstack */
    char_u     *ebuf;			/* aditional buffer for etag fname */
    int		is_etag;		/* current file is emaces style */
#endif

    struct growarray ga_match[MT_COUNT];
    int		match_count = 0;		/* number of matches found */
    char_u	**matches;
    int		mtt;
    int		len;
    int		help_save;

    char_u	*pathead;			/* start of pattern head */
    int		patheadlen;			/* lenght of pattern head */
#ifdef BINARY_TAGS
    int		findall = (mincount == MAXCOL); /* find all matching tags */
#endif
    int		haswild = (flags & TAG_WILD);	/* regexp used */
    int		help_only = (flags & TAG_HELP);
    int		name_only = (flags & TAG_NAMES);

    help_save = curbuf->b_help;

/*
 * Allocate memory for the buffers that are used
 */
    if (haswild)
	prog = vim_regcomp(pat, p_magic);
    lbuf = alloc(LSIZE);
    tag_fname = alloc(LSIZE + 1);
#ifdef EMACS_TAGS
    ebuf = alloc(LSIZE);
#endif
    for (mtt = 0; mtt < MT_COUNT; ++mtt)
    {
	ga_init(&ga_match[mtt]);
	ga_match[mtt].ga_itemsize = sizeof(char_u *);
	ga_match[mtt].ga_growsize = 100;
    }

    /* check for out of memory situation */
    if ((haswild && prog == NULL) || lbuf == NULL || tag_fname == NULL
#ifdef EMACS_TAGS
							 || ebuf == NULL
#endif
									)
	goto findtag_end;

/*
 * Initialize a few variables
 */
    if (help_only)			    /* want tags from help file */
	curbuf->b_help = TRUE;		    /* will be restored later */

    pathead = pat;
    if (!haswild)
	patheadlen = STRLEN(pat);
    else
    {
	/* When the pattern starts with '^' or "\\<", binary searching can be
	 * used (much faster). */
	if (pat[0] == '^')
	    pathead = pat + 1;
	else if (pat[0] == '\\' && pat[1] == '<')
	    pathead = pat + 2;
	if (pathead == pat)
	    patheadlen = 0;
	else
	    for (patheadlen = 0; pathead[patheadlen] != NUL; ++patheadlen)
		if (vim_strchr((char_u *)(p_magic ? ".[~*\\$" : "\\$"),
						 pathead[patheadlen]) != NULL)
		    break;
    }
    if (p_tl != 0 && patheadlen > p_tl)	    /* adjust for 'taglength' */
	patheadlen = p_tl;

/*
 * When finding a specified number of matches, first try with matching case,
 * so binary search can be used, and try ignore-case matches in a second loop.
 * When finding all matches, 'tagbsearch' is off, or there is no fixed string
 * to look for, ignore case right away to avoid going though the tags files
 * twice.
 */
#ifdef BINARY_TAGS
    reg_ic = (findall || patheadlen == 0 || !p_tbs);
    for (;;)
    {
#else
    reg_ic = TRUE;
#endif

      /*
       * Try tag file names from tags option one by one.
       */
      for (first_file = TRUE; get_tagfname(first_file, tag_fname) == OK;
							    first_file = FALSE)
      {
	/*
	 * A file that doesn't exist is silently ignored.  Only when not a
	 * single file is found, an error message is given (further on).
	 */
	if ((fp = fopen((char *)tag_fname, "r")) == NULL)
	    continue;

	did_open = TRUE;    /* remember that we found at least one file */

	state = TS_START;   /* we're at the start of the file */
#ifdef EMACS_TAGS
	is_etag = 0;	    /* default is: not emacs style */
#endif
	/*
	 * Read and parse the lines in the file one by one
	 */
	while (!got_int)
	{
	    line_breakcheck();	    /* check for CTRL-C typed */

#ifdef BINARY_TAGS
	    /*
	     * For binary search: compute the next offset to use.
	     */
	    if (state == TS_BINARY)
	    {
		offset = search_info.low_offset + ((search_info.high_offset
					       - search_info.low_offset) / 2);
		if (offset == search_info.curr_offset)
		    break;	/* End the binary search without a match. */
		else
		    search_info.curr_offset = offset;
	    }

	    /*
	     * Skipping back (after a match during binary search).
	     */
	    else if (state == TS_SKIP_BACK)
	    {
		search_info.curr_offset -= LSIZE * 2;
		if (search_info.curr_offset < 0)
		{
		    search_info.curr_offset = 0;
		    rewind(fp);
		    state = TS_STEP_FORWARD;
		}
	    }

	    /*
	     * When jumping around in the file, first read a line to find the
	     * start of the next line.
	     */
	    if (state == TS_BINARY || state == TS_SKIP_BACK)
	    {
		/* Adjust the search file offset to the correct position */
		fseek(fp, search_info.curr_offset, SEEK_SET);
		eof = vim_fgets(lbuf, LSIZE, fp);
		if (!eof && search_info.curr_offset)
		{
		    search_info.curr_offset = ftell(fp);
		    if (search_info.curr_offset == search_info.high_offset)
		    {
			/* oops, gone a bit too far; try from low offset */
			fseek(fp, search_info.low_offset, SEEK_SET);
			if (search_info.low_offset)
			    (void)vim_fgets(lbuf, LSIZE, fp);
			search_info.curr_offset = ftell(fp);
		    }
		    eof = vim_fgets(lbuf, LSIZE, fp);
		}
		/* skip empty and blank lines */
		while (!eof && vim_isblankline(lbuf))
		{
		    search_info.curr_offset = ftell(fp);
		    eof = vim_fgets(lbuf, LSIZE, fp);
		}
		if (eof)
		{
		    /* Hit end of file.  Skip backwards. */
		    state = TS_SKIP_BACK;
		    search_info.match_offset = ftell(fp);
		    continue;
		}
	    }

	    /*
	     * Not jumping around in the file: Read the next line.
	     */
	    else
#endif
	    {
		/* skip empty and blank lines */
		do
		{
		    eof = vim_fgets(lbuf, LSIZE, fp);
		} while (!eof && vim_isblankline(lbuf));

		if (eof)
#ifdef EMACS_TAGS
		    if (incstack_idx)	/* this was an included file */
		    {
			--incstack_idx;
			fclose(fp);	/* end of this file ... */
			fp = incstack[incstack_idx].fp;
			STRCPY(tag_fname, incstack[incstack_idx].etag_fname);
			vim_free(incstack[incstack_idx].etag_fname);
			is_etag = 1;	/* (only etags can include) */
			continue;	/* ... continue with parent file */
		    }
		    else
#endif
			break;			    /* end of file */
	    }

#ifdef EMACS_TAGS
	    /*
	     * Emacs tags line with CTRL-L: New file name on next line.
	     * The file name is followed by a ','.
	     */
	    if (*lbuf == Ctrl('L'))	/* remember etag file name in ebuf */
	    {
		is_etag = 1;		/* in case at the start */
		state = TS_LINEAR;
		if (!vim_fgets(ebuf, LSIZE, fp))
		{
		    for (p = ebuf; *p && *p != ','; p++)
			;
		    *p = NUL;

		    /*
		     * atoi(p+1) is the number of bytes before the next ^L
		     * unless it is an include statement.
		     */
		    if (STRNCMP(p + 1, "include", 7) == 0
					      && incstack_idx < INCSTACK_SIZE)
		    {
			if ((incstack[incstack_idx].etag_fname =
					      vim_strsave(tag_fname)) != NULL)
			{
			    incstack[incstack_idx].fp = fp;
			    if ((fp = fopen((char *)ebuf, "r")) == NULL)
			    {
				fp = incstack[incstack_idx].fp;
				vim_free(incstack[incstack_idx].etag_fname);
			    }
			    else
			    {
				STRCPY(tag_fname, ebuf);
				++incstack_idx;
			    }
			    is_etag = 0;	/* we can include anything */
			}
		    }
		}
		continue;
	    }
#endif

	    /*
	     * When still at the start of the file, check for Emacs tags file
	     * format, and for "not sorted" flag.
	     */
	    if (state == TS_START)
	    {
#ifdef BINARY_TAGS
		/*
		 * When there is no tag head, or ignoring case, need to do a
		 * linear search.
		 * When no "!_TAG_" is found, default to binary search.  If
		 * the tag file isn't sorted, the second loop will find it.
		 * When "!_TAG_FILE_SORTED" found: start binary search if
		 * flag set.
		 */
		if (patheadlen == 0 || reg_ic)
		    state = TS_LINEAR;
		else if (STRNCMP(lbuf, "!_TAG_", 6) > 0)
		    state = TS_BINARY;
		else if (STRNCMP(lbuf, "!_TAG_FILE_SORTED\t", 18) == 0)
		{
		    /* Check sorted flag */
		    if (lbuf[18] == '1')
			state = TS_BINARY;
		    else
			state = TS_LINEAR;
		}
#else
		state = TS_LINEAR;
#endif

#ifdef BINARY_TAGS
		/*
		 * When starting a binary search, get the size of the file and
		 * compute the first offset.
		 */
		if (state == TS_BINARY)
		{
		    /* Get the tag file size (don't use fstat(), it's not
		     * portable). */
		    if ((filesize = lseek(fileno(fp),
						   (off_t)0L, SEEK_END)) <= 0)
			state = TS_LINEAR;
		    else
		    {
			lseek(fileno(fp), (off_t)0L, SEEK_SET);

			/* Calculate the first read offset in the file.  Start
			 * the search in the middle of the file.
			 */
			search_info.low_offset = 0;
			search_info.high_offset = filesize;
		    }
		    continue;
		}
#endif
	    }

	    /*
	     * Figure out where the different strings are in this line.
	     * For "normal" tags: Do a quick check if the tag matches.
	     * This speeds up tag searching a lot!
	     */
	    if (patheadlen
#ifdef EMACS_TAGS
			    && !is_etag
#endif
					)
	    {
		tagp.tagname = lbuf;
#ifdef TAG_ANY_WHITE
		tagp.tagname_end = skiptowhite(lbuf);
		if (*tagp.tagname_end == NUL)	    /* corrupted tag line */
#else
		tagp.tagname_end = vim_strchr(lbuf, TAB);
		if (tagp.tagname_end == NULL)	    /* corrupted tag line */
#endif
		{
		    EMSG2(e_tagformat, tag_fname);
		    stop_searching = TRUE;
		    break;
		}

#ifdef OLD_STATIC_TAGS
		/*
		 * Check for old style static tag: "file:tag file .."
		 */
		tagp.fname = NULL;
		for (p = lbuf; p < tagp.tagname_end; ++p)
		{
		    if (*p == ':')
		    {
			if (tagp.fname == NULL)
#ifdef TAG_ANY_WHITE
			    tagp.fname = skipwhite(tagp.tagname_end);
#else
			    tagp.fname = tagp.tagname_end + 1;
#endif
			if (	   fnamencmp(lbuf, tagp.fname, p - lbuf) == 0
#ifdef TAG_ANY_WHITE
				&& vim_iswhite(tagp.fname[p - lbuf])
#else
				&& tagp.fname[p - lbuf] == TAB
#endif
				    )
			{
			    /* found one */
			    tagp.tagname = p + 1;
			    break;
			}
		    }
		}
#endif

		/*
		 * Skip this line if the length of the tag is different and
		 * there are no wildcards, or the tag is too short.
		 */
		cmplen = tagp.tagname_end - tagp.tagname;
		if (p_tl != 0 && cmplen > p_tl)	    /* adjust for 'taglength' */
		    cmplen = p_tl;
		if (haswild && patheadlen < cmplen)
		    cmplen = patheadlen;
		else if (state == TS_LINEAR && patheadlen != cmplen)
		    continue;

#ifdef BINARY_TAGS
		if (state == TS_BINARY)
		{
		    /*
		     * Compare the current tag with the searched tag.
		     */
		    tagcmp = STRNCMP(tagp.tagname, pathead, cmplen);

		    /*
		     * A match with a shorter tag means to search forward.
		     * A match with a longer tag means to search backward.
		     */
		    if (tagcmp == 0)
		    {
			if (cmplen < patheadlen)
			    tagcmp = -1;
			else if (cmplen > patheadlen)
			    tagcmp = 1;
		    }

		    if (tagcmp == 0)
		    {
			/* We've located the tag, now skip back and search
			 * forward until the first matching tag is found.
			 */
			state = TS_SKIP_BACK;
			search_info.match_offset = search_info.curr_offset;
			continue;
		    }
		    if (tagcmp < 0
			 && search_info.curr_offset != search_info.low_offset)
		    {
			search_info.low_offset = search_info.curr_offset;
			continue;
		    }
		    if (tagcmp > 0
			&& search_info.curr_offset != search_info.high_offset)
		    {
			search_info.high_offset = search_info.curr_offset;
			continue;
		    }

		    /* No match yet and are at the end of the binary search. */
		    break;
		}
		else if (state == TS_SKIP_BACK)
		{
		    if (STRNICMP(tagp.tagname, pathead, cmplen) != 0)
			state = TS_STEP_FORWARD;
		    continue;
		}
		else if (state == TS_STEP_FORWARD)
		{
		    if (STRNICMP(tagp.tagname, pathead, cmplen))
		    {
			if (ftell(fp) > search_info.match_offset)
			    break;	/* past last match */
			else
			    continue;	/* before first match */
		    }
		}
		else
#endif
		    /* skip this match if it can't match */
		    if (STRNICMP(tagp.tagname, pathead, cmplen))
		    continue;

		/*
		 * Can be a matching tag, isolate the file name and command.
		 */
#ifdef OLD_STATIC_TAGS
		if (tagp.fname == NULL)
#endif
#ifdef TAG_ANY_WHITE
		    tagp.fname = skipwhite(tagp.tagname_end);
#else
		    tagp.fname = tagp.tagname_end + 1;
#endif
#ifdef TAG_ANY_WHITE
		tagp.fname_end = skiptowhite(tagp.fname);
		tagp.command = skipwhite(tagp.fname_end);
		if (*tagp.command == NUL)
#else
		tagp.fname_end = vim_strchr(tagp.fname, TAB);
		tagp.command = tagp.fname_end + 1;
		if (tagp.fname_end == NULL)
#endif
		    i = FAIL;
		else
		    i = OK;
	    }
	    else
		i = parse_tag_line(lbuf,
#ifdef EMACS_TAGS
				       is_etag,
#endif
					       &tagp);
	    if (i == FAIL)
	    {
		EMSG2(e_tagformat, tag_fname);
		stop_searching = TRUE;
		break;
	    }

#ifdef EMACS_TAGS
	    if (is_etag)
		tagp.fname = ebuf;
#endif
	    /*
	     * Has wildcards: find tags matching regexp "prog"
	     */
	    if (haswild)
	    {
		int	cc;

		cc = *tagp.tagname_end;
		*tagp.tagname_end = NUL;
		match = vim_regexec(prog, tagp.tagname, TRUE);
		matchoff = (int)(prog->startp[0] - tagp.tagname);
		if (match && reg_ic)
		{
		    reg_ic = FALSE;
		    match_no_ic = vim_regexec(prog, tagp.tagname, TRUE);
		    reg_ic = TRUE;
		}
		*tagp.tagname_end = cc;
	    }
	    /*
	     * No wildcards: compare tag name
	     */
	    else
	    {
		/*
		 * If tag length does not match, skip the rest
		 */
		cmplen = tagp.tagname_end - tagp.tagname;
		if (p_tl != 0 && cmplen > p_tl)	    /* adjust for 'taglength' */
		    cmplen = p_tl;
		if (patheadlen != cmplen)
		    match = FALSE;
		else
		{
		    if (reg_ic)
		    {
			match = (STRNICMP(tagp.tagname, pathead, cmplen) == 0);
			if (match)
			    match_no_ic =
				(STRNCMP(tagp.tagname, pathead, cmplen) == 0);
		    }
		    else
			match = (STRNCMP(tagp.tagname, pathead, cmplen) == 0);
		}
	    }

	    /*
	     * If a match is found, add it to ga_match[].
	     */
	    if (match)
	    {
		/* Decide in which array to store this match. */
		is_current = test_for_current(
#ifdef EMACS_TAGS
			is_etag,
#endif
				 tagp.fname, tagp.fname_end, tag_fname);
#ifdef EMACS_TAGS
		is_static = FALSE;
		if (!is_etag)	/* emacs tags are never static */
#endif
		{
#ifdef OLD_STATIC_TAGS
		    if (tagp.tagname != lbuf)	/* detected static tag before */
			is_static = TRUE;
		    else
#endif
			is_static = test_for_static(&tagp);
		}

		/* decide in which of the six table to store this match */
		if (is_static)
		{
		    if (is_current)
			mtt = MT_ST_CUR;
		    else
			mtt = MT_ST_OTH;
		}
		else
		{
		    if (is_current)
			mtt = MT_GL_CUR;
		    else
			mtt = MT_GL_OTH;
		}
		if (reg_ic && !match_no_ic)
		    mtt += MT_IC_OFF;

		if (ga_grow(&ga_match[mtt], 1) == OK)
		{
		    if (help_only)
		    {
			/*
			 * Append the help-heuristic number after the
			 * tagname, for sorting it later.
			 */
			*tagp.tagname_end = NUL;
			len = tagp.tagname_end - tagp.tagname;
			p = vim_strnsave(tagp.tagname, len + 10);
			if (p != NULL)
			    sprintf((char *)p + len + 1, "%06d",
				    help_heuristic(tagp.tagname,
						     matchoff, !match_no_ic));
			*tagp.tagname_end = TAB;
		    }
		    else if (name_only)
		    {
			len = tagp.tagname_end - tagp.tagname;
			p = vim_strnsave(tagp.tagname, len);
		    }
		    else
		    {
			/* Save the tag in a buffer.
			 * Emacs tag: <mtt><tag_fname><NUL><ebuf><NUL><lbuf>
			 * other tag: <mtt><tag_fname><NUL><NUL><lbuf>
			 * without Emacs tags: <mtt><tag_fname><NUL><lbuf>
			 */
			len = STRLEN(tag_fname) + STRLEN(lbuf) + 3;
#ifdef EMACS_TAGS
			if (is_etag)
			    len += STRLEN(ebuf) + 1;
			else
			    ++len;
#endif
			p = alloc(len);
			if (p != NULL)
			{
			    p[0] = mtt;
			    STRCPY(p + 1, tag_fname);
			    s = p + 1 + STRLEN(tag_fname) + 1;
#ifdef EMACS_TAGS
			    if (is_etag)
			    {
				STRCPY(s, ebuf);
				s += STRLEN(ebuf) + 1;
			    }
			    else
				*s++ = NUL;
#endif
			    STRCPY(s, lbuf);
			}
		    }

		    /*
		     * Don't add identical matches.
		     */
		    for (i = ga_match[mtt].ga_len; --i >= 0; )
			if (vim_memcmp(((char_u **)(ga_match[mtt].ga_data))[i],
							 p, (size_t)len) == 0)
			    break;
		    if (i < 0)
		    {
			((char_u **)(ga_match[mtt].ga_data))
						 [ga_match[mtt].ga_len++] = p;
			ga_match[mtt].ga_room--;
			++match_count;
		    }
		    else
			vim_free(p);
		}
		else    /* Out of memory! Just forget about the rest. */
		{
		    retval = OK;
		    stop_searching = TRUE;
		    break;
		}
	    }
	}
	fclose(fp);
#ifdef EMACS_TAGS
	while (incstack_idx)
	{
	    --incstack_idx;
	    fclose(incstack[incstack_idx].fp);
	    vim_free(incstack[incstack_idx].etag_fname);
	}
#endif
	/*
	 * Stop searching if sufficient tags have been found.
	 */
	if (match_count >= mincount)
	{
	    retval = OK;
	    stop_searching = TRUE;
	}

	if (stop_searching)
	    break;

      } /* end of for-each-file loop */

#ifdef BINARY_TAGS
      if (stop_searching || reg_ic)
	  break;
      reg_ic = TRUE;	/* try another time while ignoring case */
    }
#endif

    if (!stop_searching)
    {
	if (!did_open)		/* never opened any tags file */
	    EMSG("No tags file");
	retval = OK;		/* It's OK even when no tag found */
    }

findtag_end:
    vim_free(lbuf);
    vim_free(prog);
    vim_free(tag_fname);
#ifdef EMACS_TAGS
    vim_free(ebuf);
#endif

    /*
     * Move the matches from the ga_match[] arrays into one list of
     * matches.  When retval == FAIL, free the matches.
     */
    if (retval == FAIL)
	match_count = 0;

    if (match_count > 0)
	matches = (char_u **)lalloc((long_u)(match_count * sizeof(char_u *)),
									TRUE);
    else
	matches = NULL;
    match_count = 0;
    for (mtt = 0; mtt < MT_COUNT; ++mtt)
    {
	for (i = 0; i < ga_match[mtt].ga_len; ++i)
	{
	    p = ((char_u **)(ga_match[mtt].ga_data))[i];
	    if (matches == NULL)
		vim_free(p);
	    else
		matches[match_count++] = p;
	}
	ga_clear(&ga_match[mtt]);
    }

    *matchesp = matches;
    *num_matches = match_count;

    curbuf->b_help = help_save;

    return retval;
}

/*
 * Get the next name of a tag file from the tag file list.
 * For help files, use "tags" file only.
 *
 * Return FAIL if no more tag file names, OK otherwise.
 */
    static int
get_tagfname(first, buf)
    int	    first;	    /* TRUE when first file name is wanted */
    char_u  *buf;	    /* pointer to buffer of LSIZE chars */
{
    static char_u   *np = NULL;
    char_u	    *fname;
    size_t	    path_len, fname_len;
    /*
     * A list is kept of the files that have been visited.
     */
    struct visited
    {
	struct visited	*v_next;
#if defined(UNIX)
	struct stat	v_st;
#else
	char_u		v_fname[1];	/* actually longer */
#endif
    };
    static struct visited   *first_visited = NULL;
    struct visited	    *vp;
#ifdef UNIX
    struct stat		    st;
#else
    char_u		    *expand_buf;
#endif

    if (first)
    {
	np = p_tags;
	while (first_visited != NULL)
	{
	    vp = first_visited->v_next;
	    vim_free(first_visited);
	    first_visited = vp;
	}
    }

    if (np == NULL)	    /* tried allready (or bogus call) */
	return FAIL;

    /*
     * For a help window only try the file 'tags' in the same
     * directory as 'helpfile'.
     */
    if (curbuf->b_help)
    {
	path_len = gettail(p_hf) - p_hf;
	if (path_len + 9 >= LSIZE)
	    return FAIL;
	vim_memmove(buf, p_hf, path_len);
	STRCPY(buf + path_len, "tags");

	np = NULL;		/* try only once */
    }

    else
    {
#ifndef UNIX
	expand_buf = alloc(MAXPATHL);
	if (expand_buf == NULL)
	    return FAIL;
#endif

	/*
	 * Loop until we have found a file name that can be used.
	 */
	for (;;)
	{
	    if (*np == NUL)	    /* tried all possibilities */
	    {
#ifndef UNIX
		vim_free(expand_buf);
#endif
		return FAIL;
	    }

	    /*
	     * Copy next file name into buf.
	     */
	    (void)copy_option_part(&np, buf, LSIZE, " ,");

	    /*
	     * Tag file name starting with "./": Replace '.' with path of
	     * current file.
	     * Only do this when 't' flag not included in 'cpo'.
	     */
	    if (buf[0] == '.' && vim_ispathsep(buf[1])
				     && vim_strchr(p_cpo, CPO_DOTTAG) == NULL)
	    {
		if (curbuf->b_fname == NULL)	/* skip if no file name */
		    continue;

		path_len = gettail(curbuf->b_fname) - curbuf->b_fname;
		fname = buf + 1;
		while (vim_ispathsep(*fname))	/* skip '/' and the like */
		    ++fname;
		fname_len = STRLEN(fname);
		if (fname_len + path_len + 1 > LSIZE)
		    continue;
		vim_memmove(buf + path_len, fname, fname_len + 1);
		vim_memmove(buf, curbuf->b_fname, path_len);
	    }

	    /*
	     * Check if this tags file has been used already.
	     * If file doesn't exist, skip it.
	     */
#if defined(UNIX)
	    if (stat((char *)buf, &st) < 0)
#else
	    if (mch_FullName(buf, expand_buf, MAXPATHL, TRUE) == FAIL)
#endif
		continue;

	    for (vp = first_visited; vp != NULL; vp = vp->v_next)
#if defined(UNIX)
		if (vp->v_st.st_dev == st.st_dev &&
						 vp->v_st.st_ino == st.st_ino)
#else
		if (fnamecmp(vp->v_fname, expand_buf) == 0)
#endif
		    break;

	    if (vp != NULL)	    /* already visited, skip it */
		continue;

	    /*
	     * Found the next name.  Add it to the list of visited files.
	     */
#ifdef UNIX
	    vp = (struct visited *)alloc((unsigned)sizeof(struct visited));
#else
	    vp = (struct visited *)alloc((unsigned)(sizeof(struct visited) +
							 STRLEN(expand_buf)));
#endif
	    if (vp != NULL)
	    {
#ifdef UNIX
		vp->v_st = st;
#else
		STRCPY(vp->v_fname, expand_buf);
#endif
		vp->v_next = first_visited;
		first_visited = vp;
	    }
	    break;
	}
#ifndef UNIX
	vim_free(expand_buf);
#endif
    }
    return OK;
}

/*
 * Parse one line from the tags file. Find start/end of tag name, start/end of
 * file name and start of search pattern.
 *
 * If is_etag is TRUE, tagp->fname and tagp->fname_end are not set.
 *
 * Return FAIL if there is a format error in this line, OK otherwise.
 */
    static int
parse_tag_line(lbuf,
#ifdef EMACS_TAGS
		    is_etag,
#endif
			      tagp)
    char_u		*lbuf;		/* line to be parsed */
#ifdef EMACS_TAGS
    int			is_etag;
#endif
    struct tag_pointers *tagp;
{
    char_u	*p;

#ifdef EMACS_TAGS
    char_u	*p_7f;

    if (is_etag)
    {
	/*
	 * There are two formats for an emacs tag line:
	 * 1:  struct EnvBase ^?EnvBase^A139,4627
	 * 2: #define	ARPB_WILD_WORLD ^?153,5194
	 */
	p_7f = vim_strchr(lbuf, 0x7f);
	if (p_7f == NULL)
	    return FAIL;

	/* Find ^A.  If not found the line number is after the 0x7f */
	p = vim_strchr(p_7f, Ctrl('A'));
	if (p == NULL)
	    p = p_7f + 1;
	else
	    ++p;

	if (!isdigit(*p))	    /* check for start of line number */
	    return FAIL;
	tagp->command = p;


	if (p[-1] == Ctrl('A'))	    /* first format: explicit tagname given */
	{
	    tagp->tagname = p_7f + 1;
	    tagp->tagname_end = p - 1;
	}
	else			    /* second format: isolate tagname */
	{
	    /* find end of tagname */
	    for (p = p_7f - 1; !vim_iswordc(*p); --p)
		if (p == lbuf)
		    return FAIL;
	    tagp->tagname_end = p + 1;
	    while (p >= lbuf && vim_iswordc(*p))
		--p;
	    tagp->tagname = p + 1;
	}
    }
    else	/* not an Emacs tag */
    {
#endif
	/* Isolate the tagname, from lbuf up to the first white */
	tagp->tagname = lbuf;
#ifdef TAG_ANY_WHITE
	p = skiptowhite(lbuf);
#else
	p = vim_strchr(lbuf, TAB);
	if (p == NULL)
	    return FAIL;
#endif
	tagp->tagname_end = p;

	/* Isolate file name, from first to second white space */
#ifdef TAG_ANY_WHITE
	p = skipwhite(p);
#else
	if (*p != NUL)
	    ++p;
#endif
	tagp->fname = p;
#ifdef TAG_ANY_WHITE
	p = skiptowhite(p);
#else
	p = vim_strchr(p, TAB);
	if (p == NULL)
	    return FAIL;
#endif
	tagp->fname_end = p;

	/* find start of search command, after second white space */
#ifdef TAG_ANY_WHITE
	p = skipwhite(p);
#else
	if (*p != NUL)
	    ++p;
#endif
	if (*p == NUL)
	    return FAIL;
	tagp->command = p;
#ifdef EMACS_TAGS
    }
#endif

    return OK;
}

/*
 * Check if tagname is a static tag
 *
 * Static tags produced by the older ctags program have the format:
 *	'file:tag  file  /pattern'.
 * This is only recognized when both occurences of 'file' are the same, to
 * avoid recognizing "string::string" or ":exit".
 *
 * Static tags produced by the new ctags program have the format:
 *	'tag  file  /pattern/;"<Tab>file:'	    "
 *
 * Return TRUE if it is a static tag and adjust *tagname to the real tag.
 * Return FALSE if it is not a static tag.
 */
    static int
test_for_static(tagp)
    struct tag_pointers	*tagp;
{
    char_u	*p;

#ifdef OLD_STATIC_TAGS
    int		len;

    /*
     * Check for old style static tag: "file:tag file .."
     */
    len = tagp->fname_end - tagp->fname;
    p = tagp->tagname + len;
    if (       p < tagp->tagname_end
	    && *p == ':'
	    && fnamencmp(tagp->tagname, tagp->fname, len) == 0)
    {
	tagp->tagname = p + 1;
	return TRUE;
    }
#endif

    /*
     * Check for new style static tag ":...<Tab>file:[<Tab>...]"
     */
    p = tagp->command;
    while ((p = vim_strchr(p, '\t')) != NULL)
    {
	++p;
	if (STRNCMP(p, "file:", 5) == 0)
	    return TRUE;
    }

    return FALSE;
}

/*
 * Parse a line from a matching tag.  Does not change the line itself.
 *
 * The line that we get looks like this:
 * Emacs tag: <mtt><tag_fname><NUL><ebuf><NUL><lbuf>
 * other tag: <mtt><tag_fname><NUL><NUL><lbuf>
 * without Emacs tags: <mtt><tag_fname><NUL><lbuf>
 *
 * Return OK or FAIL.
 */
    static int
parse_match(lbuf, tagp)
    char_u		*lbuf;	    /* input: matching line */
    struct tag_pointers	*tagp;	    /* output: pointers into the line */
{
    int		retval;
    char_u	*p;
    char_u	*pc, *pt;

    tagp->tag_fname = lbuf + 1;
    lbuf += STRLEN(tagp->tag_fname) + 2;
#ifdef EMACS_TAGS
    if (*lbuf)
    {
	tagp->is_etag = TRUE;
	tagp->fname = lbuf;
	lbuf += STRLEN(lbuf);
	tagp->fname_end = lbuf++;
    }
    else
    {
	tagp->is_etag = FALSE;
	++lbuf;
    }
#endif

    /* Find search pattern and the file name for non-etags. */
    retval = parse_tag_line(lbuf,
#ifdef EMACS_TAGS
			tagp->is_etag,
#endif
			tagp);

    tagp->tagkind = NULL;
    tagp->command_end = NULL;

    if (retval == OK)
    {
	/* Try to find a kind field: "kind:<kind>" or just "<kind>"*/
	p = tagp->command;
	if (find_extra(&p) == OK)
	{
	    tagp->command_end = p;
	    p += 3;	/* skip ";\"\t" */
	    while (isalpha(*p))
	    {
		if (STRNCMP(p, "kind:", 5) == 0)
		{
		    tagp->tagkind = p + 5;
		    break;
		}
		pc = vim_strchr(p, ':');
		pt = vim_strchr(p, '\t');
		if (pc == NULL || (pt != NULL && pc > pt))
		{
		    tagp->tagkind = p;
		    break;
		}
		if (pt == NULL)
		    break;
		p = pt + 1;
	    }
	}
	if (tagp->tagkind != NULL)
	{
	    for (p = tagp->tagkind;
			    *p && *p != '\t' && *p != '\r' && *p != '\n'; ++p)
		;
	    tagp->tagkind_end = p;
	}
    }
    return retval;
}

/*
 * Jump to a tag that has been found in one of the tag files
 *
 * returns OK for success, NOTAGFILE when file not found, FAIL otherwise.
 */
    static int
jumpto_tag(lbuf, forceit)
    char_u	*lbuf;		/* line from the tags file for this tag */
    int		forceit;	/* :ta with ! */
{
    int		save_secure;
    int		save_magic;
    int		save_p_ws, save_p_scs, save_p_ic;
    int		csave = 0;
    char_u	*str;
    char_u	*pbuf;			/* search pattern buffer */
    char_u	*pbuf_end;
    char_u	*expanded_fname = NULL;
    char_u	*tofree_fname = NULL;
    char_u	*fname = NULL;
    struct tag_pointers tagp;
    int		retval = FAIL;
    int		getfile_result;
    int		search_options;

    pbuf = alloc(LSIZE);

    /* parse the match line into the tagp structure */
    if (pbuf == NULL || parse_match(lbuf, &tagp) == FAIL)
    {
	tagp.fname_end = NULL;
	goto erret;
    }

    /* truncate the file name, so it can be used as a string */
    csave = *tagp.fname_end;
    *tagp.fname_end = NUL;
    fname = tagp.fname;

    /* copy the command to pbuf[], remove trailing CR/NL */
    str = tagp.command;
    for (pbuf_end = pbuf; *str && *str != '\n' && *str != '\r'; )
    {
#ifdef EMACS_TAGS
	if (tagp.is_etag && *str == ',')/* stop at ',' after line number */
	    break;
#endif
	*pbuf_end++ = *str++;
    }
    *pbuf_end = NUL;

#ifdef EMACS_TAGS
    if (!tagp.is_etag)
#endif
    {
	/*
	 * Remove the "<Tab>fieldname:value" stuff; we don't need it here.
	 */
	str = pbuf;
	if (find_extra(&str) == OK)
	{
	    pbuf_end = str;
	    *pbuf_end = NUL;
	}
    }

    /*
     * expand file name (for environment variables)
     */
    expanded_fname = ExpandOne((char_u *)fname, NULL, WILD_LIST_NOTFOUND,
							    WILD_EXPAND_FREE);
    if (expanded_fname != NULL)
	fname = expanded_fname;

    /*
     * if 'tagrelative' option set, may change file name
     */
    fname = expand_rel_name(fname, tagp.tag_fname);
    if (fname == NULL)
	goto erret;
    tofree_fname = fname;	/* free() it later */

    /*
     * check if file for tag exists before abandoning current file
     */
    if (mch_getperm(fname) < 0)
    {
	retval = NOTAGFILE;
	vim_free(nofile_fname);
	nofile_fname = vim_strsave(fname);
	if (nofile_fname == NULL)
	    nofile_fname = (char_u *)"";
	goto erret;
    }

    ++RedrawingDisabled;

    /* if it was a CTRL-W CTRL-] command split window now */
    if (postponed_split)
	win_split(postponed_split > 0 ? postponed_split : 0, FALSE, FALSE);

    /* A :ta from a help file will keep the b_help flag set. */
    keep_help_flag = curbuf->b_help;
    getfile_result = getfile(0, fname, NULL, TRUE, (linenr_t)0, forceit);
    keep_help_flag = FALSE;

    if (getfile_result <= 0)		/* got to the right file */
    {
	curwin->w_set_curswant = TRUE;
	postponed_split = 0;

	save_secure = secure;
	secure = 1;
	save_magic = p_magic;
	p_magic = FALSE;	/* always execute with 'nomagic' */
	tag_modified = FALSE;

	/*
	 * If 'cpoptions' contains 't', store the search pattern for the "n"
	 * command.  If 'cpoptions' does not contain 't', the search pattern
	 * is not stored.
	 */
	if (vim_strchr(p_cpo, CPO_TAGPAT) != NULL)
	    search_options = 0;
	else
	    search_options = SEARCH_KEEP;

	/*
	 * If the command is a search, try here.
	 *
	 * Reset 'smartcase' for the search, since the search pattern was not
	 * typed by the user.
	 * Only use do_search() when there is a full search command, without
	 * anything following.
	 */
	str = pbuf;
	if (pbuf[0] == '/' || pbuf[0] == '?')
	    str = skip_regexp(pbuf + 1, pbuf[0], FALSE) + 1;
	if (str > pbuf_end - 1)	/* search command with nothing following */
	{
	    save_p_ws = p_ws;
	    save_p_ic = p_ic;
	    save_p_scs = p_scs;
	    p_ws = TRUE;	/* Switch wrap-scan on temporarily */
	    p_ic = FALSE;	/* don't ignore case now */
	    p_scs = FALSE;

	    /* put pattern in search history */
	    add_to_history(HIST_SEARCH, pbuf + 1);

	    curwin->w_cursor.lnum = 0;	/* start search before first line */
	    if (do_search(NULL, pbuf[0], pbuf + 1, (long)1, search_options))
		retval = OK;
	    else
	    {
		int	found = 1;
		int	cc;

		/*
		 * try again, ignore case now
		 */
		p_ic = TRUE;
		if (!do_search(NULL, pbuf[0], pbuf + 1, (long)1,
							      search_options))
		{
		    /*
		     * Failed to find pattern, take a guess: "^func  ("
		     */
		    found = 2;
		    (void)test_for_static(&tagp);
		    cc = *tagp.tagname_end;
		    *tagp.tagname_end = NUL;
		    sprintf((char *)pbuf, "^%s\\s\\*(", tagp.tagname);
		    if (!do_search(NULL, '/', pbuf, (long)1, search_options))
		    {
			/* Guess again: "^char * \<func  (" */
			sprintf((char *)pbuf, "^\\[#a-zA-Z_]\\.\\*\\<%s\\s\\*(",
								tagp.tagname);
			if (!do_search(NULL, '/', pbuf, (long)1,
							      search_options))
			    found = 0;
		    }
		    *tagp.tagname_end = cc;
		}
		if (found == 0)
		    EMSG("Can't find tag pattern");
		else
		{
		    /*
		     * Only give a message when really guessed, not when 'ic'
		     * is set and match found while ignoring case.
		     */
		    if (found == 2 || !save_p_ic)
		    {
			MSG("Couldn't find tag, just guessing!");
			if (!msg_scrolled)
			{
			    out_flush();
			    ui_delay(1000L, TRUE);
			}
		    }
		    retval = OK;
		}
	    }
	    p_ws = save_p_ws;
	    p_ic = save_p_ic;
	    p_scs = save_p_scs;

	    /* A search command may have positioned the cursor beyond the end
	     * of the line.  May need to correct that here. */
	    adjust_cursor();
	}
	else
	{
	    curwin->w_cursor.lnum = 1;		/* start command in line 1 */
	    do_cmdline(pbuf, NULL, NULL, DOCMD_NOWAIT|DOCMD_VERBOSE);
	    retval = OK;
	}

	/*
	 * When the command has set the b_changed flag, give a warning to the
	 * user about this.
	 */
	if (tag_modified)
	{
	    secure = 2;
	    EMSG("WARNING: tag command changed a buffer!!!");
	}
	if (secure == 2)	    /* done something that is not allowed */
	    wait_return(TRUE);
	secure = save_secure;
	p_magic = save_magic;

	/* Return OK if jumped to another file (at least we found the file!). */
	if (getfile_result == -1)
	    retval = OK;

	/*
	 * For a help buffer: Put the cursor line at the top of the window,
	 * the help subject will be below it.
	 */
	if (curbuf->b_help)
	{
	    set_topline(curwin, curwin->w_cursor.lnum);
	    update_topline();		/* correct for 'so' */
	    update_screen(NOT_VALID);
	}
	--RedrawingDisabled;
    }
    else
    {
	--RedrawingDisabled;
	if (postponed_split)		/* close the window */
	{
	    close_window(curwin, FALSE);
	    postponed_split = 0;
	}
    }

erret:
    if (tagp.fname_end != NULL)
	*tagp.fname_end = csave;
    vim_free(pbuf);
    vim_free(tofree_fname);
    vim_free(expanded_fname);

    return retval;
}

/*
 * If 'tagrelative' option set, change fname (name of file containing tag)
 * according to tag_fname (name of tag file containing fname).
 * Returns a pointer to allocated memory (or NULL when out of memory).
 */
    static char_u *
expand_rel_name(fname, tag_fname)
    char_u	*fname;
    char_u	*tag_fname;
{
    char_u	*p;
    char_u	*retval;

    if ((p_tr || curbuf->b_help) && !mch_isFullName(fname) &&
				       (p = gettail(tag_fname)) != tag_fname)
    {
	retval = alloc(MAXPATHL);
	if (retval == NULL)
	    return NULL;

	STRCPY(retval, tag_fname);
	STRNCPY(retval + (p - tag_fname), fname, MAXPATHL - (p - tag_fname));
	/*
	 * Translate names like "src/a/../b/file.c" into "src/b/file.c".
	 */
	simplify_filename(retval);
    }
    else
	retval = vim_strsave(fname);

    return retval;
}

/*
 * Moves the tail part of the path (including the terminating NUL) pointed to
 * by "tail" to the new location pointed to by "here". This should accomodate
 * an overlapping move.
 */
#define movetail(here, tail)  vim_memmove(here, tail, STRLEN(tail) + (size_t)1)

/*
 * Converts a file name into a canonical form. It simplifies a file name into
 * its simplest form by stripping out unneeded components, if any.  The
 * resulting file name is simplified in place and will either be the same
 * length as that supplied, or shorter.
 */
    void
simplify_filename(filename)
    char_u *filename;
{
#ifndef AMIGA	    /* Amiga doesn't have "..", it uses "/" */
    int	    components = 0;
    char_u  *p, *tail, *start;
#ifdef UNIX
    char_u  *orig = vim_strsave(filename);

    if (orig == NULL)
	return;
#endif

    p = filename;
#ifdef BACKSLASH_IN_FILENAME
    if (p[1] == ':')	    /* skip "x:" */
	p += 2;
#endif

    while (vim_ispathsep(*p))
	++p;
    start = p;	    /* remember start after "c:/" or "/" or "//" */

    do
    {
	/* At this point "p" is pointing to the char following a "/". */
	if (vim_ispathsep(*p))
	    movetail(p, p + 1);		/* remove duplicate "/" */
	else if (p[0] == '.' && vim_ispathsep(p[1]))
	    movetail(p, p + 2);		/* strip "./" */
	else if (p[0] == '.' && p[1] == '.' && vim_ispathsep(p[2]))
	{
	    if (components > 0)		/* strip one preceding component */
	    {
		tail = p + 3;		/* skip to after "../" or "..///" */
		while (vim_ispathsep(*tail))
		    ++tail;
		--p;
		/* skip back to after previous '/' */
		while (p > start && !vim_ispathsep(p[-1]))
		    --p;
		/* skip back to after first '/' in a row */
		while (p - 1 > start && vim_ispathsep(p[-2]))
		    --p;
		movetail(p, tail);	/* strip previous component */
		--components;
	    }
	    else			/* leading "../" */
		p += 3;			/* skip to char after "/" */
	}
	else
	{
	    ++components;		/* simple path component */
	    p = getnextcomp(p);
	}
    } while (p != NULL && *p != NUL);

#ifdef UNIX
    /* Check that the new file name is really the same file.  This will not be
     * the case when using symbolic links: "dir/link/../name" != "dir/name". */
    {
	struct stat	orig_st, new_st;

	if (	   stat((char *)orig, &orig_st) < 0
		|| stat((char *)filename, &new_st) < 0
		|| orig_st.st_ino != new_st.st_ino
		|| orig_st.st_dev != new_st.st_dev)
	    STRCPY(filename, orig);
	vim_free(orig);
    }
#endif
#endif /* !AMIGA */
}

/*
 * Check if we have a tag for the current file.
 * This is a bit slow, because of the full path compare in fullpathcmp().
 * Return TRUE if tag for file "fname" if tag file "tag_fname" is for current
 * file.
 */
    static int
#ifdef EMACS_TAGS
test_for_current(is_etag, fname, fname_end, tag_fname)
    int	    is_etag;
#else
test_for_current(fname, fname_end, tag_fname)
#endif
    char_u  *fname;
    char_u  *fname_end;
    char_u  *tag_fname;
{
    int	    c;
    int	    retval = FALSE;
    char_u  *relname;

    if (curbuf->b_ffname != NULL)	/* if the current buffer has a name */
    {
#ifdef EMACS_TAGS
	if (is_etag)
	    c = 0;	    /* to shut up GCC */
	else
#endif
	{
	    c = *fname_end;
	    *fname_end = NUL;
	}
	relname = expand_rel_name(fname, tag_fname);
	if (relname != NULL)
	{
	    retval = (fullpathcmp(relname, curbuf->b_ffname, TRUE) & FPC_SAME);
	    vim_free(relname);
	}
#ifdef EMACS_TAGS
	if (!is_etag)
#endif
	    *fname_end = c;
    }

    return retval;
}

/*
 * Find the end of the tagaddress.
 * Return OK if ";\"\t" is following, FAIL otherwise.
 */
    static int
find_extra(pp)
    char_u	**pp;
{
    char_u	*str = *pp;

    /* Repeat for addresses separated with ';' */
    for (;;)
    {
	if (isdigit(*str))
	    str = skipdigits(str);
	else if (*str == '/' || *str == '?')
	{
	    str = skip_regexp(str + 1, *str, FALSE);
	    if (*str != **pp)
		str = NULL;
	    else
		++str;
	}
	else
	    str = NULL;
	if (str == NULL || *str != ';'
		      || !(isdigit(str[1]) || str[1] == '/' || str[1] == '?'))
	    break;
	++str;	/* skip ';' */
    }

    if (str != NULL && STRNCMP(str, ";\"\t", 3) == 0)
    {
	*pp = str;
	return OK;
    }
    return FAIL;
}
