/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Code to handle user-settable options. This is all pretty much table-
 * driven. Checklist for adding a new option:
 * - Put it in the options array below (copy an existing entry).
 * - For a global option: Add a variable for it in option.h.
 * - For a buffer or window local option:
 *   - Add a PV_XX entry to the enum below.
 *   - Add a variable to the window or buffer struct in structs.h.
 *   - For a window option, add some code to win_copy_options().
 *   - For a buffer option, add some code to buf_copy_options().
 *   - For a buffer string option, add code to check_buf_options().
 * - If it's a numeric option, add any necessary bounds checks to do_set().
 * - If it's a list of flags, add some code in do_set(), search for WW_ALL.
 * - When adding an option with expansion (P_EXPAND), but with a different
 *   default for Vi and Vim (no P_VI_DEF), add some code at VIMEXP.
 * - Add documentation!  One line in doc/help.txt, full description in
 *   options.txt, and any other related places.
 */

#include "vim.h"

struct vimoption
{
    char	*fullname;	/* full option name */
    char	*shortname;	/* permissible abbreviation */
    short	flags;		/* see below */
    char_u	*var;		/* pointer to variable */
    char_u	*def_val[2];	/* default values for variable (vi and vim) */
};

#define VI_DEFAULT  0	    /* def_val[VI_DEFAULT] is Vi default value */
#define VIM_DEFAULT 1	    /* def_val[VIM_DEFAULT] is Vim default value */

/*
 * Flags
 *
 * Note: P_EXPAND and P_IND can never be used at the same time.
 * Note: P_IND cannot be used for a terminal option.
 */
#define P_BOOL		0x01	/* the option is boolean */
#define P_NUM		0x02	/* the option is numeric */
#define P_STRING	0x04	/* the option is a string */
#define P_ALLOCED	0x08	/* the string option is in allocated memory,
				    must use vim_free() when assigning new
				    value. Not set if default is the same. */
#define P_EXPAND	0x10	/* environment expansion */
#define P_IND		0x20	/* indirect, is in curwin or curbuf */
#define P_NODEFAULT	0x40	/* has no default value */
#define P_DEF_ALLOCED	0x80	/* default value is in allocated memory, must
				    use vim_free() when assigning new value */
#define P_WAS_SET	0x100	/* option has been set/reset */
#define P_NO_MKRC	0x200	/* don't include in :mkvimrc output */
#define P_VI_DEF	0x400	/* Use Vi default for Vim */
#define P_VIM		0x800	/* Vim option, reset when 'cp' set */

#define P_RSTAT		0x1000	/* when changed, redraw status lines */
#define P_RBUF		0x2000	/* when changed, redraw current buffer */
#define P_RALL		0x4000	/* when changed, redraw all */

/*
 * The options that are in curwin or curbuf have P_IND set and a var field
 * that contains one of the enum values below.
 */
enum indirect_options
{
    PV_AI = 1,
    PV_BIN,
    PV_CIN,
    PV_CINK,
    PV_CINO,
    PV_CINW,
    PV_COM,
    PV_CPT,
    PV_EOL,
    PV_ET,
    PV_FF,
    PV_FO,
    PV_INF,
    PV_ISK,
    PV_LBR,
    PV_LISP,
    PV_LIST,
    PV_ML,
    PV_MOD,
    PV_NF,
    PV_NU,
    PV_RL,
    PV_RO,
    PV_SCROLL,
    PV_SI,
    PV_SN,
    PV_STS,
    PV_SW,
    PV_TS,
    PV_TW,
    PV_TX,
    PV_WM,
    PV_WRAP
};

/*
 * options[] is initialized here.
 * The order of the options MUST be alphabetic for ":set all" and findoption().
 * All option names MUST start with a lowercase letter (for findoption()).
 * Exception: "t_" options are at the end.
 * The options with a NULL variable are 'hidden': a set command for them is
 * ignored and they are not printed.
 */
static struct vimoption options[] =
{
#ifdef RIGHTLEFT
    {"aleph",	    "al",   P_NUM|P_VI_DEF,
			    (char_u *)&p_aleph,
			    {
# if (defined(MSDOS) || defined(WIN32) || defined(OS2)) && !defined(USE_GUI_WIN32)
			    (char_u *)128L,
# else
			    (char_u *)224L,
# endif
					    (char_u *)0L}},
#endif
#ifdef FKMAP
    {"altkeymap",   "akm",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_altkeymap,
			    {(char_u *)FALSE, (char_u *)0L}},
#endif
    {"autoindent",  "ai",   P_BOOL|P_IND|P_VI_DEF,
			    (char_u *)PV_AI,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"autoprint",   "ap",   P_BOOL|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"autowrite",   "aw",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_aw,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"background",  "bg",   P_STRING|P_VI_DEF|P_RALL,
			    (char_u *)&p_bg,
			    {
#if (defined(MSDOS) || defined(OS2) || defined(WIN32)) && !defined(USE_GUI)
			    (char_u *)"dark",
#else
			    (char_u *)"light",
#endif
					    (char_u *)0L}},
    {"backspace",   "bs",   P_NUM|P_VI_DEF|P_VIM,
			    (char_u *)&p_bs,
			    {(char_u *)0L, (char_u *)0L}},
    {"backup",	    "bk",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_bk,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"backupdir",   "bdir", P_STRING|P_EXPAND|P_VI_DEF,
			    (char_u *)&p_bdir,
			    {(char_u *)DEF_BDIR, (char_u *)0L}},
    {"backupext",   "bex",  P_STRING|P_VI_DEF,
			    (char_u *)&p_bex,
			    {
#ifdef VMS
			    (char_u *)"_",
#else
			    (char_u *)"~",
#endif
					    (char_u *)0L}},
    {"beautify",    "bf",   P_BOOL|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"binary",	    "bin",  P_BOOL|P_IND|P_VI_DEF|P_RSTAT,
			    (char_u *)PV_BIN,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"bioskey",	    "biosk",P_BOOL|P_VI_DEF,
#ifdef MSDOS
			    (char_u *)&p_biosk,
#else
			    (char_u *)NULL,
#endif
			    {(char_u *)TRUE, (char_u *)0L}},
    {"breakat",	    "brk",  P_STRING|P_VI_DEF|P_RALL,
			    (char_u *)&p_breakat,
			    {(char_u *)" \t!@*-+_;:,./?", (char_u *)0L}},
#ifdef CINDENT
    {"cindent",	    "cin",  P_BOOL|P_IND|P_VI_DEF|P_VIM,
			    (char_u *)PV_CIN,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"cinkeys",	    "cink", P_STRING|P_IND|P_ALLOCED|P_VI_DEF,
			    (char_u *)PV_CINK,
			    {(char_u *)"0{,0},:,0#,!^F,o,O,e", (char_u *)0L}},
    {"cinoptions",  "cino", P_STRING|P_IND|P_ALLOCED|P_VI_DEF,
			    (char_u *)PV_CINO,
			    {(char_u *)"", (char_u *)0L}},
#endif /* CINDENT */
#if defined(SMARTINDENT) || defined(CINDENT)
    {"cinwords",    "cinw", P_STRING|P_IND|P_ALLOCED|P_VI_DEF,
			    (char_u *)PV_CINW,
			    {(char_u *)"if,else,while,do,for,switch",
				(char_u *)0L}},
#endif
    {"cmdheight",   "ch",   P_NUM|P_VI_DEF|P_RALL,
			    (char_u *)&p_ch,
			    {(char_u *)1L, (char_u *)0L}},
    {"columns",	    "co",   P_NUM|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RALL,
			    (char_u *)&Columns,
			    {(char_u *)80L, (char_u *)0L}},
    {"comments",    "com",  P_STRING|P_IND|P_ALLOCED|P_VI_DEF,
			    (char_u *)PV_COM,
			    {(char_u *)"sr:/*,mb:*,el:*/,://,b:#,:%,:XCOMM,n:>,fb:-",
				(char_u *)0L}},
    {"compatible",  "cp",   P_BOOL|P_RALL,
			    (char_u *)&p_cp,
			    {(char_u *)TRUE, (char_u *)FALSE}},
#ifdef INSERT_EXPAND
    {"complete",    "cpt",  P_STRING|P_IND|P_ALLOCED|P_VI_DEF,
			    (char_u *)PV_CPT,
			    {(char_u *)".,b", (char_u *)0L}},
#endif
    {"cpoptions",   "cpo",  P_STRING|P_VIM|P_RALL,
			    (char_u *)&p_cpo,
			    {(char_u *)CPO_ALL, (char_u *)CPO_DEFAULT}},
    {"define",	    "def",  P_STRING|P_VI_DEF,
			    (char_u *)&p_def,
			    {(char_u *)"^#\\s*define", (char_u *)0L}},
    {"dictionary",  "dict", P_STRING|P_EXPAND|P_VI_DEF,
			    (char_u *)&p_dict,
			    {(char_u *)"", (char_u *)0L}},
    {"digraph",	    "dg",   P_BOOL|P_VI_DEF|P_VIM,
#ifdef DIGRAPHS
			    (char_u *)&p_dg,
#else
			    (char_u *)NULL,
#endif
			    {(char_u *)FALSE, (char_u *)0L}},
    {"directory",   "dir",  P_STRING|P_EXPAND|P_VI_DEF,
			    (char_u *)&p_dir,
			    {(char_u *)DEF_DIR, (char_u *)0L}},
    {"edcompatible","ed",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ed,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"endofline",   "eol",  P_BOOL|P_IND|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)PV_EOL,
			    {(char_u *)TRUE, (char_u *)0L}},
    {"equalalways", "ea",   P_BOOL|P_VI_DEF|P_RALL,
			    (char_u *)&p_ea,
			    {(char_u *)TRUE, (char_u *)0L}},
    {"equalprg",    "ep",   P_STRING|P_EXPAND|P_VI_DEF,
			    (char_u *)&p_ep,
			    {(char_u *)"", (char_u *)0L}},
    {"errorbells",  "eb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_eb,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"errorfile",   "ef",   P_STRING|P_EXPAND|P_VI_DEF,
			    (char_u *)&p_ef,
			    {(char_u *)ERRORFILE, (char_u *)0L}},
    {"errorformat", "efm",  P_STRING|P_VI_DEF,
			    (char_u *)&p_efm,
			    {(char_u *)EFM_DFLT, (char_u *)0L}},
    {"esckeys",	    "ek",   P_BOOL|P_VIM,
			    (char_u *)&p_ek,
			    {(char_u *)FALSE, (char_u *)TRUE}},
    {"eventignore", "ei",   P_STRING|P_VI_DEF,
#ifdef AUTOCMD
			    (char_u *)&p_ei,
#else
			    (char_u *)NULL,
#endif
			    {(char_u *)"", (char_u *)0L}},
    {"expandtab",   "et",   P_BOOL|P_IND|P_VI_DEF|P_VIM,
			    (char_u *)PV_ET,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"exrc",	    "ex",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_exrc,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"fileformat",  "ff",   P_STRING|P_IND|P_ALLOCED|P_VI_DEF|P_RSTAT,
			    (char_u *)PV_FF,
			    {(char_u *)FF_DFLT, (char_u *)0L}},
    {"fileformats", "ffs",  P_STRING|P_VIM,
			    (char_u *)&p_ffs,
			    {(char_u *)FFS_VI, (char_u *)FFS_DFLT}},
#ifdef FKMAP
    {"fkmap",	    "fk",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_fkmap,
			    {(char_u *)FALSE, (char_u *)0L}},
#endif
    {"flash",	    "fl",   P_BOOL|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"formatoptions","fo",  P_STRING|P_IND|P_ALLOCED|P_VIM,
			    (char_u *)PV_FO,
			    {(char_u *)FO_DFLT_VI, (char_u *)FO_DFLT}},
    {"formatprg",   "fp",   P_STRING|P_EXPAND|P_VI_DEF,
			    (char_u *)&p_fp,
			    {(char_u *)"", (char_u *)0L}},
    {"gdefault",    "gd",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_gd,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"graphic",	    "gr",   P_BOOL|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"guicursor",    "gcr",  P_STRING|P_VI_DEF,
#ifdef USE_GUI
			    (char_u *)&p_guicursor,
			    {(char_u *)"n-v-c:block-Cursor,o:hor50-Cursor,i-ci:ver25-Cursor,r-cr:hor20-Cursor,sm:block-Cursor-blinkwait175-blinkoff150-blinkon175",
				    (char_u *)0L}
#else
			    (char_u *)NULL,
			    {(char_u *)NULL, (char_u *)0L}
#endif
				    },
    {"guifont",	    "gfn",  P_STRING|P_VI_DEF|P_RALL,
#ifdef USE_GUI
			    (char_u *)&p_guifont,
			    {(char_u *)"", (char_u *)0L}
#else
			    (char_u *)NULL,
			    {(char_u *)NULL, (char_u *)0L}
#endif
				    },
    {"guioptions",  "go",   P_STRING|P_VI_DEF|P_RALL,
#if defined(USE_GUI) || defined(USE_CLIPBOARD)
			    (char_u *)&p_guioptions,
# ifdef UNIX
			    {(char_u *)"agmr", (char_u *)0L}
# else
			    {(char_u *)"gmr", (char_u *)0L}
# endif
#else
			    (char_u *)NULL,
			    {(char_u *)NULL, (char_u *)0L}
#endif
				    },
#if defined(USE_GUI)
    {"guipty",	    NULL,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_guipty,
			    {(char_u *)TRUE, (char_u *)0L}},
#endif
    {"hardtabs",    "ht",   P_NUM|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)0L, (char_u *)0L}},
    {"helpfile",    "hf",   P_STRING|P_EXPAND|P_VI_DEF,
			    (char_u *)&p_hf,
			    {(char_u *)VIM_HLP, (char_u *)0L}},
    {"helpheight",  "hh",   P_NUM|P_VI_DEF,
			    (char_u *)&p_hh,
			    {(char_u *)20L, (char_u *)0L}},
    {"hidden",	    "hid",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_hid,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"highlight",   "hl",   P_STRING|P_VI_DEF|P_RALL,
			    (char_u *)&p_hl,
			    {(char_u *)"8:SpecialKey,@:NonText,d:Directory,e:ErrorMsg,i:IncSearch,l:Search,m:MoreMsg,M:ModeMsg,n:LineNr,r:Question,s:StatusLine,t:Title,v:Visual,w:WarningMsg",
				(char_u *)0L}},
    {"hlsearch",    "hls",  P_BOOL|P_VI_DEF|P_VIM|P_RALL,
			    (char_u *)&p_hls,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"history",	    "hi",   P_NUM|P_VIM,
			    (char_u *)&p_hi,
			    {(char_u *)0L, (char_u *)20L}},
#ifdef RIGHTLEFT
    {"hkmap",	    "hk",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_hkmap,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"hkmapp",	    "hkp",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_hkmapp,
			    {(char_u *)FALSE, (char_u *)0L}},
#endif
    {"icon",	    NULL,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_icon,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"iconstring",  NULL,   P_STRING|P_VI_DEF,
			    (char_u *)&p_iconstring,
			    {(char_u *)"", (char_u *)0L}},
    {"ignorecase",  "ic",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ic,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"include",	    "inc",  P_STRING|P_VI_DEF,
			    (char_u *)&p_inc,
			    {(char_u *)"^#\\s*include", (char_u *)0L}},
    {"incsearch",   "is",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_is,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"infercase",   "inf",  P_BOOL|P_IND|P_VI_DEF,
			    (char_u *)PV_INF,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"insertmode",  "im",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_im,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"isfname",	    "isf",  P_STRING|P_VI_DEF,
			    (char_u *)&p_isf,
			    {
#ifdef BACKSLASH_IN_FILENAME
			    (char_u *)"@,48-57,/,.,-,_,+,,,$,:,\\",
#else
# ifdef AMIGA
			    (char_u *)"@,48-57,/,.,-,_,+,,,$,:",
# else /* UNIX */
			    (char_u *)"@,48-57,/,.,-,_,+,,,$,~",
# endif
#endif
				(char_u *)0L}},
    {"isident",	    "isi",  P_STRING|P_VI_DEF,
			    (char_u *)&p_isi,
			    {
#if defined(MSDOS) || defined(WIN32) || defined(OS2)
			    (char_u *)"@,48-57,_,128-167,224-235",
#else
			    (char_u *)"@,48-57,_,192-255",
#endif
				(char_u *)0L}},
    {"iskeyword",   "isk",  P_STRING|P_IND|P_ALLOCED|P_VIM,
			    (char_u *)PV_ISK,
			    {(char_u *)"@,48-57,_",
# if defined(MSDOS) || defined(WIN32) || defined(OS2)
				(char_u *)"@,48-57,_,128-167,224-235"
# else
				(char_u *)"@,48-57,_,192-255"
# endif
				}},
    {"isprint",	    "isp",  P_STRING|P_VI_DEF|P_RALL,
			    (char_u *)&p_isp,
			    {
#if defined(MSDOS) || defined(WIN32) || defined(OS2)
			    (char_u *)"@,~-255",
#else
			    (char_u *)"@,161-255",
#endif
				(char_u *)0L}},
    {"joinspaces",  "js",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_js,
			    {(char_u *)TRUE, (char_u *)0L}},
    {"keywordprg",  "kp",   P_STRING|P_EXPAND|P_VI_DEF,
			    (char_u *)&p_kp,
			    {
#if defined(MSDOS) || defined(WIN32)
			    (char_u *)"",
#else
# if defined(OS2)
			    (char_u *)"view /",
# else
			    (char_u *)"man",
# endif
#endif
				(char_u *)0L}},
    {"langmap",     "lmap", P_STRING|P_VI_DEF,
#ifdef HAVE_LANGMAP
			    (char_u *)&p_langmap,
			    {(char_u *)"",	/* unmatched } */
#else
			    (char_u *)NULL,
			    {(char_u *)NULL,
#endif
				(char_u *)0L}},
    {"laststatus",  "ls",   P_NUM|P_VI_DEF|P_RALL,
			    (char_u *)&p_ls,
			    {(char_u *)1L, (char_u *)0L}},
    {"lazyredraw",  "lz",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_lz,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"linebreak",   "lbr",  P_BOOL|P_IND|P_VI_DEF|P_RBUF,
			    (char_u *)PV_LBR,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"lines",	    NULL,   P_NUM|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RALL,
			    (char_u *)&Rows,
			    {
#if defined(MSDOS) || defined(WIN32) || defined(OS2)
			    (char_u *)25L,
#else
			    (char_u *)24L,
#endif
					    (char_u *)0L}},
    {"lisp",	    NULL,   P_BOOL|P_IND|P_VI_DEF,
			    (char_u *)PV_LISP,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"list",	    NULL,   P_BOOL|P_IND|P_VI_DEF|P_RBUF,
			    (char_u *)PV_LIST,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"magic",	    NULL,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_magic,
			    {(char_u *)TRUE, (char_u *)0L}},
    {"makeef",	    "mef",   P_STRING|P_EXPAND|P_VI_DEF,
			    (char_u *)&p_mef,
			    {(char_u *)MAKEEF, (char_u *)0L}},
    {"makeprg",	    "mp",   P_STRING|P_EXPAND|P_VI_DEF,
			    (char_u *)&p_mp,
			    {(char_u *)"make", (char_u *)0L}},
    {"matchtime",   "mat",  P_NUM|P_VI_DEF,
			    (char_u *)&p_mat,
			    {(char_u *)5L, (char_u *)0L}},
    {"maxmapdepth", "mmd",  P_NUM|P_VI_DEF,
			    (char_u *)&p_mmd,
			    {(char_u *)1000L, (char_u *)0L}},
    {"maxmem",	    "mm",   P_NUM|P_VI_DEF,
			    (char_u *)&p_mm,
			    {(char_u *)MAXMEM, (char_u *)0L}},
    {"maxmemtot",   "mmt",  P_NUM|P_VI_DEF,
			    (char_u *)&p_mmt,
			    {(char_u *)MAXMEMTOT, (char_u *)0L}},
    {"mesg",	    NULL,   P_BOOL|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"modeline",    "ml",   P_BOOL|P_IND|P_VIM,
			    (char_u *)PV_ML,
			    {(char_u *)FALSE, (char_u *)TRUE}},
    {"modelines",   "mls",  P_NUM|P_VI_DEF,
			    (char_u *)&p_mls,
			    {(char_u *)5L, (char_u *)0L}},
    {"modified",    "mod",  P_BOOL|P_IND|P_NO_MKRC|P_VI_DEF|P_RSTAT,
			    (char_u *)PV_MOD,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"more",	    NULL,   P_BOOL|P_VIM,
			    (char_u *)&p_more,
			    {(char_u *)FALSE, (char_u *)TRUE}},
    {"mouse",	    NULL,   P_STRING|P_VI_DEF,
			    (char_u *)&p_mouse,
			    {
#if defined(MSDOS) || defined(WIN32)
			    (char_u *)"a",
#else
			    (char_u *)"",
#endif
				(char_u *)0L}},
    {"mousehide",   "mh",   P_BOOL|P_VI_DEF,
#ifdef USE_GUI
			    (char_u *)&p_mh,
#else
			    (char_u *)NULL,
#endif
			    {(char_u *)FALSE, (char_u *)0L}},
    {"mousetime",   "mouset",	P_NUM|P_VI_DEF,
			    (char_u *)&p_mouset,
			    {(char_u *)500L, (char_u *)0L}},
    {"novice",	    NULL,   P_BOOL|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"number",	    "nu",   P_BOOL|P_IND|P_VI_DEF|P_RBUF,
			    (char_u *)PV_NU,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"nrformats",   "nf",   P_STRING|P_IND|P_ALLOCED|P_VI_DEF,
			    (char_u *)PV_NF,
			    {(char_u *)"octal,hex", (char_u *)0L}},
    {"open",	    NULL,   P_BOOL|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"optimize",    "opt",  P_BOOL|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"paragraphs",  "para", P_STRING|P_VI_DEF,
			    (char_u *)&p_para,
			    {(char_u *)"IPLPPPQPP LIpplpipbp", (char_u *)0L}},
    {"paste",	    NULL,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_paste,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"patchmode",   "pm",   P_STRING|P_VI_DEF,
			    (char_u *)&p_pm,
			    {(char_u *)"", (char_u *)0L}},
    {"path",	    "pa",   P_STRING|P_EXPAND|P_VI_DEF,
			    (char_u *)&p_path,
			    {
#if defined AMIGA || defined MSDOS || defined WIN32
			    (char_u *)".,,",
#else
# if defined(__EMX__)
			    (char_u *)".,/emx/include,,",
# else /* Unix, probably */
			    (char_u *)".,/usr/include,,",
# endif
#endif
				(char_u *)0L}},
    {"prompt",	    NULL,   P_BOOL|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"readonly",    "ro",   P_BOOL|P_IND|P_VI_DEF|P_RSTAT,
			    (char_u *)PV_RO,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"redraw",	    NULL,   P_BOOL|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"remap",	    NULL,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_remap,
			    {(char_u *)TRUE, (char_u *)0L}},
    {"report",	    NULL,   P_NUM|P_VI_DEF,
			    (char_u *)&p_report,
			    {(char_u *)2L, (char_u *)0L}},
#ifdef WIN32
    {"restorescreen", "rs", P_BOOL|P_VI_DEF,
			    (char_u *)&p_rs,
			    {(char_u *)TRUE, (char_u *)0L}},
#endif
#ifdef RIGHTLEFT
    {"revins",	    "ri",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_ri,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"rightleft",   "rl",   P_BOOL|P_IND|P_VI_DEF|P_RBUF,
			    (char_u *)PV_RL,
			    {(char_u *)FALSE, (char_u *)0L}},
#endif
    {"ruler",	    "ru",   P_BOOL|P_VI_DEF|P_VIM|P_RSTAT,
			    (char_u *)&p_ru,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"scroll",	    "scr",  P_NUM|P_IND|P_NO_MKRC|P_VI_DEF,
			    (char_u *)PV_SCROLL,
			    {(char_u *)12L, (char_u *)0L}},
    {"scrolljump",  "sj",   P_NUM|P_VI_DEF|P_VIM,
			    (char_u *)&p_sj,
			    {(char_u *)1L, (char_u *)0L}},
    {"scrolloff",   "so",   P_NUM|P_VI_DEF|P_VIM|P_RALL,
			    (char_u *)&p_so,
			    {(char_u *)0L, (char_u *)0L}},
    {"sections",    "sect", P_STRING|P_VI_DEF,
			    (char_u *)&p_sections,
			    {(char_u *)"SHNHH HUnhsh", (char_u *)0L}},
    {"secure",	    NULL,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_secure,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"shell",	    "sh",   P_STRING|P_EXPAND|P_VI_DEF,
			    (char_u *)&p_sh,
			    {
#if defined(MSDOS)
			    (char_u *)"command",
#else
# if defined(WIN32)
			    (char_u *)"",	/* set in set_init_1() */
# else
#  if defined(OS2)
			    (char_u *)"cmd.exe",
#  else
#   if defined(ARCHIE)
			    (char_u *)"gos",
#   else
			    (char_u *)"sh",
#   endif
#  endif
# endif
#endif
				(char_u *)0L}},
    {"shellcmdflag","shcf", P_STRING|P_VI_DEF,
			    (char_u *)&p_shcf,
			    {
#if defined(MSDOS) || defined(WIN32)
			    (char_u *)"/c",
#else
# if defined(OS2)
			    (char_u *)"/c",
# else
			    (char_u *)"-c",
# endif
#endif
				(char_u *)0L}},
    {"shellpipe",   "sp",   P_STRING|P_VI_DEF,
			    (char_u *)&p_sp,
			    {
#if defined(UNIX) || defined(OS2)
# ifdef ARCHIE
			    (char_u *)"2>",
# else
			    (char_u *)"| tee",
# endif
#else
			    (char_u *)">",
#endif
				(char_u *)0L}},
    {"shellquote",  "shq",  P_STRING|P_VI_DEF,
			    (char_u *)&p_shq,
			    {(char_u *)"", (char_u *)0L}},
    {"shellredir",  "srr",  P_STRING|P_VI_DEF,
			    (char_u *)&p_srr,
			    {(char_u *)">", (char_u *)0L}},
    {"shelltype",   "st",   P_NUM|P_VI_DEF,
			    (char_u *)&p_st,
			    {(char_u *)0L, (char_u *)0L}},
    {"shellxquote", "sxq",  P_STRING|P_VI_DEF,
			    (char_u *)&p_sxq,
			    {
#if defined(UNIX) && defined(USE_SYSTEM) && !defined(__EMX__)
			    (char_u *)"\"",
#else
			    (char_u *)"",
#endif
				(char_u *)0L}},
    {"shiftround",  "sr",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sr,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"shiftwidth",  "sw",   P_NUM|P_IND|P_VI_DEF,
			    (char_u *)PV_SW,
			    {(char_u *)8L, (char_u *)0L}},
    {"shortmess",   "shm",  P_STRING|P_VI_DEF|P_VIM,
			    (char_u *)&p_shm,
			    {(char_u *)"", (char_u *)0L}},
    {"shortname",   "sn",   P_BOOL|P_IND|P_VI_DEF,
#ifdef SHORT_FNAME
			    (char_u *)NULL,
#else
			    (char_u *)PV_SN,
#endif
			    {(char_u *)FALSE, (char_u *)0L}},
    {"showbreak",   "sbr",  P_STRING|P_VI_DEF|P_RALL,
			    (char_u *)&p_sbr,
			    {(char_u *)"", (char_u *)0L}},
    {"showcmd",	    "sc",   P_BOOL|P_VIM,
			    (char_u *)&p_sc,
			    {(char_u *)FALSE,
#ifdef UNIX
				(char_u *)FALSE
#else
				(char_u *)TRUE
#endif
				}},
    {"showmatch",   "sm",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_sm,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"showmode",    "smd",  P_BOOL|P_VIM,
			    (char_u *)&p_smd,
			    {(char_u *)FALSE, (char_u *)TRUE}},
    {"sidescroll",  "ss",   P_NUM|P_VI_DEF,
			    (char_u *)&p_ss,
			    {(char_u *)0L, (char_u *)0L}},
    {"slowopen",    "slow", P_BOOL|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"smartcase",   "scs",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_scs,
			    {(char_u *)FALSE, (char_u *)0L}},
#ifdef SMARTINDENT
    {"smartindent", "si",   P_BOOL|P_IND|P_VI_DEF|P_VIM,
			    (char_u *)PV_SI,
			    {(char_u *)FALSE, (char_u *)0L}},
#endif
    {"smarttab",    "sta",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sta,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"softtabstop", "sts",  P_NUM|P_IND|P_VI_DEF|P_VIM,
			    (char_u *)PV_STS,
			    {(char_u *)0L, (char_u *)0L}},
    {"sourceany",   NULL,   P_BOOL|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"splitbelow",  "sb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_sb,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"startofline", "sol",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_sol,
			    {(char_u *)TRUE, (char_u *)0L}},
    {"suffixes",    "su",   P_STRING|P_VI_DEF,
			    (char_u *)&p_su,
			    {(char_u *)".bak,~,.o,.h,.info,.swp",
				(char_u *)0L}},
    {"swapsync",    "sws",  P_STRING|P_VI_DEF,
			    (char_u *)&p_sws,
			    {(char_u *)"fsync", (char_u *)0L}},
    {"tabstop",	    "ts",   P_NUM|P_IND|P_VI_DEF|P_RBUF,
			    (char_u *)PV_TS,
			    {(char_u *)8L, (char_u *)0L}},
    {"tagbsearch",  "tbs",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_tbs,
			    {(char_u *)TRUE, (char_u *)0L}},
    {"taglength",   "tl",   P_NUM|P_VI_DEF,
			    (char_u *)&p_tl,
			    {(char_u *)0L, (char_u *)0L}},
    {"tagrelative", "tr",   P_BOOL|P_VIM,
			    (char_u *)&p_tr,
			    {(char_u *)FALSE, (char_u *)TRUE}},
    {"tags",	    "tag",  P_STRING|P_EXPAND|P_VI_DEF,
			    (char_u *)&p_tags,
			    {
#ifdef EMACS_TAGS
			    (char_u *)"./tags,./TAGS,tags,TAGS",
#else
			    (char_u *)"./tags,tags",
#endif
				(char_u *)0L}},
    {"tagstack",    "tgst", P_BOOL|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"term",	    NULL,   P_STRING|P_EXPAND|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RALL,
			    (char_u *)&T_NAME,
			    {(char_u *)"", (char_u *)0L}},
    {"terse",	    NULL,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_terse,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"textauto",    "ta",   P_BOOL|P_VIM,
			    (char_u *)&p_ta,
			    {(char_u *)TA_DFLT, (char_u *)TRUE}},
    {"textmode",    "tx",   P_BOOL|P_IND|P_VI_DEF,
			    (char_u *)PV_TX,
			    {
#ifdef USE_CRNL
			    (char_u *)TRUE,
#else
			    (char_u *)FALSE,
#endif
				(char_u *)0L}},
    {"textwidth",   "tw",   P_NUM|P_IND|P_VI_DEF|P_VIM,
			    (char_u *)PV_TW,
			    {(char_u *)0L, (char_u *)0L}},
    {"tildeop",	    "top",  P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_to,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"timeout",	    "to",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_timeout,
			    {(char_u *)TRUE, (char_u *)0L}},
    {"timeoutlen",  "tm",   P_NUM|P_VI_DEF,
			    (char_u *)&p_tm,
			    {(char_u *)1000L, (char_u *)0L}},
    {"title",	    NULL,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_title,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"titlelen",    NULL,   P_NUM|P_VI_DEF,
			    (char_u *)&p_titlelen,
			    {(char_u *)85L, (char_u *)0L}},
    {"titlestring", NULL,   P_STRING|P_VI_DEF,
			    (char_u *)&p_titlestring,
			    {(char_u *)"", (char_u *)0L}},
    {"ttimeout",    NULL,   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_ttimeout,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"ttimeoutlen", "ttm",  P_NUM|P_VI_DEF,
			    (char_u *)&p_ttm,
			    {(char_u *)-1L, (char_u *)0L}},
    {"ttybuiltin",  "tbi",  P_BOOL|P_VI_DEF,
			    (char_u *)&p_tbi,
			    {(char_u *)TRUE, (char_u *)0L}},
    {"ttyfast",	    "tf",   P_BOOL|P_NO_MKRC|P_VI_DEF,
			    (char_u *)&p_tf,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"ttyscroll",   "tsl",  P_NUM|P_VI_DEF,
			    (char_u *)&p_ttyscroll,
			    {(char_u *)999L, (char_u *)0L}},
    {"ttytype",	    "tty",  P_STRING|P_EXPAND|P_NODEFAULT|P_NO_MKRC|P_VI_DEF|P_RALL,
			    (char_u *)&T_NAME,
			    {(char_u *)"", (char_u *)0L}},
    {"undolevels",  "ul",   P_NUM|P_VI_DEF,
			    (char_u *)&p_ul,
			    {
#if defined(UNIX) || defined(WIN32) || defined(OS2)
			    (char_u *)1000L,
#else
			    (char_u *)100L,
#endif
				(char_u *)0L}},
    {"updatecount", "uc",   P_NUM|P_VI_DEF,
			    (char_u *)&p_uc,
			    {(char_u *)200L, (char_u *)0L}},
    {"updatetime",  "ut",   P_NUM|P_VI_DEF,
			    (char_u *)&p_ut,
			    {(char_u *)4000L, (char_u *)0L}},
    {"verbose",	    "vbs",  P_NUM|P_VI_DEF,
			    (char_u *)&p_verbose,
			    {(char_u *)0L, (char_u *)0L}},
    {"viminfo",	    "vi",   P_STRING|P_VI_DEF,
#ifdef VIMINFO
			    (char_u *)&p_viminfo,
#else
			    (char_u *)NULL,
#endif /* VIMINFO */
			    {(char_u *)"", (char_u *)0L}},
    {"visualbell",  "vb",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_vb,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"w300",	    NULL,   P_NUM|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)0L, (char_u *)0L}},
    {"w1200",	    NULL,   P_NUM|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)0L, (char_u *)0L}},
    {"w9600",	    NULL,   P_NUM|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)0L, (char_u *)0L}},
    {"warn",	    NULL,   P_BOOL|P_VI_DEF,
			    (char_u *)&p_warn,
			    {(char_u *)TRUE, (char_u *)0L}},
    {"weirdinvert", "wiv",  P_BOOL|P_VI_DEF|P_RALL,
			    (char_u *)&p_wiv,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"whichwrap",   "ww",   P_STRING|P_VIM,
			    (char_u *)&p_ww,
			    {(char_u *)"", (char_u *)"b,s"}},
    {"wildchar",    "wc",   P_NUM|P_VIM,
			    (char_u *)&p_wc,
			    {(char_u *)(long)Ctrl('E'), (char_u *)(long)TAB}},
    {"window",	    "wi",   P_NUM|P_VI_DEF,
			    (char_u *)NULL,
			    {(char_u *)0L, (char_u *)0L}},
    {"winheight",   "wh",   P_NUM|P_VI_DEF,
			    (char_u *)&p_wh,
			    {(char_u *)0L, (char_u *)0L}},
    {"wrap",	    NULL,   P_BOOL|P_IND|P_VI_DEF|P_RBUF,
			    (char_u *)PV_WRAP,
			    {(char_u *)TRUE, (char_u *)0L}},
    {"wrapmargin",  "wm",   P_NUM|P_IND|P_VI_DEF,
			    (char_u *)PV_WM,
			    {(char_u *)0L, (char_u *)0L}},
    {"wrapscan",    "ws",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_ws,
			    {(char_u *)TRUE, (char_u *)0L}},
    {"writeany",    "wa",   P_BOOL|P_VI_DEF,
			    (char_u *)&p_wa,
			    {(char_u *)FALSE, (char_u *)0L}},
    {"writebackup", "wb",   P_BOOL|P_VI_DEF|P_VIM,
			    (char_u *)&p_wb,
			    {
#ifdef WRITEBACKUP
			    (char_u *)TRUE,
#else
			    (char_u *)FALSE,
#endif
				(char_u *)0L}},
    {"writedelay",  "wd",   P_NUM|P_VI_DEF,
			    (char_u *)&p_wd,
			    {(char_u *)0L, (char_u *)0L}},

/* terminal output codes */
#define p_term(sss, vvv)   {sss, NULL, P_STRING|P_VI_DEF|P_RALL, \
			    (char_u *)&vvv, \
			    {(char_u *)"", (char_u *)0L}},

    p_term("t_AB", T_CAB)
    p_term("t_AF", T_CAF)
    p_term("t_AL", T_CAL)
    p_term("t_al", T_AL)
    p_term("t_bc", T_BC)
    p_term("t_cd", T_CD)
    p_term("t_ce", T_CE)
    p_term("t_cl", T_CL)
    p_term("t_cm", T_CM)
    p_term("t_Co", T_CCO)
    p_term("t_CS", T_CCS)
    p_term("t_cs", T_CS)
    p_term("t_da", T_DA)
    p_term("t_db", T_DB)
    p_term("t_DL", T_CDL)
    p_term("t_dl", T_DL)
    p_term("t_ke", T_KE)
    p_term("t_ks", T_KS)
    p_term("t_le", T_LE)
    p_term("t_mb", T_MB)
    p_term("t_md", T_MD)
    p_term("t_me", T_ME)
    p_term("t_mr", T_MR)
    p_term("t_ms", T_MS)
    p_term("t_nd", T_ND)
    p_term("t_op", T_OP)
    p_term("t_RI", T_CRI)
    p_term("t_Sb", T_CSB)
    p_term("t_Sf", T_CSF)
    p_term("t_se", T_SE)
    p_term("t_so", T_SO)
    p_term("t_sr", T_SR)
    p_term("t_te", T_TE)
    p_term("t_ti", T_TI)
    p_term("t_ue", T_UE)
    p_term("t_us", T_US)
    p_term("t_vb", T_VB)
    p_term("t_ve", T_VE)
    p_term("t_vi", T_VI)
    p_term("t_vs", T_VS)
    p_term("t_xs", T_XS)
    p_term("t_ZH", T_CZH)
    p_term("t_ZR", T_CZR)

/* terminal key codes are not in here */

    {NULL, NULL, 0, NULL, {NULL, NULL}}		/* end marker */
};

#define PARAM_COUNT (sizeof(options) / sizeof(struct vimoption))

static char *(p_bg_values[]) = {"light", "dark", NULL};
static char *(p_nf_values[]) = {"octal", "hex", NULL};
static char *(p_ff_values[]) = {FF_UNIX, FF_DOS, FF_MAC, NULL};

static void set_option_default __ARGS((int, int));
static void set_options_default __ARGS((int dofree));
static void illegal_char __ARGS((char_u *, int));
static void did_set_title __ARGS((int icon));
static char_u *option_expand __ARGS((int));
static void set_string_option __ARGS((int opt_idx, char_u *value));
static char_u *did_set_string_option __ARGS((int opt_idx, char_u **varp, int new_value_alloced, char_u *oldval, char_u *errbuf));
static char_u *set_bool_option __ARGS((int opt_idx, char_u *varp, int value));
static char_u *set_num_option __ARGS((int opt_idx, char_u *varp, long value, char_u *errbuf));
static void check_redraw __ARGS((int flags));
static int findoption __ARGS((char_u *));
static int find_key_option __ARGS((char_u *));
static void showoptions __ARGS((int));
static int option_not_default __ARGS((struct vimoption *));
static void showoneopt __ARGS((struct vimoption *));
static int  istermoption __ARGS((struct vimoption *));
static char_u *get_varp __ARGS((struct vimoption *));
static void option_value2string __ARGS((struct vimoption *));
#ifdef HAVE_LANGMAP
static void langmap_init __ARGS((void));
static void langmap_set __ARGS((void));
#endif
static void paste_option_changed __ARGS((void));
static void compatible_set __ARGS((void));
static void fill_breakat_flags __ARGS((void));
static int check_opt_strings __ARGS((char_u *val, char **values, int));

/*
 * Initialize the options, first part.
 *
 * Called only once from main(), just after creating the first buffer.
 */
    void
set_init_1()
{
    char_u  *p;
    int	    opt_idx;
    long    n;

#ifdef HAVE_LANGMAP
    langmap_init();
#endif

    /* Be Vi compatible by default */
    p_cp = TRUE;

    /*
     * Find default value for 'shell' option.
     */
    if ((p = vim_getenv((char_u *)"SHELL")) != NULL
#if defined(MSDOS) || defined(WIN32) || defined(OS2)
# ifdef __EMX__
	    || (p = vim_getenv((char_u *)"EMXSHELL")) != NULL
# endif
	    || (p = vim_getenv((char_u *)"COMSPEC")) != NULL
# ifdef WIN32
	    || (p = default_shell()) != NULL
# endif
#endif
							    )
    {
	set_string_default("sh", p);
    }

    /*
     * 'maxmemtot' and 'maxmem' may have to be adjusted for available memory
     */
    opt_idx = findoption((char_u *)"maxmemtot");
    if (options[opt_idx].def_val[VI_DEFAULT] == (char_u *)0L)
    {
	n = (mch_avail_mem(FALSE) >> 11);
	options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
	opt_idx = findoption((char_u *)"maxmem");
	if ((long)options[opt_idx].def_val[VI_DEFAULT] > n
			  || (long)options[opt_idx].def_val[VI_DEFAULT] == 0L)
	    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)n;
    }

    /*
     * set all the options (except the terminal options) to their default value
     */
    set_options_default(FALSE);

#ifdef USE_GUI
    if (found_reverse_arg)
	set_option_value((char_u *)"bg", 0L, (char_u *)"dark");
#endif

    curbuf->b_p_initialized = TRUE;
    check_buf_options(curbuf);
    check_options();

    /*
     * initialize the table for 'iskeyword' et.al.
     * Must be before option_expand(), because that one needs vim_isIDc()
     */
    init_chartab();

    /*
     * initialize the table for 'breakat'.
     */
    fill_breakat_flags();

    /*
     * Expand environment variables and things like "~" for the defaults.
     * If option_expand() returns non-NULL the variable is expanded. This can
     * only happen for non-indirect options.
     * Also set the default to the expanded value, so ":set" does not list
     * them. Don't set the P_ALLOCED flag, because we don't want to free the
     * default.
     */
    for (opt_idx = 0; !istermoption(&options[opt_idx]); opt_idx++)
    {
	p = option_expand(opt_idx);
	if (p != NULL)
	{
	    *(char_u **)options[opt_idx].var = p;
	    /* VIMEXP
	     * Defaults for all expanded options are currently the same for Vi
	     * and Vim.  When this changes, add some code here!  Also need to
	     * split P_DEF_ALLOCED in two.
	     */
	    options[opt_idx].def_val[VI_DEFAULT] = p;
	    options[opt_idx].flags |= P_DEF_ALLOCED;
	}
    }

    /* Initialize the highlight_attr[] table. */
    highlight_changed();

    curbuf->b_start_ffc = *curbuf->b_p_ff;    /* Buffer is unchanged */
}

/*
 * Set an option to its default value.
 */
    static void
set_option_default(opt_idx, dofree)
    int	    opt_idx;
    int	    dofree;	/* TRUE when old value can be freed */
{
    char_u	*varp;		/* pointer to variable for current option */
    int		dvi;		/* index in def_val[] */
    int		flags;

    varp = get_varp(&(options[opt_idx]));
    flags = options[opt_idx].flags;
    if (varp != NULL)	    /* nothing to do for hidden option */
    {
	if ((flags & P_VI_DEF) || p_cp)
	    dvi = VI_DEFAULT;
	else
	    dvi = VIM_DEFAULT;
	if (flags & P_STRING)
	{
	    /* indirect options are always in allocated memory */
	    if (flags & P_IND)
		set_string_option_direct(NULL, opt_idx,
				      options[opt_idx].def_val[dvi], dofree);
	    else
	    {
		if (dofree && (flags & P_ALLOCED))
		    free_string_option(*(char_u **)(varp));
		*(char_u **)varp = options[opt_idx].def_val[dvi];
		options[opt_idx].flags &= ~P_ALLOCED;
	    }
	}
	else if (flags & P_NUM)
	    *(long *)varp = (long)options[opt_idx].def_val[dvi];
	else	/* P_BOOL */
	    /* the cast to long is required for Manx C */
	    *(int *)varp = (int)(long)options[opt_idx].def_val[dvi];
    }
}

/*
 * Set all options (except terminal options) to their default value.
 */
    static void
set_options_default(dofree)
    int	    dofree;		/* may free old value */
{
    int	    i;

    for (i = 0; !istermoption(&options[i]); i++)
	if (!(options[i].flags & P_NODEFAULT))
	    set_option_default(i, dofree);
}

/*
 * Set the Vi-default value of a string option.
 * Used for 'sh' and 'term'.
 */
    void
set_string_default(name, val)
    char    *name;
    char_u  *val;
{
    char_u	*p;
    int		opt_idx;

    p = vim_strsave(val);
    if (p != NULL)		/* we don't want a NULL */
    {
	opt_idx = findoption((char_u *)name);
	if (options[opt_idx].flags & P_DEF_ALLOCED)
	    vim_free(options[opt_idx].def_val[VI_DEFAULT]);
	options[opt_idx].def_val[VI_DEFAULT] = p;
	options[opt_idx].flags |= P_DEF_ALLOCED;
    }
}

/*
 * Set the Vi-default value of a number option.
 * Used for 'liens' and 'columns'.
 */
    void
set_number_default(name, val)
    char	*name;
    long	val;
{
    options[findoption((char_u *)name)].def_val[VI_DEFAULT] = (char_u *)val;
}

/*
 * Initialize the options, part two: After getting Rows and Columns and
 * setting 'term'.
 */
    void
set_init_2()
{
/*
 * 'scroll' defaults to half the window height. Note that this default is
 * wrong when the window height changes.
 */
    options[findoption((char_u *)"scroll")].def_val[VI_DEFAULT]
					      = (char_u *)((long_u)Rows >> 1);
    comp_col();

#if !((defined(MSDOS) || defined(OS2) || defined(WIN32)) && !defined(USE_GUI))
    {
	int	idx4;

	/*
	 * Try guessing the value of 'background', depending on the terminal
	 * name.  Only need to check for terminals with a dark background,
	 * that can handle color.
	 */
	idx4 = findoption((char_u *)"bg");
	if (!(options[idx4].flags & P_WAS_SET))
	{
	    if (STRCMP(T_NAME, "linux") == 0)	    /* linux console */
		set_string_option_direct(NULL, idx4, (char_u *)"dark", TRUE);
	}
    }
#endif
}

/*
 * Initialize the options, part three: After reading the .vimrc
 */
    void
set_init_3()
{
#if defined(UNIX) || defined(OS2)
/*
 * Set 'shellpipe' and 'shellredir', depending on the 'shell' option.
 * This is done after other initializations, where 'shell' might have been
 * set, but only if they have not been set before.
 */
    char_u  *p;
    int	    idx1;
    int	    idx2;
    int	    do_sp;
    int	    do_srr;

    idx1 = findoption((char_u *)"sp");
    idx2 = findoption((char_u *)"srr");
    do_sp = !(options[idx1].flags & P_WAS_SET);
    do_srr = !(options[idx2].flags & P_WAS_SET);

    /*
     * Isolate the name of the shell:
     * - Skip beyond any path.  E.g., "/usr/bin/csh -f" -> "csh -f".
     * - Remove any argument.  E.g., "csh -f" -> "csh".
     */
    p = gettail(p_sh);
    p = vim_strnsave(p, skiptowhite(p) - p);
    if (p != NULL)
    {
	/*
	 * Default for p_sp is "| tee", for p_srr is ">".
	 * For known shells it is changed here to include stderr.
	 */
	if (	   fnamecmp(p, "csh") == 0
		|| fnamecmp(p, "tcsh") == 0
# ifdef OS2	    /* also check with .exe extension */
		|| fnamecmp(p, "csh.exe") == 0
		|| fnamecmp(p, "tcsh.exe") == 0
# endif
	   )
	{
	    if (do_sp)
	    {
		p_sp = (char_u *)"|& tee";
		options[idx1].def_val[VI_DEFAULT] = p_sp;
	    }
	    if (do_srr)
	    {
		p_srr = (char_u *)">&";
		options[idx2].def_val[VI_DEFAULT] = p_srr;
	    }
	}
	else
# ifndef OS2	/* Always use bourne shell style redirection if we reach this */
	    if (       STRCMP(p, "sh") == 0
		    || STRCMP(p, "ksh") == 0
		    || STRCMP(p, "zsh") == 0
		    || STRCMP(p, "bash") == 0)
# endif
	    {
		if (do_sp)
		{
		    p_sp = (char_u *)"2>&1| tee";
		    options[idx1].def_val[VI_DEFAULT] = p_sp;
		}
		if (do_srr)
		{
		    p_srr = (char_u *)">%s 2>&1";
		    options[idx2].def_val[VI_DEFAULT] = p_srr;
		}
	    }
	vim_free(p);
    }
#endif

#if defined(MSDOS) || defined(WIN32) || defined(OS2)
    /*
     * Set 'shellcmdflag and 'shellquote' depending on the 'shell' option.
     * This is done after other initializations, where 'shell' might have been
     * set, but only if they have not been set before.  Default for p_shcf is
     * "/c", for p_shq is "".  For "sh" like  shells it is changed here to
     * "-c" and "\"", but not for DJGPP, because it starts the shell without
     * command.com.  And for Win32 we need to set p_sxq instead.
     */
    if (strstr((char *)p_sh, "sh") != NULL)
    {
	int	idx3;

	idx3 = findoption((char_u *)"shcf");
	if (!(options[idx3].flags & P_WAS_SET))
	{
	    p_shcf = (char_u *)"-c";
	    options[idx3].def_val[VI_DEFAULT] = p_shcf;
	}

# ifndef DJGPP
#  ifdef WIN32
	/* Somehow Win32 requires the quotes around the redirection too */
	idx3 = findoption((char_u *)"sxq");
	if (!(options[idx3].flags & P_WAS_SET))
	{
	    p_sxq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_sxq;
	}
#  else
	idx3 = findoption((char_u *)"shq");
	if (!(options[idx3].flags & P_WAS_SET))
	{
	    p_shq = (char_u *)"\"";
	    options[idx3].def_val[VI_DEFAULT] = p_shq;
	}
#  endif
# endif
    }
#endif

    set_title_defaults();
}

#ifdef USE_GUI
/*
 * Option initializations that can only be done after opening the GUI window.
 */
    void
init_gui_options()
{
    int	    idx;

    /* Set the 'background' option according to the lightness of the
     * background color. */
    idx = findoption((char_u *)"bg");
    if (!(options[idx].flags & P_WAS_SET)
			       && gui_mch_get_lightness(gui.back_pixel) < 127)
	set_string_option_direct(NULL, idx, (char_u *)"dark", TRUE);
}
#endif

/*
 * 'title' and 'icon' only default to true if they have not been set or reset
 * in .vimrc and we can read the old value.
 * When 'title' and 'icon' have been reset in .vimrc, we won't even check if
 * they can be reset.  This reduces startup time when using X on a remote
 * machine.
 */
    void
set_title_defaults()
{
    int	    idx1;
    long    val;

    idx1 = findoption((char_u *)"title");
    if (!(options[idx1].flags & P_WAS_SET))
    {
	val = ui_can_restore_title();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)val;
	p_title = val;
    }
    idx1 = findoption((char_u *)"icon");
    if (!(options[idx1].flags & P_WAS_SET))
    {
	val = ui_can_restore_icon();
	options[idx1].def_val[VI_DEFAULT] = (char_u *)val;
	p_icon = val;
    }
}

/*
 * Parse 'arg' for option settings.
 *
 * 'arg' may be IObuff, but only when no errors can be present and option
 * does not need to be expanded with option_expand().
 *
 * return FAIL if errors are detected, OK otherwise
 */
    int
do_set(arg)
    char_u	*arg;	/* option string (may be written to!) */
{
    int		opt_idx;
    char_u	*errmsg;
    char_u	errbuf[80];
    char_u	*startarg;
    int		prefix;	/* 1: nothing, 0: "no", 2: "inv" in front of name */
    int		nextchar;	    /* next non-white char after option name */
    int		afterchar;	    /* character just after option name */
    int		len;
    int		i;
    long	value;
    int		key;
    int		flags;		    /* flags for current option */
    char_u	*varp = NULL;	    /* pointer to variable for current option */
    int		errcnt = 0;	    /* number of errornous entries */
    int		did_show = FALSE;   /* already showed one value */

    if (*arg == NUL)
    {
	showoptions(0);
	return OK;
    }

    while (*arg)	/* loop to process all options */
    {
	errmsg = NULL;
	startarg = arg;	    /* remember for error message */

	if (STRNCMP(arg, "all", 3) == 0)
	{
	    /*
	     * ":set all"  show all options.
	     * ":set all&" set all options to their default value.
	     */
	    arg += 3;
	    if (*arg == '&')
	    {
		++arg;
		set_options_default(TRUE);
	    }
	    else
		showoptions(1);
	}
	else if (STRNCMP(arg, "termcap", 7) == 0)
	{
	    showoptions(2);
	    show_termcodes();
	    arg += 7;
	}
	else
	{
	    prefix = 1;
	    if (STRNCMP(arg, "no", 2) == 0)
	    {
		prefix = 0;
		arg += 2;
	    }
	    else if (STRNCMP(arg, "inv", 3) == 0)
	    {
		prefix = 2;
		arg += 3;
	    }
		/* find end of name */
	    if (*arg == '<')
	    {
		opt_idx = -1;
		/* look out for <t_>;> */
		if (arg[1] == 't' && arg[2] == '_' && arg[3] && arg[4])
		    len = 5;
		else
		{
		    len = 1;
		    while (arg[len] != NUL && arg[len] != '>')
			++len;
		}
		if (arg[len] != '>')
		{
		    errmsg = e_invarg;
		    goto skip;
		}
		nextchar = arg[len];
		arg[len] = NUL;			    /* put NUL after name */
		if (arg[1] == 't' && arg[2] == '_') /* could be term code */
		    opt_idx = findoption(arg + 1);
		arg[len++] = nextchar;		    /* restore nextchar */
		key = 0;
		if (opt_idx == -1)
		    key = find_key_option(arg + 1);
		nextchar = arg[len];
	    }
	    else
	    {
		len = 0;
		/*
		 * The two characters after "t_" may not be alphanumeric.
		 */
		if (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
		{
		    len = 4;
		}
		else
		{
		    while (isalnum(arg[len]) || arg[len] == '_')
			++len;
		}
		nextchar = arg[len];
		arg[len] = NUL;			    /* put NUL after name */
		opt_idx = findoption(arg);
		arg[len] = nextchar;		    /* restore nextchar */
		key = 0;
		if (opt_idx == -1)
		    key = find_key_option(arg);
	    }

	    if (opt_idx == -1 && key == 0)	/* found a mismatch: skip */
	    {
		errmsg = (char_u *)"Unknown option";
		goto skip;
	    }

	    if (opt_idx >= 0)
	    {
		if (options[opt_idx].var == NULL)   /* hidden option: skip */
		    goto skip;

		flags = options[opt_idx].flags;
		varp = get_varp(&(options[opt_idx]));
	    }
	    else
		flags = P_STRING;

	    /* remember character after option name */
	    afterchar = nextchar;

	    /* skip white space, allow ":set ai  ?" */
	    while (vim_iswhite(nextchar))
		nextchar = arg[++len];

	    if (vim_strchr((char_u *)"?=:!&", nextchar) != NULL)
	    {
		arg += len;
		len = 0;
	    }

	    /*
	     * allow '=' and ':' as MSDOS command.com allows only one
	     * '=' character per "set" command line. grrr. (jw)
	     */
	    if (nextchar == '?' || (prefix == 1 && vim_strchr((char_u *)"=:&",
				      nextchar) == NULL && !(flags & P_BOOL)))
	    {					    /* print value */
		if (did_show)
		    msg_putchar('\n');	    /* cursor below last one */
		else
		{
		    gotocmdline(TRUE);	    /* cursor at status line */
		    did_show = TRUE;	    /* remember that we did a line */
		}
		if (opt_idx >= 0)
		    showoneopt(&options[opt_idx]);
		else
		{
		    char_u	    name[2];
		    char_u	    *p;

		    name[0] = KEY2TERMCAP0(key);
		    name[1] = KEY2TERMCAP1(key);
		    p = find_termcode(name);
		    if (p == NULL)
		    {
			errmsg = (char_u *)"Unknown option";
			goto skip;
		    }
		    else
			(void)show_one_termcode(name, p, TRUE);
		}
		if (nextchar != '?' && nextchar != NUL &&
						      !vim_iswhite(afterchar))
		    errmsg = e_trailing;
	    }
	    else
	    {
		if (flags & P_BOOL)		    /* boolean */
		{
		    if (nextchar == '=' || nextchar == ':')
		    {
			errmsg = e_invarg;
			goto skip;
		    }

		    /*
		     * ":set opt!" or ":set invopt": invert
		     * ":set opt&": reset to default value
		     * ":set opt" or ":set noopt": set or reset
		     */
		    if (prefix == 2 || nextchar == '!')
			value = *(int *)(varp) ^ 1;
		    else if (nextchar == '&')
		    {
			/* Use a trick here to get the default value, without
			 * setting the actual value yet. */
			i = *(int *)varp;
			set_option_default(opt_idx, FALSE);
			value = *(int *)varp;
			*(int *)varp = i;
		    }
		    else
			value = prefix;

		    errmsg = set_bool_option(opt_idx, varp, (int)value);
		}
		else				    /* numeric or string */
		{
		    if (vim_strchr((char_u *)"=:&", nextchar) == NULL
							       || prefix != 1)
		    {
			errmsg = e_invarg;
			goto skip;
		    }

		    if (flags & P_NUM)		    /* numeric */
		    {
			/*
			 * Different ways to set a number option:
			 * &	    set to default value
			 * <xx>	    accept special key codes for 'wildchar'
			 * c	    accept any non-digit for 'wildchar'
			 * [-]0-9   set number
			 * other    error
			 */
			arg += len + 1;
			if (nextchar == '&')
			{
			    long	temp;

			    /* Use a trick here to get the default value,
			     * without setting the actual value yet. */
			    temp = *(long *)varp;
			    set_option_default(opt_idx, FALSE);
			    value = *(long *)varp;
			    *(long *)varp = temp;
			}
			else if (  (long *)varp == &p_wc
				&& (*arg == '<'
				    || *arg == '^'
				    || ((!arg[1] || vim_iswhite(arg[1]))
					&& !isdigit(*arg))))
			{
			    if (*arg == '<')
				value = find_key_option(arg + 1);
			    else if (*arg == '^')
				value = arg[1] ^ 0x40;
			    else
				value = *arg;
			    if (value == 0)
			    {
				errmsg = e_invarg;
				goto skip;
			    }
			}
				/* allow negative numbers (for 'undolevels') */
			else if (*arg == '-' || isdigit(*arg))
			{
			    i = 0;
			    if (*arg == '-')
				i = 1;
#ifdef HAVE_STRTOL
			    value = strtol((char *)arg, NULL, 0);
			    if (arg[i] == '0' && TO_LOWER(arg[i + 1]) == 'x')
				i += 2;
#else
			    value = atol((char *)arg);
#endif
			    while (isdigit(arg[i]))
				++i;
			    if (arg[i] != NUL && !vim_iswhite(arg[i]))
			    {
				errmsg = e_invarg;
				goto skip;
			    }
			}
			else
			{
			    errmsg = (char_u *)"Number required after =";
			    goto skip;
			}

			errmsg = set_num_option(opt_idx, varp, value, errbuf);
		    }
		    else if (opt_idx >= 0)		    /* string */
		    {
			char_u	    *save_arg = NULL;
			char_u	    *s;
			char_u	    *oldval;	/* previous value if *varp */
			int	    new_value_alloced;	/* new string option
							   was allocated */

			/* The old value is kept until we are sure that the new
			 * value is valid.  set_option_default() is therefore
			 * called with FALSE
			 */
			oldval = *(char_u **)(varp);
			if (nextchar == '&')	    /* set to default val */
			{
			    set_option_default(opt_idx, FALSE);
			    new_value_alloced =
					 (options[opt_idx].flags & P_ALLOCED);
			}
			else
			{
			    arg += len + 1; /* jump to after the '=' or ':' */

			    /*
			     * Convert 'whichwrap' number to string, for
			     * backwards compatibility with Vim 3.0.
			     * Misuse errbuf[] for the resulting string.
			     */
			    if (varp == (char_u *)&p_ww && isdigit(*arg))
			    {
				*errbuf = NUL;
				i = getdigits(&arg);
				if (i & 1)
				    STRCAT(errbuf, "b,");
				if (i & 2)
				    STRCAT(errbuf, "s,");
				if (i & 4)
				    STRCAT(errbuf, "h,l,");
				if (i & 8)
				    STRCAT(errbuf, "<,>,");
				if (i & 16)
				    STRCAT(errbuf, "[,],");
				if (*errbuf != NUL)	/* remove trailing , */
				    errbuf[STRLEN(errbuf) - 1] = NUL;
				save_arg = arg;
				arg = errbuf;
			    }
			    /*
			     * Remove '>' before 'dir' and 'bdir', for
			     * backwards compatibility with version 3.0
			     */
			    else if (  *arg == '>'
				    && (varp == (char_u *)&p_dir
					    || varp == (char_u *)&p_bdir))
			    {
				++arg;
			    }

			    /*
			     * Copy the new string into allocated memory.
			     * Can't use set_string_option_direct(), because
			     * we need to remove the backslashes.
			     */
					    /* may get a bit too much */
			    s = alloc((unsigned)(STRLEN(arg) + 1));
			    if (s == NULL)  /* out of memory, don't change */
				break;
			    *(char_u **)(varp) = s;

			    /*
			     * Copy the string, skip over escaped chars.
			     * For MS-DOS and WIN32 backslashes before normal
			     * file name characters are not removed.
			     */
			    while (*arg && !vim_iswhite(*arg))
			    {
				if (*arg == '\\' && arg[1] != NUL
#ifdef BACKSLASH_IN_FILENAME
					&& !((flags & P_EXPAND)
						&& vim_isfilec(arg[1])
						&& arg[1] != '\\')
#endif
								    )
				    ++arg;
				*s++ = *arg++;
			    }
			    *s = NUL;

			    if (save_arg != NULL)   /* number for 'whichwrap' */
				arg = save_arg;
			    new_value_alloced = TRUE;
			}

			/* expand environment variables and ~ */
			s = option_expand(opt_idx);
			if (s != NULL)
			{
			    if (new_value_alloced)
				vim_free(*(char_u **)(varp));
			    *(char_u **)(varp) = s;
			    new_value_alloced = TRUE;
			}

			errmsg = did_set_string_option(opt_idx, (char_u **)varp,
					   new_value_alloced, oldval, errbuf);
			/*
			 * If error detected, print the error message.
			 */
			if (errmsg != NULL)
			    goto skip;
		    }
		    else	    /* key code option */
		    {
			char_u	    name[2];
			char_u	    *p;

			name[0] = KEY2TERMCAP0(key);
			name[1] = KEY2TERMCAP1(key);
			if (nextchar == '&')
			{
			    if (add_termcap_entry(name, TRUE) == FAIL)
				errmsg = (char_u *)"Not found in termcap";
			}
			else
			{
			    arg += len + 1; /* jump to after the '=' or ':' */
			    for(p = arg; *p && !vim_iswhite(*p); ++p)
			    {
				if (*p == '\\' && *(p + 1))
				    ++p;
			    }
			    nextchar = *p;
			    *p = NUL;
			    add_termcode(name, arg);
			    *p = nextchar;
			}
			if (full_screen)
			    ttest(FALSE);
			redraw_all_later(CLEAR);
		    }
		}
		if (opt_idx >= 0)
		    options[opt_idx].flags |= P_WAS_SET;
	    }

skip:
	    /*
	     * Advance to next argument.
	     * - skip until a blank found, taking care of backslashes
	     * - skip blanks
	     */
	    while (*arg != NUL && !vim_iswhite(*arg))
		if (*arg++ == '\\' && *arg != NUL)
		    ++arg;
	}
	arg = skipwhite(arg);

	if (errmsg)
	{
	    ++no_wait_return;	/* wait_return done below */
	    emsg(errmsg);	/* show error highlighted */
	    MSG_PUTS(": ");
				/* show argument normal */
	    while (startarg < arg)
		msg_puts(transchar(*startarg++));
	    msg_end();		/* check for scrolling */
	    --no_wait_return;

	    ++errcnt;		/* count number of errors */
	    did_show = TRUE;	/* error message counts as show */
	    if (sourcing_name != NULL)
		break;
	}
    }

    return (errcnt == 0 ? OK : FAIL);
}

    static void
illegal_char(errbuf, c)
    char_u	*errbuf;
    int		c;
{
    sprintf((char *)errbuf, "Illegal character <%s>", (char *)transchar(c));
}

/*
 * When changing 'title', 'titlestring', 'icon' or 'iconstring', call
 * maketitle() to create and display it.
 * When switching the title or icon off, call mch_restore_title() to get
 * the old value back.
 */
    static void
did_set_title(icon)
    int	    icon;	    /* Did set icon instead of title */
{
    if (!starting
#ifdef USE_GUI
	    && !gui.starting
#endif
				)
    {
	maketitle();
	if (icon)
	{
	    if (!p_icon && *p_iconstring == NUL)
		mch_restore_title(2);
	}
	else
	{
	    if (!p_title && *p_titlestring == NUL)
		mch_restore_title(1);
	}
    }
}

/*
 * set_options_bin -  called when 'bin' changes value.
 */
    void
set_options_bin(oldval, newval)
    int	    oldval;
    int	    newval;
{
    /*
     * The option values that are changed when 'bin' changes are
     * copied when 'bin is set and restored when 'bin' is reset.
     */
    if (newval)
    {
	if (!oldval)		/* switched on */
	{
	    curbuf->b_p_tw_nobin = curbuf->b_p_tw;
	    curbuf->b_p_wm_nobin = curbuf->b_p_wm;
	    curbuf->b_p_ml_nobin = curbuf->b_p_ml;
	    curbuf->b_p_et_nobin = curbuf->b_p_et;
	}

	curbuf->b_p_tw = 0;	/* no automatic line wrap */
	curbuf->b_p_wm = 0;	/* no automatic line wrap */
	curbuf->b_p_ml = 0;	/* no modelines */
	curbuf->b_p_et = 0;	/* no expandtab */
    }
    else if (oldval)		/* switched off */
    {
	curbuf->b_p_tw = curbuf->b_p_tw_nobin;
	curbuf->b_p_wm = curbuf->b_p_wm_nobin;
	curbuf->b_p_ml = curbuf->b_p_ml_nobin;
	curbuf->b_p_et = curbuf->b_p_et_nobin;
    }
}

#ifdef VIMINFO
/*
 * Find the parameter represented by the given character (eg ', :, ", or /),
 * and return its associated value in the 'viminfo' string.
 * Only works for number parameters, not for 'r' or 'n'.
 * If the parameter is not specified in the string, return -1.
 */
    int
get_viminfo_parameter(type)
    int	    type;
{
    char_u  *p;

    p = find_viminfo_parameter(type);
    if (p != NULL && isdigit(*p))
	return atoi((char *)p);
    return -1;
}

/*
 * Find the parameter represented by the given character (eg ', :, ", or /) in
 * the 'viminfo' option and return a pointer to the string after it.
 * Return NULL if the parameter is not specified in the string.
 */
    char_u *
find_viminfo_parameter(type)
    int	    type;
{
    char_u  *p;

    for (p = p_viminfo; *p; ++p)
    {
	if (*p == type)
	    return p + 1;
	if (*p == 'n')		    /* 'n' is always the last one */
	    break;
	p = vim_strchr(p, ',');	    /* skip until next ',' */
	if (p == NULL)		    /* hit the end without finding parameter */
	    break;
    }
    return NULL;
}
#endif

/*
 * Expand environment variables for some string options.
 * These string options cannot be indirect!
 * Return pointer to allocated memory, or NULL when not expanded.
 */
    static char_u *
option_expand(opt_idx)
    int	    opt_idx;
{
    char_u	*p;

	/* if option doesn't need expansion or is hidden: nothing to do */
    if (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == NULL)
	return NULL;

    p = *(char_u **)(options[opt_idx].var);

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     */
    expand_env(p, NameBuff, MAXPATHL);
    if (STRCMP(NameBuff, p) == 0)   /* they are the same */
	return NULL;

    return vim_strsave(NameBuff);
}

/*
 * Check for string options that are NULL (normally only termcap options).
 */
    void
check_options()
{
    int	    opt_idx;
    char_u  **p;

    for (opt_idx = 0; options[opt_idx].fullname != NULL; opt_idx++)
	if ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != NULL)
	{
	    p = (char_u **)get_varp(&(options[opt_idx]));
	    if (*p == NULL)
		*p = empty_option;
	}
}

/*
 * Check string options in a buffer for NULL value.
 */
    void
check_buf_options(buf)
    BUF	    *buf;
{
    if (buf->b_p_ff == NULL)
	buf->b_p_ff = empty_option;
    if (buf->b_p_fo == NULL)
	buf->b_p_fo = empty_option;
    if (buf->b_p_isk == NULL)
	buf->b_p_isk = empty_option;
    if (buf->b_p_com == NULL)
	buf->b_p_com = empty_option;
    if (buf->b_p_nf == NULL)
	buf->b_p_nf = empty_option;
#ifdef CINDENT
    if (buf->b_p_cink == NULL)
	buf->b_p_cink = empty_option;
    if (buf->b_p_cino == NULL)
	buf->b_p_cino = empty_option;
#endif
#if defined(SMARTINDENT) || defined(CINDENT)
    if (buf->b_p_cinw == NULL)
	buf->b_p_cinw = empty_option;
#endif
#ifdef INSERT_EXPAND
    if (buf->b_p_cpt == NULL)
	buf->b_p_cpt = empty_option;
#endif
}

/*
 * Free the string allocated for an option.
 * Checks for the string being empty_option. This may happen if we're out of
 * memory, vim_strsave() returned NULL, which was replaced by empty_option by
 * check_options().
 * Does NOT check for P_ALLOCED flag!
 */
    void
free_string_option(p)
    char_u	*p;
{
    if (p != empty_option)
	vim_free(p);
}

/*
 * Set a string option to a new value (without checking the effect).
 * The string is copied into allocated memory.
 * If 'dofree' is set, the old value may be freed.
 * if (opt_idx == -1) name is used, otherwise opt_idx is used.
 */
    void
set_string_option_direct(name, opt_idx, val, dofree)
    char_u  *name;
    int	    opt_idx;
    char_u  *val;
    int	    dofree;
{
    char_u  *s;
    char_u  **varp;

    if (opt_idx == -1)		/* use name */
    {
	opt_idx = findoption(name);
	if (opt_idx == -1)	/* not found (should not happen) */
	    return;
    }

    if (options[opt_idx].var == NULL)	/* can't set hidden option */
	return;

    s = vim_strsave(val);
    if (s != NULL)
    {
	varp = (char_u **)get_varp(&(options[opt_idx]));
	if (dofree && (options[opt_idx].flags & P_ALLOCED))
	    free_string_option(*varp);
	*varp = s;
	options[opt_idx].flags |= P_ALLOCED;
    }
}

/*
 * Set a string option to a new value, and handle the effects.
 */
    static void
set_string_option(opt_idx, value)
    int	    opt_idx;
    char_u  *value;
{
    char_u  *s;
    char_u  **varp;
    char_u  *oldval;

    if (options[opt_idx].var == NULL)	/* don't set hidden option */
	return;

    s = vim_strsave(value);
    if (s != NULL)
    {
	varp = (char_u **)get_varp(&(options[opt_idx]));
	oldval = *varp;
	*varp = s;
	(void)did_set_string_option(opt_idx, varp, TRUE, oldval, NULL);
    }
}

/*
 * Handle string options that need some action to perform when changed.
 * Returns NULL for success, or an error message for an error.
 */
    static char_u *
did_set_string_option(opt_idx, varp, new_value_alloced, oldval, errbuf)
    int		opt_idx;		/* index in options[] table */
    char_u	**varp;			/* pointer to the option variable */
    int		new_value_alloced;	/* new value was allocated */
    char_u	*oldval;		/* previous value of the option */
    char_u	*errbuf;		/* buffer for errors, or NULL */
{
    char_u	*errmsg = NULL;
    char_u	*s, *p;
    int		did_chartab = FALSE;

    /* 'term' */
    if (varp == &T_NAME)
    {
	if (T_NAME[0] == NUL)
	    errmsg = (char_u *)"Cannot set 'term' to empty string";
#ifdef USE_GUI
	if (gui.in_use)
	    errmsg = (char_u *)"Cannot change term in GUI";
	else if (term_is_gui(T_NAME))
	    errmsg = (char_u *)"Use \":gui\" to start the GUI";
#endif
	else if (set_termname(T_NAME) == FAIL)
	    errmsg = (char_u *)"Not found in termcap";
	else
	{
	    /* Screen colors may have changed. */
	    out_str(T_ME);
	    update_screen(CLEAR);
	}
    }

    /* 'backupext' and 'patchmode' */
    else if ((varp == &p_bex || varp == &p_pm))
    {
	if (STRCMP(*p_bex == '.' ? p_bex + 1 : p_bex,
		     *p_pm == '.' ? p_pm + 1 : p_pm) == 0)
	    errmsg = (char_u *)"'backupext' and 'patchmode' are equal";
    }

    /*
     * 'isident', 'iskeyword', 'isprint or 'isfname' option: refill chartab[]
     * If the new option is invalid, use old value.  'lisp' option: refill
     * chartab[] for '-' char
     */
    else if (  varp == &p_isi
	    || varp == &(curbuf->b_p_isk)
	    || varp == &p_isp
	    || varp == &p_isf)
    {
	if (init_chartab() == FAIL)
	{
	    did_chartab = TRUE;	    /* need to restore it below */
	    errmsg = e_invarg;	    /* error in value */
	}
    }

    /* 'highlight' */
    else if (varp == &p_hl)
    {
	if (highlight_changed() == FAIL)
	    errmsg = e_invarg;	/* invalid flags */
    }

    /* 'nrformats' */
    else if (varp == &(curbuf->b_p_nf))
    {
	if (check_opt_strings(curbuf->b_p_nf, p_nf_values, TRUE) != OK)
	    errmsg = e_invarg;
    }

    /* 'background' */
    else if (varp == &p_bg)
    {
	if (check_opt_strings(p_bg, p_bg_values, FALSE) == OK)
	{
	    if (full_screen)
		init_highlight(FALSE);
	}
	else
	    errmsg = e_invarg;
    }

#ifdef AUTOCMD
    /* 'eventignore' */
    else if (varp == &p_ei)
    {
	if (check_ei() == FAIL)
	    errmsg = e_invarg;
    }
#endif

    /* 'fileformat' */
    else if (varp == &(curbuf->b_p_ff))
    {
	if (check_opt_strings(curbuf->b_p_ff, p_ff_values, FALSE) != OK)
	    errmsg = e_invarg;
	else
	{
	    /* also change 'textmode' */
	    if (get_fileformat(curbuf) == EOL_DOS)
		curbuf->b_p_tx = TRUE;
	    else
		curbuf->b_p_tx = FALSE;
	}
    }

    /* 'fileformats' */
    else if (varp == &p_ffs)
    {
	if (check_opt_strings(p_ffs, p_ff_values, TRUE) != OK)
	    errmsg = e_invarg;
	else
	{
	    /* also change 'textauto' */
	    if (*p_ffs == NUL)
		p_ta = FALSE;
	    else
		p_ta = TRUE;
	}
    }

    /* 'comments' */
    else if (varp == &(curbuf->b_p_com))
    {
	for (s = curbuf->b_p_com; *s; )
	{
	    while (*s && *s != ':')
	    {
		if (vim_strchr((char_u *)COM_ALL, *s) == NULL)
		{
		    errmsg = (char_u *)"Illegal flag";
		    break;
		}
		++s;
	    }
	    if (*s++ == NUL)
		errmsg = (char_u *)"Missing colon";
	    else if (*s == ',' || *s == NUL)
		errmsg = (char_u *)"Zero length string";
	    if (errmsg != NULL)
		break;
	    while (*s && *s != ',')
	    {
		if (*s == '\\' && s[1] != NUL)
		    ++s;
		++s;
	    }
	    s = skip_to_option_part(s);
	}
    }

#ifdef VIMINFO
    /* 'viminfo' */
    else if (varp == &(p_viminfo))
    {
	for (s = p_viminfo; *s;)
	{
	    /* Check it's a valid character */
	    if (vim_strchr((char_u *)"\"'%frn:/", *s) == NULL)
	    {
		if (errbuf != NULL)
		{
		    illegal_char(errbuf, *s);
		    errmsg = errbuf;
		}
		else
		    errmsg = (char_u *)"";
		break;
	    }
	    if (*s == 'n')	/* name is always last one */
	    {
		break;
	    }
	    else if (*s == 'r') /* skip until next ',' */
	    {
		while (*++s && *s != ',')
		    ;
	    }
	    else if (*s == '%') /* no extra chars */
		++s;
	    else		    /* must have a number */
	    {
		while (isdigit(*++s))
		    ;

		if (!isdigit(*(s - 1)))
		{
		    if (errbuf != NULL)
		    {
			sprintf((char *)errbuf, "Missing number after <%s>",
							 transchar(*(s - 1)));
			errmsg = errbuf;
		    }
		    else
			errmsg = (char_u *)"";
		    break;
		}
	    }
	    s = skip_to_option_part(s);
	}
	if (*p_viminfo && errmsg == NULL && get_viminfo_parameter('\'') < 0)
	    errmsg = (char_u *)"Must specify a ' value";
    }
#endif /* VIMINFO */

    /* terminal options */
    else if (istermoption(&options[opt_idx]) && full_screen)
    {
	ttest(FALSE);
	if (varp == &T_ME)
	{
	    out_str(T_ME);
	    update_screen(CLEAR);
	}
    }

    /* 'showbreak' */
    else if (varp == &p_sbr)
    {
	for (s = p_sbr; *s; ++s)
	    if (charsize(*s) != 1)
		errmsg = (char_u *)"contains unprintable character";
    }

#ifdef USE_GUI
    /* 'guifont' */
    else if (varp == &p_guifont)
	gui_init_font(p_guifont);

    /* 'guicursor' */
    else if (varp == &p_guicursor)
	errmsg = parse_guicursor();
#endif

#ifdef HAVE_LANGMAP
    /* 'langmap' */
    else if (varp == &p_langmap)
	langmap_set();
#endif

    /* 'breakat' */
    else if (varp == &p_breakat)
	fill_breakat_flags();

    /* 'titlestring' and 'iconstring' */
    else if (varp == &p_titlestring)
	did_set_title(FALSE);
    else if (varp == &p_iconstring)
	did_set_title(TRUE);

#ifdef USE_GUI
    /* 'guioptions' */
    else if (varp == &p_guioptions)
	gui_init_which_components(oldval);
#endif

    /* Options that are a list of flags. */
    else
    {
	p = NULL;
	if (varp == &p_ww)
	    p = (char_u *)WW_ALL;
	if (varp == &p_shm)
	    p = (char_u *)SHM_ALL;
	else if (varp == &(p_cpo))
	    p = (char_u *)CPO_ALL;
	else if (varp == &(curbuf->b_p_fo))
	    p = (char_u *)FO_ALL;
	else if (varp == &p_mouse)
	{
#ifdef USE_MOUSE
	    p = (char_u *)MOUSE_ALL;
#else
	    if (*p_mouse != NUL)
		errmsg = (char_u *)"No mouse support";
#endif
	}
#if defined(USE_GUI) || defined(USE_CLIPBOARD)
	else if (varp == &p_guioptions)
	    p = (char_u *)GO_ALL;
#endif
	if (p != NULL)
	{
	    for (s = *varp; *s; ++s)
		if (vim_strchr(p, *s) == NULL)
		{
		    if (errbuf != NULL)
		    {
			illegal_char(errbuf, *s);
			errmsg = errbuf;
		    }
		    else
			errmsg = (char_u *)"";
		    break;
		}
	}
    }

    /*
     * If error detected, restore the previous value.
     */
    if (errmsg != NULL)
    {
	if (new_value_alloced)
	    vim_free(*varp);
	*varp = oldval;
	/*
	 * When resetting some values, need to act on it.
	 */
	if (did_chartab)
	    (void)init_chartab();
	if (varp == &p_hl)
	    (void)highlight_changed();
    }
    else
    {
	/*
	 * Free string options that are in allocated memory.
	 */
	if (options[opt_idx].flags & P_ALLOCED)
	    free_string_option(oldval);
	if (new_value_alloced)
	    options[opt_idx].flags |= P_ALLOCED;
	else
	    options[opt_idx].flags &= ~P_ALLOCED;
    }

#ifdef USE_MOUSE
    if (varp == &p_mouse)
    {
	if (*p_mouse == NUL)
	    mch_setmouse(FALSE);    /* switch mouse off */
	else
	    setmouse();		    /* in case 'mouse' changed */
    }
#endif

    curwin->w_set_curswant = TRUE;  /* in case 'showbreak' changed */
    check_redraw(options[opt_idx].flags);

    return errmsg;
}

/*
 * Set the value of a boolean option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    static char_u *
set_bool_option(opt_idx, varp, value)
    int	    opt_idx;		    /* index in options[] table */
    char_u  *varp;		    /* pointer to the option variable */
    int	    value;		    /* new value */
{
    int	    old_p_bin = curbuf->b_p_bin;    /* remember old 'bin' option */
    int	    old_p_ea = p_ea;		    /* remember old 'equalalways' */
    int	    old_p_wiv = p_wiv;		    /* remember old 'weirdinvert' */
#ifdef FKMAP
    int	    old_akm = p_altkeymap;	    /* previous value if p_altkeymap*/
#endif

    /*
     * in secure mode, setting of the secure option is not
     * allowed
     */
    if (secure && (int *)varp == &p_secure)
	return (char_u *)"not allowed here";

    *(int *)varp = value;	    /* set the new value */

    /* handle the setting of the compatible option */
    if ((int *)varp == &p_cp)
    {
	compatible_set();
    }
    /* when 'readonly' is reset, also reset readonlymode */
    else if ((int *)varp == &curbuf->b_p_ro && !curbuf->b_p_ro)
	readonlymode = FALSE;

    /* when 'bin' is set also set some other options */
    else if ((int *)varp == &curbuf->b_p_bin)
    {
	set_options_bin(old_p_bin, curbuf->b_p_bin);
    }
    /* when 'terse' is set change 'shortmess' */
    else if ((int *)varp == &p_terse)
    {
	char_u	*p;

	p = vim_strchr(p_shm, SHM_SEARCH);

	/* insert 's' in p_shm */
	if (p_terse && p == NULL)
	{
	    STRCPY(IObuff, p_shm);
	    STRCAT(IObuff, "s");
	    set_string_option_direct((char_u *)"shm", -1, IObuff, TRUE);
	}
	/* remove 's' from p_shm */
	else if (!p_terse && p != NULL)
	    vim_memmove(p, p + 1, STRLEN(p));
    }
    /* when 'paste' is set or reset also change other options */
    else if ((int *)varp == &p_paste)
    {
	paste_option_changed();
    }
    /* when 'textmode' is set or reset also change 'fileformat' */
    else if ((int *)varp == &curbuf->b_p_tx)
    {
	set_fileformat(curbuf->b_p_tx ? EOL_DOS : EOL_UNIX);
    }
    /* when 'textauto' is set or reset also change 'fileformats' */
    else if ((int *)varp == &p_ta)
    {
	set_string_option_direct((char_u *)"ffs", -1,
			      p_ta ? (char_u *)FFS_DFLT : (char_u *)"", TRUE);
    }
    /*
     * When 'lisp' option changes include/exclude '-' in
     * keyword characters.
     */
#ifdef LISPINDENT
    else if (varp == (char_u *)&(curbuf->b_p_lisp))
	init_chartab();	    /* ignore errors */
#endif
    /* when 'title' changed, may need to change the title; same for 'icon' */
    else if ((int *)varp == &p_title)
	did_set_title(FALSE);
    else if ((int *)varp == &p_icon)
	did_set_title(TRUE);
    else if ((int *)varp == &curbuf->b_changed)
    {
	if (!value)
	    curbuf->b_start_ffc = *curbuf->b_p_ff;    /* Buffer is unchanged */
#ifdef AUTOCMD
	modified_was_set = value;
#endif
    }

    if (p_ea && !old_p_ea)
	win_equal(curwin, FALSE);

    /*
     * When 'weirdinvert' changed, set/reset 't_xs'.
     * Then set 'weirdinvert' according to value of 't_xs'.
     */
    if (p_wiv && !old_p_wiv)
	T_XS = (char_u *)"y";
    else if (!p_wiv && old_p_wiv)
	T_XS = empty_option;
    p_wiv = (*T_XS != NUL);

#ifdef FKMAP
    /*
     * In case some second language keymapping options have changed, check
     * and correct the setting in a consistent way.
     */
    if (old_akm != p_altkeymap)
    {
	if (!p_altkeymap)
	{
	    p_hkmap = p_fkmap;
	    p_fkmap = 0;
	    init_chartab();
	}
	else
	{
	    p_fkmap = p_hkmap;
	    p_hkmap = 0;
	    init_chartab();
	}
    }

    /*
     * If hkmap set, reset Farsi keymapping.
     */
    if (p_hkmap && p_altkeymap)
    {
	p_altkeymap = 0;
	p_fkmap = 0;
	init_chartab();
    }

    /*
     * If fkmap set, reset Hebrew keymapping.
     */
    if (p_fkmap && !p_altkeymap)
    {
	p_altkeymap = 1;
	p_hkmap = 0;
	init_chartab();
    }
#endif

    options[opt_idx].flags |= P_WAS_SET;

    comp_col();			    /* in case 'ruler' or 'showcmd' changed */
    curwin->w_set_curswant = TRUE;  /* in case 'list' changed */
    check_redraw(options[opt_idx].flags);

    return NULL;
}

/*
 * Set the value of a number option, and take care of side effects.
 * Returns NULL for success, or an error message for an error.
 */
    static char_u *
set_num_option(opt_idx, varp, value, errbuf)
    int	    opt_idx;		    /* index in options[] table */
    char_u  *varp;		    /* pointer to the option variable */
    long    value;		    /* new value */
    char_u  *errbuf;		    /* buffer for error messages */
{
    char_u  *errmsg = NULL;
    long    old_Rows = Rows;		/* remember old Rows */
    long    old_Columns = Columns;	/* remember old Columns */
    long    old_p_ch = p_ch;		/* remember old command line height */
    long    old_p_uc = p_uc;		/* remember old 'updatecount' */
    long    old_titlelen = p_titlelen;	/* remember old 'titlelen' */

    *(long *)varp = value;

    /*
     * Number options that need some action when changed
     */
    if ((long *)varp == &p_wh || (long *)varp == &p_hh)
    {
	if (p_wh < 0)
	{
	    errmsg = e_positive;
	    p_wh = 0;
	}
	if (p_hh < 0)
	{
	    errmsg = e_positive;
	    p_hh = 0;
	}

	/* Change window height NOW */
	if (lastwin != firstwin)
	{
	    if (p_wh)
		win_equal(curwin, TRUE);
	    if (curbuf->b_help && curwin->w_height < p_hh)
		win_setheight((int)p_hh);
	}
    }
    /* (re)set last window status line */
    else if ((long *)varp == &p_ls)
	last_status();

    /*
     * Check the bounds for numeric options here
     */
    if (Rows < min_rows() && full_screen)
    {
	if (errbuf != NULL)
	{
	    sprintf((char *)errbuf, "Need at least %d lines", min_rows());
	    errmsg = errbuf;
	}
	Rows = min_rows();
    }
    if (Columns < MIN_COLUMNS && full_screen)
    {
	if (errbuf != NULL)
	{
	    sprintf((char *)errbuf, "Need at least %d columns", MIN_COLUMNS);
	    errmsg = errbuf;
	}
	Columns = MIN_COLUMNS;
    }
    /*
     * If the screenheight has been changed, assume it is the physical
     * screenheight.
     */
    if ((old_Rows != Rows || old_Columns != Columns) && full_screen)
    {
	ui_set_winsize();	    /* try to change the window size */
	check_winsize();	    /* in case 'columns' changed */
#ifdef MSDOS
	set_window();	    /* active window may have changed */
#endif
    }

    if (curbuf->b_p_sts < 0)
    {
	errmsg = e_positive;
	curbuf->b_p_sts = 0;
    }
    if (curbuf->b_p_ts <= 0)
    {
	errmsg = e_positive;
	curbuf->b_p_ts = 8;
    }
    if (curbuf->b_p_tw < 0)
    {
	errmsg = e_positive;
	curbuf->b_p_tw = 0;
    }
    if (p_tm < 0)
    {
	errmsg = e_positive;
	p_tm = 0;
    }
    if (p_titlelen <= 0)
    {
	errmsg = e_positive;
	p_titlelen = 85;
    }
    if ((curwin->w_p_scroll <= 0 || curwin->w_p_scroll > curwin->w_height)
	    && full_screen)
    {
	if (curwin->w_p_scroll != 0)
	    errmsg = e_scroll;
	win_comp_scroll(curwin);
    }
    if (p_report < 0)
    {
	errmsg = e_positive;
	p_report = 1;
    }
    if ((p_sj < 0 || p_sj >= Rows) && full_screen)
    {
	if (Rows != old_Rows)	    /* Rows changed, just adjust p_sj */
	    p_sj = Rows / 2;
	else
	{
	    errmsg = e_scroll;
	    p_sj = 1;
	}
    }
    if (p_so < 0 && full_screen)
    {
	errmsg = e_scroll;
	p_so = 0;
    }
    if (p_uc < 0)
    {
	errmsg = e_positive;
	p_uc = 100;
    }
    if (p_ch < 1)
    {
	errmsg = e_positive;
	p_ch = 1;
    }
    if (p_ut < 0)
    {
	errmsg = e_positive;
	p_ut = 2000;
    }
    if (p_ss < 0)
    {
	errmsg = e_positive;
	p_ss = 0;
    }

    /* when 'updatecount' changes from zero to non-zero, open swap files */
    if (p_uc && !old_p_uc)
	ml_open_files();

    /* if p_ch changed value, change the command line height */
    if (p_ch != old_p_ch)
	command_height(old_p_ch);

    /* if 'titlelen' has changed, redraw the title */
    if (old_titlelen != p_titlelen && !starting)
	maketitle();

    options[opt_idx].flags |= P_WAS_SET;

    comp_col();			    /* in case 'columns' or 'ls' changed */
    curwin->w_set_curswant = TRUE;  /* in case 'tabstop' changed */
    check_redraw(options[opt_idx].flags);

    return errmsg;
}

/*
 * Called after an option changed: check if something needs to be redrawn.
 */
    static void
check_redraw(flags)
    int	    flags;
{
    if (flags & (P_RSTAT | P_RALL))	/* mark all status lines dirty */
	status_redraw_all();

    if (flags & (P_RBUF | P_RALL))
    {
	/* Update cursor position and botline (wrapping my have changed). */
	changed_line_abv_curs();
	invalidate_botline();
	update_topline();
    }

    if (flags & P_RBUF)
	redraw_curbuf_later(NOT_VALID);
    if (flags & P_RALL)
	redraw_all_later(NOT_VALID);
}

/*
 * Find index for option 'arg'.
 * Return -1 if not found.
 */
    static int
findoption(arg)
    char_u *arg;
{
    int		    opt_idx;
    char	    *s, *p;
    static short    quick_tab[27] = {0, 0};	/* quick access table */
    int		    is_term_opt;

    /*
     * For first call: Initialize the quick-access table.
     * It contains the index for the first option that starts with a certain
     * letter.  There are 26 letters, plus the first "t_" option.
     */
    if (quick_tab[1] == 0)
    {
	p = options[0].fullname;
	for (opt_idx = 1; (s = options[opt_idx].fullname) != NULL; opt_idx++)
	{
	    if (s[0] != p[0])
	    {
		if (s[0] == 't' && s[1] == '_')
		    quick_tab[26] = opt_idx;
		else
		    quick_tab[s[0] - 'a'] = opt_idx;
	    }
	    p = s;
	}
    }

    /*
     * Check for name starting with an illegal character.
     */
    if (arg[0] < 'a' || arg[0] > 'z')
	return -1;

    is_term_opt = (arg[0] == 't' && arg[1] == '_');
    if (is_term_opt)
	opt_idx = quick_tab[26];
    else
	opt_idx = quick_tab[arg[0] - 'a'];
    for ( ; (s = options[opt_idx].fullname) != NULL; opt_idx++)
    {
	if (STRCMP(arg, s) == 0)		    /* match full name */
	    break;
    }
    if (s == NULL && !is_term_opt)
    {
	opt_idx = quick_tab[arg[0] - 'a'];
	for ( ; options[opt_idx].fullname != NULL; opt_idx++)
	{
	    s = options[opt_idx].shortname;
	    if (s != NULL && STRCMP(arg, s) == 0)   /* match short name */
		break;
	    s = NULL;
	}
    }
    if (s == NULL)
	opt_idx = -1;
    return opt_idx;
}

/*
 * Get the value for an option.
 *
 * Returns:
 * Number or Toggle option: 1, *numval gets value.
 *	     String option: 0, *stringval gets allocated string.
 *	    unknown option: -1.
 */
    int
get_option_value(name, numval, stringval)
    char_u	*name;
    long	*numval;
    char_u	**stringval;	    /* NULL when only checking existance */
{
    int	    opt_idx;
    char_u  *varp;

    opt_idx = findoption(name);
    if (opt_idx < 0)		    /* unknown option */
	return -1;

    varp = get_varp(&(options[opt_idx]));
    if (varp == NULL)		    /* hidden option */
	return -1;

    if (options[opt_idx].flags & P_STRING)
    {
	if (stringval != NULL)
	    *stringval = vim_strsave(*(char_u **)(varp));
	return 0;
    }
    if (options[opt_idx].flags & P_NUM)
	*numval = *(long *)varp;
    else
	*numval = *(int *)varp;
    return 1;
}

/*
 * Set the value of option "name".
 * Use "string" for string options, use "number" for other options.
 */
    void
set_option_value(name, number, string)
    char_u	*name;
    long	number;
    char_u	*string;
{
    int	    opt_idx;
    char_u  *varp;

    opt_idx = findoption(name);
    if (opt_idx == -1)
	EMSG2("Unknown option: %s", name);
    else if (options[opt_idx].flags & P_STRING)
	set_string_option(opt_idx, string);
    else
    {
	varp = get_varp(&options[opt_idx]);
	if (varp != NULL)	/* hidden option is not changed */
	{
	    if (options[opt_idx].flags & P_NUM)
		(void)set_num_option(opt_idx, varp, number, NULL);
	    else
		(void)set_bool_option(opt_idx, varp, (int)number);
	}
    }
}

/*
 * Get the terminal code for a terminal option.
 * Returns NULL when not found.
 */
    char_u *
get_term_code(tname)
    char_u	*tname;
{
    int	    opt_idx;
    char_u  *varp;

    if (tname[0] != 't' || tname[1] != '_' ||
	    tname[2] == NUL || tname[3] == NUL)
	return NULL;
    if ((opt_idx = findoption(tname)) >= 0)
    {
	varp = get_varp(&(options[opt_idx]));
	if (varp != NULL)
	    varp = *(char_u **)(varp);
	return varp;
    }
    return find_termcode(tname + 2);
}

    char_u *
get_highlight_default()
{
    int i;

    i = findoption((char_u *)"hl");
    if (i >= 0)
	return options[i].def_val[VI_DEFAULT];
    return (char_u *)NULL;
}

/*
 * Translate a string like "t_xx", "<t_xx>" or "<S-Tab>" to a key number.
 */
    static int
find_key_option(arg)
    char_u *arg;
{
    int		key;
    int		modifiers;

    /*
     * Don't use get_special_key_code() for t_xx, we don't want it to call
     * add_termcap_entry().
     */
    if (arg[0] == 't' && arg[1] == '_' && arg[2] && arg[3])
	key = TERMCAP2KEY(arg[2], arg[3]);
    else
    {
	--arg;			    /* put arg at the '<' */
	key = find_special_key(&arg, &modifiers);
	if (modifiers)		    /* can't handle modifiers here */
	    key = 0;
    }
    return key;
}

/*
 * if 'all' == 0: show changed options
 * if 'all' == 1: show all normal options
 * if 'all' == 2: show all terminal options
 */
    static void
showoptions(all)
    int		all;
{
    struct vimoption   *p;
    int		    col;
    int		    isterm;
    char_u	    *varp;
    struct vimoption	**items;
    int		    item_count;
    int		    run;
    int		    row, rows;
    int		    cols;
    int		    i;
    int		    len;

#define INC 20
#define GAP 3

    items = (struct vimoption **)alloc((unsigned)(sizeof(struct vimoption *) *
								PARAM_COUNT));
    if (items == NULL)
	return;

    /* Highlight title */
    if (all == 2)
	MSG_PUTS_TITLE("\n--- Terminal codes ---");
    else
	MSG_PUTS_TITLE("\n--- Options ---");

    /*
     * do the loop two times:
     * 1. display the short items
     * 2. display the long items (only strings and numbers)
     */
    for (run = 1; run <= 2 && !got_int; ++run)
    {
	/*
	 * collect the items in items[]
	 */
	item_count = 0;
	for (p = &options[0]; p->fullname != NULL; p++)
	{
	    isterm = istermoption(p);
	    varp = get_varp(p);
	    if (varp != NULL && (
		(all == 2 && isterm) ||
		(all == 1 && !isterm) ||
		(all == 0 && option_not_default(p))))
	    {
		if (p->flags & P_BOOL)
		    len = 1;		/* a toggle option fits always */
		else
		{
		    option_value2string(p);
		    len = STRLEN(p->fullname) + vim_strsize(NameBuff) + 1;
		}
		if ((len <= INC - GAP && run == 1) ||
						(len > INC - GAP && run == 2))
		    items[item_count++] = p;
	    }
	}

	/*
	 * display the items
	 */
	if (run == 1)
	{
	    cols = (Columns + GAP - 3) / INC;
	    if (cols == 0)
		cols = 1;
	    rows = (item_count + cols - 1) / cols;
	}
	else	/* run == 2 */
	    rows = item_count;
	for (row = 0; row < rows && !got_int; ++row)
	{
	    msg_putchar('\n');			/* go to next line */
	    if (got_int)			/* 'q' typed in more */
		break;
	    col = 0;
	    for (i = row; i < item_count; i += rows)
	    {
		msg_col = col;			/* make columns */
		showoneopt(items[i]);
		col += INC;
	    }
	    out_flush();
	    ui_breakcheck();
	}
    }
    vim_free(items);
}

/*
 * Return TRUE if option is different from the default value
 */
    static int
option_not_default(p)
    struct vimoption	*p;
{
    char_u  *varp;
    int	    dvi;

    varp = get_varp(p);
    if (varp == NULL)
	return FALSE;	/* hidden option is never changed */

    if ((p->flags & P_VI_DEF) || p_cp)
	dvi = VI_DEFAULT;
    else
	dvi = VIM_DEFAULT;
    if (p->flags & P_NUM)
	return (*(long *)varp != (long)p->def_val[dvi]);
    if (p->flags & P_BOOL)
			/* the cast to long is required for Manx C */
	return (*(int *)varp != (int)(long)p->def_val[dvi]);
    /* P_STRING */
    return STRCMP(*(char_u **)varp, p->def_val[dvi]);
}

/*
 * showoneopt: show the value of one option
 * must not be called with a hidden option!
 */
    static void
showoneopt(p)
    struct vimoption *p;
{
    char_u	    *varp;

    varp = get_varp(p);

    if ((p->flags & P_BOOL) && !*(int *)varp)
	MSG_PUTS("no");
    else
	MSG_PUTS("  ");
    MSG_PUTS(p->fullname);
    if (!(p->flags & P_BOOL))
    {
	msg_putchar('=');
	option_value2string(p);	    /* put string of option value in NameBuff */
	msg_outtrans(NameBuff);
    }
}

/*
 * Write modified options as set command to a file.
 * Return FAIL on error, OK otherwise.
 */
    int
makeset(fd)
    FILE *fd;
{
    struct vimoption	*p;
    char_u	    *s;
    int		    e;
    char_u	    *varp;

    /*
     * The options that don't have a default (terminal name, columns, lines)
     * are never written. Terminal options are also not written.
     */
    for (p = &options[0]; !istermoption(p); p++)
	if (!(p->flags & P_NO_MKRC) && !istermoption(p)
						   && (option_not_default(p)))
	{
	    varp = get_varp(p);
	    if (p->flags & P_BOOL)
		fprintf(fd, "set %s%s", *(int *)(varp) ? "" : "no",
								 p->fullname);
	    else if (p->flags & P_NUM)
		fprintf(fd, "set %s=%ld", p->fullname, *(long *)(varp));
	    else    /* P_STRING */
	    {
		fprintf(fd, "set %s=", p->fullname);
		s = *(char_u **)(varp);
		/* some characters have to be escaped with CTRL-V or
		 * backslash */
		if (s != NULL && putescstr(fd, s, TRUE) == FAIL)
		    return FAIL;
	    }
#ifdef USE_CRNL
	    putc('\r', fd);
#endif
		/*
		 * Only check error for this putc, should catch at least
		 * the "disk full" situation.
		 */
	    e = putc('\n', fd);
	    if (e < 0)
		return FAIL;
	}
    return OK;
}

/*
 * Clear all the terminal options.
 * If the option has been allocated, free the memory.
 * Terminal options are never hidden or indirect.
 */
    void
clear_termoptions()
{
    struct vimoption   *p;

    /*
     * Reset a few things before clearing the old options. This may cause
     * outputting a few things that the terminal doesn't understand, but the
     * screen will be cleared later, so this is OK.
     */
#ifdef USE_MOUSE
    mch_setmouse(FALSE);	    /* switch mouse off */
#endif
    mch_restore_title(3);	    /* restore window titles */
#ifdef WIN32
    /*
     * Check if this is allowed now.
     */
    if (can_end_termcap_mode(FALSE) == TRUE)
#endif
	stoptermcap();			/* stop termcap mode */

    for (p = &options[0]; p->fullname != NULL; p++)
	if (istermoption(p))
	{
	    if (p->flags & P_ALLOCED)
		free_string_option(*(char_u **)(p->var));
	    if (p->flags & P_DEF_ALLOCED)
		free_string_option(p->def_val[VI_DEFAULT]);
	    *(char_u **)(p->var) = empty_option;
	    p->def_val[VI_DEFAULT] = empty_option;
	    p->flags &= ~(P_ALLOCED|P_DEF_ALLOCED);
	}
    clear_termcodes();
}

/*
 * Set the terminal option defaults to the current value.
 * Used after setting the terminal name.
 */
    void
set_term_defaults()
{
    struct vimoption   *p;

    for (p = &options[0]; p->fullname != NULL; p++)
	if (istermoption(p) && p->def_val[VI_DEFAULT] != *(char_u **)(p->var))
	{
	    if (p->flags & P_DEF_ALLOCED)
	    {
		free_string_option(p->def_val[VI_DEFAULT]);
		p->flags &= ~P_DEF_ALLOCED;
	    }
	    p->def_val[VI_DEFAULT] = *(char_u **)(p->var);
	    if (p->flags & P_ALLOCED)
	    {
		p->flags |= P_DEF_ALLOCED;
		p->flags &= ~P_ALLOCED;	    /* don't free the value now */
	    }
	}
}

/*
 * return TRUE if 'p' starts with 't_'
 */
    static int
istermoption(p)
    struct vimoption *p;
{
    return (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Compute columns for ruler and shown command. 'sc_col' is also used to
 * decide what the maximum length of a message on the status line can be.
 * If there is a status line for the last window, 'sc_col' is independent
 * of 'ru_col'.
 */

#define COL_RULER 17	    /* columns needed by ruler */

    void
comp_col()
{
    int last_has_status = (p_ls == 2 || (p_ls == 1 && firstwin != lastwin));

    sc_col = 0;
    ru_col = 0;
    if (p_ru)
    {
	ru_col = COL_RULER + 1;
	/* no last status line, adjust sc_col */
	if (!last_has_status)
	    sc_col = ru_col;
    }
    if (p_sc)
    {
	sc_col += SHOWCMD_COLS;
	if (!p_ru || last_has_status)	    /* no need for separating space */
	    ++sc_col;
    }
    sc_col = Columns - sc_col;
    ru_col = Columns - ru_col;
    if (sc_col <= 0)		/* screen too narrow, will become a mess */
	sc_col = 1;
    if (ru_col <= 0)
	ru_col = 1;
}

    static char_u *
get_varp(p)
    struct vimoption	*p;
{
    if (!(p->flags & P_IND) || p->var == NULL)
	return p->var;

    switch ((long)(p->var))
    {
	case PV_LIST:	return (char_u *)&(curwin->w_p_list);
	case PV_NU:	return (char_u *)&(curwin->w_p_nu);
#ifdef RIGHTLEFT
	case PV_RL:	return (char_u *)&(curwin->w_p_rl);
#endif
	case PV_SCROLL:	return (char_u *)&(curwin->w_p_scroll);
	case PV_WRAP:	return (char_u *)&(curwin->w_p_wrap);
	case PV_LBR:	return (char_u *)&(curwin->w_p_lbr);

	case PV_AI:	return (char_u *)&(curbuf->b_p_ai);
	case PV_BIN:	return (char_u *)&(curbuf->b_p_bin);
#ifdef CINDENT
	case PV_CIN:	return (char_u *)&(curbuf->b_p_cin);
	case PV_CINK:	return (char_u *)&(curbuf->b_p_cink);
	case PV_CINO:	return (char_u *)&(curbuf->b_p_cino);
#endif
#if defined(SMARTINDENT) || defined(CINDENT)
	case PV_CINW:	return (char_u *)&(curbuf->b_p_cinw);
#endif
	case PV_COM:	return (char_u *)&(curbuf->b_p_com);
#ifdef INSERT_EXPAND
	case PV_CPT:	return (char_u *)&(curbuf->b_p_cpt);
#endif
	case PV_EOL:	return (char_u *)&(curbuf->b_p_eol);
	case PV_ET:	return (char_u *)&(curbuf->b_p_et);
	case PV_FF:	return (char_u *)&(curbuf->b_p_ff);
	case PV_FO:	return (char_u *)&(curbuf->b_p_fo);
	case PV_INF:	return (char_u *)&(curbuf->b_p_inf);
	case PV_ISK:	return (char_u *)&(curbuf->b_p_isk);
	case PV_LISP:	return (char_u *)&(curbuf->b_p_lisp);
	case PV_ML:	return (char_u *)&(curbuf->b_p_ml);
	case PV_MOD:	return (char_u *)&(curbuf->b_changed);
	case PV_NF:	return (char_u *)&(curbuf->b_p_nf);
	case PV_RO:	return (char_u *)&(curbuf->b_p_ro);
#ifdef SMARTINDENT
	case PV_SI:	return (char_u *)&(curbuf->b_p_si);
#endif
#ifndef SHORT_FNAME
	case PV_SN:	return (char_u *)&(curbuf->b_p_sn);
#endif
	case PV_STS:	return (char_u *)&(curbuf->b_p_sts);
	case PV_SW:	return (char_u *)&(curbuf->b_p_sw);
	case PV_TS:	return (char_u *)&(curbuf->b_p_ts);
	case PV_TW:	return (char_u *)&(curbuf->b_p_tw);
	case PV_TX:	return (char_u *)&(curbuf->b_p_tx);
	case PV_WM:	return (char_u *)&(curbuf->b_p_wm);
	default:	EMSG("get_varp ERROR");
    }
    /* always return a valid pointer to avoid a crash! */
    return (char_u *)&(curbuf->b_p_wm);
}

/*
 * Copy options from one window to another.
 * Used when creating a new window.
 * The 'scroll' option is not copied, because it depends on the window height.
 */
    void
win_copy_options(wp_from, wp_to)
    WIN	    *wp_from;
    WIN	    *wp_to;
{
    wp_to->w_p_list = wp_from->w_p_list;
    wp_to->w_p_nu = wp_from->w_p_nu;
#ifdef RIGHTLEFT
    wp_to->w_p_rl = wp_from->w_p_rl;
# ifdef FKMAP
    wp_to->w_p_pers = wp_from->w_p_pers;
# endif
#endif
    wp_to->w_p_wrap = wp_from->w_p_wrap;
    wp_to->w_p_lbr = wp_from->w_p_lbr;
}

/*
 * Copy options from one buffer to another.
 * Used when creating a new buffer and sometimes when entering a buffer.
 * flags:
 * BCO_ENTER	We will enter the bp_to buffer.
 * BCO_ALWAYS	Always copy the options, but only set b_p_initialized when
 *		appropriate.
 * BCO_NOHELP	Don't copy the help settings.
 */
    void
buf_copy_options(bp_from, bp_to, flags)
    BUF	    *bp_from;
    BUF	    *bp_to;
    int	    flags;
{
    int	    should_copy = TRUE;
    char_u  *save_p_isk = NULL;	    /* init for GCC */

    /*
     * Don't do anything of the "to" buffer is invalid.
     */
    if (bp_to == NULL || !buf_valid(bp_to))
	return;

    /*
     * Only copy if the "from" buffer is valid and "to" and "from" are
     * different.
     */
    if (bp_from != NULL && buf_valid(bp_from) && bp_from != bp_to)
    {
	/*
	 * Always copy when entering and 'cpo' contains 'S'.
	 * Don't copy when already initialized.
	 * Don't copy when 'cpo' contains 's' and not entering.
	 * 'S'	BCO_ENTER  initialized	's'  should_copy
	 * yes	  yes	       X	 X	TRUE
	 * yes	  no	      yes	 X	FALSE
	 * no	   X	      yes	 X	FALSE
	 *  X	  no	      no	yes	FALSE
	 *  X	  no	      no	no	TRUE
	 * no	  yes	      no	 X	TRUE
	 */
	if ((vim_strchr(p_cpo, CPO_BUFOPTGLOB) == NULL || !(flags & BCO_ENTER))
		&& (bp_to->b_p_initialized
		    || (!(flags & BCO_ENTER)
			&& vim_strchr(p_cpo, CPO_BUFOPT) != NULL)))
	    should_copy = FALSE;

	if (should_copy || (flags & BCO_ALWAYS))
	{
	    if ((flags & BCO_NOHELP))		/* don't free b_p_isk */
	    {
		save_p_isk = bp_to->b_p_isk;
		bp_to->b_p_isk = NULL;
	    }
	    /*
	     * Always free the allocated strings.
	     * If not already initialized, set 'readonly' and copy 'fileformat'.
	     */
	    if (!bp_to->b_p_initialized)
	    {
		free_buf_options(bp_to, TRUE);
		bp_to->b_p_ro = FALSE;		/* don't copy readonly */
		bp_to->b_p_tx = bp_from->b_p_tx;
		bp_to->b_p_ff = vim_strsave(bp_from->b_p_ff);
	    }
	    else
		free_buf_options(bp_to, FALSE);

	    bp_to->b_p_ai = bp_from->b_p_ai;
	    bp_to->b_p_ai_save = bp_from->b_p_ai_save;
	    bp_to->b_p_sw = bp_from->b_p_sw;
	    bp_to->b_p_tw = bp_from->b_p_tw;
	    bp_to->b_p_tw_save = bp_from->b_p_tw_save;
	    bp_to->b_p_tw_nobin = bp_from->b_p_tw_nobin;
	    bp_to->b_p_wm = bp_from->b_p_wm;
	    bp_to->b_p_wm_save = bp_from->b_p_wm_save;
	    bp_to->b_p_wm_nobin = bp_from->b_p_wm_nobin;
	    bp_to->b_p_bin = bp_from->b_p_bin;
	    bp_to->b_p_et = bp_from->b_p_et;
	    bp_to->b_p_et_nobin = bp_from->b_p_et_nobin;
	    bp_to->b_p_ml = bp_from->b_p_ml;
	    bp_to->b_p_ml_nobin = bp_from->b_p_ml_nobin;
	    bp_to->b_p_inf = bp_from->b_p_inf;
#ifdef INSERT_EXPAND
	    bp_to->b_p_cpt = vim_strsave(bp_from->b_p_cpt);
#endif
	    bp_to->b_p_sts = bp_from->b_p_sts;
#ifndef SHORT_FNAME
	    bp_to->b_p_sn = bp_from->b_p_sn;
#endif
	    bp_to->b_p_com = vim_strsave(bp_from->b_p_com);
	    bp_to->b_p_fo = vim_strsave(bp_from->b_p_fo);
	    bp_to->b_p_nf = vim_strsave(bp_from->b_p_nf);
#ifdef SMARTINDENT
	    bp_to->b_p_si = bp_from->b_p_si;
	    bp_to->b_p_si_save = bp_from->b_p_si_save;
#endif
#ifdef CINDENT
	    bp_to->b_p_cin = bp_from->b_p_cin;
	    bp_to->b_p_cin_save = bp_from->b_p_cin_save;
	    bp_to->b_p_cink = vim_strsave(bp_from->b_p_cink);
	    bp_to->b_p_cino = vim_strsave(bp_from->b_p_cino);
#endif
#if defined(SMARTINDENT) || defined(CINDENT)
	    bp_to->b_p_cinw = vim_strsave(bp_from->b_p_cinw);
#endif
#ifdef LISPINDENT
	    bp_to->b_p_lisp = bp_from->b_p_lisp;
	    bp_to->b_p_lisp_save = bp_from->b_p_lisp_save;
#endif

	    /*
	     * Don't copy the options set by do_help(), use the saved values
	     * Don't touch these at all when BCO_NOHELP is used.
	     */
	    if ((flags & BCO_NOHELP))
		bp_to->b_p_isk = save_p_isk;
	    else
	    {
		if (!keep_help_flag && bp_from->b_help && !bp_to->b_help
						     && help_save_isk != NULL)
		{
		    bp_to->b_p_isk = vim_strsave(help_save_isk);
		    if (bp_to->b_p_isk != NULL)
			init_chartab();
		    bp_to->b_p_ts = help_save_ts;
		    bp_to->b_help = FALSE;
		}
		else
		{
		    bp_to->b_p_isk = vim_strsave(bp_from->b_p_isk);
		    vim_memmove(bp_to->b_chartab, bp_from->b_chartab,
								 (size_t)256);
		    bp_to->b_p_ts = bp_from->b_p_ts;
		    bp_to->b_help = bp_from->b_help;
		}
	    }
	}

	/*
	 * When the options should be copied (ignoring BCO_ALWAYS), set the
	 * flag that indicates that the options have been initialized.
	 */
	if (should_copy)
	    bp_to->b_p_initialized = TRUE;
    }

    check_buf_options(bp_to);	    /* make sure we don't have NULLs */
}


static int expand_option_idx = -1;
static char_u expand_option_name[5] = {'t', '_', NUL, NUL, NUL};

    void
set_context_in_set_cmd(arg)
    char_u *arg;
{
    int		nextchar;
    int		flags = 0;	/* init for GCC */
    int		opt_idx = 0;	/* init for GCC */
    char_u	*p;
    char_u	*after_blank = NULL;
    int		is_term_option = FALSE;
    int		key;

    expand_context = EXPAND_SETTINGS;
    if (*arg == NUL)
    {
	expand_pattern = arg;
	return;
    }
    p = arg + STRLEN(arg) - 1;
    if (*p == ' ' && *(p - 1) != '\\')
    {
	expand_pattern = p + 1;
	return;
    }
    while (p != arg && (*p != ' ' || *(p - 1) == '\\'))
    {
	/* remember possible start of file name to expand */
	if ((*p == ' ' || (*p == ',' && *(p - 1) != '\\')) &&
							  after_blank == NULL)
	    after_blank = p + 1;
	p--;
    }
    if (p != arg)
	p++;
    if (STRNCMP(p, "no", 2) == 0)
    {
	expand_context = EXPAND_BOOL_SETTINGS;
	p += 2;
    }
    if (STRNCMP(p, "inv", 3) == 0)
    {
	expand_context = EXPAND_BOOL_SETTINGS;
	p += 3;
    }
    expand_pattern = arg = p;
    if (*arg == '<')
    {
	while (*p != '>')
	    if (*p++ == NUL)	    /* expand terminal option name */
		return;
	key = get_special_key_code(arg + 1);
	if (key == 0)		    /* unknown name */
	{
	    expand_context = EXPAND_NOTHING;
	    return;
	}
	nextchar = *++p;
	is_term_option = TRUE;
	expand_option_name[2] = KEY2TERMCAP0(key);
	expand_option_name[3] = KEY2TERMCAP1(key);
    }
    else
    {
	if (p[0] == 't' && p[1] == '_')
	{
	    p += 2;
	    if (*p != NUL)
		++p;
	    if (*p == NUL)
		return;		/* expand option name */
	    nextchar = *++p;
	    is_term_option = TRUE;
	    expand_option_name[2] = p[-2];
	    expand_option_name[3] = p[-1];
	}
	else
	{
	    while (isalnum(*p) || *p == '_' || *p == '*')   /* Allow * wildcard */
		p++;
	    if (*p == NUL)
		return;
	    nextchar = *p;
	    *p = NUL;
	    opt_idx = findoption(arg);
	    *p = nextchar;
	    if (opt_idx == -1 || options[opt_idx].var == NULL)
	    {
		expand_context = EXPAND_NOTHING;
		return;
	    }
	    flags = options[opt_idx].flags;
	    if (flags & P_BOOL)
	    {
		expand_context = EXPAND_NOTHING;
		return;
	    }
	}
    }
    if ((nextchar != '=' && nextchar != ':')
				    || expand_context == EXPAND_BOOL_SETTINGS)
    {
	expand_context = EXPAND_UNSUCCESSFUL;
	return;
    }
    if (expand_context != EXPAND_BOOL_SETTINGS && p[1] == NUL)
    {
	expand_context = EXPAND_OLD_SETTING;
	if (is_term_option)
	    expand_option_idx = -1;
	else
	    expand_option_idx = opt_idx;
	expand_pattern = p + 1;
	return;
    }
    expand_context = EXPAND_NOTHING;
    if (is_term_option || (flags & P_NUM))
	return;
    if (after_blank != NULL)
	expand_pattern = after_blank;
    else
	expand_pattern = p + 1;
    if (flags & P_EXPAND)
    {
	p = options[opt_idx].var;
	if (p == (char_u *)&p_bdir || p == (char_u *)&p_dir ||
						       p == (char_u *)&p_path)
	    expand_context = EXPAND_DIRECTORIES;
	else
	    expand_context = EXPAND_FILES;
    }
    return;
}

    int
ExpandSettings(prog, num_file, file)
    vim_regexp	*prog;
    int		*num_file;
    char_u	***file;
{
    int num_normal = 0;	    /* Number of matching non-term-code settings */
    int num_term = 0;	    /* Number of matching terminal code settings */
    int opt_idx;
    int match;
    int count = 0;
    char_u *str;
    int	loop;
    int is_term_opt;
    char_u  name_buf[MAX_KEY_NAME_LEN];
    int	save_reg_ic;

    /* do this loop twice:
     * loop == 0: count the number of matching options
     * loop == 1: copy the matching options into allocated memory
     */
    for (loop = 0; loop <= 1; ++loop)
    {
	if (expand_context != EXPAND_BOOL_SETTINGS)
	{
	    if (vim_regexec(prog, (char_u *)"all", TRUE))
	    {
		if (loop == 0)
		    num_normal++;
		else
		    (*file)[count++] = vim_strsave((char_u *)"all");
	    }
	    if (vim_regexec(prog, (char_u *)"termcap", TRUE))
	    {
		if (loop == 0)
		    num_normal++;
		else
		    (*file)[count++] = vim_strsave((char_u *)"termcap");
	    }
	}
	for (opt_idx = 0; (str = (char_u *)options[opt_idx].fullname) != NULL;
								    opt_idx++)
	{
	    if (options[opt_idx].var == NULL)
		continue;
	    if (expand_context == EXPAND_BOOL_SETTINGS
	      && !(options[opt_idx].flags & P_BOOL))
		continue;
	    is_term_opt = istermoption(&options[opt_idx]);
	    if (is_term_opt && num_normal > 0)
		continue;
	    match = FALSE;
	    if (vim_regexec(prog, str, TRUE) ||
					(options[opt_idx].shortname != NULL &&
			 vim_regexec(prog,
				 (char_u *)options[opt_idx].shortname, TRUE)))
		match = TRUE;
	    else if (is_term_opt)
	    {
		name_buf[0] = '<';
		name_buf[1] = 't';
		name_buf[2] = '_';
		name_buf[3] = str[2];
		name_buf[4] = str[3];
		name_buf[5] = '>';
		name_buf[6] = NUL;
		if (vim_regexec(prog, name_buf, TRUE))
		{
		    match = TRUE;
		    str = name_buf;
		}
	    }
	    if (match)
	    {
		if (loop == 0)
		{
		    if (is_term_opt)
			num_term++;
		    else
			num_normal++;
		}
		else
		    (*file)[count++] = vim_strsave(str);
	    }
	}
	/*
	 * Check terminal key codes, these are not in the option table
	 */
	if (expand_context != EXPAND_BOOL_SETTINGS  && num_normal == 0)
	{
	    for (opt_idx = 0; (str = get_termcode(opt_idx)) != NULL; opt_idx++)
	    {
		if (!isprint(str[0]) || !isprint(str[1]))
		    continue;

		name_buf[0] = 't';
		name_buf[1] = '_';
		name_buf[2] = str[0];
		name_buf[3] = str[1];
		name_buf[4] = NUL;

		match = FALSE;
		if (vim_regexec(prog, name_buf, TRUE))
		    match = TRUE;
		else
		{
		    name_buf[0] = '<';
		    name_buf[1] = 't';
		    name_buf[2] = '_';
		    name_buf[3] = str[0];
		    name_buf[4] = str[1];
		    name_buf[5] = '>';
		    name_buf[6] = NUL;

		    if (vim_regexec(prog, name_buf, TRUE))
			match = TRUE;
		}
		if (match)
		{
		    if (loop == 0)
			num_term++;
		    else
			(*file)[count++] = vim_strsave(name_buf);
		}
	    }
	    /*
	     * Check special key names.
	     */
	    for (opt_idx = 0; (str = get_key_name(opt_idx)) != NULL; opt_idx++)
	    {
		name_buf[0] = '<';
		STRCPY(name_buf + 1, str);
		STRCAT(name_buf, ">");

		save_reg_ic = reg_ic;
		reg_ic = TRUE;			/* ignore case here */
		if (vim_regexec(prog, name_buf, TRUE))
		{
		    if (loop == 0)
			num_term++;
		    else
			(*file)[count++] = vim_strsave(name_buf);
		}
		reg_ic = save_reg_ic;
	    }
	}
	if (loop == 0)
	{
	    if (num_normal > 0)
		*num_file = num_normal;
	    else if (num_term > 0)
		*num_file = num_term;
	    else
		return OK;
	    *file = (char_u **) alloc((unsigned)(*num_file * sizeof(char_u *)));
	    if (*file == NULL)
	    {
		*file = (char_u **)"";
		return FAIL;
	    }
	}
    }
    return OK;
}

    int
ExpandOldSetting(num_file, file)
    int	    *num_file;
    char_u  ***file;
{
    char_u  *var = NULL;	/* init for GCC */
    char_u  *buf;

    *num_file = 0;
    *file = (char_u **)alloc((unsigned)sizeof(char_u *));
    if (*file == NULL)
	return FAIL;

    /*
     * For a terminal key code expand_option_idx is < 0.
     */
    if (expand_option_idx < 0)
    {
	var = find_termcode(expand_option_name + 2);
	if (var == NULL)
	    expand_option_idx = findoption(expand_option_name);
    }

    if (expand_option_idx >= 0)
    {
	/* put string of option value in NameBuff */
	option_value2string(&options[expand_option_idx]);
	var = NameBuff;
    }
    else if (var == NULL)
	var = (char_u *)"";

    /* A backslash is required before some characters */
    buf = vim_strsave_escaped(var, escape_chars);

    if (buf == NULL)
    {
	vim_free(*file);
	*file = NULL;
	return FAIL;
    }

    *file[0] = buf;
    *num_file = 1;
    return OK;
}

/*
 * Get the value for the numeric or string option *opp in a nice format into
 * NameBuff[].  Must not be called with a hidden option!
 */
    static void
option_value2string(opp)
    struct vimoption	*opp;
{
    char_u  *varp;

    varp = get_varp(opp);
    if (opp->flags & P_NUM)
    {
	if ((long *)varp == &p_wc)
	{
	    if (IS_SPECIAL(p_wc) || find_special_key_in_table((int)p_wc) >= 0)
		STRCPY(NameBuff, get_special_key_name((int)p_wc, 0));
	    else
		STRCPY(NameBuff, transchar((int)p_wc));
	}
	else
	    sprintf((char *)NameBuff, "%ld", *(long *)varp);
    }
    else    /* P_STRING */
    {
	varp = *(char_u **)(varp);
	if (varp == NULL)		    /* just in case */
	    NameBuff[0] = NUL;
	else if (opp->flags & P_EXPAND)
	    home_replace(NULL, varp, NameBuff, MAXPATHL);
	else
	    STRNCPY(NameBuff, varp, MAXPATHL);
    }
}

#ifdef HAVE_LANGMAP
/*
 * Any character has an equivalent character.  This is used for keyboards that
 * have a special language mode that sends characters above 128 (although
 * other characters can be translated too).
 */

/*
 * char_u langmap_mapchar[256];
 * Normally maps each of the 128 upper chars to an <128 ascii char; used to
 * "translate" native lang chars in normal mode or some cases of
 * insert mode without having to tediously switch lang mode back&forth.
 */

    static void
langmap_init()
{
    int i;

    for (i = 0; i < 256; i++)		/* we init with a-one-to one map */
	langmap_mapchar[i] = i;
}

/*
 * Called when langmap option is set; the language map can be
 * changed at any time!
 */
    static void
langmap_set()
{
    char_u  *p;
    char_u  *p2;
    int	    from, to;

    langmap_init();			    /* back to one-to-one map first */

    for (p = p_langmap; p[0]; )
    {
	for (p2 = p; p2[0] && p2[0] != ',' && p2[0] != ';'; ++p2)
	    if (p2[0] == '\\' && p2[1])
		++p2;
	if (p2[0] == ';')
	    ++p2;	    /* abcd;ABCD form, p2 points to A */
	else
	    p2 = NULL;	    /* aAbBcCdD form, p2 is NULL */
	while (p[0])
	{
	    if (p[0] == '\\' && p[1])
		++p;
	    from = p[0];
	    if (p2 == NULL)
	    {
		if (p[1] == '\\')
		    ++p;
		to = p[1];
	    }
	    else
	    {
		if (p2[0] == '\\')
		    ++p2;
		to = p2[0];
	    }
	    if (to == NUL)
	    {
		EMSG2("'langmap': Matching character missing for %s",
							     transchar(from));
		return;
	    }
	    langmap_mapchar[from] = to;

	    /* Advance to next pair */
	    if (p2 == NULL)
	    {
		p += 2;
		if (p[0] == ',')
		{
		    ++p;
		    break;
		}
	    }
	    else
	    {
		++p;
		++p2;
		if (*p == ';')
		{
		    p = p2;
		    if (p[0])
		    {
			if (p[0] != ',')
			{
			    EMSG2("'langmap': Extra characters after semicolon: %s", p);
			    return;
			}
			++p;
		    }
		    break;
		}
	    }
	}
    }
}
#endif

/*
 * Return TRUE if format option 'x' is in effect.
 * Take care of no formatting when 'paste' is set.
 */
    int
has_format_option(x)
    int	    x;
{
    if (p_paste)
	return FALSE;
    return (vim_strchr(curbuf->b_p_fo, x) != NULL);
}

/*
 * Return TRUE if "x" is present in 'shortmess' option, or
 * 'shortmess' contains 'a' and "x" is present in SHM_A.
 */
    int
shortmess(x)
    int	    x;
{
    return (   vim_strchr(p_shm, x) != NULL
	    || (vim_strchr(p_shm, 'a') != NULL
		&& vim_strchr((char_u *)SHM_A, x) != NULL));
}

/*
 * set_paste_option() - Called after p_paste was set or reset.
 */
    static void
paste_option_changed()
{
    static int	    old_p_paste = FALSE;
    static int	    save_sm = 0;
    static int	    save_ru = 0;
#ifdef RIGHTLEFT
    static int	    save_ri = 0;
    static int	    save_hkmap = 0;
#endif
    BUF		    *buf;

    if (p_paste)
    {
	/*
	 * Paste switched from off to on.
	 * Save the current values, so they can be restored later.
	 */
	if (!old_p_paste)
	{
	    /* save options for each buffer */
	    for (buf = firstbuf; buf != NULL; buf = buf->b_next)
	    {
		buf->b_p_tw_save = buf->b_p_tw;
		buf->b_p_wm_save = buf->b_p_wm;
		buf->b_p_sts_save = buf->b_p_sts;
		buf->b_p_ai_save = buf->b_p_ai;
#ifdef SMARTINDENT
		buf->b_p_si_save = buf->b_p_si;
#endif
#ifdef CINDENT
		buf->b_p_cin_save = buf->b_p_cin;
#endif
#ifdef LISPINDENT
		buf->b_p_lisp_save = buf->b_p_lisp;
#endif
	    }

	    /* save global options */
	    save_sm = p_sm;
	    save_ru = p_ru;
#ifdef RIGHTLEFT
	    save_ri = p_ri;
	    save_hkmap = p_hkmap;
#endif
	}

	/*
	 * Always set the option values, also when 'paste' is set when it is
	 * already on.
	 */
	/* set options for each buffer */
	for (buf = firstbuf; buf != NULL; buf = buf->b_next)
	{
	    buf->b_p_tw = 0;	    /* textwidth is 0 */
	    buf->b_p_wm = 0;	    /* wrapmargin is 0 */
	    buf->b_p_sts = 0;	    /* softtabstop is 0 */
	    buf->b_p_ai = 0;	    /* no auto-indent */
#ifdef SMARTINDENT
	    buf->b_p_si = 0;	    /* no smart-indent */
#endif
#ifdef CINDENT
	    buf->b_p_cin = 0;	    /* no c indenting */
#endif
#ifdef LISPINDENT
	    buf->b_p_lisp = 0;	    /* no lisp indenting */
#endif
	}

	/* set global options */
	p_sm = 0;		    /* no showmatch */
	p_ru = 0;		    /* no ruler */
#ifdef RIGHTLEFT
	p_ri = 0;		    /* no reverse insert */
	p_hkmap = 0;		    /* no Hebrew keyboard */
#endif
    }

    /*
     * Paste switched from on to off: Restore saved values.
     */
    else if (old_p_paste)
    {
	/* restore options for each buffer */
	for (buf = firstbuf; buf != NULL; buf = buf->b_next)
	{
	    buf->b_p_tw = buf->b_p_tw_save;
	    buf->b_p_wm = buf->b_p_wm_save;
	    buf->b_p_sts = buf->b_p_sts_save;
	    buf->b_p_ai = buf->b_p_ai_save;
#ifdef SMARTINDENT
	    buf->b_p_si = buf->b_p_si_save;
#endif
#ifdef CINDENT
	    buf->b_p_cin = buf->b_p_cin_save;
#endif
#ifdef LISPINDENT
	    buf->b_p_lisp = buf->b_p_lisp_save;
#endif
	}

	/* restore global options */
	p_sm = save_sm;
	p_ru = save_ru;
#ifdef RIGHTLEFT
	p_ri = save_ri;
	p_hkmap = save_hkmap;
#endif
    }

    old_p_paste = p_paste;
}

/*
 * vimrc_found() - Called when a ".vimrc" or "VIMINIT" has been found.
 *
 * Reset 'compatible' and set the values for options that didn't get set yet
 * to the Vim defaults.
 * Don't do this if the 'compatible' option has been set or reset before.
 */
    void
vimrc_found()
{
    int	    opt_idx;

    if (!(options[findoption((char_u *)"cp")].flags & P_WAS_SET))
    {
	p_cp = FALSE;
	for (opt_idx = 0; !istermoption(&options[opt_idx]); opt_idx++)
	    if (!(options[opt_idx].flags & (P_WAS_SET|P_VI_DEF)))
		set_option_default(opt_idx, TRUE);
	init_chartab();		    /* make b_p_isk take effect */
    }
}

/*
 * Set 'compatible' on or off.  Called for "-C" and "-N" command line arg.
 */
    void
change_compatible(on)
    int	    on;
{
    if (p_cp != on)
    {
	p_cp = on;
	compatible_set();
    }
    options[findoption((char_u *)"cp")].flags |= P_WAS_SET;
}

/*
 * compatible_set() - Called when 'compatible' has been set or unset.
 *
 * When 'compatible' set: Set all relevant options (those that have the P_VIM)
 * flag) to a Vi compatible value.
 * When 'compatible' is unset: Set all options that have a different default
 * for Vim (without the P_VI_DEF flag) to that default.
 */
    static void
compatible_set()
{
    int	    opt_idx;

    for (opt_idx = 0; !istermoption(&options[opt_idx]); opt_idx++)
	if (	   ((options[opt_idx].flags & P_VIM) && p_cp)
		|| (!(options[opt_idx].flags & P_VI_DEF) && !p_cp))
	    set_option_default(opt_idx, TRUE);
    init_chartab();		    /* make b_p_isk take effect */
}

/*
 * fill_breakat_flags() -- called when 'breakat' changes value.
 */
    static void
fill_breakat_flags()
{
    char_u	*c;
    int		i;

    for (i = 0; i < 256; i++)
	breakat_flags[i] = FALSE;

    if (p_breakat != NULL)
	for (c = p_breakat; *c; c++)
	    breakat_flags[*c] = TRUE;
}

/*
 * Check an option that can be a range of string values.
 *
 * Return OK for correct value, FAIL otherwise.
 */
    static int
check_opt_strings(val, values, list)
    char_u  *val;
    char    **values;
    int	    list;	    /* when TRUE: accept a list of values */
{
    int	    i;
    int	    len;

    while (*val)
    {
	for (i = 0; ; ++i)
	{
	    if (values[i] == NULL)	/* val not found in values[] */
		return FAIL;

	    len = STRLEN(values[i]);
	    if (STRNCMP(values[i], val, len) == 0)
	    {
		if (list && val[len] == ',')
		{
		    val += len + 1;
		    break;		/* check next item in val list */
		}
		else if (val[len] == NUL)
		    return OK;
	    }
	}
    }
    return OK;
}
