/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#if !defined(GUI_BEVAL_H) && (defined(FEAT_BEVAL) || defined(PROTO))
#define GUI_BEVAL_H

#include <X11/Intrinsic.h>

typedef enum
{
    ShS_NEUTRAL,			/* nothing showing or pending */
    ShS_PENDING,			/* data requested from debugger */
    ShS_UPDATE_PENDING,			/* switching information displayed */
    ShS_SHOWING				/* the balloon is being displayed */
} BeState;

typedef struct
{
    Widget		target;		/* widget we are monitoring */
    Widget		balloonShell;
    Widget		balloonLabel;

    XtIntervalId	timerID;	/* timer for run */
    BeState		showState;	/* tells us whats currently going on */
    XtAppContext	appContext;	/* used in event handler */
    Position		x;
    Position		y;
    Position		x_root;
    Position		y_root;
    int			state;		/* Button/Modifier key state */
    int			ts;		/* tabstop setting for this buffer */
    char_u		*msg;
    void		(*msgCB)();
    void		*clientData;	/* For callback */
    Dimension		screen_width;	/* screen width in pixels */
    Dimension		screen_height;	/* screen height in pixels */
} BalloonEval;

#define EVAL_OFFSET_X 10 /* displacement of beval topleft corner from pointer */
#define EVAL_OFFSET_Y 5

#include "gui_beval.pro"

#endif /* GUI_BEVAL_H and FEAT_BEVAL */
