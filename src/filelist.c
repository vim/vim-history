/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * filelist.c: functions for dealing with file names
 *
 * The largest part is dealing with the file name list.
 *
 * Instead of storing file names all over the place, each file name is
 * stored in the file name list. It can be referenced by a number.
 *
 * The current implementation remembers all file names ever used.
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"
#include "fcntl.h"			/* for chdir() */

typedef struct fileentry	FIL;
typedef struct filelnum		FLNUM;

struct fileentry
{
	FIL			*f_next;
	FIL			*f_prev;
	char_u		*f_fname;		/* full name of the file */
	char_u		*f_sfname;		/* short name of the file */
	int			f_fnum;			/* entry number */
	FLNUM		*f_flnum;		/* list of last used lnum for each window */
};

static FIL		*filelist_first = NULL;		/* first file name entry */

/*
 * Each window can have a different line number associated with a file name.
 * The window-pointer/line-number pairs are kept in the line number list.
 * The list of line numbers is kept in most-recently-used order.
 */
struct filelnum
{
	FLNUM		*fl_next;
	FLNUM		*fl_prev;
	WIN			*fl_win;			/* pointer to window that did set f_lnum */
	linenr_t	fl_lnum;			/* last used line in the file */
};

static FIL		*filelist_new __ARGS((char_u *, char_u *, linenr_t));
static FIL		*filelist_findname __ARGS((char_u *));
static FIL		*filelist_findnr __ARGS((int));
static void		filelist_setlnum __ARGS((FIL *, linenr_t));
static linenr_t filelist_findlnum __ARGS((FIL *));

/*
 * Add a file name to the file name list. Return the number of the entry.
 * If the same file already exists return the number of that entry.
 */
	static FIL *
filelist_new(fname, sfname, lnum)
	char_u		*fname;
	char_u		*sfname;
	linenr_t	lnum;
{
	static int	top_file_num = 1;		/* highest file number */
	FIL			*fp;

	if (sfname == NULL)		/* no short file name given, use fname */
		sfname = fname;
	if (fname != IObuff)	/* if not expanded already */
	{
		(void)FullName(fname, IObuff, IOSIZE);
		fname = IObuff;
	}
/*
 * If file name already exists in the list, update the entry
 */
	if ((fp = filelist_findname(fname)) != NULL)
	{
		if (lnum != 0)
			filelist_setlnum(fp, lnum);
		return fp;
	}

	fp = (FIL *)alloc((unsigned)sizeof(FIL));
	if (fp == NULL)
		return NULL;
	
	fp->f_fname = strsave(fname);
	fp->f_sfname = strsave(sfname);
	fp->f_flnum = (FLNUM *)alloc((unsigned)sizeof(FLNUM));
	if (fp->f_fname == NULL || fp->f_sfname == NULL || fp->f_flnum == NULL)
	{
		free(fp->f_fname);
		free(fp->f_sfname);
		free(fp->f_flnum);
		free(fp);
		return NULL;
	}
	fp->f_fnum = top_file_num++;
	if (top_file_num < 0)			/* wrap around (may cause duplicates) */
	{
		EMSG("Warning: List of file names overflow");
		top_file_num = 1;
	}
	fp->f_flnum->fl_lnum = lnum;
	fp->f_flnum->fl_next = NULL;
	fp->f_flnum->fl_prev = NULL;
	fp->f_flnum->fl_win = curwin;

	fp->f_next = filelist_first;		/* link the new entry in front of the list */
	if (filelist_first)
		filelist_first->f_prev = fp;
	filelist_first = fp;
	fp->f_prev = NULL;

	return fp;
}

/*
 * get alternate file n
 * set linenr to lnum or altlnum if lnum == 0
 * if (setpm) setpcmark
 * return FAIL for failure, OK for success
 */
	int
filelist_getfile(n, lnum, setpm)
	int			n;
	linenr_t	lnum;
	int			setpm;
{
	FIL		*fp;

	fp = filelist_findnr(n);
	if (fp == NULL)
	{
		emsg(e_noalt);
		return FAIL;
	}
	if (lnum == 0)
		lnum = filelist_findlnum(fp);		/* altlnum may be changed by getfile() */
	RedrawingDisabled = TRUE;
	if (getfile(fp->f_fname, fp->f_sfname, setpm) <= 0)
	{
		RedrawingDisabled = FALSE;
		if (lnum == 0 || lnum > curbuf->b_ml.ml_line_count)		/* check for valid lnum */
			curwin->w_cursor.lnum = 1;
		else
			curwin->w_cursor.lnum = lnum;

		curwin->w_cursor.col = 0;
		return OK;
	}
	RedrawingDisabled = FALSE;
	return FAIL;
}

/*
 * go to the last know line number for the current buffer
 */
	void
filelist_getlnum()
{
	FIL			*fp;
	linenr_t	lnum;

	curwin->w_cursor.lnum = 1;
	curwin->w_cursor.col = 0;
	fp = filelist_findnr(curbuf->b_fnum);
	if (fp != NULL)
	{
		lnum = filelist_findlnum(fp);
		if (lnum != 0 && lnum <= curbuf->b_ml.ml_line_count)
			curwin->w_cursor.lnum = lnum;
	}
}

/*
 * find file in file list by name (it has to be for the current window)
 */
	static FIL	*
filelist_findname(fname)
	char_u		*fname;
{
	FIL			*fp;

	for (fp = filelist_first; fp; fp = fp->f_next)
		if (fnamecmp(fname, fp->f_fname) == 0)
			return (fp);
	return NULL;
}

/*
 * find file in file name list by number
 */
	static FIL	*
filelist_findnr(nr)
	int			nr;
{
	FIL			*fp;

	if (nr == 0)
		nr = curwin->w_alt_fnum;
	for (fp = filelist_first; fp; fp = fp->f_next)
		if (fp->f_fnum == nr)
			return (fp);
	return NULL;
}

/*
 * get name of file 'n' in the file list
 */
 	char_u *
filelist_nr2name(n)
	int n;
{
	FIL		*fp;

	fp = filelist_findnr(n);
	if (fp == NULL)
		return NULL;
	return fp->f_fname;
}

/*
 * set the lnum for the file 'fp' and the current window
 */
	static void
filelist_setlnum(fp, lnum)
	FIL			*fp;
	linenr_t	lnum;
{
	FLNUM		*flp;
	
	for (flp = fp->f_flnum; flp; flp = flp->fl_next)
		if (flp->fl_win == curwin)
			break;
	if (flp == NULL)			/* make new entry */
	{
		flp = (FLNUM *)alloc((unsigned)sizeof(FLNUM));
		if (flp == NULL)
			return;
		flp->fl_win = curwin;
	}
	else						/* remove entry from list */
	{
		if (flp->fl_prev)
			flp->fl_prev->fl_next = flp->fl_next;
		else
			fp->f_flnum = flp->fl_next;
		if (flp->fl_next)
			flp->fl_next->fl_prev = flp->fl_prev;
	}
	flp->fl_lnum = lnum;
/*
 * insert entry in front of the list
 */
	flp->fl_next = fp->f_flnum;
	fp->f_flnum = flp;
	flp->fl_prev = NULL;
	if (flp->fl_next)
		flp->fl_next->fl_prev = flp;

	return;
}

/*
 * find the lnum for the entry 'fp' for the current window
 */
	static linenr_t
filelist_findlnum(fp)
	FIL		*fp;
{
	FLNUM 	*flp;

	for (flp = fp->f_flnum; flp; flp = flp->fl_next)
		if (flp->fl_win == curwin)
			break;

	if (flp == NULL)		/* if no lnum for curwin, use the first in the list */
		flp = fp->f_flnum;

	if (flp)
		return flp->fl_lnum;
	else
		return (linenr_t)1;
}

/*
 * list all know file names (for :files command)
 */
	void
filelist_list()
{
	FIL			*fp;

	gotocmdline(TRUE, NUL);
	mch_start_listing();		/* may set cooked mode, so output can be halted */
	for (fp = filelist_first; fp; fp = fp->f_next)
	{
		sprintf((char *)IObuff, "%3d \"%s\" line %ld\n", fp->f_fnum,
				did_cd ? fp->f_fname : fp->f_sfname,
				(long)filelist_findlnum(fp));
		msg_outstr(IObuff);
		flushbuf();			/* output one line at a time */
	}
	mch_stop_listing();
	wait_return(FALSE);
}

/*
 * get file name and line number for file 'fnum'
 * used by DoOneCmd() for translating '%' and '#'
 * return FAIL if not found, OK for success
 */
	int
filelist_name_nr(fnum, fname, lnum)
	int			fnum;
	char_u		**fname;
	linenr_t	*lnum;
{
	FIL			*fp;

	fp = filelist_findnr(fnum);
	if (fp == NULL)
		return FAIL;

	if (did_cd)
		*fname = fp->f_fname;
	else
		*fname = fp->f_sfname;
	*lnum = filelist_findlnum(fp);

	return OK;
}

/*
 * Set the current file name to 's', short file name to 'ss'.
 * The file name with the full path is also remembered, for when :cd is used.
 */
	void
setfname(s, ss)
	char_u *s, *ss;
{
	FIL		*fp;

	if (s == NULL || *s == NUL)
	{
		curbuf->b_filename = NULL;
		curbuf->b_sfilename = NULL;
	}
	else
	{
		fp = filelist_new(s, ss, (linenr_t)1);
		if (fp != NULL)
		{
			curbuf->b_filename = fp->f_fname;
			curbuf->b_sfilename = fp->f_sfname;
			curbuf->b_fnum = fp->f_fnum;
		}
	}
	if (did_cd)
		curbuf->b_xfilename = curbuf->b_filename;
	else
		curbuf->b_xfilename = curbuf->b_sfilename;

#ifndef MSDOS
	curbuf->b_shortname = FALSE;
#endif
}

/*
 * set alternate file name for current window
 *
 * used by dowrite() and doecmd()
 */
	void
setaltfname(fname, sfname, lnum)
	char_u		*fname;
	char_u		*sfname;
	linenr_t	lnum;
{
	FIL		*fp;

	fp = filelist_new(fname, sfname, lnum);
	if (fp != NULL)
		curwin->w_alt_fnum = fp->f_fnum;
}

/*
 * add a file name to the filelist and return its number
 *
 * used by qf_init()
 */
	int
filelist_add(fname)
	char_u		*fname;
{
	FIL		*fp;

	fp = filelist_new(fname, NULL, (linenr_t)0);
	if (fp != NULL)
		return fp->f_fnum;
	return 0;
}

/*
 * set alternate lnum for current window
 */
	void
filelist_altlnum()
{
	FIL		*fp;

	fp = filelist_findnr(curbuf->b_fnum);
	if (fp != NULL)
		filelist_setlnum(fp, curwin->w_cursor.lnum);
}

/*
 * return nonzero if 'fname' is not the same file as current file
 * fname must have a full path (expanded by FullName)
 */
	int
otherfile(fname)
	char_u	*fname;
{									/* no name is different */
	if (fname == NULL || *fname == NUL || curbuf->b_filename == NULL)
		return TRUE;
	return fnamecmp(fname, curbuf->b_filename);
}

/*
 * If fname is not a full path, make it a full path
 */
	char_u	*
fix_fname(fname)
	char_u	*fname;
{
	if (fname != IObuff)			/* if not already expanded */
	{
#ifdef UNIX
		if (*fname != '/')
#else
		if (STRCHR(fname, ':') == NULL)
#endif
		{
			(void)FullName(fname, IObuff, IOSIZE);
			fname = IObuff;
		}
#ifdef AMIGA
		else
			fname_case(fname);			/* set correct case for filename */
#endif
	}
	return fname;
}

	void
fileinfo(fullname)
	int fullname;
{
#if 0		/* this message is quite useless */
	if (bufempty())
	{
		MSG("Buffer Empty");
		return;
	}
#endif

	sprintf((char *)IObuff, "\"%s\"%s%s%s line %ld of %ld --%d%%-- col %d",
			(!fullname && curbuf->b_sfilename != NULL) ? (char *)curbuf->b_sfilename :
				((curbuf->b_filename != NULL) ? (char *)curbuf->b_filename : "No File"),
			curbuf->b_changed ? " [Modified]" : "",
			curbuf->b_notedited ? " [Not edited]" : "",
			curbuf->b_p_ro ? " [readonly]" : "",
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_line_count,
			(int)(((long)curwin->w_cursor.lnum * 100L) / (long)curbuf->b_ml.ml_line_count),
			(int)curwin->w_cursor.col + 1);

	if (arg_count > 1)
		sprintf((char *)IObuff + STRLEN(IObuff), " (file %d of %d)", arg_current + 1, arg_count);
	msg(IObuff);
}

/*
 * put filename in title bar of window and in icon title
 */

static char_u *lasttitle = NULL;
static char_u *lasticon = NULL;

	void
maketitle()
{
	char_u		*t;
	char_u		*i;

	if (!p_title && !p_icon)
		return;

	if (curbuf->b_filename == NULL)
	{
		t = (char_u *)"";
		i = (char_u *)"No File";
	}
	else
	{
		if (arg_count <= 1)
			t = curbuf->b_filename;
		else
		{
			sprintf((char *)IObuff, "%s (%d of %d)", curbuf->b_filename, arg_current + 1, arg_count);
			t = IObuff;
		}
		i = gettail(curbuf->b_filename);		/* use filename only for icon */
	}

	free(lasttitle);
	if (p_title)
		lasttitle = alloc((unsigned)(STRLEN(t) + 7));
	else
		lasttitle = NULL;
	if (lasttitle != NULL)
		sprintf((char *)lasttitle, "VIM - %s", (char *)t);

	free(lasticon);
	if (p_icon)
		lasticon = strsave(i);
	else
		lasticon = NULL;

	resettitle();
}

	void
resettitle()
{
	mch_settitle(lasttitle, lasticon);
}
