/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * os_mac.c -- code for the MacOS
 *
 * This file is mainly based on os_unix.c.
 */

#include "vim.h"

#if defined(__MRC__) || defined(__SC__) /* for Apple MPW Compilers */
#include "StandardFile.h"
/*
 * Implements the corresponding unix function
 */
    int
stat(
    char *p,
    struct stat *p_st)
{
    /*
       TODO: Use functions which fill the FileParam struct (Files.h)
             and copy these contents to our self-defined stat struct
     */
    return 0;
}
/*
 * change the current working directory
 */
    int
mch_chdir(char *p_name)
{
    /* TODO */
    return FAIL;
}
#endif
/*
 * Recursively build up a list of files in "gap" matching the first wildcard
 * in `path'.  Called by mch_expandpath().
 * "path" has backslashes before chars that are not to be expanded.
 */
    int
mac_expandpath(
    garray_t	*gap,
    char_u	*path,
    int		flags,		/* EW_* flags */
    short	start_at,
    short	as_full)
{
    /*
     * TODO:
     *		+Get Volumes (when looking for files in current dir)
     *		+Make it work when working dir not on select volume
     *		+Cleanup
     */
    short	index = 1;
    OSErr	gErr;
    char_u	dirname[256];
    char_u	cfilename[256];
    long	dirID;
    char_u	*new_name;
    CInfoPBRec	gMyCPB;
    HParamBlockRec gMyHPBlock;
    FSSpec	usedDir;

    char_u	*buf;
    char_u	*p, *s, *e, dany;
    int		start_len, c;
    char_u	*pat;
    regmatch_t	regmatch;

    start_len = gap->ga_len;
    buf = alloc(STRLEN(path) + BASENAMELEN + 5);/* make room for file name */
    if (buf == NULL)
	return 0;

/*
 * Find the first part in the path name that contains a wildcard.
 * Copy it into buf, including the preceding characters.
 */
    p = buf;
    s = buf;
    e = NULL;
#if 1
    STRNCPY(buf, path, start_at);
    p += start_at;
    path += start_at;
#endif

    while (*path)
    {
	if (*path == ':')
	{
	    if (e)
		break;
	    else
		s = p + 1;
	}
	/* should use  WILCARDLIST but what about ` */
	/*	    if (vim_strchr((char_u *)"*?[{~$", *path) != NULL)*/
	else if (vim_strchr((char_u *)WILDCHAR_LIST, *path) != NULL)
	    e = p;
#ifdef FEAT_MBYTE
	if (has_mbyte)
	{
	    int len = (*mb_ptr2len_check)(path);

	    STRNCPY(p, path, len);
	    p += len;
	    path += len;
	}
	else
#endif
	    *p++ = *path++;
    }
    e = p;

    /* now we have one wildcard component between s and e */
    *e = NUL;

#if 1
    dany = *s;
    *s = NUL;
    backslash_halve(buf);
    *s = dany;
#endif

    /* convert the file pattern to a regexp pattern */
    pat = file_pat_to_reg_pat(s, e, NULL, FALSE);
    if (pat == NULL)
    {
	vim_free(buf);
	return 0;
    }

    /* compile the regexp into a program */
    regmatch.rm_ic = FALSE;			/* Don't ever ignore case */
    regmatch.regprog = vim_regcomp(pat, TRUE);
    vim_free(pat);

    if (regmatch.regprog == NULL)
    {
	vim_free(buf);
	return 0;
    }

    /* open the directory for scanning */
    c = *s;
    *s = NUL;

    if (*buf == NUL)
    {
	as_full = TRUE;
#if 0
	(void) mch_dirname (&dirname[1], 254);
	dirname[0] = STRLEN(&dirname[1]);
#endif
    }
    else
    {
	if (*buf == ':')  /* relative path */
	{
	    (void)mch_dirname(&dirname[1], 254);
	    new_name = concat_fnames(&dirname[1], buf+1, TRUE);
	    STRCPY(&dirname[1], new_name);
	    dirname[0] = STRLEN(new_name);
	    vim_free(new_name);
	}
	else
	{
	    STRCPY(&dirname[1], buf);
	    backslash_halve(&dirname[1]);
	    dirname[0] = STRLEN(buf);
	}
    }
    *s = c;

    FSMakeFSSpec (0, 0, dirname, &usedDir);

    gMyCPB.dirInfo.ioNamePtr    = dirname;
    gMyCPB.dirInfo.ioVRefNum    = usedDir.vRefNum;
    gMyCPB.dirInfo.ioFDirIndex  = 0;
    gMyCPB.dirInfo.ioDrDirID    = 0;

    gErr = PBGetCatInfo(&gMyCPB, false);

    gMyCPB.dirInfo.ioCompletion = NULL;
    dirID = gMyCPB.dirInfo.ioDrDirID;
    do
    {
	gMyCPB.hFileInfo.ioFDirIndex = index;
	gMyCPB.hFileInfo.ioDirID     = dirID;

	gErr = PBGetCatInfo(&gMyCPB,false);

	if (gErr == noErr)
	{
	    STRNCPY (cfilename, &dirname[1], dirname[0]);
	    cfilename[dirname[0]] = 0;
	    if (vim_regexec(&regmatch, cfilename, (colnr_t)0))
	    {
		if (s[-1] != ':')
		{
		    /* TODO: need to copy with cleaned name */
		    STRCPY(s+1, cfilename);
		    s[0] = ':';
		}
		else
		{   /* TODO: need to copy with cleeaned name */
		    STRCPY(s, cfilename);
		}
		start_at = STRLEN(buf);
		STRCAT(buf, path);
		if (mch_has_wildcard(path))	/* handle more wildcard */
		    (void)mac_expandpath(gap, buf, flags, start_at, FALSE);
		else
		{
#ifdef DONT_ADD_PATHSEP_TO_DIR
		    if ((gMyCPB.hFileInfo.ioFlAttrib & ioDirMask) !=0 )
			STRCAT(buf, PATHSEPSTR);
#endif
		    addfile(gap, buf, flags);
		}
	    }
	    if ((gMyCPB.hFileInfo.ioFlAttrib & ioDirMask) !=0 )
	    {
	    }
	    else
	    {
	    }
	}
	index++;
    }
    while (gErr == noErr);

    if (as_full)
    {
	index = 1;
	do
	{
	    gMyHPBlock.volumeParam.ioNamePtr = (char_u *) dirname;
	    gMyHPBlock.volumeParam.ioVRefNum =0;
	    gMyHPBlock.volumeParam.ioVolIndex = index;

	    gErr = PBHGetVInfo (&gMyHPBlock,false);
	    if (gErr == noErr)
	    {
		STRNCPY (cfilename, &dirname[1], dirname[0]);
		cfilename[dirname[0]] = 0;
		if (vim_regexec(&regmatch, cfilename, (colnr_t)0))
		{
		    STRCPY(s, cfilename);
		    STRCAT(buf, path);
		    if (mch_has_wildcard(path))	/* handle more wildcard */
			(void)mac_expandpath(gap, s, flags, 0, FALSE);
		    else
		    {
#ifdef DONT_ADD_PATHSEP_TO_DIR
/*			if ((gMyCPB.hFileInfo.ioFlAttrib & ioDirMask) !=0 )
*/			    STRCAT(buf, PATHSEPSTR);
#endif
			addfile(gap, s, flags);
		    }
#if 0
		    STRCAT(cfilename, PATHSEPSTR);
		    addfile (gap, cfilename, flags);
#endif
		}
	    }
	    index++;
	}
	while (gErr == noErr);
    }

    vim_free(regmatch.regprog);

    return gap->ga_len - start_len;
}

/*
 * Recursively build up a list of files in "gap" matching the first wildcard
 * in `path'.  Called by expand_wildcards().
 * "pat" has backslashes before chars that are not to be expanded.
 */
    int
mch_expandpath(
    garray_t	*gap,
    char_u	*path,
    int		flags)		/* EW_* flags */
{

    char_u first = *path;
    short  scan_volume;

    slash_n_colon_adjust(path);

    scan_volume = (first != *path);

    return mac_expandpath(gap, path, flags, 0, scan_volume);
}

    void
fname_case(name)
    char_u  *name;
{
    /*
     * TODO: get the real casing for the file
     *       make it called
     *       with USE_FNAME_CASE & USE_LONG_FNAME
     *		    CASE_INSENSITIVE_FILENAME
     *       within setfname, fix_fname, do_ecmd
     */
}
static char_u	*oldtitle = (char_u *) "gVim";

/*
 * check for an "interrupt signal": CTRL-break or CTRL-C
 */
    void
mch_breakcheck()
{
    /*
     * TODO: Scan event for a CTRL-C or COMMAND-. and do: got_int=TRUE;
     *       or only go proccess event?
     *       or do nothing
     */
    EventRecord theEvent;

    if (EventAvail (keyDownMask, &theEvent))
	if ((theEvent.message & charCodeMask) == Ctrl_C)
	    got_int = TRUE;
#if 0
    short	i = 0;
    Boolean     found = false;
    EventRecord theEvent;

    while ((i < 10) && (!found))
    {
	found = EventAvail (keyDownMask, &theEvent);
	if (found)
	{
	  if ((theEvent.modifiers & controlKey) != 0)
	    found = false;
	  if ((theEvent.what == keyDown))
	    found = false;
	  if ((theEvent.message & charCodeMask) == Ctrl_C)
	    {
		found = false;
		got_int = TRUE;
	    }
	}
	i++;
    }
#endif

}

/*
 * Return amount of memory currently available.
 */
    long_u
mch_avail_mem(special)
    int     special;
{
    /*
     * TODO: Use MaxBlock, FreeMeM, PurgeSpace, MaxBlockSys FAQ-266
     *       figure out what the special is for
     *
     * FreeMem  ->   returns all avail memory is application heap
     * MaxBlock ->   returns the biggest contigeous block in application heap
     * PurgeSpace ->
     */
    return MaxBlock();
}

    void
mch_delay(msec, ignoreinput)
    long	msec;
    int		ignoreinput;
{
#if (defined(__MWERKS__) && __MWERKS__ >= 0x2000) \
	|| defined(__MRC__) || defined(__SC__)
    unsigned
#endif
    long   finalTick;

    if (ignoreinput)
	Delay (60*msec/1000, &finalTick);
    else
	/* even thougth we should call gui stuff from here
	   it the simplest way to be safe */
	gui_mch_wait_for_chars(msec);
}

    void
mch_shellinit()
{
    /*
     *  TODO: Verify if needed, or override later.
     */
    Columns = 80;
    Rows = 24;
}

/*
 * Check_win checks whether we have an interactive stdout.
 */
    int
mch_check_win(argc, argv)
    int		argc;
    char	**argv;
{
    /*
     * TODO: Maybe to be remove through NO_CONSOLE
     */
    return OK;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    int
mch_input_isatty()
{
    /*
     * TODO: Maybe to be remove through NO_CONSOLE
     */
    return OK;
}

#ifdef FEAT_TITLE
/*
 * Set the window title and icon.
 * (The icon is not taken care of).
 */
    void
mch_settitle(title, icon)
    char_u *title;
    char_u *icon;
{
    /*
     *  TODO: Clean C-Pscal conversion
     */
    char_u   pascal_title[1024];

    if (title == NULL)		/* nothing to do */
	return;

    if (title != NULL)
    {
	pascal_title[0] = (char_u) STRLEN(title);
	pascal_title[1] = 0;
	STRCAT (&pascal_title, title);
	SetWTitle(gui.VimWindow, (char_u *) &pascal_title);
    }
}

/*
 * Restore the window/icon title.
 * which is one of:
 *	1  Just restore title
 *      2  Just restore icon
 *	3  Restore title and icon
 * but don't care about the icon.
 */
    void
mch_restore_title(which)
    int which;
{
    mch_settitle((which & 1) ? oldtitle : NULL, NULL);
}
#endif

/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    int
mch_get_user_name(s, len)
    char_u	*s;
    int		len;
{
#if !(defined(__MRC__) || defined(__SC__)) /* No solution yet */
    /*
     * TODO: clean up and try getlogin ()
     */
#if defined(HAVE_PWD_H) && defined(HAVE_GETPWUID)
    struct passwd	*pw;
#endif
    uid_t		uid;

    uid = getuid();
#if defined(HAVE_PWD_H) && defined(HAVE_GETPWUID)
    if ((pw = getpwuid(uid)) != NULL
	    && pw->pw_name != NULL && *(pw->pw_name) != NUL)
    {
	STRNCPY(s, pw->pw_name, len);
	return OK;
    }
#endif
    sprintf((char *)s, "%d", (int)uid);		/* assumes s is long enough */
#endif
    return FAIL;				/* a number is not a name */
}

/*
 * Insert host name is s[len].
 */

    void
mch_get_host_name(s, len)
    char_u	*s;
    int		len;
{
#if defined(__MRC__) || defined(__SC__)
    s[0] = '\0'; /* TODO: use Gestalt information */
#else
    struct utsname vutsname;

    uname(&vutsname);
    STRNCPY(s, vutsname.nodename, len);
#endif
}

/*
 * return process ID
 */
    long
mch_get_pid()
{
    return (long)getpid();
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    int
mch_dirname(buf, len)
    char_u	*buf;
    int		len;
{
#if defined(__MRC__) || defined(__SC__)
    return FAIL; /* No solution yet */
#else
    /* The last : is already put by getcwd */
    if (getcwd((char *)buf, len) == NULL)
	{
	    STRCPY(buf, strerror(errno));
	    return FAIL;
	}
    else if (*buf != NUL && buf[STRLEN(buf) - 1] == ':')
	buf[STRLEN(buf) - 1] = NUL;	/* remove trailing ':' */
    return OK;
#endif
}

    void
slash_n_colon_adjust (buf)
    char_u *buf;
{
    /*
     * TODO: Make it faster
     */

    char_u  temp[MAXPATHL];
    char_u  *first_colon = vim_strchr(buf, ':');
    char_u  *first_slash = vim_strchr(buf, '/');
    int     full = TRUE;
    char_u  *scanning;
    char_u  *filling;
    char_u  last_copied = NUL;

    if (*buf == NUL)
	return ;

    if ((first_colon == NULL) && (first_slash == NULL))
	full = FALSE;
    if ((first_slash == NULL) && (first_colon != NULL))
	full = TRUE;
    if ((first_colon == NULL) && (first_slash != NULL))
	full =	FALSE;
    if ((first_slash < first_colon) && (first_slash != NULL))
	full = FALSE;
    if ((first_colon < first_slash) && (first_colon != NULL))
	full = TRUE;
    if (first_slash == buf)
	full = TRUE;
    if (first_colon == buf)
	full = FALSE;

    scanning = buf;
    filling  = temp;

    while (*scanning != NUL)
    {
	if (*scanning == '/')
	{
	    if ((scanning[1] != '/') && (scanning[-1] != ':'))
	    {
		*filling++ = ':';
		scanning++;
	    }
	    else
		scanning++;
	}
	else if (*scanning == '.')
	{
	    if ((scanning[1] == NUL) || scanning[1] == '/')
	    {
		if (scanning[1] == NUL)
		    scanning += 1;
		else
		    scanning += 2;
	    }
	    else if (scanning[1] == '.')
	    {
		if ((scanning[2] == NUL) || scanning[2] == '/')
		{
		    *filling++ = ':';
		    if (scanning[2] == NUL)
			scanning +=2;
		    else
			scanning += 3;
		}
		else
		{
		    *filling++ = *scanning++;
		}
	    }
	    else
	    {
		*filling++ = *scanning++;
	    }

	}
	else
	{
	    *filling++ = *scanning++;
	}

    }

    *filling = 0;
    filling = temp;

    if (!full)
    {
	if (buf[0] != ':')
	{
	    buf[0] = ':';
	    buf[1] = NUL;
	}
	else
	    buf[0] = NUL;
    }
    else
    {
	buf[0] = NUL;
	if (filling[0] == ':')
	    filling++;
    }

    STRCAT (buf, filling);

}

/*
 * Get absolute filename into buffer 'buf' of length 'len' bytes.
 *
 * return FAIL for failure, OK for success
 */
    int
mch_FullName(fname, buf, len, force)
    char_u  *fname, *buf;
    int     len;
    int     force;	    /* also expand when already absolute path name */
{
    /*
     * TODO: Find what TODO
     */
    int		l;
    char_u	olddir[MAXPATHL];
    char_u	newdir[MAXPATHL];
    char_u	*p;
    char_u	c;
    int		retval = OK;

    if (force || !mch_isFullName(fname))
    {
	/*
	 * Forced or not an absolute path.
	 * If the file name has a path, change to that directory for a moment,
	 * and then do the getwd() (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	if ((p = vim_strrchr(fname, ':')) != NULL)
	{
	    p++;
	    if (mch_dirname(olddir, MAXPATHL) == FAIL)
	    {
		p = NULL;		/* can't get current dir: don't chdir */
		retval = FAIL;
	    }
	    else
	    {
		c = *p;
		*p = NUL;
		if (mch_chdir((char *)fname))
		    retval = FAIL;
		else
		    fname = p; /* + 1;*/
		*p = c;
	    }
	}
	if (mch_dirname(buf, len) == FAIL)
	{
	    retval = FAIL;
	    *newdir = NUL;
	}
	l = STRLEN(buf);
	if (l > 0 && buf[l - 1] != ':' && *fname != NUL)
	    STRCAT(buf, ":");
	if (p != NULL)
	    mch_chdir((char *)olddir);
	STRCAT(buf, fname);
    }
    else
    {
	STRNCPY(buf, fname, len);
	buf[len - 1] = NUL;
	slash_n_colon_adjust(buf);
    }

    return retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
	int
mch_isFullName(fname)
	char_u		*fname;
{
    /*
     * TODO: Make sure fname is always of mac still
     *       i.e: passed throught slash_n_colon_adjust
     */
	char_u	*first_colon = vim_strchr(fname, ':');
	char_u	*first_slash = vim_strchr(fname, '/');

	if (first_colon == fname)
	  return FALSE;
	if (first_slash == fname)
	  return TRUE;
	if ((first_colon < first_slash) && (first_colon != NULL))
	  return TRUE;
	if ((first_slash < first_colon) && (first_slash != NULL))
	  return FALSE;
	if ((first_colon == NULL) && (first_slash != NULL))
	  return FALSE;
	if ((first_slash == NULL) && (first_colon != NULL))
	  return TRUE;
	if ((first_colon == NULL) && (first_slash == NULL))
	  return FALSE;
	return TRUE;
}

/*
 * Replace all slashes by colons.
 */
    void
slash_adjust(p)
    char_u  *p;
{
    /*
     * TODO: keep escaped '/'
     */

    while (*p)
    {
	if (*p == '/')
	    *p = ':';
	++p;
    }
}

/*
 * Get file permissions for 'name'.
 * Returns -1 when it doesn't exist.
 */
    long
mch_getperm(name)
    char_u *name;
{
    /*
     * TODO: Maybe use AppleShare info??
     *       Use locked for non writable
     */

    struct stat statb;

    if (stat((char *)name, &statb))
	return -1;
    return statb.st_mode;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    int
mch_setperm(name, perm)
    char_u	*name;
    long	perm;
{
    /*
     * TODO: Maybe use AppleShare info??
     *       Use locked for non writable
     */
    return (OK);
}

/*
 * Set hidden flag for "name".
 */
    void
mch_hide(name)
    char_u  *name;
{
    /*
     * TODO: Hide the file throught FileManager FAQ 8-34
     *
     *  *name is mac style start with : for relative
     */
}


/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    int
mch_isdir(name)
    char_u  *name;
{
    /*
     * TODO: Find out by FileManager calls ...
     */
    struct stat statb;

    if (stat((char *)name, &statb))
	return FALSE;
#if defined(__MRC__) || defined(__SC__)
    return FALSE;   /* definitely TODO */
#else
    return ((statb.st_mode & S_IFMT) == S_IFDIR ? TRUE : FALSE);
#endif
}

#if defined(FEAT_EVAL) || defined(PROTO)
/*
 * Return 1 if "name" can be executed, 0 if not.
 * Return -1 if unknown.
 */
    int
mch_can_exe(name)
    char_u	*name;
{
    /* TODO */
    return -1;
}
#endif

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    int
mch_nodetype(name)
    char_u	*name;
{
    /* TODO */
    return NODE_NORMAL;
}

    void
mch_init()
{
}

    void
mch_windexit(r)
    int     r;
{
    mch_display_error();

    ml_close_all(TRUE);		/* remove all memfiles */
    exit(r);
}

static int curr_tmode = TMODE_COOK;     /* contains current terminal mode */

    void
mch_settmode(tmode)
    int     tmode;
{
    /*
     * TODO: remove the needs of it.
     */
}

#ifdef FEAT_MOUSE
/*
 * set mouse clicks on or off (only works for xterms)
 */
    void
mch_setmouse(on)
    int     on;
{
    /*
     * TODO: remove the needs of it.
     */
}
#endif

/*
 * set screen mode, always fails.
 */
    int
mch_screenmode(arg)
    char_u	 *arg;
{
    EMSG(_("Screen mode setting not supported"));
    return FAIL;
}

    int
mch_call_shell(cmd, options)
    char_u	*cmd;
    int		options;	/* SHELL_*, see vim.h */
{
    /*
     * TODO: find a shell or pseudo-shell to call
     *       for some simple useful command
     */

    return (-1);
}

	int
mch_has_wildcard(p)
	char_u	*p;
{
    for ( ; *p; ++p)
    {
	if (*p == '\\' && p[1] != NUL)
	    ++p;
	else if (vim_strchr((char_u *)WILDCHAR_LIST, *p) != NULL)
	    return TRUE;
    }
    return FALSE;
}

/*
 * Convert a FSSpec to a fuill path
 */

void GetFullPathFromFSSpec (char_u *fname, FSSpec file)
{
    /*
     * TODO: Add protection for 256 char max.
     */
    CInfoPBRec  theCPB;
    Str255      directoryName;
    OSErr       error;
    int		folder = 1;

    *fname = 0;

    theCPB.dirInfo.ioNamePtr = directoryName;
    theCPB.dirInfo.ioDrParID = file.parID;

    if ((TRUE) /*&& (file.parID != fsRtDirID)*/ && (file.parID != fsRtParID))
    do
    {
	theCPB.dirInfo.ioVRefNum   = file.vRefNum;
	theCPB.dirInfo.ioFDirIndex = -1;
	theCPB.dirInfo.ioDrDirID   = theCPB.dirInfo.ioDrParID;

	error = PBGetCatInfo (&theCPB,false);

	directoryName[directoryName[0] + 1] = 0;
	STRCAT(&directoryName[1], ":");
	STRCAT(&directoryName[1], fname);
	STRCPY(fname, &directoryName[1]);
    }
    while (theCPB.dirInfo.ioDrDirID != fsRtDirID);

    STRNCAT(fname, &file.name[1], file.name[0]);
    STRCAT (fname, ":");
    STRCPY(&directoryName[1], fname);
    directoryName[0] = STRLEN(&directoryName[1]);

    /*
     * Find back if the original file
     * is a folder or a file
     */
    theCPB.dirInfo.ioNamePtr = directoryName;
    theCPB.dirInfo.ioVRefNum = fsRtDirID;
    theCPB.dirInfo.ioFDirIndex = 0;  /* Scan for NamePtr in VRefNum */
    theCPB.dirInfo.ioDrDirID = 0;

    error = PBGetCatInfo (&theCPB, false);

    if (error)
	fname[directoryName[0]-1] = 0;
    if ((theCPB.hFileInfo.ioFlAttrib & ioDirMask) == 0)
        fname[directoryName[0]-1] = 0;

}

/*
 * This procedure duplicate a file, it is used in order to keep
 * the footprint of the previous file, when some info can be easily
 * restored with set_perm().
 *
 * Return -1 for failure, 0 for success.
 */
    int
mch_copy_file(from, to)
    char_u *from;
    char_u *to;
{
    char_u  from_str[256];
    char_u  to_str[256];
    char_u  to_name[256];

    HParamBlockRec paramBlock;
    char_u  *char_ptr;
    int	    len;

    /*
     * Convert C string to Pascal string
     */
     char_ptr = from;
     len = 1;
     for (; (*char_ptr != 0) && (len < 255); len++, char_ptr++)
	from_str[len] = *char_ptr;
     from_str[0] = len-1;

     char_ptr = to;
     len = 1;
     for (; (*char_ptr != 0) && (len < 255); len++, char_ptr++)
	to_str[len] = *char_ptr;
     to_str[0] = len-1;

    paramBlock.copyParam.ioCompletion = NULL;
    paramBlock.copyParam.ioNamePtr    = from_str;
 /* paramBlock.copyParam.ioVRefnum    =  overided by ioFilename; */
 /* paramBlock.copyParam.ioDirI       =  overided by ioFilename; */

    paramBlock.copyParam.ioNewName    = to_str;
    paramBlock.copyParam.ioCopyName   = to_name;     /* NIL */
 /* paramBlock.copyParam.ioDstVRefNum = overided by ioNewName;   */
 /* paramBlock.copyParam.ioNewDirID   = overided by ioNewName;   */



    /*
     * First delete the "to" file, this is required on some systems to make
     * the rename() work, on other systems it makes sure that we don't have
     * two files when the rename() fails.
     */
    mch_remove(to);

    /*
     * First try a normal rename, return if it works.
     */
    (void) PBHCopyFile(&paramBlock, false);
    return 0;

}

    int
C2PascalString (CString, PascalString)
    char_u  *CString;
    Str255  *PascalString;
{
    char_u *PascalPtr = (char_u *) PascalString;
    int    len;
    int    i;

    PascalPtr[0] = 0;
    if (CString == NULL)
	return 0;

    len = STRLEN(CString);
    if (len > 255)
	len = 255;

    for (i = 0; i < len; i++)
	PascalPtr[i+1] = CString[i];

    PascalPtr[0] = len;

    return 0;
}

    int
GetFSSpecFromPath (file, fileFSSpec)
    char_u *file;
    FSSpec *fileFSSpec;
{
    /* From FAQ 8-12 */
    Str255      filePascal;
    CInfoPBRec	myCPB;
    OSErr	err;

    (void) C2PascalString (file, &filePascal);

    myCPB.dirInfo.ioNamePtr   = filePascal;
    myCPB.dirInfo.ioVRefNum   = 0;
    myCPB.dirInfo.ioFDirIndex = 0;
    myCPB.dirInfo.ioDrDirID   = 0;

    err= PBGetCatInfo (&myCPB, false);

    /*    vRefNum, dirID, name */
    FSMakeFSSpec (0, 0, filePascal, fileFSSpec);

    /* TODO: Use an error code mechanism */
    return 0;
}

    int
mch_copy_file_attribute(from, to)
    char_u *from;
    char_u *to;
{
    FSSpec  frFSSpec;
    FSSpec  toFSSpec;
    FInfo   fndrInfo;
     Str255	name;
     ResType	type;
     ResType	sink;
     Handle	resource;
     short	idxTypes;
     short    nbTypes;
     short	idxResources;
     short	nbResources;
     short	ID;
    short frRFid;
    short toRFid;
    short attrs_orig;
    short attrs_copy;
    short temp;

    /* TODO: Handle error */
    (void) GetFSSpecFromPath (from, &frFSSpec);
    (void) GetFSSpecFromPath (to  , &toFSSpec);

    /* Copy resource fork */
    temp = 0;

#if 1
     frRFid = FSpOpenResFile (&frFSSpec, fsCurPerm);

     if (frRFid != -1)
     {
	 FSpCreateResFile(&toFSSpec, 'TEXT', '????', nil);
	 toRFid = FSpOpenResFile (&toFSSpec, fsRdWrPerm);

	 UseResFile (frRFid);

	 nbTypes = Count1Types();

	 for (idxTypes = 1; idxTypes <= nbTypes; idxTypes++)
	 {
	   Get1IndType (&type, idxTypes);
	   nbResources = Count1Resources(type);

	   for (idxResources = 1; idxResources <= nbResources; idxResources++)
	   {
	     attrs_orig = 0; /* in case GetRes fails */
	     attrs_copy = 0; /* in case GetRes fails */
	     resource = Get1IndResource(type, idxResources);
	     GetResInfo (resource, &ID, &sink, name);
	     HLock (resource);
	     attrs_orig = GetResAttrs (resource);
	     DetachResource (resource);


	     UseResFile (toRFid);
	     AddResource (resource, type, ID, name);
	     attrs_copy = GetResAttrs (resource);
	     attrs_copy = (attrs_copy & 0x2) | (attrs_orig & 0xFD);
	     SetResAttrs (resource, attrs_copy);
	     WriteResource (resource);
	     UpdateResFile (toRFid);

	     temp = GetResAttrs (resource);

	     /*SetResAttrs (resource, 0);*/
	     HUnlock(resource);
	     ReleaseResource (resource);
	     UseResFile (frRFid);
	     }
	 }
	CloseResFile (toRFid);
	CloseResFile (frRFid);
    }
#endif
    /* Copy Finder Info */
    (void) FSpGetFInfo (&frFSSpec, &fndrInfo);
    (void) FSpSetFInfo (&toFSSpec, &fndrInfo);

    return (temp == attrs_copy);
}

    int
mch_has_resource_fork (file)
    char_u *file;
{
    FSSpec  fileFSSpec;
    short fileRFid;

    /* TODO: Handle error */
    (void) GetFSSpecFromPath (file, &fileFSSpec);
    fileRFid = FSpOpenResFile (&fileFSSpec, fsCurPerm);
    if (fileRFid != -1)
	CloseResFile (fileRFid);

    return (fileRFid != -1);
}

    int
mch_get_shellsize(void)
{
    /* never used */
    return OK;
}

    void
mch_set_shellsize(void)
{
    /* never used */
}

/*
 * Rows and/or Columns has changed.
 */
    void
mch_new_shellsize(void)
{
    /* never used */
}
