/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * buffer.c: functions for dealing with the buffer structure
 */

/*
 * The buffer list is a double linked list of all buffers.
 * Each buffer can be in one of these states:
 * never loaded: b_neverloaded == TRUE, only the file name is valid
 *   not loaded: b_ml.ml_mfp == NULL, no memfile allocated
 *       hidden: b_nwindows == 0, loaded but not displayed in a window
 *       normal: loaded and displayed in a window
 *
 * Instead of storing file names all over the place, each file name is
 * stored in the buffer list. It can be referenced by a number.
 *
 * The current implementation remembers all file names ever used.
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

static void		enter_buffer __ARGS((BUF *));
static BUF		*buflist_findname __ARGS((char_u *));
static BUF		*buflist_findnr __ARGS((int));
static void		buflist_setlnum __ARGS((BUF *, linenr_t));
static linenr_t buflist_findlnum __ARGS((BUF *));

/*
 * Open current buffer, that is: open the memfile and read the file into memory
 * return FAIL for failure, OK otherwise
 */
 	int
open_buffer()
{
	if (readonlymode && curbuf->b_filename != NULL)
		curbuf->b_p_ro = TRUE;
	if (ml_open() == FAIL)
	{
		/*
		 * There MUST be a memfile, otherwise we can't do anything
		 * If we can't create one for the current buffer, take another buffer
		 */
		close_buffer(curbuf, FALSE, FALSE);
		for (curbuf = firstbuf; curbuf != NULL; curbuf = curbuf->b_next)
			if (curbuf->b_ml.ml_mfp != NULL)
				break;
		/*
		 * if there is no memfile at all, exit
		 * This is OK, since there are no changes to loose.
		 */
		if (curbuf == NULL)
		{
			EMSG("Cannot allocate buffer, exiting...");
			getout(2);
		}
		EMSG("Cannot allocate buffer, using other one...");
		enter_buffer(curbuf);
		return FAIL;
	}
	if (curbuf->b_filename != NULL)
	{
		if (readfile(curbuf->b_filename, curbuf->b_sfilename, (linenr_t)0,
										TRUE, (linenr_t)0, MAXLNUM) == FAIL)
			return FAIL;
	}
	else
		MSG("Empty Buffer");
	UNCHANGED(curbuf);
	curbuf->b_neverloaded = FALSE;
	return OK;
}

/*
 * Close the link to a buffer. If "free_buf" is TRUE free the buffer if it
 * becomes unreferenced. The caller should get a new buffer very soon!
 * if 'remove' is TRUE, remove the buffer from the buffer list.
 */
	void
close_buffer(buf, free_buf, remove)
	BUF		*buf;
	int		free_buf;
	int		remove;
{
	if (buf->b_nwindows > 0)
		--buf->b_nwindows;
	if (buf->b_nwindows > 0 || !free_buf)
	{
		if (buf == curbuf)
			u_sync();		/* sync undo before going to another buffer */
		return;
	}

	buf_freeall(buf);		/* free all things allocated for this buffer */
	/*
	 * If there is no file name, remove the buffer from the list
	 */
	if (buf->b_filename == NULL || remove)
	{
		free(buf->b_filename);
		free(buf->b_sfilename);
		if (buf->b_prev == NULL)
			firstbuf = buf->b_next;
		else
			buf->b_prev->b_next = buf->b_next;
		if (buf->b_next == NULL)
			lastbuf = buf->b_prev;
		else
			buf->b_next->b_prev = buf->b_prev;
		free(buf);
	}
	else
		buf_clear(buf);
}

/*
 * buf_clear() - make buffer empty
 */
	void
buf_clear(buf)
	BUF		*buf;
{
	buf->b_ml.ml_line_count = 1;
	buf->b_changed = FALSE;
#ifndef MSDOS
	buf->b_shortname = FALSE;
#endif
	buf->b_p_eol = TRUE;
	buf->b_ml.ml_mfp = NULL;
	buf->b_ml.ml_flags = ML_EMPTY;					/* empty buffer */
}

/*
 * buf_freeall() - free all things allocated for the buffer
 */
	void
buf_freeall(buf)
	BUF		*buf;
{
	u_blockfree(buf);				/* free the memory allocated for undo */
	ml_close(buf);					/* close and delete the memline/memfile */
	buf->b_ml.ml_line_count = 0;	/* no lines in buffer */
	u_clearall(buf);				/* reset all undo information */
}

/*
 * Implementation of the command for the buffer list
 */
	int
do_buffer(action, start, dir, count, forceit)
	int		action;		/* 0 = normal, 1 = split window, 2 = unload, 3 = delete */
	int		start;		/* 0 = current, 1 = first, 2 = last, 3 = modified */
	int		dir;		/* FORWARD or BACKWARD */
	int		count;		/* buffer number or number of buffers */
	int		forceit;	/* TRUE for :bdelete! */
{
	BUF		*buf;
	int		retval;

	switch (start)
	{
	case 0: buf = curbuf;
			break;
	case 1: buf = firstbuf;
			break;
	case 2: buf = curbuf;
			while (buf->b_next != NULL)
				buf = buf->b_next;
			break;
	default: buf = curbuf;
			break;
	}
	if (start == 3)			/* find next modified buffer */
	{
		while (count-- > 0)
		{
			do
			{
				buf = buf->b_next;
				if (buf == NULL)
					buf = firstbuf;
			}
			while (buf != curbuf && !buf->b_changed);
		}
		if (!buf->b_changed)
		{
			EMSG("No modified buffer found");
			return FAIL;
		}
	}
	else if (start == 1 && count)		/* find specified buffer number */
	{
		while (buf != NULL && buf->b_fnum != count)
			buf = buf->b_next;
	}
	else
	{
		while (buf != NULL && count-- > 0)
		{
			if (dir == FORWARD)
				buf = buf->b_next;
			else
				buf = buf->b_prev;
		}
	}
	if (buf == NULL)		/* could not find it */
	{
		if (start == 1)
			EMSG2("Cannot go to buffer %ld", (char_u *)count);
		else if (dir == FORWARD)
			EMSG("Cannot go beyond last buffer");
		else
			EMSG("Cannot go before first buffer");
		return FAIL;
	}
	/*
	 * delete buffer buf from memory and/or the list
	 */
	if (action == 2 || action == 3)
	{
		if (buf->b_nwindows > 1 || (buf != curbuf && buf->b_nwindows != 0))
		{
			EMSG2("Other window editing buffer %ld", (char_u *)buf->b_fnum);
			return FAIL;
		}
		if (!forceit && buf->b_changed)
		{
			EMSG2("No write since last change for buffer %ld (use ! to override)",
						(char_u *)buf->b_fnum);
			return FAIL;
		}
		/*
		 * if deleting last buffer, make it empty
		 */
		if (firstbuf->b_next == NULL)
		{
			buf = curbuf;
			retval = doecmd(NULL, NULL, NULL, FALSE, (linenr_t)1);
				/* the doecmd() may create a new buffer, then we have to
				 * delete the old one */
			if (action == 3 && buf != curbuf)
				close_buffer(buf, TRUE, action == 3);
			return retval;
		}
		/*
		 * If deleted buffer is not current one, delete it here.
		 * Otherwise find buffer to go to and delete it below.
		 */
		{
			if (buf != curbuf)
			{
				close_buffer(buf, TRUE, action == 3);
				return OK;
			}
			if (buf->b_next != NULL)
				buf = buf->b_next;
			else
				buf = buf->b_prev;
		}
	}
/*
 * make buf current buffer
 */
	if (action == 1)		/* split window first */
	{
		if (win_split(0L, FALSE) == FAIL)
			return FAIL;
	}
	buflist_altlnum();		/* remember curpos.lnum */
	close_buffer(curbuf, action == 2 || action == 3, action == 3);
	enter_buffer(buf);
	return OK;
}

/*
 * enter a new current buffer.
 * (old curbuf must have been freed already)
 */
	static void
enter_buffer(buf)
	BUF		*buf;
{
	int		need_fileinfo = TRUE;

	if (buf->b_neverloaded)
	{
		buf_copy_options(curbuf, buf);
		buf->b_neverloaded = FALSE;
	}
	curwin->w_buffer = buf;
	curbuf = buf;
	++curbuf->b_nwindows;
	if (curbuf->b_ml.ml_mfp == NULL)	/* need to load the file */
	{
		open_buffer();
		need_fileinfo = FALSE;
	}
	buflist_getlnum();					/* restore curpos.lnum */
	maketitle();
	updateScreen(NOT_VALID);
	if (need_fileinfo)
		fileinfo(did_cd);
}

/*
 * functions for dealing with the buffer list
 */

/*
 * Add a file name to the buffer list. Return a pointer to the buffer.
 * If the same file name already exists return a pointer to that buffer.
 * If it does not exist, or if fname == NULL, a new entry is created.
 * If use_curbuf is TRUE, may use current buffer.
 * This is the ONLY way to create a new buffer.
 */
	BUF *
buflist_new(fname, sfname, lnum, use_curbuf)
	char_u		*fname;
	char_u		*sfname;
	linenr_t	lnum;
	int			use_curbuf;
{
	static int	top_file_num = 1;			/* highest file number */
	BUF			*buf;

	fname_expand(&fname, &sfname);

/*
 * If file name already exists in the list, update the entry
 */
	if (fname != NULL && (buf = buflist_findname(fname)) != NULL)
	{
		if (lnum != 0)
			buflist_setlnum(buf, lnum);
		if (buf->b_neverloaded && curbuf != NULL && buf != curbuf)
			buf_copy_options(curbuf, buf);
		return buf;
	}

/*
 * If the current buffer has no name and no contents, use the current buffer.
 * Otherwise: Need to allocate a new buffer structure.
 *
 * This is the ONLY place where a new buffer structure is allocated!
 */
	if (use_curbuf && curbuf != NULL && curbuf->b_filename == NULL &&
				curbuf->b_nwindows <= 1 &&
				(curbuf->b_ml.ml_mfp == NULL || curbuf->b_ml.ml_flags == ML_EMPTY))
		buf = curbuf;
	else
	{
		buf = (BUF *)alloc((unsigned)sizeof(BUF));
		if (buf == NULL)
			return NULL;
		memset((char *)buf, 0, sizeof(BUF));
	}

	if (fname != NULL)
	{
		buf->b_filename = strsave(fname);
		buf->b_sfilename = strsave(sfname);
	}
	if (buf->b_winlnum == NULL)
		buf->b_winlnum = (WINLNUM *)alloc((unsigned)sizeof(WINLNUM));
	if ((fname != NULL && (buf->b_filename == NULL || buf->b_sfilename == NULL)) ||
								buf->b_winlnum == NULL)
	{
		free(buf->b_filename);
		buf->b_filename = NULL;
		free(buf->b_sfilename);
		buf->b_sfilename = NULL;
		if (buf != curbuf)
		{
			free(buf->b_winlnum);
			free(buf);
		}
		return NULL;
	}

	if (buf == curbuf)
	{
		buf_freeall(buf);		/* free all things allocated for this buffer */
		buf->b_nwindows = 0;
	}
	else
	{
		if (curbuf != NULL)		/* don't do this for first buffer */
			buf_copy_options(curbuf, buf);
	
		/*
		 * put new buffer at the end of the buffer list
		 */
		buf->b_next = NULL;
		if (firstbuf == NULL)			/* buffer list is empty */
		{
			buf->b_prev = NULL;
			firstbuf = buf;
		}
		else							/* append new buffer at end of the list */
		{
			lastbuf->b_next = buf;
			buf->b_prev = lastbuf;
		}
		lastbuf = buf;

		buf->b_fnum = top_file_num++;
		if (top_file_num < 0)			/* wrap around (may cause duplicates) */
		{
			EMSG("Warning: List of file names overflow");
			sleep(3);					/* make sure it is noticed */
			top_file_num = 1;
		}

		buf->b_winlnum->wl_lnum = lnum;
		buf->b_winlnum->wl_next = NULL;
		buf->b_winlnum->wl_prev = NULL;
		buf->b_winlnum->wl_win = curwin;
	}

	if (did_cd)
		buf->b_xfilename = buf->b_filename;
	else
		buf->b_xfilename = buf->b_sfilename;
	buf->b_u_synced = TRUE;
	buf->b_neverloaded = TRUE;
	buf_clear(buf);
	clrallmarks(buf);				/* clear marks */

	return buf;
}

/*
 * get alternate file n
 * set linenr to lnum or altlnum if lnum == 0
 * if (setpm) setpcmark
 * return FAIL for failure, OK for success
 */
	int
buflist_getfile(n, lnum, setpm)
	int			n;
	linenr_t	lnum;
	int			setpm;
{
	BUF		*buf;

	buf = buflist_findnr(n);
	if (buf == NULL)
	{
		emsg(e_noalt);
		return FAIL;
	}
	if (lnum == 0)
		lnum = buflist_findlnum(buf);	/* altlnum may be changed by getfile() */
	RedrawingDisabled = TRUE;
	if (getfile(buf->b_filename, buf->b_sfilename, setpm, lnum) <= 0)
	{
		RedrawingDisabled = FALSE;
		return OK;
	}
	RedrawingDisabled = FALSE;
	return FAIL;
}

/*
 * go to the last know line number for the current buffer
 */
	void
buflist_getlnum()
{
	linenr_t	lnum;

	curwin->w_cursor.lnum = 1;
	curwin->w_cursor.col = 0;
	lnum = buflist_findlnum(curbuf);
	if (lnum != 0 && lnum <= curbuf->b_ml.ml_line_count)
		curwin->w_cursor.lnum = lnum;
}

/*
 * find file in buffer list by name (it has to be for the current window)
 * 'fname' must have a full path.
 */
	static BUF	*
buflist_findname(fname)
	char_u		*fname;
{
	BUF			*buf;

	for (buf = firstbuf; buf != NULL; buf = buf->b_next)
		if (buf->b_filename != NULL && fnamecmp(fname, buf->b_filename) == 0)
			return (buf);
	return NULL;
}

/*
 * find file in buffer name list by number
 */
	static BUF	*
buflist_findnr(nr)
	int			nr;
{
	BUF			*buf;

	if (nr == 0)
		nr = curwin->w_alt_fnum;
	for (buf = firstbuf; buf != NULL; buf = buf->b_next)
		if (buf->b_fnum == nr)
			return (buf);
	return NULL;
}

/*
 * get name of file 'n' in the buffer list
 */
 	char_u *
buflist_nr2name(n)
	int n;
{
	BUF		*buf;
	char_u	*fname;

	buf = buflist_findnr(n);
	if (buf == NULL)
		return NULL;
	fname = did_cd ? buf->b_filename : buf->b_sfilename;
	home_replace(fname, NameBuff, MAXPATHL);
	return NameBuff;
}

/*
 * set the lnum for the buffer 'buf' and the current window
 */
	static void
buflist_setlnum(buf, lnum)
	BUF			*buf;
	linenr_t	lnum;
{
	WINLNUM		*wlp;
	
	for (wlp = buf->b_winlnum; wlp != NULL; wlp = wlp->wl_next)
		if (wlp->wl_win == curwin)
			break;
	if (wlp == NULL)			/* make new entry */
	{
		wlp = (WINLNUM *)alloc((unsigned)sizeof(WINLNUM));
		if (wlp == NULL)
			return;
		wlp->wl_win = curwin;
	}
	else						/* remove entry from list */
	{
		if (wlp->wl_prev)
			wlp->wl_prev->wl_next = wlp->wl_next;
		else
			buf->b_winlnum = wlp->wl_next;
		if (wlp->wl_next)
			wlp->wl_next->wl_prev = wlp->wl_prev;
	}
	wlp->wl_lnum = lnum;
/*
 * insert entry in front of the list
 */
	wlp->wl_next = buf->b_winlnum;
	buf->b_winlnum = wlp;
	wlp->wl_prev = NULL;
	if (wlp->wl_next)
		wlp->wl_next->wl_prev = wlp;

	return;
}

/*
 * find the lnum for the buffer 'buf' for the current window
 */
	static linenr_t
buflist_findlnum(buf)
	BUF		*buf;
{
	WINLNUM 	*wlp;

	for (wlp = buf->b_winlnum; wlp != NULL; wlp = wlp->wl_next)
		if (wlp->wl_win == curwin)
			break;

	if (wlp == NULL)		/* if no lnum for curwin, use the first in the list */
		wlp = buf->b_winlnum;

	if (wlp)
		return wlp->wl_lnum;
	else
		return (linenr_t)1;
}

/*
 * list all know file names (for :files and :buffers command)
 */
	void
buflist_list()
{
	BUF			*buf;
	int			len;

	gotocmdline(TRUE, NUL);
	for (buf = firstbuf; buf != NULL && !got_int; buf = buf->b_next)
	{
		if (buf != firstbuf)
			msg_outchar('\n');
		if (buf->b_xfilename == NULL)
			STRCPY(NameBuff, "No File");
		else
			/* careful: home_replace calls vimgetenv(), which uses IObuff! */
			home_replace(buf->b_xfilename, NameBuff, MAXPATHL);

		sprintf((char *)IObuff, "%3d %c%c%c \"",
				buf->b_fnum,
				buf == curbuf ? '%' :
						(curwin->w_alt_fnum == buf->b_fnum ? '#' : ' '),
				buf->b_ml.ml_mfp == NULL ? '-' :
						(buf->b_nwindows == 0 ? 'h' : ' '),
				buf->b_changed ? '+' : ' ');

		len = STRLEN(IObuff);
		STRNCPY(IObuff + len, NameBuff, (size_t)IOSIZE - 20 - len);

		len = STRLEN(IObuff);
		IObuff[len++] = '"';
		/*
		 * try to put the "line" strings in column 40
		 */
		do
		{
			IObuff[len++] = ' ';
		} while (len < 40 && len < IOSIZE - 18);
		sprintf((char *)IObuff + len, "line %ld",
				buf == curbuf ? curwin->w_cursor.lnum :
								(long)buflist_findlnum(buf));
		msg_outstr(IObuff);
		flushbuf();			/* output one line at a time */
		breakcheck();
	}
	msg_end();
}

/*
 * get file name and line number for file 'fnum'
 * used by DoOneCmd() for translating '%' and '#'
 * return FAIL if not found, OK for success
 */
	int
buflist_name_nr(fnum, fname, lnum)
	int			fnum;
	char_u		**fname;
	linenr_t	*lnum;
{
	BUF			*buf;

	buf = buflist_findnr(fnum);
	if (buf == NULL || buf->b_filename == NULL)
		return FAIL;

	if (did_cd)
		*fname = buf->b_filename;
	else
		*fname = buf->b_sfilename;
	*lnum = buflist_findlnum(buf);

	return OK;
}

/*
 * Set the current file name to 's', short file name to 'ss'.
 * The file name with the full path is also remembered, for when :cd is used.
 * Returns FAIL for failure (file name already in use by other buffer)
 * 		OK otherwise.
 */
	int
setfname(fname, sfname, message)
	char_u *fname, *sfname;
	int		message;
{
	BUF		*buf;

	if (fname == NULL || *fname == NUL)
	{
		curbuf->b_filename = NULL;
		curbuf->b_sfilename = NULL;
	}
	else
	{
		fname_expand(&fname, &sfname);
		/*
		 * if the file name is already used in another buffer:
		 * - if the buffer is loaded, fail
		 * - if the buffer is not loaded, delete it from the list
		 */
		buf = buflist_findname(fname);
		if (buf != NULL && buf != curbuf)
		{
			if (buf->b_ml.ml_mfp != NULL)		/* it's loaded, fail */
			{
				if (message)
					EMSG("Buffer with this name already exists");
				return FAIL;
			}
			close_buffer(buf, TRUE, TRUE);		/* delete from the list */
		}
		fname = strsave(fname);
		sfname = strsave(sfname);
		if (fname == NULL || sfname == NULL)
		{
			free(sfname);
			free(fname);
			return FAIL;
		}
		free(curbuf->b_filename);
		free(curbuf->b_sfilename);
		curbuf->b_filename = fname;
		curbuf->b_sfilename = sfname;
	}
	if (did_cd)
		curbuf->b_xfilename = curbuf->b_filename;
	else
		curbuf->b_xfilename = curbuf->b_sfilename;

#ifndef MSDOS
	curbuf->b_shortname = FALSE;
#endif
	return OK;
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
	BUF		*buf;

	buf = buflist_new(fname, sfname, lnum, FALSE);
	if (buf != NULL)
		curwin->w_alt_fnum = buf->b_fnum;
}

/*
 * add a file name to the buflist and return its number
 *
 * used by qf_init(), main() and doarglist()
 */
	int
buflist_add(fname)
	char_u		*fname;
{
	BUF		*buf;

	buf = buflist_new(fname, NULL, (linenr_t)0, FALSE);
	if (buf != NULL)
		return buf->b_fnum;
	return 0;
}

/*
 * set alternate lnum for current window
 */
	void
buflist_altlnum()
{
	buflist_setlnum(curbuf, curwin->w_cursor.lnum);
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

	void
fileinfo(fullname)
	int fullname;
{
	char_u		*name;

#if 0		/* this message is quite useless */
	if (bufempty())
	{
		MSG("Buffer Empty");
		return;
	}
#endif

	if (curbuf->b_filename == NULL)
		STRCPY(IObuff, "\"No File");
	else
	{
		if (!fullname && curbuf->b_sfilename != NULL)
			name = curbuf->b_sfilename;
		else
			name = curbuf->b_filename;
			/* careful: home_replace cals vimgetenv(), which also uses IObuff! */
		home_replace(name, IObuff + 1, IOSIZE - 1);
		IObuff[0] = '"';
	}

	sprintf((char *)IObuff + STRLEN(IObuff),
						"\"%s%s%s line %ld of %ld --%d%%-- col %d",
			curbuf->b_changed ? " [Modified]" : "",
			curbuf->b_notedited ? " [Not edited]" : "",
			curbuf->b_p_ro ? " [readonly]" : "",
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_line_count,
			(int)(((long)curwin->w_cursor.lnum * 100L) / (long)curbuf->b_ml.ml_line_count),
			(int)curwin->w_cursor.col + 1);

	if (arg_count > 1)
		sprintf((char *)IObuff + STRLEN(IObuff), " (file %d of %d)", curwin->w_arg_idx + 1, arg_count);
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
		home_replace(curbuf->b_filename, IObuff, IOSIZE);
		if (arg_count > 1)
			sprintf((char *)IObuff + STRLEN(IObuff), " (%d of %d)", curwin->w_arg_idx + 1, arg_count);
		t = IObuff;
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

/*
 * If fname is not a full path, make it a full path
 */
	char_u	*
fix_fname(fname)
	char_u	*fname;
{
	if (fname != NameBuff)			/* if not already expanded */
	{
		if (!isFullName(fname))
		{
			(void)FullName(fname, NameBuff, MAXPATHL);
			fname = NameBuff;
		}
#ifdef AMIGA
		else
		{
			STRNCPY(NameBuff, fname, (size_t)MAXPATHL);	/* make copy so we can change it */
			fname = NameBuff;
			fname_case(fname);			/* set correct case for filename */
		}
#endif
	}
	return fname;
}

/*
 * make fname a full file name, set sfname to fname if not NULL
 */
	void
fname_expand(fname, sfname)
	char_u		**fname;
	char_u		**sfname;
{
	if (*fname == NULL)			/* if no file name given, nothing to do */
		return;
	if (*sfname == NULL)		/* if no short file name given, use fname */
		*sfname = *fname;
	*fname = fix_fname(*fname);	/* expand to full path */
}

/*
 * do_arg_all: open a window for each argument
 */
	void
do_arg_all()
{
	int		win_count;
	int		i;

	if (arg_count <= 1)
	{
		EMSG("Argument list contains less than 2 files");
		return;
	}
	/*
	 * 1. close all but first window
	 * 2. make the desired number of windows
	 * 3. start editing the first window (hide the current window contents)
	 * 4. stuff commands to fill the other windows
	 */
	close_others(FALSE);
	curwin->w_arg_idx = 0;
	win_count = make_windows(arg_count);
	for (i = 0; i < win_count; ++i)
	{
												/* edit file i */
		(void)doecmd(arg_files[i], NULL, NULL, TRUE, (linenr_t)1);
		curwin->w_arg_idx = i;
		if (curwin->w_next == NULL)				/* just checking */
			break;
		win_enter(curwin->w_next, FALSE);
	}
	win_enter(firstwin, FALSE);					/* back to first window */
}

/*
 * do_arg_all: open a window for each buffer
 *
 * when 'all' is TRUE, also load inactive buffers
 */
	void
do_buffer_all(all)
	int		all;
{
	int		win_count;
	BUF		*buf;
	int		i;

/*
 * count number of desired windows
 */
	win_count = 0; 
	for (buf = firstbuf; buf != NULL; buf = buf->b_next)
		if (all || buf->b_ml.ml_mfp != NULL)
			++win_count;

	if (win_count == 0)				/* Cannot happen? */
	{
		EMSG("No relevant entries in buffer list");
		return;
	}

	/*
	 * 1. close all but first window
	 * 2. make the desired number of windows
	 * 3. stuff commands to fill the windows
	 */
	close_others(FALSE);
	curwin->w_arg_idx = 0;
	win_count = make_windows(win_count);
	buf = firstbuf;
	for (i = 0; i < win_count; ++i)
	{
		for ( ; buf != NULL; buf = buf->b_next)
			if (all || buf->b_ml.ml_mfp != NULL)
				break;
		if (buf == NULL)			/* Cannot happen? */
			break;
		if (i != 0)
			stuffReadbuff((char_u *)"\n\027\027:");	/* CTRL-W CTRL-W */
		stuffReadbuff((char_u *)":buf ");			/* edit Nth buffer */
		stuffnumReadbuff((long)buf->b_fnum);
		buf = buf->b_next;
	}
	stuffReadbuff((char_u *)"\n100\027k");		/* back to first window */
}
