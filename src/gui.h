/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/* For debugging */
/* #define D(x)	printf x; */
#define D(x)

#ifdef USE_GUI_MOTIF
# define USE_GUI_X11
# include <Xm/Xm.h>
#endif

#ifdef USE_GUI_ATHENA
# define USE_GUI_X11
# include <X11/Intrinsic.h>
# include <X11/StringDefs.h>
#endif

#ifdef USE_GUI_WIN32
# include <windows.h>
#endif

#ifdef USE_GUI_BEOS
# include "gui_beos.h"
#endif

#ifdef USE_GUI_MAC
# include <Types.h>
/*# include <Memory.h>*/
# include <Quickdraw.h>
# include <Fonts.h>
# include <Events.h>
# include <Menus.h>
# include <Windows.h>
# include <Controls.h>
/*# include <TextEdit.h>*/
# include <Dialogs.h>
# include <OSUtils.h>
/*
# include <ToolUtils.h>
# include <SegLoad.h>*/
#endif

/* In the GUI we always have the clipboard and the mouse */
#ifndef USE_CLIPBOARD
# define USE_CLIPBOARD
#endif
#ifndef USE_MOUSE
# define USE_MOUSE
#endif

/*
 * On some systems, when we compile with the GUI, we always use it.  On Mac
 * there is no terminal version, and on Windows we can't figure out how to
 * fork one off with :gui.
 */
#if defined(USE_GUI_WIN32) || defined(USE_GUI_MAC)
# define ALWAYS_USE_GUI
#endif

/*
 * These macros convert between character row/column and pixel coordinates.
 * TEXT_X   - Convert character column into X pixel coord for drawing strings.
 * TEXT_Y   - Convert character row into Y pixel coord for drawing strings.
 * FILL_X   - Convert character column into X pixel coord for filling the area
 *		under the character.
 * FILL_Y   - Convert character row into Y pixel coord for filling the area
 *		under the character.
 * X_2_COL  - Convert X pixel coord into character column.
 * Y_2_ROW  - Convert Y pixel coord into character row.
 */
#define TEXT_X(col)	((col) * gui.char_width  + gui.border_offset)
#define TEXT_Y(row)	((row) * gui.char_height + gui.char_ascent \
						 + gui.border_offset)
#define FILL_X(col)	((col) * gui.char_width  + gui.border_offset)
#define FILL_Y(row)	((row) * gui.char_height + gui.border_offset)
#define X_2_COL(x)	(((x) - gui.border_offset) / gui.char_width)
#define Y_2_ROW(y)	(((y) - gui.border_offset) / gui.char_height)

/* Indices into GuiMenu->strings[] and GuiMenu->noremap[] for each mode */
#define MENU_INDEX_INVALID	-1
#define MENU_INDEX_NORMAL	0
#define MENU_INDEX_VISUAL	1
#define MENU_INDEX_OP_PENDING	2
#define MENU_INDEX_INSERT	3
#define MENU_INDEX_CMDLINE	4
#define MENU_MODES		5

/* Menu modes */
#define MENU_NORMAL_MODE	(1 << MENU_INDEX_NORMAL)
#define MENU_VISUAL_MODE	(1 << MENU_INDEX_VISUAL)
#define MENU_OP_PENDING_MODE	(1 << MENU_INDEX_OP_PENDING)
#define MENU_INSERT_MODE	(1 << MENU_INDEX_INSERT)
#define MENU_CMDLINE_MODE	(1 << MENU_INDEX_CMDLINE)
#define MENU_ALL_MODES		((1 << MENU_MODES) - 1)

/* The character for each menu mode */
#define MENU_MODE_CHARS		"nvoic"

/* Indices for arrays of scrollbars */
#define SBAR_NONE	    -1
#define SBAR_LEFT	    0
#define SBAR_RIGHT	    1
#define SBAR_BOTTOM	    2

/* Orientations for scrollbars */
#define SBAR_VERT	    0
#define SBAR_HORIZ	    1

/* Default size of scrollbar */
#define SB_DEFAULT_WIDTH    16

/* Default height of the menu bar */
#define MENU_DEFAULT_HEIGHT 1		    /* figure it out at runtime */

/* Flags for gui_mch_outstr_nowrap() */
#define GUI_MON_WRAP_CURSOR	0x01	    /* wrap cursor at end of line */
#define GUI_MON_INVERT		0x02	    /* invert the characters */
#define GUI_MON_IS_CURSOR	0x04	    /* drawing cursor */
#define GUI_MON_TRS_CURSOR	0x08	    /* drawing transparent cursor */

/* Flags for gui_mch_draw_string() */
#define DRAW_TRANSP		0x01	    /* draw with transparant bg */
#define DRAW_BOLD		0x02	    /* draw bold text */
#define DRAW_UNDERL		0x04	    /* draw underline text */

typedef struct GuiMenu
{
    int		modes;		    /* Which modes is this menu visible for? */
    char_u	*name;		    /* Name shown in menu */
    int		priority;	    /* Menu order priority */
    void	(*cb)();	    /* Call-back routine */
    char_u	*strings[MENU_MODES]; /* Mapped string for each mode */
    int		noremap[MENU_MODES]; /* A noremap flag for each mode */
    struct GuiMenu *children;	    /* Children of sub-menu */
    struct GuiMenu *next;	    /* Next item in menu */
#ifdef USE_GUI_X11
    Widget	id;		    /* Manage this to enable item */
    Widget	submenu_id;	    /* If this is submenu, add children here */
#endif
#ifdef USE_GUI_WIN32
    UINT	id;		    /* Id of menu item */
    HMENU	submenu_id;	    /* If this is submenu, add children here */
#endif
#if USE_GUI_BEOS
    BMenuItem	*id;		    /* Id of menu item */
    BMenu	*submenu_id;	    /* If this is submenu, add children here */
#endif
#ifdef macintosh
    MenuHandle	id;
    struct GuiMenu *father;	    /* The father of the menu */
    short	index;		    /* the item index within the father menu */
    short	menu_id;	    /* the menu id to which this item belong */
    short	submenu_id;	    /* the menu id of the children (could be
				       get throught some tricks) */
    MenuHandle	menu_handle;
    MenuHandle	submenu_handle;
#endif
} GuiMenu;

typedef struct GuiScrollbar
{
    long	ident;		    /* Unique identifier for each scrollbar */
    struct window *wp;		    /* Scrollbar's window, NULL for bottom */
    int		value;		    /* Represents top line number visible */
    int		size;		    /* Size of scrollbar thumb */
    int		max;		    /* Number of lines in buffer */

    /* Values measured in characters: */
    int		top;		    /* Top of scroll bar (chars from row 0) */
    int		height;		    /* Height of scroll bar (num rows) */
    int		status_height;	    /* Height of status line */
#ifdef USE_GUI_X11
    Widget	id;		    /* Id of real scroll bar */
#endif
#ifdef USE_GUI_WIN32
    HWND	id;		    /* Id of real scroll bar */
#endif
#if USE_GUI_BEOS
    VimScrollBar *id;		    /* Pointer to real scroll bar */
#endif
#ifdef macintosh
    ControlHandle id;		    /* A handle to the scrollbar */
#endif
} GuiScrollbar;

typedef long	    GuiColor;	    /* handle for a GUI color */
typedef long_u	    GuiFont;	    /* handle for a GUI font */

typedef struct Gui
{
    int		in_focus;	    /* Vim has input focus */
    int		in_use;		    /* Is the GUI being used? */
    int		starting;	    /* GUI will start in a little while */
    int		window_created;	    /* Has the window been created yet? */
    int		dying;		    /* Is vim dying? Then output to terminal */
    int		dofork;		    /* Use fork() when GUI is starting */
    int		dragged_sb;	    /* Which scrollbar being dragged, if any? */
    struct window   *dragged_wp;    /* Which WIN's sb being dragged, if any? */
    int		pointer_hidden;	    /* Is the mouse pointer hidden? */
    int		col;		    /* Current cursor column in GUI display */
    int		row;		    /* Current cursor row in GUI display */
    int		cursor_col;	    /* Physical cursor column in GUI display */
    int		cursor_row;	    /* Physical cursor row in GUI display */
    char	cursor_is_valid;    /* There is a cursor at cursor_row/col */
    int		num_cols;	    /* Number of columns */
    int		num_rows;	    /* Number of rows */
    int		scroll_region_top;  /* Top (first) line of scroll region */
    int		scroll_region_bot;  /* Bottom (last) line of scroll region */
    int		highlight_mask;	    /* Highlight attribute mask */
    GuiMenu	*root_menu;	    /* Root of menu hierarchy */
    int		scrollbar_width;    /* Width of vertical scrollbars */
    int		scrollbar_height;   /* Height of horizontal scrollbar */
    int		left_sbar_x;	    /* Calculated x coord for left scrollbar */
    int		right_sbar_x;	    /* Calculated x coord for right scrollbar */
    int		menu_height;	    /* Height of the menu bar */
    int		menu_width;	    /* Width of the menu bar */
    char	menu_is_active;	    /* TRUE if menu is present */
    char	menu_height_fixed;  /* TRUE if menu height fixed */
    GuiScrollbar bottom_sbar;	    /* Bottom scrollbar */
    int		which_scrollbars[3];/* Which scrollbar boxes are active? */
    int		prev_wrap;	    /* For updating the horizontal scrollbar */
    int		char_width;	    /* Width of char in pixels */
    int		char_height;	    /* Height of char in pixels */
    int		char_ascent;	    /* Ascent of char in pixels */
    int		border_width;	    /* Width of our border around text area */
    int		border_offset;	    /* Total pixel offset for all borders */
    GuiFont	norm_font;
    GuiFont	bold_font;
    GuiFont	ital_font;
    GuiFont	boldital_font;
    GuiColor	back_pixel;	    /* Color of background */
    GuiColor	norm_pixel;	    /* Color of normal text */
    GuiColor	def_back_pixel;	    /* default Color of background */
    GuiColor	def_norm_pixel;	    /* default Color of normal text */
#ifdef USE_GUI_X11
    GuiColor	menu_fg_pixel;	    /* Color of menu foregound */
    GuiColor	menu_bg_pixel;	    /* Color of menu backgound */
    GuiColor	scroll_fg_pixel;    /* Color of scrollbar foregrnd */
    GuiColor	scroll_bg_pixel;    /* Color of scrollbar backgrnd */
    Display	*dpy;		    /* X display */
    Window	wid;		    /* Window id of text area */
    int		visibility;	    /* Is window partially/fully obscured? */
    GC		text_gc;
    GC		back_gc;
    GC		invert_gc;
    Cursor	blank_pointer;	    /* Blank pointer */

    /* X Resources */
    char_u	*dflt_font;	    /* Resource font, used if 'font' not set */
    char_u	*dflt_bold_fn;	    /* Resource bold font */
    char_u	*dflt_ital_fn;	    /* Resource italic font */
    char_u	*dflt_boldital_fn;  /* Resource bold-italic font */
    char_u	*geom;		    /* Geometry, eg "80x24" */
    Bool	rev_video;	    /* Use reverse video? */
#endif
#ifdef USE_GUI_WIN32
    GuiFont	currFont;	    /* Current font */
    GuiColor	currFgColor;	    /* Current foreground text color */
    GuiColor	currBgColor;	    /* Current background text color */
#endif
#ifdef USE_GUI_BEOS
    VimApp     *vimApp;
    VimWindow  *vimWindow;
    VimFormView *vimForm;
    VimTextAreaView *vimTextArea;
    int		vdcmp;		    /* Vim Direct Communication Message Port */
#endif
#ifdef USE_GUI_MAC
    WindowPtr	VimWindow;
    FontInfo	VimFont;	    /* contians width height ascent,descent*/
    short	VimFontID;

    GuiColor	menu_fg_pixel;	    /* Color of menu foregound */
    GuiColor	menu_bg_pixel;	    /* Color of menu backgound */
    GuiColor	scroll_fg_pixel;    /* Color of scrollbar foregrnd */
    GuiColor	scroll_bg_pixel;    /* Color of scrollbar backgrnd */
    WindowPtr	wid;		    /* Window id of text area */
    int		visibility;	    /* Is window partially/fully obscured? */
    /*	GC			text_gc;
	GC			back_gc;
	GC			invert_gc;
     */
    /* X Resources */
    char_u	*dflt_font;	    /* Resource font, used if 'font' not set */
    char_u	*dflt_bold_fn;	    /* Resource bold font */
    char_u	*dflt_ital_fn;	    /* Resource italic font */
    char_u	*dflt_boldital_fn;  /* Resource bold-italic font */
    char_u	*geom;		    /* Geometry, eg "80x24" */
    char_u	rev_video;	    /* Use reverse video? */
#endif
} Gui;

extern Gui gui;			    /* this is defined in gui.c */
extern int force_menu_update;	    /* this is defined in gui.c */
