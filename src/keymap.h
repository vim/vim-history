/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved
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

/*
 * On MSDOS we use other codes, because the codes 0x80 - 0xb0 are used
 * for special characters. If this is changed also change the numbers in
 * term.h and main.c.
 */
#ifdef MSDOS
# define K_OFF 0x30
#else
# define K_OFF 0x00
#endif

#define K_CCIRCM		0x1e	/* control circumflex */

/*
 * careful: the next entries must be in the same order as the termcap strings
 * in term.h and the numbers must be consecutive (used by inchar()).
 */
#define K_UARROW		(K_OFF + 0x80)
#define K_DARROW		(K_OFF + 0x81)
#define K_LARROW		(K_OFF + 0x82)
#define K_RARROW		(K_OFF + 0x83)
#define K_SUARROW		(K_OFF + 0x84)
#define K_SDARROW		(K_OFF + 0x85)
#define K_SLARROW		(K_OFF + 0x86)
#define K_SRARROW		(K_OFF + 0x87)

#define K_F1			(K_OFF + 0x88)	/* function keys */
#define K_F2			(K_OFF + 0x89)
#define K_F3			(K_OFF + 0x8a)
#define K_F4			(K_OFF + 0x8b)
#define K_F5			(K_OFF + 0x8c)
#define K_F6			(K_OFF + 0x8d)
#define K_F7			(K_OFF + 0x8e)
#define K_F8			(K_OFF + 0x8f)
#define K_F9			(K_OFF + 0x90)
#define K_F10			(K_OFF + 0x91)

#define K_SF1			(K_OFF + 0x92)	/* shifted function keys */
#define K_SF2			(K_OFF + 0x93)
#define K_SF3			(K_OFF + 0x94)
#define K_SF4			(K_OFF + 0x95)
#define K_SF5			(K_OFF + 0x96)
#define K_SF6			(K_OFF + 0x97)
#define K_SF7			(K_OFF + 0x98)
#define K_SF8			(K_OFF + 0x99)
#define K_SF9			(K_OFF + 0x9a)
#define K_SF10			(K_OFF + 0x9b)

#define K_HELP			(K_OFF + 0x9c)
#define K_UNDO			(K_OFF + 0x9d)

/*
 * NULs cannot be in the input string, therefore CTRL-@ is replaced by K_ZERO.
 * K_NUL is used for MSDOS extended keys (same value used in term.h).
 */
#define K_NUL			(K_OFF + 0x9e)	/* for MSDOS: special key follows */
#define K_ZERO			(K_OFF + 0x9f)	/* replaces ^@ */
