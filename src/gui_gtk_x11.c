/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * Porting to GTK+ was done by:
 *
 * (C) 1998,1999,2000 by Marcin Dalecki <dalecki@evision.ag>
 *
 * With GREAT support and continuous encouragements by Andy Kahn and of
 * course Bram Moolenaar!
 */

#include "vim.h"
#ifdef FEAT_GUI_GNOME
/* Gnome redefines _() and N_().  Grrr... */
# ifdef _
#  undef _
# endif
# ifdef N_
#  undef N_
# endif
# ifdef textdomain
#  undef textdomain
# endif
# ifdef bindtextdomain
#  undef bindtextdomain
# endif
# if defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	/* so the texts in the dialog boxes are translated */
# endif
# include <gnome.h>
# include "version.h"
#endif

#if !defined(FEAT_GUI_GTK) && defined(PROTO)
/* When generating prototypes we don't want syntax errors. */
# define GdkAtom int
# define GdkEventExpose int
# define GdkEventFocus int
# define GdkEventVisibility int
# define GdkEventProperty int
# define GtkContainer int
# define GtkTargetEntry int
# define GtkType int
# define GtkWidget int
# define gint int
# define gpointer int
# define guint int
# define GdkEventKey int
# define GdkEventSelection int
# define GtkSelectionData int
# define GdkEventMotion int
# define GdkEventButton int
# define GdkDragContext int
# define GdkEventConfigure int
# define GdkEventClient int
#else
# include <gdk/gdkkeysyms.h>
# include <gdk/gdk.h>
# include <gdk/gdkx.h>

# include <gtk/gtk.h>
# include "gui_gtk_f.h"
#endif

#ifdef HAVE_X11_SUNKEYSYM_H
# include <X11/Sunkeysym.h>
#endif

#define VIM_NAME	"vim"
#define VIM_CLASS	"Vim"

/* slection distinguishers */
enum
{
    SELECTION_TYPE_NONE,
    SELECTION_STRING,
    SELECTION_TEXT,
    SELECTION_COMPOUND_TEXT,
    SELECTION_VIM
};

/*
 * Enable DND feature.  Disable this if it causes problems.
 */
#ifdef FEAT_WINDOWS
# define GTK_DND
#endif

#ifdef GTK_DND
/* DND specification constants. */
enum
{
  TARGET_STRING
};

static GtkTargetEntry target_table[] =
{
    {"text/uri-list",	0, TARGET_STRING},
    {"STRING",		0, TARGET_STRING},
    {"text/plain",	0, TARGET_STRING}
};
static guint n_targets = sizeof(target_table) / sizeof(target_table[0]);
#endif

/* This is the single only fixed width font in X11, which seems to be present
 * on all servers and available in all the variants we need.
 */
#define DFLT_FONT	"-adobe-courier-medium-r-normal-*-14-*-*-*-m-*-*-*"

/*
 * Atoms used to communicate save-yourself from the X11 session manager. There
 * is no need to move them into the GUI struct, since they should be constant.
 */
static GdkAtom wm_protocols_atom = GDK_NONE;
static GdkAtom save_yourself_atom = GDK_NONE;

/*
 * Atom used to recognize requests for on the fly GTK+ style configuration
 * changes.
 */
static GdkAtom reread_rcfiles_atom = GDK_NONE;

/*
 * Atoms used to control/reference X11 selections.
 */
static GdkAtom compound_text_atom = GDK_NONE;
static GdkAtom text_atom = GDK_NONE;
static GdkAtom vim_atom = GDK_NONE;	/* Vim's own special selection format */

/*
 * Keycodes recognized by vim.
 */
static struct special_key
{
    guint key_sym;
    char_u code0;
    char_u code1;
} special_keys[] =
{
    {GDK_Up,		'k', 'u'},
    {GDK_Down,		'k', 'd'},
    {GDK_Left,		'k', 'l'},
    {GDK_Right,		'k', 'r'},
    {GDK_F1,		'k', '1'},
    {GDK_F2,		'k', '2'},
    {GDK_F3,		'k', '3'},
    {GDK_F4,		'k', '4'},
    {GDK_F5,		'k', '5'},
    {GDK_F6,		'k', '6'},
    {GDK_F7,		'k', '7'},
    {GDK_F8,		'k', '8'},
    {GDK_F9,		'k', '9'},
    {GDK_F10,		'k', ';'},
    {GDK_F11,		'F', '1'},
    {GDK_F12,		'F', '2'},
    {GDK_F13,		'F', '3'},
    {GDK_F14,		'F', '4'},
    {GDK_F15,		'F', '5'},
    {GDK_F16,		'F', '6'},
    {GDK_F17,		'F', '7'},
    {GDK_F18,		'F', '8'},
    {GDK_F19,		'F', '9'},
    {GDK_F20,		'F', 'A'},
    {GDK_F21,		'F', 'B'},
    {GDK_F22,		'F', 'C'},
    {GDK_F23,		'F', 'D'},
    {GDK_F24,		'F', 'E'},
    {GDK_F25,		'F', 'F'},
    {GDK_F26,		'F', 'G'},
    {GDK_F27,		'F', 'H'},
    {GDK_F28,		'F', 'I'},
    {GDK_F29,		'F', 'J'},
    {GDK_F30,		'F', 'K'},
    {GDK_F31,		'F', 'L'},
    {GDK_F32,		'F', 'M'},
    {GDK_F33,		'F', 'N'},
    {GDK_F34,		'F', 'O'},
    {GDK_F35,		'F', 'P'},
#ifdef SunXK_F36
    {SunXK_F36,		'F', 'Q'},
    {SunXK_F37,		'F', 'R'},
#endif
    {GDK_Help,		'%', '1'},
    {GDK_Undo,		'&', '8'},
    {GDK_BackSpace,	'k', 'b'},
    {GDK_Insert,	'k', 'I'},
    {GDK_Delete,	'k', 'D'},
    {GDK_Home,		'k', 'h'},
    {GDK_End,		'@', '7'},
    {GDK_Prior,		'k', 'P'},
    {GDK_Next,		'k', 'N'},
    {GDK_Print,		'%', '9'},
    /* Keypad keys: */
    {GDK_KP_Left,	'k', 'l'},
    {GDK_KP_Right,	'k', 'r'},
    {GDK_KP_Up,		'k', 'u'},
    {GDK_KP_Down,	'k', 'd'},
    {GDK_KP_Insert,	'k', 'I'},
    {GDK_KP_Delete,	'k', 'D'},
    {GDK_KP_Home,	'k', 'h'},
    {GDK_KP_End,	'@', '7'},
    {GDK_KP_Prior,	'k', 'P'},
    {GDK_KP_Next,	'k', 'N'},

    {GDK_KP_Add,	'K', '6'},
    {GDK_KP_Subtract,	'K', '7'},
    {GDK_KP_Divide,	'K', '8'},
    {GDK_KP_Multiply,	'K', '9'},
    {GDK_KP_Enter,	'K', 'A'},
    {GDK_KP_Decimal,	'K', 'B'},

    {GDK_KP_0,		'K', 'C'},
    {GDK_KP_1,		'K', 'D'},
    {GDK_KP_2,		'K', 'E'},
    {GDK_KP_3,		'K', 'F'},
    {GDK_KP_4,		'K', 'G'},
    {GDK_KP_5,		'K', 'H'},
    {GDK_KP_6,		'K', 'I'},
    {GDK_KP_7,		'K', 'J'},
    {GDK_KP_8,		'K', 'K'},
    {GDK_KP_9,		'K', 'L'},

    /* End of list marker: */
    {0, 0, 0}
};

/*
 * This table holds all the X GUI command line options allowed.  This includes
 * the standard ones so that we can skip them when vim is started without the
 * GUI (but the GUI might start up later).
 *
 * When changing this, also update doc/gui_x11.txt and the usage message!!!
 */
static struct cmdline_option
{
    char    *name;
    int	    has_value;
} cmdline_options[] =
{
    /* We handle these options ourselfs */
    {"-fn",		TRUE},
    {"-font",		TRUE},
    {"-geom",		TRUE},
    {"-geometry",	TRUE},
    {"-reverse",	FALSE},
    {"-rv",		FALSE},
#if 0	/* TBD */
    {"-bg",		TRUE},
    {"-background",	TRUE},
    {"-fg",		TRUE},
    {"-foreground",	TRUE},
    {"-boldfont",	TRUE},
    {"-italicfont",	TRUE},
    {"+reverse",	FALSE},
    {"+rv",		FALSE},
    {"-iconic",		FALSE},
    {"-name",		TRUE},
    {"-bw",		TRUE},
    {"-borderwidth",	TRUE},
    {"-sw",		TRUE},
    {"-scrollbarwidth", TRUE},
    {"-xrm",		TRUE},
#endif
};

/*
 * Arguments handled by GTK (and GNOME) internally.
 */
static char *gtk_cmdline_options[] =
{
    "--sync",
    "--gdk-debug=",
    "--gdk-no-debug=",
    "--no-xshm",
    "--xim-preedit",
    "--xim-status",
    "--gtk-debug=",
    "--gtk-no-debug=",
    "--g-fatal-warnings",
    "--gtk-module=",
    "-display",
    "--display",
#ifdef FEAT_GUI_GNOME
    "--disable-sound",
    "--enable-sound",
    "--espeaker=",
    "--version",
    "-?",
    "--help",
    "--usage",
    "--disable-crash-dialog",
    "--sm-client-id=",
    "--sm-config-prefix=",
    "--sm-disable",
#endif
    NULL
};

static int gui_argc = 0;
static char **gui_argv = NULL;

#ifdef FEAT_GUI_GNOME
/*
 * Can't use Gnome if --socketid given
 */
static int using_gnome = 0;
#else
# define using_gnome 0
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    void
gui_mch_prepare(int *argc, char **argv)
{
    int arg;
    int i;
    char **gtk_option;

    /*
     * Move all the entries in argv which are relevant to X into gui_argv.
     */
    gui_argc = 0;
    gui_argv = (char **)lalloc((long_u)(*argc * sizeof(char *)), FALSE);
    if (gui_argv == NULL)
	return;

    gui_argv[gui_argc++] = argv[0];
    arg = 1;
    while (arg < *argc)
    {
	/* Look for argv[arg] in cmdline_options[] table */
	for (i = 0; i < XtNumber(cmdline_options); i++)
	    if (strcmp(argv[arg], cmdline_options[i].name) == 0)
		break;

	if (i < XtNumber(cmdline_options))
	{
	    /* Remember finding "-rv" or "-reverse" */
	    if (strcmp("-rv", argv[arg]) == 0
		    || strcmp("-reverse", argv[arg]) == 0)
	    {
		found_reverse_arg = TRUE;
	    }
	    else if ((strcmp("-fn", argv[arg]) == 0
			|| strcmp("-font", argv[arg]) == 0) && arg + 1 < *argc)
	    {
		font_argument = argv[arg + 1];
	    }
	    else if ((strcmp("-geometry", argv[arg]) == 0
			|| strcmp("-geom", argv[arg]) == 0) && arg + 1 < *argc)
	    {
		gui.geom = (char_u *)g_strdup((const char *)argv[arg + 1]);
	    }

#ifndef FEAT_GUI_GNOME
	    /* Found match in table, so move it into gui_argv */
	    gui_argv[gui_argc++] = argv[arg];
#endif
	    if (--*argc > arg)
	    {
		mch_memmove(&argv[arg], &argv[arg + 1],
			    (*argc - arg) * sizeof(char *));
		if (cmdline_options[i].has_value)
		{
#ifndef FEAT_GUI_GNOME
		    /* Move the option argument as well. */
		    gui_argv[gui_argc++] = argv[arg];
#endif
		    if (--*argc > arg)
			mch_memmove(&argv[arg], &argv[arg + 1],
				    (*argc - arg) * sizeof(char *));
		}
	    }
	}
	else
	    ++arg;
    }

    /* extract the gtk internal options */
    gtk_option = gtk_cmdline_options;
    while (*gtk_option)
    {
	arg = 1;
	while (arg < *argc)
	{
	    if (strncmp(argv[arg], *gtk_option, strlen(*gtk_option)) == 0)
	    {
		/* Replace the standard X argument "-display" with the GTK
		 * argument "--display". */
		if (strncmp(*gtk_option, "-display", 8) == 0)
		    gui_argv[gui_argc++] =
				   (char *)vim_strsave((char_u *)"--display");
		else
		    gui_argv[gui_argc++] = argv[arg];

		/* These arguments make gnome_init() print a message and exit.
		 * Must start the GUI for this, otherwise ":gui" will exit
		 * later! */
		if (strcmp("-?", argv[arg]) == 0
			|| strcmp("--help", argv[arg]) == 0
			|| strcmp("--version", argv[arg]) == 0
			|| strcmp("--usage", argv[arg]) == 0)
		    gui.starting = TRUE;

		if (--*argc > arg)
		{
		    mch_memmove(&argv[arg], &argv[arg + 1],
					      (*argc - arg) * sizeof(char *));
		    /* Move the option argument as well, if there is one. */
		    if (strncmp(*gtk_option, "--xim", 5) == 0
			    || strncmp(*gtk_option, "-display", 8) == 0
			    || strncmp(*gtk_option, "--display", 9) == 0)
		    {
			gui_argv[gui_argc++] = argv[arg];
			if (--*argc > arg)
			    mch_memmove(&argv[arg], &argv[arg + 1],
				    (*argc - arg) * sizeof(char *));
		    }
		}
		break;
	    }
	    else
		arg++;
	}
	gtk_option++;
    }
}

/*
 * This should be maybe completely removed.
 */
/*ARGSUSED*/
    static gint
visibility_event(GtkWidget * widget, GdkEventVisibility * event)
{
    /* Just remember it for the creation of GCs before drawing
     * operations.
     */
    gui.visibility = event->state;

    return FALSE;
}

/*
 * Redraw the corresponding portions of the screen.
 */
/*ARGSUSED*/
    static gint
expose_event(GtkWidget * widget, GdkEventExpose * event)
{
    /* Skip this when the GUI isn't set up yet, will redraw later. */
    if (gui.starting)
	return FALSE;

    out_flush();		/* make sure all output has been processed */
    gui_redraw(event->area.x, event->area.y,
	       event->area.width, event->area.height);

    /* Clear the border areas if needed */
    if (event->area.x < FILL_X(0))
	gdk_window_clear_area(gui.drawarea->window, 0, 0, FILL_X(0), 0);
    if (event->area.y < FILL_Y(0))
	gdk_window_clear_area(gui.drawarea->window, 0, 0, 0, FILL_Y(0));
    if (event->area.x > FILL_X(Columns))
	gdk_window_clear_area(gui.drawarea->window,
			      FILL_X((int)Columns), 0, 0, 0);
    if (event->area.y > FILL_Y(Rows))
	gdk_window_clear_area(gui.drawarea->window, 0, FILL_Y((int)Rows), 0, 0);

    return FALSE;
}

#ifdef FEAT_CLIENTSERVER
/*
 * Handle changes to the "Comm" property
 */
/*ARGSUSED*/
    static gint
property_event(GtkWidget * widget, GdkEventProperty * e)
{
    if (e->type == GDK_PROPERTY_NOTIFY
	    && GDK_WINDOW_XWINDOW(e->window) == commWindow
	    && e->atom == commProperty
	    && e->state == (guint)GDK_PROPERTY_NEW_VALUE)
    {
	XEvent	    xev;

	/* Translate to XLib */
	xev.xproperty.type = PropertyNotify;
	xev.xproperty.atom = commProperty;
	xev.xproperty.window = commWindow;
	xev.xproperty.state = PropertyNewValue;
	serverEventProc(GDK_WINDOW_XDISPLAY(widget->window), &xev);
	if (gtk_main_level() > 0)
	    gtk_main_quit();
    }
    return FALSE;
}
#endif


/****************************************************************************
 * Focus handlers:
 */


/*
 * This is a simple state machine:
 * BLINK_NONE   not blinking at all
 * BLINK_OFF    blinking, cursor is not shown
 * BLINK_ON     blinking, cursor is shown
 */

#define BLINK_NONE  0
#define BLINK_OFF   1
#define BLINK_ON    2

static int blink_state = BLINK_NONE;
static long_u blink_waittime = 700;
static long_u blink_ontime = 400;
static long_u blink_offtime = 250;
static guint blink_timer = 0;

    void
gui_mch_set_blinking(long waittime, long on, long off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    void
gui_mch_stop_blink()
{
    if (blink_timer)
    {
	gtk_timeout_remove(blink_timer);
	blink_timer = 0;
    }
    if (blink_state == BLINK_OFF)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

/*ARGSUSED*/
    static gint
blink_cb(gpointer data)
{
    if (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = gtk_timeout_add((guint32)blink_offtime,
				   (GtkFunction) blink_cb, NULL);
    }
    else
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = gtk_timeout_add((guint32)blink_ontime,
				   (GtkFunction) blink_cb, NULL);
    }

    return FALSE;		/* don't happen again */
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    void
gui_mch_start_blink()
{
    if (blink_timer)
	gtk_timeout_remove(blink_timer);
    /* Only switch blinking on if none of the times is zero */
    if (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = gtk_timeout_add((guint32)blink_waittime,
				   (GtkFunction) blink_cb, NULL);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

/*ARGSUSED*/
    static void
enter_notify_event(GtkContainer * container, gpointer data)
{
    if (blink_state == BLINK_NONE)
	gui_mch_start_blink();

    /* make sure keyboard input goes there */
    if (gtk_socket_id == 0 || !GTK_WIDGET_HAS_FOCUS(gui.drawarea))
	gtk_widget_grab_focus(gui.drawarea);
}

/*ARGSUSED*/
    static void
leave_notify_event(GtkContainer * container, gpointer data)
{
    if (blink_state != BLINK_NONE)
	gui_mch_stop_blink();
}

/*ARGSUSED*/
    static gint
focus_in_event(GtkWidget *widget, GdkEventFocus *focus, gpointer data)
{
    gui_focus_change(TRUE);

    if (blink_state == BLINK_NONE)
	gui_mch_start_blink();

    /* make sure keyboard input goes there */
    if (gtk_socket_id == 0)
	gtk_widget_grab_focus(gui.drawarea);

    return TRUE;
}

/*ARGSUSED*/
    static gint
focus_out_event(GtkWidget * widget, GdkEventFocus *focus, gpointer data)
{
    gui_focus_change(FALSE);

    if (blink_state != BLINK_NONE)
	gui_mch_stop_blink();

    return TRUE;
}


/****************************************************************************
 * Main keyboard handler:
 */

/*ARGSUSED*/
    static gint
key_press_event(GtkWidget * widget, GdkEventKey * event, gpointer data)
{
    char_u	string[256], string2[256];
    guint	key_sym;
    int		len;
    int		i;
    int		modifiers;
    int		key;
    guint	state;
    char_u	*s, *d;

    key_sym = event->keyval;
    len = event->length;
    state = event->state;
    g_assert(len <= sizeof(string));

    /*
     * It appears as if we always want to consume a key-press (there currently
     * aren't any 'return FALSE's), so we always do this: when running in a
     * GtkPlug and not a window, we must prevent emission of the key_press
     * EVENT from continuing (which is 'beyond' the level of stopping mere
     * signals by returning FALSE), otherwise things like tab/cursor-keys are
     * processed by the GtkPlug default handler, which moves input focus away
     * from us!
     */
    if (gtk_socket_id != 0)
	gtk_signal_emit_stop_by_name(GTK_OBJECT(widget), "key_press_event");

#ifdef FEAT_XIM
    if (xim_queue_key_press_event((GdkEvent *)event))
	return TRUE;
#endif

#ifdef FEAT_HANGULIN
    if (event->keyval == GDK_space && (state & GDK_SHIFT_MASK))
    {
	hangul_input_state_toggle();
	return TRUE;
    }
#endif

#ifdef SunXK_F36
    /*
    * These keys have bogus lookup strings, and trapping them here is
    * easier than trying to XRebindKeysym() on them with every possible
    * combination of modifiers.
    */
    if (key_sym == SunXK_F36 || key_sym == SunXK_F37)
	len = 0;
    else
#endif
    {
#ifdef FEAT_MBYTE
	if (input_conv.vc_type != CONV_NONE)
	{
	    mch_memmove(string2, event->string, len);
	    len = convert_input(string2, len, sizeof(string2));
	    s = string2;
	}
	else
#endif
	    s = (char_u *)event->string;
	d = string;
	for (i = 0; i < len; ++i)
	{
	    *d++ = s[i];
	    if (d[-1] == CSI && d + 2 < string + sizeof(string))
	    {
		/* Turn CSI into K_CSI. */
		*d++ = KS_EXTRA;
		*d++ = (int)KE_CSI;
	    }
	}
	len = d - string;
    }

    /* Shift-Tab results in Left_Tab, but we want <S-Tab> */
    if (key_sym == GDK_ISO_Left_Tab)
	key_sym = GDK_Tab;

    if ((key_sym == GDK_2 || key_sym == GDK_at) && (state & GDK_CONTROL_MASK))
    {
	string[0] = NUL;	/* CTRL-2 and CTRL-@ is NUL */
	len = 1;
    }
    else if (len == 0 && (key_sym == GDK_space || key_sym == GDK_Tab))
    {
	/* When there are modifiers, these keys get zero length; we need the
	 * original key here to be able to add a modifier below. */
	string[0] = (key_sym & 0xff);
	len = 1;
    }

#ifdef FEAT_MENU
    /* If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
     * is a menu shortcut, we ignore everything with the ALT modifier. */
    if ((state & GDK_MOD1_MASK)
	    && gui.menu_is_active
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	return TRUE;
#endif

    /* Check for Alt/Meta key (Mod1Mask), but not for a BS, DEL or character
     * that already has the 8th bit set.
     * Don't do this for <S-M-Tab>, that should become K_S_TAB with ALT. */
    if (len == 1
	    && (state & GDK_MOD1_MASK)
	    && !(key_sym == GDK_BackSpace || key_sym == GDK_Delete)
	    && (string[0] & 0x80) == 0
	    && !(key_sym == GDK_Tab && (state & GDK_SHIFT_MASK)))
    {
	string[0] |= 0x80;
	state &= ~GDK_MOD1_MASK;	/* don't use it again */
#ifdef FEAT_MBYTE
	if (enc_utf8) /* convert to utf-8 */
	{
	    string[1] = string[0] & 0xbf;
	    string[0] = ((unsigned)string[0] >> 6) + 0xc0;
	    if (string[1] == CSI)
	    {
		string[2] = KS_EXTRA;
		string[3] = (int)KE_CSI;
		len = 4;
	    }
	    else
		len = 2;
	}
#endif
    }

    /* Check for special keys, making sure BS and DEL are recognized. */
    if (len == 0 || key_sym == GDK_BackSpace || key_sym == GDK_Delete)
    {
	for (i = 0; special_keys[i].key_sym != 0; i++)
	{
	    if (special_keys[i].key_sym == key_sym)
	    {
		string[0] = CSI;
		string[1] = special_keys[i].code0;
		string[2] = special_keys[i].code1;
		len = -3;
		break;
	    }
	}
    }

    if (len == 0)   /* Unrecognized key */
	return TRUE;

    /* Special keys (and a few others) may have modifiers */
    if (len == -3 || key_sym == GDK_space || key_sym == GDK_Tab
	    || key_sym == GDK_Return || key_sym == GDK_Linefeed
	    || key_sym == GDK_Escape)
    {
	modifiers = 0;
	if (state & GDK_SHIFT_MASK)
	    modifiers |= MOD_MASK_SHIFT;
	if (state & GDK_CONTROL_MASK)
	    modifiers |= MOD_MASK_CTRL;
	if (state & GDK_MOD1_MASK)
	    modifiers |= MOD_MASK_ALT;

	/*
	 * For some keys a shift modifier is translated into another key
	 * code.
	 */
	if (len == -3)
	    key = TO_SPECIAL(string[1], string[2]);
	else
	    key = string[0];

	key = simplify_key(key, &modifiers);
	if (key == CSI)
	    key = K_CSI;
	if (IS_SPECIAL(key))
	{
	    string[0] = CSI;
	    string[1] = K_SECOND(key);
	    string[2] = K_THIRD(key);
	    len = 3;
	}
	else
	{
	    string[0] = key;
	    len = 1;
	}

	if (modifiers != 0)
	{
	    string2[0] = CSI;
	    string2[1] = KS_MODIFIER;
	    string2[2] = modifiers;
	    add_to_input_buf(string2, 3);
	}
    }

    if (len == 1 && ((string[0] == Ctrl_C && ctrl_c_interrupts)
		   || (string[0] == intr_char && intr_char != Ctrl_C)))
    {
	trash_input_buf();
	got_int = TRUE;
    }

    add_to_input_buf(string, len);

    /* blank out the pointer if necessary */
    if (p_mh)
	gui_mch_mousehide(TRUE);

    if (gtk_main_level() > 0)
	gtk_main_quit();

    return TRUE;
}


/****************************************************************************
 * Selection handlers:
 */

/*ARGSUSED*/
    static gint
selection_clear_event(GtkWidget * widget, GdkEventSelection * event)
{
    if (event->selection == clip_plus.gtk_sel_atom)
	clip_lose_selection(&clip_plus);
    else
	clip_lose_selection(&clip_star);

    if (gtk_main_level() > 0)
	gtk_main_quit();

    return TRUE;
}

#define RS_NONE	0	/* selection_received_event() not called yet */
#define RS_OK	1	/* selection_received_event() called and OK */
#define RS_FAIL	2	/* selection_received_event() called and failed */
static int received_selection;

/*ARGSUSED*/
    static void
selection_received_event(GtkWidget * widget, GtkSelectionData * data)
{
    int		motion_type;
    long_u	len;
    char_u	*p;
    int		free_p = FALSE;
    VimClipboard *cbd;

    if (data->selection == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    else
	cbd = &clip_star;

    if ((!data->data) || (data->length <= 0))
    {
	received_selection = RS_FAIL;
	/* clip_free_selection(cbd); ??? */
	if (gtk_main_level() > 0)
	    gtk_main_quit();

	return;
    }

    motion_type = MCHAR;
    if (data->type == compound_text_atom || data->type == text_atom)
    {
	int	count, i;
	char	**list;
	GString *str = g_string_new(NULL);

	count = gdk_text_property_to_text_list(data->type, data->format,
					     data->data, data->length, &list);
	if (count > 0)
	{
	    for (i = 0; i < count; i++)
		g_string_append(str, list[i]);
	    gdk_free_text_list(list);
	}

	p = (char_u *)str->str;
	len = str->len;
	g_string_free(str, FALSE);
	free_p = TRUE;
    }
    else
    {
	p = (char_u *)data->data;
	len = data->length;
    }

    if (data->type == vim_atom)
    {
	motion_type = *p++;
	len--;
    }
    clip_yank_selection(motion_type, p, (long) len, cbd);
    received_selection = RS_OK;
    if (gtk_main_level() > 0)
	gtk_main_quit();

    if (free_p)
	g_free(p);
}

/*
 * Prepare our selection data for passing it to the external selection
 * client.
 */
/*ARGSUSED*/
    static void
selection_get_event(GtkWidget *widget,
		    GtkSelectionData *selection_data,
		    guint      info,
		    guint      time_,
		    gpointer   data)
{
    char_u	*string;
    char_u	*result;
    long_u	length;
    int		motion_type;
    GdkAtom	type = GDK_NONE;
    VimClipboard *cbd;

    if (selection_data->selection == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    else
	cbd = &clip_star;

    if (!cbd->owned)
	return;			/* Shouldn't ever happen */

    if (info != (guint)SELECTION_STRING
	    && info != (guint)SELECTION_VIM
	    && info != (guint)SELECTION_COMPOUND_TEXT
	    && info != (guint)SELECTION_TEXT)
	return;

    /* get the selection from the '*'/'+' register */
    clip_get_selection(cbd);
    motion_type = clip_convert_selection(&string, &length, cbd);
    if (motion_type < 0)
	return;

    /* For our own format, the first byte contains the motion type */
    if (info == (guint)SELECTION_VIM)
	length++;

    result = lalloc((long_u)(2 * length), FALSE);
    if (result == NULL)
    {
	vim_free(string);
	return;
    }

    if (info == (guint)SELECTION_VIM)
    {
	result[0] = motion_type;
	mch_memmove(result + 1, string, (size_t)(length - 1));
	type = vim_atom;
    }
    else if (info == (guint)SELECTION_COMPOUND_TEXT
					     || info == (guint)SELECTION_TEXT)
    {
	char *str;
	gint format, new_len;

	vim_free(result);
	str = g_new(char, length + 1);
	mch_memmove(str, string, (size_t) length);
	vim_free(string);
	str[length] = '\0';
	gdk_string_to_compound_text(str, &type, &format, &result, &new_len);
	g_free(str);
	selection_data->type = type;
	selection_data->format = format;
	gtk_selection_data_set(selection_data, type, format, result, new_len);
	gdk_free_compound_text(result);
	return;
    }
    else
    {
	mch_memmove(result, string, (size_t)length);
	type = GDK_TARGET_STRING;
    }
    selection_data->type = selection_data->target;
    selection_data->format = 8;	/* 8 bits per char */

    gtk_selection_data_set(selection_data, type, 8, result, (gint)length);
    vim_free(string);
    vim_free(result);
}


/*
 * Check if the GUI can be started.  Called before gvimrc is sourced.
 * Return OK or FAIL.
 */
    int
gui_mch_init_check(void)
{
    /* This is needed to make the locale handling consistant between the GUI
     * and the rest of VIM. */
    gtk_set_locale();

#ifdef FEAT_GUI_GNOME
    if (gtk_socket_id == 0)
	using_gnome = 1;
#endif

    if ((
#ifdef FEAT_GUI_GNOME
	    using_gnome
		&& gnome_init("vim", VIM_VERSION_SHORT, gui_argc, gui_argv))
	    || (!using_gnome &&
#endif
		!gtk_init_check(&gui_argc, &gui_argv)))
	/* Don't use gtk_init(), it exits on failure. */
    {
	gui.dying = TRUE;
	EMSG(_("E233: cannot open display"));
	return FAIL;
    }
    vim_free(gui_argv);

    /* This is needed for the X clipboard support.
     * Not very nice, but it works...
     */
    gui.dpy = GDK_DISPLAY();
    return OK;
}


/****************************************************************************
 * Mouse handling callbacks
 */


static guint mouse_click_timer = 0;
static int mouse_timed_out = TRUE;

/*
 * Timer used to recognize multiple clicks of the mouse button
 */
    static gint
mouse_click_timer_cb(gpointer data)
{
    /* we don't use this information currently */
    int *timed_out = (int *) data;

    *timed_out = TRUE;
    return FALSE;		/* don't happen again */
}

static guint motion_repeat_timer = 0;
static int motion_repeat_offset = FALSE;

static gint motion_notify_event(GtkWidget *, GdkEventMotion *);

/*
 * Timer used to recognize multiple clicks of the mouse button.
 */
/*ARGSUSED*/
    static gint
motion_repeat_timer_cb(gpointer data)
{
    gint x, y;
    GdkModifierType state;
    GdkEventMotion event;

    gdk_window_get_pointer(gui.drawarea->window, &x, &y, &state);

    if (!(state & (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK |
		    GDK_BUTTON3_MASK | GDK_BUTTON4_MASK |
		    GDK_BUTTON5_MASK)))
    {
	motion_repeat_timer = 0;
	return FALSE;
    }

    /* If there already is a mouse click in the input buffer, wait another
     * time (otherwise we would create a backlog of clicks) */
    if (vim_used_in_input_buf() > 10)
	return TRUE;

    motion_repeat_timer = 0;

    /* Fake a motion event.
     * Trick: Pretend the mouse moved to the next character on every other
     * event, otherwise drag events will be discarded, because they are still
     * in the same character. */
    event.is_hint = FALSE;
    if (motion_repeat_offset)
    {
	event.x = x + gui.char_width;
	motion_repeat_offset = FALSE;
    }
    else
    {
	event.x = x;
	motion_repeat_offset = TRUE;
    }
    event.y = y;
    event.state = state;
    motion_notify_event(gui.drawarea, &event);

    /* Don't happen again. We will get reinstalled in the synthetic event if
     * needed - thus repeating should still work.
     */
    return FALSE;
}

/*ARGSUSED*/
    static gint
motion_notify_event(GtkWidget * widget, GdkEventMotion * event)
{
    gint x, y;
    GdkModifierType state;
    int_u vim_modifiers;
    int button;

    if (event->is_hint)
	gdk_window_get_pointer(event->window, &x, &y, &state);
    else
    {
	x = event->x;
	y = event->y;
	state = event->state;
    }

    button = (event->state & (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK |
			      GDK_BUTTON3_MASK | GDK_BUTTON4_MASK |
			      GDK_BUTTON5_MASK))
	      ? MOUSE_DRAG : ' ';

    /* If our pointer is currently hidden, then we should show it. */
    gui_mch_mousehide(FALSE);

    /* Just moving the rodent above the drawing area without any button being
     * pressed. */
    if (button != MOUSE_DRAG)
    {
	gui_mouse_moved(x, y);
	return TRUE;
    }

    /* translate modifier coding between the main engine and GTK */
    vim_modifiers = 0x0;
    if (state & GDK_SHIFT_MASK)
	vim_modifiers |= MOUSE_SHIFT;
    if (state & GDK_CONTROL_MASK)
	vim_modifiers |= MOUSE_CTRL;
    if (state & GDK_MOD1_MASK)
	vim_modifiers |= MOUSE_ALT;

    /* inform the editor engine about the occurence of this event */
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);
    if (gtk_main_level() > 0)
	gtk_main_quit();

    /*
     * Auto repeat timer handling.
     */
    if (x < 0 || y < 0
	    || x >= gui.drawarea->allocation.width
	    || y >= gui.drawarea->allocation.height)
    {

	int dx;
	int dy;
	int offshoot;
	int delay = 10;

	/* Calculate the maximal distance of the cursor from the drawing area.
	 * (offshoot can't become negative here!).
	 */
	dx = x < 0 ? -x : x - gui.drawarea->allocation.width;
	dy = y < 0 ? -y : y - gui.drawarea->allocation.height;

	offshoot = dx > dy ? dx : dy;

	/* Make a linearly declaying timer delay with a threshold of 5 at a
	 * distance of 127 pixels from the main window.
	 *
	 * One could think endlessly about the most ergonomic variant here.
	 * For example it could make sense to calculate the distance from the
	 * drags start instead...
	 *
	 * Maybe a parabolic interpolation would suite us better here too...
	 */
	if (offshoot > 127)
	{
	    /* 5 appears to be somehow near to my perceptual limits :-). */
	    delay = 5;
	}
	else
	{
	    delay = (130 * (127 - offshoot)) / 127 + 5;
	}

	/* shoot again */
	if (!motion_repeat_timer)
	    motion_repeat_timer = gtk_timeout_add((guint32)delay,
						motion_repeat_timer_cb, NULL);
    }

    return TRUE; /* handled */
}


/*
 * Mouse button handling.  Note please that we are capturing multiple click's
 * by our own timeout mechanism instead of the one provided by GTK+ itself.
 * This is due to the way the generic VIM code is recognizing multiple clicks.
 */
/*ARGSUSED*/
    static gint
button_press_event(GtkWidget * widget, GdkEventButton * event)
{
    int button;
    int repeated_click = FALSE;
    int x, y;
    int_u vim_modifiers;

    /* Make sure we have focus now we've been selected */
    if (gtk_socket_id != 0 && !GTK_WIDGET_HAS_FOCUS(gui.drawarea))
	gtk_widget_grab_focus(gui.drawarea);

    /*
     * Don't let additional events about multiple clicks send by GTK to us
     * after the initial button press event confuse us.
     */
    if (event->type != GDK_BUTTON_PRESS)
	return FALSE;

#ifdef FEAT_XIM
    xim_reset();
#endif

    x = event->x;
    y = event->y;

    /* Handle multiple clicks */
    if (!mouse_timed_out && mouse_click_timer)
    {
	gtk_timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_out = FALSE;
    mouse_click_timer = gtk_timeout_add((guint32)p_mouset,
				  mouse_click_timer_cb, &mouse_timed_out);

    switch (event->button)
    {
    case 1:
	button = MOUSE_LEFT;
	break;
    case 2:
	button = MOUSE_MIDDLE;
	break;
    case 3:
	button = MOUSE_RIGHT;
	break;
    case 4:
	button = MOUSE_4;
	break;
    case 5:
	button = MOUSE_5;
	break;
    default:
	return FALSE;		/* Unknown button */
    }

    vim_modifiers = 0x0;
    if (event->state & GDK_SHIFT_MASK)
	vim_modifiers |= MOUSE_SHIFT;
    if (event->state & GDK_CONTROL_MASK)
	vim_modifiers |= MOUSE_CTRL;
    if (event->state & GDK_MOD1_MASK)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
    if (gtk_main_level() > 0)
	gtk_main_quit();	/* make sure the above will be handled immediately */

    return TRUE;
}

/*ARGSUSED*/
    static gint
button_release_event(GtkWidget * widget, GdkEventButton * event)
{
    int x, y;
    int_u vim_modifiers;

    /* Remove any motion "mashine gun" timers used for automatic further
       extension of allocation areas if outside of the applications window
       area .*/
    if (motion_repeat_timer)
    {
	gtk_timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    x = event->x;
    y = event->y;

    vim_modifiers = 0x0;
    if (event->state & GDK_SHIFT_MASK)
	vim_modifiers |= MOUSE_SHIFT;
    if (event->state & GDK_CONTROL_MASK)
	vim_modifiers |= MOUSE_CTRL;
    if (event->state & GDK_MOD1_MASK)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, vim_modifiers);
    if (gtk_main_level() > 0)
	gtk_main_quit();		/* make sure it will be handled immediately */

    return TRUE;
}


#ifdef GTK_DND
/****************************************************************************
 * Drag aNd Drop support handlers.
 */

/*
 * DND receiver.
 */
/*ARGSUSED*/
    static void
drag_data_received(GtkWidget *widget, GdkDragContext *context,
	gint x, gint y,
	GtkSelectionData *data,
	guint info, guint time)
{
    char_u	**fnames;
    int		redo_dirs = FALSE;
    int		i;
    int		n;
    char	*start;
    char	*stop;
    char	*copy;
    int		nfiles;
    int		url = FALSE;
    GdkModifierType current_modifiers;

    /* Get the current modifier state for proper distinguishment between
     * different operations later. */
    current_modifiers = 0;
    gdk_window_get_pointer(NULL, NULL, NULL, &current_modifiers);

    /* guard against trash */
    if (data->length <= 0
	    || data->format != 8
	    || ((char *)data->data)[data->length] != '\0')
    {
	gtk_drag_finish(context, FALSE, FALSE, time);
	return;
    }

    /* Count how many items there may be and normalize delimiters. */
    n = 1;
    copy = strdup((char *)data->data);
    for (i = 0; i < data->length; ++i)
    {
	if (copy[i] == '\n')
	    ++n;
	else if (copy[i] == '\r')
	{
	    copy[i] = '\n';
	    ++n;
	}
    }

    fnames = (char_u **)alloc((n + 1) * sizeof(char_u *));

    start = copy;
    stop = copy;
    nfiles = 0;
    for (i = 0; i < n; ++i)
    {
	stop = strchr(start, '\n');
	if (stop != NULL)
	    *stop = '\0';

	if (strlen(start) == 0)
	    continue;

	if (strncmp(start, "http://", 7) == 0
		|| strncmp(start, "ftp://", 6) == 0)
	{
	    url = TRUE;
	}
	else if (strncmp(start, "file:", 5) != 0)
	{
	    int j;

	    free(copy);
	    for (j = 0; j < nfiles; ++j)
		free(fnames[j]);
	    gtk_drag_finish(context, FALSE, FALSE, time);

	    return;
	}

	if (strncmp(start, "file://localhost", 16) == 0)
	{
	    fnames[nfiles] = (char_u *)strdup(start + 16);
	    ++nfiles;
	}
	else
	{
	    if (url == FALSE)
	    {
		start += 5;
		while (start[0] == '/' && start[1] == '/')
		    ++start;
	    }
	    fnames[nfiles] = (char_u *)strdup(start);
	    ++nfiles;
	}
	start = stop + 2;
    }
    free(copy);

    /* accept */
    gtk_drag_finish(context, TRUE, FALSE, time);

    /* Real files (i.e. not http and not ftp) */
    if (url == FALSE)
    {
	if (nfiles == 1)
	{
	    if (mch_isdir(fnames[0]))
	    {
		/* Handle dropping a directory on Vim. */
		if (mch_chdir((char *)fnames[0]) == 0)
		{
		    free(fnames[0]);
		    fnames[0] = NULL;
		    redo_dirs = TRUE;
		}
	    }
	}
	else
	{
	    /* Ignore any directories */
	    for (i = 0; i < nfiles; ++i)
	    {
		if (mch_isdir(fnames[i]))
		{
		    vim_free(fnames[i]);
		    fnames[i] = NULL;
		}
	    }
	}

	if (current_modifiers & GDK_SHIFT_MASK)
	{
	    /* Shift held down, change to first file's directory */
	    if (fnames[0] != NULL && vim_chdirfile(fnames[0]) == OK)
		redo_dirs = TRUE;
	}
	else
	{
	    char_u	dirname[MAXPATHL];
	    char_u	*s;

	    /* Shorten dropped file names. */
	    if (mch_dirname(dirname, MAXPATHL) == OK)
		for (i = 0; i < nfiles; ++i)
		    if (fnames[i] != NULL)
		    {
			s = shorten_fname(fnames[i], dirname);
			if (s != NULL && (s = vim_strsave(s)) != NULL)
			{
			    vim_free(fnames[i]);
			    fnames[i] = s;
			}
		    }
	}
    }

    /* Handle the drop, :edit or :split to get to the file */
    handle_drop(nfiles, fnames, current_modifiers & GDK_CONTROL_MASK);

    if (redo_dirs)
	shorten_fnames(TRUE);

    /* Update the screen display */
    update_screen(NOT_VALID);
#ifdef FEAT_MENU
    gui_update_menus(0);
#endif
    setcursor();
    out_flush();
    gui_update_cursor(FALSE, FALSE);
    gui_mch_flush();
}
#endif /* GTK_DND */

/*
 * Setup the WM_PROTOCOLS to indicate we want the WM_SAVE_YOURSELF event.
 * This is an ugly use of X functions.  GTK doesn't offer an alternative.
 */
    static void
setup_save_yourself(void)
{
    Atom	*existing;
    Atom	*new;
    int		count;
    int		i;

    /* first get the existing value */
    if (XGetWMProtocols(GDK_DISPLAY(),
		  GDK_WINDOW_XWINDOW(gui.mainwin->window), &existing, &count))
    {
	/* check if WM_SAVE_YOURSELF isn't there yet */
	for (i = 0; i < count; ++i)
	    if (existing[i] == save_yourself_atom)
		break;
	if (i == count)
	{
	    /* allocate an Atoms array which is one item longer */
	    new = (Atom *)alloc((count + 1) * sizeof(Atom));
	    if (new != NULL)
	    {
		for (i = 0; i < count; ++i)
		    new[i] = existing[i];
		new[count] = save_yourself_atom;
		XSetWMProtocols(GDK_DISPLAY(),
		     GDK_WINDOW_XWINDOW(gui.mainwin->window), new, count + 1);
		vim_free(new);
	    }
	}
	XFree(existing);
    }
}

/*
 * GDK handler for X ClientMessage events.
 */
/*ARGSUSED*/
    static GdkFilterReturn
gdk_wm_protocols_filter(GdkXEvent *xev, GdkEvent *event, gpointer data)
{
    /* From example in gdkevents.c/gdk_wm_protocols_filter */
    XEvent *xevent = (XEvent *)xev;

    if (xevent != NULL)
    {
	if ((Atom)(xevent->xclient.data.l[0]) == save_yourself_atom)
	{
	    out_flush();
	    ml_sync_all(FALSE, FALSE);      /* preserve all swap files */

	    /* Set the window's WM_COMMAND property, to let the window manager
	     * know we are done saving ourselves.  We don't want to be
	     * restarted, thus set argv to NULL. */
	    XSetCommand(gui.dpy, GDK_WINDOW_XWINDOW(gui.mainwin->window),
								     NULL, 0);
	}

	/*
	 * Functionality from gdkevents.c/gdk_wm_protocols_filter;
	 * Registering this filter apparently overrides the default GDK one,
	 * so we need to perform its functionality.  There seems no way to
	 * register for WM_PROTOCOLS, and only process the WM_SAVE_YOURSELF
	 * bit;  it's all or nothing.
	 */
	else if ((Atom)(xevent->xclient.data.l[0]) == gdk_wm_delete_window)
	{
	    event->any.type = GDK_DELETE;
	    return GDK_FILTER_TRANSLATE;
	}
    }

    return GDK_FILTER_REMOVE;
}


/*
 * Setup the window icon & xcmdsrv comm after the main window has been realized.
 */
/*ARGSUSED*/
    static void
mainwin_realize(GtkWidget *widget)
{
/* If you get an error message here, you still need to unpack the runtime
 * archive! */
#ifdef magick
# undef magick
#endif
#define magick vim32x32
#include "../runtime/vim32x32.xpm"
#undef magick
#define magick vim16x16
#include "../runtime/vim16x16.xpm"
#undef magick
#define magick vim48x48
#include "../runtime/vim48x48.xpm"
#undef magick

    static GdkPixmap	*icon = NULL;
    static GdkBitmap	*icon_mask = NULL;
    static char		**magick = vim32x32;
    Window		root_window;
    XIconSize		*size;
    int			number_sizes;

    /* When started with "--echo-wid" argument, write window ID on stdout. */
    if (echo_wid_arg)
    {
	printf("WID: %ld\n", (long)GDK_WINDOW_XWINDOW(gui.mainwin->window));
	fflush(stdout);
    }

    if (vim_strchr(p_go, GO_ICON) != NULL)
    {
	/*
	 * Add an icon to the main window. For fun and convenience of the
	 * user.
	 * Adjust the icon to the preferences of the actual window manager.
	 * This is once again a workaround for a defficiency in GTK+.
	 */
	root_window = XRootWindow(GDK_DISPLAY(), DefaultScreen(GDK_DISPLAY()));
	if (XGetIconSizes(GDK_DISPLAY(), root_window,
						   &size, &number_sizes) != 0)
	{
	    if (number_sizes > 0)
	    {
		if (size->max_height >= 48 && size->max_height >= 48)
		    magick = vim48x48;
		else if (size->max_height >= 32 && size->max_height >= 32)
		    magick = vim32x32;
		else if (size->max_height >= 16 && size->max_height >= 16)
		    magick = vim16x16;
	    }
	}

	if (!icon)
	    icon = gdk_pixmap_create_from_xpm_d(gui.mainwin->window,
		    &icon_mask, NULL, magick);
	gdk_window_set_icon(gui.mainwin->window, NULL, icon, icon_mask);
    }

    /* Register a handler for WM_SAVE_YOURSELF with GDK's low-level X I/F */
    gdk_add_client_message_filter(wm_protocols_atom,
					       gdk_wm_protocols_filter, NULL);

    /* Setup to indicate to the window manager that we want to catch the
     * WM_SAVE_YOURSELF event. */
    setup_save_yourself();

#ifdef FEAT_CLIENTSERVER
    if (serverName == NULL && serverDelayedStartName != NULL)
    {
	/* This is a :gui command in a plain vim with no previous server */
	commWindow = GDK_WINDOW_XWINDOW(gui.mainwin->window);
	(void)serverRegisterName(gui.dpy, serverDelayedStartName);
    }
    else
    {
	/*
	 * Cannot handle "XLib-only" windows with gtk event routines, we'll
	 * have to change the "server" registration to that of the main window
	 * If we have not registered a name yet, remember the window
	 */
	serverChangeRegisteredWindow(GDK_DISPLAY(),
			       GDK_WINDOW_XWINDOW(gui.mainwin->window));
    }
    gtk_widget_add_events (gui.mainwin, GDK_PROPERTY_CHANGE_MASK);
    gtk_signal_connect(GTK_OBJECT(gui.mainwin), "property_notify_event",
		       GTK_SIGNAL_FUNC(property_event), NULL);
#endif
}

/*
 * After the drawing area comes up, we calculate all colors and create the
 * dummy blank cursor.
 *
 * Don't try to set any VIM scrollbar sizes anywhere here. I'm relying on the
 * fact that the main VIM engine doesn't take them into account anywhere.
 */
    static void
drawarea_realize_cb(GtkWidget *widget)
{
    char blank_data[] = {0x0};
    GdkPixmap *blank_mask;
    GdkColor color;
    GtkWidget *sbar;

#ifdef FEAT_XIM
    xim_init();
#endif
    gui_mch_new_colors();
    gui.text_gc = gdk_gc_new(gui.drawarea->window);

    /* Create a pseudo blank pointer, which is in fact one pixel by one pixel
     * in size. */
    blank_mask = gdk_bitmap_create_from_data(NULL, blank_data, 1, 1);
    gdk_color_white(gdk_colormap_get_system(), &color);
    gui.blank_pointer = gdk_cursor_new_from_pixmap(blank_mask, blank_mask,
							&color, &color, 0, 0);
    gdk_bitmap_unref(blank_mask);
    if (gui.pointer_hidden)
	gdk_window_set_cursor(widget->window, gui.blank_pointer);

    /* get the actual size of the scrollbars, if they are realized */
    sbar = firstwin->w_scrollbars[SBAR_LEFT].id;
    if (!sbar || (!gui.which_scrollbars[SBAR_LEFT]
				    && firstwin->w_scrollbars[SBAR_RIGHT].id))
	sbar = firstwin->w_scrollbars[SBAR_RIGHT].id;
    if (sbar && GTK_WIDGET_REALIZED(sbar) && sbar->allocation.width)
	gui.scrollbar_width = sbar->allocation.width;

    sbar = gui.bottom_sbar.id;
    if (sbar && GTK_WIDGET_REALIZED(sbar) && sbar->allocation.height)
	gui.scrollbar_height = sbar->allocation.height;
}

/*
 * Callback routine for the "delete_event" signal on the toplevel window.
 * Tries to vim gracefully, or refuses to exit with changed buffers.
 */
/*ARGSUSED*/
    static int
delete_event_cb(GtkWidget *wgt, gpointer cbdata)
{
    gui_shell_closed();
    return TRUE;
}

static const GtkTargetEntry primary_targets[] = {
    {VIM_ATOM_NAME,   0, SELECTION_VIM},
    {"COMPOUND_TEXT", 0, SELECTION_COMPOUND_TEXT},
    {"TEXT",	      0, SELECTION_TEXT},
    {"STRING",	      0, SELECTION_STRING}
};

/*
 * Initialize the X GUI.  Create all the windows, set up all the call-backs
 * etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    int
gui_mch_init()
{
    GtkWidget *vbox;

    /* Initialize values */
    gui.border_width = 2;
    gui.scrollbar_width = SB_DEFAULT_WIDTH;
    gui.scrollbar_height = SB_DEFAULT_WIDTH;
    gui.fgcolor = g_new0(GdkColor, 1);
    gui.bgcolor = g_new0(GdkColor, 1);

    /* Initialise atoms */
    compound_text_atom = gdk_atom_intern("COMPOUND_TEXT", FALSE);
    text_atom = gdk_atom_intern("TEXT", FALSE);

    /* Set default foreground and background colors. */
    gui.norm_pixel = gui.def_norm_pixel;
    gui.back_pixel = gui.def_back_pixel;

    if (gtk_socket_id != 0)
    {
	GtkPlug *plug;

	/* Use GtkSocket from another app. */
	plug = GTK_PLUG(gui.mainwin = gtk_plug_new(gtk_socket_id));

	/* Pretend we never wanted it if it failed (get own window) */
	if (!plug->socket_window)
	{
	    /* Failed - using straightforward window */
	    gtk_socket_id = 0;
	}
    }
    else
    {
#ifdef FEAT_GUI_GNOME
	if (using_gnome)
	    gui.mainwin = gnome_app_new("vim", "vim");
	else
#endif
	    gui.mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    }
    gtk_window_set_policy(GTK_WINDOW(gui.mainwin), TRUE, TRUE, TRUE);
    gtk_container_border_width(GTK_CONTAINER(gui.mainwin), 0);
    gtk_widget_set_events(gui.mainwin, GDK_VISIBILITY_NOTIFY_MASK);
    (void)gtk_signal_connect(GTK_OBJECT(gui.mainwin), "delete_event",
			     GTK_SIGNAL_FUNC(delete_event_cb), NULL);

    gtk_signal_connect(GTK_OBJECT(gui.mainwin), "realize",
				      GTK_SIGNAL_FUNC(mainwin_realize), NULL);

    /* FIXME: this should eventually get the accelgroup of the gui.mainwin */
    gui.accel_group = gtk_accel_group_get_default();

    vbox = gtk_vbox_new(FALSE, 0);
#ifdef FEAT_GUI_GNOME
    if (using_gnome)
	gnome_app_set_contents(GNOME_APP(gui.mainwin), vbox);
    else
#endif
    {
	gtk_container_add(GTK_CONTAINER(gui.mainwin), vbox);
	gtk_widget_show(vbox);
    }

#ifdef FEAT_MENU
    /* create the menubar and handle */
    gui.menubar = gtk_menu_bar_new();
    gtk_widget_show(gui.menubar);
# ifdef FEAT_GUI_GNOME
    if (using_gnome)
    {
	gui.menubar_h = gnome_dock_item_new("VimMainMenu",
					    GNOME_DOCK_ITEM_BEH_EXCLUSIVE |
					    GNOME_DOCK_ITEM_BEH_NEVER_VERTICAL);
	gtk_widget_show(gui.menubar_h);
	gtk_container_add(GTK_CONTAINER(gui.menubar_h), gui.menubar);
	gnome_dock_add_item(GNOME_DOCK(GNOME_APP(gui.mainwin)->dock),
			    GNOME_DOCK_ITEM(gui.menubar_h),
			    GNOME_DOCK_TOP, /* placement */
			    1,	/* band_num */
			    0,	/* band_position */
			    0,	/* offset */
			    TRUE);
    }
    else
# endif	/* FEAT_GUI_GNOME */
	gtk_box_pack_start(GTK_BOX(vbox), gui.menubar, FALSE, TRUE, 0);
#endif	/* FEAT_MENU */

#ifdef FEAT_TOOLBAR
    /* create the toolbar */
    if (strstr((const char *)p_toolbar, "text")
	    && strstr((const char *)p_toolbar, "icons"))
	gui.toolbar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
    else if (strstr((const char *)p_toolbar, "text"))
	gui.toolbar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_TEXT);
    else
	gui.toolbar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);

    gtk_widget_show(gui.toolbar);
    /* some aesthetics on the toolbar */
    gtk_toolbar_set_button_relief(GTK_TOOLBAR(gui.toolbar), GTK_RELIEF_NONE);

# ifdef FEAT_GUI_GNOME
    if (using_gnome && gui.toolbar)
    {
	GtkWidget *dockitem;
	dockitem = gnome_dock_item_new("VimToolBar",
				       GNOME_DOCK_ITEM_BEH_EXCLUSIVE);
	gtk_container_add(GTK_CONTAINER(dockitem), GTK_WIDGET(gui.toolbar));
	gui.toolbar_h = dockitem;
	gtk_widget_show(gui.toolbar_h);
	gnome_dock_add_item(GNOME_DOCK(GNOME_APP(gui.mainwin)->dock),
			    GNOME_DOCK_ITEM(dockitem),
			    GNOME_DOCK_TOP,	/* placement */
			    1,  /* band_num */
			    1,  /* band_position */
			    0,  /* offset */
			    TRUE);
	gtk_container_border_width(GTK_CONTAINER(gui.toolbar), 2);
    }
    else if (!using_gnome)
# endif	/* FEAT_GUI_GNOME */
    {
	gtk_container_border_width(GTK_CONTAINER(gui.toolbar), 1);
	gtk_box_pack_start(GTK_BOX(vbox), gui.toolbar, FALSE, TRUE, 0);
    }
#endif	/* USE_TOOLBARS */

    gui.formwin = gtk_form_new();
    gtk_container_border_width(GTK_CONTAINER(gui.formwin), 0);
    gtk_widget_set_events(gui.formwin, GDK_EXPOSURE_MASK);

    gui.drawarea = gtk_drawing_area_new();

    /* Determine which events we will filter. */
    gtk_widget_set_events(gui.drawarea,
			  GDK_EXPOSURE_MASK |
			  GDK_ENTER_NOTIFY_MASK |
			  GDK_LEAVE_NOTIFY_MASK |
			  GDK_BUTTON_PRESS_MASK |
			  GDK_BUTTON_RELEASE_MASK |
			  GDK_POINTER_MOTION_MASK |
			  GDK_POINTER_MOTION_HINT_MASK);

    gtk_widget_show(gui.drawarea);
    gtk_form_put(GTK_FORM(gui.formwin), gui.drawarea, 0, 0);
    gtk_widget_show(gui.formwin);
    gtk_box_pack_start(GTK_BOX(vbox), gui.formwin, TRUE, TRUE, 0);

    if (gtk_socket_id != 0)
	/* For GtkSockets, key-presses must go to the focus widget (drawarea)
	 * and not the window. */
	gtk_signal_connect(GTK_OBJECT(gui.drawarea), "key_press_event",
			   (GtkSignalFunc)key_press_event, NULL);
    else
	gtk_signal_connect(GTK_OBJECT(gui.mainwin), "key_press_event",
			   (GtkSignalFunc)key_press_event, NULL);

    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "realize",
		       GTK_SIGNAL_FUNC(drawarea_realize_cb), NULL);

    gui.visibility = GDK_VISIBILITY_UNOBSCURED;
    wm_protocols_atom = gdk_atom_intern("WM_PROTOCOLS", FALSE);
    save_yourself_atom = gdk_atom_intern("WM_SAVE_YOURSELF", FALSE);
    reread_rcfiles_atom = gdk_atom_intern("_GTK_READ_RCFILES", FALSE);

    if (gtk_socket_id != 0)
	/* make sure keybord input can go to the drawarea */
	GTK_WIDGET_SET_FLAGS(gui.drawarea, GTK_CAN_FOCUS);

    /*
     * Set clipboard specific atoms
     */
    vim_atom = gdk_atom_intern(VIM_ATOM_NAME, FALSE);
    clip_star.gtk_sel_atom = GDK_SELECTION_PRIMARY;
    clip_plus.gtk_sel_atom = gdk_atom_intern("CLIPBOARD", FALSE);

    /*
     * Start out by adding the configured border width into the border offset.
     */
    gui.border_offset = gui.border_width;

    gtk_signal_connect(GTK_OBJECT(gui.mainwin), "visibility_notify_event",
		       GTK_SIGNAL_FUNC(visibility_event), NULL);
    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "expose_event",
		       GTK_SIGNAL_FUNC(expose_event), NULL);

    /*
     * Only install these enter/leave callbacks when 'p' in 'guioptions'.
     * Only needed for some window managers.
     */
    if (vim_strchr(p_go, GO_POINTER) != NULL)
    {
	gtk_signal_connect(GTK_OBJECT(gui.drawarea), "leave_notify_event",
			   GTK_SIGNAL_FUNC(leave_notify_event), NULL);
	gtk_signal_connect(GTK_OBJECT(gui.drawarea), "enter_notify_event",
			   GTK_SIGNAL_FUNC(enter_notify_event), NULL);
    }

    gtk_signal_connect(GTK_OBJECT(gui.mainwin), "focus_out_event",
			   GTK_SIGNAL_FUNC(focus_out_event), NULL);
    gtk_signal_connect(GTK_OBJECT(gui.mainwin), "focus_in_event",
			   GTK_SIGNAL_FUNC(focus_in_event), NULL);

    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "motion_notify_event",
		       GTK_SIGNAL_FUNC(motion_notify_event), NULL);
    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "button_press_event",
		       GTK_SIGNAL_FUNC(button_press_event), NULL);
    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "button_release_event",
		       GTK_SIGNAL_FUNC(button_release_event), NULL);

    /*
     * Add selection handler functions.
     */
    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "selection_clear_event",
		       GTK_SIGNAL_FUNC(selection_clear_event), NULL);
    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "selection_received",
		       GTK_SIGNAL_FUNC(selection_received_event), NULL);

    /*
     * Add selection targets for PRIMARY and CLIPBOARD selections.
     */
    gtk_selection_add_targets(gui.drawarea, (long)GDK_SELECTION_PRIMARY,
	    primary_targets,
	    sizeof(primary_targets)/sizeof(primary_targets[0]));
    gtk_selection_add_targets(gui.drawarea, (long)clip_plus.gtk_sel_atom,
	    primary_targets,
	    sizeof(primary_targets)/sizeof(primary_targets[0]));
    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "selection_get",
		       GTK_SIGNAL_FUNC(selection_get_event), NULL);

    /* Pretend we don't have input focus, we will get an event if we do. */
    gui.in_focus = FALSE;

    return OK;
}


/*
 * Called when the foreground or background color has been changed.
 */
    void
gui_mch_new_colors()
{
    /* This used to change the graphics contexts directly but we are currently
     * manipulating them where desired.
     */
    if (gui.drawarea && gui.drawarea->window)
    {
	GdkColor color;
	color.pixel = gui.back_pixel;
	gdk_window_set_background(gui.drawarea->window, &color);
    }
}

    static void
update_window_manager_hints(void)
{
    int width;
    int height;
    GdkGeometry geometry;
    GdkWindowHints geometry_mask;
    static int old_width = 0;
    static int old_height = 0;
    static int old_char_width = 0;
    static int old_char_height = 0;

    /* This also needs to be done when the main window isn't there yet,
     * otherwise the hints don't work. */
    width = gui_get_base_width();
    height = gui_get_base_height();

    /* Avoid an expose event when the size didn't change. */
    if (width != old_width
	    || height != old_height
	    || gui.char_width != old_char_width
	    || gui.char_height != old_char_height)
    {
	geometry_mask =
		     GDK_HINT_BASE_SIZE|GDK_HINT_RESIZE_INC|GDK_HINT_MIN_SIZE;
	geometry.width_inc = gui.char_width;
	geometry.height_inc = gui.char_height;
	geometry.base_width = width;
	geometry.base_height = height;
	geometry.min_width = width + MIN_COLUMNS * gui.char_width;
	geometry.min_height = height + MIN_LINES * gui.char_height;
	gtk_window_set_geometry_hints(GTK_WINDOW(gui.mainwin), gui.formwin,
				      &geometry, geometry_mask);
	old_width = width;
	old_height = height;
	old_char_width = gui.char_width;
	old_char_height = gui.char_height;
    }
}

/*
 * This signal informs us about the need to rearrange our sub-widgets.
 */
/*ARGSUSED*/
    static gint
form_configure_event(GtkWidget * widget, GdkEventConfigure * event)
{
    gtk_form_freeze(GTK_FORM(gui.formwin));
    gui_resize_shell(event->width, event->height);
    gtk_form_thaw(GTK_FORM(gui.formwin));

    return TRUE;
}

/*
 * X11 based inter client communication handler.
 */
/*ARGSUSED*/
    static gint
client_event_cb(GtkWidget *widget, GdkEventClient *event)
{
    if (event->message_type == save_yourself_atom)
    {
	/* NOTE: this is never reached!  See gdk_wm_protocols_filter(). */
	out_flush();
	ml_sync_all(FALSE, FALSE);      /* preserve all swap files */
	return TRUE;
    }
    else if (event->message_type == reread_rcfiles_atom)
    {
	gui_mch_new_colors();
	return TRUE;
    }
    return FALSE;
}

/*
 * Function called when window already closed.
 * We can't do much more here than to trying to preserve what had been done,
 * since the window is already inevitably going away.
 */
    static void
destroy_callback(void)
{
    /* preserve files and exit */
    preserve_exit();
    if (gtk_main_level() > 0)
	gtk_main_quit();
}


/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    int
gui_mch_open()
{
    int x = -1, y = -1;

    if (gui_win_x != -1 && gui_win_y != -1)
	gtk_widget_set_uposition(gui.mainwin, gui_win_x, gui_win_y);

    /* Determine user specified geometry, if present. */
    if (gui.geom != NULL)
    {
	int mask;
	unsigned w, h;

	mask = XParseGeometry((char *)gui.geom, &x, &y, &w, &h);
	if (mask & WidthValue)
	    Columns = w;
	if (mask & HeightValue)
	    Rows = h;
	if (mask & (XValue | YValue))
	    gtk_widget_set_uposition(gui.mainwin, x, y);
	g_free(gui.geom);
	gui.geom = NULL;
    }

    gtk_form_set_size(GTK_FORM(gui.formwin),
	    (guint)(gui_get_base_width() + Columns * gui.char_width),
	    (guint)(gui_get_base_height() + Rows * gui.char_height));
    update_window_manager_hints();

    if (found_reverse_arg)
    {
	gui.def_norm_pixel = gui_get_color((char_u *)"White");
	gui.def_back_pixel = gui_get_color((char_u *)"Black");
    }
    else
    {
	gui.def_norm_pixel = gui_get_color((char_u *)"Black");
	gui.def_back_pixel = gui_get_color((char_u *)"White");
    }

    /* Get the colors from the "Normal" and "Menu" group (set in syntax.c or
     * in a vimrc file)
     */
    set_normal_colors();

    /* Check that none of the colors are the same as the background color */
    gui_check_colors();

    /* Get the colors for the highlight groups (gui_check_colors() might have
     * changed them).
     */
    highlight_gui_started();	/* re-init colors and fonts */

    gtk_signal_connect(GTK_OBJECT(gui.mainwin), "destroy",
		       GTK_SIGNAL_FUNC(destroy_callback), NULL);

    /* Make this run after any internal handling of the client event happened
     * to make sure that all changes implicated by it are already in place and
     * we thus can make our own adjustments.
     */
    gtk_signal_connect_after(GTK_OBJECT(gui.mainwin), "client_event",
		    GTK_SIGNAL_FUNC(client_event_cb), NULL);

#ifdef FEAT_HANGULIN
    hangul_keyboard_set();
#endif

    /*
     * Notify the fixed area about the need to resize the contents of the
     * gui.formwin, which we use for random positioning of the included
     * components.
     *
     * We connect this signal deferred finally after anything is in place,
     * since this is intended to handle resizements coming from the window
     * manager upon us and should not interfere with what VIM is requesting
     * upon startup.
     */
    gtk_signal_connect(GTK_OBJECT(gui.formwin), "configure_event",
		       GTK_SIGNAL_FUNC(form_configure_event), NULL);

#ifdef GTK_DND
    /*
     * Set up for receiving DND items.
     */
    gtk_drag_dest_set(gui.drawarea,
	    GTK_DEST_DEFAULT_ALL,
	    target_table, n_targets,
	    GDK_ACTION_COPY | GDK_ACTION_MOVE);

    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "drag_data_received",
	    GTK_SIGNAL_FUNC(drag_data_received), NULL);
#endif

    gtk_widget_show(gui.mainwin);

    return OK;
}


/*ARGSUSED*/
    void
gui_mch_exit(int rc)
{
    gtk_exit(0);
}

/*
 * Get the position of the top left corner of the window.
 */
    int
gui_mch_get_winpos(int *x, int *y)
{
    /* For some people this must be gdk_window_get_origin() for a correct
     * result.  Where is the documentation! */
    gdk_window_get_root_origin(gui.mainwin->window, x, y);
    return OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    void
gui_mch_set_winpos(int x, int y)
{
    gdk_window_move(gui.mainwin->window, x, y);
}

/*
 * Set the windows size.
 */
/*ARGSUSED*/
    void
gui_mch_set_shellsize(int width, int height,
	int min_width, int min_height, int base_width, int base_height)
{
    /* Hack: When the form already is at the desired size, the window might
     * have been resized with the mouse.  Force a resize by setting a
     * different size first. */
    if (GTK_FORM(gui.formwin)->width == width
	    && GTK_FORM(gui.formwin)->height == height)
    {
	gtk_form_set_size(GTK_FORM(gui.formwin), width + 1, height + 1);
	gui_mch_update();
    }

    gtk_form_set_size(GTK_FORM(gui.formwin), width, height);

    /* give GTK+ a chance to put all widget's into place */
    gui_mch_update();

    /* this will cause the proper resizement to happen too */
    update_window_manager_hints();
}


/*
 * The screen size is used to make sure the initial window doesn't get bigger
 * then the screen.  This subtracts some room for menubar, toolbar and window
 * decorations.
 */
    void
gui_mch_get_screen_dimensions(int *screen_w, int *screen_h)
{
    *screen_w = gdk_screen_width();
    /* Subtract 'guihearroom' from the height to allow some room for the
     * window manager (task list and window title bar). */
    *screen_h = gdk_screen_height() - p_ghr;

    /*
     * FIXME: dirty trick: Because the gui_get_base_height() doesn't include
     * the toolbar and menubar for GTK, we subtract them from the screen
     * hight, so that the window size can be made to fit on the screen.
     * This should be completely changed later.
     */
#ifdef FEAT_TOOLBAR
    if (gui.toolbar && GTK_WIDGET_REALIZED(gui.toolbar)
	    && GTK_WIDGET_VISIBLE(gui.toolbar))
	*screen_h -= gui.toolbar->allocation.height;
#endif
#ifdef FEAT_MENU
    if (gui.menubar && GTK_WIDGET_REALIZED(gui.menubar)
	    && GTK_WIDGET_VISIBLE(gui.menubar))
	*screen_h -= gui.menubar->allocation.height;
#endif
}

#if defined(FEAT_MENU) || defined(PROTO)
    void
gui_mch_enable_menu(int flag)
{
    if (flag)
	gtk_widget_show(gui.menubar);
    else
	gtk_widget_hide(gui.menubar);

    update_window_manager_hints();
}
#endif


#if defined(FEAT_TOOLBAR) || defined(PROTO)

    void
gui_mch_show_toolbar(int showit)
{
    if (gui.toolbar == NULL)
	return;

    if (!showit)
    {
# ifdef FEAT_GUI_GNOME
	if (using_gnome && GTK_WIDGET_VISIBLE(gui.toolbar_h))
	{
	    gtk_widget_hide(gui.toolbar_h);
	    /* wait util this gets done on the server side. */
	    update_window_manager_hints();
	}
# endif
	if (!using_gnome && GTK_WIDGET_VISIBLE(gui.toolbar))
	{
	    gtk_widget_hide(gui.toolbar);
	    /* wait util this gets done on the server side. */
	    update_window_manager_hints();
	}
    }
    else
    {
	if (strstr((const char *)p_toolbar, "text")
		&& strstr((const char *)p_toolbar, "icons"))
	    gtk_toolbar_set_style(GTK_TOOLBAR(gui.toolbar), GTK_TOOLBAR_BOTH);
	else if (strstr((const char *)p_toolbar, "text"))
	    gtk_toolbar_set_style(GTK_TOOLBAR(gui.toolbar), GTK_TOOLBAR_TEXT);
	else if (strstr((const char *)p_toolbar, "icons"))
	    gtk_toolbar_set_style(GTK_TOOLBAR(gui.toolbar), GTK_TOOLBAR_ICONS);

# ifdef FEAT_GUI_GNOME
	if (using_gnome && !GTK_WIDGET_VISIBLE(gui.toolbar_h))
	{
	    gtk_widget_show(gui.toolbar_h);
	    update_window_manager_hints();
	}
	else
# endif
	if (!using_gnome && !GTK_WIDGET_VISIBLE(gui.toolbar))
	{
	    gtk_widget_show(gui.toolbar);
	    update_window_manager_hints();
	}

	if (strstr((const char *)p_toolbar, "tooltips"))
	    gtk_toolbar_set_tooltips(GTK_TOOLBAR(gui.toolbar), TRUE);
	else
	    gtk_toolbar_set_tooltips(GTK_TOOLBAR(gui.toolbar), FALSE);
    }
}
#endif


/*
 * Get a font structure for highlighting.
 * "cbdata" is a pointer to the global gui structure.
 */
/*ARGSUSED*/
    static void
font_sel_ok(GtkWidget *wgt, gpointer cbdata)
{
    gui_T *vw = (gui_T *)cbdata;
    GtkFontSelectionDialog *fs = (GtkFontSelectionDialog *)vw->fontdlg;

    if (vw->fontname)
	g_free(vw->fontname);

    vw->fontname = (char_u *)g_strdup(
				gtk_font_selection_dialog_get_font_name(fs));
    gtk_widget_hide(vw->fontdlg);
    if (gtk_main_level() > 0)
	gtk_main_quit();
}

/*ARGSUSED*/
    static void
font_sel_cancel(GtkWidget *wgt, gpointer cbdata)
{
    gui_T *vw = (gui_T *)cbdata;

    gtk_widget_hide(vw->fontdlg);
    if (gtk_main_level() > 0)
	gtk_main_quit();
}

/*ARGSUSED*/
    static void
font_sel_destroy(GtkWidget *wgt, gpointer cbdata)
{
    gui_T *vw = (gui_T *)cbdata;

    vw->fontdlg = NULL;
    if (gtk_main_level() > 0)
	gtk_main_quit();
}

    int
gui_mch_adjust_charsize(void)
{
    gui.char_height = gui.current_font->ascent + gui.current_font->descent
		      + p_linespace;
    gui.char_ascent = gui.current_font->ascent + p_linespace / 2;
    return OK;
}

#if defined(FEAT_XFONTSET) || defined(PROTO)
/*
 * Try to load the requested fontset.
 */
/*ARGSUSED*/
    GuiFontset
gui_mch_get_fontset(char_u *name, int report_error, int fixed_width)
{
    GdkFont *font;

    if (!gui.in_use || name == NULL)
	return NOFONT;

    font = gdk_fontset_load((gchar *)name);

    if (font == NULL)
    {
	if (report_error)
	    EMSG2(_("E234: Unknown fontset: %s"), name);
	return NOFONT;
    }
    /* TODO: check if the font is fixed width. */

    /* reference this font as beeing in use */
    gdk_font_ref(font);

    return (GuiFontset)font;
}
#endif

/*
 * Put up a font dialog and return the selected font name in allocated memory.
 * "oldval" is the previous value.
 * Return NULL when cancelled.
 */
    char_u *
gui_mch_font_dialog(char_u *oldval)
{
    if (!gui.fontdlg)
    {
	GtkFontSelectionDialog	*fsd = NULL;

	gui.fontdlg = gtk_font_selection_dialog_new(_("Font Selection"));
	fsd = GTK_FONT_SELECTION_DIALOG(gui.fontdlg);
	gtk_window_set_modal(GTK_WINDOW(gui.fontdlg), TRUE);
	gtk_window_set_transient_for(GTK_WINDOW(gui.fontdlg),
		GTK_WINDOW(gui.mainwin));
	gtk_signal_connect(GTK_OBJECT(gui.fontdlg), "destroy",
		GTK_SIGNAL_FUNC(font_sel_destroy), &gui);
	gtk_signal_connect(GTK_OBJECT(fsd->ok_button), "clicked",
		GTK_SIGNAL_FUNC(font_sel_ok), &gui);
	gtk_signal_connect(GTK_OBJECT(fsd->cancel_button), "clicked",
		GTK_SIGNAL_FUNC(font_sel_cancel), &gui);
    }

    if (oldval != NULL && *oldval != NUL)
	gtk_font_selection_dialog_set_font_name(
		GTK_FONT_SELECTION_DIALOG(gui.fontdlg), (char *)oldval);

    if (gui.fontname)
    {
	g_free(gui.fontname);
	gui.fontname = NULL;
    }
    gtk_window_position(GTK_WINDOW(gui.fontdlg), GTK_WIN_POS_MOUSE);
    gtk_widget_show(gui.fontdlg);
    {
	static gchar	*spacings[] = {"c", "m", NULL};

	/* In GTK 1.2.3 this must be after the gtk_widget_show() call,
	 * otherwise everything is blocked for ten seconds. */
	gtk_font_selection_dialog_set_filter(
		GTK_FONT_SELECTION_DIALOG(gui.fontdlg),
		GTK_FONT_FILTER_BASE,
		GTK_FONT_ALL, NULL, NULL,
		NULL, NULL, spacings, NULL);
    }

    /* Wait for the font dialog to be closed. */
    while (gui.fontdlg && GTK_WIDGET_DRAWABLE(gui.fontdlg))
	gtk_main_iteration_do(TRUE);

    if (gui.fontname == NULL)
	return NULL;
    return vim_strsave(gui.fontname);
}

/*
 * There is only one excuse I can give for the following attempt to manage font
 * styles:
 *
 * I HATE THE BRAIN DEAD WAY X11 IS HANDLING FONTS (--mdcki)
 */
    static void
get_styled_font_variants(char_u * font_name)
{
    char	*chunk[32];
    char	*sdup;
    char	*tmp;
    int		len, i;
    GuiFont	*styled_font[3];

    styled_font[0] = &gui.bold_font;
    styled_font[1] = &gui.ital_font;
    styled_font[2] = &gui.boldital_font;

    /* First free whatever was freviously there. */
    for (i = 0; i < 3; ++i)
	if (*styled_font[i])
	{
	    gdk_font_unref(*styled_font[i]);
	    *styled_font[i] = NULL;
	}

    if ((sdup = g_strdup((const char *)font_name)) == NULL)
	return;

    /* split up the whole */
    i = 0;
    for (tmp = sdup; *tmp != '\0'; ++tmp)
    {
	if (*tmp == '-')
	{
	    *tmp = '\0';

	    if (i == 32)
		break;

	    chunk[i] = tmp + 1;
	    ++i;
	}
    }

    if (i == 14)
    {
	GdkFont		*font = NULL;
	const char	*bold_chunk[3]	    = { "bold", NULL,	"bold" };
	const char	*italic_chunk[3]    = { NULL,	"o",	"o" };

	/* font name was complete */
	len = strlen((const char *)font_name) + 32;

	for (i = 0; i < 3; ++i)
	{
	    char *styled_name;
	    int j;

	    styled_name = (char *)alloc(len);
	    if (styled_name == NULL)
	    {
		g_free(sdup);
		return;
	    }

	    *styled_name = '\0';

	    for (j = 0; j < 14; ++j)
	    {
		strcat(styled_name, "-");
		if (j == 2 && bold_chunk[i] != NULL)
		    strcat(styled_name, bold_chunk[i]);
		else if (j == 3 && italic_chunk[i] != NULL)
		    strcat(styled_name, italic_chunk[i]);
		else
		    strcat(styled_name, chunk[j]);
	    }

	    font = gui_mch_get_font((char_u *)styled_name, FALSE);
	    if (font != NULL)
		*styled_font[i] = font;

	    vim_free(styled_name);
	}
    }

    g_free(sdup);
}

/*
 * Initialize Vim to use the font or fontset with the given name.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
/*ARGSUSED*/
    int
gui_mch_init_font(char_u *font_name, int fontset)
{
    GdkFont	*font = NULL;

#ifdef FEAT_XFONTSET
    /* Try loading a fontset.  If this fails we try loading a normal font. */
    if (fontset && font_name != NULL)
	font = gui_mch_get_fontset(font_name, TRUE, TRUE);

    if (font == NULL)
#endif
    {
	/* If font_name is NULL, this means to use the default, which should
	 * be present on all X11 servers. */
	if (font_name == NULL)
	    font_name = (char_u *)DFLT_FONT;
	font = gui_mch_get_font(font_name, FALSE);
    }

    if (font == NULL)
	return FAIL;

    gui_mch_free_font(gui.norm_font);
#ifdef FEAT_XFONTSET
    gui_mch_free_fontset(gui.fontset);
    if (font->type == GDK_FONT_FONTSET)
    {
	gui.norm_font = NOFONT;
	gui.fontset = (GuiFontset)font;
	/* Use two bytes, this works around the problem that the result would
	 * be zero if no 8-bit font was found. */
	gui.char_width = gdk_string_width(font, "xW") / 2;
    }
    else
#endif
    {
	gui.norm_font = font;
#ifdef FEAT_XFONTSET
	gui.fontset = NOFONTSET;
#endif
	gui.char_width = ((XFontStruct *)
				      GDK_FONT_XFONT(font))->max_bounds.width;
    }

    /* A zero width may cause a crash.  Happens for semi-invalid fontsets. */
    if (gui.char_width <= 0)
	gui.char_width = 8;

    gui.char_height = font->ascent + font->descent + p_linespace;
    gui.char_ascent = font->ascent + p_linespace / 2;

    /* Set the fontname, which will be used for information purposes */
    hl_set_font_name(font_name);

    if (font->type != GDK_FONT_FONTSET)
	get_styled_font_variants(font_name);

    /* Synchronize the fonts used in user input dialogs, since otherwise
     * search/replace will be esp. annoying in case of international font
     * usage.
     */
    gui_gtk_synch_fonts();

#ifdef FEAT_XIM
    /* Adjust input management behaviour to the capabilities of the new
     * fontset */
    xim_decide_input_style();
    if (xim_get_status_area_height())
    {
	/* Status area is required.  Just create the empty label so that
	 * mainwin will allocate the extra space for status area. */
	GtkWidget *label = gtk_label_new("       ");

	gtk_widget_set_usize(label, 20, gui.char_height + 2);
	gtk_box_pack_end(GTK_BOX(GTK_BIN(gui.mainwin)->child), label,
							     FALSE, FALSE, 0);
	gtk_widget_show(label);
    }
#endif

    /* Preserve the logical dimensions of the screen. */
    update_window_manager_hints();

    return OK;
}

/*
 * Get a reference to the font "name".
 * Return zero for failure.
 */
    GuiFont
gui_mch_get_font(char_u *name, int report_error)
{
    GdkFont *font;

    /* can't do this when GUI is not running */
    if (!gui.in_use || name == NULL)
	return NOFONT;

    font = gdk_font_load((const gchar *)name);

    if (font == NULL)
    {
	if (report_error)
	    EMSG2(_("E235: Unknown font: %s"), name);
	return NOFONT;
    }

    /* reference this font as being in use */
    gdk_font_ref(font);

    /* Check that this is a mono-spaced font. */
    if (((XFontStruct *)GDK_FONT_XFONT(font))->max_bounds.width
	    != ((XFontStruct *)GDK_FONT_XFONT(font))->min_bounds.width)
    {
	EMSG2(_("E236: Font \"%s\" is not fixed-width"), name);
	gdk_font_unref(font);

	return NOFONT;
    }

    return (GuiFont)font;
}

/*
 * Set the current text font.
 * Since we create all GC on demand, we use just gui.current_font to
 * indicate the desired current font.
 */
    void
gui_mch_set_font(GuiFont font)
{
    gui.current_font = font;
}

#if defined(FEAT_XFONTSET) || defined(PROTO)
/*
 * Set the current text fontset.
 */
    void
gui_mch_set_fontset(GuiFontset fontset)
{
    gui.current_font = fontset;
}
#endif

/*
 * If a font is not going to be used, free its structure.
 */
    void
gui_mch_free_font(GuiFont font)
{
    if (font != NOFONT)
	gdk_font_unref((GdkFont *)font);
}

#if defined(FEAT_XFONTSET) || defined(PROTO)
/*
 * If a fontset is not going to be used, free its structure.
 */
    void
gui_mch_free_fontset(GuiFontset fontset)
{
    if (fontset != NOFONTSET)
	gdk_font_unref((GdkFont *)fontset);
}
#endif


/*
 * Return the Pixel value (color) for the given color name.  This routine was
 * pretty much taken from example code in the Silicon Graphics OSF/Motif
 * Programmer's Guide.
 * Return -1 for error.
 */
    guicolor_T
gui_mch_get_color(char_u * name)
{
    int i;
    static char *(vimnames[][2]) =
    {
    /* A number of colors that some X11 systems don't have */
	{"LightRed",	 "#FFBBBB"},
	{"LightGreen",	 "#88FF88"},
	{"LightMagenta", "#FFBBFF"},
	{"DarkCyan",	 "#008888"},
	{"DarkBlue",	 "#0000BB"},
	{"DarkRed",	 "#BB0000"},
	{"DarkMagenta",  "#BB00BB"},
	{"DarkGrey",	 "#BBBBBB"},
	{"DarkYellow",	 "#BBBB00"},
	{NULL, NULL}
    };

    if (!gui.in_use)		/* can't do this when GUI not running */
	return (guicolor_T)(-1);

    while (name != NULL)
    {
	GdkColor	color;
	int		parsed;

	/* Since we have already called gtk_set_locale here the bugger
	 * XParseColor will accept only explicit color names in the language
	 * of the current locale.  However this will interferre with:
	 * 1. Vim's global startup files
	 * 2. Explicit color names in .vimrc
	 *
	 * Therefore we first try to parse the color in the current locale and
	 * if it fails, we fall back to the portable "C" one.
	 */

	parsed = gdk_color_parse((const gchar *)name, &color);

	if (!parsed)
	{
	    char *current;

	    current = setlocale(LC_ALL, NULL);
	    if (current != NULL)
	    {
		char *saved;

		saved = strdup(current);
		setlocale(LC_ALL, "C");
		parsed = gdk_color_parse((const gchar *)name, &color);
		setlocale(LC_ALL, saved);
		gtk_set_locale();
		free(saved);
	    }
	}

	if (parsed)
	{
	    GdkColormap *colormap;
	    colormap = gtk_widget_get_colormap(gui.drawarea);
	    gdk_color_alloc(colormap, &color);

	    return (guicolor_T) color.pixel;
	}
	/* add a few builtin names and try again */
	for (i = 0; ; ++i)
	{
	    if (vimnames[i][0] == NULL)
	    {
		name = NULL;
		break;
	    }
	    if (STRICMP(name, vimnames[i][0]) == 0)
	    {
		name = (char_u *)vimnames[i][1];
		break;
	    }
	}
    }

    return (guicolor_T)(-1);
}

/*
 * Set the current text foreground color.
 */
    void
gui_mch_set_fg_color(guicolor_T color)
{
    gui.fgcolor->pixel = (Pixel) color;
}

/*
 * Set the current text background color.
 */
    void
gui_mch_set_bg_color(guicolor_T color)
{
    gui.bgcolor->pixel = (Pixel) color;
}

    void
gui_mch_draw_string(int row, int col, char_u *s, int len, int flags)
{
    static XChar2b	*buf = NULL;
    static int		buflen = 0;
    int			is_wide;
    XChar2b		*text;
    int			textlen;
    XFontStruct		*xfont;
    char_u		*p;
#ifdef FEAT_MBYTE
    unsigned		c;
#endif
    int			width;

    if (gui.current_font == NULL || gui.drawarea->window == NULL)
	return;

    gdk_gc_set_exposures(gui.text_gc,
				 gui.visibility != GDK_VISIBILITY_UNOBSCURED);

    /*
     * Yeah yeah apparently the font support in GTK+ only cares for either:
     * asians or 8-bit fonts. It is broken there, but no wonder the whole font
     * stuff is broken in X11 in first place. And the internationalization API
     * isn't something you would really like to use.
     */
    xfont = (XFontStruct *)((GdkFontPrivate*)gui.current_font)->xfont;
    is_wide = ((xfont->min_byte1 != 0 || xfont->max_byte1 != 0)
#ifdef FEAT_XFONTSET
	    && gui.fontset == NOFONTSET
#endif
	    );
    if (is_wide)
    {
	/* Convert a byte sequence to 16 bit characters for the Gdk functions.
	 * Need a buffer for the 16 bit characters.  Keep it between calls,
	 * because allocating it each time is slow. */
	if (buflen < len)
	{
	    XtFree((char *)buf);
	    buf = (XChar2b *)XtMalloc(len * sizeof(XChar2b));
	    buflen = len;
	}

	p = s;
	textlen = 0;
	width = 0;
	while (p < s + len)
	{
#ifdef FEAT_MBYTE
	    if (enc_utf8)
	    {
		c = utf_ptr2char(p);
		if (c >= 0x10000)	/* show chars > 0xffff as ? */
		    c = 0xbf;
		buf[textlen].byte1 = c >> 8;
		buf[textlen].byte2 = c;
		p += utf_ptr2len_check(p);
		width += utf_char2cells(c);
	    }
	    else
#endif
	    {
		buf[textlen].byte1 = '\0';	/* high eight bits */
		buf[textlen].byte2 = *p;	/* low eight bits */
		++p;
		++width;
	    }
	    ++textlen;
	}
	text = buf;
	textlen = textlen * 2;
    }
    else
    {
	text = (XChar2b *)s;
	textlen = len;
#ifdef FEAT_MBYTE
	if (has_mbyte)
	{
	    width = 0;
	    for (p = s; p < s + len; p += (*mb_ptr2len_check)(p))
		width += (*mb_ptr2cells)(p);
	}
	else
#endif
	    width = len;
    }

    if (!(flags & DRAW_TRANSP))
    {
	gdk_gc_set_foreground(gui.text_gc, gui.bgcolor);
	gdk_draw_rectangle(gui.drawarea->window,
			   gui.text_gc,
			   TRUE,
			   FILL_X(col), FILL_Y(row),
			   width * gui.char_width, gui.char_height);
    }
    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
    gdk_draw_text(gui.drawarea->window,
		  gui.current_font,
		  gui.text_gc,
		  TEXT_X(col), TEXT_Y(row),
		  (const gchar *)text, textlen);

    /* redraw the contents with an offset of 1 to emulate bold */
    if (flags & DRAW_BOLD)
	gdk_draw_text(gui.drawarea->window,
		      gui.current_font,
		      gui.text_gc,
		      TEXT_X(col) + 1, TEXT_Y(row),
		      (const gchar *)text, textlen);

    if (flags & DRAW_UNDERL)
    {
	gdk_draw_line(gui.drawarea->window,
		      gui.text_gc, FILL_X(col),
	FILL_Y(row + 1) - 1, FILL_X(col + width) - 1, FILL_Y(row + 1) - 1);
    }
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    int
gui_mch_haskey(char_u * name)
{
    int i;

    for (i = 0; special_keys[i].key_sym != 0; i++)
	if (name[0] == special_keys[i].code0
		&& name[1] == special_keys[i].code1)
	    return OK;
    return FAIL;
}

#if defined(FEAT_TITLE) || defined(FEAT_XIM) || defined(PROTO)
/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    int
gui_get_x11_windis(Window * win, Display ** dis)
{
    *dis = GDK_DISPLAY();
    if (gui.mainwin != NULL && gui.mainwin->window)
    {
	*win = GDK_WINDOW_XWINDOW(gui.mainwin->window);
	return OK;
    }
    *win = 0;
    return FAIL;
}
#endif

    void
gui_mch_beep()
{
    gdk_beep();
}

    void
gui_mch_flash(int msec)
{
    GdkGCValues values;
    GdkGC *invert_gc;
    GdkColor foreground;
    GdkColor background;

    if (gui.drawarea->window == NULL)
	return;

    foreground.pixel = gui.norm_pixel ^ gui.back_pixel;
    background.pixel = gui.norm_pixel ^ gui.back_pixel;

    values.foreground = foreground;
    values.background = background;
    values.function = GDK_XOR;
    invert_gc = gdk_gc_new_with_values(gui.drawarea->window,
				       &values,
				       GDK_GC_FOREGROUND |
				       GDK_GC_BACKGROUND |
				       GDK_GC_FUNCTION);
    gdk_gc_set_exposures(invert_gc, gui.visibility != GDK_VISIBILITY_UNOBSCURED);

    /* Do a visual beep by changing back and forth in some undetermined way,
     * the foreground and background colors.  This is due to the fact that
     * there can't be really any prediction about the effects of XOR on
     * arbitrary X11 servers. However this seems to be enough for what we
     * intend it to do.
     */
    gdk_draw_rectangle(gui.drawarea->window, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((int)Columns) + gui.border_offset,
		       FILL_Y((int)Rows) + gui.border_offset);

    gdk_flush();
    ui_delay((long)msec, TRUE);	/* wait so many msec */
    gdk_draw_rectangle(gui.drawarea->window, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((int)Columns) + gui.border_offset,
		       FILL_Y((int)Rows) + gui.border_offset);

    gdk_gc_destroy(invert_gc);
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    void
gui_mch_invert_rectangle(int r, int c, int nr, int nc)
{
    GdkGCValues values;
    GdkGC *invert_gc;
    GdkColor foreground;
    GdkColor background;

    if (gui.drawarea->window == NULL)
	return;

    foreground.pixel = gui.norm_pixel ^ gui.back_pixel;
    background.pixel = gui.norm_pixel ^ gui.back_pixel;

    values.foreground = foreground;
    values.background = background;
    values.function = GDK_XOR;
    invert_gc = gdk_gc_new_with_values(gui.drawarea->window,
				       &values,
				       GDK_GC_FOREGROUND |
				       GDK_GC_BACKGROUND |
				       GDK_GC_FUNCTION);
    gdk_gc_set_exposures(invert_gc, gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    gdk_draw_rectangle(gui.drawarea->window, invert_gc,
		       TRUE,
		       FILL_X(c), FILL_Y(r),
		       (nc) * gui.char_width, (nr) * gui.char_height);
    gdk_gc_destroy(invert_gc);
}

/*
 * Iconify the GUI window.
 */
    void
gui_mch_iconify()
{
    XIconifyWindow(GDK_DISPLAY(),
		   GDK_WINDOW_XWINDOW(gui.mainwin->window),
		   DefaultScreen(GDK_DISPLAY()));
}

#if defined(FEAT_EVAL) || defined(PROTO)
/*
 * Bring the Vim window to the foreground.
 */
    void
gui_mch_set_foreground()
{
    gdk_window_raise(gui.mainwin->window);
}
#endif

/*
 * Draw a cursor without focus.
 */
    void
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    int		i = 1;

    if (gui.drawarea->window == NULL)
	return;

    gui_mch_set_fg_color(color);

    gdk_gc_set_exposures(gui.text_gc,
				 gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
#ifdef FEAT_MBYTE
    if (mb_lefthalve(gui.row, gui.col))
	i = 2;
#endif
    gdk_draw_rectangle(gui.drawarea->window, gui.text_gc,
	    FALSE,
	    FILL_X(gui.col), FILL_Y(gui.row),
	    i * gui.char_width - 1, gui.char_height - 1);
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    void
gui_mch_draw_part_cursor(int w, int h, guicolor_T color)
{
    if (gui.drawarea->window == NULL)
	return;

    gui_mch_set_fg_color(color);

    gdk_gc_set_exposures(gui.text_gc,
				 gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
    gdk_draw_rectangle(gui.drawarea->window, gui.text_gc,
	    TRUE,
#ifdef FEAT_RIGHTLEFT
	    /* vertical line should be on the right of current point */
	    !(State & CMDLINE) && curwin->w_p_rl ? FILL_X(gui.col + 1) - w :
#endif
	    FILL_X(gui.col),
	    FILL_Y(gui.row) + gui.char_height - h + (int)p_linespace / 2,
	    w, h - (int)p_linespace);
}


/*
 * Catch up with any queued X11 events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X11 event queue (& no timers pending), then we return
 * immediately.
 */
    void
gui_mch_update()
{
    while (gtk_events_pending() && !vim_is_input_buf_full())
	gtk_main_iteration_do(FALSE);
}

    static gint
input_timer_cb(gpointer data)
{
    int *timed_out = (int *) data;

    /* Just inform the caller about the occurence of it */
    *timed_out = TRUE;

    if (gtk_main_level() > 0)
	gtk_main_quit();

    return FALSE;		/* don't happen again */
}

#ifdef FEAT_SNIFF
/*
 * Callback function, used when data is available on the SNiFF connection.
 */
/* ARGSUSED */
    static void
sniff_request_cb(
    gpointer	data,
    gint	source_fd,
    GdkInputCondition condition)
{
    static char_u bytes[3] = {CSI, (int)KS_EXTRA, (int)KE_SNIFF};

    add_to_input_buf(bytes, 3);

    if (gtk_main_level() > 0)
	gtk_main_quit();
}
#endif

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1     Wait forever.
 *  wtime == 0      This should never happen.
 *  wtime > 0       Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    int
gui_mch_wait_for_chars(long wtime)
{
    int focus;
    guint timer;
    static int timed_out;
#ifdef FEAT_SNIFF
    static int	sniff_on = 0;
    static gint	sniff_input_id = 0;
#endif

#ifdef FEAT_SNIFF
    if (sniff_on && !want_sniff_request)
    {
	if (sniff_input_id)
	    gdk_input_remove(sniff_input_id);
	sniff_on = 0;
    }
    else if (!sniff_on && want_sniff_request)
    {
	/* Add fd_from_sniff to watch for available data in main loop. */
	sniff_input_id = gdk_input_add(fd_from_sniff,
			       GDK_INPUT_READ, sniff_request_cb, (gpointer)0);
	sniff_on = 1;
    }
#endif

    timed_out = FALSE;

    /* this timeout makes sure that we will return if no characters arrived in
     * time */

    if (wtime > 0)
	timer = gtk_timeout_add((guint32)wtime, input_timer_cb, &timed_out);
    else
	timer = 0;

    focus = gui.in_focus;

    do
    {
	/* Stop or start blinking when focus changes */
	if (gui.in_focus != focus)
	{
	    if (gui.in_focus)
		gui_mch_start_blink();
	    else
		gui_mch_stop_blink();
	    focus = gui.in_focus;
	}

	/*
	 * Loop in GTK+ processing  until a timeout or input occurs.
	 */
	gtk_main();

	/* Got char, return immediately */
	if (!vim_is_input_buf_empty())
	{
	    if (timer != 0 && !timed_out)
		gtk_timeout_remove(timer);
	    return OK;
	}
    } while (wtime < 0 || !timed_out);

    /*
     * Flush all eventually pending (drawing) events.
     */
    gui_mch_update();

    return FAIL;
}


/****************************************************************************
 * Output drawing routines.
 ****************************************************************************/


/* Flush any output to the screen */
    void
gui_mch_flush()
{
    gdk_flush();
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    void
gui_mch_clear_block(int row1, int col1, int row2, int col2)
{
    GdkColor color;

    if (gui.drawarea->window == NULL)
	return;

    color.pixel = gui.back_pixel;

    gdk_gc_set_exposures(gui.text_gc,
				 gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    gdk_gc_set_foreground(gui.text_gc, &color);

    /* Clear one extra pixel at the far right, for when bold characters have
     * spilled over to the window border. */
    gdk_draw_rectangle(gui.drawarea->window, gui.text_gc, TRUE,
		       FILL_X(col1), FILL_Y(row1),
		       (col2 - col1 + 1) * gui.char_width
						      + (col2 == Columns - 1),
		       (row2 - row1 + 1) * gui.char_height);
}

    void
gui_mch_clear_all(void)
{
    if (gui.drawarea->window == NULL)
	return;

    gdk_window_clear(gui.drawarea->window);
}

/*
 * Redraw any text revealed by scrolling up/down.
 */
    static void
check_copy_area(void)
{
    XEvent event;
    XGraphicsExposeEvent *gevent;

    if (gui.visibility != GDK_VISIBILITY_PARTIAL)
	return;

    gdk_flush();

    /* Wait to check whether the scroll worked or not. */
    for (;;)
    {
	if (XCheckTypedEvent(GDK_DISPLAY(), NoExpose, &event))
	    return;		/* The scroll worked. */

	if (XCheckTypedEvent(GDK_DISPLAY(), GraphicsExpose, &event))
	{
	    gevent = (XGraphicsExposeEvent *) & event;
	    gui_redraw(gevent->x, gevent->y, gevent->width, gevent->height);
	    if (gevent->count == 0)
		return;		/* This was the last expose event */
	}
	gdk_flush();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    void
gui_mch_delete_lines(int row, int num_lines)
{
    if (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	return;			/* Can't see the window */

    gdk_gc_set_exposures(gui.text_gc,
			     gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
    gdk_gc_set_background(gui.text_gc, gui.bgcolor);

    /* copy one extra pixel, for when bold has spilled over */
    gdk_window_copy_area(gui.drawarea->window, gui.text_gc,
	    FILL_X(gui.scroll_region_left), FILL_Y(row),
	    gui.drawarea->window,
	    FILL_X(gui.scroll_region_left),
	    FILL_Y(row + num_lines),
	    gui.char_width * (gui.scroll_region_right
					    - gui.scroll_region_left + 1) + 1,
	    gui.char_height * (gui.scroll_region_bot - row - num_lines + 1));

    gui_clear_block(gui.scroll_region_bot - num_lines + 1,
						       gui.scroll_region_left,
		    gui.scroll_region_bot, gui.scroll_region_right);
    check_copy_area();
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    void
gui_mch_insert_lines(int row, int num_lines)
{
    if (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	return;			/* Can't see the window */

    gdk_gc_set_exposures(gui.text_gc,
				 gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
    gdk_gc_set_background(gui.text_gc, gui.bgcolor);

    /* copy one extra pixel, for when bold has spilled over */
    gdk_window_copy_area(gui.drawarea->window, gui.text_gc,
	    FILL_X(gui.scroll_region_left), FILL_Y(row + num_lines),
	    gui.drawarea->window,
	    FILL_X(gui.scroll_region_left), FILL_Y(row),
	    gui.char_width * (gui.scroll_region_right
					    - gui.scroll_region_left + 1) + 1,
	    gui.char_height * (gui.scroll_region_bot - row - num_lines + 1));

    gui_clear_block(row, gui.scroll_region_left,
				row + num_lines - 1, gui.scroll_region_right);
    check_copy_area();
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */
    void
clip_mch_request_selection(cbd)
    VimClipboard *cbd;
{
    /* First try to get the content of our own special clipboard. */
    received_selection = RS_NONE;
    (void)gtk_selection_convert(gui.drawarea,
				    cbd->gtk_sel_atom, vim_atom,
				    (guint32)GDK_CURRENT_TIME);
    while (received_selection == RS_NONE)
	gtk_main();		/* wait for selection_received_event */

    if (received_selection == RS_FAIL)
    {
	/* Now try to get it out of the usual string selection. */
	received_selection = RS_NONE;
	(void)gtk_selection_convert(gui.drawarea, cbd->gtk_sel_atom,
				    gdk_atom_intern("COMPOUND_TEXT", FALSE),
				    (guint32)GDK_CURRENT_TIME);
	while (received_selection == RS_NONE)
	    gtk_main();		/* wait for selection_received_event */
    }
    if (received_selection == RS_FAIL)
    {
	/* Now try to get it out of the usual string selection. */
	received_selection = RS_NONE;
	(void)gtk_selection_convert(gui.drawarea, cbd->gtk_sel_atom,
				    gdk_atom_intern("TEXT", FALSE),
				    (guint32)GDK_CURRENT_TIME);
	while (received_selection == RS_NONE)
	    gtk_main();		/* wait for selection_received_event */
    }
    if (received_selection == RS_FAIL)
    {
	/* Now try to get it out of the usual string selection. */
	received_selection = RS_NONE;
	(void)gtk_selection_convert(gui.drawarea, cbd->gtk_sel_atom,
				    (GdkAtom)GDK_TARGET_STRING,
				    (guint32)GDK_CURRENT_TIME);
	while (received_selection == RS_NONE)
	    gtk_main();		/* wait for selection_received_event */
    }
    if (received_selection == RS_FAIL)
    {
	/* Final fallback position - use the X CUT_BUFFER0 store */
	int     nbytes = 0;
	char_u *buffer;

	buffer = (char_u *)XFetchBuffer(gui.dpy, &nbytes, 0);
	if (nbytes > 0)
	{
	    /* Got something */
	    clip_yank_selection(MCHAR, buffer, (long)nbytes, cbd);
	    XFree((void *)buffer);
	    if (p_verbose > 0)
		smsg((char_u *)_("Used CUT_BUFFER0 instead of empty selection") );
	}
    }
}

/*
 * Disown the selection.
 */
/*ARGSUSED*/
    void
clip_mch_lose_selection(cbd)
    VimClipboard *cbd;
{
    /* WEIRD: when using NULL to actually disown the selection, we lose the
     * selection the first time we own it. */
    /*
    gtk_selection_owner_set(NULL, cbd->gtk_sel_atom, (guint32)GDK_CURRENT_TIME);
    gui_mch_update();
     */
}

/*
 * Own the selection and return OK if it worked.
 */
    int
clip_mch_own_selection(cbd)
    VimClipboard *cbd;
{
    int r;

    r = gtk_selection_owner_set(gui.drawarea, cbd->gtk_sel_atom,
						   (guint32)GDK_CURRENT_TIME);
    gui_mch_update();
    if (r)
	return OK;
    return FAIL;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
/*ARGSUSED*/
    void
clip_mch_set_selection(cbd)
    VimClipboard* cbd;
{
}


#if defined(FEAT_MENU) || defined(PROTO)
/*
 * Make a menu item appear either active or not active (grey or not grey).
 */
    void
gui_mch_menu_grey(vimmenu_T *menu, int grey)
{
    if (menu->id == 0)
	return;

    if (menu_is_separator(menu->name))
	grey = 1;

    gui_mch_menu_hidden(menu, FALSE);
    /* Be clever about bitfields versus true booleans here! */
    if (!GTK_WIDGET_SENSITIVE(menu->id) == !grey)
    {
	gtk_widget_set_sensitive(menu->id, !grey);
	gui_mch_update();
    }
}

/*
 * Make menu item hidden or not hidden.
 */
    void
gui_mch_menu_hidden(vimmenu_T *menu, int hidden)
{
    if (menu->id == 0)
	return;

    if (hidden)
    {
	if (GTK_WIDGET_VISIBLE(menu->id))
	{
	    gtk_widget_hide(menu->id);
	    gui_mch_update();
	}
    }
    else
    {
	if (!GTK_WIDGET_VISIBLE(menu->id))
	{
	    gtk_widget_show(menu->id);
	    gui_mch_update();
	}
    }
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    void
gui_mch_draw_menubar()
{
    /* just make sure that the visual changes get effect immediately */
    gui_mch_update();
}
#endif

/*
 * Scrollbar stuff.
 */
    void
gui_mch_enable_scrollbar(scrollbar_T *sb, int flag)
{
    if (sb->id == 0)
	return;
    if (flag)
	gtk_widget_show(sb->id);
    else
	gtk_widget_hide(sb->id);
    update_window_manager_hints();
}


/*
 * Return the RGB value of a pixel as long.
 */
    long_u
gui_mch_get_rgb(guicolor_T pixel)
{
    GdkVisual		*visual;
    GdkColormap		*cmap;
    GdkColorContext	*cc;
    GdkColor		c;

    visual = gtk_widget_get_visual(gui.mainwin);
    cmap = gtk_widget_get_colormap(gui.mainwin);
    cc = gdk_color_context_new(visual, cmap);

    c.pixel = pixel;
    gdk_color_context_query_color(cc, &c);

    return ((c.red & 0xff00) << 8) + (c.green & 0xff00)
						    + ((unsigned)c.blue >> 8);
}

/*
 * Get current y mouse coordinate in text window.
 * Return -1 when unknown.
 */
    int
gui_mch_get_mouse_x(void)
{
    int winx, winy;
    GdkModifierType mask;

    gdk_window_get_pointer(gui.drawarea->window, &winx, &winy, &mask);
    return winx;
}

    int
gui_mch_get_mouse_y(void)
{
    int winx, winy;
    GdkModifierType mask;

    gdk_window_get_pointer(gui.drawarea->window, &winx, &winy, &mask);
    return winy;
}

    void
gui_mch_setmouse(int x, int y)
{
    /* Sorry for the Xlib call, but we can't avoid it, since there is no
     * internal GDK mechanism present to accomplish this.
     */
    XWarpPointer(GDK_DISPLAY(), (Window) 0,
		  GDK_WINDOW_XWINDOW(gui.drawarea->window), 0, 0, 0, 0, x, y);
}

#ifdef FEAT_MOUSESHAPE
/* The last set mouse pointer shape is remembered, to be used when it goes
 * from hidden to not hidden. */
static int last_shape = 0;
#endif

/*
 * Use the blank mouse pointer or not.
 *
 * hide: TRUE = use blank ptr, FALSE = use parent ptr
 */
    void
gui_mch_mousehide(int hide)
{
    if (gui.pointer_hidden != hide)
    {
	gui.pointer_hidden = hide;
	if (gui.drawarea->window && gui.blank_pointer)
	{
	    if (hide)
		gdk_window_set_cursor(gui.drawarea->window, gui.blank_pointer);
	    else
#ifdef FEAT_MOUSESHAPE
		mch_set_mouse_shape(last_shape);
#else
		gdk_window_set_cursor(gui.drawarea->window, NULL);
#endif
	}
    }
}

#if defined(FEAT_MOUSESHAPE) || defined(PROTO)

/* Table for shape IDs.  Keep in sync with the mshape_names[] table in
 * misc2.c! */
static int mshape_ids[] =
{
    GDK_LEFT_PTR,		/* arrow */
    0,				/* blank */
    GDK_XTERM,			/* beam */
    GDK_SB_V_DOUBLE_ARROW,	/* updown */
    GDK_SIZING,			/* udsizing */
    GDK_SB_H_DOUBLE_ARROW,	/* leftright */
    GDK_SIZING,			/* lrsizing */
    GDK_WATCH,			/* busy */
    GDK_X_CURSOR,		/* no */
    GDK_CROSSHAIR,		/* crosshair */
    GDK_HAND1,			/* hand1 */
    GDK_HAND2,			/* hand2 */
    GDK_PENCIL,			/* pencil */
    GDK_QUESTION_ARROW,		/* question */
    GDK_RIGHT_PTR,		/* right-arrow */
    GDK_CENTER_PTR,		/* up-arrow */
    GDK_LEFT_PTR		/* last one */
};

    void
mch_set_mouse_shape(shape)
    int	shape;
{
    int		   id;
    GdkCursor	   *c;

    if (!gui.drawarea->window)
	return;

    if (shape == MSHAPE_HIDE || gui.pointer_hidden)
	gdk_window_set_cursor(gui.drawarea->window, gui.blank_pointer);
    else
    {
	if (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    if (id >= GDK_NUM_GLYPHS)
		id = GDK_LEFT_PTR;
	    else
		id &= ~1;	/* they are always even (why?) */
	}
	else
	    id = mshape_ids[shape];

	c = gdk_cursor_new(id);
	gdk_window_set_cursor(gui.drawarea->window, c);
	gdk_cursor_destroy(c);
    }
    if (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif
