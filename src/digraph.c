/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

#ifdef DIGRAPHS
/*
 * digraph.c: code for digraphs
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

static void printdigraph __ARGS((char_u *));

char_u	(*digraphnew)[3];			/* pointer to added digraphs */
int		digraphcount = 0;			/* number of added digraphs */

#ifdef MSDOS
char_u	digraphdefault[][3] = 		/* standard MSDOS digraphs */
	   {{'C', ',', 128},	/* Ä */
		{'u', '"', 129},	/* Å */
		{'e', '\'', 130},	/* Ç */
		{'a', '^', 131},	/* É */
		{'a', '"', 132},	/* Ñ */
		{'a', '`', 133},	/* Ö */
		{'a', '@', 134},	/* Ü */
		{'c', ',', 135},	/* ~G (SAS C can't handle the real char) */
		{'e', '^', 136},	/* ~H (SAS C can't handle the real char) */
		{'e', '"', 137},	/* â */
		{'e', '`', 138},	/* ä */
		{'i', '"', 139},	/* ã */
		{'i', '^', 140},	/* å */
		{'i', '`', 141},	/* ç */
		{'A', '"', 142},	/* é */
		{'A', '@', 143},	/* è */
		{'E', '\'', 144},	/* ê */
		{'a', 'e', 145},	/* ë */
		{'A', 'E', 146},	/* í */
		{'o', '^', 147},	/* ì */
		{'o', '"', 148},	/* î */
		{'o', '`', 149},	/* ï */
		{'u', '^', 150},	/* ñ */
		{'u', '`', 151},	/* ó */
		{'y', '"', 152},	/* ò */
		{'O', '"', 153},	/* ô */
		{'U', '"', 154},	/* ö */
	    {'c', '|', 155},	/* õ */
	    {'$', '$', 156},	/* ú */
	    {'Y', '-', 157},	/* ~] (SAS C can't handle the real char) */
	    {'P', 't', 158},	/* û */
	    {'f', 'f', 159},	/* ü */
		{'a', '\'', 160},	/* † */
		{'i', '\'', 161},	/* ° */
		{'o', '\'', 162},	/* ¢ */
		{'u', '\'', 163},	/* xx (SAS C can't handle the real char) */
		{'n', '~', 164},	/* § */
		{'N', '~', 165},	/* • */
		{'a', 'a', 166},	/* ¶ */
		{'o', 'o', 167},	/* ß */
		{'~', '?', 168},	/* ® */
		{'-', 'a', 169},	/* © */
		{'a', '-', 170},	/* ™ */
		{'1', '2', 171},	/* ´ */
		{'1', '4', 172},	/* ¨ */
		{'~', '!', 173},	/* ≠ */
		{'<', '<', 174},	/* Æ */
		{'>', '>', 175},	/* Ø */

		{'s', 's', 225},	/* · */
		{'j', 'u', 230},	/* Ê */
		{'o', '/', 237},	/* Ì */
		{'+', '-', 241},	/* Ò */
		{'>', '=', 242},	/* Ú */
		{'<', '=', 243},	/* Û */
		{':', '-', 246},	/* ˆ */
		{'~', '~', 247},	/* ˜ */
		{'~', 'o', 248},	/* ¯ */
		{'2', '2', 253},	/* ˝ */
		{NUL, NUL, NUL}
		};

#else	/* MSDOS */

char_u	digraphdefault[][3] = 		/* standard ISO digraphs */
	   {{'~', '!', 161},	/* ° */
	    {'c', '|', 162},	/* ¢ */
	    {'$', '$', 163},	/* £ */
	    {'o', 'x', 164},	/* § */
	    {'Y', '-', 165},	/* • */
	    {'|', '|', 166},	/* ¶ */
	    {'p', 'a', 167},	/* ß */
	    {'"', '"', 168},	/* ® */
	    {'c', 'O', 169},	/* © */
		{'a', '-', 170},	/* ™ */
		{'<', '<', 171},	/* ´ */
		{'-', ',', 172},	/* ¨ */
		{'-', '-', 173},	/* ≠ */
		{'r', 'O', 174},	/* Æ */
		{'-', '=', 175},	/* Ø */
		{'~', 'o', 176},	/* ∞ */
		{'+', '-', 177},	/* ± */
		{'2', '2', 178},	/* ≤ */
		{'3', '3', 179},	/* ≥ */
		{'\'', '\'', 180},	/* ¥ */
		{'j', 'u', 181},	/* µ */
		{'p', 'p', 182},	/* ∂ */
		{'~', '.', 183},	/* ∑ */
		{',', ',', 184},	/* ∏ */
		{'1', '1', 185},	/* π */
		{'o', '-', 186},	/* ∫ */
		{'>', '>', 187},	/* ª */
		{'1', '4', 188},	/* º */
		{'1', '2', 189},	/* Ω */
		{'3', '4', 190},	/* æ */
		{'~', '?', 191},	/* ø */
		{'A', '`', 192},	/* ¿ */
		{'A', '\'', 193},	/* ¡ */
		{'A', '^', 194},	/* ¬ */
		{'A', '~', 195},	/* √ */
		{'A', '"', 196},	/* ƒ */
		{'A', '@', 197},	/* ≈ */
		{'A', 'E', 198},	/* ∆ */
		{'C', ',', 199},	/* « */
		{'E', '`', 200},	/* » */
		{'E', '\'', 201},	/* … */
		{'E', '^', 202},	/*   */
		{'E', '"', 203},	/* À */
		{'I', '`', 204},	/* Ã */
		{'I', '\'', 205},	/* Õ */
		{'I', '^', 206},	/* Œ */
		{'I', '"', 207},	/* œ */
		{'D', '-', 208},	/* – */
		{'N', '~', 209},	/* — */
		{'O', '`', 210},	/* “ */
		{'O', '\'', 211},	/* ” */
		{'O', '^', 212},	/* ‘ */
		{'O', '~', 213},	/* ’ */
		{'O', '"', 214},	/* ÷ */
		{'/', '\\', 215},	/* ◊ */
		{'O', '/', 216},	/* ÿ */
		{'U', '`', 217},	/* Ÿ */
		{'U', '\'', 218},	/* ⁄ */
		{'U', '^', 219},	/* € */
		{'U', '"', 220},	/* ‹ */
		{'Y', '\'', 221},	/* › */
		{'I', 'p', 222},	/* ﬁ */
		{'s', 's', 223},	/* ﬂ */
		{'a', '`', 224},	/* ‡ */
		{'a', '\'', 225},	/* · */
		{'a', '^', 226},	/* ‚ */
		{'a', '~', 227},	/* „ */
		{'a', '"', 228},	/* ‰ */
		{'a', '@', 229},	/* Â */
		{'a', 'e', 230},	/* Ê */
		{'c', ',', 231},	/* Á */
		{'e', '`', 232},	/* Ë */
		{'e', '\'', 233},	/* È */
		{'e', '^', 234},	/* Í */
		{'e', '"', 235},	/* Î */
		{'i', '`', 236},	/* Ï */
		{'i', '\'', 237},	/* Ì */
		{'i', '^', 238},	/* Ó */
		{'i', '"', 239},	/* Ô */
		{'d', '-', 240},	/*  */
		{'n', '~', 241},	/* Ò */
		{'o', '`', 242},	/* Ú */
		{'o', '\'', 243},	/* Û */
		{'o', '^', 244},	/* Ù */
		{'o', '~', 245},	/* ı */
		{'o', '"', 246},	/* ˆ */
		{':', '-', 247},	/* ˜ */
		{'o', '/', 248},	/* ¯ */
		{'u', '`', 249},	/* ˘ */
		{'u', '\'', 250},	/* ˙ */
		{'u', '^', 251},	/* ˚ */
		{'u', '"', 252},	/* ¸ */
		{'y', '\'', 253},	/* ˝ */
		{'i', 'p', 254},	/* ˛ */
		{'y', '"', 255},	/* ˇ */
		{NUL, NUL, NUL}
		};
#endif	/* MSDOS */
 
/*
 * handle digraphs after typing a character
 */
	int
dodigraph(c)
	int		c;
{
	static int	backspaced;		/* character before BS */
	static int	lastchar;		/* last typed character */

	if (c == -1)				/* init values */
	{
		backspaced = -1;
	}
	else if (p_dg)
	{
		if (backspaced >= 0)
			c = getdigraph(backspaced, c, FALSE);
		backspaced = -1;
		if (c == BS && lastchar >= 0)
			backspaced = lastchar;
	}
	lastchar = c;
	return c;
}

/*
 * lookup the pair char1, char2 in the digraph tables
 * if no match, return char2
 */
	int
getdigraph(char1, char2, meta)
	int	char1;
	int	char2;
	int	meta;
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

	if (retval == 0)			/* digraph deleted or not found */
	{
		if (char1 == ' ' && meta)		/* <space> <char> --> meta-char */
			return (char2 | 0x80);
		return char2;
	}
	return retval;
}

/*
 * put the digraphs in the argument string in the digraph table
 * format: {c1}{c2} char {c1}{c2} char ...
 */
	void
putdigraph(str)
	char_u *str;
{
	int		char1, char2, n;
	char_u	(*newtab)[3];
	int		i;

	while (*str)
	{
		skipspace(&str);
		if ((char1 = *str++) == 0 || (char2 = *str++) == 0)
			return;
		if (char1 == ESC || char2 == ESC)
		{
			EMSG("Escape not allowed in digraph");
			return;
		}
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
		newtab = (char_u (*)[3])alloc(digraphcount * 3 + 3);
		if (newtab)
		{
			memmove((char *)newtab, (char *)digraphnew, (size_t)(digraphcount * 3));
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
	msg_start();
	msg_outchar('\n');
	for (i = 0; digraphdefault[i][0] && !got_int; ++i)
	{
		if (getdigraph(digraphdefault[i][0], digraphdefault[i][1], FALSE) == digraphdefault[i][2])
			printdigraph(digraphdefault[i]);
		breakcheck();
	}
	for (i = 0; i < digraphcount && !got_int; ++i)
	{
		printdigraph(digraphnew[i]);
		breakcheck();
	}
	msg_outchar('\n');
	wait_return(TRUE);		/* clear screen, because some digraphs may be wrong,
							 * in which case we messed up NextScreen */
}

	static void
printdigraph(p)
	char_u *p;
{
	char_u		buf[9];
	static int	len;

	if (p == NULL)
		len = 0;
	else if (p[2] != 0)
	{
		if (len > Columns - 11)
		{
			msg_outchar('\n');
			len = 0;
		}
		if (len)
			msg_outstr((char_u *)"   ");
		sprintf((char *)buf, "%c%c %c %3d", p[0], p[1], p[2], p[2]);
		msg_outstr(buf);
		len += 11;
	}
}

#endif /* DIGRAPHS */
