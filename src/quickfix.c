/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * quickfix.c: functions for quickfix mode, using a file with error messages
 */

#include "vim.h"

#if defined(FEAT_QUICKFIX) || defined(PROTO)

/*
 * define DEBUG_CD to print out control msgs for change directory parsing
 */
/* #define DEBUG_CD */

struct dir_stack_t
{
    struct dir_stack_t	*next;
    char_u		*dirname;
};

static void	qf_msg __ARGS((void));
static void	qf_free __ARGS((int idx));
static char_u	*qf_types __ARGS((int, int));
static int	qf_get_fnum __ARGS((char_u *, char_u *));
static char_u	*qf_push_dir __ARGS((char_u *, struct dir_stack_t **));
static char_u	*qf_pop_dir __ARGS((/*char_u *, */struct dir_stack_t **));
static char_u	*qf_guess_filepath __ARGS((char_u *));
static void	qf_fmt_text __ARGS((char_u *text, char_u *buf, int bufsize));
static void	qf_clean_dir_stack __ARGS((struct dir_stack_t **));
#ifdef FEAT_WINDOWS
static buf_t	*qf_find_buf __ARGS((void));
static void	qf_update_buffer __ARGS((void));
static void	qf_fill_buffer __ARGS((void));
#endif

static struct dir_stack_t   *dir_stack = NULL;

/*
 * for each error the next struct is allocated and linked in a list
 */
struct qf_line
{
    struct qf_line  *qf_next;	/* pointer to next error in the list */
    struct qf_line  *qf_prev;	/* pointer to previous error in the list */
    linenr_t	     qf_lnum;	/* line number where the error occurred */
    int		     qf_fnum;	/* file number for the line */
    int		     qf_col;	/* column where the error occurred */
    int		     qf_nr;	/* error number */
    char_u	    *qf_text;	/* description of the error */
    char_u	     qf_cleared;/* set to TRUE if line has been deleted */
    char_u	     qf_type;	/* type of the error (mostly 'E') */
    char_u	     qf_valid;	/* valid error message detected */
};

/*
 * There is a stack of error lists.
 */
#define LISTCOUNT   10

struct qf_list
{
    struct qf_line *qf_start;	/* pointer to the first error */
    struct qf_line *qf_ptr;	/* pointer to the current error */
    int  qf_count;		/* number of errors (0 means no error list) */
    int  qf_index;		/* current index in the error list */
    int  qf_nonevalid;		/* TRUE if not a single valid entry found */
} qf_lists[LISTCOUNT];

static int	qf_curlist = 0;	/* current error list */
static int	qf_listcount = 0;   /* current number of lists */

#define FMT_PATTERNS 8		/* maximum number of % recognized */

/*
 * Structure used to hold the info of one part of 'errorformat'
 */
struct eformat
{
    regprog_t	    *prog;	/* pre-formatted part of 'errorformat' */
    struct eformat  *next;	/* pointer to next (NULL if last) */
    char_u	    addr[FMT_PATTERNS]; /* indices of used % patterns */
    char_u	    prefix;	/* prefix of this format line: */
				/*   'D' enter directory */
				/*   'X' leave directory */
				/*   'A' start of multi-line message */
				/*   'E' error message */
				/*   'W' warning message */
				/*   'I' informational message */
				/*   'C' continuation line */
				/*   'Z' end of multi-line message */
				/*   'G' general, unspecific message */
				/*   'P' push file (partial) message */
				/*   'Q' pop/quit file (partial) message */
				/*   'O' overread (partial) message */
    char_u	    flags;	/* additional flags given in prefix */
				/*   '-' do not include this line */
};

/*
 * Read the errorfile into memory, line by line, building the error list.
 * Return -1 for error, number of errors for success.
 */
    int
qf_init(efile, errorformat)
    char_u	    *efile;
    char_u	    *errorformat;
{
    char_u	    *namebuf;
    char_u	    *errmsg;
    char_u	    *fmtstr = NULL;
    int		    col = 0;
    int		    type = 0;
    int		    valid;
    long	    lnum = 0L;
    int		    enr = 0;
    FILE	    *fd;
    struct qf_line  *qfp = NULL;
    struct qf_line  *qfprev = NULL;	/* init to make SASC shut up */
    char_u	    *efmp;
    struct eformat  *fmt_first = NULL;
    struct eformat  *fmt_last = NULL;
    struct eformat  *fmt_ptr;
    char_u	    *efm;
    char_u	    *ptr;
    char_u	    *srcptr;
    int		    len;
    int		    i;
    int		    round;
    int		    idx = 0;
    int		    multiline = FALSE;
    int		    multiscan = FALSE;
    int		    retval = -1;	/* default: return error flag */
    char_u	    *directory = NULL;
    char_u	    *currfile = NULL;
    char_u	    *tail = NULL;
    struct dir_stack_t  *file_stack = NULL;
    regmatch_t	    regmatch;
    static struct fmtpattern
    {
	char_u	convchar;
	char	*pattern;
    }		    fmt_pat[FMT_PATTERNS] =
		    {
			{'f', "\\f\\+"},
			{'n', "\\d\\+"},
			{'l', "\\d\\+"},
			{'c', "\\d\\+"},
			{'t', "."},
			{'m', ".\\+"},
			{'r', ".*"},
			{'p', "[- ]*"}
		    };

    if (efile == NULL)
	return FAIL;

    namebuf = alloc(CMDBUFFSIZE + 1);
    errmsg = alloc(CMDBUFFSIZE + 1);
    if (namebuf == NULL || errmsg == NULL)
	goto qf_init_end;

    if ((fd = mch_fopen((char *)efile, "r")) == NULL)
    {
	EMSG2(_(e_openerrf), efile);
	goto qf_init_end;
    }

    /*
     * If the current entry is not the last entry, delete entries below the
     * current entry.  This makes it possible to browse in a tree-like way
     * with ":grep'.
     */
    while (qf_listcount > qf_curlist + 1)
	qf_free(--qf_listcount);

    /*
     * When the stack is full, remove to oldest entry
     * Otherwise, add a new entry.
     */
    if (qf_listcount == LISTCOUNT)
    {
	qf_free(0);
	for (i = 1; i < LISTCOUNT; ++i)
	    qf_lists[i - 1] = qf_lists[i];
	qf_curlist = LISTCOUNT - 1;
    }
    else
	qf_curlist = qf_listcount++;
    qf_lists[qf_curlist].qf_index = 0;
    qf_lists[qf_curlist].qf_count = 0;

/*
 * Each part of the format string is copied and modified from errorformat to
 * regex prog.  Only a few % characters are allowed.
 */
    efm = errorformat;
    /*
     * Get some space to modify the format string into.
     */
    i = (FMT_PATTERNS * 3) + (STRLEN(efm) << 2);
    for (round = FMT_PATTERNS; round > 0; )
	i += strlen(fmt_pat[--round].pattern);
    i += 2; /* "%f" can become two chars longer */
    if ((fmtstr = alloc(i)) == NULL)
	goto error2;

    while (efm[0])
    {
	/*
	 * Allocate a new eformat structure and put it at the end of the list
	 */
	fmt_ptr = (struct eformat *)alloc((unsigned)sizeof(struct eformat));
	if (fmt_ptr == NULL)
	    goto error2;
	if (fmt_first == NULL)	    /* first one */
	    fmt_first = fmt_ptr;
	else
	    fmt_last->next = fmt_ptr;
	fmt_last = fmt_ptr;
	fmt_ptr->prefix = NUL;
	fmt_ptr->flags = NUL;
	fmt_ptr->next = NULL;
	fmt_ptr->prog = NULL;
	for (round = FMT_PATTERNS; round > 0; )
	    fmt_ptr->addr[--round] = NUL;
	/* round is 0 now */

	/*
	 * Isolate one part in the 'errorformat' option
	 */
	for (len = 0; efm[len] != NUL && efm[len] != ','; ++len)
	    if (efm[len] == '\\' && efm[len + 1] != NUL)
		++len;

	/*
	 * Build regexp pattern from current 'errorformat' option
	 */
	ptr = fmtstr;
	*ptr++ = '^';
	for (efmp = efm; efmp < efm + len; ++efmp)
	{
	    if (*efmp == '%')
	    {
		++efmp;
		for (idx = 0; idx < FMT_PATTERNS; ++idx)
		    if (fmt_pat[idx].convchar == *efmp)
			break;
		if (idx < FMT_PATTERNS)
		{
		    if (fmt_ptr->addr[idx])
		    {
			sprintf((char *)errmsg,
				_("Too many %%%c in format string"), *efmp);
			EMSG(errmsg);
			goto error2;
		    }
		    if ((idx
				&& idx < 6
				&& vim_strchr((char_u *)"DXOPQ",
						     fmt_ptr->prefix) != NULL)
			    || (idx == 6
				&& vim_strchr((char_u *)"OPQ",
						    fmt_ptr->prefix) == NULL))
		    {
			sprintf((char *)errmsg,
				_("Unexpected %%%c in format string"), *efmp);
			EMSG(errmsg);
			goto error2;
		    }
		    fmt_ptr->addr[idx] = (char_u)++round;
		    *ptr++ = '\\';
		    *ptr++ = '(';
		    if (*efmp == 'f' && efmp[1] != NUL
					 && efmp[1] != '\\' && efmp[1] != '%')
		    {
			/* A file name may contain spaces, but this isn't in
			 * "\f".  use "[^x]\+" instead (x is next character) */
			*ptr++ = '[';
			*ptr++ = '^';
			*ptr++ = efmp[1];
			*ptr++ = ']';
			*ptr++ = '\\';
			*ptr++ = '+';
		    }
		    else
		    {
			srcptr = (char_u *)fmt_pat[idx].pattern;
			while ((*ptr = *srcptr++) != NUL)
			    ++ptr;
		    }
		    *ptr++ = '\\';
		    *ptr++ = ')';
		}
		else if (*efmp == '*')
		{
		    if (*++efmp == '[' || *efmp == '\\')
		    {
			if ((*ptr++ = *efmp) == '[')	/* %*[^a-z0-9] etc. */
			{
			    if (efmp[1] == '^')
				*ptr++ = *++efmp;
			    if (efmp < efm + len)
			    {
				*ptr++ = *++efmp;	    /* could be ']' */
				while (efmp < efm + len
					&& (*ptr++ = *++efmp) != ']')
				    /* skip */;
				if (efmp == efm + len)
				{
				    EMSG(_("Missing ] in format string"));
				    goto error2;
				}
			    }
			}
			else if (efmp < efm + len)	/* %*\D, %*\s etc. */
			    *ptr++ = *++efmp;
			*ptr++ = '\\';
			*ptr++ = '+';
		    }
		    else
		    {
			/* TODO: scanf()-like: %*ud, %*3c, %*f, ... ? */
			sprintf((char *)errmsg,
				_("Unsupported %%%c in format string"), *efmp);
			EMSG(errmsg);
			goto error2;
		    }
		}
		else if (vim_strchr((char_u *)"%\\.^$~[", *efmp) != NULL)
		    *ptr++ = *efmp;		/* regexp magic characters */
		else if (*efmp == '#')
		    *ptr++ = '*';
		else if (efmp == efm + 1)		/* analyse prefix */
		{
		    if (vim_strchr((char_u *)"+-", *efmp) != NULL)
			fmt_ptr->flags = *efmp++;
		    if (vim_strchr((char_u *)"DXAEWICZGOPQ", *efmp) != NULL)
			fmt_ptr->prefix = *efmp;
		    else
		    {
			sprintf((char *)errmsg,
				_("Invalid %%%c in format string prefix"), *efmp);
			EMSG(errmsg);
			goto error2;
		    }
		}
		else
		{
		    sprintf((char *)errmsg,
			    _("Invalid %%%c in format string"), *efmp);
		    EMSG(errmsg);
		    goto error2;
		}
	    }
	    else			/* copy normal character */
	    {
		if (*efmp == '\\' && efmp + 1 < efm + len)
		    ++efmp;
		else if (vim_strchr((char_u *)".*^$~[", *efmp) != NULL)
		    *ptr++ = '\\';	/* escape regexp atoms */
		if (*efmp)
		    *ptr++ = *efmp;
	    }
	}
	*ptr++ = '$';
	*ptr = NUL;
	if ((fmt_ptr->prog = vim_regcomp(fmtstr, TRUE)) == NULL)
	    goto error2;
	/*
	 * Advance to next part
	 */
	efm = skip_to_option_part(efm + len);	/* skip comma and spaces */
    }
    if (fmt_first == NULL)	/* nothing found */
    {
	EMSG(_("'errorformat' contains no pattern"));
	goto error2;
    }

    /*
     * got_int is reset here, because it was probably set when killing the
     * ":make" command, but we still want to read the errorfile then.
     */
    got_int = FALSE;

    /*
     * Read the lines in the error file one by one.
     * Try to recognize one of the error formats in each line.
     */
#ifdef DEBUG_CD
    emsg(_("Change dir debugging enabled."));
#endif
    while (fgets((char *)IObuff, CMDBUFFSIZE, fd) != NULL && !got_int)
    {
	IObuff[CMDBUFFSIZE] = NUL;  /* for very long lines */
	if ((efmp = vim_strrchr(IObuff, '\n')) != NULL)
	    *efmp = NUL;
#ifdef USE_CRNL
	if ((efmp = vim_strrchr(IObuff, '\r')) != NULL)
	    *efmp = NUL;
#endif

	/*
	 * Try to match each part of 'errorformat' until we find a complete
	 * match or no match.
	 */
	valid = TRUE;
restofline:
	for (fmt_ptr = fmt_first; fmt_ptr != NULL; fmt_ptr = fmt_ptr->next)
	{
	    idx = fmt_ptr->prefix;
	    if (multiscan && vim_strchr((char_u *)"OPQ", idx) == NULL)
		continue;
	    namebuf[0] = NUL;
	    if (!multiscan)
		errmsg[0] = NUL;
	    lnum = 0;
	    col = 0;
	    enr = -1;
	    type = 0;
	    tail = NULL;

	    regmatch.regprog = fmt_ptr->prog;
	    if (vim_regexec(&regmatch, IObuff, (colnr_t)0))
	    {
		if ((idx == 'C' || idx == 'Z') && !multiline)
		    continue;
		if (vim_strchr((char_u *)"EWI", idx) != NULL)
		    type = idx;
		else
		    type = 0;
		/*
		 * Extract error message data from matched line
		 */
		if ((i = (int)fmt_ptr->addr[0]) > 0)		/* %f */
		{
		    len = regmatch.endp[i] - regmatch.startp[i];
		    STRNCPY(namebuf, regmatch.startp[i], len);
		    namebuf[len] = NUL;
		    if (vim_strchr((char_u *)"OPQ", idx) != NULL
			    && mch_getperm(namebuf) == -1)
		    {
#ifdef DEBUG_CD
			char mess[200];
			sprintf(mess,_("Not a proper file name: '%s'"), namebuf);
			emsg(mess);
#endif
			continue;
		    }
#ifdef DEBUG_CD
		    {
			char mess[200];
			sprintf(mess,_("File name '%s' is valid"),namebuf);
			emsg(mess);
		    }
#endif
		}
		if ((i = (int)fmt_ptr->addr[1]) > 0)		/* %n */
		    enr = (int)atol((char *)regmatch.startp[i]);
		if ((i = (int)fmt_ptr->addr[2]) > 0)		/* %l */
		    lnum = atol((char *)regmatch.startp[i]);
		if ((i = (int)fmt_ptr->addr[3]) > 0)		/* %c */
		    col = (int)atol((char *)regmatch.startp[i]);
		if ((i = (int)fmt_ptr->addr[4]) > 0)		/* %t */
		    type = *regmatch.startp[i];
		if (fmt_ptr->flags ==  '+' && !multiscan)	/* %+ */
		    STRCPY(errmsg, IObuff);
		else if ((i = (int)fmt_ptr->addr[5]) > 0)	/* %m */
		{
		    len = regmatch.endp[i] - regmatch.startp[i];
		    STRNCPY(errmsg, regmatch.startp[i], len);
		    errmsg[len] = NUL;
		}
		if ((i = (int)fmt_ptr->addr[6]) > 0)		/* %r */
		    tail = regmatch.startp[i];
		if ((i = (int)fmt_ptr->addr[7]) > 0)		/* %p */
		    col = (int)(regmatch.endp[i] - regmatch.startp[i] + 1);
		break;
	    }
	}
	multiscan = FALSE;
	if (!fmt_ptr || idx == 'D' || idx == 'X')
	{
	    if (fmt_ptr)
	    {
		if (idx == 'D')				/* enter directory */
		{
		    if (*namebuf == NUL)
		    {
			EMSG(_("Missing or empty directory name"));
			goto error1;
		    }
		    if ((directory = qf_push_dir(namebuf, &dir_stack)) == NULL)
			goto error1;
#ifdef DEBUG_CD
		    emsg2(_("Enter: %s"), directory);
#endif
		}
		else if (idx == 'X')			/* leave directory */
		{
		    directory = qf_pop_dir(/* namebuf, */&dir_stack);
#ifdef DEBUG_CD
		    emsg2(_("Leave: %s"), directory);
#endif
		}
	    }
	    namebuf[0] = NUL;		/* no match found, remove file name */
	    lnum = 0;			/* don't jump to this line */
	    valid = FALSE;
	    STRCPY(errmsg, IObuff);	/* copy whole line to error message */
	    if (!fmt_ptr)
		multiline = FALSE;
	}
	else if (fmt_ptr)
	{
	    if (vim_strchr((char_u *)"AEWI", idx) != NULL)
		multiline = TRUE;	/* start of a multi-line message */
	    else if (vim_strchr((char_u *)"CZ", idx) != NULL)
	    {				/* continuation of multi-line msg */
		if (qfprev == NULL)
		    goto error1;
		if (*errmsg)
		{
		    len = STRLEN(qfprev->qf_text);
		    if ((ptr = alloc((unsigned)(len + STRLEN(errmsg) + 2)))
								    == NULL)
			goto error1;
		    STRCPY(ptr, qfprev->qf_text);
		    vim_free(qfprev->qf_text);
		    qfprev->qf_text = ptr;
		    *(ptr += len) = '\n';
		    STRCPY(++ptr, errmsg);
		}
		if (qfp->qf_nr == -1)
		    qfp->qf_nr = enr;
		if (vim_isprintc(type) && !qfp->qf_type)
		    qfp->qf_type = type;    /* only printable chars allowed */
		if (!qfp->qf_lnum)
		    qfp->qf_lnum = lnum;
		if (!qfp->qf_col)
		    qfp->qf_col = col;
		if (!qfp->qf_fnum)
		    qfp->qf_fnum = qf_get_fnum(directory,
					*namebuf || directory ? namebuf
					  : currfile && valid ? currfile : 0);
		if (idx == 'Z')
		    multiline = FALSE;
		line_breakcheck();
		continue;
	    }
	    else if (vim_strchr((char_u *)"OPQ", idx) != NULL)
	    {
		/* global file names */
		valid = FALSE;
		if (*namebuf == NUL || mch_getperm(namebuf) >= 0)
		{
		    if (*namebuf && idx == 'P')
			qf_push_dir(currfile = namebuf, &file_stack);
		    else if (idx == 'Q')
			currfile = qf_pop_dir(/*namebuf, */&file_stack);
		    *namebuf = NUL;
		    if (tail && *tail)
		    {
			STRCPY(IObuff, skipwhite(tail));
			multiscan = TRUE;
			goto restofline;
		    }
		}
	    }
	    if (fmt_ptr->flags == '-')	/* generally exclude this line */
		continue;
	}

	if ((qfp = (struct qf_line *)alloc((unsigned)sizeof(struct qf_line)))
								      == NULL)
	    goto error2;

	qfp->qf_fnum = qf_get_fnum(directory, *namebuf || directory ?
				  namebuf : currfile && valid ? currfile : 0);
	if ((qfp->qf_text = vim_strsave(errmsg)) == NULL)
	    goto error1;
	if (!vim_isprintc(type))	/* only printable chars allowed */
	    type = 0;
	qfp->qf_lnum = lnum;
	qfp->qf_col = col;
	qfp->qf_nr = enr;
	qfp->qf_type = type;
	qfp->qf_valid = valid;

	if (qf_lists[qf_curlist].qf_count == 0)	/* first element in the list */
	{
	    qf_lists[qf_curlist].qf_start = qfp;
	    qfp->qf_prev = qfp;	/* first element points to itself */
	}
	else
	{
	    qfp->qf_prev = qfprev;
	    qfprev->qf_next = qfp;
	}
	qfp->qf_next = qfp;	/* last element points to itself */
	qfp->qf_cleared = FALSE;
	qfprev = qfp;
	++qf_lists[qf_curlist].qf_count;
	if (qf_lists[qf_curlist].qf_index == 0 && qfp->qf_valid)
						/* first valid entry */
	{
	    qf_lists[qf_curlist].qf_index = qf_lists[qf_curlist].qf_count;
	    qf_lists[qf_curlist].qf_ptr = qfp;
	}
	line_breakcheck();
    }
    if (!ferror(fd))
    {
	if (qf_lists[qf_curlist].qf_index == 0)	/* no valid entry found */
	{
	    qf_lists[qf_curlist].qf_ptr = qf_lists[qf_curlist].qf_start;
	    qf_lists[qf_curlist].qf_index = 1;
	    qf_lists[qf_curlist].qf_nonevalid = TRUE;
	}
	else
	    qf_lists[qf_curlist].qf_nonevalid = FALSE;
	retval = qf_lists[qf_curlist].qf_count;	/* return number of matches */
	goto qf_init_ok;
    }
    EMSG(_(e_readerrf));
error1:
    vim_free(qfp);
error2:
    qf_free(qf_curlist);
    qf_listcount--;
    if (qf_curlist > 0)
	--qf_curlist;
qf_init_ok:
    fclose(fd);
    for (fmt_ptr = fmt_first; fmt_ptr != NULL; fmt_ptr = fmt_first)
    {
	fmt_first = fmt_ptr->next;
	vim_free(fmt_ptr->prog);
	vim_free(fmt_ptr);
    }
    qf_clean_dir_stack(&dir_stack);
    qf_clean_dir_stack(&file_stack);
qf_init_end:
    vim_free(namebuf);
    vim_free(errmsg);
    vim_free(fmtstr);

#ifdef FEAT_WINDOWS
    qf_update_buffer();
#endif

    return retval;
}

/*
 * get buffer number for file "dir.name"
 */
    static int
qf_get_fnum(directory, fname)
    char_u   *directory;
    char_u   *fname;
{
    if (fname == NULL || *fname == NUL)		/* no file name */
	return 0;
    {
#ifdef RISCOS
	/* Name is reported as `main.c', but file is `c.main' */
	return ro_buflist_add(fname);
#else
	char_u	    *ptr;
	int	    fnum;

	if (directory != NULL && !vim_isAbsName(fname)
	    && (ptr = concat_fnames(directory, fname, TRUE)) != NULL)
	{
	    /*
	     * Here we check if the file really exists.
	     * This should normally be true, but if make works without
	     * "leaving directory"-messages we might have missed a
	     * directory change.
	     */
	    if (mch_getperm(ptr) < 0)
	    {
		vim_free(ptr);
		directory = qf_guess_filepath(fname);
		if (directory)
		    ptr = concat_fnames(directory, fname, TRUE);
		else
		    ptr = vim_strsave(fname);
	    }
	    /* Use concatenated directory name and file name */
	    fnum = buflist_add(ptr, FALSE);
	    vim_free(ptr);
	    return fnum;
	}
	return buflist_add(fname, FALSE);
#endif
    }
}

/*
 * push dirbuf onto the directory stack and return pointer to actual dir or
 * NULL on error
 */
    static char_u *
qf_push_dir(dirbuf, stackptr)
    char_u		*dirbuf;
    struct dir_stack_t	**stackptr;
{
    struct dir_stack_t  *ds_new;
    struct dir_stack_t  *ds_ptr;

    /* allocate new stack element and hook it in */
    ds_new = (struct dir_stack_t *)alloc((unsigned)sizeof(struct dir_stack_t));
    if (ds_new == NULL)
	return NULL;

    ds_new->next = *stackptr;
    *stackptr = ds_new;

    /* store directory on the stack */
    if (vim_isAbsName(dirbuf)
	    || (*stackptr)->next == NULL
	    || (*stackptr && dir_stack != *stackptr))
	(*stackptr)->dirname = vim_strsave(dirbuf);
    else
    {
        /* Okay we don't have an absolute path.
         * dirbuf must be a subdir of one of the directories on the stack.
         * Let's search...
         */
	ds_new = (*stackptr)->next;
	(*stackptr)->dirname = NULL;
	while (ds_new)
	{
	    vim_free((*stackptr)->dirname);
	    (*stackptr)->dirname = concat_fnames(ds_new->dirname, dirbuf,
		    TRUE);
	    if (mch_isdir((*stackptr)->dirname) == TRUE)
		break;

	    ds_new = ds_new->next;
	}

        /* clean up all dirs we already left */
	while ((*stackptr)->next != ds_new)
	{
            ds_ptr = (*stackptr)->next;
            (*stackptr)->next = (*stackptr)->next->next;
	    vim_free(ds_ptr->dirname);
	    vim_free(ds_ptr);
	}

	/* Nothing found -> it must be on top level */
	if (ds_new == NULL)
	{
	    vim_free((*stackptr)->dirname);
	    (*stackptr)->dirname = vim_strsave(dirbuf);
	}
    }

    if ((*stackptr)->dirname != NULL)
	return (*stackptr)->dirname;
    else
    {
	ds_ptr = *stackptr;
	*stackptr = (*stackptr)->next;
	vim_free(ds_ptr);
	return NULL;
    }
}


/*
 * pop dirbuf from the directory stack and return previous directory or NULL if
 * stack is empty
 */
    static char_u *
qf_pop_dir(/*dirbuf, */stackptr)
    /*char_u		*dirbuf;*/
    struct dir_stack_t	**stackptr;
{
    struct dir_stack_t  *ds_ptr;

    /* TODO: Should we check if dirbuf is the directory on top of the stack?
     * What to do if it isn't?
     */

    /* pop top element and free it */
    if (*stackptr != NULL)
    {
	ds_ptr = *stackptr;
	*stackptr = (*stackptr)->next;
	vim_free(ds_ptr->dirname);
	vim_free(ds_ptr);
    }

    /* return NEW top element as current dir or NULL if stack is empty*/
    return *stackptr ? (*stackptr)->dirname : NULL;
}

/*
 * clean up directory stack
 */
    static void
qf_clean_dir_stack(stackptr)
    struct dir_stack_t	**stackptr;
{
    struct dir_stack_t  *ds_ptr;

    while ((ds_ptr = *stackptr) != NULL)
    {
	*stackptr = (*stackptr)->next;
	vim_free(ds_ptr->dirname);
	vim_free(ds_ptr);
    }
}

/*
 * Check in which directory of the directory stack the given file can be
 * found.
 * Returns a pointer to the directory name or NULL if not found
 * Cleans up intermediate directory entries.
 *
 * TODO: How to solve the following problem?
 * If we have the this directory tree:
 *     ./
 *     ./aa
 *     ./aa/bb
 *     ./bb
 *     ./bb/x.c
 * and make says:
 *     making all in aa
 *     making all in bb
 *     x.c:9: Error
 * Then qf_push_dir thinks we are in ./aa/bb, but we are in ./bb.
 * qf_guess_filepath will return NULL.
 */
    static char_u *
qf_guess_filepath(filename)
    char_u *filename;
{
    struct dir_stack_t     *ds_ptr;
    struct dir_stack_t     *ds_tmp;
    char_u                 *fullname;

    /* no dirs on the stack - there's nothing we can do */
    if (dir_stack == NULL)
        return NULL;

    ds_ptr = dir_stack->next;
    fullname = NULL;
    while (ds_ptr)
    {
	vim_free(fullname);
	fullname = concat_fnames(ds_ptr->dirname, filename, TRUE);

	/* If concat_fnames failed, just go on. The worst thing that can happen
	 * is that we delete the entire stack.
	 */
	if ((fullname != NULL) && (mch_getperm(fullname) >= 0))
	    break;

	ds_ptr = ds_ptr->next;
    }

    vim_free(fullname);

    /* clean up all dirs we already left */
    while (dir_stack->next != ds_ptr)
    {
	ds_tmp = dir_stack->next;
	dir_stack->next = dir_stack->next->next;
	vim_free(ds_tmp->dirname);
	vim_free(ds_tmp);
    }

    return ds_ptr==NULL? NULL: ds_ptr->dirname;

}

/*
 * jump to a quickfix line
 * if dir == FORWARD go "errornr" valid entries forward
 * if dir == BACKWARD go "errornr" valid entries backward
 * else if "errornr" is zero, redisplay the same line
 * else go to entry "errornr"
 */
    void
qf_jump(dir, errornr, forceit)
    int	    dir;
    int	    errornr;
    int	    forceit;
{
    struct qf_line  *qf_ptr;
    struct qf_line  *old_qf_ptr;
    int		    qf_index;
    int		    old_qf_fnum;
    int		    old_qf_index;
    static char_u   *e_no_more_items = (char_u *)N_("No more items");
    char_u	    *err = e_no_more_items;
    linenr_t	    i;
    buf_t	    *old_curbuf;
    linenr_t	    old_lnum;
#ifdef FEAT_WINDOWS
    int		    opened_window = FALSE;
#endif
    int		    print_message = TRUE;
    int		    len;

    if (qf_curlist >= qf_listcount || qf_lists[qf_curlist].qf_count == 0)
    {
	EMSG(_(e_quickfix));
	return;
    }

    qf_ptr = qf_lists[qf_curlist].qf_ptr;
    old_qf_ptr = qf_ptr;
    qf_index = qf_lists[qf_curlist].qf_index;
    old_qf_index = qf_index;
    if (dir == FORWARD || dir == FORWARD_FILE)	    /* next valid entry */
    {
	while (errornr--)
	{
	    old_qf_ptr = qf_ptr;
	    old_qf_index = qf_index;
	    old_qf_fnum = qf_ptr->qf_fnum;
	    do
	    {
		if (qf_index == qf_lists[qf_curlist].qf_count
						   || qf_ptr->qf_next == NULL)
		{
		    qf_ptr = old_qf_ptr;
		    qf_index = old_qf_index;
		    if (err != NULL)
		    {
			EMSG(_(err));
			goto theend;
		    }
		    errornr = 0;
		    break;
		}
		++qf_index;
		qf_ptr = qf_ptr->qf_next;
	    } while ((!qf_lists[qf_curlist].qf_nonevalid && !qf_ptr->qf_valid)
		  || (dir == FORWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));
	    err = NULL;
	}
    }
    else if (dir == BACKWARD)	    /* previous valid entry */
    {
	while (errornr--)
	{
	    old_qf_ptr = qf_ptr;
	    old_qf_index = qf_index;
	    do
	    {
		if (qf_index == 1 || qf_ptr->qf_prev == NULL)
		{
		    qf_ptr = old_qf_ptr;
		    qf_index = old_qf_index;
		    if (err != NULL)
		    {
			EMSG(_(err));
			goto theend;
		    }
		    errornr = 0;
		    break;
		}
		--qf_index;
		qf_ptr = qf_ptr->qf_prev;
	    } while (!qf_lists[qf_curlist].qf_nonevalid && !qf_ptr->qf_valid);
	    err = NULL;
	}
    }
    else if (errornr != 0)	/* go to specified number */
    {
	while (errornr < qf_index && qf_index > 1 && qf_ptr->qf_prev != NULL)
	{
	    --qf_index;
	    qf_ptr = qf_ptr->qf_prev;
	}
	while (errornr > qf_index && qf_index < qf_lists[qf_curlist].qf_count
						   && qf_ptr->qf_next != NULL)
	{
	    ++qf_index;
	    qf_ptr = qf_ptr->qf_next;
	}
    }

#ifdef FEAT_WINDOWS
    /*
     * If currently in the quickfix window, find another window to show the
     * file in.
     */
    if (bt_quickfix(curbuf))
    {
	win_t	*win;
	int	save_p_sb;

	/*
	 * If there is no file specified, we don't know where to go.
	 * But do advance, otherwise ":cn" gets stuck.
	 */
	if (qf_ptr->qf_fnum == 0)
	    goto theend;

	/* No need to print the error message if it's visible in the error
	 * window */
	if (qf_index >= curwin->w_topline && qf_index < curwin->w_botline)
	    print_message = FALSE;

	/*
	 * If there is only one window, create a new one above the quickfix
	 * window.
	 */
	if (firstwin == lastwin)
	{
	    save_p_sb = p_sb;
	    p_sb = FALSE;
	    i = win_split(0, 0);
	    p_sb = save_p_sb;
	    if (i == FAIL)		/* not enough room for window */
		goto failed;
	    opened_window = TRUE;	/* close it when fail */
	}
	else
	{
	    /*
	     * Try to find a window that shows the right buffer.
	     * Default to the window just above the quickfix buffer.
	     */
	    win = curwin->w_prev;
	    for (;;)
	    {
		if (win == NULL || bt_quickfix(win->w_buffer))
		{
		    /* Didn't find it, go to the window above the quickfix
		     * window. */
		    if (curwin->w_prev == NULL)
			win = curwin->w_next;
		    else
			win = curwin->w_prev;
		    break;
		}
		if (win->w_buffer->b_fnum == qf_ptr->qf_fnum)
		    break;
		if (win->w_prev == NULL)
		    win = lastwin;	/* wrap around the top */
		else
		    win = win->w_prev;	/* go to previous window */
	    }
	    win_goto(win);
	}
    }
#endif

    /*
     * If there is a file name,
     * read the wanted file if needed, and check autowrite etc.
     */
    old_curbuf = curbuf;
    old_lnum = curwin->w_cursor.lnum;
    if (qf_ptr->qf_fnum == 0 || buflist_getfile(qf_ptr->qf_fnum,
		    (linenr_t)1, GETF_SETMARK | GETF_SWITCH, forceit) == OK)
    {
	/* When not switched to another buffer, still need to set pc mark */
	if (curbuf == old_curbuf)
	    setpcmark();

	/*
	 * Go to line with error, unless qf_lnum is 0.
	 */
	i = qf_ptr->qf_lnum;
	if (i > 0)
	{
	    if (i > curbuf->b_ml.ml_line_count)
		i = curbuf->b_ml.ml_line_count;
	    curwin->w_cursor.lnum = i;
	}
	if (qf_ptr->qf_col > 0)
	{
	    curwin->w_cursor.col = qf_ptr->qf_col - 1;
	    adjust_cursor();
	}
	else
	    beginline(BL_WHITE | BL_FIX);

	if (print_message)
	{
	    /* Update the screen before showing the message */
	    update_topline_redraw();
	    sprintf((char *)IObuff, _("(%d of %d)%s%s: "), qf_index,
		    qf_lists[qf_curlist].qf_count,
		    qf_ptr->qf_cleared ? (char_u *)_(" (line deleted)")
							       : (char_u *)"",
		    qf_types(qf_ptr->qf_type, qf_ptr->qf_nr));
	    /* Add the message, skipping leading whitespace and newlines. */
	    len = STRLEN(IObuff);
	    qf_fmt_text(qf_ptr->qf_text, IObuff + len, IOSIZE - len);

	    /* Output the message.  Overwrite to avoid scrolling when the 'O'
	     * flag is present in 'shortmess'; But when not jumping, print the
	     * whole message. */
	    i = msg_scroll;
	    if (curbuf == old_curbuf && curwin->w_cursor.lnum == old_lnum)
		msg_scroll = TRUE;
	    else if (!msg_scrolled && shortmess(SHM_OVERALL))
		msg_scroll = FALSE;
	    msg(IObuff);
	    msg_scroll = i;

	    /*
	     * If the message is short, redisplay after redrawing the screen
	     */
	    if (linetabsize(IObuff) < ((int)p_ch - 1) * Columns + sc_col)
	    {
		keep_msg = IObuff;
		keep_msg_attr = 0;
	    }
	}
    }
    else
    {
#ifdef FEAT_WINDOWS
	if (opened_window)
	    win_close(curwin, TRUE);    /* Close opened window */
#endif
	if (qf_ptr->qf_fnum != 0)
	{
	    /*
	     * Couldn't open file, so put index back where it was.  This could
	     * happen if the file was readonly and we changed something.
	     */
#ifdef FEAT_WINDOWS
failed:
#endif
	    qf_ptr = old_qf_ptr;
	    qf_index = old_qf_index;
	}
    }
theend:
    qf_lists[qf_curlist].qf_ptr = qf_ptr;
    qf_lists[qf_curlist].qf_index = qf_index;
}

/*
 * ":clist": list all errors
 */
    void
qf_list(eap)
    exarg_t	*eap;
{
    buf_t		*buf;
    char_u		*fname;
    struct qf_line	*qfp;
    int			i;
    int			idx1 = 1;
    int			idx2 = -1;
    int			need_return = TRUE;
    int			last_printed = 1;
    char_u		*arg = eap->arg;
    int			all = eap->forceit;	/* if not :cl!, only show
						   recognised errors */

    if (qf_curlist >= qf_listcount || qf_lists[qf_curlist].qf_count == 0)
    {
	EMSG(_(e_quickfix));
	return;
    }
    if (!get_list_range(&arg, &idx1, &idx2) || *arg != NUL)
    {
	EMSG(_(e_trailing));
	return;
    }
    i = qf_lists[qf_curlist].qf_count;
    if (idx1 < 0)
	idx1 = (-idx1 > i) ? 0 : idx1 + i + 1;
    if (idx2 < 0)
	idx2 = (-idx2 > i) ? 0 : idx2 + i + 1;

    more_back_used = TRUE;
    if (qf_lists[qf_curlist].qf_nonevalid)
	all = TRUE;
    qfp = qf_lists[qf_curlist].qf_start;
    for (i = 1; !got_int && i <= qf_lists[qf_curlist].qf_count; )
    {
	if ((qfp->qf_valid || all) && idx1 <= i && i <= idx2)
	{
	    if (need_return)
	    {
		msg_putchar('\n');
		need_return = FALSE;
	    }
	    if (more_back == 0)
	    {
		fname = NULL;
		if (qfp->qf_fnum != 0
			      && (buf = buflist_findnr(qfp->qf_fnum)) != NULL)
		    fname = buf->b_fname;
		if (fname == NULL)
		    sprintf((char *)IObuff, "%2d", i);
		else
		    sprintf((char *)IObuff, "%2d %s", i, fname);
		msg_outtrans_attr(IObuff, i == qf_lists[qf_curlist].qf_index
			? hl_attr(HLF_L) : hl_attr(HLF_D));
		if (qfp->qf_lnum == 0)
		    IObuff[0] = NUL;
		else if (qfp->qf_col == 0)
		    sprintf((char *)IObuff, ":%ld", qfp->qf_lnum);
		else
		    sprintf((char *)IObuff, ":%ld col %d",
						   qfp->qf_lnum, qfp->qf_col);
		sprintf((char *)IObuff + STRLEN(IObuff), "%s: ",
					  qf_types(qfp->qf_type, qfp->qf_nr));
		msg_puts_attr(IObuff, hl_attr(HLF_N));
		/* Remove newlines and leading whitespace from the text. */
		qf_fmt_text(qfp->qf_text, IObuff, IOSIZE);
		msg_prt_line(IObuff);
		out_flush();		/* show one line at a time */
		need_return = TRUE;
		last_printed = i;
	    }
	}
	if (more_back)
	{
	    /* scrolling backwards from the more-prompt */
	    /* TODO: compute the number of items from the screen lines */
	    more_back = more_back * 2 - 1;
	    while (i > last_printed - more_back && i > idx1)
	    {
		do
		{
		    qfp = qfp->qf_prev;
		    --i;
		}
		while (i > idx1 && !qfp->qf_valid && !all);
	    }
	    more_back = 0;
	}
	else
	{
	    qfp = qfp->qf_next;
	    ++i;
	}
	ui_breakcheck();
    }
    more_back_used = FALSE;
}

/*
 * Remove newlines and leading whitespace from an error message.
 * Put the result in "buf[bufsize]".
 */
    static void
qf_fmt_text(text, buf, bufsize)
    char_u	*text;
    char_u	*buf;
    int		bufsize;
{
    int		i;
    char_u	*p = skipwhite(text);

    for (i = 0; *p != NUL && i < bufsize - 1; ++i)
    {
	if (*p == '\n')
	{
	    buf[i] = ' ';
	    while (*++p != NUL)
		if (!vim_iswhite(*p) && *p != '\n')
		    break;
	}
	else
	    buf[i] = *p++;
    }
    buf[i] = NUL;
}

/*
 * ":colder [count]": Up in the quickfix stack.
 * ":cnewer [count]": Down in the quickfix stack.
 */
    void
qf_age(eap)
    exarg_t	*eap;
{
    int		count;

    if (eap->addr_count != 0)
	count = eap->line2;
    else
	count = 1;
    while (count--)
    {
	if (eap->cmdidx == CMD_colder)
	{
	    if (qf_curlist == 0)
	    {
		EMSG(_("At bottom of quickfix stack"));
		return;
	    }
	    --qf_curlist;
	}
	else
	{
	    if (qf_curlist >= qf_listcount - 1)
	    {
		EMSG(_("At top of quickfix stack"));
		return;
	    }
	    ++qf_curlist;
	}
    }
    qf_msg();
}

    static void
qf_msg()
{
    smsg((char_u *)_("error list %d of %d; %d errors"),
	    qf_curlist + 1, qf_listcount, qf_lists[qf_curlist].qf_count);
#ifdef FEAT_WINDOWS
    qf_update_buffer();
#endif
}

/*
 * free the error list
 */
    static void
qf_free(idx)
    int		idx;
{
    struct qf_line *qfp;

    while (qf_lists[idx].qf_count)
    {
	qfp = qf_lists[idx].qf_start->qf_next;
	vim_free(qf_lists[idx].qf_start->qf_text);
	vim_free(qf_lists[idx].qf_start);
	qf_lists[idx].qf_start = qfp;
	--qf_lists[idx].qf_count;
    }
}

/*
 * qf_mark_adjust: adjust marks
 */
   void
qf_mark_adjust(line1, line2, amount, amount_after)
    linenr_t	line1;
    linenr_t	line2;
    long	amount;
    long	amount_after;
{
    int			i;
    struct qf_line	*qfp;
    int			idx;

    for (idx = 0; idx < qf_listcount; ++idx)
	if (qf_lists[idx].qf_count)
	    for (i = 0, qfp = qf_lists[idx].qf_start;
		       i < qf_lists[idx].qf_count; ++i, qfp = qfp->qf_next)
		if (qfp->qf_fnum == curbuf->b_fnum)
		{
		    if (qfp->qf_lnum >= line1 && qfp->qf_lnum <= line2)
		    {
			if (amount == MAXLNUM)
			    qfp->qf_cleared = TRUE;
			else
			    qfp->qf_lnum += amount;
		    }
		    else if (amount_after && qfp->qf_lnum > line2)
			qfp->qf_lnum += amount_after;
		}
}

/*
 * Make a nice message out of the error character and the error number:
 *  char    number	message
 *  e or E    0		" error"
 *  w or W    0		" warning"
 *  i or I    0		" info"
 *  0	      0		""
 *  other     0		" c"
 *  e or E    n		" error n"
 *  w or W    n		" warning n"
 *  i or I    n		" info n"
 *  0	      n		" error n"
 *  other     n		" c n"
 */
    static char_u *
qf_types(c, nr)
    int c, nr;
{
    static char_u	buf[20];
    static char_u	cc[3];
    char_u		*p;

    if (c == 'W' || c == 'w')
	p = (char_u *)" warning";
    else if (c == 'I' || c == 'i')
	p = (char_u *)" info";
    else if (c == 'E' || c == 'e' || (c == 0 && nr > 0))
	p = (char_u *)" error";
    else if (c == 0)
	p = (char_u *)"";
    else
    {
	cc[0] = ' ';
	cc[1] = c;
	cc[2] = NUL;
	p = cc;
    }

    if (nr <= 0)
	return p;

    sprintf((char *)buf, "%s %3d", p, nr);
    return buf;
}

#if defined(FEAT_WINDOWS) || defined(PROTO)
/*
 * ":cwindow": open a window that shows the list of errors.
 */
    void
ex_cwindow(eap)
    exarg_t	*eap;
{
    int		height;
    buf_t	*buf;
    win_t	*win;
    int		save_p_sb;
    int		i;

    if (eap->addr_count != 0)
	height = eap->line2;
    else
	height = QF_WINHEIGHT;

#ifdef FEAT_VISUAL
    reset_VIsual_and_resel();			/* stop Visual mode */
#endif
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * Find existing quickfix window, or open a new one.
     */
    for (win = firstwin; win != NULL; win = win->w_next)
	if (bt_quickfix(win->w_buffer))
	    break;
    if (win != NULL)
	win_goto(win);
    else
    {
	/* Create the new window at the very bottom. */
	win_goto(lastwin);
	save_p_sb = p_sb;
	p_sb = TRUE;
	i = win_split(height, 0);
	p_sb = save_p_sb;
	if (i == FAIL)		/* not enough room for window */
	    return;

	/*
	 * Find existing quickfix buffer, or create a new one.
	 */
	buf = qf_find_buf();
	if (buf == NULL)
	{
	    (void)do_ecmd(0, NULL, NULL, NULL, ECMD_ONE, ECMD_HIDE);
	    /* switch off 'swapfile' */
	    set_option_value((char_u *)"swf", 0L, NULL, TRUE);
	    set_option_value((char_u *)"bt", 0L, (char_u *)"quickfix", TRUE);
	    set_option_value((char_u *)"bh", 0L, (char_u *)"delete", TRUE);
	}
	else
	    (void)do_buffer(DOBUF_GOTO, DOBUF_FIRST, FORWARD, buf->b_fnum,
			    FALSE);
	win_setheight(height);
    }

    /*
     * Fill the buffer with the quickfix list.
     */
    qf_fill_buffer();
}

/*
 * Find quickfix buffer.
 */
    static buf_t *
qf_find_buf()
{
    buf_t	*buf;

    for (buf = firstbuf; buf != NULL; buf = buf->b_next)
	if (bt_quickfix(buf))
	    break;
    return buf;
}

/*
 * Find the quickfix buffer.  If it exists, update the contents.
 */
    static void
qf_update_buffer()
{
    buf_t	*buf;
#ifdef FEAT_AUTOCMD
    aco_save_t	aco;
#else
    buf_t	*save_curbuf;
#endif

    /* Check if a buffer for the quickfix list exists.  Update it. */
    buf = qf_find_buf();
    if (buf != NULL)
    {
#ifdef FEAT_AUTOCMD
	/* set curwin/curbuf to buf and save a few things */
	aucmd_prepbuf(&aco, buf);
#else
	save_curbuf = curbuf;
	curbuf = buf;
#endif
	qf_fill_buffer();
#ifdef FEAT_AUTOCMD
	/* restore curwin/curbuf and a few other things */
	aucmd_restbuf(&aco);
#else
	curbuf = save_curbuf;
#endif
    }
}

/*
 * Fill current buffer with quickfix errors, replacing any previous contents.
 * curbuf must be the quickfix buffer!
 */
    static void
qf_fill_buffer()
{
    linenr_t		lnum;
    struct qf_line	*qfp;
    buf_t		*errbuf;
    int			len;

    /* delete all existing lines */
    while ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
	(void)ml_delete((linenr_t)1, FALSE);

    /* Check if there is anything to display */
    if (qf_curlist < qf_listcount)
    {
	/* Add one line for each error */
	qfp = qf_lists[qf_curlist].qf_start;
	for (lnum = 0; lnum < qf_lists[qf_curlist].qf_count; ++lnum)
	{
	    if (qfp->qf_fnum != 0
		    && (errbuf = buflist_findnr(qfp->qf_fnum)) != NULL
		    && errbuf->b_fname != NULL)
	    {
		STRCPY(IObuff, errbuf->b_fname);
		len = STRLEN(IObuff);
	    }
	    else
		len = 0;
	    IObuff[len++] = '|';

	    if (qfp->qf_lnum > 0)
	    {
		sprintf((char *)IObuff + len, "%ld", qfp->qf_lnum);
		len += STRLEN(IObuff + len);

		if (qfp->qf_col > 0)
		{
		    sprintf((char *)IObuff + len, " col %d", qfp->qf_col);
		    len += STRLEN(IObuff + len);
		}

		sprintf((char *)IObuff + len, "%s",
			qf_types(qfp->qf_type, qfp->qf_nr));
		len += STRLEN(IObuff + len);
	    }
	    IObuff[len++] = '|';
	    IObuff[len++] = ' ';

	    qf_fmt_text(qfp->qf_text, IObuff + len, IOSIZE - len);

	    if (ml_append(lnum, IObuff, (colnr_t)STRLEN(IObuff) + 1, FALSE)
								      == FAIL)
		break;
	    qfp = qfp->qf_next;
	}
	/* Delete the empty line which is now at the end */
	(void)ml_delete(lnum + 1, FALSE);
    }

    /* correct cursor position */
    check_lnums(TRUE);

    /* Set the 'filetype' to "qf" each time after filling the buffer.  This
     * resembles reading a file into a buffer, it's more logical when using
     * autocommands. */
    set_option_value((char_u *)"ft", 0L, (char_u *)"qf", TRUE);

#ifdef FEAT_AUTOCMD
    apply_autocmds(EVENT_BUFREADPOST, (char_u *)"quickfix", NULL,
							       FALSE, curbuf);
#endif

    /* make sure it will be redrawn */
    redraw_curbuf_later(NOT_VALID);
}

#endif /* FEAT_WINDOWS */

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    int
bt_quickfix(buf)
    buf_t	*buf;
{
    return (buf->b_p_bt[0] == 'q');
}

/*
 * Return TRUE if "buf" is a "nofile" buffer.
 */
    int
bt_nofile(buf)
    buf_t	*buf;
{
    return (buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f');
}

/*
 * Return TRUE if "buf" is a "nowrite" or "nofile" buffer.
 */
    int
bt_dontwrite(buf)
    buf_t	*buf;
{
    return (buf->b_p_bt[0] == 'n');
}

    int
bt_dontwrite_msg(buf)
    buf_t	*buf;
{
    if (bt_dontwrite(buf))
    {
	EMSG(_("Cannot write, 'buftype' option is set"));
	return TRUE;
    }
    return FALSE;
}

/*
 * Return TRUE if the buffer should be hidden, according to 'hidden', ":hide"
 * and 'bufhidden'.
 */
    int
buf_hide(buf)
    buf_t	*buf;
{
    /* 'bufhidden' overrules 'hidden' and ":hide", check it first */
    switch (buf->b_p_bh[0])
    {
	case 'u':		    /* "unload" */
	case 'd': return FALSE;	    /* "delete" */
	case 'h': return TRUE;	    /* "hide" */
    }
    return (p_hid || cmdmod.hide);
}
#endif /* FEAT_QUICKFIX */
