/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * amiga.c
 *
 * Amiga system-dependent routines.
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

#include <fcntl.h>

#undef TRUE 			/* will be redefined by exec/types.h */
#undef FALSE

#ifndef LATTICE
# include <exec/types.h>
# include <exec/exec.h>
# include <libraries/dos.h>
# include <libraries/dosextens.h>
# include <intuition/intuition.h>
#else
# include <proto/dos.h>
# include <libraries/dosextens.h>
# include <proto/intuition.h>
# include <proto/exec.h>
#endif

#include <exec/memory.h>

#ifndef NO_ARP
#include <libraries/arpbase.h>		/* for arp.library */
#endif
#include <dos/dostags.h>			/* for 2.0 functions */
#include <dos/dosasl.h>

#if defined(LATTICE) && !defined(SASC) && !defined(NO_ARP)
# include <libraries/arp_pragmas.h>
#endif

/*
 * At this point TRUE and FALSE are defined as 1L and 0L, but we want 1 and 0.
 */
#undef TRUE
#define TRUE (1)
#undef FALSE
#define FALSE (0)

#ifndef AZTEC_C
static long dos_packet __ARGS((struct MsgPort *, long, long));
#endif
static int lock2name __ARGS((BPTR lock, char_u *buf, long	len));
static struct FileInfoBlock *get_fib __ARGS((char_u *));
static int sortcmp __ARGS((char **a, char **b));

static BPTR				raw_in = (BPTR)NULL;
static BPTR				raw_out = (BPTR)NULL;
static int				close_win = FALSE;	/* set if Vim opened the window */

struct IntuitionBase	*IntuitionBase = NULL;
#ifndef NO_ARP
struct ArpBase			*ArpBase = NULL;
#endif

static struct Window	*wb_window;
static char_u			*oldwindowtitle = NULL;

#ifndef NO_ARP
int						dos2 = FALSE;		/* Amiga DOS 2.0x or higher */
#endif
int						size_set = FALSE;	/* set to TRUE if window size was set */

	void
win_resize_on()
{
	OUTSTRN("\033[12{");
}

	void
win_resize_off()
{
	OUTSTRN("\033[12}");
}

	void
mch_write(p, len)
	char_u	*p;
	int		len;
{
	Write(raw_out, (char *)p, (long)len);
}

/*
 * GetChars(): low level input funcion.
 * Get a characters from the keyboard.
 * If time == 0 do not wait for characters.
 * If time == n wait a short time for characters.
 * If time == -1 wait forever for characters.
 *
 * Return number of characters read.
 */
	int
GetChars(buf, maxlen, time)
	char_u	*buf;
	int		maxlen;
	int		time;				/* milli seconds */
{
	int		len;
	long	utime;
	
	if (time >= 0)
	{
		if (time == 0)
			utime = 100L;			/* time = 0 causes problems in DOS 1.2 */
		else
			utime = time * 1000L;	/* convert from milli to micro secs */
		if (WaitForChar(raw_in, utime) == 0)	/* no character available */
			return 0;
	}
	else	/* time == -1 */
	{
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk
	 */
		if (WaitForChar(raw_in, p_ut * 1000L) == 0)
			updatescript(0);
	}

	for (;;)		/* repeat until we got a character */
	{
		len = Read(raw_in, (char *)buf, (long)maxlen);
		if (len > 0)
			return len;
	}
}

/*
 * return non-zero if a character is available
 */
	int
mch_char_avail()
{
	return (WaitForChar(raw_in, 100L) != 0);
}

	void
sleep(n)
	int 			n;
{
#ifndef LATTICE			/* SAS declares void Delay(UNLONG) */
	void			Delay __ARGS((long));
#endif

	if (n > 0)
		Delay((long)(50L * n));
}

	long
mch_avail_mem(special)
	int		special;
{
	return (long)AvailMem(special ? (long)MEMF_CHIP : (long)MEMF_ANY);
}

	void
vim_delay()
{
	Delay(25L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
void
mch_suspend()
{
	OUTSTR("new shell started\n");
	(void)call_shell(NULL, 0, TRUE);
}

#define DOS_LIBRARY     ((UBYTE *) "dos.library")

	void
mch_windinit()
{
	static char		intlibname[] = "intuition.library";

#ifdef AZTEC_C
	Enable_Abort = 0;			/* disallow vim to be aborted */
#endif
	Columns = 80;
	Rows = 24;

	/*
	 * Set input and output channels, unless we have opened our own window
	 */
	if (raw_in == (BPTR)NULL)
	{
		raw_in = Input();
		raw_out = Output();
	}

	flushbuf();

	wb_window = NULL;
	if ((IntuitionBase = (struct IntuitionBase *)OpenLibrary((UBYTE *)intlibname, 0L)) == NULL)
	{
		fprintf(stderr, "cannot open %s!?\n", intlibname);
		mch_windexit(3);
	}
	(void)mch_get_winsize();
}

#include <workbench/startup.h>

/*
 * Check_win checks whether we have an interactive window.
 * If not, a new window is opened with the newcli command.
 * If we would open a window ourselves, the :sh and :! commands would not
 * work properly (Why? probably because we are then running in a background CLI).
 * This also is the best way to assure proper working in a next Workbench release.
 *
 * For the -e option (quickfix mode) and -x we open our own window and disable :sh.
 * Otherwise the compiler would never know when editing is finished.
 */
#define BUF2SIZE 320		/* lenght of buffer for argument with complete path */

	void
check_win(argc, argv)
	int argc;
	char **argv;
{
	int				i;
	BPTR			nilfh, fh;
	char_u			buf1[20];
	char_u			buf2[BUF2SIZE];
	static char_u	*(constrings[3]) = {(char_u *)"con:0/0/662/210/",
									  (char_u *)"con:0/0/640/200/",
									  (char_u *)"con:0/0/320/200/"};
	static char_u	winerr[] = "VIM: Can't open window!\n";
	struct WBArg	*argp;
	int				ac;
	char			*av;
	char_u			*device = NULL;
	int				exitval = 4;
	struct Library	*DosBase;
	int				usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
    if (DosBase = OpenLibrary(DOS_LIBRARY, 37L))
    {
		CloseLibrary(DosBase);
#ifndef NO_ARP
		dos2 = TRUE;
#endif
    }
	else			/* without arp functions we NEED 2.0 */
	{
#ifdef NO_ARP
		fprintf(stderr, "Need Amigados version 2.04 or later\n");
		exit(3);
#else
					/* need arp functions for dos 1.x */
		if (!(ArpBase = (struct ArpBase *) OpenLibrary((UBYTE *)ArpName, ArpVersion)))
		{
			fprintf(stderr, "Need %s version %ld\n", ArpName, ArpVersion);
			exit(3);
		}
#endif
	}

/*
 * scan argv[] for the '-e', '-x' and '-d' arguments
 */
	for (i = 1; i < argc; ++i)
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'e':
			case 'x':
				usewin = TRUE;
				break;

			case 'd':
				if (i < argc - 1)
					device = (char_u *)argv[i + 1];
				break;
			}
		}

/*
 * If we were not started from workbench, do not have a '-d' argument and
 * we have been started with an interactive window, use that window.
 */
	if (argc != 0 && device == NULL &&
				IsInteractive(Input()) && IsInteractive(Output()))
		return;

/*
 * If we are in quickfix mode, we open our own window. We can't use the
 * newcli trick below, because the compiler would not know when we are finished.
 * We do the same with the '-x' option, for mail, rn, etc.
 */
	if (usewin)
	{
		/*
		 * Try to open a window. First try the specified device.
		 * Then try a 24 line 80 column window.
		 * If that fails, try two smaller ones.
		 */
		for (i = -1; i < 3; ++i)
		{
			if (i >= 0)
				device = constrings[i];
			if (device && (raw_in = Open((UBYTE *)device, (long)MODE_NEWFILE)) != (BPTR)NULL)
				break;
		}
		if (raw_in == (BPTR)NULL)		/* all three failed */
		{
			fprintf(stderr, (char *)winerr);
			goto exit;
		}
		raw_out = raw_in;
		close_win = TRUE;
		return;
	}

	if ((nilfh = Open((UBYTE *)"NIL:", (long)MODE_NEWFILE)) == (BPTR)NULL)
	{
		fprintf(stderr, "Cannot open NIL:\n");
		goto exit;
	}

	/*
	 * make a unique name for the temp file (which we will not delete!)
	 */
	sprintf((char *)buf1, "t:nc%ld", (char *)buf1);	/* nobody else is using our stack */
	if ((fh = Open((UBYTE *)buf1, (long)MODE_NEWFILE)) == (BPTR)NULL)
	{
		fprintf(stderr, "Cannot create %s\n", (char *)buf1);
		goto exit;
	}
	/*
	 * Write the command into the file, put quotes around the arguments that
	 * have a space in them.
	 */
	if (argc == 0)		/* run from workbench */
		ac = ((struct WBStartup *)argv)->sm_NumArgs;
	else
		ac = argc;
	for (i = 0; i < ac; ++i)
	{
		if (argc == 0)
		{
			*buf2 = NUL;
			argp = &(((struct WBStartup *)argv)->sm_ArgList[i]);
			if (argp->wa_Lock)
				(void)lock2name(argp->wa_Lock, buf2, (long)(BUF2SIZE - 1));
#ifndef NO_ARP
			if (dos2)		/* use 2.0 function */
#endif
				AddPart((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (long)(BUF2SIZE - 1));
#ifndef NO_ARP
			else			/* use arp function */
				TackOn((char *)buf2, argp->wa_Name);
#endif
			av = (char *)buf2;
		}
		else
			av = argv[i];

		if (av[0] == '-' && av[1] == 'd')		/* skip '-d' option */
		{
			++i;
			continue;
		}
		if (strchr(av, ' '))
			Write(fh, "\"", 1L);
		Write(fh, av, (long)strlen(av));
		if (strchr(av, ' '))
			Write(fh, "\"", 1L);
		Write(fh, " ", 1L);
	}
	Write(fh, "\nendcli\n", 8L);
	Close(fh);

/*
 * Try to open a new cli in a window. If '-d' argument was given try to open
 * the specified device. Then try a 24 line 80 column window.
 * If that fails, try two smaller ones.
 */
	for (i = -1; i < 3; ++i)
	{
		if (i >= 0)
			device = constrings[i];
		else if (device == NULL)
			continue;
		sprintf((char *)buf2, "newcli <nil: >nil: %s from %s", (char *)device, (char *)buf1);
#ifndef NO_ARP
		if (dos2)
		{
#endif
			if (!SystemTags((UBYTE *)buf2, SYS_UserShell, TRUE, TAG_DONE))
				break;
#ifndef NO_ARP
		}
		else
		{
			if (Execute((UBYTE *)buf2, nilfh, nilfh))
				break;
		}
#endif
	}
	if (i == 3)		/* all three failed */
	{
		DeleteFile((UBYTE *)buf1);
		fprintf(stderr, (char *)winerr);
		goto exit;
	}
	exitval = 0;	/* The Execute succeeded: exit this program */

exit:
#ifndef NO_ARP
	if (ArpBase)
		CloseLibrary((struct Library *) ArpBase);
#endif
	exit(exitval);
}

/*
 * fname_case(): Set the case of the filename, if it already exists.
 *				 This will cause the filename to remain exactly the same.
 */
	void
fname_case(name)
	char_u	*name;
{
	register struct FileInfoBlock	*fib;
	register size_t					len;

	fib = get_fib(name);
	if (fib != NULL)
	{
		len = STRLEN(name);
		if (len == strlen(fib->fib_FileName))	/* safety check */
			memmove((char *)name, fib->fib_FileName, len);
		free(fib);
	}
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
	static struct FileInfoBlock *
get_fib(fname)
	char_u *fname;
{
	register BPTR					flock;
	register struct FileInfoBlock	*fib;

	if (fname == NULL)		/* safety check */
		return NULL;
	fib = (struct FileInfoBlock *)malloc(sizeof(struct FileInfoBlock));
	if (fib != NULL)
	{
		flock = Lock((UBYTE *)fname, (long)ACCESS_READ);
		if (flock == (BPTR)NULL || !Examine(flock, fib))
		{
			free(fib);	/* in case of an error the memory is freed here */
			fib = NULL;
		}
		if (flock)
			UnLock(flock);
	}
	return fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
	void
mch_settitle(title, icon)
	char_u	*title;
	char_u	*icon;
{
	if (wb_window != NULL && title != NULL)
		SetWindowTitles(wb_window, (UBYTE *)title, (UBYTE *)-1L);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *	1  Just restore title
 *  2  Just restore icon (which we don't have)
 *	3  Restore title and icon (which we don't have)
 */
	void
mch_restore_title(which)
	int which;
{
	mch_settitle((which & 1) ? oldwindowtitle : NULL, NULL);
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
	int
vim_dirname(buf, len)
	char_u		*buf;
	int			len;
{
	return FullName((char_u *)"", buf, len);
}

/*
 * get absolute filename into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
	int
FullName(fname, buf, len)
	char_u		*fname, *buf;
	int			len;
{
	BPTR		l;
	int			retval = FAIL;
	int			i;

	*buf = NUL;
	if (fname == NULL)	/* always fail */
		return FAIL;

	if ((l = Lock((UBYTE *)fname, (long)ACCESS_READ)) != (BPTR)0)/* lock the file */
	{
		retval = lock2name(l, buf, (long)len);
		UnLock(l);
	}
	else if (!isFullName(fname))		/* not a full path yet */
	{
		/*
		 * if cannot lock the file, try to lock the current directory and then
		 * concatenate the file name
		 */
		if ((l = Lock((UBYTE *)"", (long)ACCESS_READ)) != (BPTR)NULL) /* lock current dir */
		{
			retval = lock2name(l, buf, (long)len);
			UnLock(l);
			if (retval == OK)
			{
				i = STRLEN(buf);
				if (i < len - 1 && (i == 0 || buf[i - 1] != ':'))
					buf[i++] = '/';
				STRNCPY(buf + i, fname, (size_t)(len - i));	/* concatenate the fname */
			}
		}
	}
	if (retval == FAIL || *buf == 0 || *buf == ':')
		STRCPY(buf, fname);	/* something failed; use the filename */
	return retval;
}

/*
 * return TRUE is fname is an absolute path name
 */
	int
isFullName(fname)
	char_u		*fname;
{
	return (STRCHR(fname, ':') != NULL);
}

/*
 * Get the full filename from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
	static int
lock2name(lock, buf, len)
	BPTR	lock;
	char_u	*buf;
	long	len;
{
#ifndef NO_ARP
	if (dos2)				/* use 2.0 function */
#endif
		return ((int)NameFromLock(lock, (UBYTE *)buf, len) ? OK : FAIL);
#ifndef NO_ARP
	else				/* use arp function */
		return ((int)PathName(lock, (char *)buf, (long)(len/32)) ? OK : FAIL);
#endif
}

/*
 * get file permissions for 'name'
 */
	long
getperm(name)
	char_u		*name;
{
	struct FileInfoBlock	*fib;
	long 					retval = -1;

	fib = get_fib(name);
	if (fib != NULL)
	{
		retval = fib->fib_Protection;
		free(fib);
	}
	return retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
	int
setperm(name, perm)
	char_u		*name;
	long		perm;
{
	perm &= ~FIBF_ARCHIVE;				/* reset archived bit */
	return (SetProtection((UBYTE *)name, (long)perm) ? OK : FAIL);
}

/*
 * return FALSE if "name" is not a directory
 * return TRUE if "name" is a directory.
 * return -1 for error.
 */
	int
isdir(name)
	char_u		*name;
{
	struct FileInfoBlock	*fib;
	int 					retval = -1;

	fib = get_fib(name);
	if (fib != NULL)
	{
		retval = ((fib->fib_DirEntryType >= 0) ? TRUE : FALSE);
		free(fib);
	}
	return retval;
}

/*
 * Careful: mch_windexit() may be called before mch_windinit()!
 */
	void
mch_windexit(r)
	int 			r;
{
	if (raw_in)						/* put terminal in 'normal' mode */
	{
		settmode(0);
		stoptermcap();
	}
	if (raw_out)
	{
		if (term_console)
		{
			win_resize_off();		/* window resize events de-activated */
			if (size_set)
				OUTSTR("\233t\233u");		/* reset window size (CSI t CSI u) */
		}
		flushbuf();
	}

	mch_restore_title(3);			/* restore window title */

	ml_close_all(); 				/* remove all memfiles */

#ifndef NO_ARP
	if (ArpBase)
		CloseLibrary((struct Library *) ArpBase);
#endif
	if (close_win)
		Close(raw_in);
	if (r)
		printf("Vim exiting with %d\n", r);	/* somehow this makes :cq work!? */
	exit(r);
}

/*
 * This is a routine for setting a given stream to raw or cooked mode on the
 * Amiga . This is useful when you are using Lattice C to produce programs
 * that want to read single characters with the "getch()" or "fgetc" call.
 *
 * Written : 18-Jun-87 By Chuck McManis.
 */

#define MP(xx)	((struct MsgPort *)((struct FileHandle *) (BADDR(xx)))->fh_Type)

/*
 * Function mch_settmode() - Convert the specified file pointer to 'raw' or 'cooked'
 * mode. This only works on TTY's.
 *
 * Raw: keeps DOS from translating keys for you, also (BIG WIN) it means
 *		getch() will return immediately rather than wait for a return. You
 *		lose editing features though.
 *
 * Cooked: This function returns the designate file pointer to it's normal,
 *		wait for a <CR> mode. This is exactly like raw() except that
 *		it sends a 0 to the console to make it back into a CON: from a RAW:
 */
	void
mch_settmode(raw)
	int			raw;
{
	if (dos_packet(MP(raw_in), (long)ACTION_SCREEN_MODE, raw ? -1L : 0L) == 0)
		fprintf(stderr, "cannot change console mode ?!\n");
}

/*
 * set screen mode, always fails.
 */
	int
mch_screenmode(arg)
	char_u	 *arg;
{
	EMSG("Screen mode setting not supported");
	return FAIL;
}

/*
 * Code for this routine came from the following :
 *
 * ConPackets.c -  C. Scheppner, A. Finkel, P. Lindsay	CBM
 *	 DOS packet example
 *	 Requires 1.2
 *
 * Found on Fish Disk 56.
 *
 * Heavely modified by mool.
 */

#include <devices/conunit.h>

/*
 * try to get the real window size
 * return FAIL for failure, OK otherwise
 */
	int
mch_get_winsize()
{
	struct ConUnit	*conUnit;
 	char			id_a[sizeof(struct InfoData) + 3];
	struct InfoData *id;

	if (!term_console)	/* not an amiga window */
		return FAIL;

	/* insure longword alignment */
 	id = (struct InfoData *)(((long)id_a + 3L) & ~3L);

	/*
	 * Should make console aware of real window size, not the one we set.
	 * Unfortunately, under DOS 2.0x this redraws the window and it
	 * is rarely needed, so we skip it now, unless we changed the size.
	 */
	if (size_set)
		OUTSTR("\233t\233u");	/* CSI t CSI u */
	flushbuf();

	if (dos_packet(MP(raw_out), (long)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0 ||
				(wb_window = (struct Window *)id->id_VolumeNode) == NULL)
	{
		/* it's not an amiga window, maybe aux device */
		/* terminal type should be set */
		term_console = FALSE;
		return FAIL;
	}
	if (oldwindowtitle == NULL)
		oldwindowtitle = (char_u *)wb_window->Title;
	if (id->id_InUse == (BPTR)NULL)
	{
		fprintf(stderr, "mch_get_winsize: not a console??\n");
		return FAIL;
	}
	conUnit = (struct ConUnit *) ((struct IOStdReq *) id->id_InUse)->io_Unit;

	/* get window size */
	Rows = conUnit->cu_YMax + 1;
	Columns = conUnit->cu_XMax + 1;
	if (Rows < 0 || Rows > 200) 	/* cannot be an amiga window */
	{
		Columns = 80;
		Rows = 24;
		term_console = FALSE;
		return FAIL;
	}
	check_winsize();

	return OK;
}

/*
 * try to set the real window size
 */
	void
mch_set_winsize()
{
	if (term_console)
	{
		size_set = TRUE;
		outchar(CSI);
		outnum((long)Rows);
		outchar('t');
		outchar(CSI);
		outnum((long)Columns);
		outchar('u');
		flushbuf();
	}
}

#ifdef SETKEYMAP
/*
 * load and activate a new keymap for our CLI - DOES NOT WORK -
 * The problem is that after the setting of the keymap the input blocks
 * But the new keymap works allright in another window.
 * Tried but no improvement:
 * - remembering the length, data and command fields in request->io_xxx
 * - settmode(0) first, settmode(1) afterwards
 * - putting the keymap directly in conunit structure
 */

#include <devices/keymap.h>

	void
set_keymap(name)
	char_u *name;
{
 	char					id_a[sizeof(struct InfoData) + 3];
	struct InfoData			*id;
	static struct KeyMap	*old;
	static BPTR				segment = (BPTR)NULL;
	struct IOStdReq			*request;
	int						c;

	if (!term_console)
		return;

	/* insure longword alignment */
 	id = (struct InfoData *)(((long)id_a + 3L) & ~3L);

	if (dos_packet(MP(raw_out), (long)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0)
	{
		EMSG("dos_packet failed");
		return;
	}
	if (id->id_InUse == (BPTR)NULL)
	{
		EMSG("not a console??");
		return;
	}
	request = (struct IOStdReq *) id->id_InUse;

	if (segment != (BPTR)NULL)	/* restore old keymap */
	{
		request->io_Command = CD_SETKEYMAP;
		request->io_Length = sizeof(struct KeyMap);
		request->io_Data = (APTR)old;
		DoIO((struct IORequest *)request);
		if (request->io_Error)
			EMSG("Cannot reset keymap");
		else				/* no error, free the allocated memory */
		{
			UnLoadSeg(segment);
			FreeMem(old, sizeof(struct KeyMap));
			segment = (BPTR)NULL;
		}
	}
	if (name != NULL)
	{
		segment = LoadSeg(name);
		if (segment == (BPTR)NULL)
		{
			EMSG("Cannot open keymap file");
			return;
		}
		old = (struct KeyMap *)AllocMem(sizeof(struct KeyMap), MEMF_PUBLIC);
		if (old == NULL)
		{
			EMSG(e_outofmem);
			UnLoadSeg(segment);
			segment = (BPTR)NULL;
		}
		else
		{
			request->io_Command = CD_ASKKEYMAP;
			request->io_Length = sizeof(struct KeyMap);
			request->io_Data = (APTR)old;
			DoIO((struct IORequest *)request);
			if (request->io_Error)
			{
				EMSG("Cannot get old keymap");
				UnLoadSeg(segment);
				segment = (BPTR)NULL;
				FreeMem(old, sizeof(struct KeyMap));
			}
			else
			{
				request->io_Command = CD_SETKEYMAP;
				request->io_Length = sizeof(struct KeyMap);
				request->io_Data = (APTR)((segment << 2) + 18);
				DoIO((struct IORequest *)request);
				if (request->io_Error)
					EMSG("Cannot set keymap");

				/* test for blocking */
				request->io_Command = CMD_READ;
				request->io_Length = 1;
				request->io_Data = (APTR)&c;
				DoIO((struct IORequest *)request);	/* BLOCK HERE! */
				if (request->io_Error)
					EMSG("Cannot set keymap");
			}
		}
	}
}
#endif

#ifndef AZTEC_C
/*
 * Sendpacket.c
 *
 * An invaluable addition to your Amiga.lib file. This code sends a packet to
 * the given message port. This makes working around DOS lots easier.
 *
 * Note, I didn't write this, those wonderful folks at CBM did. I do suggest
 * however that you may wish to add it to Amiga.Lib, to do so, compile it and
 * say 'oml lib:amiga.lib -r sendpacket.o'
 */

/* #include <proto/exec.h> */
/* #include <proto/dos.h> */
#include <exec/memory.h>

/*
 * Function - dos_packet written by Phil Lindsay, Carolyn Scheppner, and Andy
 * Finkel. This function will send a packet of the given type to the Message
 * Port supplied.
 */

	static long
dos_packet(pid, action, arg)
	struct MsgPort *pid;	/* process indentifier ... (handlers message port) */
	long			action, /* packet type ... (what you want handler to do)   */
					arg;	/* single argument */
{
# ifndef NO_ARP
	struct MsgPort			*replyport;
	struct StandardPacket	*packet;
	long					res1;

	if (dos2)
# endif
		return DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	/* use 2.0 function */
# ifndef NO_ARP

	replyport = (struct MsgPort *) CreatePort(NULL, 0);	/* use arp function */
	if (!replyport)
		return (0);

	/* Allocate space for a packet, make it public and clear it */
	packet = (struct StandardPacket *)
		AllocMem((long) sizeof(struct StandardPacket), MEMF_PUBLIC | MEMF_CLEAR);
	if (!packet) {
		DeletePort(replyport);
		return (0);
	}
	packet->sp_Msg.mn_Node.ln_Name = (char *) &(packet->sp_Pkt);
	packet->sp_Pkt.dp_Link = &(packet->sp_Msg);
	packet->sp_Pkt.dp_Port = replyport;
	packet->sp_Pkt.dp_Type = action;
	packet->sp_Pkt.dp_Arg1 = arg;

	PutMsg(pid, (struct Message *)packet);		/* send packet */

	WaitPort(replyport);
	GetMsg(replyport);

	res1 = packet->sp_Pkt.dp_Res1;

	FreeMem(packet, (long) sizeof(struct StandardPacket));
	DeletePort(replyport);

	return (res1);
# endif
}
#endif

/*
 * call shell, return FAIL for failure, OK otherwise
 */
	int
call_shell(cmd, filter, cooked)
	char_u	*cmd;
	int		filter;		/* if != 0: called by dofilter() */
	int		cooked;
{
	BPTR	mydir;
	int		x;
#ifndef LATTICE
	int		use_execute;
	char_u	*shellcmd = NULL;
	char_u	*shellarg;
#endif
	int	retval = OK;

	if (close_win)
	{
		/* if Vim opened a window: Executing a shell may cause crashes */
		EMSG("Cannot execute shell with -e or -x option");
		return FAIL;
	}

	if (term_console)
		win_resize_off(); 			/* window resize events de-activated */
	flushbuf();

	if (cooked)
		settmode(0); 				/* set to cooked mode */
	mydir = Lock((UBYTE *)"", (long)ACCESS_READ);	/* remember current directory */

#ifdef LATTICE						/* not tested very much */
	if (cmd == NULL)
	{
# ifndef NO_ARP
		if (dos2)
# endif
			x = SystemTags(p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifndef NO_ARP
		else
			x = Execute(p_sh, raw_in, raw_out);
# endif
	}
	else
	{
# ifndef NO_ARP
		if (dos2)
# endif
			x = SystemTags((char *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifndef NO_ARP
		else
			x = Execute((char *)cmd, 0L, raw_out);
# endif
	}
# ifdef NO_ARP
	if (x < 0)
# else
	if ((dos2 && x < 0) || (!dos2 && !x))
# endif
	{
		outstr((char_u *)"Cannot execute ");
		if (cmd == NULL)
		{
			outstr((char_u *)"shell ");
			outstr(p_sh);
		}
		else
			outstr(cmd);
		outchar('\n');
		retval = FAIL;
	}
# ifdef NO_ARP
	else if (x)
# else
	else if (!dos2 || x)
# endif
	{
		if (x = IoErr())
		{
#ifdef WEBB_COMPLETE
			if (!expand_interactively)
#endif
			{
				outchar('\n');
				outnum(x);
				outstr((char_u *)" returned\n");
			}
			retval = FAIL;
		}
	}
#else	/* !LATTICE */
	if (p_st >= 4 || (p_st >= 2 && !filter))
		use_execute = 1;
	else
		use_execute = 0;
	if (!use_execute)
	{
		/*
		 * separate shell name from argument
		 */
		shellcmd = strsave(p_sh);
		if (shellcmd == NULL)		/* out of memory, use Execute */
			use_execute = 1;
		else
		{
			shellarg = shellcmd;
			skiptospace(&shellarg);	/* find start of arguments */
			if (*shellarg != NUL)
			{
				*shellarg++ = NUL;
				skipspace(&shellarg);
			}
		}
	}
	if (cmd == NULL)
	{
		if (use_execute)
		{
# ifndef NO_ARP
			if (dos2)
# endif
				x = SystemTags((UBYTE *)p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifndef NO_ARP
			else
				x = !Execute((UBYTE *)p_sh, raw_in, raw_out);
# endif
		}
		else
			x = fexecl((char *)shellcmd, (char *)shellcmd, (char *)shellarg, NULL);
	}
	else if (use_execute)
	{
# ifndef NO_ARP
		if (dos2)
# endif
			x = SystemTags((UBYTE *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifndef NO_ARP
		else
			x = !Execute((UBYTE *)cmd, 0L, raw_out);
# endif
	}
	else if (p_st & 1)
		x = fexecl((char *)shellcmd, (char *)shellcmd, (char *)shellarg, (char *)cmd, NULL);
	else
		x = fexecl((char *)shellcmd, (char *)shellcmd, (char *)shellarg, "-c", (char *)cmd, NULL);
# ifdef NO_ARP
	if (x < 0)
# else
	if ((dos2 && x < 0) || (!dos2 && x))
# endif
	{
		outstr((char_u *)"Cannot execute ");
		if (use_execute)
		{
			if (cmd == NULL)
				outstr(p_sh);
			else
				outstr(cmd);
		}
		else
		{
			outstr((char_u *)"shell ");
			outstr(shellcmd);
		}
		outchar('\n');
		retval = FAIL;
	}
	else
	{
		if (use_execute)
		{
# ifdef NO_ARP
			if (x)
# else
			if (!dos2 || x)
# endif
				x = IoErr();
		}
		else
			x = wait();
		if (x)
		{
#ifdef WEBB_COMPLETE
			if (!expand_interactively)
#endif
			{
				outchar('\n');
				outnum((long)x);
				outstrn((char_u *)" returned\n");
			}
			retval = FAIL;
		}
	}
	free(shellcmd);
#endif	/* !LATTICE */

	if (mydir = CurrentDir(mydir))		/* make sure we stay in the same directory */
		UnLock(mydir);
	if (cooked)
		settmode(1); 					/* set to raw mode */
	resettitle();
	if (term_console)
		win_resize_on(); 				/* window resize events activated */
	return retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid trouble
 * with lattice-c programs.
 */
	void
breakcheck()
{
   if (SetSignal(0L, (long)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
		got_int = TRUE;
}

/* this routine causes manx to use this Chk_Abort() rather than it's own */
/* otherwise it resets our ^C when doing any I/O (even when Enable_Abort */
/* is zero).  Since we want to check for our own ^C's                    */

#ifdef _DCC
#define Chk_Abort chkabort
#endif

	long
Chk_Abort()
{
	return(0L);
}

/*
 * ExpandWildCard() - this code does wild-card pattern matching using the arp
 *					  routines. This is based on WildDemo2.c (found in arp1.1
 *					  distribution). That code's copyright follows :
 *-------------------------------------------------------------------------
 * WildDemo2.c - Search filesystem for patterns, and separate into directories
 *		 and files, sorting each separately using DA lists.
 *
 * -+=SDB=+-
 *
 * Copyright (c) 1987, Scott Ballantyne
 * Use and abuse as you please.
 *
 * num_pat is number of input patterns
 * pat is array of pointers to input patterns
 * num_file is pointer to number of matched file names
 * file is pointer to array of pointers to matched file names
 * if file_only is TRUE we match only files, no dirs
 * if list_notfound is TRUE we include not-found entries (probably locked)
 * return OK for success, FAIL for error (you may loose some memory)
 *-------------------------------------------------------------------------
 */

/* #include <arpfunctions.h> */
extern void *malloc __ARGS((size_t)), *calloc __ARGS((size_t, size_t));
static int insfile __ARGS((char_u *, int));
static void freefiles __ARGS((void));

#define ANCHOR_BUF_SIZE (512)
#define ANCHOR_SIZE (sizeof(struct AnchorPath) + ANCHOR_BUF_SIZE)

/*
 * we use this structure to built a list of file names
 */
struct onefile
{
	struct onefile	*next;
	char_u			name[1];	/* really longer */
} *namelist = NULL;

/*
 * insert one file into the list of file names
 * return FAIL for failure
 * return OK for success
 */
	static int
insfile(name, isdir)
	char_u	*name;
	int		isdir;
{
	struct onefile *new;

	new = (struct onefile *)alloc((unsigned)(sizeof(struct onefile) + STRLEN(name) + isdir));
	if (new == NULL)
		return FAIL;
	STRCPY(&(new->name[0]), name);
	if (isdir)
		STRCAT(&(new->name[0]), "/");
	new->next = namelist;
	namelist = new;
	return OK;
}

/*
 * free a whole list of file names
 */
	static void
freefiles()
{
	struct onefile *p;

	while (namelist)
	{
		p = namelist->next;
		free(namelist);
		namelist = p;
	}
}

	static int
sortcmp(a, b)
	char **a, **b;
{
	return strcmp(*a, *b);
}

	int
ExpandWildCards(num_pat, pat, num_file, file, files_only, list_notfound)
	int 			num_pat;
	char_u		  **pat;
	int 		   *num_file;
	char_u		 ***file;
	int			files_only;
	int			list_notfound;
{
	int 					i;
	struct AnchorPath		*Anchor;
	int						domatchend = FALSE;
	LONG					Result;
	struct onefile			*p;
	char_u					*errmsg = NULL;
	char_u					*starbuf, *sp, *dp;
	int						foundone;

	*num_file = 0;
	*file = (char_u **)"";

	/* Get our AnchorBase */
	Anchor = (struct AnchorPath *) calloc((size_t)1, (size_t)ANCHOR_SIZE);
	if (!Anchor)
		goto OUT_OF_MEMORY;
	Anchor->ap_StrLen = ANCHOR_BUF_SIZE;	/* ap_Length not supported anymore */
#ifdef APF_DODOT
	Anchor->ap_Flags = APF_DODOT | APF_DOWILD;		/* allow '.' for current dir */
#else
	Anchor->ap_Flags = APF_DoDot | APF_DoWild;		/* allow '.' for current dir */
#endif

	for (i = 0; i < num_pat; i++)
	{
#ifndef NO_ARP
		if (dos2)
		{
#endif
				/* hack to replace '*' by '#?' */
			starbuf = alloc((unsigned)(2 * STRLEN(pat[i]) + 1));	/* maximum required */
			if (starbuf == NULL)
				goto OUT_OF_MEMORY;
			for (sp = pat[i], dp = starbuf; *sp; ++sp)
			{
				if (*sp == '*')
				{
					*dp++ = '#';
					*dp++ = '?';
				}
				else
					*dp++ = *sp;
			}
			*dp = NUL;
			Result = MatchFirst((UBYTE *)starbuf, Anchor);
			free(starbuf);
#ifndef NO_ARP
		}
		else
			Result = FindFirst((char *)pat[i], Anchor);
#endif
		domatchend = TRUE;
		foundone = FALSE;
		while (Result == 0)
		{
			if (!files_only || Anchor->ap_Info.fib_DirEntryType < 0)
			{
				(*num_file)++;
				if (insfile((char_u *)Anchor->ap_Buf, Anchor->ap_Info.fib_DirEntryType >= 0) == FAIL)
				{
OUT_OF_MEMORY:
					errmsg = (char_u *)"Out of memory";
					goto Return;
				}
				foundone = TRUE;
			}
#ifndef NO_ARP
			if (dos2)
#endif
				Result = MatchNext(Anchor);
#ifndef NO_ARP
			else
				Result = FindNext(Anchor);
#endif
		}
		if (Result == ERROR_BUFFER_OVERFLOW)
		{
			errmsg = (char_u *)"ANCHOR_BUF_SIZE too small.";
			goto Return;
		}
		if (!foundone)
		{
			if (list_notfound)	/* put object with error in list */
			{
				(*num_file)++;
				if (insfile(pat[i], FALSE) == FAIL)
					goto OUT_OF_MEMORY;
			}
			else if (Result != ERROR_OBJECT_NOT_FOUND && Result != ERROR_NO_MORE_ENTRIES)
			{
				errmsg = (char_u *)"I/O ERROR";
				goto Return;
			}
		}
#ifndef NO_ARP
		if (dos2)
#endif
			MatchEnd(Anchor);
#ifndef NO_ARP
		else
			FreeAnchorChain(Anchor);
#endif
		domatchend = FALSE;
	}

	p = namelist;
	if (p)
	{
		*file = (char_u **) malloc(sizeof(char_u *) * (*num_file));
		if (*file == NULL)
			goto OUT_OF_MEMORY;
		for (i = *num_file - 1; p; p = p->next, --i)
		{
			(*file)[i] = (char_u *) malloc(STRLEN(p->name) + 1);
			if ((*file)[i] == NULL)
				goto OUT_OF_MEMORY;
			STRCPY((*file)[i], p->name);
		}
		qsort((void *)*file, (size_t)*num_file, sizeof(char_u *), sortcmp);
	}
Return:
	if (domatchend)
	{
#ifndef NO_ARP
		if (dos2)
#endif
			MatchEnd(Anchor);
#ifndef NO_ARP
		else
			FreeAnchorChain(Anchor);
#endif
	}
	if (Anchor)
		free(Anchor);
	freefiles();
	if (errmsg)
	{
		emsg(errmsg);
		*num_file = 0;
		return FAIL;
	}
	return OK;
}

	void
FreeWild(num, file)
	int		num;
	char_u	**file;
{
	if (file == NULL || num == 0)
		return;
	while (num--)
		free(file[num]);
	free(file);
}

	int
has_wildcard(p)
	char_u *p;
{
	for ( ; *p; ++p)
		if (strchr("*?[(~#", *p) != NULL)
			return TRUE;
	return FALSE;
}

/*
 * With 2.0 support for reading local environment variables
 * Careful: uses IObuff!
 */

	char_u *
vimgetenv(var)
	char_u *var;
{
	int len;

#ifndef NO_ARP
	if (!dos2)
		return (char_u *)getenv((char *)var);
#endif

	len = GetVar((UBYTE *)var, (UBYTE *)IObuff, (long)(IOSIZE - 1), (long)0);

	if (len == -1)
		return NULL;
	else
		return IObuff;
}
