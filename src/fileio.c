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
#include "globals.h"
#include "proto.h"
#include "param.h"
#include "fcntl.h"

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
	int
readfile(fname, from, newfile)
	char		   *fname;
	linenr_t		from;
	int				newfile;
{
#ifdef UNIX
	int 				fd = -1;
#else
	int 				fd;
#endif
	register u_char 	c;
	register linenr_t	lnum = from;
	register u_char 	*ptr = NULL;			/* pointer into read buffer */
	register u_char		*buffer = NULL;			/* read buffer */
	register long		size;
	long				filesize;
#define UNKNOWN		0x0fffffff					/* file size is unknown */
	linenr_t			linecnt = line_count;
	int					incomplete = FALSE; 	/* was the last line incomplete? */
	int 				error = 0;				/* read errors encountered */
	long				linerest = 0;			/* remaining characters in line */
	long				filerest;				/* remaining characters in file */

	if (bufempty())		/* special case: buffer has no lines */
		linecnt = 0;

	if (
#ifdef UNIX
		!(getperm(fname) & 0200) ||				/* root's way to check RO */
#endif
		(fd = open(fname, O_RDWR)) == -1)		/* cannot open r/w */
	{
		if ((fd = open(fname, O_RDONLY)) == -1) /* cannot open at all */
		{
			if (newfile)
				filemess(fname, "[New File]");

#ifdef MSDOS		/* the screen may be messed up by the "insert disk
							in drive b: and hit return" message */
			updateScreen(CLEAR);
#endif
			return TRUE;
		}
		if (newfile)							/* set file readonly */
			p_ro = TRUE;
	}
	else if (newfile && !readonlymode)		/* set file not readonly */
		p_ro = FALSE;

	if (
#ifdef MSDOS
	/* the CR characters disappear in read(), so the file lenght is wrong */
		p_tx == TRUE ||
#endif
			(filesize = lseek(fd, 0L, 2)) < 0)	/* get length of file */
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
			if ((buffer = (u_char *)m_blockalloc((u_long)(size + linerest + 4), FALSE))
						!= NULL)
				break;
		}
		if (buffer == NULL)
		{
			emsg(e_outofmem);
			error = 1;
			break;
		}
		buffer[0] = NUL;	/* make sure there is a NUL in front of the first line */
		++buffer;
		if (linerest)		/* copy characters from the previous buffer */
		{
			ptr -= linerest;
			memmove((char *)buffer, (char *)ptr, linerest);
			memset((char *)ptr, 1, linerest);	/* fill with non-NULs */
			ptr[linerest - 1] = NUL;			/* add a NUL on the end */
			free_line((char *)ptr);				/* free the space we don't use */
		}
		ptr = buffer + linerest;
		
		if ((size = (unsigned)read(fd, (char *)ptr, (size_t)size)) <= 0)
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

#ifdef MSDOS		/* the screen may be messed up by the "insert disk
							in drive b: and hit return" message */
	updateScreen(CLEAR);
#endif

	if (got_int)
	{
		filemess(fname, e_interr);
		return FALSE;			/* an interrupt isn't really an error */
	}

	linecnt = line_count - linecnt;
	smsg("\"%s\" %s%s%s%ld line%s, %ld character%s",
			fname,
			p_ro ? "[readonly] " : "",
			incomplete ? "[Incomplete last line] " : "",
			error ? "[READ ERRORS] " : "",
			(long)linecnt, plural((long)linecnt),
			filesize, plural(filesize));

	u_clearline();		/* cannot use "U" command after adding lines */

	if (newfile)		/* edit a new file: read mode from lines */
		do_mlines();
	if (from < line_count)
	{
		Curpos.lnum = from + 1;	/* put cursor at first new line */
		Curpos.col = 0;
	}

	return FALSE;
}

/*
 * writeit - write to file 'fname' lines 'start' through 'end'
 *
 * If either 'start' or 'end' contain null line pointers, the default is to use
 * the start or end of the file respectively.
 *
 * We do our own buffering here because fwrite() is so slow.
 *
 * If forceit is true, we don't care for errors when attempting backups (jw).
 * In case of an error everything possible is done to restore the original file.
 * But when forceit is TRUE, we risk loosing it.
 */
	int
writeit(fname, start, end, append, forceit)
	char			*fname;
	linenr_t		start, end;
	int				append;
	int				forceit;
{
	int 				fd;
	char			   *backup = NULL;
	register char	   *s;
	register u_char	   *ptr;
	register u_char		c;
	register int		len;
	register linenr_t	lnum;
	long				nchars;
	char				*errmsg = NULL;
	char				*buffer;
	long 				perm = -1;			/* file permissions */
	int					retval = TRUE;
#ifdef UNIX
	struct stat			old;
#endif

#ifdef DEBUG
# ifdef AMIGA
	if (writelock(fname) == FALSE)	/* file is locked, so what? */
	{
		emsg("File is locked");
		return FALSE;
	}
# endif
#endif

	filemess(fname, "");

	buffer = alloc(BUFSIZE);
	if (buffer == NULL)
		return FALSE;

#ifdef UNIX
		/* get information about original file */
	old.st_dev = old.st_ino = 0;
	stat(fname, &old);
	perm = getperm(fname);
/*
 * If we are not appending, the file exists, and the 'writebackup' or
 * 'backup' option is set, try to make a backup copy of the file.
 */
	if (!append && perm >= 0 && (p_wb || p_bk) &&
					(fd = open(fname, O_RDONLY)) >= 0)
	{
		int				bfd, buflen, wlen;
		char			buf[BUFSIZE + 1], *wp;
		int				some_error = FALSE;
		struct stat		new;

		new.st_dev = new.st_ino = 0;

		/*
		 * Unix semantics has it, that we may have a writable file, 
		 * that cannot be recreated with a simple open(..., O_CREATE, ) e.g:
		 *  - the directory is not writable, 
		 *  - the file may be a symbolic link, 
		 *  - the file may belong to another user/group, etc.
		 *
		 * For these reasons, the existing writable file must be truncated and
		 * reused. Creation of a backup COPY will be attempted.
		 */
		if ((backup = modname(fname, ".bak")) == NULL)
		{
			some_error = TRUE;
			goto nobackup;
		}			
		stat(backup, &new);
		if (new.st_dev == old.st_dev && new.st_ino == old.st_ino)
		{
			/*
			 * may happen when modname gave the same file back.
			 * E.g. silly link, or filename-length reached.
			 * If we don't check here, we either ruin the file when
			 * copying or erase it after writing. jw.
			 */
			errmsg = "Invalid backup file";
			free(backup);
			backup = NULL;	/* there is no backup file to delete */
			goto nobackup;
		}
		remove(backup);		/* remove old backup, if present */
		if ((bfd = open(backup, O_WRONLY | O_CREAT, 0666)) < 0)
		{
			char *home;

			/* 
			 * oops, no write/create permission here?
			 * try again in p_bdir directory. 
			 */
			for (wp = fname + strlen(fname); wp >= fname; wp--)
				if (*wp == '/')
					break;
			++wp;
			if (p_bdir[0] == '~' && p_bdir[1] == '/' && (home = getenv("HOME")) != NULL)
				sprintf(buf, "%s/%s/%s", home, p_bdir + 2, wp);
			else
				sprintf(buf, "%s/%s", p_bdir, wp);
			free(backup);
			if ((backup = modname(buf, ".bak")) == NULL)
			{
				some_error = TRUE;
				goto nobackup;
			}
			stat(backup, &new);
			if (new.st_dev == old.st_dev && new.st_ino == old.st_ino)
			{
				errmsg = "Invalid backup file";
				free(backup);
				backup = NULL;	/* there is no backup file to delete */
				goto nobackup;
			}
			remove(backup);
			if ((bfd = open(backup, O_WRONLY | O_CREAT, 0666)) < 0)
			{
				free(backup);
				backup = NULL;	/* there is no backup file to delete */
				errmsg = "Can't make backup file";
				goto nobackup;
			}
		}
		/* set file protection same as original file, but strip s-bit */
		setperm(backup, perm & 0777);

		/* copy the file. */
		while ((buflen = read(fd, buf, BUFSIZE)) > 0)
		{
			wp = buf;
			do
			{
				if ((wlen = write(bfd, wp, buflen)) <= 0)
				{
					errmsg = "Can't write to backup file";
					goto writeerr;
				}
				wp += wlen;
				buflen -= wlen;
			}
			while (buflen > 0);
		}
writeerr:
		close(bfd);
		if (buflen < 0)
			errmsg = "Can't read file for backup";
nobackup:
		close(fd);
	/* ignore errors when forceit is TRUE */
		if ((some_error || errmsg) && !forceit)
		{
			retval = FALSE;
			goto fail;
		}
		errmsg = NULL;
	}
		/* if forceit and the file was read-only: make it writable */
	if (forceit && (old.st_uid == getuid()) && perm >= 0 && !(perm & 0200))
 	{
		perm |= 0200;	
		setperm(fname, perm);
 	}
#else /* UNIX */

/*
 * If we are not appending, the file exists, and the 'writebackup' or
 * 'backup' option is set, make a backup.
 * Do not make any backup, if "writebackup" and "backup" are 
 * both switched off. This helps when editing large files on
 * almost-full disks. (jw)
 */
	if (!append && (perm = getperm(fname)) >= 0 && (p_wb || p_bk))
	{
		/*
		 * Form the backup file name - change path/fo.o.h to path/fo.o.h.bak
		 */
		backup = modname(fname, ".bak");
		if (backup == NULL)
		{
			if (!forceit)
				goto fail;
		}
		else
		{
			/*
			 * Delete any existing backup and move the current version to the backup.
			 * For safety, we don't remove the backup until the write has finished
			 * successfully. And if the 'backup' option is set, leave it around.
			 */
			remove(backup);
			if (rename(fname, backup) != 0)
			{
				if (forceit)
				{
					free(backup);	/* don't do the rename below */
					backup = NULL;
				}
				else
				{
					errmsg = "Can't make backup file";
					goto fail;
				}
			}
		}
	}
#endif /* UNIX */

		/* 
		 * We may try to open the file twice: If we can't write to the
		 * file and forceit is TRUE we delete the existing file and try to create
		 * a new one. If this still fails we lost the original file!
		 */
	while ((fd = open(fname, O_WRONLY | (append ? O_APPEND : (O_CREAT | O_TRUNC)), 0666)) < 0)
 	{
		/*
		 * A forced write will try to create a new file if the old one is
		 * still readonly. This may also happen when the directory is
		 * read-only. In that case the remove() will fail.
		 */
		if (!errmsg)
		{
			errmsg = "Can't open file for writing";
			if (forceit)
			{
#ifdef UNIX
				/* we write to the file, thus it should be marked
													writable after all */
				perm |= 0200;		
				if (perm >= 0 && ((old.st_uid != getuid()) || (old.st_gid != getgid())))
					perm &= 0777;
#endif /* UNIX */
				remove(fname);
				continue;
			}
		}
/*
 * If we failed to open the file, we don't need a backup.
 * If we moved or removed the original file try to put the backup in its place.
 */
#ifdef UNIX
 		if (backup)
		{
			if (forceit)
				rename(backup, fname);	/* we removed the original, move
												the copy in its place */
			else
				remove(backup);			/* it was a copy, throw away */
		}
#else
 		if (backup)
 			rename(backup, fname);	/* try to put the original file back */
#endif
 		goto fail;
 	}
	errmsg = NULL;

#ifdef MSDOS
	setperm(fname, 0); 
	/* 
	 * Just to be sure we can write it next time. 
	 */
#endif

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

	if (close(fd) != 0)
	{
		errmsg = "Close failed";
		goto fail;
	}
	if (perm >= 0)
		setperm(fname, perm);	/* set permissions of new file same as old file */

	if (end == 0)
	{
		errmsg = "write error (file system full?)";
		goto fail;
	}

#ifdef MSDOS		/* the screen may be messed up by the "insert disk
							in drive b: and hit return" message */
	updateScreen(CLEAR);
#endif

	lnum -= start;		/* compute number of written lines */
	smsg("\"%s\"%s %ld line%s, %ld character%s",
			fname,
			(backup == NULL && !append) ? " [New File]" : " ",
			(long)lnum, plural((long)lnum),
			nchars, plural(nchars));
	if (start == 1 && end == line_count)		/* when written everything */
	{
		UNCHANGED;
		startscript();		/* re-start auto script file */
	}

	/*
	 * Remove the backup unless 'backup' option is set
	 */
	if (!p_bk && backup != NULL && remove(backup) != 0)
		emsg("Can't delete backup file");
	
	goto nofail;

fail:
#ifdef MSDOS		/* the screen may be messed up by the "insert disk
							in drive b: and hit return" message */
	updateScreen(CLEAR);
#endif
nofail:

	free((char *) backup);
	free(buffer);

	if (errmsg != NULL)
	{
		filemess(fname, errmsg);
		retval = FALSE;
	}
	return retval;
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

		if ((nmlines = p_ml) == 0)
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
	int				end;

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
