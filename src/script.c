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

static char *scriptname;			/* name of the script in use */
static FILE *autoscriptfd = NULL;
static char *makescriptname __ARGS((void));

/*
 * startscript(): open automatic script file
 */
	void
startscript()
{
	int n = 0;

	stopscript();		/* stop any old script */
	if (P(P_UC) == 0)	/* no auto script wanted */
		return;
	if (Changed)
		emsg("Warning: buffer already changed, auto script file will be incomplete");
	scriptname = makescriptname();
	if (scriptname)
	{
		while (autoscriptfd = fopen(scriptname, "r"))	/* file already exists */
		{
			fclose(autoscriptfd);
			autoscriptfd = NULL;
			if (n == 0)				/* first time; give error message */
				emsg(".vim file exists: an edit of this file has not been finished");
			n = strlen(scriptname);
			if (n == 0 || scriptname[n-1] == '0') /* should not happen */
				return;
			--scriptname[n-1];				/* change the name */
		}
		autoscriptfd = fopen(scriptname, "w");		/* errors are ignored */
		script_winsize();			/* always start with a :win command */
	}
}

	void
stopscript()
{
	if (!autoscriptfd)
		return;
	fclose(autoscriptfd);
	remove(scriptname);		/* delete the file */
	autoscriptfd = NULL;
	free(scriptname);
}

extern char *notopen;

/*
 * open new script file
 * return 0 on success, 1 on error
 */
openscript(name)
	char *name;
{
	if (curscript + 1 == NSCRIPT)
	{
		emsg("scripts nested too deep");
		return 1;
	}
	else
	{
		if (scriptin[curscript] != NULL)	/* already reading script */
			++curscript;
		if ((scriptin[curscript] = fopen((char *)name, "r")) == NULL)
		{
			emsg(notopen);
			if (curscript)
				--curscript;
			return 1;
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

	if (scriptout)
		putc(c, scriptout);
	if (autoscriptfd == NULL || c == 0 && count == 0)		/* nothing to do */
		return;
	if (c)
		putc(c, autoscriptfd);
	if (c == 0 || ++count >= P(P_UC))
	{
		/*
		 * we have to close and open the file in order to really get the
		 * characters in the file to disk!
		 */
		fclose(autoscriptfd);
		autoscriptfd = fopen(scriptname, "a");
		count = 0;
	}
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
			windgoto(Cursrow, Curscol);
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
	if (*PS(P_DIR) == 0 || r == NULL)
		return r;

	for (fname = s = r; *s; ++s)		/* skip path */
	{
		if (*s == ':' || *s == '/')
			fname = s + 1;
	}

	s = alloc((unsigned)(strlen(PS(P_DIR)) + strlen(fname) + 1));
	if (s != NULL)
	{
		strcpy(s, PS(P_DIR));
		strcat(s, fname);
	}
	free(r);
	return s;
}

/*
 * add extention to filename - change path/fo.o.h to path/fo.o.h.ext
 */
	char *
modname(fname, ext)
	char *fname, *ext;
{
	register char	*retval;
	register char   *s;
	register char   *ptr;
	register int	fnamelen, extlen;

	extlen = strlen(ext);
	if (fname == NULL)
		fnamelen = 0;
	else
		fnamelen = strlen(fname);
	retval = alloc((unsigned) (fnamelen + extlen + 1));
	if (retval != NULL)
	{
		if (fname == NULL)
			*retval = NUL;
		else
			strcpy(retval, fname);
		for (ptr = s = retval; *s; s++)
		{
			if (*s == ':' || *s == '/')
				ptr = s + 1;
		}
		if (s - ptr > 30 - extlen)			/* if filename too long, truncate it */
			s = ptr + 30 - extlen;
		strcpy(s, ext);
		if (fname != NULL && strcmp(fname, retval) == 0)
			*s = '_';				/* filename already ended in ext */
	}
	return retval;
}

/*
 * the new window size must be used in scripts;
 * write a ":winsize width height" command to the (auto)script
 * Postpone this action if not in NORMAL State, otherwise we may insert the
 * command halfway another command.
 */
bool_t script_winsize_postponed = FALSE;

	void
script_winsize()
{
	char			buf[25];
	register char	*p;

	if (State != NORMAL)		/* postpone action */
	{
		script_winsize_postponed = TRUE;
		return;
	}

	sprintf(buf, ":win %d %d\r", Columns, Rows);
	for (p = buf; *p; ++p)
		updatescript(*p);
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
