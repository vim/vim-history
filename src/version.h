/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Define the version number, name, etc.
 * Be careful, keep the numbers in sync!
 * This could be produced by a program, but it doesn't change very often.
 */

#define VIM_VERSION_MAJOR		 5
#define VIM_VERSION_MAJOR_STR		"5"
#define VIM_VERSION_MINOR		 0
#define VIM_VERSION_MINOR_STR		"0"
#define VIM_VERSION_BUILD		 25
#define VIM_VERSION_BUILD_STR		"25"
#define VIM_VERSION_PATCHLEVEL		 0
#define VIM_VERSION_PATCHLEVEL_STR	"0"

/*
 * VIM_VERSION_SHORT is copied into the swap file (max. length is 6 chars).
 * VIM_VERSION_MEDIUM is used for the startup-screen.
 * VIM_VERSION_LONG is used for the ":version" command and "Vim -h".
 */
#define VIM_VERSION_SHORT	"5.0"
#define VIM_VERSION_MEDIUM	"5.0"
#define VIM_VERSION_LONG	"VIM - Vi IMproved 5.0 (1998 Feb 19)"
#define VIM_VERSION_LONG_DATE	"VIM - Vi IMproved 5.0 (1998 Feb 19, compiled "
