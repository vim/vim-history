/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * fileio.c: read from and write to a file
 */

#if defined(MSDOS) || defined(WIN32)
# include <io.h>	/* for lseek(), must be before vim.h */
#endif

#if defined __EMX__
# include <io.h>	/* for mktemp(), CJW 1997-12-03 */
#endif

#include "vim.h"

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif

#ifdef LATTICE
# include <proto/dos.h>	    /* for Lock() and UnLock() */
#endif

#define BUFSIZE	    8192	    /* size of normal write buffer */
#define SMBUFSIZE    256	    /* size of emergency write buffer */

#ifdef VIMINFO
static void check_marks_read __ARGS((void));
#endif
#ifdef UNIX
static void set_file_time __ARGS((char_u *fname, time_t atime, time_t mtime));
#endif
static void msg_add_fname __ARGS((BUF *, char_u *));
static int msg_add_fileformat __ARGS((int eol_type));
static void msg_add_lines __ARGS((int, long, long));
static void msg_add_eol __ARGS((void));
static int check_mtime __ARGS((BUF *buf, struct stat *s));
static int  write_buf __ARGS((int, char_u *, int));

static linenr_t	write_no_eol_lnum = 0;	/* non-zero lnum when last line of
					   next binary write should not have
					   an end-of-line */

    void
filemess(buf, name, s, attr)
    BUF		*buf;
    char_u	*name;
    char_u	*s;
    int		attr;
{
    msg_add_fname(buf, name);	    /* put file name in IObuff with quotes */
    STRCAT(IObuff, s);
    /*
     * For the first message may have to start a new line.
     * For further ones overwrite the previous one, reset msg_scroll before
     * calling filemess().
     */
    msg_start();
    msg_outtrans_attr(IObuff, attr);
    msg_clr_eos();
    out_flush();
}

/*
 * Read lines from file 'fname' into the buffer after line 'from'.
 *
 * 1. We allocate blocks with lalloc, as big as possible.
 * 2. Each block is filled with characters from the file with a single read().
 * 3. The lines are inserted in the buffer with ml_append().
 *
 * (caller must check that fname != NULL, unless READ_STDIN is used)
 *
 * lines_to_skip is the number of lines that must be skipped
 * lines_to_read is the number of lines that are appended
 * When not recovering lines_to_skip is 0 and lines_to_read MAXLNUM.
 *
 * flags:
 * READ_NEW	starting to edit a new buffer
 * READ_FILTER	reading filter output
 * READ_STDIN	read from stdin instead of a file
 *
 * return FAIL for failure, OK otherwise
 */
    int
readfile(fname, sfname, from, lines_to_skip, lines_to_read, flags)
    char_u	*fname;
    char_u	*sfname;
    linenr_t	from;
    linenr_t	lines_to_skip;
    linenr_t	lines_to_read;
    int		flags;
{
    int		fd;
    int		newfile = (flags & READ_NEW);
    int		check_readonly;
    int		filtering = (flags & READ_FILTER);
    int		read_stdin = (flags & READ_STDIN);
    char_u	c;
    linenr_t	lnum = from;
    char_u	*ptr = NULL;		/* pointer into read buffer */
    char_u	*buffer = NULL;		/* read buffer */
    char_u	*new_buffer = NULL;	/* init to shut up gcc */
    char_u	*line_start = NULL;	/* init to shut up gcc */
    colnr_t	len;
    long	size;
    char_u	*p;
    long	filesize;
    int		split = 0;		/* number of split lines */
#define UNKNOWN	 0x0fffffff		/* file size is unknown */
    linenr_t	linecnt;
    int		error = FALSE;		/* errors encountered */
    int		ff_error = EOL_UNKNOWN; /* file format with errors */
    long	linerest;		/* remaining chars in line */
#ifdef UNIX
    int		perm = 0;
#endif
    int		fileformat;		/* end-of-line format */
    struct stat	st;
    int		file_readonly;
    linenr_t	skip_count;
    linenr_t	read_count;
    int		msg_save = msg_scroll;
    linenr_t	read_no_eol_lnum = 0;   /* non-zero lnum when last line of
					 * last read was missing the eol */
    int		try_mac = (vim_strchr(p_ffs, 'm') != NULL);
    int		try_dos = (vim_strchr(p_ffs, 'd') != NULL);
    int		try_unix = (vim_strchr(p_ffs, 'x') != NULL);


#ifdef AUTOCMD
    write_no_eol_lnum = 0;	/* in case it was set by the previous read */
#endif

    /*
     * If there is no file name yet, use the one for the read file.
     * b_notedited is set to reflect this.
     * Don't do this for a read from a filter.
     * Only do this when 'cpoptions' contains the 'f' flag.
     */
    if (curbuf->b_ffname == NULL && !filtering && !read_stdin &&
					vim_strchr(p_cpo, CPO_FNAMER) != NULL)
    {
	if (setfname(fname, sfname, FALSE) == OK)
	    curbuf->b_notedited = TRUE;
    }

    if (shortmess(SHM_OVER) || curbuf->b_help)
	msg_scroll = FALSE;	/* overwrite previous file message */
    else
	msg_scroll = TRUE;	/* don't overwrite previous file message */
    if (sfname == NULL)
	sfname = fname;
    /*
     * For Unix: Use the short file name whenever possible.
     * Avoids problems with networks and when directory names are changed.
     * Don't do this for MS-DOS, a "cd" in a sub-shell may have moved us to
     * another directory, which we don't detect.
     */
#if defined(UNIX) || defined(__EMX__)
    fname = sfname;
#endif

    /* set default 'fileformat' */
    if (newfile && *p_ffs)
	set_fileformat(default_fileformat());

#ifdef UNIX
    /*
     * On Unix it is possible to read a directory, so we have to
     * check for it before the open().
     */
    if (!read_stdin)
    {
	perm = mch_getperm(fname);
# ifdef _POSIX_SOURCE
	if (perm >= 0 && !S_ISREG(perm)		    /* not a regular file ... */
#  ifdef S_ISFIFO
		      && !S_ISFIFO(perm)	    /* ... or fifo or socket */
#  endif
					       )
# else
	if (perm >= 0 && (perm & S_IFMT) != S_IFREG /* not a regular file ... */
#  ifdef S_IFIFO
		      && (perm & S_IFMT) != S_IFIFO /* ... or fifo ... */
#  endif
#  ifdef S_IFSOCK
		      && (perm & S_IFMT) != S_IFSOCK/* ... or socket */
#  endif
						)
# endif
	{
# ifdef _POSIX_SOURCE
	    if (S_ISDIR(perm))
# else
	    if ((perm & S_IFMT) == S_IFDIR)
# endif
		filemess(curbuf, fname, (char_u *)"is a directory", 0);
	    else
		filemess(curbuf, fname, (char_u *)"is not a file", 0);
	    msg_end();
	    msg_scroll = msg_save;
	    return FAIL;
	}
    }
#endif

    /*
     * When opening a new file we take the readonly flag from the file.
     * Default is r/w, can be set to r/o below.
     * Don't reset it when in readonly mode
     * Only set/reset b_p_ro when BF_CHECK_RO is set.
     */
    check_readonly = (newfile && (curbuf->b_flags & BF_CHECK_RO));
    if (check_readonly && !readonlymode)    /* default: set file not readonly */
	curbuf->b_p_ro = FALSE;

    if (newfile && !read_stdin)
    {
	if (stat((char *)fname, &st) >= 0)  /* remember time of file */
	{
	    curbuf->b_mtime = st.st_mtime;
	    curbuf->b_mtime_read = st.st_mtime;
#ifdef UNIX
	    /*
	     * Set the protection bits of the swap file equal to the original
	     * file. This makes it possible for others to read the name of the
	     * original file from the swapfile.
	     */
	    if (curbuf->b_ml.ml_mfp->mf_fname != NULL)
		(void)mch_setperm(curbuf->b_ml.ml_mfp->mf_fname,
					  (long)((st.st_mode & 0777) | 0600));
#endif
	}
	else
	{
	    curbuf->b_mtime = 0;
	    curbuf->b_mtime_read = 0;
	}
    }

/*
 * for UNIX: check readonly with perm and access()
 * for MSDOS and Amiga: check readonly by trying to open the file for writing
 */
    file_readonly = FALSE;
    if (read_stdin)
	fd = 0;
    else
    {
#if defined(UNIX) || defined(DJGPP) || defined(__EMX__) || defined(VMS)
	if (
# ifdef UNIX
	    !(perm & 0222) ||
# endif
				access((char *)fname, W_OK))
	    file_readonly = TRUE;
	fd = open((char *)fname, O_RDONLY | O_EXTRA);
#else
	if (!newfile || readonlymode || (fd =
				   open((char *)fname, O_RDWR | O_EXTRA)) < 0)
	{
	    file_readonly = TRUE;
	    fd = open((char *)fname, O_RDONLY | O_EXTRA);   /* try to open ro */
	}
#endif
    }

    if (fd < 0)			    /* cannot open at all */
    {
#ifndef UNIX
	int	isdir_f;
#endif
	msg_scroll = msg_save;
#ifndef UNIX
    /*
     * On MSDOS and Amiga we can't open a directory, check here.
     */
	isdir_f = (mch_isdir(fname));
	fname = sfname;		    /* use short name now, for the messages */
	if (isdir_f)
	    filemess(curbuf, fname, (char_u *)"is a directory", 0);
	else
#endif
	    if (newfile)
	    {
#ifdef UNIX
		if (perm < 0)
#endif
		{
		    check_need_swap(newfile);	/* may create swap file now */
		    filemess(curbuf, fname, (char_u *)"[New File]", 0);
#ifdef AUTOCMD
		    apply_autocmds(EVENT_BUFNEWFILE, fname, fname, FALSE);
#endif
		    /* remember the current file format */
		    curbuf->b_start_ffc = *curbuf->b_p_ff;
		    return OK;	    /* a new file is not an error */
		}
#ifdef UNIX
		else
		    filemess(curbuf, fname, (char_u *)"[Permission Denied]", 0);
#endif
	    }

	return FAIL;
    }

    /*
     * Only set the 'ro' flag for readonly files the first time they are
     * loaded.	Help files always get readonly mode
     */
    if ((check_readonly && file_readonly) || curbuf->b_help)
	curbuf->b_p_ro = TRUE;

    if (newfile)
	curbuf->b_p_eol = TRUE;

    check_need_swap(newfile);	/* may create swap file now */

#ifndef UNIX
    fname = sfname;	    /* replace with short name now, for the messages */
#endif
    ++no_wait_return;	    /* don't wait for return yet */

    /*
     * Set '[ mark to the line above where the lines go (line 1 if zero).
     */
    curbuf->b_op_start.lnum = ((from == 0) ? 1 : from);
    curbuf->b_op_start.col = 0;

#ifdef AUTOCMD
    {
	int m = msg_scroll;
	int n = msg_scrolled;
	BUF *old_curbuf = curbuf;

	/*
	 * The file must be closed again, the autocommands may want to change
	 * the file before reading it.
	 */
	if (!read_stdin)
	    close(fd);		/* ignore errors */

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	msg_scroll = TRUE;
	if (filtering)
	    apply_autocmds(EVENT_FILTERREADPRE, NULL, fname, FALSE);
	else if (read_stdin)
	    apply_autocmds(EVENT_STDINREADPRE, NULL, fname, FALSE);
	else if (newfile)
	    apply_autocmds(EVENT_BUFREADPRE, NULL, fname, FALSE);
	else
	    apply_autocmds(EVENT_FILEREADPRE, fname, fname, FALSE);
	if (msg_scrolled == n)
	    msg_scroll = m;

	/*
	 * Don't allow the autocommands to change the current buffer.
	 * Try to re-open the file.
	 */
	if (!read_stdin && (curbuf != old_curbuf ||
			  (fd = open((char *)fname, O_RDONLY | O_EXTRA)) < 0))
	{
	    --no_wait_return;
	    msg_scroll = msg_save;
	    if (fd < 0)
		EMSG("*ReadPre autocommands made the file unreadable");
	    else
		EMSG("*ReadPre autocommands must not change current buffer");
	    return FAIL;
	}
    }
#endif

    if (!recoverymode && !filtering && !read_stdin)
	filemess(curbuf, fname, (char_u *)"", 0);   /* show that we are busy */

    msg_scroll = FALSE;				/* overwrite the file message */

    /*
     * Set fileformat and linecnt now, before the "retry" caused by a wrong
     * guess for fileformat, and after the autocommands, which may change
     * them.
     */
    if (curbuf->b_p_bin)
	fileformat = EOL_UNIX;		    /* binary: use Unix format */
    else if (*p_ffs == NUL)
	fileformat = get_fileformat(curbuf);  /* use format from buffer */
    else
	fileformat = EOL_UNKNOWN;	    /* detect from file */
    linecnt = curbuf->b_ml.ml_line_count;

retry:
    linerest = 0;
    filesize = 0;
    skip_count = lines_to_skip;
    read_count = lines_to_read;
    while (!error && !got_int)
    {
	/*
	 * We allocate as much space for the file as we can get, plus
	 * space for the old line plus room for one terminating NUL.
	 * The amount is limited by the fact that read() only can read
	 * upto max_unsigned characters (and other things).
	 */
#if SIZEOF_INT <= 2
	if (linerest >= 0x7ff0)
	{
	    ++split;
	    *ptr = NL;		    /* split line by inserting a NL */
	    size = 1;
	}
	else
#endif
	{
#if SIZEOF_INT > 2
	    size = 0x10000L;		    /* use buffer >= 64K */
#else
	    size = 0x7ff0L - linerest;	    /* limit buffer to 32K */
#endif

	    for ( ; size >= 10; size = (long_u)size >> 1)
	    {
		if ((new_buffer = lalloc((long_u)(size + linerest + 1),
							      FALSE)) != NULL)
		    break;
	    }
	    if (new_buffer == NULL)
	    {
		do_outofmem_msg();
		error = TRUE;
		break;
	    }
	    if (linerest)	/* copy characters from the previous buffer */
		vim_memmove(new_buffer, ptr - linerest, (size_t)linerest);
	    vim_free(buffer);
	    buffer = new_buffer;
	    ptr = buffer + linerest;
	    line_start = buffer;

	    if ((size = read(fd, (char *)ptr, (size_t)size)) <= 0)
	    {
		if (size < 0)		    /* read error */
		    error = TRUE;
		break;
	    }
	    filesize += size;		    /* count the number of characters */

	    /*
	     * when reading the first part of a file: guess EOL type
	     */
	    if (fileformat == EOL_UNKNOWN)
	    {
		/* First try finding a NL, for Dos and Unix */
		if (try_dos || try_unix)
		{
		    for (p = ptr; p < ptr + size; ++p)
			if (*p == NL)
			{
			    if (!try_unix
				    || (try_dos && p > ptr && p[-1] == CR))
				fileformat = EOL_DOS;
			    else
				fileformat = EOL_UNIX;
			    break;
			}
		}

		/* No NL found: may use Mac format */
		if (fileformat == EOL_UNKNOWN && try_mac)
		    fileformat = EOL_MAC;

		/* Still nothing found?  Use first format in 'ffs' */
		if (fileformat == EOL_UNKNOWN)
		    fileformat = default_fileformat();

		/* if editing a new file: may set p_tx and p_ff */
		if (newfile)
		    set_fileformat(fileformat);
	    }
	}

	/*
	 * This loop is executed once for every character read.
	 * Keep it fast!
	 */
	if (fileformat == EOL_MAC)
	{
	    --ptr;
	    while (++ptr, --size >= 0)
	    {
		/* catch most common case first */
		if ((c = *ptr) != NUL && c != CR && c != NL)
		    continue;
		if (c == NUL)
		    *ptr = NL;	/* NULs are replaced by newlines! */
		else
		{
		    if (skip_count == 0)
		    {
			if (c == NL)
			{
			    /*
			     * Reading in Mac format, but a NL found!
			     * When 'fileformats' includes "unix" or "dos",
			     * delete all the lines read so far and start all
			     * over again.  Otherwise give an error message
			     * later.
			     */
			    if (ff_error == EOL_UNKNOWN)
			    {
				if ((try_dos || try_unix)
					&& !read_stdin
					&& lseek(fd, (off_t)0L, SEEK_SET) == 0)
				{
				    while (lnum > from)
					ml_delete(lnum--, FALSE);
				    if (!try_unix || ptr[-1] == NUL)
					fileformat = EOL_DOS;
				    else
					fileformat = EOL_UNIX;
				    if (newfile)
					set_fileformat(fileformat);
				    goto retry;
				}
				else
				    ff_error = EOL_MAC;
			    }
			}
			*ptr = NUL;	    /* end of line */
			len = ptr - line_start + 1;
			if (ml_append(lnum, line_start, len, newfile) == FAIL)
			{
			    error = TRUE;
			    break;
			}
			++lnum;
			if (--read_count == 0)
			{
			    error = TRUE;	    /* break loop */
			    line_start = ptr;	/* nothing left to write */
			    break;
			}
		    }
		    else
			--skip_count;
		    line_start = ptr + 1;
		}
	    }
	}
	else
	{
	    --ptr;
	    while (++ptr, --size >= 0)
	    {
		if ((c = *ptr) != NUL && c != NL)  /* catch most common case */
		    continue;
		if (c == NUL)
		    *ptr = NL;	/* NULs are replaced by newlines! */
		else
		{
		    if (skip_count == 0)
		    {
			*ptr = NUL;		/* end of line */
			len = ptr - line_start + 1;
			if (fileformat == EOL_DOS)
			{
			    if (ptr[-1] == CR)	/* remove CR */
			    {
				ptr[-1] = NUL;
				--len;
			    }
			    /*
			     * Reading in Dos format, but no CR-LF found!
			     * When 'fileformats' includes "unix", delete all
			     * the lines read so far and start all over again.
			     * Otherwise give an error message later.
			     */
			    else if (ff_error != EOL_DOS)
			    {
				if (	   try_unix
					&& !read_stdin
					&& lseek(fd, (off_t)0L, SEEK_SET) == 0)
				{
				    while (lnum > from)
					ml_delete(lnum--, FALSE);
				    fileformat = EOL_UNIX;
				    if (newfile)
					set_fileformat(EOL_UNIX);
				    goto retry;
				}
				else
				    ff_error = EOL_DOS;
			    }
			}
			if (ml_append(lnum, line_start, len, newfile) == FAIL)
			{
			    error = TRUE;
			    break;
			}
			++lnum;
			if (--read_count == 0)
			{
			    error = TRUE;	    /* break loop */
			    line_start = ptr;	/* nothing left to write */
			    break;
			}
		    }
		    else
			--skip_count;
		    line_start = ptr + 1;
		}
	    }
	}
	linerest = ptr - line_start;
	ui_breakcheck();
    }

    /* not an error, max. number of lines reached */
    if (error && read_count == 0)
	error = FALSE;

    /*
     * If we get EOF in the middle of a line, note the fact and
     * complete the line ourselves.
     * In Dos format ignore a trailing CTRL-Z, unless 'binary' set.
     */
    if (!error && !got_int && linerest != 0 &&
	    !(!curbuf->b_p_bin && fileformat == EOL_DOS &&
		    *line_start == Ctrl('Z') && ptr == line_start + 1))
    {
	if (newfile)		    /* remember for when writing */
	    curbuf->b_p_eol = FALSE;
	*ptr = NUL;
	if (ml_append(lnum, line_start,
			(colnr_t)(ptr - line_start + 1), newfile) == FAIL)
	    error = TRUE;
	else
	    read_no_eol_lnum = ++lnum;
    }
    if (lnum != from && !newfile)   /* added at least one line */
	CHANGED;

    invalidate_botline();	    /* need to recompute w_botline */
    changed_line_abv_curs();	    /* need to recompute cursor posn */
    if (newfile)
	curbuf->b_start_ffc = *curbuf->b_p_ff;	/* remember 'fileformat' */

    close(fd);			    /* errors are ignored */
    vim_free(buffer);

    --no_wait_return;		    /* may wait for return now */

    /*
     * In recovery mode everything but autocommands are skipped.
     */
    if (!recoverymode)
    {
	/* need to delete the last line, which comes from the empty buffer */
	if (newfile && !(curbuf->b_ml.ml_flags & ML_EMPTY))
	{
	    ml_delete(curbuf->b_ml.ml_line_count, FALSE);
	    --linecnt;
	}
	linecnt = curbuf->b_ml.ml_line_count - linecnt;
	if (filesize == 0)
	    linecnt = 0;
	if (!newfile)
	    mark_adjust(from + 1, (linenr_t)MAXLNUM, (long)linecnt, 0L);

	/*
	 * If we were reading from the same terminal as where messages go,
	 * the screen will have been messed up.
	 * Switch on raw mode now and clear the screen.
	 */
	if (read_stdin)
	{
	    settmode(TMODE_RAW);	    /* set to raw mode */
	    starttermcap();
	    screenclear();
	}

	if (got_int)
	{
	    filemess(curbuf, fname, e_interr, 0);
	    msg_scroll = msg_save;
#ifdef VIMINFO
	    check_marks_read();
#endif
	    return OK;		/* an interrupt isn't really an error */
	}

	if (!filtering)
	{
	    msg_add_fname(curbuf, fname);   /* fname in IObuff with quotes */
	    c = FALSE;

#ifdef UNIX
# ifdef S_ISFIFO
	    if (S_ISFIFO(perm))			    /* fifo or socket */
	    {
		STRCAT(IObuff, "[fifo/socket]");
		c = TRUE;
	    }
# else
#  ifdef S_IFIFO
	    if ((perm & S_IFMT) == S_IFIFO)	    /* fifo */
	    {
		STRCAT(IObuff, "[fifo]");
		c = TRUE;
	    }
#  endif
#  ifdef S_IFSOCK
	    if ((perm & S_IFMT) == S_IFSOCK)	    /* or socket */
	    {
		STRCAT(IObuff, "[socket]");
		c = TRUE;
	    }
#  endif
# endif
#endif
	    if (curbuf->b_p_ro)
	    {
		STRCAT(IObuff, shortmess(SHM_RO) ? "[RO]" : "[readonly]");
		c = TRUE;
	    }
	    if (read_no_eol_lnum)
	    {
		msg_add_eol();
		c = TRUE;
	    }
	    if (ff_error == EOL_DOS)
	    {
		STRCAT(IObuff, "[CR missing]");
		c = TRUE;
	    }
	    if (ff_error == EOL_MAC)
	    {
		STRCAT(IObuff, "[NL found]");
		c = TRUE;
	    }
	    if (split)
	    {
		STRCAT(IObuff, "[long lines split]");
		c = TRUE;
	    }
	    if (error)
	    {
		STRCAT(IObuff, "[READ ERRORS]");
		c = TRUE;
	    }
	    if (msg_add_fileformat(fileformat))
		c = TRUE;
	    msg_add_lines(c, (long)linecnt, filesize);
	    msg_trunc(IObuff);
	}

	if (error && newfile)	/* with errors we should not write the file */
	    curbuf->b_p_ro = TRUE;

	u_clearline();	    /* cannot use "U" command after adding lines */

	/*
	 * In Ex mode: cursor at last new line.
	 * Otherwise: cursor at first new line.
	 */
	if (exmode_active)
	    curwin->w_cursor.lnum = from + linecnt;
	else
	    curwin->w_cursor.lnum = from + 1;
	check_cursor_lnum();
	beginline(BL_WHITE | BL_FIX);	    /* on first non-blank */

	/*
	 * Set '[ and '] marks to the newly read lines.
	 */
	curbuf->b_op_start.lnum = from + 1;
	curbuf->b_op_start.col = 0;
	curbuf->b_op_end.lnum = from + linecnt;
	curbuf->b_op_end.col = 0;
    }
    msg_scroll = msg_save;

#ifdef VIMINFO
    /*
     * Get the marks before executing autocommands, so they can be used there.
     */
    check_marks_read();
#endif

#ifdef AUTOCMD
    {
	int m = msg_scroll;
	int n = msg_scrolled;

	/*
	 * Trick: We remember if the last line of the read didn't have
	 * an eol for when writing it again.  This is required for
	 * ":autocmd FileReadPost *.gz set bin|'[,']!gunzip" to work.
	 */
	write_no_eol_lnum = read_no_eol_lnum;

	/*
	 * The output from the autocommands should not overwrite anything and
	 * should not be overwritten: Set msg_scroll, restore its value if no
	 * output was done.
	 */
	msg_scroll = TRUE;
	if (filtering)
	    apply_autocmds(EVENT_FILTERREADPOST, NULL, fname, FALSE);
	else if (read_stdin)
	    apply_autocmds(EVENT_STDINREADPOST, NULL, fname, FALSE);
	else if (newfile)
	    apply_autocmds(EVENT_BUFREADPOST, NULL, fname, FALSE);
	else
	    apply_autocmds(EVENT_FILEREADPOST, fname, fname, FALSE);
	if (msg_scrolled == n)
	    msg_scroll = m;
    }
#endif

    if (recoverymode && error)
	return FAIL;
    return OK;
}

#ifdef VIMINFO
    static void
check_marks_read()
{
    if (!curbuf->b_marks_read && get_viminfo_parameter('\'') > 0)
    {
	read_viminfo(NULL, FALSE, TRUE, FALSE);
	curbuf->b_marks_read = TRUE;
    }
}
#endif

#ifdef UNIX
    static void
set_file_time(fname, atime, mtime)
    char_u  *fname;
    time_t  atime;	    /* access time */
    time_t  mtime;	    /* modification time */
{
# if defined(HAVE_UTIME) && defined(HAVE_UTIME_H)
#  include <utime.h>

    struct utimbuf  buf;

    buf.actime	= atime;
    buf.modtime	= mtime;
    (void)utime((char *)fname, &buf);
# else
#  if defined(HAVE_UTIMES)

    struct timeval  tvp[2];

    tvp[0].tv_sec   = atime;
    tvp[0].tv_usec  = 0;
    tvp[1].tv_sec   = mtime;
    tvp[1].tv_usec  = 0;
    (void)utimes((char *)fname, &tvp);
#  endif
# endif
}
#endif /* UNIX */

/*
 * buf_write() - write to file 'fname' lines 'start' through 'end'
 *
 * We do our own buffering here because fwrite() is so slow.
 *
 * If forceit is true, we don't care for errors when attempting backups (jw).
 * In case of an error everything possible is done to restore the original file.
 * But when forceit is TRUE, we risk loosing it.
 * When reset_changed is TRUE and start == 1 and end ==
 * curbuf->b_ml.ml_line_count, reset curbuf->b_changed.
 *
 * This function must NOT use NameBuff (because it's called by autowrite()).
 *
 * return FAIL for failure, OK otherwise
 */
    int
buf_write(buf, fname, sfname, start, end, append, forceit,
						      reset_changed, filtering)
    BUF		    *buf;
    char_u	    *fname;
    char_u	    *sfname;
    linenr_t	    start, end;
    int		    append;
    int		    forceit;
    int		    reset_changed;
    int		    filtering;
{
    int		    fd;
    char_u	    *backup = NULL;
    char_u	    *ffname;
#ifdef AUTOCMD
    BUF		    *save_buf;
#endif
    char_u	    *s;
    char_u	    *ptr;
    char_u	    c;
    int		    len;
    linenr_t	    lnum;
    long	    nchars;
    char_u	    *errmsg = NULL;
    char_u	    *buffer;
    char_u	    smallbuf[SMBUFSIZE];
    char_u	    *backup_ext;
    int		    bufsize;
    long	    perm;		    /* file permissions */
    int		    retval = OK;
    int		    newfile = FALSE;	    /* TRUE if file doesn't exist yet */
    int		    msg_save = msg_scroll;
    int		    overwriting;	    /* TRUE if writing over original */
    int		    no_eol = FALSE;	    /* no end-of-line written */
#if defined(UNIX) || defined(__EMX__XX)	    /*XXX fix me sometime? */
    struct stat	    st_old;
    int		    made_writable = FALSE;  /* 'w' bit has been set */
#endif
#ifdef AMIGA
    BPTR	    flock;
#endif
					    /* writing everything */
    int		    whole = (start == 1 && end == buf->b_ml.ml_line_count);
#ifdef AUTOCMD
    linenr_t	    old_line_count = buf->b_ml.ml_line_count;
#endif
    int		    attr;
    int		    fileformat = get_fileformat(buf);

    if (fname == NULL || *fname == NUL)	    /* safety check */
	return FAIL;

    /*
     * If there is no file name yet, use the one for the written file.
     * b_notedited is set to reflect this (in case the write fails).
     * Don't do this when the write is for a filter command.
     * Only do this when 'cpoptions' contains the 'f' flag.
     */
    if (reset_changed && whole && buf == curbuf &&
				   curbuf->b_ffname == NULL && !filtering &&
					vim_strchr(p_cpo, CPO_FNAMEW) != NULL)
    {
	if (setfname(fname, sfname, FALSE) == OK)
	    curbuf->b_notedited = TRUE;
    }

    if (sfname == NULL)
	sfname = fname;
    /*
     * For Unix: Use the short file name whenever possible.
     * Avoids problems with networks and when directory names are changed.
     * Don't do this for MS-DOS, a "cd" in a sub-shell may have moved us to
     * another directory, which we don't detect
     */
    ffname = fname;			    /* remember full fname */
#ifdef UNIX
    fname = sfname;
#endif

	/* make sure we have a valid backup extension to use */
    if (*p_bex == NUL)
	backup_ext = (char_u *)".bak";
    else
	backup_ext = p_bex;

    if (buf->b_ffname != NULL && fnamecmp(ffname, buf->b_ffname) == 0)
	overwriting = TRUE;
    else
	overwriting = FALSE;

    /*
     * Disallow writing from .exrc and .vimrc in current directory for
     * security reasons.
     */
    if (check_secure())
	return FAIL;

    if (exiting)
	settmode(TMODE_COOK);	    /* when exiting allow typahead now */

    ++no_wait_return;		    /* don't wait for return yet */

    /*
     * Set '[ and '] marks to the lines to be written.
     */
    buf->b_op_start.lnum = start;
    buf->b_op_start.col = 0;
    buf->b_op_end.lnum = end;
    buf->b_op_end.col = 0;

#ifdef AUTOCMD
    /*
     * Apply PRE aucocommands.
     * Set curbuf to the buffer to be written.
     * Careful: The autocommands may call buf_write() recursively!
     */
    save_buf = curbuf;
    curbuf = buf;
    curwin->w_buffer = buf;
    if (append)
	apply_autocmds(EVENT_FILEAPPENDPRE, fname, fname, FALSE);
    else if (filtering)
	apply_autocmds(EVENT_FILTERWRITEPRE, NULL, fname, FALSE);
    else if (reset_changed && whole)
	apply_autocmds(EVENT_BUFWRITEPRE, fname, fname, FALSE);
    else
	apply_autocmds(EVENT_FILEWRITEPRE, fname, fname, FALSE);
    /*
     * If the autocommands deleted or unloaded the buffer, give an error
     * message.
     */
    if (!buf_valid(buf) || buf->b_ml.ml_mfp == NULL)
    {
	--no_wait_return;
	msg_scroll = msg_save;
	EMSG("Autocommands deleted or unloaded buffer to be written");
	return FAIL;
    }
    /*
     * If the autocommands didn't change the current buffer, go back to the
     * original current buffer, if it still exists.
     */
    if (curbuf == buf && buf_valid(save_buf))
    {
	curbuf = save_buf;
	curwin->w_buffer = save_buf;
    }

    /*
     * The autocommands may have changed the number of lines in the file.
     * When writing the whole file, adjust the end.
     * When writing part of the file, assume that the autocommands only
     * changed the number of lines that are to be written (tricky!).
     */
    if (buf->b_ml.ml_line_count != old_line_count)
    {
	if (whole)					    /* writing all */
	    end = buf->b_ml.ml_line_count;
	else if (buf->b_ml.ml_line_count > old_line_count)  /* more lines */
	    end += buf->b_ml.ml_line_count - old_line_count;
	else						    /* less lines */
	{
	    end -= old_line_count - buf->b_ml.ml_line_count;
	    if (end < start)
	    {
		--no_wait_return;
		msg_scroll = msg_save;
		EMSG("Autocommand changed number of lines in unexpected way");
		return FAIL;
	    }
	}
    }
#endif

    if (shortmess(SHM_OVER))
	msg_scroll = FALSE;	    /* overwrite previous file message */
    else
	msg_scroll = TRUE;	    /* don't overwrite previous file message */
    if (!filtering)
	filemess(buf,
#ifndef UNIX
		sfname,
#else
		fname,
#endif
		    (char_u *)"", 0);	/* show that we are busy */
    msg_scroll = FALSE;		    /* always overwrite the file message now */

    buffer = alloc(BUFSIZE);
    if (buffer == NULL)		    /* can't allocate big buffer, use small
				     * one (to be able to write when out of
				     * memory) */
    {
	buffer = smallbuf;
	bufsize = SMBUFSIZE;
    }
    else
	bufsize = BUFSIZE;

#if defined(UNIX) && !defined(ARCHIE)
	/* get information about original file (if there is one) */
    st_old.st_dev = st_old.st_ino = 0;
    perm = -1;
    if (stat((char *)fname, &st_old))
	newfile = TRUE;
    else
    {
#ifdef _POSIX_SOURCE
	if (!S_ISREG(st_old.st_mode))		/* not a file */
#else
	if ((st_old.st_mode & S_IFMT) != S_IFREG)   /* not a file */
#endif
	{
#ifdef _POSIX_SOURCE
	    if (S_ISDIR(st_old.st_mode))
#else
	    if ((st_old.st_mode & S_IFMT) == S_IFDIR)
#endif
		errmsg = (char_u *)"is a directory";
	    else
		errmsg = (char_u *)"is not a file";
	    goto fail;
	}
	if (overwriting)
	{
	    retval = check_mtime(buf, &st_old);
	    if (retval == FAIL)
		goto fail;
	}
	perm = st_old.st_mode;
    }
/*
 * If we are not appending or filtering, the file exists, and the
 * 'writebackup', 'backup' or 'patchmode' option is set, try to make a backup
 * copy of the file.
 */
    if (!append && !filtering && perm >= 0 && (p_wb || p_bk || *p_pm != NUL)
	    && (fd = open((char *)fname, O_RDONLY | O_EXTRA)) >= 0)
    {
	int		bfd, buflen;
	char_u		copybuf[BUFSIZE + 1], *wp;
	int		some_error = FALSE;
	struct stat	st_new;
	char_u		*dirp;
	char_u		*rootname;
#ifndef SHORT_FNAME
	int		did_set_shortname;
#endif

	/*
	 * Try to make the backup in each directory in the 'bdir' option.
	 *
	 * Unix semantics has it, that we may have a writable file,
	 * that cannot be recreated with a simple open(..., O_CREAT, ) e.g:
	 *  - the directory is not writable,
	 *  - the file may be a symbolic link,
	 *  - the file may belong to another user/group, etc.
	 *
	 * For these reasons, the existing writable file must be truncated
	 * and reused. Creation of a backup COPY will be attempted.
	 */
	dirp = p_bdir;
	while (*dirp)
	{
	    st_new.st_dev = st_new.st_ino = 0;
	    st_new.st_gid = 0;

	    /*
	     * Isolate one directory name, using an entry in 'bdir'.
	     */
	    (void)copy_option_part(&dirp, copybuf, BUFSIZE, ",");
	    rootname = get_file_in_dir(fname, copybuf);
	    if (rootname == NULL)
	    {
		some_error = TRUE;	    /* out of memory */
		goto nobackup;
	    }


#ifndef SHORT_FNAME
	    did_set_shortname = FALSE;
#endif

	    /*
	     * May try twice if 'shortname' not set.
	     */
	    for (;;)
	    {
		/*
		 * Make backup file name.
		 */
		backup = buf_modname(
#ifdef SHORT_FNAME
					TRUE,
#else
					(buf->b_p_sn || buf->b_shortname),
#endif
						 rootname, backup_ext, FALSE);
		if (backup == NULL)
		{
		    some_error = TRUE;		/* out of memory */
		    vim_free(rootname);
		    goto nobackup;
		}

		/*
		 * Check if backup file already exists.
		 */
		if (!stat((char *)backup, &st_new))
		{
		    /*
		     * Check if backup file is same as original file.
		     * May happen when modname gave the same file back.
		     * E.g. silly link, or file name-length reached.
		     * If we don't check here, we either ruin the file when
		     * copying or erase it after writing. jw.
		     */
		    if (st_new.st_dev == st_old.st_dev &&
					   st_new.st_ino == st_old.st_ino)
		    {
			vim_free(backup);
			backup = NULL;	/* there is no backup file to delete */
#ifndef SHORT_FNAME
			/*
			 * may try again with 'shortname' set
			 */
			if (!(buf->b_shortname || buf->b_p_sn))
			{
			    buf->b_shortname = TRUE;
			    did_set_shortname = TRUE;
			    continue;
			}
			    /* setting shortname didn't help */
			if (did_set_shortname)
			    buf->b_shortname = FALSE;
#endif
			break;
		    }

		    /*
		     * If we are not going to keep the backup file, don't
		     * delete an existing one, try to use another name.
		     * Change one character, just before the extension.
		     */
		    if (!p_bk)
		    {
			wp = backup + STRLEN(backup) - 1 - STRLEN(backup_ext);
			if (wp < backup)	/* empty file name ??? */
			    wp = backup;
			*wp = 'z';
			while (*wp > 'a' && !stat((char *)backup, &st_new))
			    --*wp;
			/* They all exist??? Must be something wrong. */
			if (*wp == 'a')
			{
			    vim_free(backup);
			    backup = NULL;
			}
		    }
		}
		break;
	    }
	    vim_free(rootname);

	    /*
	     * Try to create the backup file
	     */
	    if (backup != NULL)
	    {
		/* remove old backup, if present */
		vim_remove(backup);
		bfd = open((char *)backup, O_WRONLY | O_CREAT | O_EXTRA, 0666);
		if (bfd < 0)
		{
		    vim_free(backup);
		    backup = NULL;
		}
		else
		{
		    /* set file protection same as original file, but strip
		     * s-bit */
		    (void)mch_setperm(backup, perm & 0777);

		    /*
		     * Try to set the group of the backup same as the original
		     * file. If this fails, set the protection bits for the
		     * group same as the protection bits for others.
		     */
		    if (st_new.st_gid != st_old.st_gid &&
#ifdef HAVE_FCHOWN  /* sequent-ptx lacks fchown() */
				    fchown(bfd, (uid_t)-1, st_old.st_gid) != 0
#else
			  chown((char *)backup, (uid_t)-1, st_old.st_gid) != 0
#endif
					    )
			mch_setperm(backup, (perm & 0707) | ((perm & 07) << 3));

		    /* copy the file. */
		    while ((buflen = read(fd, (char *)copybuf, BUFSIZE)) > 0)
		    {
			if (write_buf(bfd, copybuf, buflen) == FAIL)
			{
			    errmsg = (char_u *)"Can't write to backup file (use ! to override)";
			    break;
			}
		    }
		    if (close(bfd) < 0 && errmsg == NULL)
			errmsg = (char_u *)"Close error for backup file (use ! to override)";
		    if (buflen < 0)
			errmsg = (char_u *)"Can't read file for backup (use ! to override)";
		    set_file_time(backup, st_old.st_atime, st_old.st_mtime);
		    break;
		}
	    }
	}
nobackup:
	close(fd);		/* ignore errors for closing read file */

	if (backup == NULL && errmsg == NULL)
	    errmsg = (char_u *)"Cannot create backup file (use ! to override)";
	/* ignore errors when forceit is TRUE */
	if ((some_error || errmsg) && !forceit)
	{
	    retval = FAIL;
	    goto fail;
	}
	errmsg = NULL;
    }
    /* When using ":w!" and the file was read-only: make it writable */
    if (forceit && st_old.st_uid == getuid() && perm >= 0 && !(perm & 0200)
				     && vim_strchr(p_cpo, CPO_FWRITE) == NULL)
    {
	perm |= 0200;
	(void)mch_setperm(fname, perm);
	made_writable = TRUE;
    }

#else /* end of UNIX, start of the rest */

/*
 * If we are not appending, the file exists, and the 'writebackup' or
 * 'backup' option is set, make a backup.
 * Do not make any backup, if "writebackup" and "backup" are
 * both switched off. This helps when editing large files on
 * almost-full disks. (jw)
 */
    perm = mch_getperm(fname);
    if (perm < 0)
	newfile = TRUE;
    else if (mch_isdir(fname))
    {
	errmsg = (char_u *)"is a directory";
	goto fail;
    }
    else if (overwriting)
    {
	struct stat	st;

	if (stat((char *)fname, &st) >= 0)
	{
	    retval = check_mtime(buf, &st);
	    if (retval == FAIL)
		goto fail;
	}
    }

    if (!append && !filtering && perm >= 0 && (p_wb || p_bk || *p_pm != NUL))
    {
	char_u		*dirp;
	char_u		*p;
	char_u		*rootname;

	/*
	 * Form the backup file name - change path/fo.o.h to path/fo.o.h.bak
	 * Try all directories in 'backupdir', first one that works is used.
	 */
	dirp = p_bdir;
	while (*dirp)
	{
	    /*
	     * Isolate one directory name and make the backup file name.
	     */
	    (void)copy_option_part(&dirp, IObuff, IOSIZE, ",");
	    rootname = get_file_in_dir(fname, IObuff);
	    if (rootname == NULL)
		backup = NULL;
	    else
	    {
		backup = buf_modname(
#ifdef SHORT_FNAME
					TRUE,
#else
					(buf->b_p_sn || buf->b_shortname),
#endif
						 rootname, backup_ext, FALSE);
		vim_free(rootname);
	    }

	    if (backup != NULL)
	    {
		/*
		 * If we are not going to keep the backup file, don't
		 * delete an existing one, try to use another name.
		 * Change one character, just before the extension.
		 */
		if (!p_bk && mch_getperm(backup) >= 0)
		{
		    p = backup + STRLEN(backup) - 1 - STRLEN(backup_ext);
		    if (p < backup)	/* empty file name ??? */
			p = backup;
		    *p = 'z';
		    while (*p > 'a' && mch_getperm(backup) >= 0)
			--*p;
		    /* They all exist??? Must be something wrong! */
		    if (*p == 'a')
		    {
			vim_free(backup);
			backup = NULL;
		    }
		}
	    }
	    if (backup != NULL)
	    {

		/*
		 * Delete any existing backup and move the current version to
		 * the backup.	For safety, we don't remove the backup until
		 * the write has finished successfully. And if the 'backup'
		 * option is set, leave it around.
		 */
#ifdef AMIGA
		/*
		 * With MSDOS-compatible filesystems (crossdos, messydos) it is
		 * possible that the name of the backup file is the same as the
		 * original file. To avoid the chance of accidently deleting the
		 * original file (horror!) we lock it during the remove.
		 * This should not happen with ":w", because startscript()
		 * should detect this problem and set buf->b_shortname,
		 * causing modname to return a correct ".bak" file name. This
		 * problem does exist with ":w file name", but then the
		 * original file will be somewhere else so the backup isn't
		 * really important. If autoscripting is off the rename may
		 * fail.
		 */
		flock = Lock((UBYTE *)fname, (long)ACCESS_READ);
#endif
		vim_remove(backup);
#ifdef AMIGA
		if (flock)
		    UnLock(flock);
#endif
		/*
		 * If the renaming of the original file to the backup file
		 * works, quit here.
		 */
		if (vim_rename(fname, backup) == 0)
		    break;

		vim_free(backup);   /* don't do the rename below */
		backup = NULL;
	    }
	}
	if (backup == NULL && !forceit)
	{
	    errmsg = (char_u *)"Can't make backup file (use ! to override)";
	    goto fail;
	}
    }
#endif /* UNIX */

    /* When using ":w!" and writing to the current file, readonly makes no
     * sense, reset it */
    if (forceit && overwriting)
	buf->b_p_ro = FALSE;

    /*
     * If the original file is being overwritten, there is a small chance that
     * we crash in the middle of writing. Therefore the file is preserved now.
     * This makes all block numbers positive so that recovery does not need
     * the original file.
     * Don't do this if there is a backup file and we are exiting.
     */
    if (reset_changed && !newfile && !otherfile(ffname) &&
					    !(exiting && backup != NULL))
	ml_preserve(buf, FALSE);

    /*
     * We may try to open the file twice: If we can't write to the
     * file and forceit is TRUE we delete the existing file and try to create
     * a new one. If this still fails we may have lost the original file!
     * (this may happen when the user reached his quotum for number of files).
     * Appending will fail if the file does not exist and forceit is FALSE.
     */
    while ((fd = open((char *)fname, O_WRONLY | O_EXTRA | (append ?
		    (forceit ? (O_APPEND | O_CREAT) : O_APPEND) :
		    (O_CREAT | O_TRUNC))
#ifndef macintosh
			, 0666
#endif
				)) < 0)
    {
	/*
	 * A forced write will try to create a new file if the old one is
	 * still readonly. This may also happen when the directory is
	 * read-only. In that case the vim_remove() will fail.
	 */
	if (!errmsg)
	{
	    errmsg = (char_u *)"Can't open file for writing";
	    if (forceit && vim_strchr(p_cpo, CPO_FWRITE) == NULL)
	    {
#ifdef UNIX
		/* we write to the file, thus it should be marked
						    writable after all */
		if (!(perm & 0200))
		    made_writable = TRUE;
		perm |= 0200;
		if (st_old.st_uid != getuid() || st_old.st_gid != getgid())
		    perm &= 0777;
#endif
		if (!append)	    /* don't remove when appending */
		    vim_remove(fname);
		continue;
	    }
	}
/*
 * If we failed to open the file, we don't need a backup. Throw it away.
 * If we moved or removed the original file try to put the backup in its place.
 */
	if (backup != NULL)
	{
#ifdef UNIX
	    struct stat st;

	    /*
	     * There is a small chance that we removed the original, try
	     * to move the copy in its place.
	     * This may not work if the vim_rename() fails.
	     * In that case we leave the copy around.
	     */
					/* file does not exist */
	    if (stat((char *)fname, &st) < 0)
					/* put the copy in its place */
		vim_rename(backup, fname);
					/* original file does exist */
	    if (stat((char *)fname, &st) >= 0)
		vim_remove(backup); /* throw away the copy */
#else
					/* try to put the original file back */
	    vim_rename(backup, fname);
#endif
	}
	goto fail;
    }
    errmsg = NULL;

    if (end > buf->b_ml.ml_line_count)
	end = buf->b_ml.ml_line_count;
    len = 0;
    s = buffer;
    nchars = 0;
    if (buf->b_ml.ml_flags & ML_EMPTY)
	start = end + 1;
    for (lnum = start; lnum <= end; ++lnum)
    {
	/*
	 * The next while loop is done once for each character written.
	 * Keep it fast!
	 */
	ptr = ml_get_buf(buf, lnum, FALSE) - 1;
	while ((c = *++ptr) != NUL)
	{
	    if (c == NL)
		*s = NUL;		/* replace newlines with NULs */
	    else
		*s = c;
	    ++s;
	    if (++len != bufsize)
		continue;
	    if (write_buf(fd, buffer, bufsize) == FAIL)
	    {
		end = 0;		/* write error: break loop */
		break;
	    }
	    nchars += bufsize;
	    s = buffer;
	    len = 0;
	}
	/* write failed or last line has no EOL: stop here */
	if (end == 0 || (lnum == end && buf->b_p_bin &&
						(lnum == write_no_eol_lnum ||
			 (lnum == buf->b_ml.ml_line_count && !buf->b_p_eol))))
	{
	    ++lnum;			/* written the line, count it */
	    no_eol = TRUE;
	    break;
	}
	if (fileformat == EOL_UNIX)
	    *s++ = NL;
	else
	{
	    *s++ = CR;			/* EOL_MAC or EOL_DOS: write CR */
	    if (fileformat == EOL_DOS)	/* write CR-NL */
	    {
		if (++len == bufsize)
		{
		    if (write_buf(fd, buffer, bufsize) == FAIL)
		    {
			end = 0;	/* write error: break loop */
			break;
		    }
		    nchars += bufsize;
		    s = buffer;
		    len = 0;
		}
		*s++ = NL;
	    }
	}
	if (++len == bufsize && end)
	{
	    if (write_buf(fd, buffer, bufsize) == FAIL)
	    {
		end = 0;		/* write error: break loop */
		break;
	    }
	    nchars += bufsize;
	    s = buffer;
	    len = 0;
	}
    }
    if (len && end)
    {
	if (write_buf(fd, buffer, len) == FAIL)
	    end = 0;		    /* write error */
	nchars += len;
    }

    if (close(fd) != 0)
    {
	errmsg = (char_u *)"Close failed";
	goto fail;
    }
#ifdef UNIX
    if (made_writable)
	perm &= ~0200;		/* reset 'w' bit for security reasons */
#endif
    if (perm >= 0)		/* set perm. of new file same as old file */
	(void)mch_setperm(fname, perm);

    if (end == 0)
    {
	errmsg = (char_u *)"write error (file system full?)";
	/*
	 * If we have a backup file, try to put it in place of the new file,
	 * because it is probably corrupt. This avoids loosing the original
	 * file when trying to make a backup when writing the file a second
	 * time.
	 * For unix this means copying the backup over the new file.
	 * For others this means renaming the backup file.
	 * If this is OK, don't give the extra warning message.
	 */
	if (backup != NULL)
	{
#ifdef UNIX
	    char_u	copybuf[BUFSIZE + 1];
	    int		bfd, buflen;

	    if ((bfd = open((char *)backup, O_RDONLY | O_EXTRA)) >= 0)
	    {
		if ((fd = open((char *)fname,
			  O_WRONLY | O_CREAT | O_TRUNC | O_EXTRA, 0666 )) >= 0)
		{
		    /* copy the file. */
		    while ((buflen = read(bfd, (char *)copybuf, BUFSIZE)) > 0)
			if (write_buf(fd, copybuf, buflen) == FAIL)
			    break;
		    if (close(fd) >= 0 && buflen == 0)	/* success */
			end = 1;
		}
		close(bfd);	/* ignore errors for closing read file */
	    }
#else
	    if (vim_rename(backup, fname) == 0)
		end = 1;
#endif
	}
	goto fail;
    }

    lnum -= start;	    /* compute number of written lines */
    --no_wait_return;	    /* may wait for return now */

#ifndef UNIX
    fname = sfname;	    /* use shortname now, for the messages */
#endif
    if (!filtering)
    {
	msg_add_fname(buf, fname);	/* put fname in IObuff with quotes */
	c = FALSE;
	if (newfile)
	{
	    STRCAT(IObuff, shortmess(SHM_NEW) ? "[New]" : "[New File]");
	    c = TRUE;
	}
	if (no_eol)
	{
	    msg_add_eol();
	    c = TRUE;
	}
	/* may add [unix/dos/mac] */
	if (msg_add_fileformat(fileformat))
	    c = TRUE;
	msg_add_lines(c, (long)lnum, nchars);	/* add line/char count */
	if (!shortmess(SHM_WRITE))
	    STRCAT(IObuff, shortmess(SHM_WRI) ? " [w]" : " written");

	keep_msg = msg_trunc(IObuff);
	keep_msg_attr = 0;
    }

    if (reset_changed && whole)		/* when written everything */
    {
	UNCHANGED(buf);
	if (buf->b_start_ffc != *buf->b_p_ff)
	{
	    buf->b_start_ffc = *buf->b_p_ff;	/* keep this fileformat */
	    check_status(buf);
	}
	u_unchanged(buf);
    }

    /*
     * If written to the current file, update the timestamp of the swap file
     * and reset the 'notedited' flag. Also sets buf->b_mtime.
     */
    if (!exiting && overwriting)
    {
	ml_timestamp(buf);
	buf->b_notedited = FALSE;
    }

    /*
     * If we kept a backup until now, and we are in patch mode, then we make
     * the backup file our 'original' file.
     */
    if (*p_pm)
    {
	char *org = (char *)buf_modname(
#ifdef SHORT_FNAME
					TRUE,
#else
					(buf->b_p_sn || buf->b_shortname),
#endif
							  fname, p_pm, FALSE);

	if (backup != NULL)
	{
	    struct stat st;

	    /*
	     * If the original file does not exist yet
	     * the current backup file becomes the original file
	     */
	    if (org == NULL)
		EMSG("patchmode: can't save original file");
	    else if (stat(org, &st) < 0)
	    {
		vim_rename(backup, (char_u *)org);
		vim_free(backup);	    /* don't delete the file */
		backup = NULL;
#ifdef UNIX
		set_file_time((char_u *)org, st_old.st_atime, st_old.st_mtime);
#endif
	    }
	}
	/*
	 * If there is no backup file, remember that a (new) file was
	 * created.
	 */
	else
	{
	    int empty_fd;

	    if (org == NULL || (empty_fd = open(org, O_CREAT | O_EXTRA
#ifndef macintosh
				, 0666
#endif
					)) < 0)
	      EMSG("patchmode: can't touch empty original file");
	    else
	      close(empty_fd);
	}
	if (org != NULL)
	{
	    mch_setperm((char_u *)org, mch_getperm(fname) & 0777);
	    vim_free(org);
	}
    }

    /*
     * Remove the backup unless 'backup' option is set
     */
    if (!p_bk && backup != NULL && vim_remove(backup) != 0)
	EMSG("Can't delete backup file");

    goto nofail;

fail:
    --no_wait_return;	    /* may wait for return now */
nofail:

    vim_free(backup);
    if (buffer != smallbuf)
	vim_free(buffer);

    if (errmsg != NULL)
    {
	attr = highlight_attr[HLF_E];	/* set highlight for error messages */
	msg_add_fname(buf,
#ifndef UNIX
		sfname
#else
		fname
#endif
		     );		    /* put file name in IObuff with quotes */
	STRCAT(IObuff, errmsg);
	emsg(IObuff);

	retval = FAIL;
	if (end == 0)
	{
	    MSG_PUTS_ATTR("\nWARNING: Original file may be lost or damaged\n",
		    attr);
	    MSG_PUTS_ATTR("don't quit the editor until the file is successfully written!",
		    attr);
	}
    }
    msg_scroll = msg_save;

#ifdef AUTOCMD
    write_no_eol_lnum = 0;	/* in case it was set by the previous read */

    /*
     * Apply POST autocommands.
     * Careful: The autocommands may call buf_write() recursively!
     */
    save_buf = curbuf;
    curbuf = buf;
    curwin->w_buffer = buf;
    if (append)
	apply_autocmds(EVENT_FILEAPPENDPOST, fname, fname, FALSE);
    else if (filtering)
	apply_autocmds(EVENT_FILTERWRITEPOST, NULL, fname, FALSE);
    else if (reset_changed && whole)
	apply_autocmds(EVENT_BUFWRITEPOST, fname, fname, FALSE);
    else
	apply_autocmds(EVENT_FILEWRITEPOST, fname, fname, FALSE);
    /*
     * If the autocommands didn't change the current buffer, go back to the
     * original current buffer, if it still exists.
     */
    if (curbuf == buf && buf_valid(save_buf))
    {
	curbuf = save_buf;
	curwin->w_buffer = save_buf;
    }
#endif

    return retval;
}

/*
 * Put file name into IObuff with quotes.
 */
    static void
msg_add_fname(buf, fname)
    BUF	    *buf;
    char_u  *fname;
{
    if (fname == NULL)
	fname = (char_u *)"-stdin-";
    home_replace(buf, fname, IObuff + 1, IOSIZE - 1);
    IObuff[0] = '"';
    STRCAT(IObuff, "\" ");
}

/*
 * Append message for text mode to IObuff.
 * Return TRUE if something appended.
 */
    static int
msg_add_fileformat(eol_type)
    int	    eol_type;
{
#ifndef USE_CRNL
    if (eol_type == EOL_DOS)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? "[dos]" : "[dos format]");
	return TRUE;
    }
#endif
#ifndef USE_CR
    if (eol_type == EOL_MAC)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? "[mac]" : "[mac format]");
	return TRUE;
    }
#endif
#if defined(USE_CRNL) || defined(USE_CR)
    if (eol_type == EOL_UNIX)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? "[unix]" : "[unix format]");
	return TRUE;
    }
#endif
    return FALSE;
}

/*
 * Append line and character count to IObuff.
 */
    static void
msg_add_lines(insert_space, lnum, nchars)
    int	    insert_space;
    long    lnum;
    long    nchars;
{
    char_u  *p;

    p = IObuff + STRLEN(IObuff);

    if (insert_space)
	*p++ = ' ';
    if (shortmess(SHM_LINES))
	sprintf((char *)p, "%ldL, %ldC", lnum, nchars);
    else
	sprintf((char *)p, "%ld line%s, %ld character%s",
	    lnum, plural(lnum),
	    nchars, plural(nchars));
}

/*
 * Append message for missing line separator to IObuff.
 */
    static void
msg_add_eol()
{
    STRCAT(IObuff, shortmess(SHM_LAST) ? "[noeol]" : "[Incomplete last line]");
}

/*
 * Check modification time of file, before writing to it.
 */
    static int
check_mtime(buf, st)
    BUF			*buf;
    struct stat		*st;
{
    if (buf->b_mtime_read != 0
#ifdef __linux__
	    /* In Linux, on a FAT filesystem, there are only 5 bits to store
	     * the seconds.  Since the roundoff is done when flushing the
	     * inode, the time may change unexpectedly by one second!!! */
	    && st->st_mtime - buf->b_mtime_read > 1
#else
	    && buf->b_mtime_read != st->st_mtime
#endif
	    )
    {
	msg_scroll = TRUE;	    /* don't overwrite messages here */
	/* don't use emsg() here, don't want to flush the buffers */
	MSG_ATTR("WARNING: The file has been changed since reading it!!!",
						       highlight_attr[HLF_E]);
	if (ask_yesno((char_u *)"Do you really want to write to it",
								 TRUE) == 'n')
	    return FAIL;
	msg_scroll = FALSE;	    /* always overwrite the file message now */
    }
    return OK;
}

/*
 * write_buf: call write() to write a buffer
 *
 * return FAIL for failure, OK otherwise
 */
    static int
write_buf(fd, buf, len)
    int	    fd;
    char_u  *buf;
    int	    len;
{
    int	    wlen;

    while (len)
    {
	wlen = write(fd, (char *)buf, (size_t)len);
	if (wlen <= 0)		    /* error! */
	    return FAIL;
	len -= wlen;
	buf += wlen;
    }
    return OK;
}

/*
 * shorten_fname: Try to find a shortname by comparing the fullname with the
 * current directory.
 * Returns NULL if not shorter name possible, pointer into "full_path"
 * otherwise.
 */
    char_u *
shorten_fname(full_path, dir_name)
    char_u  *full_path;
    char_u  *dir_name;
{
    int		    len;
    char_u	    *p;

    if (full_path == NULL)
	return NULL;
    len = STRLEN(dir_name);
    if (fnamencmp(dir_name, full_path, len) == 0)
    {
	p = full_path + len;
	if (vim_ispathsep(*p))
	    ++p;
	else
	    p = NULL;
    }
    else
	p = NULL;
    return p;
}

/*
 * add extention to file name - change path/fo.o.h to path/fo.o.h.ext or
 * fo_o_h.ext for MSDOS or when shortname option set.
 *
 * Assumed that fname is a valid name found in the filesystem we assure that
 * the return value is a different name and ends in 'ext'.
 * "ext" MUST be at most 4 characters long if it starts with a dot, 3
 * characters otherwise.
 * Space for the returned name is allocated, must be freed later.
 * Returns NULL when out of memory.
 */
    char_u *
modname(fname, ext, prepend_dot)
    char_u *fname, *ext;
    int	    prepend_dot;	/* may prepend a '.' to file name */
{
    return buf_modname(
#ifdef SHORT_FNAME
			TRUE,
#else
			(curbuf->b_p_sn || curbuf->b_shortname),
#endif
						     fname, ext, prepend_dot);
}

    char_u *
buf_modname(shortname, fname, ext, prepend_dot)
    int	    shortname;		/* use 8.3 file name */
    char_u  *fname, *ext;
    int	    prepend_dot;	/* may prepend a '.' to file name (for Unix) */
{
    char_u	*retval;
    char_u	*s;
    char_u	*e;
    char_u	*ptr;
    int		fnamelen, extlen;

    extlen = STRLEN(ext);

    /*
     * If there is no file name we must get the name of the current directory
     * (we need the full path in case :cd is used).
     */
    if (fname == NULL || *fname == NUL)
    {
	retval = alloc((unsigned)(MAXPATHL + extlen + 3));
	if (retval == NULL)
	    return NULL;
	if (mch_dirname(retval, MAXPATHL) == FAIL ||
					     (fnamelen = STRLEN(retval)) == 0)
	{
	    vim_free(retval);
	    return NULL;
	}
	if (!vim_ispathsep(retval[fnamelen - 1]))
	{
	    retval[fnamelen++] = PATHSEP;
	    retval[fnamelen] = NUL;
	}
#ifndef SHORT_FNAME
	prepend_dot = FALSE;	    /* nothing to prepend a dot to */
#endif
    }
    else
    {
	fnamelen = STRLEN(fname);
	retval = alloc((unsigned)(fnamelen + extlen + 3));
	if (retval == NULL)
	    return NULL;
	STRCPY(retval, fname);
    }

    /*
     * search backwards until we hit a '/', '\' or ':' replacing all '.'
     * by '_' for MSDOS or when shortname option set and ext starts with a dot.
     * Then truncate what is after the '/', '\' or ':' to 8 characters for
     * MSDOS and 26 characters for AMIGA, a lot more for UNIX.
     */
    for (ptr = retval + fnamelen; ptr >= retval; ptr--)
    {
	if (*ext == '.'
#ifdef USE_LONG_FNAME
		    && (!USE_LONG_FNAME || shortname)
#else
# ifndef SHORT_FNAME
		    && shortname
# endif
#endif
								)
	    if (*ptr == '.')	/* replace '.' by '_' */
		*ptr = '_';
	if (vim_ispathsep(*ptr))
	    break;
    }
    ptr++;

    /* the file name has at most BASENAMELEN characters. */
#ifndef SHORT_FNAME
    if (STRLEN(ptr) > (unsigned)BASENAMELEN)
	ptr[BASENAMELEN] = '\0';
#endif

    s = ptr + STRLEN(ptr);

    /*
     * For 8.3 file names we may have to reduce the length.
     */
#ifdef USE_LONG_FNAME
    if (!USE_LONG_FNAME || shortname)
#else
# ifndef SHORT_FNAME
    if (shortname)
# endif
#endif
    {
	/*
	 * If there is no file name, and the extension starts with '.', put a
	 * '_' before the dot, because just ".ext" is invalid.
	 */
	if (fname == NULL || *fname == NUL)
	{
	    if (*ext == '.')
		*s++ = '_';
	}
	/*
	 * If the extension starts with '.', truncate the base name at 8
	 * characters
	 */
	else if (*ext == '.')
	{
	    if (s - ptr > (size_t)8)
	    {
		s = ptr + 8;
		*s = '\0';
	    }
	}
	/*
	 * If the extension doesn't start with '.', and the file name
	 * doesn't have an extension yet, append a '.'
	 */
	else if ((e = vim_strchr(ptr, '.')) == NULL)
	    *s++ = '.';
	/*
	 * If If the extension doesn't start with '.', and there already is an
	 * extension, it may need to be tructated
	 */
	else if ((int)STRLEN(e) + extlen > 4)
	    s = e + 4 - extlen;
    }
#if defined(OS2) || defined(USE_LONG_FNAME) || defined(WIN32)
    /*
     * If there is no file name, and the extension starts with '.', put a
     * '_' before the dot, because just ".ext" may be invalid if it's on a
     * FAT partition, and on HPFS it doesn't matter.
     */
    else if ((fname == NULL || *fname == NUL) && *ext == '.')
	*s++ = '_';
#endif

    /*
     * Append the extention.
     * ext can start with '.' and cannot exceed 3 more characters.
     */
    STRCPY(s, ext);

#ifndef SHORT_FNAME
    /*
     * Prepend the dot.
     */
    if (prepend_dot && !shortname && *(e = gettail(retval)) != '.'
#ifdef USE_LONG_FNAME
	    && USE_LONG_FNAME
#endif
				)
    {
	vim_memmove(e + 1, e, STRLEN(e) + 1);
	*e = '.';
    }
#endif

    /*
     * Check that, after appending the extension, the file name is really
     * different.
     */
    if (fname != NULL && STRCMP(fname, retval) == 0)
    {
	/* we search for a character that can be replaced by '_' */
	while (--s >= ptr)
	{
	    if (*s != '_')
	    {
		*s = '_';
		break;
	    }
	}
	if (s < ptr)	/* fname was "________.<ext>" how tricky! */
	    *ptr = 'v';
    }
    return retval;
}

/* vim_fgets();
 *
 * Like fgets(), but if the file line is too long, it is truncated and the
 * rest of the line is thrown away.  Returns TRUE for end-of-file.
 * Note: do not pass IObuff as the buffer since this is used to read and
 * discard the extra part of any long lines.
 */
    int
vim_fgets(buf, size, fp)
    char_u	*buf;
    int		size;
    FILE	*fp;
{
    char *eof;

    buf[size - 2] = NUL;
    eof = fgets((char *)buf, size, fp);
    if (buf[size - 2] != NUL && buf[size - 2] != '\n')
    {
	buf[size - 1] = NUL;	    /* Truncate the line */

	/* Now throw away the rest of the line: */
	do
	{
	    IObuff[IOSIZE - 2] = NUL;
	    fgets((char *)IObuff, IOSIZE, fp);
	} while (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != '\n');
    }
    return (eof == NULL);
}

/*
 * rename() only works if both files are on the same file system, this
 * function will (attempts to?) copy the file across if rename fails -- webb
 * Return -1 for failure, 0 for success.
 */
    int
vim_rename(from, to)
    char_u *from;
    char_u *to;
{
    int	    fd_in;
    int	    fd_out;
    int	    n;
    char    *errmsg = NULL;

    /*
     * First delete the "to" file, this is required on some systems to make
     * the rename() work, on other systems it makes sure that we don't have
     * two files when the rename() fails.
     */
    vim_remove(to);

    /*
     * First try a normal rename, return if it works.
     */
    if (rename((char *)from, (char *)to) == 0)
	return 0;

    /*
     * Rename() failed, try copying the file.
     */
    fd_in = open((char *)from, O_RDONLY | O_EXTRA);
    if (fd_in == -1)
	return -1;
    fd_out = open((char *)to, O_CREAT | O_TRUNC | O_WRONLY | O_EXTRA
#ifndef macintosh
			, 0666
#endif
			);
    if (fd_out == -1)
    {
	close(fd_in);
	return -1;
    }
    while ((n = read(fd_in, (char *)IObuff, (size_t)IOSIZE)) > 0)
	if (write(fd_out, (char *)IObuff, (size_t)n) != n)
	{
	    errmsg = "writing to";
	    break;
	}
    close(fd_in);
    if (close(fd_out) < 0)
	errmsg = "closing";
    if (n < 0)
    {
	errmsg = "reading";
	to = from;
    }
    if (errmsg != NULL)
    {
	sprintf((char *)IObuff, "Error %s '%s'", errmsg, to);
	emsg(IObuff);
	return -1;
    }
    vim_remove(from);
    return 0;
}

/*
 * Check if any not hidden buffer has been changed.
 * Postpone the check if there are characters in the stuff buffer, a global
 * command is being executed, a mapping is being executed or an autocommand is
 * busy.
 */
    void
check_timestamps()
{
    BUF	    *buf;

    if (!stuff_empty() || global_busy || !typebuf_typed()
#ifdef AUTOCMD
			|| autocmd_busy
#endif
					)
	need_check_timestamps = TRUE;		/* check later */
    else
    {
	++no_wait_return;
	for (buf = firstbuf; buf != NULL; buf = buf->b_next)
	    buf_check_timestamp(buf);
	--no_wait_return;
	need_check_timestamps = FALSE;
    }
}

/*
 * Check if buffer "buf" has been changed.
 */
    void
buf_check_timestamp(buf)
    BUF	    *buf;
{
    struct stat	    st;
    char_u	    *path;

    if (    buf->b_ffname != NULL
	    && buf->b_ml.ml_mfp != NULL
	    && !buf->b_notedited
	    && buf->b_mtime != 0
	    && stat((char *)buf->b_ffname, &st) >= 0
#ifdef __linux__
	    /* In Linux, on a FAT filesystem, there are only 5 bits to
	     * store the seconds.  Since the roundoff is done when
	     * flushing the inode, the time may change unexpectedly by one
	     * second!!! */
	    && st.st_mtime - buf->b_mtime > 1
#else
	    && buf->b_mtime != st.st_mtime
#endif
	    )
    {
#ifdef AUTOCMD
	/*
	 * Only give the warning if there are no FileChangedShell autocommands.
	 */
	if (!apply_autocmds(EVENT_FILECHANGEDSHELL, NULL, NULL, FALSE))
#endif
	{
	    path = home_replace_save(buf, buf->b_fname);
	    if (path != NULL)
	    {
		EMSG2("Warning: File \"%s\" has changed since editing started",
								    path);
		vim_free(path);
	    }
	}
	buf->b_mtime = st.st_mtime;
    }
}

/*
 * Adjust the line with missing eol, used for the next write.
 * Used for do_filter(), when the input lines for the filter are deleted.
 */
    void
write_lnum_adjust(offset)
    linenr_t	offset;
{
    if (write_no_eol_lnum)		/* only if there is a missing eol */
	write_no_eol_lnum += offset;
}

/*
 * vim_tempname(): Return a unique name that can be used for a temp file.
 *
 * The temp file is NOT created.
 *
 * The returned pointer is to allocated memory.
 * The returned pointer is NULL if no valid name was found.
 */
    char_u  *
vim_tempname(extra_char)
    int	    extra_char;	    /* character to use in the name instead of '?' */
{
#ifdef USE_GUI_WIN32
# undef USE_TMPNAM
# define TEMPNAMELEN 256
    char	szTempFile[_MAX_PATH+1];
#endif
#ifdef USE_TMPNAM
    char_u	itmp[L_tmpnam];	/* use tmpnam() */
#else
    char_u	itmp[TEMPNAMELEN];
#endif
#if defined(TEMPDIRNAMES) || (!defined(USE_GUI_WIN32) && !defined(USE_TMPNAM))
    char_u	    *p;
#endif

#ifdef TEMPDIRNAMES
    static char	*(tempdirs[]) = {TEMPDIRNAMES};
    static int	first_dir = 0;
    int		first_try = TRUE;
    int		i;

    /*
     * Try a few places to put the temp file.
     * To avoid waisting time with non-existing environment variables and
     * directories, they are skipped next time.
     */
    for (i = first_dir; i < sizeof(tempdirs) / sizeof(char *); ++i)
    {
	/* expand $TMP, leave room for '/', "v?XXXXXX" and NUL */
	expand_env((char_u *)tempdirs[i], itmp, TEMPNAMELEN - 10);
	if (mch_isdir(itmp))		/* directory exists */
	{
	    if (first_try)
		first_dir = i;		/* start here next time */
	    first_try = FALSE;
#ifdef __EMX__
	    /*
	     * if $TMP contains a forward slash (perhaps because we're using
	     * bash or tcsh, right Stefan?), don't add a backslash to the
	     * directory before tacking on the file name; use a forward slash!
	     * I first tried adding 2 backslashes, but somehow that didn't
	     * work (something in the EMX system() ate them, I think).
	     */
	    if (vim_strchr(itmp, '/'))
		STRCAT(itmp, "/");
	    else
#endif
		STRCAT(itmp, PATHSEPSTR);
	    STRCAT(itmp, TEMPNAME);
	    if ((p = vim_strchr(itmp, '?')) != NULL)
		*p = extra_char;
	    if (*mktemp((char *)itmp) == NUL)
		continue;
	    return vim_strsave(itmp);
	}
    }
    return NULL;
#else /* !TEMPDIRNAMES */

# ifdef USE_GUI_WIN32
    STRCPY(itmp, "");
    GetTempPath(_MAX_PATH, szTempFile);
    if (GetTempFileName(szTempFile, "VIM", 0, itmp) == 0)
# else
#  ifdef USE_TMPNAM
    /* tmpnam() will make its own name */
    if (*tmpnam((char *)itmp) == NUL)
#  else
    STRCPY(itmp, TEMPNAME);
    if ((p = vim_strchr(itmp, '?')) != NULL)
	*p = extra_char;
    if (*mktemp((char *)itmp) == NUL)
#  endif
# endif
	return NULL;
# ifdef WIN32
    {
	char_u	*p;
	char_u	*retval;

	/* backslashes in a temp file name cause problems when filtering with
	 * "sh" */
	retval = vim_strsave(itmp);
	if (*p_shcf == '-')
	    for (p = retval; *p; ++p)
		if (*p == '\\')
		    *p = '/';
	return retval;
    }
# else
    return vim_strsave(itmp);
# endif
#endif /* !TEMPDIRNAMES */
}

/*
 * Code for automatic commands.
 *
 * Only included when "AUTOCMD" has been defined.
 */

#ifdef AUTOCMD

/*
 * The autocommands are stored in a list for each event.
 * Autocommands for the same pattern, that are consecutive, are joined
 * together, to avoid having to match the pattern too often.
 * The result is an array of Autopat lists, which point to AutoCmd lists:
 *
 * first_autopat[0] --> Autopat.next  -->  Autopat.next -->  NULL
 *			Autopat.cmds	   Autopat.cmds
 *			    |			 |
 *			    V			 V
 *			AutoCmd.next	   AutoCmd.next
 *			    |			 |
 *			    V			 V
 *			AutoCmd.next		NULL
 *			    |
 *			    V
 *			   NULL
 *
 * first_autopat[1] --> Autopat.next  -->  NULL
 *			Autopat.cmds
 *			    |
 *			    V
 *			AutoCmd.next
 *			    |
 *			    V
 *			   NULL
 *   etc.
 *
 *   The order of AutoCmds is important, this is the order in which they were
 *   defined and will have to be executed.
 */
typedef struct AutoCmd
{
    char_u	    *cmd;		/* The command to be executed (NULL
					   when command has been removed) */
    char	    nested;		/* If autocommands nest here */
    char	    last;		/* last command in list */
    struct AutoCmd  *next;		/* Next AutoCmd in list */
} AutoCmd;

typedef struct AutoPat
{
    int		    group;		/* group ID */
    char_u	    *pat;		/* pattern as typed (NULL when pattern
					   has been removed) */
    char_u	    *reg_pat;		/* pattern converted to regexp */
    char	    allow_dirs;		/* Pattern may match whole path */
    char	    last;		/* last pattern for apply_autocmds() */
    AutoCmd	    *cmds;		/* list of commands to do */
    struct AutoPat  *next;		/* next AutoPat in AutoPat list */
} AutoPat;

static struct event_name
{
    char	*name;	    /* event name */
    EVENT_T	event;	    /* event number */
} event_names[] =
{
    {"BufDelete",	EVENT_BUFDELETE},
    {"BufEnter",	EVENT_BUFENTER},
    {"BufLeave",	EVENT_BUFLEAVE},
    {"BufNewFile",	EVENT_BUFNEWFILE},
    {"BufReadPost",	EVENT_BUFREADPOST},
    {"BufReadPre",	EVENT_BUFREADPRE},
    {"BufRead",		EVENT_BUFREADPOST},
    {"BufUnload",	EVENT_BUFUNLOAD},
    {"BufWritePost",	EVENT_BUFWRITEPOST},
    {"BufWritePre",	EVENT_BUFWRITEPRE},
    {"BufWrite",	EVENT_BUFWRITEPRE},
    {"FileAppendPost",	EVENT_FILEAPPENDPOST},
    {"FileAppendPre",	EVENT_FILEAPPENDPRE},
    {"FileChangedShell",    EVENT_FILECHANGEDSHELL},
    {"FileReadPost",	EVENT_FILEREADPOST},
    {"FileReadPre",	EVENT_FILEREADPRE},
    {"FileWritePost",	EVENT_FILEWRITEPOST},
    {"FileWritePre",	EVENT_FILEWRITEPRE},
    {"FilterReadPost",	EVENT_FILTERREADPOST},
    {"FilterReadPre",	EVENT_FILTERREADPRE},
    {"FilterWritePost",	EVENT_FILTERWRITEPOST},
    {"FilterWritePre",	EVENT_FILTERWRITEPRE},
    {"StdinReadPost",	EVENT_STDINREADPOST},
    {"StdinReadPre",	EVENT_STDINREADPRE},
    {"TermChanged",	EVENT_TERMCHANGED},
    {"User",		EVENT_USER},
    {"VimLeave",	EVENT_VIMLEAVE},
    {"WinEnter",	EVENT_WINENTER},
    {"WinLeave",	EVENT_WINLEAVE},
    {NULL,	    0}
};

static AutoPat *first_autopat[NUM_EVENTS] =
{
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL
};

/*
 * struct used to keep status while executing autocommands for an event.
 */
typedef struct AutoPatCmd
{
    AutoPat	*curpat;	/* next AutoPat to examine */
    AutoCmd	*nextcmd;	/* next AutoCmd to execute */
    int		group;		/* group being used */
    char_u	*fname;		/* fname to match with */
    char_u	*sfname;	/* sfname to match with */
    char_u	*tail;		/* tail of fname */
    EVENT_T	event;		/* current event */
} AutoPatCmd;

/*
 * augroups stores a list of autocmd group names.
 */
struct growarray augroups = {0, 0, sizeof(char_u *), 10, NULL};
#define AUGROUP_NAME(i) (((char_u **)augroups.ga_data)[i])

/*
 * The ID of the current group.  Group 0 is the default one.
 */
#define AUGROUP_DEFAULT	    -1	    /* default autocmd group */
#define AUGROUP_ERROR	    -2	    /* errornouse autocmd group */
#define AUGROUP_ALL	    -3	    /* all autocmd groups */
static int current_augroup = AUGROUP_DEFAULT;

static int au_need_clean = FALSE;   /* need to delete marked patterns */

static void show_autocmd __ARGS((AutoPat *ap, EVENT_T event));
static void au_remove_pat __ARGS((AutoPat *ap));
static void au_remove_cmds __ARGS((AutoPat *ap));
static void au_cleanup __ARGS((void));
static int au_find_group __ARGS((char_u *name));
static int au_new_group __ARGS((char_u *name));
static int au_find_group __ARGS((char_u *name));
static EVENT_T event_name2nr __ARGS((char_u *start, char_u **end));
static char_u *event_nr2name __ARGS((EVENT_T event));
static char_u *find_end_event __ARGS((char_u *arg));
static int event_ignored __ARGS((EVENT_T event));
static int au_get_grouparg __ARGS((char_u **argp));
static int do_autocmd_event __ARGS((EVENT_T event, char_u *pat, int nested, char_u *cmd, int forceit, int group));
static int apply_autocmds_group __ARGS((EVENT_T event, char_u *fname,
				     char_u *fname_io, int force, int group));
static char_u *getnextac __ARGS((int c, void *cookie, int indent));
static void auto_next_pat __ARGS((AutoPatCmd *apc, int stop_at_last));

static EVENT_T	last_event;
static int	last_group;

/*
 * Show the autocommands for one AutoPat.
 */
    static void
show_autocmd(ap, event)
    AutoPat	*ap;
    EVENT_T	event;
{
    AutoCmd *ac;

    if (got_int)			/* "q" hit for "--more--" */
	return;
    if (ap->pat == NULL)		/* pattern has been removed */
	return;

    msg_putchar('\n');
    if (got_int)			/* "q" hit for "--more--" */
	return;
    if (event != last_event || ap->group != last_group)
    {
	if (ap->group != AUGROUP_DEFAULT)
	{
	    msg_puts_attr(AUGROUP_NAME(ap->group), highlight_attr[HLF_T]);
	    msg_puts((char_u *)"  ");
	}
	msg_puts_attr(event_nr2name(event), highlight_attr[HLF_T]);
	last_event = event;
	last_group = ap->group;
	msg_putchar('\n');
	if (got_int)			/* "q" hit for "--more--" */
	    return;
    }
    msg_col = 4;
    msg_outtrans(ap->pat);

    for (ac = ap->cmds; ac != NULL; ac = ac->next)
    {
	if (ac->cmd != NULL)		/* skip removed commands */
	{
	    if (msg_col >= 14)
		msg_putchar('\n');
	    msg_col = 14;
	    if (got_int)		/* "q" hit for "--more--" */
		return;
	    msg_outtrans(ac->cmd);
	    if (got_int)		/* "q" hit for "--more--" */
		return;
	    if (ac->next != NULL)
	    {
		msg_putchar('\n');
		if (got_int)		/* "q" hit for "--more--" */
		    return;
	    }
	}
    }
}

/*
 * Mark an autocommand pattern for deletion.
 */
    static void
au_remove_pat(ap)
    AutoPat *ap;
{
    vim_free(ap->pat);
    ap->pat = NULL;
    au_need_clean = TRUE;
}

/*
 * Mark all commands for a pattern for deletion.
 */
    static void
au_remove_cmds(ap)
    AutoPat *ap;
{
    AutoCmd *ac;

    for (ac = ap->cmds; ac != NULL; ac = ac->next)
    {
	vim_free(ac->cmd);
	ac->cmd = NULL;
    }
    au_need_clean = TRUE;
}

/*
 * Cleanup autocommands and patterns that have been deleted.
 * This is only done when not executing autocommands.
 */
    static void
au_cleanup()
{
    AutoPat	*ap, **prev_ap;
    AutoCmd	*ac, **prev_ac;
    EVENT_T	event;

    if (autocmd_busy || !au_need_clean)
	return;

    /* loop over all events */
    for (event = (EVENT_T)0; event < NUM_EVENTS;
					    event = (EVENT_T)((int)event + 1))
    {
	/* loop over all autocommand patterns */
	prev_ap = &(first_autopat[event]);
	for (ap = *prev_ap; ap != NULL; ap = *prev_ap)
	{
	    /* loop over all commands for this pattern */
	    prev_ac = &(ap->cmds);
	    for (ac = *prev_ac; ac != NULL; ac = *prev_ac)
	    {
		/* remove the command if the pattern is to be deleted or when
		 * the command has been marked for deletion */
		if (ap->pat == NULL || ac->cmd == NULL)
		{
		    *prev_ac = ac->next;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		else
		    prev_ac = &(ac->next);
	    }

	    /* remove the pattern if it has been marked for deletion */
	    if (ap->pat == NULL)
	    {
		*prev_ap = ap->next;
		vim_free(ap->reg_pat);
		vim_free(ap);
	    }
	    else
		prev_ap = &(ap->next);
	}
    }

    au_need_clean = FALSE;
}

/*
 * Add an autocmd group name.
 * Return it's ID.  Returns AUGROUP_ERROR for error.
 */
    static int
au_new_group(name)
    char_u	*name;
{
    int	    i;

    i = au_find_group(name);
    if (i == AUGROUP_ERROR)	/* the group doesn't exist yet, add it */
    {
	if (ga_grow(&augroups, 1) == FAIL)
	    return AUGROUP_ERROR;
	i = augroups.ga_len;
	AUGROUP_NAME(i) = vim_strsave(name);
	if (AUGROUP_NAME(i) == NULL)
	    return AUGROUP_ERROR;
	++augroups.ga_len;
	--augroups.ga_room;
    }

    return i;
}

/*
 * Find the ID of an autocmd group name.
 * Return it's ID.  Returns AUGROUP_ERROR for error.
 */
    static int
au_find_group(name)
    char_u	*name;
{
    int	    i;

    for (i = 0; i < augroups.ga_len; ++i)
    {
	if (AUGROUP_NAME(i) != NULL && STRCMP(AUGROUP_NAME(i), name) == 0)
	    return i;
    }
    return AUGROUP_ERROR;
}

/*
 * Implementation of the ":augroup name" command.
 */
    void
do_augroup(arg)
    char_u	*arg;
{
    int	    i;

    if (STRICMP(arg, "end") == 0)   /* ":aug end": back to group 0 */
	current_augroup = AUGROUP_DEFAULT;
    else if (*arg)		    /* ":aug xxx": switch to group xxx */
    {
	i = au_new_group(arg);
	if (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    else			    /* ":aug": list the group names */
    {
	msg_start();
	for (i = 0; i < augroups.ga_len; ++i)
	{
	    if (AUGROUP_NAME(i) != NULL)
	    {
		msg_puts(AUGROUP_NAME(i));
		msg_puts((char_u *)"  ");
	    }
	}
	msg_clr_eos();
	msg_end();
    }
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(idx)
    int	    idx;
{
    if (idx == augroups.ga_len)		/* add "END" add the end */
	return (char_u *)"END";
    if (idx >= augroups.ga_len)		/* end of list */
	return NULL;
    if (AUGROUP_NAME(idx) == NULL)	/* skip deleted entries */
	return (char_u *)"";
    return AUGROUP_NAME(idx);		/* return a name */
}

/*
 * Return the event number for event name "start".
 * Return NUM_EVENTS if the event name was not found.
 * Return a pointer to the next event name in "end".
 */
    static EVENT_T
event_name2nr(start, end)
    char_u  *start;
    char_u  **end;
{
    char_u	*p;
    int		i;
    int		len;

    /* the event name ends with end of line, a blank or a comma */
    for (p = start; *p && !vim_iswhite(*p) && *p != ','; ++p)
	;
    for (i = 0; event_names[i].name != NULL; ++i)
    {
	len = strlen(event_names[i].name);
	if (len == p - start && STRNICMP(event_names[i].name, start, len) == 0)
	    break;
    }
    if (*p == ',')
	++p;
    *end = p;
    if (event_names[i].name == NULL)
	return NUM_EVENTS;
    return event_names[i].event;
}

/*
 * Return the name for event "event".
 */
    static char_u *
event_nr2name(event)
    EVENT_T	event;
{
    int	    i;

    for (i = 0; event_names[i].name != NULL; ++i)
	if (event_names[i].event == event)
	    return (char_u *)event_names[i].name;
    return (char_u *)"Unknown";
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    static char_u *
find_end_event(arg)
    char_u  *arg;
{
    char_u  *pat;
    char_u  *p;

    if (*arg == '*')
    {
	if (arg[1] && !vim_iswhite(arg[1]))
	{
	    EMSG2("Illegal character after *: %s", arg);
	    return NULL;
	}
	pat = arg + 1;
    }
    else
    {
	for (pat = arg; *pat && !vim_iswhite(*pat); pat = p)
	{
	    if (event_name2nr(pat, &p) >= NUM_EVENTS)
	    {
		EMSG2("No such event: %s", pat);
		return NULL;
	    }
	}
    }
    return pat;
}

/*
 * Return TRUE if "event" is included in 'eventignore'.
 */
    static int
event_ignored(event)
    EVENT_T	event;
{
    char_u	*p = p_ei;

    if (STRICMP(p_ei, "all") == 0)
	return TRUE;

    while (*p)
	if (event_name2nr(p, &p) == event)
	    return TRUE;

    return FALSE;
}

/*
 * Return OK when the contents of p_ei is valid, FAIL otherwise.
 */
    int
check_ei()
{
    char_u	*p = p_ei;

    if (STRICMP(p_ei, "all") == 0)
	return OK;

    while (*p)
	if (event_name2nr(p, &p) == NUM_EVENTS)
	    return FAIL;

    return OK;
}

/*
 * do_autocmd() -- implements the :autocmd command.  Can be used in the
 *  following ways:
 *
 * :autocmd <event> <pat> <cmd>	    Add <cmd> to the list of commands that
 *				    will be automatically executed for <event>
 *				    when editing a file matching <pat>, in
 *				    the current group.
 * :autocmd <event> <pat>	    Show the auto-commands associated with
 *				    <event> and <pat>.
 * :autocmd <event>		    Show the auto-commands associated with
 *				    <event>.
 * :autocmd			    Show all auto-commands.
 * :autocmd! <event> <pat> <cmd>    Remove all auto-commands associated with
 *				    <event> and <pat>, and add the command
 *				    <cmd>, for the current group.
 * :autocmd! <event> <pat>	    Remove all auto-commands associated with
 *				    <event> and <pat> for the current group.
 * :autocmd! <event>		    Remove all auto-commands associated with
 *				    <event> for the current group.
 * :autocmd!			    Remove ALL auto-commands for the current
 *				    group.
 *
 *  Multiple events and patterns may be given separated by commas.  Here are
 *  some examples:
 * :autocmd bufread,bufenter *.c,*.h	set tw=0 smartindent noic
 * :autocmd bufleave	     *		set tw=79 nosmartindent ic infercase
 *
 * :autocmd * *.c		show all autocommands for *.c files.
 */
    void
do_autocmd(arg, forceit)
    char_u  *arg;
    int	    forceit;
{
    char_u	*pat;
    char_u	*cmd;
    EVENT_T	event;
    int		need_free = FALSE;
    int		nested = FALSE;
    int		group;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    group = au_get_grouparg(&arg);
    if (arg == NULL)	    /* out of memory */
	return;

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = find_end_event(arg);
    if (pat == NULL)
	return;

    /*
     * Scan over the pattern.  Put a NUL at the end.
     */
    pat = skipwhite(pat);
    cmd = pat;
    while (*cmd && (!vim_iswhite(*cmd) || cmd[-1] == '\\'))
	cmd++;
    if (*cmd)
	*cmd++ = NUL;

    /*
     * Check for "nested" flag.
     */
    cmd = skipwhite(cmd);
    if (*cmd != NUL && STRNCMP(cmd, "nested", 6) == 0 && vim_iswhite(cmd[6]))
    {
	nested = TRUE;
	cmd = skipwhite(cmd + 6);
    }

    /*
     * Find the start of the commands.
     * Expand <sfile> in it.
     */
    if (*cmd != NUL)
    {
	cmd = expand_sfile(cmd);
	if (cmd == NULL)	    /* some error */
	    return;
	need_free = TRUE;
    }

    /*
     * Print header when showing autocommands.
     */
    if (!forceit && *cmd == NUL)
    {
	/* Highlight title */
	MSG_PUTS_TITLE("\n--- Auto-Commands ---");
    }

    /*
     * Loop over the events.
     */
    last_event = (EVENT_T)-1;		/* for listing the event name */
    last_group = AUGROUP_ERROR;		/* for listing the group name */
    if (*arg == '*' || *arg == NUL)
    {
	for (event = (EVENT_T)0; event < NUM_EVENTS;
					    event = (EVENT_T)((int)event + 1))
	    if (do_autocmd_event(event, pat,
					 nested, cmd, forceit, group) == FAIL)
		break;
    }
    else
    {
	while (*arg && !vim_iswhite(*arg))
	    if (do_autocmd_event(event_name2nr(arg, &arg), pat,
					nested,	cmd, forceit, group) == FAIL)
		break;
    }

    if (need_free)
	vim_free(cmd);
}

/*
 * Find the group ID in a ":autocmd" or ":doautocmd" argument.
 * The "argp" argument is advanced to the following argument.
 *
 * Returns the group ID, AUGROUP_ERROR for error (out of memory).
 */
    static int
au_get_grouparg(argp)
    char_u	**argp;
{
    char_u	*group_name;
    char_u	*p;
    char_u	*arg = *argp;
    int		group = AUGROUP_ALL;

    p = skiptowhite(arg);
    if (p > arg)
    {
	group_name = vim_strnsave(arg, (int)(p - arg));
	if (group_name == NULL)		/* out of memory */
	    return AUGROUP_ERROR;
	group = au_find_group(group_name);
	if (group == AUGROUP_ERROR)
	    group = AUGROUP_ALL;	/* no match, use all groups */
	else
	    *argp = skipwhite(p);	/* match, skip over group name */
	vim_free(group_name);
    }
    return group;
}

/*
 * do_autocmd() for one event.
 * If *pat == NUL do for all patterns.
 * If *cmd == NUL show entries.
 * If forceit == TRUE delete entries.
 * If group is not AUGROUP_ALL, only use this group.
 */
    static int
do_autocmd_event(event, pat, nested, cmd, forceit, group)
    EVENT_T	event;
    char_u	*pat;
    int		nested;
    char_u	*cmd;
    int		forceit;
    int		group;
{
    AutoPat	*ap;
    AutoPat	**prev_ap;
    AutoCmd	*ac;
    AutoCmd	**prev_ac;
    int		brace_level;
    char_u	*endpat;
    int		len;

    /*
     * Show or delete all patterns for an event.
     */
    if (*pat == NUL)
    {
	for (ap = first_autopat[event]; ap != NULL; ap = ap->next)
	{
	    if (forceit)  /* delete the AutoPat, if it's in the current group */
	    {
		if (ap->group == (group == AUGROUP_ALL ? current_augroup
						       : group))
		    au_remove_pat(ap);
	    }
	    else if (group == AUGROUP_ALL || ap->group == group)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    for ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	for (endpat = pat; *endpat && (*endpat != ',' || brace_level
					     || endpat[-1] == '\\'); ++endpat)
	{
	    if (*endpat == '{')
		brace_level++;
	    else if (*endpat == '}')
		brace_level--;
	}
	if (pat == endpat)		/* ignore single comma */
	    continue;

	/*
	 * Find AutoPat entries with this pattern.
	 */
	prev_ap = &first_autopat[event];
	while ((ap = *prev_ap) != NULL)
	{
	    if (ap->pat != NULL)
	    {
		/* Accept a pattern when:
		 * - a group was specified and it's that group, or a group was
		 *   not specified and it's the current group, or a group was
		 *   not specified and we are listing
		 * - the length of the pattern matches
		 * - the pattern matches
		 */
		len = STRLEN(ap->pat);
		if ((ap->group == (group == AUGROUP_ALL ? current_augroup
							: group)
			   || (group == AUGROUP_ALL && !forceit && *cmd == NUL))
			&& len == endpat - pat
			&& STRNCMP(pat, ap->pat, len) == 0)
		{
		    /*
		     * Remove existing autocommands.
		     * If adding any new autocmd's for this AutoPat, don't
		     * delete the pattern from the autopat list, append to
		     * this list.
		     */
		    if (forceit)
		    {
			if (*cmd != NUL && ap->next == NULL)
			{
			    au_remove_cmds(ap);
			    break;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat
		     */
		    else if (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    else if (ap->next == NULL)
			break;
		}
	    }
	    prev_ap = &ap->next;
	}

	/*
	 * Add a new command.
	 */
	if (*cmd != NUL)
	{
	    /*
	     * If the pattern we want to add a command to does appear at the
	     * end of the list (or not is not in the list at all), add the
	     * pattern at the end of the list.
	     */
	    if (ap == NULL)
	    {
		ap = (AutoPat *)alloc((unsigned)sizeof(AutoPat));
		if (ap == NULL)
		    return FAIL;
		ap->pat = vim_strnsave(pat, (int)(endpat - pat));
		if (ap->pat == NULL)
		{
		    vim_free(ap);
		    return FAIL;
		}
		ap->reg_pat = file_pat_to_reg_pat(pat, endpat, &ap->allow_dirs);
		if (ap->reg_pat == NULL)
		{
		    vim_free(ap->pat);
		    vim_free(ap);
		    return FAIL;
		}
		ap->cmds = NULL;
		*prev_ap = ap;
		ap->next = NULL;
		if (group == AUGROUP_ALL)
		    ap->group = current_augroup;
		else
		    ap->group = group;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    while ((ac = *prev_ac) != NULL)
		prev_ac = &ac->next;
	    ac = (AutoCmd *)alloc((unsigned)sizeof(AutoCmd));
	    if (ac == NULL)
		return FAIL;
	    ac->cmd = vim_strsave(cmd);
	    if (ac->cmd == NULL)
	    {
		vim_free(ac);
		return FAIL;
	    }
	    ac->next = NULL;
	    *prev_ac = ac;
	    ac->nested = nested;
	}
    }

    au_cleanup();	/* may really delete removed patterns/commands now */
    return OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    int
do_doautocmd(arg, do_msg)
    char_u	*arg;
    int		do_msg;	    /* give message for no matching autocmds? */
{
    char_u	*fname;
    int		nothing_done = TRUE;
    int		group;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    group = au_get_grouparg(&arg);
    if (arg == NULL)	    /* out of memory */
	return FAIL;

    if (*arg == '*')
    {
	EMSG("Can't execute autocommands for ALL events");
	return FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = find_end_event(arg);
    if (fname == NULL)
	return FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    while (*arg && !vim_iswhite(*arg))
	if (apply_autocmds_group(event_name2nr(arg, &arg),
						    fname, NULL, TRUE, group))
	    nothing_done = FALSE;

    if (nothing_done && do_msg)
	MSG("No matching autocommands");

    return OK;
}

/*
 * ":doautoall" command: execute autocommands for each loaded buffer.
 */
    void
do_autoall(arg)
    char_u	*arg;
{
    BUF	    *save_win_buf = NULL;
    WIN	    *win;
    WIN	    *save_curwin = NULL;
    int	    retval;

    /*
     * This is a bit tricky: For some commands curwin->w_buffer needs to be
     * equal to curbuf, but for some buffers there may not be a window.
     * So we change the buffer for the current window for a moment.  This
     * gives problems when the autocommands make changes to the list of
     * buffers or windows...
     */
    for (curbuf = firstbuf; curbuf != NULL; curbuf = curbuf->b_next)
    {
	if (curbuf->b_ml.ml_mfp != NULL)
	{
	    for (win = firstwin; win != NULL; win = win->w_next)
		if (win->w_buffer == curbuf)
		    break;
	    /* if there is a window for this buffer, make it the curwin */
	    if (win != NULL)
	    {
		save_curwin = curwin;
		curwin = win;
	    }
	    else
	    {
		/* if there is no window for this buffer, use curwin */
		--curwin->w_buffer->b_nwindows;
		save_win_buf = curwin->w_buffer;
		curwin->w_buffer = curbuf;
		++curwin->w_buffer->b_nwindows;
	    }
	    retval = do_doautocmd(arg, FALSE);
	    do_modelines();
	    if (win != NULL)	    /* restore curwin */
	    {
		if (win_valid(save_curwin))
		    curwin = save_curwin;
	    }
	    else		    /* restore buffer for curwin */
	    {
		if (buf_valid(save_win_buf))
		{
		    --curwin->w_buffer->b_nwindows;
		    curwin->w_buffer = save_win_buf;
		    ++curwin->w_buffer->b_nwindows;
		}
	    }
	    if (retval == FAIL)
		break;
	}
    }

    curbuf = curwin->w_buffer;
}

static int	autocmd_nested = FALSE;

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    int
apply_autocmds(event, fname, fname_io, force)
    EVENT_T	event;
    char_u	*fname;	    /* NULL or empty means use actual file name */
    char_u	*fname_io;  /* fname to use for "^Vf" on cmdline */
    int		force;	    /* when TRUE, ignore autocmd_busy */
{
    return apply_autocmds_group(event, fname, fname_io, force, AUGROUP_ALL);
}

    static int
apply_autocmds_group(event, fname, fname_io, force, group)
    EVENT_T	event;
    char_u	*fname;	    /* NULL or empty means use actual file name */
    char_u	*fname_io;  /* fname to use for "^Vf" on cmdline */
    int		force;	    /* when TRUE, ignore autocmd_busy */
    int		group;	    /* group ID, or AUGROUP_ALL */
{
    char_u	    *sfname = NULL;	/* short file name */
    char_u	    *tail;
    int		    temp;
    int		    save_changed = curbuf->b_changed;
    BUF		    *old_curbuf = curbuf;
    char_u	    *save_name;
    int		    retval = FALSE;
    int		    old_autocmd_busy = autocmd_busy;
    int		    old_autocmd_nested = autocmd_nested;
    static int	    nesting = 0;
    AutoPatCmd	    patcmd;
    AutoPat	    *ap;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    if (autocmd_busy && !(force || autocmd_nested))
	return retval;

    /*
     * Ignore events in 'eventignore'.
     */
    if (event_ignored(event))
	return retval;

    /*
     * Allow nesting of autocommands, but restrict the depth, because it's
     * possible to create an endless loop.
     */
    if (nesting == 10)
    {
	EMSG("autocommand nesting too deep");
	return retval;
    }

    /*
     * Check if these autocommands are disabled.  Used when doing ":all" or
     * ":ball".
     */
    if (    (autocmd_no_enter &&
		(event == EVENT_WINENTER || event == EVENT_BUFENTER)) ||
	    (autocmd_no_leave &&
		(event == EVENT_WINLEAVE || event == EVENT_BUFLEAVE)))
	return retval;

    /*
     * When the file name is NULL or empty, use the file name of the current
     * buffer.	Always use the full path of the file name to match with, in
     * case "allow_dirs" is set.
     */
    if (fname == NULL || *fname == NUL)
    {
	if (curbuf->b_sfname != NULL)
	    sfname = vim_strsave(curbuf->b_sfname);
	fname = curbuf->b_ffname;
	if (fname == NULL)
	    fname = (char_u *)"";
	fname = vim_strsave(fname);	/* make a copy, so we can change it */
    }
    else
    {
	sfname = vim_strsave(fname);
	fname = FullName_save(fname, FALSE);
    }
    if (fname == NULL)	    /* out of memory */
	return FALSE;

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    {
	char_u	    *p;

	if (sfname != NULL)
	{
	    for (p = sfname; *p; ++p)
		if (*p == '\\')
		    *p = '/';
	}
	for (p = fname; *p; ++p)
	    if (*p == '\\')
		*p = '/';
    }
#endif

    /* Don't redraw while doing auto commands. */
    temp = RedrawingDisabled;
    RedrawingDisabled = TRUE;
    save_name = sourcing_name;	/* may be called from .vimrc */
    sourcing_name = NULL;	/* don't free this one */
    autocmd_fname = fname_io;

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    if (!autocmd_busy)
    {
	save_search_patterns();
	saveRedobuff();
    }

    /*
     * Note that we are applying autocmds.  Some commands need to know.
     */
    autocmd_busy = TRUE;
    ++nesting;

    tail = gettail(fname);

    /* Find first autocommand that matches */
    patcmd.curpat = first_autopat[event];
    patcmd.nextcmd = NULL;
    patcmd.group = group;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.tail = tail;
    patcmd.event = event;
    auto_next_pat(&patcmd, FALSE);

    /* found one, start executing the autocommands */
    if (patcmd.curpat != NULL)
    {
	retval = TRUE;
	/* mark the last pattern, to avoid an endless loop when more patterns
	 * are added when executing autocommands */
	for (ap = patcmd.curpat; ap->next != NULL; ap = ap->next)
	    ap->last = FALSE;
	ap->last = TRUE;
	check_lnums(TRUE);	/* make sure cursor and topline are valid */
	do_cmdline(NULL, getnextac, (void *)&patcmd,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);
    }

    RedrawingDisabled = temp;
    autocmd_busy = old_autocmd_busy;
    autocmd_nested = old_autocmd_nested;
    sourcing_name = save_name;
    autocmd_fname = NULL;
    vim_free(fname);
    vim_free(sfname);
    --nesting;

    /*
     * When stopping to execute autocommands, restore the search patterns and
     * the redo buffer.
     */
    if (!autocmd_busy)
    {
	restore_search_patterns();
	restoreRedobuff();
    }

    /*
     * Some events don't set or reset the Changed flag.
     * Check if still in the same buffer!
     */
    if (curbuf == old_curbuf &&
	    (event == EVENT_BUFREADPOST || event == EVENT_BUFWRITEPOST ||
	    event == EVENT_FILEAPPENDPOST || event == EVENT_VIMLEAVE))
	curbuf->b_changed = save_changed;

    au_cleanup();	/* may really delete removed patterns/commands now */
    return retval;
}

/*
 * Find next autocommand pattern that matches.
 */
    static void
auto_next_pat(apc, stop_at_last)
    AutoPatCmd	*apc;
    int		stop_at_last;	    /* stop when 'last' flag is set */
{
    AutoPat	    *ap;
    vim_regexp	    *prog;
    AutoCmd	    *cp;
    char_u	    *name;

    vim_free(sourcing_name);
    sourcing_name = NULL;

    for (ap = apc->curpat; ap != NULL && !got_int; ap = ap->next)
    {
	apc->curpat = NULL;

	/* only use a pattern when it has not been removed, has commands and
	 * the group matches */
	if (ap->pat != NULL && ap->cmds != NULL
		&& (apc->group == AUGROUP_ALL || apc->group == ap->group))
	{
#ifdef CASE_INSENSITIVE_FILENAME
	    reg_ic = TRUE;			/* Always ignore case */
#else
	    reg_ic = FALSE;			/* Don't ever ignore case */
#endif
	    prog = vim_regcomp(ap->reg_pat, TRUE);	/* Always use magic */

	    /*
	     * Try for a match with the pattern with:
	     * 1. the full file name, when the pattern has a '/'.
	     * 2. the short file name, when the pattern has a '/'.
	     * 3. the tail of the file name, when the pattern has no '/'.
	     */
	    if (   prog != NULL
		&& ((ap->allow_dirs
			&& (vim_regexec(prog, apc->fname, TRUE)
			    || (apc->sfname != NULL
				&& vim_regexec(prog, apc->sfname, TRUE))))
		    || (!ap->allow_dirs
			&& vim_regexec(prog, apc->tail, TRUE))))
	    {
		name = event_nr2name(apc->event);
		sourcing_name = alloc((unsigned)(STRLEN(name)
						     + STRLEN(ap->pat) + 25));
		if (sourcing_name != NULL)
		{
		    sprintf((char *)sourcing_name,
			    "%s Auto commands for \"%s\"",
					       (char *)name, (char *)ap->pat);
		    if (p_verbose >= 8)
			smsg((char_u *)"Executing %s", sourcing_name);
		}

		apc->curpat = ap;
		apc->nextcmd = ap->cmds;
		/* mark last command */
		for (cp = ap->cmds; cp->next != NULL; cp = cp->next)
		    cp->last = FALSE;
		cp->last = TRUE;
	    }
	    vim_free(prog);
	    line_breakcheck();
	    if (apc->curpat != NULL)	    /* found a match */
		break;
	}
	if (stop_at_last && ap->last)
	    break;
    }
}

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
/* ARGSUSED */
    static char_u *
getnextac(c, cookie, indent)
    int	    c;		    /* not used */
    void    *cookie;
    int	    indent;	    /* not used */
{
    AutoPatCmd	    *acp = (AutoPatCmd *)cookie;
    char_u	    *retval;

    /* repeat until we find an autocommand to execute */
    for (;;)
    {
	/* skip removed commands */
	while (acp->nextcmd != NULL && acp->nextcmd->cmd == NULL)
	    if (acp->nextcmd->last)
		acp->nextcmd = NULL;
	    else
		acp->nextcmd = acp->nextcmd->next;

	if (acp->nextcmd != NULL)
	    break;

	/* at end of commands, find next pattern that matches */
	if (acp->curpat->last)
	    acp->curpat = NULL;
	else
	    acp->curpat = acp->curpat->next;
	if (acp->curpat != NULL)
	    auto_next_pat(acp, TRUE);
	if (acp->curpat == NULL)
	    return NULL;
    }

    if (p_verbose >= 9)
    {
	msg_scroll = TRUE;	    /* always scroll up, don't overwrite */
	smsg((char_u *)"autocommand %s", acp->nextcmd->cmd);
	msg_puts((char_u *)"\n");   /* don't overwrite this either */
	cmdline_row = msg_row;
    }
    retval = vim_strsave(acp->nextcmd->cmd);
    autocmd_nested = acp->nextcmd->nested;
    if (acp->nextcmd->last)
	acp->nextcmd = NULL;
    else
	acp->nextcmd = acp->nextcmd->next;
    return retval;
}

static int include_groups = FALSE;

    char_u  *
set_context_in_autocmd(arg, doautocmd)
    char_u  *arg;
    int	    doautocmd;	    /* TRUE for :doautocmd, FALSE for :autocmd */
{
    char_u  *p;
    int	    group;

    /* check for a group name, skip it if present */
    include_groups = FALSE;
    group = au_get_grouparg(&arg);
    if (group == AUGROUP_ERROR)
	return NULL;

    /* skip over event name */
    for (p = arg; *p && !vim_iswhite(*p); ++p)
	if (*p == ',')
	    arg = p + 1;
    if (*p == NUL)
    {
	if (group == AUGROUP_ALL)
	    include_groups = TRUE;
	expand_context = EXPAND_EVENTS;	    /* expand event name */
	expand_pattern = arg;
	return NULL;
    }

    /* skip over pattern */
    arg = skipwhite(p);
    while (*arg && (!vim_iswhite(*arg) || arg[-1] == '\\'))
	arg++;
    if (*arg)
	return arg;			    /* expand (next) command */

    if (doautocmd)
	expand_context = EXPAND_FILES;	    /* expand file names */
    else
	expand_context = EXPAND_NOTHING;    /* pattern is not expanded */
    return NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(idx)
    int	    idx;
{
    if (idx < augroups.ga_len)		/* First list group names, if wanted */
    {
	if (!include_groups || AUGROUP_NAME(idx) == NULL)
	    return (char_u *)"";	/* skip deleted entries */
	return AUGROUP_NAME(idx);	/* return a name */
    }
    return (char_u *)event_names[idx - augroups.ga_len].name;
}

#endif	/* AUTOCMD */

/*
 * Convert the given pattern "pat" which has shell style wildcards in it, into
 * a regular expression, and return the result.  If there is a directory path
 * separator to be matched, then TRUE is put in allow_dirs, otherwise FALSE is
 * put there -- webb.
 */
    char_u *
file_pat_to_reg_pat(pat, pat_end, allow_dirs)
    char_u  *pat;
    char_u  *pat_end;		/* first char after pattern */
    char    *allow_dirs;	/* Result passed back out in here */
{
    int		size;
    char_u	*endp;
    char_u	*reg_pat;
    char_u	*p;
    int		i;
    int		nested = 0;
    int		add_dollar = TRUE;

    if (allow_dirs != NULL)
	*allow_dirs = FALSE;

    size = 2;		/* '^' at start, '$' at end */
    for (p = pat; p < pat_end; p++)
    {
	switch (*p)
	{
	    case '*':
	    case '.':
	    case ',':
	    case '{':
	    case '}':
	    case '~':
#ifdef BACKSLASH_IN_FILENAME
	    case '\\':
#endif
		size += 2;
		break;
	    default:
		size++;
		break;
	}
    }
    reg_pat = alloc(size + 1);
    if (reg_pat == NULL)
	return NULL;
    i = 0;
    if (pat[0] == '*')
	while (pat[0] == '*' && pat < pat_end - 1)
	    pat++;
    else
	reg_pat[i++] = '^';
    endp = pat_end - 1;
    if (*endp == '*')
    {
	while (endp - pat > 0 && *endp == '*')
	    endp--;
	add_dollar = FALSE;
    }
    for (p = pat; *p && nested >= 0 && p <= endp; p++)
    {
	switch (*p)
	{
	    case '*':
		reg_pat[i++] = '.';
		reg_pat[i++] = '*';
		break;
	    case '.':
	    case '~':
		reg_pat[i++] = '\\';
		reg_pat[i++] = *p;
		break;
	    case '?':
		reg_pat[i++] = '.';
		break;
	    case '\\':
		if (p[1] == NUL)
		    break;
#ifdef BACKSLASH_IN_FILENAME
		/* translate "\x" to "\\x", "\*" to "\\.*", and "\?" to "\\." */
		if (vim_isfilec(p[1]) || p[1] == '*' || p[1] == '?')
		{
		    reg_pat[i++] = '\\';
		    reg_pat[i++] = '\\';
		    if (allow_dirs != NULL)
			*allow_dirs = TRUE;
		    break;
		}
		++p;
#else
		if (*++p == '?')
		    reg_pat[i++] = '?';
		else
#endif
		    if (*p == ',')
			reg_pat[i++] = ',';
		    else
		    {
			if (allow_dirs != NULL && vim_ispathsep(*p))
			    *allow_dirs = TRUE;
			reg_pat[i++] = '\\';
			reg_pat[i++] = *p;
		    }
		break;
	    case '{':
		reg_pat[i++] = '\\';
		reg_pat[i++] = '(';
		nested++;
		break;
	    case '}':
		reg_pat[i++] = '\\';
		reg_pat[i++] = ')';
		--nested;
		break;
	    case ',':
		if (nested)
		{
		    reg_pat[i++] = '\\';
		    reg_pat[i++] = '|';
		}
		else
		    reg_pat[i++] = ',';
		break;
	    default:
		if (allow_dirs != NULL && vim_ispathsep(*p))
		    *allow_dirs = TRUE;
		reg_pat[i++] = *p;
		break;
	}
    }
    if (add_dollar)
	reg_pat[i++] = '$';
    reg_pat[i] = NUL;
    if (nested != 0)
    {
	if (nested < 0)
	    EMSG("Missing {.");
	else
	    EMSG("Missing }.");
	vim_free(reg_pat);
	reg_pat = NULL;
    }
    return reg_pat;
}
