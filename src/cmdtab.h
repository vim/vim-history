/* vi:ts=4
 *
 * VIM - Vi IMproved
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
	{"abbreviate",	EXTRA+TRLBAR+NOTRLCOM+USECTRLV},
#define CMD_abbreviate 1
	{"args",		TRLBAR},
#define CMD_args 2
	{"change",		BANG+RANGE+COUNT+TRLBAR},	/* not supported */
#define CMD_change 3
	{"cabbrev",		EXTRA+TRLBAR+NOTRLCOM+USECTRLV},
#define CMD_cabbrev 4
	{"cc",			TRLBAR+WORD1+BANG},
#define CMD_cc 5
	{"cd",			NAMEDF+TRLBAR},
#define CMD_cd 6
	{"center",		TRLBAR+RANGE+EXTRA},
#define CMD_center 7
	{"cf",			TRLBAR+FILE1+BANG},
#define CMD_cf 8
	{"chdir",		NAMEDF+TRLBAR},
#define CMD_chdir 9
	{"cl",			TRLBAR},
#define CMD_cl 10
	{"cmap",		BANG+EXTRA+TRLBAR+NOTRLCOM+USECTRLV},
#define CMD_cmap 11
	{"cn",			TRLBAR+BANG},
#define CMD_cn 12
	{"cnoremap",	BANG+EXTRA+TRLBAR+NOTRLCOM+USECTRLV},
#define CMD_cnoremap 13
	{"cnoreabbrev",	EXTRA+TRLBAR+NOTRLCOM+USECTRLV},
#define CMD_cnoreabbrev 14
	{"copy",		RANGE+EXTRA+TRLBAR},
#define CMD_copy 15
	{"cp",			TRLBAR+BANG},
#define CMD_cp 16
	{"cq",			TRLBAR+BANG},
#define CMD_cq 17
	{"cunmap",		BANG+EXTRA+TRLBAR+USECTRLV},
#define CMD_cunmap 18
	{"cunabbrev",	EXTRA+TRLBAR+USECTRLV},
#define CMD_cunabbrev 19
	{"delete",		RANGE+REGSTR+COUNT+TRLBAR},
#define CMD_delete 20
	{"display",		TRLBAR},
#define CMD_display 21
	{"digraphs",	EXTRA+TRLBAR},
#define CMD_digraphs 22
	{"edit",		BANG+FILE1+TRLBAR},
#define CMD_edit 23
	{"ex",			BANG+FILE1+TRLBAR},
#define CMD_ex 24
	{"file",		FILE1+TRLBAR},
#define CMD_file 25
	{"files",		TRLBAR},
#define CMD_files 26
	{"global",		RANGE+BANG+EXTRA+DFLALL},
#define CMD_global 27
	{"help",		TRLBAR},
#define CMD_help 28
	{"insert",		BANG+RANGE+TRLBAR},			/* not supported */
#define CMD_insert 29
	{"iabbrev",		EXTRA+TRLBAR+NOTRLCOM+USECTRLV},
#define CMD_iabbrev 30
	{"imap",		BANG+EXTRA+TRLBAR+NOTRLCOM+USECTRLV},
#define CMD_imap 31
	{"inoremap",	BANG+EXTRA+TRLBAR+NOTRLCOM+USECTRLV},
#define CMD_inoremap 32
	{"inoreabbrev",	EXTRA+TRLBAR+NOTRLCOM+USECTRLV},
#define CMD_inoreabbrev 33
	{"iunmap",		BANG+EXTRA+TRLBAR+USECTRLV},
#define CMD_iunmap 34
	{"iunabbrev",	EXTRA+TRLBAR+USECTRLV},
#define CMD_iunabbrev 35
	{"join",		RANGE+COUNT+TRLBAR},
#define CMD_join 36
	{"jumps",		TRLBAR},
#define CMD_jumps 37
	{"k",			RANGE+WORD1+TRLBAR},
#define CMD_k 38
	{"list",		RANGE+COUNT+TRLBAR},
#define CMD_list 39
	{"left",		TRLBAR+RANGE+EXTRA},
#define CMD_left 40
	{"move",		RANGE+EXTRA+TRLBAR},
#define CMD_move 41
	{"mark",		RANGE+WORD1+TRLBAR},
#define CMD_mark 42
	{"marks",		TRLBAR},
#define CMD_marks 43
	{"map",			BANG+EXTRA+TRLBAR+NOTRLCOM+USECTRLV},
#define CMD_map 44
	{"make",		NEEDARG+EXTRA+TRLBAR+XFILE},
#define CMD_make 45
	{"mkexrc",		BANG+FILE1+TRLBAR},
#define CMD_mkexrc 46
	{"mkvimrc",		BANG+FILE1+TRLBAR},
#define CMD_mkvimrc 47
	{"next",		RANGE+BANG+NAMEDFS+TRLBAR},
#define CMD_next 48
	{"number",		RANGE+COUNT+TRLBAR},
#define CMD_number 49
	{"noremap",		BANG+EXTRA+TRLBAR+NOTRLCOM+USECTRLV},
#define CMD_noremap 50
	{"noreabbrev",	EXTRA+TRLBAR+NOTRLCOM+USECTRLV},
#define CMD_noreabbrev 51
	{"Next",		RANGE+BANG+TRLBAR},
#define CMD_Next 52
	{"print",		RANGE+COUNT+TRLBAR},
#define CMD_print 53
	{"pop",			RANGE+TRLBAR+ZEROR},
#define CMD_pop 54
	{"put",			RANGE+BANG+REGSTR+TRLBAR},
#define CMD_put 55
	{"previous",	RANGE+BANG+TRLBAR},
#define CMD_previous 56
	{"pwd",			TRLBAR},
#define CMD_pwd 57
	{"quit",		BANG+TRLBAR},
#define CMD_quit 58
	{"read",		RANGE+NAMEDF+NEEDARG+TRLBAR+ZEROR},
#define CMD_read 59
	{"rewind",		BANG+TRLBAR},
#define CMD_rewind 60
	{"recover",		FILE1+TRLBAR},				/* not supported */
#define CMD_recover 61
	{"redo",		TRLBAR},
#define CMD_redo 62
	{"right",		TRLBAR+RANGE+EXTRA},
#define CMD_right 63
	{"substitute",	RANGE+EXTRA},
#define CMD_substitute 64
	{"suspend",		TRLBAR+BANG},
#define CMD_suspend 65
	{"set",			EXTRA+TRLBAR},
#define CMD_set 66
	{"setkeymap",	NAMEDF+TRLBAR},
#define CMD_setkeymap 67
	{"shell",		TRLBAR},
#define CMD_shell 68
	{"source",		NAMEDF+NEEDARG+TRLBAR},
#define CMD_source 69
	{"stop",		TRLBAR+BANG},
#define CMD_stop 70
	{"t",			RANGE+EXTRA+TRLBAR},
#define CMD_t 71
	{"tag",			RANGE+BANG+WORD1+TRLBAR+ZEROR},
#define CMD_tag 72
	{"tags",		TRLBAR},
#define CMD_tags 73
	{"undo",		TRLBAR},
#define CMD_undo 74
	{"unabbreviate", EXTRA+TRLBAR+USECTRLV},
#define CMD_unabbreviate 75
	{"unmap",		BANG+EXTRA+TRLBAR+USECTRLV},
#define CMD_unmap 76
	{"vglobal",		RANGE+EXTRA+DFLALL},
#define CMD_vglobal 77
	{"version",		TRLBAR},
#define CMD_version 78
	{"visual",		RANGE+BANG+FILE1+TRLBAR},
#define CMD_visual 79
	{"write",		RANGE+BANG+FILE1+DFLALL+TRLBAR},
#define CMD_write 80
	{"wnext",		RANGE+BANG+FILE1+TRLBAR},
#define CMD_wnext 81
	{"winsize",		EXTRA+NEEDARG+TRLBAR},
#define CMD_winsize 82
	{"wq",			BANG+FILE1+DFLALL+TRLBAR},
#define CMD_wq 83
	{"xit",			BANG+FILE1+DFLALL+TRLBAR},
#define CMD_xit 84
	{"yank",		RANGE+REGSTR+COUNT+TRLBAR},
#define CMD_yank 85
	{"z",			RANGE+COUNT+TRLBAR},		/* not supported */
#define CMD_z 86
	{"@",			RANGE+EXTRA+TRLBAR},
#define CMD_at 87
	{"!",			RANGE+NAMEDFS},
#define CMD_bang 88
	{"<",			RANGE+COUNT+TRLBAR},
#define CMD_lshift 89
	{">",			RANGE+COUNT+TRLBAR},
#define CMD_rshift 90
	{"=",			RANGE+TRLBAR},
#define CMD_equal 91
	{"&",			RANGE+EXTRA},
#define CMD_and 92
	{"~",			RANGE+TRLBAR}				/* not supported */
#define CMD_tilde 93
#define CMD_SIZE 94

};
