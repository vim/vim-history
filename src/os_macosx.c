/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * os_macosx.c -- election of os_mac.c or os_unix.c
 *
 */

#ifdef MACOS_X_UNIX
# include "os_unix.c"
#else
# include "os_mac.c"
#endif
