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
 * quickfix.c: functions for quickfix mode, using the Manx errorfile
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"

static void qf_free __ARGS((void));

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
	char			 qf_cleared;/* set to TRUE if qf_mark has been cleared */
	char			 qf_type;	/* type of the error (mostly 'E') */
	int				 qf_nr;		/* error number */
	char			*qf_fname;	/* file name where the error occurred */
	char			*qf_text;	/* description of the error */
};

static struct qf_line *qf_start;		/* pointer to the first error */
static struct qf_line *qf_ptr;			/* pointer to the current error */

static int	qf_count = 0;		/* number of errors (0 means no error list) */
	   int	qf_index;			/* current index in the error list */
static int	qf_marksset;		/* set to 1 when qf_mark-s have been set */

/*
 * Read the errorfile into memory, line by line, building the error list.
 * Return 1 for error, 0 for success.
 */
	int
qf_init(fname)
	char *fname;
{
	char 			namebuf[CMDBUFFSIZE];
	char			errmsg[CMDBUFFSIZE];
	FILE			*fd;
	struct qf_line	*qfp = NULL;

	if (fname == NULL)
	{
		emsg(e_errorf);
		return 1;
	}
	if ((fd = fopen(fname, "r")) == NULL)
	{
		emsg(e_openerrf);
		return 1;
	}
	qf_free();

	while (fgets(IObuff, IOSIZE, fd) != NULL)
	{
		if ((qfp = (struct qf_line *)alloc((unsigned)sizeof(struct qf_line))) == NULL)
			goto error2;

	/* parse the line: "filename>linenr:colnr:type:number:text" */
		if (sscanf(IObuff, "%[^>]>%ld:%d:%c:%d:%[^\n]", namebuf,
				&qfp->qf_lnum, &qfp->qf_col, &qfp->qf_type,
								&qfp->qf_nr, errmsg) != 6)
			goto error;
		if ((qfp->qf_fname = strsave(namebuf)) == NULL)
			goto error1;
		if ((qfp->qf_text = strsave(errmsg)) == NULL)
		{
			free(qfp->qf_fname);
			goto error1;
		}
		if (qf_count == 0)		/* first element in the list */
		{
			qf_start = qfp;
			qfp->qf_prev = qfp;	/* first element points to itself */
		}
		else
		{
			qfp->qf_prev = qf_ptr;
			qf_ptr->qf_next = qfp;
		}
		qfp->qf_next = qfp;		/* last element points to itself */
		qfp->qf_mark = NULL;
		qfp->qf_cleared = FALSE;
		qf_ptr = qfp;
		++qf_count;
	}
	if (!ferror(fd))
	{
		qf_ptr = qf_start;
		qf_index = 1;
		fclose(fd);
		return 0;
	}
error:
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
	if (errornr == 0)
		errornr = qf_index;
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

	/*
	 * read the wanted file if needed, and check autowrite etc.
	 */
	if (getfile(qf_ptr->qf_fname, TRUE) <= 0)
	{
		/*
		 * use mark if possible, because the line number may be invalid
		 * after line inserts / deletes
		 */
		i = 0;
		msgp = "";
		if ((qf_ptr->qf_mark != NULL && (i = ptr2nr(qf_ptr->qf_mark, (linenr_t)0)) == 0) || qf_ptr->qf_cleared)
			msgp = "(line changed) ";
		if (i == 0)
			i = qf_ptr->qf_lnum;
		if (i > line_count)
			i = line_count;
		Curpos.lnum = i;
		Curpos.col = qf_ptr->qf_col;
		adjustCurpos();
		cursupdate();
		smsg("(%d of %d) %s%s %d: %s", qf_index, qf_count, msgp, qf_ptr->qf_type == 'E' ? "Error" : "Warning", qf_ptr->qf_nr, qf_ptr->qf_text);

		if (!qf_marksset)		/* marks not set yet: try to find them for
									the errors in the curren file */
		{
			for (i = 0, qfp = qf_start; i < qf_count; ++i, qfp = qfp->qf_next)
				if (strcmp(qfp->qf_fname, qf_ptr->qf_fname) == 0 && qfp->qf_lnum <= line_count)
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
	settmode(0);
	for (i = 1; i <= qf_count; ++i)
	{
		sprintf(IObuff, "%2d line %ld col %2d %s %3d: %s",
			i,
			(long)qfp->qf_lnum,
			qfp->qf_col,
			qfp->qf_type == 'E' ? "Error" : "Warning",
			qfp->qf_nr,
			qfp->qf_text);
		outstr(IObuff);
		outchar('\n');
		qfp = qfp->qf_next;
		flushbuf();
	}
	settmode(1);
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
