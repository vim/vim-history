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
extern int dos2;					/* this is in amiga.c */
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
	int		n = 0;
	char	buf[25];

	script_started = TRUE;
#if AMIGA
/*
 * With Amiga DOS 2.0 the system may lockup with the sequence: write to .vim
 * file, close it, delete it, create a new .vim file and write to it.
 * The Delay seems to solve this problem, maybe because DOS gets a chance to
 * finish the close and delete of the old .vim file.
 */
	if (stopscript() && dos2)
		Delay(2L);		/* This should fix the lockup bug */
#else
	stopscript();		/* stop any old script */
#endif
	if (p_uc == 0)	/* no auto script wanted */
		return;
	if (Changed)
		emsg("Warning: buffer already changed, auto script file will be incomplete");
	scriptname = makescriptname();
	if (scriptname)
	{
		while (
#ifdef AMIGA
			dos2 ? ((autoscriptfd = (FILE *)Open((UBYTE *)scriptname, (long)MODE_OLDFILE)) != NULL) :
#endif
			((autoscriptfd = fopen(scriptname,
#ifdef MSDOS
												"rb"
#else
												"r"
#endif
													)) != 0))
							/* file already exists */
		{
#ifdef AMIGA
			if (dos2)
				Close((BPTR)autoscriptfd);
			else
#endif
				fclose(autoscriptfd);
			autoscriptfd = NULL;
			if (n == 0)				/* first time; give error message */
				emsg(".vim file exists: an edit of this file has not been finished");
			n = strlen(scriptname);
			/*
			 * If we are not able to find a name with the last character changed
			 * we probably have a device with shorter file names (e.g. MSDOS
			 * compatible). We should try with another change.
			 */
			if (n == 0 || scriptname[n-1] == 'a')
			{
				free(scriptname);
				return;
			}
			--scriptname[n-1];				/* change last char of the name */
		}
#ifdef AMIGA
		if (dos2)
			autoscriptfd = (FILE *)Open((UBYTE *)scriptname, (long)MODE_NEWFILE);		/* errors are ignored */
		else
#endif
			autoscriptfd = fopen(scriptname,
#ifdef MSDOS
												"wb"
#else
												"w"
#endif
													);		/* errors are ignored */
		script_winsize();			/* always start with a :win command */
		if (Curpos.lnum > 1 || Curpos.col > 0)
		{
			sprintf(buf, "%ldG0%dl", (long)Curpos.lnum, (int)Curpos.col);
			Supdatescript(buf);
		}

	}
}

	int
stopscript()
{
	if (!autoscriptfd)
		return FALSE;		/* nothing to stop */

#ifdef AMIGA
	if (dos2)
		Close((BPTR)autoscriptfd);
	else
#endif
		fclose(autoscriptfd);
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
		if ((scriptin[curscript] = fopen((char *)name,
#ifdef MSDOS
													"rb"
#else
													"r"
#endif
														)) == NULL)
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
		if (dos2)
			FPutC((BPTR)autoscriptfd, (unsigned long)c);
		else
#endif
			putc(c, autoscriptfd);
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
		if (dos2)
			Flush((BPTR)autoscriptfd);
		else
#endif
		{
			fclose(autoscriptfd);
			autoscriptfd = fopen(scriptname,
#ifdef MSDOS
												"ab"
#else
												"a"
#endif
													 );
		}
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

#ifndef MSDOS		/* MSDOS has only one extention, see msdos.c */
/*
 * add extention to filename - change path/fo.o.h to path/fo.o.h.ext
 *	Space for the new name is allocated.
 */
	char *
modname(fname, ext)
	char *fname, *ext;
{
	register char	*retval;
	register char   *s;
	register char   *ptr;
	register int	fnamelen, extlen;
			 char	currentdir[512];

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
		for (ptr = s = retval; *s; s++)
		{
#ifdef UNIX
			if (*s == '/')		/* UNIX has ':' inside file names */
#else
			if (*s == ':' || *s == PATHSEP)
#endif
				ptr = s + 1;
		}
		if (s - ptr > 30 - extlen)			/* if filename too long, truncate it */
			s = ptr + 30 - extlen;
		/*
		 * Replace all '.' by '_' in filename. This avoids problems with
		 * MSDOS-like filesystems.
		 */
		for ( ; *ptr; ++ptr)
			if (*ptr == '.')
				*ptr = '_';
		strcpy(s, ext);
		if (fname != NULL && strcmp(fname, retval) == 0)
			*s = '_';				/* filename already ended in ext */
	}
	return retval;
}
#endif	/* MSDOS */

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
