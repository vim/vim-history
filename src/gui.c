/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#include "vim.h"

/* Structure containing all the GUI information */
Gui gui;

/* Set to TRUE after adding/removing menus to ensure they are updated */
int force_menu_update = FALSE;

static void gui_check_screen __ARGS((void));
static void gui_position_components __ARGS((int, int));
static void gui_outstr __ARGS((char_u *, int));
static void gui_delete_lines __ARGS((int row, int count));
static void gui_insert_lines __ARGS((int row, int count));
static int gui_get_menu_cmd_modes __ARGS((char_u *, int, int *, int *));
static char_u *popup_mode_name __ARGS((char_u *name, int idx));
static void  gui_update_menus_recurse __ARGS((GuiMenu *, int));
#ifdef USE_GUI_WIN32
static int gui_add_menu_path __ARGS((char_u *, int, int *, void (*)(), char_u *, int, int));
#else
static int gui_add_menu_path __ARGS((char_u *, int, int *, void (*)(), char_u *, int));
#endif
static int gui_remove_menu __ARGS((GuiMenu **, char_u *, int, int silent));
static void gui_free_menu __ARGS((GuiMenu *));
static void gui_free_menu_string __ARGS((GuiMenu *, int));
static int gui_show_menus __ARGS((char_u *, int));
static void gui_show_menus_recursive __ARGS((GuiMenu *, int, int));
static int menu_name_equal __ARGS((char_u *name, GuiMenu *menu));
static int menu_namecmp __ARGS((char_u *name, char_u *mname));
static void gui_create_initial_menus __ARGS((GuiMenu *, GuiMenu *));
static void gui_update_scrollbars __ARGS((int));
static void gui_update_horiz_scrollbar __ARGS((int));
static WIN *y2win __ARGS((int y));
static int get_menu_mode __ARGS((void));
#ifdef USE_GUI_WIN32
static void gui_create_tearoffs_recurse __ARGS((GuiMenu *menu, const char_u *pname, int *pri_tab, int pri_idx));
static void gui_add_tearoff __ARGS((char_u *tearpath, int *pri_tab, int pri_idx));
static void gui_destroy_tearoffs_recurse __ARGS((GuiMenu *menu));
static int s_tearoffs = FALSE;
#endif

/*
 * The Athena scrollbars can move the thumb to after the end of the scrollbar,
 * this makes the thumb indicate the part of the text that is shown.  Motif
 * can't do this.
 */
#if defined(USE_GUI_ATHENA) || defined(macintosh)
# define SCROLL_PAST_END
#endif

/*
 * While defining the system menu, gui_sys_menu is TRUE.  This avoids
 * overruling of menus that the user already defined.
 */
static int	gui_sys_menu = FALSE;

/*
 * gui_start -- Called when user wants to start the GUI.
 */
    void
gui_start()
{
    char_u  *old_term;
#if defined(UNIX) && !defined(__BEOS__)
    pid_t   pid;
#endif

    old_term = vim_strsave(T_NAME);
    mch_setmouse(FALSE);		/* first switch mouse off */

    /*
     * Set_termname() will call gui_init() to start the GUI.
     * Set the "starting" flag, to indicate that the GUI will start.
     *
     * We don't want to open the GUI window until after we've read .gvimrc,
     * otherwise we don't know what font we will use, and hence we don't know
     * what size the window should be.	So if there are errors in the .gvimrc
     * file, they will have to go to the terminal: Set full_screen to FALSE.
     * full_screen will be set to TRUE again by a successful termcapinit().
     */
    settmode(TMODE_COOK);		/* stop RAW mode */
    if (full_screen)
	cursor_on();			/* needed for ":gui" in .vimrc */
    gui.starting = TRUE;
    full_screen = FALSE;
    termcapinit((char_u *)"builtin_gui");
    gui.starting = FALSE;

    if (!gui.in_use)			/* failed to start GUI */
    {
	termcapinit(old_term);		/* back to old term settings */
	settmode(TMODE_RAW);		/* restart RAW mode */
	set_title_defaults();		/* set 'title' and 'icon' again */
    }

    vim_free(old_term);

#if defined(UNIX) && !defined(__BEOS__)
    /*
     * Quit the current process and continue in the child.
     * Makes "gvim file" disconnect from the shell it was started in.
     * Don't do this when Vim was started with "-f" or the 'f' flag is present
     * in 'guioptions'.
     */
    if (gui.in_use && gui.dofork && vim_strchr(p_guioptions, GO_FORG) == NULL)
    {
	pid = fork();
	if (pid > 0)	    /* Parent */
	    exit(0);
#if defined(HAVE_SETSID) || defined(HAVE_SETPGID)
	/*
	 * Change our process group.  On some systems/shells a CTRL-C in the
	 * shell where Vim was started would otherwise kill gvim!
	 */
	if (pid == 0)	    /* child */
# if defined(HAVE_SETSID)
	    (void)setsid();
# else
	    (void)setpgid(0, 0);
# endif
#endif
    }
#endif
}

/*
 * Call this when vim starts up, whether or not the GUI is started
 */
    void
gui_prepare(argc, argv)
    int	    *argc;
    char    **argv;
{
    /* Menu items may be added before the GUI is started, so set this now */
    gui.root_menu = NULL;
    gui.in_use = FALSE;		    /* No GUI yet (maybe later) */
    gui.starting = FALSE;	    /* No GUI yet (maybe later) */
    gui.dofork = TRUE;		    /* default is to use fork() */
    gui_mch_prepare(argc, argv);
}

/*
 * This is the call which starts the GUI.
 */
    void
gui_init()
{
    WIN		*wp;
    static int	recursive = 0;

    /*
     * It's possible to use ":gui" in a .gvimrc file.  The first halve of this
     * function will then be executed at the first call, the rest by the
     * recursive call.  This allow the window to be opened halfway reading a
     * gvimrc file.
     */
    if (!recursive)
    {
	++recursive;

	gui.window_created = FALSE;
	gui.dying = FALSE;
	gui.in_focus = TRUE;		/* so the guicursor setting works */
	gui.dragged_sb = SBAR_NONE;
	gui.dragged_wp = NULL;
	gui.pointer_hidden = FALSE;
	gui.col = gui.num_cols = 0;
	gui.row = gui.num_rows = 0;

	gui.cursor_is_valid = FALSE;
	gui.scroll_region_top = 0;
	gui.scroll_region_bot = Rows - 1;
	gui.highlight_mask = HL_NORMAL;
	gui.char_width = 1;
	gui.char_height = 1;
	gui.char_ascent = 0;
	gui.border_width = 0;

	gui.norm_font = (GuiFont)NULL;
	gui.bold_font = (GuiFont)NULL;
	gui.ital_font = (GuiFont)NULL;
	gui.boldital_font = (GuiFont)NULL;

	clip_init(TRUE);

	gui.menu_is_active = TRUE;	    /* default: include menu */

	gui.scrollbar_width = gui.scrollbar_height = SB_DEFAULT_WIDTH;
	gui.menu_height = MENU_DEFAULT_HEIGHT;
	gui.menu_width = 0;

	gui.prev_wrap = -1;

	/*
	 * Set up system-wide default menus.
	 */
#ifdef SYS_MENU_FILE
	gui_sys_menu = TRUE;
	do_source((char_u *)SYS_MENU_FILE, FALSE, FALSE);
	gui_sys_menu = FALSE;
#endif

	/*
	 * Switch on the mouse by default, unless the user changed it already.
	 * This can then be changed in the .gvimrc.
	 */
	if (!option_was_set((char_u *)"mouse"))
	    set_string_option_direct((char_u *)"mouse", -1,
							 (char_u *)"a", TRUE);

	/*
	 * If -U option given, use only the initializations from that file and
	 * nothing else.  Skip all initializations for "-U NONE".
	 */
	if (use_gvimrc != NULL)
	{
	    if (STRCMP(use_gvimrc, "NONE") != 0
		    && do_source(use_gvimrc, FALSE, FALSE) != OK)
		EMSG2("Cannot read from \"%s\"", use_gvimrc);
	}
	else
	{
	    /*
	     * Get system wide defaults for gvim, only when file name defined.
	     */
#ifdef SYS_GVIMRC_FILE
	    do_source((char_u *)SYS_GVIMRC_FILE, FALSE, FALSE);
#endif

	    /*
	     * Try to read GUI initialization commands from the following
	     * places:
	     * - environment variable GVIMINIT
	     * - the user gvimrc file (~/.gvimrc)
	     * - the second user gvimrc file ($VIM/.gvimrc for Dos)
	     * The first that exists is used, the rest is ignored.
	     */
	    if (process_env((char_u *)"GVIMINIT") == FAIL
		 && do_source((char_u *)USR_GVIMRC_FILE, TRUE, FALSE) == FAIL)
		{
#ifdef USR_GVIMRC_FILE2
		    (void)do_source((char_u *)USR_GVIMRC_FILE2, TRUE, FALSE);
#endif
		}

	    /*
	     * Read initialization commands from ".gvimrc" in current
	     * directory.  This is only done if the 'exrc' option is set.
	     * Because of security reasons we disallow shell and write
	     * commands now, except for unix if the file is owned by the user
	     * or 'secure' option has been reset in environment of global
	     * ".gvimrc".
	     * Only do this if GVIMRC_FILE is not the same as USR_GVIMRC_FILE,
	     * USR_GVIMRC_FILE2 or SYS_GVIMRC_FILE.
	     */
	    if (p_exrc)
	    {
#ifdef UNIX
		{
		    struct stat s;

		    /* if ".gvimrc" file is not owned by user, set 'secure'
		     * mode */
		    if (stat(GVIMRC_FILE, &s) || s.st_uid != getuid())
			secure = p_secure;
		}
#else
		secure = p_secure;
#endif

		if (       fullpathcmp((char_u *)USR_GVIMRC_FILE,
				     (char_u *)GVIMRC_FILE, FALSE) != FPC_SAME
#ifdef SYS_GVIMRC_FILE
			&& fullpathcmp((char_u *)SYS_GVIMRC_FILE,
				     (char_u *)GVIMRC_FILE, FALSE) != FPC_SAME
#endif
#ifdef USR_GVIMRC_FILE2
			&& fullpathcmp((char_u *)USR_GVIMRC_FILE2,
				     (char_u *)GVIMRC_FILE, FALSE) != FPC_SAME
#endif
			)
		    do_source((char_u *)GVIMRC_FILE, FALSE, FALSE);

		if (secure == 2)
		    need_wait_return = TRUE;
		secure = 0;
	    }
	}

	if (need_wait_return || msg_didany)
	    wait_return(TRUE);

	--recursive;
    }

    /* If recursive call opened the window, return here from the first call */
    if (gui.in_use)
	return;

    /*
     * Create the GUI windows ready for opening.
     */
    gui.in_use = TRUE;		/* Must be set after menus have been set up */
    if (gui_mch_init() == FAIL)
	goto error;

    /*
     * Check validity of any generic resources that may have been loaded.
     */
    if (gui.border_width < 0)
	gui.border_width = 0;

    /*
     * Set up the fonts.
     */
    if (font_opt)
	set_option_value((char_u *)"gfn", 0L, (char_u *)font_opt);
    if (gui_init_font(p_guifont) == FAIL)
	goto error;

    gui.num_cols = Columns;
    gui.num_rows = Rows;
    gui_reset_scroll_region();

    /* Create initial scrollbars */
    for (wp = firstwin; wp; wp = wp->w_next)
    {
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_LEFT], wp);
	gui_create_scrollbar(&wp->w_scrollbars[SBAR_RIGHT], wp);
    }
    gui_create_scrollbar(&gui.bottom_sbar, NULL);

    gui_create_initial_menus(gui.root_menu, NULL);

    /* Configure the desired menu and scrollbars */
    gui_init_which_components(NULL);

    /* All components of the window have been created now */
    gui.window_created = TRUE;

    gui_set_winsize(TRUE);

    /*
     * Actually open the GUI window.
     */
    if (gui_mch_open() != FAIL)
    {
	maketitle();
	init_gui_options();
	return;
    }

error:
    gui.in_use = FALSE;
    clip_init(FALSE);
}

    void
gui_exit(rc)
    int		rc;
{
    free_highlight_fonts();
    gui.in_use = FALSE;
    gui_mch_exit(rc);
}

/*
 * Set the font. Uses the 'font' option. The first font name that works is
 * used. If none is found, use the default font.
 * Return OK when able to set the font.
 */
    int
gui_init_font(font_list)
    char_u  *font_list;
{
#define FONTLEN 100
    char_u  font_name[FONTLEN];
    int     font_list_empty = FALSE;
    int	    ret = FAIL;

    if (!gui.in_use)
	return FAIL;

    font_name[0] = NUL;
    if (*font_list == NUL)
	font_list_empty = TRUE;
    else
	while (*font_list != NUL)
	{
	    /* Isolate one font name */
	    (void)copy_option_part(&font_list, font_name, FONTLEN, ",");
	    if (gui_mch_init_font(font_name) == OK)
	    {
		ret = OK;
		break;
	    }
	}

    if (ret != OK && STRCMP(font_name, "*") != 0
	    && (font_list_empty || gui.norm_font == (GuiFont)0))
    {
	/*
	 * Couldn't load any font in 'font_list', keep the current font if
	 * there is one.  If 'font_list' is empty, or if there is no current
	 * font, tell gui_mch_init_font() to try to find a font we can load.
	 */
	ret = gui_mch_init_font(NULL);
    }

    if (ret == OK)
    {
	/* Set normal font as current font */
	gui_mch_set_font(gui.norm_font);
	gui_set_winsize(
#ifdef WIN32
		TRUE
#else
		FALSE
#endif
		);
    }

    return ret;
}

    void
gui_set_cursor(row, col)
    int	    row;
    int	    col;
{
    gui.row = row;
    gui.col = col;
}

/*
 * gui_check_screen - check if the cursor is on the screen.
 */
    static void
gui_check_screen()
{
    if (gui.row >= Rows)
	gui.row = Rows - 1;
    if (gui.col >= Columns)
	gui.col = Columns - 1;
    if (gui.cursor_row >= Rows || gui.cursor_col >= Columns)
	gui.cursor_is_valid = FALSE;
}

/*
 * Redraw the cursor if necessary or when forced.
 * Careful: The contents of LinePointers[] must match what is on the screen,
 * otherwise this goes wrong.  May need to call out_flush() first.
 */
    void
gui_update_cursor(force, clear_selection)
    int		force;		/* when TRUE, update even when not moved */
    int		clear_selection;/* clear selection under cursor */
{
    int		cur_width = 0;
    int		cur_height = 0;
    long_u	old_hl_mask;
    int		idx;
    int		id;
    GuiColor	cfg, cbg, cc;	/* cursor fore-/background color */
    int		cattr;		/* cursor attributes */
    int		attr;
    struct attr_entry *aep = NULL;

    gui_check_screen();
    if (!gui.cursor_is_valid || force
		    || gui.row != gui.cursor_row || gui.col != gui.cursor_col)
    {
	gui_undraw_cursor();
	if (gui.row <0)
	    return;
	gui.cursor_row = gui.row;
	gui.cursor_col = gui.col;
	gui.cursor_is_valid = TRUE;

	/* Only write to the screen after LinePointers[] has been initialized */
	if (!screen_cleared || NextScreen == NULL)
	    return;

	/* Clear the selection if we are about to write over it */
	if (clear_selection)
	    clip_may_clear_selection(gui.row, gui.row);
	/* Check that the cursor is inside the window (resizing may have made
	 * it invalid) */
	if (gui.row >= screen_Rows || gui.col >= screen_Columns)
	    return;

	/*
	 * How the cursor is drawn depends on the current mode.
	 */
	idx = get_cursor_idx();
	id = cursor_table[idx].id;

	/* get the colors and attributes for the cursor.  Default is inverted */
	cfg = (GuiColor)-1;
	cbg = (GuiColor)-1;
	cattr = HL_INVERSE;
	gui_mch_set_blinking(cursor_table[idx].blinkwait,
			     cursor_table[idx].blinkon,
			     cursor_table[idx].blinkoff);
	if (id > 0)
	{
	    cattr = syn_id2colors(id, &cfg, &cbg);
	    --cbg;
	    --cfg;
	}

	/*
	 * Get the attributes for the character under the cursor.
	 * When no cursor color was given, use the character color.
	 */
	attr = *(LinePointers[gui.row] + gui.col + screen_Columns);
	if (attr > HL_ALL)
	    aep = syn_gui_attr2entry(attr);
	if (aep != NULL)
	{
	    attr = aep->ae_attr;
	    if (cfg < 0)
		cfg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.bg_color
					    : aep->ae_u.gui.fg_color) - 1;
	    if (cbg < 0)
		cbg = ((attr & HL_INVERSE)  ? aep->ae_u.gui.fg_color
					    : aep->ae_u.gui.bg_color) - 1;
	}
	if (cfg < 0)
	    cfg = (attr & HL_INVERSE) ? gui.back_pixel : gui.norm_pixel;
	if (cbg < 0)
	    cbg = (attr & HL_INVERSE) ? gui.norm_pixel : gui.back_pixel;
	attr &= ~HL_INVERSE;
	if (cattr & HL_INVERSE)
	{
	    cc = cbg;
	    cbg = cfg;
	    cfg = cc;
	}
	cattr &= ~HL_INVERSE;

	/*
	 * When we don't have window focus, draw a hollow cursor.
	 */
	if (!gui.in_focus)
	{
	    gui_mch_draw_hollow_cursor(cbg);
	    return;
	}

	old_hl_mask = gui.highlight_mask;
	if (cursor_table[idx].shape == SHAPE_BLOCK)
	{
	    /*
	     * Draw the text character with the cursor colors.	Use the
	     * character attributes plus the cursor attributes.
	     */
	    gui.highlight_mask = (cattr | attr);
	    gui_outstr_nowrap(LinePointers[gui.row] + gui.col, 1,
			    GUI_MON_IS_CURSOR | GUI_MON_NOCLEAR, cfg, cbg, 0);
	}
	else
	{
	    /*
	     * First draw the partial cursor, then overwrite with the text
	     * character, using a transparant background.
	     */
	    if (cursor_table[idx].shape == SHAPE_VER)
	    {
		cur_height = gui.char_height;
		cur_width = (gui.char_width * cursor_table[idx].percentage
								  + 99) / 100;
	    }
	    else
	    {
		cur_height = (gui.char_height * cursor_table[idx].percentage
								  + 99) / 100;
		cur_width = gui.char_width;
	    }
	    gui_mch_draw_part_cursor(cur_width, cur_height, cbg);

#ifndef USE_GUI_WIN32	    /* doesn't seem to work for Win32 */
	    gui.highlight_mask = *(LinePointers[gui.row] + gui.col
							    + screen_Columns);
	    gui_outstr_nowrap(LinePointers[gui.row] + gui.col, 1,
		    GUI_MON_TRS_CURSOR | GUI_MON_NOCLEAR,
		    (GuiColor)0, (GuiColor)0, 0);
#endif
	}
	gui.highlight_mask = old_hl_mask;
    }
}

    void
gui_position_menu()
{
    if (gui.menu_is_active && gui.in_use)
	gui_mch_set_menu_pos(0, 0, gui.menu_width, gui.menu_height);
}

/*
 * Position the various GUI components (text area, menu).  The vertical
 * scrollbars are NOT handled here.  See gui_update_scrollbars().
 */
/* ARGSUSED */
    static void
gui_position_components(total_width, total_height)
    int	    total_width;
    int	    total_height;
{
    int	    text_area_x;
    int	    text_area_y;
    int	    text_area_width;
    int	    text_area_height;

    gui.menu_width = total_width;

    text_area_x = 0;
    if (gui.which_scrollbars[SBAR_LEFT])
	text_area_x += gui.scrollbar_width;

#ifdef USE_GUI_WIN32
    if (vim_strchr(p_guioptions, GO_TOOLBAR) != NULL)
	text_area_y = TOOLBAR_BUTTON_HEIGHT + 12;
    else
#endif
	text_area_y = 0;

    if (gui.menu_is_active)
	text_area_y += gui.menu_height;
    text_area_width = gui.num_cols * gui.char_width + gui.border_offset * 2;
    text_area_height = gui.num_rows * gui.char_height + gui.border_offset * 2;

    gui_mch_set_text_area_pos(text_area_x,
			      text_area_y,
			      text_area_width,
			      text_area_height);
    gui_position_menu();
    if (gui.which_scrollbars[SBAR_BOTTOM])
	gui_mch_set_scrollbar_pos(&gui.bottom_sbar,
				  text_area_x,
				  text_area_y + text_area_height,
				  text_area_width,
				  gui.scrollbar_height);
    gui.left_sbar_x = 0;
    gui.right_sbar_x = text_area_x + text_area_width;
}

    int
gui_get_base_width()
{
    int	    base_width;

    base_width = 2 * gui.border_offset;
    if (gui.which_scrollbars[SBAR_LEFT])
	base_width += gui.scrollbar_width;
    if (gui.which_scrollbars[SBAR_RIGHT])
	base_width += gui.scrollbar_width;
    return base_width;
}

    int
gui_get_base_height()
{
    int	    base_height;

    base_height = 2 * gui.border_offset;
    if (gui.which_scrollbars[SBAR_BOTTOM])
	base_height += gui.scrollbar_height;
    if (gui.menu_is_active)
	base_height += gui.menu_height;
#ifdef USE_GUI_WIN32
    if (vim_strchr(p_guioptions, GO_TOOLBAR) != NULL)
	base_height += (TOOLBAR_BUTTON_HEIGHT+12);
#endif
    return base_height;
}

/*
 * Should be called after the GUI window has been resized.  Its arguments are
 * the new width and height of the window in pixels.
 */
    void
gui_resize_window(pixel_width, pixel_height)
    int	    pixel_width;
    int	    pixel_height;
{
    if (!gui.window_created)
	return;

#ifdef USE_GUI_BEOS
    vim_lock_screen();
#endif

    /* Flush pending output before redrawing */
    out_flush();

    gui.num_cols = (pixel_width - gui_get_base_width()) / gui.char_width;
    gui.num_rows = (pixel_height - gui_get_base_height()) / gui.char_height;

    gui_position_components(pixel_width, pixel_height);

    gui_reset_scroll_region();
    /*
     * At the "more" and ":confirm" prompt there is no redraw, put the cursor
     * at the last line here (why does it have to be one row too low?).
     */
    if (State == ASKMORE || State == CONFIRM)
	gui.row = gui.num_rows;

    if (gui.num_rows != screen_Rows || gui.num_cols != screen_Columns)
	set_winsize(0, 0, FALSE);

#ifdef USE_GUI_BEOS
    vim_unlock_screen();
#endif

    gui_update_scrollbars(TRUE);
    gui_update_cursor(FALSE, TRUE);
}

    int
gui_get_winsize()
{
    Rows = gui.num_rows;
    Columns = gui.num_cols;
    return OK;
}

/*
 * Set the size of the window according to Rows and Columns.
 */
    void
gui_set_winsize(fit_to_display)
    int	    fit_to_display;
{
    int	    base_width;
    int	    base_height;
    int	    width;
    int	    height;
    int	    min_width;
    int	    min_height;
    int	    screen_w;
    int	    screen_h;

    if (!gui.window_created)
	return;

    base_width = gui_get_base_width();
    base_height = gui_get_base_height();
    width = Columns * gui.char_width + base_width;
    height = Rows * gui.char_height + base_height;

    if (fit_to_display)
    {
	gui_mch_get_screen_dimensions(&screen_w, &screen_h);
	if (width > screen_w)
	{
	    Columns = (screen_w - base_width) / gui.char_width;
	    if (Columns < MIN_COLUMNS)
		Columns = MIN_COLUMNS;
	    gui.num_cols = Columns;
	    gui_reset_scroll_region();
	    width = Columns * gui.char_width + base_width;
	}
	if (height > screen_h)
	{
	    Rows = (screen_h - base_height) / gui.char_height;
	    if (Rows < MIN_LINES)
		Rows = MIN_LINES;
	    gui.num_rows = Rows;
	    gui_reset_scroll_region();
	    height = Rows * gui.char_height + base_height;
	}
    }

    min_width = base_width + MIN_COLUMNS * gui.char_width;
    min_height = base_height + MIN_LINES * gui.char_height;

    gui_mch_set_winsize(width, height, min_width, min_height,
			base_width, base_height);
    gui_position_components(width, height);
    gui_update_scrollbars(TRUE);
}

/*
 * Make scroll region cover whole screen.
 */
    void
gui_reset_scroll_region()
{
    gui.scroll_region_top = 0;
    gui.scroll_region_bot = gui.num_rows - 1;
}

    void
gui_start_highlight(mask)
    int	    mask;
{
    if (mask > HL_ALL)		    /* highlight code */
	gui.highlight_mask = mask;
    else			    /* mask */
	gui.highlight_mask |= mask;
}

    void
gui_stop_highlight(mask)
    int	    mask;
{
    if (mask > HL_ALL)		    /* highlight code */
	gui.highlight_mask = HL_NORMAL;
    else			    /* mask */
	gui.highlight_mask &= ~mask;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    void
gui_clear_block(row1, col1, row2, col2)
    int	    row1;
    int	    col1;
    int	    row2;
    int	    col2;
{
    /* Clear the selection if we are about to write over it */
    clip_may_clear_selection(row1, row2);

    gui_mch_clear_block(row1, col1, row2, col2);

    /* Invalidate cursor if it was in this block */
    if (       gui.cursor_row >= row1 && gui.cursor_row <= row2
	    && gui.cursor_col >= col1 && gui.cursor_col <= col2)
	gui.cursor_is_valid = FALSE;
}

/*
 * Write code to update cursor shape later.
 */
    void
gui_upd_cursor_shape()
{
    OUT_STR("\033|s");
}

    void
gui_write(s, len)
    char_u  *s;
    int	    len;
{
    char_u  *p;
    int	    arg1 = 0, arg2 = 0;
#ifdef RISCOS
    int     force = TRUE;  /* JK230798, stop Vim being smart or our redraw speed will suffer */
#else
    int	    force = FALSE;	/* force cursor update */
#endif

/* #define DEBUG_GUI_WRITE */
#ifdef DEBUG_GUI_WRITE
    {
	int i;
	char_u *str;

	printf("gui_write(%d):\n    ", len);
	for (i = 0; i < len; i++)
	    if (s[i] == ESC)
	    {
		if (i != 0)
		    printf("\n    ");
		printf("<ESC>");
	    }
	    else
	    {
		str = transchar(s[i]);
		if (str[0] && str[1])
		    printf("<%s>", (char *)str);
		else
		    printf("%s", (char *)str);
	    }
	printf("\n");
    }
#endif
    while (len)
    {
	if (s[0] == ESC && s[1] == '|')
	{
	    p = s + 2;
	    if (isdigit(*p))
	    {
		arg1 = getdigits(&p);
		if (p > s + len)
		    break;
		if (*p == ';')
		{
		    ++p;
		    arg2 = getdigits(&p);
		    if (p > s + len)
			break;
		}
	    }
	    switch (*p)
	    {
		case 'C':	/* Clear screen */
		    clip_scroll_selection(9999);
		    gui_mch_clear_all();
		    gui.cursor_is_valid = FALSE;
		    break;
		case 'M':	/* Move cursor */
		    gui_set_cursor(arg1, arg2);
		    break;
		case 's':	/* force cursor (shape) update */
		    force = TRUE;
		    break;
		case 'R':	/* Set scroll region */
		    if (arg1 < arg2)
		    {
			gui.scroll_region_top = arg1;
			gui.scroll_region_bot = arg2;
		    }
		    else
		    {
			gui.scroll_region_top = arg2;
			gui.scroll_region_bot = arg1;
		    }
		    break;
		case 'd':	/* Delete line */
		    gui_delete_lines(gui.row, 1);
		    break;
		case 'D':	/* Delete lines */
		    gui_delete_lines(gui.row, arg1);
		    break;
		case 'i':	/* Insert line */
		    gui_insert_lines(gui.row, 1);
		    break;
		case 'I':	/* Insert lines */
		    gui_insert_lines(gui.row, arg1);
		    break;
		case '$':	/* Clear to end-of-line */
		    gui_clear_block(gui.row, gui.col, gui.row,
							    (int)Columns - 1);
		    break;
		case 'h':	/* Turn on highlighting */
		    gui_start_highlight(arg1);
		    break;
		case 'H':	/* Turn off highlighting */
		    gui_stop_highlight(arg1);
		    break;
		case 'f':	/* flash the window (visual bell) */
		    gui_mch_flash();
		    break;
		default:
		    p = s + 1;	/* Skip the ESC */
		    break;
	    }
	    len -= ++p - s;
	    s = p;
	}
	else if (s[0] < 0x20)		/* Ctrl character */
	{
	    if (s[0] == '\n')		/* NL */
	    {
		gui.col = 0;
		if (gui.row < gui.scroll_region_bot)
		    gui.row++;
		else
		    gui_delete_lines(gui.scroll_region_top, 1);
	    }
	    else if (s[0] == '\r')	/* CR */
	    {
		gui.col = 0;
	    }
	    else if (s[0] == '\b')	/* Backspace */
	    {
		if (gui.col)
		    --gui.col;
	    }
	    else if (s[0] == Ctrl('L'))	/* cursor-right */
	    {
		++gui.col;
	    }
	    else if (s[0] == Ctrl('G'))	/* Beep */
	    {
		gui_mch_beep();
	    }
	    /* Other Ctrl character: shouldn't happen! */

	    --len;	/* Skip this char */
	    ++s;
	}
	else
	{
	    p = s;
	    while (len && *p >= 0x20)
	    {
		len--;
		p++;
	    }
	    gui_outstr(s, p - s);
	    s = p;
	}
    }
    gui_update_cursor(force, TRUE);
    gui_update_scrollbars(FALSE);

    /*
     * We need to make sure this is cleared since Athena doesn't tell us when
     * he is done dragging.
     */
#ifdef USE_GUI_ATHENA
    gui.dragged_sb = SBAR_NONE;
#endif

    if (vim_strchr(p_guioptions, GO_ASEL) != NULL)
	clip_update_selection();
    gui_mch_flush();		    /* In case vim decides to take a nap */
}

    static void
gui_outstr(s, len)
    char_u  *s;
    int	    len;
{
    int	    this_len;

    if (len == 0)
	return;

    if (len < 0)
	len = STRLEN(s);

    while (gui.col + len > Columns)
    {
	this_len = Columns - gui.col;
	gui_outstr_nowrap(s, this_len, GUI_MON_WRAP_CURSOR, (GuiColor)0,
							      (GuiColor)0, 0);
	s += this_len;
	len -= this_len;
    }
    gui_outstr_nowrap(s, len, GUI_MON_WRAP_CURSOR, (GuiColor)0, (GuiColor)0, 0);
}

/*
 * Output the given string at the current cursor position.  If the string is
 * too long to fit on the line, then it is truncated.
 * "flags":
 * GUI_MON_WRAP_CURSOR may be used if the cursor position should be wrapped
 * when the end of the line is reached, however the string will still be
 * truncated and not continue on the next line.
 * GUI_MON_IS_CURSOR should only be used when this function is being called to
 * actually draw (an inverted) cursor.
 * GUI_MON_TRS_CURSOR is used to draw the cursor text with a transparant
 * background.
 */
    void
gui_outstr_nowrap(s, len, flags, fg, bg, back)
    char_u	*s;
    int		len;
    int		flags;
    GuiColor	fg, bg;	    /* colors for cursor */
    int		back;	    /* backup this many chars when using bold trick */
{
    long_u	    highlight_mask;
    GuiColor	    fg_color;
    GuiColor	    bg_color;
    GuiFont	    font;
    struct attr_entry *aep = NULL;
    int		    draw_flags;
    int		    col = gui.col;

    if (len == 0)
	return;

    if (len < 0)
	len = STRLEN(s);

    if (gui.highlight_mask > HL_ALL)
    {
	aep = syn_gui_attr2entry(gui.highlight_mask);
	if (aep == NULL)	    /* highlighting not set */
	    highlight_mask = 0;
	else
	    highlight_mask = aep->ae_attr;
    }
    else
	highlight_mask = gui.highlight_mask;

    /* Set the font */
    if (aep != NULL && aep->ae_u.gui.font != 0)
	font = aep->ae_u.gui.font;
    else
    {
	if ((highlight_mask & (HL_BOLD | HL_STANDOUT)) && gui.bold_font != 0)
	{
	    if ((highlight_mask & HL_ITALIC) && gui.boldital_font != 0)
		font = gui.boldital_font;
	    else
		font = gui.bold_font;
	}
	else if ((highlight_mask & HL_ITALIC) && gui.ital_font != 0)
	    font = gui.ital_font;
	else
	    font = gui.norm_font;
    }
    gui_mch_set_font(font);

    /* Set the color */
    bg_color = gui.back_pixel;
    if ((flags & GUI_MON_IS_CURSOR) && gui.in_focus)
    {
	fg_color = fg;
	bg_color = bg;
    }
    else if (aep != NULL)
    {
	fg_color = aep->ae_u.gui.fg_color;
	if (fg_color == 0)
	    fg_color = gui.norm_pixel;
	else
	    --fg_color;
	bg_color = aep->ae_u.gui.bg_color;
	if (bg_color == 0)
	    bg_color = gui.back_pixel;
	else
	    --bg_color;
    }
    else
	fg_color = gui.norm_pixel;

    if (highlight_mask & (HL_INVERSE | HL_STANDOUT))
    {
#if defined(AMIGA) || defined(RISCOS)
	gui_mch_set_colors(bg_color, fg_color);
#else
	gui_mch_set_fg_color(bg_color);
	gui_mch_set_bg_color(fg_color);
#endif
    }
    else
    {
#if defined(AMIGA) || defined(RISCOS)
	gui_mch_set_colors(fg_color, bg_color);
#else
	gui_mch_set_fg_color(fg_color);
	gui_mch_set_bg_color(bg_color);
#endif
    }

    /* Clear the selection if we are about to write over it */
    if (!(flags & GUI_MON_NOCLEAR))
	clip_may_clear_selection(gui.row, gui.row);

    draw_flags = 0;

    /* If there's no bold font, then fake it */
    if ((highlight_mask & (HL_BOLD | HL_STANDOUT)) &&
	    (gui.bold_font == 0 || (aep != NULL && aep->ae_u.gui.font != 0)))
    {
	draw_flags |= DRAW_BOLD;
	s -= back;
	len += back;
	col -= back;
    }

#ifdef RISCOS
    /* If there's no italic font, then fake it */
    if ((highlight_mask & HL_ITALIC) && gui.ital_font == 0)
	draw_flags |= DRAW_ITALIC;

    /* Do we underline the text? */
    if (highlight_mask & HL_UNDERLINE)
	draw_flags |= DRAW_UNDERL;
#else
    /* Do we underline the text? */
    if ((highlight_mask & HL_UNDERLINE) ||
	    ((highlight_mask & HL_ITALIC) && gui.ital_font == 0))
	draw_flags |= DRAW_UNDERL;
#endif

    /* Do we draw transparantly? */
    if ((flags & GUI_MON_TRS_CURSOR))
	draw_flags |= DRAW_TRANSP;

    /* Draw the text */
    gui_mch_draw_string(gui.row, col, s, len, draw_flags);

    /* May need to invert it when it's part of the selection (assumes len==1) */
    if (flags & GUI_MON_NOCLEAR)
	clip_may_redraw_selection(gui.row, col);

    if (!(flags & (GUI_MON_IS_CURSOR | GUI_MON_TRS_CURSOR)))
    {
	/* Invalidate the old physical cursor position if we wrote over it */
	if (gui.cursor_row == gui.row && gui.cursor_col >= col
		&& gui.cursor_col < col + len)
	    gui.cursor_is_valid = FALSE;

	/* Update the cursor position */
	gui.col = col + len;
	if ((flags & GUI_MON_WRAP_CURSOR) && gui.col >= Columns)
	{
	    gui.col = 0;
	    gui.row++;
	}
    }
}

/*
 * Un-draw the cursor.	Actually this just redraws the character at the given
 * position.  The character just before it too, for when it was in bold.
 */
    void
gui_undraw_cursor()
{
    if (gui.cursor_is_valid)
    {
	if (gui_redraw_block(gui.cursor_row, gui.cursor_col,
			      gui.cursor_row, gui.cursor_col, GUI_MON_NOCLEAR)
		&& gui.cursor_col > 0)
	    (void)gui_redraw_block(gui.cursor_row, gui.cursor_col - 1,
			 gui.cursor_row, gui.cursor_col - 1, GUI_MON_NOCLEAR);
    }
}

    void
gui_redraw(x, y, w, h)
    int	    x;
    int	    y;
    int	    w;
    int	    h;
{
    int	    row1, col1, row2, col2;

    row1 = Y_2_ROW(y);
    col1 = X_2_COL(x);
    row2 = Y_2_ROW(y + h - 1);
    col2 = X_2_COL(x + w - 1);

    (void)gui_redraw_block(row1, col1, row2, col2, 0);

    /*
     * We may need to redraw the cursor, but don't take it upon us to change
     * its location after a scroll.
     * (maybe be more strict even and test col too?)
     * These things may be outside the update/clipping region and reality may
     * not reflect Vims internal ideas if these operations are clipped away.
     */
    if (gui.row == gui.cursor_row)
	gui_update_cursor(FALSE, TRUE);

    if (clipboard.state != SELECT_CLEARED)
	clip_redraw_selection(x, y, w, h);
}

/*
 * Draw a rectangular block of characters, from row1 to row2 (inclusive) and
 * from col1 to col2 (inclusive).
 * Return TRUE when the character before the first drawn character has
 * different attributes (may have to be redrawn too).
 */
    int
gui_redraw_block(row1, col1, row2, col2, flags)
    int	    row1;
    int	    col1;
    int	    row2;
    int	    col2;
    int	    flags;	/* flags for gui_outstr_nowrap() */
{
    int	    old_row, old_col;
    long_u  old_hl_mask;
    char_u  *screenp, *attrp, first_attr;
    int	    idx, len;
    int	    back;
    int	    retval = FALSE;

    /* Don't try to update when NextScreen is not valid */
    if (!screen_cleared || NextScreen == NULL)
	return retval;

    /* Don't try to draw outside the window! */
    /* Check everything, strange values may be caused by big border width */
    col1 = check_col(col1);
    col2 = check_col(col2);
    row1 = check_row(row1);
    row2 = check_row(row2);

    /* Remember where our cursor was */
    old_row = gui.row;
    old_col = gui.col;
    old_hl_mask = gui.highlight_mask;

    for (gui.row = row1; gui.row <= row2; gui.row++)
    {
	gui.col = col1;
	screenp = LinePointers[gui.row] + gui.col;
	attrp = screenp + screen_Columns;
	len = col2 - col1 + 1;

	/* Find how many chars back this highlighting starts, or where a space
	 * is.  Needed for when the bold trick is used */
	for (back = 0; back < col1; ++back)
	    if (attrp[-1 - back] != attrp[0] || screenp[-1 - back] == ' ')
		break;
	retval = (col1 && attrp[-1] && back == 0);

	/* break it up in strings of characters with the same attributes */
	while (len > 0)
	{
	    first_attr = attrp[0];
	    for (idx = 0; len > 0 && attrp[idx] == first_attr; idx++)
		--len;
	    gui.highlight_mask = first_attr;
	    gui_outstr_nowrap(screenp, idx, flags,
					      (GuiColor)0, (GuiColor)0, back);
	    screenp += idx;
	    attrp += idx;
	    back = 0;
	}
    }

    /* Put the cursor back where it was */
    gui.row = old_row;
    gui.col = old_col;
    gui.highlight_mask = old_hl_mask;

    return retval;
}

    static void
gui_delete_lines(row, count)
    int	    row;
    int	    count;
{
    if (row == 0)
	clip_scroll_selection(count);
    gui_mch_delete_lines(row, count);
}

    static void
gui_insert_lines(row, count)
    int	    row;
    int	    count;
{
    if (row == 0)
	clip_scroll_selection(-count);
    gui_mch_insert_lines(row, count);
}

/*
 * Check bounds for column number
 */
    int
check_col(col)
    int	    col;
{
    if (col < 0)
	return 0;
    if (col >= (int)screen_Columns)
	return (int)screen_Columns - 1;
    return col;
}

/*
 * Check bounds for row number
 */
    int
check_row(row)
    int	    row;
{
    if (row < 0)
	return 0;
    if (row >= (int)screen_Rows)
	return (int)screen_Rows - 1;
    return row;
}

/*
 * The main GUI input routine.	Waits for a character from the keyboard.
 * wtime == -1	    Wait forever.
 * wtime == 0	    Don't wait.
 * wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    int
gui_wait_for_chars(wtime)
    long    wtime;
{
    int	    retval;

    /*
     * If we're going to wait a bit, update the menus for the current
     * State.
     */
    if (wtime != 0)
	gui_update_menus(0);
    gui_mch_update();
    if (!vim_is_input_buf_empty())	/* Got char, return immediately */
	return OK;
    if (wtime == 0)	/* Don't wait for char */
	return FAIL;
    if (wtime > 0)
    {
	/* Blink when waiting for a character.	Probably only does something
	 * for showmatch() */
	gui_mch_start_blink();
	retval = gui_mch_wait_for_chars(wtime);
	gui_mch_stop_blink();
	return retval;
    }

    /*
     * While we are waiting indefenitely for a character, blink the cursor.
     */
    gui_mch_start_blink();
    if (gui_mch_wait_for_chars(p_ut) == OK)
	retval = OK;
    else
    {
	/*
	 * If no characters arrive within 'updatetime' milli-seconds, flush
	 * all the swap files to disk.
	 */
	updatescript(0);
	retval = gui_mch_wait_for_chars(-1L);
    }
    gui_mch_stop_blink();
    return retval;
}

/*
 * Generic mouse support function.  Add a mouse event to the input buffer with
 * the given properties.
 *  button	    --- may be any of MOUSE_LEFT, MOUSE_MIDDLE, MOUSE_RIGHT,
 *			MOUSE_DRAG, or MOUSE_RELEASE.
 *  x, y	    --- Coordinates of mouse in pixels.
 *  repeated_click  --- TRUE if this click comes only a short time after a
 *			previous click.
 *  modifiers	    --- Bit field which may be any of the following modifiers
 *			or'ed together: MOUSE_SHIFT | MOUSE_CTRL | MOUSE_ALT.
 * This function will ignore drag events where the mouse has not moved to a new
 * character.
 */
    void
gui_send_mouse_event(button, x, y, repeated_click, modifiers)
    int	    button;
    int	    x;
    int	    y;
    int	    repeated_click;
    int_u   modifiers;
{
    static int	    prev_row = 0, prev_col = 0;
    static int	    prev_button = -1;
    static int	    num_clicks = 1;
    char_u	    string[6];
    int		    row, col;

#ifdef USE_CLIPBOARD

    int		    checkfor;
    int		    did_clip = FALSE;

    /* If a clipboard selection is in progress, handle it */
    if (clipboard.state == SELECT_IN_PROGRESS)
    {
	clip_process_selection(button, x, y, repeated_click, modifiers);
	return;
    }

    /* Determine which mouse settings to look for based on the current mode */
    switch (get_real_state())
    {
	case NORMAL_BUSY:
	case OP_PENDING:
	case NORMAL:	    checkfor = MOUSE_NORMAL;	break;
	case VISUAL:	    checkfor = MOUSE_VISUAL;	break;
	case REPLACE:
	case INSERT:	    checkfor = MOUSE_INSERT;	break;
	case HITRETURN:	    checkfor = MOUSE_RETURN;	break;

	    /*
	     * On the command line, use the clipboard selection on all lines
	     * but the command line.  But not when pasting.
	     */
	case CMDLINE:
	    if (Y_2_ROW(y) < cmdline_row && button != MOUSE_MIDDLE)
		checkfor = ' ';
	    else
		checkfor = MOUSE_COMMAND;
	    break;

	default:
	    checkfor = ' ';
	    break;
    };

    /*
     * Allow clipboard selection of text on the command line in "normal"
     * modes.  Don't do this when dragging the status line, or extending a
     * Visual selection.
     */
    if ((State == NORMAL || State == NORMAL_BUSY
				       || State == INSERT || State == REPLACE)
	    && Y_2_ROW(y) >= gui.num_rows - p_ch
	    && button != MOUSE_DRAG)
	checkfor = ' ';

    /*
     * If the mouse settings say to not use the mouse, use the non-Visual mode
     * selection.  But if Visual is active, assume that only the Visual area
     * will be selected.
     * Exception: On the command line, both the selection is used and a mouse
     * key is send.
     */
    if (!mouse_has(checkfor) || checkfor == MOUSE_COMMAND)
    {
	/* Don't do non-visual selection in Visual mode. */
	if (VIsual_active)
	    return;

	/*
	 * When 'mousemodel' is "popup", shift-left is translated to right.
	 */
	if (mouse_model_popup())
	{
	    if (button == MOUSE_LEFT && (modifiers & MOUSE_SHIFT))
	    {
		button = MOUSE_RIGHT;
		modifiers &= ~ MOUSE_SHIFT;
	    }
	}

	/* If the selection is done, allow the right button to extend it.
	 * If the selection is cleared, allow the right button to start it
	 * from the cursor position. */
	if (button == MOUSE_RIGHT)
	{
	    if (clipboard.state == SELECT_CLEARED)
	    {
		if (State == CMDLINE)
		{
		    col = msg_col;
		    row = msg_row;
		}
		else
		{
		    col = curwin->w_wcol;
		    row = curwin->w_wrow + curwin->w_winpos;
		}
		clip_start_selection(MOUSE_LEFT, FILL_X(col), FILL_Y(row),
								    FALSE, 0);
	    }
	    clip_process_selection(button, x, y, repeated_click, modifiers);
	    did_clip = TRUE;
	}
	/* Allow the left button to start the selection */
	else if (button ==
# ifdef RISCOS
		/* Only start a drag on a drag event. Otherwise
		 * we don't get a release event.
		 */
		    MOUSE_DRAG
# else
		    MOUSE_LEFT
# endif
				)
	{
	    clip_start_selection(button, x, y, repeated_click, modifiers);
	    did_clip = TRUE;
	}
# ifdef RISCOS
	else if (button == MOUSE_LEFT)
	{
	    clip_clear_selection();
	    did_clip = TRUE;
	}
# endif

	/* Always allow pasting */
	if (button != MOUSE_MIDDLE)
	{
	    if (!mouse_has(checkfor) || button == MOUSE_RELEASE)
		return;
	    button = MOUSE_LEFT;
	}
	repeated_click = FALSE;
    }

    if (clipboard.state != SELECT_CLEARED && !did_clip)
	clip_clear_selection();
#endif

    /*
     * Don't put mouse events in the input queue while executing an external
     * command.
     */
    if (!termcap_active)
	return;

    row = check_row(Y_2_ROW(y));
    col = check_col(X_2_COL(x));

    /*
     * If we are dragging and the mouse hasn't moved far enough to be on a
     * different character, then don't send an event to vim.
     */
    if (button == MOUSE_DRAG && row == prev_row && col == prev_col)
	return;

    /*
     * If topline has changed (window scrolled) since the last click, reset
     * repeated_click, because we don't want starting Visual mode when
     * clicking on a different character in the text.
     */
    if (curwin->w_topline != gui_prev_topline)
	repeated_click = FALSE;

    string[0] = CSI;	/* this sequence is recognized by check_termcode() */
    string[1] = KS_MOUSE;
    string[2] = K_FILLER;
    if (button != MOUSE_DRAG && button != MOUSE_RELEASE)
    {
	if (repeated_click)
	{
	    /*
	     * Handle multiple clicks.	They only count if the mouse is still
	     * pointing at the same character.
	     */
	    if (button != prev_button || row != prev_row || col != prev_col)
		num_clicks = 1;
	    else if (++num_clicks > 4)
		num_clicks = 1;
	}
	else
	    num_clicks = 1;
	prev_button = button;
	gui_prev_topline = curwin->w_topline;

	string[3] = (char_u)(button | 0x20);
	SET_NUM_MOUSE_CLICKS(string[3], num_clicks);
    }
    else
	string[3] = (char_u)button;

    string[3] |= modifiers;
    string[4] = (char_u)(col + ' ' + 1);
    string[5] = (char_u)(row + ' ' + 1);
    add_to_input_buf(string, 6);

    prev_row = row;
    prev_col = col;
}

/*
 * Menu stuff.
 */

    void
gui_menu_cb(menu)
    GuiMenu *menu;
{
    char_u  bytes[3 + sizeof(long_u)];

    bytes[0] = CSI;
    bytes[1] = KS_MENU;
    bytes[2] = K_FILLER;
    add_long_to_buf((long_u)menu, bytes + 3);
    add_to_input_buf(bytes, 3 + sizeof(long_u));
}

/*
 * Return the index into the menu->strings or menu->noremap arrays for the
 * current state.  Returns MENU_INDEX_INVALID if there is no mapping for the
 * given menu in the current mode.
 */
    int
gui_get_menu_index(menu, state)
    GuiMenu *menu;
    int	    state;
{
    int	    idx;

    if (VIsual_active)
	idx = MENU_INDEX_VISUAL;
    else if ((state & INSERT))
	idx = MENU_INDEX_INSERT;
    else if ((state & CMDLINE))
	idx = MENU_INDEX_CMDLINE;
    else if (finish_op)
	idx = MENU_INDEX_OP_PENDING;
    else if ((state & NORMAL))
	idx = MENU_INDEX_NORMAL;
    else
	idx = MENU_INDEX_INVALID;

    if (idx != MENU_INDEX_INVALID && menu->strings[idx] == NULL)
	idx = MENU_INDEX_INVALID;
    return idx;
}

/*
 * Return the modes specified by the given menu command (eg :menu! returns
 * MENU_CMDLINE_MODE | MENU_INSERT_MODE).  If noremap is not NULL, then the
 * flag it points to is set according to whether the command is a "nore"
 * command.  If unmenu is not NULL, then the flag it points to is set
 * according to whether the command is an "unmenu" command.
 */
    static int
gui_get_menu_cmd_modes(cmd, forceit, noremap, unmenu)
    char_u  *cmd;
    int	    forceit;	    /* Was there a "!" after the command? */
    int	    *noremap;
    int	    *unmenu;
{
    int	    modes;

    switch (*cmd++)
    {
	case 'v':			/* vmenu, vunmenu, vnoremenu */
	    modes = MENU_VISUAL_MODE;
	    break;
	case 'o':			/* omenu */
	    modes = MENU_OP_PENDING_MODE;
	    break;
	case 'i':			/* imenu */
	    modes = MENU_INSERT_MODE;
	    break;
	case 't':
	    modes = MENU_TIP_MODE;	/* tmenu */
	    break;
	case 'c':			/* cmenu */
	    modes = MENU_CMDLINE_MODE;
	    break;
	case 'a':			/* amenu */
	    modes = MENU_INSERT_MODE | MENU_CMDLINE_MODE | MENU_NORMAL_MODE
				    | MENU_VISUAL_MODE | MENU_OP_PENDING_MODE;
	    break;
	case 'n':
	    if (cmd[1] != 'o')		/* nmenu */
	    {
		modes = MENU_NORMAL_MODE;
		break;
	    }
	    /* FALLTHROUGH */
	default:
	    --cmd;
	    if (forceit)		/* menu!! */
		modes = MENU_INSERT_MODE | MENU_CMDLINE_MODE;
	    else			/* menu */
		modes = MENU_NORMAL_MODE | MENU_VISUAL_MODE
						       | MENU_OP_PENDING_MODE;
    }

    if (noremap != NULL)
	*noremap = (*cmd == 'n');
    if (unmenu != NULL)
	*unmenu = (*cmd == 'u');
    return modes;
}

#define MENUDEPTH   10		/* maximum depth of menus */

/*
 * Do the :menu commands.
 */
    void
do_menu(eap)
    EXARG	*eap;		    /* Ex command arguments */
{
    char_u	*menu_path;
    int		modes;
    char_u	*map_to;
    int		noremap;
    int		unmenu;
    char_u	*map_buf;
    char_u	*arg;
    char_u	*p;
    int		i;
    int		old_menu_height;
    int		pri_tab[MENUDEPTH + 1];

    modes = gui_get_menu_cmd_modes(eap->cmd, eap->forceit, &noremap, &unmenu);
    arg = eap->arg;

    /* fill in the priority table */
    for (p = arg; *p; ++p)
	if (!isdigit(*p) && *p != '.')
	    break;
    if (vim_iswhite(*p))
    {
	for (i = 0; i < MENUDEPTH && !vim_iswhite(*arg); ++i)
	{
	    pri_tab[i] = getdigits(&arg);
	    if (pri_tab[i] == 0)
		pri_tab[i] = 500;
	    if (*arg == '.')
		++arg;
	}
	arg = skipwhite(arg);
    }
    else if (eap->addr_count)
    {
	pri_tab[0] = eap->line2;
	i = 1;
    }
    else
	i = 0;
    while (i < MENUDEPTH)
	pri_tab[i++] = 500;
    pri_tab[MENUDEPTH] = -1;		/* mark end of the table */

    menu_path = arg;
    if (*menu_path == NUL)
    {
	gui_show_menus(menu_path, modes);
	return;
    }

    /* skip the menu name, and translate <Tab> into a real TAB */
    while (*arg && !vim_iswhite(*arg))
    {
	if ((*arg == '\\' || *arg == Ctrl('V')) && arg[1] != NUL)
	    arg++;
	else if (STRNICMP(arg, "<TAB>", 5) == 0)
	{
	    *arg = TAB;
	    mch_memmove(arg + 1, arg + 5, STRLEN(arg + 4));
	}
	arg++;
    }
    if (*arg != NUL)
	*arg++ = NUL;
    arg = skipwhite(arg);
    map_to = arg;
    if (*map_to == NUL && !unmenu)
    {
	gui_show_menus(menu_path, modes);
	return;
    }
    else if (*map_to != NUL && unmenu)
    {
	EMSG("Trailing characters");
	return;
    }

    old_menu_height = gui.menu_height;
    if (unmenu)
    {
	if (STRCMP(menu_path, "*") == 0)	/* meaning: remove all menus */
	    menu_path = (char_u *)"";
	gui_remove_menu(&gui.root_menu, menu_path, modes, FALSE);

	/*
	 * For the PopUp menu, remove a menu for each mode separately.
	 */
	if (STRNCMP(menu_path, "PopUp", 5) == 0)
	{
	    for (i = 0; i < MENU_INDEX_TIP; ++i)
		if (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    if (p != NULL)
		    {
			gui_remove_menu(&gui.root_menu, p, 1 << i, TRUE);
			vim_free(p);
		    }
		}
	}
    }
    else
    {
	/* Replace special key codes */
	map_to = replace_termcodes(map_to, &map_buf, FALSE, TRUE);
	gui_add_menu_path(menu_path, modes,
		pri_tab, gui_menu_cb, map_to, noremap
#ifdef USE_GUI_WIN32
		, TRUE
#endif
		);

	/*
	 * For the PopUp menu, add a menu for each mode separately.
	 */
	if (STRNCMP(menu_path, "PopUp", 5) == 0)
	{
	    for (i = 0; i < MENU_INDEX_TIP; ++i)
		if (modes & (1 << i))
		{
		    p = popup_mode_name(menu_path, i);
		    if (p != NULL)
		    {
			/* Include all modes, to make ":amenu" work */
			gui_add_menu_path(p, modes,
				pri_tab, gui_menu_cb, map_to, noremap
#ifdef USE_GUI_WIN32
				, TRUE
#endif
					 );
			vim_free(p);
		    }
		}
	}

	vim_free(map_buf);
    }

    /* If the menubar height changed, resize the window */
    if (gui.menu_height != old_menu_height)
	gui_set_winsize(FALSE);
}

/*
 * Modify a menu name starting with "PopUp" to include the mode character.
 * Returns the name in allocated memory (NULL for failure).
 */
    static char_u *
popup_mode_name(name, idx)
    char_u	*name;
    int		idx;
{
    char_u	*p;
    int		len = STRLEN(name);

    p = vim_strnsave(name, len + 1);
    if (p != NULL)
    {
	mch_memmove(p + 6, p + 5, len - 4);
	p[5] = MENU_MODE_CHARS[idx];
    }
    return p;
}

/*
 * Used recursively by gui_update_menus (see below)
 */
    static void
gui_update_menus_recurse(menu, mode)
    GuiMenu *menu;
    int	    mode;
{
    int	    i;

    while (menu)
    {
	if (menu->modes & mode)
	    i = FALSE;
	else
	    i = TRUE;
	if (vim_strchr(p_guioptions, GO_GREY) != NULL)
	    gui_mch_menu_grey(menu, i);
	else
	    gui_mch_menu_hidden(menu, i);
	gui_update_menus_recurse(menu->children, mode);
	menu = menu->next;
    }
}

/*
 * Make sure only the valid menu items appear for this mode.  If
 * force_menu_update is not TRUE, then we only do this if the mode has changed
 * since last time.  If "modes" is not 0, then we use these modes instead.
 */
    void
gui_update_menus(modes)
    int	    modes;
{
    static int	    prev_mode = -1;
    int		    mode = 0;

    if (modes != 0x0)
	mode = modes;
    else
    {
	mode = get_menu_mode();
	if (mode == MENU_INDEX_INVALID)
	    mode = 0;
	else
	    mode = (1 << mode);
    }

    if (force_menu_update || mode != prev_mode)
    {
	gui_update_menus_recurse(gui.root_menu, mode);
	gui_mch_draw_menubar();
	prev_mode = mode;
	force_menu_update = FALSE;
    }
}

    static int
get_menu_mode()
{
    if (VIsual_active)
	return MENU_INDEX_VISUAL;
    if (State & INSERT)
	return MENU_INDEX_INSERT;
    if (State & CMDLINE)
	return MENU_INDEX_CMDLINE;
    if (finish_op)
	return MENU_INDEX_OP_PENDING;
    if (State & NORMAL)
	return MENU_INDEX_NORMAL;
    return MENU_INDEX_INVALID;
}

/*
 * Check if a key is used as a mnemonic for a toplevel menu.
 * Case of the key is ignored.
 */
    int
gui_is_menu_shortcut(key)
    int		key;
{
    GuiMenu	*menu;

    key = TO_LOWER(key);
    for (menu = gui.root_menu; menu != NULL; menu = menu->next)
	if (TO_LOWER(menu->mnemonic) == key)
	    return TRUE;
    return FALSE;
}

#if defined(USE_GUI_WIN32) || defined(PROTO)

/*
 * Deal with tearoff items that are added like a menu item.
 * Currently only for Win32 GUI.  Others may follow later.
 */

    void
gui_mch_toggle_tearoffs(int enable)
{
    int		pri_tab[MENUDEPTH + 1];
    int		i;

    if (enable)
    {
	for (i = 0; i < MENUDEPTH; ++i)
	    pri_tab[i] = 500;
	pri_tab[MENUDEPTH] = -1;
	gui_create_tearoffs_recurse(gui.root_menu, (char_u *)"", pri_tab, 0);
    }
    else
	gui_destroy_tearoffs_recurse(gui.root_menu);
    s_tearoffs = enable;
}

/*
 * Recursively add tearoff items
 */
    static void
gui_create_tearoffs_recurse(menu, pname, pri_tab, pri_idx)
    GuiMenu		*menu;
    const char_u	*pname;
    int			*pri_tab;
    int			pri_idx;
{
    char_u	*newpname = NULL;
    int		len;
    char_u	*s;
    char_u	*d;

    if (pri_tab[pri_idx + 1] != -1)
	++pri_idx;
    while (menu != NULL)
    {
	if (menu->children != NULL && gui_menubar_menu(menu->name))
	{
	    /* Add the menu name to the menu path.  Insert a backslash before
	     * dots (it's used to separate menu names). */
	    len = STRLEN(pname) + STRLEN(menu->name);
	    for (s = menu->name; *s; ++s)
		if (*s == '.' || *s == '\\')
		    ++len;
	    newpname = alloc(len + TEAR_LEN + 2);
	    if (newpname != NULL)
	    {
		STRCPY(newpname, pname);
		d = newpname + STRLEN(newpname);
		for (s = menu->name; *s; ++s)
		{
		    if (*s == '.' || *s == '\\')
			*d++ = '\\';
		    *d++ = *s;
		}
		*d = NUL;

		/* check if tearoff already exists */
		if (STRCMP(menu->children->name, TEAR_STRING) != 0)
		{
		    gui_add_tearoff(newpname, pri_tab, pri_idx - 1);
		    *d = NUL;			/* remove TEAR_STRING */
		}

		STRCAT(newpname, ".");
		gui_create_tearoffs_recurse(menu->children, newpname,
							    pri_tab, pri_idx);
		vim_free(newpname);
	    }
	}
	menu = menu->next;
    }
}

/*
 * Add tear-off menu item for a submenu.
 * "tearpath" is the menu path, and must have room to add TEAR_STRING.
 */
    static void
gui_add_tearoff(tearpath, pri_tab, pri_idx)
    char_u	*tearpath;
    int		*pri_tab;
    int		pri_idx;
{
    char_u	*tbuf;
    int		t;

    tbuf = alloc(5 + STRLEN(tearpath));
    if (tbuf != NULL)
    {
	tbuf[0] = K_SPECIAL;
	tbuf[1] = K_SECOND(K_TEAROFF);
	tbuf[2] = K_THIRD(K_TEAROFF);
	STRCPY(tbuf + 3, tearpath);
	STRCAT(tbuf + 3, "\r");

	STRCAT(tearpath, ".");
	STRCAT(tearpath, TEAR_STRING);

	/* Priority of tear-off is always 1 */
	t = pri_tab[pri_idx + 1];
	pri_tab[pri_idx + 1] = 1;

	gui_add_menu_path(tearpath, MENU_ALL_MODES, pri_tab,
		gui_menu_cb, tbuf, TRUE, FALSE);

	gui_add_menu_path(tearpath, MENU_TIP_MODE, pri_tab,
		gui_menu_cb, (char_u *)"Tear off this menu", TRUE, FALSE);

	pri_tab[pri_idx + 1] = t;
	vim_free(tbuf);
    }
}

/*
 * Recursively destroy tearoff items
 */
    static void
gui_destroy_tearoffs_recurse(menu)
    GuiMenu *menu;
{
    GuiMenu *oldmenu;

    while (menu)
    {
	if (menu->children)
	{
	    /* check if tearoff exists */
	    if ( STRCMP(menu->children->name, TEAR_STRING) == 0)
	    {
		/* Disconnect the item */
		oldmenu = menu->children;
		menu->children = oldmenu->next;
		/* Free the memory */
		gui_free_menu(oldmenu);
	    }
	    if (menu->children != NULL) /* might have been last one */
		gui_destroy_tearoffs_recurse(menu->children);
	}
	menu = menu->next;
    }
}

#endif /*USE_GUI_WIN32*/

/*
 * Add the menu with the given name to the menu hierarchy
 */
    static int
gui_add_menu_path(menu_path, modes, pri_tab, call_back, call_data, noremap
#ifdef USE_GUI_WIN32
	, addtearoff
#endif
	)
    char_u	*menu_path;
    int		modes;
    int		*pri_tab;
    void	(*call_back)();
    char_u	*call_data;
    int		noremap;
#ifdef USE_GUI_WIN32
    int		addtearoff;	/* may add tearoff item */
#endif
{
    char_u	*path_name;
    GuiMenu	**menup;
    GuiMenu	*menu = NULL;
    GuiMenu	*parent;
    GuiMenu	**lower_pri;
    char_u	*p;
    char_u	*name;
    char_u	*dname;
    char_u	*next_name;
    int		i;
    int		c;
    int		idx, new_idx;
    int		pri_idx = 0;
    int		old_modes = 0;
    int		amenu;

    /* Make a copy so we can stuff around with it, since it could be const */
    path_name = vim_strsave(menu_path);
    if (path_name == NULL)
	return FAIL;
    menup = &gui.root_menu;
    parent = NULL;
    name = path_name;
    while (*name)
    {
	/* Get name of this element in the menu hierarchy, and the simplified
	 * name (without mnemonic and accelerator text). */
	next_name = gui_menu_name_skip(name);
	dname = gui_menu_text(name, NULL, NULL);

	/* See if it's already there */
	lower_pri = menup;
	idx = 0;
	new_idx = 0;
	menu = *menup;
	while (menu != NULL)
	{
	    if (menu_name_equal(name, menu) || menu_name_equal(dname, menu))
	    {
		if (*next_name == NUL && menu->children != NULL)
		{
		    if (!gui_sys_menu)
			EMSG("Menu path must not lead to a sub-menu");
		    vim_free(path_name);
		    vim_free(dname);
		    return FAIL;
		}
		if (*next_name != NUL && menu->children == NULL
#ifdef USE_GUI_WIN32
			&& addtearoff
#endif
			)
		{
		    if (!gui_sys_menu)
			EMSG("Part of menu-item path is not sub-menu");
		    vim_free(path_name);
		    vim_free(dname);
		    return FAIL;
		}
		break;
	    }
	    menup = &menu->next;

	    /* Count menus, to find where this one needs to be inserted.
	     * Ignore menus that are not in the menubar (PopUp and Toolbar) */
	    if (parent != NULL || gui_menubar_menu(menu->name))
	    {
		++idx;
		if (menu->priority <= pri_tab[pri_idx])
		{
		    lower_pri = menup;
		    new_idx = idx;
		}
	    }
	    menu = menu->next;
	}

	if (menu == NULL)
	{
	    if (*next_name == NUL && parent == NULL)
	    {
		EMSG("Must not add menu items directly to menu bar");
		vim_free(path_name);
		vim_free(dname);
		return FAIL;
	    }

	    /* Not already there, so lets add it */
	    menu = (GuiMenu *)alloc_clear(sizeof(GuiMenu));
	    if (menu == NULL)
	    {
		vim_free(path_name);
		vim_free(dname);
		return FAIL;
	    }
	    menu->modes = modes;
	    menu->name = vim_strsave(name);
	    /* separate mnemonic and accelerator text from actual menu name */
	    menu->dname = gui_menu_text(name, &menu->mnemonic, &menu->actext);
	    menu->priority = pri_tab[pri_idx];

	    /*
	     * Add after menu that has lower priority.
	     */
	    menu->next = *lower_pri;
	    *lower_pri = menu;

	    if (gui.in_use)  /* Otherwise it will be added when GUI starts */
	    {
		if (*next_name == NUL)
		{
		    /* Real menu item, not sub-menu */
		    gui_mch_add_menu_item(menu, parent, new_idx);

		    /* Want to update menus now even if mode not changed */
		    force_menu_update = TRUE;
		}
		else
		{
		    /* Sub-menu (not at end of path yet) */
		    gui_mch_add_menu(menu, parent, new_idx);
		}
	    }

#ifdef USE_GUI_WIN32
	    /* When adding a new submenu, may add a tearoff item */
	    if (	addtearoff
		    && *next_name
		    && vim_strchr(p_guioptions, GO_TEAROFF) != NULL
		    && gui_menubar_menu(name))
	    {
		char_u		*tearpath;

		/*
		 * The pointers next_name & path_name refer to a string with
		 * \'s and ^V's stripped out. But menu_path is a "raw"
		 * string, so we must correct for special characters.
		 */
		tearpath = alloc(STRLEN(menu_path) + TEAR_LEN + 2);
		if (tearpath != NULL)
		{
		    char_u  *s;
		    int	    idx;

		    STRCPY(tearpath, menu_path);
		    idx = next_name - path_name - 1;
		    for (s = tearpath; *s && s < tearpath + idx; ++s)
			if ((*s == '\\' || *s == Ctrl('V')) && s[1])
			{
			    ++idx;
			    ++s;
			}
		    tearpath[idx] = NUL;
		    gui_add_tearoff(tearpath, pri_tab, pri_idx);
		    vim_free(tearpath);
		}
	    }
#endif
	    old_modes = 0;
	}
	else
	{
	    old_modes = menu->modes;

	    /*
	     * If this menu option was previously only available in other
	     * modes, then make sure it's available for this one now
	     */
#ifdef USE_GUI_WIN32
	    /* If adding a tearbar (addtearoff == FALSE) don't update modes */
	    if (addtearoff)
#endif
		menu->modes |= modes;
	}

	menup = &menu->children;
	parent = menu;
	name = next_name;
	vim_free(dname);
	if (pri_tab[pri_idx + 1] != -1)
	    ++pri_idx;
    }
    vim_free(path_name);

    /*
     * Only add system menu items which have not been defined yet.
     * First check if this was an ":amenu".
     */
    amenu = ((modes & (MENU_NORMAL_MODE | MENU_INSERT_MODE)) ==
				       (MENU_NORMAL_MODE | MENU_INSERT_MODE));
    if (gui_sys_menu)
	modes &= ~old_modes;

    if (menu != NULL && modes)
    {
	menu->cb = call_back;
	p = (call_data == NULL) ? NULL : vim_strsave(call_data);

	/* loop over all modes, may add more than one */
	for (i = 0; i < MENU_MODES; ++i)
	{
	    if (modes & (1 << i))
	    {
		/* free any old menu */
		gui_free_menu_string(menu, i);

		/* For "amenu", may insert an extra character */
		/* Don't do this if adding a tearbar (addtearoff == FALSE) */
		c = 0;
		if (amenu
#ifdef USE_GUI_WIN32
		       && addtearoff
#endif
				       )
		{
		    switch (1 << i)
		    {
			case MENU_VISUAL_MODE:
			case MENU_OP_PENDING_MODE:
			case MENU_CMDLINE_MODE:
			    c = Ctrl('C');
			    break;
			case MENU_INSERT_MODE:
			    c = Ctrl('O');
			    break;
		    }
		}

		if (c)
		{
		    menu->strings[i] = alloc((unsigned)(STRLEN(call_data) + 2));
		    if (menu->strings[i] != NULL)
		    {
			menu->strings[i][0] = c;
			STRCPY(menu->strings[i] + 1, call_data);
		    }
		}
		else
		    menu->strings[i] = p;
		menu->noremap[i] = noremap;
	    }
	}
    }
    return OK;
}

/*
 * Remove the (sub)menu with the given name from the menu hierarchy
 * Called recursively.
 */
    static int
gui_remove_menu(menup, name, modes, silent)
    GuiMenu	**menup;
    char_u	*name;
    int		modes;
    int		silent;		/* don't give error messages */
{
    GuiMenu	*menu;
    GuiMenu	*child;
    char_u	*p;

    if (*menup == NULL)
	return OK;		/* Got to bottom of hierarchy */

    /* Get name of this element in the menu hierarchy */
    p = gui_menu_name_skip(name);

    /* Find the menu */
    menu = *menup;
    while (menu != NULL)
    {
	if (*name == NUL || menu_name_equal(name, menu))
	{
	    if (*p != NUL && menu->children == NULL)
	    {
		if (!silent)
		    EMSG("Part of menu-item path is not sub-menu");
		return FAIL;
	    }
	    if ((menu->modes & modes) != 0x0)
	    {
		if (gui_remove_menu(&menu->children, p, modes, silent) == FAIL)
		    return FAIL;
	    }
	    else if (*name != NUL)
	    {
		if (!silent)
		    EMSG("Menu only exists in another mode");
		return FAIL;
	    }

	    /*
	     * When name is empty, we are removing all menu items for the given
	     * modes, so keep looping, otherwise we are just removing the named
	     * menu item (which has been found) so break here.
	     */
	    if (*name != NUL)
		break;

	    /* Remove the menu item for the given mode[s] */
	    menu->modes &= ~modes;
	    if (modes & MENU_TIP_MODE)
		gui_free_menu_string(menu, MENU_INDEX_TIP);
	    if ((menu->modes & MENU_ALL_MODES) == 0)
	    {
		/* The menu item is no longer valid in ANY mode, so delete it */
		*menup = menu->next;
		gui_free_menu(menu);
	    }
	    else
		menup = &menu->next;
	}
	else
	    menup = &menu->next;
	menu = *menup;
    }
    if (*name != NUL)
    {
	if (menu == NULL)
	{
	    if (!silent)
		EMSG("No menu of that name");
	    return FAIL;
	}


	/* Recalculate modes for menu based on the new updated children */
	menu->modes &= ~modes;
#ifdef USE_GUI_WIN32
	if ((s_tearoffs) && (menu->children != NULL)) /* there's a tear bar.. */
	    child = menu->children->next; /* don't count tearoff bar */
	else
#endif
	    child = menu->children;
	for ( ; child != NULL; child = child->next)
	    menu->modes |= child->modes;
	if (modes & MENU_TIP_MODE)
	    gui_free_menu_string(menu, MENU_INDEX_TIP);
	if ((menu->modes & MENU_ALL_MODES) == 0)
	{
	    /* The menu item is no longer valid in ANY mode, so delete it */
#ifdef USE_GUI_WIN32
	    if ((s_tearoffs) && (menu->children != NULL)) /* there's a tear bar.. */
		gui_free_menu(menu->children);
#endif
	    *menup = menu->next;
	    gui_free_menu(menu);
	}
    }

    return OK;
}

/*
 * Free the given menu structure
 */
    static void
gui_free_menu(menu)
    GuiMenu *menu;
{
    int	    i;

    /* Free machine specific menu structures (only when already created) */
    if (gui.in_use)
	gui_mch_destroy_menu(menu);
    vim_free(menu->name);
    vim_free(menu->dname);
    vim_free(menu->actext);
    for (i = 0; i < MENU_MODES; i++)
	gui_free_menu_string(menu, i);
    vim_free(menu);

    /* Want to update menus now even if mode not changed */
    force_menu_update = TRUE;
}

/*
 * Free the menu->string with the given index.
 */
    static void
gui_free_menu_string(menu, idx)
    GuiMenu *menu;
    int	    idx;
{
    int	    count = 0;
    int	    i;

    for (i = 0; i < MENU_MODES; i++)
	if (menu->strings[i] == menu->strings[idx])
	    count++;
    if (count == 1)
	vim_free(menu->strings[idx]);
    menu->strings[idx] = NULL;
}

/*
 * Show the mapping associated with a menu item or hierarchy in a sub-menu.
 */
    static int
gui_show_menus(path_name, modes)
    char_u  *path_name;
    int	    modes;
{
    char_u  *p;
    char_u  *name;
    GuiMenu *menu;
    GuiMenu *parent = NULL;

    menu = gui.root_menu;
    name = path_name = vim_strsave(path_name);
    if (path_name == NULL)
	return FAIL;

    /* First, find the (sub)menu with the given name */
    while (*name)
    {
	p = gui_menu_name_skip(name);
	while (menu != NULL)
	{
	    if (menu_name_equal(name, menu))
	    {
		/* Found menu */
		if (*p != NUL && menu->children == NULL)
		{
		    EMSG("Part of menu-item path is not sub-menu");
		    vim_free(path_name);
		    return FAIL;
		}
		else if ((menu->modes & modes) == 0x0)
		{
		    EMSG("Menu only exists in another mode");
		    vim_free(path_name);
		    return FAIL;
		}
		break;
	    }
	    menu = menu->next;
	}
	if (menu == NULL)
	{
	    EMSG("No menu of that name");
	    vim_free(path_name);
	    return FAIL;
	}
	name = p;
	parent = menu;
	menu = menu->children;
    }

    /* Now we have found the matching menu, and we list the mappings */
						    /* Highlight title */
    MSG_PUTS_TITLE("\n--- Menus ---");

    gui_show_menus_recursive(parent, modes, 0);
    return OK;
}

/*
 * Recursively show the mappings associated with the menus under the given one
 */
    static void
gui_show_menus_recursive(menu, modes, depth)
    GuiMenu *menu;
    int	    modes;
    int	    depth;
{
    int	    i;
    int	    bit;

    if (menu != NULL && (menu->modes & modes) == 0x0)
	return;

    if (menu != NULL)
    {
	msg_putchar('\n');
	if (got_int)		/* "q" hit for "--more--" */
	    return;
	for (i = 0; i < depth; i++)
	    MSG_PUTS("  ");
	if (menu->priority)
	{
	    msg_outnum((long)menu->priority);
	    MSG_PUTS(" ");
	}
				/* Same highlighting as for directories!? */
	msg_puts_attr(menu->name, hl_attr(HLF_D));
    }

    if (menu != NULL && menu->children == NULL)
    {
	for (bit = 0; bit < MENU_MODES; bit++)
	    if ((menu->modes & modes & (1 << bit)) != 0)
	    {
		msg_putchar('\n');
		if (got_int)		/* "q" hit for "--more--" */
		    return;
		for (i = 0; i < depth + 2; i++)
		    MSG_PUTS("  ");
		msg_putchar(MENU_MODE_CHARS[bit]);
		if (menu->noremap[bit])
		    msg_putchar('*');
		else
		    msg_putchar(' ');
		MSG_PUTS("  ");
		msg_outtrans_special(menu->strings[bit], TRUE);
	    }
    }
    else
    {
	if (menu == NULL)
	{
	    menu = gui.root_menu;
	    depth--;
	}
	else
	    menu = menu->children;

	/* recursively show all children.  Skip PopUp[nvoci]. */
	for (; menu != NULL && !got_int; menu = menu->next)
	    if (!gui_popup_menu(menu->dname) || menu->dname[5] == NUL)
		gui_show_menus_recursive(menu, modes, depth + 1);
    }
}

/*
 * Used when expanding menu names.
 */
static GuiMenu	*expand_menu = NULL;
static int	expand_modes = 0x0;

/*
 * Work out what to complete when doing command line completion of menu names.
 */
    char_u *
gui_set_context_in_menu_cmd(cmd, arg, forceit)
    char_u  *cmd;
    char_u  *arg;
    int	    forceit;
{
    char_u  *after_dot;
    char_u  *p;
    char_u  *path_name = NULL;
    char_u  *name;
    int	    unmenu;
    GuiMenu *menu;

    expand_context = EXPAND_UNSUCCESSFUL;

    after_dot = arg;
    for (p = arg; *p && !vim_iswhite(*p); ++p)
    {
	if ((*p == '\\' || *p == Ctrl('V')) && p[1] != NUL)
	    p++;
	else if (*p == '.')
	    after_dot = p + 1;
    }
    if (*p == NUL)		/* Complete the menu name */
    {
	/*
	 * With :unmenu, you only want to match menus for the appropriate mode.
	 * With :menu though you might want to add a menu with the same name as
	 * one in another mode, so match menus fom other modes too.
	 */
	expand_modes = gui_get_menu_cmd_modes(cmd, forceit, NULL, &unmenu);
	if (!unmenu)
	    expand_modes = MENU_ALL_MODES;

	menu = gui.root_menu;
	if (after_dot != arg)
	{
	    path_name = alloc(after_dot - arg);
	    if (path_name == NULL)
		return NULL;
	    STRNCPY(path_name, arg, after_dot - arg - 1);
	    path_name[after_dot - arg - 1] = NUL;
	}
	name = path_name;
	while (name != NULL && *name)
	{
	    p = gui_menu_name_skip(name);
	    while (menu != NULL)
	    {
		if (menu_name_equal(name, menu))
		{
		    /* Found menu */
		    if ((*p != NUL && menu->children == NULL)
			|| ((menu->modes & expand_modes) == 0x0))
		    {
			/*
			 * Menu path continues, but we have reached a leaf.
			 * Or menu exists only in another mode.
			 */
			vim_free(path_name);
			return NULL;
		    }
		    break;
		}
		menu = menu->next;
	    }
	    if (menu == NULL)
	    {
		/* No menu found with the name we were looking for */
		vim_free(path_name);
		return NULL;
	    }
	    name = p;
	    menu = menu->children;
	}

	expand_context = EXPAND_MENUS;
	expand_pattern = after_dot;
	expand_menu = menu;
    }
    else			/* We're in the mapping part */
	expand_context = EXPAND_NOTHING;
    return NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the list of group names.
 */
    char_u *
get_menu_name(idx)
    int	    idx;
{
    static GuiMenu  *menu = NULL;
    static int	    get_dname = FALSE;	/* return menu->dname next time */
    char_u	    *str;

    if (idx == 0)	    /* first call: start at first item */
    {
	menu = expand_menu;
	get_dname = FALSE;
    }

    /* Skip PopUp[nvoci]. */
    while (menu != NULL && gui_popup_menu(menu->dname) && menu->dname[5])
	menu = menu->next;

    if (menu == NULL)	    /* at end of linked list */
	return NULL;

    if (menu->modes & expand_modes)
    {
	if (get_dname)
	{
	    str = menu->dname;
	    get_dname = FALSE;
	}
	else
	{
	    str = menu->name;
	    if (STRCMP(menu->name, menu->dname))
		get_dname = TRUE;
	}
    }
    else
    {
	str = (char_u *)"";
	get_dname = FALSE;
    }

    /* Advance to next menu entry. */
    if (!get_dname)
	menu = menu->next;

    return str;
}

/*
 * Skip over this element of the menu path and return the start of the next
 * element.  Any \ and ^Vs are removed from the current element.
 */
    char_u *
gui_menu_name_skip(name)
    char_u  *name;
{
    char_u  *p;

    for (p = name; *p && *p != '.'; p++)
	if (*p == '\\' || *p == Ctrl('V'))
	{
	    mch_memmove(p, p + 1, STRLEN(p));
	    if (*p == NUL)
		break;
	}
    if (*p)
	*p++ = NUL;
    return p;
}

/*
 * Return TRUE when "name" matches with menu "menu".  The name is compared in
 * two ways: raw menu name and menu name without '&'.  ignore part after a TAB.
 */
    static int
menu_name_equal(name, menu)
    char_u	*name;
    GuiMenu	*menu;
{
    return (menu_namecmp(name, menu->name) || menu_namecmp(name, menu->dname));
}

    static int
menu_namecmp(name, mname)
    char_u	*name;
    char_u	*mname;
{
    int		i;

    for (i = 0; name[i] != NUL && name[i] != TAB; ++i)
	if (name[i] != mname[i])
	    break;
    return ((name[i] == NUL || name[i] == TAB)
	    && (mname[i] == NUL || mname[i] == TAB));
}

/*
 * After we have started the GUI, then we can create any menus that have been
 * defined.  This is done once here.  gui_add_menu_path() may have already been
 * called to define these menus, and may be called again.  This function calls
 * itself recursively.	Should be called at the top level with:
 * gui_create_initial_menus(gui.root_menu, NULL);
 */
    static void
gui_create_initial_menus(menu, parent)
    GuiMenu	*menu;
    GuiMenu	*parent;
{
    int		idx = 0;

    while (menu)
    {
	if (menu->children != NULL)
	{
	    gui_mch_add_menu(menu, parent, idx);
	    gui_create_initial_menus(menu->children, menu);
	}
	else
	    gui_mch_add_menu_item(menu, parent, idx);
	menu = menu->next;
	++idx;
    }
}


/*
 * Set which components are present.
 * If "oldval" is not NULL, "oldval" is the previous value, the new * value is
 * in p_guioptions.
 */
    void
gui_init_which_components(oldval)
    char_u  *oldval;
{
    static int prev_which_scrollbars[3] = {-1, -1, -1};
    static int prev_menu_is_active = -1;
#ifdef USE_GUI_WIN32_TOOLBAR
    static int	prev_toolbar = -1;
    int		using_toolbar = FALSE;
#endif
    static int	prev_tearoff = -1;
    int		using_tearoff = FALSE;

    char_u  *p;
    int	    i;
    int	    grey_old, grey_new;
    char_u  *temp;
    WIN	    *wp;
    int	    need_winsize;

    if (oldval != NULL && gui.in_use)
    {
	/*
	 * Check if the menu's go from grey to non-grey or vise versa.
	 */
	grey_old = (vim_strchr(oldval, GO_GREY) != NULL);
	grey_new = (vim_strchr(p_guioptions, GO_GREY) != NULL);
	if (grey_old != grey_new)
	{
	    temp = p_guioptions;
	    p_guioptions = oldval;
	    gui_update_menus(MENU_ALL_MODES);
	    p_guioptions = temp;
	}
    }

    gui.menu_is_active = FALSE;
    for (i = 0; i < 3; i++)
	gui.which_scrollbars[i] = FALSE;
    for (p = p_guioptions; *p; p++)
	switch (*p)
	{
	    case GO_LEFT:
		gui.which_scrollbars[SBAR_LEFT] = TRUE;
		break;
	    case GO_RIGHT:
		gui.which_scrollbars[SBAR_RIGHT] = TRUE;
		break;
	    case GO_BOT:
		gui.which_scrollbars[SBAR_BOTTOM] = TRUE;
		break;
	    case GO_MENUS:
		gui.menu_is_active = TRUE;
		break;
	    case GO_GREY:
		/* make menu's have grey items, ignored here */
		break;
#ifdef USE_GUI_WIN32_TOOLBAR
	    case GO_TOOLBAR:
		using_toolbar = TRUE;
		break;
#endif
	    case GO_TEAROFF:
		using_tearoff = TRUE;
		break;
	    default:
		/* Should give error message for internal error */
		break;
	}
    if (gui.in_use)
    {
	need_winsize = FALSE;
	for (i = 0; i < 3; i++)
	{
	    if (gui.which_scrollbars[i] != prev_which_scrollbars[i])
	    {
		if (i == SBAR_BOTTOM)
		{
		    gui_mch_enable_scrollbar(&gui.bottom_sbar,
					     gui.which_scrollbars[i]);
		}
		else
		{
		    for (wp = firstwin; wp != NULL; wp = wp->w_next)
			gui_mch_enable_scrollbar(&wp->w_scrollbars[i],
						 gui.which_scrollbars[i]);
		}
		need_winsize = TRUE;
	    }
	    prev_which_scrollbars[i] = gui.which_scrollbars[i];
	}

	if (gui.menu_is_active != prev_menu_is_active)
	{
	    gui_mch_enable_menu(gui.menu_is_active);
	    prev_menu_is_active = gui.menu_is_active;
	    need_winsize = TRUE;
	}

#ifdef USE_GUI_WIN32_TOOLBAR
	if (using_toolbar != prev_toolbar)
	{
	    gui_mch_show_toolbar(using_toolbar);
	    prev_toolbar = using_toolbar;
	    need_winsize = TRUE;
	}
#endif
	if (using_tearoff != prev_tearoff)
	{
	    gui_mch_toggle_tearoffs(using_tearoff);
	    prev_tearoff = using_tearoff;
	}
	if (need_winsize)
	    gui_set_winsize(FALSE);
    }
}


/*
 * Scrollbar stuff:
 */

    void
gui_create_scrollbar(sb, wp)
    GuiScrollbar    *sb;
    WIN		    *wp;
{
    static int	sbar_ident = 0;
    int	    which;

    sb->ident = sbar_ident++;	/* No check for too big, but would it happen? */
    sb->wp = wp;
    sb->value = -1;
    sb->pixval = -1;
    sb->size = -1;
    sb->max = -1;
    sb->top = -1;
    sb->height = -1;
    sb->status_height = -1;
    gui_mch_create_scrollbar(sb, (wp == NULL) ? SBAR_HORIZ : SBAR_VERT);
    if (wp != NULL)
    {
	which = (sb == &wp->w_scrollbars[SBAR_LEFT]) ? SBAR_LEFT : SBAR_RIGHT;
	gui_mch_enable_scrollbar(sb, gui.which_scrollbars[which]);
    }
}

/*
 * Find the scrollbar with the given index.
 */
    GuiScrollbar *
gui_find_scrollbar(ident)
    long	ident;
{
    WIN		    *wp;

    if (gui.bottom_sbar.ident == ident)
	return &gui.bottom_sbar;
    for (wp = firstwin; wp != NULL; wp = wp->w_next)
    {
	if (wp->w_scrollbars[SBAR_LEFT].ident == ident)
	    return &wp->w_scrollbars[SBAR_LEFT];
	if (wp->w_scrollbars[SBAR_RIGHT].ident == ident)
	    return &wp->w_scrollbars[SBAR_RIGHT];
    }
    return NULL;
}

    void
gui_drag_scrollbar(sb, value, still_dragging)
    GuiScrollbar    *sb;
    long	    value;
    int		    still_dragging;
{
    char_u	bytes[4 + sizeof(long_u)];
    WIN		*wp;
    int		sb_num;
    int		byte_count;

    if (sb == NULL)
	return;

    /*
     * Ignore the scrollbars while executing an external command.
     */
    if (!termcap_active)
	return;

    if (still_dragging)
    {
	if (sb->wp == NULL)
	    gui.dragged_sb = SBAR_BOTTOM;
	else if (sb == &sb->wp->w_scrollbars[SBAR_LEFT])
	    gui.dragged_sb = SBAR_LEFT;
	else
	    gui.dragged_sb = SBAR_RIGHT;
	gui.dragged_wp = sb->wp;
    }
    else
	gui.dragged_sb = SBAR_NONE;

    if (sb->wp != NULL)
    {
	/* Vertical sbar info is kept in the first sbar (the left one) */
	sb = &sb->wp->w_scrollbars[0];
    }

    /*
     * Check validity of value
     */
    if (value < 0)
	value = 0;
#ifdef SCROLL_PAST_END
    else if (value > sb->max)
	value = sb->max;
#else
    if (value > sb->max - sb->size + 1)
	value = sb->max - sb->size + 1;
#endif

    sb->value = value;

#ifdef RIGHTLEFT
    if (sb->wp == NULL && curwin->w_p_rl)
    {
	value = sb->max + 1 - sb->size - value;
	if (value < 0)
	    value = 0;
    }
#endif

    if (sb->wp != NULL)
    {
	sb_num = 0;
	for (wp = firstwin; wp != sb->wp && wp != NULL; wp = wp->w_next)
	    sb_num++;

	if (wp == NULL)
	    return;

	bytes[0] = CSI;
	bytes[1] = KS_SCROLLBAR;
	bytes[2] = K_FILLER;
	bytes[3] = (char_u)sb_num;
	byte_count = 4;
    }
    else
    {
	bytes[0] = CSI;
	bytes[1] = KS_HORIZ_SCROLLBAR;
	bytes[2] = K_FILLER;
	byte_count = 3;
    }

    add_long_to_buf((long)value, bytes + byte_count);
    add_to_input_buf(bytes, byte_count + sizeof(long_u));
}

/*
 * Scrollbar stuff:
 */

    static void
gui_update_scrollbars(force)
    int		    force;	    /* Force all scrollbars to get updated */
{
    WIN		    *wp;
    GuiScrollbar    *sb;
    int		    val, size, max;
    int		    which_sb;
    int		    h, y;

    /* Update the horizontal scrollbar */
    gui_update_horiz_scrollbar(force);

    /* Return straight away if there is neither a left nor right scrollbar */
    if (!gui.which_scrollbars[SBAR_LEFT] && !gui.which_scrollbars[SBAR_RIGHT])
	return;

    /*
     * Don't want to update a scrollbar while we're dragging it.  But if we
     * have both a left and right scrollbar, and we drag one of them, we still
     * need to update the other one.
     */
    if (       (gui.dragged_sb == SBAR_LEFT
		|| gui.dragged_sb == SBAR_RIGHT)
	    && (!gui.which_scrollbars[SBAR_LEFT]
		|| !gui.which_scrollbars[SBAR_RIGHT])
	    && !force)
	return;

    if (!force && (gui.dragged_sb == SBAR_LEFT || gui.dragged_sb == SBAR_RIGHT))
    {
	/*
	 * If we have two scrollbars and one of them is being dragged, just
	 * copy the scrollbar position from the dragged one to the other one.
	 */
	which_sb = SBAR_LEFT + SBAR_RIGHT - gui.dragged_sb;
	if (gui.dragged_wp != NULL)
	    gui_mch_set_scrollbar_thumb(
		    &gui.dragged_wp->w_scrollbars[which_sb],
		    gui.dragged_wp->w_scrollbars[0].value,
		    gui.dragged_wp->w_scrollbars[0].size,
		    gui.dragged_wp->w_scrollbars[0].max);
	return;
    }

    for (wp = firstwin; wp; wp = wp->w_next)
    {
	if (wp->w_buffer == NULL)	/* just in case */
	    continue;
#ifdef SCROLL_PAST_END
	max = wp->w_buffer->b_ml.ml_line_count - 1;
#else
	max = wp->w_buffer->b_ml.ml_line_count + wp->w_height - 2;
#endif
	if (max < 0)			/* empty buffer */
	    max = 0;
	val = wp->w_topline - 1;
	size = wp->w_height;
#ifdef SCROLL_PAST_END
	if (val > max)			/* just in case */
	    val = max;
#else
	if (size > max + 1)		/* just in case */
	    size = max + 1;
	if (val > max - size + 1)
	    val = max - size + 1;
#endif
	if (val < 0)			/* minimal value is 0 */
	    val = 0;

	/*
	 * Scrollbar at index 0 (the left one) contains all the information.
	 * It would be the same info for left and right so we just store it for
	 * one of them.
	 */
	sb = &wp->w_scrollbars[0];

	/*
	 * Note: no check for valid w_botline.	If it's not valid the
	 * scrollbars will be updated later anyway.
	 */
	if (size < 1 || wp->w_botline - 2 > max)
	{
	    /*
	     * This can happen during changing files.  Just don't update the
	     * scrollbar for now.
	     */
	    sb->height = 0;	    /* Force update next time */
	    if (gui.which_scrollbars[SBAR_LEFT])
		gui_mch_enable_scrollbar(&wp->w_scrollbars[SBAR_LEFT], FALSE);
	    if (gui.which_scrollbars[SBAR_RIGHT])
		gui_mch_enable_scrollbar(&wp->w_scrollbars[SBAR_RIGHT], FALSE);
	    continue;
	}
	if (force || sb->height != wp->w_height
	    || sb->top != wp->w_winpos
	    || sb->status_height != wp->w_status_height)
	{
	    /* Height or position of scrollbar has changed */
	    sb->top = wp->w_winpos;
	    sb->height = wp->w_height;
	    sb->status_height = wp->w_status_height;

	    /* Calculate height and position in pixels */
	    h = (sb->height + sb->status_height) * gui.char_height;
	    y = sb->top * gui.char_height + gui.border_offset;
	    if (gui.menu_is_active)
		y += gui.menu_height;

#ifdef USE_GUI_WIN32
	    if (vim_strchr(p_guioptions, GO_TOOLBAR) != NULL)
		y += (TOOLBAR_BUTTON_HEIGHT+12);
#endif

	    if (wp == firstwin)
	    {
		/* Height of top scrollbar includes width of top border */
		h += gui.border_offset;
		y -= gui.border_offset;
	    }
	    if (gui.which_scrollbars[SBAR_LEFT])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_LEFT],
					  gui.left_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_mch_enable_scrollbar(&wp->w_scrollbars[SBAR_LEFT], TRUE);
	    }
	    if (gui.which_scrollbars[SBAR_RIGHT])
	    {
		gui_mch_set_scrollbar_pos(&wp->w_scrollbars[SBAR_RIGHT],
					  gui.right_sbar_x, y,
					  gui.scrollbar_width, h);
		gui_mch_enable_scrollbar(&wp->w_scrollbars[SBAR_RIGHT], TRUE);
	    }
	}

	/* reduce the number of calls to gui_mch_set_scrollbar_thumb() by
	 * checking if the thumb moved at least a pixel */
	if (max == 0)
	    y = 0;
	else
	    y = (val * (sb->height + 2) * gui.char_height + max / 2) / max;
	if (force
#ifdef RISCOS
		/* RISCOS does scrollbars differently - use the old check */
		|| sb->pixval != val
#else
		|| sb->pixval != y
#endif
		|| sb->size != size || sb->max != max)
	{
	    /* Thumb of scrollbar has moved */
	    sb->value = val;
	    sb->pixval = y;
	    sb->size = size;
	    sb->max = max;
	    if (gui.which_scrollbars[SBAR_LEFT] && gui.dragged_sb != SBAR_LEFT)
		gui_mch_set_scrollbar_thumb(&wp->w_scrollbars[SBAR_LEFT],
					    val, size, max);
	    if (gui.which_scrollbars[SBAR_RIGHT]
					&& gui.dragged_sb != SBAR_RIGHT)
		gui_mch_set_scrollbar_thumb(&wp->w_scrollbars[SBAR_RIGHT],
					    val, size, max);
	}
    }
}

/*
 * Scroll a window according to the values set in the globals current_scrollbar
 * and scrollbar_value.  Return TRUE if the cursor in the current window moved
 * or FALSE otherwise.
 */
    int
gui_do_scroll()
{
    WIN		*wp, *old_wp;
    int		i;
    long	nlines;
    FPOS	old_cursor;
    linenr_t	old_topline;

    for (wp = firstwin, i = 0; i < current_scrollbar; i++)
    {
	if (wp == NULL)
	    break;
	wp = wp->w_next;
    }
    if (wp == NULL)
    {
	/* Couldn't find window */
	return FALSE;
    }

    /*
     * Compute number of lines to scroll.  If zero, nothing to do.
     */
    nlines = (long)scrollbar_value + 1 - (long)wp->w_topline;
    if (nlines == 0)
	return FALSE;

    old_cursor = curwin->w_cursor;
    old_wp = curwin;
    old_topline = wp->w_topline;
    curwin = wp;
    curbuf = wp->w_buffer;
    if (nlines < 0)
	scrolldown(-nlines);
    else
	scrollup(nlines);
    if (old_topline != wp->w_topline)
    {
	if (p_so)
	{
	    cursor_correct();		/* fix window for 'so' */
	    update_topline();		/* avoid up/down jump */
	}
	coladvance(curwin->w_curswant);
    }

    curwin = old_wp;
    curbuf = old_wp->w_buffer;

    /*
     * Don't call updateWindow() when nothing has changed (it will overwrite
     * the status line!).
     */
    if (old_topline != wp->w_topline)
    {
	wp->w_redr_type = VALID;
	updateWindow(wp);   /* update window, status line, and cmdline */
    }

    return !equal(curwin->w_cursor, old_cursor);
}


/*
 * Horizontal scrollbar stuff:
 */

    static void
gui_update_horiz_scrollbar(force)
    int	    force;
{
    int	    value, size, max;
    char_u  *p;

    if (!gui.which_scrollbars[SBAR_BOTTOM])
	return;

    if (!force && gui.dragged_sb == SBAR_BOTTOM)
	return;

    if (!force && curwin->w_p_wrap && gui.prev_wrap)
	return;

    /*
     * It is possible for the cursor to be invalid if we're in the middle of
     * something (like changing files).  If so, don't do anything for now.
     */
    if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
    {
	gui.bottom_sbar.value = -1;
	return;
    }

    size = Columns;
    if (curwin->w_p_wrap)
    {
	value = 0;
#ifdef SCROLL_PAST_END
	max = 0;
#else
	max = Columns - 1;
#endif
    }
    else
    {
	value = curwin->w_leftcol;

	/* Calculate max for horizontal scrollbar */
	p = ml_get_curline();
	max = 0;
	if (p != NULL && p[0] != NUL)
	    while (p[1] != NUL)		    /* Don't count last character */
		max += chartabsize(*p++, (colnr_t)max);
#ifndef SCROLL_PAST_END
	max += Columns - 1;
#endif
    }

#ifndef SCROLL_PAST_END
    if (value > max - size + 1)
	value = max - size + 1;	    /* limit the value to allowable range */
#endif

#ifdef RIGHTLEFT
    if (curwin->w_p_rl)
    {
	value = max + 1 - size - value;
	if (value < 0)
	{
	    size += value;
	    value = 0;
	}
    }
#endif
    if (!force && value == gui.bottom_sbar.value && size == gui.bottom_sbar.size
						&& max == gui.bottom_sbar.max)
	return;

    gui.bottom_sbar.value = value;
    gui.bottom_sbar.size = size;
    gui.bottom_sbar.max = max;
    gui.prev_wrap = curwin->w_p_wrap;

    gui_mch_set_scrollbar_thumb(&gui.bottom_sbar, value, size, max);
}

/*
 * Do a horizontal scroll.  Return TRUE if the cursor moved, FALSE otherwise.
 */
    int
gui_do_horiz_scroll()
{
    /* no wrapping, no scrolling */
    if (curwin->w_p_wrap)
	return FALSE;

    if (curwin->w_leftcol == scrollbar_value)
	return FALSE;

    curwin->w_leftcol = scrollbar_value;
    return leftcol_changed();
}

/*
 * Check that none of the colors are the same as the background color
 */
    void
gui_check_colors()
{
    if (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == (GuiColor)-1)
    {
	gui_set_bg_color((char_u *)"White");
	if (gui.norm_pixel == gui.back_pixel || gui.norm_pixel == (GuiColor)-1)
	    gui_set_fg_color((char_u *)"Black");
    }
}

    void
gui_set_fg_color(name)
    char_u	*name;
{
    gui.norm_pixel = gui_mch_get_color(name);
    hl_set_fg_color_name(vim_strsave(name));
}

    void
gui_set_bg_color(name)
    char_u	*name;
{
    gui.back_pixel = gui_mch_get_color(name);
    hl_set_bg_color_name(vim_strsave(name));
}

#ifdef USE_GUI_X11
    void
gui_new_scrollbar_colors()
{
    WIN	    *wp;

    for (wp = firstwin; wp != NULL; wp = wp->w_next)
    {
	gui_mch_set_scrollbar_colors(&(wp->w_scrollbars[SBAR_LEFT]));
	gui_mch_set_scrollbar_colors(&(wp->w_scrollbars[SBAR_RIGHT]));
    }
    gui_mch_set_scrollbar_colors(&gui.bottom_sbar);
}
#endif

/*
 * Call this when focus has changed.
 */
    void
gui_focus_change(in_focus)
    int	    in_focus;
{
    gui.in_focus = in_focus;
    out_flush();		/* make sure output has been written */
    gui_update_cursor(TRUE, FALSE);
}

/*
 * Duplicate the menu item text and then process to see if a mnemonic key
 * and/or accelerator text has been identified.
 * Returns a pointer to allocated memory, or NULL for failure.
 * If mnemonic != NULL, *mnemonic is set to the character after the first '&'.
 * If actext != NULL, *actext is set to the text after the first TAB.
 */
    char_u *
gui_menu_text(text, mnemonic, actext)
    char_u	*text;
    int		*mnemonic;
    char_u	**actext;
{
    char_u	*p;
    char_u	*menu_text;

    /* Locate accelerator text, after the first TAB */
    p = vim_strchr(text, TAB);
    if (p != NULL)
    {
	if (actext != NULL)
	    *actext = vim_strsave(p + 1);
	menu_text = vim_strnsave(text, (int)(p - text));
    }
    else
	menu_text = vim_strsave(text);
    if (menu_text != NULL)
    {
	p = vim_strchr(menu_text, '&');
	if (p != NULL)
	{
	    if (mnemonic != NULL)
		*mnemonic = p[1];
	    mch_memmove(p, p + 1, STRLEN(p));
	}
    }
    return menu_text;
}

/*
 * Return TRUE if "name" can be a menu in the MenuBar.
 */
    int
gui_menubar_menu(name)
    char_u	*name;
{
    return (!gui_popup_menu(name)
	    && !gui_toolbar_menu(name)
	    && *name != MNU_HIDDEN_CHAR);
}

    int
gui_popup_menu(name)
    char_u	*name;
{
    return (STRNCMP(name, "PopUp", 5) == 0);
}

    int
gui_toolbar_menu(name)
    char_u	*name;
{
    return (STRNCMP(name, "ToolBar", 7) == 0);
}

/*
 * Called when the mouse moved (but not when dragging).
 * Ignore this while in command-line mode, or while changing the window layout
 * (causes a mouse-move event in Win32 GUI).
 */
    void
gui_mouse_moved(y)
    int		y;
{
    WIN		*wp;
    char_u	st[6];
    int		x;

    if (p_mousef && State != CMDLINE && !need_mouse_correct && gui.in_focus)
    {
	x = gui_mch_get_mouse_x();
	/* Don't move the mouse when it's left or right of the Vim window */
	if (x < 0 || x > Columns * gui.char_width)
	    return;
	wp = y2win(y);
	if (wp == curwin || wp == NULL)
	    return;	/* still in the same old window, or none at all */

	/*
	 * format a mouse click on status line input
	 * ala gui_send_mouse_event(0, x, y, 0, 0);
	 */
	st[0] = CSI;
	st[1] = KS_MOUSE;
	st[2] = K_FILLER;
	st[3] = (char_u)MOUSE_LEFT;
	st[4] = (char_u)(2 + ' ');
	st[5] = (char_u)(wp->w_height + wp->w_winpos + ' ' + 1);
	add_to_input_buf(st, 6);
    }
}

/*
 * Called when mouse should be moved to window with focus.
 */
    void
gui_mouse_correct()
{
    int		x, y;
    WIN		*wp = NULL;

    need_mouse_correct = FALSE;
    if (gui.in_use && p_mousef)
    {
	x = gui_mch_get_mouse_x();
	/* Don't move the mouse when it's left or right of the Vim window */
	if (x < 0 || x > Columns * gui.char_width)
	    return;
	y = gui_mch_get_mouse_y();
	if (y >= 0)
	    wp = y2win(y);
	if (wp != curwin && wp != NULL)	/* If in other than current window */
	{
	    validate_cline_row();
	    gui_mch_setmouse((int)Columns * gui.char_width - 3,
			 (curwin->w_winpos + curwin->w_wrow) * gui.char_height
						     + (gui.char_height) / 2);
	}
    }
}

/*
 * Find window where the mouse pointer "y" coordinate is in.
 */
    static WIN *
y2win(y)
    int		y;
{
    int		row;
    WIN		*wp;

    row = Y_2_ROW(y);
    if (row < firstwin->w_winpos)    /* before first window (Toolbar) */
	return NULL;
    for (wp = firstwin; wp != NULL; wp = wp->w_next)
	if (row < wp->w_winpos + wp->w_height + wp->w_status_height)
	    break;
    return wp;
}

/*
 * Display the Special "PopUp" menu as a pop-up at the current mouse
 * position.  The "PopUpn" menu is for Normal mode, "PopUpi" for Insert mode,
 * etc.
 */
    void
gui_show_popupmenu()
{
    GuiMenu	*menu;
    int		mode;

    mode = get_menu_mode();
    if (mode == MENU_INDEX_INVALID)
	return;
    mode = MENU_MODE_CHARS[mode];

    for (menu = gui.root_menu; menu != NULL; menu = menu->next)
	if (STRNCMP("PopUp", menu->name, 5) == 0 && menu->name[5] == mode)
	    break;

    /* Only show a popup when it is defined and has entries */
    if (menu != NULL && menu->children != NULL)
	gui_mch_show_popupmenu(menu);
}
