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
 * param.h: data definitions for settable parameters
 */

struct param
{
	char		*fullname;		/* full parameter name */
	char		*shortname; 	/* permissible abbreviation */
	union
	{
		long	intval; 		/* integer parameter value */
		char	*strval;		/* string parameter value */
	} val;
	int 		flags;
};

extern struct param params[];

/*
 * Flags
 */
#define P_BOOL			0x01	/* the parameter is boolean */
#define P_NUM			0x02	/* the parameter is numeric */
#define P_CHANGED		0x04	/* the parameter has been changed */
#define P_STRING		0x08	/* the parameter is a string */

/*
 * The following are the indices in the params array for each parameter
 */

#define P_AI			0		/* auto-indent */
#define P_AW			1		/* auto-write */
#define P_BS			2		/* backspace over newlines in insert mode */
#define P_BK			3		/* make backups when writing out files */
#define P_EB			4		/* ring bell for errors */
#define P_EF			5		/* name of errofile */
#define P_HI			6		/* command line history size */
#define P_IC			7		/* ignore case in searches */
#define P_LI			8		/* lines */
#define P_LS			9		/* show tabs and newlines graphically */
#define P_MAGIC 		10		/* use some characters for pattern matching */
#define P_ML			11		/* number of mode lines */
#define P_NU			12		/* number lines on the screen */
#define P_PARA			13		/* paragraphs */
#define P_RO			14		/* readonly */
#define P_REMAP			15		/* remap */
#define P_RP			16		/* minimum number of lines for report */
#define P_SS			17		/* scroll size */
#define P_SECTIONS		18		/* sections */
#define P_SHELL 		19		/* name of shell to use */
#define P_SW			20		/* shiftwidth (for < and >) */
#define P_SM			21		/* showmatch */
#define P_MO			22		/* show mode */
#define P_SI			23		/* smart-indent for c programs */
#define P_SU			24		/* suffixes for wildcard expansion */
#define P_TS			25		/* tab size in the file */
#define P_TL			26		/* used tag length */
#define P_TAGS			27		/* tags search path */
#define P_TE			28		/* terse (not used) */
#define P_TW			29		/* textwidth */
#define P_TO			30		/* tilde is an operator */
#define P_UL			31		/* number of Undo Levels */
#define P_UC			32		/* update count for auto script file */
#define P_UT			33		/* update time for auto script file */
#define P_WARN			34		/* warn for changes at shell command */
#define P_WS			35		/* wrap scan */
#define P_WM			36		/* wrapmargin */
#define P_WA			37		/* write any */
#define P_YE			38		/* Y yanks to end of line */

/*
 * Macro to get the value of a parameter
 */
#define P(n)	((params[n].val.intval))
#define PS(n)	(params[n].val.strval)
