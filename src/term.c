/* vi:sw=4:ts=4:
 *
 * term.c -- VIM - Vi IMitation
 *
 * primitive termcap support added
 *
 * NOTE: padding and variable substitution is not performed,
 * when compiling without TERMCAP, we use tputs() and tgoto() dummies.
 *
 * 14.6.92
 */

#include "vim.h"
#include "globals.h"
#include "param.h"
#include "proto.h"
#ifdef TERMCAP
# ifdef linux
#  include <termcap.h>
#  define TPUTSFUNCAST (outfuntype)
# else
#  define TPUTSFUNCAST
#  ifdef AMIGA
#   include "proto/termlib.pro"
#  endif
# endif
#endif

#ifdef DEBUG
# define TTEST(a) debug1("%s: ", "a"); if (a) {debug2("%02x %s\n", *a, a + 1);} else debug("NULL\n");
#endif

static void parse_builtin_tcap __ARGS((Tcarr *tc, char *s));

/*
 * Builtin_tcaps must always contain DFLT_TCAP as the first entry!
 * DFLT_TCAP is used, when no terminal is specified with -T option or $TERM.
 * The entries are compact, therefore they normally are included even when
 * TERMCAP is defined.
 * When TERMCAP is defined, the builtin entries can be accessed with
 * "builtin_amiga", "builtin_ansi", "builtin_debug", etc.
 */
static char *builtin_tcaps[] =
{
#ifndef NO_BUILTIN_TCAPS
  DFLT_TCAP,		/* almost allways included */
# if !defined(UNIX) && (defined(ALL_BUILTIN_TCAPS) || defined(SOME_BUILTIN_TCAPS))
  ANSI_TCAP,		/* default for unix */
# endif
# if !defined(AMIGA) && (defined(ALL_BUILTIN_TCAPS) || defined(SOME_BUILTIN_TCAPS))
  AMIGA_TCAP,		/* default for amiga */
# endif
# if !defined(MSDOS) && (defined(ALL_BUILTIN_TCAPS) || defined(SOME_BUILTIN_TCAPS))
  PCTERM_TCAP,		/* default for MSdos */
# endif
# if !defined(ATARI) && defined(ALL_BUILTIN_TCAPS)
  ATARI_TCAP,		/* default for Atari */
# endif
# if defined(UNIX) || defined(ALL_BUILTIN_TCAPS) || defined(SOME_BUILTIN_TCAPS)
  XTERM_TCAP,		/* always included on unix */
# endif
# ifdef ALL_BUILTIN_TCAPS
  VT52_TCAP,
# endif
# if defined(DEBUG) || defined(ALL_BUILTIN_TCAPS)
  DEBUG_TCAP,		/* always included when debugging */
# endif
#else /* NO_BUILTIN_TCAPS */
  "dump\0\0\0",
#endif /* NO_BUILTIN_TCAPS */
  NULL,
};

/*
 * Term_strings contains currently used terminal strings.
 * It is initialized with the default values by parse_builtin_tcap().
 * The values can be changed by setting the parameter with the same name.
 */
Tcarr term_strings;

/*
 * Parsing of the builtin termcap entries.
 * The terminal's name is not set, as this is already done in termcapinit().
 * Chop builtin termcaps, string entries are already '\0' terminated.
 * not yet implemented:
 *   boolean entries could be empty strings;
 *   numeric entries would need a flag (e.g. high bit of the skip byte),
 *   so that parse_builtin_tcap can handle them.
 */
	static void
parse_builtin_tcap(tc, s)
	Tcarr *tc;
	char *s;
{
	char **p = &tc->t_name;

	p++;
	for (;;)
    {
		while (*s++)
			;
		p += *s++;
		if (!*s)
			return;
		*p++ = s;
    }
}

#ifdef TERMCAP
# ifndef linux		/* included in <termlib.h> */
#  ifndef AMIGA		/* included in proto/termlib.pro */
int				tgetent();
int				tgetnum();
char			*tgetstr();
int				tputs();
#  endif /* AMIGA */
#  ifndef hpux
extern short	ospeed;
#  endif
# endif /* linux */
# ifndef hpux
extern char		*UP, *BC, PC;
# endif
#endif /* TERMCAP */

	void
set_term(term)
	char *term;
{
	char **p = builtin_tcaps;
#ifdef TERMCAP
	int builtin = 0;
#endif
	int width = 0, height = 0;

	clear_termparam();		/* clear old parameters */
	if (!strncmp(term, "builtin_", (size_t)8))
	{
		term += 8;
#ifdef TERMCAP
		builtin = 1;
#endif
	}
#ifdef TERMCAP
	else
	{
		for (;;)
		{
			char			*p;
			static char	tstrbuf[TBUFSZ];
			char			tbuf[TBUFSZ];
			char			*tp = tstrbuf;
			int			i;

			i = tgetent(tbuf, term);
			if (i == -1)
			{
				emsg("Cannot open termcap file");
				builtin = 1;
				break;
			}
			if (i == 0)
			{
				emsg("terminal entry not found");
				builtin = 1;
				break;
			}

		/* output strings */
			T_EL = tgetstr("ce", &tp);
			T_IL = tgetstr("al", &tp);
			T_CIL = tgetstr("AL", &tp);
			T_DL = tgetstr("dl", &tp);
			T_CDL = tgetstr("DL", &tp);
			T_ED = tgetstr("cl", &tp);
			T_CI = tgetstr("vi", &tp);
			T_CV = tgetstr("ve", &tp);
			T_TP = tgetstr("me", &tp);
			T_TI = tgetstr("mr", &tp);
			T_CM = tgetstr("cm", &tp);
			T_SR = tgetstr("sr", &tp);
			T_CRI = tgetstr("RI", &tp);
			T_VB = tgetstr("vb", &tp);
			T_KS = tgetstr("ks", &tp);
			T_KE = tgetstr("ke", &tp);
			T_TS = tgetstr("ti", &tp);
			T_TE = tgetstr("te", &tp);

		/* key codes */
			term_strings.t_ku = tgetstr("ku", &tp);
			term_strings.t_kd = tgetstr("kd", &tp);
			term_strings.t_kl = tgetstr("kl", &tp);
			term_strings.t_kr = tgetstr("kr", &tp);
			/* term_strings.t_sku = tgetstr("", &tp); termcap code unknown */
			/* term_strings.t_skd = tgetstr("", &tp); termcap code unknown */
			term_strings.t_sku = NULL;
			term_strings.t_skd = NULL;
			term_strings.t_skl = tgetstr("#4", &tp);
			term_strings.t_skr = tgetstr("%i", &tp);
			term_strings.t_f1 = tgetstr("k1", &tp);
			term_strings.t_f2 = tgetstr("k2", &tp);
			term_strings.t_f3 = tgetstr("k3", &tp);
			term_strings.t_f4 = tgetstr("k4", &tp);
			term_strings.t_f5 = tgetstr("k5", &tp);
			term_strings.t_f6 = tgetstr("k6", &tp);
			term_strings.t_f7 = tgetstr("k7", &tp);
			term_strings.t_f8 = tgetstr("k8", &tp);
			term_strings.t_f9 = tgetstr("k9", &tp);
			term_strings.t_f10 = tgetstr("k;", &tp);
			term_strings.t_sf1 = tgetstr("F1", &tp);	/* really function keys 11-20 */
			term_strings.t_sf2 = tgetstr("F2", &tp);
			term_strings.t_sf3 = tgetstr("F3", &tp);
			term_strings.t_sf4 = tgetstr("F4", &tp);
			term_strings.t_sf5 = tgetstr("F5", &tp);
			term_strings.t_sf6 = tgetstr("F6", &tp);
			term_strings.t_sf7 = tgetstr("F7", &tp);
			term_strings.t_sf8 = tgetstr("F8", &tp);
			term_strings.t_sf9 = tgetstr("F9", &tp);
			term_strings.t_sf10 = tgetstr("FA", &tp);
			term_strings.t_help = tgetstr("%1", &tp);
			term_strings.t_undo = tgetstr("&8", &tp);

			height = tgetnum("li");
			width = tgetnum("co");

# ifndef hpux
			BC = tgetstr("bc", &tp);
			UP = tgetstr("up", &tp);
			p = tgetstr("pc", &tp);
			if (p)
				PC = *p;
			ospeed = 0;
# endif
			break;
		}
	}
	if (builtin)
#endif
	{
		while (*p && strcmp(term, *p))
			p++;
		if (!*p)
		{
			fprintf(stderr, "'%s' not builtin. Available terminals are:\n", term);
			for (p = builtin_tcaps; *p; p++)
				fprintf(stderr, "\t%s\n", *p);
			sleep(2);
			fprintf(stderr, "defaulting to '%s'\n", *builtin_tcaps);
			sleep(2);
			p = builtin_tcaps;
			free(term_strings.t_name);
			term_strings.t_name = strsave(term = *p);
		}
		parse_builtin_tcap(&term_strings, *p);
	}
#if defined(AMIGA) || defined(MSDOS)
		/* DFLT_TCAP indicates that it is the machine console. */
	if (strcmp(term, *builtin_tcaps))
		term_console = FALSE;
	else
	{
		term_console = TRUE;
# ifdef AMIGA
		win_resize_on();		/* enable window resizing reports */
# endif
	}
#endif
	ttest(TRUE);
		/* display initial screen after ttest() checking. jw. */
	if (width <= 0 || height <= 0)
    {
		/* termcap failed to report size */
		/* set defaults, in case mch_get_winsize also fails */
		width = 80;
		height = 24;
	}
	set_winsize(width, height, FALSE);  
}

#if defined(TERMCAP) && defined(UNIX)
/*
 * Get Columns and Rows from the termcap. Used after a window signal if the
 * ioctl() fails. It doesn't make sense to call tgetent each time if the "co"
 * and "li" entries never change. But this may happen on some systems.
 */
	void
getlinecol()
{
	char			tbuf[TBUFSZ];

	if (term_strings.t_name && tgetent(tbuf, term_strings.t_name) > 0)
	{
		if (Columns == 0)
			Columns = tgetnum("co");
		if (Rows == 0)
			Rows = tgetnum("li");
	}
}
#endif

static char *tltoa __PARMS((unsigned long));

	static char *
tltoa(i)
	unsigned long i;
{
	static char buf[16];
	char		*p;

	p = buf + 15;
	*p = '\0';
	do
	{
		--p;
		*p = i % 10 + '0';
		i /= 10;
    }
	while (i > 0 && p > buf);
	return p;
}

#ifndef TERMCAP

/*
 * minimal tgoto() implementation.
 * no padding and we only parse for %i %d and %+char
 */

	char *
tgoto(cm, x, y)
	char *cm;
	int x, y;
{
	static char buf[30];
	char *p, *s, *e;

	if (!cm)
		return "OOPS";
	e = buf + 29;
	for (s = buf; s < e && *cm; cm++)
    {
		if (*cm != '%')
        {
			*s++ = *cm;
			continue;
		}
		switch (*++cm)
        {
		case 'd':
			p = tltoa((unsigned long)y);
			y = x;
			while (*p)
				*s++ = *p++;
			break;
		case 'i':
			x++;
			y++;
			break;
		case '+':
			*s++ = (char)(*++cm + y);
			y = x;
			break;
        case '%':
			*s++ = *cm;
			break;
		default:
			return "OOPS";
		}
    }
	*s = '\0';
	return buf;
}

#endif /* TERMCAP */

/*
 * Termcapinit is called from main() to initialize the terminal.
 * The optional argument is given with the -T command line option.
 */
	void
termcapinit(term)
	char *term;
{
	if (!term)
		term = (char *)vimgetenv("TERM");
	if (!term || !*term)
		term = *builtin_tcaps;
	term_strings.t_name = strsave(term);
	set_term(term);
}

/*
 * a never-padding outstr.
 * use this whenever you don't want to run the string through tputs.
 * tputs above is harmless, but tputs from the termcap library 
 * is likely to strip off leading digits, that it mistakes for padding
 * information. (jw)
 */
	void
outstrn(s)
	char *s;
{
	while (*s)
		outchar(*s++);
}

#ifndef MSDOS		/* there is a special version of outstr() in msdos.c */
/*
 * outstr(s): put a string character at a time into the output buffer.
 * If TERMCAP is defined use the termcap parser. (jw)
 */
	void
outstr(s)
	register char			 *s;
{
	if (s)
#ifdef TERMCAP
		tputs(s, 1, TPUTSFUNCAST outchar);
#else
		while (*s)
			outchar(*s++);
#endif
}
#endif /* MSDOS */

/* 
 * cursor positioning using termcap parser. (jw)
 */
	void
windgoto(row, col)
	int		row;
	int		col;
{
#ifdef TERMCAP
	tputs(tgoto(T_CM, col, row), 1, TPUTSFUNCAST outchar);
#else
	outstr(tgoto(T_CM, col, row));
#endif
}

/*
 * Set cursor to current position.
 * Should be optimized for minimal terminal output.
 */

	void
setcursor()
{
	if (!RedrawingDisabled)
		windgoto(Cursrow, Curscol);
}

	void
ttest(pairs)
	int	pairs;
{
	char buf[70];
	char *s = "terminal capability %s required.\n";
	char *t = NULL;

#ifdef TTEST
  TTEST(T_EL);
  TTEST(T_IL);
  TTEST(T_CIL);
  TTEST(T_DL);
  TTEST(T_CDL);
  TTEST(T_ED);
  TTEST(T_CI);
  TTEST(T_CV);
  TTEST(T_TP);
  TTEST(T_TI);
  TTEST(T_CM);
  TTEST(T_SR);
  TTEST(T_CRI);
#endif /* TTEST */

  /* hard requirements */
	if (!T_ED || !*T_ED)	/* erase display */
		t = "cl";
	if (!T_CM || !*T_CM)	/* cursor motion */
		t = "cm";

	if (t)
    {
    	sprintf(buf, s, t);
    	emsg(buf);
    }

	if (pairs)
	{
	  /* optional pairs */
		if ((!T_TP || !*T_TP) ^ (!T_TI || !*T_TI))
		{
			debug2("cap :me=%s:mr=%s: ignored\n", T_TP, T_TI);
			T_TP = T_TI = NULL;
		}
		if ((!T_CI || !*T_CI) ^ (!T_CV || !*T_CV))
		{
			debug2("cap :vi=%s:ve=%s: ignored\n", T_CI, T_CV);
			T_CI = T_CV = NULL;
		}
	}
}

/*
 * inchar() - get a character from
 *		1. a scriptfile
 *		2. the keyboard - take care of interpreting escape sequences
 *
 *	If we got an interrupt all input is read until none is available.
 *  If async is TRUE there is no waiting for the char and we keep the
 *   character.
 *  If waitforever is FALSE we wait for 1 second for a character to arrive.
 */

#define INBUFLEN 50		/* buffer lengt, must be enough to contain an
							Amiga raw keycode report */

	int
inchar(async, waitforever)
	int async;
	int waitforever;
{
	static u_char	buf[INBUFLEN+1];	/* already entered characters */
	static int		len = 0;			/* number of valid chars in buf */
	static int		lastc = -1;			/* last character from script file */
	register int 	c;
	register int 	slen = 0;
	register char 	**p;
#ifdef AMIGA
			 char	*s;
#endif /* AMIGA */
			 int	first;
			 int	mode_deleted = FALSE;

	if (!async)
		flushbuf();

/*
 * Check for interrupt.
 */
	breakcheck();

/*
 * first try script file
 *	If interrupted: Stop reading script files.
 */
retry:
	if (scriptin[curscript] != NULL)
	{
		if (!got_int && lastc >= 0)
			c = lastc;
		else if (got_int || (c = getc(scriptin[curscript])) < 0)	/* reached EOF */
		{
			lastc = -1;
			fclose(scriptin[curscript]);
			scriptin[curscript] = NULL;
			if (curscript > 0)
				--curscript;
				/* recovery may be delayed till after reading a script file */
			if (recoverymode)
				openrecover();
			goto retry;		/* may read other script if this one was nested */
		}
		if (c == 0)
			c = K_ZERO;		/* replace ^@ with special code */
		if (async)
			lastc = c;
		else
			lastc = -1;
		return c;
	}

/*
 * If we got an interrupt, skip all previously typed characters and
 * reset interrupt flag.
 */
	if (got_int)		/* skip typed characters */
	{
		while (GetChars((char *)buf, INBUFLEN, T_PEEK))
			;
		got_int = FALSE;
		len = 0;
	}
/*
 * get character(s) from the keyboard
 * this is repeated until we have found that
 * - it is not an escape sequence, return the first char
 * - it is the start of an escape sequence, but it is not completed within
 *   a short time, return the first char
 * - it is an escape seqence, translate it into a single byte code
 */
	first = TRUE;
	for (;;)
	{
		if (!first || len == 0)
		{
			/*
			 * special case: if we get an <ESC> in insert mode and there are
			 * no more characters at once, we pretend to go out of insert mode.
			 * This prevents the one second delay after typing an <ESC>.
			 * If we get something after all, we may have to redisplay the
			 * mode. That the cursor is in the wrong place does not matter.
			 */
			if (!async && len == 1 && buf[0] == ESC && (State == INSERT || State == REPLACE) && (p_timeout || p_ttimeout))
			{
				slen = GetChars((char *)buf + len, INBUFLEN - len, T_PEEK);
				if (slen != 0)
				{
					len += slen;
					goto gotchar;
				}
				if (p_mo)
				{
					delmode();
					mode_deleted = TRUE;
				}
				if (Curscol)		/* move cursor one left if possible */
					--Curscol;
				else if (Curpos.col != 0 && Cursrow)
				{
						--Cursrow;
						Curscol = Columns - 1;
				}
				setcursor();
				flushbuf();
			}
			/*
			 * When async == TRUE do not wait for characters to arrive: T_PEEK
			 * When getting the first character wait until one arrives: T_BLOCK
			 * When waiting for a key sequence to complete
			 *		and 'timeout' or !waitforever set wait a short time: T_WAIT
			 *		and 'timeout' not set wait until one arrives: T_BLOCK
			 */
			slen = GetChars((char *)buf + len, INBUFLEN - len,
						async ? T_PEEK : ((len == 0 || !(p_timeout || p_ttimeout))
									&& waitforever) ? T_BLOCK : T_WAIT);
			if (got_int)
				goto retry;		/* CTRL-C typed, flush input buffer */
			len += slen;
			if ((async || !waitforever) && len == 0)	/* nothing typed */
				return NUL;
			if (slen == 0)			/* escape sequence not completed in time */
				break;
		}
		first = FALSE;

		if (async)		/* when we get here: len != 0 */
			return (int)buf[0];

gotchar:
		if (State != NOMAPPING)
		{
			/* search in the escape sequences for the characters we have */
			for (p = (char **)&term_strings.t_ku; p != (char **)&term_strings.t_undo + 1; ++p)
			{
				if (*p == NULL)
					continue;
				slen = strlen(*p);
				if (strncmp(*p, (char *)buf, (size_t)(slen > len ? len : slen)) == 0)
					break;
			}
		}
		if (State == NOMAPPING || p == (char **)&term_strings.t_undo + 1)	/* no match */
		{
#ifdef AMIGA		/* check for window bounds report */
			buf[len] = 0;
			if ((buf[0] & 0xff) == CSI && ((s = strchr((char *)buf, 'r')) || (s = strchr((char *)buf, '|'))))
			{
				slen = s - buf + 1;
				p = NULL;
			}
			else
#endif
				break;
		}
		if (slen <= len)		/* got the complete sequence */
		{
			if (mode_deleted)
				showmode();
			len -= slen;
			memmove((char *)buf, (char *)buf + slen, (size_t)len);
#ifdef AMIGA
			if (p == NULL)		/* got window bounds report */
			{
				set_winsize(0, 0, FALSE);      /* get size and redraw screen */
				continue;
			}
#endif

				/* this relies on the Key numbers to be consecutive! */
			return (int)(K_UARROW + (p - (char **)&term_strings.t_ku));
		}
		/*
		 * get here when we got an incomplete sequence,
		 * we need some more characters
		 */
	}
	c = buf[0];
	memmove((char *)buf, (char *)buf+1, (size_t)--len);
	if (c == 0)
		c = K_ZERO;		/* replace ^@ with special code */
		/* delete "INSERT" message if we return an ESC */
	if (c == ESC && p_mo && !mode_deleted && (State == INSERT || State == REPLACE))
		delmode();
	return c;
}

/*
 * outnum - output a (big) number fast
 */
	void
outnum(n)
	register long n;
{
	outstrn(tltoa((unsigned long)n));
}
 
	void
check_winsize()
{
	if (Columns < 5)
		Columns = 5;
	else if (Columns > MAX_COLUMNS)
		Columns = MAX_COLUMNS;
	if (Rows < 2)
		Rows = 2;
	p_scroll = Rows >> 1;
}

/*
 * set window size
 * If 'mustset' is TRUE, we must set Rows and Columns, do not get real
 * window size (this is used for the :win command during recovery).
 * If 'mustset' is FALSE, we may try to get the real window size and if
 * it fails use 'width' and 'height'.
 */
	void
set_winsize(width, height, mustset)
	int		width, height;
	int		mustset;
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
#ifdef AMIGA
	flushbuf(); 		/* must do this before mch_get_winsize for some obscure reason */
#endif /* AMIGA */
	if (mustset || mch_get_winsize())
	{
		debug("mch_get_win failed\n");
		Rows = height;
		Columns = width;
		mch_set_winsize();
	}
	check_winsize();		/* always check, to get p_scroll right */
	if (State == HELP)
		redrawhelp();
	else if (!starting)
	{
		tmp = RedrawingDisabled;
		RedrawingDisabled = FALSE;
		comp_Botline();
		updateScreen(CURSUPD);
		RedrawingDisabled = tmp;
		if (State == CMDLINE)
			redrawcmdline();
		else
			setcursor();
	}
	flushbuf();
}

	void
settmode(raw)
	int	 raw;
{
	static int		oldraw = FALSE;

	if (oldraw == raw)		/* skip if already in desired mode */
		return;
	oldraw = raw;

	mch_settmode(raw);	/* machine specific function */
}

	void
starttermcap()
{
	outstr(T_KS);	/* start "keypad transmit" mode */
	outstr(T_TS);	/* start termcap mode */
}

	void
stoptermcap()
{
	outstr(T_KE);	/* stop "keypad transmit" mode */
	outstr(T_TE);	/* stop termcap mode */
}
