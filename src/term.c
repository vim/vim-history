/* vi:sw=4:ts=4:
 *
 * term.c -- VIM - Vi IMproved
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
# if defined(MSDOS) || defined(ALL_BUILTIN_TCAPS)
  PCANSI_TCAP,
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
  DUMB_TCAP,		/* minimal termcap, used when everything else fails */
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
char		*UP, *BC, PC;		/* should be extern, but some don't have them */
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
		char			*p;
		static char		tstrbuf[TBUFSZ];
		char			tbuf[TBUFSZ];
		char			*tp = tstrbuf;
		int				i;

		i = tgetent(tbuf, term);
		if (i == -1)
		{
			emsg("Cannot open termcap file");
			builtin = 1;
		}
		else if (i == 0)
		{
			emsg("terminal entry not found");
			builtin = 1;
		}
		else
		{
			clear_termparam();		/* clear old parameters */
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
				/* if 'mr' or 'me' is not defined use 'so' and 'se' */
			if (T_TP == NULL || *T_TP == NUL || T_TI == NULL || *T_TI == NUL)
			{
				T_TP = tgetstr("se", &tp);
				T_TI = tgetstr("so", &tp);
			}
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
				/* if cursor-left == backspace, ignore it (televideo 925) */
			if (term_strings.t_kl != NULL && *term_strings.t_kl == Ctrl('H'))
				term_strings.t_kl = NULL;
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
		}
	}
	if (builtin)
#endif
	{
		while (*p && strcmp(term, *p))
			p++;
		if (!*p)
		{
			fprintf(stderr, "'%s' not builtin. Available terminals are:\r\n", term);
			for (p = builtin_tcaps; *p; p++)
#ifdef TERMCAP
				fprintf(stderr, "\tbuiltin_%s\r\n", *p);
#else
				fprintf(stderr, "\t%s\r\n", *p);
#endif
			if (!starting)		/* when user typed :set term=xxx, quit here */
			{
				wait_return(TRUE);
				return;
			}
			sleep(2);
			fprintf(stderr, "defaulting to '%s'\r\n", *builtin_tcaps);
			sleep(2);
			p = builtin_tcaps;
			free(term_strings.t_name);
			term_strings.t_name = strsave(term = *p);
		}
		clear_termparam();		/* clear old parameters */
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
#ifdef MSDOS
		height = 25;		/* console is often 25 lines */
#else
		height = 24;		/* most terminals are 24 lines */
#endif
	}
	Rows_max = Rows;		/* remember max. physical nr. of Rows */
	set_winsize(width, height, FALSE);	/* may change Rows_max */
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
 * the number of calls to mch_write is reduced by using the buffer "outbuf"
 */
#undef BSIZE			/* hpux has BSIZE in sys/param.h */
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
		mch_write((char *)outbuf, bpos);
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
#ifdef UNIX
	if (c == '\n')		/* turn LF into CR-LF (CRMOD does not seem to do this) */
		outchar('\r');
#endif
	outbuf[bpos] = c;
	++bpos;
	if (bpos >= BSIZE)
		flushbuf();
	if (c == '\n')
		char_count += Columns;
	else
		++char_count;
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
	if (bpos > BSIZE - 20)		/* avoid terminal strings being split up */
		flushbuf();
	while (*s)
		outchar(*s++);
}

/*
 * outstr(s): put a string character at a time into the output buffer.
 * If TERMCAP is defined use the termcap parser. (jw)
 */
	void
outstr(s)
	register char			 *s;
{
	if (bpos > BSIZE - 20)		/* avoid terminal strings being split up */
		flushbuf();
	if (s)
#ifdef TERMCAP
		tputs(s, 1, TPUTSFUNCAST outchar);
#else
		while (*s)
			outchar(*s++);
#endif
}

/* 
 * cursor positioning using termcap parser. (jw)
 */
	void
windgoto(row, col)
	int		row;
	int		col;
{
	outstr(tgoto(T_CM, col, row));
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
 * inchar() - get one character from
 *		1. a scriptfile
 *		2. the keyboard
 *
 *  As much characters as we can get (upto 'maxlen') are put in buf and
 *  NUL terminated (buffer length must be 'maxlen' + 1).
 *
 *	If we got an interrupt all input is read until none is available.
 *
 *  If time == 0  there is no waiting for the char.
 *  If time == n  we wait for n msec for a character to arrive.
 *  If time == -1 we wait forever for a character to arrive.
 *
 *  Return the number of obtained characters.
 */

	int
inchar(buf, maxlen, time)
	char	*buf;
	int		maxlen;
	int		time;						/* milli seconds */
{
	int				len;
	int				retesc = FALSE;		/* return ESC with gotint */
	register int 	c;
	register int	i;

	if (time == -1)				/* flush output before blocking */
		flushbuf();
	did_outofmem_msg = FALSE;	/* display out of memory message (again) */

/*
 * first try script file
 *	If interrupted: Stop reading script files.
 */
retry:
	if (scriptin[curscript] != NULL)
	{
		if (got_int || (c = getc(scriptin[curscript])) < 0)	/* reached EOF */
		{
				/* when reading script file is interrupted, return an ESC to
									get back to normal mode */
			if (got_int)
				retesc = TRUE;
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
		*buf++ = c;
		*buf = NUL;
		return 1;
	}

/*
 * If we got an interrupt, skip all previously typed characters and
 * return TRUE if quit reading script file.
 */
	if (got_int)			/* skip typed characters */
	{
		while (GetChars(buf, maxlen, T_PEEK))
			;
		return retesc;
	}
	len = GetChars(buf, maxlen, time);

	for (i = len; --i >= 0; ++buf)
		if (*buf == 0)
			*(u_char *)buf = K_ZERO;		/* replace ^@ with special code */
	*buf = NUL;								/* add trailing NUL */
	return len;
}

/*
 * Check if buf[] begins with a terminal key code.
 * Return 0 for no match, -1 for partial match, > 0 for full match.
 * With a match the replacement code is put in buf[0], the match is
 * removed and the number characters in buf is returned.
 */
	int
check_termcode(buf)
	char	*buf;
{
	char 	**p;
	int		slen;
	int		len;

	len = strlen(buf);
	for (p = (char **)&term_strings.t_ku; p != (char **)&term_strings.t_undo + 1; ++p)
	{
		if (*p == NULL || (slen = strlen(*p)) == 0)		/* empty entry */
			continue;
		if (strncmp(*p, buf, (size_t)(slen > len ? len : slen)) == 0)
		{
			if (len >= slen)		/* got the complete sequence */
			{
				len -= slen;
				memmove(buf + 1, buf + slen, (size_t)(len + 1));
					/* this relies on the Key numbers to be consecutive! */
				buf[0] = K_UARROW + (p - (char **)&term_strings.t_ku);
				return (len + 1);
			}
			return -1;				/* got a partial sequence */
		}
	}
	return 0;						/* no match found */
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
 
/*
 * outnuml - output a (big) number fast and return the number of characters
 */
	int
outnuml(n)
	register long n;
{
	char *s;

	s = tltoa((unsigned long)n);
	outstrn(s);
	return (int)strlen(s);
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

	if (State == HITRETURN || State == SETWSIZE)	/* postpone the resizing */
	{
		State = SETWSIZE;
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

/*
 * set active window height (for "z<number><CR>" command)
 */
	void
set_winheight(height)
	int		height;
{
	if (height > Rows_max)			/* can't make it larger */
		height = Rows_max;
	Rows = height;
	check_winsize();
	updateScreen(CLEAR);
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
	flushbuf();
	termcap_active = TRUE;
}

	void
stoptermcap()
{
	outstr(T_KE);	/* stop "keypad transmit" mode */
	outstr(T_TE);	/* stop termcap mode */
	flushbuf();
	termcap_active = FALSE;
}

/*
 * enable cursor, unless in Visual mode or no inversion possible
 */
	void
cursor_on()
{
	if (!Visual.lnum || T_TI == NULL || *T_TI == NUL)
		outstr(T_CV);
}

	void
cursor_off()
{
	outstr(T_CI);			/* disable cursor */
}
