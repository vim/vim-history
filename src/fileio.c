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
 * fileio.c: read from and write to a file
 */

/*
 * special feature of this version: NUL characters in the file are
 * replaced by newline characters in memory. This allows us to edit
 * binary files!
 */

#ifdef MSDOS
# include <io.h>
#endif

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"
#include "fcntl.h"

#ifdef LATTICE
# include <proto/dos.h>		/* for Lock() and UnLock() */
#endif

static int	noendofline = FALSE;	/* Set to TRUE when last line has no
															EOL in binary mode */

#define BUFSIZE 4096				/* size of normal write buffer */
#define SBUFSIZE 256				/* size of emergency write buffer */

static int  write_buf __ARGS((int, char *, int));
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
readfile(fname, sfname, from, newfile)
	char		   *fname;
	char		   *sfname;
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
	register u_char		*p;
	long				filesize;
#define UNKNOWN		0x0fffffff					/* file size is unknown */
	linenr_t			linecnt = line_count;
	int					incomplete = FALSE; 	/* was the last line incomplete? */
	int 				error = 0;				/* read errors encountered */
	long				linerest = 0;			/* remaining characters in line */
	long				filerest;				/* remaining characters in file */
	int					firstpart = TRUE;		/* reading first part */
#ifdef UNIX
	int					perm;
#endif
	int					textmode = p_tx;		/* accept CR-LF for line break */

	if (sfname == NULL)
		sfname = fname;
	/*
	 * Use the short filename whenever possible.
	 * Avoids problems with networks and when directory names are changed.
	 */
	if (!did_cd)
		fname = sfname;

	if (bufempty())		/* special case: buffer has no lines */
		linecnt = 0;

#ifdef UNIX
		/*
		 * On Unix it is possible to read a directory, so we have to
		 * check for it before the open().
		 */
	perm = getperm(fname);
#ifdef _POSIX_SOURCE
	if (perm >= 0 && !S_ISREG(perm))				/* not a regular file */
#else
	if (perm >= 0 && (perm & S_IFMT) != S_IFREG)	/* not a regular file */
#endif
	{
#ifdef _POSIX_SOURCE
		if (S_ISDIR(perm))
#else
		if ((perm & S_IFMT) == S_IFDIR)
#endif
			filemess(fname, "is a directory");
		else
			filemess(fname, "is not a file");
		return TRUE;
	}
#endif

	if (
#ifdef UNIX
		!(perm & 0200) ||						/* root's way to check RO */
#endif
		(fd = open(fname, O_RDWR)) == -1)		/* cannot open r/w */
	{
		if ((fd = open(fname, O_RDONLY)) == -1) /* cannot open at all */
		{
#ifdef MSDOS
		/*
		 * The screen may be messed up by the "insert disk
		 * in drive b: and hit return" message
		 */
			updateScreen(CLEAR);
#endif

#ifndef UNIX
		/*
		 * On MSDOS and Amiga we can't open a directory, check here.
		 */
			if (isdir(fname) > 0)
				filemess(fname, "is a directory");
			else
#endif
				if (newfile)
					filemess(fname, "[New File]");

			return TRUE;
		}
		if (newfile)						/* set file readonly */
			p_ro = TRUE;
	}
	else if (newfile && !readonlymode)		/* set file not readonly */
		p_ro = FALSE;

	if (newfile)
		noendofline = FALSE;

	if ((filesize = lseek(fd, 0L, 2)) < 0)	/* get length of file */
		filesize = UNKNOWN;
	lseek(fd, 0L, 0);

	filemess(fname, "");					/* show that we are busy */

	for (filerest = filesize; !error && !got_int && filerest != 0; breakcheck())
	{
		/*
		 * We allocate as much space for the file as we can get, plus
		 * space for the old line, one NUL in front and one NUL at the tail.
		 * The amount is limited by the fact that read() only can read
		 * upto max_unsigned characters (and other things).
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
		 * when reading the first part of a file: guess EOL type
		 */
		if (firstpart && p_ta)
		{
			for (p = ptr; p < ptr + size; ++p)
				if (*p == NL)
				{
					if (p > ptr && p[-1] == CR)	/* found CR-NL */
						textmode = TRUE;
					else						/* found a single NL */
						textmode = FALSE;
						/* if editing a new file: may set p_tx */
					if (newfile && p_tx != textmode)
					{
						p_tx = textmode;
						paramchanged("tx");
					}
					break;
				}
		}

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
				if (textmode && ptr[-1] == CR)	/* remove CR */
					ptr[-1] = NUL;
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
		firstpart = FALSE;
	}
	if (lnum != from && !newfile)	/* added at least one line */
		CHANGED;
	if (error != 1 && linerest != 0)
	{
		/*
		 * If we get EOF in the middle of a line, note the fact and
		 * complete the line ourselves.
		 */
		incomplete = TRUE;
		if (newfile && p_bin)		/* remember for when writing */
			noendofline = TRUE;
		*ptr = NUL;
		if (!appendline(lnum, (char *)buffer))
			error = 1;
		else if (!newfile)
			CHANGED;
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
	smsg("\"%s\" %s%s%s%s%ld line%s, %ld character%s",
			fname,
			p_ro ? "[readonly] " : "",
			incomplete ? "[Incomplete last line] " : "",
			error ? "[READ ERRORS] " : "",
#ifdef MSDOS
			textmode ? "" : "[notextmode] ",
#else
			textmode ? "[textmode] " : "",
#endif
			(long)linecnt, plural((long)linecnt),
			filesize, plural(filesize));

	if (error && newfile)	/* with errors we should not write the file */
	{
		p_ro = TRUE;
		paramchanged("ro");
	}

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
 * We do our own buffering here because fwrite() is so slow.
 *
 * If forceit is true, we don't care for errors when attempting backups (jw).
 * In case of an error everything possible is done to restore the original file.
 * But when forceit is TRUE, we risk loosing it.
 * When whole is TRUE and start == 1 and end == line_count, reset Changed.
 */
	int
writeit(fname, sfname, start, end, append, forceit, whole)
	char			*fname;
	char			*sfname;
	linenr_t		start, end;
	int				append;
	int				forceit;
	int				whole;
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
	char				smallbuf[SBUFSIZE];
	int					bufsize;
	long 				perm = -1;			/* file permissions */
	int					retval = TRUE;
	int					newfile = FALSE;	/* TRUE if file does not exist yet */
#ifdef UNIX
	struct stat			old;
	int					made_writable = FALSE;	/* 'w' bit has been set */
#endif
#ifdef AMIGA
	BPTR				flock;
#endif

	if (fname == NULL || *fname == NUL)		/* safety check */
		return FALSE;
	if (sfname == NULL)
		sfname = fname;
	/*
	 * Use the short filename whenever possible.
	 * Avoids problems with networks and when directory names are changed.
	 */
	if (!did_cd)
		fname = sfname;

	/*
	 * Disallow writing from .exrc and .vimrc in current directory for
	 * security reasons.
	 */
	if (secure)
	{
		secure = 2;
		emsg(e_curdir);
		return FALSE;
	}

	if (exiting)
		settmode(0);			/* when exiting allow typahead now */

	filemess(fname, "");		/* show that we are busy */

	buffer = alloc(BUFSIZE);
	if (buffer == NULL)			/* can't allocate big buffer, use small one */
	{
		buffer = smallbuf;
		bufsize = SBUFSIZE;
	}
	else
		bufsize = BUFSIZE;

#ifdef UNIX
		/* get information about original file (if there is one) */
	old.st_dev = old.st_ino = 0;
	if (stat(fname, &old))
		newfile = TRUE;
	else
	{
#ifdef _POSIX_SOURCE
		if (!S_ISREG(old.st_mode))      		/* not a file */
#else
		if ((old.st_mode & S_IFMT) != S_IFREG)	/* not a file */
#endif
		{
#ifdef _POSIX_SOURCE
			if (S_ISDIR(old.st_mode))
#else
			if ((old.st_mode & S_IFMT) == S_IFDIR)
#endif
				errmsg = "is a directory";
			else
				errmsg = "is not a file";
			goto fail;
		}
		perm = old.st_mode;
	}
/*
 * If we are not appending, the file exists, and the 'writebackup' or
 * 'backup' option is set, try to make a backup copy of the file.
 */
	if (!append && perm >= 0 && (p_wb || p_bk) &&
					(fd = open(fname, O_RDONLY)) >= 0)
	{
		int				bfd, buflen;
		char			buf[BUFSIZE + 1], *wp;
		int				some_error = FALSE;
		struct stat		new;

		new.st_dev = new.st_ino = 0;

		/*
		 * Unix semantics has it, that we may have a writable file, 
		 * that cannot be recreated with a simple open(..., O_CREAT, ) e.g:
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
		if (!stat(backup, &new) &&
					new.st_dev == old.st_dev && new.st_ino == old.st_ino)
		{
			/*
			 * may happen when modname gave the same file back.
			 * E.g. silly link, or filename-length reached.
			 * If we don't check here, we either ruin the file when
			 * copying or erase it after writing. jw.
			 */
			errmsg = "Invalid backup file (use ! to override)";
			free(backup);
			backup = NULL;	/* there is no backup file to delete */
			goto nobackup;
		}
		remove(backup);		/* remove old backup, if present */
		if ((bfd = open(backup, O_WRONLY | O_CREAT, 0666)) < 0)
		{
			/* 
			 * oops, no write/create permission here?
			 * try again in p_bdir directory. 
			 */
			for (wp = fname + strlen(fname); wp >= fname; wp--)
				if (*wp == '/')
					break;
			++wp;
			sprintf(buf, "%s/%s", p_bdir, wp);
			free(backup);
			if ((backup = modname(buf, ".bak")) == NULL)
			{
				some_error = TRUE;
				goto nobackup;
			}
			if (!stat(backup, &new) &&
						new.st_dev == old.st_dev && new.st_ino == old.st_ino)
			{
				errmsg = "Invalid backup file (use ! to override)";
				free(backup);
				backup = NULL;	/* there is no backup file to delete */
				goto nobackup;
			}
			remove(backup);
			if ((bfd = open(backup, O_WRONLY | O_CREAT, 0666)) < 0)
			{
				free(backup);
				backup = NULL;	/* there is no backup file to delete */
				errmsg = "Can't make backup file (use ! to override)";
				goto nobackup;
			}
		}
		/* set file protection same as original file, but strip s-bit */
		setperm(backup, perm & 0777);

		/* copy the file. */
		while ((buflen = read(fd, buf, BUFSIZE)) > 0)
		{
			if (write_buf(bfd, buf, buflen) == -1)
			{
				errmsg = "Can't write to backup file (use ! to override)";
				goto writeerr;
			}
		}
writeerr:
		close(bfd);
		if (buflen < 0)
			errmsg = "Can't read file for backup (use ! to override)";
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
		made_writable = TRUE;
			/* if we are writing to the current file, readonly makes no sense */
		if (fname == Filename || fname == sFilename)
			p_ro = FALSE;
 	}
#else /* UNIX */

/*
 * If we are not appending, the file exists, and the 'writebackup' or
 * 'backup' option is set, make a backup.
 * Do not make any backup, if "writebackup" and "backup" are 
 * both switched off. This helps when editing large files on
 * almost-full disks. (jw)
 */
	perm = getperm(fname);
	if (perm < 0)
		newfile = TRUE;
	else if (isdir(fname) > 0)
	{
		errmsg = "is a directory";
		goto fail;
	}
	if (!append && perm >= 0 && (p_wb || p_bk))
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
#ifdef AMIGA
			/*
			 * With MSDOS-compatible filesystems (crossdos, messydos) it is
			 * possible that the name of the backup file is the same as the
			 * original file. To avoid the chance of accidently deleting the
			 * original file (horror!) we lock it during the remove.
			 * This should not happen with ":w", because startscript() should
			 * detect this problem and set thisfile_sn, causing modname to
			 * return a correct ".bak" filename. This problem does exist with
			 * ":w filename", but then the original file will be somewhere else
			 * so the backup isn't really important. If autoscripting is off
			 * the rename may fail.
			 */
			flock = Lock((UBYTE *)fname, (long)ACCESS_READ);
#endif
			remove(backup);
#ifdef AMIGA
			if (flock)
				UnLock(flock);
#endif
			len = rename(fname, backup);
			if (len != 0)
			{
				if (forceit)
				{
					free(backup);	/* don't do the rename below */
					backup = NULL;
				}
				else
				{
					errmsg = "Can't make backup file (use ! to override)";
					goto fail;
				}
			}
		}
	}
#endif /* UNIX */

		/* 
		 * We may try to open the file twice: If we can't write to the
		 * file and forceit is TRUE we delete the existing file and try to create
		 * a new one. If this still fails we may have lost the original file!
		 * (this may happen when the user reached his quotum for number of files).
		 * Appending will fail if the file does not exist and forceit is FALSE.
		 */
	while ((fd = open(fname, O_WRONLY | (append ?
					(forceit ? (O_APPEND | O_CREAT) : O_APPEND) :
					(O_CREAT | O_TRUNC)), 0666)) < 0)
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
				made_writable = TRUE;
				if (old.st_uid != getuid() || old.st_gid != getgid())
					perm &= 0777;
#endif /* UNIX */
				if (!append)		/* don't remove when appending */
					remove(fname);
				continue;
			}
		}
/*
 * If we failed to open the file, we don't need a backup. Throw it away.
 * If we moved or removed the original file try to put the backup in its place.
 */
 		if (backup)
		{
#ifdef UNIX
			struct stat st;

			/*
			 * There is a small chance that we removed the original, try
			 * to move the copy in its place.
			 * This won't work if the backup is in another file system!
			 * In that case we leave the copy around.
			 */
			if (stat(fname, &st) < 0)	/* file does not exist */
				rename(backup, fname);	/* put the copy in its place */
			if (stat(fname, &st) >= 0)	/* original file does exist */
				remove(backup);			/* throw away the copy */
#else
 			rename(backup, fname);	/* try to put the original file back */
#endif
		}
 		goto fail;
 	}
	errmsg = NULL;

	if (end > line_count)
		end = line_count;
	len = 0;
	s = buffer;
	nchars = 0;
	for (lnum = start; lnum <= end; ++lnum)
	{
		/*
		 * The next while loop is done once for each character written.
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
			if (++len != bufsize)
				continue;
			if (write_buf(fd, buffer, bufsize) == -1)
			{
				end = 0;				/* write error: break loop */
				break;
			}
			nchars += bufsize;
			s = buffer;
			len = 0;
		}
			/* write failed or last line has no EOL: stop here */
		if (end == 0 || (p_bin && lnum == line_count && noendofline))
			break;
		if (p_tx)		/* write CR-NL */
		{
			*s = CR;
			++s;
			if (++len == bufsize)
			{
				if (write_buf(fd, buffer, bufsize) == -1)
				{
					end = 0;				/* write error: break loop */
					break;
				}
				nchars += bufsize;
				s = buffer;
				len = 0;
			}
		}
		*s = NL;
		++s;
		if (++len == bufsize && end)
		{
			if (write_buf(fd, buffer, bufsize) == -1)
			{
				end = 0;				/* write error: break loop */
				break;
			}
			nchars += bufsize;
			s = buffer;
			len = 0;
		}
	}
	if (len && end)
	{
		if (write_buf(fd, buffer, len) == -1)
			end = 0;				/* write error */
		nchars += len;
	}

	if (close(fd) != 0)
	{
		errmsg = "Close failed";
		goto fail;
	}
#ifdef UNIX
	if (made_writable)
		perm &= ~0200;			/* reset 'w' bit for security reasons */
#endif
	if (perm >= 0)
		setperm(fname, perm);	/* set permissions of new file same as old file */

	if (end == 0)
	{
		errmsg = "write error (file system full?)";
		goto fail;
	}

#ifdef MSDOS		/* the screen may be messed up by the "insert disk
							in drive b: and hit return" message */
	if (!exiting)
		updateScreen(CLEAR);
#endif

	lnum -= start;		/* compute number of written lines */
	smsg("\"%s\"%s%s %ld line%s, %ld character%s",
			fname,
			newfile ? " [New File]" : " ",
#ifdef MSDOS
			p_tx ? "" : "[notextmode]",
#else
			p_tx ? "[textmode]" : "",
#endif
			(long)lnum, plural((long)lnum),
			nchars, plural(nchars));
	if (whole && start == 1 && end == line_count)	/* when written everything */
	{
		UNCHANGED;
		NotEdited = FALSE;
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
 * write_buf: call write() to write a buffer
 */
	static int
write_buf(fd, buf, len)
	int		fd;
	char	*buf;
	int		len;
{
	int		wlen;

	while (len)
	{
		wlen = write(fd, buf, (size_t)len);
		if (wlen <= 0)				/* error! */
			return -1;
		len -= wlen;
		buf += wlen;
	}
	return 0;
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

	if (!p_ml || (nmlines = (int)p_mls) == 0)
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
		if (isspace(prev) && (strncmp(s, "vi:", (size_t)3) == 0 || strncmp(s, "ex:", (size_t)3) == 0 || strncmp(s, "vim:", (size_t)4) == 0))
		{
			do
				++s;
			while (s[-1] != ':');
			s = cs = strsave(s);
			if (cs == NULL)
				break;
			end = FALSE;
			while (end == FALSE)
			{
				while (*s == ' ' || *s == TAB)
					++s;
				if (*s == NUL)
					break;
				for (e = s; (*e != ':' || *(e - 1) == '\\') && *e != NUL; ++e)
					;
				if (*e == NUL)
					end = TRUE;
				*e = NUL;
				if (strncmp(s, "set ", (size_t)4) == 0) /* "vi:set opt opt opt: foo" */
				{
					doset(s + 4);
					break;
				}
				if (doset(s))		/* stop if error found */
					break;
				s = e + 1;
			}
			free(cs);
			break;
		}
		prev = *s;
	}
}
