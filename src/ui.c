/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * ui.c: functions that handle the user interface.
 * 1. Keyboard input stuff, and a bit of windowing stuff.  These are called
 *    before the machine specific stuff (mch_*) so that we can call the GUI
 *    stuff instead if the GUI is running.
 * 2. Clipboard stuff.
 * 3. Input buffer stuff.
 */

#include "vim.h"

    void
ui_write(s, len)
    char_u  *s;
    int	    len;
{
#ifdef USE_GUI
    if (gui.in_use && !gui.dying)
    {
	gui_write(s, len);
	if (p_wd)
	    gui_wait_for_chars(p_wd);
	return;
    }
#endif
#ifndef NO_CONSOLE
    /* Don't output anything in silent mode ("ex -s") */
    if (!silent_mode)
	mch_write(s, len);
#endif
}

/*
 * ui_inchar(): low level input funcion.
 * Get a characters from the keyboard.
 * Return the number of characters that are available.
 * If wtime == 0 do not wait for characters.
 * If wtime == -1 wait forever for characters.
 * If wtime > 0 wait wtime milliseconds for a character.
 */
    int
ui_inchar(buf, maxlen, wtime)
    char_u  *buf;
    int	    maxlen;
    long    wtime;	    /* don't use "time", MIPS cannot handle it */
{
#ifdef USE_GUI
    if (gui.in_use)
    {
	if (!gui_wait_for_chars(wtime))
	    return 0;
	return read_from_input_buf(buf, (long)maxlen);
    }
#endif
#ifndef NO_CONSOLE
    return mch_inchar(buf, maxlen, wtime);
#endif
}

/*
 * return non-zero if a character is available
 */
    int
ui_char_avail()
{
#ifdef USE_GUI
    if (gui.in_use)
    {
	gui_mch_update();
	return !vim_is_input_buf_empty();
    }
#endif
#ifndef NO_CONSOLE
    return mch_char_avail();
#endif
}

/*
 * Delay for the given number of milliseconds.	If ignoreinput is FALSE then we
 * cancel the delay if a key is hit.
 */
    void
ui_delay(msec, ignoreinput)
    long	msec;
    int		ignoreinput;
{
#ifdef USE_GUI
    if (gui.in_use && !ignoreinput)
	gui_wait_for_chars(msec);
    else
#endif
	mch_delay(msec, ignoreinput);
}

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 * When running the GUI iconify the window.
 */
    void
ui_suspend()
{
#ifdef USE_GUI
    if (gui.in_use)
    {
	gui_mch_iconify();
	return;
    }
#endif
    mch_suspend();
}

/*
 * When the OS can't really suspend, call this function to start a shell.
 */
    void
suspend_shell()
{
    MSG_PUTS("new shell started\n");
    mch_call_shell(NULL, SHELL_COOKED);
    need_check_timestamps = TRUE;
}

    int
ui_can_restore_title()
{
#ifdef USE_GUI
    /*
     * If GUI is (going to be) used, we can always set the window title.
     * Saves a bit of time, because the X11 display server does not need to be
     * contacted.
     */
    if (gui.starting || gui.in_use)
	return TRUE;
#endif
    return mch_can_restore_title();
}

    int
ui_can_restore_icon()
{
#ifdef USE_GUI
    /*
     * If GUI is (going to be) used, we can always set the icon name.
     * Saves a bit of time, because the X11 display server does not need to be
     * contacted.
     */
    if (gui.starting || gui.in_use)
	return TRUE;
#endif
    return mch_can_restore_icon();
}

/*
 * Try to get the current window size.	Put the result in Rows and Columns.
 * Return OK when size could be determined, FAIL otherwise.
 */
    int
ui_get_winsize()
{
    int	    retval;

#ifdef USE_GUI
    if (gui.in_use)
	retval = gui_get_winsize();
    else
#endif
	retval = mch_get_winsize();

    /* adjust the default for 'lines' and 'columns' */
    if (retval == OK)
    {
	set_number_default("lines", Rows);
	set_number_default("columns", Columns);
    }
    return retval;
}

/*
 * Set the size of the window according to Rows and Columns, if possible.
 */
    void
ui_set_winsize()
{
#ifdef USE_GUI
    if (gui.in_use)
	gui_set_winsize(FALSE);
    else
#endif
	mch_set_winsize();
}

    void
ui_breakcheck()
{
#ifdef USE_GUI
    if (gui.in_use)
	gui_mch_update();
    else
#endif /* USE_GUI */
	mch_breakcheck();
}

/*****************************************************************************
 * Functions for copying and pasting text between applications.
 * This is always included in a GUI version, but may also be included when the
 * clipboard and mouse is available to a terminal version such as xterm.
 * Note: there are some more functions in ops.c that handle selection stuff.
 */

#ifdef USE_CLIPBOARD

static void clip_invert_area __ARGS((int, int, int, int));
static void clip_yank_non_visual_selection __ARGS((int, int, int, int));
static void clip_get_word_boundaries __ARGS((VimClipboard *, int, int));
static int  clip_get_line_end __ARGS((int));
static void clip_update_non_visual_selection __ARGS((VimClipboard *, int, int,
						    int, int));

#define char_class(c)	(c <= ' ' ? ' ' : vim_iswordc(c))

/*
 * Selection stuff using Visual mode, for cutting and pasting text to other
 * windows.
 */

/*
 * Call this to initialise the clipboard.  Pass it FALSE if the clipboard code
 * is included, but the clipboard can not be used, or TRUE if the clipboard can
 * be used.  Eg unix may call this with FALSE, then call it again with TRUE if
 * the GUI starts.
 */
    void
clip_init(can_use)
    int	    can_use;
{
    clipboard.available = can_use;
    clipboard.owned = FALSE;
    clipboard.start.lnum = 0;
    clipboard.start.col = 0;
    clipboard.end.lnum = 0;
    clipboard.end.col = 0;
    clipboard.state = SELECT_CLEARED;
}

/*
 * Check whether the VIsual area has changed, and if so try to become the owner
 * of the selection, and free any old converted selection we may still have
 * lying around.  If the VIsual mode has ended, make a copy of what was
 * selected so we can still give it to others.	Will probably have to make sure
 * this is called whenever VIsual mode is ended.
 */
    void
clip_update_selection()
{
    FPOS    start, end;

    /* If visual mode is only due to a redo command ("."), then ignore it */
    if (!redo_VIsual_busy && VIsual_active)
    {
	if (lt(VIsual, curwin->w_cursor))
	{
	    start = VIsual;
	    end = curwin->w_cursor;
	}
	else
	{
	    start = curwin->w_cursor;
	    end = VIsual;
	}
	if (!equal(clipboard.start, start) || !equal(clipboard.end, end)
					    || clipboard.vmode != VIsual_mode)
	{
	    clip_clear_selection();
	    clipboard.start = start;
	    clipboard.end = end;
	    clipboard.vmode = VIsual_mode;
	    clip_free_selection();
	    clip_own_selection();
	    clip_mch_set_selection();
	}
    }
}

    void
clip_own_selection()
{
    /*
     * Also want to check somehow that we are reading from the keyboard rather
     * than a mapping etc.
     */
    if (!clipboard.owned)
	clipboard.owned = (clip_mch_own_selection() == OK);
}

    void
clip_lose_selection()
{
    clip_free_selection();
    clipboard.owned = FALSE;
    clip_clear_selection();
    clip_mch_lose_selection();
}

    void
clip_copy_selection()
{
    if (VIsual_active)
    {
	if (vim_strchr(p_guioptions, GO_ASEL) == NULL)
	    clip_update_selection();
	clip_free_selection();
	clip_own_selection();
	if (clipboard.owned)
	    clip_get_selection();
	clip_mch_set_selection();
    }
}

    void
clip_auto_select()
{
    if (vim_strchr(p_guioptions, GO_ASEL) != NULL)
	clip_copy_selection();
}


#ifdef USE_GUI

/*
 * Stuff for general mouse selection, without using Visual mode.
 */

static int clip_compare_pos __ARGS((int row1, int col1, int row2, int col2));

/*
 * Compare two screen positions ala strcmp()
 */
    static int
clip_compare_pos(row1, col1, row2, col2)
    int	    row1;
    int	    col1;
    int	    row2;
    int	    col2;
{
    if (row1 > row2) return( 1);
    if (row1 < row2) return(-1);
    if (col1 > col2) return( 1);
    if (col1 < col2) return(-1);
		     return( 0);
}

/*
 * Start out the selection
 */
/* ARGSUSED */
    void
clip_start_selection(button, x, y, repeated_click, modifiers)
    int	    button;
    int	    x;
    int	    y;
    int	    repeated_click;
    int_u   modifiers;
{
    VimClipboard    *cb = &clipboard;

    if (cb->state == SELECT_DONE)
	clip_clear_selection();

    cb->start.lnum  = check_row(Y_2_ROW(y));
    cb->start.col   = check_col(X_2_COL(x));
    cb->end	    = cb->start;
    cb->origin_row  = (short_u)cb->start.lnum;
    cb->state	    = SELECT_IN_PROGRESS;

    if (repeated_click)
    {
	if (++(cb->mode) > SELECT_MODE_LINE)
	    cb->mode = SELECT_MODE_CHAR;
    }
    else
	cb->mode = SELECT_MODE_CHAR;

#ifdef USE_GUI
    /* clear the cursor until the selection is made */
    gui_undraw_cursor();
#endif

    switch (cb->mode)
    {
	case SELECT_MODE_CHAR:
	    cb->origin_start_col = cb->start.col;
	    cb->word_end_col = clip_get_line_end((int)cb->start.lnum);
	    break;

	case SELECT_MODE_WORD:
	    clip_get_word_boundaries(cb, (int)cb->start.lnum, cb->start.col);
	    cb->origin_start_col = cb->word_start_col;
	    cb->origin_end_col	 = cb->word_end_col;

	    clip_invert_area((int)cb->start.lnum, cb->word_start_col,
			    (int)cb->end.lnum, cb->word_end_col);
	    cb->start.col = cb->word_start_col;
	    cb->end.col   = cb->word_end_col;
	    break;

	case SELECT_MODE_LINE:
	    clip_invert_area((int)cb->start.lnum, 0, (int)cb->start.lnum,
			    (int)Columns);
	    cb->start.col = 0;
	    cb->end.col   = Columns;
	    break;
    }

    cb->prev = cb->start;

#ifdef DEBUG_SELECTION
    printf("Selection started at (%u,%u)\n", cb->start.lnum, cb->start.col);
#endif
}

/*
 * Continue processing the selection
 */
/* ARGSUSED */
    void
clip_process_selection(button, x, y, repeated_click, modifiers)
    int	    button;
    int	    x;
    int	    y;
    int	    repeated_click;
    int_u   modifiers;
{
    VimClipboard    *cb = &clipboard;
    int		    row;
    int_u	    col;
    int		    diff;

    if (button == MOUSE_RELEASE)
    {
	/* Check to make sure we have something selected */
	if (cb->start.lnum == cb->end.lnum && cb->start.col == cb->end.col)
	{
#ifdef USE_GUI
	    if (gui.in_use)
		gui_update_cursor(FALSE);
#endif
	    cb->state = SELECT_CLEARED;
	    return;
	}

#ifdef DEBUG_SELECTION
	printf("Selection ended: (%u,%u) to (%u,%u)\n", cb->start.lnum,
		cb->start.col, cb->end.lnum, cb->end.col);
#endif
	clip_free_selection();
	clip_own_selection();
	clip_yank_non_visual_selection((int)cb->start.lnum, cb->start.col,
					      (int)cb->end.lnum, cb->end.col);
	clip_mch_set_selection();
#ifdef USE_GUI
	if (gui.in_use)
	    gui_update_cursor(FALSE);
#endif

	cb->state = SELECT_DONE;
	return;
    }

    row = check_row(Y_2_ROW(y));
    col = check_col(X_2_COL(x));

    if (col == cb->prev.col && row == cb->prev.lnum)
	return;

    /*
     * When extending the selection with the right mouse button, swap the
     * start and end if the position is before half the selection
     */
    if (cb->state == SELECT_DONE && button == MOUSE_RIGHT)
    {
	/*
	 * If the click is before the start, or the click is inside the
	 * selection and the start is the closest side, set the origin to the
	 * end of the selection.
	 */
	if (clip_compare_pos(row, col, (int)cb->start.lnum, cb->start.col) < 0
		|| (clip_compare_pos(row, col,
					   (int)cb->end.lnum, cb->end.col) < 0
		    && (((cb->start.lnum == cb->end.lnum
			    && cb->end.col - col > col - cb->start.col))
			|| ((diff = (cb->end.lnum - row) -
						   (row - cb->start.lnum)) > 0
			    || (diff == 0 && col < (cb->start.col +
							 cb->end.col) / 2)))))
	{
	    cb->origin_row = (short_u)cb->end.lnum;
	    cb->origin_start_col = cb->end.col - 1;
	    cb->origin_end_col = cb->end.col;
	}
	else
	{
	    cb->origin_row = (short_u)cb->start.lnum;
	    cb->origin_start_col = cb->start.col;
	    cb->origin_end_col = cb->start.col;
	}
	if (cb->mode == SELECT_MODE_WORD)
	{
	    clip_get_word_boundaries(cb, cb->origin_row, cb->origin_start_col);
	    cb->origin_start_col = cb->word_start_col;
	    cb->origin_end_col	 = cb->word_end_col;
	}
    }

    /* set state, for when using the right mouse button */
    cb->state = SELECT_IN_PROGRESS;

#ifdef DEBUG_SELECTION
    printf("Selection extending to (%d,%d)\n", row, col);
#endif

    switch (cb->mode)
    {
	case SELECT_MODE_CHAR:
	    /* If we're on a different line, find where the line ends */
	    if (row != cb->prev.lnum)
		cb->word_end_col = clip_get_line_end(row);

	    /* See if we are before or after the origin of the selection */
	    if (clip_compare_pos(row, col, cb->origin_row,
						   cb->origin_start_col) >= 0)
	    {
		if (col >= (int)cb->word_end_col)
		    clip_update_non_visual_selection(cb, cb->origin_row,
			    cb->origin_start_col, row, (int)Columns);
		else
		    clip_update_non_visual_selection(cb, cb->origin_row,
			    cb->origin_start_col, row, col + 1);
	    }
	    else
	    {
		if (col >= (int)cb->word_end_col)
		    clip_update_non_visual_selection(cb, row, cb->word_end_col,
			    cb->origin_row, cb->origin_start_col + 1);
		else
		    clip_update_non_visual_selection(cb, row, col,
			    cb->origin_row, cb->origin_start_col + 1);
	    }
	    break;

	case SELECT_MODE_WORD:
	    /* If we are still within the same word, do nothing */
	    if (row == cb->prev.lnum && col >= (int)cb->word_start_col
		    && col < (int)cb->word_end_col)
		return;

	    /* Get new word boundaries */
	    clip_get_word_boundaries(cb, row, col);

	    /* Handle being after the origin point of selection */
	    if (clip_compare_pos(row, col, cb->origin_row,
		    cb->origin_start_col) >= 0)
		clip_update_non_visual_selection(cb, cb->origin_row,
			cb->origin_start_col, row, cb->word_end_col);
	    else
		clip_update_non_visual_selection(cb, row, cb->word_start_col,
			cb->origin_row, cb->origin_end_col);
	    break;

	case SELECT_MODE_LINE:
	    if (row == cb->prev.lnum)
		return;

	    if (clip_compare_pos(row, col, cb->origin_row,
		    cb->origin_start_col) >= 0)
		clip_update_non_visual_selection(cb, cb->origin_row, 0, row,
			(int)Columns);
	    else
		clip_update_non_visual_selection(cb, row, 0, cb->origin_row,
			(int)Columns);
	    break;
    }

    cb->prev.lnum = row;
    cb->prev.col  = col;

#ifdef DEBUG_SELECTION
	printf("Selection is: (%u,%u) to (%u,%u)\n", cb->start.lnum,
		cb->start.col, cb->end.lnum, cb->end.col);
#endif
}

/*
 * Called after an Expose event to redraw the selection
 */
    void
clip_redraw_selection(x, y, w, h)
    int	    x;
    int	    y;
    int	    w;
    int	    h;
{
    VimClipboard    *cb = &clipboard;
    int		    row1, col1, row2, col2;
    int		    row;
    int		    start;
    int		    end;

    if (cb->state == SELECT_CLEARED)
	return;

#ifdef USE_GUI	    /* TODO: how do we invert for non-GUI versions? */
    row1 = check_row(Y_2_ROW(y));
    col1 = check_col(X_2_COL(x));
    row2 = check_row(Y_2_ROW(y + h - 1));
    col2 = check_col(X_2_COL(x + w - 1));

    /* Limit the rows that need to be re-drawn */
    if (cb->start.lnum > row1)
	row1 = cb->start.lnum;
    if (cb->end.lnum < row2)
	row2 = cb->end.lnum;

    /* Look at each row that might need to be re-drawn */
    for (row = row1; row <= row2; row++)
    {
	/* For the first selection row, use the starting selection column */
	if (row == cb->start.lnum)
	    start = cb->start.col;
	else
	    start = 0;

	/* For the last selection row, use the ending selection column */
	if (row == cb->end.lnum)
	    end = cb->end.col;
	else
	    end = Columns;

	if (col1 > start)
	    start = col1;

	if (col2 < end)
	    end = col2 + 1;

	if (end > start)
	    gui_mch_invert_rectangle(row, start, 1, end - start);
    }
#endif
}

/*
 * Called from outside to clear selected region from the display
 */
    void
clip_clear_selection()
{
    VimClipboard    *cb = &clipboard;

    if (cb->state == SELECT_CLEARED)
	return;

    clip_invert_area((int)cb->start.lnum, cb->start.col, (int)cb->end.lnum,
	    cb->end.col);
    cb->state = SELECT_CLEARED;
}

/*
 * Called before the screen is scrolled up or down.  Adjusts the line numbers
 * of the selection.  Call with big number when clearing the screen.
 */
    void
clip_scroll_selection(rows)
    int	    rows;		/* negative for scroll down */
{
    int	    lnum;

    if (clipboard.state == SELECT_CLEARED)
	return;

    lnum = clipboard.start.lnum - rows;
    if (lnum <= 0)
	clipboard.start.lnum = 0;
    else if (lnum >= screen_Rows)	/* scrolled off of the screen */
	clipboard.state = SELECT_CLEARED;
    else
	clipboard.start.lnum = lnum;

    lnum = clipboard.end.lnum - rows;
    if (lnum < 0)			/* scrolled off of the screen */
	clipboard.state = SELECT_CLEARED;
    else if (lnum >= screen_Rows)
	clipboard.end.lnum = screen_Rows - 1;
    else
	clipboard.end.lnum = lnum;
}

/*
 * Invert a region of the display between a starting and ending row and column
 */
    static void
clip_invert_area(row1, col1, row2, col2)
    int	    row1;
    int	    col1;
    int	    row2;
    int	    col2;
{
#ifdef USE_GUI	    /* TODO: how do we invert for non-GUI versions? */
    /* Swap the from and to positions so the from is always before */
    if (clip_compare_pos(row1, col1, row2, col2) > 0)
    {
	int tmp_row, tmp_col;
	tmp_row = row1;
	tmp_col = col1;
	row1	= row2;
	col1	= col2;
	row2	= tmp_row;
	col2	= tmp_col;
    }

    /* If all on the same line, do it the easy way */
    if (row1 == row2)
    {
	gui_mch_invert_rectangle(row1, col1, 1, col2 - col1);
	return;
    }

    /* Handle a piece of the first line */
    if (col1 > 0)
    {
	gui_mch_invert_rectangle(row1, col1, 1, (int)Columns - col1);
	row1++;
    }

    /* Handle a piece of the last line */
    if (col2 < Columns - 1)
    {
	gui_mch_invert_rectangle(row2, 0, 1, col2);
	row2--;
    }

    /* Handle the rectangle thats left */
    if (row2 >= row1)
	gui_mch_invert_rectangle(row1, 0, row2 - row1 + 1, (int)Columns);
#endif
}

/*
 * Yank the currently selected area into the special selection buffer so it
 * will be available for pasting.
 */
    static void
clip_yank_non_visual_selection(row1, col1, row2, col2)
    int	    row1;
    int	    col1;
    int	    row2;
    int	    col2;
{
    char_u  *buffer;
    char_u  *bufp;
    int	    row;
    int	    start_col;
    int	    end_col;
    int	    line_end_col;
    int	    add_newline_flag = FALSE;

    /*
     * Make sure row1 <= row2, and if row1 == row2 that col1 <= col2.
     */
    if (row1 > row2)
    {
	row = row1; row1 = row2; row2 = row;
	row = col1; col1 = col2; col2 = row;
    }
    else if (row1 == row2 && col1 > col2)
    {
	row = col1; col1 = col2; col2 = row;
    }

    /* Create a temporary buffer for storing the text */
    buffer = lalloc((row2 - row1 + 1) * Columns + 1, TRUE);
    if (buffer == NULL)	    /* out of memory */
	return;

    /* Process each row in the selection */
    for (bufp = buffer, row = row1; row <= row2; row++)
    {
	if (row == row1)
	    start_col = col1;
	else
	    start_col = 0;

	if (row == row2)
	    end_col = col2;
	else
	    end_col = Columns;

	line_end_col = clip_get_line_end(row);

	/* See if we need to nuke some trailing whitespace */
	if (end_col >= Columns && (row < row2 || end_col > line_end_col))
	{
	    /* Get rid of trailing whitespace */
	    end_col = line_end_col;
	    if (end_col < start_col)
		end_col = start_col;

	    /* If the last line extended to the end, add an extra newline */
	    if (row == row2)
		add_newline_flag = TRUE;
	}

	/* If after the first row, we need to always add a newline */
	if (row > row1)
	    *bufp++ = NL;

	if (row < screen_Rows && end_col <= screen_Columns)
	{
	    STRNCPY(bufp, &LinePointers[row][start_col], end_col - start_col);
	    bufp += end_col - start_col;
	}
    }

    /* Add a newline at the end if the selection ended there */
    if (add_newline_flag)
	*bufp++ = NL;

    clip_yank_selection(MCHAR, buffer, (long)(bufp - buffer));
    vim_free(buffer);
}

/*
 * Find the starting and ending positions of the word at the given row and
 * column.
 */
    static void
clip_get_word_boundaries(cb, row, col)
    VimClipboard    *cb;
    int		    row;
    int		    col;
{
    char    start_class;
    int	    temp_col;

    if (row >= screen_Rows || col >= screen_Columns)
	return;

    start_class = char_class(LinePointers[row][col]);

    temp_col = col;
    for ( ; temp_col > 0; temp_col--)
	if (char_class(LinePointers[row][temp_col - 1]) != start_class)
	    break;

    cb->word_start_col = temp_col;

    temp_col = col;
    for ( ; temp_col < screen_Columns; temp_col++)
	if (char_class(LinePointers[row][temp_col]) != start_class)
	    break;
    cb->word_end_col = temp_col;

#ifdef DEBUG_SELECTION
    printf("Current word: col %u to %u\n", cb->word_start_col,
	    cb->word_end_col);
#endif
}

/*
 * Find the column position for the last non-whitespace character on the given
 * line.
 */
    static int
clip_get_line_end(row)
    int		row;
{
    int	    i;

    if (row >= screen_Rows)
	return 0;
    for (i = screen_Columns; i > 0; i--)
	if (LinePointers[row][i - 1] != ' ')
	    break;
    return i;
}

/*
 * Update the currently selected region by adding and/or subtracting from the
 * beginning or end and inverting the changed area(s).
 */
    static void
clip_update_non_visual_selection(cb, row1, col1, row2, col2)
    VimClipboard    *cb;
    int		    row1;
    int		    col1;
    int		    row2;
    int		    col2;
{
    /* See if we changed at the beginning of the selection */
    if (row1 != cb->start.lnum || col1 != (int)cb->start.col)
    {
	clip_invert_area(row1, col1, (int)cb->start.lnum, cb->start.col);
	cb->start.lnum = row1;
	cb->start.col  = col1;
    }

    /* See if we changed at the end of the selection */
    if (row2 != cb->end.lnum || col2 != (int)cb->end.col)
    {
	clip_invert_area(row2, col2, (int)cb->end.lnum, cb->end.col);
	cb->end.lnum = row2;
	cb->end.col  = col2;
    }
}

#else /* If USE_GUI not defined */

/*
 * Called from outside to clear selected region from the display
 */
    void
clip_clear_selection()
{
    /*
     * Dummy version for now... the point of this code is to set the selected
     * area back to "normal" colour if we are clearing the selection. As we
     * don't have GUI-style mouse selection, we can ignore this for now.
     * Eventually we could actually invert the area in a terminal by redrawing
     * in reverse mode, but we don't do that yet.
     */
    clipboard.state = SELECT_CLEARED;
}
#endif /* USE_GUI */


#endif /* USE_CLIPBOARD */

/*****************************************************************************
 * Functions that handle the input buffer.
 * This is used for any GUI version, and the unix terminal version.
 *
 * For Unix, the input characters are buffered to be able to check for a
 * CTRL-C.  This should be done with signals, but I don't know how to do that
 * in a portable way for a tty in RAW mode.
 */

#if defined(UNIX) || defined(USE_GUI) || defined(OS2)

/*
 * Internal typeahead buffer.  Includes extra space for long key code
 * descriptions which would otherwise overflow.  The buffer is considered full
 * when only this extra space (or part of it) remains.
 */
#define INBUFLEN 250

static char_u	inbuf[INBUFLEN + MAX_KEY_CODE_LEN];
static int	inbufcount = 0;	    /* number of chars in inbuf[] */

/*
 * vim_is_input_buf_full(), vim_is_input_buf_empty(), add_to_input_buf(), and
 * trash_input_buf() are functions for manipulating the input buffer.  These
 * are used by the gui_* calls when a GUI is used to handle keyboard input.
 */

    int
vim_is_input_buf_full()
{
    return (inbufcount >= INBUFLEN);
}

    int
vim_is_input_buf_empty()
{
    return (inbufcount == 0);
}

    int
vim_free_in_input_buf()
{
    return (INBUFLEN - inbufcount);
}

/* Add the given bytes to the input buffer */
    void
add_to_input_buf(s, len)
    char_u  *s;
    int	    len;
{
    if (inbufcount + len > INBUFLEN + MAX_KEY_CODE_LEN)
	return;	    /* Shouldn't ever happen! */

    while (len--)
	inbuf[inbufcount++] = *s++;
}

/* Remove everything from the input buffer.  Called when ^C is found */
    void
trash_input_buf()
{
    inbufcount = 0;
}

/*
 * Read as much data from the input buffer as possible up to maxlen, and store
 * it in buf.
 * Note: this function used to be Read() in unix.c
 */
    int
read_from_input_buf(buf, maxlen)
    char_u  *buf;
    long    maxlen;
{
    if (inbufcount == 0)	/* if the buffer is empty, fill it */
	fill_input_buf(TRUE);
    if (maxlen > inbufcount)
	maxlen = inbufcount;
    vim_memmove(buf, inbuf, (size_t)maxlen);
    inbufcount -= maxlen;
    if (inbufcount)
	vim_memmove(inbuf, inbuf + maxlen, (size_t)inbufcount);
    return (int)maxlen;
}

    void
fill_input_buf(exit_on_error)
    int	exit_on_error;
{
#if defined(UNIX) || defined(OS2)
    int		len;
    int		try;
    static int	did_read_something = FALSE;
#endif

#ifdef USE_GUI
    if (gui.in_use)
    {
	gui_mch_update();
	return;
    }
#endif
#if defined(UNIX) || defined(OS2)
    if (vim_is_input_buf_full())
	return;
    /*
     * Fill_input_buf() is only called when we really need a character.
     * If we can't get any, but there is some in the buffer, just return.
     * If we can't get any, and there isn't any in the buffer, we give up and
     * exit Vim.
     */
# ifdef __BEOS__
    /*
     * On the BeBox version (for now), all input is secretly performed within
     * beos_select() which is called from RealWaitForChar().
     */
    while (!vim_is_input_buf_full() && RealWaitForChar(read_cmd_fd, 0))
	    ;
    len = inbufcount;
    inbufcount = 0;
# else

#ifdef USE_SNIFF
    if (sniff_request_waiting)
    {
	add_to_input_buf((char_u *)"\233sniff",6); /* results in K_SNIFF */
	sniff_request_waiting = 0;
	want_sniff_request = 0;
	return;
    }
#endif

    for (try = 0; try < 100; ++try)
    {
	len = read(read_cmd_fd, (char *)inbuf + inbufcount,
					     (size_t)(INBUFLEN - inbufcount));
	if (len > 0 || got_int)
	    break;
	/*
	 * If reading stdin results in an error, continue reading stderr.
	 * This helps when using "foo | xargs vim".
	 */
	if (!did_read_something && !isatty(read_cmd_fd) && read_cmd_fd == 0)
	    read_cmd_fd = 2;
	if (!exit_on_error)
	    return;
    }
# endif
    if (len <= 0 && !got_int)
	read_error_exit();
    did_read_something = TRUE;
    if (got_int)
    {
	inbuf[inbufcount] = 3;
	inbufcount = 1;
    }
    else
	while (len-- > 0)
	{
	    /*
	     * if a CTRL-C was typed, remove it from the buffer and set got_int
	     */
	    if (inbuf[inbufcount] == 3)
	    {
		/* remove everything typed before the CTRL-C */
		vim_memmove(inbuf, inbuf + inbufcount, (size_t)(len + 1));
		inbufcount = 0;
		got_int = TRUE;
	    }
	    ++inbufcount;
	}
#endif /* UNIX or OS2 */
}
#endif /* defined(UNIX) || defined(USE_GUI) || defined(OS2) */

/*
 * Exit because of an input read error.
 */
    void
read_error_exit()
{
    if (silent_mode)	/* Normal way to exit for "ex -s" */
	getout(0);
    STRCPY(IObuff, "Vim: Error reading input, exiting...\n");
    preserve_exit();
}
