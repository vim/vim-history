/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved
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

static int msg_invert = FALSE;		/* message should be inverted */

/*
 * msg(s) - displays the string 's' on the status line
 */
	void
msg(s)
	char		   *s;
{
	if (Columns == 0)	/* terminal not initialized */
	{
		fprintf(stderr, s);
		fflush(stderr);
		return;
	}

	start_msg();
	if (msg_invert && T_TI)
	{
		outstr(T_TI);
		char_count -= strlen(T_TI);
	}
	outtrans(s, -1);
	if (msg_invert && T_TP)
	{
		outstr(T_TP);
		msg_invert = FALSE;
		char_count -= strlen(T_TP);
	}
	end_msg();
}

#ifndef PROTO		/* automatic prototype generation does not understand this */
/* VARARGS */
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
		beep();					/* also includes flush_buffers() */
	else
		flush_buffers(FALSE);	/* flush internal buffers */
	msg_invert = TRUE;
	msg(s);
	if (char_count < sc_col)	/* if wait_return not called */
		sleep(1);				/* give the user a chance to read the message */
}

	void
emsg2(s, a1)
	char *s, *a1;
{
	sprintf(IObuff, s, a1);
	emsg(IObuff);
}

extern int global_busy, global_wait;	/* shared with csearch.c, cmdline.c */

	void
wait_return(redraw)
	int		redraw;
{
	u_char			c;
	int				oldState;
	int				tmpState;

		/* with the global command we only need one return at the end */
	if (global_busy)
	{
		global_wait = 1;
		starttermcap();
		return;
	}
	oldState = State;
	State = HITRETURN;
	if (got_int)
		outstrn("Interrupt: ");

#ifdef ORG_HITRETURN
	outstrn("Press RETURN to continue");
	do {
		c = vgetc();
	} while (strchr("\r\n: ", c) == NULL);
	if (c == ':')			 		/* this can vi too (but not always!) */
		stuffcharReadbuff(c);
#else
	outstrn("Press RETURN or enter command to continue");
	c = vgetc();
	breakcheck();
	if (strchr("\r\n ", c) == NULL)
		stuffcharReadbuff(c);
#endif

	if (!termcap_active)			/* start termcap before redrawing */
		starttermcap();

/*
 * If the window size changed set_winsize() will redraw the screen.
 * Otherwise the screen is only redrawn if 'redraw' is set.
 */
	tmpState = State;
	State = oldState;				/* restore State before set_winsize */
	if (tmpState == SETWSIZE)		/* got resize event while in vgetc() */
		set_winsize(0, 0, FALSE);
	else if (redraw)
		updateScreen(CLEAR);

	script_winsize_pp();
}

	void
start_msg()
{
	gotocmdline(TRUE, NUL);
	char_count = 0;
}

	void
end_msg()
{
	/*
	 * if the string is larger than the window,
	 * or the ruler option is set and we run into it,
	 * we have to redraw the window.
	 * Do not do this if we are abandoning the file.
	 */
	if (!exiting && char_count >= sc_col)
	{
		outchar('\n');
		wait_return(TRUE);
	}
	else
		flushbuf();
}

	void
check_msg()
{
	/*
	 * if the string is larger than the window,
	 * or the ruler option is set and we run into it,
	 * we have to redraw the window later.
	 */
	if (char_count >= sc_col)
		must_redraw = CLEAR;
	cmdoffset = char_count / Columns;
}
