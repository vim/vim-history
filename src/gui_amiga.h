/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 *
 * Windows GUI.
 *
 * GUI support for Microsoft Windows.  Win32 initially; maybe Win16 later 
 *
 * George V. Reilly <gvr@halcyon.com> wrote the original Win32 GUI.
 * Robert Webb reworked it to use the existing GUI stuff and added menu,
 * scrollbars, etc.
 *
 * Note: Clipboard stuff, for cutting and pasting text to other windows, is in
 * os_win32.c.	(It can also be done from the terminal version).
 */

#if !defined(__GUI_AMIGA__H)
#define __GUI_AMIGA__H

#define SetAttrib(_ptr,_attrib,_value) ((_ptr)->_attrib=(_value))

#if defined(USE_GUI_AMIGA)

#include <intuition/intuition.h>

enum event {
    ev_NullEvent,
    ev_MenuVerify,
    ev_MenuPick,
    ev_CloseWindow,
    ev_NewSize,
    ev_RefreshWindow,
    ev_MouseButtons,
    ev_MouseMove,
    ev_GadgetDown,
    ev_GadgetUp,
    ev_KeyStroke,
    ev_IntuiTicks,
    ev_MenuHelp,
    ev_GadgetHelp
};

struct MyMenuItem {
    struct MenuItem menuItem;
    struct GuiMenu *guiMenu;
};

union myMenuItemUnion {
    struct MenuItem menuItem;
    struct MyMenuItem myMenuItem;
};


#include "vim.h"


#endif /* USE_GUI_AMIGA*/
#endif /* __GUI_AMIGA__H */

