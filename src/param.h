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
#define P_DIR			4		/* directory for autoscript file */
#define P_EB			5		/* ring bell for errors */
#define P_EF			6		/* name of errofile */
#define P_ET			7		/* expand tabs into spaces */
#define P_HI			8		/* command line history size */
#define P_IC			9		/* ignore case in searches */
#define P_JS			10		/* use two spaces after period with Join */
#define P_LI			11		/* lines */
#define P_LS			12		/* show tabs and newlines graphically */
#define P_MAGIC 		13		/* use some characters for pattern matching */
#define P_ML			14		/* number of mode lines */
#define P_NU			15		/* number lines on the screen */
#define P_PARA			16		/* paragraphs */
#define P_RO			17		/* readonly */
#define P_REMAP			18		/* remap */
#define P_RD			19		/* delete when replacing */
#define P_RP			20		/* minimum number of lines for report */
#define P_SS			21		/* scroll size */
#define P_SECTIONS		22		/* sections */
#define P_SHELL 		23		/* name of shell to use */
#define P_ST			24		/* type of shell */
#define P_SR			25		/* shift round off (for < and >) */
#define P_SW			26		/* shiftwidth (for < and >) */
#define P_SC			27		/* show command in status line */
#define P_SM			28		/* showmatch */
#define P_MO			29		/* show mode */
#define P_SI			30		/* smart-indent for c programs */
#define P_SU			31		/* suffixes for wildcard expansion */
#define P_TS			32		/* tab size in the file */
#define P_TL			33		/* used tag length */
#define P_TAGS			34		/* tags search path */
#define P_TE			35		/* terse (not used) */
#define P_TW			36		/* textwidth */
#define P_TO			37		/* tilde is an operator */
#define P_UL			38		/* number of Undo Levels */
#define P_UC			39		/* update count for auto script file */
#define P_UT			40		/* update time for auto script file */
#define P_VB			41		/* visual bell only (no beep) */
#define P_WARN			42		/* warn for changes at shell command */
#define P_WS			43		/* wrap scan */
#define P_WM			44		/* wrapmargin */
#define P_WA			45		/* write any */
#define P_YE			46		/* Y yanks to end of line */

/*
 * Macro to get the value of a parameter
 */
#define P(n)	((params[n].val.intval))
#define PS(n)	(params[n].val.strval)
