/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * quickfix.c: functions for quickfix mode, using a file with error messages
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

static void qf_free __ARGS((void));
static char_u *qf_types __ARGS((int, int));

/*
 * for each error the next struct is allocated and linked in a list
 */
struct qf_line
{
	struct qf_line	*qf_next;	/* pointer to next error in the list */
	struct qf_line	*qf_prev;	/* pointer to previous error in the list */
	linenr_t		 qf_lnum;	/* line number where the error occurred */
	int				 qf_fnum;	/* file number for the line */
	int				 qf_col;	/* column where the error occurred */
	int				 qf_nr;		/* error number */
	char_u			*qf_text;	/* description of the error */
	char_u			 qf_cleared;/* set to TRUE if line has been deleted */
	char_u			 qf_type;	/* type of the error (mostly 'E') */
	char_u			 qf_valid;	/* valid error message detected */
};

static struct qf_line *qf_start;		/* pointer to the first error */
static struct qf_line *qf_ptr;			/* pointer to the current error */

static int	qf_count = 0;		/* number of errors (0 means no error list) */
static int	qf_index;			/* current index in the error list */
static int	qf_nonevalid;		/* set to TRUE if not a single valid entry found */

/*
 * Read the errorfile into memory, line by line, building the error list.
 * Return FAIL for error, OK for success.
 */
	int
qf_init()
{
	char_u 			namebuf[CMDBUFFSIZE + 1];
	char_u			errmsg[CMDBUFFSIZE + 1];
	int				col;
	int				type;
	int				valid;
	long			lnum;
	int				enr;
	FILE			*fd;
	struct qf_line	*qfp = NULL;
	struct qf_line	*qfprev = NULL;		/* init to make SASC shut up */
	char_u			*pfmt, *fmtstr;
#ifdef UTS2
	char_u			*(adr[7]);
#else
	void			*(adr[7]);
#endif
	int				adr_cnt = 0;
	int				maxlen;
	int				i;

	if (p_ef == NULL || *p_ef == NUL)
	{
		emsg(e_errorf);
		return FAIL;
	}
	if ((fd = fopen((char *)p_ef, "r")) == NULL)
	{
		emsg2(e_openerrf, p_ef);
		return FAIL;
	}
	qf_free();
	qf_index = 0;
	for (i = 0; i < 7; ++i)
		adr[i] = NULL;

/*
 * The format string is copied and modified from p_efm to fmtstr.
 * Only a few % characters are allowed.
 */
		/* get some space to modify the format string into */
		/* must be able to do the largest expansion 7 times (7 x 3) */
	maxlen = STRLEN(p_efm) + 25;
	fmtstr = alloc(maxlen);
	if (fmtstr == NULL)
		goto error2;
	for (pfmt = p_efm, i = 0; *pfmt; ++pfmt, ++i)
	{
		if (pfmt[0] != '%')				/* copy normal character */
			fmtstr[i] = pfmt[0];
		else
		{
			fmtstr[i++] = '%';
			switch (pfmt[1])
			{
			case 'f':		/* filename */
					adr[adr_cnt++] = namebuf;

			case 'm':		/* message */
					if (pfmt[1] == 'm')
						adr[adr_cnt++] = errmsg;
					fmtstr[i++] = '[';
					fmtstr[i++] = '^';
					if (pfmt[2])
						fmtstr[i++] = pfmt[2];
					else
#ifdef MSDOS
						fmtstr[i++] = '\r';
#else
						fmtstr[i++] = '\n';
#endif
					fmtstr[i] = ']';
					break;
			case 'c':		/* column */
					adr[adr_cnt++] = &col;
					fmtstr[i] = 'd';
					break;
			case 'l':		/* line */
					adr[adr_cnt++] = &lnum;
					fmtstr[i++] = 'l';
					fmtstr[i] = 'd';
					break;
			case 'n':		/* error number */
					adr[adr_cnt++] = &enr;
					fmtstr[i] = 'd';
					break;
			case 't':		/* error type */
					adr[adr_cnt++] = &type;
					fmtstr[i] = 'c';
					break;
			case '%':		/* %% */
			case '*':		/* %*: no assignment */
					fmtstr[i] = pfmt[1];
					break;
			default:
					EMSG("invalid % in format string");
					goto error2;
			}
			if (adr_cnt == 7)
			{
				EMSG("too many % in format string");
				goto error2;
			}
			++pfmt;
		}
		if (i >= maxlen - 6)
		{
			EMSG("invalid format string");
			goto error2;
		}
	}
	fmtstr[i] = NUL;

	while (fgets((char *)IObuff, CMDBUFFSIZE, fd) != NULL && !got_int)
	{
		if ((qfp = (struct qf_line *)alloc((unsigned)sizeof(struct qf_line))) == NULL)
			goto error2;

		IObuff[CMDBUFFSIZE] = NUL;	/* for very long lines */
		namebuf[0] = NUL;
		errmsg[0] = NUL;
		lnum = 0;
		col = 0;
		enr = -1;
		type = 0;
		valid = TRUE;

		if (sscanf((char *)IObuff, (char *)fmtstr, adr[0], adr[1], adr[2], adr[3],
												adr[4], adr[5]) != adr_cnt)
		{
			namebuf[0] = NUL;			/* something failed, remove file name */
			valid = FALSE;
			STRCPY(errmsg, IObuff);		/* copy whole line to error message */
			if ((pfmt = STRRCHR(errmsg, '\n')) != NULL)
				*pfmt = NUL;
#ifdef MSDOS
			if ((pfmt = STRRCHR(errmsg, '\r')) != NULL)
				*pfmt = NUL;
#endif
		}

		if (namebuf[0] == NUL)			/* no file name */
			qfp->qf_fnum = 0;
		else
			qfp->qf_fnum = buflist_add(namebuf);
		if ((qfp->qf_text = strsave(errmsg)) == NULL)
			goto error1;
		qfp->qf_lnum = lnum;
		qfp->qf_col = col;
		qfp->qf_nr = enr;
		qfp->qf_type = type;
		qfp->qf_valid = valid;

		if (qf_count == 0)		/* first element in the list */
		{
			qf_start = qfp;
			qfp->qf_prev = qfp;	/* first element points to itself */
		}
		else
		{
			qfp->qf_prev = qfprev;
			qfprev->qf_next = qfp;
		}
		qfp->qf_next = qfp;		/* last element points to itself */
		qfp->qf_cleared = FALSE;
		qfprev = qfp;
		++qf_count;
		if (qf_index == 0 && qfp->qf_valid)		/* first valid entry */
		{
			qf_index = qf_count;
			qf_ptr = qfp;
		}
		breakcheck();
	}
	free(fmtstr);
	if (!ferror(fd))
	{
		if (qf_index == 0)		/* no valid entry found */
		{
			qf_ptr = qf_start;
			qf_index = 1;
			qf_nonevalid = TRUE;
		}
		else
			qf_nonevalid = FALSE;
		fclose(fd);
		qf_jump(0, 0);			/* display first error */
		return OK;
	}
	emsg(e_readerrf);
error1:
	free(qfp);
error2:
	fclose(fd);
	qf_free();
	return FAIL;
}

/*
 * jump to a quickfix line
 * if dir == FORWARD go "errornr" valid entries forward
 * if dir == BACKWARD go "errornr" valid entries backward
 * else if "errornr" is zero, redisplay the same line
 * else go to entry "errornr"
 */
	void
qf_jump(dir, errornr)
	int		dir;
	int		errornr;
{
	linenr_t		i;

	if (qf_count == 0)
	{
		emsg(e_quickfix);
		return;
	}

	if (dir == FORWARD)		/* next valid entry */
	{
		while (errornr--)
		{
			do
			{
				if (qf_index == qf_count || qf_ptr->qf_next == NULL)
					break;
				++qf_index;
				qf_ptr = qf_ptr->qf_next;
			} while (!qf_nonevalid && !qf_ptr->qf_valid);
		}
	}
	else if (dir == BACKWARD)		/* previous valid entry */
	{
		while (errornr--)
		{
			do
			{
				if (qf_index == 1 || qf_ptr->qf_prev == NULL)
					break;
				--qf_index;
				qf_ptr = qf_ptr->qf_prev;
			} while (!qf_nonevalid && !qf_ptr->qf_valid);
		}
	}
	else if (errornr != 0)		/* go to specified number */
	{
		while (errornr < qf_index && qf_index > 1 && qf_ptr->qf_prev != NULL)
		{
			--qf_index;
			qf_ptr = qf_ptr->qf_prev;
		}
		while (errornr > qf_index && qf_index < qf_count && qf_ptr->qf_next != NULL)
		{
			++qf_index;
			qf_ptr = qf_ptr->qf_next;
		}
	}

	/*
	 * If there is a file name, 
	 * read the wanted file if needed, and check autowrite etc.
	 */
	if (qf_ptr->qf_fnum == 0 || buflist_getfile(qf_ptr->qf_fnum, (linenr_t)1, TRUE) == OK)
	{
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
		curwin->w_cursor.col = qf_ptr->qf_col;
		adjust_cursor();
		cursupdate();
		smsg((char_u *)"(%d of %d) %s%s: %s", qf_index, qf_count, 
					qf_ptr->qf_cleared ? (char_u *)"(line deleted) " : (char_u *)"",
					qf_types(qf_ptr->qf_type, qf_ptr->qf_nr), qf_ptr->qf_text);
	}
}

/*
 * list all errors
 */
	void
qf_list()
{
	struct qf_line *qfp;
	int i;

	if (qf_count == 0)
	{
		emsg(e_quickfix);
		return;
	}
	qfp = qf_start;
	gotocmdline(TRUE, NUL);
	for (i = 1; !got_int && i <= qf_count; ++i)
	{
		sprintf((char *)IObuff, "%2d line %3ld col %2d %s: %s",
			i,
			(long)qfp->qf_lnum,
			qfp->qf_col,
			qf_types(qfp->qf_type, qfp->qf_nr),
			qfp->qf_text);
		msg_outstr(IObuff);
		msg_outchar('\n');
		qfp = qfp->qf_next;
		flushbuf();					/* show one line at a time */
		breakcheck();
	}
	wait_return(FALSE);
}

/*
 * free the error list
 */
	static void
qf_free()
{
	struct qf_line *qfp;

	while (qf_count)
	{
		qfp = qf_start->qf_next;
		free(qf_start->qf_text);
		free(qf_start);
		qf_start = qfp;
		--qf_count;
	}
}

/*
 * qf_mark_adjust: adjust marks
 */
   void
qf_mark_adjust(line1, line2, inc)
	linenr_t	line1;
	linenr_t	line2;
	long		inc;
{
	register int i;
	struct qf_line *qfp;

	if (qf_count)
		for (i = 0, qfp = qf_start; i < qf_count; ++i, qfp = qfp->qf_next)
			if (qfp->qf_fnum == curbuf->b_fnum &&
							qfp->qf_lnum >= line1 && qfp->qf_lnum <= line2)
			{
				if (inc == MAXLNUM)
					qfp->qf_cleared = TRUE;
				else
					qfp->qf_lnum += inc;
			}
}

/*
 * Make a nice message out of the error character and the error number:
 *	char	number		message
 *  e or E    0			"  Error"
 *  w or W    0			"Warning"
 *  other     0			 ""
 *  w or W    n			"Warning n"
 *  other     n			"  Error n"
 */
	static char_u *
qf_types(c, nr)
	int c, nr;
{
	static char_u	buf[20];
	char_u		*p1;

	p1 = (char_u *)"  Error";
	if (c == 'W' || c == 'w')
		p1 =  (char_u *)"Warning";
	else if (nr <= 0 && c != 'E' && c != 'e')
		p1 = (char_u *)"";

	if (nr <= 0)
		return p1;

	sprintf((char *)buf, "%s %3d", p1, nr);
	return buf;
}
