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
 * quickfix.c: functions for quickfix mode, using a file with error messages
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

static void qf_free __ARGS((void));
static char *qf_types __ARGS((int, int));

/*
 * for each error the next struct is allocated and linked in a list
 */
struct qf_line
{
	struct qf_line	*qf_next;	/* pointer to next error in the list */
	struct qf_line	*qf_prev;	/* pointer to previous error in the list */
	linenr_t		 qf_lnum;	/* line number where the error occurred */
	char			*qf_mark;	/* pointer to that line (if != NULL) */
	int				 qf_col;	/* column where the error occurred */
	int				 qf_nr;		/* error number */
	char			*qf_fname;	/* file name where the error occurred */
	char			*qf_text;	/* description of the error */
	char			 qf_cleared;/* set to TRUE if qf_mark has been cleared */
	char			 qf_type;	/* type of the error (mostly 'E') */
	char			 qf_valid;	/* valid error message detected */
};

static struct qf_line *qf_start;		/* pointer to the first error */
static struct qf_line *qf_ptr;			/* pointer to the current error */

static int	qf_count = 0;		/* number of errors (0 means no error list) */
static int	qf_index;			/* current index in the error list */
static int	qf_nonevalid;		/* set to TRUE if not a single valid entry found */
static int	qf_marksset;		/* set to 1 when qf_mark-s have been set */

/*
 * Read the errorfile into memory, line by line, building the error list.
 * Return 1 for error, 0 for success.
 */
	int
qf_init()
{
	char 			namebuf[CMDBUFFSIZE + 1];
	char			errmsg[CMDBUFFSIZE + 1];
	int				col;
	char			type;
	char			valid;
	long			lnum;
	int				enr;
	FILE			*fd;
	struct qf_line	*qfp = NULL;
	struct qf_line	*qfprev = NULL;		/* init to make SASC shut up */
	char			*pfmt, *fmtstr;
#ifdef UTS2
	char			*(adr[7]);
#else
	void			*(adr[7]);
#endif
	int				adr_cnt = 0;
	int				maxlen;
	int				i;

	if (p_ef == NULL || *p_ef == NUL)
	{
		emsg(e_errorf);
		return 1;
	}
	if ((fd = fopen(p_ef, "r")) == NULL)
	{
		emsg(e_openerrf);
		return 1;
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
	maxlen = strlen(p_efm) + 25;
	fmtstr = (char *)alloc(maxlen);
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
					emsg("invalid % in format string");
					goto error2;
			}
			if (adr_cnt == 7)
			{
				emsg("too many % in format string");
				goto error2;
			}
			++pfmt;
		}
		if (i >= maxlen - 6)
		{
			emsg("invalid format string");
			goto error2;
		}
	}
	fmtstr[i] = NUL;

	while (fgets(IObuff, CMDBUFFSIZE, fd) != NULL)
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

		if (sscanf(IObuff, fmtstr, adr[0], adr[1], adr[2], adr[3],
												adr[4], adr[5]) != adr_cnt)
		{
			namebuf[0] = NUL;			/* something failed, remove file name */
			valid = FALSE;
			strcpy(errmsg, IObuff);		/* copy whole line to error message */
			if ((pfmt = strrchr(errmsg, '\n')) != NULL)
				*pfmt = NUL;
#ifdef MSDOS
			if ((pfmt = strrchr(errmsg, '\r')) != NULL)
				*pfmt = NUL;
#endif
		}

		if ((qfp->qf_fname = strsave(namebuf)) == NULL)
			goto error1;
		if ((qfp->qf_text = strsave(errmsg)) == NULL)
		{
			free(qfp->qf_fname);
			goto error1;
		}
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
		qfp->qf_mark = NULL;
		qfp->qf_cleared = FALSE;
		qfprev = qfp;
		++qf_count;
		if (qf_index == 0 && qfp->qf_valid)		/* first valid entry */
		{
			qf_index = qf_count;
			qf_ptr = qfp;
		}
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
		qf_jump(0);				/* display first error */
		return 0;
	}
	emsg(e_readerrf);
error1:
	free(qfp);
error2:
	fclose(fd);
	qf_free();
	return 1;
}

/*
 * jump to quickfix line "errornr"; if "errornr" is zero, redisplay the same line
 */
	void
qf_jump(errornr)
	int errornr;
{
	struct qf_line *qfp;
	linenr_t		i;
	char			*msgp;

	if (qf_count == 0)
	{
		emsg(e_quickfix);
		return;
	}

	if (errornr == -1)		/* next valid entry */
	{
		do
		{
			if (qf_index == qf_count)
				break;
			++qf_index;
			qf_ptr = qf_ptr->qf_next;
		} while (!qf_nonevalid && !qf_ptr->qf_valid);
	}
	else if (errornr == -2)		/* previous valid entry */
	{
		do
		{
			if (qf_index == 1)
				break;
			--qf_index;
			qf_ptr = qf_ptr->qf_prev;
		} while (!qf_nonevalid && !qf_ptr->qf_valid);
	}
	else if (errornr != 0)		/* go to specified number */
	{
		while (errornr < qf_index && qf_index > 1)
		{
			--qf_index;
			qf_ptr = qf_ptr->qf_prev;
		}
		while (errornr > qf_index && qf_index < qf_count)
		{
			++qf_index;
			qf_ptr = qf_ptr->qf_next;
		}
	}

	/*
	 * If there is a file name, 
	 * read the wanted file if needed, and check autowrite etc.
	 */
	if (qf_ptr->qf_fname[0] == NUL || getfile(qf_ptr->qf_fname, NULL, TRUE) <= 0)
	{
		/*
		 * Use mark if possible, because the line number may be invalid
		 * after line inserts / deletes.
		 * If qf_lnum is 0, stay on the same line.
		 */
		i = 0;
		msgp = "";
		if ((qf_ptr->qf_mark != NULL && (i = ptr2nr(qf_ptr->qf_mark, (linenr_t)0)) == 0) || qf_ptr->qf_cleared)
			msgp = "(line changed) ";
		if (i == 0)
			i = qf_ptr->qf_lnum;
		if (i > line_count)
			i = line_count;
		if (i > 0)
			Curpos.lnum = i;
		Curpos.col = qf_ptr->qf_col;
		adjustCurpos();
		cursupdate();
		smsg("(%d of %d) %s%s: %s", qf_index, qf_count, msgp,
					qf_types(qf_ptr->qf_type, qf_ptr->qf_nr), qf_ptr->qf_text);

		if (!qf_marksset)		/* marks not set yet: try to find them for
									the errors in the curren file */
		{
			for (i = 0, qfp = qf_start; i < qf_count; ++i, qfp = qfp->qf_next)
				if (qfp->qf_fname != NUL && strcmp(qfp->qf_fname, qf_ptr->qf_fname) == 0 && qfp->qf_lnum <= line_count && qfp->qf_lnum > 0)
					qfp->qf_mark = nr2ptr(qfp->qf_lnum);
			qf_marksset = 1;
		}
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
#ifdef AMIGA
	settmode(0);		/* set cooked mode so output can be halted */
#endif
	for (i = 1; i <= qf_count; ++i)
	{
		sprintf(IObuff, "%2d line %3ld col %2d %s: %s",
			i,
			(long)qfp->qf_lnum,
			qfp->qf_col,
			qf_types(qfp->qf_type, qfp->qf_nr),
			qfp->qf_text);
		outstr(IObuff);
		outchar('\n');
		qfp = qfp->qf_next;
		flushbuf();
	}
#ifdef AMIGA
	settmode(1);
#endif
	wait_return(TRUE);
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
		free(qf_start->qf_fname);
		free(qf_start->qf_text);
		free(qf_start);
		qf_start = qfp;
		--qf_count;
	}
	qf_marksset = 0;
}

/*
 * qf_clrallmarks() - clear all marks
 *
 * Used mainly when trashing the entire buffer during ":e" type commands
 */
	void
qf_clrallmarks()
{
	int 			i;
	struct qf_line *qfp;

	if (qf_count)
		for (i = 0, qfp = qf_start; i < qf_count; i++, qfp = qfp->qf_next)
			qfp->qf_mark = NULL;
	qf_marksset = 0;
}

/*
 * qf_adjustmark: set new ptr for a mark
 */
   void
qf_adjustmark(old, new)
	char		*old, *new;
{
	register int i;
	struct qf_line *qfp;

	if (qf_count)
	{
		for (i = 0, qfp = qf_start; i < qf_count; ++i, qfp = qfp->qf_next)
			if (qfp->qf_mark == old)
			{
				qfp->qf_mark = new;
				if (new == NULL)
					qfp->qf_cleared = TRUE;
			}
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
	static char *
qf_types(c, nr)
	int c, nr;
{
	static char	buf[20];
	char		*p1;

	p1 = "  Error";
	if (c == 'W' || c == 'w')
		p1 =  "Warning";
	else if (nr <= 0 && c != 'E' && c != 'e')
		p1 = "";

	if (nr <= 0)
		return p1;

	sprintf(buf, "%s %3d", p1, nr);
	return buf;
}
