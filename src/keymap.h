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
 * Keycode definitions for special keys
 *
 * On systems that have any of these keys, the routine 'inchar' in the
 * machine-dependent code should return one of the codes here.
 */

#define K_CCIRCM		0x1e	/* control circumflex */

#define K_HELP			0x80
#define K_UNDO			0x81
#define K_INSERT		0x82
#define K_HOME			0x83
#define K_UARROW		0x84
#define K_DARROW		0x85
#define K_LARROW		0x86
#define K_RARROW		0x87
#define K_SUARROW		0x88
#define K_SDARROW		0x89
#define K_SLARROW		0x8a
#define K_SRARROW		0x8b

#define K_F1			0x8c	/* function keys */
#define K_F2			0x8d
#define K_F3			0x8e
#define K_F4			0x8f
#define K_F5			0x90
#define K_F6			0x91
#define K_F7			0x92
#define K_F8			0x93
#define K_F9			0x94
#define K_F10			0x95

#define K_SF1			0x96	/* shifted function keys */
#define K_SF2			0x97
#define K_SF3			0x98
#define K_SF4			0x99
#define K_SF5			0x9a
#define K_SF6			0x9b
#define K_SF7			0x9c
#define K_SF8			0x9d
#define K_SF9			0x9e
#define K_SF10			0x9f

#define K_ZERO			0xd7	/* zero is ignored, this replaces ^@ */
