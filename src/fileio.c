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
 * fileio.c: read from and write to a file
 */

/*
 * special feature of this version: NUL characters in the file are
 * replaced by newline characters in memory. This allows us to edit
 * binary files!
 */

#include "vim.h"
#include "fcntl.h"

extern char				emsg_interrupted[];

#define BUFSIZE 4096
static void do_mlines __ARGS((void));

	void
filemess(name, s)
	char		*name;
	char		*s;
{
	smsg("\"%s\" %s", ((name == NULL) ? "" : name), s);
}

/*
 * Read lines from file 'fname' into the buffer after line 'from'.
 *
 * 1. We allocate blocks with m_blockalloc, as big as possible.
 * 2. Each block is filled with characters from the file with a single read().
 * 3. The lines are inserted in the buffer with appendline().
 *
 * (caller must check that fname != NULL)
 */
	bool_t
readfile(fname, from)
	char		   *fname;
	linenr_t		from;
{
	int 				fd;
	register u_char 	c;
	register linenr_t	lnum = from;
	register u_char 	*ptr = NULL;			/* pointer into read buffer */
	register u_char		*buffer;				/* read buffer */
	register long		size;
	long				filesize;
#define UNKNOWN		0x0fffffff					/* file size is unknown */
	linenr_t			linecnt = line_count;
	bool_t				incomplete = FALSE; 	/* was the last line incomplete? */
	int 				error = 0;				/* read errors encountered */
	long				linerest = 0;			/* remaining characters in line */
	long				filerest;				/* remaining characters in file */

	if (bufempty())		/* special case: buffer has no lines */
		linecnt = 0;

	if ((fd = open(fname, O_RDWR)) == -1)		/* cannot open r/w */
	{
		if ((fd = open(fname, O_RDONLY)) == -1) /* cannot open at all */
		{
			if (from == 0)
				filemess(fname, "[New File]");
			return TRUE;
		}
		if (from == 0)							/* set file readonly */
			P(P_RO) = TRUE;
	}
	else if (from == 0 && !readonlymode)		/* set file not readonly */
		P(P_RO) = FALSE;

	if ((filesize = lseek(fd, 0L, 2)) < 0)		/* get length of file */
		filesize = UNKNOWN;
	lseek(fd, 0L, 0);

	filemess(fname, "");

	for (filerest = filesize; !error && !got_int && filerest != 0; breakcheck())
	{
		/*
		 * We allocate as much space for the file as we can get, plus
		 * space for the old line, one NUL in front and one NUL at the tail.
		 * The amount is limited by the fact that read() only can read
		 * upto max_unsigned characters.
		 * If we don't know the file size, just get one Kbyte.
		 */
		if (filesize >= UNKNOWN)
			size = 1024;
		else if (filerest > 0xff00L)
			size = 0xff00L;
		else if (filerest < 10)
			size = 10;
		else
			size = filerest;

		for ( ; size >= 10; size /= 2)
		{
			if ((buffer = (u_char *)m_blockalloc((u_long)(size + linerest + 2), (bool_t)FALSE))
						!= NULL)
				break;
		}
		if (buffer == NULL)
		{
			emsg("not enough memory!");
			error = 1;
			break;
		}
		buffer[0] = NUL;	/* make sure there is a NUL in front of the first line */
		++buffer;
		if (linerest)		/* copy characters from the previous buffer */
		{
			ptr -= linerest;
			memmove(buffer, ptr, linerest);
			memset(ptr, 1, linerest);			/* fill with non-NULs */
			ptr[linerest - 1] = NUL;			/* add a NUL on the end */
			free_line((char *)ptr);				/* free the space we don't use */
		}
		ptr = buffer + linerest;
		
		if ((size = (unsigned)read(fd, ptr, (size_t)size)) <= 0)
		{
			error = 2;
			break;
		}
		if (filesize >= UNKNOWN)			/* if we don't know the file size */
			filesize += size;				/* .. count the number of characters */
		else								/* .. otherwise */
			filerest -= size;				/* .. compute the remaining length */

		/*
		 * This loop is executed once for every character read.
		 * Keep it fast!
		 */
		--ptr;
		while (++ptr, --size >= 0)
		{
			if ((c = *ptr) != NUL && c != NL)	/* catch most common case */
				continue;
			if (c == NUL)
				*ptr = NL;		/* NULs are replaced by newlines! */
			else
			{
				*ptr = NUL;		/* end of line */
				if (!appendline(lnum, (char *)buffer))
				{
					error = 1;
					break;
				}
				++lnum;
				buffer = ptr + 1;
			}
		}
		linerest = ptr - buffer;
	}
	if (error != 1 && linerest != 0)
	{
		/*
		 * If we get EOF in the middle of a line, note the fact and
		 * complete the line ourselves.
		 */
		incomplete = TRUE;
		*ptr = NUL;
		if (!appendline(lnum, (char *)buffer))
			error = 1;
	}
	if (error == 2 && filesize >= UNKNOWN)	/* no error, just EOF encountered */
	{
		filesize -= UNKNOWN;
		error = 0;
	}

	close(fd);

	if (got_int)
	{
		filemess(fname, emsg_interrupted);
		return FALSE;			/* an interrupt isn't really an error */
	}

	linecnt = line_count - linecnt;
	smsg("\"%s\" %s%s%s%ld line%s, %ld character%s",
			fname,
			P(P_RO) ? "[readonly] " : "",
			incomplete ? "[Incomplete last line] " : "",
			error ? "[READ ERRORS] " : "",
			(long)linecnt, plural((long)linecnt),
			filesize, plural(filesize));

	u_clearline();		/* cannot use "U" command after adding lines */

	if (from == 0)		/* edit a new file: read mode from lines */
		do_mlines();
	return FALSE;
}

/*
 * writeit - write to file 'fname' lines 'start' through 'end'
 *
 * If either 'start' or 'end' contain null line pointers, the default is to use
 * the start or end of the file respectively.
 *
 * We do our own buffering here because fwrite() is so slow.
 */
	bool_t
writeit(fname, start, end, append)
	char			*fname;
	linenr_t		start, end;
	bool_t			append;
{
	int 				fd;
	char			   *backup = NULL;
	register char	   *s;
	register u_char	   *ptr;
	register u_char		c;
	register int		len;
	register linenr_t	lnum;
	long				nchars;
	char				*errmsg;
	char				*buffer;
	int 				perm = -1;			/* file permissions */

	filemess(fname, "");

	buffer = alloc(BUFSIZE);
	if (buffer == NULL)
		return FALSE;

	if (!append && (perm = getperm(fname)) >= 0)
	{										/* file exists, make a backup */
		/*
		 * Form the backup file name - change path/fo.o.h to path/fo.o.bak
		 */
		backup = modname(fname, ".bak");
		if (backup == NULL)
				goto fail2;

		/*
		 * Delete any existing backup and move the current version to the backup.
		 * For safety, we don't remove the backup until the write has finished
		 * successfully. And if the 'backup' option is set, leave it around.
		 */
		remove(backup);
		if (rename(fname, backup) != 0)
		{
				errmsg = "Can't make backup file";
				goto fail;
		}
	}

	if ((fd = open(fname, O_WRONLY | (append ? O_APPEND : O_CREAT))) < 0)
	{
		errmsg = "Can't open file for writing";
		goto fail;
	}

	len = 0;
	s = buffer;
	nchars = 0;
	for (lnum = start; lnum <= end; ++lnum)
	{
		/*
		 * The next loop is done once for each character written.
		 * Keep it fast!
		 */
		ptr = (u_char *)nr2ptr(lnum) - 1;
		while ((c = *++ptr) != NUL)
		{
			if (c == NL)
				*s = NUL;		/* replace newlines with NULs */
			else
				*s = c;
			++s;
			if (++len != BUFSIZE)
				continue;
			if (write(fd, buffer, (size_t)BUFSIZE) == -1)
			{
				end = 0;				/* write error: break loop */
				break;
			}
			nchars += BUFSIZE;
			s = buffer;
			len = 0;
		}
		*s = NL;
		++s;
		if (++len == BUFSIZE)
		{
			if (write(fd, buffer, (size_t)BUFSIZE) == -1)
				end = 0;				/* write error: break loop */
			nchars += BUFSIZE;
			s = buffer;
			len = 0;
		}
	}
	if (len)
	{
		if (write(fd, buffer, (size_t)len) == -1)
			end = 0;				/* write error */
		nchars += len;
	}

	close(fd);
	if (perm >= 0)
		setperm(fname, perm);	/* set permissions of new file same as old file */

	if (end == 0)
	{
		errmsg = "write error (file system full?)";
		goto fail;
	}

	lnum -= start;		/* compute number of written lines */
	smsg("\"%s\" %s %ld line%s, %ld character%s", fname,
			backup == NULL && !append ? "[New File]" : "",
			(long)lnum, plural((long)lnum),
			nchars, plural(nchars));
	if (start == 1 && end == line_count)		/* when written everything */
	{
		UNCHANGED;
		startscript();		/* re-start auto script file */
	}

	/*
	 * Remove the backup unless they want it left around
	 */
	if (!P(P_BK) && backup != NULL)
#ifdef AMIGA
		unlink(backup);
#else
		remove(backup);
#endif

	free((char *) backup);
	free(buffer);

	return TRUE;

fail:
	filemess(fname, errmsg);
	free((char *) backup);
fail2:
	free(buffer);
	return FALSE;
}

/*
 * do_mlines() - process mode lines for the current file
 *
 * Returns immediately if the "ml" parameter isn't set.
 */
static void 	chk_mline __ARGS((linenr_t));

	static void
do_mlines()
{
		linenr_t		lnum;
		int 			nmlines;

		if ((nmlines = P(P_ML)) == 0)
				return;

		for (lnum = 1; lnum <= line_count && lnum <= nmlines; ++lnum)
				chk_mline(lnum);

		for (lnum = line_count; lnum > 0 && lnum > nmlines &&
								lnum > line_count - nmlines; --lnum)
				chk_mline(lnum);
}

/*
 * chk_mline() - check a single line for a mode string
 */
	static void
chk_mline(lnum)
	linenr_t lnum;
{
		register char	*s;
		register char	*e;
		char			*cs;			/* local copy of any modeline found */
		char			prev;
		bool_t			end;

		prev = ' ';
		for (s = nr2ptr(lnum); *s != NUL; ++s)
		{
				if (isspace(prev) && (strncmp(s, "vi:", (size_t)3) == 0 || strncmp(s, "ex:", (size_t)3) == 0))
				{
						s += 3;
						end = FALSE;
						s = cs = strsave(s);
						if (cs == NULL)
								break;
						while (end == FALSE)
						{
								while (*s == ' ' || *s == TAB)
										++s;
								if (*s == NUL)
										break;
								for (e = s; *e != ':' && *e != NUL; ++e)
										;
								if (*e == NUL)
										end = TRUE;
								*e = NUL;
								doset(s);
								s = e + 1;
						}
						free(cs);
						break;
				}
				prev = *s;
		}
}
