/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */
/*
 * if_perl.xs: Main code for Perl interface support.
 *		Mostly written by Sven Verdoolaege.
 */

#define _memory_h	/* avoid memset redeclaration */
#define IN_PERL_FILE	/* don't include if_perl.pro from prot.h */

#include "vim.h"
#include "globals.h"
#include "proto.h"

/*
 * Avoid clashes between Perl and Vim namespace.
 */
#undef MAGIC
#undef NORMAL
#undef STRLEN
#undef FF
#undef OP_DELETE

#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>

/*
 * Work around clashes between Perl and Vim namespace.	proto.h doesn't
 * include if_perl.pro and perlsfio.pro when IN_PERL_FILE is defined, because
 * we need the CV typedef.  proto.h can't be moved to after including
 * if_perl.h, because we get all sorts of name clashes then.
 */
#ifndef PROTO
# include "proto/if_perl.pro"
# include "proto/if_perlsfio.pro"
#endif

static void *perl_interp = NULL;
static void xs_init __ARGS((void));

/*
 * perl_init(): initialize perl interpreter
 * We have to call perl_parse to initialize some structures,
 * there's nothing to actually parse.
 */
    static void
perl_init()
{
    char	*bootargs[] = { "VI", NULL };
    static char *args[] = { "", "-e", "" };

    perl_interp = perl_alloc();
    perl_construct(perl_interp);
    perl_parse(perl_interp, xs_init, 3, args, 0);
    perl_call_argv("VIM::bootstrap", G_DISCARD, bootargs);
#ifdef USE_SFIO
    sfdisc(PerlIO_stdout(), sfdcnewvim());
    sfdisc(PerlIO_stderr(), sfdcnewvim());
    sfsetbuf(PerlIO_stdout(), NULL, 0);
    sfsetbuf(PerlIO_stderr(), NULL, 0);
#endif
}

/*
 * perl_end(): clean up after ourselves
 */
    void
perl_end()
{
    if (perl_interp)
    {
	perl_run(perl_interp);
	perl_destruct(perl_interp);
	perl_free(perl_interp);
    }
}

/*
 * perl_eval(): evaluate a string
 * We don't use mortal SVs because no one will clean up after us.
 */
    static void
perl_eval(string)
    char *string;
{
    SV* sv = newSVpv(string, 0);

    perl_eval_sv(sv, G_DISCARD | G_NOARGS);
    SvREFCNT_dec(sv);
}

/*
 * msg_split(): send a message to the message handling routines
 * split at '\n' first though.
 */
    void
msg_split(s)
    char_u  *s;
{
    char *next;
    char *token = (char *)s;

    while ((next = strchr(token, '\n')))
    {
	*next++ = '\0';			/* replace \n with \0 */
	MSG(token);
	token = next;
    }
    if (*token)
	MSG(token);
}

#ifndef WANT_EVAL
/*
 * This stub is needed because an "#ifdef WANT_EVAL" around Eval() doesn't work properly.
 */
    char_u *
eval_to_string(arg, nextcmd)
    char_u	*arg;
    char_u	**nextcmd;
{
    return NULL;
}
#endif

/*
 * Create a new reference to an SV pointing to the SCR structure
 * The perl_private part of the SCR structure points to the SV,
 * so there can only be one such SV for a particular SCR structure.
 * When the last reference has gone (DESTROY is called),
 * perl_private is reset; When the screen goes away before
 * all references are gone, the value of the SV is reset;
 * any subsequent use of any of those reference will produce
 * a warning. (see typemap)
 */
#define newANYrv(TYPE)						\
static SV *							\
new ## TYPE ## rv(rv, ptr)					\
    SV *rv;							\
    TYPE *ptr;							\
{								\
    sv_upgrade(rv, SVt_RV);					\
    if (!ptr->perl_private)					\
    {								\
	ptr->perl_private = newSV(0);				\
	sv_setiv(ptr->perl_private, (IV)ptr);			\
    }								\
    else							\
	SvREFCNT_inc(ptr->perl_private);			\
    SvRV(rv) = ptr->perl_private;				\
    SvROK_on(rv);						\
    return sv_bless(rv, gv_stashpv("VI" #TYPE, TRUE));		\
}

newANYrv(WIN)
newANYrv(BUF)

/*
 * perl_win_free
 *	Remove all refences to the window to be destroyed
 */
    void
perl_win_free(wp)
    WIN *wp;
{
    if (wp->perl_private)
	sv_setiv((SV *)wp->perl_private, 0);
    return;
}

    void
perl_buf_free(bp)
    BUF *bp;
{
    if (bp->perl_private)
	sv_setiv((SV *)bp->perl_private, 0);
    return;
}

    int
do_perl(eap)
    EXARG	*eap;
{
    static SV	*svcurwin = 0;
    char	*err;
    STRLEN	length;

    if (!svcurwin)
    {
	if (!perl_interp)
	{
	    perl_init();
	}
	SvREADONLY_on(svcurwin = perl_get_sv("curwin", TRUE));
    }

    newWINrv(svcurwin, curwin);

    perl_eval(eap->arg);
    err = SvPV(GvSV(errgv), length);

    SvREFCNT_dec(SvRV(svcurwin));
    SvROK_off(svcurwin);

    if (!length)
	return OK;

    msg_split((char_u *)err);
    return FAIL;
}

    static int
replace_line(line, end)
    linenr_t	*line, *end;
{
    char *str;

    if (SvOK(GvSV(defgv)))
    {
	str = SvPV(GvSV(defgv),na);
	ml_replace(*line, (char_u *)str, 1);
    }
    else
    {
	mark_adjust(*line, *line, MAXLNUM, -1);
	ml_delete((*line)--, FALSE);
	(*end)--;
    }
    return OK;
}

    int
do_perldo(eap)
    EXARG	*eap;
{
    STRLEN	length;
    SV		*sv;
    char	*str;
    linenr_t	i;
    dSP;

    if (bufempty())
	return FAIL;

    if (!perl_interp)
    {
	perl_init();
	SPAGAIN;
    }
    length = strlen((char *)eap->arg);
    sv = newSV(length + sizeof("sub VIM::perldo {")-1 + 1);
    sv_setpvn(sv, "sub VIM::perldo {", sizeof("sub VIM::perldo {")-1);
    sv_catpvn(sv, (char *)eap->arg, length);
    sv_catpvn(sv, "}", 1);
    perl_eval_sv(sv, G_DISCARD | G_NOARGS);
    SvREFCNT_dec(sv);
    str = SvPV(GvSV(errgv), length);
    if (length)
	goto err;

    if (u_save(eap->line1 - 1, eap->line2 + 1) != OK)
	return FAIL;

    ENTER;
    SAVETMPS;
    for (i = eap->line1; i <= eap->line2; i++)
    {
	sv_setpv(GvSV(defgv),(char *)ml_get(i));
	PUSHMARK(sp);
	perl_call_pv("VIM::perldo", G_SCALAR | G_EVAL);
	str = SvPV(GvSV(errgv), length);
	if (length)
	    break;
	SPAGAIN;
	if (SvTRUEx(POPs))
	{
	    if (replace_line(&i, &eap->line2) != OK)
	    {
		PUTBACK;
		break;
	    }
	}
	PUTBACK;
    }
    FREETMPS;
    LEAVE;
    adjust_cursor();
    update_screen(NOT_VALID);
    if (!length)
	return OK;

err:
    msg_split((char_u *)str);
    return FAIL;
}

/* Register any extra external extensions */

extern void boot_DynaLoader _((CV* cv));
extern void boot_VIM _((CV* cv));

    static void
xs_init()
{
#ifndef UNIX
    dXSUB_SYS;	    /* causes an error with Perl 5.003_97 */
#endif
    char *file = __FILE__;

    newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
    newXS("VIM::bootstrap", boot_VIM, file);
}

typedef WIN *	VIWIN;
typedef BUF *	VIBUF;

MODULE = VIM	    PACKAGE = VIM

void
Msg(text)
    char    *text

    CODE:
    if (text != NULL)
	msg_split((char_u *)text);

void
SetOption(line)
    char *line;

    PPCODE:
    if (line != NULL)
	do_set((char_u *)line);
    update_screen(NOT_VALID);

void
DoCommand(line)
    char *line;

    PPCODE:
    if (line != NULL)
	do_cmdline((char_u *)line, NULL, NULL, DOCMD_VERBOSE + DOCMD_NOWAIT);

void
Eval(str)
    char *str;

    PREINIT:
	char_u *value;
    PPCODE:
	value = eval_to_string((char_u *)str, (char_u**)0);
	if (value==NULL)
	{
	    XPUSHs(sv_2mortal(newSViv(0)));
	    XPUSHs(sv_2mortal(newSVpv("", 0)));
	}
	else
	{
	    XPUSHs(sv_2mortal(newSViv(1)));
	    XPUSHs(sv_2mortal(newSVpv((char *)value, 0)));
	    free(value);
	}

void
Buffers(...)

    PREINIT:
    BUF *vimbuf;
    int i, b;
    char text[50];

    PPCODE:
    if(items == 0)
    {
	if(GIMME == G_SCALAR)
	{
	    XPUSHs(sv_2mortal(newSViv(buflist_maxbufnr())));
	}
	else
	{
	  for(i=0; i<=buflist_maxbufnr(); i++)
	  {
	    vimbuf = buflist_findnr(i);
	    if(vimbuf)
	    {
		XPUSHs(newBUFrv(newSV(0), vimbuf));
	    }
	  }
	}
    }
    else
    {
	for(i=0; i<items; i++)
	{
	    sprintf(text, "calling args");
	    msg_split((char_u *)text);
	    b = SvIV(ST(i));
	    vimbuf = buflist_findnr(b);
	    if(vimbuf)
	    {
		XPUSHs(newBUFrv(newSV(0), vimbuf));
	    }
	}
    }

void
Windows(...)

    PREINIT:
    WIN *vimwin;
    int i, b;
    char text[50];

    PPCODE:
    if(items == 0)
    {
	if(GIMME == G_SCALAR)
	{
	    XPUSHs(sv_2mortal(newSViv(win_count())));
	}
	else
	{
	    for(i=0; i<=buflist_maxbufnr(); i++)
	    {
	      vimwin = win_goto_nr(i);
	      if(vimwin)
	      {
		  XPUSHs(newBUFrv(newSV(0), vimwin));
	      }
	    }
	}
    }
    else
    {
	for(i=0; i<items; i++)
	{
	    sprintf(text, "calling args");
	    msg_split((char_u *)text);
	    b = SvIV(ST(i));
	    vimwin = win_goto_nr(b);
	    if(vimwin)
	    {
		XPUSHs(newBUFrv(newSV(0), vimwin));
	    }
	}
    }

MODULE = VIM	    PACKAGE = VIWIN

void
DESTROY(win)
    VIWIN win

    CODE:
    if (win_valid(win))
	win->perl_private = 0;

SV *
Buffer(win)
    VIWIN win

    CODE:
    if (!win_valid(win))
	win = curwin;
    RETVAL = newBUFrv(newSV(0), win->w_buffer);
    OUTPUT:
    RETVAL

void
SetHeight(win, height)
    VIWIN win
    int height;

    PREINIT:
    WIN *savewin;

    PPCODE:
    if (!win_valid(win))
	win = curwin;
    savewin = curwin;
    curwin = win;
    win_setheight(height);
    curwin = savewin;

void
Cursor(win, ...)
    VIWIN win

    PPCODE:
    if(items == 1)
    {
      EXTEND(sp, 2);
      if (!win_valid(win))
	  win = curwin;
      PUSHs(sv_2mortal(newSViv(win->w_cursor.lnum)));
      PUSHs(sv_2mortal(newSViv(win->w_cursor.col)));
    }
    else if(items == 3)
    {
      int lnum, col;

      if (!win_valid(win))
	  win = curwin;
      lnum = SvIV(ST(1));
      col = SvIV(ST(2));
      win->w_cursor.lnum = lnum;
      win->w_cursor.col = col;
      adjust_cursor();		    /* put cursor on an existing line */
      update_screen(NOT_VALID);
    }

MODULE = VIM	    PACKAGE = VIBUF

void
DESTROY(vimbuf)
    VIBUF vimbuf;

    CODE:
    if (buf_valid(vimbuf))
	vimbuf->perl_private = 0;

void
Name(vimbuf)
    VIBUF vimbuf;

    PPCODE:
    if (!buf_valid(vimbuf))
	vimbuf = curbuf;
    if (vimbuf->b_fname == NULL)
	XPUSHs(sv_2mortal(newSVpv("", 0)));
    else
	XPUSHs(sv_2mortal(newSVpv((char *)vimbuf->b_fname, 0)));

void
Count(vimbuf)
    VIBUF vimbuf;

    PPCODE:
    if (!buf_valid(vimbuf))
	vimbuf = curbuf;
    XPUSHs(sv_2mortal(newSViv(vimbuf->b_ml.ml_line_count)));

void
Get(vimbuf, ...)
    VIBUF vimbuf;

    PREINIT:
    char_u *line;
    int i;
    long lnum;
    PPCODE:
    if (buf_valid(vimbuf))
    {
	for(i=1; i<items; i++)
	{
	    lnum = SvIV(ST(i));
	    if (lnum > 0 && lnum <= vimbuf->b_ml.ml_line_count)
	    {
		line = ml_get_buf(vimbuf, lnum, FALSE);
		XPUSHs(sv_2mortal(newSVpv((char *)line, 0)));
	    }
	}
    }

void
Set(vimbuf, ...)
    VIBUF vimbuf;

    PREINIT:
    int i;
    long lnum;
    char *line;
    BUF *savebuf;
    PPCODE:
    if (buf_valid(vimbuf))
    {
	if (items < 3)
	    croak("Usage: VIBUF::Set(vimbuf, lnum, @lines)");

	lnum = SvIV(ST(1));
	for(i=2; i<items; i++, lnum++)
	{
	    line = SvPV(ST(i),na);
	    if(lnum > 0 && lnum <= vimbuf->b_ml.ml_line_count && line != NULL)
	    {
		savebuf = curbuf;
		curbuf = vimbuf;
		if (u_savesub(lnum) == OK)
		    ml_replace(lnum, (char_u *)line, TRUE);
		curbuf = savebuf;
		update_curbuf(NOT_VALID);
	    }
	}
    }

void
Delete(vimbuf, ...)
    VIBUF vimbuf;

    PREINIT:
    long i, lnum = 0, count = 0;
    BUF *savebuf;
    PPCODE:
    if (buf_valid(vimbuf))
    {
	if (items == 2)
	{
	    lnum = SvIV(ST(1));
	    count = 1;
	}
	else if (items == 3)
	{
	    lnum = SvIV(ST(1));
	    count = 1 + SvIV(ST(2)) - lnum;
	    if(count == 0)
		count = 1;
	    if(count < 0)
	    {
		lnum -= count;
		count = -count;
	    }
	}
	if (items >= 2)
	{
	    for (i=0; i<count; i++)
	    {
		if (lnum > 0 && lnum <= vimbuf->b_ml.ml_line_count)
		{
		    savebuf = curbuf;
		    curbuf = vimbuf;
		    if (u_savedel(lnum, 1) == OK)
		    {
			mark_adjust(lnum, lnum, MAXLNUM, -1);
			ml_delete(lnum, 0);
		    }
		    curbuf = savebuf;
		    update_curbuf(NOT_VALID);
		}
	    }
	}
    }

void
Append(vimbuf, ...)
    VIBUF vimbuf;

    PREINIT:
    int i;
    long lnum;
    char *line;
    BUF *savebuf;
    PPCODE:
    if (buf_valid(vimbuf))
    {
	if (items < 3)
	    croak("Usage: VIBUF::Append(vimbuf, lnum, @lines)");

	lnum = SvIV(ST(1));
	for(i=2; i<items; i++, lnum++)
	{
	    line = SvPV(ST(i),na);
	    if(lnum >= 0 && lnum <= vimbuf->b_ml.ml_line_count && line != NULL)
	    {
		savebuf = curbuf;
		curbuf = vimbuf;
		if (u_inssub(lnum + 1) == OK)
		{
		    mark_adjust(lnum + 1, MAXLNUM, 1L, 0L);
		    ml_append(lnum, (char_u *)line, (colnr_t)0, FALSE);
		}
		curbuf = savebuf;
		update_curbuf(NOT_VALID);
	    }
	}
    }

