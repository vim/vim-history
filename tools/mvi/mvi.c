/* 

From: tye@spillman.uucp (E. Tye McQueen)
Subject: Re: Using 'rename'
Date: Fri, 05 Jun 1992 19:11:16 GMT
Distribution: comp.lang.perl
Organization: Spillman Data Systems
Followup-To: alt.sources.d
Lines: 357

tchrist@convex.COM (Tom Christiansen) writes:
)From the keyboard of glenn@clsi.COM (Glenn Boysko):
):  % ls -1 | perl -ne 'print "Failed: $_" unless rename($_, s/foo/bar/);'
)
)Substitute returns not the new string, but the count of successful 
)substitutions.

No, it's not Perl (it was written long before I'd heard of Perl),
but it is very handy for this.  [Cross-post to alt.sources with
follow-up directed to alt.sources.d]  It hasn't needed other than
minor changes for years so the design and coding style are old and
not defended by the author (me).

*/

/* mvi.c -- mv (rename) files using vi (visual editor).
 *      by Tye McQueen
 */

#include <ctype.h>      /* ispunct() */
#include <errno.h>      /* errno */
#include <setjmp.h>     /* jmp_buf setjmp() longjmp() */
#include <signal.h>     /* signal() SIG_DFL SIG_IGN */
#include <stdio.h>      /* FILE f*() */

extern char *getenv();
static int rmtmp= 0;        /* If tmp needs to be deleted */
static jmp_buf death;      /* Place to jump to to have tmp deleted */
static char *Self;         /* Program name (argument zero) */
static int Verbose= 0;     /* If we echo each renaming to stderr */
static char *Editor= NULL; /* Editor to use to edit file names... */
static char **Eargs= NULL; /* ...and arguments to pass to it */

#define  Fprintf  (void) fprintf
#define  Sprintf  (void) sprintf
#define  Signal   (void) signal
#define  Wait     (void) wait

static void
die( text )
  char *text;
{
        if(  ispunct( text[strlen(text)-1] )  ) {
                Fprintf( stderr, "%s: %s\n", Self, text );
        } else {
                Fprintf( stderr, "%s: %s%c\n", Self, text, errno ? ':' : '.' );
                if(  0 != errno  )
                        perror( Self );
        }
        if(  rmtmp  )
                longjmp( death, 1 );
        exit( 1 );
}
static void
Unlink( name )
  char *name;
{
        errno= 0;
        if(  0 != unlink(name)  ) {
          char msg[255];
                Sprintf( msg, "Error deleting `%s'", name );
                die( msg );
        }
}
static FILE *
Fopen( name, acc )
  char *name;
  char *acc;
{
  FILE *fp;
        errno= 0;
        if(  NULL == ( fp= fopen(name,acc) )  ) {
          char msg[255];
                Sprintf( msg, "Error opening `%s' for `%s' access", name, acc );
                die( msg );
        }
        return( fp );
}
static void
Fclose( fp )
  FILE *fp;
{
        errno= 0;
        if(  0 != fclose(fp)  )
                die( "Error closing file" );
}

static void
usage()
{
        Fprintf( stderr,
          "usage: %s [-v] [-e editor [options -e]] wildcard(s)\n", Self );
        Fprintf( stderr, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
          " -v  Verbose- echo each name change as it is done.",
          " -e  Editor - specifies an alternate editor to use (instead of that",
          "       specified by environment variable VISUAL or EDITOR or just",
          "       vi) and optionally specifies command line arguments for it.",
          " --  Quotes a `-' at the front of an argument.  After the first",
          "       argument that cannot be an option, this quoting is ignored.",
          "       Note that using `-e editor' witout a closing `-e' means",
          "       all arguments must be checked.",
          " Examples:" );
        Fprintf( stderr, "   %s -e myed --e -e *\n", Self );
        Fprintf( stderr, "%s\n%s\n",
          "     uses `myed -e /tmp/mvi????' to edit the names of all files in",
          "     the current working directory." );
        Fprintf( stderr, "   %s -e ex --file1- --file2-\n", Self );
        Fprintf( stderr, "   %s -e ex -e --file1- -file2-\n", Self );
        Fprintf( stderr,
          "     both use `ex' to edit the names of `-file1-' and `-file2-'\n" );
        exit( 1 );
}

static char **
flags( argc, argv )
  int *argc;
  char **argv;
{
        Self= *argv;
        while(  --*argc &&  '-' == **++argv  ) {
                switch(  *++*argv  ) {
                  case 'e':  case 'E':
                        --*argc;
                        if(  NULL == *( Eargs= ++argv )  )
                                die( "Name of editor must follow -e." );
                        Editor= *Eargs;
                        while(  --*argc,  *++argv  ) {
                                if(  '-' == **argv  )
                                        switch( argv[0][1]  ) {
                                          case '-':
                                                if( '\0' == *++*argv  )   die(
                                                  "Invalid argument: -" );
                                                break;
                                          case 'e':  case 'E':
                                                *argv= NULL;
                                                break;
                                        }
                                if(  NULL == *argv  )
                                        break;
                        }
                        break;
                  case 'v':  case 'V':
                        Verbose= !Verbose;
                        break;
                  case '-':
                        ++*argv;
                        return( argv );
                  default:
                    { char msg[255];
                        Sprintf( msg, "Unknown switch `-%s'.", *argv );
                        die( msg );
                    }
                }
        }
        if(  NULL == Editor  ) {
                Editor= getenv("VISUAL");
                if(  NULL == Editor  )
                        Editor= getenv("EDITOR");
                if(  NULL == Editor  )
                        Editor= "vi";
        }
        return( argv );
}

static void
writemp( tmp, head, argv )
  char *tmp;
  char *head;
  char **argv;
{
  FILE *fp= Fopen( tmp, "w" );
        rmtmp= 1;
        Fprintf( fp, head );
        while(  *argv  )
                Fprintf( fp, "%s\n", *(argv++) );
        Fclose( fp );
}

static void
editemp( tmp )
  char *tmp;
{
  char **larg;
        switch(  fork()  ) {
          case 0:       /* Subprocess */
                if(  NULL != Eargs  ) {
                        larg= Eargs;
                        while(  *larg  )
                                ++larg;
                        *larg= tmp;
                        *++larg= NULL;
                        execvp( Editor, Eargs );
                } else
                        execlp( Editor, Editor, tmp, (char *)0 );
                /* Fall through if error */
          case -1:      /* Error */
                Fprintf( stderr, "%s: can't exec %s\n", Self, Editor );
                Unlink( tmp );
                exit( 1 );
        }
        Signal( SIGINT, SIG_IGN );
        Signal( SIGQUIT, SIG_IGN );
        Wait( (int *)0 );
        Signal( SIGINT, SIG_DFL );
        Signal( SIGQUIT, SIG_DFL );
}

static void
chktemp( tmp, head, argv )
  char *tmp;
  char *head;
  char **argv;
{
  char new[255], *cp;
  FILE *fp= Fopen( tmp, "r" );
        for(  cp= head;  *cp;  cp++  )
                if(  fgetc(fp) != *cp  )
                        die( "First line modified.  Abort." );
        for(  ;  fgets(new,sizeof(new),fp);  argv++  ) {
                cp= new + strlen(new) - 1;
                if(  '\n' != *cp  ) {
                  char msg[128];
                        Sprintf( msg, "New name is longer than %u characters.",
                          sizeof(new) );
                        die( msg );
                }
                *cp= '\0';
                if(  NULL == argv  )
                        die( "Extra line(s) added to file.  Abort." );
        }
        if(  NULL != *argv  )
                die( "Line(s) deleted from file.  Abort." );
        Fclose( fp );
}

static int cnt= 0, dcnt= 0, lcnt= 0;

static void
domv( tmp, head, argv )
  char *tmp;
  char *head;
  char **argv;
{
  char new[255], *cp;
  FILE *fp= Fopen( tmp, "r" );
        for(  cp= head;  *cp;  cp++  )
                if(  fgetc(fp) != *cp  )
                        die( "First line modified.  Abort." );
        for(  ;  fgets(new,sizeof(new),fp);  argv++  ) {
                cp= new + strlen(new) - 1;
                if(  '\n' != *cp  ) {
                  char msg[128];
                        Sprintf( msg, "New name is longer than %u characters.",
                          sizeof(new) );
                        die( msg );
                }
                *cp= '\0';
                if(  NULL == argv  )
                        die( "Extra lines added to file.  Abort." );
                if(  0 == strncmp( new, "D ", 2 )  ) {
                        if(  0 != strcmp( new+2, *argv )  ) {
                          char msg[512];
                                Sprintf( msg, "`%s' became `%s'.  Abort.",
                                  *argv, new );
                                die( msg );
                        }
                        errno= 0;
                        if(  0 != unlink( *argv )  ) {
                          char msg[255];
                                Fprintf( stderr, "Error deleting `%s':\n",
                                  *argv );
                                perror( Self );
                        } else {
                                dcnt++;
                                if(  Verbose  )
                                    Fprintf( stdout, "rm %s\n", *argv );
                        }
                } else if(  0 == strncmp( new, "L ", 2 )  ) {
                        if(  0 == strcmp( new+2, *argv )  ) {
                                Fprintf( stderr,
                                  "%s: Won't link `%s' to itself\n",
                                  Self, *argv );
                        } else {
                                errno= 0;
                                if(  0 <= link( *argv, new+2 )  ) {
                                        lcnt++;
                                        if(  Verbose  )
                                            Fprintf( stdout, "ln %s %s\n",
                                              *argv, new+2 );
                                } else {
                                        Fprintf( stderr,
                                          "%s: Couldn't link `%s' to `%s'\n",
                                          Self, *argv, new+2 );
                                        perror( Self );
                                }
                        }
                } else if(  0 != strcmp( *argv, new )  ) {
                        errno= 0;
                        if(  0 <= link( *argv, new )  ) {
                                cnt++;
                                if(  Verbose  )
                                        Fprintf( stdout, "mv %s %s\n",
                                          *argv, new );
                                Unlink( *argv );
                        } else {
                                Fprintf( stderr,
                                  "%s: Couldn't rename `%s' to `%s'\n",
                                  Self, *argv, new );
                                perror( Self );
                        }
                }
        }
        if(  NULL != *argv  )
                Fprintf( stderr, "%s: Warning, line(s) were deleted.\n", Self );
        Fclose( fp );
}

static void
summarize()
{
        Fprintf( stderr, "%s: %d file%s renamed, %d deleted, %d linked.\n",
          Self, cnt, 1==cnt ? "" : "s", dcnt, lcnt );
}

void
main( argc, argv )
  int argc;
  char **argv;
{
  char tmp[40];
  static char head[]=
"Edit file name to rename, \"D file\" to delete, \"L link\" to link.\n";
        argv= flags( &argc, argv );
        if(  argc <= 0  )
                usage();
        Sprintf( tmp, "/tmp/mvi%d", getpid() );
        if(  0 != setjmp( death )  ) {
                (void) unlink( tmp );
                summarize();
                exit( 1 );
        }
        writemp( tmp, head, argv );
        editemp( tmp );
        chktemp( tmp, head, argv );
        domv( tmp, head, argv );
        Unlink( tmp );
        summarize();
}

/*

-- 
 Tye McQueen, E.     [if uucp]             [if internet]
                   [...uunet!]spillman!tye[@uunet.uu.net]
----------------------------------------------------------
 Nothing is obvious unless you are overlooking something. 
----------------------------------------------------------

*/
