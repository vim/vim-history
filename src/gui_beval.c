/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ifdef HAVE_CONFIG_H
# include "auto/config.h"
#endif
#include "feature.h"

#if defined(FEAT_BEVAL) || defined(PROTO)

#include <stdlib.h>
#include <stdio.h>
#include <sys/param.h>

#include <X11/keysym.h>
#include <Xm/PushB.h>
#include <Xm/Separator.h>
#include <Xm/List.h>
#include <Xm/Label.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>

#include "vim.h"
#include "gui_beval.h"

extern Widget vimShell;

static void addEventHandler(Widget, BalloonEval *);
static void removeEventHandler(BalloonEval *);
static void pointerEventEH(Widget, XtPointer, XEvent *, Boolean *);
static void pointerEvent(BalloonEval *, XEvent *);
static void timerRoutine(XtPointer, XtIntervalId *);
static void cancelBalloon(BalloonEval *);
static void requestBalloon(BalloonEval *);
static void drawBalloon(BalloonEval *);
static void createBalloonEvalWindow(BalloonEval *);
static int virtlen(buf_t *, char_u *);



#define EVAL_OFFSET_X 10 /* displacement of beval topleft corner from pointer */
#define EVAL_OFFSET_Y 5



    BalloonEval *
gui_mch_create_beval_area(target, msg, msgCB, clientData)
    Widget	target;
    char	*msg;
    void	(*msgCB)__ARGS((BalloonEval *, int));
    XtPointer	clientData;
{
    BalloonEval	*beval;
    char	*display_name;	    /* get from gui.dpy */
    int		screen_num;
    char	*p;

    if (msg != NULL && msgCB != NULL)
    {
	EMSG(_("Cannot create BalloonEval with both message and callback"));
	return NULL;
    }

    beval = (BalloonEval *) alloc(sizeof(BalloonEval));
    if (beval != NULL)
    {
	beval->target = target;
	beval->balloonShell = NULL;
	beval->timerID = NULL;
	beval->appContext = XtWidgetToApplicationContext(target);
	beval->showState = ShS_NEUTRAL;
	beval->x = 0;
	beval->y = 0;
	beval->msg = msg;
	beval->msgCB = msgCB;
	beval->clientData = clientData;

	/*
	 * Set up event handler which will keep its eyes on the pointer,
	 * and when the pointer rests in a certain spot for a given time
	 * interval, show the beval.
	 */
	if (p_beval)
		addEventHandler(target, beval);
	createBalloonEvalWindow(beval);

	/*
	 * Now create and save the screen width and height. Used in drawing.
	 */
	display_name = DisplayString(gui.dpy);
	p = strrchr(display_name, '.');
	if (p++ != NULL)
	    screen_num = atoi(p);
	else
	    screen_num = 0;
	beval->screen_width = DisplayWidth(gui.dpy, screen_num);
	beval->screen_height = DisplayHeight(gui.dpy, screen_num);
    }

    return beval;
}

    void
gui_mch_destroy_beval_area(beval)
    BalloonEval	*beval;
{
    cancelBalloon(beval);
    removeEventHandler(beval);
}

    void
gui_mch_enable_beval_area(beval)
    BalloonEval *beval;
{
    addEventHandler(beval->target, beval);
}

    void
gui_mch_disable_beval_area(beval)
    BalloonEval *beval;
{
    removeEventHandler(beval);
}

    Boolean
gui_mch_get_beval_info(beval, filename, line, text, index)
    BalloonEval	*beval;
    char_u     **filename;
    int		*line;
    char_u     **text;
    int		*index;
{
    win_t	*wp;
    int		row, col;
    int		row_off;
    int		i;
    int		top_off;
    char_u	*lbuf;

    top_off = Y_2_ROW(beval->y);
    for (wp = firstwin; wp != NULL; wp = W_NEXT(wp))
	if (W_WINROW(wp) <= top_off && top_off < (W_WINROW(wp) + wp->w_height))
	    break;

    if (wp != NULL)
    {
	row_off = top_off - W_WINROW(wp) + 1;
	i = 0;
	row = 0;
	while (i < row_off && row < wp->w_lines_valid)
	{
	    i += wp->w_lines[row].wl_size;
	    row++;
	}

	if (wp->w_lines[row - 1].wl_size > 1)
	    col = ((top_off - (i - (int)(wp->w_lines[row - 1]).wl_size)) *
		    gui.num_cols) + X_2_COL(beval->x) + 1;
	else
	    col = X_2_COL(beval->x) + 1;
	if (wp->w_p_nu)
	    col -= 8;

	if (wp->w_p_wrap == 0)
	    col += wp->w_leftcol;

	row += wp->w_topline - 1;
	if (col > 0)
	{
	    lbuf = ml_get_buf(wp->w_buffer, row, FALSE);
	    i = virtlen(wp->w_buffer, lbuf);
	    if (i >= col)		/* don't send if past end of line */
	    {
		*filename = wp->w_buffer->b_ffname;
		*line = row;
		*text = lbuf;
		*index = col;
		beval->ts = wp->w_buffer->b_p_ts;
		return True;
	    }
	}
    }

    return False;
}

    void
gui_mch_post_balloon(beval, msg)
    BalloonEval	*beval;
    char	*msg;
{
    beval->msg = msg;
    drawBalloon(beval);
}

    static void
addEventHandler(target, beval)
    Widget	target;
    BalloonEval	*beval;
{
    XtAddEventHandler(target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | KeyPressMask |
			KeyReleaseMask,
			False,
			pointerEventEH, (XtPointer)beval);
}


    static void
removeEventHandler(beval)
    BalloonEval	*beval;
{
    XtRemoveEventHandler(beval->target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | KeyPressMask |
			KeyReleaseMask,
			False,
			pointerEventEH, (XtPointer)beval);
}


/*
 * The X event handler. All it does is call the real event handler.
 */

    static void
pointerEventEH(w, client_data, event, unused)
    Widget	w;
    XtPointer	client_data;
    XEvent	*event;
    Boolean	*unused;
{
    BalloonEval *beval = (BalloonEval *)client_data;
    pointerEvent(beval, event);
}


/*
 * The real event handler. Called by pointerEventEH() whenever an event we are
 * interested in ocurrs.
 */

    static void
pointerEvent(beval, event)
    BalloonEval	*beval;
    XEvent	*event;
{
    Position	distance;	    /* a measure of how much the ponter moved */
    Position	delta;		    /* used to compute distance */

    if (!p_beval)
	return;

    switch (event->type)
    {
	case EnterNotify:
	case MotionNotify:
	    delta = event->xmotion.x - beval->x;
	    if (delta < 0)
		delta = -delta;
	    distance = delta;
	    delta = event->xmotion.y - beval->y;
	    if (delta < 0)
		delta = -delta;
	    distance += delta;
	    if (distance > 4)
	    {
		/*
		 * Moved out of the balloon location: cancel it.
		 * Remember button state
		 */
		beval->state = event->xmotion.state;
		if (beval->state & (Button1Mask|Button2Mask|Button3Mask))
		{
		    /* Mouse buttons are pressed - no balloon now */
		    cancelBalloon(beval);
		}
		else if (beval->state & (Mod1Mask|Mod2Mask|Mod3Mask))
		{
		    /*
		     * Alt is pressed -- enter super-evaluate-mode,
		     * where there is no time delay
		     */
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_root = event->xmotion.x_root;
		    beval->y_root = event->xmotion.y_root;
		    cancelBalloon(beval);
		    beval->showState = ShS_PENDING;
		    (*beval->msgCB)(beval, beval->state);
		}
		else
		{
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_root = event->xmotion.x_root;
		    beval->y_root = event->xmotion.y_root;
		    cancelBalloon(beval);
		    beval->timerID = XtAppAddTimeOut(
			    beval->appContext, p_bdlay, timerRoutine, beval);
		}
	    }
	    break;

	case KeyPress:
	    if (beval->showState == ShS_SHOWING)
	    {
		Modifiers   modifier;
		KeySym	    keysym;

		XtTranslateKeycode(gui.dpy,
				       event->xkey.keycode, event->xkey.state,
				       &modifier, &keysym);
		if ((keysym == XK_Shift_L) || (keysym == XK_Shift_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->msgCB)(beval, ShiftMask);
		}
		else if ((keysym == XK_Control_L) || (keysym == XK_Control_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->msgCB)(beval, ControlMask);
		}
		else
		    cancelBalloon(beval);
	    }
	    else
		cancelBalloon(beval);
	    break;

	case KeyRelease:
	    if (beval->showState == ShS_SHOWING)
	    {
		Modifiers modifier;
		KeySym keysym;

		XtTranslateKeycode(gui.dpy, event->xkey.keycode,
				event->xkey.state, &modifier, &keysym);
		if ((keysym == XK_Shift_L) || (keysym == XK_Shift_R)) {
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->msgCB)(beval, 0);
		}
		else if ((keysym == XK_Control_L) || (keysym == XK_Control_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->msgCB)(beval, 0);
		}
		else
		    cancelBalloon(beval);
	    }
	    else
		cancelBalloon(beval);
	    break;

	case LeaveNotify:
	case ButtonPress:
		cancelBalloon(beval);

	default:
	    break;
	}
}

    static int
virtlen(buf, line)
    buf_t	*buf;
    char_u	*line;
{
    int		count = 0;
    int		ts = buf->b_p_ts;

    while (*line)
    {
	if (*line == '\t')
	    count += ts - (count % ts);
	else
	    count++;
	line++;
    }

    return count;
}

    static void
timerRoutine(dx, id)
    XtPointer	    dx;
    XtIntervalId    *id;
{
    BalloonEval *beval = (BalloonEval *)dx;

    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);
}

    static void
requestBalloon(beval)
    BalloonEval	*beval;
{
    beval->timerID = NULL;
    if (beval->showState != ShS_PENDING)
    {
	/* Determine the beval to display */
	if (beval->msg)
		drawBalloon(beval);
	else if (beval->msgCB)
	{
	    beval->showState = ShS_PENDING;
	    (*beval->msgCB)(beval, beval->state);
	}
    }
}

    static void
drawBalloon(beval)
    BalloonEval	*beval;
{
	XmString s;
	Position tx;
	Position ty;
	Dimension	w;
	Dimension	h;

	if (beval->msg != NULL)
	{
	    /* Show the Balloon */

	    s = XmStringCreateLocalized((char *)beval->msg);
	    XmStringExtent(gui.balloonEval_fontList, s, &w, &h);
	    w += gui.border_offset << 1;
	    h += gui.border_offset << 1;
	    XtVaSetValues(beval->balloonLabel,
			XmNlabelString, s,
			NULL);
	    XmStringFree(s);

	    /* Compute position of the balloon area */
	    tx = beval->x_root + EVAL_OFFSET_X;
	    ty = beval->y_root + EVAL_OFFSET_Y;
	    if ((tx + w) > beval->screen_width)
		tx = beval->screen_width - w;
	    if ((ty + h) > beval->screen_height)
		ty = beval->screen_height - h;
	    XtVaSetValues(beval->balloonShell,
			XmNx, tx,
			XmNy, ty,
			XmNwidth, w,
			XmNheight, h,
			NULL);

	    XtPopup(beval->balloonShell, XtGrabNone);

	    beval->showState = ShS_SHOWING;
	}
	else
	{
	    if (beval->balloonShell)
		    XtPopdown(beval->balloonShell);
	}
}

    static void
cancelBalloon(beval)
    BalloonEval	*beval;
{
    if (beval->showState == ShS_SHOWING ||
		    beval->showState == ShS_UPDATE_PENDING)
    {
	beval->msg = NULL;		/* don't free message */
	drawBalloon(beval);
    }

    if (beval->timerID)
    {
	XtRemoveTimeOut(beval->timerID);
	beval->timerID = NULL;
    }
    beval->showState = ShS_NEUTRAL;
}


    static void
createBalloonEvalWindow(beval)
    BalloonEval	*beval;
{
    Arg		args[32];
    int		n;

    beval->balloonShell = XtAppCreateShell("balloonEval", "BalloonEval",
		    overrideShellWidgetClass, gui.dpy, NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNforeground, gui.balloonEval_fg_pixel); n++;
    XtSetArg(args[n], XmNbackground, gui.balloonEval_bg_pixel); n++;
    XtSetArg(args[n], XmNfontList, gui.balloonEval_fontList); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    beval->balloonLabel = XtCreateManagedWidget("balloonLabel",
		    xmLabelWidgetClass, beval->balloonShell, args, n);
}

#endif /* FEAT_BEVAL */
