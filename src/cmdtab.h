/* vi:ts=4
 *
 * VIM - Vi IMitation
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
 */

/*
 * THIS FILE IS AUTOMATICALLY PRODUCED - DO NOT EDIT
 */

#define RANGE	0x01			/* allow a linespecs */
#define BANG	0x02			/* allow a ! after the command name */
#define EXTRA	0x04			/* allow extra args after command name */
#define XFILE	0x08			/* expand wildcards in extra part */
#define NOSPC	0x10			/* no spaces allowed in the extra part */
#define	DFLALL	0x20			/* default file range is 1,$ */
#define NODFL	0x40			/* do not default to the current file name */
#define NEEDARG	0x80			/* argument required */
#define TRLBAR	0x100			/* check for trailing vertical bar */
#define REGSTR	0x200			/* allow "x for register designation */
#define COUNT	0x400			/* allow count in argument */
#define NOTRLCOM 0x800			/* no trailing comment allowed */
#define ZEROR	0x1000			/* zero line number allowed */
#define USECTRLV 0x2000			/* do not remove CTRL-V from argument */
#define FILES	(XFILE + EXTRA)	/* multiple extra files allowed */
#define WORD1	(EXTRA + NOSPC)	/* one extra word allowed */
#define FILE1	(FILES + NOSPC)	/* 1 file allowed, defaults to current file */
#define NAMEDF	(FILE1 + NODFL)	/* 1 file allowed, defaults to "" */
#define NAMEDFS	(FILES + NODFL)	/* multiple files allowed, default is "" */

/*
 * This array maps ex command names to command codes. The order in which
 * command names are listed below is significant -- ambiguous abbreviations
 * are always resolved to be the first possible match (e.g. "r" is taken
 * to mean "read", not "rewind", because "read" comes before "rewind").
 * Not supported commands are included to avoid ambiguities.
 */
static struct
{
	char	*cmd_name;	/* name of the command */
	short	 cmd_argt;	/* command line arguments permitted/needed/used */
} cmdnames[] =
{
	{"append",		BANG+RANGE+TRLBAR},			/* not supported */
#define CMD_append 0
	{"abbreviate",	EXTRA+TRLBAR+NOTRLCOM},		/* not supported */
#define CMD_abbreviate 1
	{"args",		TRLBAR},
#define CMD_args 2
	{"change",		BANG+RANGE+COUNT+TRLBAR},	/* not supported */
#define CMD_change 3
	{"cd",			NAMEDF+TRLBAR},
#define CMD_cd 4
	{"cc",			TRLBAR+WORD1+BANG},
#define CMD_cc 5
	{"cf",			TRLBAR+FILE1+BANG},
#define CMD_cf 6
	{"cl",			TRLBAR},
#define CMD_cl 7
	{"cn",			TRLBAR+BANG},
#define CMD_cn 8
	{"cp",			TRLBAR+BANG},
#define CMD_cp 9
	{"cq",			TRLBAR+BANG},
#define CMD_cq 10
	{"copy",		RANGE+EXTRA+TRLBAR},
#define CMD_copy 11
	{"chdir",		NAMEDF+TRLBAR},
#define CMD_chdir 12
	{"delete",		RANGE+REGSTR+COUNT+TRLBAR},
#define CMD_delete 13
	{"display",		TRLBAR},
#define CMD_display 14
	{"digraph",		EXTRA+TRLBAR},
#define CMD_digraph 15
	{"edit",		BANG+FILE1+TRLBAR},
#define CMD_edit 16
	{"ex",			BANG+FILE1+TRLBAR},
#define CMD_ex 17
	{"file",		FILE1+TRLBAR},
#define CMD_file 18
	{"files",		TRLBAR},
#define CMD_files 19
	{"global",		RANGE+BANG+EXTRA+DFLALL},
#define CMD_global 20
	{"help",		TRLBAR},
#define CMD_help 21
	{"insert",		BANG+RANGE+TRLBAR},			/* not supported */
#define CMD_insert 22
	{"join",		RANGE+COUNT+TRLBAR},
#define CMD_join 23
	{"jumps",		TRLBAR},
#define CMD_jumps 24
	{"k",			RANGE+WORD1+TRLBAR},
#define CMD_k 25
	{"list",		RANGE+COUNT+TRLBAR},
#define CMD_list 26
	{"move",		RANGE+EXTRA+TRLBAR},
#define CMD_move 27
	{"mark",		RANGE+WORD1+TRLBAR},
#define CMD_mark 28
	{"marks",		TRLBAR},
#define CMD_marks 29
	{"map",			BANG+EXTRA+TRLBAR+NOTRLCOM+USECTRLV},
#define CMD_map 30
	{"mkexrc",		BANG+FILE1+TRLBAR},
#define CMD_mkexrc 31
	{"mkvimrc",		BANG+FILE1+TRLBAR},
#define CMD_mkvimrc 32
	{"next",		RANGE+BANG+NAMEDFS+TRLBAR},
#define CMD_next 33
	{"number",		RANGE+COUNT+TRLBAR},
#define CMD_number 34
	{"noremap",		BANG+EXTRA+TRLBAR+NOTRLCOM+USECTRLV},
#define CMD_noremap 35
	{"Next",		RANGE+BANG+TRLBAR},
#define CMD_Next 36
	{"print",		RANGE+COUNT+TRLBAR},
#define CMD_print 37
	{"pop",			RANGE+TRLBAR+ZEROR},
#define CMD_pop 38
	{"put",			RANGE+BANG+REGSTR+TRLBAR},
#define CMD_put 39
	{"previous",	RANGE+BANG+TRLBAR},
#define CMD_previous 40
	{"quit",		BANG+TRLBAR},
#define CMD_quit 41
	{"read",		RANGE+NAMEDF+NEEDARG+TRLBAR+ZEROR},
#define CMD_read 42
	{"rewind",		BANG+TRLBAR},
#define CMD_rewind 43
	{"recover",		FILE1+TRLBAR},				/* not supported */
#define CMD_recover 44
	{"substitute",	RANGE+EXTRA},
#define CMD_substitute 45
	{"set",			EXTRA+TRLBAR},
#define CMD_set 46
	{"setkeymap",	NAMEDF+TRLBAR},
#define CMD_setkeymap 47
	{"shell",		TRLBAR},
#define CMD_shell 48
	{"source",		NAMEDF+NEEDARG+TRLBAR},
#define CMD_source 49
	{"stop",		TRLBAR+BANG},
#define CMD_stop 50
	{"t",			RANGE+EXTRA+TRLBAR},
#define CMD_t 51
	{"tag",			RANGE+BANG+WORD1+TRLBAR+ZEROR},
#define CMD_tag 52
	{"tags",		TRLBAR},
#define CMD_tags 53
	{"undo",		TRLBAR},
#define CMD_undo 54
	{"unabbreviate", EXTRA+TRLBAR},				/* not supported */
#define CMD_unabbreviate 55
	{"unmap",		BANG+EXTRA+TRLBAR},
#define CMD_unmap 56
	{"vglobal",		RANGE+EXTRA+DFLALL},
#define CMD_vglobal 57
	{"version",		TRLBAR},
#define CMD_version 58
	{"visual",		RANGE+BANG+FILE1+TRLBAR},
#define CMD_visual 59
	{"write",		RANGE+BANG+FILE1+DFLALL+TRLBAR},
#define CMD_write 60
	{"wnext",		RANGE+BANG+FILE1+TRLBAR},
#define CMD_wnext 61
	{"winsize",		EXTRA+NEEDARG+TRLBAR},
#define CMD_winsize 62
	{"wq",			BANG+FILE1+DFLALL+TRLBAR},
#define CMD_wq 63
	{"xit",			BANG+FILE1+DFLALL+TRLBAR},
#define CMD_xit 64
	{"yank",		RANGE+REGSTR+COUNT+TRLBAR},
#define CMD_yank 65
	{"z",			RANGE+COUNT+TRLBAR},		/* not supported */
#define CMD_z 66
	{"@",			RANGE+EXTRA+TRLBAR},
#define CMD_at 67
	{"!",			RANGE+NAMEDFS},
#define CMD_bang 68
	{"<",			RANGE+COUNT+TRLBAR},
#define CMD_lshift 69
	{">",			RANGE+COUNT+TRLBAR},
#define CMD_rshift 70
	{"=",			RANGE+TRLBAR},
#define CMD_equal 71
	{"&",			RANGE+EXTRA},
#define CMD_and 72
	{"~",			RANGE+TRLBAR}				/* not supported */
#define CMD_tilde 73
#define CMD_SIZE 74

};
