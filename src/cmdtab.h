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
	{"edit",		BANG+FILE1+TRLBAR},
#define CMD_edit 15
	{"ex",			BANG+FILE1+TRLBAR},
#define CMD_ex 16
	{"file",		FILE1+TRLBAR},
#define CMD_file 17
	{"files",		TRLBAR},
#define CMD_files 18
	{"global",		RANGE+BANG+EXTRA+DFLALL},
#define CMD_global 19
	{"help",		TRLBAR},
#define CMD_help 20
	{"insert",		BANG+RANGE+TRLBAR},			/* not supported */
#define CMD_insert 21
	{"join",		RANGE+COUNT+TRLBAR},
#define CMD_join 22
	{"jumps",		TRLBAR},
#define CMD_jumps 23
	{"k",			RANGE+WORD1+TRLBAR},
#define CMD_k 24
	{"list",		RANGE+COUNT+TRLBAR},		/* not supported */
#define CMD_list 25
	{"move",		RANGE+EXTRA+TRLBAR},
#define CMD_move 26
	{"map",			BANG+EXTRA+TRLBAR+NOTRLCOM},
#define CMD_map 27
	{"mark",		RANGE+WORD1+TRLBAR},
#define CMD_mark 28
	{"marks",		TRLBAR},
#define CMD_marks 29
	{"mkexrc",		BANG+FILE1+TRLBAR},
#define CMD_mkexrc 30
	{"next",		RANGE+BANG+NAMEDFS+TRLBAR},
#define CMD_next 31
	{"number",		RANGE+COUNT+TRLBAR},		/* not supported */
#define CMD_number 32
	{"Next",		RANGE+BANG+TRLBAR},
#define CMD_Next 33
	{"print",		RANGE+COUNT+TRLBAR},
#define CMD_print 34
	{"pop",			RANGE+TRLBAR+ZEROR},
#define CMD_pop 35
	{"put",			RANGE+REGSTR+TRLBAR},
#define CMD_put 36
	{"previous",	RANGE+BANG+TRLBAR},
#define CMD_previous 37
	{"quit",		BANG+TRLBAR},
#define CMD_quit 38
	{"read",		RANGE+NAMEDF+NEEDARG+TRLBAR+ZEROR},
#define CMD_read 39
	{"rewind",		BANG+TRLBAR},
#define CMD_rewind 40
	{"recover",		FILE1+TRLBAR},				/* not supported */
#define CMD_recover 41
	{"substitute",	RANGE+EXTRA},
#define CMD_substitute 42
	{"set",			EXTRA+TRLBAR},
#define CMD_set 43
	{"setkeymap",	NAMEDF+TRLBAR},
#define CMD_setkeymap 44
	{"shell",		TRLBAR},
#define CMD_shell 45
	{"source",		NAMEDF+NEEDARG+TRLBAR},
#define CMD_source 46
	{"stop",		TRLBAR},					/* not supported */
#define CMD_stop 47
	{"t",			RANGE+EXTRA+TRLBAR},
#define CMD_t 48
	{"tag",			RANGE+BANG+WORD1+TRLBAR+ZEROR},
#define CMD_tag 49
	{"tags",		TRLBAR},
#define CMD_tags 50
	{"undo",		TRLBAR},
#define CMD_undo 51
	{"unabbreviate", EXTRA+TRLBAR},				/* not supported */
#define CMD_unabbreviate 52
	{"unmap",		BANG+EXTRA+TRLBAR},
#define CMD_unmap 53
	{"vglobal",		RANGE+EXTRA+DFLALL},
#define CMD_vglobal 54
	{"version",		TRLBAR},
#define CMD_version 55
	{"visual",		RANGE+BANG+FILE1+TRLBAR},
#define CMD_visual 56
	{"write",		RANGE+BANG+FILE1+DFLALL+TRLBAR},
#define CMD_write 57
	{"winsize",		EXTRA+NEEDARG+TRLBAR},
#define CMD_winsize 58
	{"wq",			BANG+FILE1+DFLALL+TRLBAR},
#define CMD_wq 59
	{"xit",			BANG+FILE1+DFLALL+TRLBAR},
#define CMD_xit 60
	{"yank",		RANGE+REGSTR+COUNT+TRLBAR},
#define CMD_yank 61
	{"z",			RANGE+COUNT+TRLBAR},		/* not supported */
#define CMD_z 62
	{"!",			RANGE+NAMEDFS},
#define CMD_bang 63
	{"<",			RANGE+COUNT+TRLBAR},
#define CMD_lshift 64
	{">",			RANGE+COUNT+TRLBAR},
#define CMD_rshift 65
	{"=",			RANGE+TRLBAR},
#define CMD_equal 66
	{"&",			RANGE+EXTRA},
#define CMD_and 67
	{"~",			RANGE+TRLBAR}				/* not supported */
#define CMD_tilde 68
#define CMD_SIZE 69

};
