/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * This file contains the machine dependent escape sequences that the editor
 * needs to perform various operations. Some of the sequences here are
 * optional. Anything not available should be indicated by a null string. In
 * the case of insert/delete line sequences, the editor checks the capability
 * and works around the deficiency, if necessary.
 */

#ifdef SASC
/*
 * the SAS C compiler has a bug that makes typedefs being forget sometimes
 */
typedef unsigned char char_u;
#endif

/*
 * the terminal capabilities are stored in this structure
 * keep in sync with array in term.c
 */
typedef struct _tcarr
{
/* output codes */
  char_u *t_name;	/* name of this terminal entry */
  char_u *t_el;		/* el       ce	clear to end of line */
  char_u *t_il;		/* il1      al	add new blank line */
  char_u *t_cil;	/* il       AL	add number of blank lines */
  char_u *t_dl;		/* dl1      dl	delete line */
  char_u *t_cdl;	/* dl       DL	delete number of lines */
  char_u *t_cs;		/*          cs	scroll region */
  char_u *t_ed;		/* clear    cl	clear screen */
  char_u *t_ci;		/* civis    vi	cursur invisible */
  char_u *t_cv;		/* cnorm    ve	cursur visible */
  char_u *t_cvv;	/* cvvis    vs  cursor very visible */
  char_u *t_tp;		/* sgr0     me	normal mode */
  char_u *t_ti;		/* rev      mr	reverse mode */
  char_u *t_tb;		/* bold     md	bold mode */
  char_u *t_se;		/* rmso     se	normal mode */
  char_u *t_so;		/* smso     so	standout mode */
  char_u *t_ms;		/* msgr     ms	save to move cursor in reverse mode */
  char_u *t_cm;		/* cup      cm	cursor motion */
  char_u *t_sr;		/* ri       sr	scroll reverse (backward) */
  char_u *t_cri;	/* cuf      RI	cursor number of chars right */
  char_u *t_vb;		/* flash    vb	visual bell */
  char_u *t_ks;		/* smkx     ks	put terminal in "keypad transmit" mode */
  char_u *t_ke;		/* rmkx     ke	out of "keypad transmit" mode */
  char_u *t_ts;		/*          ti	put terminal in termcap mode */
  char_u *t_te;		/*          te	out of termcap mode */

/* key codes */
  char_u *t_ku;		/* kcuu1    ku	arrow up */
  char_u *t_kd;		/* kcud1    kd	arrow down */
  char_u *t_kl;		/* kcub1    kl	arrow left */
  char_u *t_kr;		/* kcuf1    kr	arrow right */
  char_u *t_sku;	/* shift arrow up */
  char_u *t_skd;	/* shift arrow down */
  char_u *t_skl;	/* kLFT     #4	shift arrow left */
  char_u *t_skr;	/* kRIT     %	shift arrow right */
  char_u *t_f1;		/* kf1      k1	function key 1 */
  char_u *t_f2;		/* kf2      k2	function key 2 */
  char_u *t_f3;		/* kf3      k3	function key 3 */
  char_u *t_f4;		/* kf4      k4	function key 4 */
  char_u *t_f5;		/* kf5      k5	function key 5 */
  char_u *t_f6;		/* kf6      k6	function key 6 */
  char_u *t_f7;		/* kf7      k7	function key 7 */
  char_u *t_f8;		/* kf8      k8	function key 8 */
  char_u *t_f9;		/* kf9      k9	function key 9 */
  char_u *t_f10;	/* kf10     k;	function key 10 */
  char_u *t_sf1;	/* kf11     F1	shifted function key 1 */
  char_u *t_sf2;	/* kf12     F2	shifted function key 2 */
  char_u *t_sf3;	/* kf13     F3	shifted function key 3 */
  char_u *t_sf4;	/* kf14     F4	shifted function key 4 */
  char_u *t_sf5;	/* kf15     F5	shifted function key 5 */
  char_u *t_sf6;	/* kf16     F6	shifted function key 6 */
  char_u *t_sf7;	/* kf17     F7	shifted function key 7 */
  char_u *t_sf8;	/* kf18     F8	shifted function key 8 */
  char_u *t_sf9;	/* kf19     F9	shifted function key 9 */
  char_u *t_sf10;	/* kf20     FA	shifted function key 10 */
  char_u *t_help;	/* khlp     %1	help key */
  char_u *t_undo;	/* kund     &8	undo key */
  /* adjust inchar() for last key entry! */

  char_u *t_csc;	/* -		-	cursor relative to scrolling region */
} Tcarr;

extern Tcarr term_strings;	/* currently used terminal strings */

/*
 * strings used for terminal
 */
#define T_EL	(term_strings.t_el)
#define T_IL	(term_strings.t_il)
#define T_CIL	(term_strings.t_cil)
#define T_DL	(term_strings.t_dl)
#define T_CDL	(term_strings.t_cdl)
#define T_CS	(term_strings.t_cs)
#define T_ED	(term_strings.t_ed)
#define T_CI	(term_strings.t_ci)
#define T_CV	(term_strings.t_cv)
#define T_CVV	(term_strings.t_cvv)
#define T_TP	(term_strings.t_tp)
#define T_TI	(term_strings.t_ti)
#define T_TB	(term_strings.t_tb)
#define T_SE	(term_strings.t_se)
#define T_SO	(term_strings.t_so)
#define T_MS	(term_strings.t_ms)
#define T_CM	(term_strings.t_cm)
#define T_SR	(term_strings.t_sr)
#define T_CRI	(term_strings.t_cri)
#define T_VB	(term_strings.t_vb)
#define T_KS	(term_strings.t_ks)
#define T_KE	(term_strings.t_ke)
#define T_TS	(term_strings.t_ts)
#define T_TE	(term_strings.t_te)
#define T_CSC	(term_strings.t_csc)


#ifndef TERMINFO
# ifndef NO_BUILTIN_TCAPS
/*
 * here are the builtin termcap entries.
 * They not stored as complete Tcarr structures, as such a structure 
 * is to big. 
 * Each termcap is a concatenated string of entries, where '\0' characters
 * followed by a skip character sepereate the capabilities. The skip 
 * character is the relative structure offset for the following entry.
 * See parse_builtin_tcap() in term.c for all details.
 */
#  define AMIGA_TCAP "amiga\0\
\0\033[K\0\
\0\033[L\0\
\0\033[%dL\0\
\0\033[M\0\
\0\033[%dM\0\
\1\014\0\
\0\033[0 p\0\
\0\033[1 p\0\
\1\033[0m\0\
\0\033[7m\0\
\0\033[1m\0\
\0\033[0m\0\
\0\033[33m\0\
\0\001\0\
\0\033[%i%d;%dH\0\
\1\033[%dC\0\
\5\233A\0\
\0\233B\0\
\0\233D\0\
\0\233C\0\
\0\233T\0\
\0\233S\0\
\0\233 A\0\
\0\233 @\0\
\0\233\060~\0\
\0\233\061~\0\
\0\233\062~\0\
\0\233\063~\0\
\0\233\064~\0\
\0\233\065~\0\
\0\233\066~\0\
\0\233\067~\0\
\0\233\070~\0\
\0\233\071~\0\
\0\233\061\060~\0\
\0\233\061\061~\0\
\0\233\061\062~\0\
\0\233\061\063~\0\
\0\233\061\064~\0\
\0\233\061\065~\0\
\0\233\061\066~\0\
\0\233\061\067~\0\
\0\233\061\070~\0\
\0\233\061\071~\0\
\0\233?~\0\
\0\0"

#  define ATARI_TCAP "atari\0\
\0\033l\0\
\0\033L\0\
\1\033M\0\
\2\033E\0\
\0\033f\0\
\0\033e\0\
\0\0"

#  define ANSI_TCAP "ansi\0\
\0\033[2K\0\
\0\033[L\0\
\0\033[%dL\0\
\0\033[M\0\
\0\033[%dM\0\
\1\033[2J\0\
\3\033[0m\0\
\0\033[7m\0\
\3\001\0\
\0\033[%i%d;%dH\0\
\1\033[%dC\0\
\0\0"

/*
 * These codes are valid when nansi.sys or equivalent has been installed.
 * Function keys on a PC are preceded with a NUL. These are converted into
 * K_NUL '\316' in GetChars(), because we cannot handle NULs in key codes.
 * CTRL-arrow is used instead of SHIFT-arrow.
 */
#  define PCANSI_TCAP "pcansi\0\
\0\033[K\0\
\0\033[L\0\
\1\033[M\0\
\2\033[2J\0\
\3\033[0m\0\
\0\033[7m\0\
\3\001\0\
\0\033[%i%d;%dH\0\
\1\033[%dC\0\
\5\316H\0\
\0\316P\0\
\0\316K\0\
\0\316M\0\
\2\316s\0\
\0\316t\0\
\0\316;\0\
\0\316<\0\
\0\316=\0\
\0\316>\0\
\0\316?\0\
\0\316@\0\
\0\316A\0\
\0\316B\0\
\0\316C\0\
\0\316D\0\
\0\316T\0\
\0\316U\0\
\0\316V\0\
\0\316W\0\
\0\316X\0\
\0\316Y\0\
\0\316Z\0\
\0\316[\0\
\0\316\\\0\
\0\316]\0\
\0\0"

/*
 * These codes are valid for the pc video.
 * The entries that start with ESC | are translated into conio calls in msdos.c.
 */
#  define PCTERM_TCAP "pcterm\0\
\0\033|K\0\
\0\033|L\0\
\1\033|M\0\
\1\033|%i%d;%dr\0\
\0\033|J\0\
\3\033|0m\0\
\0\033|112m\0\
\0\033|63m\0\
\0\033|0m\0\
\0\033|31m\0\
\0\001\0\
\0\033|%i%d;%dH\0\
\7\316H\0\
\0\316P\0\
\0\316K\0\
\0\316M\0\
\2\316s\0\
\0\316t\0\
\0\316;\0\
\0\316<\0\
\0\316=\0\
\0\316>\0\
\0\316?\0\
\0\316@\0\
\0\316A\0\
\0\316B\0\
\0\316C\0\
\0\316D\0\
\0\316T\0\
\0\316U\0\
\0\316V\0\
\0\316W\0\
\0\316X\0\
\0\316Y\0\
\0\316Z\0\
\0\316[\0\
\0\316\\\0\
\0\316]\0\
\0\0"

/*
 * These codes are valid for the NT Console 
 * The entries that start with ESC | are translated into console calls 
 * in winnt.c.
 */
#  define NTCONSOLE_TCAP "ntconsole\0\
\0\033|K\0\
\0\033|L\0\
\0\033|%dL\0\
\0\033|M\0\
\0\033|%dM\0\
\1\033|J\0\
\0\033|v\0\
\0\033|V\0\
\1\033|0m\0\
\0\033|112m\0\
\0\033|63m\0\
\0\033|0m\0\
\0\033|31m\0\
\0\001\0\
\0\033|%i%d;%dH\0\
\7\316H\0\
\0\316P\0\
\0\316K\0\
\0\316M\0\
\2\316s\0\
\0\316t\0\
\0\316;\0\
\0\316<\0\
\0\316=\0\
\0\316>\0\
\0\316?\0\
\0\316@\0\
\0\316A\0\
\0\316B\0\
\0\316C\0\
\0\316D\0\
\0\316T\0\
\0\316U\0\
\0\316V\0\
\0\316W\0\
\0\316X\0\
\0\316Y\0\
\0\316Z\0\
\0\316[\0\
\0\316\\\0\
\0\316]\0\
\0\0"


#  define VT52_TCAP "vt52\0\
\0\033K\0\
\0\033T\0\
\1\033U\0\
\2\014\0\
\3\033SO\0\
\0\033S2\0\
\3\001\0\
\0\033Y%+ %+ \0\
\0\0"

/*
 * The xterm termcap is missing F14 and F15, because they send the same
 * codes as the undo and help key, although they don't work on all keyboards.
 */
#  define XTERM_TCAP "xterm\0\
\0\033[K\0\
\0\033[L\0\
\0\033[%dL\0\
\0\033[M\0\
\0\033[%dM\0\
\0\033[%i%d;%dr\0\
\0\033[H\033[2J\0\
\3\033[m\0\
\0\033[7m\0\
\0\033[1m\0\
\2\001\0\
\0\033[%i%d;%dH\0\
\0\033M\0\
\0\033[%dC\0\
\1\033[?1h\033=\0\
\0\033[?1l\033>\0\
\0\0337\033[?47h\0\
\0\033[2J\033[?47l\0338\0\
\0\033OA\0\
\0\033OB\0\
\0\033OD\0\
\0\033OC\0\
\0\033Ox\0\
\0\033Or\0\
\0\033Ot\0\
\0\033Ov\0\
\0\033[11~\0\
\0\033[12~\0\
\0\033[13~\0\
\0\033[14~\0\
\0\033[15~\0\
\0\033[17~\0\
\0\033[18~\0\
\0\033[19~\0\
\0\033[20~\0\
\0\033[21~\0\
\0\033[23~\0\
\0\033[24~\0\
\0\033[25~\0\
\2\033[29~\0\
\0\033[31~\0\
\0\033[32~\0\
\0\033[33~\0\
\0\033[34~\0\
\0\033[28~\0\
\0\033[26~\0\
\0\0"

#  define DEBUG_TCAP "debug\0\
\0[EL]\0\
\0[IL]\0\
\0[CIL%d]\0\
\0[DL]\0\
\0[CDL%d]\0\
\0[%dCS%d]\0\
\0[ED]\0\
\0[CI]\0\
\0[CV]\0\
\0[CVV]\0\
\0[TP]\0\
\0[TI]\0\
\0[TB]\0\
\0[SE]\0\
\0[SO]\0\
\0[MS]\0\
\0[%dCM%d]\0\
\0[SR]\0\
\0[CRI%d]\0\
\0[VB]\0\
\0[KS]\0\
\0[KE]\0\
\0[TI]\0\
\0[TE]\0\
\0[KU]\0\
\0[KD]\0\
\0[KL]\0\
\0[KR]\0\
\0[SKU]\0\
\0[SKD]\0\
\0[SKL]\0\
\0[SKR]\0\
\0[F1]\0\
\0[F2]\0\
\0[F3]\0\
\0[F4]\0\
\0[F5]\0\
\0[F6]\0\
\0[F7]\0\
\0[F8]\0\
\0[F9]\0\
\0[F10]\0\
\0[SF1]\0\
\0[SF2]\0\
\0[SF3]\0\
\0[SF4]\0\
\0[SF5]\0\
\0[SF6]\0\
\0[SF7]\0\
\0[SF8]\0\
\0[SF9]\0\
\0[SF10]\0\
\0[HELP]\0\
\0[UNDO]\0\
\0\0"

#  ifdef ATARI
#   define DFLT_TCAP ATARI_TCAP
#  endif /* ATARI */

#  ifdef AMIGA
#   define DFLT_TCAP AMIGA_TCAP
#  endif /* AMIGA */

#  ifdef NT
#   define DFLT_TCAP NTCONSOLE_TCAP
#  else
#   ifdef MSDOS
#    define DFLT_TCAP PCTERM_TCAP                       
#   endif /* MSDOS */                                   
#  endif /* NT */ 

#  ifdef UNIX
#   define DFLT_TCAP ANSI_TCAP
#  endif /* UNIX */

# else /* NO_BUILTIN_TCAPS */
#  define DUMB_TCAP "dumb\0\
\6\014\0\
\9\033[%i%d;%dH\0\
\0\0"
# endif /* NO_BUILTIN_TCAPS */

#else /* TERMINFO */
# ifndef NO_BUILTIN_TCAPS
/*
 * here are the builtin termcap entries.
 * They not stored as complete Tcarr structures, as such a structure 
 * is to big. 
 * Each termcap is a concatenated string of entries, where '\0' characters
 * followed by a skip character sepereate the capabilities. The skip 
 * character is the relative structure offset for the following entry.
 * See parse_builtin_tcap() in term.c for all details.
 */
#  define AMIGA_TCAP "amiga\0\
\0\033[K\0\
\0\033[L\0\
\0\033[%p1%dL\0\
\0\033[M\0\
\0\033[%p1%dM\0\
\1\014\0\
\0\033[0 p\0\
\0\033[1 p\0\
\1\033[0m\0\
\0\033[7m\0\
\0\033[1m\0\
\0\033[0m\0\
\0\033[33m\0\
\0\001\0\
\0\033[%i%p1%d;%p2%dH\0\
\1\033[%p1%dC\0\
\5\233A\0\
\0\233B\0\
\0\233D\0\
\0\233C\0\
\0\233T\0\
\0\233S\0\
\0\233 A\0\
\0\233 @\0\
\0\233\060~\0\
\0\233\061~\0\
\0\233\062~\0\
\0\233\063~\0\
\0\233\064~\0\
\0\233\065~\0\
\0\233\066~\0\
\0\233\067~\0\
\0\233\070~\0\
\0\233\071~\0\
\0\233\061\060~\0\
\0\233\061\061~\0\
\0\233\061\062~\0\
\0\233\061\063~\0\
\0\233\061\064~\0\
\0\233\061\065~\0\
\0\233\061\066~\0\
\0\233\061\067~\0\
\0\233\061\070~\0\
\0\233\061\071~\0\
\0\233?~\0\
\0\0"

#  define ATARI_TCAP "atari\0\
\0\033l\0\
\0\033L\0\
\1\033M\0\
\2\033E\0\
\0\033f\0\
\0\033e\0\
\0\0"

#  define ANSI_TCAP "ansi\0\
\0\033[2K\0\
\0\033[L\0\
\0\033[%p1%dL\0\
\0\033[M\0\
\0\033[%p1%dM\0\
\1\033[2J\0\
\3\033[0m\0\
\0\033[7m\0\
\3\001\0\
\0\033[%i%p1%d;%p2%dH\0\
\1\033[%p1%dC\0\
\0\0"

/*
 * These codes are valid when nansi.sys or equivalent has been installed.
 * Function keys on a PC are preceded with a NUL. These are converted into
 * K_NUL '\316' in GetChars(), because we cannot handle NULs in key codes.
 * CTRL-arrow is used instead of SHIFT-arrow.
 */
#  define PCANSI_TCAP "pcansi\0\
\0\033[K\0\
\0\033[L\0\
\1\033[M\0\
\2\033[2J\0\
\3\033[0m\0\
\0\033[7m\0\
\3\001\0\
\0\033[%i%p1%d;%p2%dH\0\
\1\033[%p1%dC\0\
\5\316H\0\
\0\316P\0\
\0\316K\0\
\0\316M\0\
\2\316s\0\
\0\316t\0\
\0\316;\0\
\0\316<\0\
\0\316=\0\
\0\316>\0\
\0\316?\0\
\0\316@\0\
\0\316A\0\
\0\316B\0\
\0\316C\0\
\0\316D\0\
\0\316T\0\
\0\316U\0\
\0\316V\0\
\0\316W\0\
\0\316X\0\
\0\316Y\0\
\0\316Z\0\
\0\316[\0\
\0\316\\\0\
\0\316]\0\
\0\0"

/*
 * These codes are valid for the pc video.
 * The entries that start with ESC | are translated into conio calls in msdos.c.
 */
#  define PCTERM_TCAP "pcterm\0\
\0\033|K\0\
\0\033|L\0\
\1\033|M\0\
\1\033|%i%p1%d;%p2%dr\0\
\0\033|J\0\
\3\033|0m\0\
\0\033|112m\0\
\0\033|63m\0\
\0\033|0m\0\
\0\033|31m\0\
\0\001\0\
\0\033|%i%p1%d;%p2%dH\0\
\7\316H\0\
\0\316P\0\
\0\316K\0\
\0\316M\0\
\2\316s\0\
\0\316t\0\
\0\316;\0\
\0\316<\0\
\0\316=\0\
\0\316>\0\
\0\316?\0\
\0\316@\0\
\0\316A\0\
\0\316B\0\
\0\316C\0\
\0\316D\0\
\0\316T\0\
\0\316U\0\
\0\316V\0\
\0\316W\0\
\0\316X\0\
\0\316Y\0\
\0\316Z\0\
\0\316[\0\
\0\316\\\0\
\0\316]\0\
\0\0"

/*
 * These codes are valid for the NT Console 
 * The entries that start with ESC | are translated into console calls 
 * in winnt.c.
 */
#  define NTCONSOLE_TCAP "ntconsole\0\
\0\033|K\0\
\0\033|L\0\
\0\033|%dL\0\
\0\033|M\0\
\0\033|%dM\0\
\1\033|J\0\
\0\033|v\0\
\0\033|V\0\
\1\033|0m\0\
\0\033|112m\0\
\0\033|63m\0\
\0\033|0m\0\
\0\033|31m\0\
\0\001\0\
\0\033|%i%p1%d;%p2%dH\0\
\7\316H\0\
\0\316P\0\
\0\316K\0\
\0\316M\0\
\2\316s\0\
\0\316t\0\
\0\316;\0\
\0\316<\0\
\0\316=\0\
\0\316>\0\
\0\316?\0\
\0\316@\0\
\0\316A\0\
\0\316B\0\
\0\316C\0\
\0\316D\0\
\0\316T\0\
\0\316U\0\
\0\316V\0\
\0\316W\0\
\0\316X\0\
\0\316Y\0\
\0\316Z\0\
\0\316[\0\
\0\316\\\0\
\0\316]\0\
\0\0"


#  define VT52_TCAP "vt52\0\
\0\033K\0\
\0\033T\0\
\1\033U\0\
\2\014\0\
\3\033SO\0\
\0\033S2\0\
\3\001\0\
\0\033Y%+ %+ \0\
\0\0"

/*
 * The xterm termcap is missing F14 and F15, because they send the same
 * codes as the undo and help key, although they don't work on all keyboards.
 */
#  define XTERM_TCAP "xterm\0\
\0\033[K\0\
\0\033[L\0\
\0\033[%p1%dL\0\
\0\033[M\0\
\0\033[%p1%dM\0\
\0\033[%i%p1%d;%p2%dr\0\
\0\033[H\033[2J\0\
\3\033[m\0\
\0\033[7m\0\
\3\001\0\
\0\033[%i%p1%d;%p2%dH\0\
\0\033M\0\
\0\033[%p1%dC\0\
\1\033[?1h\033=\0\
\0\033[?1l\033>\0\
\0\0337\033[?47h\0\
\0\033[2J\033[?47l\0338\0\
\0\033OA\0\
\0\033OB\0\
\0\033OD\0\
\0\033OC\0\
\0\033Ox\0\
\0\033Or\0\
\0\033Ot\0\
\0\033Ov\0\
\0\033[11~\0\
\0\033[12~\0\
\0\033[13~\0\
\0\033[14~\0\
\0\033[15~\0\
\0\033[17~\0\
\0\033[18~\0\
\0\033[19~\0\
\0\033[20~\0\
\0\033[21~\0\
\0\033[23~\0\
\0\033[24~\0\
\0\033[25~\0\
\2\033[29~\0\
\0\033[31~\0\
\0\033[32~\0\
\0\033[33~\0\
\0\033[34~\0\
\0\033[28~\0\
\0\033[26~\0\
\0\0"

#  define DEBUG_TCAP "debug\0\
\0[EL]\0\
\0[IL]\0\
\0[CIL%p1%d]\0\
\0[DL]\0\
\0[CDL%p1%d]\0\
\0[%p1%dCS%p2%d]\0\
\0[ED]\0\
\0[CI]\0\
\0[CV]\0\
\0[CVV]\0\
\0[TP]\0\
\0[TI]\0\
\0[TB]\0\
\0[SE]\0\
\0[SO]\0\
\0[MS]\0\
\0[%p1%dCM%p2%d]\0\
\0[SR]\0\
\0[CRI%p1%d]\0\
\0[VB]\0\
\0[KS]\0\
\0[KE]\0\
\0[TI]\0\
\0[TE]\0\
\0[KU]\0\
\0[KD]\0\
\0[KL]\0\
\0[KR]\0\
\0[SKU]\0\
\0[SKD]\0\
\0[SKL]\0\
\0[SKR]\0\
\0[F1]\0\
\0[F2]\0\
\0[F3]\0\
\0[F4]\0\
\0[F5]\0\
\0[F6]\0\
\0[F7]\0\
\0[F8]\0\
\0[F9]\0\
\0[F10]\0\
\0[SF1]\0\
\0[SF2]\0\
\0[SF3]\0\
\0[SF4]\0\
\0[SF5]\0\
\0[SF6]\0\
\0[SF7]\0\
\0[SF8]\0\
\0[SF9]\0\
\0[SF10]\0\
\0[HELP]\0\
\0[UNDO]\0\
\0\0"

#  ifdef ATARI
#   define DFLT_TCAP ATARI_TCAP
#  endif /* ATARI */

#  ifdef AMIGA
#   define DFLT_TCAP AMIGA_TCAP
#  endif /* AMIGA */

#  ifdef NT
#   define DFLT_TCAP NTCONSOLE_TCAP
#  else
#   ifdef MSDOS
#    define DFLT_TCAP PCTERM_TCAP                       
#   endif /* MSDOS */                                   
#  endif /* NT */ 

#  ifdef UNIX
#   define DFLT_TCAP ANSI_TCAP
#  endif /* UNIX */

# else /* NO_BUILTIN_TCAPS */
/*
 * The most minimal terminal: only clear screen and cursor positioning
 */
#  define DUMB_TCAP "dumb\0\
\6\014\0\
\9\033[%i%p1%d;%p2%dH\0\
\0\0"
# endif /* NO_BUILTIN_TCAPS */

#endif
