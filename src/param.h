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
 * param.h: definition of global variables for settable parameters
 *
 * EXTERN is only defined in main.c (and vim.h)
 */

#ifndef EXTERN
# define EXTERN extern
# define INIT(x)
#else
# ifndef INIT
#  define INIT(x) x
# endif
#endif

/*
 * The following are actual variabables for the parameters
 */

EXTERN int	p_ai	INIT(= FALSE);		/* auto-indent */
EXTERN int	p_aw	INIT(= FALSE);		/* auto-write */
EXTERN long	p_bs	INIT(= 0);			/* backspace over newlines in insert mode */
EXTERN int	p_bk	INIT(= TRUE);		/* make backups when writing out files */
#ifdef UNIX
EXTERN char *p_bdir	INIT(= BACKUPDIR);	/* directory for backups */
#endif
#ifdef DIGRAPHS
EXTERN int	p_dg	INIT(= FALSE);		/* enable digraphs */
#endif /* DIGRAPHS */
EXTERN char *p_dir	INIT(= "");			/* directory for autoscript file */
EXTERN char *p_ep	INIT(= "indent");	/* program name for '=' command */
EXTERN int	p_eb	INIT(= FALSE);		/* ring bell for errors */
#ifdef AMIGA
EXTERN char *p_ef	INIT(= "AztecC.Err");	/* name of errorfile */
#else
EXTERN char *p_ef	INIT(= "");			/* name of errorfile */
#endif
EXTERN int	p_et	INIT(= FALSE);		/* expand tabs into spaces */
#ifdef MSDOS
EXTERN int	p_gr	INIT(= TRUE);		/* display graphic characters */
#else
EXTERN int	p_gr	INIT(= FALSE);		/* display graphic characters */
#endif
EXTERN long p_hi	INIT(= 20);			/* command line history size */
EXTERN char *p_hf	INIT(= VIM_HLP);	/* name of help file */
EXTERN int	p_ic	INIT(= FALSE);		/* ignore case in searches */
EXTERN int	p_im	INIT(= FALSE);		/* start editing in input mode */
EXTERN char *p_kp	INIT(= "ref");		/* keyword program */
EXTERN int	p_js	INIT(= TRUE);		/* use two spaces after period with Join */
EXTERN int	p_list	INIT(= FALSE);		/* show tabs and newlines graphically */
EXTERN int	p_magic INIT(= TRUE);		/* use some characters for reg exp */
EXTERN long p_ml	INIT(= 5);			/* number of mode lines */
EXTERN int	p_nu	INIT(= FALSE);		/* number lines on the screen */
EXTERN char *p_para	INIT(= "IPLPPPQPP LIpplpipbp");		/* paragraphs */
EXTERN int	p_ro	INIT(= FALSE);		/* readonly */
EXTERN int	p_remap	INIT(= TRUE);		/* remap */
EXTERN int	p_rd	INIT(= TRUE);		/* delete when replacing */
EXTERN long	p_report	INIT(= 2);		/* minimum number of lines for report */
EXTERN int	p_ru	INIT(= FALSE);		/* show column/line number */
EXTERN long	p_scroll	INIT(= 12);		/* scroll size */
EXTERN long	p_sj	INIT(= 1);			/* scroll jump size */
EXTERN char *p_sections	INIT(= "SHNHH HUnhsh");		/* sections */
#ifdef MSDOS
EXTERN char *p_sh 	INIT(= "command");		/* name of shell to use */
#else
EXTERN char *p_sh 	INIT(= "sh");		/* name of shell to use */
#endif
EXTERN long	p_st	INIT(= 0);			/* type of shell */
EXTERN int	p_sr	INIT(= FALSE);		/* shift round off (for < and >) */
EXTERN long	p_sw	INIT(= 8);			/* shiftwidth (for < and >) */
EXTERN int	p_sc	INIT(= TRUE);		/* show command in status line */
EXTERN int	p_sm	INIT(= FALSE);		/* showmatch */
EXTERN int	p_mo	INIT(= TRUE);		/* show mode */
EXTERN int	p_si	INIT(= FALSE);		/* smart-indent for c programs */
EXTERN char *p_su	INIT(= ".bak.o.h.info.vim");	/* suffixes for wildcard expansion */
EXTERN long p_ts	INIT(= 8);			/* tab size in the file */
EXTERN long p_tl	INIT(= 0);			/* used tag length */
EXTERN char *p_tags	INIT(= "tags");		/* tags search path */
EXTERN int	p_terse	INIT(= TRUE);		/* terse (not used) */
#ifdef MSDOS
EXTERN int	p_tx	INIT(= TRUE);		/* textmode for file I/O */
#endif
EXTERN long p_tw	INIT(= 9999);		/* textwidth */
EXTERN int	p_to	INIT(= FALSE);		/* tilde is an operator */
EXTERN int	p_timeout	INIT(= TRUE);	/* mappings entered within one second */
EXTERN long p_ul	INIT(= 100);		/* number of Undo Levels */
EXTERN long p_uc	INIT(= 100);		/* update count for auto script file */
EXTERN long p_ut	INIT(= 2000);		/* update time for auto script file */
EXTERN int	p_vb	INIT(= FALSE);		/* visual bell only (no beep) */
EXTERN int	p_warn	INIT(= TRUE);		/* warn for changes at shell command */
EXTERN int	p_ws	INIT(= TRUE);		/* wrap scan */
EXTERN long p_wm	INIT(= 0);			/* wrapmargin */
EXTERN int	p_wa	INIT(= FALSE);		/* write any */
EXTERN int	p_wb	INIT(= TRUE);		/* write backup files */
EXTERN int	p_ye	INIT(= FALSE);		/* Y yanks to end of line */
