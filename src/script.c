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
 * script.c: functions for handling script files
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

static char *scriptname;			/* name of the script in use */
static FILE *autoscriptfd = NULL;
static char *makescriptname __ARGS((void));
static void Supdatescript __ARGS((char *));

extern int global_busy;			/* this is in csearch.c */

/*
 * for Amiga Dos 2.0x we use Open/Close/Flush instead of fopen/fclose
 */
#ifdef AMIGA
# ifndef NO_ARP
extern int dos2;					/* this is in amiga.c */
# endif
# ifdef SASC
#  include <proto/dos.h>
# endif
#endif

/*
 * We use this flag to avoid writing :win to commands to the script file
 * during startup.
 */
static int script_started = FALSE;

/*
 * startscript(): open automatic script file
 */
	void
startscript()
{
	int		n;
	char	buf[25];
#ifdef AMIGA
	int		r;
	FILE	*dummyfd = NULL;
#endif

	script_started = TRUE;

#ifdef AMIGA
/*
 * With Amiga DOS 2.0 the system may lockup with the sequence: write to .vim
 * file, close it, delete it, create a new .vim file and write to it.
 * This is a problem in the filesystem hash chains (solved in version 39.xx).
 * The Delay seems to solve this problem, maybe because DOS gets a chance to
 * finish closing and deleting the old .vim file. Also do this for DOS 1.3,
 * just in case.
 */
	if (stopscript())
		Delay(10L);		/* This should fix the lockup bug */
#else
	stopscript();		/* stop any old script */
#endif

	if (p_uc == 0 || exiting)	/* no auto script wanted/needed */
		return;
	if (Changed)
		emsg("Warning: buffer already changed, auto script file will be incomplete");

#ifdef AMIGA
/*
 * If we start editing a new file, e.g. "test.doc", which resides on an MSDOS
 * compatible filesystem, it is possible that the file "test.doc.vim" which we
 * create will be exactly the same file. To avoid this problem we temporarily
 * create "test.doc".
 */
	if (!(p_sn || thisfile_sn) && Filename && getperm(Filename) < 0)
		dummyfd = fopen(Filename, "w");
#endif

/*
 * we try different names until we find one that does not exist yet
 */
	scriptname = makescriptname();
	for (;;)
	{
		if (scriptname == NULL)		/* must be out of memory */
			break;
		if ((n = strlen(scriptname)) == 0)	/* safety check */
		{
			free(scriptname);
			break;
		}
		
		/*
		 * check if the scriptfile already exists
		 */
		if (getperm(scriptname) < 0)		/* it does not exist */
		{
				/*
				 * Create the autoscript file.
				 */
#ifdef AMIGA
# ifndef NO_ARP
			if (dos2)
# endif
				autoscriptfd = (FILE *)Open((UBYTE *)scriptname, (long)MODE_NEWFILE);
# ifndef NO_ARP
			else
				autoscriptfd = fopen(scriptname, "w");
# endif
#else	/* !AMIGA */
				autoscriptfd = fopen(scriptname, WRITEBIN);
#endif	/* AMIGA */

#ifdef AMIGA
			/*
			 * on the Amiga getperm() will return -1 when the file exists but
			 * is being used by another program. This happens if you edit
			 * a file twice.
			 */
			if (autoscriptfd != NULL || (IoErr() != ERROR_OBJECT_IN_USE && IoErr() != ERROR_OBJECT_EXISTS))
#endif
				break;
		}
	/*
	 * get here when file already exists
	 */
		if (scriptname[n - 1] == 'm')		/* first try */
		{
#ifdef AMIGA
		/*
		 * on MS-DOS compatible filesystems (e.g. messydos) file.doc.vim
		 * and file.doc are the same file. To guess if this problem is
		 * present try if file.doc.vix exists. If it does, we set thisfile_sn
		 * and try file_doc.vim (dots replaced by underscores for this file),
		 * and try again. If it doesn't we assume that "file.doc.vim" already
		 * exists.
		 */
			if (!(p_sn || thisfile_sn))		/* not tried yet */
			{
				scriptname[n - 1] = 'x';
				r = getperm(scriptname);	/* try "file.vix" */
				scriptname[n - 1] = 'm';
				if (r >= 0)					/* it seems to exist */
				{
					thisfile_sn = TRUE;
					free(scriptname);
					scriptname = makescriptname();	/* '.' replaced by '_' */
					continue;						/* try again */
				}
			}
#endif
			/* if we get here ".vim" file really exists */
			emsg(".vim file exists: an edit of this file has not been finished");
		}

		if (scriptname[n - 1] == 'a')	/* tried enough names, give up */
		{
			free(scriptname);
			break;
		}
		--scriptname[n - 1];				/* change last char of the name */
	}
	if (autoscriptfd != NULL)		/* ".vim" file has been created */
	{
		script_winsize();			/* always start with a :win command */
									/* output cursor position if neccessary */
		if (Curpos.lnum > 1 || Curpos.col > 0)
		{
			sprintf(buf, "%ldG0%dl", (long)Curpos.lnum, (int)Curpos.col);
			Supdatescript(buf);
		}
	}

#ifdef AMIGA
	if (dummyfd)		/* file has been created temporarily */
	{
		fclose(dummyfd);
		remove(Filename);
	}
#endif
}

	int
stopscript()
{
	if (!autoscriptfd)
		return FALSE;		/* nothing to stop */

#ifdef AMIGA
# ifndef NO_ARP
	if (dos2)
# endif
		Close((BPTR)autoscriptfd);
# ifndef NO_ARP
	else
		fclose(autoscriptfd);
# endif
#else
	fclose(autoscriptfd);
#endif
	remove(scriptname);		/* delete the file */
	autoscriptfd = NULL;
	free(scriptname);
	return TRUE;
}

/*
 * open new script file
 * return 0 on success, 1 on error
 */
	int
openscript(name)
	char *name;
{
	int oldcurscript;

	if (curscript + 1 == NSCRIPT)
	{
		emsg(e_nesting);
		return 1;
	}
	else
	{
		if (scriptin[curscript] != NULL)	/* already reading script */
			++curscript;
		if ((scriptin[curscript] = fopen((char *)name, READBIN)) == NULL)
		{
			emsg(e_notopen);
			if (curscript)
				--curscript;
			return 1;
		}
		/*
		 * With command ":g/pat/so! file" we have to execute the
		 * commands from the file now.
		 */
		if (global_busy)
		{
			State = NORMAL;
			oldcurscript = curscript;
			do
			{
				normal();
				vpeekc();			/* check for end of file */
			}
			while (scriptin[oldcurscript]);
			State = CMDLINE;
		}
	}
	return 0;
}

/*
 * updatescipt() is called when a character has to be written into the script file
 * or when we have waited some time for a character (c == 0)
 */
	void
updatescript(c)
	int c;
{
	static int count = 0;

	if (c && scriptout)
		putc(c, scriptout);
	if (autoscriptfd == NULL || (c == 0 && count == 0))		/* nothing to do */
		return;
	if (c)
	{
#ifdef AMIGA
# ifndef NO_ARP
		if (dos2)
# endif
			FPutC((BPTR)autoscriptfd, (unsigned long)c);
# ifndef NO_ARP
		else
			putc(c, autoscriptfd);
# endif
#else
		putc(c, autoscriptfd);
#endif
		++count;
	}
	if ((c == 0 || count >= p_uc) && Updated)
	{
		/*
		 * Before DOS 2.0x we have to close and open the file in order to really
		 * get the characters in the file to disk!
		 * With DOS 2.0x Flush() can be used for that
		 */
#ifdef AMIGA
# ifndef NO_ARP
		if (dos2)
# endif
			Flush((BPTR)autoscriptfd);
# ifndef NO_ARP
		else
		{
			fclose(autoscriptfd);
			autoscriptfd = fopen(scriptname, "a");
		}
# endif
#else 	/* !AMIGA */
		fclose(autoscriptfd);
# ifdef MSDOS
		autoscriptfd = fopen(scriptname, "ab");
# else
		autoscriptfd = fopen(scriptname, "a");
# endif
#endif
		count = 0;
		Updated = 0;
	}
}

	static void
Supdatescript(str)
	char *str;
{
	while (*str)
		updatescript(*str++);
}

/*
 * try to open the ".vim" file for recovery
 * if recoverymode is 1: start recovery, set recoverymode to 2
 * if recoverymode is 2: stop recovery mode
 */
	void
openrecover()
{
	char *fname;
	struct stat efile, rfile;

	if (recoverymode == 2)		/* end of recovery */
	{
		if (got_int)
		{
			emsg("Recovery Interrupted");
				/* somehow the cursor ends up in the wrong place (why?) */
			setcursor();
			flushbuf();
		}
		else
			msg("Recovery completed");
		recoverymode = 0;
	}
	else
	{
		fname = makescriptname();
		if (fname)
		{
			recoverymode = 2;
			if (Filename != NULL &&
					stat(Filename, &efile) != -1 &&
					stat(fname, &rfile) != -1 &&
					efile.st_mtime > rfile.st_mtime)
				emsg(".vim file is older; file not recovered");
			else
			{
				if (openscript(fname))
					emsg("Cannot open .vim file; file not recovered");
			}
			free(fname);
		}
	}
}

/*
 * make script name out of the filename
 */
	static char *
makescriptname()
{
	char *r, *s, *fname;

	r = modname(Filename, ".vim");
	if (*p_dir == 0 || r == NULL)
		return r;

	for (fname = s = r; *s; ++s)		/* skip path */
	{
#ifdef UNIX
		if (*s == '/')		/* UNIX has ':' inside file names */
#else
		if (*s == ':' || *s == PATHSEP)
#endif
			fname = s + 1;
	}

	s = alloc((unsigned)(strlen(p_dir) + strlen(fname) + 1));
	if (s != NULL)
	{
		strcpy(s, p_dir);
		strcat(s, fname);
	}
	free(r);
	return s;
}

/*
 * add extention to filename - change path/fo.o.h to path/fo.o.h.ext or
 * fo_o_h.ext for MSDOS or when dotfname option reset.
 *
 * Assumed that fname is a valid name found in the filesystem we assure that
 * the return value is a different name and ends in ".ext".
 * "ext" MUST start with a "." and MUST be at most 4 characters long.
 * Space for the returned name is allocated, must be freed later.
 */

	char *
modname(fname, ext)
	char *fname, *ext;
{
	char			*retval;
	register char   *s;
	register char   *ptr;
	register int	fnamelen, extlen;
	char			currentdir[512];

	extlen = strlen(ext);

	/*
	 * if there is no filename we must get the name of the current directory
	 * (we need the full path in case :cd is used)
	 */
	if (fname == NULL || *fname == NUL)
	{
		(void)dirname(currentdir, 511);
		strcat(currentdir, PATHSEPSTR);
		fnamelen = strlen(currentdir);
	}
	else
		fnamelen = strlen(fname);
	retval = alloc((unsigned) (fnamelen + extlen + 1));
	if (retval != NULL)
	{
		if (fname == NULL || *fname == NUL)
			strcpy(retval, currentdir);
		else
			strcpy(retval, fname);
		/*
		 * search backwards until we hit a '\' or ':' replacing all '.' by '_'
		 * for MSDOS or when dotfname option reset.
		 * Then truncate what is after the '\' or ':' to 8 characters for MSDOS
		 * and 26 characters for AMIGA and UNIX.
		 */
		for (ptr = retval + fnamelen; ptr >= retval; ptr--)
		{
#ifndef MSDOS
			if (p_sn || thisfile_sn)
#endif
				if (*ptr == '.')	/* replace '.' by '_' */
					*ptr = '_';
#ifdef UNIX
			if (*ptr == '/')		/* UNIX has ':' inside file names */
#else
			if (*ptr == ':' || *ptr == PATHSEP)
#endif
				break;
		}
		ptr++;

		/* the filename has at most BASENAMELEN characters. */
		if (strlen(ptr) > BASENAMELEN)
			ptr[BASENAMELEN] = '\0';
#ifndef MSDOS
		if ((p_sn || thisfile_sn) && strlen(ptr) > 8)
			ptr[8] = '\0';
#endif
		s = ptr + strlen(ptr);

		/*
		 * Append the extention.
		 * ext must start with '.' and cannot exceed 3 more characters.
		 */
		strcpy(s, ext);
		if (fname != NULL && strcmp(fname, retval) == 0)
		{
			/* after modification still the same name? */
			/* we search for a character that can be replaced by '_' */
			while (--s >= ptr)
			{
				if (*s != '_')
				{
					*s = '_';
					break;
				}
			}
			if (s < ptr)
			{
				/* fname was "________.<ext>" how tricky! */
				*ptr = 'v';
			}
		}
	}
	return retval;
}

/*
 * the new window size must be used in scripts;
 * write a ":winsize width height" command to the (auto)script
 * Postpone this action if not in NORMAL State, otherwise we may insert the
 * command halfway another command.
 */
int script_winsize_postponed = FALSE;

	void
script_winsize()
{
	char			buf[25];

	if (!script_started || State != NORMAL)		/* postpone action */
	{
		script_winsize_postponed = TRUE;
		return;
	}

	sprintf(buf, ":win %d %d\r", (int)Columns, (int)Rows);
	Supdatescript(buf);
	script_winsize_postponed = FALSE;
}

/*
 * This function is called after each "State = NORMAL"
 */
	void
script_winsize_pp()
{
	if (script_winsize_postponed)
		script_winsize();
}
