/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * Definitions of various common control characters
 */

#define NUL 					'\000'
#define BS						'\010'
#define BS_STR					(char_u *)"\010"
#define TAB 					'\011'
#define NL						'\012'
#define NL_STR					(char_u *)"\012"
#define CR						'\015'
#define ESC 					'\033'
#define ESC_STR 				(char_u *)"\033"
#define DEL 					0x7f
#define CSI 					0x9b

#define Ctrl(x) ((x) & 0x1f)
#define Meta(x) ((x) | 0x80)

/*
 * character that separates dir names in a path
 */
#ifdef MSDOS
# define PATHSEP '\\'
# define PATHSEPSTR "\\"
#else
# define PATHSEP '/'
# define PATHSEPSTR "/"
#endif
