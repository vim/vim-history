/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */
/*
 *
 * term.c: functions for controlling the terminal
 *
 * primitive termcap support for Amiga and MSDOS included
 *
 * NOTE: padding and variable substitution is not performed,
 * when compiling without TERMCAP, we use tputs() and tgoto() dummies.
 */

#include "vim.h"
#include "globals.h"
#include "param.h"
#include "proto.h"
#ifdef TERMCAP
# ifdef linux
#  include <termcap.h>
#  if 0		/* only required for old versions, it's now in termcap.h */
    typedef int (*outfuntype) (int);
#  endif
#  define TPUTSFUNCAST (outfuntype)
# else
#  define TPUTSFUNCAST
#  ifdef AMIGA
#   include "proto/termlib.pro"
#  endif
# endif
#endif

static void parse_builtin_tcap __ARGS((Tcarr *tc, char_u *s));

/*
 * Builtin_tcaps must always contain DFLT_TCAP as the first entry!
 * DFLT_TCAP is used, when no terminal is specified with -T option or $TERM.
 * The entries are compact, therefore they normally are included even when
 * TERMCAP is defined.
 * When TERMCAP is defined, the builtin entries can be accessed with
 * "builtin_amiga", "builtin_ansi", "builtin_debug", etc.
 */
static char_u *builtin_tcaps[] =
{
#ifndef NO_BUILTIN_TCAPS
  (char_u *)DFLT_TCAP,		/* almost allways included */
# if !defined(UNIX) && (defined(ALL_BUILTIN_TCAPS) || defined(SOME_BUILTIN_TCAPS))
  (char_u *)ANSI_TCAP,		/* default for unix */
# endif
# if !defined(AMIGA) && (defined(ALL_BUILTIN_TCAPS) || defined(SOME_BUILTIN_TCAPS))
  (char_u *)AMIGA_TCAP,		/* default for amiga */
# endif
# if !defined(MSDOS) && (defined(ALL_BUILTIN_TCAPS) || defined(SOME_BUILTIN_TCAPS))
  (char_u *)PCTERM_TCAP,		/* default for MSdos */
# endif
# if defined(MSDOS) || defined(ALL_BUILTIN_TCAPS)
  (char_u *)PCANSI_TCAP,
# endif
# if !defined(ATARI) && defined(ALL_BUILTIN_TCAPS)
  (char_u *)ATARI_TCAP,		/* default for Atari */
# endif
# if defined(UNIX) || defined(ALL_BUILTIN_TCAPS) || defined(SOME_BUILTIN_TCAPS)
  (char_u *)XTERM_TCAP,		/* always included on unix */
# endif
# ifdef ALL_BUILTIN_TCAPS
  (char_u *)VT52_TCAP,
# endif
# if defined(DEBUG) || defined(ALL_BUILTIN_TCAPS)
  (char_u *)DEBUG_TCAP,		/* always included when debugging */
# endif
#else /* NO_BUILTIN_TCAPS */
  (char_u *)DUMB_TCAP,		/* minimal termcap, used when everything else fails */
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
	char_u *s;
{
	char_u **p = &tc->t_name;

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
int				tgetflag();
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

#ifdef linux
# define TGETSTR(s, p)	(char_u *)tgetstr((s), (char **)(p))
#else
# define TGETSTR(s, p)	(char_u *)tgetstr((s), (char *)(p))
#endif

	void
set_term(term)
	char_u *term;
{
	char_u **p = builtin_tcaps;
#ifdef TERMCAP
	int builtin = 0;
#endif
	int width = 0, height = 0;

	if (!STRNCMP(term, "builtin_", (size_t)8))
	{
		term += 8;
#ifdef TERMCAP
		builtin = 1;
#endif
	}
#ifdef TERMCAP
	else
	{
		char_u			*p;
		static char_u	tstrbuf[TBUFSZ];
		char_u			tbuf[TBUFSZ];
		char_u			*tp = tstrbuf;
		int				i;

		i = tgetent(tbuf, term);
		if (i == -1)
		{
			EMSG("Cannot open termcap file");
			builtin = 1;
		}
		else if (i == 0)
		{
			EMSG("terminal entry not found");
			builtin = 1;
		}
		else
		{
			clear_termparam();		/* clear old parameters */
		/* output strings */
			T_EL = TGETSTR("ce", &tp);
			T_IL = TGETSTR("al", &tp);
			T_CIL = TGETSTR("AL", &tp);
			T_DL = TGETSTR("dl", &tp);
			T_CDL = TGETSTR("DL", &tp);
			T_CS = TGETSTR("cs", &tp);
			T_ED = TGETSTR("cl", &tp);
			T_CI = TGETSTR("vi", &tp);
			T_CV = TGETSTR("ve", &tp);
			T_CVV = TGETSTR("vs", &tp);
			T_TP = TGETSTR("me", &tp);
			T_TI = TGETSTR("mr", &tp);
			T_TB = TGETSTR("md", &tp);
			T_SE = TGETSTR("se", &tp);
			T_SO = TGETSTR("so", &tp);
			T_CM = TGETSTR("cm", &tp);
			T_SR = TGETSTR("sr", &tp);
			T_CRI = TGETSTR("RI", &tp);
			T_VB = TGETSTR("vb", &tp);
			T_KS = TGETSTR("ks", &tp);
			T_KE = TGETSTR("ke", &tp);
			T_TS = TGETSTR("ti", &tp);
			T_TE = TGETSTR("te", &tp);

		/* key codes */
			term_strings.t_ku = TGETSTR("ku", &tp);
			term_strings.t_kd = TGETSTR("kd", &tp);
			term_strings.t_kl = TGETSTR("kl", &tp);
				/* if cursor-left == backspace, ignore it (televideo 925) */
			if (term_strings.t_kl != NULL && *term_strings.t_kl == Ctrl('H'))
				term_strings.t_kl = NULL;
			term_strings.t_kr = TGETSTR("kr", &tp);
			/* term_strings.t_sku = TGETSTR("", &tp); termcap code unknown */
			/* term_strings.t_skd = TGETSTR("", &tp); termcap code unknown */
#ifdef ARCHIE
            /* Termcap code made up! */
            term_strings.t_sku = tgetstr("su", &tp);
            term_strings.t_skd = tgetstr("sd", &tp);
#else
            term_strings.t_sku = NULL;
            term_strings.t_skd = NULL;
#endif
			term_strings.t_skl = TGETSTR("#4", &tp);
			term_strings.t_skr = TGETSTR("%i", &tp);
			term_strings.t_f1 = TGETSTR("k1", &tp);
			term_strings.t_f2 = TGETSTR("k2", &tp);
			term_strings.t_f3 = TGETSTR("k3", &tp);
			term_strings.t_f4 = TGETSTR("k4", &tp);
			term_strings.t_f5 = TGETSTR("k5", &tp);
			term_strings.t_f6 = TGETSTR("k6", &tp);
			term_strings.t_f7 = TGETSTR("k7", &tp);
			term_strings.t_f8 = TGETSTR("k8", &tp);
			term_strings.t_f9 = TGETSTR("k9", &tp);
			term_strings.t_f10 = TGETSTR("k;", &tp);
			term_strings.t_sf1 = TGETSTR("F1", &tp);	/* really function keys 11-20 */
			term_strings.t_sf2 = TGETSTR("F2", &tp);
			term_strings.t_sf3 = TGETSTR("F3", &tp);
			term_strings.t_sf4 = TGETSTR("F4", &tp);
			term_strings.t_sf5 = TGETSTR("F5", &tp);
			term_strings.t_sf6 = TGETSTR("F6", &tp);
			term_strings.t_sf7 = TGETSTR("F7", &tp);
			term_strings.t_sf8 = TGETSTR("F8", &tp);
			term_strings.t_sf9 = TGETSTR("F9", &tp);
			term_strings.t_sf10 = TGETSTR("FA", &tp);
			term_strings.t_help = TGETSTR("%1", &tp);
			term_strings.t_undo = TGETSTR("&8", &tp);

			height = tgetnum("li");
			width = tgetnum("co");

			T_MS = tgetflag("ms") ? (char_u *)"yes" : (char_u *)NULL;

# ifndef hpux
			BC = (char *)TGETSTR("bc", &tp);
			UP = (char *)TGETSTR("up", &tp);
			p = TGETSTR("pc", &tp);
			if (p)
				PC = *p;
			ospeed = 0;
# endif
		}
	}
	if (builtin)
#endif
	{
		while (*p && STRCMP(term, *p))
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
/*
 * special: There is no info in the termcap about whether the cursor positioning
 * is relative to the start of the screen or to the start of the scrolling region.
 * We just guess here. Only msdos pcterm is known to do it relative.
 */
	if (STRCMP(term, "pcterm") == 0)
		T_CSC = (char_u *)"yes";
	else
		T_CSC = NULL;

#if defined(AMIGA) || defined(MSDOS)
		/* DFLT_TCAP indicates that it is the machine console. */
	if (STRCMP(term, *builtin_tcaps))
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
	set_winsize(width, height, FALSE);	/* may change Rows */
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
	char_u			tbuf[TBUFSZ];

	if (term_strings.t_name != NULL && tgetent(tbuf, term_strings.t_name) > 0)
	{
		if (Columns == 0)
			Columns = tgetnum("co");
		if (Rows == 0)
			Rows = tgetnum("li");
	}
}
#endif

static char_u *tltoa __PARMS((unsigned long));

	static char_u *
tltoa(i)
	unsigned long i;
{
	static char_u buf[16];
	char_u		*p;

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
			p = (char *)tltoa((unsigned long)y);
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
	char_u *term;
{
	if (!term)
		term = vimgetenv((char_u *)"TERM");
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
static char_u			outbuf[BSIZE];
static int				bpos = 0;		/* number of chars in outbuf */

/*
 * flushbuf(): flush the output buffer
 */
	void
flushbuf()
{
	if (bpos != 0)
	{
		mch_write(outbuf, bpos);
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

	if (p_nb)			/* for testing: unbuffered screen output (not for MSDOS) */
		mch_write(outbuf, 1);
	else
		++bpos;

	if (bpos >= BSIZE)
		flushbuf();
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
	char_u *s;
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
	register char_u			 *s;
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
	OUTSTR(tgoto((char *)T_CM, col, row));
}

/*
 * Set cursor to current position.
 * Should be optimized for minimal terminal output.
 */

	void
setcursor()
{
	if (!RedrawingDisabled)
		windgoto(curwin->w_winpos + curwin->w_row, curwin->w_col);
}

	void
ttest(pairs)
	int	pairs;
{
	char buf[70];
	char *s = "terminal capability %s required.\n";
	char *t = NULL;

  /* hard requirements */
	if (!T_ED || !*T_ED)	/* erase display */
		t = "cl";
	if (!T_CM || !*T_CM)	/* cursor motion */
		t = "cm";

	if (t)
    {
    	sprintf(buf, s, t);
    	EMSG(buf);
    }

/*
 * if "cs" defined, use a scroll region, it's faster.
 */
	if (T_CS && *T_CS != NUL)
		scroll_region = TRUE;
	else
		scroll_region = FALSE;

	if (pairs)
	{
	  /* optional pairs */
			/* TP goes to normal mode for TI (invert) and TB (bold) */
		if ((!T_TP || !*T_TP))
			T_TP = T_TI = T_TB = NULL;
		if ((!T_SO || !*T_SO) ^ (!T_SE || !*T_SE))
			T_SO = T_SE = NULL;
			/* T_CV is needed even though T_CI is not defined */
		if ((!T_CV || !*T_CV))
			T_CI = NULL;
			/* if 'mr' or 'me' is not defined use 'so' and 'se' */
		if (T_TP == NULL || *T_TP == NUL)
		{
			T_TP = T_SE;
			T_TI = T_SO;
			T_TB = T_SO;
		}
			/* if 'so' or 'se' is not defined use 'mr' and 'me' */
		if (T_SO == NULL || *T_SO == NUL)
		{
			T_SE = T_TP;
			if (T_TI == NULL)
				T_SO = T_TB;
			else
				T_SO = T_TI;
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
	char_u	*buf;
	int		maxlen;
	int		time;						/* milli seconds */
{
	int				len;
	int				retesc = FALSE;		/* return ESC with gotint */
	register int 	c;
	register int	i;

	if (time == -1 || time > 100)	/* flush output before waiting */
	{
		cursor_on();
		flushbuf();
	}
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
			*(char_u *)buf = K_ZERO;		/* replace ^@ with special code */
	*buf = NUL;								/* add trailing NUL */
	return len;
}

/*
 * Check if buf[] begins with a terminal key code.
 * Return 0 for no match, -1 for partial match, > 0 for full match.
 * With a match the replacement code is put in buf[0], the match is
 * removed and the number characters in buf is returned.
 *
 * Note: should always be called with buf == typestr!
 */
	int
check_termcode(buf)
	char_u	*buf;
{
	char_u 	**p;
	int		slen;
	int		len;

	len = STRLEN(buf);
	for (p = (char_u **)&term_strings.t_ku; p != (char_u **)&term_strings.t_undo + 1; ++p)
	{
		if (*p == NULL || (slen = STRLEN(*p)) == 0)		/* empty entry */
			continue;
		if (STRNCMP(*p, buf, (size_t)(slen > len ? len : slen)) == 0)
		{
			if (len >= slen)		/* got the complete sequence */
			{
				len -= slen;
					/* remove matched chars, taking care of noremap */
				del_typestr(slen - 1);
					/* this relies on the Key numbers to be consecutive! */
				buf[0] = K_UARROW + (p - (char_u **)&term_strings.t_ku);
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
	OUTSTRN(tltoa((unsigned long)n));
}
 
	void
check_winsize()
{
	if (Columns < MIN_COLUMNS)
		Columns = MIN_COLUMNS;
	else if (Columns > MAX_COLUMNS)
		Columns = MAX_COLUMNS;
	if (Rows < MIN_ROWS + 1)	/* need room for one window and command line */
		Rows = MIN_ROWS + 1;
	screen_new_rows();			/* may need to update window sizes */
}

/*
 * set window size
 * If 'mustset' is TRUE, we must set Rows and Columns, do not get real
 * window size (this is used for the :win command).
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
	if (mustset || mch_get_winsize() == FAIL)
	{
		Rows = height;
		Columns = width;
		check_winsize();		/* always check, to get p_scroll right */
		mch_set_winsize();
	}
	else
		check_winsize();		/* always check, to get p_scroll right */
	if (State == HELP)
		(void)redrawhelp();
	else if (!starting)
	{
		tmp = RedrawingDisabled;
		RedrawingDisabled = FALSE;
		comp_Botline_all();
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
	outstr(T_TS);	/* start termcap mode */
	outstr(T_KS);	/* start "keypad transmit" mode */
	flushbuf();
	termcap_active = TRUE;
}

	void
stoptermcap()
{
	outstr(T_KE);	/* stop "keypad transmit" mode */
	flushbuf();
	termcap_active = FALSE;
	cursor_on();	/* just in case it is still off */
	outstr(T_TE);	/* stop termcap mode */
}

/*
 * enable cursor, unless in Visual mode or no inversion possible
 */
static int cursor_is_off = FALSE;

	void
cursor_on()
{
	if (cursor_is_off && (!VIsual.lnum || highlight == NULL))
	{
		outstr(T_CV);
		cursor_is_off = FALSE;
	}
}

	void
cursor_off()
{
	if (!cursor_is_off)
		outstr(T_CI);			/* disable cursor */
	cursor_is_off = TRUE;
}

/*
 * set scrolling region for window 'wp'
 */
	void
scroll_region_set(wp)
	WIN		*wp;
{
	OUTSTR(tgoto((char *)T_CS, wp->w_winpos + wp->w_height - 1, wp->w_winpos));
}

/*
 * reset scrolling region to the whole screen
 */
	void
scroll_region_reset()
{
	OUTSTR(tgoto((char *)T_CS, (int)Rows - 1, 0));
}
