/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				Amiga GUI support by Michael Nielsen
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#include <stdlib.h>
#include <string.h>
#include <exec/types.h>
#include <intuition/intuition.h>
#include <utility/tagitem.h>
#include <graphics/text.h>
#include <graphics/rastport.h>
#include <graphics/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <assert.h>
#include "gui_amiga.h"
#include "vim.h"
#include <math.h>
#include <limits.h>

#include "version.h"

#if defined(FEAT_GUI_AMIGA) || defined(PROTO)

#define KEYUP 76
#define KEYDOWN 77
#define KEYRIGHT 78
#define KEYLEFT 79

/* When generating prototypes on Unix, these need to be defined */
#ifdef PROTO
# define STRPTR char *
#endif

static struct PropInfo Gadget2SInfo = { AUTOKNOB+PROPBORDERLESS, (unsigned short)-1, (unsigned short)-1, 6553, 6553, };
static struct Image Image1 = { 0, 0, 10, 397,	0, NULL, 0x0000, 0x0000, NULL };
static struct Gadget propGadget = { NULL, -12, 15, 10, -28,
	GFLG_RELRIGHT+GFLG_RELHEIGHT,
	GACT_RELVERIFY+GACT_RIGHTBORDER+GACT_IMMEDIATE,
	GTYP_PROPGADGET+GTYP_GZZGADGET,
	(APTR)&Image1, NULL,
	NULL, NULL, (APTR)&Gadget2SInfo, NULL, NULL };

struct GFXBase		*gfxBase;
struct ExecBase		*execBase;
struct LayersBase	*layersBase;

struct TagItem tags[] =
{
    {WA_Left, 0},
    {WA_Top, 0},
    {WA_Width, 400},
    {WA_Height, 400},
    {WA_Title, (ULONG)VIM_VERSION_SHORT},
    {WA_ScreenTitle, (ULONG)VIM_VERSION_LONG},
    {WA_DragBar, TRUE},			/* enable dragging of the window */
    {WA_DepthGadget, TRUE},		/* enable the depth gadget */
    {WA_CloseGadget, TRUE},		/* enable the close gadget*/
    {WA_SizeGadget, TRUE},		/* enable the size gadget */
    {WA_SizeBBottom, TRUE},		/* sizegadget contained in bottom border */
    {WA_SmartRefresh, TRUE},		/* choose smart refresh, saves us doing a lot of work */
    {WA_ReportMouse, TRUE},		/* Report the position of the mouse */
    {WA_GimmeZeroZero, TRUE},
    {WA_Activate, TRUE},		/* Activate window on startup */
    {WA_Activate, TRUE},		/* Activate window on startup */
    {WA_NoCareRefresh, TRUE},		/* Refresh screen, don't tell us */
    {WA_NewLookMenus, TRUE},		/* use the new options for the menu */
    {WA_AutoAdjust, TRUE},		/* If window is too big for screen adjust size*/
    {WA_NoCareRefresh, TRUE},		/* If window is too big for screen adjust size*/
    {WA_MouseQueue, 1},			/* Limit number of pending mouse movement*/
    {WA_RptQueue, 10},			/* Limit number of pending keystrokes*/
    {WA_IDCMP,				/* IDCMP, what events interest us  */
	IDCMP_NEWSIZE			/* Notify us about size change of window*/
	    |IDCMP_REFRESHWINDOW	/* Notify us when the window needs refreshing */
	    |IDCMP_MOUSEBUTTONS		/* Notify us when the mouse buttons have been used */
	    |IDCMP_MOUSEMOVE		/* Notify us when the mouse is moving */
	    |IDCMP_GADGETDOWN		/* Notify us when a gadget has been selected */
	    |IDCMP_GADGETUP		/* Notify us when a gadget has been released */
	    |IDCMP_MENUPICK		/* Notify us when a menu has been picked */
	    |IDCMP_CLOSEWINDOW		/* Notify us when the user tries to close the window */
	    |IDCMP_VANILLAKEY		/* Notify us about keystrokes */
	    |IDCMP_RAWKEY		/* Notify us when raw key events have been used, ie cursor*/
	    |IDCMP_INTUITICKS		/* Simpler timer for the blink option */
	    |IDCMP_MENUHELP		/* Allow the help key to be used during menu events */
	    |IDCMP_GADGETHELP		/* Allow the help key to be used during gadget events */
	    |IDCMP_INACTIVEWINDOW	/* notify of inactive window */
	    |IDCMP_ACTIVEWINDOW		/* notify of inactive window */
    },
    {TAG_DONE, NULL}
};

#if defined(D)
#undef D
#endif

/*#define D(_msg) fprintf(stderr, "%s\n", _msg)*/

#define D(_A)

static void AmigaError(const char *string);

void HandleEvent(unsigned long * object);

static struct NewWindow vimNewWindow =
{
    0, 0,		/* window XY origin relative to TopLeft of screen */
    0, 0,		/* window width and height */
    0, 1,		/* detail and block pens */
    NULL,		/* IDCMP flags */
    NULL,		/* other window flags */
    &propGadget,	/* first gadget in gadget list */
    NULL,		/* custom CHECKMARK imagery */
    "Amiga Vim gui",	/* window title */
    NULL,		/* custom screen pointer */
    NULL,		/* custom bitmap */
    50, 50,		/* minimum width and height */
    (unsigned short)-1, (unsigned short)-1,	/* maximum width and height */
    WBENCHSCREEN	/* destination screen type */
};

static struct
{
    unsigned int  key_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_keys[] =
{
    {0,		    0, 0}
};

#if 0
    /* not used? */
    static int
hex_digit(int c)
{
    if (isdigit(c))
	return c - '0';
    c = TO_LOWER(c);
    if (c >= 'a' && c <= 'f')
	return c - 'a' + 10;
    return -1000;
}
#endif

static int characterWidth = -1;
static int characterHeight = -1;
static struct
{
    BOOL	active;
    enum
    {
	CursorOff,
	CursorOn,
	CursorWait
    }		state;
    int		onTime;
    int		offTime;
    int		waitTime;
    int		current;
} cursor =
{
    TRUE,
    CursorWait,
    10,
    10,
    7,
    0
};

enum DrawBoxMode
{
    DB_Filled,
    DB_NotFilled
};

    static void
TextDimensions(void)
{
    struct TextExtent textExt;

    TextExtent(gui.window->RPort, "s", 1, &textExt);

    characterWidth = textExt.te_Width;
    characterHeight = textExt.te_Height;
}

    static int
posWidthCharToPoint(int width)
{
    return (width)*characterWidth;
}

    static int
posHeightCharToPoint(int height)
{
    return (int)(height+1)*characterHeight;
}

    static int
posWidthPointToChar(int width)
{
    return (int)floor((float)width/(float)characterWidth)-1;
}

    static int
posHeightPointToChar(int height)
{
    return (int)floor((float)height/(float)characterHeight)-2;
}

    static int
widthCharToPoint(int width)
{
    return (width)*(characterWidth);
}

    static int
heightCharToPoint(int height)
{
    return (height)*characterHeight;
}

    static int
widthPointToChar(int width)
{
    return width/characterWidth+13;
}

    static int
heightPointToChar(int height)
{
    return height/characterHeight - 3;
}

    static void
refreshBorder(void)
{
    /*WaitBOVP(gui.window->);*/
    RefreshWindowFrame(gui.window);
}

    static void
drawBox(enum DrawBoxMode mode, unsigned short col, unsigned short row, guicolor_T color)
{
    /*
       SetDrMd(gui.window->RPort, COMPLEMENT);
       SetAPen(gui.window->RPort, -1);
       SetBPen(gui.window->RPort, -1);
       Move(gui.window->RPort, posWidthCharToPoint(col), posHeightCharToPoint(row));
       Text(gui.window->RPort, " ", 1);
       SetDrMd(gui.window->RPort, JAM2);
     */
}

    static enum event
EventHandler(void)
{
    struct IntuiMessage *msg;
    enum event		returnEvent;
    int			class, code;
    static int		dragging = 0;
    static int		mouseX, mouseY;
    char_u		string[40];

    msg = (struct IntuiMessage *)GetMsg(gui.window->UserPort);

    if (!msg)
    {
	returnEvent = ev_NullEvent;
    }
    else
    {

	class = msg->Class;
	code = msg->Code;

	switch(class)
	{
	    case IDCMP_INTUITICKS:
		/*
		   if (cursor.active)
		   {
		   cursor.current ++;
		   if (cursor.state == CursorOff)
		   {
		   printf("cursor turned on\n");
		   if (cursor.offTime < cursor.current)
		   {
		   gui_undraw_cursor();
		   cursor.state = CursorOn;
		   cursor.current = 0;
		   }
		   }
		   else if (cursor.state == CursorOn)
		   {
		   printf("cursor turned off\n");
		   if (cursor.onTime < cursor.current)
		   {
		   cursor.state = CursorOff;
		   gui_update_cursor(FALSE);
		   cursor.current = 0;
		   }
		   }
		   else if (cursor.state == CursorWait)
		   {
		   printf("cursor turned Wait\n");
		   if (cursor.waitTime < cursor.current)
		   {
		   cursor.state = CursorOn;
		   cursor.current = 0;
		   }
	}
	}
		   else
		   {
		   }
		   returnEvent = ev_IntuiTicks;
		   */
		       break;
	    case IDCMP_MOUSEBUTTONS:
		   {
		       int vim_modifiers=0;
		       D("Mouse button event detected");
		       switch (msg->Qualifier )
		       {
			   case IEQUALIFIER_LSHIFT:
			   case IEQUALIFIER_RSHIFT:
			       D("detected a shift key");
			       vim_modifiers|=MOUSE_SHIFT;
			       break;
			   case IEQUALIFIER_CONTROL:
			       D("detected a Control key");
			       vim_modifiers |= MOUSE_CTRL;
			       break;
		       }
		       if (code == SELECTDOWN)
		       {
			   D("Select Down detected\n");
			   dragging = 1;
			   gui_send_mouse_event(MOUSE_LEFT,
				   posWidthPointToChar(mouseX = msg->MouseX),
				   posHeightPointToChar(mouseY = msg->MouseY),
				   FALSE,
				   vim_modifiers);
			   /*gui_start_highlight(HL_ALL);*/
		       }
		       else if (code == SELECTUP)
		       {
			   D("Select UP detected\n");
			   dragging = 0;
			   gui_send_mouse_event(MOUSE_RELEASE,
				   posWidthPointToChar(msg->MouseX),
				   posHeightPointToChar(msg->MouseY),
				   FALSE, vim_modifiers);
			   /*gui_stop_highlight(mask);*/
		       }
		       returnEvent = ev_MouseButtons;
		       break;
		   }
	    case IDCMP_MOUSEMOVE:
		   if ((abs(mouseX-msg->MouseX) > characterWidth) || (abs(mouseY-msg->MouseY)>characterHeight))
		   {
		       mouseX = msg->MouseX;
		       mouseY = msg->MouseY;
		       if (!dragging)
		       {
			   gui_send_mouse_event(MOUSE_SETPOS, posWidthPointToChar(msg->MouseX), posHeightPointToChar(msg->MouseY), FALSE, 0);
			   break;
		       }
		       else
		       {
			   D("dragging\n");
			   gui_send_mouse_event(MOUSE_DRAG, posWidthPointToChar(msg->MouseX), posHeightPointToChar(msg->MouseY), FALSE, 0);
		       }
		   }
		   returnEvent = ev_MouseMove;
		   break;
	    case IDCMP_VANILLAKEY:
		   {
		       string[0] = (char_u)code;
		       if (code == CSI)
		       {
			   /* Insert CSI as K_CSI.  Untested! */
			   string[1] = KS_EXTRA;
			   string[2] = (int)KE_CSI;
			   add_to_input_buf(string, 3);
		       }
		       else
		       {
			   int	len = 1;

			   if (input_conv.vc_type != CONV_NONE)
			       len = convert_input(string, 1, sizeof(string));
			   add_to_input_buf(string, len);
		       }
		       returnEvent = ev_KeyStroke;
		       break;
		       case IDCMP_RAWKEY:
		       if (msg->Qualifier & IEQUALIFIER_LSHIFT)
		       {
		       }
		       else if (msg->Qualifier & IEQUALIFIER_RSHIFT)
		       {
		       }
		       else if (msg->Qualifier & IEQUALIFIER_CONTROL)
		       {
			   if (code == 33)
			   {
			       trash_input_buf();
			   }
		       }
		       else if (msg->Code == KEYUP)
		       {
			   string[0] = CSI;
			   string[1] = 'k';
			   string[2] = 'u';
			   add_to_input_buf(string, 3);
		       }
		       else if (msg->Code == KEYLEFT)
		       {
			   string[0] = CSI;
			   string[1] = 'k';
			   string[2] = 'l';
			   add_to_input_buf(string, 1);
		       }
		       else if (msg->Code == KEYRIGHT)
		       {
			   string[0] = CSI;
			   string[1] = 'k';
			   string[2] = 'r';
			   add_to_input_buf(string, 1);
		       }
		       else if (msg->Code == KEYDOWN)
		       {
			   string[0] = CSI;
			   string[1] = 'k';
			   string[2] = 'd';
			   add_to_input_buf(string, 1);
		       }
		       returnEvent = ev_KeyStroke;
		       break;
		   }
	    case IDCMP_MENUVERIFY:
		   returnEvent = ev_MenuVerify;
		   /* Menu verification requested */
		   switch (code)
		   {
		       case MENUWAITING:
			   /*
			    ** It's not for us, the user is accessing another
			    ** programs menu, this is a good time to do some
			    ** cleanup etc
			    */
			   break;
		       case MENUHOT:
			   /*
			    ** It is our menu that is going hot, we have kontrol
			    ** Menu action can be cancelled by
			    ** msg->Code = MENUCANCEL;
			    */
			   break;
		       default:
			   break;
		   }
		   break;
	    case IDCMP_MENUPICK:
		   returnEvent = ev_MenuPick;
		   {
		       /*
			** one of our menu's have been selected, let's find out which
			*/
		       union myMenuItemUnion *item;
		       int menuNumber;

		       menuNumber = code;

		       item = (union myMenuItemUnion *) ItemAddress(gui.menu, menuNumber);


		       if (item)
		       {
			   gui_menu_cb(item->myMenuItem.guiMenu);
		       }
		   }
		   break;
	    case IDCMP_CLOSEWINDOW:
		   {
		       gui_mch_exit(1);
		       break;
		   }
	    case IDCMP_NEWSIZE:
		   {
		       int cx, cy;
		       cx = widthPointToChar(gui.window->Width);
		       cy = heightPointToChar(gui.window->Height);

		       gui_resize_shell(cx, cy);

		       returnEvent = ev_NewSize;
		       break;
		   }
	    case IDCMP_REFRESHWINDOW:
		   refreshBorder();
		   returnEvent = ev_RefreshWindow;
		   break;
	    case IDCMP_GADGETDOWN:
		   returnEvent = ev_GadgetDown;
		   break;
	    case IDCMP_GADGETUP:
		   returnEvent = ev_GadgetUp;
		   break;
	    case IDCMP_MENUHELP:
		   returnEvent = ev_MenuHelp;
		   break;
	    case IDCMP_GADGETHELP:
		   returnEvent = ev_GadgetHelp;
		   break;
	    case IDCMP_INACTIVEWINDOW:
		   gui.in_focus = FALSE;
	    case IDCMP_ACTIVEWINDOW:
		   gui.in_focus = TRUE;
		   gui_update_cursor(TRUE, FALSE);
		   break;
	    default:
		   break;
	}
	ReplyMsg((struct Message*)msg);
    }

    return returnEvent;
    /* mouse positin gui.window->MoseY, gui.window->MouseX) */
}

    static int
checkEventHandler(void)
{
    enum event happened;

    do
    {
	happened = EventHandler() ;
    }
    while  (happened != ev_NullEvent);

    return OK;
}

    static int
charEventHandler(int wtime)
{
    enum event happened;
    int rc;

    do
    {
	Wait(1<<gui.window->UserPort->mp_SigBit);

	happened = EventHandler() ;
    }
    while ((happened != ev_IntuiTicks) && (happened != ev_KeyStroke) && (happened != ev_MenuPick) && (happened != ev_MouseMove) &&(happened != ev_MouseButtons) );

    if (happened == ev_KeyStroke || happened == ev_MenuPick)
	rc = OK;
    else
	rc = FAIL;

    return rc;
}


/*
 * add primary menu
 */
    void
gui_mch_add_menu_item(vimmenu_T *menu, int idx)
{
    union myMenuItemUnion *menuItemUnion = NULL;
    struct IntuiText *menutext = NULL;
    vimmenu_T *parent;

    assert(menu != NULL);
    assert(menu->parent != NULL);
    parent = menu->parent;

    /* Don't add menu separator */
    if (menu_is_separator(menu->name))
	return;

    if (parent->menuItemPtr == NULL)
	return;

    /* TODO: use menu->mnemonic and menu->actext */
    menutext = (struct IntuiText *) malloc(sizeof(struct IntuiText));

    SetAttrib(menutext, FrontPen, 3);
    SetAttrib(menutext, BackPen, 1);
    SetAttrib(menutext, DrawMode, COMPLEMENT);
    SetAttrib(menutext, LeftEdge, 0);
    SetAttrib(menutext, TopEdge, 0);
    SetAttrib(menutext, ITextFont, NULL);
    SetAttrib(menutext, NextText, NULL);

    menuItemUnion = malloc(sizeof(*menuItemUnion));

    SetAttrib(&menuItemUnion->menuItem, NextItem, parent->menuItemPtr);
    SetAttrib(&menuItemUnion->menuItem, LeftEdge, 0);
    SetAttrib(&menuItemUnion->menuItem, Width, characterWidth*strlen(menu->dname));
    SetAttrib(&menuItemUnion->menuItem, Height, characterHeight+2);
    SetAttrib(&menuItemUnion->menuItem, Flags, ITEMTEXT+ITEMENABLED+HIGHCOMP);
    SetAttrib(&menuItemUnion->menuItem, MutualExclude, 0);
    SetAttrib(&menuItemUnion->menuItem, ItemFill, (APTR)menutext);
    SetAttrib(&menuItemUnion->menuItem, SelectFill, NULL);
    SetAttrib(&menuItemUnion->menuItem, Command, NULL);
    SetAttrib(&menuItemUnion->menuItem, SubItem, NULL);
    SetAttrib(&menuItemUnion->menuItem, NextSelect, MENUNULL);

    menutext->IText = malloc(strlen(menu->dname) + 1);

    strcpy(menutext->IText, menu->dname);

    menuItemUnion->menuItem.NextItem = NULL;


    if (parent)
    {
	if (!parent->menuItemPtr)
	{
	    D("Adding first subElement");
	    SetAttrib(&menuItemUnion->menuItem, TopEdge, 0);
	    parent->menuPtr->FirstItem = &menuItemUnion->menuItem;
	    parent->menuItemPtr = &menuItemUnion->menuItem;
	}
	else
	{
	    struct MenuItem *tmpMenuItem;
	    tmpMenuItem = parent->menuItemPtr;
	    while (tmpMenuItem->NextItem)
	    {
		tmpMenuItem = tmpMenuItem->NextItem;
	    }
	    tmpMenuItem->NextItem = &menuItemUnion->menuItem;
	    SetAttrib(&menuItemUnion->menuItem, TopEdge, tmpMenuItem->TopEdge+tmpMenuItem->Height);
	}
    }
    menu->menuPtr= NULL;
    menu->menuItemPtr = &menuItemUnion->menuItem;
    menuItemUnion->myMenuItem.guiMenu = menu;
}


    static struct Menu *
getMenu(struct RastPort *rast, int left, STRPTR name)
{
    struct Menu *menu;
    struct TextExtent textExt;

    menu = malloc(sizeof(*menu));
    menu->NextMenu = NULL;
    menu->LeftEdge = left;

    TextExtent(rast, name, strlen(name), &textExt);

    menu->TopEdge = 0;
    menu->Width = textExt.te_Width;
    menu->Height = textExt.te_Height;
    menu->Flags = ITEMTEXT+HIGHCOMP+MENUENABLED;
    menu->MenuName = name;
    menu->FirstItem = NULL;

    return menu;
}

/*
 * add  1st level submenu item
 */
    void
gui_mch_add_menu(vimmenu_T *menu, int idx)
{
    struct Menu	*newMenu;
    int		pos = 0;

    if (!menu_is_menubar(menu->name))
	return;

    menu->menuPtr = newMenu = getMenu(gui.window->RPort, 0, menu->dname);
    menu->menuItemPtr = NULL;
    newMenu->NextMenu = NULL;

    if (!gui.menu)
    {
	D("Adding head menu");
	gui.menu = newMenu ;
    }
    else
    {
	struct Menu *tmpMenu;

	tmpMenu = gui.menu;
	while (tmpMenu->NextMenu)
	    tmpMenu = tmpMenu->NextMenu;
	tmpMenu->NextMenu = newMenu;
	pos = tmpMenu->LeftEdge +
	    TextLength(gui.window->RPort, tmpMenu->MenuName,
		    strlen(tmpMenu->MenuName));
	newMenu->LeftEdge = pos;
    }
}

    void
gui_mch_toggle_tearoffs(enable)
    int		enable;
{
    /* no tearoff menus */
}

    int
gui_mch_set_blinking(long wait, long on, long off)
{
    cursor.waitTime = wait/100;
    cursor.onTime = on/100;
    cursor.offTime = off/100;
    return OK;
}

    void
gui_mch_prepare(int *argc, char **argv)
{
    D("gui_mch_prepare");

    execBase = (struct ExecBase *)OpenLibrary("exec.library", NULL);
    gfxBase = (struct GFXBase *)OpenLibrary("graphics.library", NULL);
    layersBase = (struct LayersBase *)OpenLibrary("layers.library", NULL);

    if (!execBase)
    {
	D("Cannot open exec.library, aborting");
    }
    if (!gfxBase)
    {
	D("Cannot open graphics.library, aborting");
    }
    if (!layersBase)
    {
	D("Cannot open graphics.library, aborting");
    }
    D("gui_mch_prepare done ");
}

    void
atexitDoThis(void)
{
    gui_mch_exit(-1);
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced.
 * Return OK or FAIL.
 */
    int
gui_mch_init_check(void)
{
    if (execBase && gfxBase && layersBase)
	return OK;
    return FAIL;
}

    int
gui_mch_init(void)
{
    int returnCode = FAIL; /* assume failure*/

    gui.window = OpenWindowTagList(&vimNewWindow, tags);
    if (gui.window)
    {
	gui.in_use = TRUE;
	gui.in_focus=TRUE;
	SetDrMd(gui.window->RPort, JAM2);
	atexit(atexitDoThis);
	TextDimensions();
	returnCode = OK; /* we've had sucess */
	if (gui_win_x != -1 && gui_win_y != -1)
	    gui_mch_set_winpos(gui_win_x, gui_win_y)
    }
    gui.menu = NULL;

    return returnCode;
}

    void
gui_mch_new_colors(void)
{
    D("gui_mch_new_colors");
}

    int
gui_mch_open(void)
{
    D("gui_mch_open");

    return OK;
}

    void
gui_mch_exit(int returnCode)
{
    D("****gui_mch_exit");
    if (gui.window)
    {
	D("Closeing window ");
	CloseWindow(gui.window);
	CloseLibrary((struct Library*)execBase);
	CloseLibrary((struct Library*)gfxBase);
	gui.window = NULL;
	gui.in_use = FALSE;
	getout(1);
    }
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
gui_mch_set_shellsize(int width, int height,
	int min_width, int min_height, int base_width, int base_height)
{
    D("gui_mch_set_shellsize");

    ChangeWindowBox(gui.window, gui.window->TopEdge,
	    gui.window->LeftEdge, widthCharToPoint(width),
	    heightCharToPoint(height));
    checkEventHandler();
}

    void
gui_mch_get_screen_dimensions(int *screen_w, int *screen_h)
{
    *screen_w = widthPointToChar(gui.window->Width);
    *screen_h = heightPointToChar(gui.window->Height);
}

    void
gui_mch_set_text_area_pos(int x, int y, int w, int h)
{
    D("gui_mch_set_text_area_pos");
}

    void
gui_mch_enable_scrollbar(scrollbar_T *sb, int flag)
{
    /* done by default */
    /* TODO: disable scrollbar when it's too small */
}

    void
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, long val, long size, long max)
{
}

    void
gui_mch_set_scrollbar_pos(scrollbar_T *sb, int x, int y, int w, int h)
{
    D("gui_mch_set_scrollbar_pos");
    /*NewModifyProp(&propGadget, gui.window, NULL, MAXPOT, MAXPOT/sb->max*y, MAXPOT, MAXBODY/sb->max/sb->size, 1);*/
}

    void
gui_mch_create_scrollbar(scrollbar_T *sb, int orient)
{
    /* this is done by default */
}

#if defined(FEAT_WINDOWS) || defined(PROTO)
    void
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    /* this is done by default */
}
#endif

int gui_mch_init_font(char_u *font_name, int fontset)
{
    /*D("gui_mch_init_font");*/
    return OK;
}

    int
gui_mch_adjust_charsize()
{
    return FAIL;
}

    GuiFont
gui_mch_get_font( char_u *name, int giveErrorIfMissing)
{
    /*D("gui_mch_get_font");*/
    return NULL;
}

    void
gui_mch_set_font(GuiFont font)
{
    /*D("gui_mch_set_font");*/
}

#if 0 /* not used */
    int
gui_mch_same_font(GuiFont f1, GuiFont f2)
{
    D("gui_mch_same_font");
}
#endif

    void
gui_mch_free_font(GuiFont font)
{
    if (font)
	D("gui_mch_free_font");
}

#define RGB(a, b, c) ((a && 0xff) * 0x10000 + (b * 0xff) * 0x100 + (c & 0xff))

/*
 * Get color handle for color "name".
 * Return INVALCOLOR when not possible.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    typedef struct guicolor_tTable
    {
	char	    *name;
	unsigned long    color;
    } guicolor_tTable;

    static guicolor_tTable table[] =
    {
	{"Grey",	0},
	{"Black",	1},
	{"DarkBlue",	2},
	{"DarkGreen",	3},
	{"DarkCyan",	4},
	{"DarkRed",	5},
	{"DarkMagenta",	6},
	{"Brown",	7},
	{"Gray",	8},
	{"Grey",	9},
	{"LightGray",	10},
	{"LightGrey",	11},
	{"DarkGray",	12},
	{"DarkGrey",	13},
	{"Blue",	14},
	{"LightBlue",	15},
	{"Green",	16},
	{"LightGreen",	17},
	{"Cyan",	18},
	{"LightCyan",	19},
	{"Red",		20},
	{"LightRed",	21},
	{"Magenta",	22},
	{"LightMagenta",23},
	{"Yellow",	24},
	{"LightYellow",	25},	/* TODO: add DarkYellow */
	{"White",	26},
	{"SeaGreen",	27},
	{"Orange",	28},
	{"Purple",	30},
	{"SlateBlue",	31},
	{"grey90",	32},
	{"grey95",	33},
	{"grey80",	34},
	{NULL, NULL},
    };

    guicolor_T color = INVALCOLOR;

    int i;

    for (i = 0; table[i].name != NULL;i++)
    {
	if (stricmp(name, table[i].name) == 0)
	    color = table[i].color;
    }

    if (color == INVALCOLOR)
    {
	char *looky = NULL;

	color = strtol((char*)name, &looky, 10);
	if (*looky != NUL)
	    color = INVALCOLOR;
    }

    return color;
}

    void
gui_mch_set_colors(guicolor_T fg, guicolor_T bg)
{
    if (fg == 0)
    {
	fg = 1;
    }
    SetABPenDrMd(gui.window->RPort, fg, bg, JAM2);
}

    void
gui_mch_set_fg_color(guicolor_T color)
{
    if (color == 0)
    {
	color = 1; /* vim sends 0 as default color which is ALWAYS the
		      background on the amiga scrolling with colours as the
		      background is a very bad idea on slow machines*/
    }
    SetAPen(gui.window->RPort, color);
    SetDrMd(gui.window->RPort, JAM2);
}

    void
gui_mch_set_bg_color(guicolor_T color)
{
    SetBPen(gui.window->RPort, color);
}

    void
gui_mch_draw_string(int row, int col, char_u *s, int len, int flags)
{
    switch(flags)
    {
	case 0:
	    Move(gui.window->RPort, posWidthCharToPoint(col), posHeightCharToPoint(row));
	    Text(gui.window->RPort, s, len);
	    break;
	case DRAW_TRANSP:
	    SetDrMd(gui.window->RPort, INVERSVID);
	    Move(gui.window->RPort, posWidthCharToPoint(col), posHeightCharToPoint(row));
	    Text(gui.window->RPort, s, len);
	    break;
	case DRAW_BOLD:
	    Move(gui.window->RPort, posWidthCharToPoint(col), posHeightCharToPoint(row));
	    Text(gui.window->RPort, s, len);
	    SetDrMd(gui.window->RPort, JAM1);
	    Move(gui.window->RPort, posWidthCharToPoint(col), posHeightCharToPoint(row));
	    Draw(gui.window->RPort, posWidthCharToPoint(col+len), posHeightCharToPoint(row));
	    SetDrMd(gui.window->RPort, JAM2);
	    break;
	case DRAW_UNDERL:
	    Move(gui.window->RPort, posWidthCharToPoint(col), posHeightCharToPoint(row));
	    Text(gui.window->RPort, s, len);
	    Move(gui.window->RPort, posWidthCharToPoint(col)+1, posHeightCharToPoint(row));
	    Text(gui.window->RPort, s, len);
	    break;
    }
}

    int
gui_mch_haskey(char_u *name)
{
    int i;

    D("gui_mch_haskey");

    for (i = 0; special_keys[i].vim_code1 != NUL; i++)
	if (name[0] == special_keys[i].vim_code0 &&
		name[1] == special_keys[i].vim_code1)
	    return OK;
    return FAIL;
}

    void
gui_mch_beep(void)
{
    D("gui_mch_beep");
}

    void
gui_mch_flash(int msec)
{
    D("gui_mch_flash");
}

    void
gui_mch_invert_rectangle( int r, int c, int nr, int nc)
{
    printf("gui_mch_invert_rectangle %d %d %d %d\n", r, c, nr, nc);
}

    void
gui_mch_iconify(void)
{
    D("gui_mch_iconify");
}

#if defined(FEAT_EVAL) || defined(PROTO)
/*
 * Bring the Vim window to the foreground.
 */
    void
gui_mch_set_foreground()
{
    D("gui_mch_set_foreground");
}
#endif

    void
gui_mch_settitle(char_u  *title, char_u  *icon)
{
    D("gui_mch_settitle");
}

    void
gui_mch_stop_blink(void)
{
    gui_undraw_cursor();
    D("gui_mch_stop_blink");
}

    void
gui_mch_start_blink(void)
{
    gui_update_cursor(FALSE, FALSE);
    D("gui_mch_start_blink");
}

    void
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    drawBox(DB_NotFilled, gui.col, gui.row, color);
}

    void
gui_mch_draw_part_cursor( int col, int row, guicolor_T color)
{
    D("gui_mch_part_cursor");
    drawBox(DB_Filled, col, row, color);
}

    void
gui_mch_update(void)
{
    checkEventHandler();
    return ;
}

    int
gui_mch_wait_for_chars(int wtime)
{
    assert(wtime != 0);
    return charEventHandler(wtime);
}

    void
gui_mch_flush(void)
{
}

    void
gui_mch_clear_block(int row1, int col1, int row2, int col2)
{
    register int start;

    /* TODO: this isn't using "col2"! */
    for (start = row1; start < row2; start ++)
    {
	Move(gui.window->RPort, 0, posHeightCharToPoint(start));
	gui_mch_set_fg_color(0);
	gui_mch_set_bg_color(0);
	ClearEOL(gui.window->RPort);
    }
}

    void
gui_mch_clear_all(void)
{
    SetRast(gui.window->RPort, 0);
    refreshBorder();
    D("gui_mch_clear_all");
}

    void
gui_mch_delete_lines(int row, int num_lines)
{
    gui_clear_block(row, 0, row + num_lines, Columns - 1);
    /* changed without checking! */
    ScrollRaster(gui.window->RPort,
	    posWidthCharToPoint(gui.scroll_region_left),
	    characterHeight * num_lines,
	    posWidthCharToPoint(gui.scroll_region_left),
	    posHeightCharToPoint(row - 1) + 2,
	    posWidthCharToPoint(gui.scroll_region_right + 1),
	    posHeightCharToPoint(gui.scroll_region_bot) + 3);
}

    void
gui_mch_insert_lines(int row, int num_lines)
{
    SetABPenDrMd(gui.window->RPort, 0, 0, JAM2);
    /* changed without checking! */
    ScrollRaster(gui.window->RPort,
	    posWidthCharToPoint(gui.scroll_region_left),
	    -characterHeight*num_lines,
	    posWidthCharToPoint(gui.scroll_region_left),
	    posHeightCharToPoint(row-1)+2,
	    posWidthCharToPoint(gui.scroll_region_right + 1),
	    posHeightCharToPoint(gui.scroll_region_bot-num_lines+1)+1);

    gui_clear_block(row, gui.scroll_region_left,
				    row + num_lines, gui.scroll_region_right);
}

    void
gui_mch_enable_menu(int flag)
{
    D("gui_mch_enable_menu");
}

    void
gui_mch_set_menu_pos(int x, int y, int w, int h)
{
    D("gui_mch_set_menu_pos");
}

    void
gui_mch_destroy_menu(vimmenu_T *menu)
{
    D("gui_mch_destroy_menu");
    ClearMenuStrip(gui.window);
}

    void
gui_mch_menu_grey(vimmenu_T *menu, int grey)
{
    D("gui_mch_menu_grey");
}

    void
gui_mch_menu_hidden(vimmenu_T *menu, int hidden)
{
    D("gui_mch_menu_hidden");
    ClearMenuStrip(gui.window);
}

    void
gui_mch_draw_menubar(void)
{
    D("gui_mch_draw_menubar");
    SetMenuStrip(gui.window, gui.menu);
}

    static void
AmigaError(const char *string)
{
    static struct IntuiText pos = { 3, 0, JAM2, 17, 5, NULL, "Cancel", NULL} ;
    static struct IntuiText neg = { 3, 0, JAM2, 17, 5, NULL, "Cancel", NULL} ;
    static struct IntuiText message = { 3, 0, JAM2, 17, 5, NULL, NULL, NULL} ;
    static char *strptr = 0;

    if (strptr)
	free(strptr);
    strptr = malloc(strlen(string)+1);

    message.IText = strptr;
    strcpy(strptr, string);

    AutoRequest(NULL, &message, &pos, &neg, 0, 0, 300, 300);
}

    int
clip_mch_own_selection(VimClipboard *cbd)
{
    D("clib_mch_own_selection");
    return OK;
}

    void
mch_setmouse(int  on)
{
}

/*
 * Get current y mouse coordinate in text window.
 * Return -1 when unknown.
 */
    int
gui_mch_get_mouse_x()
{
    /* TODO */
    return -1;
}

    int
gui_mch_get_mouse_y()
{
    /* TODO */
    return -1;
}

    void
gui_mch_setmouse(x, y)
    int		x;
    int		y;
{
    /* TODO */
}

    void
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    /* TODO */
}

    void
clip_mch_lose_selection(VimClipboard *cbd)
{
    D("clip_mch_lose_selecction");
}

    void
clip_mch_request_selection(VimClipboard *cbd)
{
    D("clip_mch_requst_selection");
}

    void
clip_mch_set_selection(VimClipboard *cbd)
{
}

    long_u
gui_mch_get_rgb(guicolor_T pixel)
{
    unsigned long  color;

    color = GetRGB4(gui.window->WScreen->ViewPort.ColorMap, pixel);
    return ((color & 0xf00) << 12) + ((color & 0x0f0) << 8)
						     + ((color & 0x00f) << 4);
}

#endif /* USE_AMIGA_GUI*/
