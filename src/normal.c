/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Contains the main routine for processing characters in command mode.
 * Communicates closely with the code in ops.c to handle the operators.
 */

#include "vim.h"

#ifdef FEAT_VISUAL
/*
 * The Visual area is remembered for reselection.
 */
static int	resel_VIsual_mode = NUL;	/* 'v', 'V', or Ctrl-V */
static linenr_t	resel_VIsual_line_count;	/* number of lines */
static colnr_t	resel_VIsual_col;		/* nr of cols or end col */

static int	restart_VIsual_select = 0;
#endif

static int
# ifdef __BORLANDC__
    _RTLENTRYF
# endif
		nv_compare __ARGS((const void *s1, const void *s2));
static int	find_command __ARGS((int cmdchar));
static void	op_colon __ARGS((oparg_t *oap));
#if defined(FEAT_MOUSE) && defined(FEAT_VISUAL)
static void	find_start_of_word __ARGS((pos_t *));
static void	find_end_of_word __ARGS((pos_t *));
static int	get_mouse_class __ARGS((int));
#endif
static void	prep_redo_cmd __ARGS((cmdarg_t *cap));
static void	prep_redo __ARGS((int regname, long, int, int, int, int, int));
static int	checkclearop __ARGS((oparg_t *oap));
static int	checkclearopq __ARGS((oparg_t *oap));
static void	clearop __ARGS((oparg_t *oap));
static void	clearopbeep __ARGS((oparg_t *oap));
#ifdef FEAT_VISUAL
static void	unshift_special __ARGS((cmdarg_t *cap));
#endif
#ifdef FEAT_CMDL_INFO
static void	del_from_showcmd __ARGS((int));
#endif

/*
 * nv_*(): functions called to handle Normal and Visual mode commands.
 * n_*(): functions called to handle Normal mode commands.
 * v_*(): functions called to handle Visual mode commands.
 */
static void	nv_ignore __ARGS((cmdarg_t *cap));
static void	nv_error __ARGS((cmdarg_t *cap));
static void	nv_help __ARGS((cmdarg_t *cap));
static void	nv_addsub __ARGS((cmdarg_t *cap));
static void	nv_page __ARGS((cmdarg_t *cap));
static void	nv_gd __ARGS((oparg_t *oap, int nchar));
static int	nv_screengo __ARGS((oparg_t *oap, int dir, long dist));
#ifdef FEAT_MOUSE
static void	nv_mousescroll __ARGS((cmdarg_t *cap));
static void	nv_mouse __ARGS((cmdarg_t *cap));
#endif
static void	nv_scroll_line __ARGS((cmdarg_t *cap));
static void	nv_zet __ARGS((cmdarg_t *cap));
#ifdef FEAT_GUI
static void	nv_ver_scrollbar __ARGS((cmdarg_t *cap));
static void	nv_hor_scrollbar __ARGS((cmdarg_t *cap));
#endif
static void	nv_exmode __ARGS((cmdarg_t *cap));
static void	nv_colon __ARGS((cmdarg_t *cap));
static void	nv_ctrlg __ARGS((cmdarg_t *cap));
static void	nv_ctrlh __ARGS((cmdarg_t *cap));
static void	nv_clear __ARGS((cmdarg_t *cap));
static void	nv_ctrlo __ARGS((cmdarg_t *cap));
static void	nv_hat __ARGS((cmdarg_t *cap));
static void	nv_Zet __ARGS((cmdarg_t *cap));
static void	nv_ident __ARGS((cmdarg_t *cap));
#ifdef FEAT_VISUAL
static int	get_visual_text __ARGS((cmdarg_t *cap, char_u **pp, int *lenp));
#endif
static void	nv_tagpop __ARGS((cmdarg_t *cap));
static void	nv_scroll __ARGS((cmdarg_t *cap));
static void	nv_kright __ARGS((cmdarg_t *cap));
static void	nv_right __ARGS((cmdarg_t *cap));
static void	nv_kleft __ARGS((cmdarg_t *cap));
static void	nv_left __ARGS((cmdarg_t *cap));
static void	nv_up __ARGS((cmdarg_t *cap));
static void	nv_down __ARGS((cmdarg_t *cap));
#ifdef FEAT_SEARCHPATH
static void	nv_gotofile __ARGS((cmdarg_t *cap));
#endif
static void	nv_end __ARGS((cmdarg_t *cap));
static void	nv_dollar __ARGS((cmdarg_t *cap));
static void	nv_search __ARGS((cmdarg_t *cap));
static void	nv_next __ARGS((cmdarg_t *cap));
static void	nv_csearch __ARGS((cmdarg_t *cap));
static void	nv_brackets __ARGS((cmdarg_t *cap));
static void	nv_percent __ARGS((cmdarg_t *cap));
static void	nv_brace __ARGS((cmdarg_t *cap));
static void	nv_mark __ARGS((cmdarg_t *cap));
static void	nv_findpar __ARGS((cmdarg_t *cap));
static void	nv_undo __ARGS((cmdarg_t *cap));
static void	nv_kundo __ARGS((cmdarg_t *cap));
static void	nv_Replace __ARGS((cmdarg_t *cap));
static void	nv_VReplace __ARGS((cmdarg_t *cap));
static void	nv_vreplace __ARGS((cmdarg_t *cap));
#ifdef FEAT_VISUAL
static void	v_swap_corners __ARGS((cmdarg_t *cap));
#endif
static void	nv_replace __ARGS((cmdarg_t *cap));
static void	n_swapchar __ARGS((cmdarg_t *cap));
static void	nv_cursormark __ARGS((cmdarg_t *cap, int flag, pos_t *pos));
#ifdef FEAT_VISUAL
static void	v_visop __ARGS((cmdarg_t *cap));
#endif
static void	nv_subst __ARGS((cmdarg_t *cap));
static void	nv_abbrev __ARGS((cmdarg_t *cap));
static void	nv_optrans __ARGS((cmdarg_t *cap));
static void	nv_gomark __ARGS((cmdarg_t *cap));
static void	nv_pcmark __ARGS((cmdarg_t *cap));
static void	nv_regname __ARGS((cmdarg_t *cap));
#ifdef FEAT_VISUAL
static void	nv_visual __ARGS((cmdarg_t *cap));
static void	n_start_visual_mode __ARGS((int c));
#endif
static void	nv_window __ARGS((cmdarg_t *cap));
static void	nv_suspend __ARGS((cmdarg_t *cap));
static void	nv_g_cmd __ARGS((cmdarg_t *cap));
static void	n_opencmd __ARGS((cmdarg_t *cap));
static void	nv_dot __ARGS((cmdarg_t *cap));
static void	nv_redo __ARGS((cmdarg_t *cap));
static void	nv_Undo __ARGS((cmdarg_t *cap));
static void	nv_tilde __ARGS((cmdarg_t *cap));
static void	nv_operator __ARGS((cmdarg_t *cap));
static void	nv_lineop __ARGS((cmdarg_t *cap));
static void	nv_home __ARGS((cmdarg_t *cap));
static void	nv_pipe __ARGS((cmdarg_t *cap));
static void	nv_bck_word __ARGS((cmdarg_t *cap));
static void	nv_wordcmd __ARGS((cmdarg_t *cap));
static void	nv_beginline __ARGS((cmdarg_t *cap));
#ifdef FEAT_VISUAL
static void	adjust_for_sel __ARGS((cmdarg_t *cap));
static void	unadjust_for_sel __ARGS((void));
static void	nv_select __ARGS((cmdarg_t *cap));
#endif
static void	nv_goto __ARGS((cmdarg_t *cap));
static void	nv_normal __ARGS((cmdarg_t *cap));
static void	nv_esc __ARGS((cmdarg_t *oap));
static void	nv_edit __ARGS((cmdarg_t *cap));
#ifdef FEAT_TEXTOBJ
static void	nv_object __ARGS((cmdarg_t *cap));
#endif
static void	nv_record __ARGS((cmdarg_t *cap));
static void	nv_at __ARGS((cmdarg_t *cap));
static void	nv_halfpage __ARGS((cmdarg_t *cap));
static void	nv_join __ARGS((cmdarg_t *cap));
static void	nv_put __ARGS((cmdarg_t *cap));
static void	nv_open __ARGS((cmdarg_t *cap));
#ifdef FEAT_SNIFF
static void	nv_sniff __ARGS((cmdarg_t *cap));
#endif

/*
 * Function to be called for a Normal or Visual mode command.
 * The argument is a cmdarg_t.
 */
typedef void (*nv_func_t) __ARGS((cmdarg_t *cap));

/* Values for cmd_flags. */
#define NV_NCH	    0x01	  /* may need to get a second char */
#define NV_NCH_NOP  (0x02|NV_NCH) /* get second char when no operator pending */
#define NV_NCH_ALW  (0x04|NV_NCH) /* always get a second char */
#define NV_LANG	    0x08	/* second char needs language adjustment */

#define NV_SS	    0x10	/* may start selection */
#define NV_SSS	    0x20	/* may start selection with shift modifier */
#define NV_STS	    0x40	/* may stop selection without shift modif. */
#define NV_RL	    0x80	/* 'rightleft' modifies command */
#define NV_KEEPREG  0x100	/* don't clear regname */
#define NV_NCW	    0x200	/* not allowed in command-line window */

/*
 * Generally speaking, every Normal mode command should either clear any
 * pending operator (with *clearop*()), or set the motion type variable
 * oap->motion_type.
 *
 * When a cursor motion command is made, it is marked as being a character or
 * line oriented motion.  Then, if an operator is in effect, the operation
 * becomes character or line oriented accordingly.
 */

/*
 * This table contains one entry for every Normal or Visual mode command.
 * The order doesn't matter, init_normal_cmds() will create a sorted index.
 * It is faster when all keys from zero to '~' are present.
 */
struct nv_cmd
{
    int		cmd_char;	/* (first) command character */
    nv_func_t   cmd_func;	/* function for this command */
    short_u	cmd_flags;	/* NV_ flags */
    short	cmd_arg;	/* value for ca.arg */
} nv_cmds[] =
{
    {NUL,	nv_error,	0,			0},
    {Ctrl_A,	nv_addsub,	0,			0},
    {Ctrl_B,	nv_page,	NV_STS,			BACKWARD},
    {Ctrl_C,	nv_esc,		0,			TRUE},
    {Ctrl_D,	nv_halfpage,	0,			0},
    {Ctrl_E,	nv_scroll_line,	0,			TRUE},
    {Ctrl_F,	nv_page,	NV_STS,			FORWARD},
    {Ctrl_G,	nv_ctrlg,	0,			0},
    {Ctrl_H,	nv_ctrlh,	0,			0},
    {Ctrl_I,	nv_pcmark,	0,			0},
    {NL,	nv_down,	0,			FALSE},
    {Ctrl_K,	nv_error,	0,			0},
    {Ctrl_L,	nv_clear,	0,			0},
    {CR,	nv_down,	0,			TRUE},
    {Ctrl_N,	nv_down,	NV_STS,			FALSE},
    {Ctrl_O,	nv_ctrlo,	0,			0},
    {Ctrl_P,	nv_up,		NV_STS,			FALSE},
    {Ctrl_Q,	nv_ignore,	0,			0},
    {Ctrl_R,	nv_redo,	0,			0},
    {Ctrl_S,	nv_ignore,	0,			0},
    {Ctrl_T,	nv_tagpop,	NV_NCW,			0},
    {Ctrl_U,	nv_halfpage,	0,			0},
#ifdef FEAT_VISUAL
    {Ctrl_V,	nv_visual,	0,			FALSE},
    {'V',	nv_visual,	0,			FALSE},
    {'v',	nv_visual,	0,			FALSE},
#else
    {Ctrl_V,	nv_error,	0,			0},
    {'V',	nv_error,	0,			0},
    {'v',	nv_error,	0,			0},
#endif
    {Ctrl_W,	nv_window,	0,			0},
    {Ctrl_X,	nv_addsub,	0,			0},
    {Ctrl_Y,	nv_scroll_line,	0,			FALSE},
    {Ctrl_Z,	nv_suspend,	0,			0},
    {ESC,	nv_esc,		0,			FALSE},
    {Ctrl_BSL,	nv_normal,	0,			0},
    {Ctrl_RSB,	nv_ident,	NV_NCW,			0},
    {Ctrl_HAT,	nv_hat,		NV_NCW,			0},
    {Ctrl__,	nv_error,	0,			0},
    {' ',	nv_right,	0,			0},
    {'!',	nv_operator,	0,			0},
    {'"',	nv_regname,	NV_NCH_NOP|NV_KEEPREG,	0},
    {'#',	nv_ident,	0,			0},
    {'$',	nv_dollar,	0,			0},
    {'%',	nv_percent,	0,			0},
    {'&',	nv_optrans,	0,			0},
    {'\'',	nv_gomark,	NV_NCH_ALW,		TRUE},
    {'(',	nv_brace,	0,			BACKWARD},
    {')',	nv_brace,	0,			FORWARD},
    {'*',	nv_ident,	0,			0},
    {'+',	nv_down,	0,			TRUE},
    {',',	nv_csearch,	0,			TRUE},
    {'-',	nv_up,		0,			TRUE},
    {'.',	nv_dot,		NV_KEEPREG,		0},
    {'/',	nv_search,	0,			FALSE},
    {'0',	nv_beginline,	0,			0},
    {'1',	nv_ignore,	0,			0},
    {'2',	nv_ignore,	0,			0},
    {'3',	nv_ignore,	0,			0},
    {'4',	nv_ignore,	0,			0},
    {'5',	nv_ignore,	0,			0},
    {'6',	nv_ignore,	0,			0},
    {'7',	nv_ignore,	0,			0},
    {'8',	nv_ignore,	0,			0},
    {'9',	nv_ignore,	0,			0},
    {':',	nv_colon,	0,			0},
    {';',	nv_csearch,	0,			FALSE},
    {'<',	nv_operator,	NV_RL,			0},
    {'=',	nv_operator,	0,			0},
    {'>',	nv_operator,	NV_RL,			0},
    {'?',	nv_search,	0,			FALSE},
    {'@',	nv_at,		NV_NCH_NOP,		FALSE},
    {'A',	nv_edit,	0,			0},
    {'B',	nv_bck_word,	0,			1},
    {'C',	nv_abbrev,	NV_KEEPREG,		0},
    {'D',	nv_abbrev,	NV_KEEPREG,		0},
    {'E',	nv_wordcmd,	0,			TRUE},
    {'F',	nv_csearch,	NV_NCH_ALW|NV_LANG,	BACKWARD},
    {'G',	nv_goto,	0,			TRUE},
    {'H',	nv_scroll,	0,			0},
    {'I',	nv_edit,	0,			0},
    {'J',	nv_join,	0,			0},
    {'K',	nv_ident,	0,			0},
    {'L',	nv_scroll,	0,			0},
    {'M',	nv_scroll,	0,			0},
    {'N',	nv_next,	0,			SEARCH_REV},
    {'O',	nv_open,	0,			0},
    {'P',	nv_put,		0,			0},
    {'Q',	nv_exmode,	NV_NCW,			0},
    {'R',	nv_Replace,	0,			0},
    {'S',	nv_subst,	NV_KEEPREG,		0},
    {'T',	nv_csearch,	NV_NCH_ALW|NV_LANG,	BACKWARD},
    {'U',	nv_Undo,	0,			0},
    {'W',	nv_wordcmd,	0,			TRUE},
    {'X',	nv_abbrev,	NV_KEEPREG,		0},
    {'Y',	nv_abbrev,	NV_KEEPREG,		0},
    {'Z',	nv_Zet,		NV_NCH_NOP|NV_NCW,	0},
    {'[',	nv_brackets,	NV_NCH_ALW,		BACKWARD},
    {'\\',	nv_error,	0,			0},
    {']',	nv_brackets,	NV_NCH_ALW,		FORWARD},
    {'^',	nv_beginline,	0,			BL_WHITE | BL_FIX},
    {'_',	nv_lineop,	0,			0},
    {'`',	nv_gomark,	NV_NCH_ALW,		FALSE},
    {'a',	nv_edit,	NV_NCH,			0},
    {'b',	nv_bck_word,	0,			0},
    {'c',	nv_operator,	0,			0},
    {'d',	nv_operator,	0,			0},
    {'e',	nv_wordcmd,	0,			FALSE},
    {'f',	nv_csearch,	NV_NCH_ALW|NV_LANG,	FORWARD},
    {'g',	nv_g_cmd,	NV_NCH_ALW,		FALSE},
    {'h',	nv_left,	NV_RL,			0},
    {'i',	nv_edit,	NV_NCH,			0},
    {'j',	nv_down,	0,			FALSE},
    {'k',	nv_up,		0,			FALSE},
    {'l',	nv_right,	NV_RL,			0},
    {'m',	nv_mark,	NV_NCH_NOP,		0},
    {'n',	nv_next,	0,			0},
    {'o',	nv_open,	0,			0},
    {'p',	nv_put,		0,			0},
    {'q',	nv_record,	NV_NCH,			0},
    {'r',	nv_replace,	NV_NCH_NOP|NV_LANG,	0},
    {'s',	nv_subst,	NV_KEEPREG,		0},
    {'t',	nv_csearch,	NV_NCH_ALW|NV_LANG,	FORWARD},
    {'u',	nv_undo,	0,			0},
    {'w',	nv_wordcmd,	0,			FALSE},
    {'x',	nv_abbrev,	NV_KEEPREG,		0},
    {'y',	nv_operator,	0,			0},
    {'z',	nv_zet,		NV_NCH_NOP,		0},
    {'{',	nv_findpar,	0,			BACKWARD},
    {'|',	nv_pipe,	0,			0},
    {'}',	nv_findpar,	0,			FORWARD},
    {'~',	nv_tilde,	0,			0},

    /* pound sign */
    {POUND,	nv_ident,	0,			0},
#ifdef FEAT_MOUSE
    {K_MOUSEUP, nv_mousescroll,	0,			TRUE},
    {K_MOUSEDOWN, nv_mousescroll, 0,			FALSE},
    {K_LEFTMOUSE, nv_mouse,	0,			0},
    {K_LEFTMOUSE_NM, nv_mouse,	0,			0},
    {K_LEFTDRAG, nv_mouse,	0,			0},
    {K_LEFTRELEASE, nv_mouse,	0,			0},
    {K_LEFTRELEASE_NM, nv_mouse, 0,			0},
    {K_MIDDLEMOUSE, nv_mouse,	0,			0},
    {K_MIDDLEDRAG, nv_mouse,	0,			0},
    {K_MIDDLERELEASE, nv_mouse,	0,			0},
    {K_RIGHTMOUSE, nv_mouse,	0,			0},
    {K_RIGHTDRAG, nv_mouse,	0,			0},
    {K_RIGHTRELEASE, nv_mouse,	0,			0},
    {K_IGNORE,	nv_ignore,	0,			0},
#endif
    {K_INS,	nv_edit,	0,			0},
    {K_KINS,	nv_edit,	0,			0},
    {K_BS,	nv_ctrlh,	0,			0},
    {K_UP,	nv_up,		NV_SSS|NV_STS,		FALSE},
    {K_S_UP,	nv_page,	NV_SS,			BACKWARD},
    {K_DOWN,	nv_down,	NV_SSS|NV_STS,		FALSE},
    {K_S_DOWN,	nv_page,	NV_SS,			FORWARD},
    {K_LEFT,	nv_kleft,	NV_SSS|NV_STS|NV_RL,	0},
    {K_S_LEFT,	nv_bck_word,	NV_SS|NV_RL,		0},
    {K_RIGHT,	nv_kright,	NV_SSS|NV_STS|NV_RL,	0},
    {K_S_RIGHT,	nv_wordcmd,	NV_SS|NV_RL,		FALSE},
    {K_PAGEUP,	nv_page,	NV_SSS|NV_STS,		BACKWARD},
    {K_KPAGEUP,	nv_page,	NV_SSS|NV_STS,		BACKWARD},
    {K_PAGEDOWN, nv_page,	NV_SSS|NV_STS,		FORWARD},
    {K_KPAGEDOWN, nv_page,	NV_SSS|NV_STS,		FORWARD},
    {K_END,	nv_end,		NV_SSS|NV_STS,		0},
    {K_KEND,	nv_end,		NV_SSS|NV_STS,		0},
    {K_XEND,	nv_end,		NV_SSS|NV_STS,		0},
    {K_S_END,	nv_end,		NV_SS,			0},
    {K_HOME,	nv_home,	NV_SSS|NV_STS,		0},
    {K_KHOME,	nv_home,	NV_SSS|NV_STS,		0},
    {K_XHOME,	nv_home,	NV_SSS|NV_STS,		0},
    {K_S_HOME,	nv_home,	NV_SS,			0},
    {K_DEL,	nv_abbrev,	0,			0},
    {K_KDEL,	nv_abbrev,	0,			0},
    {K_UNDO,	nv_kundo,	0,			0},
    {K_HELP,	nv_help,	NV_NCW,			0},
    {K_F1,	nv_help,	NV_NCW,			0},
    {K_XF1,	nv_help,	NV_NCW,			0},
#ifdef FEAT_VISUAL
    {K_SELECT,	nv_select,	0,			0},
#endif
#ifdef FEAT_GUI
    {K_VER_SCROLLBAR, nv_ver_scrollbar, 0,		0},
    {K_HOR_SCROLLBAR, nv_hor_scrollbar, 0,		0},
#endif
#ifdef FEAT_FKMAP
    {K_F8,	farsi_fkey,	0,			0},
    {K_F9,	farsi_fkey,	0,			0},
#endif
#ifdef FEAT_SNIFF
    {K_SNIFF,	nv_sniff,	0,			0},
#endif
};

/* Number of commands in nv_cmds[]. */
#define NV_CMDS_SIZE (sizeof(nv_cmds) / sizeof(struct nv_cmd))

/* Sorted index of commands in nv_cmds[]. */
static short nv_cmd_idx[NV_CMDS_SIZE];

/* The highest index for which
 * nv_cmds[idx].cmd_char == nv_cmd_idx[nv_cmds[idx].cmd_char] */
static int nv_max_linear;

/*
 * Compare functions for qsort() below, that checks the command character
 * through the index in nv_cmd_idx[].
 */
    static int
#ifdef __BORLANDC__
_RTLENTRYF
#endif
nv_compare(s1, s2)
    const void	*s1;
    const void	*s2;
{
    int		c1, c2;

    /* The commands are sorted on absolute value. */
    c1 = nv_cmds[*(const short *)s1].cmd_char;
    c2 = nv_cmds[*(const short *)s2].cmd_char;
    if (c1 < 0)
	c1 = -c1;
    if (c2 < 0)
	c2 = -c2;
    return c1 - c2;
}

/*
 * Initialize the nv_cmd_idx[] table.
 */
    void
init_normal_cmds()
{
    int		i;

    /* Fill the index table with a one to one relation. */
    for (i = 0; i < NV_CMDS_SIZE; ++i)
	nv_cmd_idx[i] = i;

    /* Sort the commands by the command character.  */
    qsort((void *)&nv_cmd_idx, (size_t)NV_CMDS_SIZE, sizeof(short), nv_compare);

    /* Find the first entry that can't be indexed by the command character. */
    for (i = 0; i < NV_CMDS_SIZE; ++i)
	if (i != nv_cmds[nv_cmd_idx[i]].cmd_char)
	    break;
    nv_max_linear = i - 1;
}

/*
 * Search for a command in the commands table.
 * Returns -1 for invalid command.
 */
    static int
find_command(cmdchar)
    int		cmdchar;
{
    int		i;
    int		idx;
    int		top, bot;
    int		c;

#ifdef FEAT_MBYTE
    /* A multi-byte character is never a command. */
    if (cmdchar >= 0x100)
	return -1;
#endif

    /* We use the absolute value of the character.  Special keys have a
     * negative value, but are sorted on their absolute value. */
    if (cmdchar < 0)
	cmdchar = -cmdchar;

    /* If the character is in the first part: The character is the index into
     * nv_cmd_idx[]. */
    if (cmdchar <= nv_max_linear)
	return nv_cmd_idx[cmdchar];

    /* Perform a binary search. */
    bot = nv_max_linear + 1;
    top = NV_CMDS_SIZE - 1;
    idx = -1;
    while (bot <= top)
    {
	i = (top + bot) / 2;
	c = nv_cmds[nv_cmd_idx[i]].cmd_char;
	if (c < 0)
	    c = -c;
	if (cmdchar == c)
	{
	    idx = nv_cmd_idx[i];
	    break;
	}
	if (cmdchar > c)
	    bot = i + 1;
	else
	    top = i - 1;
    }
    return idx;
}

/*
 * Execute a command in Normal mode.
 */
/*ARGSUSED*/
    void
normal_cmd(oap, toplevel)
    oparg_t	*oap;
    int		toplevel;		/* TRUE when called from main() */
{
    static long	opcount = 0;		/* ca.opcount saved here */
    cmdarg_t	ca;			/* command arguments */
    int		c;
    int		ctrl_w = FALSE;		/* got CTRL-W command */
    int		old_col = curwin->w_curswant;
#ifdef FEAT_CMDL_INFO
    int		need_flushbuf;		/* need to call out_flush() */
#endif
#ifdef FEAT_VISUAL
    pos_t	old_pos;		/* cursor position before command */
    int		mapped_len;
#endif
    static int	old_mapped_len = 0;
    int		idx;

    vim_memset(&ca, 0, sizeof(ca));	/* also resets ca.retval */
    ca.oap = oap;
    ca.opcount = opcount;

#ifdef FEAT_SCROLLBIND
    do_check_scrollbind(FALSE);
#endif
#ifdef FEAT_SNIFF
    want_sniff_request = sniff_connected;
#endif

    /*
     * If there is an operator pending, then the command we take this time
     * will terminate it. Finish_op tells us to finish the operation before
     * returning this time (unless the operation was cancelled).
     */
#ifdef CURSOR_SHAPE
    c = finish_op;
#endif
    finish_op = (oap->op_type != OP_NOP);
#ifdef CURSOR_SHAPE
    if (finish_op != c)
	ui_cursor_shape();		/* may show different cursor shape */
#endif

    if (!finish_op && !oap->regname)
	ca.opcount = 0;

#ifdef FEAT_VISUAL
    mapped_len = typebuf_maplen();
#endif

    State = NORMAL_BUSY;
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = FALSE;	/* allow scrolling here */
#endif

    /*
     * Get the command character from the user.
     */
#if defined(WIN32) && defined(FEAT_SNIFF)
    if (sniff_request_waiting)
    {
	c = K_SNIFF;
	sniff_request_waiting = 0;
	want_sniff_request = 0;
    }
    else
#endif
	c = safe_vgetc();

#ifdef FEAT_LANGMAP
    LANGMAP_ADJUST(c, TRUE);
#endif

    /*
     * If a mapping was started in Visual or Select mode, remember the length
     * of the mapping.  This is used below to not return to Insert mode for as
     * long as the mapping is being executed.
     */
    if (!restart_edit)
	old_mapped_len = 0;
    else if (old_mapped_len
#ifdef FEAT_VISUAL
	    || (VIsual_active && mapped_len == 0 && typebuf_maplen() > 0)
#endif
	    )
	old_mapped_len = typebuf_maplen();

    if (c == NUL)
	c = K_ZERO;

#ifdef FEAT_VISUAL
    /*
     * In Select mode, typed text replaces the selection.
     */
    if (VIsual_active
	    && VIsual_select
	    && (vim_isprintc(c) || c == NL || c == CR || c == K_KENTER))
    {
	stuffcharReadbuff(c);
	c = 'c';
    }
#endif

#ifdef FEAT_CMDL_INFO
    need_flushbuf = add_to_showcmd(c);
#endif

getcount:
#ifdef FEAT_VISUAL
    if (!(VIsual_active && VIsual_select))
#endif
    {
	/*
	 * Handle a count before a command and compute ca.count0.
	 * Note that '0' is a command and not the start of a count, but it's
	 * part of a count after other digits.
	 */
	while (    (c >= '1' && c <= '9')
		|| (ca.count0 != 0 && (c == K_DEL || c == K_KDEL || c == '0')))
	{
	    if (c == K_DEL || c == K_KDEL)
	    {
		ca.count0 /= 10;
#ifdef FEAT_CMDL_INFO
		del_from_showcmd(4);	/* delete the digit and ~@% */
#endif
	    }
	    else
		ca.count0 = ca.count0 * 10 + (c - '0');
	    if (ca.count0 < 0)	    /* got too large! */
		ca.count0 = 999999999L;
	    if (ctrl_w)
	    {
		++no_mapping;
		++allow_keys;		/* no mapping for nchar, but keys */
	    }
	    c = safe_vgetc();
#ifdef FEAT_LANGMAP
	    LANGMAP_ADJUST(c, TRUE);
#endif
	    if (ctrl_w)
	    {
		--no_mapping;
		--allow_keys;
	    }
#ifdef FEAT_CMDL_INFO
	    need_flushbuf |= add_to_showcmd(c);
#endif
	}

	/*
	 * If we got CTRL-W there may be a/another count
	 */
	if (c == Ctrl_W && !ctrl_w && oap->op_type == OP_NOP)
	{
	    ctrl_w = TRUE;
	    ca.opcount = ca.count0;	/* remember first count */
	    ca.count0 = 0;
	    ++no_mapping;
	    ++allow_keys;		/* no mapping for nchar, but keys */
	    c = safe_vgetc();		/* get next character */
#ifdef FEAT_LANGMAP
	    LANGMAP_ADJUST(c, TRUE);
#endif
	    --no_mapping;
	    --allow_keys;
#ifdef FEAT_CMDL_INFO
	    need_flushbuf |= add_to_showcmd(c);
#endif
	    goto getcount;		/* jump back */
	}
    }

    /*
     * If we're in the middle of an operator (including after entering a yank
     * buffer with '"') AND we had a count before the operator, then that
     * count overrides the current value of ca.count0.
     * What this means effectively, is that commands like "3dw" get turned
     * into "d3w" which makes things fall into place pretty neatly.
     * If you give a count before AND after the operator, they are multiplied.
     */
    if (ca.opcount != 0)
    {
	if (ca.count0)
	    ca.count0 *= ca.opcount;
	else
	    ca.count0 = ca.opcount;
    }

    /*
     * Always remember the count.  It will be set to zero (on the next call,
     * above) when there is no pending operator.
     * When called from main(), save the count for use by the "count" built-in
     * variable.
     */
    ca.opcount = ca.count0;
    ca.count1 = (ca.count0 == 0 ? 1 : ca.count0);

#ifdef FEAT_EVAL
    /*
     * Only set v:count when called from main() and not a stuffed command.
     */
    if (toplevel && stuff_empty())
	set_vcount(ca.count0, ca.count1);
#endif

    /*
     * Find the command character in the table of commands.
     * For CTRL-W we already got nchar when looking for a count.
     */
    if (ctrl_w)
    {
	ca.nchar = c;
	ca.cmdchar = Ctrl_W;
    }
    else
	ca.cmdchar = c;
    idx = find_command(ca.cmdchar);
    if (idx < 0)
    {
	/* Not a known command: beep. */
	clearopbeep(oap);
	goto normal_end;
    }
#ifdef FEAT_CMDWIN
    if (cmdwin_type != 0 && (nv_cmds[idx].cmd_flags & NV_NCW))
    {
	/* This command is not allowed in the cmdline window: beep. */
	clearopbeep(oap);
	EMSG(_(e_cmdwin));
	goto normal_end;
    }
#endif

#ifdef FEAT_VISUAL
    /*
     * In Visual/Select mode, a few keys are handled in a special way.
     */
    if (VIsual_active)
    {
	/* when 'keymodel' contains "stopsel" may stop Select/Visual mode */
	if (km_stopsel
		&& (nv_cmds[idx].cmd_flags & NV_STS)
		&& !(mod_mask & MOD_MASK_SHIFT))
	{
	    end_visual_mode();
	    redraw_curbuf_later(INVERTED);
	}

	/* Keys that work different when 'keymodel' contains "startsel" */
	if (km_startsel)
	{
	    if (nv_cmds[idx].cmd_flags & NV_SS)
	    {
		unshift_special(&ca);
		idx = find_command(ca.cmdchar);
	    }
	    else if ((nv_cmds[idx].cmd_flags & NV_SSS)
					       && (mod_mask & MOD_MASK_SHIFT))
	    {
		mod_mask &= ~MOD_MASK_SHIFT;
	    }
	}
    }
#endif

#ifdef FEAT_RIGHTLEFT
    if (curwin->w_p_rl && KeyTyped && (nv_cmds[idx].cmd_flags & NV_RL))
    {
	/* invert horizontal movements and operations */
	switch (ca.cmdchar)
	{
	    case 'l':	    ca.cmdchar = 'h'; break;
	    case K_RIGHT:   ca.cmdchar = K_LEFT; break;
	    case K_S_RIGHT: ca.cmdchar = K_S_LEFT; break;
	    case 'h':	    ca.cmdchar = 'l'; break;
	    case K_LEFT:    ca.cmdchar = K_RIGHT; break;
	    case K_S_LEFT:  ca.cmdchar = K_S_RIGHT; break;
	    case '>':	    ca.cmdchar = '<'; break;
	    case '<':	    ca.cmdchar = '>'; break;
	}
	idx = find_command(ca.cmdchar);
    }
#endif

    /*
     * Get an additional character if we need one.
     */
    if ((nv_cmds[idx].cmd_flags & NV_NCH)
	    && (((nv_cmds[idx].cmd_flags & NV_NCH_NOP) == NV_NCH_NOP
		    && oap->op_type == OP_NOP)
		|| (nv_cmds[idx].cmd_flags & NV_NCH_ALW) == NV_NCH_ALW
		|| (ca.cmdchar == 'q'
		    && oap->op_type == OP_NOP
		    && !Recording
		    && !Exec_reg)
		|| ((ca.cmdchar == 'a' || ca.cmdchar == 'i')
		    && (oap->op_type != OP_NOP
#ifdef FEAT_VISUAL
			|| VIsual_active
#endif
			))))
    {
	int	*cp;
#if (defined(FEAT_GUI_W32) && defined(FEAT_MBYTE_IME)) || defined(CURSOR_SHAPE)
	int	repl = FALSE;	/* get character for replace mode */
#endif
	int	lit = FALSE;	/* get extra character literally */

	++no_mapping;
	++allow_keys;		/* no mapping for nchar, but allow key codes */
	if (ca.cmdchar == 'g')
	{
	    /*
	     * For 'g' get the next character now, so that we can check for
	     * "gr", "g'" and "g`".
	     */
	    ca.nchar = safe_vgetc();
#ifdef FEAT_CMDL_INFO
	    need_flushbuf |= add_to_showcmd(ca.nchar);
#endif
	    if (ca.nchar == 'r' || ca.nchar == '\'' || ca.nchar == '`')
	    {
		cp = &ca.extra_char;	/* need to get a third character */
		if (ca.nchar != 'r')
		    lit = TRUE;			/* get it literally */
#if (defined(FEAT_GUI_W32) && defined(FEAT_MBYTE_IME)) || defined(CURSOR_SHAPE)
		else
		    repl = TRUE;		/* get it in replace mode */
#endif
	    }
	    else
		cp = NULL;		/* no third character needed */
	}
	else
	{
#if (defined(FEAT_GUI_W32) && defined(FEAT_MBYTE_IME)) || defined(CURSOR_SHAPE)
	    if (ca.cmdchar == 'r')		/* get it in replace mode */
		repl = TRUE;
#endif
	    cp = &ca.nchar;
	}

	/*
	 * Get a second or third character.
	 */
	if (cp != NULL)
	{
#ifdef CURSOR_SHAPE
	    if (repl)
	    {
		State = REPLACE;	/* pretend Replace mode */
		ui_cursor_shape();	/* show different cursor shape */
	    }
#endif
#if defined(FEAT_GUI_W32) && defined(FEAT_MBYTE_IME)
	    if (repl)
		ImeSetOriginMode();
#endif
	    *cp = safe_vgetc();
#if defined(FEAT_GUI_W32) && defined(FEAT_MBYTE_IME)
	    if (repl)
		ImeSetEnglishMode();
#endif
#ifdef CURSOR_SHAPE
	    State = NORMAL_BUSY;
#endif
#ifdef FEAT_CMDL_INFO
	    need_flushbuf |= add_to_showcmd(*cp);
#endif

	    if (!lit)
	    {
#ifdef FEAT_DIGRAPHS
		/* Typing CTRL-K gets a digraph. */
		if (*cp == Ctrl_K
			&& ((nv_cmds[idx].cmd_flags & NV_LANG)
			    || cp == &ca.extra_char)
			&& vim_strchr(p_cpo, CPO_DIGRAPH) == NULL)
		{
		    c = get_digraph(FALSE);
		    if (c > 0)
		    {
			*cp = c;
# ifdef FEAT_CMDL_INFO
			/* Guessing how to update showcmd here... */
			del_from_showcmd(3);
			need_flushbuf |= add_to_showcmd(*cp);
# endif
		    }
		}
#endif

#ifdef FEAT_LANGMAP
		/* adjust chars > 127, except after "tTfFr" commands */
		LANGMAP_ADJUST(*cp, !((nv_cmds[idx].cmd_flags & NV_LANG)
						    || cp == &ca.extra_char));
#endif
#ifdef FEAT_RIGHTLEFT
		/* adjust Hebrew mapped char */
		if (p_hkmap
			&& ((nv_cmds[idx].cmd_flags & NV_LANG)
						      || cp == &ca.extra_char)
			&& KeyTyped)
		    *cp = hkmap(*cp);
# ifdef FEAT_FKMAP
		/* adjust Farsi mapped char */
		if (p_fkmap
			&& ((nv_cmds[idx].cmd_flags & NV_LANG)
						      || cp == &ca.extra_char)
			&& KeyTyped)
		    *cp = fkmap(*cp);
# endif
#endif
	    }
	}
	--no_mapping;
	--allow_keys;
    }

#ifdef FEAT_CMDL_INFO
    /*
     * Flush the showcmd characters onto the screen so we can see them while
     * the command is being executed.  Only do this when the shown command was
     * actually displayed, otherwise this will slow down a lot when executing
     * mappings.
     */
    if (need_flushbuf)
	out_flush();
#endif

    State = NORMAL;

    if (ca.nchar == ESC)
    {
	clearop(oap);
	if (p_im && !restart_edit)
	    restart_edit = 'a';
	goto normal_end;
    }

    msg_didout = FALSE;	    /* don't scroll screen up for normal command */
    msg_col = 0;

#ifdef FEAT_VISUAL
    old_pos = curwin->w_cursor;		/* remember where cursor was */

    /* When 'keymodel' contains "startsel" some keys start Select/Visual
     * mode. */
    if (!VIsual_active && km_startsel)
    {
	if (nv_cmds[idx].cmd_flags & NV_SS)
	{
	    start_selection();
	    unshift_special(&ca);
	    idx = find_command(ca.cmdchar);
	}
	else if ((nv_cmds[idx].cmd_flags & NV_SSS)
					   && (mod_mask & MOD_MASK_SHIFT))
	{
	    start_selection();
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
    }
#endif

    /*
     * Execute the command!
     * Call the command function found in the commands table.
     */
    ca.arg = nv_cmds[idx].cmd_arg;
    (nv_cmds[idx].cmd_func)(&ca);

    /*
     * If we didn't start or finish an operator, reset oap->regname, unless we
     * need it later.
     */
    if (!finish_op
	    && !oap->op_type
	    && (idx < 0 || !(nv_cmds[idx].cmd_flags & NV_KEEPREG)))
    {
	oap->regname = 0;
	oap->motion_force = NUL;
    }

    /*
     * If an operation is pending, handle it...
     */
    do_pending_operator(&ca, old_col, FALSE);

    /*
     * Wait for a moment when a message is displayed that will be overwritten
     * by the mode message.
     * In Visual mode and with "^O" in Insert mode, a short message will be
     * overwritten by the mode message.  Wait a bit, until a key is hit.
     * In Visual mode, it's more important to keep the Visual area updated
     * than keeping a message (e.g. from a /pat search).
     * Only do this if the command was typed, not from a mapping.
     * Also wait a bit after an error message, e.g. for "^O:".
     * Don't redraw the screen, it would remove the message.
     */
    if (       ((p_smd
		    && (restart_edit
#ifdef FEAT_VISUAL
			|| (VIsual_active
			    && old_pos.lnum == curwin->w_cursor.lnum
			    && old_pos.col == curwin->w_cursor.col)
#endif
			)
		    && (clear_cmdline
			|| redraw_cmdline)
		    && msg_didany
		    && !msg_nowait
		    && KeyTyped)
		|| (restart_edit
#ifdef FEAT_VISUAL
		    && !VIsual_active
#endif
		    && (msg_scroll
			|| emsg_on_display)))
	    && oap->regname == 0
	    && !(ca.retval & CA_COMMAND_BUSY)
	    && stuff_empty()
	    && typebuf_typed()
	    && oap->op_type == OP_NOP)
    {
	int	save_State = State;

	/* Draw the cursor with the right shape here */
	if (restart_edit)
	    State = INSERT;

	/* If need to redraw, and there is a "keep_msg", redraw before the
	 * delay */
	if (must_redraw && keep_msg != NULL && !emsg_on_display)
	{
	    char_u	*kmsg = keep_msg;

	    /* showmode() will clear keep_msg, but we want to use it anyway */
	    update_screen(0);
	    msg_attr(kmsg, keep_msg_attr);
	}
	setcursor();
	cursor_on();
	out_flush();
	if (msg_scroll || emsg_on_display)
	    ui_delay(1000L, TRUE);	/* wait at least one second */
	ui_delay(3000L, FALSE);		/* wait up to three seconds */
	State = save_State;

	msg_scroll = FALSE;
	emsg_on_display = FALSE;
    }

    /*
     * Finish up after executing a Normal mode command.
     */
normal_end:

    msg_nowait = FALSE;

    /* Reset finish_op, in case it was set */
#ifdef CURSOR_SHAPE
    c = finish_op;
#endif
    finish_op = FALSE;
#ifdef CURSOR_SHAPE
    /* Redraw the cursor with another shape, if we were in Operator-pending
     * mode or did a replace command. */
    if (c || ca.cmdchar == 'r')
	ui_cursor_shape();		/* may show different cursor shape */
#endif

#ifdef FEAT_CMDL_INFO
    if (oap->op_type == OP_NOP && oap->regname == 0)
	clear_showcmd();
#endif

    checkpcmark();		/* check if we moved since setting pcmark */
    vim_free(ca.searchbuf);

#ifdef FEAT_SCROLLBIND
    if (curwin->w_p_scb)
    {
	validate_cursor();	/* may need to update w_leftcol */
	do_check_scrollbind(TRUE);
    }
#endif

    /*
     * May restart edit(), if we got here with CTRL-O in Insert mode (but not
     * if still inside a mapping that started in Visual mode).
     * May switch from Visual to Select mode after CTRL-O command.
     */
    if (       oap->op_type == OP_NOP
#ifdef FEAT_VISUAL
	    && ((restart_edit && !VIsual_active && old_mapped_len == 0)
		|| restart_VIsual_select == 1)
#endif
	    && !(ca.retval & CA_COMMAND_BUSY)
	    && stuff_empty()
	    && oap->regname == 0)
    {
#ifdef FEAT_VISUAL
	if (restart_VIsual_select == 1)
	{
	    VIsual_select = TRUE;
	    showmode();
	    restart_VIsual_select = 0;
	}
#endif
	if (restart_edit
#ifdef FEAT_VISUAL
		&& !VIsual_active
#endif
		&& old_mapped_len == 0)
	    (void)edit(restart_edit, FALSE, 1L);
    }

#ifdef FEAT_VISUAL
    if (restart_VIsual_select == 2)
	restart_VIsual_select = 1;
#endif

#ifdef FEAT_MBYTE
    if (has_mbyte)
	mb_adjust_cursor();
#endif

    /* Save count before an operator for next time. */
    opcount = ca.opcount;
}

/*
 * Handle an operator after visual mode or when the movement is finished
 */
    void
do_pending_operator(cap, old_col, gui_yank)
    cmdarg_t	*cap;
    int		old_col;
    int		gui_yank;
{
    oparg_t	*oap = cap->oap;
    pos_t	old_cursor;
#ifdef FEAT_VIRTUALEDIT
    int		old_coladd;
#endif
    int		empty_region_error;

#ifdef FEAT_VISUAL
    /* The visual area is remembered for redo */
    static int	    redo_VIsual_mode = NUL; /* 'v', 'V', or Ctrl-V */
    static linenr_t redo_VIsual_line_count; /* number of lines */
    static colnr_t  redo_VIsual_col;	    /* number of cols or end column */
    static long	    redo_VIsual_count;	    /* count for Visual operator */
#endif

#if defined(FEAT_CLIPBOARD)
    /*
     * Yank the visual area into the GUI selection register before we operate
     * on it and lose it forever.
     * Don't do it if a specific register was specified, so that ""x"*P works.
     * This could call do_pending_operator() recursively, but that's OK
     * because gui_yank will be TRUE for the nested call.
     */
    if (clipboard.available
	    && oap->op_type != OP_NOP
	    && !gui_yank
#ifdef FEAT_VISUAL
	    && VIsual_active
	    && !redo_VIsual_busy
#endif
	    && oap->regname == 0)
	clip_auto_select();
#endif
    old_cursor = curwin->w_cursor;
#ifdef FEAT_VIRTUALEDIT
    old_coladd = curwin->w_coladd;
#endif

    /*
     * If an operation is pending, handle it...
     */
    if ((finish_op
#ifdef FEAT_VISUAL
		|| VIsual_active
#endif
		) && oap->op_type != OP_NOP)
    {
#ifdef FEAT_VISUAL
	oap->is_VIsual = VIsual_active;
	if (oap->motion_force == 'V')
	    oap->motion_type = MLINE;
	else if (oap->motion_force == 'v')
	{
	    /* If the motion was linewise, "inclusive" will not have been set.
	     * Use "exclusive" to be consistent.  Makes "dvj" work nice. */
	    if (oap->motion_type == MLINE)
		oap->inclusive = FALSE;
	    oap->motion_type = MCHAR;
	}
	else if (oap->motion_force == Ctrl_V)
	{
	    /* Change linewise or characterwise motion into Visual block mode.
	     * */
	    VIsual_active = TRUE;
	    VIsual = oap->start;
	    VIsual_mode = Ctrl_V;
	    VIsual_select = FALSE;
	    VIsual_reselect = FALSE;
	}
#endif

#ifdef FEAT_VIRTUALEDIT
       /* If virtual editing is ON, we have to make sure the cursor position
	* is identical with the text position. */
	if (ve_flags == VE_ALL
		&& !VIsual_active
		&& curwin->w_coladd
		&& oap->motion_type != MLINE
		&& (oap->op_type == OP_DELETE
		    || oap->op_type == OP_JOIN_NS
		    || oap->op_type == OP_CHANGE
		    || oap->op_type == OP_INSERT
		    || oap->op_type == OP_APPEND
		    || oap->op_type == OP_REPLACE))
	{
	    u_save_cursor();
	    coladvance_force(getviscol());
	}
#endif

	/* only redo yank when 'y' flag is in 'cpoptions' */
	/* never redo "zf" (define fold) */
	if ((vim_strchr(p_cpo, CPO_YANK) != NULL || oap->op_type != OP_YANK)
#ifdef FEAT_VISUAL
		&& (!VIsual_active || oap->motion_force)
#endif
#ifdef FEAT_FOLDING
		&& oap->op_type != OP_FOLD
		&& oap->op_type != OP_FOLDOPEN
		&& oap->op_type != OP_FOLDOPENREC
		&& oap->op_type != OP_FOLDCLOSE
		&& oap->op_type != OP_FOLDCLOSEREC
#endif
		)
	{
	    prep_redo(oap->regname, cap->count0,
		    get_op_char(oap->op_type), get_extra_op_char(oap->op_type),
		    oap->motion_force, cap->cmdchar, cap->nchar);
	    if (cap->cmdchar == '/' || cap->cmdchar == '?') /* was a search */
	    {
		/*
		 * If 'cpoptions' does not contain 'r', insert the search
		 * pattern to really repeat the same command.
		 */
		if (vim_strchr(p_cpo, CPO_REDO) == NULL)
		    AppendToRedobuff(cap->searchbuf);
		AppendToRedobuff(NL_STR);
	    }
	}

#ifdef FEAT_VISUAL
	if (redo_VIsual_busy)
	{
	    oap->start = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual_line_count - 1;
	    if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	    VIsual_mode = redo_VIsual_mode;
	    if (VIsual_mode == 'v')
	    {
		if (redo_VIsual_line_count <= 1)
		    curwin->w_cursor.col += redo_VIsual_col - 1;
		else
		    curwin->w_cursor.col = redo_VIsual_col;
	    }
	    if (redo_VIsual_col == MAXCOL)
	    {
		curwin->w_curswant = MAXCOL;
		coladvance((colnr_t)MAXCOL);
	    }
	    cap->count0 = redo_VIsual_count;
	    if (redo_VIsual_count != 0)
		cap->count1 = redo_VIsual_count;
	    else
		cap->count1 = 1;
	}
	else if (VIsual_active)
	{
	    /* In Select mode, a linewise selection is operated upon like a
	     * characterwise selection. */
	    if (VIsual_select && VIsual_mode == 'V')
	    {
		if (lt(VIsual, curwin->w_cursor))
		{
		    VIsual.col = 0;
		    curwin->w_cursor.col =
					STRLEN(ml_get(curwin->w_cursor.lnum));
		}
		else
		{
		    curwin->w_cursor.col = 0;
		    VIsual.col = STRLEN(ml_get(VIsual.lnum));
		}
		VIsual_mode = 'v';
	    }
	    /* If 'selection' is "exclusive", backup one character for
	     * charwise selections. */
	    else if (VIsual_mode == 'v')
		unadjust_for_sel();

# ifdef FEAT_VIRTUALEDIT
	    curbuf->b_visual_start_coladd = VIsual_coladd;
	    curbuf->b_visual_end_coladd = curwin->w_coladd;
# endif
	    /* Save the current VIsual area for '< and '> marks, and "gv" */
	    curbuf->b_visual_start = VIsual;
	    curbuf->b_visual_end = curwin->w_cursor;
	    curbuf->b_visual_mode = VIsual_mode;

	    oap->start = VIsual;
	    if (VIsual_mode == 'V')
		oap->start.col = 0;
	}
#endif /* FEAT_VISUAL */

	/*
	 * Set oap->start to the first position of the operated text, oap->end
	 * to the end of the operated text.  w_cursor is equal to oap->start.
	 */
	if (lt(oap->start, curwin->w_cursor))
	{
#ifdef FEAT_FOLDING
	    /* When operating linewise, include folded lines completely. */
	    if (!VIsual_active && oap->motion_type == MLINE)
	    {
		hasFolding(oap->start.lnum, &oap->start.lnum, NULL);
		hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum);
	    }
#endif
	    oap->end = curwin->w_cursor;
	    curwin->w_cursor = oap->start;
	}
	else
	{
#ifdef FEAT_FOLDING
	    /* When operating linewise, include folded lines completely. */
	    if (!VIsual_active && oap->motion_type == MLINE)
	    {
		foldAdjustCursor();
		hasFolding(oap->start.lnum, NULL, &oap->start.lnum);
	    }
#endif
	    oap->end = oap->start;
	    oap->start = curwin->w_cursor;

#ifdef FEAT_VIRTUALEDIT
	    {
		/* swap the two coladds */
		colnr_t tmp = curbuf->b_visual_start_coladd;
		curbuf->b_visual_start_coladd = curbuf->b_visual_end_coladd;
		curbuf->b_visual_end_coladd = tmp;
	    }
#endif
	}

	oap->line_count = oap->end.lnum - oap->start.lnum + 1;

#ifdef FEAT_VISUAL
	if (VIsual_active || redo_VIsual_busy)
	{
	    if (VIsual_mode == Ctrl_V)	/* block mode */
	    {
		colnr_t	    start, end;

		oap->block_mode = TRUE;

		getvcol(curwin, &(oap->start),
				      &oap->start_vcol, NULL, &oap->end_vcol);
		if (!redo_VIsual_busy)
		{
		    getvcol(curwin, &(oap->end), &start, NULL, &end);
# ifdef FEAT_VIRTUALEDIT
		    if (virtual_active())
		    {
			start += curbuf->b_visual_end_coladd;
			oap->start_vcol += curbuf->b_visual_start_coladd;

			end = start;
			oap->end_vcol = oap->start_vcol;
		    }
# endif

		    if (start < oap->start_vcol)
			oap->start_vcol = start;
		    if (end > oap->end_vcol)
		    {
			if (*p_sel == 'e' && start - 1 >= oap->end_vcol)
			    oap->end_vcol = start - 1;
			else
			    oap->end_vcol = end;
		    }
		}

		/* if '$' was used, get oap->end_vcol from longest line */
		if (curwin->w_curswant == MAXCOL)
		{
		    curwin->w_cursor.col = MAXCOL;
		    oap->end_vcol = 0;
		    for (curwin->w_cursor.lnum = oap->start.lnum;
			    curwin->w_cursor.lnum <= oap->end.lnum;
						      ++curwin->w_cursor.lnum)
		    {
			getvcol(curwin, &curwin->w_cursor, NULL, NULL, &end);
			if (end > oap->end_vcol)
			    oap->end_vcol = end;
		    }
		}
		else if (redo_VIsual_busy)
		    oap->end_vcol = oap->start_vcol + redo_VIsual_col - 1;
		/*
		 * Correct oap->end.col and oap->start.col to be the
		 * upper-left and lower-right corner of the block area.
		 *
		 * (Actually, this does convert column positions into character
		 * positions)
		 */
		curwin->w_cursor.lnum = oap->end.lnum;
		coladvance(oap->end_vcol);
		oap->end = curwin->w_cursor;

		curwin->w_cursor = oap->start;
		coladvance(oap->start_vcol);
		oap->start = curwin->w_cursor;
	    }

	    if (!redo_VIsual_busy && !gui_yank)
	    {
		/*
		 * Prepare to reselect and redo Visual: this is based on the
		 * size of the Visual text
		 */
		resel_VIsual_mode = VIsual_mode;
		if (curwin->w_curswant == MAXCOL)
		    resel_VIsual_col = MAXCOL;
		else if (VIsual_mode == Ctrl_V)
		    resel_VIsual_col = oap->end_vcol - oap->start_vcol + 1;
		else if (oap->line_count > 1)
		    resel_VIsual_col = oap->end.col;
		else
		    resel_VIsual_col = oap->end.col - oap->start.col + 1;
		resel_VIsual_line_count = oap->line_count;
	    }

	    /* can't redo yank (unless 'y' is in 'cpoptions') and ":" */
	    if ((vim_strchr(p_cpo, CPO_YANK) != NULL || oap->op_type != OP_YANK)
		    && oap->op_type != OP_COLON
#ifdef FEAT_FOLDING
		    && oap->op_type != OP_FOLD
		    && oap->op_type != OP_FOLDOPEN
		    && oap->op_type != OP_FOLDOPENREC
		    && oap->op_type != OP_FOLDCLOSE
		    && oap->op_type != OP_FOLDCLOSEREC
#endif
		    && oap->motion_force == NUL
		    )
	    {
		prep_redo(oap->regname, 0L, NUL, 'v', NUL,
			get_op_char(oap->op_type),
					     get_extra_op_char(oap->op_type));
		redo_VIsual_mode = resel_VIsual_mode;
		redo_VIsual_col = resel_VIsual_col;
		redo_VIsual_line_count = resel_VIsual_line_count;
		redo_VIsual_count = cap->count0;
	    }

	    /*
	     * oap->inclusive defaults to TRUE.
	     * If oap->end is on a NUL (empty line) oap->inclusive becomes
	     * FALSE.  This makes "d}P" and "v}dP" work the same.
	     */
	    if (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    if (VIsual_mode == 'V')
		oap->motion_type = MLINE;
	    else
	    {
		oap->motion_type = MCHAR;
		if (VIsual_mode != Ctrl_V && *ml_get_pos(&(oap->end)) == NUL)
		{
		    oap->inclusive = FALSE;
		    /* Try to include the newline, unless it's an operator
		     * that works on lines only */
		    if (*p_sel != 'o'
			    && !op_on_lines(oap->op_type)
			    && oap->end.lnum < curbuf->b_ml.ml_line_count)
		    {
			++oap->end.lnum;
			oap->end.col = 0;
			++oap->line_count;
		    }
		}
	    }

	    redo_VIsual_busy = FALSE;
	    /*
	     * Switch Visual off now, so screen updating does
	     * not show inverted text when the screen is redrawn.
	     * With OP_YANK and sometimes with OP_COLON and OP_FILTER there is
	     * no screen redraw, so it is done here to remove the inverted
	     * part.
	     */
	    if (!gui_yank)
	    {
		VIsual_active = FALSE;
# ifdef FEAT_MOUSE
		setmouse();
		mouse_dragging = 0;
# endif
		if (p_smd)
		    clear_cmdline = TRUE;   /* unshow visual mode later */
		if ((oap->op_type == OP_YANK
			    || oap->op_type == OP_COLON
			    || oap->op_type == OP_FILTER)
			&& oap->motion_force == NUL)
		    redraw_curbuf_later(INVERTED);
	    }
	}
#endif

#ifdef FEAT_MBYTE
	/* Include the trailing byte of a multi-byte char. */
	if (has_mbyte && (oap->inclusive
# ifdef FEAT_VISUAL
		    || (VIsual_active && *p_sel != 'e')
# endif
		    ))
	{
	    int		l;

	    l = mb_ptr2len_check(ml_get_pos(&oap->end));
	    if (l > 1)
		oap->end.col += l - 1;
	}
#endif
	curwin->w_set_curswant = TRUE;

	/*
	 * oap->empty is set when start and end are the same.  The inclusive
	 * flag affects this too, unless yanking and the end is on a NUL.
	 */
	oap->empty = (oap->motion_type == MCHAR
		    && (!oap->inclusive
			|| (oap->op_type == OP_YANK
			    && gchar_pos(&oap->end) == NUL))
		    && equal(oap->start, oap->end));
	/*
	 * For delete, change and yank, it's an error to operate on an
	 * empty region, when 'E' inclucded in 'cpoptions' (Vi compatible).
	 */
	empty_region_error = (oap->empty
				&& vim_strchr(p_cpo, CPO_EMPTYREGION) != NULL);

#ifdef FEAT_VISUAL
	/* Force a redraw when operating on an empty Visual region or when
	 * 'modifiable is off. */
	if (oap->is_VIsual && (oap->empty || !curbuf->b_p_ma))
	    redraw_curbuf_later(INVERTED);
#endif

	/*
	 * If the end of an operator is in column one while oap->motion_type
	 * is MCHAR and oap->inclusive is FALSE, we put op_end after the last
	 * character in the previous line. If op_start is on or before the
	 * first non-blank in the line, the operator becomes linewise
	 * (strange, but that's the way vi does it).
	 */
	if (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_END)
		&& oap->end.col == 0
#ifdef FEAT_VISUAL
		&& (!oap->is_VIsual || *p_sel == 'o')
#endif
		&& oap->line_count > 1)
	{
	    oap->end_adjusted = TRUE;	    /* remember that we did this */
	    --oap->line_count;
	    --oap->end.lnum;
	    if (inindent(0))
		oap->motion_type = MLINE;
	    else
	    {
		oap->end.col = STRLEN(ml_get(oap->end.lnum));
		if (oap->end.col)
		{
		    --oap->end.col;
		    oap->inclusive = TRUE;
		}
	    }
	}
	else
	    oap->end_adjusted = FALSE;

	switch (oap->op_type)
	{
	case OP_LSHIFT:
	case OP_RSHIFT:
	    op_shift(oap, TRUE,
#ifdef FEAT_VISUAL
		    oap->is_VIsual ? (int)cap->count1 :
#endif
		    1);
	    break;

	case OP_JOIN_NS:
	case OP_JOIN:
	    if (oap->line_count < 2)
		oap->line_count = 2;
	    if (curwin->w_cursor.lnum + oap->line_count - 1 >
						   curbuf->b_ml.ml_line_count)
		beep_flush();
	    else
		do_do_join(oap->line_count, oap->op_type == OP_JOIN);
	    break;

	case OP_DELETE:
#ifdef FEAT_VISUAL
	    VIsual_reselect = FALSE;	    /* don't reselect now */
#endif
	    if (empty_region_error)
		vim_beep();
	    else
		(void)op_delete(oap);
	    break;

	case OP_YANK:
	    if (empty_region_error)
	    {
		if (!gui_yank)
		    vim_beep();
	    }
	    else
		(void)op_yank(oap, FALSE, !gui_yank);
	    check_cursor_col();
	    break;

	case OP_CHANGE:
#ifdef FEAT_VISUAL
	    VIsual_reselect = FALSE;	    /* don't reselect now */
#endif
	    if (empty_region_error)
		vim_beep();
	    else
	    {
		/* This is a new edit command, not a restart.  We don't edit
		 * recursively. */
		restart_edit = 0;
		if (op_change(oap))	/* will call edit() */
		    cap->retval |= CA_COMMAND_BUSY;
	    }
	    break;

	case OP_FILTER:
	    if (vim_strchr(p_cpo, CPO_FILTER) != NULL)
		AppendToRedobuff((char_u *)"!\r");  /* use any last used !cmd */
	    else
		bangredo = TRUE;    /* do_bang() will put cmd in redo buffer */

	case OP_INDENT:
	case OP_COLON:

#if defined(FEAT_LISP) || defined(FEAT_CINDENT)
	    /*
	     * If 'equalprg' is empty, do the indenting internally.
	     */
	    if (oap->op_type == OP_INDENT && *get_equalprg() == NUL)
	    {
# ifdef FEAT_LISP
		if (curbuf->b_p_lisp)
		{
		    op_reindent(oap, get_lisp_indent);
		    break;
		}
# endif
# ifdef FEAT_CINDENT
		op_reindent(oap,
#  ifdef FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_expr_indent :
#  endif
			    get_c_indent);
		break;
# endif
	    }
#endif

	    op_colon(oap);
	    break;

	case OP_TILDE:
	case OP_UPPER:
	case OP_LOWER:
	case OP_ROT13:
	    if (empty_region_error)
		vim_beep();
	    else
		op_tilde(oap);
	    check_cursor_col();
	    break;

	case OP_FORMAT:
	    if (*p_fp != NUL)
		op_colon(oap);		/* use external command */
	    else
		op_format(oap);		/* use internal function */
	    break;

	case OP_INSERT:
	case OP_APPEND:
#ifdef FEAT_VISUAL
	    VIsual_reselect = FALSE;	/* don't reselect now */
#endif
#ifdef FEAT_VISUALEXTRA
	    if (empty_region_error)
#endif
		vim_beep();
#ifdef FEAT_VISUALEXTRA
	    else
	    {
		/* This is a new edit command, not a restart.  We don't edit
		 * recursively. */
		restart_edit = 0;
		op_insert(oap, cap->count1);/* handles insert & append
					     * will call edit() */
	    }
#endif
	    break;

	case OP_REPLACE:
#ifdef FEAT_VISUAL
	    VIsual_reselect = FALSE;	/* don't reselect now */
#endif
#ifdef FEAT_VISUALEXTRA
	    if (empty_region_error)
#endif
		vim_beep();
#ifdef FEAT_VISUALEXTRA
	    else
		op_replace(oap, cap->nchar);
#endif
	    break;

#ifdef FEAT_FOLDING
	case OP_FOLD:
	    VIsual_reselect = FALSE;	/* don't reselect now */
	    foldCreate(oap->start.lnum, oap->end.lnum);
	    break;

	case OP_FOLDOPEN:
	case OP_FOLDOPENREC:
	case OP_FOLDCLOSE:
	case OP_FOLDCLOSEREC:
	    VIsual_reselect = FALSE;	/* don't reselect now */
	    opFoldRange(oap->start.lnum, oap->end.lnum,
		    oap->op_type == OP_FOLDOPEN
					    || oap->op_type == OP_FOLDOPENREC,
		    oap->op_type == OP_FOLDOPENREC
					  || oap->op_type == OP_FOLDCLOSEREC);
	    break;
#endif
	default:
	    clearopbeep(oap);
	}
	if (!gui_yank)
	{
	    /*
	     * if 'sol' not set, go back to old column for some commands
	     */
	    if (!p_sol && oap->motion_type == MLINE && !oap->end_adjusted
		    && (oap->op_type == OP_LSHIFT || oap->op_type == OP_RSHIFT
						|| oap->op_type == OP_DELETE))
		coladvance(curwin->w_curswant = old_col);
	    oap->op_type = OP_NOP;
	}
	else
	{
	    curwin->w_cursor = old_cursor;
#ifdef FEAT_VIRTUALEDIT
	    curwin->w_coladd = old_coladd;
#endif
	}
	oap->block_mode = FALSE;
	oap->regname = 0;
	oap->motion_force = NUL;
    }
}

/*
 * Handle indent and format operators and visual mode ":".
 */
    static void
op_colon(oap)
    oparg_t	*oap;
{
    stuffcharReadbuff(':');
#ifdef FEAT_VISUAL
    if (oap->is_VIsual)
	stuffReadbuff((char_u *)"'<,'>");
    else
#endif
    {
	/*
	 * Make the range look nice, so it can be repeated.
	 */
	if (oap->start.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	else
	    stuffnumReadbuff((long)oap->start.lnum);
	if (oap->end.lnum != oap->start.lnum)
	{
	    stuffcharReadbuff(',');
	    if (oap->end.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    else if (oap->end.lnum == curbuf->b_ml.ml_line_count)
		stuffcharReadbuff('$');
	    else if (oap->start.lnum == curwin->w_cursor.lnum)
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((long)oap->line_count - 1);
	    }
	    else
		stuffnumReadbuff((long)oap->end.lnum);
	}
    }
    if (oap->op_type != OP_COLON)
	stuffReadbuff((char_u *)"!");
    if (oap->op_type == OP_INDENT)
    {
#ifndef FEAT_CINDENT
	if (*get_equalprg() == NUL)
	    stuffReadbuff((char_u *)"indent");
	else
#endif
	    stuffReadbuff(get_equalprg());
	stuffReadbuff((char_u *)"\n");
    }
    else if (oap->op_type == OP_FORMAT)
    {
	if (*p_fp == NUL)
	    stuffReadbuff((char_u *)"fmt");
	else
	    stuffReadbuff(p_fp);
	stuffReadbuff((char_u *)"\n");
    }

    /*
     * do_cmdline() does the rest
     */
}

#if defined(FEAT_MOUSE) || defined(PROTO)
/*
 * Do the appropriate action for the current mouse click in the current mode.
 *
 * Normal Mode:
 * event	 modi-	position      visual	   change   action
 *		 fier	cursor			   window
 * left press	  -	yes	    end		    yes
 * left press	  C	yes	    end		    yes	    "^]" (2)
 * left press	  S	yes	    end		    yes	    "*" (2)
 * left drag	  -	yes	start if moved	    no
 * left relse	  -	yes	start if moved	    no
 * middle press	  -	yes	 if not active	    no	    put register
 * middle press	  -	yes	 if active	    no	    yank and put
 * right press	  -	yes	start or extend	    yes
 * right press	  S	yes	no change	    yes	    "#" (2)
 * right drag	  -	yes	extend		    no
 * right relse	  -	yes	extend		    no
 *
 * Insert or Replace Mode:
 * event	 modi-	position      visual	   change   action
 *		 fier	cursor			   window
 * left press	  -	yes	(cannot be active)  yes
 * left press	  C	yes	(cannot be active)  yes	    "CTRL-O^]" (2)
 * left press	  S	yes	(cannot be active)  yes	    "CTRL-O*" (2)
 * left drag	  -	yes	start or extend (1) no	    CTRL-O (1)
 * left relse	  -	yes	start or extend (1) no	    CTRL-O (1)
 * middle press	  -	no	(cannot be active)  no	    put register
 * right press	  -	yes	start or extend	    yes	    CTRL-O
 * right press	  S	yes	(cannot be active)  yes	    "CTRL-O#" (2)
 *
 * (1) only if mouse pointer moved since press
 * (2) only if click is in same buffer
 *
 * Return TRUE if start_arrow() should be called for edit mode.
 */
    int
do_mouse(oap, c, dir, count, fix_indent)
    oparg_t	*oap;		/* operator argument, can be NULL */
    int		c;		/* K_LEFTMOUSE, etc */
    int		dir;		/* Direction to 'put' if necessary */
    long	count;
    int		fix_indent;	/* PUT_FIXINDENT if fixing indent necessary */
{
    static int	do_always = FALSE;	/* ignore 'mouse' setting next time */
    static int	got_click = FALSE;	/* got a click some time back */

    int		which_button;	/* MOUSE_LEFT, _MIDDLE or _RIGHT */
    int		is_click;	/* If FALSE it's a drag or release event */
    int		is_drag;	/* If TRUE it's a drag event */
    int		jump_flags = 0;	/* flags for jump_to_mouse() */
    pos_t	start_visual;
    int		moved;		/* Has cursor moved? */
    int		in_status_line;	/* mouse in status line */
#ifdef FEAT_VERTSPLIT
    int		in_sep_line;	/* mouse in vertical separator line */
#endif
    int		c1, c2;
#if defined(FEAT_FOLDING)
    pos_t	save_cursor = curwin->w_cursor;
    win_t	*save_curwin = curwin;
#endif
#ifdef FEAT_VISUAL
    static pos_t	orig_cursor;
    colnr_t		leftcol, rightcol;
    pos_t		end_visual;
    int			diff;
    int			VIsual_was_active = VIsual_active;
#endif
    int		regname;

    /*
     * When GUI is active, always recognize mouse events, otherwise:
     * - Ignore mouse event in normal mode if 'mouse' doesn't include 'n'.
     * - Ignore mouse event in visual mode if 'mouse' doesn't include 'v'.
     * - For command line and insert mode 'mouse' is checked before calling
     *	 do_mouse().
     */
    if (do_always)
	do_always = FALSE;
    else
#ifdef FEAT_GUI
	if (!gui.in_use)
#endif
	{
#ifdef FEAT_VISUAL
	    if (VIsual_active)
	    {
		if (!mouse_has(MOUSE_VISUAL))
		    return FALSE;
	    }
	    else
#endif
		if (State == NORMAL && !mouse_has(MOUSE_NORMAL))
		return FALSE;
	}

    which_button = get_mouse_button(KEY2TERMCAP1(c), &is_click, &is_drag);

#ifdef FEAT_MOUSESHAPE
    /* May have stopped dragging the status or separator line. */
    if (!is_drag && (drag_status_line
# ifdef FEAT_VERTSPLIT
		|| drag_sep_line
# endif
		))
    {
	drag_status_line = FALSE;
# ifdef FEAT_VERTSPLIT
	drag_sep_line = FALSE;
# endif
	update_mouseshape(-1);
    }
#endif

    /*
     * Ignore drag and release events if we didn't get a click.
     */
    if (is_click)
	got_click = TRUE;
    else
    {
	if (!got_click)			/* didn't get click, ignore */
	    return FALSE;
	if (!is_drag)			/* release, reset got_click */
	    got_click = FALSE;
    }

    /*
     * ALT is currently ignored
     */
    if ((mod_mask & MOD_MASK_ALT))
	return FALSE;

    /*
     * CTRL right mouse button does CTRL-T
     */
    if (is_click && (mod_mask & MOD_MASK_CTRL) && which_button == MOUSE_RIGHT)
    {
	if (State & INSERT)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_T);
	got_click = FALSE;		/* ignore drag&release now */
	return FALSE;
    }

    /*
     * CTRL only works with left mouse button
     */
    if ((mod_mask & MOD_MASK_CTRL) && which_button != MOUSE_LEFT)
	return FALSE;

    /*
     * When a modifier is down, ignore drag and release events, as well as
     * multiple clicks and the middle mouse button.
     * Accept shift-leftmouse drags when 'mousemodel' is "popup.*".
     */
    if ((mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL | MOD_MASK_ALT))
	    && (!is_click
		|| (mod_mask & MOD_MASK_MULTI_CLICK)
		|| which_button == MOUSE_MIDDLE)
	    && !((mod_mask & MOD_MASK_SHIFT)
		&& mouse_model_popup()
		&& which_button == MOUSE_LEFT)
	    )
	return FALSE;

    /*
     * If the button press was used as the movement command for an operator
     * (eg "d<MOUSE>"), or it is the middle button that is held down, ignore
     * drag/release events.
     */
    if (!is_click && which_button == MOUSE_MIDDLE)
	return FALSE;

    if (oap != NULL)
	regname = oap->regname;
    else
	regname = 0;

    /*
     * Middle mouse button does a 'put' of the selected text
     */
    if (which_button == MOUSE_MIDDLE)
    {
	if (State == NORMAL)
	{
	    /*
	     * If an operator was pending, we don't know what the user wanted
	     * to do. Go back to normal mode: Clear the operator and beep().
	     */
	    if (oap != NULL && oap->op_type != OP_NOP)
	    {
		clearopbeep(oap);
		return FALSE;
	    }

#ifdef FEAT_VISUAL
	    /*
	     * If visual was active, yank the highlighted text and put it
	     * before the mouse pointer position.
	     */
	    if (VIsual_active)
	    {
		stuffcharReadbuff('y');
		stuffcharReadbuff(K_MIDDLEMOUSE);
		do_always = TRUE;	/* ignore 'mouse' setting next time */
		return FALSE;
	    }
#endif
	    /*
	     * The rest is below jump_to_mouse()
	     */
	}

	/*
	 * Middle click in insert mode doesn't move the mouse, just insert the
	 * contents of a register.  '.' register is special, can't insert that
	 * with do_put().
	 */
	else if (State & INSERT)
	{
	    if (regname == '.')
		insert_reg(regname, TRUE);
	    else
	    {
#ifdef FEAT_CLIPBOARD
		if (clipboard.available && regname == 0)
		    regname = '*';
#endif
		if ((State == REPLACE || State == VREPLACE)
					    && !yank_register_mline(regname))
		    insert_reg(regname, TRUE);
		else
		{
		    do_put(regname, BACKWARD, 1L, fix_indent | PUT_CURSEND);

		    /* Repeat it with CTRL-R CTRL-O r or CTRL-R CTRL-P r */
		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(fix_indent ? Ctrl_P : Ctrl_O);
		    AppendCharToRedobuff(regname == 0 ? '"' : regname);
		}
	    }
	    return FALSE;
	}
	else
	    return FALSE;
    }

    if (!is_click)
	jump_flags |= MOUSE_FOCUS | MOUSE_DID_MOVE;

    start_visual.lnum = 0;

    /*
     * When 'mousemodel' is "popup" or "popup_setpos", translate mouse events:
     * right button up   -> pop-up menu
     * shift-left button -> right button
     */
    if (mouse_model_popup())
    {
	if (which_button == MOUSE_RIGHT
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	{
	    /*
	     * NOTE: Ignore right button down and drag mouse events.
	     * Windows only shows the popup menu on the button up event.
	     */
#if defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK)
	    if (!is_click)
		return FALSE;
#endif
#if defined(FEAT_GUI_ATHENA) || defined(FEAT_GUI_MSWIN)
	    if (is_click || is_drag)
		return FALSE;
#endif
#if defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
	    || defined(FEAT_GUI_ATHENA) || defined(FEAT_GUI_MSWIN) \
	    || defined(FEAT_GUI_MAC)
	    if (gui.in_use)
	    {
		jump_flags = 0;
		if (STRCMP(p_mousem, "popup_setpos") == 0)
		{
		    /* First set the cursor position before showing the popup
		     * menu. */
#ifdef FEAT_VISUAL
		    if (VIsual_active)
		    {
			pos_t    m_pos;

			/*
			 * set MOUSE_MAY_STOP_VIS if we are outside the
			 * selection or the current window (might have false
			 * negative here)
			 */
			if (mouse_row < W_WINROW(curwin)
			     || mouse_row
				      > (W_WINROW(curwin) + curwin->w_height))
			    jump_flags = MOUSE_MAY_STOP_VIS;
			else if (get_fpos_of_mouse(&m_pos) != IN_BUFFER)
			    jump_flags = MOUSE_MAY_STOP_VIS;
			else
			{
			    if ((lt(curwin->w_cursor, VIsual)
					&& (lt(m_pos, curwin->w_cursor)
					    || lt(VIsual, m_pos)))
				    || (lt(VIsual, curwin->w_cursor)
					&& (lt(m_pos, VIsual)
					    || lt(curwin->w_cursor, m_pos))))
			    {
				jump_flags = MOUSE_MAY_STOP_VIS;
			    }
			    else if (VIsual_mode == Ctrl_V)
			    {
				getvcols(&curwin->w_cursor, &VIsual,
							 &leftcol, &rightcol);
				getvcol(curwin, &m_pos, NULL, &m_pos.col, NULL);
				if (m_pos.col < leftcol || m_pos.col > rightcol)
				    jump_flags = MOUSE_MAY_STOP_VIS;
			    }
			}
		    }
		    else
			jump_flags = MOUSE_MAY_STOP_VIS;
#endif
		}
		if (jump_flags)
		{
		    jump_flags = jump_to_mouse(jump_flags, NULL);
		    update_curbuf(
#ifdef FEAT_VISUAL
			    VIsual_active ? INVERTED :
#endif
			    VALID);
		    setcursor();
		    out_flush();    /* Update before showing popup menu */
		}
# ifdef FEAT_MENU
		gui_show_popupmenu();
# endif
		return (jump_flags & CURSOR_MOVED) != 0;
	    }
	    else
		return FALSE;
#else
	    return FALSE;
#endif
	}
	if (which_button == MOUSE_LEFT && (mod_mask & MOD_MASK_SHIFT))
	{
	    which_button = MOUSE_RIGHT;
	    mod_mask &= ~ MOD_MASK_SHIFT;
	}
    }

#ifdef FEAT_VISUAL
    if ((State & (NORMAL | INSERT))
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
    {
	if (which_button == MOUSE_LEFT)
	{
	    if (is_click)
	    {
		/* stop Visual mode for a left click in a window, but not when
		 * on a status line */
		if (VIsual_active)
		    jump_flags |= MOUSE_MAY_STOP_VIS;
	    }
	    else
		jump_flags |= MOUSE_MAY_VIS;
	}
	else if (which_button == MOUSE_RIGHT)
	{
	    if (is_click && VIsual_active)
	    {
		/*
		 * Remember the start and end of visual before moving the
		 * cursor.
		 */
		if (lt(curwin->w_cursor, VIsual))
		{
		    start_visual = curwin->w_cursor;
		    end_visual = VIsual;
		}
		else
		{
		    start_visual = VIsual;
		    end_visual = curwin->w_cursor;
		}
	    }
	    jump_flags |= MOUSE_MAY_VIS;
	}
    }
#endif

    /*
     * If an operator is pending, ignore all drags and releases until the
     * next mouse click.
     */
    if (!is_drag && oap != NULL && oap->op_type != OP_NOP)
    {
	got_click = FALSE;
	oap->motion_type = MCHAR;
    }

    /*
     * JUMP!
     */
    jump_flags = jump_to_mouse(jump_flags,
				      oap == NULL ? NULL : &(oap->inclusive));
    moved = (jump_flags & CURSOR_MOVED);
    in_status_line = (jump_flags & IN_STATUS_LINE);
#ifdef FEAT_VERTSPLIT
    in_sep_line = (jump_flags & IN_SEP_LINE);
#endif

#ifdef FEAT_FOLDING
    if (mod_mask == 0
	    && !is_drag
	    && (jump_flags & (MOUSE_FOLD_CLOSE | MOUSE_FOLD_OPEN))
	    && which_button == MOUSE_LEFT)
    {
	/* open or close a fold at this line */
	if (jump_flags & MOUSE_FOLD_OPEN)
	    openFold(curwin->w_cursor.lnum);
	else
	    closeFold(curwin->w_cursor.lnum);
	/* don't move the cursor if still in the same window */
	if (curwin == save_curwin)
	    curwin->w_cursor = save_cursor;
    }
#endif

#ifdef FEAT_VISUAL
    /* Set global flag that we are extending the Visual area with mouse
     * dragging; temporarily mimimize 'scrolloff'. */
    if (VIsual_active && is_drag && p_so)
    {
	/* In the very first line, allow scrolling one line */
	if (mouse_row == 0)
	    mouse_dragging = 2;
	else
	    mouse_dragging = 1;
    }

    /* When dragging the mouse above the window, scroll down. */
    if (is_drag && mouse_row < 0)
    {
	scroll_redraw(FALSE, 1L);
	mouse_row = 0;
    }

    if (start_visual.lnum)		/* right click in visual mode */
    {
	/*
	 * In Visual-block mode, divide the area in four, pick up the corner
	 * that is in the quarter that the cursor is in.
	 */
	if (VIsual_mode == Ctrl_V)
	{
	    getvcols(&start_visual, &end_visual, &leftcol, &rightcol);
	    if (curwin->w_curswant > (leftcol + rightcol) / 2)
		end_visual.col = leftcol;
	    else
		end_visual.col = rightcol;
	    if (curwin->w_cursor.lnum <
				    (start_visual.lnum + end_visual.lnum) / 2)
		end_visual.lnum = end_visual.lnum;
	    else
		end_visual.lnum = start_visual.lnum;

	    /* move VIsual to the right column */
	    start_visual = curwin->w_cursor;	    /* save the cursor pos */
	    curwin->w_cursor = end_visual;
	    coladvance(end_visual.col);
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = start_visual;	    /* restore the cursor */
	}
	else
	{
	    /*
	     * If the click is before the start of visual, change the start.
	     * If the click is after the end of visual, change the end.  If
	     * the click is inside the visual, change the closest side.
	     */
	    if (lt(curwin->w_cursor, start_visual))
		VIsual = end_visual;
	    else if (lt(end_visual, curwin->w_cursor))
		VIsual = start_visual;
	    else
	    {
		/* In the same line, compare column number */
		if (end_visual.lnum == start_visual.lnum)
		{
		    if (curwin->w_cursor.col - start_visual.col >
				    end_visual.col - curwin->w_cursor.col)
			VIsual = start_visual;
		    else
			VIsual = end_visual;
		}

		/* In different lines, compare line number */
		else
		{
		    diff = (curwin->w_cursor.lnum - start_visual.lnum) -
				(end_visual.lnum - curwin->w_cursor.lnum);

		    if (diff > 0)		/* closest to end */
			VIsual = start_visual;
		    else if (diff < 0)	/* closest to start */
			VIsual = end_visual;
		    else			/* in the middle line */
		    {
			if (curwin->w_cursor.col <
					(start_visual.col + end_visual.col) / 2)
			    VIsual = end_visual;
			else
			    VIsual = start_visual;
		    }
		}
	    }
	}
    }
    /*
     * If Visual mode started in insert mode, execute "CTRL-O"
     */
    else if ((State & INSERT) && VIsual_active)
	stuffcharReadbuff(Ctrl_O);
#endif

    /*
     * Middle mouse click: Put text before cursor.
     */
    if (which_button == MOUSE_MIDDLE)
    {
#ifdef FEAT_CLIPBOARD
	if (clipboard.available && regname == 0)
	    regname = '*';
#endif
	if (yank_register_mline(regname))
	{
	    if (mouse_past_bottom)
		dir = FORWARD;
	}
	else if (mouse_past_eol)
	    dir = FORWARD;

	if (fix_indent)
	{
	    c1 = (dir == BACKWARD) ? '[' : ']';
	    c2 = 'p';
	}
	else
	{
	    c1 = (dir == FORWARD) ? 'p' : 'P';
	    c2 = NUL;
	}
	prep_redo(regname, count, NUL, c1, NUL, c2, NUL);

	/*
	 * Remember where the paste started, so in edit() Insstart can be set
	 * to this position
	 */
	if (restart_edit)
	    where_paste_started = curwin->w_cursor;
	do_put(regname, dir, count, fix_indent | PUT_CURSEND);
    }

#if defined(FEAT_WINDOWS) && defined(FEAT_QUICKFIX)
    /*
     * Ctrl-Mouse click or double click in a quickfix window jumps to the
     * error under the mouse pointer.
     */
    else if (((mod_mask & MOD_MASK_CTRL) || (mod_mask & MOD_MASK_2CLICK))
	    && bt_quickfix(curbuf))
    {
	if (State & INSERT)
	    stuffcharReadbuff(Ctrl_O);
	stuffReadbuff((char_u *)":.cc\n");
	got_click = FALSE;		/* ignore drag&release now */
    }
#endif

    /*
     * Ctrl-Mouse click (or double click in a help window) jumps to the tag
     * under the mouse pointer.
     */
    else if ((mod_mask & MOD_MASK_CTRL)
			  || (curbuf->b_help && (mod_mask & MOD_MASK_2CLICK)))
    {
	if (State & INSERT)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_RSB);
	got_click = FALSE;		/* ignore drag&release now */
    }

    /*
     * Shift-Mouse click searches for the next occurrence of the word under
     * the mouse pointer
     */
    else if ((mod_mask & MOD_MASK_SHIFT))
    {
	if (State & INSERT
#ifdef FEAT_VISUAL
		|| (VIsual_active && VIsual_select)
#endif
		)
	    stuffcharReadbuff(Ctrl_O);
	if (which_button == MOUSE_LEFT)
	    stuffcharReadbuff('*');
	else	/* MOUSE_RIGHT */
	    stuffcharReadbuff('#');
    }

    /* Handle double clicks, unless on status line */
    else if (in_status_line)
    {
#ifdef FEAT_MOUSESHAPE
	if ((is_drag || is_click) && !drag_status_line)
	{
	    drag_status_line = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
#ifdef FEAT_VERTSPLIT
    else if (in_sep_line)
    {
# ifdef FEAT_MOUSESHAPE
	if ((is_drag || is_click) && !drag_sep_line)
	{
	    drag_sep_line = TRUE;
	    update_mouseshape(-1);
	}
# endif
    }
#endif
#ifdef FEAT_VISUAL
    else if ((mod_mask & MOD_MASK_MULTI_CLICK) && (State & (NORMAL | INSERT)))
    {
	if (is_click || !VIsual_active)
	{
	    if (VIsual_active)
		orig_cursor = VIsual;
	    else
	    {
		check_visual_highlight();
		VIsual = curwin->w_cursor;
		orig_cursor = VIsual;
		VIsual_active = TRUE;
		VIsual_reselect = TRUE;
		/* start Select mode if 'selectmode' contains "mouse" */
		may_start_select('o');
		setmouse();
		if (p_smd)
		    redraw_cmdline = TRUE;  /* show visual mode later */
	    }
	    if (mod_mask & MOD_MASK_2CLICK)
		VIsual_mode = 'v';
	    else if (mod_mask & MOD_MASK_3CLICK)
		VIsual_mode = 'V';
	    else if (mod_mask & MOD_MASK_4CLICK)
		VIsual_mode = Ctrl_V;
#ifdef FEAT_CLIPBOARD
	    /* Make sure the clipboard gets updated.  Needed because start and
	     * end may still be the same, and the selection needs to be owned */
	    clipboard.vmode = NUL;
#endif
	}
	/*
	 * A double click selects a word or a block.
	 */
	if (is_click && (mod_mask & MOD_MASK_2CLICK))
	{
	    pos_t	*pos;

	    /* If the character under the cursor (skipping white space) is not
	     * a word character, try finding a match and select a (), {}, [],
	     * #if/#endif, etc. block. */
	    end_visual = curwin->w_cursor;
	    while (vim_iswhite(gchar_pos(&end_visual)))
		inc(&end_visual);
	    if (oap != NULL)
		oap->motion_type = MCHAR;
	    if (oap != NULL
		    && VIsual_mode == 'v'
		    && !vim_isIDc(gchar_pos(&end_visual))
		    && equal(curwin->w_cursor, VIsual)
		    && (pos = findmatch(oap, NUL)) != NULL)
	    {
		curwin->w_cursor = *pos;
		if (oap->motion_type == MLINE)
		    VIsual_mode = 'V';
		else if (*p_sel == 'e')
		    ++curwin->w_cursor.col;
	    }
	    else if (lt(curwin->w_cursor, orig_cursor))
	    {
		find_start_of_word(&curwin->w_cursor);
		find_end_of_word(&VIsual);
	    }
	    else
	    {
		find_start_of_word(&VIsual);
		find_end_of_word(&curwin->w_cursor);
	    }
	    curwin->w_set_curswant = TRUE;
	}
	if (is_click)
	    redraw_curbuf_later(INVERTED);	/* update the inversion */
    }
    else if (VIsual_active && VIsual_was_active != VIsual_active)
	VIsual_mode = 'v';
#endif

    return moved;
}

#ifdef FEAT_VISUAL
    static void
find_start_of_word(pos)
    pos_t    *pos;
{
    char_u  *ptr;
    int	    cclass;

    ptr = ml_get(pos->lnum);
    cclass = get_mouse_class(ptr[pos->col]);

    /* Can't test pos->col >= 0 because pos->col is unsigned */
    while (pos->col > 0 && get_mouse_class(ptr[pos->col]) == cclass)
	pos->col--;
    if (pos->col != 0 || get_mouse_class(ptr[0]) != cclass)
	pos->col++;
}

    static void
find_end_of_word(pos)
    pos_t    *pos;
{
    char_u  *ptr;
    int	    cclass;

    ptr = ml_get(pos->lnum);
    if (*p_sel == 'e' && pos->col)
	pos->col--;
    cclass = get_mouse_class(ptr[pos->col]);
    while (ptr[pos->col] && get_mouse_class(ptr[pos->col]) == cclass)
	pos->col++;
    if (*p_sel != 'e' && pos->col)
	pos->col--;
}

    static int
get_mouse_class(c)
    int	    c;
{
    if (c == ' ' || c == '\t')
	return ' ';

    if (vim_isIDc(c))
	return 'a';

    /*
     * There are a few special cases where we want certain combinations of
     * characters to be considered as a single word.  These are things like
     * "->", "/ *", "*=", "+=", "&=", "<=", ">=", "!=" etc.  Otherwise, each
     * character is in it's own class.
     */
    if (c != NUL && vim_strchr((char_u *)"-+*/%<>&|^!=", c) != NULL)
	return '=';
    return c;
}
#endif /* FEAT_VISUAL */
#endif /* FEAT_MOUSE */

#if defined(FEAT_VISUAL) || defined(PROTO)
/*
 * Check if  highlighting for visual mode is possible, give a warning message
 * if not.
 */
    void
check_visual_highlight()
{
    static int	    did_check = FALSE;

    if (!did_check && hl_attr(HLF_V) == 0)
	MSG(_("Warning: terminal cannot highlight"));
    did_check = TRUE;
}

/*
 * End visual mode.
 * This function should ALWAYS be called to end Visual mode, except from
 * do_pending_operator().
 */
    void
end_visual_mode()
{
#ifdef FEAT_CLIPBOARD
    /*
     * If we are using the clipboard, then remember what was selected in case
     * we need to paste it somewhere while we still own the selection.
     * Only do this when the clipboard is already owned.  Don't want to grab
     * the selection when hitting ESC.
     */
    if (clipboard.available && clipboard.owned)
	clip_auto_select();
#endif

    VIsual_active = FALSE;
#ifdef FEAT_MOUSE
    setmouse();
    mouse_dragging = 0;
#endif

    /* Save the current VIsual area for '< and '> marks, and "gv" */
    curbuf->b_visual_mode = VIsual_mode;
    curbuf->b_visual_start = VIsual;
    curbuf->b_visual_end = curwin->w_cursor;
#ifdef FEAT_VIRTUALEDIT
    curbuf->b_visual_start_coladd = VIsual_coladd;
    curbuf->b_visual_end_coladd = curwin->w_coladd;
    if (!virtual_active())
	curwin->w_coladd = 0;
#endif

    if (p_smd)
	clear_cmdline = TRUE;		/* unshow visual mode later */

    /* Don't leave the cursor past the end of the line */
    if (curwin->w_cursor.col > 0 && *ml_get_cursor() == NUL)
	--curwin->w_cursor.col;
}

/*
 * Reset VIsual_active and VIsual_reselect.
 */
    void
reset_VIsual_and_resel()
{
    if (VIsual_active)
    {
	end_visual_mode();
	redraw_curbuf_later(INVERTED);	/* delete the inversion later */
    }
    VIsual_reselect = FALSE;
}

/*
 * Reset VIsual_active and VIsual_reselect if it's set.
 */
    void
reset_VIsual()
{
    if (VIsual_active)
    {
	end_visual_mode();
	redraw_curbuf_later(INVERTED);	/* delete the inversion later */
	VIsual_reselect = FALSE;
    }
}
#endif /* FEAT_VISUAL */

/*
 * Find the identifier under or to the right of the cursor.  If none is
 * found and find_type has FIND_STRING, then find any non-white string.  The
 * length of the string is returned, or zero if no string is found.  If a
 * string is found, a pointer to the string is put in *string, but note that
 * the caller must use the length returned as this string may not be NUL
 * terminated.
 */
    int
find_ident_under_cursor(string, find_type)
    char_u  **string;
    int	    find_type;
{
    char_u  *ptr;
    int	    col = 0;	    /* init to shut up GCC */
    int	    i;

    /*
     * if i == 0: try to find an identifier
     * if i == 1: try to find any string
     */
    ptr = ml_get_curline();
    for (i = (find_type & FIND_IDENT) ? 0 : 1;	i < 2; ++i)
    {
	/*
	 * skip to start of identifier/string
	 */
	col = curwin->w_cursor.col;
	while (ptr[col] != NUL &&
		    (i == 0 ? !vim_iswordc(ptr[col]) : vim_iswhite(ptr[col])))
	    ++col;

	/*
	 * Back up to start of identifier/string. This doesn't match the
	 * real vi but I like it a little better and it shouldn't bother
	 * anyone.
	 * When FIND_IDENT isn't defined, we backup until a blank.
	 */
	while (col > 0 && (i == 0 ? vim_iswordc(ptr[col - 1]) :
		    (!vim_iswhite(ptr[col - 1]) &&
		   (!(find_type & FIND_IDENT) || !vim_iswordc(ptr[col - 1])))))
	    --col;

	/*
	 * if we don't want just any old string, or we've found an identifier,
	 * stop searching.
	 */
	if (!(find_type & FIND_STRING) || vim_iswordc(ptr[col]))
	    break;
    }
    /*
     * didn't find an identifier or string
     */
    if (ptr[col] == NUL || (!vim_iswordc(ptr[col]) && i == 0))
    {
	if (find_type & FIND_STRING)
	    EMSG(_("No string under cursor"));
	else
	    EMSG(_("No identifier under cursor"));
	return 0;
    }
    ptr += col;
    *string = ptr;
    col = 0;
    while (i == 0 ? vim_iswordc(*ptr) : (*ptr != NUL && !vim_iswhite(*ptr)))
    {
	++ptr;
	++col;
    }
    return col;
}

/*
 * Prepare for redo of a normal command.
 */
    static void
prep_redo_cmd(cap)
    cmdarg_t  *cap;
{
    prep_redo(cap->oap->regname, cap->count0,
				     NUL, cap->cmdchar, NUL, NUL, cap->nchar);
}

/*
 * Prepare for redo of any command.
 * Note that only the last argument can be a multi-byte char.
 */
    static void
prep_redo(regname, num, cmd1, cmd2, cmd3, cmd4, cmd5)
    int	    regname;
    long    num;
    int	    cmd1;
    int	    cmd2;
    int	    cmd3;
    int	    cmd4;
    int	    cmd5;
{
    ResetRedobuff();
    if (regname != 0)	/* yank from specified buffer */
    {
	AppendCharToRedobuff('"');
	AppendCharToRedobuff(regname);
    }
    if (num)
	AppendNumberToRedobuff(num);

    if (cmd1 != NUL)
	AppendCharToRedobuff(cmd1);
    if (cmd2 != NUL)
	AppendCharToRedobuff(cmd2);
    if (cmd3 != NUL)
	AppendCharToRedobuff(cmd3);
    if (cmd4 != NUL)
	AppendCharToRedobuff(cmd4);
    if (cmd5 != NUL)
    {
#ifdef FEAT_MBYTE
	{
	    char_u	buf[MB_MAXBYTES + 1];

	    buf[mb_char2bytes(cmd5, buf)] = NUL;
	    AppendToRedobuff(buf);
	}
#else
	AppendCharToRedobuff(cmd5);
#endif
    }
}

/*
 * check for operator active and clear it
 *
 * return TRUE if operator was active
 */
    static int
checkclearop(oap)
    oparg_t	*oap;
{
    if (oap->op_type == OP_NOP)
	return FALSE;
    clearopbeep(oap);
    return TRUE;
}

/*
 * check for operator or Visual active and clear it
 *
 * return TRUE if operator was active
 */
    static int
checkclearopq(oap)
    oparg_t	*oap;
{
    if (oap->op_type == OP_NOP
#ifdef FEAT_VISUAL
	    && !VIsual_active
#endif
	    )
	return FALSE;
    clearopbeep(oap);
    return TRUE;
}

    static void
clearop(oap)
    oparg_t	*oap;
{
    oap->op_type = OP_NOP;
    oap->regname = 0;
    oap->motion_force = NUL;
}

    static void
clearopbeep(oap)
    oparg_t	*oap;
{
    clearop(oap);
    beep_flush();
}

#ifdef FEAT_VISUAL
/*
 * Remove the shift modifier from a special key.
 */
    static void
unshift_special(cap)
    cmdarg_t	*cap;
{
    switch (cap->cmdchar)
    {
	case K_S_RIGHT:	cap->cmdchar = K_RIGHT; break;
	case K_S_LEFT:	cap->cmdchar = K_LEFT; break;
	case K_S_UP:	cap->cmdchar = K_UP; break;
	case K_S_DOWN:	cap->cmdchar = K_DOWN; break;
	case K_S_HOME:	cap->cmdchar = K_HOME; break;
	case K_S_END:	cap->cmdchar = K_END; break;
    }
}
#endif

#ifdef FEAT_CMDL_INFO
/*
 * Routines for displaying a partly typed command
 */

static char_u	showcmd_buf[SHOWCMD_COLS + 1];
static char_u	old_showcmd_buf[SHOWCMD_COLS + 1];  /* For push_showcmd() */
static int	showcmd_is_clear = TRUE;

static void display_showcmd __ARGS((void));

    void
clear_showcmd()
{
    if (!p_sc)
	return;

    showcmd_buf[0] = NUL;

    /*
     * Don't actually display something if there is nothing to clear.
     */
    if (showcmd_is_clear)
	return;

    display_showcmd();
}

/*
 * Add 'c' to string of shown command chars.
 * Return TRUE if output has been written (and setcursor() has been called).
 */
    int
add_to_showcmd(c)
    int	    c;
{
    char_u  *p;
    int	    old_len;
    int	    extra_len;
    int	    overflow;
#if defined(FEAT_MOUSE)
    int	    i;
    static int	    ignore[] =
	       {
#ifdef FEAT_GUI
		K_VER_SCROLLBAR, K_HOR_SCROLLBAR,
		K_LEFTMOUSE_NM, K_LEFTRELEASE_NM,
#endif
		K_IGNORE,
		K_LEFTMOUSE, K_LEFTDRAG, K_LEFTRELEASE,
		K_MIDDLEMOUSE, K_MIDDLEDRAG, K_MIDDLERELEASE,
		K_RIGHTMOUSE, K_RIGHTDRAG, K_RIGHTRELEASE,
		K_MOUSEDOWN, K_MOUSEUP,
		0};
#endif

    if (!p_sc)
	return FALSE;

#if defined(FEAT_MOUSE)
    /* Ignore keys that are scrollbar updates and mouse clicks */
    for (i = 0; ignore[i]; ++i)
	if (ignore[i] == c)
	    return FALSE;
#endif

    p = transchar(c);
    old_len = STRLEN(showcmd_buf);
    extra_len = STRLEN(p);
    overflow = old_len + extra_len - SHOWCMD_COLS;
    if (overflow > 0)
	STRCPY(showcmd_buf, showcmd_buf + overflow);
    STRCAT(showcmd_buf, p);

    if (char_avail())
	return FALSE;

    display_showcmd();

    return TRUE;
}

    void
add_to_showcmd_c(c)
    int		c;
{
    if (!add_to_showcmd(c))
	setcursor();
}

/*
 * Delete 'len' characters from the end of the shown command.
 */
    static void
del_from_showcmd(len)
    int	    len;
{
    int	    old_len;

    if (!p_sc)
	return;

    old_len = STRLEN(showcmd_buf);
    if (len > old_len)
	len = old_len;
    showcmd_buf[old_len - len] = NUL;

    if (!char_avail())
	display_showcmd();
}

    void
push_showcmd()
{
    if (p_sc)
	STRCPY(old_showcmd_buf, showcmd_buf);
}

    void
pop_showcmd()
{
    if (!p_sc)
	return;

    STRCPY(showcmd_buf, old_showcmd_buf);

    display_showcmd();
}

    static void
display_showcmd()
{
    int	    len;

    cursor_off();

    len = STRLEN(showcmd_buf);
    if (len == 0)
	showcmd_is_clear = TRUE;
    else
    {
	screen_puts(showcmd_buf, (int)Rows - 1, sc_col, 0);
	showcmd_is_clear = FALSE;
    }

    /*
     * clear the rest of an old message by outputing up to SHOWCMD_COLS spaces
     */
    screen_puts((char_u *)"          " + len, (int)Rows - 1, sc_col + len, 0);

    setcursor();	    /* put cursor back where it belongs */
}
#endif

#ifdef FEAT_SCROLLBIND
/*
 * When "check" is FALSE, prepare for commands that scroll the window.
 * When "check" is TRUE, take care of scroll-binding after the window has
 * scrolled.  Called from normal_cmd() and edit().
 */
    void
do_check_scrollbind(check)
    int		check;
{
    static win_t	*old_curwin = NULL;
    static linenr_t	old_topline = 0;
    static buf_t	*old_buf = NULL;
    static colnr_t	old_leftcol = 0;

    if (check && curwin->w_p_scb)
    {
	if (did_syncbind)
	    did_syncbind = FALSE;
	else if (curwin == old_curwin)
	{
	    /*
	     * synchronize other windows, as necessary according to
	     * 'scrollbind'
	     */
	    if (curwin->w_buffer == old_buf
		    && (curwin->w_topline != old_topline
			|| curwin->w_leftcol != old_leftcol))
	    {
		check_scrollbind(curwin->w_topline - old_topline,
			(long)(curwin->w_leftcol - old_leftcol));
	    }
	}
	else if (vim_strchr(p_sbo, 'j')) /* jump flag set in 'scrollopt' */
	{
	    /*
	     * When switching between windows, make sure that the relative
	     * vertical offset is valid for the new window.  The relative
	     * offset is invalid whenever another 'scrollbind' window has
	     * scrolled to a point that would force the current window to
	     * scroll past the beginning or end of its buffer.  When the
	     * resync is performed, some of the other 'scrollbind' windows may
	     * need to jump so that the current window's relative position is
	     * visible on-screen.
	     */
	    check_scrollbind(curwin->w_topline - curwin->w_scbind_pos, 0L);
	}
	curwin->w_scbind_pos = curwin->w_topline;
    }

    old_curwin = curwin;
    old_topline = curwin->w_topline;
    old_buf = curwin->w_buffer;
    old_leftcol = curwin->w_leftcol;
}

/*
 * Synchronize any windows that have "scrollbind" set, based on the
 * number of rows by which the current window has changed
 * (1998-11-02 16:21:01  R. Edward Ralston <eralston@computer.org>)
 */
    void
check_scrollbind(topline_diff, leftcol_diff)
    linenr_t	topline_diff;
    long	leftcol_diff;
{
    int		want_ver;
    int		want_hor;
    win_t	*old_curwin = curwin;
    buf_t	*old_curbuf = curbuf;
#ifdef FEAT_VISUAL
    int		old_VIsual_select = VIsual_select;
    int		old_VIsual_active = VIsual_active;
#endif
    colnr_t	tgt_leftcol = curwin->w_leftcol;
    long	topline;
    long	y;

    /*
     * check 'scrollopt' string for vertical and horizontal scroll options
     */
    want_ver = (vim_strchr(p_sbo, 'v') && topline_diff);
    want_hor = (vim_strchr(p_sbo, 'h') && (leftcol_diff || topline_diff));

    /*
     * loop through the scrollbound windows and scroll accordingly
     */
#ifdef FEAT_VISUAL
    VIsual_select = VIsual_active = 0;
#endif
    for (curwin = firstwin; curwin; curwin = curwin->w_next)
    {
	curbuf = curwin->w_buffer;
	if (curwin != old_curwin  /* don't scroll in original window */
		&& curwin->w_p_scb)
	{
	    /*
	     * do the vertical scroll
	     */
	    if (want_ver)
	    {
		curwin->w_scbind_pos += topline_diff;
		topline = curwin->w_scbind_pos;
		if (topline > curbuf->b_ml.ml_line_count - p_so)
		    topline = curbuf->b_ml.ml_line_count - p_so;
		if (topline < 1)
		    topline = 1;

		y = topline - curwin->w_topline;
		if (y > 0)
		    scrollup(y, TRUE);
		else
		    scrolldown(-y, TRUE);

		redraw_later(VALID);
		cursor_correct();
#ifdef FEAT_WINDOWS
		curwin->w_redr_status = TRUE;
#endif
	    }

	    /*
	     * do the horizontal scroll
	     */
	    if (want_hor && curwin->w_leftcol != tgt_leftcol)
	    {
		curwin->w_leftcol = tgt_leftcol;
		leftcol_changed();
	    }
	}
    }

    /*
     * reset current-window
     */
#ifdef FEAT_VISUAL
    VIsual_select = old_VIsual_select;
    VIsual_active = old_VIsual_active;
#endif
    curwin = old_curwin;
    curbuf = old_curbuf;
}
#endif /* #ifdef FEAT_SCROLLBIND */

/*
 * Command character that's ignored.
 * Used for CTRL-Q and CTRL-S to avoid problems with terminals that use
 * xon/xoff
 */
/*ARGSUSED */
    static void
nv_ignore(cap)
    cmdarg_t	*cap;
{
}

/*
 * Command character doesn't exist.
 */
    static void
nv_error(cap)
    cmdarg_t	*cap;
{
    clearopbeep(cap->oap);
}

/*
 * <Help> and <F1> commands.
 */
    static void
nv_help(cap)
    cmdarg_t	*cap;
{
    if (!checkclearopq(cap->oap))
	ex_help(NULL);
}

/*
 * CTRL-A and CTRL-X: Add or subtract from letter or number under cursor.
 */
    static void
nv_addsub(cap)
    cmdarg_t	*cap;
{
    if (!checkclearopq(cap->oap)
	    && do_addsub((int)cap->cmdchar, cap->count1) == OK)
	prep_redo_cmd(cap);
}

/*
 * CTRL-F, CTRL-B, etc: Scroll page up or down.
 */
    static void
nv_page(cap)
    cmdarg_t	*cap;
{
    if (!checkclearop(cap->oap))
	(void)onepage(cap->arg, cap->count1);
}

/*
 * Implementation of "gd" and "gD" command.
 */
    static void
nv_gd(oap, nchar)
    oparg_t   *oap;
    int	    nchar;
{
    int		len;
    char_u	*pat;
    pos_t	old_pos;
    int		t;
    int		save_p_ws;
    int		save_p_scs;
    char_u	*ptr;

    if ((len = find_ident_under_cursor(&ptr, FIND_IDENT)) == 0 ||
					       (pat = alloc(len + 5)) == NULL)
    {
	clearopbeep(oap);
	return;
    }
    sprintf((char *)pat, vim_iswordc(*ptr) ? "\\<%.*s\\>" :
	    "%.*s", len, ptr);
    old_pos = curwin->w_cursor;
    save_p_ws = p_ws;
    save_p_scs = p_scs;
    p_ws = FALSE;	/* don't wrap around end of file now */
    p_scs = FALSE;	/* don't switch ignorecase off now */
#ifdef FEAT_COMMENTS
    fo_do_comments = TRUE;
#endif

    /*
     * With "gD" go to line 1.
     * With "gd" Search back for the start of the current function, then go
     * back until a blank line.  If this fails go to line 1.
     */
    if (nchar == 'D' || !findpar(oap, BACKWARD, 1L, '{', FALSE))
    {
	setpcmark();			/* Set in findpar() otherwise */
	curwin->w_cursor.lnum = 1;
    }
    else
    {
	while (curwin->w_cursor.lnum > 1 && *skipwhite(ml_get_curline()) != NUL)
	    --curwin->w_cursor.lnum;
    }
    curwin->w_cursor.col = 0;

    /* Search forward for the identifier, ignore comment lines. */
    while ((t = searchit(curbuf, &curwin->w_cursor, FORWARD, pat, 1L, 0,
							     RE_LAST)) != FAIL
#ifdef FEAT_COMMENTS
	    && get_leader_len(ml_get_curline(), NULL, FALSE)
#endif
	    && old_pos.lnum > curwin->w_cursor.lnum)
	++curwin->w_cursor.lnum;
    if (t == FAIL || old_pos.lnum <= curwin->w_cursor.lnum)
    {
	clearopbeep(oap);
	curwin->w_cursor = old_pos;
    }
    else
	curwin->w_set_curswant = TRUE;

    vim_free(pat);
    p_ws = save_p_ws;
    p_scs = save_p_scs;
#ifdef FEAT_COMMENTS
    fo_do_comments = FALSE;
#endif
}

/*
 * Move 'dist' lines in direction 'dir', counting lines by *screen*
 * lines rather than lines in the file.
 * 'dist' must be positive.
 *
 * Return OK if able to move cursor, FAIL otherwise.
 */
    static int
nv_screengo(oap, dir, dist)
    oparg_t	*oap;
    int		dir;
    long	dist;
{
    int		linelen = linetabsize(ml_get_curline());
    int		retval = OK;
    int		atend = FALSE;
    int		n;
    int		col_off1;	/* margin offset for first screen line */
    int		col_off2;	/* margin offset for wrapped screen line */
    int		width1;		/* text width for first screen line */
    int		width2;		/* test width for wrapped screen line */

    oap->motion_type = MCHAR;
    oap->inclusive = FALSE;

    col_off1 = curwin_col_off();
    col_off2 = col_off1 - curwin_col_off2();
    width1 = W_WIDTH(curwin) - col_off1;
    width2 = W_WIDTH(curwin) - col_off2;

#ifdef FEAT_VERTSPLIT
    if (curwin->w_width != 0)
    {
#endif
      /*
       * Instead of sticking at the last character of the buffer line we
       * try to stick in the last column of the screen.
       */
      if (curwin->w_curswant == MAXCOL)
      {
	atend = TRUE;
	validate_virtcol();
	if (width1 <= 0)
	    curwin->w_curswant = 0;
	else
	{
	    curwin->w_curswant = width1 - 1;
	    if (curwin->w_virtcol > curwin->w_curswant)
		curwin->w_curswant += ((curwin->w_virtcol
			     - curwin->w_curswant - 1) / width2 + 1) * width2;
	}
      }
      else
      {
	if (linelen > width1)
	    n = ((linelen - width1 - 1) / width2 + 1) * width2 + width1;
	else
	    n = width1;
	if (curwin->w_curswant > (colnr_t)n + 1)
	    curwin->w_curswant -= ((curwin->w_curswant - n) / width2 + 1)
								     * width2;
      }

      while (dist--)
      {
	if (dir == BACKWARD)
	{
	    if ((long)curwin->w_curswant >= width2)
		/* move back within line */
		curwin->w_curswant -= width2;
	    else
	    {
		/* to previous line */
		if (curwin->w_cursor.lnum == 1)
		{
		    retval = FAIL;
		    break;
		}
		--curwin->w_cursor.lnum;
		linelen = linetabsize(ml_get_curline());
		if (linelen > width1)
		    curwin->w_curswant += (((linelen - width1 - 1) / width2)
								+ 1) * width2;
	    }
	}
	else /* dir == FORWARD */
	{
	    if (linelen > width1)
		n = ((linelen - width1 - 1) / width2 + 1) * width2 + width1;
	    else
		n = width1;
	    if (curwin->w_curswant + width2 < (colnr_t)n)
		/* move forward within line */
		curwin->w_curswant += width2;
	    else
	    {
		/* to next line */
		if (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count)
		{
		    retval = FAIL;
		    break;
		}
		curwin->w_cursor.lnum++;
		curwin->w_curswant %= width2;
	    }
	}
      }
#ifdef FEAT_VERTSPLIT
    }
#endif

    coladvance(curwin->w_curswant);

#if defined(FEAT_LINEBREAK) || defined(FEAT_MBYTE)
    if (curwin->w_cursor.col > 0 && curwin->w_p_wrap)
    {
	/*
	 * Check for landing on a character that got split at the end of the
	 * last line.  We want to advance a screenline, not end up in the same
	 * screenline or move two screenlines.
	 */
	validate_virtcol();
	if (curwin->w_virtcol > curwin->w_curswant
		&& (curwin->w_curswant < (colnr_t)width1
		    ? (curwin->w_curswant > (colnr_t)width1 / 2)
		    : ((curwin->w_curswant - width1) % width2
						      > (colnr_t)width2 / 2)))
	    --curwin->w_cursor.col;
    }
#endif

    if (atend)
	curwin->w_curswant = MAXCOL;	    /* stick in the last column */

    return retval;
}

#ifdef FEAT_MOUSE
/*
 * Mouse scroll wheel: Default action is to scroll three lines, or one page
 * when Shift or Ctrl is used.
 * K_MOUSEUP (cap->arg == TRUE) or K_MOUSEDOWN (cap->arg == FALSE)
 */
    static void
nv_mousescroll(cap)
    cmdarg_t	*cap;
{
    if (mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL))
    {
	(void)onepage(cap->arg ? FORWARD : BACKWARD, 1L);
    }
    else
    {
	cap->count1 = 3;
	cap->count0 = 3;
	nv_scroll_line(cap);
    }
}

/*
 * Mouse clicks and drags.
 */
    static void
nv_mouse(cap)
    cmdarg_t	*cap;
{
    (void)do_mouse(cap->oap, cap->cmdchar, BACKWARD, cap->count1, 0);
}
#endif

/*
 * Handle CTRL-E and CTRL-Y commands: scroll a line up or down.
 * cap->arg must be TRUE for CTRL-E.
 */
    static void
nv_scroll_line(cap)
    cmdarg_t	*cap;
{
    if (!checkclearop(cap->oap))
	scroll_redraw(cap->arg, cap->count1);
}

/*
 * Scroll "count" lines up or down, and redraw.
 */
    void
scroll_redraw(up, count)
    int		up;
    long	count;
{
    linenr_t	prev_topline = curwin->w_topline;
    linenr_t	prev_lnum = curwin->w_cursor.lnum;

    if (up)
	scrollup(count, TRUE);
    else
	scrolldown(count, TRUE);
    if (p_so)
    {
	cursor_correct();
	update_topline();
	/* If moved back to where we were, at least move the cursor, otherwise
	 * we get stuck at one position.  Don't move the cursor up if the
	 * first line of the buffer is already on the screen */
	while (curwin->w_topline == prev_topline)
	{
	    if (up)
	    {
		if (curwin->w_cursor.lnum > prev_lnum
			|| cursor_down(1L, FALSE) == FAIL)
		    break;
	    }
	    else
	    {
		if (curwin->w_cursor.lnum < prev_lnum
			|| prev_topline == 1L
			|| cursor_up(1L, FALSE) == FAIL)
		    break;
	    }
	    /* Now recompute the topline, otherwise the cursor will be moved
	     * back again. */
	    update_topline();
	}
    }
    if (curwin->w_cursor.lnum != prev_lnum)
	coladvance(curwin->w_curswant);
    redraw_later(VALID);
}

/*
 * Commands that start with "z".
 */
    static void
nv_zet(cap)
    cmdarg_t  *cap;
{
    long	n;
    colnr_t	col;
    int		nchar = cap->nchar;
#ifdef FEAT_FOLDING
    long	old_fdl = curwin->w_p_fdl;
    int		old_fen = curwin->w_p_fen;
#endif

    if (vim_isdigit(nchar))
    {
	if (checkclearop(cap->oap))
	    return;
	n = nchar - '0';
	for (;;)
	{
#ifdef USE_ON_FLY_SCROLL
	    dont_scroll = TRUE;		/* disallow scrolling here */
#endif
	    ++no_mapping;
	    ++allow_keys;   /* no mapping for nchar, but allow key codes */
	    nchar = safe_vgetc();
#ifdef FEAT_LANGMAP
	    LANGMAP_ADJUST(nchar, TRUE);
#endif
	    --no_mapping;
	    --allow_keys;
#ifdef FEAT_CMDL_INFO
	    (void)add_to_showcmd(nchar);
#endif
	    if (nchar == K_DEL || nchar == K_KDEL)
		n /= 10;
	    else if (vim_isdigit(nchar))
		n = n * 10 + (nchar - '0');
	    else if (nchar == CR)
	    {
#ifdef FEAT_GUI
		need_mouse_correct = TRUE;
#endif
		win_setheight((int)n);
		break;
	    }
	    else if (nchar == 'l'
		    || nchar == 'h'
		    || nchar == K_LEFT
		    || nchar == K_RIGHT)
	    {
		cap->count1 = n ? n * cap->count1 : cap->count1;
		goto dozet;
	    }
	    else
	    {
		clearopbeep(cap->oap);
		break;
	    }
	}
	cap->oap->op_type = OP_NOP;
	return;
    }

dozet:
    if (
#ifdef FEAT_FOLDING
	    /* "zf" and "zF" are always an operator, "zo", "zO", "zc" and "zC"
	     * only in Visual mode. */
	    cap->nchar != 'f' && cap->nchar != 'F'
	    && !(VIsual_active && vim_strchr((char_u *)"cCoO", cap->nchar))
	    &&
#endif
	    checkclearop(cap->oap))
	return;

    /*
     * For "z+", "z<CR>", "zt", "z.", "zz", "z^", "z-", "zb":
     * If line number given, set cursor.
     */
    if ((vim_strchr((char_u *)"+\r\nt.z^-b", nchar) != NULL)
	    && cap->count0
	    && cap->count0 != curwin->w_cursor.lnum)
    {
	setpcmark();
	if (cap->count0 > curbuf->b_ml.ml_line_count)
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	else
	    curwin->w_cursor.lnum = cap->count0;
    }

    switch (nchar)
    {
		/* "z+", "z<CR>" and "zt": put cursor at top of screen */
    case '+':
		if (cap->count0 == 0)
		{
		    /* No count given: put cursor at the line below screen */
		    validate_botline();	/* make sure w_botline is valid */
		    if (curwin->w_botline > curbuf->b_ml.ml_line_count)
			curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
		    else
			curwin->w_cursor.lnum = curwin->w_botline;
		}
		/* FALLTHROUGH */
    case NL:
    case CR:
    case K_KENTER:
		beginline(BL_WHITE | BL_FIX);
		/* FALLTHROUGH */

    case 't':	scroll_cursor_top(0, TRUE);
		redraw_later(VALID);
		break;

		/* "z." and "zz": put cursor in middle of screen */
    case '.':	beginline(BL_WHITE | BL_FIX);
		/* FALLTHROUGH */

    case 'z':	scroll_cursor_halfway(TRUE);
		redraw_later(VALID);
		break;

		/* "z^", "z-" and "zb": put cursor at bottom of screen */
    case '^':	/* Strange Vi behavior: <count>z^ finds line at top of window
		 * when <count> is at bottom of window, and puts that one at
		 * bottom of window. */
		if (cap->count0 != 0)
		{
		    scroll_cursor_bot(0, TRUE);
		    curwin->w_cursor.lnum = curwin->w_topline;
		}
		else if (curwin->w_topline == 1)
		    curwin->w_cursor.lnum = 1;
		else
		    curwin->w_cursor.lnum = curwin->w_topline - 1;
		/* FALLTHROUGH */
    case '-':
		beginline(BL_WHITE | BL_FIX);
		/* FALLTHROUGH */

    case 'b':	scroll_cursor_bot(0, TRUE);
		redraw_later(VALID);
		break;

		/* "zH" - scroll screen right half-page */
    case 'H':
		cap->count1 *= W_WIDTH(curwin) / 2;
		/* FALLTHROUGH */

		/* "zh" - scroll screen to the right */
    case 'h':
    case K_LEFT:
		if (!curwin->w_p_wrap)
		{
		    if ((colnr_t)cap->count1 > curwin->w_leftcol)
			curwin->w_leftcol = 0;
		    else
			curwin->w_leftcol -= (colnr_t)cap->count1;
		    leftcol_changed();
		}
		break;

		/* "zL" - scroll screen left half-page */
    case 'L':	cap->count1 *= W_WIDTH(curwin) / 2;
		/* FALLTHROUGH */

		/* "zl" - scroll screen to the left */
    case 'l':
    case K_RIGHT:
		if (!curwin->w_p_wrap)
		{
		    /* scroll the window left */
		    curwin->w_leftcol += (colnr_t)cap->count1;
		    leftcol_changed();
		}
		break;

		/* "zs" - scroll screen, cursor at the start */
    case 's':	if (!curwin->w_p_wrap)
		{
		    getvcol(curwin, &curwin->w_cursor, &col, NULL, NULL);
		    if (curwin->w_leftcol != col)
		    {
			curwin->w_leftcol = col;
			redraw_later(NOT_VALID);
		    }
		}
		break;

		/* "ze" - scroll screen, cursor at the end */
    case 'e':	if (!curwin->w_p_wrap)
		{
		    getvcol(curwin, &curwin->w_cursor, NULL, NULL, &col);
		    n = W_WIDTH(curwin) - curwin_col_off();
		    if ((long)col < n)
			col = 0;
		    else
			col = col - n + 1;
		    if (curwin->w_leftcol != col)
		    {
			curwin->w_leftcol = col;
			redraw_later(NOT_VALID);
		    }
		}
		break;

#ifdef FEAT_FOLDING
		/* "zF": create fold command */
		/* "zf": create fold operator */
    case 'F':
    case 'f':   if (!foldmethodIsManual(curwin))
		{
		    EMSG(_("Cannot create fold with current 'foldmethod'"));
		    clearopbeep(cap->oap);
		}
		else
		{
		    cap->nchar = 'f';
		    nv_operator(cap);
		    curwin->w_p_fen = TRUE;

		    /* "zF" is like "zfzf" */
		    if (nchar == 'F' && cap->oap->op_type == OP_FOLD)
		    {
			nv_operator(cap);
			finish_op = TRUE;
		    }
		}
		break;

		/* "zd": delete fold at cursor */
    case 'd':	if (!foldmethodIsManual(curwin))
		    EMSG(_("Cannot delete fold with current 'foldmethod'"));
		else
		    deleteFold(curwin->w_cursor.lnum);
		break;

		/* "zD": delete all folds */
    case 'D':	if (!foldmethodIsManual(curwin))
		   EMSG(_("Cannot delete fold with current 'foldmethod'"));
		else
		{
		    clearFolding(curwin);
		    changed_window_setting();
		}
		break;

		/* "zn": fold none: reset 'foldenable' */
    case 'n':	curwin->w_p_fen = FALSE;
		break;

		/* "zN": fold Normal: set 'foldenable' */
    case 'N':	curwin->w_p_fen = TRUE;
		break;

		/* "zi": invert folding: toggle 'foldenable' */
    case 'i':	curwin->w_p_fen = !curwin->w_p_fen;
		break;

		/* "za": open fold at cursor */
    case 'a':	if (hasFolding(curwin->w_cursor.lnum, NULL, NULL))
		    openFold(curwin->w_cursor.lnum);
		else
		{
		    closeFold(curwin->w_cursor.lnum);
		    curwin->w_p_fen = TRUE;
		}
		break;

		/* "zA": open fold at cursor recursively */
    case 'A':	if (hasFolding(curwin->w_cursor.lnum, NULL, NULL))
		    openFoldRecurse(curwin->w_cursor.lnum);
		else
		{
		    closeFoldRecurse(curwin->w_cursor.lnum);
		    curwin->w_p_fen = TRUE;
		}
		break;

		/* "zo": open fold at cursor or Visual area */
    case 'o':	if (VIsual_active)
		    nv_operator(cap);
		else
		    openFold(curwin->w_cursor.lnum);
		break;

		/* "zO": open fold recursively */
    case 'O':	if (VIsual_active)
		    nv_operator(cap);
		else
		    openFoldRecurse(curwin->w_cursor.lnum);
		break;

		/* "zc": close fold at cursor or Visual area */
    case 'c':	if (VIsual_active)
		    nv_operator(cap);
		else
		    closeFold(curwin->w_cursor.lnum);
		curwin->w_p_fen = TRUE;
		break;

		/* "zC": close fold recursively */
    case 'C':	if (VIsual_active)
		    nv_operator(cap);
		else
		    closeFoldRecurse(curwin->w_cursor.lnum);
		curwin->w_p_fen = TRUE;
		break;

		/* "zm": fold more */
    case 'm':	if (curwin->w_p_fdl > 0)
		    --curwin->w_p_fdl;
		old_fdl = -1;		/* force an update */
		curwin->w_p_fen = TRUE;
		break;

		/* "zM": close all folds */
    case 'M':	curwin->w_p_fdl = 0;
		old_fdl = -1;		/* force an update */
		curwin->w_p_fen = TRUE;
		break;

		/* "zr": reduce folding */
    case 'r':	++curwin->w_p_fdl;
		break;

		/* "zR": open all folds */
    case 'R':	curwin->w_p_fdl = getDeepestNesting();
		old_fdl = -1;		/* force an update */
		break;

#endif /* FEAT_FOLDING */

    default:	clearopbeep(cap->oap);
    }

#ifdef FEAT_FOLDING
    /* Redraw when 'foldenable' or 'foldlevel' changed */
    if (old_fen != curwin->w_p_fen)
	changed_window_setting();
    if (old_fdl != curwin->w_p_fdl)
	newFoldLevel();
#endif
}

#ifdef FEAT_GUI
/*
 * Vertical scrollbar movement.
 */
    static void
nv_ver_scrollbar(cap)
    cmdarg_t	*cap;
{
    if (cap->oap->op_type != OP_NOP)
	clearopbeep(cap->oap);

    /* Even if an operator was pending, we still want to scroll */
    gui_do_scroll();
}

/*
 * Horizontal scrollbar movement.
 */
    static void
nv_hor_scrollbar(cap)
    cmdarg_t	*cap;
{
    if (cap->oap->op_type != OP_NOP)
	clearopbeep(cap->oap);

    /* Even if an operator was pending, we still want to scroll */
    gui_do_horiz_scroll();
}
#endif

/*
 * "Q" command.
 */
    static void
nv_exmode(cap)
    cmdarg_t	*cap;
{
    /*
     * Ignore 'Q' in Visual mode, just give a beep.
     */
#ifdef FEAT_VISUAL
    if (VIsual_active)
	vim_beep();
    else
#endif
	if (!checkclearop(cap->oap))
	do_exmode(FALSE);
}

/*
 * Handle a ":" command.
 */
    static void
nv_colon(cap)
    cmdarg_t  *cap;
{
    int	    old_p_im;

#ifdef FEAT_VISUAL
    if (VIsual_active)
	nv_operator(cap);
    else
#endif
	if (!checkclearop(cap->oap))
    {
	/* translate "count:" into ":.,.+(count - 1)" */
	if (cap->count0)
	{
	    stuffcharReadbuff('.');
	    if (cap->count0 > 1)
	    {
		stuffReadbuff((char_u *)",.+");
		stuffnumReadbuff((long)cap->count0 - 1L);
	    }
	}

	/* When typing, don't type below an old message */
	if (KeyTyped)
	    compute_cmdrow();

	old_p_im = p_im;

	/* get a command line and execute it */
	do_cmdline(NULL, getexline, NULL, 0);

	/* If 'insertmode' changed, enter or exit Insert mode */
	if (p_im != old_p_im)
	{
	    if (p_im)
		restart_edit = 'i';
	    else
		restart_edit = 0;
	}
    }
}

/*
 * Handle CTRL-G command.
 */
    static void
nv_ctrlg(cap)
    cmdarg_t *cap;
{
#ifdef FEAT_VISUAL
    if (VIsual_active)	/* toggle Selection/Visual mode */
    {
	VIsual_select = !VIsual_select;
	showmode();
    }
    else
#endif
	if (!checkclearop(cap->oap))
	/* print full name if count given or :cd used */
	fileinfo((int)cap->count0, FALSE, TRUE);
}

/*
 * Handle CTRL-H <Backspace> command.
 */
    static void
nv_ctrlh(cap)
    cmdarg_t *cap;
{
#ifdef FEAT_VISUAL
    if (VIsual_active && VIsual_select)
    {
	cap->cmdchar = 'x';	/* BS key behaves like 'x' in Select mode */
	v_visop(cap);
    }
    else
#endif
	nv_left(cap);
}

/*
 * CTRL-L: clear screen and redraw.
 */
    static void
nv_clear(cap)
    cmdarg_t	*cap;
{
    if (!checkclearop(cap->oap))
    {
#if defined(__BEOS__) && !USE_THREAD_FOR_INPUT_WITH_TIMEOUT
	/*
	 * Right now, the BeBox doesn't seem to have an easy way to detect
	 * window resizing, so we cheat and make the user detect it
	 * manually with CTRL-L instead
	 */
	ui_get_shellsize();
#endif
#ifdef FEAT_SYN_HL
	/* Clear all syntax states to force resyncing. */
	syn_stack_free_all(curbuf);
#endif
	redraw_later(CLEAR);
    }
}

/*
 * CTRL-O: In Select mode: switch to Visual mode for one command.
 * Otherwise: Go to older pcmark.
 */
    static void
nv_ctrlo(cap)
    cmdarg_t	*cap;
{
#ifdef FEAT_VISUAL
    if (VIsual_active && VIsual_select)
    {
	VIsual_select = FALSE;
	showmode();
	restart_VIsual_select = 2;	/* restart Select mode later */
    }
    else
#endif
    {
	cap->count1 = -cap->count1;
	nv_pcmark(cap);
    }
}

/*
 * CTRL-^ command, short for ":e #"
 */
    static void
nv_hat(cap)
    cmdarg_t	*cap;
{
    if (!checkclearopq(cap->oap))
	(void)buflist_getfile((int)cap->count0, (linenr_t)0,
						GETF_SETMARK|GETF_ALT, FALSE);
}

/*
 * "Z" commands.
 */
    static void
nv_Zet(cap)
    cmdarg_t *cap;
{
    if (!checkclearopq(cap->oap))
    {
	switch (cap->nchar)
	{
			/* "ZZ": equivalent to ":x". */
	    case 'Z':	stuffReadbuff((char_u *)":x\n");
			break;

			/* "ZQ": equivalent to ":q!" (Elvis compatible). */
	    case 'Q':	stuffReadbuff((char_u *)":q!\n");
			break;

	    default:	clearopbeep(cap->oap);
	}
    }
}

/*
 * Handle the commands that use the word under the cursor.
 * [g] CTRL-]	:ta to current identifier
 * [g] 'K'	run program for current identifier
 * [g] '*'	/ to current identifier or string
 * [g] '#'	? to current identifier or string
 *  g  ']'	:tselect for current identifier
 */
    static void
nv_ident(cap)
    cmdarg_t	*cap;
{
    char_u	*ptr = NULL;
    int		n = 0;		/* init for GCC */
    int		cmdchar;
    int		g_cmd;		/* "g" command */
    char_u	*aux_ptr;
    int		isman;
    int		isman_s;

    if (cap->cmdchar == 'g')	/* "g*", "g#", "g]" and "gCTRL-]" */
    {
	cmdchar = cap->nchar;
	g_cmd = TRUE;
    }
    else
    {
	cmdchar = cap->cmdchar;
	g_cmd = FALSE;
    }

    if (cmdchar == POUND)	/* the pound sign, '#' for English keyboards */
	cmdchar = '#';

    /*
     * The "]", "CTRL-]" and "K" commands accept an argument in Visual mode.
     */
    if (cmdchar == ']' || cmdchar == Ctrl_RSB || cmdchar == 'K')
    {
#ifdef FEAT_VISUAL
	if (VIsual_active && get_visual_text(cap, &ptr, &n) == FAIL)
	    return;
#endif
	if (checkclearopq(cap->oap))
	    return;
    }

    if (ptr == NULL && (n = find_ident_under_cursor(&ptr,
		    (cmdchar == '*' || cmdchar == '#')
				 ? FIND_IDENT|FIND_STRING : FIND_IDENT)) == 0)
    {
	clearop(cap->oap);
	return;
    }

    isman = (STRCMP(p_kp, "man") == 0);
    isman_s = (STRCMP(p_kp, "man -s") == 0);
    if (cap->count0 && !(cmdchar == 'K' && (isman || isman_s))
	    && !(cmdchar == '*' || cmdchar == '#'))
	stuffnumReadbuff(cap->count0);
    switch (cmdchar)
    {
	case '*':
	case '#':
	    /*
	     * Put cursor at start of word, makes search skip the word
	     * under the cursor.
	     * Call setpcmark() first, so "*``" puts the cursor back where
	     * it was.
	     */
	    setpcmark();
	    curwin->w_cursor.col = ptr - ml_get_curline();

	    if (!g_cmd && vim_iswordc(*ptr))
		stuffReadbuff((char_u *)"\\<");
	    no_smartcase = TRUE;	/* don't use 'smartcase' now */
	    break;

	case 'K':
	    if (*p_kp == NUL)
		stuffReadbuff((char_u *)":he ");
	    else
	    {
		stuffReadbuff((char_u *)":! ");
		if (!cap->count0 && isman_s)
		    stuffReadbuff((char_u *)"man");
		else
		    stuffReadbuff(p_kp);
		stuffReadbuff((char_u *)" ");
		if (cap->count0 && (isman || isman_s))
		{
		    stuffnumReadbuff(cap->count0);
		    stuffReadbuff((char_u *)" ");
		}
	    }
	    break;

	case ']':
#ifdef FEAT_CSCOPE
	    if (p_cst)
		stuffReadbuff((char_u *)":cstag ");
	    else
#endif
	    stuffReadbuff((char_u *)":ts ");
	    break;

	default:
	    if (curbuf->b_help)
		stuffReadbuff((char_u *)":he ");
	    else if (g_cmd)
		stuffReadbuff((char_u *)":tj ");
	    else
		stuffReadbuff((char_u *)":ta ");
    }

    /*
     * Now grab the chars in the identifier
     */
    if (cmdchar == '*' || cmdchar == '#')
	aux_ptr = (char_u *)(p_magic ? "/?.*~[^$\\" : "/?^$\\");
    else if (cmdchar == 'K' && *p_kp != NUL)
	aux_ptr = escape_chars;
    else
	/* Don't escape spaces and Tabs in a tag with a backslash */
	aux_ptr = (char_u *)"\\|\"";
    while (n--)
    {
	/* put a backslash before \ and some others */
	if (vim_strchr(aux_ptr, *ptr) != NULL)
	    stuffcharReadbuff('\\');
	/* don't interpret the characters as edit commands */
#ifdef EBCDIC
	else if (CtrlChar(*ptr) != 0)
#else
	else if (*ptr < ' ' || *ptr > '~')
#endif
	    stuffcharReadbuff(Ctrl_V);
	stuffcharReadbuff(*ptr++);
    }

    if (       !g_cmd
	    && (cmdchar == '*' || cmdchar == '#')
	    && vim_iswordc(ptr[-1]))
	stuffReadbuff((char_u *)"\\>");
    stuffReadbuff((char_u *)"\n");

    /*
     * The search commands may be given after an operator.  Therefore they
     * must be executed right now.
     */
    if (cmdchar == '*' || cmdchar == '#')
    {
	if (cmdchar == '*')
	    cap->cmdchar = '/';
	else
	    cap->cmdchar = '?';
	cap->arg = TRUE;
	nv_search(cap);
    }
}

#ifdef FEAT_VISUAL
/*
 * Get visually selected text, within one line only.
 * Returns FAIL if more than one line selected.
 */
    static int
get_visual_text(cap, pp, lenp)
    cmdarg_t	*cap;
    char_u	**pp;	    /* return: start of selected text */
    int		*lenp;	    /* return: length of selected text */
{
    if (VIsual_mode != 'V')
	unadjust_for_sel();
    if (VIsual.lnum != curwin->w_cursor.lnum)
    {
	clearopbeep(cap->oap);
	return FAIL;
    }
    if (VIsual_mode == 'V')
    {
	*pp = ml_get_curline();
	*lenp = STRLEN(*pp);
    }
    else if (lt(curwin->w_cursor, VIsual))
    {
	*pp = ml_get_pos(&curwin->w_cursor);
	*lenp = VIsual.col - curwin->w_cursor.col + 1;
    }
    else
    {
	*pp = ml_get_pos(&VIsual);
	*lenp = curwin->w_cursor.col - VIsual.col + 1;
    }
    reset_VIsual_and_resel();
    return OK;
}
#endif

/*
 * CTRL-T: backwards in tag stack
 */
    static void
nv_tagpop(cap)
    cmdarg_t	*cap;
{
    if (!checkclearopq(cap->oap))
	do_tag((char_u *)"", DT_POP, (int)cap->count1, FALSE, TRUE);
}

/*
 * Handle scrolling command 'H', 'L' and 'M'.
 */
    static void
nv_scroll(cap)
    cmdarg_t  *cap;
{
    int		used = 0;
    long	n;
#ifdef FEAT_FOLDING
    linenr_t	lnum;
#endif

    cap->oap->motion_type = MLINE;
    setpcmark();

    if (cap->cmdchar == 'L')
    {
	validate_botline();	    /* make sure curwin->w_botline is valid */
	curwin->w_cursor.lnum = curwin->w_botline - 1;
	if (cap->count1 - 1 >= curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	else
	    curwin->w_cursor.lnum -= cap->count1 - 1;
    }
    else
    {
	if (cap->cmdchar == 'M')
	{
	    validate_botline();	    /* make sure w_empty_rows is valid */
	    for (n = 0; curwin->w_topline + n < curbuf->b_ml.ml_line_count; ++n)
	    {
		if ((used += plines(curwin->w_topline + n)) >=
			    (curwin->w_height - curwin->w_empty_rows + 1) / 2)
		    break;
#ifdef FEAT_FOLDING
		if (hasFolding(curwin->w_topline + n, NULL, &lnum))
		    n = lnum - curwin->w_topline;
#endif
	    }
	    if (n && used > curwin->w_height)
		--n;
	}
	else
	    n = cap->count1 - 1;
	curwin->w_cursor.lnum = curwin->w_topline + n;
	if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
    }

    cursor_correct();	/* correct for 'so' */
    beginline(BL_SOL | BL_FIX);
}

/*
 * <Right> and <C-Right> commands.
 */
    static void
nv_kright(cap)
    cmdarg_t	*cap;
{
    if (mod_mask & MOD_MASK_CTRL)
    {
	cap->arg = TRUE;
	nv_wordcmd(cap);
    }
    else
	nv_right(cap);
}

/*
 * Cursor right commands.
 */
    static void
nv_right(cap)
    cmdarg_t	*cap;
{
    long	n;
#ifdef FEAT_VISUAL
    int		past_line;
#else
# define past_line 0
#endif

    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = FALSE;
#ifdef FEAT_VISUAL
    past_line = (VIsual_active && *p_sel != 'o');

# ifdef FEAT_VIRTUALEDIT
    /*
     * In virtual mode, there's no such thing as "past_line", as lines are
     * (theoretically) infinitly long.
     */
    if (virtual_active())
	past_line = 0;
# endif
#endif

    for (n = cap->count1; n > 0; --n)
    {
	if ((!past_line && oneright() == FAIL)
		|| (past_line && *ml_get_cursor() == NUL))
	{
	    /*
	     *	  <Space> wraps to next line if 'whichwrap' bit 1 set.
	     *	      'l' wraps to next line if 'whichwrap' bit 2 set.
	     * CURS_RIGHT wraps to next line if 'whichwrap' bit 3 set
	     */
	    if (       ((cap->cmdchar == ' '
			    && vim_strchr(p_ww, 's') != NULL)
			|| (cap->cmdchar == 'l'
			    && vim_strchr(p_ww, 'l') != NULL)
			|| (cap->cmdchar == K_RIGHT
			    && vim_strchr(p_ww, '>') != NULL))
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
	    {
		/* When deleting we also count the NL as a character.
		 * Set cap->oap->inclusive when last char in the line is
		 * included, move to next line after that */
		if (	   (cap->oap->op_type == OP_DELETE
			    || cap->oap->op_type == OP_CHANGE)
			&& !cap->oap->inclusive
			&& !lineempty(curwin->w_cursor.lnum))
		    cap->oap->inclusive = TRUE;
		else
		{
		    ++curwin->w_cursor.lnum;
		    curwin->w_cursor.col = 0;
#ifdef FEAT_VIRTUALEDIT
		    curwin->w_coladd = 0;
#endif
		    curwin->w_set_curswant = TRUE;
		    cap->oap->inclusive = FALSE;
		}
		continue;
	    }
	    if (cap->oap->op_type == OP_NOP)
	    {
		/* Only beep and flush if not moved at all */
		if (n == cap->count1)
		    beep_flush();
	    }
	    else
	    {
		if (!lineempty(curwin->w_cursor.lnum))
		    cap->oap->inclusive = TRUE;
	    }
	    break;
	}
#ifdef FEAT_VISUAL
	else if (past_line)
	{
	    curwin->w_set_curswant = TRUE;
# ifdef FEAT_VIRTUALEDIT
	    if (virtual_active())
		oneright();
	    else
# endif
	    {
# ifdef FEAT_MBYTE
		if (has_mbyte)
		    curwin->w_cursor.col += mb_ptr2len_check(ml_get_cursor());
		else
# endif
		    ++curwin->w_cursor.col;
	    }
	}
#endif
    }
}

/*
 * <Left> Command.
 */
    static void
nv_kleft(cap)
    cmdarg_t	*cap;
{
    if (mod_mask & MOD_MASK_CTRL)
    {
	cap->arg = 1;
	nv_bck_word(cap);
    }
    else
	nv_left(cap);
}

/*
 * Cursor left commands.
 *
 * Returns TRUE when operator end should not be adjusted.
 */
    static void
nv_left(cap)
    cmdarg_t	*cap;
{
    long	n;

    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = FALSE;
    for (n = cap->count1; n > 0; --n)
    {
	if (oneleft() == FAIL)
	{
	    /* <BS> and <Del> wrap to previous line if 'whichwrap' has 'b'.
	     *		 'h' wraps to previous line if 'whichwrap' has 'h'.
	     *	   CURS_LEFT wraps to previous line if 'whichwrap' has '<'.
	     */
	    if (       (((cap->cmdchar == K_BS
				|| cap->cmdchar == Ctrl_H)
			    && vim_strchr(p_ww, 'b') != NULL)
			|| (cap->cmdchar == 'h'
			    && vim_strchr(p_ww, 'h') != NULL)
			|| (cap->cmdchar == K_LEFT
			    && vim_strchr(p_ww, '<') != NULL))
		    && curwin->w_cursor.lnum > 1)
	    {
		--(curwin->w_cursor.lnum);
		coladvance((colnr_t)MAXCOL);
		curwin->w_set_curswant = TRUE;

		/* When the NL before the first char has to be deleted we
		 * put the cursor on the NUL after the previous line.
		 * This is a very special case, be careful!
		 * don't adjust op_end now, otherwise it won't work */
		if (	   (cap->oap->op_type == OP_DELETE
			    || cap->oap->op_type == OP_CHANGE)
			&& !lineempty(curwin->w_cursor.lnum))
		{
		    ++curwin->w_cursor.col;
		    cap->retval |= CA_NO_ADJ_OP_END;
		}
		continue;
	    }
	    /* Only beep and flush if not moved at all */
	    else if (cap->oap->op_type == OP_NOP && n == cap->count1)
		beep_flush();
	    break;
	}
    }
}

/*
 * Cursor up commands.
 * cap->arg is TRUE for "-": Move cursor to first non-blank.
 */
    static void
nv_up(cap)
    cmdarg_t	*cap;
{
    cap->oap->motion_type = MLINE;
    if (cursor_up(cap->count1, cap->oap->op_type == OP_NOP) == FAIL)
	clearopbeep(cap->oap);
    else if (cap->arg)
	beginline(BL_WHITE | BL_FIX);
}

/*
 * Cursor down commands.
 * cap->arg is TRUE for CR and "+": Move cursor to first non-blank.
 */
    static void
nv_down(cap)
    cmdarg_t	*cap;
{
#if defined(FEAT_WINDOWS) && defined(FEAT_QUICKFIX)
    /* In a quickfix window a <CR> jumps to the error under the cursor. */
    if (bt_quickfix(curbuf) && cap->cmdchar == '\r')
	stuffReadbuff((char_u *)":.cc\n");
    else
#endif
#ifdef FEAT_CMDWIN
    /* In the cmdline window a <CR> executes the command. */
    if (cmdwin_type != 0 && cap->cmdchar == '\r')
	cmdwin_result = CR;
    else
#endif
    {
	cap->oap->motion_type = MLINE;
	if (cursor_down(cap->count1, cap->oap->op_type == OP_NOP) == FAIL)
	    clearopbeep(cap->oap);
	else if (cap->arg)
	    beginline(BL_WHITE | BL_FIX);
    }
}

#ifdef FEAT_SEARCHPATH
/*
 * Grab the file name under the cursor and edit it.
 */
    static void
nv_gotofile(cap)
    cmdarg_t	*cap;
{
    char_u	*ptr;

#ifdef FEAT_CMDWIN
    if (cmdwin_type != 0)
    {
	clearopbeep(cap->oap);
	return;
    }
#endif

# ifdef FEAT_VISUAL
    /*
     * In Visual mode, use the selected text as a file name.
     * Don't allow selection across multiple lines.
     */
    if (VIsual_active)
    {
	int	len;

	if (get_visual_text(cap, &ptr, &len) == FAIL)
	    return;
	ptr = find_file_name_in_path(ptr, len,
					   FNAME_MESS|FNAME_EXP, cap->count1);
    }
    else
# endif
	ptr = file_name_at_cursor(FNAME_MESS|FNAME_HYP|FNAME_EXP, cap->count1);

    if (ptr != NULL)
    {
	/* do autowrite if necessary */
	if (curbufIsChanged() && curbuf->b_nwindows <= 1 && !P_HID(curbuf))
	    autowrite(curbuf, FALSE);
	setpcmark();
	(void)do_ecmd(0, ptr, NULL, NULL, ECMD_LAST,
					       P_HID(curbuf) ? ECMD_HIDE : 0);
	vim_free(ptr);
    }
    else
	clearop(cap->oap);
}
#endif

/*
 * <End> command: to end of current line or last line.
 */
    static void
nv_end(cap)
    cmdarg_t	*cap;
{
    if (mod_mask & MOD_MASK_CTRL)	/* CTRL-END = goto last line */
    {
	cap->arg = TRUE;
	nv_goto(cap);
	cap->count1 = 1;		/* to end of current line */
    }
    nv_dollar(cap);
}

/*
 * Handle the "$" command.
 */
    static void
nv_dollar(cap)
    cmdarg_t	*cap;
{
    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = TRUE;
    curwin->w_curswant = MAXCOL;	/* so we stay at the end */
    if (cursor_down((long)(cap->count1 - 1),
					 cap->oap->op_type == OP_NOP) == FAIL)
	clearopbeep(cap->oap);
}

/*
 * Implementation of '?' and '/' commands.
 * If cap->arg is TRUE don't set PC mark.
 */
    static void
nv_search(cap)
    cmdarg_t	    *cap;
{
    oparg_t	*oap = cap->oap;
    int		i;

    if (cap->cmdchar == '?' && cap->oap->op_type == OP_ROT13)
    {
	/* Translate "g??" to "g?g?" */
	cap->cmdchar = 'g';
	cap->nchar = '?';
	nv_operator(cap);
	return;
    }

#if defined(FEAT_GUI_W32) && defined(FEAT_MBYTE_IME)
    ImeSetOriginMode();
#endif
    cap->searchbuf = getcmdline(cap->cmdchar, cap->count1, 0);
#if defined(FEAT_GUI_W32) && defined(FEAT_MBYTE_IME)
    ImeSetEnglishMode();
#endif
    if (cap->searchbuf == NULL)
    {
	clearop(oap);
	return;
    }
    oap->motion_type = MCHAR;
    oap->inclusive = FALSE;
    curwin->w_set_curswant = TRUE;

    i = do_search(oap, cap->cmdchar, cap->searchbuf, cap->count1,
	    (cap->arg ? 0 : SEARCH_MARK)
				     | SEARCH_OPT | SEARCH_ECHO | SEARCH_MSG);
    if (i == 0)
	clearop(oap);
    else
    {
	if (i == 2)
	    oap->motion_type = MLINE;
#ifdef FEAT_VIRTUALEDIT
	curwin->w_coladd = 0;
#endif
    }

    /* "/$" will put the cursor after the end of the line, may need to
     * correct that here */
    check_cursor();
}

/*
 * Handle "N" and "n" commands.
 * cap->arg is SEARCH_REV for "N", 0 for "n".
 */
    static void
nv_next(cap)
    cmdarg_t	*cap;
{
    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = FALSE;
    curwin->w_set_curswant = TRUE;
    if (!do_search(cap->oap, 0, NULL, cap->count1,
	      SEARCH_MARK | SEARCH_OPT | SEARCH_ECHO | SEARCH_MSG | cap->arg))
	clearop(cap->oap);
#ifdef FEAT_VIRTUALEDIT
    else
	curwin->w_coladd = 0;
#endif

    /* "/$" will put the cursor after the end of the line, may need to
     * correct that here */
    check_cursor();
}

/*
 * Character search commands.
 * cap->arg is BACKWARD for 'F' and 'T', FORWARD for 'f' and 't', TRUE for
 * ',' and FALSE for ';'.
 * cap->nchar is NUL for ',' and ';' (repeat the search)
 */
    static void
nv_csearch(cap)
    cmdarg_t	*cap;
{
    int		type;

    if (cap->cmdchar == 't' || cap->cmdchar == 'T')
	type = TRUE;
    else
	type = FALSE;

    cap->oap->motion_type = MCHAR;
    if (cap->arg == BACKWARD)
	cap->oap->inclusive = FALSE;
    else
	cap->oap->inclusive = TRUE;
    if (IS_SPECIAL(cap->nchar)
	    || !searchc(cap->nchar, cap->arg, type, cap->count1))
	clearopbeep(cap->oap);
    else
    {
	curwin->w_set_curswant = TRUE;
#ifdef FEAT_VIRTUALEDIT
	curwin->w_coladd = 0;
#endif
#ifdef FEAT_VISUAL
	adjust_for_sel(cap);
#endif
    }
}

/*
 * "[" and "]" commands.
 * cap->arg is BACKWARD for "[" and FORWARD for "]".
 */
    static void
nv_brackets(cap)
    cmdarg_t	*cap;
{
    pos_t	new_pos;
    pos_t	prev_pos;
    pos_t	*pos = NULL;	    /* init for GCC */
    pos_t	old_pos;	    /* cursor position before command */
    int		flag;
    long	n;
    int		findc;
    int		c;

    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = FALSE;
    old_pos = curwin->w_cursor;
#ifdef FEAT_VIRTUALEDIT
    curwin->w_coladd = 0;	    /* TODO: don't do this for an error. */
#endif

#ifdef FEAT_SEARCHPATH
    /*
     * "[f" or "]f" : Edit file under the cursor (same as "gf")
     */
    if (cap->nchar == 'f')
	nv_gotofile(cap);
    else
#endif

#ifdef FEAT_FIND_ID
    /*
     * Find the occurence(s) of the identifier or define under cursor
     * in current and included files or jump to the first occurence.
     *
     *			search	     list	    jump
     *		      fwd   bwd    fwd	 bwd	 fwd	bwd
     * identifier     "]i"  "[i"   "]I"  "[I"	"]^I"  "[^I"
     * define	      "]d"  "[d"   "]D"  "[D"	"]^D"  "[^D"
     */
    if (vim_strchr((char_u *)
#ifdef EBCDIC
		"iI\005dD\067",
#else
		"iI\011dD\004",
#endif
		cap->nchar) != NULL)
    {
	char_u	*ptr;
	int	len;

	if ((len = find_ident_under_cursor(&ptr, FIND_IDENT)) == 0)
	    clearop(cap->oap);
	else
	{
	    find_pattern_in_path(ptr, 0, len, TRUE,
		cap->count0 == 0 ? !isupper(cap->nchar) : FALSE,
		((cap->nchar & 0xf) == ('d' & 0xf)) ?  FIND_DEFINE : FIND_ANY,
		cap->count1,
		isupper(cap->nchar) ? ACTION_SHOW_ALL :
			    islower(cap->nchar) ? ACTION_SHOW : ACTION_GOTO,
		cap->cmdchar == ']' ? curwin->w_cursor.lnum : (linenr_t)1,
		(linenr_t)MAXLNUM);
	    curwin->w_set_curswant = TRUE;
	}
    }
    else
#endif

    /*
     * "[{", "[(", "]}" or "])": go to Nth unclosed '{', '(', '}' or ')'
     * "[#", "]#": go to start/end of Nth innermost #if..#endif construct.
     * "[/", "[*", "]/", "]*": go to Nth comment start/end.
     * "[m" or "]m" search for prev/next start of (Java) method.
     * "[M" or "]M" search for prev/next end of (Java) method.
     */
    if (  (cap->cmdchar == '['
		&& vim_strchr((char_u *)"{(*/#mM", cap->nchar) != NULL)
	    || (cap->cmdchar == ']'
		&& vim_strchr((char_u *)"})*/#mM", cap->nchar) != NULL))
    {
	if (cap->nchar == '*')
	    cap->nchar = '/';
	new_pos.lnum = 0;
	prev_pos.lnum = 0;
	if (cap->nchar == 'm' || cap->nchar == 'M')
	{
	    if (cap->cmdchar == '[')
		findc = '{';
	    else
		findc = '}';
	    n = 9999;
	}
	else
	{
	    findc = cap->nchar;
	    n = cap->count1;
	}
	for ( ; n > 0; --n)
	{
	    if ((pos = findmatchlimit(cap->oap, findc,
		(cap->cmdchar == '[') ? FM_BACKWARD : FM_FORWARD, 0)) == NULL)
	    {
		if (new_pos.lnum == 0)	/* nothing found */
		{
		    if (cap->nchar != 'm' && cap->nchar != 'M')
			clearopbeep(cap->oap);
		}
		else
		    pos = &new_pos;	/* use last one found */
		break;
	    }
	    prev_pos = new_pos;
	    curwin->w_cursor = *pos;
	    new_pos = *pos;
	}
	curwin->w_cursor = old_pos;

	/*
	 * Handle "[m", "]m", "[M" and "[M".  The findmatchlimit() only
	 * brought us to the match for "[m" and "]M" when inside a method.
	 * Try finding the '{' or '}' we want to be at.
	 * Also repeat for the given count.
	 */
	if (cap->nchar == 'm' || cap->nchar == 'M')
	{
	    /* norm is TRUE for "]M" and "[m" */
	    int	    norm = ((findc == '{') == (cap->nchar == 'm'));

	    n = cap->count1;
	    /* found a match: we were inside a method */
	    if (prev_pos.lnum != 0)
	    {
		pos = &prev_pos;
		curwin->w_cursor = prev_pos;
		if (norm)
		    --n;
	    }
	    else
		pos = NULL;
	    while (n > 0)
	    {
		for (;;)
		{
		    if ((findc == '{' ? dec_cursor() : inc_cursor()) < 0)
		    {
			/* if not found anything, that's an error */
			if (pos == NULL)
			    clearopbeep(cap->oap);
			n = 0;
			break;
		    }
		    c = gchar_cursor();
		    if (c == '{' || c == '}')
		    {
			/* Must have found end/start of class: use it.
			 * Or found the place to be at. */
			if ((c == findc && norm) || (n == 1 && !norm))
			{
			    new_pos = curwin->w_cursor;
			    pos = &new_pos;
			    n = 0;
			}
			/* if no match found at all, we started outside of the
			 * class and we're inside now.  Just go on. */
			else if (new_pos.lnum == 0)
			{
			    new_pos = curwin->w_cursor;
			    pos = &new_pos;
			}
			/* found start/end of other method: go to match */
			else if ((pos = findmatchlimit(cap->oap, findc,
			    (cap->cmdchar == '[') ? FM_BACKWARD : FM_FORWARD,
								  0)) == NULL)
			    n = 0;
			else
			    curwin->w_cursor = *pos;
			break;
		    }
		}
		--n;
	    }
	    curwin->w_cursor = old_pos;
	    if (pos == NULL && new_pos.lnum != 0)
		clearopbeep(cap->oap);
	}
	if (pos != NULL)
	{
	    setpcmark();
	    curwin->w_cursor = *pos;
	    curwin->w_set_curswant = TRUE;
	}
    }

    /*
     * "[[", "[]", "]]" and "][": move to start or end of function
     */
    else if (cap->nchar == '[' || cap->nchar == ']')
    {
	if (cap->nchar == cap->cmdchar)		    /* "]]" or "[[" */
	    flag = '{';
	else
	    flag = '}';		    /* "][" or "[]" */

	curwin->w_set_curswant = TRUE;
	/*
	 * Imitate strange Vi behaviour: When using "]]" with an operator
	 * we also stop at '}'.
	 */
	if (!findpar(cap->oap, cap->arg, cap->count1, flag,
	      (cap->oap->op_type != OP_NOP
				      && cap->arg == FORWARD && flag == '{')))
	    clearopbeep(cap->oap);
	else if (cap->oap->op_type == OP_NOP)
	    beginline(BL_WHITE | BL_FIX);
    }

    /*
     * "[p", "[P", "]P" and "]p": put with indent adjustment
     */
    else if (cap->nchar == 'p' || cap->nchar == 'P')
    {
	if (!checkclearopq(cap->oap))
	{
	    prep_redo_cmd(cap);
	    do_put(cap->oap->regname,
	      (cap->cmdchar == ']' && cap->nchar == 'p') ? FORWARD : BACKWARD,
						  cap->count1, PUT_FIXINDENT);
	}
    }

    /*
     * "['", "[`", "]'" and "]`": jump to next mark
     */
    else if (cap->nchar == '\'' || cap->nchar == '`')
    {
	pos = &curwin->w_cursor;
	for (n = cap->count1; n > 0; --n)
	{
	    prev_pos = *pos;
	    pos = getnextmark(pos, cap->cmdchar == '[' ? BACKWARD : FORWARD,
							  cap->nchar == '\'');
	    if (pos == NULL)
		break;
	}
	if (pos == NULL)
	    pos = &prev_pos;
	nv_cursormark(cap, cap->nchar == '\'', pos);
    }

#ifdef FEAT_MOUSE
    /*
     * [ or ] followed by a middle mouse click: put selected text with
     * indent adjustment.  Any other button just does as usual.
     */
    else if (cap->nchar >= K_LEFTMOUSE && cap->nchar <= K_RIGHTRELEASE)
    {
	(void)do_mouse(cap->oap, cap->nchar,
		       (cap->cmdchar == ']') ? FORWARD : BACKWARD,
		       cap->count1, PUT_FIXINDENT);
    }
#endif /* FEAT_MOUSE */

    /* Not a valid cap->nchar. */
    else
	clearopbeep(cap->oap);
}

/*
 * Handle Normal mode "%" command.
 */
    static void
nv_percent(cap)
    cmdarg_t	*cap;
{
    pos_t	*pos;

    cap->oap->inclusive = TRUE;
    if (cap->count0)	    /* {cnt}% : goto {cnt} percentage in file */
    {
	if (cap->count0 > 100)
	    clearopbeep(cap->oap);
	else
	{
	    cap->oap->motion_type = MLINE;
	    setpcmark();
			    /* round up, so CTRL-G will give same value */
	    curwin->w_cursor.lnum = (curbuf->b_ml.ml_line_count *
						    cap->count0 + 99L) / 100L;
	    beginline(BL_SOL | BL_FIX);
	}
    }
    else		    /* "%" : go to matching paren */
    {
	cap->oap->motion_type = MCHAR;
	if ((pos = findmatch(cap->oap, NUL)) == NULL)
	    clearopbeep(cap->oap);
	else
	{
	    setpcmark();
	    curwin->w_cursor = *pos;
	    curwin->w_set_curswant = TRUE;
#ifdef FEAT_VIRTUALEDIT
	    curwin->w_coladd = 0;
#endif
#ifdef FEAT_VISUAL
	    adjust_for_sel(cap);
#endif
	}
    }
}

/*
 * Handle "(" and ")" commands.
 * cap->arg is BACKWARD for "(" and FORWARD for ")".
 */
    static void
nv_brace(cap)
    cmdarg_t	*cap;
{
    cap->oap->motion_type = MCHAR;
    if (cap->cmdchar == ')')
	cap->oap->inclusive = FALSE;
    else
	cap->oap->inclusive = TRUE;
    curwin->w_set_curswant = TRUE;

    if (findsent(cap->arg, cap->count1) == FAIL)
	clearopbeep(cap->oap);
#ifdef FEAT_VIRTUALEDIT
    else
	curwin->w_coladd = 0;
#endif
}

/*
 * "m" command: Mark a position.
 */
    static void
nv_mark(cap)
    cmdarg_t	*cap;
{
    if (!checkclearop(cap->oap))
    {
	if (setmark(cap->nchar) == FAIL)
	    clearopbeep(cap->oap);
    }
}

/*
 * "{" and "}" commands.
 * cmd->arg is BACKWARD for "{" and FORWARD for "}".
 */
    static void
nv_findpar(cap)
    cmdarg_t	*cap;
{
    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = FALSE;
    curwin->w_set_curswant = TRUE;
    if (!findpar(cap->oap, cap->arg, cap->count1, NUL, FALSE))
	clearopbeep(cap->oap);
#ifdef FEAT_VIRTUALEDIT
    else
	curwin->w_coladd = 0;
#endif
}

/*
 * "u" command: Undo or make lower case.
 */
    static void
nv_undo(cap)
    cmdarg_t	*cap;
{
    if (cap->oap->op_type == OP_LOWER
#ifdef FEAT_VISUAL
	    || VIsual_active
#endif
	    )
    {
	/* translate "<Visual>u" to "<Visual>gu" and "guu" to "gugu" */
	cap->cmdchar = 'g';
	cap->nchar = 'u';
	nv_operator(cap);
    }
    else
	nv_kundo(cap);
}

/*
 * <Undo> command.
 */
    static void
nv_kundo(cap)
    cmdarg_t	*cap;
{
    if (!checkclearopq(cap->oap))
    {
	u_undo((int)cap->count1);
	curwin->w_set_curswant = TRUE;
    }
}

/*
 * Handle the "r" command.
 */
    static void
nv_replace(cap)
    cmdarg_t	*cap;
{
    char_u	*ptr;
    int		had_ctrl_v;
    long	n;

    if (checkclearop(cap->oap))
	return;

    /* get another character */
    if (cap->nchar == Ctrl_V)
    {
	had_ctrl_v = Ctrl_V;
	cap->nchar = get_literal();
    }
    else
	had_ctrl_v = NUL;

#ifdef FEAT_VISUAL
    /* Visual mode "r" */
    if (VIsual_active)
    {
	nv_operator(cap);
	return;
    }
#endif

#ifdef FEAT_VIRTUALEDIT
   /* If virtual editing is ON, we have to make sure the cursor position
    * is identical with the text position */
    if (ve_flags == VE_ALL && curwin->w_coladd)
    {
	u_save_cursor();
	coladvance_force(getviscol());
    }
#endif

    /*
     * Check for a special key or not enough characters to replace.
     */
    ptr = ml_get_cursor();
    if (IS_SPECIAL(cap->nchar) || STRLEN(ptr) < (unsigned)cap->count1
#ifdef FEAT_MBYTE
	    || (has_mbyte && mb_charlen(ptr) < cap->count1)
#endif
	    )
    {
	clearopbeep(cap->oap);
	return;
    }

    /*
     * Replacing with a TAB is done by edit(), because it is complicated when
     * 'expandtab', 'smarttab' or 'softtabstop' is set.
     * Other characters are done below to avoid problems with things like
     * CTRL-V 048 (for edit() this would be R CTRL-V 0 ESC).
     */
    if (cap->nchar == '\t' && (curbuf->b_p_et || p_sta))
    {
	stuffnumReadbuff(cap->count1);
	stuffcharReadbuff('R');
	stuffcharReadbuff('\t');
	stuffcharReadbuff(ESC);
	return;
    }

    /* save line for undo */
    if (u_save_cursor() == FAIL)
	return;

    if (had_ctrl_v != Ctrl_V && (cap->nchar == '\r' || cap->nchar == '\n'))
    {
	/*
	 * Replace character(s) by a single newline.
	 * Strange vi behaviour: Only one newline is inserted.
	 * Delete the characters here.
	 * Insert the newline with an insert command, takes care of
	 * autoindent.	The insert command depends on being on the last
	 * character of a line or not.
	 */
	(void)del_chars(cap->count1, FALSE);	/* delete the characters */
	stuffcharReadbuff('\r');
	stuffcharReadbuff(ESC);
	/*
	 * Give 'r' to edit(), to get the redo command right.
	 */
	if (edit('r', FALSE, cap->count1))
	    cap->retval |= CA_COMMAND_BUSY;
    }
    else
    {
	prep_redo(cap->oap->regname, cap->count1,
				       NUL, 'r', NUL, had_ctrl_v, cap->nchar);

	curbuf->b_op_start = curwin->w_cursor;
#ifdef FEAT_MBYTE
	if (has_mbyte)
	{
	    int		old_State = State;

	    /* This is slow, but it handles replacing a single-byte with a
	     * multi-byte and the other way around. */
	    State = REPLACE;
	    for (n = cap->count1; n > 0; --n)
		ins_char(cap->nchar);
	    State = old_State;
	}
	else
#endif
	{
	    /* mark the buffer as changed and prepare for displaying */
	    changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);

	    /*
	     * Replace the characters within one line.
	     */
	    for (n = cap->count1; n > 0; --n)
	    {
		/*
		 * Get ptr again, because u_save and/or showmatch() will have
		 * released the line.  At the same time we let know that the
		 * line will be changed.
		 */
		ptr = ml_get_buf(curbuf, curwin->w_cursor.lnum, TRUE);
		ptr[curwin->w_cursor.col] = cap->nchar;
		if (       p_sm
			&& (cap->nchar == ')'
			    || cap->nchar == '}'
			    || cap->nchar == ']'))
		    showmatch();
		++curwin->w_cursor.col;
	    }
	}
	--curwin->w_cursor.col;	    /* cursor on the last replaced char */
#ifdef FEAT_MBYTE
	/* if the character on the left of the current cursor is a multi-byte
	 * character, move two characters left */
	if (has_mbyte)
	    mb_adjust_cursor();
#endif
	curbuf->b_op_end = curwin->w_cursor;
	curwin->w_set_curswant = TRUE;
	set_last_insert(cap->nchar);
    }
}

#ifdef FEAT_VISUAL
/*
 * 'o': Exchange start and end of Visual area.
 * 'O': same, but in block mode exchange left and right corners.
 */
    static void
v_swap_corners(cap)
    cmdarg_t	*cap;
{
    pos_t	old_cursor;
    colnr_t	left, right;
#ifdef FEAT_VIRTUALEDIT
    int		coladd;
#endif

    if (cap->cmdchar == 'O' && VIsual_mode == Ctrl_V)
    {
	old_cursor = curwin->w_cursor;
	getvcols(&old_cursor, &VIsual, &left, &right);
	curwin->w_cursor.lnum = VIsual.lnum;
	coladvance(left);
	VIsual = curwin->w_cursor;
#ifdef FEAT_VIRTUALEDIT
	VIsual_coladd = curwin->w_coladd;
#endif
	curwin->w_cursor.lnum = old_cursor.lnum;
	coladvance(right);
	curwin->w_curswant = right;
	if (curwin->w_cursor.col == old_cursor.col)
	{
	    curwin->w_cursor.lnum = VIsual.lnum;
	    coladvance(right);
	    VIsual = curwin->w_cursor;
#ifdef FEAT_VIRTUALEDIT
	    VIsual_coladd = curwin->w_coladd;
#endif
	    curwin->w_cursor.lnum = old_cursor.lnum;
	    coladvance(left);
	    curwin->w_curswant = left;
	}
    }
    else
    {
	old_cursor = curwin->w_cursor;
	curwin->w_cursor = VIsual;
	VIsual = old_cursor;
#ifdef FEAT_VIRTUALEDIT
	coladd = curwin->w_coladd;
	curwin->w_coladd = VIsual_coladd;
	VIsual_coladd = coladd;
#endif
	curwin->w_set_curswant = TRUE;
    }
}
#endif /* FEAT_VISUAL */

/*
 * "R".
 */
    static void
nv_Replace(cap)
    cmdarg_t	    *cap;
{
#ifdef FEAT_VISUAL
    if (VIsual_active)		/* "R" is replace lines */
    {
	cap->cmdchar = 'c';
	VIsual_mode = 'V';
	nv_operator(cap);
    }
    else
#endif
	if (!checkclearopq(cap->oap))
    {
	if (u_save_cursor() == OK)
	{
#ifdef FEAT_VIRTUALEDIT
	    /* If virtual editing is ON, we have to make sure the cursor
	     * position is identical with the text position. */
	    if (ve_flags == VE_ALL && curwin->w_coladd)
		coladvance_force(getviscol());
#endif
	    /* This is a new edit command, not a restart.  We don't edit
	     * recursively. */
	    restart_edit = 0;
	    if (edit('R', FALSE, cap->count1))
		cap->retval |= CA_COMMAND_BUSY;
	}
    }
}

/*
 * "gR".
 */
    static void
nv_VReplace(cap)
    cmdarg_t	    *cap;
{
#ifdef FEAT_VISUAL
    if (VIsual_active)
    {
	cap->cmdchar = 'R';
	cap->nchar = NUL;
	nv_Replace(cap);	/* Do same as "R" in Visual mode for now */
    }
    else
#endif
	if (!checkclearopq(cap->oap))
    {
	if (u_save_cursor() == OK)
	{
	    /* This is a new edit command, not a restart.  We don't edit
	     * recursively. */
	    restart_edit = 0;
	    if (edit('V', FALSE, cap->count1))
		cap->retval |= CA_COMMAND_BUSY;
	}
    }
}

/*
 * "gr".
 */
    static void
nv_vreplace(cap)
    cmdarg_t	    *cap;
{
#ifdef FEAT_VISUAL
    if (VIsual_active)
    {
	cap->cmdchar = 'r';
	cap->nchar = cap->extra_char;
	nv_replace(cap);	/* Do same as "r" in Visual mode for now */
    }
    else
#endif
	if (!checkclearopq(cap->oap))
    {
	if (u_save_cursor() == OK)
	{
	    if (cap->extra_char == Ctrl_V)	/* get another character */
		cap->extra_char = get_literal();
	    stuffcharReadbuff(cap->extra_char);
	    stuffcharReadbuff(ESC);
	    /* This is a new edit command, not a restart.  We don't edit
	     * recursively. */
	    restart_edit = 0;
	    if (edit('v', FALSE, cap->count1))
		cap->retval |= CA_COMMAND_BUSY;
	}
    }
}

/*
 * Swap case for "~" command, when it does not work like an operator.
 */
    static void
n_swapchar(cap)
    cmdarg_t	*cap;
{
    long	n;
    pos_t	startpos;
    int		did_change = 0;

    if (checkclearopq(cap->oap))
	return;

    if (lineempty(curwin->w_cursor.lnum) && vim_strchr(p_ww, '~') == NULL)
    {
	clearopbeep(cap->oap);
	return;
    }

    prep_redo_cmd(cap);

    if (u_save_cursor() == FAIL)
	return;

    startpos = curwin->w_cursor;
    for (n = cap->count1; n > 0; --n)
    {
	did_change |= swapchar(cap->oap->op_type, &curwin->w_cursor);
	inc_cursor();
	if (gchar_cursor() == NUL)
	{
	    if (vim_strchr(p_ww, '~') != NULL
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
	    {
		++curwin->w_cursor.lnum;
		curwin->w_cursor.col = 0;
		if (n > 1)
		{
		    if (u_savesub(curwin->w_cursor.lnum) == FAIL)
			break;
		    u_clearline();
		}
	    }
	    else
		break;
	}
    }

    check_cursor();
    curwin->w_set_curswant = TRUE;
    if (did_change)
    {
	changed_lines(startpos.lnum, startpos.col, curwin->w_cursor.lnum + 1,
									  0L);
	curbuf->b_op_start = startpos;
	curbuf->b_op_end = curwin->w_cursor;
	if (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }
}

/*
 * Move cursor to mark.
 */
    static void
nv_cursormark(cap, flag, pos)
    cmdarg_t	*cap;
    int		flag;
    pos_t	*pos;
{
    if (check_mark(pos) == FAIL)
	clearop(cap->oap);
    else
    {
	if (cap->cmdchar == '\''
		|| cap->cmdchar == '`'
		|| cap->cmdchar == '['
		|| cap->cmdchar == ']')
	    setpcmark();
	curwin->w_cursor = *pos;
	if (flag)
	    beginline(BL_WHITE | BL_FIX);
	else
	    check_cursor();
    }
    cap->oap->motion_type = flag ? MLINE : MCHAR;
    cap->oap->inclusive = FALSE;		/* ignored if not MCHAR */
    curwin->w_set_curswant = TRUE;
}

#ifdef FEAT_VISUAL
/*
 * Handle commands that are operators in Visual mode.
 */
    static void
v_visop(cap)
    cmdarg_t	*cap;
{
    static char_u trans[] = "YyDdCcxdXdAAIIrr";

    /* Uppercase means linewise, except in block mode, then "D" deletes till
     * the end of the line, and "C" replaces til EOL */
    if (isupper(cap->cmdchar))
    {
	if (VIsual_mode != Ctrl_V)
	    VIsual_mode = 'V';
	else if (cap->cmdchar == 'C' || cap->cmdchar == 'D')
	    curwin->w_curswant = MAXCOL;
    }
    cap->cmdchar = *(vim_strchr(trans, cap->cmdchar) + 1);
    nv_operator(cap);
}
#endif

/*
 * "s" and "S" commands.
 */
    static void
nv_subst(cap)
    cmdarg_t	*cap;
{
#ifdef FEAT_VISUAL
    if (VIsual_active)	/* "vs" and "vS" are the same as "vc" */
    {
	if (cap->cmdchar == 'S')
	    VIsual_mode = 'V';
	cap->cmdchar = 'c';
	nv_operator(cap);
    }
    else
#endif
	nv_optrans(cap);
}

/*
 * Abbreviated commands.
 */
    static void
nv_abbrev(cap)
    cmdarg_t	*cap;
{
    if (cap->cmdchar == K_DEL || cap->cmdchar == K_KDEL)
	cap->cmdchar = 'x';		/* DEL key behaves like 'x' */

#ifdef FEAT_VISUAL
    /* in Visual mode these commands are operators */
    if (VIsual_active)
	v_visop(cap);
    else
#endif
	nv_optrans(cap);
}

/*
 * Translate a command into another command.
 */
    static void
nv_optrans(cap)
    cmdarg_t	*cap;
{
    static char_u *(ar[8]) = {(char_u *)"dl", (char_u *)"dh",
			      (char_u *)"d$", (char_u *)"c$",
			      (char_u *)"cl", (char_u *)"cc",
			      (char_u *)"yy", (char_u *)":s\r"};
    static char_u *str = (char_u *)"xXDCsSY&";

    if (!checkclearopq(cap->oap))
    {
	if (cap->count0)
	    stuffnumReadbuff(cap->count0);
	stuffReadbuff(ar[(int)(vim_strchr(str, cap->cmdchar) - str)]);
    }
    cap->opcount = 0;
}

/*
 * "'" and "`" commands.  Also for "g'" and "g`".
 * cap->arg is TRUE for "'" and "g'".
 */
    static void
nv_gomark(cap)
    cmdarg_t	*cap;
{
    pos_t	*pos;
    int		c;

    if (cap->cmdchar == 'g')
	c = cap->extra_char;
    else
	c = cap->nchar;
    pos = getmark(c, (cap->oap->op_type == OP_NOP));
    if (pos == (pos_t *)-1)	    /* jumped to other file */
    {
	if (cap->arg)
	{
	    check_cursor_lnum();
	    beginline(BL_WHITE | BL_FIX);
	}
	else
	    check_cursor();
    }
    else
	nv_cursormark(cap, cap->arg, pos);

#ifdef FEAT_VIRTUALEDIT
    if (virtual_active())
	getmark_coladd(c, (cap->oap->op_type == OP_NOP));
#endif
}

/*
 * Handle CTRL-O and CTRL-I commands.
 */
    static void
nv_pcmark(cap)
    cmdarg_t	*cap;
{
    pos_t	*pos;

    if (!checkclearopq(cap->oap))
    {
	pos = movemark((int)cap->count1);
	if (pos == (pos_t *)-1)		/* jump to other file */
	{
	    curwin->w_set_curswant = TRUE;
	    check_cursor();
	}
	else if (pos != NULL)		    /* can jump */
	    nv_cursormark(cap, FALSE, pos);
	else
	    clearopbeep(cap->oap);
    }
}

/*
 * Handle '"' command.
 */
    static void
nv_regname(cap)
    cmdarg_t	*cap;
{
    if (checkclearop(cap->oap))
	return;
#ifdef FEAT_EVAL
    if (cap->nchar == '=')
	cap->nchar = get_expr_register();
#endif
    if (cap->nchar != NUL && valid_yank_reg(cap->nchar, FALSE))
    {
	cap->oap->regname = cap->nchar;
	cap->opcount = cap->count0;	/* remember count before '"' */
    }
    else
	clearopbeep(cap->oap);
}

#ifdef FEAT_VISUAL
/*
 * Handle "v", "V" and "CTRL-V" commands.
 * Also for "gh", "gH" and "g^H" commands: Always start Select mode, cap->arg
 * is TRUE.
 */
    static void
nv_visual(cap)
    cmdarg_t	*cap;
{
    /* 'v', 'V' and CTRL-V can be used while an operator is pending to make it
     * characterwise, linewise, or blockwise. */
    if (cap->oap->op_type != OP_NOP)
    {
	cap->oap->motion_force = cap->cmdchar;
	finish_op = FALSE;	/* operator doesn't finish now but later */
	return;
    }

    /* Don't do anything when an operator is pending. */
    if (checkclearop(cap->oap))
	return;

    VIsual_select = cap->arg;
    if (VIsual_active)	    /* change Visual mode */
    {
	if (VIsual_mode == cap->cmdchar)    /* stop visual mode */
	    end_visual_mode();
	else				    /* toggle char/block mode */
	{				    /*	   or char/line mode */
	    VIsual_mode = cap->cmdchar;
	    showmode();
	}
	redraw_curbuf_later(INVERTED);	    /* update the inversion */
    }
    else		    /* start Visual mode */
    {
	check_visual_highlight();
	if (cap->count0)		    /* use previously selected part */
	{
	    if (resel_VIsual_mode == NUL)   /* there is none */
	    {
		beep_flush();
		return;
	    }
	    VIsual = curwin->w_cursor;

	    VIsual_active = TRUE;
	    VIsual_reselect = TRUE;
	    if (!cap->arg)
		/* start Select mode when 'selectmode' contains "cmd" */
		may_start_select('c');
#ifdef FEAT_MOUSE
	    setmouse();
#endif
	    if (p_smd)
		redraw_cmdline = TRUE;	    /* show visual mode later */
	    /*
	     * For V and ^V, we multiply the number of lines even if there
	     * was only one -- webb
	     */
	    if (resel_VIsual_mode != 'v' || resel_VIsual_line_count > 1)
	    {
		curwin->w_cursor.lnum +=
				    resel_VIsual_line_count * cap->count0 - 1;
		if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
		    curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	    }
	    VIsual_mode = resel_VIsual_mode;
	    if (VIsual_mode == 'v')
	    {
		if (resel_VIsual_line_count <= 1)
		    curwin->w_cursor.col += resel_VIsual_col * cap->count0 - 1;
		else
		    curwin->w_cursor.col = resel_VIsual_col;
	    }
	    if (resel_VIsual_col == MAXCOL)
	    {
		curwin->w_curswant = MAXCOL;
		coladvance((colnr_t)MAXCOL);
	    }
	    else if (VIsual_mode == Ctrl_V)
	    {
		validate_virtcol();
		curwin->w_curswant = curwin->w_virtcol
#ifdef FEAT_VIRTUALEDIT
		    + curwin->w_coladd
#endif
		    + resel_VIsual_col * cap->count0 - 1;
		coladvance(curwin->w_curswant);
	    }
	    else
		curwin->w_set_curswant = TRUE;
	    redraw_curbuf_later(INVERTED);	/* show the inversion */
	}
	else
	{
	    if (!cap->arg)
		/* start Select mode when 'selectmode' contains "cmd" */
		may_start_select('c');
	    n_start_visual_mode(cap->cmdchar);
	}
    }
}

/*
 * Start selection for Shift-movement keys.
 */
    void
start_selection()
{
    /* if 'selectmode' contains "key", start Select mode */
    may_start_select('k');
    n_start_visual_mode('v');
}

/*
 * Start Select mode, if "c" is in 'selectmode' and not in a mapping or menu.
 */
    void
may_start_select(c)
    int		c;
{
    VIsual_select = (stuff_empty() && typebuf_typed()
		    && (vim_strchr(p_slm, c) != NULL));
}

/*
 * Start Visual mode "c".
 * Should set VIsual_select before calling this.
 */
    static void
n_start_visual_mode(c)
    int		c;
{
    VIsual = curwin->w_cursor;
#ifdef FEAT_VIRTUALEDIT
    VIsual_coladd = curwin->w_coladd;
#endif
    VIsual_mode = c;
    VIsual_active = TRUE;
    VIsual_reselect = TRUE;

#ifdef FEAT_FOLDING
    foldAdjustVisual();
#endif

#ifdef FEAT_MOUSE
    setmouse();
#endif
    if (p_smd)
	redraw_cmdline = TRUE;	/* show visual mode later */
#ifdef FEAT_CLIPBOARD
    /* Make sure the clipboard gets updated.  Needed because start and
     * end may still be the same, and the selection needs to be owned */
    clipboard.vmode = NUL;
#endif
}

#endif /* FEAT_VISUAL */

/*
 * CTRL-W: Window commands
 */
    static void
nv_window(cap)
    cmdarg_t	*cap;
{
#ifdef FEAT_WINDOWS
    if (!checkclearop(cap->oap))
	do_window(cap->nchar, cap->count0);	/* everything is in window.c */
#else
    (void)checkclearop(cap->oap);
#endif
}

/*
 * CTRL-Z: Suspend
 */
    static void
nv_suspend(cap)
    cmdarg_t	*cap;
{
    clearop(cap->oap);
#ifdef FEAT_VISUAL
    if (VIsual_active)
	end_visual_mode();		/* stop Visual mode */
#endif
    stuffReadbuff((char_u *)":st\r");
}

/*
 * Commands starting with "g".
 */
    static void
nv_g_cmd(cap)
    cmdarg_t	*cap;
{
    oparg_t	*oap = cap->oap;
#ifdef FEAT_VISUAL
    pos_t	tpos;
#endif
#ifdef FEAT_VIRTUALEDIT
    int		coladd;
#endif
    int		i;
    int		flag = FALSE;

    switch (cap->nchar)
    {
#ifdef MEM_PROFILE
    /*
     * "g^A": dump log of used memory.
     */
    case Ctrl_A:
	vim_mem_profile_dump();
	break;
#endif

    /*
     * "gR": Enter virtual replace mode.
     */
    case 'R':
	nv_VReplace(cap);
	break;

    case 'r':
	nv_vreplace(cap);
	break;

    case '&':
	stuffReadbuff((char_u *)":%s//~/&\r");
	break;

#ifdef FEAT_VISUAL
    /*
     * "gv": Reselect the previous Visual area.  If Visual already active,
     *	     exchange previous and current Visual area.
     */
    case 'v':
	if (checkclearop(oap))
	    break;

	if (	   curbuf->b_visual_start.lnum == 0
		|| curbuf->b_visual_start.lnum > curbuf->b_ml.ml_line_count
		|| curbuf->b_visual_end.lnum == 0)
	    beep_flush();
	else
	{
	    /* set w_cursor to the start of the Visual area, tpos to the end */
	    if (VIsual_active)
	    {
		i = VIsual_mode;
		VIsual_mode = curbuf->b_visual_mode;
		curbuf->b_visual_mode = i;

		tpos = curbuf->b_visual_end;
		curbuf->b_visual_end = curwin->w_cursor;
		curwin->w_cursor = curbuf->b_visual_start;
		curbuf->b_visual_start = VIsual;

#ifdef FEAT_VIRTUALEDIT
		coladd = curbuf->b_visual_end_coladd;
		curbuf->b_visual_end_coladd = curwin->w_coladd;
		curwin->w_coladd = curbuf->b_visual_start_coladd;
		curbuf->b_visual_start_coladd = VIsual_coladd;
#endif
	    }
	    else
	    {
		VIsual_mode = curbuf->b_visual_mode;
		tpos = curbuf->b_visual_end;
		curwin->w_cursor = curbuf->b_visual_start;
#ifdef FEAT_VIRTUALEDIT
		coladd = curbuf->b_visual_end_coladd;
		curwin->w_coladd = curbuf->b_visual_start_coladd;
#endif
	    }

	    VIsual_active = TRUE;
	    VIsual_reselect = TRUE;

	    /* Set Visual to the start and w_cursor to the end of the Visual
	     * area.  Make sure they are on an existing character. */
	    check_cursor();
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = tpos;
#ifdef FEAT_VIRTUALEDIT
	    VIsual_coladd = curwin->w_coladd;
	    curwin->w_coladd = coladd;
#endif
	    check_cursor();
	    update_topline();
	    /*
	     * When called from normal "g" command: start Select mode when
	     * 'selectmode' contains "cmd".  When called for K_SELECT, always
	     * start Select mode.
	     */
	    if (cap->arg)
		VIsual_select = TRUE;
	    else
		may_start_select('c');
#ifdef FEAT_MOUSE
	    setmouse();
#endif
#ifdef FEAT_CLIPBOARD
	    /* Make sure the clipboard gets updated.  Needed because start and
	     * end are still the same, and the selection needs to be owned */
	    clipboard.vmode = NUL;
#endif
	    redraw_curbuf_later(INVERTED);
	    showmode();
	}
	break;
    /*
     * "gV": Don't reselect the previous Visual area after a Select mode
     *	     mapping of menu.
     */
    case 'V':
	VIsual_reselect = FALSE;
	break;

    /*
     * "gh":  start Select mode.
     * "gH":  start Select line mode.
     * "g^H": start Select block mode.
     */
    case K_BS:
	cap->nchar = Ctrl_H;
	/* FALLTHROUGH */
    case 'h':
    case 'H':
    case Ctrl_H:
# ifdef EBCDIC
	/* EBCDIC: 'v'-'h' != '^v'-'^h' */
	if (cap->nchar == Ctrl_H)
	    cap->cmdchar = Ctrl_V;
	else
# endif
	cap->cmdchar = cap->nchar + ('v' - 'h');
	cap->arg = TRUE;
	nv_visual(cap);
	break;
#endif /* FEAT_VISUAL */

    /*
     * "gj" and "gk" two new funny movement keys -- up and down
     * movement based on *screen* line rather than *file* line.
     */
    case 'j':
    case K_DOWN:
	/* with 'nowrap' it works just like the normal "j" command */
	if (!curwin->w_p_wrap)
	{
	    oap->motion_type = MLINE;
	    i = cursor_down(cap->count1, oap->op_type == OP_NOP);
	}
	else
	    i = nv_screengo(oap, FORWARD, cap->count1);
	if (i == FAIL)
	    clearopbeep(oap);
	break;

    case 'k':
    case K_UP:
	/* with 'nowrap' it works just like the normal "k" command */
	if (!curwin->w_p_wrap)
	{
	    oap->motion_type = MLINE;
	    i = cursor_up(cap->count1, oap->op_type == OP_NOP);
	}
	else
	    i = nv_screengo(oap, BACKWARD, cap->count1);
	if (i == FAIL)
	    clearopbeep(oap);
	break;

    /*
     * "gJ": join two lines without inserting a space.
     */
    case 'J':
	nv_join(cap);
	break;

    /*
     * "g0", "g^" and "g$": Like "0", "^" and "$" but for screen lines.
     * "gm": middle of "g0" and "g$".
     */
    case '^':
	flag = TRUE;
	/* FALLTHROUGH */

    case '0':
    case 'm':
    case K_HOME:
    case K_KHOME:
    case K_XHOME:
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	if (curwin->w_p_wrap
#ifdef FEAT_VERTSPLIT
		&& curwin->w_width != 0
#endif
		)
	{
	    int		width1 = W_WIDTH(curwin) - curwin_col_off();
	    int		width2 = width1 + curwin_col_off2();

	    validate_virtcol();
	    i = 0;
	    if (curwin->w_virtcol >= (colnr_t)width1 && width2 > 0)
		i = (curwin->w_virtcol - width1) / width2 * width2 + width1;
	}
	else
	    i = curwin->w_leftcol;
	/* Go to the middle of the screen line.  When 'number' is on and lines
	 * are wrapping the middle can be more to the left.*/
	if (cap->nchar == 'm')
	    i += (W_WIDTH(curwin) - curwin_col_off()
		    + ((curwin->w_p_wrap && i > 0)
			? curwin_col_off2() : 0)) / 2;
	coladvance((colnr_t)i);
	if (flag)
	{
	    do
		i = gchar_cursor();
	    while (vim_iswhite(i) && oneright() == OK);
	}
	curwin->w_set_curswant = TRUE;
	break;

    case '_':
	cap->oap->motion_type = MCHAR;
	cap->oap->inclusive = TRUE;
	curwin->w_curswant = MAXCOL;
	if (cursor_down((long)(cap->count1 - 1),
					 cap->oap->op_type == OP_NOP) == FAIL)
	    clearopbeep(cap->oap);
	else
	{
	    char_u  *ptr = ml_get_curline();

	    while (curwin->w_cursor.col > 0
				    && vim_iswhite(ptr[curwin->w_cursor.col]))
		--curwin->w_cursor.col;
	    curwin->w_set_curswant = TRUE;
	}
	break;

    case '$':
    case K_END:
    case K_KEND:
    case K_XEND:
	{
	    int col_off = curwin_col_off();

	    oap->motion_type = MCHAR;
	    oap->inclusive = TRUE;
	    if (curwin->w_p_wrap
#ifdef FEAT_VERTSPLIT
		    && curwin->w_width != 0
#endif
	       )
	    {
		curwin->w_curswant = MAXCOL;    /* so we stay at the end */
		if (cap->count1 == 1)
		{
		    int		width1 = W_WIDTH(curwin) - col_off;
		    int		width2 = width1 + curwin_col_off2();

		    validate_virtcol();
		    i = width1 - 1;
		    if (curwin->w_virtcol >= (colnr_t)width1)
			i += ((curwin->w_virtcol - width1) / width2 + 1)
								     * width2;
		    coladvance((colnr_t)i);
#if defined(FEAT_LINEBREAK) || defined(FEAT_MBYTE)
		    if (curwin->w_cursor.col > 0 && curwin->w_p_wrap)
		    {
			/*
			 * Check for landing on a character that got split at
			 * the end of the line.  We do not want to advance to
			 * the next screen line.
			 */
			validate_virtcol();
			if (curwin->w_virtcol > (colnr_t)i)
			    --curwin->w_cursor.col;
		    }
#endif
		}
		else if (nv_screengo(oap, FORWARD, cap->count1 - 1) == FAIL)
		    clearopbeep(oap);
	    }
	    else
	    {
		i = curwin->w_leftcol + W_WIDTH(curwin) - col_off - 1;
		coladvance((colnr_t)i);
		curwin->w_set_curswant = TRUE;
	    }
	}
	break;

    /*
     * "g*" and "g#", like "*" and "#" but without using "\<" and "\>"
     */
    case '*':
    case '#':
    case POUND:		/* pound sign */
    case Ctrl_RSB:		/* :tag or :tselect for current identifier */
    case ']':			/* :tselect for current identifier */
	nv_ident(cap);
	break;

    /*
     * ge and gE: go back to end of word
     */
    case 'e':
    case 'E':
	oap->motion_type = MCHAR;
	curwin->w_set_curswant = TRUE;
	oap->inclusive = TRUE;
	if (bckend_word(cap->count1, cap->nchar == 'E', FALSE) == FAIL)
	    clearopbeep(oap);
	break;

    /*
     * "g CTRL-G": display info about cursor position
     */
    case Ctrl_G:
	cursor_pos_info();
	break;

    /*
     * "gi": start Insert at the last position.
     */
    case 'i':
	if (curbuf->b_last_insert.lnum != 0)
	{
	    curwin->w_cursor = curbuf->b_last_insert;
	    check_cursor_lnum();
	    i = STRLEN(ml_get_curline());
	    if (curwin->w_cursor.col > (colnr_t)i)
		curwin->w_cursor.col = i;
	}
	cap->cmdchar = 'i';
	nv_edit(cap);
	break;

    /*
     * "gI": Start insert in column 1.
     */
    case 'I':
	beginline(0);
	if (!checkclearopq(oap))
	{
	    if (u_save_cursor() == OK)
	    {
		/* This is a new edit command, not a restart.  We don't edit
		 * recursively. */
		restart_edit = 0;
		if (edit('g', FALSE, cap->count1))
		    cap->retval |= CA_COMMAND_BUSY;
	    }
	}
	break;

#ifdef FEAT_SEARCHPATH
    /*
     * "gf": goto file, edit file under cursor
     * "]f" and "[f": can also be used.
     */
    case 'f':
	nv_gotofile(cap);
	break;
#endif

	/* "g'm" and "g`m": jump to mark without setting pcmark */
    case '\'':
	cap->arg = TRUE;
	/*FALLTHROUGH*/
    case '`':
	nv_gomark(cap);
	break;

    /*
     * "gs": Goto sleep, but keep on checking for CTRL-C
     */
    case 's':
	out_flush();
	while (cap->count1-- && !got_int)
	{
	    ui_delay(1000L, TRUE);
	    ui_breakcheck();
	}
	break;

    /*
     * "ga": Display the ascii value of the character under the
     * cursor.	It is displayed in decimal, hex, and octal. -- webb
     */
    case 'a':
	do_ascii(NULL);
	break;

#ifdef FEAT_MBYTE
    /*
     * "g8": Display the bytes used for the UTF-8 character under the
     * cursor.	It is displayed in hex.
     */
    case '8':
	show_utf8();
	break;
#endif

    /*
     * "gg": Goto the first line in file.  With a count it goes to
     * that line number like for "G". -- webb
     */
    case 'g':
	cap->arg = FALSE;
	nv_goto(cap);
	break;

    /*
     *	 Two-character operators:
     *	 "gq"	    Format text
     *	 "g~"	    Toggle the case of the text.
     *	 "gu"	    Change text to lower case.
     *	 "gU"	    Change text to upper case.
     *   "g?"	    rot13 encoding
     */
    case 'q':
    case '~':
    case 'u':
    case 'U':
    case '?':
	nv_operator(cap);
	break;

    /*
     * "gd": Find first occurence of pattern under the cursor in the
     *	 current function
     * "gD": idem, but in the current file.
     */
    case 'd':
    case 'D':
	nv_gd(oap, cap->nchar);
	break;

#ifdef FEAT_MOUSE
    /*
     * g<*Mouse> : <C-*mouse>
     */
    case K_MIDDLEMOUSE:
    case K_MIDDLEDRAG:
    case K_MIDDLERELEASE:
    case K_LEFTMOUSE:
    case K_LEFTDRAG:
    case K_LEFTRELEASE:
    case K_RIGHTMOUSE:
    case K_RIGHTDRAG:
    case K_RIGHTRELEASE:
	mod_mask = MOD_MASK_CTRL;
	(void)do_mouse(oap, cap->nchar, BACKWARD, cap->count1, 0);
	break;

    case K_IGNORE:
	break;
#endif

    /*
     * "gP" and "gp": same as "P" and "p" but leave cursor just after new text
     */
    case 'p':
    case 'P':
	nv_put(cap);
	break;

#ifdef FEAT_BYTEOFF
    /* "go": goto byte count from start of buffer */
    case 'o':
	goto_byte(cap->count0);
	break;
#endif

    /* "gQ": improved Ex mode */
    case 'Q':
#ifdef FEAT_CMDWIN
	if (cmdwin_type != 0)
	{
	    clearopbeep(cap->oap);
	    break;
	}
#endif
	if (!checkclearopq(oap))
	    do_exmode(TRUE);
	break;

    default:
	clearopbeep(oap);
	break;
    }
}

/*
 * Handle "o" and "O" commands.
 */
    static void
n_opencmd(cap)
    cmdarg_t	*cap;
{
    if (!checkclearopq(cap->oap))
    {
#ifdef FEAT_COMMENTS
	if (has_format_option(FO_OPEN_COMS))
	    fo_do_comments = TRUE;
#endif
#ifdef FEAT_FOLDING
	if (cap->cmdchar == 'O')
	    /* Open above the first line of a folded sequence of lines */
	    (void)hasFolding(curwin->w_cursor.lnum,
						&curwin->w_cursor.lnum, NULL);
	else
	    /* Open below the last line of a folded sequence of lines */
	    (void)hasFolding(curwin->w_cursor.lnum,
						NULL, &curwin->w_cursor.lnum);
#endif
	if (u_save((linenr_t)(curwin->w_cursor.lnum -
					       (cap->cmdchar == 'O' ? 1 : 0)),
		   (linenr_t)(curwin->w_cursor.lnum +
					       (cap->cmdchar == 'o' ? 1 : 0))
		       ) == OK
		&& open_line(cap->cmdchar == 'O' ? BACKWARD : FORWARD,
								    FALSE, 0))
	{
	    /* This is a new edit command, not a restart.  We don't edit
	     * recursively. */
	    restart_edit = 0;
	    if (edit(cap->cmdchar, TRUE, cap->count1))
		cap->retval |= CA_COMMAND_BUSY;
	}
#ifdef FEAT_COMMENTS
	fo_do_comments = FALSE;
#endif
    }
}

/*
 * "." command: redo last change.
 */
    static void
nv_dot(cap)
    cmdarg_t	*cap;
{
    if (!checkclearopq(cap->oap))
    {
	/*
	 * if restart_edit is TRUE, the last but one command is repeated
	 * instead of the last command (inserting text). This is used for
	 * CTRL-O <.> in insert mode
	 */
	if (start_redo(cap->count0, restart_edit && !arrow_used) == FAIL)
	    clearopbeep(cap->oap);
    }
}

/*
 * CTRL-R: undo undo
 */
    static void
nv_redo(cap)
    cmdarg_t	*cap;
{
    if (!checkclearopq(cap->oap))
    {
	u_redo((int)cap->count1);
	curwin->w_set_curswant = TRUE;
    }
}

/*
 * Handle "U" command.
 */
    static void
nv_Undo(cap)
    cmdarg_t	*cap;
{
    /* In Visual mode and typing "gUU" triggers an operator */
    if (cap->oap->op_type == OP_UPPER
#ifdef FEAT_VISUAL
	    || VIsual_active
#endif
	    )
    {
	/* translate "gUU" to "gUgU" */
	cap->cmdchar = 'g';
	cap->nchar = 'U';
	nv_operator(cap);
    }
    else if (!checkclearopq(cap->oap))
    {
	u_undoline();
	curwin->w_set_curswant = TRUE;
    }
}

/*
 * '~' command: If tilde is not an operator and Visual is off: swap case of a
 * single character.
 */
    static void
nv_tilde(cap)
    cmdarg_t	*cap;
{
    if (!p_to
#ifdef FEAT_VISUAL
	    && !VIsual_active
#endif
	    && cap->oap->op_type != OP_TILDE)
	n_swapchar(cap);
    else
	nv_operator(cap);
}

/*
 * Handle an operator command.
 */
    static void
nv_operator(cap)
    cmdarg_t	*cap;
{
    int	    op_type;

    op_type = get_op_type(cap->cmdchar, cap->nchar);

    if (op_type == cap->oap->op_type)	    /* double operator works on lines */
	nv_lineop(cap);
    else if (!checkclearop(cap->oap))
    {
	cap->oap->start = curwin->w_cursor;
	cap->oap->op_type = op_type;
    }
}

/*
 * Handle linewise operator "dd", "yy", etc.
 *
 * "_" is is a strange motion command that helps make operators more logical.
 * It is actually implemented, but not documented in the real Vi.  This motion
 * command actually refers to "the current line".  Commands like "dd" and "yy"
 * are really an alternate form of "d_" and "y_".  It does accept a count, so
 * "d3_" works to delete 3 lines.
 */
    static void
nv_lineop(cap)
    cmdarg_t	*cap;
{
    cap->oap->motion_type = MLINE;
    if (cursor_down(cap->count1 - 1L, cap->oap->op_type == OP_NOP) == FAIL)
	clearopbeep(cap->oap);
    else if (  cap->oap->op_type == OP_DELETE
	    || cap->oap->op_type == OP_LSHIFT
	    || cap->oap->op_type == OP_RSHIFT)
	beginline(BL_SOL | BL_FIX);
    else if (cap->oap->op_type != OP_YANK)	/* 'Y' does not move cursor */
	beginline(BL_WHITE | BL_FIX);
}

/*
 * <Home> command.
 */
    static void
nv_home(cap)
    cmdarg_t	*cap;
{
    if (mod_mask & MOD_MASK_CTRL)	    /* CTRL-HOME = goto line 1 */
    {
	cap->arg = FALSE;
	nv_goto(cap);
    }
    else
    {
	cap->count0 = 1;
	nv_pipe(cap);
    }
}

/*
 * "|" command.
 */
    static void
nv_pipe(cap)
    cmdarg_t *cap;
{
    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = FALSE;
    beginline(0);
    if (cap->count0 > 0)
    {
	coladvance((colnr_t)(cap->count0 - 1));
	curwin->w_curswant = (colnr_t)(cap->count0 - 1);
    }
    else
	curwin->w_curswant = 0;
    /* keep curswant at the column where we wanted to go, not where
       we ended; differs is line is too short */
    curwin->w_set_curswant = FALSE;
}

/*
 * Handle back-word command "b" and "B".
 * cap->arg is 1 for "B"
 */
    static void
nv_bck_word(cap)
    cmdarg_t	*cap;
{
    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = FALSE;
    curwin->w_set_curswant = TRUE;
    if (bck_word(cap->count1, cap->arg, FALSE) == FAIL)
	clearopbeep(cap->oap);
}

/*
 * Handle word motion commands "e", "E", "w" and "W".
 * cap->arg is TRUE for "E" and "W".
 */
    static void
nv_wordcmd(cap)
    cmdarg_t	*cap;
{
    int		n;
    int		word_end;
    int		flag = FALSE;

    /*
     * Set inclusive for the "E" and "e" command.
     */
    if (cap->cmdchar == 'e' || cap->cmdchar == 'E')
	word_end = TRUE;
    else
	word_end = FALSE;
    cap->oap->inclusive = word_end;

    /*
     * "cw" and "cW" are a special case.
     */
    if (!word_end && cap->oap->op_type == OP_CHANGE)
    {
	n = gchar_cursor();
	if (n != NUL)			/* not an empty line */
	{
	    if (vim_iswhite(n))
	    {
		/*
		 * Reproduce a funny Vi behaviour: "cw" on a blank only
		 * changes one character, not all blanks until the start of
		 * the next word.  Only do this when the 'w' flag is included
		 * in 'cpoptions'.
		 */
		if (cap->count1 == 1 && vim_strchr(p_cpo, CPO_CW) != NULL)
		{
		    cap->oap->inclusive = TRUE;
		    cap->oap->motion_type = MCHAR;
		    return;
		}
	    }
	    else
	    {
		/*
		 * This is a little strange. To match what the real Vi does,
		 * we effectively map 'cw' to 'ce', and 'cW' to 'cE', provided
		 * that we are not on a space or a TAB.  This seems impolite
		 * at first, but it's really more what we mean when we say
		 * 'cw'.
		 * Another strangeness: When standing on the end of a word
		 * "ce" will change until the end of the next wordt, but "cw"
		 * will change only one character! This is done by setting
		 * flag.
		 */
		cap->oap->inclusive = TRUE;
		word_end = TRUE;
		flag = TRUE;
	    }
	}
    }

    cap->oap->motion_type = MCHAR;
    curwin->w_set_curswant = TRUE;
    if (word_end)
	n = end_word(cap->count1, cap->arg, flag, FALSE);
    else
	n = fwd_word(cap->count1, cap->arg, cap->oap->op_type != OP_NOP);

    /* Don't leave the cursor on the NUL past a line */
    if (curwin->w_cursor.col && gchar_cursor() == NUL)
    {
	--curwin->w_cursor.col;
	cap->oap->inclusive = TRUE;
    }

    if (n == FAIL && cap->oap->op_type == OP_NOP)
	clearopbeep(cap->oap);
#ifdef FEAT_VISUAL
    else
	adjust_for_sel(cap);
#endif
}

/*
 * "0" and "^" commands.
 * cap->arg is the argument for beginline().
 */
    static void
nv_beginline(cap)
    cmdarg_t	*cap;
{
    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = FALSE;
    beginline(cap->arg);
}

#ifdef FEAT_VISUAL
/*
 * In exclusive Visual mode, may include the last character.
 */
    static void
adjust_for_sel(cap)
    cmdarg_t	*cap;
{
    if (VIsual_active && cap->oap->inclusive && *p_sel == 'e'
						     && gchar_cursor() != NUL)
    {
# ifdef FEAT_MBYTE
	if (has_mbyte)
	    inc_cursor();
	else
# endif
	    ++curwin->w_cursor.col;
	cap->oap->inclusive = FALSE;
    }
}

/*
 * Exclude last character at end of Visual area for 'selection' == "exclusive".
 * Should check VIsual_mode before calling this.
 */
    static void
unadjust_for_sel()
{
    pos_t	*pp;

    if (*p_sel == 'e' && !equal(VIsual, curwin->w_cursor))
    {
	if (lt(VIsual, curwin->w_cursor))
	    pp = &curwin->w_cursor;
	else
	    pp = &VIsual;
	if (pp->col > 0)
	    --pp->col;
	else if (pp->lnum > 1)
	{
	    --pp->lnum;
	    pp->col = STRLEN(ml_get(pp->lnum));
	}
    }
}

/*
 * SELECT key in Normal or Visual mode: end of Select mode mapping.
 */
    static void
nv_select(cap)
    cmdarg_t	*cap;
{
    pos_t	*pp;

    if (VIsual_active)
	VIsual_select = TRUE;
    else if (VIsual_reselect)
    {
	cap->nchar = 'v';	    /* fake "gv" command */
	cap->arg = TRUE;
	nv_g_cmd(cap);
	if (*p_sel == 'e' && VIsual_mode == 'v')
	{
	    /* exclusive mode: advance the end one character */
	    if (lt(VIsual, curwin->w_cursor))
		pp = &curwin->w_cursor;
	    else
		pp = &VIsual;
	    if (*ml_get_pos(pp) != NUL)
		++pp->col;
	    else if (pp->lnum < curbuf->b_ml.ml_line_count)
	    {
		++pp->lnum;
		pp->col = 0;
	    }
	    curwin->w_set_curswant = TRUE;
	    redraw_curbuf_later(INVERTED);
	}
    }
}

#endif

/*
 * "G", "gg", CTRL-END, CTRL-HOME.
 * cap->arg is TRUE for "G".
 */
    static void
nv_goto(cap)
    cmdarg_t	*cap;
{
    linenr_t	lnum;

    if (cap->arg)
	lnum = curbuf->b_ml.ml_line_count;
    else
	lnum = 1L;
    cap->oap->motion_type = MLINE;
    setpcmark();

    /* When a count is given, use it instead of the default lnum */
    if (cap->count0 != 0)
	lnum = cap->count0;
    if (lnum < 1L)
	lnum = 1L;
    else if (lnum > curbuf->b_ml.ml_line_count)
	lnum = curbuf->b_ml.ml_line_count;
    curwin->w_cursor.lnum = lnum;
    beginline(BL_SOL | BL_FIX);
}

/*
 * CTRL-\ in Normal mode.
 */
    static void
nv_normal(cap)
    cmdarg_t	*cap;
{
    if (safe_vgetc() == Ctrl_N)
    {
	clearop(cap->oap);
#ifdef FEAT_CMDWIN
	if (cmdwin_type != 0)
	    cmdwin_result = ESC;
#endif
#ifdef FEAT_VISUAL
	if (VIsual_active)
	{
	    end_visual_mode();		/* stop Visual */
	    redraw_curbuf_later(INVERTED);
	}
#endif
    }
    else
	clearopbeep(cap->oap);
}

/*
 * ESC in Normal mode: beep, but don't flush buffers.
 * Don't even beep if we are canceling a command.
 */
    static void
nv_esc(cap)
    cmdarg_t	*cap;
{
    if (cap->arg)		/* TRUE for CTRL-C */
    {
	restart_edit = 0;
#ifdef FEAT_CMDWIN
	if (cmdwin_type != 0)
	{
	    cmdwin_result = K_IGNORE;
	    return;
	}
#endif
    }

#ifdef FEAT_VISUAL
    if (VIsual_active)
    {
	end_visual_mode();	/* stop Visual */
	check_cursor_col();	/* make sure cursor is not beyond EOL */
	curwin->w_set_curswant = TRUE;
	redraw_curbuf_later(INVERTED);
    }
    else
#endif
	if (cap->oap->op_type == OP_NOP
	    && cap->opcount == 0
	    && cap->count0 == 0
	    && cap->oap->regname == 0
	    && !p_im)
	vim_beep();
    clearop(cap->oap);
    if (p_im && !restart_edit)
	restart_edit = 'a';
}

/*
 * Handle "A", "a", "I", "i" and <Insert> commands.
 */
    static void
nv_edit(cap)
    cmdarg_t *cap;
{
    /* <Insert> is equal to "i" */
    if (cap->cmdchar == K_INS || cap->cmdchar == K_KINS)
	cap->cmdchar = 'i';

#ifdef FEAT_VISUAL
    /* in Visual mode "A" and "I" are an operator */
    if (VIsual_active && (cap->cmdchar == 'A' || cap->cmdchar == 'I'))
	v_visop(cap);

    /* in Visual mode and after an operator "a" and "i" are for text objects */
    else
#endif
	if ((cap->cmdchar == 'a' || cap->cmdchar == 'i')
	    && (cap->oap->op_type != OP_NOP
#ifdef FEAT_VISUAL
		|| VIsual_active
#endif
		))
    {
#ifdef FEAT_TEXTOBJ
	nv_object(cap);
#else
	clearopbeep(cap->oap);
#endif
    }
    else if (!checkclearopq(cap->oap) && u_save_cursor() == OK)
    {
	switch (cap->cmdchar)
	{
	    case 'A':	/* "A"ppend after the line */
		curwin->w_set_curswant = TRUE;
#ifdef FEAT_VIRTUALEDIT
		if (ve_flags == VE_ALL)
		{
		    int save_State = State;

		    /* Pretent Insert mode here to allow the cursor on the
		     * character past the end of the line */
		    State = INSERT;
		    coladvance_force((colnr_t)MAXCOL);
		    State = save_State;
		}
		else
#endif
		    curwin->w_cursor.col += STRLEN(ml_get_cursor());
		break;

	    case 'I':	/* "I"nsert before the first non-blank */
		beginline(BL_WHITE);
		break;

	    case 'a':	/* "a"ppend is like "i"nsert on the next character. */
		if (*ml_get_cursor() != NUL)
		    inc_cursor();
		break;
	}

#ifdef FEAT_VIRTUALEDIT
	if (curwin->w_coladd && cap->cmdchar != 'A')
	{
	    int save_State = State;

	    /* Pretent Insert mode here to allow the cursor on the
	     * character past the end of the line */
	    State = INSERT;
	    coladvance_force(getviscol());
	    State = save_State;
	}
#endif

	/* This is a new edit command, not a restart.  We don't edit
	 * recursively. */
	restart_edit = 0;
	if (edit(cap->cmdchar, FALSE, cap->count1))
	    cap->retval |= CA_COMMAND_BUSY;
    }
}

#ifdef FEAT_TEXTOBJ
/*
 * "a" or "i" while an operator is pending or in Visual mode: object motion.
 */
    static void
nv_object(cap)
    cmdarg_t	*cap;
{
    int		flag;
    int		include;
    char_u	*mps_save;

    if (cap->cmdchar == 'i')
	include = FALSE;    /* "ix" = inner object: exclude white space */
    else
	include = TRUE;	    /* "ax" = an object: include white space */

    /* Make sure (), [], {} and <> are in 'matchpairs' */
    mps_save = curbuf->b_p_mps;
    curbuf->b_p_mps = (char_u *)"(:),{:},[:],<:>";

    switch (cap->nchar)
    {
	case 'w': /* "aw" = a word */
		flag = current_word(cap->oap, cap->count1, include, FALSE);
		break;
	case 'W': /* "aW" = a WORD */
		flag = current_word(cap->oap, cap->count1, include, TRUE);
		break;
	case 'b': /* "ab" = a braces block */
	case '(':
	case ')':
		flag = current_block(cap->oap, cap->count1, include, '(', ')');
		break;
	case 'B': /* "aB" = a Brackets block */
	case '{':
	case '}':
		flag = current_block(cap->oap, cap->count1, include, '{', '}');
		break;
	case '[': /* "a[" = a [] block */
	case ']':
		flag = current_block(cap->oap, cap->count1, include, '[', ']');
		break;
	case '<': /* "a<" = a <> block */
	case '>':
		flag = current_block(cap->oap, cap->count1, include, '<', '>');
		break;
	case 'p': /* "ap" = a paragraph */
		flag = current_par(cap->oap, cap->count1, include, 'p');
		break;
	case 's': /* "as" = a sentence */
		flag = current_sent(cap->oap, cap->count1, include);
		break;
#if 0	/* TODO */
	case 'S': /* "aS" = a section */
	case 'f': /* "af" = a filename */
	case 'u': /* "au" = a URL */
#endif
	default:
		flag = FAIL;
		break;
    }

    curbuf->b_p_mps = mps_save;
    if (flag == FAIL)
	clearopbeep(cap->oap);
    adjust_cursor_col();
    curwin->w_set_curswant = TRUE;
}
#endif

/*
 * "q" command: Start/stop recording.
 * "q:", "q/", "q?": edit command-line in command-line window.
 */
    static void
nv_record(cap)
    cmdarg_t	*cap;
{
    if (cap->oap->op_type == OP_FORMAT)
    {
	/* "gqq" is the same as "gqgq": format line */
	cap->cmdchar = 'g';
	cap->nchar = 'q';
	nv_operator(cap);
    }
    else if (!checkclearop(cap->oap))
    {
#ifdef FEAT_CMDWIN
	if (cap->nchar == ':' || cap->nchar == '/' || cap->nchar == '?')
	{
	    stuffcharReadbuff(cap->nchar);
	    stuffcharReadbuff(K_CMDWIN);
	}
	else
#endif
	    /* (stop) recording into a named register, unless executing a
	     * register */
	    if (!Exec_reg && do_record(cap->nchar) == FAIL)
		clearopbeep(cap->oap);
    }
}

/*
 * Handle the "@r" command.
 */
    static void
nv_at(cap)
    cmdarg_t	*cap;
{
    if (checkclearop(cap->oap))
	return;
#ifdef FEAT_EVAL
    if (cap->nchar == '=')
    {
	if (get_expr_register() == NUL)
	    return;
    }
#endif
    while (cap->count1-- && !got_int)
    {
	if (do_execreg(cap->nchar, FALSE, FALSE) == FAIL)
	{
	    clearopbeep(cap->oap);
	    break;
	}
	line_breakcheck();
    }
}

/*
 * Handle the CTRL-U and CTRL-D commands.
 */
    static void
nv_halfpage(cap)
    cmdarg_t	*cap;
{
    if ((cap->cmdchar == Ctrl_U && curwin->w_cursor.lnum == 1)
	    || (cap->cmdchar == Ctrl_D
		&& curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count))
	clearopbeep(cap->oap);
    else if (!checkclearop(cap->oap))
	halfpage(cap->cmdchar == Ctrl_D, cap->count0);
}

/*
 * Handle "J" or "gJ" command.
 */
    static void
nv_join(cap)
    cmdarg_t *cap;
{
#ifdef FEAT_VISUAL
    if (VIsual_active)	/* join the visual lines */
	nv_operator(cap);
    else
#endif
	if (!checkclearop(cap->oap))
    {
	if (cap->count0 <= 1)
	    cap->count0 = 2;	    /* default for join is two lines! */
	if (curwin->w_cursor.lnum + cap->count0 - 1 >
						   curbuf->b_ml.ml_line_count)
	    clearopbeep(cap->oap);  /* beyond last line */
	else
	{
	    prep_redo(cap->oap->regname, cap->count0,
			 NUL, cap->cmdchar, NUL, NUL, cap->nchar);
	    do_do_join(cap->count0, cap->nchar == NUL);
	}
    }
}

/*
 * "P", "gP", "p" and "gp" commands.
 */
    static void
nv_put(cap)
    cmdarg_t  *cap;
{
    if (cap->oap->op_type != OP_NOP
#ifdef FEAT_VISUAL
	    || VIsual_active
#endif
	    )
	clearopbeep(cap->oap);
    else
    {
	prep_redo_cmd(cap);
	do_put(cap->oap->regname,
		(cap->cmdchar == 'P'
		 || (cap->cmdchar == 'g' && cap->nchar == 'P'))
							 ? BACKWARD : FORWARD,
		cap->count1, cap->cmdchar == 'g' ? PUT_CURSEND : 0);
    }
}

/*
 * "o" and "O" commands.
 */
    static void
nv_open(cap)
    cmdarg_t	*cap;
{
#ifdef FEAT_VISUAL
    if (VIsual_active)  /* switch start and end of visual */
	v_swap_corners(cap);
    else
#endif
	n_opencmd(cap);
}

#ifdef FEAT_SNIFF
/*ARGSUSED*/
    static void
nv_sniff(cap)
    cmdarg_t	*cap;
{
    ProcessSniffRequests();
}
#endif
