/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/Separator.h>
#include <Xm/Label.h>
#include <Xm/CascadeB.h>
#include <Xm/ScrollBar.h>
#include <Xm/MenuShell.h>
#include <Xm/DrawingA.h>
#if (XmVersion >= 1002)
# include <Xm/RepType.h>
#endif
#include <Xm/Frame.h>

#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>

#include "vim.h"

#define MOTIF_POPUP

extern Widget vimShell;

static Widget vimForm;
static Widget textAreaFrame;
static Widget textAreaForm;
Widget textArea;
#ifdef FEAT_TOOLBAR
static Widget toolBarFrame;
static Widget toolBar;
#endif
#ifdef FEAT_FOOTER
static Widget footer;
#endif
#ifdef FEAT_MENU
# if (XmVersion >= 1002)
/* remember the last set value for the tearoff item */
static int tearoff_val = (int)XmTEAR_OFF_ENABLED;
# endif
static Widget menuBar;
#endif

static void scroll_cb __ARGS((Widget w, XtPointer client_data, XtPointer call_data));
#ifdef FEAT_TOOLBAR
static void toolbar_enter_cb __ARGS((Widget, XtPointer, XEvent *, Boolean *));
static void toolbar_leave_cb __ARGS((Widget, XtPointer, XEvent *, Boolean *));
static void toolbarbutton_enter_cb __ARGS((Widget, XtPointer, XEvent *, Boolean *));
static void toolbarbutton_leave_cb __ARGS((Widget, XtPointer, XEvent *, Boolean *));
#endif
#ifdef FEAT_FOOTER
static int gui_mch_compute_footer_height __ARGS((void));
#endif

#ifdef WSDEBUG
static void attachDump(Widget, char *);
#endif

static void gui_motif_menu_colors __ARGS((Widget id));
static void gui_motif_scroll_colors __ARGS((Widget id));
#ifdef FEAT_MENU
static void gui_motif_menu_fontlist __ARGS((Widget id));
#endif

#if (XmVersion >= 1002)
# define STRING_TAG  XmFONTLIST_DEFAULT_TAG
#else
# define STRING_TAG  XmSTRING_DEFAULT_CHARSET
#endif

/*
 * Call-back routines.
 */

/* ARGSUSED */
    static void
scroll_cb(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data, call_data;
{
    scrollbar_T *sb;
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

#if 0 /* not needed? */
    XtInitializeWidgetClass(xmFormWidgetClass);
    XtInitializeWidgetClass(xmRowColumnWidgetClass);
    XtInitializeWidgetClass(xmDrawingAreaWidgetClass);
    XtInitializeWidgetClass(xmCascadeButtonWidgetClass);
    XtInitializeWidgetClass(xmMenuShellWidgetClass);
    XtInitializeWidgetClass(xmPushButtonWidgetClass);
    XtInitializeWidgetClass(xmScrollBarWidgetClass);
#endif

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
	NULL);
    gui_motif_menu_colors(vimForm);

#ifdef FEAT_MENU
    {
	Arg al[7]; /* Make sure there is enough room for arguments! */
	int ac = 0;

	if (gui.menu_fg_pixel != -1)
	{
	    XtSetArg(al[ac], XmNforeground, gui.menu_fg_pixel); ac++;
	}
	if (gui.menu_bg_pixel != -1)
	{
	    XtSetArg(al[ac], XmNbackground, gui.menu_bg_pixel); ac++;
	}
# if (XmVersion >= 1002)
	XtSetArg(al[ac], XmNtearOffModel, tearoff_val); ac++;
# endif
	XtSetArg(al[ac], XmNleftAttachment,  XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNtopAttachment,   XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
# ifndef FEAT_TOOLBAR
	/* Always stick to right hand side. */
	XtSetArg(al[ac], XmNrightOffset, 0); ac++;
# endif
	menuBar = XmCreateMenuBar(vimForm, "menuBar", al, ac);
	XtManageChild(menuBar);
    }
#endif

#ifdef FEAT_TOOLBAR
    /*
     * Create an empty ToolBar. We should get buttons defined from menu.vim.
     */
    toolBarFrame = XtVaCreateWidget("toolBarFrame",
	xmFrameWidgetClass, vimForm,
	XmNchildType, XmFRAME_WORKAREA_CHILD,
	XmNshadowType, XmSHADOW_OUT,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	NULL);
    gui_motif_menu_colors(toolBarFrame);

    toolBar = XtVaCreateManagedWidget("toolBar",
	xmRowColumnWidgetClass, toolBarFrame,
	XmNrowColumnType, XmWORK_AREA,
	XmNorientation, XmHORIZONTAL,
	XmNtraversalOn, False,
	XmNisHomogeneous, False,
	XmNpacking, XmPACK_TIGHT,
	XmNspacing, 0,
	NULL);
    gui_motif_menu_colors(toolBar);

    XtAddEventHandler(toolBar, EnterWindowMask, False,
	    toolbar_enter_cb, NULL);
    XtAddEventHandler(toolBar, LeaveWindowMask, False,
	    toolbar_leave_cb, NULL);
#endif

    textAreaFrame = XtVaCreateManagedWidget("textAreaFrame",
	xmFrameWidgetClass, vimForm,
	XmNchildType, XmFRAME_WORKAREA_CHILD,
	XmNshadowType, XmSHADOW_OUT,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	NULL);
    gui_motif_scroll_colors(textAreaFrame);

    textAreaForm = XtVaCreateManagedWidget("textAreaForm",
	xmFormWidgetClass, textAreaFrame,
	XmNborderWidth, 0,
	XmNshadowThickness, 0,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	NULL);
    gui_motif_scroll_colors(textAreaForm);

    textArea = XtVaCreateManagedWidget("textArea",
	xmDrawingAreaWidgetClass, textAreaForm,
	XmNforeground, gui.norm_pixel,
	XmNbackground, gui.back_pixel,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,

	/*
	 * These take some control away from the user, but avoids making them
	 * add resources to get a decent looking setup.
	 */
	XmNborderWidth, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	NULL);

#ifdef FEAT_FOOTER
    /*
     * Create the Footer.
     */
    footer = XtVaCreateWidget("footer",
	xmLabelWidgetClass, vimForm,
	XmNalignment, XmALIGNMENT_BEGINNING,
	XmNmarginHeight, 0,
	XmNmarginWidth, 0,
	XmNtraversalOn, False,
	XmNrecomputeSize, False,
	XmNleftAttachment, XmATTACH_FORM,
	XmNleftOffset, 5,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	NULL);
    gui_mch_set_footer((char_u *) "");
#endif

    /*
     * Install the callbacks.
     */
    gui_x11_callbacks(textArea, vimForm);

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
}

/*ARGSUSED*/
    void
gui_mch_set_text_area_pos(x, y, w, h)
    int	    x;
    int	    y;
    int	    w;
    int	    h;
{
#ifdef FEAT_TOOLBAR
    /* Give keyboard focus to the textArea instead of the toolbar. */
    gui_mch_reset_focus();
#endif
}

    void
gui_x11_set_back_color()
{
    if (textArea != NULL)
#if (XmVersion >= 1002)
	XmChangeColor(textArea, gui.back_pixel);
#else
	XtVaSetValues(textArea,
		  XmNbackground, gui.back_pixel,
		  NULL);
#endif
}

#if defined(FEAT_MENU) || defined(FEAT_SUN_WORKSHOP) || defined(PROTO)
/*
 * Encapsulate the way an XmFontList is created.
 */
    XmFontList
gui_motif_create_fontlist(font)
    XFontStruct    *font;
{
    XmFontList font_list;

#if (XmVersion <= 1001)
    /* Motif 1.1 method */
    font_list = XmFontListCreate(font, STRING_TAG);
#else
    /* Motif 1.2 method */
    XmFontListEntry font_list_entry;

    font_list_entry = XmFontListEntryCreate(STRING_TAG,
# ifdef FEAT_XFONTSET
	    gui.fontset != NOFONTSET ? XmFONT_IS_FONTSET :
# endif
	    XmFONT_IS_FONT, (XtPointer)font);
    font_list = XmFontListAppendEntry(NULL, font_list_entry);
    XmFontListEntryFree(&font_list_entry);
#endif
    return font_list;
}

/*
 * Menu stuff.
 */

static void gui_motif_add_actext __ARGS((vimmenu_T *menu));
#if (XmVersion >= 1002)
static void toggle_tearoff __ARGS((Widget wid));
static void gui_mch_recurse_tearoffs __ARGS((vimmenu_T *menu));
#endif
static void gui_mch_submenu_change __ARGS((vimmenu_T *mp, int colors));

static void do_set_mnemonics __ARGS((int enable));
static int menu_enabled = TRUE;

    void
gui_mch_enable_menu(flag)
    int	    flag;
{
    if (flag)
    {
	XtManageChild(menuBar);
#ifdef FEAT_TOOLBAR
	if (XtIsManaged(XtParent(toolBar)))
	{
	    /* toolBar is attached to top form */
	    XtVaSetValues(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		NULL);
	    XtVaSetValues(textAreaFrame,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, XtParent(toolBar),
		NULL);
	}
	else
#endif
	{
	    XtVaSetValues(textAreaFrame,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		NULL);
	}
    }
    else
    {
	XtUnmanageChild(menuBar);
#ifdef FEAT_TOOLBAR
	if (XtIsManaged(XtParent(toolBar)))
	{
	    XtVaSetValues(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_FORM,
		NULL);
	    XtVaSetValues(textAreaFrame,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, XtParent(toolBar),
		NULL);
	}
	else
#endif
	{
	    XtVaSetValues(textAreaFrame,
		XmNtopAttachment, XmATTACH_FORM,
		NULL);
	}
    }

}

/* ARGSUSED */
    void
gui_mch_set_menu_pos(x, y, w, h)
    int	    x;
    int	    y;
    int	    w;
    int	    h;
{
    /* menuBar is now using XmForm attachments to be positioned */
}

/*
 * Enable or disable mnemonics for the toplevel menus.
 */
    void
gui_motif_set_mnemonics(enable)
    int		enable;
{
    /*
     * Don't enable menu mnemonics when the menu bar is disabled, Lesstif
     * crashes when using a mnemonic then.
     */
    if (!menu_enabled)
	enable = FALSE;
    do_set_mnemonics(enable);
}

    static void
do_set_mnemonics(enable)
    int		enable;
{
    vimmenu_T	*menu;

    for (menu = root_menu; menu != NULL; menu = menu->next)
	if (menu->id != (Widget)0)
	    XtVaSetValues(menu->id,
		    XmNmnemonic, enable ? menu->mnemonic : NUL,
		    NULL);
}

    void
gui_mch_add_menu(menu, idx)
    vimmenu_T	*menu;
    int		idx;
{
    XmString	label;
    Widget	shell;
    vimmenu_T	*parent = menu->parent;

#ifdef MOTIF_POPUP
    if (menu_is_popup(menu->name))
    {
	Arg arg[2];
	int n = 0;

	/* Only create the popup menu when it's actually used, otherwise there
	 * is a delay when using the right mouse button. */
# if (XmVersion <= 1002)
	if (mouse_model_popup())
# endif
	{
	    if (gui.menu_bg_pixel != -1)
	    {
		XtSetArg(arg[0], XmNbackground, gui.menu_bg_pixel); n++;
	    }
	    if (gui.menu_fg_pixel != -1)
	    {
		XtSetArg(arg[1], XmNforeground, gui.menu_fg_pixel); n++;
	    }
	    menu->submenu_id = XmCreatePopupMenu(textArea, "contextMenu",
								      arg, n);
	    menu->id = (Widget)0;
	}
	return;
    }
#endif

    if (!menu_is_menubar(menu->name)
	    || (parent != NULL && parent->submenu_id == (Widget)0))
	return;

    label = XmStringCreate((char *)menu->dname, STRING_TAG);
    if (label == NULL)
	return;
    menu->id = XtVaCreateWidget("subMenu",
	    xmCascadeButtonWidgetClass,
	    (parent == NULL) ? menuBar : parent->submenu_id,
	    XmNlabelString, label,
	    XmNmnemonic, p_wak[0] == 'n' ? NUL : menu->mnemonic,
#if (XmVersion >= 1002)
	    /* submenu: count the tearoff item (needed for LessTif) */
	    XmNpositionIndex, idx + (parent != NULL
			   && tearoff_val == (int)XmTEAR_OFF_ENABLED ? 1 : 0),
#endif
	    NULL);
    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);
    XmStringFree(label);

    if (menu->id == (Widget)0)		/* failed */
	return;

    /* add accelerator text */
    gui_motif_add_actext(menu);

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	NULL);
    gui_motif_menu_colors(shell);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xmRowColumnWidgetClass, shell,
	XmNrowColumnType, XmMENU_PULLDOWN,
	NULL);
    gui_motif_menu_colors(menu->submenu_id);

    if (menu->submenu_id == (Widget)0)		/* failed */
	return;

#if (XmVersion >= 1002)
    /* Set the colors for the tear off widget */
    toggle_tearoff(menu->submenu_id);
#endif

    XtVaSetValues(menu->id,
	XmNsubMenuId, menu->submenu_id,
	NULL);

    /*
     * The "Help" menu is a special case, and should be placed at the far
     * right hand side of the menu-bar.  It's recognized by its high priority.
     */
    if (parent == NULL && menu->priority >= 9999)
	XtVaSetValues(menuBar,
		XmNmenuHelpWidget, menu->id,
		NULL);

    /*
     * When we add a top-level item to the menu bar, we can figure out how
     * high the menu bar should be.
     */
    if (parent == NULL)
	gui_mch_compute_menu_height(menu->id);
}


/*
 * Add mnemonic and accelerator text to a menu button.
 */
    static void
gui_motif_add_actext(menu)
    vimmenu_T	*menu;
{
    XmString	label;

    /* Add accelrator text, if there is one */
    if (menu->actext != NULL && menu->id != (Widget)0)
    {
	label = XmStringCreate((char *)menu->actext, STRING_TAG);
	if (label == NULL)
	    return;
	XtVaSetValues(menu->id, XmNacceleratorText, label, NULL);
	XmStringFree(label);
    }
}

    void
gui_mch_toggle_tearoffs(enable)
    int		enable;
{
#if (XmVersion >= 1002)
    if (enable)
	tearoff_val = (int)XmTEAR_OFF_ENABLED;
    else
	tearoff_val = (int)XmTEAR_OFF_DISABLED;
    toggle_tearoff(menuBar);
    gui_mch_recurse_tearoffs(root_menu);
#endif
}

#if (XmVersion >= 1002)
/*
 * Set the tearoff for one menu widget on or off, and set the color of the
 * tearoff widget.
 */
    static void
toggle_tearoff(wid)
    Widget	wid;
{
    Widget	w;

    XtVaSetValues(wid, XmNtearOffModel, tearoff_val, NULL);
    if (tearoff_val == (int)XmTEAR_OFF_ENABLED
	    && (w = XmGetTearOffControl(wid)) != (Widget)0)
	gui_motif_menu_colors(w);
}

    static void
gui_mch_recurse_tearoffs(menu)
    vimmenu_T	*menu;
{
    while (menu != NULL)
    {
	if (!menu_is_popup(menu->name))
	{
	    if (menu->submenu_id != (Widget)0)
		toggle_tearoff(menu->submenu_id);
	    gui_mch_recurse_tearoffs(menu->children);
	}
	menu = menu->next;
    }
}
#endif

    int
gui_mch_text_area_extra_height()
{
    Dimension	shadowHeight;

    XtVaGetValues(textAreaFrame, XmNshadowThickness, &shadowHeight, NULL);
    return shadowHeight;
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position and height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    void
gui_mch_compute_menu_height(id)
    Widget	id;		    /* can be NULL when deleting menu */
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    vimmenu_T	*mp;
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
	for (mp = root_menu; mp != NULL; mp = mp->next)
	    if (mp->id != (Widget)0 && menu_is_menubar(mp->name))
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
    for (mp = root_menu; mp != NULL; mp = mp->next)
    {
	if (mp->id != (Widget)0 && menu_is_menubar(mp->name))
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
     * Add 4 for the underlining of shortcut keys.
     */
    gui.menu_height = maxy + height - 2 * shadow + 2 * margin + 4;

    /* Somehow the menu bar doesn't resize automatically.  Set it here,
     * even though this is a catch 22.  Don't do this when starting up,
     * somehow the menu gets very high then. */
    if (gui.shell_created)
	XtVaSetValues(menuBar, XmNheight, gui.menu_height, NULL);
}

    void
gui_mch_add_menu_item(menu, idx)
    vimmenu_T	*menu;
    int		idx;
{
    XmString	label;
    vimmenu_T	*parent = menu->parent;

# ifdef EBCDIC
    menu->mnemonic = 0;
# endif

# if (XmVersion <= 1002)
    /* Don't add Popup menu items when the popup menu isn't used. */
    if (menu_is_child_of_popup(menu) && !mouse_model_popup())
	return;
# endif

# ifdef FEAT_TOOLBAR
    if (menu_is_toolbar(parent->name))
    {
	WidgetClass	type;
	XmString	xms;	    /* fallback label if pixmap not found */
	int		n;
	Arg		args[15];

	n = 0;
	if (menu_is_separator(menu->name))
	{
	    char	*cp;
	    Dimension	wid;

	    /*
	     * A separator has the format "-sep%d[:%d]-". The optional :%d is
	     * a width specifier. If no width is specified then we choose one.
	     */
	    cp = (char *) vim_strchr(menu->name, ':');
	    if (cp != NULL)
		wid = (Dimension) atoi(++cp);
	    else
		wid = 5;

	    type = xmDrawingAreaWidgetClass;
	    XtSetArg(args[n], XmNwidth, wid); n++;
	    XtSetArg(args[n], XmNmappedWhenManaged, False); n++;
	}
	else
	{
	    Pixmap pixmap = 0;
	    Pixmap insensitive = 0;

	    if (strstr((const char *)p_toolbar, "icons") != NULL)
		get_pixmap(menu->name, &pixmap, &insensitive);
	    if (pixmap == 0)
	    {
		xms = XmStringCreate((char *)menu->dname, STRING_TAG);
		XtSetArg(args[n], XmNlabelString, xms); n++;
		XtSetArg(args[n], XmNlabelType, XmSTRING); n++;
	    }
	    else
	    {
		XtSetArg(args[n], XmNlabelPixmap, pixmap); n++;
		XtSetArg(args[n], XmNlabelInsensitivePixmap, insensitive); n++;
		XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
#ifndef FEAT_SUN_WORKSHOP
		XtSetArg(args[n], XmNshadowThickness, 0); n++;
		XtSetArg(args[n], XmNmarginWidth, 0); n++;
		XtSetArg(args[n], XmNmarginHeight, 0); n++;
#endif
	    }
	    type = xmPushButtonWidgetClass;
	    XtSetArg(args[n], XmNwidth, 80); n++;
	}

	XtSetArg(args[n], XmNpositionIndex, idx); n++;
	if (menu->id == NULL)
	{
	    menu->id = XtCreateManagedWidget((char *)menu->dname,
			type, toolBar, args, n);
	    if (menu->id != NULL && type == xmPushButtonWidgetClass)
	    {
		XtAddCallback(menu->id,
			XmNactivateCallback, gui_x11_menu_cb, menu);

		XtAddEventHandler(menu->id, EnterWindowMask, False,
			toolbarbutton_enter_cb, menu);
		XtAddEventHandler(menu->id, LeaveWindowMask, False,
			toolbarbutton_leave_cb, menu);
#ifdef FEAT_BEVAL
		if (menu->strings[MENU_INDEX_TIP] && menu->tip == NULL)
		    menu->tip = gui_mch_create_beval_area(
					menu->id,
					menu->strings[MENU_INDEX_TIP],
					NULL,
					NULL);
#endif
	    }
	}
	else
	    XtSetValues(menu->id, args, n);

	menu->parent = parent;
	menu->submenu_id = NULL;
	/* When adding first item to toolbar it might have to be enabled .*/
	if (!XtIsManaged(XtParent(toolBar))
		    && vim_strchr(p_go, GO_TOOLBAR) != NULL)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_height = gui_mch_compute_toolbar_height();
	return;
    } /* toolbar menu item */
# endif

    /* No parent, must be a non-menubar menu */
    if (parent->submenu_id == (Widget)0)
	return;

    menu->submenu_id = (Widget)0;

    /* Add menu separator */
    if (menu_is_separator(menu->name))
    {
	menu->id = XtVaCreateWidget("subMenu",
		xmSeparatorWidgetClass, parent->submenu_id,
#if (XmVersion >= 1002)
		/* count the tearoff item (neede for LessTif) */
		XmNpositionIndex, idx + (tearoff_val == (int)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
#endif
		NULL);
	gui_motif_menu_colors(menu->id);
	return;
    }

    label = XmStringCreate((char *)menu->dname, STRING_TAG);
    if (label == NULL)
	return;
    menu->id = XtVaCreateWidget("subMenu",
	xmPushButtonWidgetClass, parent->submenu_id,
	XmNlabelString, label,
	XmNmnemonic, menu->mnemonic,
#if (XmVersion >= 1002)
	/* count the tearoff item (neede for LessTif) */
	XmNpositionIndex, idx + (tearoff_val == (int)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
#endif
	NULL);
    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);
    XmStringFree(label);

    if (menu->id != (Widget)0)
    {
	XtAddCallback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
		(XtPointer)menu);
	/* add accelerator text */
	gui_motif_add_actext(menu);
    }
}

#if (XmVersion <= 1002)
/*
 * This function will destroy/create the popup menus dynamically,
 * according to the value of 'mousemodel'.
 * This will fix the "right mouse button freeze" that occurs when
 * there exists a popup menu but it isn't managed.
 */
    void
gui_motif_update_mousemodel(menu)
    vimmenu_T	*menu;
{
    int		idx = 0;

    /* When GUI hasn't started the menus have not been created. */
    if (!gui.in_use)
      return;

    while (menu)
    {
      if (menu->children != NULL)
      {
	  if (menu_is_popup(menu->name))
	  {
	      if (mouse_model_popup())
	      {
		  /* Popup menu will be used.  Create the popup menus. */
		  gui_mch_add_menu(menu, idx);
		  gui_motif_update_mousemodel(menu->children);
	      }
	      else
	      {
		  /* Popup menu will not be used.  Destroy the popup menus. */
		  gui_motif_update_mousemodel(menu->children);
		  gui_mch_destroy_menu(menu);
	      }
	  }
      }
      else if (menu_is_child_of_popup(menu))
      {
	  if (mouse_model_popup())
	      gui_mch_add_menu_item(menu, idx);
	  else
	      gui_mch_destroy_menu(menu);
      }
      menu = menu->next;
      ++idx;
    }
}
#endif

    void
gui_mch_new_menu_colors()
{
    if (menuBar == (Widget)0)
	return;
    gui_motif_menu_colors(menuBar);
#ifdef FEAT_TOOLBAR
    gui_motif_menu_colors(toolBarFrame);
    gui_motif_menu_colors(toolBar);
#endif

    gui_mch_submenu_change(root_menu, TRUE);
}

    void
gui_mch_new_menu_font()
{
    if (menuBar == (Widget)0)
	return;
    gui_mch_submenu_change(root_menu, FALSE);
    if (!gui.menu_height_fixed)
    {
	Dimension   height;

	XtVaGetValues(menuBar, XmNheight, &height, NULL);
	gui.menu_height = height;
    }
    {
	Position w, h;

	XtVaGetValues(vimShell, XtNwidth, &w, XtNheight, &h, NULL);
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
		gui_motif_menu_colors(mp->id);
	    else
		gui_motif_menu_fontlist(mp->id);
	}

	if (mp->children != NULL)
	{
#if (XmVersion >= 1002)
	    /* Set the colors/font for the tear off widget */
	    if (mp->submenu_id != (Widget)0)
	    {
		if (colors)
		    gui_motif_menu_colors(mp->submenu_id);
		else
		    gui_motif_menu_fontlist(mp->submenu_id);
		toggle_tearoff(mp->submenu_id);
	    }
#endif
	    /* Set the colors for the children */
	    gui_mch_submenu_change(mp->children, colors);
	}
	mp = mp->next;
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    void
gui_mch_destroy_menu(menu)
    vimmenu_T	*menu;
{
    Widget	parent;
    int		num_children;


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
	if (parent != menuBar
#ifdef FEAT_TOOLBAR
		&& parent != toolBar
#endif
	   )
	{
	    XtVaGetValues(parent, XtNnumChildren, &num_children, NULL);
	    if (num_children > 1)
		XtDestroyWidget(menu->id);
	}
	else
	    XtDestroyWidget(menu->id);
	menu->id = (Widget)0;

	if (parent == menuBar)
	    gui_mch_compute_menu_height((Widget)0);
#ifdef FEAT_TOOLBAR
	else if (parent == toolBar)
	{
# ifdef FEAT_BEVAL
	    if (menu->tip != NULL)
	    {
		gui_mch_destroy_beval_area(menu->tip);
		menu->tip = NULL;
	    }
# endif
	    /* When removing last toolbar item, don't display the toolbar. */
	    XtVaGetValues(toolBar, XmNnumChildren, &num_children, NULL);
	    if (num_children == 0)
		gui_mch_show_toolbar(FALSE);
	    else
		gui.toolbar_height = gui_mch_compute_toolbar_height();
	}
#endif
    }
}

/* ARGSUSED */
    void
gui_mch_show_popupmenu(menu)
    vimmenu_T *menu;
{
#ifdef MOTIF_POPUP
    XmMenuPosition(menu->submenu_id, gui_x11_get_last_mouse_event());
    XtManageChild(menu->submenu_id);
#endif
}

#endif /* FEAT_MENU */


/*
 * Scrollbar stuff.
 */

    void
gui_mch_set_scrollbar_thumb(sb, val, size, max)
    scrollbar_T *sb;
    long	val;
    long	size;
    long	max;
{
    if (sb->id != (Widget)0)
	XtVaSetValues(sb->id,
		  XmNvalue, val,
		  XmNsliderSize, size,
		  XmNpageIncrement, (size > 2 ? size - 2 : 1),
		  XmNmaximum, max + 1,	    /* Motif has max one past the end */
		  NULL);
}

    void
gui_mch_set_scrollbar_pos(sb, x, y, w, h)
    scrollbar_T *sb;
    int		x;
    int		y;
    int		w;
    int		h;
{
    if (sb->id != (Widget)0)
    {
	if (sb->type == SBAR_LEFT || sb->type == SBAR_RIGHT)
	{
	    if (y == 0)
		h -= gui.border_offset;
	    else
		y -= gui.border_offset;
	    XtVaSetValues(sb->id,
			  XmNtopOffset, y,
			  XmNbottomOffset, -y - h,
			  XmNwidth, w,
			  NULL);
	}
	else
	    XtVaSetValues(sb->id,
			  XmNtopOffset, y,
			  XmNleftOffset, x,
			  XmNrightOffset, gui.scrollbar_width,
			  XmNheight, h,
			  NULL);
	XtManageChild(sb->id);
    }
}

    void
gui_mch_enable_scrollbar(sb, flag)
    scrollbar_T *sb;
    int		flag;
{
    Arg		args[16];
    int		n;

    if (sb->id != (Widget)0)
    {
	n = 0;
	if (flag)
	{
	    switch (sb->type)
	    {
		case SBAR_LEFT:
		    XtSetArg(args[n], XmNleftOffset, gui.scrollbar_width); n++;
		    break;

		case SBAR_RIGHT:
		    XtSetArg(args[n], XmNrightOffset, gui.scrollbar_width); n++;
		    break;

		case SBAR_BOTTOM:
		    XtSetArg(args[n], XmNbottomOffset, gui.scrollbar_height);n++;
		    break;
	    }
	    XtSetValues(textArea, args, n);
	    XtManageChild(sb->id);
	}
	else
	{
	    if (!gui.which_scrollbars[sb->type])
	    {
		/* The scrollbars of this type are all disabled, adjust the
		 * textArea attachment offset. */
		switch (sb->type)
		{
		    case SBAR_LEFT:
			XtSetArg(args[n], XmNleftOffset, 0); n++;
			break;

		    case SBAR_RIGHT:
			XtSetArg(args[n], XmNrightOffset, 0); n++;
			break;

		    case SBAR_BOTTOM:
			XtSetArg(args[n], XmNbottomOffset, 0);n++;
			break;
		}
		XtSetValues(textArea, args, n);
	    }
	    XtUnmanageChild(sb->id);
	}
    }
}

    void
gui_mch_create_scrollbar(sb, orient)
    scrollbar_T *sb;
    int		orient;	/* SBAR_VERT or SBAR_HORIZ */
{
    Arg		args[16];
    int		n;

    n = 0;
    XtSetArg(args[n], XmNshadowThickness, 1); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation,
	    (orient == SBAR_VERT) ? XmVERTICAL : XmHORIZONTAL); n++;

    switch (sb->type)
    {
	case SBAR_LEFT:
	    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    break;

	case SBAR_RIGHT:
	    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    break;

	case SBAR_BOTTOM:
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    break;
    }

    sb->id = XtCreateWidget("scrollBar",
	    xmScrollBarWidgetClass, textAreaForm, args, n);

    if (sb->id != (Widget)0)
    {
	gui_mch_set_scrollbar_colors(sb);
	XtAddCallback(sb->id, XmNvalueChangedCallback,
		      scroll_cb, (XtPointer)sb->ident);
	XtAddCallback(sb->id, XmNdragCallback,
		      scroll_cb, (XtPointer)sb->ident);
	XtAddEventHandler(sb->id, KeyPressMask, FALSE, gui_x11_key_hit_cb,
	    (XtPointer)0);
    }
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
    {
	if (gui.scroll_bg_pixel != -1)
	{
#if (XmVersion>=1002)
	    XmChangeColor(sb->id, gui.scroll_bg_pixel);
#else
	    XtVaSetValues(sb->id,
		    XmNtroughColor, gui.scroll_bg_pixel,
		    NULL);
#endif
	}

	if (gui.scroll_fg_pixel != -1)
	    XtVaSetValues(sb->id,
		    XmNforeground, gui.scroll_fg_pixel,
#if (XmVersion<1002)
		    XmNbackground, gui.scroll_fg_pixel,
#endif
		    NULL);
    }

    /* This is needed for the rectangle below the vertical scrollbars. */
    if (sb == &gui.bottom_sbar && textAreaForm != (Widget)0)
    {
	gui_motif_scroll_colors(textAreaForm);
	gui_motif_scroll_colors(textAreaFrame);
    }
}

/*
 * Miscellaneous stuff:
 */

    Window
gui_x11_get_wid()
{
    return(XtWindow(textArea));
}

#if defined(FEAT_BROWSE) || defined(PROTO)

/*
 * file selector related stuff
 */

#include <Xm/FileSB.h>
#include <Xm/XmStrDefs.h>

typedef struct dialog_callback_arg
{
    char *  args;   /* not used right now */
    int	    id;
} dcbarg_T;

static Widget dialog_wgt;
static char *browse_fname = NULL;
static XmStringCharSet charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;
				/* used to set up XmStrings */

static void DialogCancelCB __ARGS((Widget, XtPointer, XtPointer));
static void DialogAcceptCB __ARGS((Widget, XtPointer, XtPointer));

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
/* ARGSUSED */
    char_u *
gui_mch_browse(saving, title, dflt, ext, initdir, filter)
    int		saving;		/* select file to write */
    char_u	*title;		/* title for the window */
    char_u	*dflt;		/* default name */
    char_u	*ext;		/* not used (extension added) */
    char_u	*initdir;	/* initial directory, NULL for current dir */
    char_u	*filter;	/* not used (file name filter) */
{
    char_u	dirbuf[MAXPATHL];

    dialog_wgt = XmCreateFileSelectionDialog(vimShell, (char *)title, NULL, 0);

    if (dflt == NULL)
	dflt = (char_u *)"";
    if (initdir == NULL || *initdir == NUL)
    {
	mch_dirname(dirbuf, MAXPATHL);
	initdir = dirbuf;
    }
    if (filter == NULL)
	filter = (char_u *)"";	/* An empty pattern matches all files */

    XtVaSetValues(dialog_wgt,
	XtVaTypedArg,
	    XmNdirectory,   XmRString,	(char *)initdir, STRLEN(initdir) + 1,
	XtVaTypedArg,
	    XmNdirSpec,	    XmRString,  (char *)dflt,	STRLEN(dflt) + 1,
	XtVaTypedArg,
	    XmNpattern,	    XmRString,  (char *)filter,	STRLEN(filter) + 1,
	XtVaTypedArg,
	    XmNdialogTitle, XmRString,  (char *)title,	STRLEN(title) + 1,
/*
    currently, the background color of the input and selection
    fields are "motif blue".  i'm sure there must be a resource
    that corresponds to this, but i don't know what it is.

	XmNhighlightColor,	gui.norm_pixel,
	XmNborderColor,		gui.back_pixel,
	XmNtopShadowColor,	gui.menu_bg_pixel,
	XmNbottomShadowColor,	gui.norm_pixel,
*/
	NULL);
    gui_motif_menu_colors(dialog_wgt);
    if (gui.scroll_bg_pixel != -1)
	XtVaSetValues(dialog_wgt, XmNtroughColor, gui.scroll_bg_pixel, NULL);

    XtAddCallback(dialog_wgt, XmNokCallback, DialogAcceptCB, (XtPointer)0);
    XtAddCallback(dialog_wgt, XmNcancelCallback, DialogCancelCB, (XtPointer)0);
    /* We have no help in this window, so hide help button */
    XtUnmanageChild(XmFileSelectionBoxGetChild(dialog_wgt,
					(unsigned char)XmDIALOG_HELP_BUTTON));

    XtManageChild(dialog_wgt);

    /* sit in a loop until the dialog box has gone away */
    do
    {
	XtAppProcessEvent(XtWidgetToApplicationContext(dialog_wgt),
	    (XtInputMask)XtIMAll);
    } while (XtIsManaged(dialog_wgt));

    XtDestroyWidget(dialog_wgt);

    if (browse_fname == NULL)
	return NULL;
    return vim_strsave((char_u *)browse_fname);
}

/*
 * The code below was originally taken from
 *	/usr/examples/motif/xmsamplers/xmeditor.c
 * on Digital Unix 4.0d, but heavily modified.
 */

/*
 * Process callback from Dialog cancel actions.
 */
/* ARGSUSED */
    static void
DialogCancelCB(w, client_data, call_data)
    Widget	w;		/*  widget id		*/
    XtPointer	client_data;	/*  data from application   */
    XtPointer	call_data;	/*  data from widget class  */
{
    if (browse_fname != NULL)
    {
	XtFree(browse_fname);
	browse_fname = NULL;
    }
    XtUnmanageChild(dialog_wgt);
}

/*
 * Process callback from Dialog actions.
 */
/* ARGSUSED */
    static void
DialogAcceptCB(w, client_data, call_data)
    Widget	w;		/*  widget id		*/
    XtPointer	client_data;	/*  data from application   */
    XtPointer	call_data;	/*  data from widget class  */
{
    XmFileSelectionBoxCallbackStruct *fcb;

    if (browse_fname != NULL)
    {
	XtFree(browse_fname);
	browse_fname = NULL;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_data;

    /* get the filename from the file selection box */
    XmStringGetLtoR(fcb->value, charset, &browse_fname);

    /* popdown the file selection box */
    XtUnmanageChild(dialog_wgt);
}

#endif /* FEAT_BROWSE */

#if defined(FEAT_GUI_DIALOG) || defined(PROTO)

static int	dialogStatus;

static void butproc __ARGS((Widget w, XtPointer client_data, XtPointer call_data));

/* ARGSUSED */
    static void
butproc(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data;
    XtPointer	call_data;
{
    dialogStatus = (int)(long)client_data + 1;
}

/* ARGSUSED */
    int
gui_mch_dialog(type, title, message, buttons, dfltbutton)
    int		type;
    char_u	*title;
    char_u	*message;
    char_u	*buttons;
    int		dfltbutton;
{
    char_u		*buts;
    char_u		*p, *next;
    XtAppContext	app;
    XmString		label;
    int			butcount;
    static Widget	dialogbb = NULL;
    static Widget	dialogmessage = NULL;
    Widget		*dialogButton;
    int			vertical;

    if (title == NULL)
	title = (char_u *)_("Vim dialog");

    /* if our pointer is currently hidden, then we should show it. */
    gui_mch_mousehide(FALSE);

    /* First time called: create the dialog and the message */
    if (dialogbb == NULL)
    {
	dialogbb = XmCreateFormDialog(vimShell,
		(char *)"dialog", NULL, 0);

	dialogmessage = XtVaCreateManagedWidget("dialogMessage",
		xmLabelWidgetClass, dialogbb,
		XmNtopAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		NULL);
    }

    /* Set the message string */
    label = XmStringLtoRCreate((char *)message, STRING_TAG);
    if (label == NULL)
	return -1;
    XtVaSetValues(dialogmessage,
	    XmNlabelString, label,
	    NULL);
    XmStringFree(label);

    /* Check 'v' flag in 'guioptions': vertical button placement. */
    vertical = (vim_strchr(p_go, GO_VERTICAL) != NULL);

    /* Set the title of the Dialog window */
    label = XmStringCreateSimple((char *)title);
    if (label == NULL)
	return -1;
    XtVaSetValues(dialogbb,
	    XmNdialogTitle, label,
	    XmNhorizontalSpacing, 20,
	    XmNverticalSpacing, vertical ? 0 : 20,
	    NULL);
    XmStringFree(label);

    /* make a copy, so that we can insert NULs */
    buts = vim_strsave(buttons);
    if (buts == NULL)
	return -1;

    /* Count the number of buttons and allocate dialogButton[]. */
    butcount = 1;
    for (p = buts; *p; ++p)
	if (*p == DLG_BUTTON_SEP)
	    ++butcount;
    dialogButton = (Widget *)alloc((unsigned)(butcount * sizeof(Widget)));
    if (dialogButton == NULL)
    {
	vim_free(buts);
	return -1;
    }

    /*
     * Create the buttons.
     */
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
	label = XmStringCreate((char *)p, STRING_TAG);
	if (label == NULL)
	    break;

	dialogButton[butcount] = XtVaCreateManagedWidget("button",
		xmPushButtonWidgetClass, dialogbb,
		XmNlabelString, label,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, dialogmessage,
		NULL);
	XmStringFree(label);
	if (butcount)
	{
	    if (vertical)
		XtVaSetValues(dialogButton[butcount],
			XmNtopWidget, dialogButton[butcount - 1],
			NULL);
	    else
		XtVaSetValues(dialogButton[butcount],
			XmNleftAttachment, XmATTACH_WIDGET,
			XmNleftWidget, dialogButton[butcount - 1],
			NULL);
	}

	XtAddCallback(dialogButton[butcount], XmNactivateCallback,
			  (XtCallbackProc)butproc, (XtPointer)(long)butcount);
	p = next;
    }
    vim_free(buts);

    if (dfltbutton < 1)
	dfltbutton = 1;
    if (dfltbutton > butcount)
	dfltbutton = butcount;
    XtVaSetValues(dialogbb,
	    XmNdefaultButton, dialogButton[dfltbutton - 1], NULL);
    XtManageChild(dialogbb);

    app = XtWidgetToApplicationContext(dialogbb);

    dialogStatus = -1;

    /* Loop until a button is pressed or the dialog is killed somehow. */
    while (1)
    {
	XtAppProcessEvent(app, (XtInputMask)XtIMAll);
	if (dialogStatus >= 0 || !XtIsManaged(dialogbb))
	    break;
    }

    XtUnmanageChild(dialogbb);

    while (butcount)
    {
	--butcount;
	XtUnmanageChild(dialogButton[butcount]);
	XtDestroyWidget(dialogButton[butcount]);
    }

    vim_free(dialogButton);

    return dialogStatus;
}
#endif /* FEAT_GUI_DIALOG */

#if defined(FEAT_FOOTER) || defined(PROTO)

    static int
gui_mch_compute_footer_height()
{
    Dimension	height;		    /* total Toolbar height */
    Dimension	top;		    /* XmNmarginTop */
    Dimension	bottom;		    /* XmNmarginBottom */
    Dimension	shadow;		    /* XmNshadowThickness */

    XtVaGetValues(footer,
	    XmNheight, &height,
	    XmNmarginTop, &top,
	    XmNmarginBottom, &bottom,
	    XmNshadowThickness, &shadow,
	    NULL);

    return (int) height + top + bottom + (shadow << 1);
}


    void
gui_mch_set_footer_pos(h)
    int	    h;			    /* textArea height */
{
    XtVaSetValues(footer,
		  XmNtopOffset, h + 7,
		  NULL);
}

    void
gui_mch_enable_footer(showit)
    int		showit;
{
    if (showit)
    {
	gui.footer_height = gui_mch_compute_footer_height();
	XtManageChild(footer);
    }
    else
    {
	gui.footer_height = 0;
	XtUnmanageChild(footer);
    }
    XtVaSetValues(textAreaFrame, XmNbottomOffset, gui.footer_height, NULL);
}

    void
gui_mch_set_footer(char_u *msg)
{
    XmString	xms;

    xms = XmStringCreate((char *)msg, STRING_TAG);
    XtVaSetValues(footer, XmNlabelString, xms, NULL);
    XmStringFree(xms);
}

#endif


#if defined(FEAT_TOOLBAR) || defined(PROTO)
    void
gui_mch_show_toolbar(int showit)
{
    Cardinal	numChildren;	    /* how many children toolBar has */

    XtVaGetValues(toolBar, XmNnumChildren, &numChildren, NULL);
    if (showit && numChildren > 0)
    {
	gui.toolbar_height = gui_mch_compute_toolbar_height();
	XtManageChild(XtParent(toolBar));
	XtVaSetValues(textAreaFrame,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, XtParent(toolBar),
		NULL);
	if (XtIsManaged(menuBar))
	    XtVaSetValues(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, menuBar,
		    NULL);
	else
	    XtVaSetValues(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_FORM,
		    NULL);
    }
    else
    {
	gui.toolbar_height = 0;
	if (XtIsManaged(menuBar))
	    XtVaSetValues(textAreaFrame,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		NULL);
	else
	    XtVaSetValues(textAreaFrame,
		XmNtopAttachment, XmATTACH_FORM,
		NULL);

	XtUnmanageChild(XtParent(toolBar));
    }
}

/*
 * A toolbar button has been pushed; now reset the input focus
 * such that the user can type page up/down etc. and have the
 * input go to the editor window, not the button
 */
    void
gui_mch_reset_focus()
{
    if (textArea != NULL)
	XmProcessTraversal(textArea, XmTRAVERSE_CURRENT);
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
    if (toolBar != (Widget)0 && toolBarFrame != (Widget)0)
    {				    /* get height of XmFrame parent */
	XtVaGetValues(toolBarFrame,
		XmNshadowThickness, &shadowThickness,
		NULL);
	XtVaGetValues(toolBar,
		XmNmarginHeight, &marginHeight,
		XmNchildren, &children,
		XmNnumChildren, &numChildren, NULL);
	for (i = 0; i < numChildren; i++)
	{
	    whgt = 0;
	    XtVaGetValues(children[i], XmNheight, &whgt, NULL);
	    if (height < whgt)
		height = whgt;
	}
    }

    return (int)(height + (marginHeight << 1) + (shadowThickness << 1));
}


/*
 * The next toolbar enter/leave callbacks make sure the text area gets the
 * keyboard focus when the pointer is not in the toolbar.
 */
/*ARGSUSED*/
    static void
toolbar_enter_cb(w, client_data, event, cont)
    Widget	w;
    XtPointer	client_data;
    XEvent	*event;
    Boolean	*cont;
{
    XmProcessTraversal(toolBar, XmTRAVERSE_CURRENT);
}

/*ARGSUSED*/
    static void
toolbar_leave_cb(w, client_data, event, cont)
    Widget	w;
    XtPointer	client_data;
    XEvent	*event;
    Boolean	*cont;
{
    XmProcessTraversal(textArea, XmTRAVERSE_CURRENT);
}

/*
 * The next toolbar enter/leave callbacks should really do balloon help.  But
 * I have to use footer help for backwards compatability.  Hopefully both will
 * get implemented and the user will have a choice.
 */
/*ARGSUSED*/
    static void
toolbarbutton_enter_cb(w, client_data, event, cont)
    Widget	w;
    XtPointer	client_data;
    XEvent	*event;
    Boolean	*cont;
{
    vimmenu_T	*menu = (vimmenu_T *) client_data;

    if (menu->strings[MENU_INDEX_TIP] != NULL)
    {
# ifdef FEAT_FOOTER
	if (vim_strchr(p_go, GO_FOOTER) != NULL)
	    gui_mch_set_footer(menu->strings[MENU_INDEX_TIP]);
# endif
    }
}

/*ARGSUSED*/
    static void
toolbarbutton_leave_cb(w, client_data, event, cont)
    Widget	w;
    XtPointer	client_data;
    XEvent	*event;
    Boolean	*cont;
{
# ifdef FEAT_FOOTER
    gui_mch_set_footer((char_u *) "");
# endif
}

    void
gui_mch_get_toolbar_colors(bgp, fgp)
    Pixel	*bgp;
    Pixel	*fgp;
{
    XtVaGetValues(toolBar, XmNbackground, bgp, XmNforeground, fgp, NULL);
}
#endif

/*
 * Set the colors of Widget "id" to the menu colors.
 */
    static void
gui_motif_menu_colors(id)
    Widget  id;
{
    if (gui.menu_bg_pixel != -1)
#if (XmVersion >= 1002)
	XmChangeColor(id, gui.menu_bg_pixel);
#else
	XtVaSetValues(id, XmNbackground, gui.menu_bg_pixel, NULL);
#endif
    if (gui.menu_fg_pixel != -1)
	XtVaSetValues(id, XmNforeground, gui.menu_fg_pixel, NULL);
}

/*
 * Set the colors of Widget "id" to the scrollbar colors.
 */
    static void
gui_motif_scroll_colors(id)
    Widget  id;
{
    if (gui.scroll_bg_pixel != -1)
#if (XmVersion >= 1002)
	XmChangeColor(id, gui.scroll_bg_pixel);
#else
	XtVaSetValues(id, XmNbackground, gui.scroll_bg_pixel, NULL);
#endif
    if (gui.scroll_fg_pixel != -1)
	XtVaSetValues(id, XmNforeground, gui.scroll_fg_pixel, NULL);
}

#ifdef FEAT_MENU
/*
 * Set the fontlist for Widget "id" to use gui.menu_font.
 */
    static void
gui_motif_menu_fontlist(id)
    Widget  id;
{
    if (gui.menu_font != NOFONT)
    {
	XmFontList fl;

	fl = gui_motif_create_fontlist((XFontStruct *)gui.menu_font);
	if (fl != NULL)
	{
	    if (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		XtVaSetValues(id, XmNfontList, fl, NULL);
		/* We should force the widget to recalculate it's
		 * geometry now. */
		XtManageChild(id);
	    }
	    else
		XtVaSetValues(id, XmNfontList, fl, NULL);
	    XmFontListFree(fl);
	}
    }
}
#endif
