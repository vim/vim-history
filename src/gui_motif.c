/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/ScrollBar.h>
#include <Xm/MenuShell.h>
#if (XmVersion >= 1002)
# include <Xm/RepType.h>
#endif

#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>

#include "vim.h"

extern Widget vimShell;

static Widget vimForm;
static Widget textArea;
static Widget menuBar;

static void gui_mch_compute_menu_height __ARGS((Widget));
static void gui_mch_submenu_colors __ARGS((GuiMenu *mp));

/*
 * Call-back routines.
 */

/* ARGSUSED */
    static void
scroll_cb(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data, call_data;
{
    GuiScrollbar *sb;
    long	value;
    int		dragging;

    sb = gui_find_scrollbar((long)client_data);

    value = ((XmScrollBarCallbackStruct *)call_data)->value;
    dragging = (((XmScrollBarCallbackStruct *)call_data)->reason ==
							      (int)XmCR_DRAG);
    gui_drag_scrollbar(sb, value, dragging);
}


/*
 * End of call-back routines
 */

/*
 * Create all the motif widgets necessary.
 */
    void
gui_x11_create_widgets()
{
    /*
     * Start out by adding the configured border width into the border offset
     */
    gui.border_offset = gui.border_width;

    /*
     * Install the tearOffModel resource converter.
     */
#if (XmVersion >= 1002)
    XmRepTypeInstallTearOffModelConverter();
#endif

    XtInitializeWidgetClass(xmFormWidgetClass);
    XtInitializeWidgetClass(xmRowColumnWidgetClass);
    XtInitializeWidgetClass(xmPrimitiveWidgetClass);
    XtInitializeWidgetClass(xmCascadeButtonWidgetClass);
    XtInitializeWidgetClass(xmMenuShellWidgetClass);
    XtInitializeWidgetClass(xmPushButtonWidgetClass);
    XtInitializeWidgetClass(xmScrollBarWidgetClass);

    /* Make sure the "Quit" menu entry of the window manager is ignored */
    XtVaSetValues(vimShell, XmNdeleteResponse, XmDO_NOTHING, NULL);

    vimForm = XtVaCreateManagedWidget("vimForm",
	xmFormWidgetClass, vimShell,
	XmNborderWidth, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	XmNforeground, gui.menu_fg_pixel,
	XmNbackground, gui.menu_bg_pixel,
	NULL);

    textArea = XtVaCreateManagedWidget("textArea",
	xmPrimitiveWidgetClass, vimForm,
	XmNbackground, gui.back_pixel,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_OPPOSITE_FORM,
	XmNbottomAttachment, XmATTACH_OPPOSITE_FORM,

	/*
	 * These take some control away from the user, but avoids making them
	 * add resources to get a decent looking setup.
	 */
	XmNborderWidth, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	NULL);

    menuBar = XtVaCreateManagedWidget("menuBar",
	xmRowColumnWidgetClass, vimForm,
#if (XmVersion >= 1002)
	XmNtearOffModel, XmTEAR_OFF_ENABLED,
#endif
	XmNrowColumnType, XmMENU_BAR,
	XmNforeground, gui.menu_fg_pixel,
	XmNbackground, gui.menu_bg_pixel,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_OPPOSITE_FORM,
	XmNrightOffset, 0,	/* Always stick to rigth hand side */
	NULL);

    /*
     * Install the callbacks.
     */
    gui_x11_callbacks(textArea, vimForm);
}

    void
gui_mch_set_text_area_pos(x, y, w, h)
    int	    x;
    int	    y;
    int	    w;
    int	    h;
{
    XtVaSetValues(textArea,
		  XmNleftOffset, x,
		  XmNtopOffset, y,
		  XmNrightOffset, -x - w,
		  XmNbottomOffset, -y - h,
		  NULL);
}

    void
gui_x11_set_back_color()
{
    if (textArea != NULL)
	XtVaSetValues(textArea,
		  XmNbackground, gui.back_pixel,
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

/* ARGSUSED */
    void
gui_mch_set_menu_pos(x, y, w, h)
    int	    x;
    int	    y;
    int	    w;
    int	    h;
{
    XtVaSetValues(menuBar,
		  XmNleftOffset, x,
		  XmNtopOffset, y,
		  XmNbottomOffset, -y - h,
		  NULL);
}

    void
gui_mch_add_menu(menu, parent)
    GuiMenu *menu;
    GuiMenu *parent;
{
#if (XmVersion >= 1002)
    Widget	widget;
    XmString	label = XmStringCreate((char *)menu->name,
						      XmFONTLIST_DEFAULT_TAG);
#else
    XmString	label = XmStringCreate((char *)menu->name,
						    XmSTRING_DEFAULT_CHARSET);
#endif
    Widget	shell;
    GuiMenu	*mp;
    int		idx;

    menu->id = XtVaCreateWidget("subMenu",
	xmCascadeButtonWidgetClass,
	(parent == NULL) ? menuBar : parent->submenu_id,
	XmNlabelString, label,
	XmNforeground, gui.menu_fg_pixel,
	XmNbackground, gui.menu_bg_pixel,
	NULL);
    /* XtFree((char *)label); makes Lesstif crash */

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	XmNforeground, gui.menu_fg_pixel,
	XmNbackground, gui.menu_bg_pixel,
	NULL);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xmRowColumnWidgetClass, shell,
	XmNrowColumnType, XmMENU_PULLDOWN,
#if (XmVersion >= 1002)
	XmNtearOffModel, XmTEAR_OFF_ENABLED,
#endif
	NULL);

#if (XmVersion >= 1002)
    /* Set the colors for the tear off widget */
    if ((widget = XmGetTearOffControl(menu->submenu_id)) != (Widget)NULL)
	XtVaSetValues(widget,
	    XmNforeground, gui.menu_fg_pixel,
	    XmNbackground, gui.menu_bg_pixel,
	    NULL);
#endif

    XtVaSetValues(menu->id,
	XmNsubMenuId, menu->submenu_id,
	NULL);

    /*
     * The "Help" menu is a special case, and should be placed at the far right
     * hand side of the menu-bar.
     */
    if (parent == NULL && STRCMP((char *)menu->name, "Help") == 0)
	XtVaSetValues(menuBar,
	    XmNmenuHelpWidget, menu->id,
	    NULL);

    if (parent == NULL)
    {
#if (XmVersion >= 1002)
	/* XmNpositionIndex was added in Motif 1.2 */
	/*
	 * Put the menu at the right position.
	 */
	mp = gui.root_menu;
	for (idx = 0; mp != NULL; ++idx)
	{
	    /*
	     * Skip the Help menu.
	     */
	    if (STRCMP((char *)mp->name, "Help") == 0)
		--idx;
	    else if (mp == menu)
	    {
		XtVaSetValues(menu->id,
		    XmNpositionIndex, idx,
		    NULL);
		break;
	    }
	    mp = mp->next;
	}
#endif

	/*
	 * When we add a top-level item to the menu bar, we can figure out how
	 * high the menu bar should be.
	 */
	gui_mch_compute_menu_height(menu->id);
    }
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position an height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    static void
gui_mch_compute_menu_height(id)
    Widget	id;		    /* can be NULL when deleting menu */
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    GuiMenu	*mp;
    static Dimension	height = 21;	/* normal height of a menu item */

    /* Don't update the menu height when it was set at a fixed value */
    if (gui.menu_height_fixed)
	return;

    /*
     * Get the height of the new item, before managing it, because it will
     * still reflect the font size.  After managing it depends on the menu
     * height, which is what we just wanted to get!.
     */
    if (id != (Widget)0)
	XtVaGetValues(id, XmNheight, &height, NULL);

    /* Find any menu Widget, to be able to call XtManageChild() */
    else
	for (mp = gui.root_menu; mp != NULL; mp = mp->next)
	    if (mp->id != (Widget)0)
	    {
		id = mp->id;
		break;
	    }

    /*
     * Now manage the menu item, to make them all be positioned (makes an
     * extra row when needed, removes it when not needed).
     */
    if (id != (Widget)0)
	XtManageChild(id);

    /*
     * Now find the menu item that is the furthest down, and get it's position.
     */
    maxy = 0;
    for (mp = gui.root_menu; mp != NULL; mp = mp->next)
    {
	if (mp->id != (Widget)0)
	{
	    XtVaGetValues(mp->id, XmNy, &y, NULL);
	    if (y > maxy)
		maxy = y;
	}
    }

    XtVaGetValues(menuBar,
	XmNmarginHeight, &margin,
	XmNshadowThickness, &shadow,
	NULL);

    /*
     * This computation is the result of trial-and-error:
     * maxy =	The maximum position of an item; required for when there are
     *		two or more rows
     * height = height of an item, before managing it;	Hopefully this will
     *		change with the font height.  Includes shadow-border.
     * shadow =	shadow-border; must be subtracted from the height.
     * margin = margin around the menu buttons;  Must be added.
     */
    gui.menu_height = maxy + height - 2 * shadow + 2 * margin;
}

    void
gui_mch_add_menu_item(menu, parent)
    GuiMenu *menu;
    GuiMenu *parent;
{
#if (XmVersion >= 1002)
    XmString label = XmStringCreate((char *)menu->name,
						      XmFONTLIST_DEFAULT_TAG);
#else
    XmString label = XmStringCreate((char *)menu->name,
						    XmSTRING_DEFAULT_CHARSET);
#endif

    menu->submenu_id = (Widget)0;
    menu->id = XtVaCreateWidget("subMenu",
	xmPushButtonWidgetClass, parent->submenu_id,
	XmNlabelString, label,
	XmNforeground, gui.menu_fg_pixel,
	XmNbackground, gui.menu_bg_pixel,
	NULL);
    /* XtFree((char *)label); makes Lesstif crash */

    XtAddCallback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
	(XtPointer)menu);
}

    void
gui_mch_new_menu_colors()
{
    if (menuBar == NULL)
	return;
    XtVaSetValues(menuBar,
	XmNforeground, gui.menu_fg_pixel,
	XmNbackground, gui.menu_bg_pixel,
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
		XmNforeground, gui.menu_fg_pixel,
		XmNbackground, gui.menu_bg_pixel,
		NULL);

	if (mp->children != NULL)
	{
#if (XmVersion >= 1002)
	    Widget	widget;

	    /* Set the colors for the tear off widget */
	    if ((widget = XmGetTearOffControl(mp->submenu_id)) != (Widget)NULL)
		XtVaSetValues(widget,
			XmNforeground, gui.menu_fg_pixel,
			XmNbackground, gui.menu_bg_pixel,
			NULL);
#endif
	    /* Set the colors for the children */
	    gui_mch_submenu_colors(mp->children);
	}
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
    Widget  parent;

    if (menu->submenu_id != (Widget)0)
    {
	XtDestroyWidget(menu->submenu_id);
	menu->submenu_id = (Widget)0;
    }
    if (menu->id != (Widget)0)
    {
	/*
	 * This is a hack to stop LessTif from crashing when a menu's last
	 * child is destroyed. We check to see if this is the last child and if
	 * so, don't delete it. The parent will be deleted soon anyway, and it
	 * will delete it's children like all good widgets do.
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
	menu->id = (Widget)0;

	if (parent == menuBar)
	    gui_mch_compute_menu_height((Widget)NULL);
    }
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
    XtVaSetValues(sb->id,
		  XmNvalue, val,
		  XmNsliderSize, size,
		  XmNpageIncrement, (size > 2 ? size - 2 : 1),
		  XmNmaximum, max + 1,	    /* Motif has max one past the end */
		  NULL);
}

    void
gui_mch_set_scrollbar_pos(sb, x, y, w, h)
    GuiScrollbar    *sb;
    int		    x;
    int		    y;
    int		    w;
    int		    h;
{
    XtVaSetValues(sb->id,
		  XmNleftOffset, x,
		  XmNtopOffset, y,
		  XmNrightOffset, -x - w,
		  XmNbottomOffset, -y - h,
		  NULL);
}

    void
gui_mch_create_scrollbar(sb, orient)
    GuiScrollbar    *sb;
    int		    orient;	/* SBAR_VERT or SBAR_HORIZ */
{
    sb->id = XtVaCreateManagedWidget("scrollBar",
	    xmScrollBarWidgetClass, vimForm,
	    XmNshadowThickness, 1,
	    XmNminimum, 0,
	    XmNorientation, (orient == SBAR_VERT) ? XmVERTICAL
						  : XmHORIZONTAL,
	    XmNforeground, gui.scroll_fg_pixel,
	    XmNbackground, gui.scroll_fg_pixel,
	    XmNtroughColor, gui.scroll_bg_pixel,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_OPPOSITE_FORM,
	    XmNbottomAttachment, XmATTACH_OPPOSITE_FORM,
	    NULL);
    XtAddCallback(sb->id, XmNvalueChangedCallback,
		  scroll_cb, (XtPointer)sb->ident);
    XtAddCallback(sb->id, XmNdragCallback,
		  scroll_cb, (XtPointer)sb->ident);
    XtAddEventHandler(sb->id, KeyPressMask, FALSE, gui_x11_key_hit_cb,
	(XtPointer)0);
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
	    XmNforeground, gui.scroll_fg_pixel,
	    XmNbackground, gui.scroll_fg_pixel,
	    XmNtroughColor, gui.scroll_bg_pixel,
	    NULL);
}

/*
 * Miscellaneous stuff:
 */

    Window
gui_x11_get_wid()
{
    return(XtWindow(textArea));
}
