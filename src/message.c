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

/*
 * msg(s) - displays the string 's' on the status line
 */
	void
msg(s)
	char		   *s;
{
	int len = -1;

	if (RedrawingDisabled)
		return;
	gotocmdline(YES, NUL);
	/*
	 * Truncate the string if it's larger than the window.
	 * (this may still happen with non-printable characters).
	 */
	if (strlen(s) >= Columns)
		len = Columns - 1;
	outtrans(s, len);
	flushbuf();
}

/* VARARGS */
	void
smsg(s, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
	char		*s;
	long		a1, a2, a3, a4, a5, a6, a7, a8, a9, a10;
{
	sprintf(IObuff, s, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
	msg(IObuff);
}

/*
 * emsg() - display an error message
 *
 * Rings the bell, if appropriate, and calls message() to do the real work
 */
	void
emsg(s)
	char		   *s;
{
	int temp;

	if (P(P_EB))
		beep();				/* also includes flush_buffers() */
	else
		flush_buffers();		/* delete all typeahead characters */
	outstr(T_TI);
	temp = RedrawingDisabled;
	RedrawingDisabled = FALSE;
	msg(s);
	RedrawingDisabled = temp;
	outstr(T_TP);
	flushbuf();
	sleep(1);	/* give the user a chance to read the message */
}

	void
msgmore(n)
	long n;
{
	long pn;

	if (n > 0)
		pn = n;
	else
		pn = -n;

	if (pn > P(P_RP))
		smsg("%ld %s lines %s", pn, n > 0 ? "more" : "fewer", got_int ? "(Interrupted)" : "");
}

	void
wait_return(redraw)
	bool_t	redraw;
{
	u_char			c;
	int				oldstate;

	oldstate = State;
	State = HITRETURN;
	if (got_int)
		outstr("Interrupt: ");
	outstr("Press RETURN to continue");
	do {
		c = vgetc();
	} while (c != '\r' && c != '\n' && c != ' ' && c != ':');

	if (State == SETWINSIZE)
	{
		State = oldstate;
		set_winsize(0, 0);
	}
	State = oldstate;
	script_winsize_pp();

	if (c == ':')								 /* this can vi too  */
	{
		outchar('\n');
		docmdline(NULL);
	}
	if (redraw)
	{
		screenclear();
		updateScreen(NOT_VALID);
	}
}

ask_yesno(str)
	char *str;
{
	int r = ' ';

	while (r != 'y' && r != 'n')
	{
		smsg("%s (y/n)? ", str);
		r = vgetc();
		outchar(r);		/* show what you typed */
		flushbuf();
	}
	return r;
}
