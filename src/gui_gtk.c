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

#ifdef FEAT_GUI_GTK
# include "gui_gtk_f.h"
#endif

#ifdef MIN
# undef MIN
#endif
#ifdef MAX
# undef MAX
#endif

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
#endif

#if defined(FEAT_GUI_DIALOG)
# include "../pixmaps/alert.xpm"
# include "../pixmaps/error.xpm"
# include "../pixmaps/generic.xpm"
# include "../pixmaps/info.xpm"
# include "../pixmaps/quest.xpm"
#endif

#ifdef FEAT_TOOLBAR
/*
 * Icons used by the toolbar code.
 */
#include "../pixmaps/tb_new.xpm"
#include "../pixmaps/tb_open.xpm"
#include "../pixmaps/tb_close.xpm"
#include "../pixmaps/tb_save.xpm"
#include "../pixmaps/tb_print.xpm"
#include "../pixmaps/tb_cut.xpm"
#include "../pixmaps/tb_copy.xpm"
#include "../pixmaps/tb_paste.xpm"
#include "../pixmaps/tb_find.xpm"
#include "../pixmaps/tb_find_next.xpm"
#include "../pixmaps/tb_find_prev.xpm"
#include "../pixmaps/tb_find_help.xpm"
#include "../pixmaps/tb_exit.xpm"
#include "../pixmaps/tb_undo.xpm"
#include "../pixmaps/tb_redo.xpm"
#include "../pixmaps/tb_help.xpm"
#include "../pixmaps/tb_macro.xpm"
#include "../pixmaps/tb_make.xpm"
#include "../pixmaps/tb_save_all.xpm"
#include "../pixmaps/tb_jump.xpm"
#include "../pixmaps/tb_ctags.xpm"
#include "../pixmaps/tb_load_session.xpm"
#include "../pixmaps/tb_save_session.xpm"
#include "../pixmaps/tb_new_session.xpm"
#include "../pixmaps/tb_blank.xpm"
#include "../pixmaps/tb_maximize.xpm"
#include "../pixmaps/tb_split.xpm"
#include "../pixmaps/tb_minimize.xpm"
#include "../pixmaps/tb_shell.xpm"
#include "../pixmaps/tb_replace.xpm"
#include "../pixmaps/tb_vsplit.xpm"
#include "../pixmaps/tb_maxwidth.xpm"
#include "../pixmaps/tb_minwidth.xpm"
#endif

#ifdef FEAT_GUI_GTK
# include <gdk/gdkkeysyms.h>
# include <gdk/gdk.h>
# include <gdk/gdkx.h>

# include <gtk/gtk.h>
#else
/* define these items to be able to generate prototypes without GTK */
typedef int GtkWidget;
# define gpointer int
# define GdkPixmap int
# define GdkBitmap int
# define GtkToolbar int
# define GtkAdjustment int
# define gboolean int
# define GdkEventKey int
# define CancelData int
#endif

static void entry_activate_cb(GtkWidget *widget, GtkWidget *with);
static void entry_changed_cb(GtkWidget *entry, GtkWidget *dialog);
static void find_direction_cb(GtkWidget *widget, gpointer data);
static void find_replace_cb(GtkWidget *widget, unsigned int flags);
static void exact_match_cb(GtkWidget *widget, gpointer data);
static void repl_dir_cb(GtkWidget * widget, gpointer data);

/*
 * Only use accelerators when gtk_menu_ensure_uline_accel_group() is
 * available, which is in version 1.2.1.  That was the first version where
 * accelerators properly worked (according to the change log).
 */
#ifdef GTK_CHECK_VERSION
# if GTK_CHECK_VERSION(1, 2, 1)
#  define GTK_USE_ACCEL
# endif
#endif

#if defined(FEAT_MENU) || defined(PROTO)

/*
 * Create a highly customized menu item by hand instead of by using:
 *
 * gtk_menu_item_new_with_label(menu->dname);
 *
 * This is neccessary, since there is no other way in GTK+ to get the
 * not automatically parsed accellerator stuff right.
 */
/*ARGSUSED*/
    static void
menu_item_new(vimmenu_T *menu, GtkWidget *parent_widget, int sub_menu)
{
    char *name, *tmp;
    GtkWidget *widget;
    GtkWidget *bin, *label;
    int num;
    guint accel_key;

    widget = gtk_widget_new(GTK_TYPE_MENU_ITEM,
			    "GtkWidget::visible", TRUE,
			    "GtkWidget::sensitive", TRUE,
			    /* "GtkWidget::parent", parent->submenu_id, */
			    NULL);
    bin = gtk_widget_new(GTK_TYPE_HBOX,
			 "GtkWidget::visible", TRUE,
			 "GtkWidget::parent", widget,
			 "GtkBox::spacing", 16,
			 NULL);
    label = gtk_widget_new(GTK_TYPE_ACCEL_LABEL,
			   "GtkWidget::visible", TRUE,
			   "GtkWidget::parent", bin,
			   "GtkAccelLabel::accel_widget", widget,
			   "GtkMisc::xalign", 0.0,
			   NULL);
    menu->label = label;

    if (menu->actext)
	gtk_widget_new(GTK_TYPE_LABEL,
		       "GtkWidget::visible", TRUE,
		       "GtkWidget::parent", bin,
		       "GtkLabel::label", menu->actext,
		       "GtkMisc::xalign", 1.0,
			NULL);

    /*
     * Translate VIM accelerator tagging into GTK+'s.  Note that since GTK uses
     * underscores as the accelerator key, we need to add an additional under-
     * score for each understore that appears in the menu name.
     */

    /* First count how many underscore's are in the menu name. */
    for (num = 0, tmp = (char *)menu->name; *tmp; tmp++)
	if (*tmp == '_')
	    num++;

    /*
     * now allocate a new buffer to hold all the menu name along with the
     * additional underscores.
     */
    name = g_new(char, strlen((char *)menu->name) + num + 1);
    for (num = 0, tmp = (char *)menu->name; *tmp; ++tmp)
    {
	/* actext has been added above, stop at the TAB */
	if (*tmp == TAB)
	    break;
	if (*tmp == '&')
	{
# ifdef GTK_USE_ACCEL
	    if (*p_wak != 'n' || !GTK_IS_MENU_BAR(parent_widget))
	    {
		name[num] = '_';
		num++;
	    }
# endif
	}
	else
	{
	    name[num] = *tmp;
	    num++;
	    if (*tmp == '_')
	    {
		name[num] = '_';
		num++;
	    }
	}
    }
    name[num] = '\0';

    /* let GTK do its thing */
    accel_key = gtk_label_parse_uline(GTK_LABEL(label), name);
    g_free(name);

# ifdef GTK_USE_ACCEL
    /* Don't add accelator if 'winaltkeys' is "no". */
    if (accel_key != GDK_VoidSymbol)
    {
	if (GTK_IS_MENU_BAR(parent_widget))
	{
	    if (*p_wak != 'n')
		gtk_widget_add_accelerator(widget,
			"activate_item",
			gui.accel_group,
			accel_key, GDK_MOD1_MASK,
			GTK_ACCEL_LOCKED);
	}
	else
	{
	    gtk_widget_add_accelerator(widget,
		    "activate_item",
		    gtk_menu_ensure_uline_accel_group(GTK_MENU(parent_widget)),
		    accel_key, 0,
		    GTK_ACCEL_LOCKED);
	}
    }
# endif

    menu->id = widget;
}


/*ARGSUSED*/
    void
gui_mch_add_menu(vimmenu_T *menu, int idx)
{
    vimmenu_T	*parent = menu->parent;

    if (menu_is_popup(menu->name))
    {
	menu->submenu_id = gtk_menu_new();
	return;
    }

    if (menu_is_menubar(menu->name) == 0
	    || (parent != NULL && parent->submenu_id == 0))
	return;

    if (parent == NULL)
	menu_item_new(menu, gui.menubar, TRUE);
    else
	menu_item_new(menu, parent->submenu_id, TRUE);

    if (menu->id == NULL)
	return;			/* failed */

    if (parent == NULL)
	gtk_menu_bar_insert(GTK_MENU_BAR(gui.menubar), menu->id, idx);
    else
    {
	/* since the tearoff should always appear first, increment idx */
	++idx;
	gtk_menu_insert(GTK_MENU(parent->submenu_id), menu->id, idx);
    }

    /*
     * The "Help" menu is a special case, and should be placed at the far
     * right hand side of the menu-bar.  It's detected by its high priority.
     */
    if (parent == NULL && menu->priority >= 9999)
	gtk_menu_item_right_justify(GTK_MENU_ITEM(menu->id));

    if ((menu->submenu_id = gtk_menu_new()) == NULL)	/* failed */
	return;

    gtk_menu_set_accel_group(GTK_MENU(menu->submenu_id), gui.accel_group);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu->id), menu->submenu_id);

    menu->tearoff_handle = gtk_tearoff_menu_item_new();
    if (vim_strchr(p_go, GO_TEAROFF) != NULL)
	gtk_widget_show(menu->tearoff_handle);
    gtk_menu_prepend(GTK_MENU(menu->submenu_id), menu->tearoff_handle);
}

/*ARGSUSED*/
    static void
menu_item_activate(GtkWidget * widget, gpointer data)
{
    gui_menu_cb((vimmenu_T *) data);

    /* make sure the menu action is taken immediately */
    if (gtk_main_level() > 0)
	gtk_main_quit();
}

#ifdef FEAT_TOOLBAR
/*
 * These are the pixmaps used for the default buttons.
 * Order must exactly match toolbar_names[] in menu.c!
 */
static char **(built_in_pixmaps[]) =
{
    tb_new_xpm,
    tb_open_xpm,
    tb_save_xpm,
    tb_undo_xpm,
    tb_redo_xpm,
    tb_cut_xpm,
    tb_copy_xpm,
    tb_paste_xpm,
    tb_print_xpm,
    tb_help_xpm,
    tb_find_xpm,
    tb_save_all_xpm,
    tb_save_session_xpm,
    tb_new_session_xpm,
    tb_load_session_xpm,
    tb_macro_xpm,
    tb_replace_xpm,
    tb_close_xpm,
    tb_maximize_xpm,
    tb_minimize_xpm,
    tb_split_xpm,
    tb_shell_xpm,
    tb_find_prev_xpm,
    tb_find_next_xpm,
    tb_find_help_xpm,
    tb_make_xpm,
    tb_jump_xpm,
    tb_ctags_xpm,
    tb_vsplit_xpm,
    tb_maxwidth_xpm,
    tb_minwidth_xpm,
    tb_exit_xpm
};

/*
 * creates a blank pixmap using tb_blank
 */
    static void
pixmap_create_from_xpm(char **xpm, GdkPixmap **pixmap, GdkBitmap **mask)
{
    *pixmap = gdk_pixmap_colormap_create_from_xpm_d(
	    NULL,
	    gtk_widget_get_colormap(gui.mainwin),
	    mask,
	    NULL,
	    xpm);
}

/*
 * creates a pixmap by using a built-in number
 */
    static void
pixmap_create_by_num(int pixmap_num, GdkPixmap **pixmap, GdkBitmap **mask)
{
    if (pixmap_num >= 0 && pixmap_num < (sizeof(built_in_pixmaps)
					   / sizeof(built_in_pixmaps[0])) - 1)
	pixmap_create_from_xpm(built_in_pixmaps[pixmap_num], pixmap, mask);
}

/*
 * Creates a pixmap by using the pixmap "name" found in 'runtimepath'/bitmaps/
 */
    static void
pixmap_create_by_dir(char_u *name, GdkPixmap **pixmap, GdkBitmap **mask)
{
    char_u full_pathname[MAXPATHL + 1];

    if (gui_find_bitmap(name, full_pathname, "xpm") == OK)
	*pixmap = gdk_pixmap_colormap_create_from_xpm(
		NULL,
		gtk_widget_get_colormap(gui.mainwin),
		mask,
		&gui.mainwin->style->bg[GTK_STATE_NORMAL],
		(const char *)full_pathname);
}

/*
 * Creates a pixmap by using the pixmap "file".
 */
    static void
pixmap_create_from_file(char_u *file, GdkPixmap **pixmap, GdkBitmap **mask)
{
    char_u full_pathname[MAXPATHL + 1];

    expand_env(file, full_pathname, MAXPATHL);
    *pixmap = gdk_pixmap_colormap_create_from_xpm(
		NULL,
		gtk_widget_get_colormap(gui.mainwin),
		mask,
		&gui.mainwin->style->bg[GTK_STATE_NORMAL],
		(const char *)full_pathname);
}
#endif

/*ARGSUSED*/
    void
gui_mch_add_menu_item(vimmenu_T *menu, int idx)
{
    vimmenu_T	*parent = menu->parent;

# ifdef FEAT_TOOLBAR
    if (menu_is_toolbar(parent->name))
    {
	if (menu_is_separator(menu->name))
	{
	    gtk_toolbar_insert_space(GTK_TOOLBAR(gui.toolbar), idx);
	}
	else
	{
	    GdkPixmap *pixmap = NULL;
	    GdkBitmap *mask = NULL;

	    /* First try user specified bitmap, then builtin, the a blank. */
	    if (menu->iconfile != NULL)
		pixmap_create_from_file(menu->iconfile, &pixmap, &mask);
	    if (pixmap == NULL && !menu->icon_builtin)
		pixmap_create_by_dir(menu->name, &pixmap, &mask);
	    if (pixmap == NULL && menu->iconidx >= 0)
		pixmap_create_by_num(menu->iconidx, &pixmap, &mask);
	    if (pixmap == NULL)
		pixmap_create_from_xpm(tb_blank_xpm, &pixmap, &mask);
	    if (pixmap == NULL)
		return; /* should at least have blank pixmap, but if not... */

	    menu->id = gtk_toolbar_insert_item(
				    GTK_TOOLBAR(gui.toolbar),
				    (char *)(menu->dname),
				    (char *)(menu->strings[MENU_INDEX_TIP]),
				    (char *)(menu->dname),
				    gtk_pixmap_new(pixmap, mask),
				    GTK_SIGNAL_FUNC(menu_item_activate),
				    (gpointer)menu,
				    idx);
	}
	menu->parent = parent;
	menu->submenu_id = NULL;
	return;
    } /* toolbar menu item */
# endif

    /* No parent, must be a non-menubar menu */
    if (parent->submenu_id == 0)
	return;

    /* make place for the possible tearoff handle item */
    ++idx;
    if (menu_is_separator(menu->name))
    {
	/* Separator: Just add it */
	menu->id = gtk_menu_item_new();
	gtk_widget_set_sensitive(menu->id, FALSE);
	gtk_widget_show(menu->id);
	gtk_menu_insert(GTK_MENU(parent->submenu_id), menu->id, idx);

	return;
    }

    /* Add textual menu item. */
    menu_item_new(menu, parent->submenu_id, FALSE);
    gtk_widget_show(menu->id);
    gtk_menu_insert(GTK_MENU(parent->submenu_id), menu->id, idx);

    if (menu->id != 0)
	gtk_signal_connect(GTK_OBJECT(menu->id), "activate",
		GTK_SIGNAL_FUNC(menu_item_activate), (gpointer) menu);
}
#endif


    void
gui_mch_set_text_area_pos(int x, int y, int w, int h)
{
    gtk_form_move_resize(GTK_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}


#if defined(FEAT_MENU) || defined(PROTO)
/*
 * Enable or disable accelators for the toplevel menus.
 */
/*ARGSUSED*/
    void
gui_gtk_set_mnemonics(int enable)
{
    vimmenu_T	*menu;
    guint	accel_key;
    char	*accel;
    char	*blank;
    char	*tmp;
    int		i;
    int		j;

    for (menu = root_menu; menu != NULL; menu = menu->next)
    {
	if (menu->id == NULL)
	    continue;

	/* Reparse the accelerator information from the main engines menu
	 * system.
	 *
	 * First count how many underscore's are in the menu name.
	 */
	for (i = 0, tmp = (char *)menu->name; *tmp; tmp++)
	    if (*tmp == '_')
		++i;

	/* Now allocate a new buffer to hold all the menu name along with the
	 * additional underscores.
	 */
	accel = g_new(char, strlen((char *)menu->name) + i + 1);
	blank = g_new(char, strlen((char *)menu->name) + i + 1);
	i = 0;
	j = 0;
	for (tmp = (char *)menu->name; *tmp; ++tmp)
	{
	    /* actext has been added above, stop at the TAB */
	    if (*tmp == TAB)
		break;
	    if (*tmp == '&')
	    {
# ifdef GTK_USE_ACCEL
		accel[i] = '_';
		++i;
# endif
	    }
	    else
	    {
		accel[i] = blank[j] = *tmp;
		++i; ++j;
		if (*tmp == '_')
		{
		    accel[i] =  blank[j] = '_';
		    ++i; ++j;
		}
	    }
	}
	accel[i] = blank[j] = '\0';

	/* let GTK do its thing */
	accel_key = gtk_label_parse_uline(GTK_LABEL(menu->label), accel);
	if (!enable)
	    gtk_label_parse_uline(GTK_LABEL(menu->label), blank);
	g_free(accel);
	g_free(blank);

	if (enable)
	    gtk_widget_add_accelerator(menu->id,
		    "activate_item",
		    gui.accel_group,
		    accel_key, GDK_MOD1_MASK,
		    0);
	else
	    gtk_widget_remove_accelerator(menu->id,
		    gui.accel_group,
		    accel_key, GDK_MOD1_MASK);
    }
}


    static void
recurse_tearoffs(vimmenu_T *menu, int val)
{
    while (menu != NULL)
    {
	if (!menu_is_popup(menu->name))
	{
	    if (menu->submenu_id != 0)
	    {
		if (val)
		    gtk_widget_show(menu->tearoff_handle);
		else
		    gtk_widget_hide(menu->tearoff_handle);
	    }
	    recurse_tearoffs(menu->children, val);
	}
	menu = menu->next;
    }
}


    void
gui_mch_toggle_tearoffs(int enable)
{
    recurse_tearoffs(root_menu, enable);
}
#endif


#ifdef FEAT_TOOLBAR

/*
 * Seems like there's a hole in the GTK Toolbar API: there's no provision for
 * removing an item from the toolbar.  Therefore I need to resort to going
 * really deeply into the internal widget structures.
 */
    static void
toolbar_remove_item_by_text(GtkToolbar *tb, const char *text)
{
    GtkContainer *container;
    GList *childl;
    GtkToolbarChild *gtbc;

    g_return_if_fail(tb != NULL);
    g_return_if_fail(GTK_IS_TOOLBAR(tb));
    container = GTK_CONTAINER(&tb->container);

    for (childl = tb->children; childl; childl = childl->next)
    {
	gtbc = (GtkToolbarChild *)childl->data;

	if (gtbc->type != GTK_TOOLBAR_CHILD_SPACE
		&& strcmp(GTK_LABEL(gtbc->label)->label, text) == 0)
	{
	    gboolean was_visible;

	    was_visible = GTK_WIDGET_VISIBLE(gtbc->widget);
	    gtk_widget_unparent(gtbc->widget);

	    tb->children = g_list_remove_link(tb->children, childl);
	    g_free(gtbc);
	    g_list_free(childl);
	    tb->num_children--;

	    if (was_visible && GTK_WIDGET_VISIBLE(container))
		gtk_widget_queue_resize(GTK_WIDGET(container));

	    break;
	}
    }
}
#endif

#if defined(FEAT_MENU) || defined(PROTO)
/*
 * Destroy the machine specific menu widget.
 */
    void
gui_mch_destroy_menu(vimmenu_T *menu)
{
#ifdef FEAT_TOOLBAR
    if (menu->parent && menu_is_toolbar(menu->parent->name))
    {
	toolbar_remove_item_by_text(GTK_TOOLBAR(gui.toolbar),
					    (const char *)menu->dname);
	return;
    }
#endif

    if (menu->submenu_id != 0)
    {
	gtk_widget_destroy(menu->submenu_id);
	menu->submenu_id = 0;
    }
    if (menu->id != 0)
    {
	/* parent = gtk_widget_get_parent(menu->id); */
	gtk_widget_destroy(menu->id);
	menu->id = 0;
    }
}
#endif /* FEAT_MENU */


/*
 * Scrollbar stuff.
 */

/* This variable is set when we asked for a scrollbar change ourselves.  Don't
 * pass scrollbar changes on to the GUI code then. */
static int did_ask_for_change = FALSE;

    void
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, long val, long size, long max)
{
    if (sb->id != 0)
    {
	GtkAdjustment *adjustment = GTK_RANGE(sb->id)->adjustment;
	adjustment->lower = 0;
	adjustment->value = val;
	adjustment->upper = max + 1;
	adjustment->page_size = size;
	adjustment->page_increment = (size > 2 ? size - 2 : 1);
	adjustment->step_increment = 1;
	did_ask_for_change = TRUE;
	gtk_adjustment_changed(adjustment);
	did_ask_for_change = FALSE;
    }
}

    void
gui_mch_set_scrollbar_pos(scrollbar_T *sb, int x, int y, int w, int h)
{
    if (!sb->id)
	return;
    gtk_form_move_resize(GTK_FORM(gui.formwin), sb->id, x, y, w, h);
}

/*
 * Take action upon scrollbar dragging.
 */
    static void
adjustment_value_changed(GtkAdjustment *adjustment, gpointer data)
{
    scrollbar_T *sb;
    long	value;
    int		dragging = FALSE;

    if (did_ask_for_change)
	return;

    sb = gui_find_scrollbar((long) data);
    value = adjustment->value;

    /* The dragging argument must be right for the scrollbar to work with
     * closed folds.  This isn't documented, hopefully this will keep on
     * working in later GTK versions. */
    if (sb != NULL)
	dragging = GTK_RANGE((GtkScrollbar *)sb->id)->scroll_type
							   == GTK_SCROLL_NONE;
    gui_drag_scrollbar(sb, value, dragging);

    if (gtk_main_level() > 0)
	gtk_main_quit();
}

/* SBAR_VERT or SBAR_HORIZ */
    void
gui_mch_create_scrollbar(scrollbar_T * sb, int orient)
{
    if (orient == SBAR_HORIZ)
    {
	sb->id = gtk_hscrollbar_new(NULL);
	GTK_WIDGET_UNSET_FLAGS(sb->id, GTK_CAN_FOCUS);
	gtk_form_put(GTK_FORM(gui.formwin), sb->id, 0, 0);
    }
    if (orient == SBAR_VERT)
    {
	sb->id = gtk_vscrollbar_new(NULL);
	GTK_WIDGET_UNSET_FLAGS(sb->id, GTK_CAN_FOCUS);
	gtk_form_put(GTK_FORM(gui.formwin), sb->id, 0, 0);
    }
    if (sb->id != NULL)
    {
	GtkAdjustment *adjustment;

	adjustment = gtk_range_get_adjustment(
		GTK_RANGE((GtkScrollbar *)sb->id));
	gtk_signal_connect(GTK_OBJECT(adjustment), "value_changed",
		(GtkSignalFunc)adjustment_value_changed,
			   (gpointer)sb->ident);
    }
    gui_mch_update();
}

#if defined(FEAT_WINDOWS) || defined(PROTO)
    void
gui_mch_destroy_scrollbar(scrollbar_T * sb)
{
    if (sb->id != 0)
    {
	gtk_widget_destroy(sb->id);
	sb->id = 0;
    }
    gui_mch_update();
}
#endif

#if defined(FEAT_BROWSE) || defined(PROTO)
/*
 * Implementation of the file selector related stuff
 */

/*ARGSUSED*/
    static void
browse_ok_cb(GtkWidget *widget, gpointer cbdata)
{
    gui_T *vw = (gui_T *)cbdata;

    if (vw->browse_fname != NULL)
	g_free(vw->browse_fname);

    vw->browse_fname = (char_u *)g_strdup(gtk_file_selection_get_filename(
					GTK_FILE_SELECTION(vw->filedlg)));
    gtk_widget_hide(vw->filedlg);
    if (gtk_main_level() > 0)
	gtk_main_quit();
}

/*ARGSUSED*/
    static void
browse_cancel_cb(GtkWidget *widget, gpointer cbdata)
{
    gui_T *vw = (gui_T *)cbdata;

    if (vw->browse_fname != NULL)
    {
	g_free(vw->browse_fname);
	vw->browse_fname = NULL;
    }
    gtk_widget_hide(vw->filedlg);
    if (gtk_main_level() > 0)
	gtk_main_quit();
}

/*ARGSUSED*/
    static gboolean
browse_destroy_cb(GtkWidget * widget)
{
    if (gui.browse_fname != NULL)
    {
	g_free(gui.browse_fname);
	gui.browse_fname = NULL;
    }
    gui.filedlg = NULL;

    if (gtk_main_level() > 0)
	gtk_main_quit();

    return FALSE;
}

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 * saving,			select file to write
 * title			title for the window
 * dflt				default name
 * ext				not used (extension added)
 * initdir			initial directory, NULL for current dir
 * filter			not used (file name filter)
 */
/*ARGSUSED*/
    char_u *
gui_mch_browse(int saving,
	       char_u * title,
	       char_u * dflt,
	       char_u * ext,
	       char_u * initdir,
	       char_u * filter)
{
    GtkFileSelection *fs;	/* shortcut */
    char_u dirbuf[MAXPATHL];
    char_u *p;

    if (!gui.filedlg)
    {
	gui.filedlg = gtk_file_selection_new((const gchar *)title);
	gtk_window_set_modal(GTK_WINDOW(gui.filedlg), TRUE);
	gtk_window_set_transient_for(GTK_WINDOW(gui.filedlg),
		GTK_WINDOW(gui.mainwin));
	fs = GTK_FILE_SELECTION(gui.filedlg);

	gtk_container_border_width(GTK_CONTAINER(fs), 4);

	gtk_signal_connect(GTK_OBJECT(fs->ok_button),
		"clicked", GTK_SIGNAL_FUNC(browse_ok_cb), &gui);
	gtk_signal_connect(GTK_OBJECT(fs->cancel_button),
		"clicked", GTK_SIGNAL_FUNC(browse_cancel_cb), &gui);
	/* gtk_signal_connect() doesn't work for destroy, it causes a hang */
	gtk_signal_connect_object(GTK_OBJECT(gui.filedlg),
		"destroy", GTK_SIGNAL_FUNC(browse_destroy_cb),
		GTK_OBJECT(gui.filedlg));
    }
    else
	gtk_window_set_title(GTK_WINDOW(gui.filedlg), (const gchar *)title);

    /* if our pointer is currently hidden, then we should show it. */
    gui_mch_mousehide(FALSE);

    /* Concatenate "initdir" and "dflt". */
    if (initdir == NULL || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    else if (STRLEN(initdir) + 2 < MAXPATHL)
	STRCPY(dirbuf, initdir);
    else
	dirbuf[0] = NUL;
    /* Always need a trailing slash for a directory. */
    add_pathsep(dirbuf);
    if (dflt != NULL && *dflt != NUL
			      && STRLEN(dirbuf) + 2 + STRLEN(dflt) < MAXPATHL)
	STRCAT(dirbuf, dflt);

    gtk_file_selection_set_filename(GTK_FILE_SELECTION(gui.filedlg),
						      (const gchar *)dirbuf);

    gui_gtk_position_in_parent(GTK_WIDGET(gui.mainwin),
				       GTK_WIDGET(gui.filedlg), VW_POS_MOUSE);

    gtk_widget_show(gui.filedlg);
    while (gui.filedlg && GTK_WIDGET_DRAWABLE(gui.filedlg))
	gtk_main_iteration_do(TRUE);

    if (gui.browse_fname == NULL)
	return NULL;

    /* shorten the file name if possible */
    mch_dirname(dirbuf, MAXPATHL);
    p = shorten_fname(gui.browse_fname, dirbuf);
    if (p == NULL)
	p = gui.browse_fname;
    return vim_strsave(p);
}

#endif	/* FEAT_BROWSE */

#if defined(FEAT_GUI_DIALOG) || defined(PROTO)

static char_u *dialog_textfield = NULL;
static GtkWidget *dialog_textentry;

    static void
dlg_destroy(GtkWidget *dlg)
{
    gchar *p;

    if (dialog_textfield != NULL)
    {
	/* Get the text from the textentry widget. */
	p = gtk_editable_get_chars(GTK_EDITABLE(dialog_textentry),
							       0, IOSIZE - 1);
	STRCPY(dialog_textfield, p);
	g_free(p);
    }
    /* Destroy the dialog, will break the waiting loop. */
    gtk_widget_destroy(dlg);
}

# ifdef FEAT_GUI_GNOME
/* ARGSUSED */
    static int
gui_gnome_dialog( int	type,
		char_u	*title,
		char_u	*message,
		char_u	*buttons,
		int	dfltbutton,
		char_u  *textfield)
{
    GtkWidget	*dlg;
    char	*gdtype;
    char_u	*buttons_copy, *p, *next;
    char	**buttons_list;
    int		butcount, cur;

    /* make a copy, so that we can insert NULs */
    if ((buttons_copy = vim_strsave(buttons)) == NULL)
	return -1;

    /* determine exact number of buttons and allocate array to hold them */
    for (butcount = 0, p = buttons; *p; p++)
    {
	if (*p == '\n')
	    butcount++;
    }
    butcount++;
    buttons_list = g_new0(char *, butcount + 1);

    /* Add pixmap */
    switch (type)
    {
    case VIM_ERROR:
	gdtype = GNOME_MESSAGE_BOX_ERROR;
	break;
    case VIM_WARNING:
	gdtype = GNOME_MESSAGE_BOX_WARNING;
	break;
    case VIM_INFO:
	gdtype = GNOME_MESSAGE_BOX_INFO;
	break;
    case VIM_QUESTION:
	gdtype = GNOME_MESSAGE_BOX_QUESTION;
	break;
    default:
	gdtype = GNOME_MESSAGE_BOX_GENERIC;
    };

    p = buttons_copy;
    for (cur = 0; cur < butcount; ++cur)
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

	/* this should probably go into a table, but oh well */
	if (g_strcasecmp((char *)p, "Ok") == 0)
	    buttons_list[cur] = g_strdup(GNOME_STOCK_BUTTON_OK);
	else if (g_strcasecmp((char *)p, "Cancel") == 0)
	    buttons_list[cur] = g_strdup(GNOME_STOCK_BUTTON_CANCEL);
	else if (g_strcasecmp((char *)p, "Yes") == 0)
	    buttons_list[cur] = g_strdup(GNOME_STOCK_BUTTON_YES);
	else if (g_strcasecmp((char *)p, "No") == 0)
	    buttons_list[cur] = g_strdup(GNOME_STOCK_BUTTON_NO);
	else if (g_strcasecmp((char *)p, "Close") == 0)
	    buttons_list[cur] = g_strdup(GNOME_STOCK_BUTTON_CLOSE);
	else if (g_strcasecmp((char *)p, "Help") == 0)
	    buttons_list[cur] = g_strdup(GNOME_STOCK_BUTTON_HELP);
	else if (g_strcasecmp((char *)p, "Apply") == 0)
	    buttons_list[cur] = g_strdup(GNOME_STOCK_BUTTON_APPLY);
#if 0
	/*
	 * these aren't really used that often anyway, but are listed here as
	 * placeholders in case we need them.
	 */
	else if (g_strcasecmp((char *)p, "Next") == 0)
	    buttons_list[cur] = g_strdup(GNOME_STOCK_BUTTON_NEXT);
	else if (g_strcasecmp((char *)p, "Prev") == 0)
	    buttons_list[cur] = g_strdup(GNOME_STOCK_BUTTON_PREV);
	else if (g_strcasecmp((char *)p, "Up") == 0)
	    buttons_list[cur] = g_strdup(GNOME_STOCK_BUTTON_UP);
	else if (g_strcasecmp((char *)p, "Down") == 0)
	    buttons_list[cur] = g_strdup(GNOME_STOCK_BUTTON_DOWN);
	else if (g_strcasecmp((char *)p, "Font") == 0)
	    buttons_list[cur] = g_strdup(GNOME_STOCK_BUTTON_FONT);
#endif
	else
	    buttons_list[cur] = g_strdup((char *)p);

	if (*next == NUL)
	    break;

	p = next;
    }
    vim_free(buttons_copy);

    dlg = gnome_message_box_newv((const char *)message,
				 (const char *)gdtype,
				 (const char **)buttons_list);
    for (cur = 0; cur < butcount; ++cur)
	g_free(buttons_list[cur]);
    g_free(buttons_list);

    dialog_textfield = textfield;
    if (textfield != NULL)
    {
	/* Add text entry field */
	dialog_textentry = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dlg)->vbox), dialog_textentry,
			   TRUE, TRUE, 0);
	gtk_entry_set_text(GTK_ENTRY(dialog_textentry),
			   (const gchar *)textfield);
	gtk_entry_select_region(GTK_ENTRY(dialog_textentry), 0,
				STRLEN(textfield));
	gtk_entry_set_max_length(GTK_ENTRY(dialog_textentry), IOSIZE - 1);
	gtk_entry_set_position(GTK_ENTRY(dialog_textentry), STRLEN(textfield));
	gtk_widget_show(dialog_textentry);
	gtk_window_set_focus(GTK_WINDOW(dlg), dialog_textentry);
    }

    gtk_signal_connect_object(GTK_OBJECT(dlg), "destroy",
			      GTK_SIGNAL_FUNC(dlg_destroy), GTK_OBJECT(dlg));
    gnome_dialog_set_default(GNOME_DIALOG(dlg), dfltbutton + 1);
    gui_gtk_position_in_parent(GTK_WIDGET(gui.mainwin),
			       GTK_WIDGET(dlg), VW_POS_MOUSE);

    return (1 + gnome_dialog_run_and_close(GNOME_DIALOG(dlg)));
}

# endif /* FEAT_GUI_GNOME */

typedef struct _ButtonData
{
    int		*status;
    int		index;
    GtkWidget	*dialog;
} ButtonData;

typedef struct _CancelData
{
    int		*status;
    GtkWidget	*dialog;
} CancelData;

/* ARGSUSED */
    static void
dlg_button_clicked(GtkWidget * widget, ButtonData *data)
{
    *(data->status) = data->index + 1;
    dlg_destroy(data->dialog);
}

/*
 * This makes the Escape key equivalent to the cancel button.
 */
/*ARGSUSED*/
    static int
dlg_key_press_event(GtkWidget * widget, GdkEventKey * event, CancelData *data)
{
    if (event->keyval != GDK_Escape && event->keyval != GDK_Return)
	return FALSE;

    /* The result value of 0 from a dialog is signaling cancelation.
     * 1 means OK. */
    *(data->status) = (event->keyval == GDK_Return);
    dlg_destroy(data->dialog);

    return TRUE;
}

/*
 * Callback function for when the dialog was destroyed by a window manager.
 */
    static void
dlg_destroy_cb(int *p)
{
    *p = TRUE;		/* set dialog_destroyed to break out of the loop */
    if (gtk_main_level() > 0)
	gtk_main_quit();
}

/* ARGSUSED */
    int
gui_mch_dialog(	int	type,		/* type of dialog */
		char_u	*title,		/* title of dialog */
		char_u	*message,	/* message text */
		char_u	*buttons,	/* names of buttons */
		int	def_but,	/* default button */
		char_u	*textfield)	/* text for textfield or NULL */
{
    char_u	*names;
    char_u	*p;
    int		i;
    int		butcount;
    int		dialog_status = -1;
    int		dialog_destroyed = FALSE;
    int		vertical;

    GtkWidget		*dialog;
    GtkWidget		*frame;
    GtkWidget		*vbox;
    GtkWidget		*table;
    GtkWidget		*pixmap;
    GtkWidget		*dialogmessage;
    GtkWidget		*action_area;
    GtkWidget		*sub_area;
    GtkWidget		*separator;
    GtkAccelGroup	*accel_group;

    GdkPixmap		*icon = NULL;
    GdkBitmap		*mask = NULL;
    char		**icon_data = NULL;

    GtkWidget		**button;
    ButtonData		*data;
    CancelData		cancel_data;

    /* if our pointer is currently hidden, then we should show it. */
    gui_mch_mousehide(FALSE);

# ifdef FEAT_GUI_GNOME
    /* If Gnome is available, use it for the dialog. */
    if (gtk_socket_id == 0)
	return gui_gnome_dialog(type, title, message, buttons, def_but,
								   textfield);
# endif

    if (title == NULL)
	title = (char_u *)_("Vim dialog...");

    if ((type < 0) || (type > VIM_LAST_TYPE))
	type = VIM_GENERIC;

    /* Check 'v' flag in 'guioptions': vertical button placement. */
    vertical = (vim_strchr(p_go, GO_VERTICAL) != NULL);

    dialog = gtk_window_new(GTK_WINDOW_DIALOG);
    gtk_window_set_title(GTK_WINDOW(dialog), (const gchar *)title);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(gui.mainwin));
    gtk_widget_realize(dialog);
    gdk_window_set_decorations(dialog->window, GDK_DECOR_BORDER);
    gdk_window_set_functions(dialog->window, GDK_FUNC_MOVE);

    cancel_data.status = &dialog_status;
    cancel_data.dialog = dialog;
    gtk_signal_connect_after(GTK_OBJECT(dialog), "key_press_event",
		    GTK_SIGNAL_FUNC(dlg_key_press_event),
		    (gpointer) &cancel_data);
    /* Catch the destroy signal, otherwise we don't notice a window manager
     * destroying the dialog window. */
    gtk_signal_connect_object(GTK_OBJECT(dialog), "destroy",
		    GTK_SIGNAL_FUNC(dlg_destroy_cb),
		    (gpointer)&dialog_destroyed);

    gtk_grab_add(dialog);

    /* this makes it look beter on Motif style window managers */
    frame = gtk_frame_new(NULL);
    gtk_container_add(GTK_CONTAINER(dialog), frame);
    gtk_widget_show(frame);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    gtk_widget_show(vbox);

    table = gtk_table_new(1, 3, FALSE);
    gtk_table_set_row_spacings(GTK_TABLE(table), 4);
    gtk_table_set_col_spacings(GTK_TABLE(table), 8);
    gtk_container_border_width(GTK_CONTAINER(table), 4);
    gtk_box_pack_start(GTK_BOX(vbox), table, 4, 4, 0);
    gtk_widget_show(table);

    /* Add pixmap */
    switch (type)
    {
    case VIM_GENERIC:
	icon_data = generic_xpm;
	break;
    case VIM_ERROR:
	icon_data = error_xpm;
	break;
    case VIM_WARNING:
	icon_data = alert_xpm;
	break;
    case VIM_INFO:
	icon_data = info_xpm;
	break;
    case VIM_QUESTION:
	icon_data = quest_xpm;
	break;
    default:
	icon_data = generic_xpm;
    };
    icon = gdk_pixmap_colormap_create_from_xpm_d(NULL,
				     gtk_widget_get_colormap(dialog),
				     &mask, NULL, icon_data);
    if (icon)
    {
	pixmap = gtk_pixmap_new(icon, mask);
	/* gtk_misc_set_alignment(GTK_MISC(pixmap), 0.5, 0.5); */
	gtk_table_attach_defaults(GTK_TABLE(table), pixmap, 0, 1, 0, 1);
	gtk_widget_show(pixmap);
    }

    /* Add label */
    dialogmessage = gtk_label_new((const gchar *)message);
    gtk_table_attach_defaults(GTK_TABLE(table), dialogmessage, 1, 2, 0, 1);
    gtk_widget_show(dialogmessage);

    dialog_textfield = textfield;
    if (textfield != NULL)
    {
	/* Add text entry field */
	dialog_textentry = gtk_entry_new();
	gtk_widget_set_usize(dialog_textentry, 400, -2);
	gtk_box_pack_start(GTK_BOX(vbox), dialog_textentry, TRUE, TRUE, 0);
	gtk_entry_set_text(GTK_ENTRY(dialog_textentry),
						    (const gchar *)textfield);
	gtk_entry_select_region(GTK_ENTRY(dialog_textentry), 0,
							   STRLEN(textfield));
	gtk_entry_set_max_length(GTK_ENTRY(dialog_textentry), IOSIZE - 1);
	gtk_entry_set_position(GTK_ENTRY(dialog_textentry), STRLEN(textfield));
	gtk_widget_show(dialog_textentry);
    }

    /* Add box for buttons */
    action_area = gtk_hbox_new(FALSE, 0);
    gtk_container_border_width(GTK_CONTAINER(action_area), 4);
    gtk_box_pack_end(GTK_BOX(vbox), action_area, FALSE, TRUE, 0);
    gtk_widget_show(action_area);

    /* Add a [vh]box in the hbox to center the buttons in the dialog. */
    if (vertical)
	sub_area = gtk_vbox_new(FALSE, 0);
    else
	sub_area = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(sub_area), 0);
    gtk_box_pack_start(GTK_BOX(action_area), sub_area, TRUE, FALSE, 0);
    gtk_widget_show(sub_area);

    /*
     * Create the buttons.
     */

    /*
     * Translate the Vim accelerator character into an underscore for GTK+.
     * Double underscores to keep them in the label.
     */
    /* count the number of underscores */
    i = 1;
    for (p = buttons; *p; ++p)
	if (*p == '_')
	    ++i;

    /* make a copy of "buttons" with the translated characters */
    names = alloc(STRLEN(buttons) + i);
    if (names == NULL)
	return -1;

    p = names;
    for (i = 0; buttons[i]; ++i)
    {
	if (buttons[i] == DLG_HOTKEY_CHAR)
	    *p++ = '_';
	else
	{
	    if (buttons[i] == '_')
		*p++ = '_';
	    *p++ = buttons[i];
	}
    }
    *p = NUL;

    /* Count the number of buttons and allocate button[] and data[]. */
    butcount = 1;
    for (p = names; *p; ++p)
	if (*p == DLG_BUTTON_SEP)
	    ++butcount;
    button = (GtkWidget **)alloc((unsigned)(butcount * sizeof(GtkWidget *)));
    data = (ButtonData *)alloc((unsigned)(butcount * sizeof(ButtonData)));
    if (button == NULL || data == NULL)
    {
	vim_free(names);
	vim_free(button);
	vim_free(data);
	return -1;
    }

    /* Attach the new accelerator group to the window. */
    accel_group = gtk_accel_group_new();
    gtk_accel_group_attach(accel_group, GTK_OBJECT(dialog));

    p = names;
    for (butcount = 0; *p; ++butcount)
    {
	char_u		*next;
	GtkWidget	*label;
# ifdef GTK_USE_ACCEL
	guint		accel_key;
# endif

	/* Chunk out this single button. */
	for (next = p; *next; ++next)
	{
	    if (*next == DLG_BUTTON_SEP)
	    {
		*next++ = NUL;
		break;
	    }
	}

	button[butcount] = gtk_button_new();
	GTK_WIDGET_SET_FLAGS(button[butcount], GTK_CAN_DEFAULT);

	label = gtk_accel_label_new("");
	gtk_accel_label_set_accel_widget(GTK_ACCEL_LABEL(label), dialog);

# ifdef GTK_USE_ACCEL
	accel_key = gtk_label_parse_uline(GTK_LABEL(label), (const gchar *)p);
	/* Don't add accelator if 'winaltkeys' is "no". */
	if (accel_key != GDK_VoidSymbol)
	{
	    gtk_widget_add_accelerator(button[butcount],
		    "clicked",
		    accel_group,
		    accel_key, 0,
		    0);
	}
# else
	(void)gtk_label_parse_uline(GTK_LABEL(label), (const gchar *)p);
# endif

	gtk_container_add(GTK_CONTAINER(button[butcount]), label);
	gtk_widget_show_all(button[butcount]);

	data[butcount].status = &dialog_status;
	data[butcount].index = butcount;
	data[butcount].dialog = dialog;
	gtk_signal_connect(GTK_OBJECT(button[butcount]),
			   (const char *)"clicked",
			   GTK_SIGNAL_FUNC(dlg_button_clicked),
			   (gpointer) &data[butcount]);

	gtk_box_pack_start(GTK_BOX(sub_area), button[butcount],
			   TRUE, FALSE, 0);
	p = next;
    }

    vim_free(names);

    if (butcount > 0)
    {
	--def_but;		/* 1 is first button */
	if (def_but < 0)
	    def_but = 0;
	if (def_but >= butcount)
	    def_but = butcount - 1;

	gtk_widget_grab_focus(button[def_but]);
	gtk_widget_grab_default(button[def_but]);
    }

    if (textfield != NULL)
	gtk_window_set_focus(GTK_WINDOW(dialog), dialog_textentry);

    separator = gtk_hseparator_new();
    gtk_box_pack_end(GTK_BOX(vbox), separator, FALSE, TRUE, 0);
    gtk_widget_show(separator);

    dialog_status = -1;

    gui_gtk_position_in_parent(GTK_WIDGET(gui.mainwin),
					    GTK_WIDGET(dialog), VW_POS_MOUSE);

    gtk_widget_show(dialog);

    /* loop here until the dialog goes away */
    while (dialog_status == -1 && !dialog_destroyed
					       && GTK_WIDGET_DRAWABLE(dialog))
	gtk_main_iteration_do(TRUE);

    if (dialog_status < 0)
	dialog_status = 0;
    if (dialog_status != 1 && textfield != NULL)
	*textfield = NUL;	/* dialog was cancelled */

    /* let the garbage collector know that we don't need it anylonger */
    gtk_accel_group_unref(accel_group);

    vim_free(button);
    vim_free(data);

    return dialog_status;
}


#endif	/* FEAT_GUI_DIALOG */

#if defined(FEAT_MENU) || defined(PROTO)
    void
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    gtk_menu_popup(GTK_MENU(menu->submenu_id), NULL, NULL,
	       (GtkMenuPositionFunc)NULL, NULL, 3, (guint32)GDK_CURRENT_TIME);
}
#endif


/*
 * We don't create it twice.
 */

typedef struct _SharedFindReplace
{
    GtkWidget *dialog;	/* the main dialog widget */
    GtkWidget *exact;	/* 'Exact match' check button */
    GtkWidget *up;	/* search direction 'Up' radio button */
    GtkWidget *down;    /* search direction 'Down' radio button */
    GtkWidget *what;	/* 'Find what' entry text widget */
    GtkWidget *with;	/* 'Replace with' entry text widget */
    GtkWidget *find;	/* 'Find Next' action button */
    GtkWidget *replace;	/* 'Replace With' action button */
    GtkWidget *all;	/* 'Replace All' action button */
} SharedFindReplace;

static SharedFindReplace find_widgets = { NULL };
static SharedFindReplace repl_widgets = { NULL };

/* ARGSUSED */
    static int
find_key_press_event(
		GtkWidget	*widget,
		GdkEventKey	*event,
		SharedFindReplace *frdp)
{
    /* If the user is holding one of the key modifiers we will just bail out,
     * thus preserving the possibility of normal focus traversal.
     */
    if (event->state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
	return FALSE;

    /* the scape key synthesizes a cancellation action */
    if (event->keyval == GDK_Escape)
    {
	gtk_widget_hide(frdp->dialog);

	return TRUE;
    }

    /* block traversal resulting from those keys */
    if (event->keyval == GDK_Left
	    || event->keyval == GDK_Right
	    || event->keyval == GDK_space)
	return TRUE;

    /* It would be delightfull if it where possible to do search history
     * operations on the K_UP and K_DOWN keys here.
     */

    return FALSE;
}

    static void
find_replace_dialog_create(char_u *arg, int do_replace)
{
    GtkWidget	*frame;
    GtkWidget	*hbox;		/* main top down box */
    GtkWidget	*actionarea;
    GtkWidget	*table;
    GtkWidget	*tmp;
    GtkWidget	*vbox;
    gboolean	sensitive;
    SharedFindReplace *frdp;
    char_u	*entry_text;
    int		exact_word = FALSE;

    frdp = (do_replace) ? (&repl_widgets) : (&find_widgets);

    /* Get the search string to use. */
    entry_text = get_find_dialog_text(arg, &exact_word);

    /*
     * If the dialog already exists, just raise it.
     */
    if (frdp->dialog)
    {
	/* always make the dialog appear where you want it even if the mainwin
	 * has moved -- dbv */
	gui_gtk_position_in_parent(GTK_WIDGET(gui.mainwin),
				      GTK_WIDGET(frdp->dialog), VW_POS_MOUSE);

	gui_gtk_synch_fonts();
	if (!GTK_WIDGET_VISIBLE(frdp->dialog))
	{
	    gtk_widget_grab_focus(frdp->what);
	    gtk_widget_show(frdp->dialog);
	}

	if (entry_text != NULL)
	{
	    gtk_entry_set_text(GTK_ENTRY(frdp->what), (char *)entry_text);
	    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(frdp->exact),
							(gboolean)exact_word);
	}
	gdk_window_raise(frdp->dialog->window);

	vim_free(entry_text);
	return;
    }

    frdp->dialog = gtk_window_new(GTK_WINDOW_DIALOG);
    if (do_replace)
    {
	gtk_window_set_wmclass(GTK_WINDOW(frdp->dialog), "searchrepl", "gvim");
	gtk_window_set_title(GTK_WINDOW(frdp->dialog), _("VIM - Search and Replace..."));
    }
    else
    {
	gtk_window_set_wmclass(GTK_WINDOW(frdp->dialog), "search", "gvim");
	gtk_window_set_title(GTK_WINDOW(frdp->dialog), _("VIM - Search..."));
    }

    gtk_widget_realize(frdp->dialog);
    gdk_window_set_decorations(frdp->dialog->window,
	    GDK_DECOR_TITLE | GDK_DECOR_BORDER | GDK_DECOR_RESIZEH);
    gdk_window_set_functions(frdp->dialog->window,
	    GDK_FUNC_RESIZE | GDK_FUNC_MOVE);

    /* this makes it look beter on Motif style window managers */
    frame = gtk_frame_new(NULL);
    gtk_container_add(GTK_CONTAINER(frdp->dialog), frame);

    hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(frame), hbox);

    if (do_replace)
	table = gtk_table_new(1024, 4, FALSE);
    else
	table = gtk_table_new(1024, 3, FALSE);
    gtk_box_pack_start(GTK_BOX(hbox), table, TRUE, TRUE, 0);
    gtk_container_border_width(GTK_CONTAINER(table), 4);

    tmp = gtk_label_new(_("Find what:"));
    gtk_misc_set_alignment(GTK_MISC(tmp), (float)1.0, (float)0.5);
    gtk_table_attach(GTK_TABLE(table), tmp, 0, 1, 0, 1,
		     GTK_FILL, GTK_EXPAND, 2, 2);
    frdp->what = gtk_entry_new();
    sensitive = (entry_text != NULL && STRLEN(entry_text) != 0);
    if (entry_text != NULL)
	gtk_entry_set_text(GTK_ENTRY(frdp->what), (char *)entry_text);
    gtk_signal_connect(GTK_OBJECT(frdp->what), "changed",
		       GTK_SIGNAL_FUNC(entry_changed_cb), frdp->dialog);
    gtk_signal_connect_after(GTK_OBJECT(frdp->what), "key_press_event",
				 GTK_SIGNAL_FUNC(find_key_press_event),
				 (gpointer) frdp);
    gtk_table_attach(GTK_TABLE(table), frdp->what, 1, 1024, 0, 1,
		     GTK_EXPAND | GTK_FILL, GTK_EXPAND, 2, 2);

    if (do_replace)
    {
	tmp = gtk_label_new(_("Replace with:"));
	gtk_misc_set_alignment(GTK_MISC(tmp), (float)1.0, (float)0.5);
	gtk_table_attach(GTK_TABLE(table), tmp, 0, 1, 1, 2,
			 GTK_FILL, GTK_EXPAND, 2, 2);
	frdp->with = gtk_entry_new();
	gtk_signal_connect(GTK_OBJECT(frdp->with), "activate",
			   GTK_SIGNAL_FUNC(find_replace_cb),
			   (gpointer) FR_R_FINDNEXT);
	gtk_signal_connect_after(GTK_OBJECT(frdp->with), "key_press_event",
				 GTK_SIGNAL_FUNC(find_key_press_event),
				 (gpointer) frdp);
	gtk_table_attach(GTK_TABLE(table), frdp->with, 1, 1024, 1, 2,
			 GTK_EXPAND | GTK_FILL, GTK_EXPAND, 2, 2);

	/*
	 * Make the entry activation only change the input focus onto the
	 * with item.
	 */
	gtk_signal_connect(GTK_OBJECT(frdp->what), "activate",
			   GTK_SIGNAL_FUNC(entry_activate_cb), frdp->with);
    }
    else
    {
	/*
	 * Make the entry activation do the search.
	 */
	gtk_signal_connect(GTK_OBJECT(frdp->what), "activate",
			   GTK_SIGNAL_FUNC(find_replace_cb),
			   (gpointer) FR_FINDNEXT);
    }

    /* exact match only button */
    frdp->exact = gtk_check_button_new_with_label(_("Match exact word only"));
    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(frdp->exact),
							(gboolean)exact_word);
    gtk_signal_connect(GTK_OBJECT(frdp->exact), "clicked",
		       GTK_SIGNAL_FUNC(exact_match_cb), NULL);
    if (do_replace)
	gtk_table_attach(GTK_TABLE(table), frdp->exact, 0, 1023, 3, 4,
			 GTK_FILL, GTK_EXPAND, 2, 2);
    else
	gtk_table_attach(GTK_TABLE(table), frdp->exact, 0, 1023, 2, 3,
			 GTK_FILL, GTK_EXPAND, 2, 2);

    tmp = gtk_frame_new(_("Direction"));
    if (do_replace)
	gtk_table_attach(GTK_TABLE(table), tmp, 1023, 1024, 2, 4,
			 GTK_FILL, GTK_FILL, 2, 2);
    else
	gtk_table_attach(GTK_TABLE(table), tmp, 1023, 1024, 1, 3,
			 GTK_FILL, GTK_FILL, 2, 2);
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_border_width(GTK_CONTAINER(vbox), 0);
    gtk_container_add(GTK_CONTAINER(tmp), vbox);

    /* 'Up' and 'Down' buttons */
    frdp->up = gtk_radio_button_new_with_label(NULL, _("Up"));
    gtk_box_pack_start(GTK_BOX(vbox), frdp->up, TRUE, TRUE, 0);
    frdp->down = gtk_radio_button_new_with_label(
			gtk_radio_button_group(GTK_RADIO_BUTTON(frdp->up)),
			_("Down"));
    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(frdp->down), TRUE);
    if (do_replace)
	gtk_signal_connect(GTK_OBJECT(frdp->down), "clicked",
			   GTK_SIGNAL_FUNC(repl_dir_cb), NULL);
    else
	gtk_signal_connect(GTK_OBJECT(frdp->down), "clicked",
			   GTK_SIGNAL_FUNC(find_direction_cb), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), frdp->down, TRUE, TRUE, 0);

    /* vbox to hold the action buttons */
    actionarea = gtk_vbutton_box_new();
    gtk_container_border_width(GTK_CONTAINER(actionarea), 2);
    if (do_replace)
    {
	gtk_button_box_set_layout(GTK_BUTTON_BOX(actionarea),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(actionarea), 0);
    }
    gtk_box_pack_end(GTK_BOX(hbox), actionarea, FALSE, FALSE, 0);

    /* 'Find Next' button */
    frdp->find = gtk_button_new_with_label(_("Find Next"));
    gtk_widget_set_sensitive(frdp->find, sensitive);
    if (do_replace)
	gtk_signal_connect(GTK_OBJECT(frdp->find), "clicked",
			   GTK_SIGNAL_FUNC(find_replace_cb),
			   (gpointer) FR_R_FINDNEXT);
    else
	gtk_signal_connect(GTK_OBJECT(frdp->find), "clicked",
			   GTK_SIGNAL_FUNC(find_replace_cb),
			   (gpointer) FR_FINDNEXT);
    GTK_WIDGET_SET_FLAGS(frdp->find, GTK_CAN_DEFAULT);
    gtk_box_pack_start(GTK_BOX(actionarea), frdp->find, FALSE, FALSE, 0);
    gtk_widget_grab_default(frdp->find);

    if (do_replace)
    {
	/* 'Replace' button */
	frdp->replace = gtk_button_new_with_label(_("Replace"));
	gtk_widget_set_sensitive(frdp->replace, sensitive);
	GTK_WIDGET_SET_FLAGS(frdp->replace, GTK_CAN_DEFAULT);
	gtk_box_pack_start(GTK_BOX(actionarea), frdp->replace, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(frdp->replace), "clicked",
			   GTK_SIGNAL_FUNC(find_replace_cb),
			   (gpointer) FR_REPLACE);

	/* 'Replace All' button */
	frdp->all = gtk_button_new_with_label(_("Replace All"));
	gtk_widget_set_sensitive(frdp->all, sensitive);
	GTK_WIDGET_SET_FLAGS(frdp->all, GTK_CAN_DEFAULT);
	gtk_box_pack_start(GTK_BOX(actionarea), frdp->all, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(frdp->all), "clicked",
			   GTK_SIGNAL_FUNC(find_replace_cb),
			   (gpointer) FR_REPLACEALL);
    }

    /* 'Cancel' button */
    tmp = gtk_button_new_with_label(_("Cancel"));
    GTK_WIDGET_SET_FLAGS(tmp, GTK_CAN_DEFAULT);
    gtk_box_pack_end(GTK_BOX(actionarea), tmp, FALSE, FALSE, 0);
    gtk_signal_connect_object(GTK_OBJECT(tmp),
			      "clicked", GTK_SIGNAL_FUNC(gtk_widget_hide),
			      GTK_OBJECT(frdp->dialog));
    gtk_signal_connect_object(GTK_OBJECT(frdp->dialog),
			      "delete_event", GTK_SIGNAL_FUNC(gtk_widget_hide),
			      GTK_OBJECT(frdp->dialog));

    tmp = gtk_vseparator_new();
    gtk_box_pack_end(GTK_BOX(hbox), tmp, FALSE, TRUE, 0);
    gtk_widget_grab_focus(frdp->what);

    /* show the frame and realize the frdp->dialog this gives us a window size
     * request that we'll use to position the window within the boundary of
     * the mainwin --dbv */
    gtk_widget_show_all(frame);
    gui_gtk_position_in_parent(GTK_WIDGET(gui.mainwin),
				      GTK_WIDGET(frdp->dialog), VW_POS_MOUSE);

    gui_gtk_synch_fonts();

    gtk_widget_show_all(frdp->dialog);

    vim_free(entry_text);
}

    void
gui_mch_find_dialog(exarg_T *eap)
{
    if (gui.in_use)
	find_replace_dialog_create(eap->arg, FALSE);
}


    void
gui_mch_replace_dialog(exarg_T *eap)
{
    if (gui.in_use)
	find_replace_dialog_create(eap->arg, TRUE);
}


/*
 * Synchronize all gui elements, which are dependant upon the
 * main text font used. Those are in esp. the find/replace dialogs.
 * If you don't understand why this should be needed, please try to
 * search for "piê¶æ" in iso8859-2.
 */
    void
gui_gtk_synch_fonts(void)
{
    SharedFindReplace *frdp;
    int do_replace;

    /* OK this loop is a bit tricky... */
    for (do_replace = 0; do_replace <= 1; ++do_replace)
    {
	frdp = (do_replace) ? (&repl_widgets) : (&find_widgets);
	if (frdp->dialog)
	{
	    GtkStyle *style;

	    /* synch the font with whats used by the text itself */
	    style = gtk_style_copy(gtk_widget_get_style(frdp->what));
	    gdk_font_unref(style->font);
#ifdef FEAT_XFONTSET
	    if (gui.fontset != NOFONTSET)
		style->font = gui.fontset;
	    else
#endif
		style->font = gui.norm_font;
	    gdk_font_ref(style->font);
	    gtk_widget_set_style(frdp->what, style);
	    gtk_style_unref(style);
	    if (do_replace)
	    {
		style = gtk_style_copy(gtk_widget_get_style(frdp->with));
		gdk_font_unref(style->font);
#ifdef FEAT_XFONTSET
		if (gui.fontset != NOFONTSET)
		    style->font = gui.fontset;
		else
#endif
		    style->font = gui.norm_font;
		gdk_font_ref(style->font);
		gtk_widget_set_style(frdp->with, style);
		gtk_style_unref(style);
	    }
	}
    }
}


/*
 * Callback for actions of the find and replace dialogs
 */
/*ARGSUSED*/
    static void
find_replace_cb(GtkWidget *widget, unsigned int flags)
{
    char	*find_text, *repl_text;
    gboolean	direction_down = TRUE;
    gboolean	exact_match = FALSE;
    SharedFindReplace *sfr;

    /* Get the search/replace strings from the dialog */
    if (flags == FR_FINDNEXT)
    {
	repl_text = NULL;
	sfr = &find_widgets;
    }
    else
    {
	repl_text = gtk_entry_get_text(GTK_ENTRY(repl_widgets.with));
	sfr = &repl_widgets;
    }
    find_text = gtk_entry_get_text(GTK_ENTRY(sfr->what));
    direction_down = GTK_TOGGLE_BUTTON(sfr->down)->active;
    exact_match = GTK_TOGGLE_BUTTON(sfr->exact)->active;

    if (gui_do_findrepl((int)flags, (char_u *)find_text, (char_u *)repl_text,
				       (int)direction_down, (int)exact_match))
	if (gtk_main_level() > 0)
	    gtk_main_quit();	/* make sure cmd will be handled immediately */
}

/* our usual callback function */
/*ARGSUSED*/
    static void
entry_activate_cb(GtkWidget * widget, GtkWidget * with)
{
    gtk_widget_grab_focus(with);
}


/*
 * The following are used to synchronize the direction setting
 * between the search and the replace dialog.
 */
/*ARGSUSED*/
    static void
find_direction_cb(GtkWidget * widget, gpointer data)
{
    gboolean direction_down = GTK_TOGGLE_BUTTON(widget)->active;

    if (repl_widgets.dialog)
    {
	GtkWidget *w;
	w = direction_down ? repl_widgets.down : repl_widgets.up;

	if (!GTK_TOGGLE_BUTTON(w)->active)
	    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(w), TRUE);
    }
}

/*ARGSUSED*/
    static void
repl_dir_cb(GtkWidget *widget, gpointer data)
{
    gboolean direction_down = GTK_TOGGLE_BUTTON(widget)->active;

    if (find_widgets.dialog)
    {
	GtkWidget *w;
	w = direction_down ? find_widgets.down : find_widgets.up;

	if (!GTK_TOGGLE_BUTTON(w)->active)
	    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(w), TRUE);
    }
}

/*ARGSUSED*/
    static void
exact_match_cb(GtkWidget * widget, gpointer data)
{
    gboolean exact_match = GTK_TOGGLE_BUTTON(widget)->active;

    if (find_widgets.dialog)
	gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(find_widgets.exact),
				    exact_match);
    if (repl_widgets.dialog)
	gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(repl_widgets.exact),
				    exact_match);
}

    static void
entry_changed_cb(GtkWidget * entry, GtkWidget * dialog)
{
    gchar	*entry_text;
    gboolean	nonempty;

    entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

    if (!entry_text)
	return;			/* shouldn't happen */

    nonempty = (strlen(entry_text) != 0);

    if (dialog == find_widgets.dialog)
    {
	gtk_widget_set_sensitive(find_widgets.find, nonempty);
	if (repl_widgets.dialog)
	{
	    gtk_widget_set_sensitive(repl_widgets.find, nonempty);
	    gtk_widget_set_sensitive(repl_widgets.replace, nonempty);
	    gtk_widget_set_sensitive(repl_widgets.all, nonempty);
	    if (strcmp(entry_text,
		       gtk_entry_get_text(GTK_ENTRY(repl_widgets.what))))
		gtk_entry_set_text(GTK_ENTRY(repl_widgets.what), entry_text);
	}
    }
    if (dialog == repl_widgets.dialog)
    {
	gtk_widget_set_sensitive(repl_widgets.find, nonempty);
	gtk_widget_set_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_set_sensitive(repl_widgets.all, nonempty);
	if (find_widgets.dialog)
	{
	    gtk_widget_set_sensitive(find_widgets.find, nonempty);
	    if (strcmp(entry_text,
		       gtk_entry_get_text(GTK_ENTRY(find_widgets.what))))
		gtk_entry_set_text(GTK_ENTRY(find_widgets.what), entry_text);
	}
    }
}

/*
 * ":helpfind"
 */
/*ARGSUSED*/
    void
ex_helpfind(eap)
    exarg_T	*eap;
{
    /* This will fail when menus are not loaded.  Well, it's only for
     * backwards compatibility anyway. */
    do_cmdline_cmd((char_u *)"emenu ToolBar.FindHelp");
}

/*  gui_gtk_position_in_parent
 *
 *  this function causes a child window to be placed within the boundary of
 *  the parent (mainwin) window.
 *
 *  you can specify where the window will be positioned by the third argument
 *  (defined in gui.h):
 *	VW_POS_CENTER		at center of parent window
 *	VW_POS_MOUSE		center of child at mouse position
 *	VW_POS_TOP_CENTER	top of child at top of parent centered
 *				horizontally about the mouse.
 *
 *  NOTE: for this function to act as desired the child window must have a
 *	  window size requested.  this can be accomplished by packing/placing
 *	  child widgets onto a gtk_frame widget rather than the gtk_window
 *	  widget...
 *
 *  brent -- dbv
 */
    void
gui_gtk_position_in_parent(
	GtkWidget	*parent,
	GtkWidget	*child,
	gui_win_pos_T	where)
{
    GtkRequisition	c_size;
    gint		xPm, yPm;
    gint		xP, yP, wP, hP, pos_x, pos_y;

    /* make sure the child widget is set up then get its size. */
    gtk_widget_size_request(child, &c_size);

    /* get origin and size of parent window */
    gdk_window_get_origin((GdkWindow *)(parent->window), &xP, &yP);
    gdk_window_get_size((GdkWindow *)(parent->window), &wP, &hP);

    if (c_size.width > wP || c_size.height > hP)
    {
	/* doh! maybe the user should consider giving gVim a little more
	 * screen real estate */
	gtk_widget_set_uposition(child , xP + 2 , yP + 2);
	return;
    }

    if (where == VW_POS_MOUSE)
    {
	/* position window at mouse pointer */
	gtk_widget_get_pointer(parent, &xPm, &yPm);
	pos_x = xP + xPm - (c_size.width) / 2;
	pos_y = yP + yPm - (c_size.height) / 2;
    }
    else
    {
	/* set child x origin so it is in center of Vim window */
	pos_x =  xP + (wP - c_size.width) / 2;

	if (where == VW_POS_TOP_CENTER)
	    pos_y = yP + 2;
	else
	    /* where == VW_POS_CENTER */
	    pos_y = yP + (hP - c_size.height) / 2;
    }

    /* now, make sure the window will be inside the Vim window... */
    if (pos_x < xP)
	pos_x = xP + 2;
    if (pos_y < yP)
	pos_y = yP + 2;
    if ((pos_x + c_size.width) > (wP + xP))
	pos_x = xP + wP - c_size.width - 2;
    /* Assume 'guiheadroom' indicates the title bar height... */
    if ((pos_y + c_size.height + p_ghr / 2) > (hP + yP))
	pos_y = yP + hP - c_size.height - 2 - p_ghr / 2;

    gtk_widget_set_uposition(child, pos_x, pos_y);
}
