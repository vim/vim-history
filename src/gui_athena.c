/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *				Athena port by Bill Foster
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/Box.h>

#include "vim.h"
#include "gui_at_sb.h"

#define puller_width	19
#define puller_height	19

static char_u puller_bits[] =
{
    0x00,0x00,0xf8,0x00,0x00,0xf8,0xf8,0x7f,0xf8,0x04,0x80,0xf8,0x04,0x80,0xf9,
    0x84,0x81,0xf9,0x84,0x83,0xf9,0x84,0x87,0xf9,0x84,0x8f,0xf9,0x84,0x8f,0xf9,
    0x84,0x87,0xf9,0x84,0x83,0xf9,0x84,0x81,0xf9,0x04,0x80,0xf9,0x04,0x80,0xf9,
    0xf8,0xff,0xf9,0xf0,0x7f,0xf8,0x00,0x00,0xf8,0x00,0x00,0xf8
};

extern Widget vimShell;

static Widget vimForm = (Widget)NULL;
static Widget textArea;
static Widget menuBar;

static void gui_athena_scroll_cb_jump	__ARGS((Widget, XtPointer, XtPointer));
static void gui_athena_scroll_cb_scroll __ARGS((Widget, XtPointer, XtPointer));
static void gui_mch_submenu_colors __ARGS((GuiMenu *mp));
static void gui_athena_reorder_menus	__ARGS((void));
static void gui_athena_pullright_action __ARGS((Widget, XEvent *, String *,
						Cardinal *));
static void gui_athena_pullleft_action __ARGS((Widget, XEvent *, String *,
						Cardinal *));
static Widget get_popup_entry __ARGS((Widget w));

static XtActionsRec	pullAction[2] = {{ "menu-pullright",
				(XtActionProc)gui_athena_pullright_action},
					 { "menu-pullleft",
				(XtActionProc)gui_athena_pullleft_action}};
static XtTranslations	parentTrans, menuTrans, supermenuTrans;
static Pixmap		pullerBitmap;

/*
 * Scrollbar callback (XtNjumpProc) for when the scrollbar is dragged with the
 * left or middle mouse button.
 */
/* ARGSUSED */
    static void
gui_athena_scroll_cb_jump(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data, call_data;
{
    GuiScrollbar *sb, *sb_info;
    long	value;

    sb = gui_find_scrollbar((long)client_data);

    if (sb == NULL)
	return;
    else if (sb->wp != NULL)	    /* Left or right scrollbar */
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];
    }
    else	    /* Bottom scrollbar */
	sb_info = sb;

    value = (long)(*((float *)call_data) * (float)(sb_info->max + 1) + 0.001);
    if (value > sb_info->max)
	value = sb_info->max;

    gui_drag_scrollbar(sb, value, TRUE);
}

/*
 * Scrollbar callback (XtNscrollProc) for paging up or down with the left or
 * right mouse buttons.
 */
/* ARGSUSED */
    static void
gui_athena_scroll_cb_scroll(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data, call_data;
{
    GuiScrollbar *sb, *sb_info;
    long	value;
    int		data = (int)(long)call_data;
    int		page;

    sb = gui_find_scrollbar((long)client_data);

    if (sb == NULL)
	return;
    else if (sb->wp != NULL)	    /* Left or right scrollbar */
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];

	if (sb_info->size > 5)
	    page = sb_info->size - 2;	    /* use two lines of context */
	else
	    page = sb_info->size;
	switch (data)
	{
	    case  ONE_LINE_DATA: data = 1; break;
	    case -ONE_LINE_DATA: data = -1; break;
	    case  ONE_PAGE_DATA: data = page; break;
	    case -ONE_PAGE_DATA: data = -page; break;
	    case  END_PAGE_DATA: data = sb_info->max; break;
	    case -END_PAGE_DATA: data = -sb_info->max; break;
			default: data = 0; break;
	}
    }
    else	    /* Bottom scrollbar */
    {
	sb_info = sb;
	if (data < -1)
	    data = -(Columns - 5);
	else if (data > 1)
	    data = (Columns - 5);
    }

    value = sb_info->value + data;
    if (value > sb_info->max)
	value = sb_info->max;
    else if (value < 0)
	value = 0;

    gui_drag_scrollbar(sb, value, FALSE);
}

/*
 * Create all the Athena widgets necessary.
 */
    void
gui_x11_create_widgets()
{
    /*
     * We don't have any borders handled internally by the textArea to worry
     * about so only skip over the configured border width.
     */
    gui.border_offset = gui.border_width;

    XtInitializeWidgetClass(formWidgetClass);
    XtInitializeWidgetClass(boxWidgetClass);
    XtInitializeWidgetClass(coreWidgetClass);
    XtInitializeWidgetClass(menuButtonWidgetClass);
    XtInitializeWidgetClass(simpleMenuWidgetClass);
    XtInitializeWidgetClass(vim_scrollbarWidgetClass);

    /* The form containing all the other widgets */
    vimForm = XtVaCreateManagedWidget("vimForm",
	formWidgetClass,	vimShell,
	XtNborderWidth,		0,
	XtNforeground,		gui.menu_fg_pixel,
	XtNbackground,		gui.menu_bg_pixel,
	NULL);

    /* The top menu bar */
    menuBar = XtVaCreateManagedWidget("menuBar",
	boxWidgetClass,		vimForm,
	XtNresizable,		True,
	XtNtop,			XtChainTop,
	XtNbottom,		XtChainTop,
	XtNleft,		XtChainLeft,
	XtNright,		XtChainRight,
	XtNforeground,		gui.menu_fg_pixel,
	XtNbackground,		gui.menu_bg_pixel,
	XtNborderColor,		gui.menu_fg_pixel,
	NULL);

    /* The text area. */
    textArea = XtVaCreateManagedWidget("textArea",
	coreWidgetClass,	vimForm,
	XtNresizable,		True,
	XtNtop,			XtChainTop,
	XtNbottom,		XtChainTop,
	XtNleft,		XtChainLeft,
	XtNright,		XtChainLeft,
	XtNbackground,		gui.back_pixel,
	XtNborderWidth,		0,
	NULL);

    /*
     * Install the callbacks.
     */
    gui_x11_callbacks(textArea, vimForm);

    parentTrans = XtParseTranslationTable("<EnterWindow>: highlight()\n<LeaveWindow>:\n<BtnUp>: notify() unhighlight() MenuPopdown()\n<BtnMotion>: highlight() menu-pullright()");
    menuTrans = XtParseTranslationTable("<EnterWindow>: highlight()\n<LeaveWindow>: unhighlight() MenuPopdown()\n<BtnUp>: notify() unhighlight() MenuPopdown()\n<BtnMotion>: highlight() menu-pullright()");
    supermenuTrans = XtParseTranslationTable("<EnterWindow>: highlight() menu-pullleft()\n<LeaveWindow>:\n<BtnUp>: notify() unhighlight() MenuPopdown()\n<BtnMotion>:");

    XtAppAddActions(XtWidgetToApplicationContext(vimForm), pullAction, 2);

    pullerBitmap = XCreateBitmapFromData(gui.dpy, DefaultRootWindow(gui.dpy),
			    (char *)puller_bits, puller_width, puller_height);
}

    void
gui_mch_set_text_area_pos(x, y, w, h)
    int	    x;
    int	    y;
    int	    w;
    int	    h;
{
    XtUnmanageChild(textArea);
    XtVaSetValues(textArea,
		  XtNhorizDistance, x,
		  XtNvertDistance, y,
		  XtNwidth, w,
		  XtNheight, h,
		  NULL);
    XtManageChild(textArea);
}

    void
gui_x11_set_back_color()
{
    if (textArea != NULL)
	XtVaSetValues(textArea,
		  XtNbackground, gui.back_pixel,
		  NULL);
}

/*
 * Menu stuff.
 */

    void
gui_mch_enable_menu(flag)
    int	    flag;
{
    if (flag)
	XtManageChild(menuBar);
    else
	XtUnmanageChild(menuBar);
}

    void
gui_mch_set_menu_pos(x, y, w, h)
    int	    x;
    int	    y;
    int	    w;
    int	    h;
{
    Dimension	border;
    int		height;

    XtUnmanageChild(menuBar);
    XtVaGetValues(menuBar,
		XtNborderWidth, &border,
		NULL);
    /* avoid trouble when there are no menu items, and h is 1 */
    height = h - 2 * border;
    if (height < 0)
	height = 1;
    XtVaSetValues(menuBar,
		XtNhorizDistance, x,
		XtNvertDistance, y,
		XtNwidth, w - 2 * border,
		XtNheight, height,
		NULL);
    XtManageChild(menuBar);
}

    void
gui_mch_add_menu(menu, parent)
    GuiMenu *menu;
    GuiMenu *parent;
{
    char_u	*pullright_name;
    Dimension	height, space, border;

    if (parent == NULL)
    {
	menu->id = XtVaCreateManagedWidget((char *)menu->name,
	    menuButtonWidgetClass, menuBar,
	    XtNmenuName, menu->name,
	    XtNforeground, gui.menu_fg_pixel,
	    XtNbackground, gui.menu_bg_pixel,
	    NULL);

	menu->submenu_id = XtVaCreatePopupShell((char *)menu->name,
	    simpleMenuWidgetClass, menu->id,
	    XtNforeground, gui.menu_fg_pixel,
	    XtNbackground, gui.menu_bg_pixel,
	    NULL);

	/* Don't update the menu height when it was set at a fixed value */
	if (!gui.menu_height_fixed)
	{
	    /*
	     * When we add a top-level item to the menu bar, we can figure out
	     * how high the menu bar should be.
	     */
	    XtVaGetValues(menuBar,
		    XtNvSpace,	&space,
		    XtNborderWidth, &border,
		    NULL);
	    XtVaGetValues(menu->id,
		    XtNheight,	&height,
		    NULL);
	    gui.menu_height = height + 2 * (space + border);
	}

	gui_athena_reorder_menus();
    }
    else
    {
	menu->id = XtVaCreateManagedWidget((char *)menu->name,
	    smeBSBObjectClass, parent->submenu_id,
	    XtNforeground, gui.menu_fg_pixel,
	    XtNbackground, gui.menu_bg_pixel,
	    XtNrightMargin, puller_width,
	    XtNrightBitmap, pullerBitmap,
	    NULL);
	XtAddCallback(menu->id, XtNcallback, gui_x11_menu_cb,
	    (XtPointer)menu);

	pullright_name = vim_strnsave(menu->name,
				   STRLEN(menu->name) + strlen("-pullright"));
	strcat((char *)pullright_name, "-pullright");
	menu->submenu_id = XtVaCreatePopupShell((char *)pullright_name,
	    simpleMenuWidgetClass, parent->submenu_id,
	    XtNforeground, gui.menu_fg_pixel,
	    XtNbackground, gui.menu_bg_pixel,
	    XtNtranslations, menuTrans,
	    NULL);
	vim_free(pullright_name);

	XtOverrideTranslations(parent->submenu_id, parentTrans);
    }
}

    void
gui_mch_add_menu_item(menu, parent)
    GuiMenu *menu;
    GuiMenu *parent;
{
    menu->submenu_id = (Widget)0;
    menu->id = XtVaCreateManagedWidget((char *)menu->name,
	smeBSBObjectClass, parent->submenu_id,
	XtNforeground, gui.menu_fg_pixel,
	XtNbackground, gui.menu_bg_pixel,
	NULL);
    XtAddCallback(menu->id, XtNcallback, gui_x11_menu_cb,
	(XtPointer)menu);
}

    void
gui_mch_new_menu_colors()
{
    if (menuBar == NULL)
	return;
    XtVaSetValues(menuBar,
	XtNforeground, gui.menu_fg_pixel,
	XtNbackground, gui.menu_bg_pixel,
	XtNborderColor,	gui.menu_fg_pixel,
	NULL);

    gui_mch_submenu_colors(gui.root_menu);
}

    static void
gui_mch_submenu_colors(mp)
    GuiMenu	*mp;
{
    while (mp != NULL)
    {
	XtVaSetValues(mp->id,
		XtNforeground, gui.menu_fg_pixel,
		XtNbackground, gui.menu_bg_pixel,
		NULL);
	if (mp->submenu_id != (Widget)0)
	    XtVaSetValues(mp->submenu_id,
		    XtNforeground, gui.menu_fg_pixel,
		    XtNbackground, gui.menu_bg_pixel,
		    NULL);

	    /* Set the colors for the children */
	if (mp->children != NULL)
	    gui_mch_submenu_colors(mp->children);
	mp = mp->next;
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    void
gui_mch_destroy_menu(menu)
    GuiMenu *menu;
{
    if (menu->submenu_id != (Widget)0)
    {
	XtDestroyWidget(menu->submenu_id);
	menu->submenu_id = (Widget)0;
    }
    if (menu->id != (Widget)0)
    {
#if 0
	Widget	parent;

	/*
	 * This is a hack to stop the Athena simpleMenuWidget from getting a
	 * BadValue error when a menu's last child is destroyed. We check to
	 * see if this is the last child and if so, don't delete it. The parent
	 * will be deleted soon anyway, and it will delete it's children like
	 * all good widgets do.
	 */
	parent = XtParent(menu->id);
	if (parent != menuBar)
	{
	    int num_children;

	    XtVaGetValues(parent, XtNnumChildren, &num_children, NULL);
	    if (num_children > 1)
		XtDestroyWidget(menu->id);
	}
	else
	    XtDestroyWidget(menu->id);
#else
	/*
	 * The code above causes a crash.  Apparantly because the highlighting
	 * is still there, and removing it later causes the crash.
	 * This fix just unmanages the menu item, without destroying it.  The
	 * only problem is that the highlighting will be wrong.
	 */
	XtUnmanageChild(menu->id);
#endif
	menu->id = (Widget)0;
    }
}

/*
 * Reorder the menus to make them appear in the right order.
 */
    static void
gui_athena_reorder_menus()
{
    Widget  *children;
    Widget  swap_widget;
    int	    num_children;
    int	    to, from;
    GuiMenu *menu;

    XtVaGetValues(menuBar,
	    XtNchildren,    &children,
	    XtNnumChildren, &num_children,
	    NULL);

    XtUnmanageChildren(children, num_children);

    menu = gui.root_menu;
    for (to = 0; to < num_children - 1; ++to)
    {
	for (from = to; from < num_children; ++from)
	{
	    if (strcmp((char *)XtName(children[from]), (char *)menu->name) == 0)
	    {
		if (to != from)		/* need to move this one */
		{
		    swap_widget = children[to];
		    children[to] = children[from];
		    children[from] = swap_widget;
		}
		break;
	    }
	}
	menu = menu->next;
	if (menu == NULL)	/* cannot happen */
	    break;
    }

    XtManageChildren(children, num_children);
}


/*
 * Scrollbar stuff.
 */

    void
gui_mch_set_scrollbar_thumb(sb, val, size, max)
    GuiScrollbar    *sb;
    int		    val;
    int		    size;
    int		    max;
{
    double	    v, s;

    /*
     * Athena scrollbar must go from 0.0 to 1.0.
     */
    if (max == 0)
    {
	/* So you can't scroll it at all (normally it scrolls past end) */
	vim_XawScrollbarSetThumb(sb->id, 0.0, 1.0, 0.0);
    }
    else
    {
	v = (double)val / (double)(max + 1);
	s = (double)size / (double)(max + 1);
	vim_XawScrollbarSetThumb(sb->id, v, s, 1.0);
    }
}

    void
gui_mch_set_scrollbar_pos(sb, x, y, w, h)
    GuiScrollbar    *sb;
    int		    x;
    int		    y;
    int		    w;
    int		    h;
{
    XtUnmanageChild(sb->id);
    XtVaSetValues(sb->id,
		  XtNhorizDistance, x,
		  XtNvertDistance, y,
		  XtNwidth, w,
		  XtNheight, h,
		  NULL);
    XtManageChild(sb->id);
}

    void
gui_mch_create_scrollbar(sb, orient)
    GuiScrollbar    *sb;
    int		    orient;	/* SBAR_VERT or SBAR_HORIZ */
{
    sb->id = XtVaCreateWidget("scrollBar",
	    vim_scrollbarWidgetClass, vimForm,
	    XtNresizable,   True,
	    XtNtop,	    XtChainTop,
	    XtNbottom,	    XtChainTop,
	    XtNleft,	    XtChainLeft,
	    XtNright,	    XtChainLeft,
	    XtNborderWidth, 0,
	    XtNorientation, (orient == SBAR_VERT) ? XtorientVertical
						  : XtorientHorizontal,
	    XtNforeground, gui.scroll_fg_pixel,
	    XtNbackground, gui.scroll_bg_pixel,
	    NULL);
    XtAddCallback(sb->id, XtNjumpProc,
		  gui_athena_scroll_cb_jump, (XtPointer)sb->ident);
    XtAddCallback(sb->id, XtNscrollProc,
		  gui_athena_scroll_cb_scroll, (XtPointer)sb->ident);

    vim_XawScrollbarSetThumb(sb->id, 0.0, 1.0, 0.0);
}

    void
gui_mch_destroy_scrollbar(sb)
    GuiScrollbar    *sb;
{
    XtDestroyWidget(sb->id);
}

    void
gui_mch_set_scrollbar_colors(sb)
    GuiScrollbar    *sb;
{
    if (sb->id != NULL)
	XtVaSetValues(sb->id,
	    XtNforeground, gui.scroll_fg_pixel,
	    XtNbackground, gui.scroll_bg_pixel,
	    NULL);
}

/*
 * Miscellaneous stuff:
 */
    Window
gui_x11_get_wid()
{
    return( XtWindow(textArea) );
}

/* ARGSUSED */
    static void
gui_athena_pullright_action(w, event, args, nargs)
    Widget	w;
    XEvent	*event;
    String	*args;
    Cardinal	*nargs;
{
    Dimension	width, height;
    Widget	popup;

    if (event->type != MotionNotify)
	return;

    XtVaGetValues(w,
	XtNwidth,   &width,
	XtNheight,  &height,
	NULL);

    if (event->xmotion.x >= (int)width || event->xmotion.y >= (int)height)
	return;

    /* We do the pull-off when the pointer is in the rightmost 1/4th */
    if (event->xmotion.x < (int)(width * 3) / 4)
	return;

    popup = get_popup_entry(w);
    if (popup == (Widget)NULL)
	return;

    /* Don't Popdown the previous submenu now */
    XtOverrideTranslations(w, supermenuTrans);

    XtVaSetValues(popup,
	XtNx, event->xmotion.x_root,
	XtNy, event->xmotion.y_root - 7,
	NULL);

    XtOverrideTranslations(popup, menuTrans);

    XtPopup(popup, XtGrabNonexclusive);
}

/*
 * Called when a submenu with another submenu gets focus again.
 */
/* ARGSUSED */
    static void
gui_athena_pullleft_action(w, event, args, nargs)
    Widget	w;
    XEvent	*event;
    String	*args;
    Cardinal	*nargs;
{
    Widget	popup;
    Widget	parent;

    if (event->type != EnterNotify)
	return;

    /* Do Popdown the submenu now */
    popup = get_popup_entry(w);
    if (popup != (Widget)NULL)
	XtPopdown(popup);

    /* If this is the toplevel menu item, set parentTrans */
    if ((parent = XtParent(w)) != (Widget)NULL && XtParent(parent) == menuBar)
	XtOverrideTranslations(w, parentTrans);
    else
	XtOverrideTranslations(w, menuTrans);
}

    static Widget
get_popup_entry(w)
    Widget  w;
{
    Widget	menuw;
    char_u	*pullright_name;
    Widget	popup;

    /* Get the active entry for the current menu */
    if ((menuw = XawSimpleMenuGetActiveEntry(w)) == (Widget)NULL)
	return NULL;

    pullright_name = vim_strnsave((char_u *)XtName(menuw),
				strlen(XtName(menuw)) + strlen("-pullright"));
    strcat((char *)pullright_name, "-pullright");
    popup = XtNameToWidget(w, (char *)pullright_name);
    vim_free(pullright_name);

    return popup;
}
