/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *				Macintosh port by Dany St-Amant and Axel
 *				Kielhorn
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*#define USE_SIOUX*/
#if UNIVERSAL_INTERFACES_VERSION >= 0x0320
# define USE_CTRLCLICKMENU
#endif
#define USE_HELPMENU
#define USE_AEVENT
/*#define USE_OFFSETED_WINDOW*/
#define USE_VIM_CREATOR_ID

#include <Devices.h> /* included first to avoid CR problems */
#include "vim.h"
#include "globals.h"
#include "option.h"
#include <Menus.h>
#include <Resources.h>
#include <StandardFile.h>
#include <Traps.h>
#include <Balloons.h>
#include <Processes.h>
#ifdef USE_AEVENT
# include <AppleEvents.h>
# include <AERegistry.h>
#endif
#ifdef USE_CTRLCLICKMENU
# include <Gestalt.h>
#endif
#ifdef USE_SIOUX
# include <stdio.h>
# include <sioux.h>
# include <console.h>
#endif
#if UNIVERSAL_INTERFACES_VERSION >= 0x0330
# include <ControlDefinitions.h>
#endif

#define kNothing 0
#define kCreateEmpty 2 /*1*/
#define kCreateRect 2
#define kDestroy 3

static long lastMouseTick = 0;

static RgnHandle cursorRgn;
static RgnHandle dragRgn;
static Rect dragRect;
static short dragRectEnbl;
static short dragRectControl;
#ifdef USE_CTRLCLICKMENU
static short clickIsPopup;
#endif
garray_t error_ga = {0, 0, 0, 0, NULL};

ControlActionUPP gScrollAction;
ControlActionUPP gScrollDrag;

RgnHandle	theMovingRgn;

#define RGB(r,g,b)	(r << 16) + (g << 8) + b
#define Red(c)		((c & 0x00FF0000) >> 16)
#define Green(c)	((c & 0x0000FF00) >>  8)
#define Blue(c)		((c & 0x000000FF) >>  0)

static ControlHandle dragged_sb = NULL;


#define vk_Esc		0x35	/* -> 1B */

#define vk_F1		0x7A	/* -> 10 */
#define vk_F2		0x78  /*0x63*/
#define vk_F3		0x63  /*0x76*/
#define vk_F4		0x76  /*0x60*/
#define vk_F5		0x60  /*0x61*/
#define vk_F6		0x61  /*0x62*/
#define vk_F7		0x62  /*0x63*/  /*?*/
#define vk_F8		0x64
#define vk_F9		0x65
#define vk_F10		0x6D
#define vk_F11		0x67
#define vk_F12		0x6F
#define vk_F13		0x69
#define vk_F14		0x6B
#define vk_F15		0x71

#define vk_Clr		0x47	/* -> 1B (ESC) */
#define vk_Enter	0x4C	/* -> 03 */

#define vk_Space	0x31	/* -> 20 */
#define vk_Tab		0x30	/* -> 09 */
#define vk_Return	0x24	/* -> 0D */
#define vk_Delete	0X08	/* -> 08 BackSpace */

#define vk_Help		0x72	/* -> 05 */
#define vk_Home		0x73	/* -> 01 */
#define	vk_PageUp	0x74	/* -> 0D */
#define vk_FwdDelete	0x75	/* -> 7F */
#define	vk_End		0x77	/* -> 04 */
#define vk_PageDown	0x79	/* -> 0C */

#define vk_Up		0x7E	/* -> 1E */
#define vk_Down		0x7D	/* -> 1F */
#define	vk_Left		0x7B	/* -> 1C */
#define vk_Right	0x7C	/* -> 1D */

#define vk_Undo		vk_F1
#define vk_Cut		vk_F2
#define	vk_Copy		vk_F3
#define	vk_Paste	vk_F4
#define vk_PrintScreen	vk_F13
#define vk_SCrollLock	vk_F14
#define	vk_Pause	vk_F15
#define	vk_NumLock	vk_Clr
#define vk_Insert	vk_Help

#define KeySym	char

static struct
{
    KeySym  key_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_keys[] =
{
    {vk_Up,		'k', 'u'},
    {vk_Down,		'k', 'd'},
    {vk_Left,		'k', 'l'},
    {vk_Right,		'k', 'r'},

    {vk_F1,		'k', '1'},
    {vk_F2,		'k', '2'},
    {vk_F3,		'k', '3'},
    {vk_F4,		'k', '4'},
    {vk_F5,		'k', '5'},
    {vk_F6,		'k', '6'},
    {vk_F7,		'k', '7'},
    {vk_F8,		'k', '8'},
    {vk_F9,		'k', '9'},
    {vk_F10,		'k', ';'},

    {vk_F11,		'F', '1'},
    {vk_F12,		'F', '2'},
    {vk_F13,		'F', '3'},
    {vk_F14,		'F', '4'},
    {vk_F15,		'F', '5'},

/*  {XK_Help,		'%', '1'}, */
/*  {XK_Undo,		'&', '8'}, */
    {vk_Delete,		'k', 'b'},
    {vk_Insert,		'k', 'I'},
    {vk_FwdDelete,	'k', 'D'},
    {vk_Home,		'k', 'h'},
    {vk_End,		'@', '7'},
/*  {XK_Prior,		'k', 'P'}, */
/*  {XK_Next,		'k', 'N'}, */
/*  {XK_Print,		'%', '9'}, */

    {vk_PageUp,		'k', 'P'},
    {vk_PageDown,	'k', 'N'},

    /* End of list marker: */
    {(KeySym)0,		0, 0}

/** {XK_BackSpace,	'k', 'b'}, **/

};


short gui_mac_get_menu_item_index (vimmenu_t *menu, vimmenu_t *parent);
GuiFont gui_mac_find_font (char_u *font_name);
#ifdef USE_AEVENT
OSErr HandleUnusedParms (AppleEvent *theAEvent);
pascal OSErr HandleODocAE (AppleEvent *theAEvent, AppleEvent *theReply, long refCon);
pascal OSErr Handle_aevt_oapp_AE (AppleEvent *theAEvent, AppleEvent *theReply, long refCon);
pascal OSErr Handle_aevt_quit_AE (AppleEvent *theAEvent, AppleEvent *theReply, long refCon);
pascal OSErr Handle_aevt_pdoc_AE (AppleEvent *theAEvent, AppleEvent *theReply, long refCon);
pascal OSErr Handle_KAHL_SRCH_AE (AppleEvent *theAEvent, AppleEvent *theReply, long refCon);
pascal OSErr Handle_KAHL_MOD_AE (AppleEvent *theAEvent, AppleEvent *theReply, long refCon);
pascal OSErr Handle_KAHL_GTTX_AE (AppleEvent *theAEvent, AppleEvent *theReply, long refCon);
pascal OSErr Handle_unknown_AE (AppleEvent *theAEvent, AppleEvent *theReply, long refCon);
pascal OSErr FindProcessBySignature ( const OSType targetType, const OSType targetCreator, ProcessSerialNumberPtr psnPtr );
void Send_KAHL_MOD_AE (buf_t *buf);
OSErr   InstallAEHandlers (void);
#endif

static int gui_argc = 0;
static char **gui_argv = NULL;

    short
gui_mac_get_menu_item_index (pMenu, pElderMenu)
    vimmenu_t *pMenu;
    vimmenu_t *pElderMenu;
{
    /* pMenu is the one we inquiries */
    /* pElderMenu the menu where we start looking for */

    short	itemIndex = -1;
    short	index;
    vimmenu_t	*pChildren = pElderMenu->children;;

    while ((pElderMenu != NULL) && (itemIndex == -1))
    {
	pChildren = pElderMenu->children;

	if (pChildren)
	{
	    if (pChildren->menu_id == pMenu->menu_id)
	    {
		for (index = 1;(pChildren != pMenu) && (pChildren != NULL);index++)
		    pChildren = pChildren->next;
		if (pChildren == pMenu)
#ifdef USE_HELPMENU
		{
#endif
		    itemIndex = index;
#ifdef USE_HELPMENU
		    if (pMenu->menu_id == kHMHelpMenuID)
			itemIndex += gui.MacOSHelpItems;
		}
#endif
	    }
	    else
	    {
		itemIndex = gui_mac_get_menu_item_index (pMenu, pChildren);
	    }
	}
	pElderMenu = pElderMenu->next;
    }

    return itemIndex;
}

    static vimmenu_t *
gui_mac_get_vim_menu (menuID, itemIndex, pMenu)
    short	menuID;
    short	itemIndex;
    vimmenu_t	*pMenu;
{
    short	index;
    vimmenu_t	*pChildMenu;

    /* pMenu is the first menu of the level (no next point to it) */
    /* TODO: Help menu would be tricky */

    if (pMenu->menu_id == menuID)
    {
#ifdef USE_HELPMENU
	if (menuID == kHMHelpMenuID)
	    itemIndex -= gui.MacOSHelpItems;
#endif

	for (index = 1; (index != itemIndex) && (pMenu != NULL); index++)
	    pMenu = pMenu->next;
    }
    else
    {
	for (; pMenu != NULL; pMenu = pMenu->next)
	{
	if (pMenu->children != NULL)
	{
	    pChildMenu = gui_mac_get_vim_menu
			   (menuID, itemIndex, pMenu->children);
	    if (pChildMenu)
	    {
		pMenu = pChildMenu;
		break;
	    }
	}
	}
    }
    return pMenu;
}

/*
 * Handle the Update Event
 */

    void
gui_mac_update(event)
    EventRecord	*event;
{
    WindowPtr	whichWindow;
    GrafPtr	savePort;
    RgnHandle	updateRgn;
    Rect	*updateRect;
    Rect	rc;
    Rect	growRect;
    RgnHandle	saveRgn;

    GetPort (&savePort);
    whichWindow = (WindowPtr) event->message;
    SetPort (whichWindow);
      BeginUpdate (whichWindow);
	updateRgn = whichWindow->visRgn;
	HLock ((Handle) updateRgn);
	  updateRect = &(*updateRgn)->rgnBBox;
	  gui_redraw(updateRect->left, updateRect->top,
		      updateRect->right - updateRect->left,
		      updateRect->bottom   - updateRect->top);
	  /* Clear the border areas if needed */
	  gui_mch_set_bg_color(gui.back_pixel);
	  if (updateRect->left < FILL_X(0))
	  {
	    SetRect (&rc, 0, 0, FILL_X(0), FILL_Y(Rows));
	    EraseRect (&rc);
	  }
	  if (updateRect->top < FILL_Y(0))
	  {
	    SetRect (&rc, 0, 0, FILL_X(Columns), FILL_Y(0));
	    EraseRect (&rc);
	  }
	  if (updateRect->right > FILL_X(Columns))
	  {
	    SetRect (&rc, FILL_X(Columns), 0,
			   FILL_X(Columns) + gui.border_offset, FILL_Y(Rows));
	    EraseRect (&rc);
	  }
	  if (updateRect->bottom > FILL_Y(Rows))
	  {
	    SetRect (&rc, 0, FILL_Y(Rows), FILL_X(Columns) + gui.border_offset,
					    FILL_Y(Rows) + gui.border_offset);
	    EraseRect (&rc);
	  }
	HUnlock ((Handle) updateRgn);
	DrawControls (whichWindow);
	/* FAQ 33-27 */
	growRect = whichWindow->portRect;
	growRect.top  = growRect.bottom - 15;
	growRect.left = growRect.right  - 15;
	saveRgn = NewRgn();
	GetClip (saveRgn);
	ClipRect (&growRect);
	DrawGrowIcon (whichWindow);
	SetClip (saveRgn);
	DisposeRgn (saveRgn);
      EndUpdate (whichWindow);
    SetPort (savePort);
}

    void
gui_mac_focus_change(event)
    EventRecord	*event;
{
    if (event->message & 1)
	gui.in_focus = TRUE;
    else
	gui.in_focus = FALSE;
    gui_update_cursor(TRUE, FALSE);
}

#if 0

/*
 * This would be the normal way of invoking the contextual menu
 * but the Vim API doesn't seem to a support a request to get
 * the menu that we should display
 */
    void
gui_mac_handle_contextual_menu(event)
    EventRecord *event;
{
/*
 *  Clone PopUp to use menu
 *  Create a object descriptor for the current selection
 *  Call the procedure
 */

//  Call to Handle Popup
    OSStatus status = ContextualMenuSelect(CntxMenu, event->where, false, kCMHelpItemNoHelp, "", NULL, &CntxType, &CntxMenuID, &CntxMenuItem);

    if (status != noErr)
	return;

    if (CntxType == kCMMenuItemSelected)
    {
	/* Handle the menu CntxMenuID, CntxMenuItem */
	/* The submenu can be handle directly by gui_mac_handle_menu */
	/* But what about the current menu, is the meny changed by ContextualMenuSelect */
	gui_mac_handle_menu ((CntxMenuID << 16) + CntxMenuItem);
    }
    else if (CntxMenuID == kCMShowHelpSelected)
    {
	/* Should come up with the help */
    }

}
#endif

    void
gui_mac_handle_menu(menuChoice)
    long menuChoice;
{
    short	menu = HiWord(menuChoice);
    short	item = LoWord(menuChoice);
    vimmenu_t	*theVimMenu = root_menu;
    MenuHandle	appleMenu;
    Str255	itemName;

    if (menu == 256)  /* TODO: use constant or gui.xyz */
    {
	if (item == 1)
	    SysBeep(1); /* TODO: Popup dialog or do :intro */
	else
	{
	    appleMenu = GetMenuHandle (menu);
	    GetMenuItemText (appleMenu, item, itemName);
	    (void) OpenDeskAcc (itemName);
	}
    }
    else if (item != 0)
    {
	theVimMenu = gui_mac_get_vim_menu(menu, item, root_menu);

	if (theVimMenu)
	    gui_menu_cb(theVimMenu);
    }
    HiliteMenu (0);
}

    pascal
    void
gui_mac_drag_thumb (theControl)
    ControlHandle   theControl;
{
    /* TODO: have live support */
    scrollbar_t *sb;
    int		value, dragging;

    sb = gui_find_scrollbar((long) GetControlReference (theControl));

    value = GetControlValue (theControl);
    dragging = TRUE;

    gui_drag_scrollbar(sb, value, dragging);
}

    pascal
    void
gui_mac_scroll_action (ControlHandle theControl, short partCode)
{
    /* TODO: have live support */
    scrollbar_t *sb, *sb_info;
    long	data;
    long	value;
    int		page;
    int		dragging = FALSE;
    win_t	*wp;
    int		sb_num;

    sb = gui_find_scrollbar((long) GetControlReference (theControl));

    if (sb == NULL)
	return;

    if (sb->wp != NULL)		/* Left or right scrollbar */
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];

	if (sb_info->size > 5)
	    page = sb_info->size - 2;	/* use two lines of context */
	else
	    page = sb_info->size;
    }
    else			/* Bottom scrollbar */
    {
	sb_info = sb;
	page = W_WIDTH(sb->wp) - 5;
    }

    switch (partCode)
    {
	case  kControlUpButtonPart:   data = -1;    break;
	case  kControlDownButtonPart: data = 1;     break;
	case  kControlPageDownPart:   data = page;  break;
	case  kControlPageUpPart:     data = -page; break;
		    default: data = 0; break;
    }

    value = sb_info->value + data;
/*  if (value > sb_info->max)
	value = sb_info->max;
    else if (value < 0)
	value = 0;*/

    gui_drag_scrollbar(sb, value, dragging);

    out_flush();
    gui_mch_set_scrollbar_thumb(sb, value, sb_info->size, sb_info->max);

/*  if (sb_info->wp != NULL)
    {
	sb_num = 0;
	for (wp = firstwin; wp != sb->wp && wp != NULL; wp = wp->w_next)
	sb_num++;

	if (wp != NULL)
	{
	    current_scrollbar = sb_num;
	    scrollbar_value = value;
	    gui_do_scroll();
	    gui_mch_set_scrollbar_thumb(sb, value, sb_info->size, sb_info->max);
	}
    }*/
}

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    void
gui_mch_prepare(argc, argv)
    int		*argc;
    char	**argv;
{
    /* TODO: Move most of this stuff toward gui_mch_init */
    Rect	windRect;
    MenuHandle	pomme;
#ifdef USE_EXE_NAME
    short	applVRefNum;
    long	applDirID;
    Str255	volName;
    char_u	temp[256];
    FSSpec	applDir;
#endif
    long	gestalt_rc;

    MaxApplZone();
    InitGraf(&qd.thePort);
    InitFonts();
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs(nil);
    InitCursor();

#ifdef USE_AEVENT
    (void) InstallAEHandlers();
#endif

#ifdef USE_CTRLCLICKMENU
    if (Gestalt(gestaltContextualMenuAttr, &gestalt_rc) == noErr)
	gui.MacOSHaveCntxMenu = BitTst(&gestalt_rc, 31-gestaltContextualMenuTrapAvailable);
    else
	gui.MacOSHaveCntxMenu = false;

    if (gui.MacOSHaveCntxMenu)
	gui.MacOSHaveCntxMenu = (InitContextualMenus()==noErr);
#endif

#ifdef USE_SIOUX
    SIOUXSettings.standalone = false;
    SIOUXSettings.initializeTB = false;
    SIOUXSettings.setupmenus = false;
    SIOUXSettings.asktosaveonclose = false;
    SIOUXSettings.showstatusline = true;
    SIOUXSettings.toppixel = 300;
    SIOUXSettings.leftpixel = 10;
    InstallConsole (1); /* fileno(stdout) = 1, on page 430 of MSL C */
    printf ("Debugging console enabled\n");
    /*	SIOUXSetTitle ((char_u *) "Vim Stdout"); */
#endif

#ifndef USE_OFFSETED_WINDOW
    SetRect (&windRect, 10, 48, 10+80*7 + 16, 48+24*11);
#else
    SetRect (&windRect, 300, 40, 300+80*7 + 16, 40+24*11);
#endif

    gui.VimWindow = NewCWindow(nil, &windRect, "\pgVim on Macintosh", true, documentProc,
			(WindowPtr) -1, false, 0);

    SetPort(gui.VimWindow);

    gui.char_width = 7;
    gui.char_height = 11;
    gui.char_ascent = 6;
    gui.num_rows = 24;
    gui.num_cols = 80;
    gui.rev_video = FALSE;
    gui.in_focus = TRUE; /* For the moment -> syn. of front application */

    gScrollAction = NewControlActionProc (gui_mac_scroll_action);
    gScrollDrag   = NewControlActionProc (gui_mac_drag_thumb);

    pomme = NewMenu (256, "\p\024"); /* 0x14= = Apple Menu */
    InsertMenu (pomme, 0);

    AppendMenu (pomme, "\pAbout VIM");
    AppendMenu (pomme, "\p-");
    AppendResMenu (pomme, 'DRVR');

    DrawMenuBar();

#ifdef USE_HELPMENU
    (void) HMGetHelpMenuHandle(&gui.MacOSHelpMenu); /* Getting a handle to the Help menu */

    if (gui.MacOSHelpMenu != nil)
	gui.MacOSHelpItems = CountMItems (gui.MacOSHelpMenu);
    else
	gui.MacOSHelpItems = 0;
#endif

    dragRectEnbl = FALSE;
    dragRgn = NULL;
    dragRectControl = kCreateEmpty;
    cursorRgn = NewRgn();
#ifdef USE_EXE_NAME
    HGetVol (volName, &applVRefNum, &applDirID);
    FSMakeFSSpec (applVRefNum, applDirID, "\p", &applDir);
    GetFullPathFromFSSpec (temp, applDir);
    exe_name = FullName_save((char_u *)temp, FALSE);
#endif

#ifdef USE_VIM_CREATOR_ID
    _fcreator = 'VIM!';
    _ftype = 'TEXT';
#endif
}

/*
 * Initialise the GUI.  Create all the windows, set up all the call-backs
 * etc.
 */
    int
gui_mch_init()
{
    guicolor_t tmp_pixel;


    /* Display any pending error messages */
    mch_display_error();

    /* Get background/foreground colors from system */
    /* TODO: do the approriate call to get real defaults */
    gui.norm_pixel = 0x00000000;
    gui.back_pixel = 0x00FFFFFF;

    /* Get the colors from the "Normal" group (set in syntax.c or in a vimrc
     * file). */
    set_normal_colors();

    /*
     * Check that none of the colors are the same as the background color.
     * Then store the current values as the defaults.
     */
    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;

    /* Get the colors for the highlight groups (gui_check_colors() might have
     * changed them) */
    highlight_gui_started();

    /*
     * Setting the gui constants
     */
#ifdef FEAT_MENU
    gui.menu_height = 0;
#endif
    gui.scrollbar_height = gui.scrollbar_width = 15; /* cheat 1 overlap */
    gui.border_offset = gui.border_width = 2;

    return OK;
}
/*
 * Called when the foreground or background color has been changed.
 */
    void
gui_mch_new_colors()
{
    /* TODO:
     * This proc iscalled when Normal is set to a value
     * so what msut be done? I don't know */
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    int
gui_mch_open()
{
    ShowWindow (gui.VimWindow);
    return OK;
}

    void
gui_mch_exit()
{
    /* TODO: find out all what is missing here? */
    DisposeRgn (cursorRgn);
    /* Exit to shell? */
}

/*
 * Get the position of the top left corner of the window.
 */
    int
gui_mch_get_winpos(int *x, int *y)
{
    /* TODO */
    return FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    void
gui_mch_set_winpos(int x, int y)
{
    /* TODO */
}

    void
gui_mch_set_shellsize(width, height, min_width, min_height,
		base_width, base_height)
    int		width;
    int		height;
    int		min_width;
    int		min_height;
    int		base_width;
    int		base_height;
{
    if (gui.which_scrollbars[SBAR_LEFT])
    {
	gui.VimWindow->portRect.left = -gui.scrollbar_width; /* + 1;*/
    }
    else
    {
	gui.VimWindow->portRect.left = 0;
    }

    SizeWindow(gui.VimWindow, width, height, TRUE);

    gui_resize_shell(width, height);
}

/*
 * Get the screen dimensions.
 * Allow 10 pixels for horizontal borders, 40 for vertical borders.
 * Is there no way to find out how wide the borders really are?
 * TODO: Add live udate of those value on suspend/resume.
 */
    void
gui_mch_get_screen_dimensions(screen_w, screen_h)
    int		*screen_w;
    int		*screen_h;
{
    GDHandle	dominantDevice = GetMainDevice();
    Rect	screenRect = (**dominantDevice).gdRect;

    *screen_w = screenRect.right - 10;
    *screen_h = screenRect.bottom - 40;
}

/* Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    static int
points_to_pixels(char_u *str, char_u **end, int vertical)
{
    int		pixels;
    int		points = 0;
    int		divisor = 0;

    while (*str)
    {
	if (*str == '.' && divisor == 0)
	{
	    /* Start keeping a divisor, for later */
	    divisor = 1;
	    continue;
	}

	if (!isdigit(*str))
	    break;

	points *= 10;
	points += *str - '0';
	divisor *= 10;

	++str;
    }

    if (divisor == 0)
	divisor = 1;

    pixels = points/divisor;
    *end = str;
    return pixels;
}

    GuiFont
gui_mac_find_font (font_name)
    char_u *font_name;
{
    char_u	c;
    char_u	*p;
    char_u	pFontName[256];
    Str255	systemFontname;
    short	font_id;
    short	size=9;
    GuiFont	font;

    for (p = font_name; ((*p != 0) && (*p != ':')); p++)
	;

    c = *p;
    *p = 0;
    STRCPY(&pFontName[1], font_name);
    pFontName[0] = STRLEN(font_name);
    *p = c;

    GetFNum (pFontName, &font_id);

    if (font_id == 0)
    {
	/* Oups, the system font was it the one the user want */

	GetFontName (0, systemFontname);
	if (!EqualString(pFontName, systemFontname, false, false))
	    return NOFONT;
    }
    if (*p == ':')
    {
	p++;
	/* Set the values found after ':' */
	while (*p)
	{
	    switch (*p++)
	    {
		case 'h':
		    size = points_to_pixels(p, &p, TRUE);
		    break;
		    /*
		     * TODO: Maybe accept width and styles
		     */
	    }
	    while (*p == ':')
		p++;
	}
    }

    if (size < 1)
	size = 1;   /* Avoid having a size of 0 with system font */

    font = (size << 16) + ((long) font_id & 0xFFFF);

    return font;
}

/*
 * Initialise vim to use the font with the given name.	Return FAIL if the font
 * could not be loaded, OK otherwise.
 */
    int
gui_mch_init_font(font_name, fontset)
    char_u	*font_name;
    int		fontset;	    /* not used */
{
    /* TODO: Add support for bold italic underline proportional etc... */
    Str255	suggestedFont = "\pMonaco";
    int		suggestedSize = 9;
    FontInfo	font_info;
    short	font_id;
    GuiFont	font;

    if (font_name == NULL)
    {
	/* First try to get the suggested font */
	GetFNum(suggestedFont, &font_id);

	if (font_id == 0)
	{
	    /* Then pickup the standard application font */
	    font_id = GetAppFont();
	}
	font = (suggestedSize << 16) + ((long) font_id & 0xFFFF);
    }
    else
    {
	font = gui_mac_find_font (font_name);

	if (font == NOFONT)
	    return FAIL;
    }
    gui.norm_font = font;

    TextSize (font >> 16);
    TextFont (font & 0xFFFF);

    GetFontInfo (&font_info);

    gui.char_ascent = font_info.ascent;
    gui.char_width  = CharWidth ('_');
    gui.char_height = font_info.ascent + font_info.descent + p_linespace;

    return OK;

}

    int
gui_mch_adjust_charsize()
{
    FontInfo    font_info;

    GetFontInfo (&font_info);
    gui.char_height = font_info.ascent + font_info.descent + p_linespace;
    gui.char_ascent = font_info.ascent + p_linespace / 2;
    return OK;
}

/*
 * Get a font structure for highlighting.
 */
    GuiFont
gui_mch_get_font(name, giveErrorIfMissing)
    char_u	*name;
    int		giveErrorIfMissing;
{
    GuiFont font;

    font = gui_mac_find_font(name);

    if (font == NULL)
    {
	if (giveErrorIfMissing)
	    EMSG2(_("Unknown font: %s"), name);
	return NOFONT;
    }
    /*
     * TODO : Accept only monospace
     */

    return font;
}

/*
 * Set the current text font.
 */
    void
gui_mch_set_font(font)
    GuiFont	font;
{
    /*
     * TODO: maybe avoid set again the current font.
     */
    TextSize(font >> 16);
    TextFont(font & 0xFFFF);
}

#if 0 /* not used */
/*
 * Return TRUE if the two fonts given are equivalent.
 */
    int
gui_mch_same_font(f1, f2)
    GuiFont	f1;
    GuiFont	f2;
{
    return f1 == f2;
}
#endif

/*
 * If a font is not going to be used, free its structure.
 */
    void
gui_mch_free_font(font)
    GuiFont	font;
{
    /*
     * Free font when "font" is not 0.
     * Nothing to do in the current implementation, since
     * nothing is allocated for each font used.
     */
}

    static int
hex_digit(c)
    int		c;
{
    if (isdigit(c))
	return c - '0';
    c = TO_LOWER(c);
    if (c >= 'a' && c <= 'f')
	return c - 'a' + 10;
    return -1000;
}

/*
 * Return the Pixel value (color) for the given color name.  This routine was
 * pretty much taken from example code in the Silicon Graphics OSF/Motif
 * Programmer's Guide.
 */
    guicolor_t
gui_mch_get_color(name)
    char_u *name;
{
    /* TODO: Add color support for a rgb.txt file
     *	     Add support for the new named colr of MacOS 8\
     */
    RGBColor	MacColor;
    guicolor_t	color = 0;

    typedef struct guicolor_tTable
    {
	char	    *name;
	guicolor_t  color;
    } guicolor_tTable;

    /*
     * The comment at the end of each line is the source
     * (Mac, Window, Unix) and the number is th eunix rgb.txt value
     */
    static guicolor_tTable table[] =
    {
	{"Black",	RGB(0x00, 0x00, 0x00)},
	{"darkgray",	RGB(0x80, 0x80, 0x80)}, /*W*/
	{"darkgrey",	RGB(0x80, 0x80, 0x80)}, /*W*/
	{"Gray",	RGB(0xC0, 0xC0, 0xC0)}, /*W*/
	{"Grey",	RGB(0xC0, 0xC0, 0xC0)}, /*W*/
	{"lightgray",	RGB(0xE0, 0xE0, 0xE0)}, /*W*/
	{"lightgrey",	RGB(0xE0, 0xE0, 0xE0)}, /*W*/
	{"white",	RGB(0xFF, 0xFF, 0xFF)},
	{"darkred",	RGB(0x80, 0x00, 0x00)}, /*W*/
	{"red",		RGB(0xDD, 0x08, 0x06)}, /*M*/
	{"lightred",	RGB(0xFF, 0xA0, 0xA0)}, /*W*/
	{"DarkBlue",	RGB(0x00, 0x00, 0x80)}, /*W*/
	{"Blue",	RGB(0x00, 0x00, 0xD4)}, /*M*/
	{"lightblue",	RGB(0xA0, 0xA0, 0xFF)}, /*W*/
	{"DarkGreen",	RGB(0x00, 0x80, 0x00)}, /*W*/
	{"Green",	RGB(0x00, 0x64, 0x11)}, /*M*/
	{"lightgreen",	RGB(0xA0, 0xFF, 0xA0)}, /*W*/
	{"DarkCyan",	RGB(0x00, 0x80, 0x80)}, /*W ?0x307D7E */
	{"cyan",	RGB(0x02, 0xAB, 0xEA)}, /*M*/
	{"lightcyan",	RGB(0xA0, 0xFF, 0xFF)}, /*W*/
	{"darkmagenta",	RGB(0x80, 0x00, 0x80)}, /*W*/
	{"magenta",	RGB(0xF2, 0x08, 0x84)}, /*M*/
	{"lightmagenta",RGB(0xF0, 0xA0, 0xF0)}, /*W*/
	{"brown",	RGB(0x80, 0x40, 0x40)}, /*W*/
	{"yellow",	RGB(0xFC, 0xF3, 0x05)}, /*M*/
	{"lightyellow",	RGB(0xFF, 0xFF, 0xA0)}, /*M*/
	{"SeaGreen",	RGB(0x2E, 0x8B, 0x57)}, /*W 0x4E8975 */
	{"orange",	RGB(0xFC, 0x80, 0x00)}, /*W 0xF87A17 */
	{"Purple",	RGB(0xA0, 0x20, 0xF0)}, /*W 0x8e35e5 */
	{"SlateBlue",	RGB(0x6A, 0x5A, 0xCD)}, /*W 0x737CA1 */
	{"Violet",	RGB(0x8D, 0x38, 0xC9)}, /*U*/
    };

    unsigned short	r, g, b;
    int			i;

    if (name[0] == '#' && strlen((char *) name) == 7)
    {
	/* Name is in "#rrggbb" format */
	r = hex_digit(name[1]) * 16 + hex_digit(name[2]);
	g = hex_digit(name[3]) * 16 + hex_digit(name[4]);
	b = hex_digit(name[5]) * 16 + hex_digit(name[6]);
	if (r < 0 || g < 0 || b < 0)
	    return (guicolor_t) -1;
	return RGB(r, g, b);
    }
    else
    {
	if (STRICMP (name, "hilite") == 0)
	{
	    LMGetHiliteRGB (&MacColor);
	    return (RGB (MacColor.red >> 8, MacColor.green >> 8, MacColor.blue >>8));
	}
	/* Check if the name is one of the colors we know */
	for (i = 0; i < sizeof(table) / sizeof(table[0]); i++)
	    if (STRICMP(name, table[i].name) == 0)
		return table[i].color;
    }


    /*
     * Last attempt. Look in the file "$VIM/rgb.txt".
     */
    {
#define LINE_LEN 100
	FILE	*fd;
	char	line[LINE_LEN];
	char_u	*fname;

	fname = expand_env_save((char_u *)"$VIMRUNTIME:rgb.txt");
	if (fname == NULL)
	    return (guicolor_t)-1;

	fd = fopen((char *)fname, "rt");
	vim_free(fname);
	if (fd == NULL)
	    return (guicolor_t)-1;

	while (!feof(fd))
	{
	    int		len;
	    int		pos;
	    char	*color;
	    int		dummy;

	    fgets(line, LINE_LEN, fd);
	    len = strlen(line);

	    if (len <= 1 || line[len-1] != '\n')
		continue;

	    line[len-1] = '\0';

	    i = sscanf(line, "%hd %hd %hd %n", &r, &g, &b, &pos);
	    if (i != 3)
		continue;

	    color = line + pos;

	    if (STRICMP(color, name) == 0)
	    {
		fclose(fd);
		return (guicolor_t) RGB(r,g,b);
	    }
	}
	fclose(fd);
    }

    return -1;
}

/*
 * Set the current text foreground color.
 */
    void
gui_mch_set_fg_color(color)
    guicolor_t	color;
{
    RGBColor TheColor;

    TheColor.red = Red(color) * 0x0101;
    TheColor.green = Green(color) * 0x0101;
    TheColor.blue = Blue(color) * 0x0101;

    RGBForeColor (&TheColor);
}

/*
 * Set the current text background color.
 */
    void
gui_mch_set_bg_color(color)
    guicolor_t	color;
{
    RGBColor TheColor;

    TheColor.red = Red(color) * 0x0101;
    TheColor.green = Green(color) * 0x0101;
    TheColor.blue = Blue(color) * 0x0101;

    RGBBackColor (&TheColor);
}

    void
gui_mch_draw_string(row, col, s, len, flags)
    int		row;
    int		col;
    char_u	*s;
    int		len;
    int		flags;
{
    TextMode (srcCopy);
    TextFace (normal);

/*  SelectFont(hdc, gui.currFont); */

    if (flags & DRAW_TRANSP)
    {
	TextMode (srcOr);
    }

    MoveTo (TEXT_X(col), TEXT_Y(row));
    DrawText ((char *)s, 0, len);


    if (flags & DRAW_BOLD)
    {
	TextMode (srcOr);
	MoveTo (TEXT_X(col) + 1, TEXT_Y(row));
	DrawText ((char *)s, 0, len);
    }

    if (flags & DRAW_UNDERL)
    {
	MoveTo (FILL_X(col), FILL_Y(row + 1) - 1);
	LineTo (FILL_X(col + len) - 1, FILL_Y(row + 1) - 1);
    }
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    int
gui_mch_haskey(name)
    char_u  *name;
{
    int i;

    for (i = 0; special_keys[i].key_sym != (KeySym)0; i++)
	if (name[0] == special_keys[i].vim_code0 &&
					 name[1] == special_keys[i].vim_code1)
	    return OK;
    return FAIL;
}

    void
gui_mch_beep()
{
    SysBeep (1);
}

    void
gui_mch_flash(msec)
    int	    msec;
{
    /* Do a visual beep by reversing the foreground and background colors */
    Rect    rc;

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.left = 0;
    rc.top = 0;
    rc.right = gui.num_cols * gui.char_width;
    rc.bottom = gui.num_rows * gui.char_height;
    InvertRect(&rc);

    ui_delay((long)msec, TRUE);		/* wait for some msec */

    InvertRect(&rc);
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    void
gui_mch_invert_rectangle(r, c, nr, nc)
    int		r;
    int		c;
    int		nr;
    int		nc;
{
    Rect	rc;

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.left = FILL_X(c);
    rc.top = FILL_Y(r);
    rc.right = rc.left + nc * gui.char_width;
    rc.bottom = rc.top + nr * gui.char_height;
    InvertRect(&rc);

}

/*
 * Iconify the GUI window.
 */
    void
gui_mch_iconify()
{
    /* TODO: find out what could replace iconify
     *	     -window shade?
     *	     -hide application?
     */
}

/*
 * Draw a cursor without focus.
 */
    void
gui_mch_draw_hollow_cursor(color)
    guicolor_t	color;
{
    Rect rc;

    gui_mch_set_fg_color(color);

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.left = FILL_X(gui.col);
    rc.top = FILL_Y(gui.row);
    rc.right = rc.left + gui.char_width;
    rc.bottom = rc.top + gui.char_height;

    gui_mch_set_fg_color(color);

    FrameRect (&rc);
}

/*
 * Draw part of a cursor, only w pixels wide, and h pixels high.
 */
    void
gui_mch_draw_part_cursor(w, h, color)
    int		w;
    int		h;
    guicolor_t	color;
{
    Rect rc;

    rc.left = FILL_X(gui.col);
    rc.top = FILL_Y(gui.row) + gui.char_height - h;
    rc.right = rc.left + w;
    rc.bottom = rc.top + h;

    gui_mch_set_fg_color(color);

    PaintRect (&rc);
}

    void
gui_mac_do_key(EventRecord *theEvent)
{
    /* TODO: add support for COMMAND KEY */
    long		menu;
    unsigned char	string[3], string2[3];
    short		num, i;
    KeySym		key_sym;

    if (p_mh)
	ObscureCursor();

    key_sym = ((theEvent->message & keyCodeMask) >> 8);
    string[0] = theEvent->message & charCodeMask;
    num = 1;

    if (theEvent->modifiers & controlKey)
	if (string[0] == Ctrl_C)
	    got_int = TRUE;
    if (theEvent->modifiers & cmdKey)
	if (string[0] == '.')
	    got_int = TRUE;

    if (theEvent->modifiers & cmdKey)
	if ((theEvent->modifiers & (~(cmdKey | btnState | alphaLock))) == 0)
	{
	    menu = MenuKey(string[0]);
	    if (HiWord(menu))
	    {
		gui_mac_handle_menu(menu);
		return;
	    }
	}

#if 0
    if	(!(theEvent->modifiers & (cmdKey | controlKey | rightControlKey)))
#endif
    {
	if  ((string[0] < 0x20) || (string[0] == 0x7f))
	{
	    /* num = 0; */

	    for (i = 0; special_keys[i].key_sym != (KeySym)0; i++)
	    {
		if (special_keys[i].key_sym == key_sym)
		{
		    string[0] = CSI;
		    string[1] = special_keys[i].vim_code0;
		    string[2] = special_keys[i].vim_code1;
		    num = 3;
		}
	    }
	}
    }

    /* Special keys (and a few others) may have modifiers */
    if (num == 3 || key_sym == vk_Space || key_sym == vk_Tab
	|| key_sym == vk_Return || key_sym == vk_Esc
#ifdef USE_CMD_KEY
	|| ((theEvent->modifiers & cmdKey) != 0)
#endif
	)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = 0;
	if (theEvent->modifiers & shiftKey)
	    string2[2] |= MOD_MASK_SHIFT;
	if (theEvent->modifiers & controlKey)
	    string2[2] |= MOD_MASK_CTRL;
	if (theEvent->modifiers & optionKey)
	    string2[2] |= MOD_MASK_ALT;
#ifdef USE_CMD_KEY
	if (theEvent->modifiers & cmdKey)
	    string2[2] |= MOD_MASK_CMD;
#endif
	if (string2[2] != 0)
	    add_to_input_buf(string2, 3);
    }

    if (num == 1 && string[0] == CSI)
    {
	/* Insert CSI as K_CSI.  Not tested! */
	string[1] = KS_EXTRA;
	string[2] = KE_CSI;
	num = 3;
    }

    add_to_input_buf(string, num);
}

    void
gui_mac_doMouseDown (theEvent)
    EventRecord *theEvent;
{
    /* TODO: clean up */
    short		thePart;
    WindowPtr		whichWindow;
    Point		thePoint;
    long		dblClkTreshold;
    int_u		vimModifier;
    Rect		movingLimits;
    short		thePortion;
    ControlHandle	theControl;
    long		menu;

    Rect		sizeRect;
    long		newSize;
    int			vimButton;

    thePart = FindWindow (theEvent->where, &whichWindow);

    switch (thePart)
    {
	case (inDesk):  /* TODO: what to do? */
	    break;
	case (inMenuBar):
	    gui_mac_handle_menu(MenuSelect (theEvent->where));
	    break;

	case (inContent):
	    dblClkTreshold = GetDblTime();
	    thePoint = theEvent->where;
	    GlobalToLocal (&thePoint);
	    SelectWindow (whichWindow);

	    thePortion = FindControl (thePoint, whichWindow, &theControl);

	    if (theControl == NUL)
	    {
		vimModifier = 0x0;
		if (theEvent->modifiers & (shiftKey | rightShiftKey))
		    vimModifier |= MOUSE_SHIFT;
		if (theEvent->modifiers & (controlKey | rightControlKey))
		    vimModifier |= MOUSE_CTRL;
		if (theEvent->modifiers & (optionKey | rightOptionKey))
		    vimModifier |= MOUSE_ALT;

		vimButton = MOUSE_LEFT;
#ifdef USE_CTRLCLICKMENU
		clickIsPopup = FALSE;

		if (gui.MacOSHaveCntxMenu)
		{
		    if (IsShowContextualMenuClick(theEvent))
		    {
			vimButton = MOUSE_RIGHT;
			vimModifier &= ~MOUSE_CTRL;
			clickIsPopup = TRUE;
		    }
		}
#endif
		gui_send_mouse_event(vimButton, thePoint.h, thePoint.v,
				 (theEvent->when - lastMouseTick) < dblClkTreshold, vimModifier);

#ifdef USE_CTRLCLICKMENU
#if 0
		if (vimButton == MOUSE_LEFT)
#endif
		{
#endif
		    SetRect (&dragRect, FILL_X(X_2_COL(thePoint.h)),
					FILL_Y(Y_2_ROW(thePoint.v)),
					FILL_X(X_2_COL(thePoint.h)+1),
					FILL_Y(Y_2_ROW(thePoint.v)+1));

		    dragRectEnbl = TRUE;
		    dragRectControl = kCreateRect;
#ifdef USE_CTRLCLICKMENU
		}
#endif
	    }
	    else
	    {
		if (thePortion != kControlIndicatorPart)
		{
		    TrackControl(theControl, thePoint, gScrollAction);
		    dragged_sb = NULL;
		}
		else
		{
		    dragged_sb = theControl;
			TrackControl(theControl, thePoint, NULL);
/*			gui_mch_scroll_drag();*/
		    gui_mac_drag_thumb (theControl);
		    dragged_sb = NULL;
		}
	    /*	gui_mac_do_control(theControl, thePoint);*/
	    }
	    break;

	case (inDrag):
	    /* SetRect (&movingLimits, 0, 0, 832, 624); */
	    movingLimits = (*GetGrayRgn())->rgnBBox;
	    DragWindow (whichWindow, theEvent->where, &movingLimits);
	    break;

	case (inGrow):

    sizeRect = qd.screenBits.bounds;
    sizeRect.top = 100;
    sizeRect.left = 100;
    newSize = GrowWindow(whichWindow, theEvent->where, &sizeRect);
    if (newSize != 0) {

	gui_mch_set_bg_color(gui.back_pixel);

	gui_resize_shell(newSize & 0x0000FFFF, newSize >> 16);
/*	if ((gui.right_sbar_x + gui.scrollbar_width) != (newSize & 0x0000FFFF))
	 newSize=gui.right_sbar_x + gui.scrollbar_width + (newSize & 0xFFFF0000);
	if ((gui.right_sbar_x + gui.scrollbar_width) != (newSize & 0x0000FFFF))
	 newSize=gui.right_sbar_x + gui.scrollbar_width + (newSize & 0xFFFF0000);
	gui_resize_shell(newSize & 0x0000FFFF, newSize >> 16);
	SizeWindow(whichWindow, newSize & 0x0000FFFF, newSize >> 16, true);*/
	/* Is this really needed??? */
	gui_set_shellsize(FALSE);
/*	SetPort(wp);
	InvalRect(&wp->portRect);
	if (isUserWindow(wp)) {
	    DrawingWindowPeek	aWindow = (DrawingWindowPeek)wp;

	    if (aWindow->toolRoutines.toolWindowResizedProc)
		CallToolWindowResizedProc(aWindow->toolRoutines.toolWindowResizedProc, wp);
	}*/
    };
    break;
	case (inGoAway):  /* TODO */
	case (inZoomIn):
	case (inZoomOut):
	    break;
    }
}

    void
gui_mac_handle_event (event)
    EventRecord *event;
{
    WindowPtr	whichWindow;
    Point	thePoint;
    Boolean	a_bool;
    int_u	vimModifier;
    OSErr	error;

    char	touche;

#ifdef USE_CTRLCLICKMENU
    /*
     * if (IsShowContextualMenuClick(event))
     *   do context
     */
    if (gui.MacOSHaveCntxMenu)
    {
	if (IsShowContextualMenuClick(event))
	{
#if 0
	    gui_mac_handle_contextual_menu(event);
#else
	    gui_mac_doMouseDown(event);
#endif
	    return;
	}
    }
#endif

    switch (event->what)
    {
	case (keyDown):
	case (autoKey):
	    gui_mac_do_key (event);
	    break;

	case (mouseDown):
	    gui_mac_doMouseDown(event);
	    break;

	case (mouseUp):
	    /* TODO: Properly convert the Contextual menu mouse-up */
	    /*       Potential source of the double menu */
	    lastMouseTick = event->when;
	    dragRectEnbl = FALSE;
	    dragRectControl = kCreateEmpty;
	    thePoint = event->where;
	    GlobalToLocal (&thePoint);
	    vimModifier = 0x0;
	    if (event->modifiers & (shiftKey | rightShiftKey))
		vimModifier |= MOUSE_SHIFT;
	    if (event->modifiers & (controlKey | rightControlKey))
		vimModifier |= MOUSE_CTRL;
	    if (event->modifiers & (optionKey | rightOptionKey))
		vimModifier |= MOUSE_ALT;
#ifdef USE_CTRLCLICKMENU
	    if (clickIsPopup)
	    {
		vimModifier &= ~MOUSE_CTRL;
	    }
#endif
	    gui_send_mouse_event(MOUSE_RELEASE, thePoint.h, thePoint.v,
				 FALSE, vimModifier);
	    break;

	case (keyUp):
	    break;

	case (updateEvt):
	    gui_mac_update (event);
	    break;
	case (diskEvt):
	    break;
	case (activateEvt):
	    whichWindow = (WindowPtr) event->message;
	    if ((event->message) & activeFlag)
		gui.in_focus = FALSE;
	    else
	    {
		gui.in_focus = TRUE;
		a_bool = false;
		SetPreserveGlyph (a_bool);
		SetOutlinePreferred (a_bool);
	    }
	    break;
	case (osEvt):
	    if	((event->message &0xFF000000) == 0xFA000000)
	    {
		thePoint = event->where;
		GlobalToLocal (&thePoint);
		vimModifier = 0x0;
		if (event->modifiers & (shiftKey | rightShiftKey))
		    vimModifier |= MOUSE_SHIFT;
		if (event->modifiers & (controlKey | rightControlKey))
		    vimModifier |= MOUSE_CTRL;
		if (event->modifiers & (optionKey | rightOptionKey))
		    vimModifier |= MOUSE_ALT;

		if (!Button())
		    gui_mouse_moved (thePoint.h, thePoint.v);
		else
#ifdef USE_CTRLCLICKMENU
		    if (!clickIsPopup)
#endif
			gui_send_mouse_event(MOUSE_DRAG, thePoint.h, thePoint.v,
					     FALSE, vimModifier);

		SetRect (&dragRect, FILL_X(X_2_COL(thePoint.h)),
				    FILL_Y(Y_2_ROW(thePoint.v)),
				    FILL_X(X_2_COL(thePoint.h)+1),
				    FILL_Y(Y_2_ROW(thePoint.v)+1));

		if (dragRectEnbl)
		    dragRectControl = kCreateRect;

	    }
	    else if (event->message & 0x01000000)
	    {
		gui_mac_focus_change(event);
	    }
	    break;

#ifdef USE_AEVENT
	case (kHighLevelEvent):
	    error = AEProcessAppleEvent(event); /* TODO: Error Handling */
	    break;
#endif
    }
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    void
gui_mch_update()
{
    /* TODO: find what to do
     *	     maybe call gui_mch_wait_for_chars (0)
     *	     more like look at EventQueue then
     *	     call heart of gui_mch_wait_for_chars;
     *
     *	if (eventther)
     *	    gui_mac_handle_event(&event);
     */
    EventRecord theEvent;

    if (EventAvail (everyEvent, &theEvent))
	if (theEvent.what != nullEvent)
	    gui_mch_wait_for_chars(0);
}

/*
 * Simple wrapper to neglect more easily the time
 * spent inside WaitNextEvent while profiling.
 */

#pragma profile reset
    pascal
    Boolean
WaitNextEventWrp (EventMask eventMask, EventRecord *theEvent, UInt32 sleep, RgnHandle mouseRgn)
{
    if (((long) sleep) < -1)
	sleep = 32767;
    return WaitNextEvent(eventMask, theEvent, sleep, mouseRgn);
}

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1	    Wait forever.
 *  wtime == 0	    This should never happen.
 *  wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
#pragma profile reset
    int
gui_mch_wait_for_chars(wtime)
    int	    wtime;
{
    RgnHandle	watchingRgn;

    EventMask	mask  = (everyEvent);
    EventRecord event;
    long	entryTick;
    long	currentTick;

    short	oldDragRectEnbl;

    entryTick = TickCount();

    do
    {
/*	    if (dragRectControl == kCreateEmpty)
	    {
		dragRgn = NULL;
		dragRectControl = kNothing;
	    }
	    else*/ if (dragRectControl == kCreateRect)
	    {
		dragRgn = cursorRgn;
		RectRgn (dragRgn, &dragRect);
		dragRectControl = kNothing;
	    }
	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use XtAppProcessEvent() to hang until an
	 * event arrives.  No need to check for input_buf_full because we are
	 * returning as soon as it contains a single char.  Note that
	 * XtAppNextEvent() may not be used because it will not return after a
	 * timer event has arrived -- webb
	 */
	/* TODO: reduce wtime accordinly???  */
	if (WaitNextEventWrp (mask, &event, 60*wtime/1000, dragRgn))
	{
#ifdef USE_SIOUX
	    if (!SIOUXHandleOneEvent(&event))
#endif
		gui_mac_handle_event (&event);
	    if (!vim_is_input_buf_empty())
		return OK;
	}
	currentTick = TickCount();
    }
    while ((wtime == -1) || ((currentTick - entryTick) < 60*wtime/1000));

    return FAIL;
}

#pragma profile reset

/*
 * Output routines.
 */

/* Flush any output to the screen */
    void
gui_mch_flush()
{
    /* TODO: Is anything needed here? */
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    void
gui_mch_clear_block(row1, col1, row2, col2)
    int		row1;
    int		col1;
    int		row2;
    int		col2;
{
    Rect rc;
    /*
     * Clear one extra pixel at the right, for when bold characters have
     * spilled over to the next column.
     * Can this ever erase part of the next character? - webb
     */

    rc.left = FILL_X(col1);
    rc.top = FILL_Y(row1);
    rc.right = FILL_X(col2 + 1) + 1;
    rc.bottom = FILL_Y(row2 + 1);

    gui_mch_set_bg_color(gui.back_pixel);
    EraseRect (&rc);
}

/*
 * Clear the whole text window.
 */
    void
gui_mch_clear_all()
{
    Rect	rc;

    rc.left = 0;
    rc.top = 0;
    rc.right = Columns * gui.char_width + 2 * gui.border_width;
    rc.bottom = Rows * gui.char_height + 2 * gui.border_width;

    gui_mch_set_bg_color(gui.back_pixel);
    EraseRect(&rc);
/*  gui_mch_set_fg_color(gui.norm_pixel);
    FrameRect(&rc);
*/
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    void
gui_mch_delete_lines(row, num_lines)
    int		row;
    int		num_lines;
{
    if (num_lines <= 0)
	return;

    if (row + num_lines > gui.scroll_region_bot)
    {
	/* Scrolled out of region, just blank the lines out */
	gui_clear_block(row, 0, gui.scroll_region_bot, Columns - 1);
    }
    else
    {
	Rect	rc;

	rc.left = FILL_X(0);
	rc.right = FILL_X(Columns);
	rc.top = FILL_Y(row);
	rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

	gui_mch_set_bg_color(gui.back_pixel);
	ScrollRect (&rc, 0, -num_lines * gui.char_height, (RgnHandle) nil);

	/* Update gui.cursor_row if the cursor scrolled or copied over */
	if (gui.cursor_row >= row)
	{
	    if (gui.cursor_row < row + num_lines)
		gui.cursor_is_valid = FALSE;
	    else if (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_row -= num_lines;
	}

	gui_clear_block(gui.scroll_region_bot - num_lines + 1, 0,
	    gui.scroll_region_bot, Columns - 1);
    }
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    void
gui_mch_insert_lines(row, num_lines)
    int		row;
    int		num_lines;
{
    if (num_lines <= 0)
	return;

    if (row + num_lines > gui.scroll_region_bot)
    {
	/* Scrolled out of region, just blank the lines out */
	gui_clear_block(row, 0, gui.scroll_region_bot, Columns - 1);
    }
    else
    {
	Rect rc;

	rc.left = FILL_X(0);
	rc.right = FILL_X(Columns);
	rc.top = FILL_Y(row);
	rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

	gui_mch_set_bg_color(gui.back_pixel);

	ScrollRect (&rc, 0, gui.char_height * num_lines, (RgnHandle) nil);

	/* Update gui.cursor_row if the cursor scrolled or copied over */
	if (gui.cursor_row >= gui.row)
	{
	    if (gui.cursor_row <= gui.scroll_region_bot - num_lines)
		gui.cursor_row += num_lines;
	    else if (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_is_valid = FALSE;
	}

	gui_clear_block(row, 0, row + num_lines - 1, Columns - 1);
    }
}

    /*
     * TODO: add a vim format to the clipboard which remember
     *	     LINEWISE, CHARWISE, BLOCKWISE
     */

    void
clip_mch_request_selection()
{

    Handle textOfClip;
    long	scrapOffset;
    long	scrapSize;
    int		type;
    char	*searchCR;
    char	*tempclip;

    scrapSize = LoadScrap (); /* This seem to avoid problem with crash on first paste */
    scrapSize = GetScrap ( nil, 'TEXT', &scrapOffset);

    if (scrapSize > 0)
    {
	textOfClip = NewHandle(0);
	HLock (textOfClip);
	scrapSize = GetScrap ( textOfClip, 'TEXT', &scrapOffset);

	type = (strchr (*textOfClip, '\r') != NULL) ? MLINE : MCHAR;

	tempclip = (char *)lalloc(scrapSize+1, TRUE);
	STRNCPY (tempclip, *textOfClip, scrapSize);
	tempclip[scrapSize] = 0;

	searchCR=tempclip;
	while (searchCR != NULL)
	{
	    searchCR = strchr(searchCR, '\r');

	    if (searchCR != NULL)
		searchCR[0] = '\n';

	}

	clip_yank_selection (type, (char_u *) tempclip, scrapSize);

	free (tempclip);
	HUnlock (textOfClip);

	DisposeHandle (textOfClip);
    }
}

    void
clip_mch_lose_selection()
{
    /*
     * TODO: Really nothing to do?
     */
}

    int
clip_mch_own_selection()
{
    return OK;
}
/*
 * Send the current selection to the clipboard.
 */
    void
clip_mch_set_selection()
{
    Handle	textOfClip;
    long	scrapOffset;
    long	scrapSize;
    int		type;
    char	*searchCR;
    char	*tempclip;

    long_u	cch;
    char_u	*str = NULL;

    if (!clipboard.owned)
	return;

    clip_get_selection();

    /*
     * Once we set the clipboard, lose ownership.  If another application sets
     * the clipboard, we don't want to think that we still own it.
     *
     */

    clipboard.owned = FALSE;

    type = clip_convert_selection (&str, (long_u *) &scrapSize);

    if (type >= 0)
    {

	ZeroScrap ();

	textOfClip = NewHandle(scrapSize);

	HLock (textOfClip);

	    STRNCPY (*textOfClip, str, scrapSize);
	    PutScrap (scrapSize, 'TEXT', *textOfClip);

	HUnlock (textOfClip);

	DisposeHandle (textOfClip);
    }

    vim_free (str);
}

    void
gui_mch_set_text_area_pos(x, y, w, h)
    int		x;
    int		y;
    int		w;
    int		h;
{
/*  HideWindow (gui.VimWindow); */
    if (gui.which_scrollbars[SBAR_LEFT])
	gui.VimWindow->portRect.left = -gui.scrollbar_width + 1;
    else
	gui.VimWindow->portRect.left = 0;
    ShowWindow (gui.VimWindow);
}

/*
 * Menu stuff.
 */

    void
gui_mch_enable_menu(flag)
    int		flag;
{
    /*
     * Menu is always active in itself
     * (maybe we should only disable a vim menu
     *	and keep standard menu)
     *
     */
}

    void
gui_mch_set_menu_pos(x, y, w, h)
    int		x;
    int		y;
    int		w;
    int		h;
{
    /*
     * The menu is always at the top of the screen
     * Maybe a futur version will permit a menu in the window
     *
     */
}

/*
 * Add a sub menu to the menu bar.
 */
    void
gui_mch_add_menu(menu, parent, idx)
    vimmenu_t	*menu;
    vimmenu_t	*parent;
    int		idx;
{
    /*
     * TODO: Simplify handling of index, handle and id of menu and submenu
     * TODO: use menu->mnemonic and menu->actext
     */
    static long	 next_avail_id = 128;
    char_u	*name;
    long	len;
    short	index;

    if (/* !menu_is_menubar(menu->name)
	    || */ (parent != NULL && parent->submenu_id == 0))
	return;

    if (next_avail_id == 1024)
	return;

    len = STRLEN(menu->dname);
    name = alloc(len + 1);
    if (name == NULL)
	return;
    mch_memmove(name + 1, menu->dname, len);
    name[0] = len;

#ifdef USE_HELPMENU
    if (STRNCMP(name, "\4Help", 5) == 0)
    {
	menu->submenu_id = kHMHelpMenuID;
	menu->submenu_handle = gui.MacOSHelpMenu;
    }
    else
    {
#endif
    menu->submenu_id = next_avail_id;
    menu->submenu_handle = NewMenu (menu->submenu_id, name);
    next_avail_id++;
#ifdef USE_HELPMENU
    }
#endif

    if (parent == NULL)
    {
	menu->menu_id = 0;
	menu->menu_handle = NULL;
/*
	menu->index = 0;
*/
	/* TODO: Should the menubat_menu if be before or after? */
#ifdef USE_HELPMENU
	if (menu->submenu_id == kHMHelpMenuID)
	    menu->submenu_id = kHMHelpMenuID;
	else
#endif
	if (menu_is_menubar(menu->name))
	    InsertMenu (menu->submenu_handle, idx); /* before */
	else
	    InsertMenu (menu->submenu_handle, hierMenu); /* before */
#if 1
	DrawMenuBar();
#endif
    }
    else
    {
	menu->menu_id = parent->submenu_id;
	menu->menu_handle = parent->submenu_handle;
	index = gui_mac_get_menu_item_index (menu, parent);
/*
	menu->index = index;
*/
/*	AppendMenu(menu->menu_handle, name); */
	InsertMenuItem(menu->menu_handle, "\p ", idx); /*afterItem */
	SetMenuItemText(menu->menu_handle, idx+1, name);
	SetItemCmd(menu->menu_handle, idx+1, 0x1B);
	SetItemMark(menu->menu_handle, idx+1, menu->submenu_id);
	InsertMenu(menu->submenu_handle, hierMenu);
    }

    vim_free (name);

#if 0
    DrawMenuBar();
#endif
}

/*
 * Add a menu item to a menu
 */
    void
gui_mch_add_menu_item(menu, parent, idx)
    vimmenu_t	*menu;
    vimmenu_t	*parent;
    int		idx;
{
    char_u	*name;
    long	len;

    if (parent->submenu_id == 0)
	return;

    /* Don't add menu separator */
/*  if (menu_is_separator(menu->name))
	return;
*/
    len = STRLEN(menu->dname);
    name = alloc(len + 1);
    if (name == NULL)
	return;
    mch_memmove(name + 1, menu->dname, len);
    name[0] = len;

    menu->menu_id = parent->submenu_id;
    menu->submenu_id = 0;
    menu->menu_handle = parent->submenu_handle;
    menu->submenu_handle = NULL;
/*
    menu->index = gui_mac_get_menu_item_index(menu, parent);
*/
#ifdef USE_HELPMENU
    if (menu->menu_id == kHMHelpMenuID)
	idx += gui.MacOSHelpItems;
#endif
    /* AppendMenu(menu->menu_handle, name); */
    InsertMenuItem(menu->menu_handle, "\p ", idx); /*afterItem */
    SetMenuItemText(menu->menu_handle, idx+1, name);
#if 0
    DrawMenuBar();
#endif
    /* TODO: Can name be freed? */
    vim_free(name);
}

    void
gui_mch_toggle_tearoffs(enable)
    int	    enable;
{
    /* no tearoff menus */
}

/*
 * Destroy the machine specific menu widget.
 */
    void
gui_mch_destroy_menu(menu)
    vimmenu_t	*menu;
{
    short	index = gui_mac_get_menu_item_index (menu, root_menu);
    vimmenu_t	*brother;
/*
    index = menu->index;
*/
    if (index > 0)
    {
	/* Scroll all index number */
/*
	for (brother = menu->next; brother != NULL; brother = brother->next, index++)
	    brother->index = index;
*/
#ifdef USE_HELPMENU
	if (menu->menu_handle != nil) /*gui.MacOSHelpMenu)*/
	{
	/* For now just don't delete help menu items */
#endif
	DeleteMenuItem (menu->menu_handle, index);

	if (menu->submenu_id != 0)
	{
	    DeleteMenu (menu->submenu_id);
	    DisposeMenu (menu->submenu_handle);
	}
#ifdef USE_HELPMENU
	}
#endif
    }
    else
    {
#ifdef USE_HELPMENU
	if (menu->submenu_id != kHMHelpMenuID)
	{
#endif
	DeleteMenu (menu->submenu_id);
	DisposeMenu (menu->submenu_handle);
#ifdef USE_HELPMENU
	}
#endif
    }
    DrawMenuBar();
}

/*
 * Make a menu either grey or not grey.
 */
    void
gui_mch_menu_grey(menu, grey)
    vimmenu_t	*menu;
    int		grey;
{
    /* TODO: Check if menu really exists */
    short index = gui_mac_get_menu_item_index (menu, root_menu);
/*
    index = menu->index;
*/
    if (grey)
    {
	if (menu->children)
	    DisableItem(menu->submenu_handle, index);
	if (menu->menu_handle)
	    DisableItem(menu->menu_handle, index);
    }
    else
    {
	if (menu->children)
	    EnableItem(menu->submenu_handle, index);
	if (menu->menu_handle)
	    EnableItem(menu->menu_handle, index);
    }
}

/*
 * Make menu item hidden or not hidden
 */
    void
gui_mch_menu_hidden(menu, hidden)
    vimmenu_t	*menu;
    int		hidden;
{
    /* TODO: Check if menu really exists */
    short index = gui_mac_get_menu_item_index (menu, root_menu);
/*
    index = menu->index;
*/
    if (hidden)
    {
	if (menu->children)
	    DisableItem(menu->submenu_handle, menu->index);
	if (menu->menu_handle)
	    DisableItem(menu->menu_handle, menu->index);
    }
    else
    {
	if (menu->children)
	    EnableItem(menu->submenu_handle, index);
	if (menu->menu_handle)
	    EnableItem(menu->menu_handle, index);
    }
}


/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    void
gui_mch_draw_menubar()
{
    DrawMenuBar();
}

/*
 * Scrollbar stuff.
 */

    void
gui_mch_enable_scrollbar(sb, flag)
    scrollbar_t	*sb;
    int		flag;
{
    if (flag)
	ShowControl(sb->id);
    else
	HideControl(sb->id);
}

    void
gui_mch_set_scrollbar_thumb(sb, val, size, max)
    scrollbar_t *sb;
    long	val;
    long	size;
    long	max;
{
    SetControlMaximum (sb->id, max);
    SetControlMinimum (sb->id, 0);
    SetControlValue   (sb->id, val);
}

    void
gui_mch_set_scrollbar_pos(sb, x, y, w, h)
    scrollbar_t *sb;
    int		x;
    int		y;
    int		w;
    int		h;
{
    gui_mch_set_bg_color(gui.back_pixel);
/*  if (gui.which_scrollbars[SBAR_LEFT])
    {
	MoveControl (sb->id, x-16, y);
	SizeControl (sb->id, w + 1, h);
    }
    else
    {
	MoveControl (sb->id, x, y);
	SizeControl (sb->id, w + 1, h);
    }*/
    if (sb == &gui.bottom_sbar)
	h += 1;
    else
	w += 1;

    if (gui.which_scrollbars[SBAR_LEFT])
	x -= 15;

    MoveControl (sb->id, x, y);
    SizeControl (sb->id, w, h);
}

    void
gui_mch_create_scrollbar(sb, orient)
    scrollbar_t *sb;
    int		orient;	/* SBAR_VERT or SBAR_HORIZ */
{
    Rect bounds;

    bounds.top = -16;
    bounds.bottom = -10;
    bounds.right = -10;
    bounds.left = -16;

    sb->id = NewControl (gui.VimWindow,
			 &bounds,
			 "\pScrollBar",
			 TRUE,
			 0, /* current*/
			 0, /* top */
			 0, /* bottom */
			 scrollBarProc,
			 (long) sb->ident);
}

#if defined(FEAT_WINDOWS) || defined(PROTO)
    void
gui_mch_destroy_scrollbar(sb)
    scrollbar_t *sb;
{
    gui_mch_set_bg_color(gui.back_pixel);
    DisposeControl (sb->id);
}
#endif


/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON blinking, cursor is shown
 */
    void
gui_mch_set_blinking(long wait, long on, long off)
{
    /* TODO: TODO: TODO: TODO: */
/*    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;*/
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    void
gui_mch_stop_blink()
{
    gui_update_cursor(TRUE, FALSE);
    /* TODO: TODO: TODO: TODO: */
/*    gui_w32_rm_blink_timer();
    if (blink_state == BLINK_OFF)
    gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;*/
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    void
gui_mch_start_blink()
{
    gui_update_cursor(TRUE, FALSE);
    /* TODO: TODO: TODO: TODO: */
/*    gui_w32_rm_blink_timer(); */

    /* Only switch blinking on if none of the times is zero */
/*    if (blink_waittime && blink_ontime && blink_offtime)
    {
    blink_timer = SetTimer(NULL, 0, (UINT)blink_waittime,
			    (TIMERPROC)_OnBlinkTimer);
    blink_state = BLINK_ON;
    gui_update_cursor(TRUE, FALSE);
    }*/
}

/*
 * Return the lightness of a pixel.  White is 255.
 */
    int
gui_mch_get_lightness(pixel)
    guicolor_t	pixel;
{
    return (Red(pixel)*3 + Green(pixel)*6 + Blue(pixel)) / 10;
}

#if (defined(FEAT_SYN_HL) && defined(FEAT_EVAL)) || defined(PROTO)
/*
 * Return the RGB value of a pixel as "#RRGGBB".
 */
    char_u *
gui_mch_get_rgb(pixel)
    guicolor_t	pixel;
{
    static char_u retval[10];

    sprintf((char *)retval, "#%02x%02x%02x",
	Red(pixel), Green(pixel), Blue(pixel));
    return retval;
}
#endif

#ifdef FEAT_BROWSE

/*
 * Pop open a file browser and return the file selected, in allocated memory,
 * or NULL if Cancel is hit.
 *  saving  - TRUE if the file will be saved to, FALSE if it will be opened.
 *  title   - Title message for the file browser dialog.
 *  dflt    - Default name of file.
 *  ext     - Default extension to be added to files without extensions.
 *  initdir - directory in which to open the browser (NULL = current dir)
 *  filter  - Filter for matched files to choose from.
 *  Has a format like this:
 *  "C Files (*.c)\0*.c\0"
 *  "All Files\0*.*\0\0"
 *  If these two strings were concatenated, then a choice of two file
 *  filters will be selectable to the user.  Then only matching files will
 *  be shown in the browser.  If NULL, the default allows all files.
 *
 *  *NOTE* - the filter string must be terminated with TWO nulls.
 */
    char_u *
gui_mch_browse(
    int saving,
    char_u *title,
    char_u *dflt,
    char_u *ext,
    char_u *initdir,
    char_u *filter)
{
    SFTypeList		fileTypes;
    StandardFileReply	reply;
    Str255		Prompt;
    Str255		DefaultName;
    Str255		Directory;
    char_u		fname[256];

    /* TODO: split dflt in path and filename */

    (void) C2PascalString (title,   &Prompt);
    (void) C2PascalString (dflt,    &DefaultName);
    (void) C2PascalString (initdir, &Directory);

    if (saving)
    {
	/* Use a custon filter instead of nil FAQ 9-4 */
	StandardPutFile (Prompt, DefaultName,  &reply);
	if (!reply.sfGood)
	    return NULL;
    }
    else
    {
	StandardGetFile (nil, -1, fileTypes, &reply);
	if (!reply.sfGood)
	    return NULL;
    }

    GetFullPathFromFSSpec ((char_u *) &fname, reply.sfFile);
    /* Work fine but append a : for new file */

    return vim_strsave(fname);

    /* Shorten the file name if possible */
/*    mch_dirname(IObuff, IOSIZE);
    p = shorten_fname(fileBuf, IObuff);
    if (p == NULL)
    p = fileBuf;
    return vim_strsave(p);
*/
}

#endif /* FEAT_BROWSE */

#ifdef FEAT_GUI_DIALOG
/*
 * stuff for dialogues
 */

/*
 * Create a dialogue dynamically from the parameter strings.
 * type       = type of dialogue (question, alert, etc.)
 * title      = dialogue title. may be NULL for default title.
 * message    = text to display. Dialogue sizes to accommodate it.
 * buttons    = '\n' separated list of button captions, default first.
 * dfltbutton = number of default button.
 *
 * This routine returns 1 if the first button is pressed,
 *	    2 for the second, etc.
 *
 *	    0 indicates Esc was pressed.
 *	    -1 for unexpected error
 *
 * If stubbing out this fn, return 1.
 */

    int
gui_mch_dialog(
    int		type,
    char_u	*title,
    char_u	*message,
    char_u	*buttons,
    int		dfltbutton)
{
    Handle	buttonDITL;
    Handle	iconDITL;
    Handle	messageDITL;
    Handle	itemHandle;
    Handle	iconHandle;
    DialogPtr	theDialog;
    char_u	len;
    char_u	name[256];
    GrafPtr	oldPort;
    short	itemHit;
    char_u	*buttonChar;
    char_u	*messageChar;
    Rect	box;
    short	button;
    short	itemType;
    short	useIcon;
    short	appendWhere;

    theDialog = GetNewDialog (129, nil, (WindowRef) -1);
    /*	SetTitle (title); */

    buttonDITL = GetResource ('DITL', 130);
    buttonChar = buttons;
    button = 0;

    for (;*buttonChar != 0;)
    {
	button++;
	len = 0;
	for (;((*buttonChar != DLG_BUTTON_SEP) && (*buttonChar != 0) && (len < 255)); buttonChar++)
	{
	    if (*buttonChar != DLG_HOTKEY_CHAR)
		name[++len] = *buttonChar;
	}
	if (*buttonChar != 0)
	  buttonChar++;
	name[0] = len;

	AppendDITL (theDialog, buttonDITL, appendDITLRight);
	GetDialogItem (theDialog, button, &itemType, &itemHandle, &box);
	SetControlTitle ((ControlRef) itemHandle, name);
	SetDialogItem (theDialog, button, itemType, itemHandle, &box);
    }
    ReleaseResource (buttonDITL);

    iconDITL = GetResource ('DITL', 131);
    switch (type)
    {
	case VIM_GENERIC:  useIcon = kNoteIcon;
	case VIM_ERROR:    useIcon = kStopIcon;
	case VIM_WARNING:  useIcon = kCautionIcon;
	case VIM_INFO:     useIcon = kNoteIcon;
	case VIM_QUESTION: useIcon = kNoteIcon;
	default:      useIcon = kStopIcon;
    };
    AppendDITL (theDialog, iconDITL, overlayDITL);
    ReleaseResource (iconDITL);
    GetDialogItem (theDialog, button + 1, &itemType, &itemHandle, &box);
    /* Should the item be freed */
    iconHandle = GetIcon (useIcon);
    SetDialogItem (theDialog, button + 1, itemType, (Handle) iconHandle, &box);


    messageDITL = GetResource ('DITL', 132);
    AppendDITL (theDialog, iconDITL, overlayDITL);
    ReleaseResource (iconDITL);
    GetDialogItem (theDialog, button + 2, &itemType, &itemHandle, &box);
    messageChar = message;
    len = 1;
    for (; (*messageChar != 0) && (len <255); len++, messageChar++)
    {
	name[len] = *messageChar;
    }
    name[0] = len;

    SetDialogItemText (itemHandle, name);

    SetDialogDefaultItem (theDialog, dfltbutton);
    SetDialogCancelItem (theDialog, 0);

    GetPort (&oldPort);
    SetPort (theDialog);

    ModalDialog (nil, &itemHit);
    SetPort (oldPort);
    DisposeDialog (theDialog);

    return itemHit;
}
#endif /* GUI_DIALOGUE */
/*
 * Apple Event Handling procedure
 *
 */

#ifdef USE_AEVENT

/*
 * Handle the Unused parms of an AppleEvent
 */

OSErr HandleUnusedParms (AppleEvent *theAEvent)
{
    OSErr	error;
    long	actualSize;
    DescType	dummyType;
    AEKeyword	missedKeyword;

/* Get the "missed keyword" attribute from the AppleEvent. */
    error = AEGetAttributePtr(theAEvent, keyMissedKeywordAttr,
			      typeKeyword, &dummyType,
			      (Ptr)&missedKeyword, sizeof(missedKeyword),
			      &actualSize);

/* If the descriptor isn't found, then we got the required parameters. */
    if (error == errAEDescNotFound)
    {
	error = noErr;
    }
    else
    {
/*	error = errAEEventNotHandled;*/
    }

    return error;
}


/*
 * Handle the ODoc AppleEvent
 *
 * Deals with all files dragged to the application icon.
 *
 */

#pragma options align=mac68k
typedef struct SelectionRange SelectionRange;
struct SelectionRange /* for handling kCoreClassEvent:kOpenDocuments:keyAEPosition typeChar */
{
    short unused1; // 0 (not used)
    short lineNum; // line to select (<0 to specify range)
    long startRange; // start of selection range (if line < 0)
    long endRange; // end of selection range (if line < 0)
    long unused2; // 0 (not used)
    long theDate; // modification date/time
};
#pragma options align=reset
/* The IDE uses the optional keyAEPosition parameter to tell the ed-
   itor the selection range. If lineNum is zero or greater, scroll the text
   to the specified line. If lineNum is less than zero, use the values in
   startRange and endRange to select the specified characters. Scroll
   the text to display the selection. If lineNum, startRange, and
   endRange are all negative, there is no selection range specified.
 */

pascal OSErr HandleODocAE (AppleEvent *theAEvent, AppleEvent *theReply, long refCon)
{
    /*
     * TODO: Clean up the code with convert the AppleEvent into
     *       a ":args"
     */
    OSErr	error = noErr;
    OSErr	firstError = noErr;
    short	numErrors = 0;
    AEDesc	theList;
    DescType	typeCode;
    long	numFiles, fileCount;
    char_u	**fnames;
    char_u	fname[256];
    Size	actualSize;
    SelectionRange thePosition;
    short	gotPosition = false;
    long	lnum;

#ifdef USE_SIOUX
    printf ("aevt_odoc:\n");
#endif

/* the direct object parameter is the list of aliases to files (one or more) */
    error = AEGetParamDesc(theAEvent, keyDirectObject, typeAEList, &theList);
    if (error)
    {
#ifdef USE_SIOUX
	printf ("aevt_odoc: AEGetParamDesc error: %d\n", error);
#endif
	return(error);
    }


    error = AEGetParamPtr(theAEvent, keyAEPosition, typeChar, &typeCode, (Ptr) &thePosition, sizeof(SelectionRange), &actualSize);
    if (error == noErr)
	gotPosition = true;
    if (error == errAEDescNotFound)
	error = noErr;
    if (error)
    {
#ifdef USE_SIOUX
	printf ("aevt_odoc: AEGetParamPtr error: %d\n", error);
#endif
	return(error);
    }

#ifdef USE_SIOUX
    printf ("aevt_odoc: lineNum: %d, startRange %d, endRange %d, [date %lx]\n",
	    thePosition.lineNum, thePosition.startRange, thePosition.endRange,
	    thePosition.theDate);
#endif
/*
    error = AEGetParamDesc(theAEvent, keyAEPosition, typeChar, &thePosition);

    if (^error) then
    {
	if (thePosition.lineNum >= 0)
	{
	  // Goto this line
	}
	else
	{
	  // Set the range char wise
	}
    }
 */

/* get number of files in list */
    error = AECountItems(&theList, &numFiles);
    if (error)
    {
#ifdef USE_SIOUX
	printf ("aevt_odoc: AECountItems error: %d\n", error);
#endif
	return(error);
    }

#ifdef FEAT_VISUAL
    reset_VIsual();
#endif

    fnames = (char_u **) alloc(numFiles * sizeof(char_u *));

/* open each file - keep track of errors     */
    for (fileCount = 1; fileCount <= numFiles; fileCount++)
    {
	FSSpec		fileToOpen;
	long		actualSize;
	AEKeyword	dummyKeyword;
	DescType	dummyType;

	/* get the alias for the nth file, convert to an FSSpec */
	error = AEGetNthPtr(&theList, fileCount, typeFSS, &dummyKeyword, &dummyType,
				(Ptr) &fileToOpen, sizeof(FSSpec), &actualSize);
	if (error)
	{
	    /* TODO: free fnames and it's child */
#ifdef USE_SIOUX
	    printf ("aevt_odoc: AEGetNthPtr error: %d\n", error);
#endif
	    return(error);
	}
	GetFullPathFromFSSpec (fname, fileToOpen);
	fnames[fileCount - 1] = vim_strsave(fname);
#ifdef USE_SIOUX
	printf ("aevt_odoc: %s\n", fname);
#endif
    }


    /* Handle the drop, :edit to get to the file */
    handle_drop(numFiles, fnames, FALSE);

    /* TODO: Handle the goto/select line more cleanly */
    if ((numFiles == 1) & (gotPosition))
    {
	if (thePosition.lineNum >= 0)
	{
	    lnum = thePosition.lineNum;
	/*  oap->motion_type = MLINE;
	    setpcmark();*/
	    if (lnum < 1L)
		lnum = 1L;
	    else if (lnum > curbuf->b_ml.ml_line_count)
		lnum = curbuf->b_ml.ml_line_count;
	    curwin->w_cursor.lnum = lnum;
	/*  beginline(BL_SOL | BL_FIX);*/
	}
	else
	    goto_byte(thePosition.startRange + 1);
    }

    /* Update the screen display */
    update_screen(NOT_VALID);
    setcursor();
    out_flush();

    AEDisposeDesc(&theList); /* dispose what we allocated */

    error = HandleUnusedParms (theAEvent);
    if (error)
    {
#ifdef USE_SIOUX
	printf ("aevt_odoc: HandleUnusedParms error: %d\n", error);
#endif
	return(error);
    }
    return(error);
}

pascal OSErr Handle_aevt_oapp_AE (AppleEvent *theAEvent, AppleEvent *theReply, long refCon)
{
    OSErr	error = noErr;

#ifdef USE_SIOUX
    printf ("aevt_oapp:\n");
#endif

    error = HandleUnusedParms (theAEvent);
    if (error)
    {
	return(error);
    }

    return(error);
}

pascal OSErr Handle_aevt_quit_AE (AppleEvent *theAEvent, AppleEvent *theReply, long refCon)
{
    OSErr	error = noErr;

#ifdef USE_SIOUX
    printf ("aevt_quit\n");
#endif

    error = HandleUnusedParms (theAEvent);
    if (error)
    {
	return(error);
    }

    return(error);
}

pascal OSErr Handle_aevt_pdoc_AE (AppleEvent *theAEvent, AppleEvent *theReply, long refCon)
{
    OSErr	error = noErr;

#ifdef USE_SIOUX
    printf ("aevt_pdoc:\n");
#endif

    error = HandleUnusedParms (theAEvent);
    if (error)
    {
	return(error);
    }

    return(error);
}

/*
 * Handle the Window Search event from CodeWarrior
 *
 * Description
 * -----------
 *
 * The IDE sends the Window Search AppleEvent to the editor when it
 * needs to know whether a particular file is open in the editor.
 *
 * Event Reply
 * -----------
 *
 * None. Put data in the location specified in the structure received.
 *
 * Remarks
 * -------
 *
 * When the editor receives this event, determine whether the specified
 * file is open. If it is, return the modification date/time for that file
 * in the appropriate location specified in the structure. If the file is
 * not opened, put the value fnfErr (file not found) in that location.
 *
 */

#pragma options align=mac68k
typedef struct WindowSearch WindowSearch;
struct WindowSearch /* for handling class 'KAHL', event 'SRCH', keyDirectObject typeChar*/
{
    FSSpec theFile; // identifies the file
    long *theDate; // where to put the modification date/time
};
#pragma options align=reset

pascal OSErr Handle_KAHL_SRCH_AE (AppleEvent *theAEvent, AppleEvent *theReply, long refCon)
{
    OSErr	error = noErr;
    buf_t	*buf;
    int		foundFile = false;
    FSSpec	find_FSSpec;
    DescType	typeCode;
    WindowSearch SearchData;
    Size	actualSize;

    error = AEGetParamPtr(theAEvent, keyDirectObject, typeChar, &typeCode, (Ptr) &SearchData, sizeof(WindowSearch), &actualSize);
    if (error)
    {
#ifdef USE_SIOUX
	printf ("KAHL_SRCH: AEGetParamPtr error: %d\n", error);
#endif
	return(error);
    }

    error = HandleUnusedParms (theAEvent);
    if (error)
    {
#ifdef USE_SIOUX
	printf ("KAHL_SRCH: HandleUnusedParms error: %d\n", error);
#endif
	return(error);
    }

    for (buf = firstbuf; buf != NULL; buf = buf->b_next)
	if (buf->b_ml.ml_mfp != NULL)
	    if (SearchData.theFile.parID == buf->b_FSSpec.parID)
	      if (SearchData.theFile.name[0] = buf->b_FSSpec.name[0])
		if (STRNCMP(SearchData.theFile.name, buf->b_FSSpec.name, buf->b_FSSpec.name[0]+1))
	    {
		foundFile = true;
		break;
	    }

    if (foundFile == false)
	*SearchData.theDate = fnfErr;
    else
	*SearchData.theDate = buf->b_mtime;

#ifdef USE_SIOUX
    printf ("KAHL_SRCH: file \"%#s\" {%d}", SearchData.theFile.name,SearchData.theFile.parID);
    if (foundFile == false)
	printf (" NOT");
    printf (" found. [date %lx, %lx]\n", *SearchData.theDate, buf->b_mtime_read);
#endif

    return error;
};

/*
 * Handle the Modified (from IDE to Editor) event from CodeWarrior
 *
 * Description
 * -----------
 *
 * The IDE sends this event to the external editor when it wants to
 * know which files that are open in the editor have been modified.
 *
 * Parameters   None.
 * ----------
 *
 * Event Reply
 * -----------
 * The reply for this event is:
 *
 * keyDirectObject typeAEList required
 *  each element in the list is a structure of typeChar
 *
 * Remarks
 * -------
 *
 * When building the reply event, include one element in the list for
 * each open file that has been modified.
 *
 */

#pragma options align=mac68k
typedef struct ModificationInfo ModificationInfo;
struct ModificationInfo /* for replying to class 'KAHL', event 'MOD ', keyDirectObject typeAEList*/
{
    FSSpec theFile; // identifies the file
    long theDate; // the date/time the file was last modified
    short saved; // set this to zero when replying, unused
};
#pragma options align=reset

pascal OSErr Handle_KAHL_MOD_AE (AppleEvent *theAEvent, AppleEvent *theReply, long refCon)
{
    OSErr	error = noErr;
    AEDescList	replyList;
    long	numFiles;
    ModificationInfo theFile;
    buf_t	*buf;

    theFile.saved = 0;

    error = HandleUnusedParms (theAEvent);
    if (error)
    {
#ifdef USE_SIOUX
	printf ("KAHL_MOD: HandleUnusedParms error: %d\n", error);
#endif
	return(error);
    }

    /* Send the reply */
/*  replyObject.descriptorType = typeNull;
    replyObject.dataHandle     = nil;*/

/* AECreateDesc(typeChar, (Ptr)&title[1], title[0], &data) */
    error = AECreateList(nil, 0, false, &replyList);
    if (error)
    {
#ifdef USE_SIOUX
	printf ("KAHL_MOD: AECreateList error: %d\n", error);
#endif
	return(error);
    }

#if 0
    error = AECountItems(&replyList, &numFiles);
#ifdef USE_SIOUX
    printf ("KAHL_MOD ReplyList: %x %x\n", replyList.descriptorType, replyList.dataHandle);
    printf ("KAHL_MOD ItemInList: %d\n", numFiles);
#endif

    /* AEPutKeyDesc (&replyList, keyAEPnject, &aDesc)
     * AEPutKeyPtr  (&replyList, keyAEPosition, typeChar, (Ptr)&theType,
     * sizeof(DescType))
     */

    /* AEPutDesc */
#endif

    numFiles = 0;
    for (buf = firstbuf; buf != NULL; buf = buf->b_next)
	if (buf->b_ml.ml_mfp != NULL)
	{
	    /* Add this file to the list */
	    theFile.theFile = buf->b_FSSpec;
	    theFile.theDate = buf->b_mtime;
/*	    theFile.theDate = time (NULL) & (time_t) 0xFFFFFFF0; */
	    error = AEPutPtr (&replyList, numFiles, typeChar, (Ptr) &theFile, sizeof(theFile));
#ifdef USE_SIOUX
	    if (numFiles == 0)
		printf ("KAHL_MOD: ");
	    else
		printf (", ");
	    printf ("\"%#s\" {%d} [date %lx, %lx]", theFile.theFile.name, theFile.theFile.parID, theFile.theDate, buf->b_mtime_read);
	    if (error)
		printf (" (%d)", error);
	    numFiles++;
#endif
	};

#ifdef USE_SIOUX
    printf ("\n");
#endif

#if 0
    error = AECountItems(&replyList, &numFiles);
#ifdef USE_SIOUX
    printf ("KAHL_MOD ItemInList: %d\n", numFiles);
#endif
#endif

    /* We can add data only if something to reply */
    error = AEPutParamDesc (theReply, keyDirectObject, &replyList);

#ifdef USE_SIOUX
    if (error)
	printf ("KAHL_MOD: AEPutParamDesc error: %d\n", error);
#endif

    if (replyList.dataHandle)
	AEDisposeDesc(&replyList);

    return error;
};

/*
 * Handle the Get Text event from CodeWarrior
 *
 * Description
 * -----------
 *
 * The IDE sends the Get Text AppleEvent to the editor when it needs
 * the source code from a file. For example, when the user issues a
 * Check Syntax or Compile command, the compiler needs access to
 * the source code contained in the file.
 *
 * Event Reply
 * -----------
 *
 * None. Put data in locations specified in the structure received.
 *
 * Remarks
 * -------
 *
 * When the editor receives this event, it must set the size of the handle
 * in theText to fit the data in the file. It must then copy the entire
 * contents of the specified file into the memory location specified in
 * theText.
 *
 */

#pragma options align=mac68k
typedef struct CW_GetText CW_GetText;
struct CW_GetText /* for handling class 'KAHL', event 'GTTX', keyDirectObject typeChar*/
{
    FSSpec theFile; /* identifies the file */
    Handle theText; /* the location where you return the text (must be resized properly) */
    long *unused;   /* 0 (not used) */
    long *theDate;  /* where to put the modification date/time */
};
#pragma options align=reset

pascal OSErr Handle_KAHL_GTTX_AE (AppleEvent *theAEvent, AppleEvent *theReply, long refCon)
{
    OSErr	error = noErr;
    buf_t	*buf;
    int		foundFile = false;
    DescType	typeCode;
    CW_GetText	GetTextData;
    Size	actualSize;
    char_u	*line;
    char_u	*fullbuffer;
    long	linesize;
    long	lineStart;
    long	BufferSize;
    long	lineno;

    error = AEGetParamPtr(theAEvent, keyDirectObject, typeChar, &typeCode, (Ptr) &GetTextData, sizeof(GetTextData), &actualSize);

    if (error)
    {
#ifdef USE_SIOUX
	printf ("KAHL_GTTX: AEGetParamPtr error: %d\n", error);
#endif
	return(error);
    }

    for (buf = firstbuf; buf != NULL; buf = buf->b_next)
	if (buf->b_ml.ml_mfp != NULL)
	    if (GetTextData.theFile.parID == buf->b_FSSpec.parID)
	    {
		foundFile = true;
		break;
	    }

    if (foundFile)
    {
	BufferSize = 0; /* GetHandleSize (GetTextData.theText); */
        for (lineno = 0; lineno <= buf->b_ml.ml_line_count; lineno++)
	{
            /* Must use the right buffer */
            line = ml_get_buf(buf, (linenr_t) lineno, FALSE);
	    linesize = STRLEN(line) + 1;
	    lineStart = BufferSize;
	    BufferSize += linesize;
	    /* Resize handle to linesize+1 to include the linefeed */
	    SetHandleSize (GetTextData.theText, BufferSize);
	    if (GetHandleSize (GetTextData.theText) != BufferSize)
	    {
	#ifdef USE_SIOUX
		printf ("KAHL_GTTX: SetHandleSize increase: %d, size %d\n",
			linesize, BufferSize);
	#endif
		break; /* Simple handling for now */
	    }
	    else
	    {
		HLock (GetTextData.theText);
		fullbuffer = (char_u *) *GetTextData.theText;
		STRCPY ((char_u *) (fullbuffer + lineStart), line);
		fullbuffer[BufferSize-1] = '\r';
		HUnlock (GetTextData.theText);
	    }
	}
	HLock (GetTextData.theText);
	fullbuffer[BufferSize-1] = 0;
	HUnlock (GetTextData.theText);
	if (foundFile == false)
	    *GetTextData.theDate = fnfErr;
	else
/*	    *GetTextData.theDate = time (NULL) & (time_t) 0xFFFFFFF0;*/
	    *GetTextData.theDate = buf->b_mtime;
    }
#ifdef USE_SIOUX
    printf ("KAHL_GTTX: file \"%#s\" {%d} [date %lx, %lx]", GetTextData.theFile.name, GetTextData.theFile.parID, *GetTextData.theDate, buf->b_mtime_read);
    if (foundFile == false)
	printf (" NOT");
    printf (" found. (BufferSize = %d)\n", BufferSize);
#endif

    error = HandleUnusedParms (theAEvent);
    if (error)
    {
#ifdef USE_SIOUX
	printf ("KAHL_GTTX: HandleUnusedParms error: %d\n", error);
#endif
	return(error);
    }

    return(error);
}

pascal OSErr Handle_unknown_AE (AppleEvent *theAEvent, AppleEvent *theReply, long refCon)
{
    OSErr	error = noErr;

#ifdef USE_SIOUX
    printf ("Unknown Event: %x\n", theAEvent->descriptorType);
#endif

    error = HandleUnusedParms (theAEvent);
    if (error)
    {
	return(error);
    }

    return(error);
}

/*
 * Install the various AppleEvent Handlers
 */
OSErr   InstallAEHandlers (void)
{
    OSErr   error;

/* install open application handler */
    error = AEInstallEventHandler(kCoreEventClass, kAEOpenApplication,
		    NewAEEventHandlerProc(Handle_aevt_oapp_AE), 0, false);
    if (error)
    {
	return error;
    }

/* install quit application handler */
    error = AEInstallEventHandler(kCoreEventClass, kAEQuitApplication,
		    NewAEEventHandlerProc(Handle_aevt_quit_AE), 0, false);
    if (error)
    {
	return error;
    }

/* install open document handler */
    error = AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments,
		    NewAEEventHandlerProc(HandleODocAE), 0, false);
    if (error)
    {
	return error;
    }

/* install print document handler */
    error = AEInstallEventHandler(kCoreEventClass, kAEPrintDocuments,
		    NewAEEventHandlerProc(Handle_aevt_pdoc_AE), 0, false);

/* Install Core Suite */
/*  error = AEInstallEventHandler(kAECoreSuite, kAEClone,
		    NewAEEventHandlerProc(Handle_unknown_AE), nil, false);

    error = AEInstallEventHandler(kAECoreSuite, kAEClose,
		    NewAEEventHandlerProc(Handle_unknown_AE), nil, false);

    error = AEInstallEventHandler(kAECoreSuite, kAECountElements,
		    NewAEEventHandlerProc(Handle_unknown_AE), nil, false);

    error = AEInstallEventHandler(kAECoreSuite, kAECreateElement,
		    NewAEEventHandlerProc(Handle_unknown_AE), nil, false);

    error = AEInstallEventHandler(kAECoreSuite, kAEDelete,
		    NewAEEventHandlerProc(Handle_unknown_AE), nil, false);

    error = AEInstallEventHandler(kAECoreSuite, kAEDoObjectsExist,
		    NewAEEventHandlerProc(Handle_unknown_AE), nil, false);

    error = AEInstallEventHandler(kAECoreSuite, kAEGetData,
		    NewAEEventHandlerProc(Handle_unknown_AE), kAEGetData, false);

    error = AEInstallEventHandler(kAECoreSuite, kAEGetDataSize,
		    NewAEEventHandlerProc(Handle_unknown_AE), kAEGetDataSize, false);

    error = AEInstallEventHandler(kAECoreSuite, kAEGetClassInfo,
		    NewAEEventHandlerProc(Handle_unknown_AE), nil, false);

    error = AEInstallEventHandler(kAECoreSuite, kAEGetEventInfo,
		    NewAEEventHandlerProc(Handle_unknown_AE), nil, false);

    error = AEInstallEventHandler(kAECoreSuite, kAEMove,
		    NewAEEventHandlerProc(Handle_unknown_AE), nil, false);

    error = AEInstallEventHandler(kAECoreSuite, kAESave,
		    NewAEEventHandlerProc(Handle_unknown_AE), nil, false);

    error = AEInstallEventHandler(kAECoreSuite, kAESetData,
		    NewAEEventHandlerProc(Handle_unknown_AE), nil, false);

    /*
     * Bind codewarrior support handlers
     */
    error = AEInstallEventHandler('KAHL', 'GTTX',
		    NewAEEventHandlerProc(Handle_KAHL_GTTX_AE), 0, false);
    if (error)
    {
	return error;
    }
    error = AEInstallEventHandler('KAHL', 'SRCH',
		    NewAEEventHandlerProc(Handle_KAHL_SRCH_AE), 0, false);
    if (error)
    {
	return error;
    }
    error = AEInstallEventHandler('KAHL', 'MOD ',
		    NewAEEventHandlerProc(Handle_KAHL_MOD_AE), 0, false);
    if (error)
    {
	return error;
    }

    return error;

}

/* Taken from MoreAppleEvents:ProcessHelpers*/
pascal	OSErr	FindProcessBySignature( const OSType targetType,
					const OSType targetCreator,
					      ProcessSerialNumberPtr psnPtr )
{
    OSErr	anErr = noErr;
    Boolean	lookingForProcess = true;

    ProcessInfoRec  infoRec;

    infoRec.processInfoLength = sizeof( ProcessInfoRec );
    infoRec.processName = nil;
    infoRec.processAppSpec = nil;

    psnPtr->lowLongOfPSN = kNoProcess;
    psnPtr->highLongOfPSN = kNoProcess;

    while ( lookingForProcess )
    {
	anErr = GetNextProcess( psnPtr );
	if ( anErr != noErr )
	{
	    lookingForProcess = false;
	}
	else
	{
	    anErr = GetProcessInformation( psnPtr, &infoRec );
	    if ( ( anErr == noErr )
		 && ( infoRec.processType == targetType )
		 && ( infoRec.processSignature == targetCreator ) )
	    {
		lookingForProcess = false;
	    }
	}
    }

    return anErr;
}//end FindProcessBySignature

void Send_KAHL_MOD_AE (buf_t *buf)
{
    OSErr   anErr = noErr;
    AEDesc  targetAppDesc = { typeNull, nil };
    ProcessSerialNumber	    psn = { kNoProcess, kNoProcess };
    AppleEvent	theReply = { typeNull, nil };
    AESendMode	sendMode;
    AppleEvent  theEvent = {typeNull, nil };
    AEIdleUPP   idleProcUPP = nil;
    ModificationInfo ModData;


    anErr = FindProcessBySignature( 'APPL', 'CWIE', &psn );
#ifdef USE_SIOUX
    printf ("CodeWarrior is");
    if (anErr != noErr)
	printf (" NOT");
    printf (" running\n");
#endif
    if ( anErr == noErr )
    {
	anErr = AECreateDesc (typeProcessSerialNumber, &psn,
			      sizeof( ProcessSerialNumber ), &targetAppDesc);

	if ( anErr == noErr )
	{
	    anErr = AECreateAppleEvent( 'KAHL', 'MOD ', &targetAppDesc,
					kAutoGenerateReturnID, kAnyTransactionID, &theEvent);
	}

	AEDisposeDesc( &targetAppDesc );

	/* Add the parms */
	ModData.theFile = buf->b_FSSpec;
	ModData.theDate = buf->b_mtime;

	if (anErr == noErr)
	    anErr =AEPutParamPtr (&theEvent, keyDirectObject, typeChar, &ModData, sizeof(ModData));

	if ( idleProcUPP == nil )
	    sendMode = kAENoReply;
	else
	    sendMode = kAEWaitReply;

	if ( anErr == noErr )
	    anErr = AESend( &theEvent, &theReply, sendMode, kAENormalPriority, kNoTimeOut, idleProcUPP, nil );
	if ( anErr == noErr  &&  sendMode == kAEWaitReply )
	{
#ifdef USE_SIOUX
	    printf ("KAHL_MOD: Send error: %d\n", anErr);
#endif
/*	    anErr =  AEHGetHandlerError( &theReply );*/
	}
	(void) AEDisposeDesc( &theReply );
    }
}
#endif /* USE_AEVENT */


/*
 * Record an error message for later display.
 */
    void
mch_errmsg(char *str)
{
    int		len = STRLEN(str) + 1;

    if (error_ga.ga_growsize == 0)
    {
	error_ga.ga_growsize = 80;
	error_ga.ga_itemsize = 1;
    }
    if (ga_grow(&error_ga, len) == OK)
    {
	mch_memmove((char_u *)error_ga.ga_data + error_ga.ga_len,
			      (char_u *)str, len);
	--len;		/* don't count the NUL at the end */
	error_ga.ga_len += len;
	error_ga.ga_room -= len;
    }
}

/*
 * Display the saved error message(s).
 */
    void
mch_display_error()
{
    char	*p;
    char_u	pError[256];

    if (error_ga.ga_data != NULL)
    {
	/* avoid putting up a message box with blanks only */
	for (p = (char *)error_ga.ga_data; *p; ++p)
	    if (!isspace(*p))
	    {
		if (STRLEN(p) > 255)
		    pError[0] = 255;
		else
		    pError[0] = STRLEN(p);

		STRNCPY(&pError[1], p, pError[0]);
		ParamText (pError, nil, nil, nil);
		Alert (128, nil);
		break;
		/* TODO: handled message longer than 256 chars
		 *	 use auto-sizeable alert
		 *	 or dialog with scrollbars (TextEdit zone)
		 */
	    }
	ga_clear(&error_ga);
    }
}

/*
 * Get current y mouse coordinate in text window.
 * Return -1 when unknown.
 */
    int
gui_mch_get_mouse_x()
{
    Point where;

    GetMouse(&where);

    return (where.h);
}

    int
gui_mch_get_mouse_y()
{
    Point where;

    GetMouse(&where);

    return (where.v);
}

    void
gui_mch_setmouse(x, y)
    int		x;
    int		y;
{
    /* TODO */
#if 0
    /* From FAQ 3-11 */

    CursorDevicePtr myMouse;
    Point           where;

    if (   NGetTrapAddress (_CursorDeviceDispatch, ToolTrap)
	!= NGetTrapAddress (_Unimplemented,   ToolTrap) )
    {
	/* New way */

	/*
	 * Get first devoice with one button.
	 * This will probably be the standad mouse
	 * startat head of cursor dev list
	 *
	 */

	myMouse = nil;

	do
	{
	    /* Get the next cursor device */
	    CursorDeviceNextDevice(&myMouse);
	}
	while ( (myMouse != nil) && (myMouse->cntButtons != 1) );

	CursorDeviceMoveTo (myMouse, x, y);
    }
    else
    {
	/* Old way */
	where.h = x;
	where.v = y;

	*(Point *)RawMouse = where;
	*(Point *)MTemp    = where;
	*(Ptr)    CrsrNew  = 0xFFFF;
    }
#endif
}

    void
gui_mch_show_popupmenu(menu)
    vimmenu_t *menu;
{
#ifdef USE_CTRLCLICKMENU
/*
 *  Clone PopUp to use menu
 *  Create a object descriptor for the current selection
 *  Call the procedure
 */

    MenuHandle	CntxMenu;
    Point	where;
    OSStatus	status;
    UInt32	CntxType;
    SInt16	CntxMenuID;
    UInt16	CntxMenuItem;
    Str255	HelpName = "";

    GetMouse (&where);
    CntxMenu = menu->submenu_handle;

//  Call to Handle Popup
    status = ContextualMenuSelect(CntxMenu, where, false, kCMHelpItemNoHelp, HelpName, NULL, &CntxType, &CntxMenuID, &CntxMenuItem);

    if (status != noErr)
	return;

    if (CntxType == kCMMenuItemSelected)
    {
	/* Handle the menu CntxMenuID, CntxMenuItem */
	/* The submenu can be handle directly by gui_mac_handle_menu */
	/* But what about the current menu, is the meny changed by ContextualMenuSelect */
	gui_mac_handle_menu ((CntxMenuID << 16) + CntxMenuItem);
    }
    else if (CntxMenuID == kCMShowHelpSelected)
    {
	/* Should come up with the help */
    }
#endif
}

void mch_post_buffer_write (buf_t *buf)
{
#ifdef USE_SIOUX
    printf ("Writing Buf...\n");
#endif
    GetFSSpecFromPath (buf->b_ffname, &buf->b_FSSpec);
    Send_KAHL_MOD_AE (buf);
}
