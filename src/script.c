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

/*
 * startscript(): open automatic script file
 */
startscript()
{
	int n = 0;

	stopscript();		/* stop any old script */
	if (P(P_UC) == 0)	/* no auto script wanted */
		return;
	if (Changed)
		emsg("Warning: buffer already changed, auto script file will be incomplete");
	scriptname = modname(Filename, ".vim");
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
	}
}

stopscript()
{
	if (!autoscriptfd)
		return;
	fclose(autoscriptfd);
	remove(scriptname);		/* delete the file */
	autoscriptfd = NULL;
	free(scriptname);
}

/*
 * updatescipt() is called when a character has to be written into the script file
 * or when we have waited some time for a character (c == 0)
 */
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
 */
openrecover()
{
	char *fname;
	struct stat efile, rfile;

	fname = modname(Filename, ".vim");
	if (fname)
	{
		recoverymode = 0;
		if (stat(Filename, &efile) != -1 && stat(fname, &rfile) != -1 &&
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
/*
 * add extention to filename - change path/fo.o.h to path/fo.o.ext
 */
	char *
modname(fname, ext)
	char *fname, *ext;
{
	register char	*retval;
	register char   *s;
	register char   *ptr;
	register int	len;

	len = strlen(ext);
	retval = alloc((unsigned) (strlen(fname) + len + 1));
	if (retval != NULL)
	{
		strcpy(retval, fname);
		for (ptr = s = retval; *s; s++)
		{
			if (*s == ':' || *s == '/')
				ptr = s + 1;
		}
		if (s - ptr > 31 - len)			/* if filename too long, truncate it */
			s = ptr + 31 - len;
		strcpy(s, ext);
		if (strcmp(fname, retval) == 0)	/* filename already ended in ext */
			*s = '_';
	}
	return retval;
}
