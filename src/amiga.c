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
#ifdef LATTICE
# include <libraries/arp_pragmas.h>
#endif

/*
 * At this point TRUE and FALSE are defined as 1L and 0L, but we want 1 and 0.
 */
#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0

static unsigned GetCharacter __ARGS((bool_t));
static int getCSIsequence __ARGS((void));
static int get_winsize __ARGS((void));

#ifndef AZTEC_C
static long dos_packet __ARGS((struct MsgPort *, long, long));
#endif

static BPTR raw_in = (BPTR)NULL;
static BPTR raw_out = (BPTR)NULL;

struct Library	*IntuitionBase;
struct ArpBase *ArpBase;

static struct Window   *wb_window;
static char *oldwindowtitle = NULL;
static int quickfix = FALSE;

static char win_resize_on[]  = "\033[12{";
static char win_resize_off[] = "\033[12}";

/*
 * the number of calls to Write is reduced by using the buffer "outbuf"
 */
#define BSIZE	2048
static u_char	outbuf[BSIZE];
static int		bpos = 0;

/*
 * flushbuf(): flush the output buffer
 */
	void
flushbuf()
{
	if (bpos != 0)
		Write(raw_out, (char *)outbuf, (long)bpos);
	bpos = 0;
}

/*
 * outchar(c): put a character into the output buffer. Flush it if it becomes full.
 */
	void
outchar(c)
	unsigned			  c;
{
		outbuf[bpos] = c;
		++bpos;
		if (bpos >= BSIZE)
				flushbuf();
}

/*
 * outstr(s): put a string character at a time into the output buffer.
 */
	void
outstr(s)
	register char			 *s;
{
	while (*s)
	{
		outchar(*s);
		++s;
	}
}

/*
 * GetCharacter(): low level input funcion.
 * Get a character from the script file or from the keyboard.
 * If peek == TRUE only look whether there is a character.
 *
 * Use a buffer because Read() is slow.
 */
	static unsigned
GetCharacter(peek)
		bool_t peek;
{
	static u_char inbuf[20];
	static int len = 0;			/* number of valid chars in inbuf[] */
	static int idx = 0;			/* index of next char in inbuf[] */
	register unsigned cc;
	register int c;

	if (idx >= len) 	/* inbuf is empty: fill it */
	{
		idx = 0;		/* reset idx and len */
		len = 0;
retry:
		if (scriptin[curscript] != NULL)
		{
				if (got_int || (c = getc(scriptin[curscript])) < 0)	/* reached EOF */
				{
						fclose(scriptin[curscript]);
						scriptin[curscript] = NULL;
						if (curscript > 0)
							--curscript;
						if (recoverymode)
							openrecover();
						goto retry;		/* may read from other script */
				}
				else
				{
						inbuf[0] = c;
						len = 1;
				}
		}
		if (scriptin[curscript] == NULL)
		{
				if (peek)
				{
					if (WaitForChar(raw_in, 100L) == 0)	/* no character available */
						return 0;
				}
				/*
				 * If there is no character available within 2 seconds (default)
				 * write the autoscript file to disk
				 */
				else if (WaitForChar(raw_in, P(P_UT) * 1000L) == 0)
						updatescript(0);

				/*
				 * if we are really getting a character, reset interrupt flag
				 */
				breakcheck();
				if (got_int)		/* skip typed characters */
				{
					got_int = FALSE;
					while (WaitForChar(raw_in, 100L) != 0)	/* characters available */
						Read(raw_in, (char *)inbuf, (long)(sizeof(inbuf)));
				}
				if ((len = Read(raw_in, (char *)inbuf, (long)(sizeof(inbuf)))) <= 0)
						return 0;
		}
	}

	if (peek)
		return TRUE;
	cc = inbuf[idx];
	++idx;
	if (cc == 0)
		cc = K_ZERO;		/* 0 means no-character, replace ^@ with K_ZERO */
	return cc;
}

/*
 * getCSIsequence - get a CSI sequence
 *				  - either cursor keys, help, functionkeys, or some
 *					other sequence (if other, check window size)
 */
	static int
getCSIsequence()
{
	register int 		tmp;
	register unsigned	c;

	c = GetCharacter(FALSE);
	if (isdigit(c))
	{
		tmp = 0;
		while (isdigit(c))
		{
				tmp = tmp * 10 + c - '0';
				c = GetCharacter(FALSE);
		}
		if (c == '~')			/* function key (may be shifted) */
			return (K_F1 + tmp);
	}
	switch (c)
	{
	  case 'A': 		/* cursor up */
		return K_UARROW;
	  case 'B': 		/* cursor down */
		return K_DARROW;
	  case 'C': 		/* cursor right */
		return K_RARROW;
	  case 'D': 		/* cursor left */
		return K_LARROW;
	  case 'T': 		/* shift cursor up */
		return K_SUARROW;
	  case 'S': 		/* shift cursor down */
		return K_SDARROW;
	  case ' ': 		/* shift cursor left or right */
		if ((c = GetCharacter(FALSE)) == 'A')			/* shift cursor left */
			return K_SLARROW;
		if (c == '@')			/* shift cursor right */
			return K_SRARROW;
		break;
	  case '?': 		/* help */
		if ((c = GetCharacter(FALSE)) == '~')
			return K_HELP;
		break;
	}

	/* must have been a window resize event; skip rest of the sequence */
	while (c != 'r')
	{
		if (!GetCharacter(TRUE))
				break;
		c = GetCharacter(FALSE);
	}
	set_winsize(0, 0);		/* get size and redraw screen */

	return 0;				/* some other control code */
}

/*
 * inchar() - get a character from the keyboard
 */
	int
inchar(async)
		bool_t async;	/* set to TRUE when not waiting for the char */
{
	register int c;

	flushbuf();

	if (async && !GetCharacter(TRUE))
		return NUL;

	for (;;)
	{
		c = GetCharacter(FALSE);
		if (c == CSI)
				c = getCSIsequence();
		if (c != 0)
				break;
	}
	return c;
}

/*
 * outnum - output a number fast (used for row and column numbers)
 */
	void
outnum(n)
		register long n;
{
		char buf[20];
		register char *p;

		p = &buf[19];
		*p = NUL;
		do
		{
			--p;
			*p = n % 10 + '0';
			n /= 10;
		}
		while (n > 0);
		outstr(p);
}

/*
 * give a warning for an error
 */
	void
beep()
{
	flush_buffers();
	if (P(P_VB))
	{
#ifdef AUX
		if (!Aux_Device)
			outstr("\007\007");
		else
#endif
		{
			msg("     ^G");		/* primitive visual bell for AUX */
			msg("       ");
			msg("      ^G");
			msg("        ");
		}
	}
	else
		outchar('\007');
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
delay()
{
	Delay(25L);
}

	void
windinit()
{
	static char intlibname[] = "intuition.library";

#ifdef AZTEC_C
	Enable_Abort = 0;			/* disallow vim to be aborted */
#endif
	Columns = 80;
	P(P_LI) = Rows = 24;

	if (!quickfix)
	{
		raw_in = Input();
		raw_out = Output();
	}
	setmode(1);

#ifdef AUX
	if (!Aux_Device)
#endif
		outstr(win_resize_on); 			/* window resize events activated */
	flushbuf();

	wb_window = NULL;
	if ((IntuitionBase = OpenLibrary(intlibname, 0L)) == NULL)
	{
		fprintf(stderr, "cannot open %s!?\n", intlibname);
		windexit(3);
	}
	if (get_winsize())
		windexit(4);
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
 * Otherwise we would never know when editing is finished.
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
	static char		exstr[] = "newcli <nil: >nil: con:0/0/%d/200/ from %s";
	static char		winerr[] = "VIM: Can't open window!\n";
	struct WBArg	*argp;
	int				ac;
	char			*av;

	if (!(ArpBase = (struct ArpBase *) OpenLibrary(ArpName, ArpVersion)))
	{
		fprintf(stderr, "Need %s version %ld\n", ArpName, ArpVersion);
		exit(3);
	}
	if (argc != 0)		/* only check Input and Output when not started from WB */
	{
		if (IsInteractive(Input()) && IsInteractive(Output()))
			return;
	}

/*
 * if we are in quickfix mode, we open our own window
 */
	for (i = 1; i < argc; ++i)
		if (argv[i][0] == '-' && argv[i][1] == 'e')
		{
			quickfix = TRUE;
			raw_in = Open("CON:0/0/640/200/", (long)MODE_NEWFILE);
			if (raw_in == (BPTR)NULL)
			{
				raw_in = Open("CON:0/0/480/200/", (long)MODE_NEWFILE);
				if (raw_in == (BPTR)NULL)
				{
					fprintf(stderr, winerr);
					exit(2);
				}
			}
			raw_out = raw_in;
			return;
		}

	if ((nilfh = Open("NIL:", (long)MODE_NEWFILE)) == (BPTR)NULL)
	{
		fprintf(stderr, "Cannot open NIL:\n");
		exit(3);
	}

	/*
	 * make a unique name for the temp file (which we will not delete!)
	 */
	sprintf(buf1, "t:nc%ld", buf1);	/* nobody else is using our stack */
	if ((fh = Open(buf1, (long)MODE_NEWFILE)) == (BPTR)NULL)
	{
		fprintf(stderr, "Cannot create %s\n", buf1);
		exit(3);
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
				PathName(argp->wa_Lock, buf2, (long)(BUF2SIZE / 32 - 1));
			TackOn(buf2, argp->wa_Name);
			av = buf2;
		}
		else
			av = argv[i];

		if (strchr(av, ' '))
			Write(fh, "\"", 1L);
		Write(fh, av, (long)strlen(av));
		if (strchr(av, ' '))
			Write(fh, "\"", 1L);
		Write(fh, " ", 1L);
	}
	Write(fh, "\nendcli\n", 8L);
	Close(fh);

	sprintf(buf2, exstr, 640, buf1);
	if (!Execute(buf2, nilfh, nilfh))
	{
		sprintf(buf2, exstr, 320, buf1);
		if (!Execute(buf2, nilfh, nilfh))
		{
				DeleteFile(buf1);
				fprintf(stderr, winerr);
				exit(2);
		}
	}
	exit(0);	/* The Execute succeeded: exit this program */
}

/*
 * fname_case(): Set the case of the filename, if it already exists.
 *				 This will cause the filename to remain exactly the same.
 */
	void
fname_case(name)
	char *name;
{
	BPTR	flock;
	struct FileInfoBlock *fib;

	if (name == NULL)
		return;
	flock = Lock(name, (long)ACCESS_READ);
	if (flock != (BPTR)NULL)
	{
		fib = (struct FileInfoBlock *)malloc(sizeof(struct FileInfoBlock));
		if (fib != NULL)
		{
			if (Examine(flock, fib))
			{
				/* check name length, might not be necessary */
				if (strlen(name) == strlen(fib->fib_FileName))
					memmove(name, fib->fib_FileName, strlen(name));
			}
			free(fib);
		}
		UnLock(flock);
	}
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
			SetWindowTitles(wb_window, lasttitle, (char *)-1L);
		}
	}
}

	void
resettitle()
{
		if (wb_window != NULL && lasttitle != NULL)
				SetWindowTitles(wb_window, lasttitle, (char *)-1L);
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
	BPTR l;
	int retval = 0;

	if (fname == NULL)	/* always fail */
		return 0;

	if ((l = Lock(fname, (long)ACCESS_READ)))/* lock the file */
	{
		retval = PathName(l, buf, (long)(len/32));		/* call arp function */
		UnLock(l);
	}
	if (retval == 0)		/* something failed; use at least the filename */
		strcpy(buf, fname);
	return retval;
}

/*
 * get file permissions for 'name'
 */
getperm(name)
	char		*name;
{
	BPTR			lock;
	char			buf[sizeof(struct FileInfoBlock) + 2];
	struct FileInfoBlock *fib;
	int 			retval = -1;

	lock = Lock(name, (long)ACCESS_READ);
	if (lock)
	{
		fib = (struct FileInfoBlock *)(((long)buf + 3) & ~3);
								/* make fib start on longword boundary */
		if (Examine(lock, fib))
			retval = fib->fib_Protection;
		UnLock(lock);
	}
	return retval;
}

/*
 * set file permission for 'name' to 'perm'
 */
setperm(name, perm)
	char		*name;
	int			perm;
{
	perm &= ~FIBF_ARCHIVE;				/* reset archived bit */
	return SetProtection(name, (long)perm);
}

/*
 * check if "name" is a directory
 */
isdir(name)
	char		*name;
{
	BPTR			lock;
	char			buf[sizeof(struct FileInfoBlock) + 2];
	struct FileInfoBlock *fib;
	int 			retval = -1;

	lock = Lock(name, (long)ACCESS_READ);
	if (lock)
	{
		fib = (struct FileInfoBlock *)(((long)buf + 3) & ~3);
								/* make fib start on longword boundary */
		if (Examine(lock, fib))
			retval = (fib->fib_DirEntryType >= 0);
		UnLock(lock);
	}
	return retval;
}

	void
windexit(r)
	int 			r;
{
#ifdef AUX
	if (!Aux_Device)
#endif
		outstr(win_resize_off);		/* window resize events de-activated */
	flushbuf();

	if (wb_window != NULL)			/* disable window title */
		SetWindowTitles(wb_window, oldwindowtitle, (char *)-1L);
	setmode(0);
	stopscript();					/* remove autoscript file */
	CloseLibrary((struct Library *) ArpBase);
	if (quickfix)
		Close(raw_in);
	exit(r);
}

	void
windgoto(r, c)
	int		r;
	int		c;
{
#ifdef AUX
	if (Aux_Device)
		outstr("\033[");
	else
#endif
		outchar(CSI);
	outnum((long)(r + 1));
	outchar(';');
	outnum((long)(c + 1));
	outchar('H');
}

/*
 * raw.c
 *
 * This is a routine for setting a given stream to raw or cooked mode on the
 * Amiga . This is useful when you are using Lattice C to produce programs
 * that want to read single characters with the "getch()" or "fgetc" call.
 *
 * Written : 18-Jun-87 By Chuck McManis.
 */

#define MP(xx)	((struct MsgPort *)((struct FileHandle *) (BADDR(xx)))->fh_Type)

/*
 * Function setmode() - Convert the specified file pointer to 'raw' or 'cooked'
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
setmode(raw)
	bool_t			raw;
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

static void check_winsize __ARGS((void));

	static int
get_winsize()
{
	struct ConUnit	*conUnit;
 	char			id_a[sizeof(struct InfoData) + 3];
	struct InfoData *id;

	/* insure longword alignment */
 	id = (struct InfoData *)(((long)id_a + 3L) & ~3L);

	/* Make console aware of real window size, not the one we set. */
	outstr("\233t\233u");	/* CSI t CSI u */
	flushbuf();

	if (dos_packet(MP(raw_out), (long)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0)
	{
#ifdef AUX
		Rows = -1;
		Aux_Device = TRUE;
#else
		fprintf(stderr, "get_winsize: cannot send packet\n");
		return (1);
#endif
	}
#ifdef AUX
	if (!Aux_Device)
#endif
	{
		wb_window = (struct Window *) id->id_VolumeNode; /* for settitle() */
		if (oldwindowtitle == NULL)
			oldwindowtitle = (char *)wb_window->Title;
		if (id->id_InUse == (BPTR)NULL)
		{
			fprintf(stderr, "get_winsize: not a console??\n");
			return (2);
		}
		conUnit = (struct ConUnit *) ((struct IOStdReq *) id->id_InUse)->io_Unit;

		/* get window size */
		Rows = conUnit->cu_YMax + 1;
		Columns = conUnit->cu_XMax + 1;
	}
#ifdef AUX
	if (Rows < 0 || Rows > 200) 	/* AUX: device (I hope) */
	{
		Columns = 80;
		Rows = 24;
		Aux_Device = TRUE;
	}
#endif

	check_winsize();
	script_winsize();

	return (0);
}

	static void
check_winsize()
{
	if (Columns < 5)
		Columns = 5;
	else if (Columns > MAX_COLUMNS)
		Columns = MAX_COLUMNS;
	if (Rows < 2)
		Rows = 2;
	P(P_LI) = Rows;
	P(P_SS) = Rows >> 1;
}

	void
set_winsize(width, height)
	int		width, height;
{
	register int 		tmp;

	if (width < 0 || height < 0)	/* just checking... */
		return;

	if (State == HITRETURN)	/* postpone the resizing */
	{
		State = SETWINSIZE;
		return;
	}
	screenclear();
	flushbuf(); 		/* must do this before get_winsize for some obscure reason */
	if (width == 0)
		get_winsize();
	else
	{
		Rows = height;
		Columns = width;
		check_winsize();

		outchar(CSI);
		outnum((long)Rows);
		outchar('t');
		outchar(CSI);
		outnum((long)Columns);
		outchar('u');
		flushbuf();
	}
	if (State == HELP)
			redrawhelp();
	else
	{
			tmp = RedrawingDisabled;
			RedrawingDisabled = FALSE;
			cursupdate();
			updateScreen(NOT_VALID);
			RedrawingDisabled = tmp;
			if (State == CMDLINE)
				redrawcmdline();
			else
				windgoto(Cursrow, Curscol);
	}
	flushbuf();
}

#ifdef SETKEYMAP
/*
 * load and activate a new keymap for our CLI - DOES NOT WORK -
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

#ifdef AUX
	if (Aux_Device)
		return;
#endif

	/* insure longword alignment */
 	id = (struct InfoData *)(((long)id_a + 3L) & ~3L);

	if (dos_packet(MP(raw_out), ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0)
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
			emsg("Cannot AllocMem");
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
			}
		}
	}
}
#endif

#ifndef AZTEC_C
/*
 * Sendpacket.c
 *
 * An invaluable addition to your Amiga.lib file. This code sends a packet the
 * given message port. This makes working around DOS lots easier.
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

	void
call_shell(cmd, filter)
		char	*cmd;
		bool_t	filter;		/* if != 0: called by dofilter() */
{
	BPTR mydir;
	int x;
#ifndef LATTICE
	int	use_execute;
#endif

	if (quickfix)
	{
		emsg("Cannot execute shell with -e option");
		return;
	}

#ifdef AUX
	if (!Aux_Device)
#endif
		outstr(win_resize_off); 	/* window resize events de-activated */
	flushbuf();

	setmode(0); 				/* set to cooked mode */
	mydir = Lock("", (long)ACCESS_READ);	/* remember current directory */

#ifdef LATTICE		/* not tested yet */
	if (cmd == NULL)
		x = Execute(PS(P_SHELL), raw_in, raw_out);
	else
		x = Execute(cmd, 0L, raw_out);
	if (!x)
	{
		if (cmd == NULL)
			smsg("Cannot execute shell %s", PS(P_SHELL));
		else
			smsg("Cannot execute %s", cmd);
		outchar('\n');
	}
	else
	{
		if (x = IoErr())
		{
			smsg("%d returned", x);
			outchar('\n');
		}
	}
#else
	if (P(P_ST) >= 4 || (P(P_ST) >= 2 && !filter))
		use_execute = 1;
	else
		use_execute = 0;
	if (cmd == NULL)
	{
		use_execute = 0;
		x = fexecl(PS(P_SHELL), PS(P_SHELL), NULL);
	}
	else if (use_execute)
		x = !Execute(cmd, 0L, raw_out);
	else if (P(P_ST) & 1)
		x = fexecl(PS(P_SHELL), PS(P_SHELL), cmd, NULL);
	else
		x = fexecl(PS(P_SHELL), PS(P_SHELL), "-c", cmd, NULL);
	if (x)
	{
		if (use_execute)
			smsg("Cannot execute %s", cmd);
		else
			smsg("Cannot execute shell %s", PS(P_SHELL));
		outchar('\n');
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
		}
	}
#endif

	if (mydir = CurrentDir(mydir))		/* make sure we stay in the same directory */
		UnLock(mydir);
	setmode(1); 						/* set to raw mode */
	resettitle();
#ifdef AUX
	if (!Aux_Device)
#endif
	outstr(win_resize_on); 			/* window resize events activated */
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
 * Mool: return 0 for success, 1 for error (you may loose some memory)
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
	bool_t		files_only;
	bool_t		list_notfound;
{
	int 		i;
	int 		retval = 0;

	struct DirectoryEntry *FileList = NULL;
	struct DirectoryEntry *de;
	struct AnchorPath *Anchor;
	LONG			Result;

	*num_file = 0;
	*file = NULL;

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
			while (Result == 0 || Result == ERROR_OBJECT_NOT_FOUND)
			{
				if (Result == ERROR_OBJECT_NOT_FOUND)
				{
					if (list_notfound)	/* put not found object in list */
					{
						(*num_file)++;
						if (!AddDANode(pat[i], &FileList, 0L, (long)i))
						{
							FreeAnchorChain(Anchor);
							FreeDAList(FileList);
							goto OUT_OF_MEMORY;
						}
					}
				}
				else if (!files_only || Anchor->ap_Info.fib_DirEntryType < 0)
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
			} else if (Result != ERROR_NO_MORE_ENTRIES)
			{
				FreeAnchorChain(Anchor);
				FreeDAList(FileList);
				retval = 1;
				*file = (char **)"I/O ERROR";
				goto Return;
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
