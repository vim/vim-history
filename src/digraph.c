/* vi:ts=4:sw=4
 *
 * VIM - Vi IMitation
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
 */

#ifdef DIGRAPHS
/*
 * digraph.c: code for digraphs
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"

static void printdigraph __ARGS((u_char *));

u_char	(*digraphnew)[3];			/* pointer to added digraphs */
int		digraphcount = 0;			/* number of added digraphs */

u_char	digraphdefault[][3] = 		/* standard digraphs */
	   {'~', '!', 161,	/* ¡ */
	    'c', '|', 162,	/* ¢ */
	    '$', '$', 163,	/* £ */
	    'o', 'x', 164,	/* ¤ */
	    'Y', '-', 165,	/* ¥ */
	    '|', '|', 166,	/* ¦ */
	    'p', 'a', 167,	/* § */
	    '"', '"', 168,	/* ¨ */
	    'c', 'O', 169,	/* © */
		'a', '-', 170,	/* ª */
		'<', '<', 171,	/* « */
		'a', '-', 172,	/* ¬ */
		'-', '-', 173,	/* ­ */
		'r', 'O', 174,	/* ® */
		'-', '=', 175,	/* ¯ */
		'~', 'o', 176,	/* ° */
		'+', '-', 177,	/* ± */
		'2', '2', 178,	/* ² */
		'3', '3', 179,	/* ³ */
		'\'', '\'', 180,	/* ´ */
		'j', 'u', 181,	/* µ */
		'p', 'p', 182,	/* ¶ */
		'~', '.', 183,	/* · */
		',', ',', 184,	/* ¸ */
		'1', '1', 185,	/* ¹ */
		'o', '-', 186,	/* º */
		'>', '>', 187,	/* » */
		'1', '4', 188,	/* ¼ */
		'1', '2', 189,	/* ½ */
		'3', '4', 190,	/* ¾ */
		'~', '?', 191,	/* ¿ */
		'A', '`', 192,	/* À */
		'A', '\'', 193,	/* Á */
		'A', '^', 194,	/* Â */
		'A', '~', 195,	/* Ã */
		'A', '"', 196,	/* Ä */
		'A', '@', 197,	/* Å */
		'A', 'E', 198,	/* Æ */
		'C', ',', 199,	/* Ç */
		'E', '`', 200,	/* È */
		'E', '\'', 201,	/* É */
		'E', '^', 202,	/* Ê */
		'E', '"', 203,	/* Ë */
		'I', '`', 204,	/* Ì */
		'I', '\'', 205,	/* Í */
		'I', '^', 206,	/* Î */
		'I', '"', 207,	/* Ï */
		'-', 'D', 208,	/* Ð */
		'N', '~', 209,	/* Ñ */
		'O', '`', 210,	/* Ò */
		'O', '\'', 211,	/* Ó */
		'O', '^', 212,	/* Ô */
		'O', '~', 213,	/* Õ */
		'O', '"', 214,	/* Ö */
		'/', '\\', 215,	/* × */
		'O', '/', 216,	/* Ø */
		'U', '`', 217,	/* Ù */
		'U', '\'', 218,	/* Ú */
		'U', '^', 219,	/* Û */
		'U', '"', 220,	/* Ü */
		'Y', '\'', 221,	/* Ý */
		'I', 'p', 222,	/* Þ */
		's', 's', 223,	/* ß */
		'a', '`', 224,	/* à */
		'a', '\'', 225,	/* á */
		'a', '^', 226,	/* â */
		'a', '~', 227,	/* ã */
		'a', '"', 228,	/* ä */
		'a', '@', 229,	/* å */
		'a', 'e', 230,	/* æ */
		'c', ',', 231,	/* ç */
		'e', '`', 232,	/* è */
		'e', '\'', 233,	/* é */
		'e', '^', 234,	/* ê */
		'e', '"', 235,	/* ë */
		'i', '`', 236,	/* ì */
		'i', '\'', 237,	/* í */
		'i', '^', 238,	/* î */
		'i', '"', 239,	/* ï */
		'-', 'd', 240,	/* ð */
		'n', '~', 241,	/* ñ */
		'o', '`', 242,	/* ò */
		'o', '\'', 243,	/* ó */
		'o', '^', 244,	/* ô */
		'o', '~', 245,	/* õ */
		'o', '"', 246,	/* ö */
		':', '-', 247,	/* ÷ */
		'o', '/', 248,	/* ø */
		'u', '`', 249,	/* ù */
		'u', '\'', 250,	/* ú */
		'u', '^', 251,	/* û */
		'u', '"', 252,	/* ü */
		'y', '\'', 253,	/* ý */
		'i', 'p', 254,	/* þ */
		'y', '"', 255,	/* ÿ */
		NUL, NUL, NUL
		};

/*
 * lookup the pair char1, char2 in the digraph tables
 * if no match, return char2
 */
	int
getdigraph(char1, char2)
	int	char1;
	int	char2;
{
	int		i;
	int		retval;

	retval = 0;
	for (i = 0; ; ++i)			/* search added digraphs first */
	{
		if (i == digraphcount)	/* end of added table, search defaults */
		{
			for (i = 0; digraphdefault[i][0] != 0; ++i)
				if (digraphdefault[i][0] == char1 && digraphdefault[i][1] == char2)
				{
					retval = digraphdefault[i][2];
					break;
				}
			break;
		}
		if (digraphnew[i][0] == char1 && digraphnew[i][1] == char2)
		{
			retval = digraphnew[i][2];
			break;
		}
	}

	if (retval == 0)	/* digraph deleted or not found */
		return char2;
	return retval;
}

/*
 * put the digraphs in the argument string in the digraph table
 * format: {c1}{c2} char {c1}{c2} char ...
 */
	void
putdigraph(str)
	char *str;
{
	int		char1, char2, n;
	u_char	(*newtab)[3];
	int		i;

	while (*str)
	{
		skipspace(&str);
		char1 = *str++;
		char2 = *str++;
		if (char1 == 0 || char2 == 0)
			return;
		skipspace(&str);
		if (!isdigit(*str))
		{
			emsg(e_number);
			return;
		}
		n = getdigits(&str);
		if (digraphnew)		/* search the table for existing entry */
		{
			for (i = 0; i < digraphcount; ++i)
				if (digraphnew[i][0] == char1 && digraphnew[i][1] == char2)
				{
					digraphnew[i][2] = n;
					break;
				}
			if (i < digraphcount)
				continue;
		}
		newtab = (u_char (*)[3])alloc(digraphcount * 3 + 3);
		if (newtab)
		{
			memmove(newtab, digraphnew, (size_t)(digraphcount * 3));
			free(digraphnew);
			digraphnew = newtab;
			digraphnew[digraphcount][0] = char1;
			digraphnew[digraphcount][1] = char2;
			digraphnew[digraphcount][2] = n;
			++digraphcount;
		}
	}
}

	void
listdigraphs()
{
	int		i;

	printdigraph(NULL);
	for (i = 0; digraphdefault[i][0]; ++i)
		if (getdigraph(digraphdefault[i][0], digraphdefault[i][1]) == digraphdefault[i][2])
			printdigraph(digraphdefault[i]);
	for (i = 0; i < digraphcount; ++i)
		printdigraph(digraphnew[i]);
	outchar('\n');
	wait_return(TRUE);
}

	static void
printdigraph(p)
	u_char *p;
{
	char		buf[9];
	static int	len;

	if (p == NULL)
		len = 0;
	else if (p[2] != 0)
	{
		if (len > Columns - 11)
		{
			outchar('\n');
			len = 0;
		}
		if (len)
			outstrn("   ");
		sprintf(buf, "%c%c %c %3d", p[0], p[1], p[2], p[2]);
		outstrn(buf);
		len += 11;
	}
}

#endif /* DIGRAPHS */
