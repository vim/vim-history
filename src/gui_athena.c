/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *				Athena port by Bill Foster
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/AsciiText.h>

#include "vim.h"
#include "gui_at_sb.h"

#define puller_width	19
#define puller_height	19

extern Widget vimShell;

static Widget vimForm = (Widget)0;
static Widget textArea = (Widget)0;
#ifdef FEAT_MENU
static Widget menuBar = (Widget)0;
static void gui_athena_pullright_action __ARGS((Widget, XEvent *, String *,
						Cardinal *));
static void gui_athena_pullleft_action __ARGS((Widget, XEvent *, String *,
						Cardinal *));
static XtActionsRec	pullAction[2] = {{ "menu-pullright",
				(XtActionProc)gui_athena_pullright_action},
					 { "menu-pullleft",
				(XtActionProc)gui_athena_pullleft_action}};
#endif
#ifdef FEAT_TOOLBAR
static Widget toolBar = (Widget)0;
#endif

static void gui_athena_scroll_cb_jump	__ARGS((Widget, XtPointer, XtPointer));
static void gui_athena_scroll_cb_scroll __ARGS((Widget, XtPointer, XtPointer));
#if defined(FEAT_GUI_DIALOG) || defined(FEAT_MENU)
static void gui_athena_menu_colors __ARGS((Widget id));
#endif
static void gui_athena_scroll_colors __ARGS((Widget id));

#ifdef FEAT_MENU
static XtTranslations	popupTrans, parentTrans, menuTrans, supermenuTrans;
static Pixmap		pullerBitmap;
static char_u puller_bits[] =
{
    0x00,0x00,0xf8,0x00,0x00,0xf8,0xf8,0x7f,0xf8,0x04,0x80,0xf8,0x04,0x80,0xf9,
    0x84,0x81,0xf9,0x84,0x83,0xf9,0x84,0x87,0xf9,0x84,0x8f,0xf9,0x84,0x8f,0xf9,
    0x84,0x87,0xf9,0x84,0x83,0xf9,0x84,0x81,0xf9,0x04,0x80,0xf9,0x04,0x80,0xf9,
    0xf8,0xff,0xf9,0xf0,0x7f,0xf8,0x00,0x00,0xf8,0x00,0x00,0xf8
};
#endif

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
    scrollbar_T *sb, *sb_info;
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
    scrollbar_T *sb, *sb_info;
    long	value;
    int		data = (int)(long)call_data;
    int		page;

    sb = gui_find_scrollbar((long)client_data);

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
    else			/* Bottom scrollbar */
    {
	sb_info = sb;
	if (data < -1)		/* page-width left */
	    data = -(W_WIDTH(sb->wp) - 5);
	else if (data > 1)	/* page-width right */
	    data = (W_WIDTH(sb->wp) - 5);
    }

    value = sb_info->value + data;
    if (value > sb_info->max)
	value = sb_info->max;
    else if (value < 0)
	value = 0;

    /* Update the bottom scrollbar an extra time (why is this needed?? */
    if (sb->wp == NULL)		/* Bottom scrollbar */
	gui_mch_set_scrollbar_thumb(sb, value, sb->size, sb->max);

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

#if 0 /* not needed? */
    XtInitializeWidgetClass(formWidgetClass);
    XtInitializeWidgetClass(boxWidgetClass);
    XtInitializeWidgetClass(coreWidgetClass);
#ifdef FEAT_MENU
    XtInitializeWidgetClass(menuButtonWidgetClass);
#endif
    XtInitializeWidgetClass(simpleMenuWidgetClass);
    XtInitializeWidgetClass(vim_scrollbarWidgetClass);
#endif

    /* The form containing all the other widgets */
    vimForm = XtVaCreateManagedWidget("vimForm",
	formWidgetClass,	vimShell,
	XtNborderWidth,		0,
	NULL);
    gui_athena_scroll_colors(vimForm);

#ifdef FEAT_MENU
    /* The top menu bar */
    menuBar = XtVaCreateManagedWidget("menuBar",
	boxWidgetClass,		vimForm,
	XtNresizable,		True,
	XtNtop,			XtChainTop,
	XtNbottom,		XtChainTop,
	XtNleft,		XtChainLeft,
	XtNright,		XtChainRight,
	NULL);
    gui_athena_menu_colors(menuBar);
    if (gui.menu_fg_pixel != -1)
	XtVaSetValues(menuBar, XtNborderColor, gui.menu_fg_pixel, NULL);
#endif

#ifdef FEAT_TOOLBAR
    toolBar = XtVaCreateManagedWidget("toolBar",
	boxWidgetClass,		vimForm,
	XtNresizable,		True,
	XtNtop,			XtChainTop,
	XtNbottom,		XtChainTop,
	XtNleft,		XtChainLeft,
	XtNright,		XtChainRight,
	XtNorientation,		XtorientHorizontal,
	XtNhSpace,		1,
	XtNvSpace,		3,
	NULL);
    gui_athena_menu_colors(toolBar);
#endif

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

#ifdef FEAT_MENU
    popupTrans = XtParseTranslationTable("<EnterWindow>: highlight()\n<LeaveWindow>: unhighlight()\n<BtnDown>: notify() unhighlight() MenuPopdown()\n<Motion>: highlight() menu-pullright()");
    parentTrans = XtParseTranslationTable("<EnterWindow>: highlight()\n<LeaveWindow>: unhighlight()\n<BtnUp>: notify() unhighlight() MenuPopdown()\n<BtnMotion>: highlight() menu-pullright()");
    menuTrans = XtParseTranslationTable("<EnterWindow>: highlight()\n<LeaveWindow>: unhighlight() MenuPopdown()\n<BtnUp>: notify() unhighlight() MenuPopdown()\n<BtnMotion>: highlight() menu-pullright()");
    supermenuTrans = XtParseTranslationTable("<EnterWindow>: highlight() menu-pullleft()\n<LeaveWindow>:\n<BtnUp>: notify() unhighlight() MenuPopdown()\n<BtnMotion>:");

    XtAppAddActions(XtWidgetToApplicationContext(vimForm), pullAction, 2);

    pullerBitmap = XCreateBitmapFromData(gui.dpy, DefaultRootWindow(gui.dpy),
			    (char *)puller_bits, puller_width, puller_height);
#endif

    /* Pretend we don't have input focus, we will get an event if we do. */
    gui.in_focus = FALSE;
}

/*
 * Called when the GUI is not going to start after all.
 */
    void
gui_x11_destroy_widgets()
{
    textArea = NULL;
#ifdef FEAT_MENU
    menuBar = NULL;
#endif
#ifdef FEAT_TOOLBAR
    toolBar = NULL;
#endif
}

#if defined(FEAT_TOOLBAR) || defined(PROTO)
    void
gui_mch_set_toolbar_pos(x, y, w, h)
    int	    x;
    int	    y;
    int	    w;
    int	    h;
{
    Dimension	border;
    int		height;

    XtUnmanageChild(toolBar);
    XtVaGetValues(toolBar,
		XtNborderWidth, &border,
		NULL);
    height = h - 2 * border;
    if (height < 0)
	height = 1;
    XtVaSetValues(toolBar,
		  XtNhorizDistance, x,
		  XtNvertDistance, y,
		  XtNwidth, w - 2 * border,
		  XtNheight,	height,
		  NULL);
    XtManageChild(toolBar);
}
#endif

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

#if defined(FEAT_MENU) || defined(PROTO)
/*
 * Menu stuff.
 */

static char_u *make_pull_name __ARGS((char_u * name));
static void gui_athena_reorder_menus	__ARGS((void));
static Widget get_popup_entry __ARGS((Widget w));
static void gui_mch_submenu_change __ARGS((vimmenu_T *mp, int colors));
static void gui_athena_menu_font __ARGS((Widget id));

    void
gui_mch_enable_menu(flag)
    int	    flag;
{
    if (flag)
    {
	XtManageChild(menuBar);
# ifdef FEAT_TOOLBAR
	if (XtIsManaged(toolBar))
	{
	    XtVaSetValues(toolBar,
		XtNvertDistance,    gui.menu_height,
		NULL);
	    XtVaSetValues(textArea,
		XtNvertDistance,    gui.menu_height + gui.toolbar_height,
		NULL);
	}
# endif
    }
    else
    {
	XtUnmanageChild(menuBar);
# ifdef FEAT_TOOLBAR
	if (XtIsManaged(toolBar))
	{
	    XtVaSetValues(toolBar,
		XtNvertDistance,    0,
		NULL);
	}
# endif
    }
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

/* ARGSUSED */
    void
gui_mch_add_menu(menu, idx)
    vimmenu_T	*menu;
    int		idx;
{
    char_u	*pullright_name;
    Dimension	height, space, border;
    vimmenu_T	*parent = menu->parent;

    if (parent == NULL)
    {
	if (menu_is_popup(menu->dname))
	{
	    menu->submenu_id = XtVaCreatePopupShell((char *)menu->dname,
		simpleMenuWidgetClass, vimShell,
		NULL);
	    gui_athena_menu_colors(menu->submenu_id);
	}
	else if (menu_is_menubar(menu->dname))
	{
	    menu->id = XtVaCreateManagedWidget((char *)menu->dname,
		menuButtonWidgetClass, menuBar,
		XtNmenuName, menu->dname,
		NULL);
	    if (menu->id == (Widget)0)
		return;
	    gui_athena_menu_colors(menu->id);
	    gui_athena_menu_font(menu->id);

	    menu->submenu_id = XtVaCreatePopupShell((char *)menu->dname,
		simpleMenuWidgetClass, menu->id,
		NULL);
	    gui_athena_menu_colors(menu->submenu_id);
	    gui_athena_menu_font(menu->submenu_id);

	    /* Don't update the menu height when it was set at a fixed value */
	    if (!gui.menu_height_fixed)
	    {
		/*
		 * When we add a top-level item to the menu bar, we can figure
		 * out how high the menu bar should be.
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
    }
    else if (parent->submenu_id != (Widget)0)
    {
	menu->id = XtVaCreateManagedWidget((char *)menu->dname,
	    smeBSBObjectClass, parent->submenu_id,
	    XtNrightMargin, puller_width,
	    XtNrightBitmap, pullerBitmap,
	    NULL);
	if (menu->id == (Widget)0)
	    return;
	gui_athena_menu_colors(menu->id);
	gui_athena_menu_font(menu->id);
	XtAddCallback(menu->id, XtNcallback, gui_x11_menu_cb,
	    (XtPointer)menu);

	pullright_name = make_pull_name(menu->dname);
	menu->submenu_id = XtVaCreatePopupShell((char *)pullright_name,
	    simpleMenuWidgetClass, parent->submenu_id,
	    XtNtranslations, menuTrans,
	    NULL);
	gui_athena_menu_colors(menu->submenu_id);
	gui_athena_menu_font(menu->submenu_id);
	vim_free(pullright_name);

	XtOverrideTranslations(parent->submenu_id, parentTrans);
    }
}

    static void
gui_athena_menu_font(id)
    Widget	id;
{
    int		managed = FALSE;

#ifdef EXPERIMENTAL
# ifdef FEAT_XFONTSET
    if (gui.menu_fontset != NOFONTSET)
    {
	if (XtIsManaged(id))
	{
	    XtUnmanageChild(id);
	    XtVaSetValues(id, XtNfontSet, gui.menu_fontset, NULL);
	    /* We should force the widget to recalculate it's
	     * geometry now.
	     */
	    XtManageChild(id);
	}
	else
	    XtVaSetValues(id, XtNfontSet, gui.menu_fontset, NULL);
    }
    else
# endif
#endif
    if (gui.menu_font != NOFONT)
    {
	if (XtIsManaged(id))
	{
	    XtUnmanageChild(id);
	    managed = TRUE;
	}

#ifdef FEAT_XFONTSET
	if (gui.fontset != NOFONTSET)
	    XtVaSetValues(id, XtNfontSet, gui.menu_font, NULL);
	else
#endif
	    XtVaSetValues(id, XtNfont, gui.menu_font, NULL);

	/* Force the widget to recalculate it's geometry now. */
	if (managed)
	    XtManageChild(id);
    }
}


    void
gui_mch_new_menu_font()
{
    if (menuBar == (Widget)0)
	return;
    gui_mch_submenu_change(root_menu, FALSE);

    {
	/* Iterate through the menubar menu items and get the height of
	 * each one.  The menu bar height is set to the maximum of all
	 * the heights.
	 */
	vimmenu_T *mp;
	int max_height = INT_MAX;

	for (mp = root_menu; mp != NULL; mp = mp->next)
	{
	    if (menu_is_menubar(mp->dname))
	    {
		Dimension height;

		XtVaGetValues(mp->id,
			XtNheight,(XtArgVal *)&height,
			NULL);
		if (height < max_height)
		    max_height = height;
	    }
	}
	if (max_height != INT_MAX)
	{
	    /* Don't update the menu height when it was set at a fixed value */
	    if (!gui.menu_height_fixed)
	    {
		Dimension   space, border;

		XtVaGetValues(menuBar,
			XtNvSpace,	&space,
			XtNborderWidth, &border,
			NULL);
		gui.menu_height = max_height + 2 * (space + border);
	    }
	}
    }
    /* Now, to simulate the window being resized.  Only, this
     * will resize the window to it's current state.
     *
     * Hopefully, the menu bar will be resized correctly.
     *
     * There has to be a better way, but I do not see one at this time.
     * (David Harrison)
     */
    {
	Position w, h;

	XtVaGetValues(vimShell,
		XtNwidth, &w,
		XtNheight, &h,
		NULL);
	gui_resize_shell(w, h
#ifdef FEAT_XIM
						- xim_get_status_area_height()
#endif
		     );
    }
    gui_set_shellsize(TRUE);
    ui_new_shellsize();
}


    static void
gui_mch_submenu_change(mp, colors)
    vimmenu_T	*mp;
    int		colors;		/* TRUE for colors, FALSE for font */
{
    while (mp != NULL)
    {
	if (mp->id != (Widget)0)
	{
	    if (colors)
		gui_athena_menu_colors(mp->id);
	    else
		gui_athena_menu_font(mp->id);
	}

	if (mp->children != NULL)
	{
	    /* Set the colors/font for the tear off widget */
	    if (mp->submenu_id != (Widget)0)
	    {
		if (colors)
		    gui_athena_menu_colors(mp->submenu_id);
		else
		    gui_athena_menu_font(mp->submenu_id);
	    }
	    /* Set the colors for the children */
	    gui_mch_submenu_change(mp->children, colors);
	}
	mp = mp->next;
    }
}

/*
 * Make a submenu name into a pullright name.
 * Replace '.' by '_', can't include '.' in the submenu name.
 */
    static char_u *
make_pull_name(name)
    char_u * name;
{
    char_u  *pname;
    char_u  *p;

    pname = vim_strnsave(name, STRLEN(name) + strlen("-pullright"));
    if (pname != NULL)
    {
	strcat((char *)pname, "-pullright");
	while ((p = vim_strchr(pname, '.')) != NULL)
	    *p = '_';
    }
    return pname;
}

/* ARGSUSED */
    void
gui_mch_add_menu_item(menu, idx)
    vimmenu_T	*menu;
    int		idx;
{
    vimmenu_T	*parent = menu->parent;

# ifdef FEAT_TOOLBAR
    if (menu_is_toolbar(parent->name))
    {
	WidgetClass	type;
	int		n;
	Arg		args[21];

	n = 0;
	if (menu_is_separator(menu->name))
	{
	    XtSetArg(args[n], XtNlabel, ""); n++;
	    XtSetArg(args[n], XtNborderWidth, 0); n++;
	}
	else
	{
	    Pixmap pixmap = 0;
	    Pixmap insensitive = 0;

	    if (strstr((const char *)p_toolbar, "icons") != NULL)
		get_pixmap(menu->name, &pixmap, &insensitive);
	    if (pixmap == 0)
	    {
		XtSetArg(args[n], XtNlabel, menu->dname); n++;
	    }
	    else
	    {
		XtSetArg(args[n], XtNbitmap, pixmap); n++;
		XtSetArg(args[n], XtNinternalHeight, 1); n++;
		XtSetArg(args[n], XtNinternalWidth, 1); n++;
		XtSetArg(args[n], XtNborderWidth, 1); n++;
	    }
	}
	XtSetArg(args[n], XtNhighlightThickness, 0); n++;
	type = commandWidgetClass;
	/* TODO: figure out the position in the toolbar?
	 *       This currently works fine for the default toolbar, but
	 *       what if we add/remove items during later runtime?
	 */

	if (menu->id == NULL)
	{
	    menu->id = XtCreateManagedWidget((char *)menu->dname,
			type, toolBar, args, n);
	    XtAddCallback(menu->id,
		    XtNcallback, gui_x11_menu_cb, menu);
#ifdef FEAT_BEVAL
	    if (menu->strings[MENU_INDEX_TIP] && menu->tip == NULL)
		menu->tip = gui_mch_create_beval_area(
				    menu->id,
				    menu->strings[MENU_INDEX_TIP],
				    NULL,
				    NULL);
#endif
	}
	else
	    XtSetValues(menu->id, args, n);
	gui_athena_menu_colors(menu->id);

	menu->parent = parent;
	menu->submenu_id = NULL;
	if (!XtIsManaged(toolBar)
		    && vim_strchr(p_go, GO_TOOLBAR) != NULL)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_height = gui_mch_compute_toolbar_height();
	return;
    } /* toolbar menu item */
# endif

    /* Add menu separator */
    if (menu_is_separator(menu->name))
    {
	menu->submenu_id = (Widget)0;
	menu->id = XtVaCreateManagedWidget((char *)menu->dname,
		smeLineObjectClass, parent->submenu_id,
		NULL);
	if (menu->id == (Widget)0)
	    return;
	gui_athena_menu_colors(menu->id);
    }
    else
    {
	if (parent != NULL && parent->submenu_id != (Widget)0)
	{
	    menu->submenu_id = (Widget)0;
	    menu->id = XtVaCreateManagedWidget((char *)menu->dname,
		    smeBSBObjectClass, parent->submenu_id,
		    NULL);
	    if (menu->id == (Widget)0)
		return;
	    gui_athena_menu_colors(menu->id);
	    gui_athena_menu_font(menu->id);
	    XtAddCallback(menu->id, XtNcallback, gui_x11_menu_cb,
		    (XtPointer)menu);
	}
    }
}

#if defined(FEAT_TOOLBAR) || defined(PROTO)
    void
gui_mch_show_toolbar(int showit)
{
    Cardinal	numChildren;	    /* how many children toolBar has */

    XtVaGetValues(toolBar, XtNnumChildren, &numChildren, NULL);
    if (showit && numChildren > 0)
    {
	gui.toolbar_height = gui_mch_compute_toolbar_height();
	XtManageChild(toolBar);
	if (XtIsManaged(menuBar))
	{
	    XtVaSetValues(textArea,
		    XtNvertDistance,    gui.toolbar_height + gui.menu_height,
		    NULL);
	    XtVaSetValues(toolBar,
		    XtNvertDistance,    gui.menu_height,
		    NULL);
	}
	else
	{
	    XtVaSetValues(textArea,
		    XtNvertDistance,    gui.toolbar_height,
		    NULL);
	    XtVaSetValues(toolBar,
		    XtNvertDistance,    0,
		    NULL);
	}
    }
    else
    {
	gui.toolbar_height = 0;
	if (XtIsManaged(menuBar))
	    XtVaSetValues(textArea,
		XtNvertDistance,    gui.menu_height,
		NULL);
	else
	    XtVaSetValues(textArea,
		XtNvertDistance,    0,
		NULL);

	XtUnmanageChild(toolBar);
    }
}


    int
gui_mch_compute_toolbar_height()
{
    Dimension	height;		    /* total Toolbar height */
    Dimension	whgt;		    /* height of each widget */
    Dimension	marginHeight;	    /* XmNmarginHeight of toolBar */
    Dimension	shadowThickness;    /* thickness of Xtparent(toolBar) */
    WidgetList	children;	    /* list of toolBar's children */
    Cardinal	numChildren;	    /* how many children toolBar has */
    int		i;

    height = 0;
    shadowThickness = 0;
    marginHeight = 0;
    if (toolBar != (Widget)0)
    {
	XtVaGetValues(toolBar,
		XtNborderWidth,	    &shadowThickness,
		XtNvSpace,	    &marginHeight,
		XtNchildren,	    &children,
		XtNnumChildren,	    &numChildren,
		NULL);
	for (i = 0; i < numChildren; i++)
	{
	    whgt = 0;

	    XtVaGetValues(children[i], XtNheight, &whgt, NULL);
	    if (height < whgt)
		height = whgt;
	}
    }

    return (int)(height + (marginHeight << 1) + (shadowThickness << 1));
}

    void
gui_mch_get_toolbar_colors(bgp, fgp)
    Pixel	*bgp;
    Pixel	*fgp;
{
    XtVaGetValues(toolBar, XtNbackground, bgp, XtNborderColor, fgp, NULL);
}
#endif


/* ARGSUSED */
    void
gui_mch_toggle_tearoffs(enable)
    int		enable;
{
    /* no tearoff menus */
}

    void
gui_mch_new_menu_colors()
{
    if (menuBar == (Widget)0)
	return;
    if (gui.menu_fg_pixel != -1)
	XtVaSetValues(menuBar, XtNborderColor,	gui.menu_fg_pixel, NULL);
    gui_athena_menu_colors(menuBar);
#ifdef FEAT_TOOLBAR
    gui_athena_menu_colors(toolBar);
#endif

    gui_mch_submenu_change(root_menu,TRUE);
}


/*
 * We can't always delete widgets, it would cause a crash.
 * Keep a list of dead widgets, so that we can avoid re-managing them.  This
 * means that they are still there but never shown.
 */
struct deadwid
{
    struct deadwid	*next;
    Widget		id;
};

static struct deadwid *first_deadwid = NULL;

/*
 * Destroy the machine specific menu widget.
 */
    void
gui_mch_destroy_menu(menu)
    vimmenu_T *menu;
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
	 * The code above causes a crash.  Apparently because the highlighting
	 * is still there, and removing it later causes the crash.
	 * This fix just unmanages the menu item, without destroying it.  The
	 * problem now is that the highlighting will be wrong, and we need to
	 * remember the ID to avoid that the item will be re-managed later...
	 */
	struct deadwid    *p;

	p = (struct deadwid *)alloc((unsigned)sizeof(struct deadwid));
	if (p != NULL)
	{
	    p->id = menu->id;
	    p->next = first_deadwid;
	    first_deadwid = p;
	}
	XtUnmanageChild(menu->id);
#endif
	menu->id = (Widget)0;
    }
}

/*
 * Reorder the menus to make them appear in the right order.
 * (This doesn't work for me...).
 */
    static void
gui_athena_reorder_menus()
{
    Widget	*children;
    Widget	swap_widget;
    int		num_children;
    int		to, from;
    vimmenu_T	*menu;
    struct deadwid *p;

    XtVaGetValues(menuBar,
	    XtNchildren,    &children,
	    XtNnumChildren, &num_children,
	    NULL);

    XtUnmanageChildren(children, num_children);

    menu = root_menu;
    for (to = 0; to < num_children - 1; ++to)
    {
	for (from = to; from < num_children; ++from)
	{
	    if (strcmp((char *)XtName(children[from]),
						    (char *)menu->dname) == 0)
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
#if 1
    /* Only manage children that have not been destroyed */
    for (to = 0; to < num_children; ++to)
    {
	for (p = first_deadwid; p != NULL; p = p->next)
	    if (p->id == children[to])
		break;
	if (p == NULL)
	    XtManageChild(children[to]);
    }
#else
    XtManageChildren(children, num_children);
#endif
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
    if (popup == (Widget)0)
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
    if (popup != (Widget)0)
	XtPopdown(popup);

    /* If this is the toplevel menu item, set parentTrans */
    if ((parent = XtParent(w)) != (Widget)0 && XtParent(parent) == menuBar)
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
    if ((menuw = XawSimpleMenuGetActiveEntry(w)) == (Widget)0)
	return NULL;

    pullright_name = make_pull_name((char_u *)XtName(menuw));
    popup = XtNameToWidget(w, (char *)pullright_name);
    vim_free(pullright_name);

    return popup;
}

/* ARGSUSED */
    void
gui_mch_show_popupmenu(menu)
    vimmenu_T *menu;
{
    int		rootx, rooty, winx, winy;
    Window	root, child;
    unsigned int mask;

    if (menu->submenu_id == (Widget)0)
	return;

    /* Position the popup menu at the pointer */
    if (XQueryPointer(gui.dpy, XtWindow(vimShell), &root, &child,
		&rootx, &rooty, &winx, &winy, &mask))
    {
	rootx -= 30;
	if (rootx < 0)
	    rootx = 0;
	rooty -= 5;
	if (rooty < 0)
	    rooty = 0;
	XtVaSetValues(menu->submenu_id,
		XtNx, rootx,
		XtNy, rooty,
		NULL);
    }

    XtOverrideTranslations(menu->submenu_id, popupTrans);
    XtPopupSpringLoaded(menu->submenu_id);
}

#endif /* FEAT_MENU */


/*
 * Scrollbar stuff.
 */

    void
gui_mch_set_scrollbar_thumb(sb, val, size, max)
    scrollbar_T	*sb;
    long	val;
    long	size;
    long	max;
{
    double	v, s;

    if (sb->id == (Widget)0)
	return;

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
    scrollbar_T *sb;
    int		x;
    int		y;
    int		w;
    int		h;
{
    if (sb->id == (Widget)0)
	return;

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
gui_mch_enable_scrollbar(sb, flag)
    scrollbar_T	*sb;
    int		flag;
{
    if (sb->id != (Widget)0)
    {
	if (flag)
	    XtManageChild(sb->id);
	else
	    XtUnmanageChild(sb->id);
    }
}

    void
gui_mch_create_scrollbar(sb, orient)
    scrollbar_T *sb;
    int		orient;	/* SBAR_VERT or SBAR_HORIZ */
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
    if (sb->id == (Widget)0)
	return;

    XtAddCallback(sb->id, XtNjumpProc,
		  gui_athena_scroll_cb_jump, (XtPointer)sb->ident);
    XtAddCallback(sb->id, XtNscrollProc,
		  gui_athena_scroll_cb_scroll, (XtPointer)sb->ident);

    vim_XawScrollbarSetThumb(sb->id, 0.0, 1.0, 0.0);
}

#if defined(FEAT_WINDOWS) || defined(PROTO)
    void
gui_mch_destroy_scrollbar(sb)
    scrollbar_T *sb;
{
    if (sb->id != (Widget)0)
	XtDestroyWidget(sb->id);
}
#endif

    void
gui_mch_set_scrollbar_colors(sb)
    scrollbar_T *sb;
{
    if (sb->id != (Widget)0)
	XtVaSetValues(sb->id,
	    XtNforeground, gui.scroll_fg_pixel,
	    XtNbackground, gui.scroll_bg_pixel,
	    NULL);

    /* This is needed for the rectangle below the vertical scrollbars. */
    if (sb == &gui.bottom_sbar && vimForm != (Widget)0)
	gui_athena_scroll_colors(vimForm);
}

/*
 * Miscellaneous stuff:
 */
    Window
gui_x11_get_wid()
{
    return XtWindow(textArea);
}

#if defined(FEAT_BROWSE) || defined(PROTO)
/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
/* ARGSUSED */
    char_u *
gui_mch_browse(saving, title, dflt, ext, initdir, filter)
    int		saving;		/* select file to write */
    char_u	*title;		/* not used (title for the window) */
    char_u	*dflt;		/* not used (default name) */
    char_u	*ext;		/* not used (extension added) */
    char_u	*initdir;	/* initial directory, NULL for current dir */
    char_u	*filter;	/* not used (file name filter) */
{
    Position x, y;

    /* Position the file selector just below the menubar */
    XtTranslateCoords(vimShell, (Position)0, (Position)
#ifdef FEAT_MENU
	    gui.menu_height
#else
	    0
#endif
	    , &x, &y);
    return (char_u *)vim_SelFile(vimShell, (char *)title, (char *)initdir,
		  NULL, (int)x, (int)y, gui.menu_fg_pixel, gui.menu_bg_pixel);
}
#endif

#if defined(FEAT_GUI_DIALOG) || defined(PROTO)

static int	dialogStatus;
static Atom	dialogatom;

static void keyhit_callback __ARGS((Widget w, XtPointer client_data, XEvent *event, Boolean *cont));
static void butproc __ARGS((Widget w, XtPointer client_data, XtPointer call_data));
static void dialog_wm_handler __ARGS((Widget w, XtPointer client_data, XEvent *event, Boolean *dum));

/*
 * Callback function for the textfield.  When CR is hit this works like
 * hitting the "OK" button, ESC like "Cancel".
 */
/* ARGSUSED */
    static void
keyhit_callback(w, client_data, event, cont)
    Widget		w;
    XtPointer		client_data;
    XEvent		*event;
    Boolean		*cont;
{
    char	buf[2];

    if (XLookupString(&(event->xkey), buf, 2, NULL, NULL) == 1)
    {
	if (*buf == CR)
	    dialogStatus = 1;
	else if (*buf == ESC)
	    dialogStatus = 2;
    }
}

/* ARGSUSED */
    static void
butproc(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data;
    XtPointer	call_data;
{
    dialogStatus = (int)(long)client_data + 1;
}

/*
 * Function called when dialog window closed.
 */
/*ARGSUSED*/
    static void
dialog_wm_handler(w, client_data, event, dum)
    Widget	w;
    XtPointer	client_data;
    XEvent	*event;
    Boolean	*dum;
{
    if (event->type == ClientMessage
	    && ((XClientMessageEvent *)event)->data.l[0] == dialogatom)
	dialogStatus = 2;
}

/* ARGSUSED */
    int
gui_mch_dialog(type, title, message, buttons, dfltbutton, textfield)
    int		type;
    char_u	*title;
    char_u	*message;
    char_u	*buttons;
    int		dfltbutton;
    char_u	*textfield;
{
    char_u		*buts;
    char_u		*p, *next;
    XtAppContext	app;
    XEvent		event;
    Position		wd, hd;
    Position		wv, hv;
    Position		x, y;
    Widget		dialog;
    Widget		dialogshell;
    Widget		dialogmessage;
    Widget		dialogtextfield = 0;
    Widget		dialogButton;
    Widget		prev_dialogButton = NULL;
    int			butcount;
    int			vertical;

    if (title == NULL)
	title = (char_u *)_("Vim dialog");
    dialogStatus = -1;

    /* if our pointer is currently hidden, then we should show it. */
    gui_mch_mousehide(FALSE);

    /* Check 'v' flag in 'guioptions': vertical button placement. */
    vertical = (vim_strchr(p_go, GO_VERTICAL) != NULL);

    /* The shell is created each time, to make sure it is resized properly */
    dialogshell = XtVaCreatePopupShell("dialogShell",
	    transientShellWidgetClass, vimShell,
	    XtNlabel, title,
	    NULL);
    if (dialogshell == (Widget)0)
	goto error;

    dialog = XtVaCreateManagedWidget("dialog",
	    formWidgetClass, dialogshell,
	    XtNdefaultDistance, 20,
	    NULL);
    if (dialog == (Widget)0)
	goto error;
    gui_athena_menu_colors(dialog);
    dialogmessage = XtVaCreateManagedWidget("dialogMessage",
	    labelWidgetClass, dialog,
	    XtNlabel, message,
	    XtNtop, XtChainTop,
	    XtNbottom, XtChainTop,
	    XtNleft, XtChainLeft,
	    XtNright, XtChainLeft,
	    XtNresizable, True,
	    XtNborderWidth, 0,
	    NULL);
    gui_athena_menu_colors(dialogmessage);

    if (textfield != NULL)
    {
	dialogtextfield = XtVaCreateManagedWidget("textfield",
		asciiTextWidgetClass, dialog,
		XtNwidth, 400,
		XtNtop, XtChainTop,
		XtNbottom, XtChainTop,
		XtNleft, XtChainLeft,
		XtNright, XtChainRight,
		XtNfromVert, dialogmessage,
		XtNresizable, True,
		XtNstring, textfield,
		XtNlength, IOSIZE,
		XtNuseStringInPlace, True,
		XtNeditType, XawtextEdit,
		XtNwrap, XawtextWrapNever,
		XtNresize, XawtextResizeHeight,
		NULL);
	XtManageChild(dialogtextfield);
	XtAddEventHandler(dialogtextfield, KeyPressMask, False,
			    (XtEventHandler)keyhit_callback, (XtPointer)NULL);
	XawTextSetInsertionPoint(dialogtextfield, STRLEN(textfield));
	XtSetKeyboardFocus(dialog, dialogtextfield);
    }

    /* make a copy, so that we can insert NULs */
    buts = vim_strsave(buttons);
    if (buts == NULL)
	return -1;

    p = buts;
    for (butcount = 0; *p; ++butcount)
    {
	for (next = p; *next; ++next)
	{
	    if (*next == DLG_HOTKEY_CHAR)
		mch_memmove(next, next + 1, STRLEN(next));
	    if (*next == DLG_BUTTON_SEP)
	    {
		*next++ = NUL;
		break;
	    }
	}
	dialogButton = XtVaCreateManagedWidget("button",
		commandWidgetClass, dialog,
		XtNlabel, p,
		XtNtop, XtChainBottom,
		XtNbottom, XtChainBottom,
		XtNleft, XtChainLeft,
		XtNright, XtChainLeft,
		XtNfromVert, textfield == NULL ? dialogmessage : dialogtextfield,
		XtNvertDistance, vertical ? 4 : 20,
		XtNresizable, False,
		NULL);
	gui_athena_menu_colors(dialogButton);
	if (butcount > 0)
	    XtVaSetValues(dialogButton,
		    vertical ? XtNfromVert : XtNfromHoriz, prev_dialogButton,
		    NULL);

	XtAddCallback(dialogButton, XtNcallback, butproc, (XtPointer)butcount);
	p = next;
	prev_dialogButton = dialogButton;
    }
    vim_free(buts);

    XtRealizeWidget(dialogshell);

    /* Setup for catching the close-window event, don't let it close Vim! */
    dialogatom = XInternAtom(gui.dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(gui.dpy, XtWindow(dialogshell), &dialogatom, 1);
    XtAddEventHandler(dialogshell, NoEventMask, True, dialog_wm_handler, NULL);

    XtVaGetValues(dialogshell,
	    XtNwidth, &wd,
	    XtNheight, &hd,
	    NULL);
    XtVaGetValues(vimShell,
	    XtNwidth, &wv,
	    XtNheight, &hv,
	    NULL);
    XtTranslateCoords(vimShell,
	    (Position)((wv - wd) / 2),
	    (Position)((hv - hd) / 2),
	    &x, &y);
    if (x < 0)
	x = 0;
    if (y < 0)
	y = 0;
    XtVaSetValues(dialogshell, XtNx, x, XtNy, y, NULL);

    /* Position the mouse pointer in the dialog, required for when focus
     * follows mouse. */
    XWarpPointer(gui.dpy, (Window)0, XtWindow(dialogshell), 0, 0, 0, 0, 20, 40);


    app = XtWidgetToApplicationContext(dialogshell);

    XtPopup(dialogshell, XtGrabNonexclusive);

    while (1)
    {
	XtAppNextEvent(app, &event);
	XtDispatchEvent(&event);
	if (dialogStatus >= 0)
	    break;
    }

    XtPopdown(dialogshell);

    if (textfield != NULL && dialogStatus < 0)
	*textfield = NUL;

error:
    XtDestroyWidget(dialogshell);

    return dialogStatus;
}
#endif

#if defined(FEAT_GUI_DIALOG) || defined(FEAT_MENU)
/*
 * Set the colors of Widget "id" to the menu colors.
 */
    static void
gui_athena_menu_colors(id)
    Widget  id;
{
    if (gui.menu_bg_pixel != -1)
	XtVaSetValues(id, XtNbackground, gui.menu_bg_pixel, NULL);
    if (gui.menu_fg_pixel != -1)
	XtVaSetValues(id, XtNforeground, gui.menu_fg_pixel, NULL);
}
#endif

/*
 * Set the colors of Widget "id" to the scroll colors.
 */
    static void
gui_athena_scroll_colors(id)
    Widget  id;
{
    if (gui.scroll_bg_pixel != -1)
	XtVaSetValues(id, XtNbackground, gui.scroll_bg_pixel, NULL);
    if (gui.scroll_fg_pixel != -1)
	XtVaSetValues(id, XtNforeground, gui.scroll_fg_pixel, NULL);
}
