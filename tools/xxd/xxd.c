/* xxd: my hexdump facility. jw
 * changed to word output 2.10.90
 * new indent style, dumb bug inserted and fixed. 3.3.93
 * -c option, mls
 * better option parser, -ps, -l, -s  added, 26.4.94
 * -r badly needs - as input file, added. 1.7.94
 * Per default autoskip over consequtive lines of zeroes, as unix od does.
 * -a shows them too.
 * -i dump as c-style #include "file.h"
 *    if "xxd -i" knows the filename, an 'unsigned char filename_bits[]' array
 *    is written in correct c-syntax. added 1.11.95
 * -s improved, now defaults to absolute seek, relative requires a '+'.
 * -r improved, now -r -s -0x... is supported change/suppress leading \0 bytes.
 * -l n improved: stops exactly after n bytes, even if partial lines result.
 * -r improved, better handling of partial lines with trailing garbage.
 * -r improved, now -r -p works again!
 * -r improved, less flushing, much faster now! (that was silly)
 * Per repeated request of a single person: autoskip defaults to off.
 *	3.4.96. jw.
 * -v added. They want to know the version. 15.5.96
 * -a fixed, to show last line inf file ends in all zeros.
 * -u added: Print upper case hex-letters, as preferred by unix bc.
 * -h added to usage message. Usage message extended.
 * Now using outfile if specified even in normal mode, aehem.
 * No longer mixing of ints and longs. May help doze people.
 * Added binify ioctl for same reason. (Enough Doze stress for 1996!)
 * -p improved, removed occasional superfluous linefeed.  16.5.96
 * -l 0 fixed. tried to read anyway. 20.5.96
 *
 * (c) 1990-1996 by Juergen Weigert (jnweiger@informatik.uni-erlangen.de)
 *
 * Distribute freely and credit me,
 * make money and share with me,
 * lose money and don't ask me.
 */
#include <stdio.h>
#include <fcntl.h>
#ifdef __TSC__
# define MSDOS
#endif
#if defined(MSDOS) || defined(WIN32) || defined(OS2)
# include <io.h>	/* for setmode() */
#else
# include <unistd.h>
#endif
#include <stdlib.h>
#include <string.h>	/* for strncmp() */
#include <ctype.h>	/* for isalnum() */

extern long int strtol();
extern long int ftell();

char version[] = "xxd V1.1m 21may96 by Juergen Weigert";

#if defined(MSDOS) || defined(WIN32) || defined(OS2)
# define BIN_READ(yes)  ((yes) ? "rb" : "rt")
# define BIN_WRITE(yes) ((yes) ? "wb" : "wt")
# define BIN_ASSIGN(fp, yes) setmode(fileno(fp), (yes) ? O_BINARY : O_TEXT)
#else
# define BIN_READ(dummy)  "r"
# define BIN_WRITE(dummy) "w"
# define BIN_ASSIGN(fp, dummy) fp
#endif

#define TRY_SEEK 	/* attempt to use lseek, or skip forward by reading */
#define COLS 64		/* change here, if you ever need more columns */
#define LLEN (9 + (5*COLS-1)/2 + 2 + COLS)

char hexxa[] = "0123456789abcdef0123456789ABCDEF", *hexx = hexxa;

/* the different hextypes known by this program: */
#define HEX_NORMAL 0
#define HEX_POSTSCRIPT 1
#define HEX_CINCLUDE 2

void
exit_with_usage(pname)
char *pname;
{
  fprintf(stderr, "Usage:\n       %s [options] [infile [outfile]]\n", pname);
  fprintf(stderr, "    or\n       %s -r [-s [-]offset] [-c cols] [-ps] [infile [outfile]]\n", pname);
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "    -a          toggle autoskip: A single '*' replaces nul-lines. Default off.\n");
  fprintf(stderr, "    -c cols     format <cols> octets per line. Default 16 (-i: 12, -ps: 30).\n");
  fprintf(stderr, "    -h          print this summary.\n");
  fprintf(stderr, "    -i          output in C include file style.\n");
  fprintf(stderr, "    -l len      stop after <len> octets.\n");
  fprintf(stderr, "    -ps         output in postscript continuous hexdump style.\n");
  fprintf(stderr, "    -r          reverse operation: convert (or patch) hexdump into binary.\n");
  fprintf(stderr, "    -r -s off   revert with <off> added to file positions found in hexdump.\n");
  fprintf(stderr, "    -s %sseek  start at <seek> bytes abs. %sinfile offset.\n", 
#ifdef TRY_SEEK
	  "[+][-]", "(or +: rel.) ");
#else
	  "", "");
#endif
  fprintf(stderr, "    -u          use upper case hex letters.\n");
  fprintf(stderr, "    -v          show version: \"%s\".\n", version);
  exit(1);
}

/* 
 * Max. cols binary characters are decoded from the input stream per line.
 * Two adjacent garbage characters after evaluated data delimit valid data.
 * Everything up to the next newline is discarded. 
 *
 * The name is historic and came from 'undo type opt h'.
 */
int
huntype(fpi, fpo, fperr, pname, cols, hextype, base_off)
FILE *fpi, *fpo, *fperr;
char *pname;
int cols, hextype;
long base_off;
{
  int c, ign_garb = 1, n1 = -1, n2 = 0, n3, p = cols;
  long have_off = 0, want_off = 0;

  rewind(fpi);

  while ((c = getc(fpi)) != EOF)
    {
      if (c == '\r')	/* Doze style input file? */
        continue;

      n3 = n2;
      n2 = n1;

      if (c >= '0' && c <= '9')
        n1 = c - '0';
      else if (c >= 'a' && c <= 'f')
        n1 = c - 'a' + 10;
      else if (c >= 'A' && c <= 'F')
        n1 = c - 'A' + 10;
      else 
        {
          n1 = -1;
	  if (ign_garb)
	    continue;
	}

      ign_garb = 0;

      if (p >= cols)
	{
	  if (!hextype)
	    {
	      if (n1 < 0)
		{
		  p = 0;
		  continue;
		}
	      want_off = (want_off << 4) | n1;
	      continue;
	    }
	  else
	    p = 0;
	}

      if (base_off + want_off != have_off)
        {
	  fflush(fpo);
#ifdef TRY_SEEK
	  c = fseek(fpo, base_off + want_off - have_off, 1);
	  if (c >= 0)
	    have_off = base_off + want_off;
#endif
	  if (base_off + want_off < have_off)
	    {
	      fprintf(fperr, "%s: sorry, cannot seek backwards.\n", pname);
	      return 5;
	    }
	  for (; have_off < base_off + want_off; have_off++)
	    putc(0, fpo);
	}

      if (n2 >= 0 && n1 >= 0)
        {
	  putc((n2 << 4) | n1, fpo);
	  have_off++;
	  want_off++;
	  n1 = -1;
	  if (++p >= cols)
	    {
	      if (!hextype)
	        want_off = 0;
	      while ((c = getc(fpi)) != '\n' && c != EOF)
	        ;
	      ign_garb = 1;
	    }
	}
      else if (n1 < 0 && n2 < 0 && n3 < 0)
        {
	  if (!hextype)
	    want_off = 0;
	  while ((c = getc(fpi)) != '\n' && c != EOF)
	    ;
	  ign_garb = 1;
	}
    }
  fflush(fpo);
#ifdef TRY_SEEK
  fseek(fpo, 0, 2);
#endif
  fclose(fpo);
  fclose(fpi);
  return 0;
}

/*
 * Print line l. If nz is false, outline regards the line a line of 
 * zeroes. If there are three or more consecutive lines of zeroes,
 * they are replaced by a single '*' character. 
 *
 * If the output ends with more than two lines of zeroes, you
 * should call outline again with l being the last line and nz 
 * negative. This ensures that the last line is shown even when
 * it is all zeroes.
 * 
 * If nz is always positive, lines are never suppressed.
 */
static void
outline(fp, l, nz)
FILE *fp;
char *l;
int nz;
{
  static char z[LLEN+1];
  static int zero_seen = 0;

  if (!nz && zero_seen == 1)
    strcpy(z, l);

  if (nz || !zero_seen++)
    {
      if (nz)
	{
	  if (nz < 0)
	    zero_seen--;
	  if (zero_seen == 2)
	    fputs(z, fp);
	  if (zero_seen > 2)
	    fputs("*\n", fp);
	}
      if (nz >= 0 || zero_seen > 0)
        fputs(l, fp);
      if (nz)
	zero_seen = 0;
    }
}

int
main(argc, argv)
int argc;
char *argv[];
{
  FILE *fp, *fpo;
  int c, e, p = 0, relseek = 1, negseek = 0, hextype, revert = 0;
  int autoskip = 0;
  long length = -1, n = 0, seekoff = 0;
  char l[LLEN+1];
  int cols = 0, nonzero = 0;
  char *pname, *pp;
  hextype = HEX_NORMAL;

  pname = argv[0];
  for (pp = pname; *pp; )
    if (*pp++ == '/')
      pname = pp;

  while (argc >= 2)
    {
           if (!strncmp(argv[1], "-a", 2)) autoskip = 1 - autoskip;
      else if (!strncmp(argv[1], "-u", 2)) hexx = hexxa + 16;
      else if (!strncmp(argv[1], "-p", 2)) hextype = HEX_POSTSCRIPT;
      else if (!strncmp(argv[1], "-i", 2)) hextype = HEX_CINCLUDE;
      else if (!strncmp(argv[1], "-r", 2)) revert++;
      else if (!strncmp(argv[1], "-c", 2))
	{
	  if (argv[1][2] && strncmp("ols", argv[1] + 2, 3))
	    cols = (int)strtol(argv[1] + 2, NULL, 0);
	  else
	    {
	      if (!argv[2])
	        exit_with_usage(pname);
	      cols = (int)strtol(argv[2], NULL, 0);
	      argv++;
	      argc--;
	    }
	}
      else if (!strncmp(argv[1], "-s", 2))
	{
	  relseek = 0;
	  negseek = 0;
	  if (argv[1][2] && 
	      strncmp("kip", argv[1] +2, 3) && strncmp("eek", argv[1] +2, 3))
	    {
#ifdef TRY_SEEK
	      if (argv[1][2] == '+')
	        relseek++;
	      if (argv[1][2+relseek] == '-')
	        negseek++;
#endif
	      seekoff = strtol(argv[1] + 2+relseek+negseek, (char **)NULL, 0);
	    }
	  else
	    {
	      if (!argv[2])
	        exit_with_usage(pname);
#ifdef TRY_SEEK
	      if (argv[2][0] == '+')
	        relseek++;
	      if (argv[2][relseek] == '-')
	        negseek++;
#endif
	      seekoff = strtol(argv[2] + relseek+negseek, (char **)NULL, 0);
	      argv++;
	      argc--;
	    }
	}
      else if (!strncmp(argv[1], "-l", 2))
	{
	  if (argv[1][2] && strncmp("en", argv[1] + 2, 2))
	    length = strtol(argv[1] + 2, (char **)NULL, 0);
	  else
	    {
	      if (!argv[2])
	        exit_with_usage(pname);
	      length = strtol(argv[2], (char **)NULL, 0);
	      argv++;
	      argc--;
	    }
	}
      else if (!strcmp(argv[1], "-v"))
        {
	  fprintf(stderr, "%s\n", version);
	  exit(0);
	}
      else if (!strcmp(argv[1], "--"))	/* end of options */
        {
	  argv++;
	  argc--;
	  break;
	}
      else if (argv[1][0] == '-' && argv[1][1])	/* unknown option */
        exit_with_usage(pname);
      else 
        break;				/* not an option */

      argv++;				/* advance to next argument */
      argc--;
    }

  if (!cols)
    switch (hextype)
      {
      case HEX_POSTSCRIPT: 	cols = 30; break;
      case HEX_CINCLUDE:	cols = 12; break;
      case HEX_NORMAL:
      default:			cols = 16; break;
      }

  if (cols < 1 || (!hextype && (cols > COLS)))
    {
      fprintf(stderr, "%s: invalid number of columns (max. %d).\n", pname, COLS);
      exit(1);
    }

  if (argc > 3) 
    exit_with_usage(pname);

  if (argc == 1 || (argv[1][0] == '-' && !argv[1][1]))
    BIN_ASSIGN(fp = stdin, !revert);
  else
    {
      if ((fp = fopen(argv[1], BIN_READ(!revert))) == NULL)
	{
	  fprintf(stderr,"%s: ", pname);
	  perror(argv[1]);
	  return 2;
	}
    }
  
  if (argc < 3 || (argv[2][0] == '-' && !argv[2][1]))
    BIN_ASSIGN(fpo = stdout, revert);
  else
    {
      int fd;

      if (((fd = open(argv[2], O_WRONLY | O_CREAT, 0666)) < 0) ||
          (fpo = fdopen(fd, BIN_WRITE(revert))) == NULL)
	{
	  fprintf(stderr, "%s: ", pname);
	  perror(argv[2]);
	  return 3;
	}
      rewind(fpo);
    }
  
  if (revert)
    {
      if (hextype && (hextype != HEX_POSTSCRIPT))
        {
          fprintf(stderr, "%s: sorry, cannot revert this type of hexdump\n", pname);
	  return -1;
	}
      return huntype(fp, fpo, stderr, pname, cols, hextype, 
      		negseek ? -seekoff : seekoff);
    }

  if (seekoff || negseek || !relseek)
    {
#ifdef TRY_SEEK
      if (relseek)
        e = fseek(fp, negseek ? -seekoff : seekoff, 1);
      else
        e = fseek(fp, negseek ? -seekoff : seekoff, negseek ? 2 : 0);
      if (e < 0 && negseek)
        {
	  fprintf(stderr, "%s: sorry cannot seek.\n", pname);
	  return 4;
	}
      if (e >= 0)
        seekoff = ftell(fp);
      else
#endif
	{
	  long s = seekoff;

	  while (s--)
	    getc(fp); 
	}
    }

  if (hextype == HEX_CINCLUDE)
    {
      if (fp != stdin)
	{
	  fprintf(fpo, "unsigned char ");
	  for (e = 0; (c = argv[1][e]); e++)
	    putc(isalnum(c) ? c : '_', fpo);
	  fputs("[] = {\n", fpo);
	}

      p = 0;
      while ((length < 0 || p < length) && (c = getc(fp)) != EOF)
	{
	  fprintf(fpo, "%s0x%02x", (p % cols) ? ", " : ",\n  "+2*!p,  c);
	  p++;
	}

      if (p)
        fputs("\n};\n"+3*(fp == stdin), fpo);

      if (fp != stdin)
	{
	  fprintf(fpo, "unsigned int ");
	  for (e = 0; (c = argv[1][e]); e++)
	    putc(isalnum(c) ? c : '_', fpo);
	  fprintf(fpo, "_len = %d;\n", p);
	}

      fclose(fp);
      fclose(fpo);
      return 0;
    }

  if (hextype == HEX_POSTSCRIPT)
    {
      p = cols;
      while ((length < 0 || n < length) && (e = getc(fp)) != EOF)
	{
          putchar(hexx[(e >> 4) & 0xf]);
          putchar(hexx[(e     ) & 0xf]);
	  n++;
	  if (!--p)
	    {
	      putchar('\n');
	      p = cols;
	    }
	}
      if (p < cols)
	putchar('\n');
      fclose(fp);
      fclose(fpo);
      return 0;
    }

  /* hextype == HEX_NORMAL */

  while ((length < 0 || n < length) && (e = getc(fp)) != EOF)
    {
      if (p == 0)
	{
	  sprintf(l, "%07lx: ", n + seekoff);
	  for (c = 9; c < LLEN; l[c++] = ' ');
	}
      l[ 9 + (5 * p) / 2] = hexx[(e >> 4) & 0xf];
      l[10 + (5 * p) / 2] = hexx[ e       & 0xf];
      l[11 + (5 * cols - 1) / 2 + p] = (e > 31 && e < 127) ? e : '.';
      if (e) 
        nonzero++;
      n++;
      if (++p == cols)
	{
	  l[c = (11 + (5 * cols - 1) / 2 + p)] = '\n'; l[++c] = '\0';
	  outline(fpo, l, autoskip ? nonzero : 1);
	  nonzero = 0;
	  p = 0;
	}
    }
  if (p)
    {
      l[c = (11 + (5 * cols - 1) / 2 + p)] = '\n'; l[++c] = '\0';
      outline(fpo, l, 1);
    }
  else if (autoskip)
    outline(fpo, l, -1);	/* last chance to flush out supressed lines */
    
  fclose(fp);
  fclose(fpo);
  return 0;
}
