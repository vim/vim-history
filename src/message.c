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
 * message.c: functions for displaying messages on the command line
 */

#include "vim.h"
#include "globals.h"
#define MESSAGE
#include "proto.h"
#include "param.h"

static int msg_invert = FALSE;

/*
 * msg(s) - displays the string 's' on the status line
 */
	void
msg(s)
	char		   *s;
{
	int len;

	if (Columns == 0)	/* terminal not initialized */
	{
		fprintf(stderr, s);
		fflush(stderr);
		return;
	}

	gotocmdline(TRUE, NUL);
	if (msg_invert)
		outstr(T_TI);
	len = outtrans(s, -1);
	if (msg_invert)
	{
		outstr(T_TP);
		msg_invert = FALSE;
	}
	flushbuf();
	/*
	 * if the string is larger than the window,
	 * or the ruler option is set and we run into it,
	 * we have to redraw the window.
	 * Do not do this if we are abandoning the file.
	 */
	if (!exiting && len >= Columns - (p_ru ? 22 : 0))
	{
		outchar('\n');
		wait_return(TRUE);
	}
}

/* VARARGS */
#ifndef PROTO		/* automatic prototype generation does not understand this */
	void
smsg(s, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
	char		*s;
	long		a1, a2, a3, a4, a5, a6, a7, a8, a9, a10;
{
	sprintf(IObuff, s, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
	msg(IObuff);
}
#endif

/*
 * emsg() - display an error message
 *
 * Rings the bell, if appropriate, and calls message() to do the real work
 */
	void
emsg(s)
	char		   *s;
{
	if (p_eb)
		beep();				/* also includes flush_buffers() */
	else
		flush_buffers();		/* delete all typeahead characters */
	msg_invert = TRUE;
	msg(s);
	flushbuf();
	if (got_int)		/* remove typeahead now, allow typeadhead during sleep */
		inchar(TRUE, FALSE);
	sleep(1);	/* give the user a chance to read the message */
}

	void
wait_return(redraw)
	int		redraw;
{
	u_char			c;
	int				oldstate;

	oldstate = State;
	State = HITRETURN;
	if (got_int)
		outstrn("Interrupt: ");

#ifdef ORG_HITRETURN
	outstrn("Press RETURN to continue");
	do {
		c = vgetc();
	} while (strchr("\r\n: ", c) == NULL);
	if (c == ':')			 /* this can vi too (but not always!) */
		stuffReadbuff(mkstr(c));
#else
	outstrn("Press RETURN or enter command to continue");
	c = vgetc();
	if (strchr("\r\n ", c) == NULL)
		stuffReadbuff(mkstr(c));
#endif

	if (State == SETWINSIZE)
	{
		State = oldstate;
		set_winsize(0, 0, FALSE);
	}
	State = oldstate;
	script_winsize_pp();

	if (redraw)
		updateScreen(CLEAR);
}
