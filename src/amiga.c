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
#  include <exec/types.h>
#  include <exec/exec.h>
#  include <libraries/dos.h>
#  include <libraries/dosextens.h>
#  include <intuition/intuition.h>
#else
#  include <proto/dos.h>
#  include <libraries/dosextens.h>
#  include <proto/intuition.h>
#  include <proto/exec.h>
#endif

#include <libraries/arpbase.h>
#if defined(LATTICE) && !defined(SASC)
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
static int lock2name __ARGS((BPTR lock, char *buf, long	len));
static struct FileInfoBlock *get_fib __ARGS((char *));

static BPTR				raw_in = (BPTR)NULL;
static BPTR				raw_out = (BPTR)NULL;
static int				close_win = FALSE;	/* set if Vim opened the window */

struct IntuitionBase	*IntuitionBase = NULL;
struct ArpBase			*ArpBase = NULL;

static struct Window	*wb_window;
static char				*oldwindowtitle = NULL;
static int				quickfix = FALSE;

int						dos2 = FALSE;		/* Amiga DOS 2.0x or higher */
int						size_set = FALSE;	/* set to TRUE if window size was set */

static char				win_resize_on[]  = "\033[12{";
static char				win_resize_off[] = "\033[12}";

/*
 * the number of calls to Write is reduced by using the buffer "outbuf"
 */
#define BSIZE	2048
static u_char			outbuf[BSIZE];
static int				bpos = 0;		/* number of chars in outbuf */

/*
 * flushbuf(): flush the output buffer
 */
	void
flushbuf()
{
	if (bpos != 0)
	{
		Write(raw_out, (char *)outbuf, (long)bpos);
		bpos = 0;
	}
}

/*
 * outchar(c): put a character into the output buffer.
 *			   Flush it if it becomes full.
 */
	void
outchar(c)
	unsigned	c;
{
	outbuf[bpos] = c;
	++bpos;
	if (bpos >= BSIZE)
		flushbuf();
}

/*
 * GetChars(): low level input funcion.
 * Get a characters from the keyboard.
 * If type == T_PEEK do not wait for characters.
 * If type == T_WAIT wait a short time for characters.
 * If type == T_BLOCK wait for characters.
 */
	int
GetChars(buf, maxlen, type)
	char	*buf;
	int		maxlen;
	int		type;
{
	int		len;
	long	time = 1000000L;	/* one second */

	switch (type)
	{
	case T_PEEK:
		time = 100L;
	case T_WAIT:
		if (WaitForChar(raw_in, time) == 0)	/* no character available */
			return 0;
		break;

	case T_BLOCK:
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk
	 */
		if (WaitForChar(raw_in, p_ut * 1000L) == 0)
			updatescript(0);
	}

	for (;;)		/* repeat until we got a character */
	{
		len = Read(raw_in, buf, (long)maxlen);
		if (len > 0)
			return len;
	}
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
	outstr("new shell started\n");
	call_shell(NULL, 0);
}

#define DOS_LIBRARY     ((UBYTE *) "dos.library")

	void
mch_windinit()
{
	static char		intlibname[] = "intuition.library";
	struct Library	*DosBase;

#ifdef AZTEC_C
	Enable_Abort = 0;			/* disallow vim to be aborted */
#endif
	Columns = 80;
	Rows = 24;

/*
 * check if we are running under DOS 2.0x or higher
 */
    if (DosBase = OpenLibrary(DOS_LIBRARY, 37L))
    {
		CloseLibrary(DosBase);
		dos2 = TRUE;
    }

	/*
	 * Set input and output channels, unless we have opened our own window
	 */
	if (raw_in == (BPTR)NULL)
	{
		raw_in = Input();
		raw_out = Output();
	}

	if (term_console)
		outstr(win_resize_on); 			/* window resize events activated */
	flushbuf();

	wb_window = NULL;
	if ((IntuitionBase = (struct IntuitionBase *)OpenLibrary((UBYTE *)intlibname, 0L)) == NULL)
	{
		fprintf(stderr, "cannot open %s!?\n", intlibname);
		mch_windexit(3);
	}
	mch_get_winsize();
}

#include <workbench/startup.h>

/*
 * Check_win checks whether we have an interactive window.
 * If not, a new window is opened with the newcli command.
 * If we would open a window ourselves, the :sh and :! commands would not
 * work properly (Why? probably because we are then running in a background CLI).
 * This also is the best way to assure proper working in a next Workbench release.
 *
 * For the -e option (quickfix mode) we open our own window and disable :sh.
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
	char			buf1[20];
	char			buf2[BUF2SIZE];
	static char		*(constrings[3]) = {"con:0/0/662/210/",
									  "con:0/0/640/200/",
									  "con:0/0/320/200/"};
	static char		winerr[] = "VIM: Can't open window!\n";
	struct WBArg	*argp;
	int				ac;
	char			*av;
	char			*device = NULL;
	int				exitval = 4;

	if (!(ArpBase = (struct ArpBase *) OpenLibrary((UBYTE *)ArpName, ArpVersion)))
	{
		fprintf(stderr, "Need %s version %ld\n", ArpName, ArpVersion);
		exit(3);
	}

/*
 * scan argv[] for the '-e' and '-d' arguments
 */
	for (i = 1; i < argc; ++i)
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'e':
				quickfix = TRUE;
				break;

			case 'd':
				if (i < argc - 1)
					device = argv[i + 1];
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
 */
	if (quickfix)
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
			fprintf(stderr, winerr);
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
	sprintf(buf1, "t:nc%ld", buf1);	/* nobody else is using our stack */
	if ((fh = Open((UBYTE *)buf1, (long)MODE_NEWFILE)) == (BPTR)NULL)
	{
		fprintf(stderr, "Cannot create %s\n", buf1);
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
				lock2name(argp->wa_Lock, buf2, (long)(BUF2SIZE - 1));
			TackOn(buf2, argp->wa_Name);
			av = buf2;
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
		sprintf(buf2, "newcli <nil: >nil: %s from %s", device, buf1);
		if (Execute((UBYTE *)buf2, nilfh, nilfh))
			break;
	}
	if (i == 3)		/* all three failed */
	{
		DeleteFile((UBYTE *)buf1);
		fprintf(stderr, winerr);
		goto exit;
	}
	exitval = 0;	/* The Execute succeeded: exit this program */

exit:
	CloseLibrary((struct Library *) ArpBase);
	exit(exitval);
}

/*
 * fname_case(): Set the case of the filename, if it already exists.
 *				 This will cause the filename to remain exactly the same.
 */
	void
fname_case(name)
	char *name;
{
	register struct FileInfoBlock	*fib;
	register size_t					len;

	fib = get_fib(name);
	if (fib != NULL)
	{
		len = strlen(name);
		if (len == strlen(fib->fib_FileName))	/* safety check */
				memmove(name, fib->fib_FileName, len);
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
	char *fname;
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
 * settitle(): set titlebar of our window
 */
static char *lasttitle = NULL;

	void
settitle(str)
	char *str;
{

	if (wb_window != NULL)
	{
		free(lasttitle);
		lasttitle = alloc((unsigned)(strlen(str) + 7));
		if (lasttitle != NULL)
		{
			sprintf(lasttitle, "VIM - %s", str);
			SetWindowTitles(wb_window, (UBYTE *)lasttitle, (UBYTE *)-1L);
		}
	}
}

	void
resettitle()
{
		if (wb_window != NULL && lasttitle != NULL)
				SetWindowTitles(wb_window, (UBYTE *)lasttitle, (UBYTE *)-1L);
}

/*
 * get name of current directory into buffer 'buf' of length 'len' bytes
 */
dirname(buf, len)
	char		*buf;
	int			len;
{
	return FullName("", buf, len);
}

/*
 * get absolute filename into buffer 'buf' of length 'len' bytes
 */
FullName(fname, buf, len)
	char		*fname, *buf;
	int			len;
{
	BPTR		l;
	int			retval = 0;

	if (fname == NULL)	/* always fail */
		return 0;

	if ((l = Lock((UBYTE *)fname, (long)ACCESS_READ)))/* lock the file */
	{
		retval = lock2name(l, buf, (long)len);
		UnLock(l);
	}
	if (retval == 0 || *buf == 0 || *buf == ':')
		strcpy(buf, fname);			/* something failed; use the filename */
	return retval;
}

/*
 * Get the full filename from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 */
	static int
lock2name(lock, buf, len)
	BPTR	lock;
	char	*buf;
	long	len;
{
	if (dos2)
		return (int)NameFromLock(lock, (UBYTE *)buf, len);
	else
		return (int)PathName(lock, buf, (long)(len/32));		/* call arp function */
}

/*
 * get file permissions for 'name'
 */
	long
getperm(name)
	char		*name;
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

#ifdef DEBUG
/*
 * check for write lock
 */
	long
writelock(name)
	char		*name;
{
	BPTR			lock;

	lock = Lock(name, (long)ACCESS_READ);
	if (lock)
	{
		UnLock(lock);
		lock = Lock(name, (long)ACCESS_WRITE);
		if (lock)
		{
			UnLock(lock);
			return TRUE;		/* can write-lock */
		}
		return FALSE;			/* can read-lock but not write-lock */
	}
	return TRUE;				/* file does not exist */
}
#endif

/*
 * set file permission for 'name' to 'perm'
 */
setperm(name, perm)
	char		*name;
	long		perm;
{
	perm &= ~FIBF_ARCHIVE;				/* reset archived bit */
	return (int)SetProtection((UBYTE *)name, (long)perm);
}

/*
 * check if "name" is a directory
 */
isdir(name)
	char		*name;
{
	struct FileInfoBlock	*fib;
	int 					retval = -1;

	fib = get_fib(name);
	if (fib != NULL)
	{
		retval = (fib->fib_DirEntryType >= 0);
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
	if (raw_out)
	{
		if (term_console)
		{
			outstr(win_resize_off);		/* window resize events de-activated */
			if (size_set)
				outstr("\233t\233u");		/* reset window size (CSI t CSI u) */
		}
		flushbuf();
	}

	if (wb_window != NULL)			/* disable window title */
		SetWindowTitles(wb_window, (UBYTE *)oldwindowtitle, (UBYTE *)-1L);
	if (raw_in)
		settmode(0);
	stopscript();					/* remove autoscript file */
	if (ArpBase)
		CloseLibrary((struct Library *) ArpBase);
	if (close_win)
		Close(raw_in);
	if (r)
		printf("exiting with %d\n", r);	/* somehow this makes :cq work!? */
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
 * return non-zero for failure
 */
	int
mch_get_winsize()
{
	struct ConUnit	*conUnit;
 	char			id_a[sizeof(struct InfoData) + 3];
	struct InfoData *id;

	if (!term_console)	/* not an amiga window */
		return 1;

	/* insure longword alignment */
 	id = (struct InfoData *)(((long)id_a + 3L) & ~3L);

	/*
	 * Should make console aware of real window size, not the one we set.
	 * Unfortunately, under DOS 2.0x this redraws the window and it
	 * is rarely needed, so we skip it now, unless we changed the size.
	 */
	if (size_set)
		outstr("\233t\233u");	/* CSI t CSI u */
	flushbuf();

	if (dos_packet(MP(raw_out), (long)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0 ||
				(wb_window = (struct Window *)id->id_VolumeNode) == NULL)
	{
		/* it's not an amiga window, maybe aux device */
		/* terminal type should be set */
		term_console = FALSE;
		return 1;
	}
	if (oldwindowtitle == NULL)
		oldwindowtitle = (char *)wb_window->Title;
	if (id->id_InUse == (BPTR)NULL)
	{
		fprintf(stderr, "mch_get_winsize: not a console??\n");
		return (2);
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
		return 1;
	}

	check_winsize();
	script_winsize();

	return 0;
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
		outnum((int)Rows);
		outchar('t');
		outchar(CSI);
		outnum((int)Columns);
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
	char *name;
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
		emsg("dos_packet failed");
		return;
	}
	if (id->id_InUse == (BPTR)NULL)
	{
		emsg("not a console??");
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
			emsg("Cannot reset keymap");
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
			emsg("Cannot open keymap file");
			return;
		}
		old = (struct KeyMap *)AllocMem(sizeof(struct KeyMap), MEMF_PUBLIC);
		if (old == NULL)
		{
			emsg(e_outofmem);
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
				emsg("Cannot get old keymap");
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
					emsg("Cannot set keymap");

				/* test for blocking */
				request->io_Command = CMD_READ;
				request->io_Length = 1;
				request->io_Data = (APTR)&c;
				DoIO((struct IORequest *)request);	/* BLOCK HERE! */
				if (request->io_Error)
					emsg("Cannot set keymap");
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
	struct MsgPort *pid;		/* process indentifier ... (handlers message
								 * port ) */
	long			action, 	/* packet type ... (what you want handler to
								 * do )   */
					arg;		/* single argument */
{
	struct MsgPort *replyport;
	struct StandardPacket *packet;

	long			res1;

	replyport = (struct MsgPort *) CreatePort(NULL, 0);
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
}
#endif

/*
 * call shell, return non-zero for failure
 */
	int
call_shell(cmd, filter)
		char	*cmd;
		int		filter;		/* if != 0: called by dofilter() */
{
	BPTR mydir;
	int x;
#ifndef LATTICE
	int	use_execute;
#endif
	int	retval = 0;

	if (close_win)
	{
		/* if Vim opened a window: Executing a shell may cause crashes */
		emsg("Cannot execute shell with -e option");
		return 1;
	}

	if (term_console)
		outstr(win_resize_off); 	/* window resize events de-activated */
	flushbuf();

	settmode(0); 				/* set to cooked mode */
	mydir = Lock((UBYTE *)"", (long)ACCESS_READ);	/* remember current directory */

#ifdef LATTICE		/* not tested yet */
	if (cmd == NULL)
		x = Execute(p_sh, raw_in, raw_out);
	else
		x = Execute(cmd, 0L, raw_out);
	if (!x)
	{
		if (cmd == NULL)
			smsg("Cannot execute shell %s", p_sh);
		else
			smsg("Cannot execute %s", cmd);
		outchar('\n');
		retval = 1;
	}
	else
	{
		if (x = IoErr())
		{
			smsg("%d returned", x);
			outchar('\n');
			retval = 1;
		}
	}
#else
	if (p_st >= 4 || (p_st >= 2 && !filter))
		use_execute = 1;
	else
		use_execute = 0;
	if (cmd == NULL)
	{
		use_execute = 0;
		x = fexecl(p_sh, p_sh, NULL);
	}
	else if (use_execute)
		x = !Execute((UBYTE *)cmd, 0L, raw_out);
	else if (p_st & 1)
		x = fexecl(p_sh, p_sh, cmd, NULL);
	else
		x = fexecl(p_sh, p_sh, "-c", cmd, NULL);
	if (x)
	{
		if (use_execute)
			smsg("Cannot execute %s", cmd);
		else
			smsg("Cannot execute shell %s", p_sh);
		outchar('\n');
		retval = 1;
	}
	else
	{
		if (use_execute)
			x = IoErr();
		else
			x = wait();
		if (x)
		{
			smsg("%d returned", x);
			outchar('\n');
			retval = 1;
		}
	}
#endif

	if (mydir = CurrentDir(mydir))		/* make sure we stay in the same directory */
		UnLock(mydir);
	settmode(1); 						/* set to raw mode */
	resettitle();
	if (term_console)
		outstr(win_resize_on); 			/* window resize events activated */
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
	{
		got_int = TRUE;
		flush_buffers();	/* remove all typeahead and macro stuff */
	}
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

#ifdef WILD_CARDS
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
 * return 0 for success, 1 for error (you may loose some memory)
 *-------------------------------------------------------------------------
 */

/* #include <arpfunctions.h> */
extern void *malloc __ARGS((size_t)), *calloc __ARGS((size_t, size_t));

#define ANCHOR_BUF_SIZE (512)
#define ANCHOR_SIZE (sizeof(struct AnchorPath) + ANCHOR_BUF_SIZE)

ExpandWildCards(num_pat, pat, num_file, file, files_only, list_notfound)
	int 			num_pat;
	char		  **pat;
	int 		   *num_file;
	char		 ***file;
	int			files_only;
	int			list_notfound;
{
	int 		i;
	int 		retval = 0;

	struct DirectoryEntry *FileList = NULL;
	struct DirectoryEntry *de;
	struct AnchorPath *Anchor;
	LONG			Result;

	*num_file = 0;
	*file = (char **)"";

	/* Get our AnchorBase */
	Anchor = (struct AnchorPath *) calloc((size_t)1, (size_t)ANCHOR_SIZE);
	if (!Anchor)
	{
OUT_OF_MEMORY:
		retval = 1;
		*file = (char **)"Out of memory";
		goto Return;
	}
#ifdef LATTICE
	Anchor->ap_Length = ANCHOR_BUF_SIZE;
#else
	Anchor->ap_StrLen = ANCHOR_BUF_SIZE;
#endif

	if (num_pat > 0)
	{
		for (i = 0; i < num_pat; i++)
		{
			Result = FindFirst(pat[i], Anchor);
			while (Result == 0)
			{
				if (!files_only || Anchor->ap_Info.fib_DirEntryType < 0)
				{
					(*num_file)++;
					if (!AddDANode(Anchor->ap_Buf, &FileList, 0L, (long)i))
					{
						FreeAnchorChain(Anchor);
						FreeDAList(FileList);
						goto OUT_OF_MEMORY;
					}
				}
				Result = FindNext(Anchor);
			}
			if (Result == ERROR_BUFFER_OVERFLOW)
			{
				FreeAnchorChain(Anchor);
				FreeDAList(FileList);
				retval = 1;
				*file = (char **)"ANCHOR_BUF_SIZE too small.";
				goto Return;
			}
			if (Result != ERROR_NO_MORE_ENTRIES)
			{
				if (list_notfound)	/* put object with error in list */
				{
					(*num_file)++;
					if (!AddDANode(pat[i], &FileList, 0L, (long)i))
					{
						FreeAnchorChain(Anchor);
						FreeDAList(FileList);
						goto OUT_OF_MEMORY;
					}
				}
				else if (Result != ERROR_OBJECT_NOT_FOUND)
				{
					FreeAnchorChain(Anchor);
					FreeDAList(FileList);
					retval = 1;
					*file = (char **)"I/O ERROR";
					goto Return;
				}
			}
		}
		FreeAnchorChain(Anchor);

		de = FileList;
		if (de)
		{
			*file = (char **) malloc(sizeof(char *) * (*num_file));
			if (*file == NULL)
				goto OUT_OF_MEMORY;
			for (i = 0; de; de = de->de_Next, i++)
			{
				(*file)[i] = (char *) malloc(strlen(de->de_Name) + 1);
				if ((*file)[i] == NULL)
					goto OUT_OF_MEMORY;
				strcpy((*file)[i], de->de_Name);
			}
		}
		FreeDAList(FileList);
	}
Return:
	return retval;
}

void
FreeWild(num, file)
		int num;
		char **file;
{
		if (file == NULL || num == 0)
				return;
		while (num--)
				free(file[num]);
		free(file);
}
#endif
