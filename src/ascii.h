/* vi:ts=4:sw=4
 *
 * VIM - Vi IMitation
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
 */

/*
 * Definitions of various common control characters
 */

#define NUL 					'\000'
#define BS						'\010'
#define BS_STR					"\010"
#define TAB 					'\011'
#define NL						'\012'
#define NL_STR					"\012"
#define CR						'\015'
#define ESC 					'\033'
#define ESC_STR 				"\033"
#define DEL 					'\x7f'
#define CSI 					0x9b

#define CTRL(x) ((x) & 0x1f)
