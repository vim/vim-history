/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
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

EXTERN int	p_aw	INIT(= FALSE);		/* auto-write */
EXTERN long	p_bs	INIT(= 0);			/* backspace over newlines in insert mode */
#if defined(COMPATIBLE) || defined(NOBACKUP)
EXTERN int	p_bk	INIT(= FALSE);		/* make backups when writing out files */
#else
EXTERN int	p_bk	INIT(= TRUE);		/* make backups when writing out files */
#endif
#ifdef MSDOS
EXTERN int	p_biosk	INIT(= TRUE);		/* Use bioskey() instead of kbhit() */
#endif
#ifdef UNIX
EXTERN char_u *p_bdir	INIT(= (char_u *)BACKUPDIR);	/* directory for backups */
#endif
EXTERN long	p_ch	INIT(= 1L);			/* command line height */
EXTERN int	p_cp	INIT(= FALSE);		/* vi-compatible */
#ifdef DIGRAPHS
EXTERN int	p_dg	INIT(= FALSE);		/* enable digraphs */
#endif /* DIGRAPHS */
EXTERN char_u *p_dir	INIT(= (char_u *)DEF_DIR);	/* directory for swap file */
EXTERN char_u *p_ep	INIT(= (char_u *)"indent");	/* program name for '=' command */
EXTERN int	p_ea	INIT(= TRUE);		/* make windows equal height */
EXTERN int	p_ed	INIT(= FALSE);		/* :s is ed compatible */
EXTERN int	p_eb	INIT(= FALSE);		/* ring bell for errors */
#ifdef AMIGA
EXTERN char_u *p_ef	INIT(= (char_u *)"AztecC.Err");	/* name of errorfile */
#else
EXTERN char_u *p_ef	INIT(= (char_u *)"errors");			/* name of errorfile */
#endif
#ifdef AMIGA
EXTERN char_u *p_efm	INIT(= (char_u *)"%f>%l:%c:%t:%n:%m");/* error format */
#else
# ifdef ARCHIE
EXTERN char_u *p_efm	INIT(= (char_u *)"%f:%l:%m");	/* error format */
# else
EXTERN char_u *p_efm	INIT(= (char_u *)"\"%f\",%*[^0123456789]%l: %m");	/* error format */
# endif
#endif
#ifdef COMPATIBLE
EXTERN int	p_ek	INIT(= FALSE);		/* function keys with ESC in insert mode */
#else
EXTERN int	p_ek	INIT(= TRUE);		/* function keys with ESC in insert mode */
#endif
EXTERN int	p_exrc	INIT(= FALSE);		/* read .exrc in current dir */
EXTERN char_u *p_fp	INIT(= (char_u *)"");			/* name of format program */
EXTERN int	p_gd	INIT(= FALSE);		/* /g is default for :s */
#ifdef MSDOS
EXTERN int	p_gr	INIT(= TRUE);		/* display graphic characters */
#else
EXTERN int	p_gr	INIT(= FALSE);		/* display graphic characters */
#endif
EXTERN int	p_icon	INIT(= FALSE);		/* put file name in icon if possible */
EXTERN long p_hi	INIT(= 20);			/* command line history size */
EXTERN char_u *p_hf	INIT(= (char_u *)VIM_HLP);	/* name of help file */
EXTERN int	p_hid	INIT(= FALSE);		/* buffers can be hidden */
EXTERN char_u *p_hl	INIT(= (char_u *)"db,es,hs,rs,vi,si");
										/* which highlight mode to use */
EXTERN int	p_ic	INIT(= FALSE);		/* ignore case in searches */
EXTERN int	p_im	INIT(= FALSE);		/* start editing in input mode */
EXTERN int	p_wi	INIT(= FALSE);		/* inversion of text is weird */
EXTERN char_u *p_kp	INIT(= (char_u *)"ref");		/* keyword program */
EXTERN int	p_js	INIT(= TRUE);		/* use two spaces after period with Join */
EXTERN long	p_ls	INIT(= 1);			/* last window has status line */
EXTERN int	p_magic INIT(= TRUE);		/* use some characters for reg exp */
EXTERN char_u *p_mp	INIT(= (char_u *)"make");		/* program for :make command */
EXTERN long p_mm	INIT(= MAXMEM);		/* maximal amount of memory for buffer */
EXTERN long p_mmt	INIT(= MAXMEMTOT);	/* maximal amount of memory for Vim */
EXTERN long p_mls	INIT(= 5);			/* number of mode lines */
#ifdef COMPATIBLE
EXTERN int	p_more	INIT(= FALSE);		/* wait when screen full when listing */
#else
EXTERN int	p_more	INIT(= TRUE);		/* wait when screen full when listing */
#endif
EXTERN int	p_nb	INIT(= FALSE);		/* screen output not buffered (for testing) */
EXTERN int	p_paste	INIT(= FALSE);		/* paste mode */
#ifdef AMIGA
EXTERN char_u *p_path	INIT(= (char_u *)".");	/* path for "]f" and "^Wf" */
#else
# ifdef MSDOS
EXTERN char_u *p_path	INIT(= (char_u *)".");	/* path for "]f" and "^Wf" */
# else
EXTERN char_u *p_path	INIT(= (char_u *)". /usr/include");	/* path for "]f" and "^Wf" */
# endif
#endif
EXTERN char_u *p_pm INIT(= (char_u *)"");  /* patchmode file suffix */
EXTERN char_u *p_para	INIT(= (char_u *)"IPLPPPQPP LIpplpipbp");		/* paragraphs */
EXTERN int	p_remap	INIT(= TRUE);		/* remap */
EXTERN long	p_report	INIT(= 2);		/* minimum number of lines for report */
EXTERN int	p_ru	INIT(= FALSE);		/* show column/line number */
EXTERN int	p_ri	INIT(= FALSE);		/* reverse direction of insert */
EXTERN int	p_secure	INIT(= FALSE);	/* do .exrc and .vimrc in secure mode */
EXTERN long	p_sj	INIT(= 1);			/* scroll jump size */
EXTERN char_u *p_sections	INIT(= (char_u *)"SHNHH HUnhsh");		/* sections */
#ifdef MSDOS
EXTERN char_u *p_sh 	INIT(= (char_u *)"command");		/* name of shell to use */
#else
# ifdef ARCHIE
EXTERN char_u *p_sh 	INIT(= (char_u *)"gos");	/* name of shell to use */
# else
EXTERN char_u *p_sh 	INIT(= (char_u *)"sh");		/* name of shell to use */
# endif
#endif
#ifdef UNIX
# ifdef ARCHIE
EXTERN char_u *p_sp	INIT(= (char_u *)"2>");		/* string for output of make */
# else
EXTERN char_u *p_sp	INIT(= (char_u *)"| tee");	/* string for output of make */
# endif
#else
EXTERN char_u *p_sp	INIT(= (char_u *)">");		/* string for output of make */
#endif
EXTERN long	p_ss	INIT(= 0);			/* sideways scrolling offset */
EXTERN long	p_st	INIT(= 0);			/* type of shell */
EXTERN int	p_sr	INIT(= FALSE);		/* shift round off (for < and >) */
EXTERN int	p_sb	INIT(= FALSE);		/* split window backwards */
#if defined(COMPATIBLE) || defined(UNIX)
EXTERN int	p_sc	INIT(= FALSE);		/* show command in status line */
#else
EXTERN int	p_sc	INIT(= TRUE);		/* show command in status line */
#endif
EXTERN int	p_sm	INIT(= FALSE);		/* showmatch */
#if defined(COMPATIBLE)
EXTERN int	p_smd	INIT(= FALSE);		/* show mode */
#else
EXTERN int	p_smd	INIT(= TRUE);		/* show mode */
#endif
EXTERN int	p_sta	INIT(= FALSE);		/* smart-tab for expand-tab */
EXTERN char_u *p_su	INIT(= (char_u *)".bak.o.h.info.swp");	/* suffixes for wildcard expansion */
EXTERN long p_tl	INIT(= 0);			/* used tag length */
EXTERN char_u *p_tags	INIT(= (char_u *)"tags");		/* tags search path */
#if defined(COMPATIBLE)
EXTERN int	p_ta	INIT(= FALSE);		/* auto textmode detection */
#else
EXTERN int	p_ta	INIT(= TRUE);		/* auto textmode detection */
#endif
EXTERN int	p_terse	INIT(= FALSE);		/* terse messages */
EXTERN int	p_tf	INIT(= FALSE);		/* terminal fast I/O */
EXTERN int	p_to	INIT(= FALSE);		/* tilde is an operator */
#if defined(COMPATIBLE)
EXTERN int	p_tr	INIT(= FALSE);		/* tag file name is relative */
#else
EXTERN int	p_tr	INIT(= TRUE);		/* tag file name is relative */
#endif
EXTERN int	p_timeout	INIT(= TRUE);	/* mappings entered within one second */
EXTERN long p_tm	INIT(= 1000);		/* timeoutlen (msec) */
#ifdef NOTITLE
EXTERN int	p_title	INIT(= FALSE);		/* set window title if possible */
#else
EXTERN int	p_title	INIT(= TRUE);		/* set window title if possible */
#endif
EXTERN int	p_ttimeout	INIT(= FALSE);	/* key codes entered within one second */
#ifdef COMPATIBLE
EXTERN long p_ul	INIT(= 0);			/* number of Undo Levels */
EXTERN long p_uc	INIT(= 0);			/* update count for swap file */
#else
EXTERN long p_ul	INIT(= 100);		/* number of Undo Levels */
EXTERN long p_uc	INIT(= 200);		/* update count for swap file */
#endif
EXTERN long p_ut	INIT(= 4000);		/* update time for swap file */
EXTERN int	p_vb	INIT(= FALSE);		/* visual bell only (no beep) */
EXTERN int	p_warn	INIT(= TRUE);		/* warn for changes at shell command */
EXTERN int	p_ws	INIT(= TRUE);		/* wrap scan */
#ifdef COMPATIBLE
EXTERN long	p_ww	INIT(= 0);			/* which keys wrap to next/prev line */
#else
EXTERN long	p_ww	INIT(= 3);			/* which keys wrap to next/prev line */
#endif
#ifdef COMPATIBLE
EXTERN long	p_wc	INIT(= Ctrl('E'));	/* character for wildcard exapansion */
#else
EXTERN long	p_wc	INIT(= TAB);		/* character for wildcard exapansion */
#endif
EXTERN long	p_wh	INIT(= 0);			/* desired window height */
EXTERN int	p_wa	INIT(= FALSE);		/* write any */
#if defined(COMPATIBLE) || defined(NOBACKUP)
EXTERN int	p_wb	INIT(= FALSE);		/* write backup files */
#else
EXTERN int	p_wb	INIT(= TRUE);		/* write backup files */
#endif
EXTERN int	p_ye	INIT(= FALSE);		/* Y yanks to end of line */
