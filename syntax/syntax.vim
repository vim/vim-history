" Vim syntax support file
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	1998 Feb 24

" This is the startup file for syntax highlighting.
" 1. Set the default highlight groups.
" 2. Install autocommands for all the available syntax files.
" 3. Install the Syntax menu.

if has("syntax")

  " Make sure the '<' flag is not included in 'cpoptions', otherwise <CR> would
  " not be recognized.  See ":help 'cpoptions'".
  let syntax_cpo_save = &cpo
  let &cpo = ""

  " let others know that syntax has been switched on
  let syntax_on = 1

  " The default methods for highlighting.  Can be overridden later.
  " Many terminals can only use six different colors (plus black and white).
  " Therefore the number of colors used is kept low. It doesn't look nice with
  " too many colors anyway.
  " Careful with "cterm=bold", it may change the color to bright.

  " There are two sets of defaults: for a dark and a light background.
  if &background == "dark"
    hi Comment	term=bold ctermfg=Cyan guifg=#80a0ff
    hi Constant	term=underline ctermfg=Magenta guifg=#ffa0a0
    hi Special	term=bold ctermfg=LightRed guifg=Orange
    hi Identifier term=underline cterm=bold ctermfg=Cyan guifg=#40ffff
    hi Statement term=bold ctermfg=Yellow guifg=#ffff60 gui=bold
    hi PreProc	term=underline ctermfg=LightBlue guifg=#ff80ff
    hi Type	term=underline ctermfg=LightGreen guifg=#60ff60 gui=bold
    hi Ignore	ctermfg=black guifg=bg
  else
    hi Comment	term=bold ctermfg=DarkBlue guifg=Blue
    hi Constant	term=underline ctermfg=DarkRed guifg=Magenta
    hi Special	term=bold ctermfg=DarkMagenta guifg=SlateBlue
    hi Identifier term=underline ctermfg=DarkCyan guifg=DarkCyan
    hi Statement term=bold ctermfg=Brown gui=bold guifg=Brown
    hi PreProc	term=underline ctermfg=DarkMagenta guifg=Purple
    hi Type	term=underline ctermfg=DarkGreen guifg=SeaGreen gui=bold
    hi Ignore	ctermfg=white guifg=bg
  endif
  hi Error	term=reverse ctermbg=Red ctermfg=White guibg=Red guifg=White
  hi Todo	term=standout ctermbg=Yellow ctermfg=Black guifg=Blue guibg=Yellow

  " Common groups that link to default highlighting.
  " You can specify other highlighting easily.
  hi link String	Constant
  hi link Character	Constant
  hi link Number	Constant
  hi link Boolean	Constant
  hi link Float		Number
  hi link Function	Identifier
  hi link Conditional	Statement
  hi link Repeat	Statement
  hi link Label		Statement
  hi link Operator	Statement
  hi link Keyword	Statement
  hi link Exception	Statement
  hi link Include	PreProc
  hi link Define	PreProc
  hi link Macro		PreProc
  hi link PreCondit	PreProc
  hi link StorageClass	Type
  hi link Structure	Type
  hi link Typedef	Type
  hi link Tag		Special
  hi link SpecialChar	Special
  hi link Delimiter	Special
  hi link SpecialComment Special
  hi link Debug		Special


  augroup syntax

  " First remove all old syntax autocommands.
  au!


  " Ada (83, 9X, 95)
  au BufNewFile,BufRead *.adb,*.ads		so $VIM/syntax/ada.vim

  " Assembly (all kinds)
  au BufNewFile,BufRead *.asm,*.s,*.i,*.mac	so $VIM/syntax/asmselect.vim

  " ASN.1
  au BufNewFile,BufRead *.asn,*.asn1		so $VIM/syntax/asn.vim

  " Atlas
  au BufNewFile,BufRead *.atl,*.as		so $VIM/syntax/atlas.vim

  " Awk
  au BufNewFile,BufRead *.awk			so $VIM/syntax/awk.vim

  " BASIC
  au BufNewFile,BufRead *.bas			so $VIM/syntax/basic.vim

  " Batch file for MSDOS
  au BufNewFile,BufRead *.bat,*.sys		so $VIM/syntax/dosbatch.vim

  " BibTeX bibliography database file
  au BufNewFile,BufRead *.bib			so $VIM/syntax/bib.vim

  " C
  au BufNewFile,BufRead *.c			so $VIM/syntax/c.vim

  " C++
  if has("fname_case")
    au BufNewFile,BufRead *.cpp,*.cc,*.cxx,*.c++,*.C,*.h,*.H,*.hh,*.hxx,*.hpp,*.inl so $VIM/syntax/cpp.vim
  else
    au BufNewFile,BufRead *.cpp,*.cc,*.cxx,*.c++,*.h,*.hh,*.hxx,*.hpp,*.inl so $VIM/syntax/cpp.vim
  endif

  " Cascading Style Sheets
  au BufNewFile,BufRead *.css			so $VIM/syntax/css.vim

  " Century Term Command Scripts
  au BufNewFile,BufRead *.cmd,*.con		so $VIM/syntax/cterm.vim

  " Clean
  au BufNewFile,BufReadPost *.dcl,*.icl		so $VIM/syntax/clean.vim

  " Clipper
  au BufNewFile,BufRead *.prg			so $VIM/syntax/clipper.vim

  " Cobol
  au BufNewFile,BufRead *.cbl,*.cob,*.cpy,*.lib	so $VIM/syntax/cobol.vim

  " Diff files
  au BufNewFile,BufRead *.diff,*.rej		so $VIM/syntax/diff.vim

  " DCL (Digital Command Language - vms)
  au BufNewFile,BufRead *.com			so $VIM/syntax/dcl.vim

  " Dracula
  au BufNewFile,BufRead drac.*,*.drac,*.drc,*lvs,*lpe so $VIM/syntax/dracula.vim

  " Eiffel
  au BufNewFile,BufRead *.e,*.E			so $VIM/syntax/eiffel.vim

  " Elm Filter Rules file
  au BufNewFile,BufReadPost filter-rules	so $VIM/syntax/elmfilt.vim

  " ESQL-C
  au BufNewFile,BufRead *.ec,*.EC		so $VIM/syntax/esqlc.vim

  " Exports
  au BufNewFile,BufRead exports			so $VIM/syntax/exports.vim

  " Fortran
  au BufNewFile,BufRead *.f,*.F,*.for,*.fpp	so $VIM/syntax/fortran.vim

  " Fvwm
  au BufNewFile,BufRead *fvwm*rc*		so $VIM/syntax/fvwm.vim

  " GP scripts (2.0 and onward)
  au BufNewFile,BufRead *.gp			so $VIM/syntax/gp.vim

  " HTML (.shtml for server side)
  au BufNewFile,BufRead *.html,*.htm,*.shtml	so $VIM/syntax/html.vim

  " IDL (Interface Description Language)
  au BufNewFile,BufRead *.idl			so $VIM/syntax/idl.vim

  " Inform
  au BufNewFile,BufRead *.inf,*.INF		so $VIM/syntax/inform.vim

  " .INI file for MSDOS
  au BufNewFile,BufRead *.ini			so $VIM/syntax/dosini.vim

  " Java
  au BufNewFile,BufRead *.java,*.jav		so $VIM/syntax/java.vim

  " JavaCC
  au BufNewFile,BufRead *.jj,*.jjt		so $VIM/syntax/javacc.vim

  " JavaScript
  au BufNewFile,BufRead *.js,*.javascript	so $VIM/syntax/javascript.vim

  " Jgraph
  au BufNewFile,BufRead *.jgr			so $VIM/syntax/jgraph.vim

  " Lace (ISE)
  au BufNewFile,BufRead *.ace,*.ACE		so $VIM/syntax/lace.vim

  " Lex
  au BufNewFile,BufRead *.lex,*.l		so $VIM/syntax/lex.vim

  " Lisp
  if has("fname_case")
    au BufNewFile,BufRead *.lsp,*.L		so $VIM/syntax/lisp.vim
  else
    au BufNewFile,BufRead *.lsp			so $VIM/syntax/lisp.vim
  endif

  " Lite
  au BufNewFile,BufRead *.lite,*.lt		so $VIM/syntax/lite.vim

  " LOTOS
  au BufNewFile,BufRead *.lot,*.lotos		so $VIM/syntax/lotos.vim

  " Lynx style file
  au BufNewFile,BufRead *.lss			so $VIM/syntax/lss.vim

  " Mail (for Elm, trn and rn)
  au BufNewFile,BufRead snd.*,.letter,.followup,.article,.article.[0-9]\+,pico.[0-9]\+,mutt*[0-9] so $VIM/syntax/mail.vim

  " Makefile
  au BufNewFile,BufRead [mM]akefile*,GNUmakefile,*.mk,*.mak so $VIM/syntax/make.vim

  " Manpage
  au BufNewFile,BufRead *.man			so $VIM/syntax/man.vim

  " Maple V
  au BufNewFile,BufRead *.mv,*.mpl,*.mws	so $VIM/syntax/maple.vim

  " Matlab
  au BufNewFile,BufRead *.m			so $VIM/syntax/matlab.vim

  " Metafont
  au BufNewFile,BufRead *.mf			so $VIM/syntax/mf.vim

  " MetaPost
  au BufNewFile,BufRead *.mp			so $VIM/syntax/mp.vim

  " Modula 2
  au BufNewFile,BufRead *.m2			so $VIM/syntax/modula2.vim

  " Msql
  au BufNewFile,BufRead *.msql			so $VIM/syntax/msql.vim

  " M$ Resource files
  au BufNewFile,BufRead *.rc			so $VIM/syntax/rc.vim

  " Mutt setup file
  au BufNewFile,BufRead .muttrc			so $VIM/syntax/muttrc.vim

  " Nroff/Troff
  au BufNewFile,BufRead *.me,*.mm,*.tr,*.nr,*.[1-7] so $VIM/syntax/nroff.vim

  " Pascal
  au BufNewFile,BufRead *.p,*.pas		so $VIM/syntax/pascal.vim

  " Perl
  au BufNewFile,BufRead *.pl,*.pm		so $VIM/syntax/perl.vim

  " Perl POD
  au BufNewFile,BufRead *.pod			so $VIM/syntax/pod.vim

  " Php3
  au BufNewFile,BufRead *.php3			so $VIM/syntax/php3.vim

  " Phtml
  au BufNewFile,BufRead *.phtml			so $VIM/syntax/phtml.vim

  " Pike
  au BufNewFile,BufRead *.pike,*.lpc,*.ulpc,*.pmod so $VIM/syntax/pike.vim

  " PO (GNU gettext)
  au BufNewFile,BufRead *.po			so $VIM/syntax/po.vim

  " PostScript
  au BufNewFile,BufRead *.ps,*.eps		so $VIM/syntax/postscr.vim

  " Povray
  au BufNewFile,BufRead *.pov,*.inc		so $VIM/syntax/pov.vim

  " Printcap
  au BufNewFile,BufRead printcap		so $VIM/syntax/pcap.vim

  " Procmail
  au BufNewFile,BufRead .procmail,.procmailrc	so $VIM/syntax/procmail.vim

  " Prolog
  au BufNewFile,BufRead *.pdb			so $VIM/syntax/prolog.vim

  " Python
  au BufNewFile,BufRead *.py			so $VIM/syntax/python.vim

  " Rexx
  au BufNewFile,BufRead *.rexx,*.rex,*.r	so $VIM/syntax/rexx.vim

  " S-lang
  au BufNewFile,BufRead *.sl			so $VIM/syntax/slang.vim

  " Sather
  au BufNewFile,BufRead *.sa			so $VIM/syntax/sather.vim

  " SDL
  au BufNewFile,BufRead *.sdl,*.pr		so $VIM/syntax/sdl.vim

  " Sendmail
  au BufNewFile,BufRead sendmail.cf		so $VIM/syntax/sm.vim

  " SGML
  au BufNewFile,BufRead *.sgm,*.sgml		so $VIM/syntax/sgml.vim

  " Shell scripts (sh, ksh, bash, csh)
  au BufNewFile,BufRead .profile,.bashrc,.kshrc,*.sh,*.ksh,*.bash,*.env so $VIM/syntax/sh.vim
  au BufNewFile,BufRead .login,.cshrc,.tcshrc,*.csh,*.tcsh so $VIM/syntax/csh.vim

  " Z-Shell script
  au BufNewFile,BufRead .z*,zsh*,zlog*		so $VIM/syntax/zsh.vim

  " Scheme
  au BufNewFile,BufRead *.scm			so $VIM/syntax/scheme.vim

  " Simula
  au BufNewFile,BufRead *.sim			so $VIM/syntax/simula.vim

  " Skill
  au BufNewFile,BufRead *.il,*.rul		so $VIM/syntax/skill.vim

  " SLRN
  au BufNewFile,BufRead .slrnrc			so $VIM/syntax/slrnrc.vim
  au BufNewFile,BufRead *.score			so $VIM/syntax/slrnsc.vim

  " Smalltalk
  au BufNewFile,BufRead *.st,*.cls		so $VIM/syntax/st.vim

  " SNMP MIB files
  au BufNewFile,BufReadPost *.mib,*.smi		so $VIM/syntax/mib.vim

  " Spec (Linux RPM)
  au BufNewFile,BufRead *.spec			so $VIM/syntax/spec.vim

  " SQL
  au BufNewFile,BufRead *.sql			so $VIM/syntax/sql.vim

  " Tags
  au BufNewFile,BufRead tags			so $VIM/syntax/tags.vim

  " Tcl
  au BufNewFile,BufRead *.tcl			so $VIM/syntax/tcl.vim

  " Telix Salt
  au BufNewFile,BufRead *.slt			so $VIM/syntax/tsalt.vim

  " TeX
  au BufNewFile,BufRead *.tex,*.sty		so $VIM/syntax/tex.vim

  " TF mud client
  au BufNewFile,BufRead *.tf			so $VIM/syntax/tf.vim

  " Motif UIT/UIL files
  au BufNewFile,BufRead *.uit,*.uil		so $VIM/syntax/uil.vim

  " Verilog HDL
  au BufNewFile,BufRead *.v			so $VIM/syntax/verilog.vim

  " VHDL
  au BufNewFile,BufRead *.hdl,*.vhd,*.vhdl,*.vhdl_[0-9]*,*.vbe,*.vst  so $VIM/syntax/vhdl.vim

  " Vim Help file
  if has("mac")
    au BufNewFile,BufRead *[/:]vim*[/:]doc[/:]*.txt	so $VIM/syntax/help.vim
  else
    au BufNewFile,BufRead */vim*/doc/*.txt	so $VIM/syntax/help.vim
  endif

  " Vim script
  au BufNewFile,BufRead *vimrc*,*.vim,.exrc,_exrc so $VIM/syntax/vim.vim

  " Viminfo file
  au BufNewFile,BufRead .viminfo,_viminfo	so $VIM/syntax/viminfo.vim

  " Vgrindefs file
  au BufNewFile,BufRead vgrindefs		so $VIM/syntax/vgrindefs.vim

  " VRML V1.0c
  au BufNewFile,BufRead *.wrl			so $VIM/syntax/vrml.vim

  " X Pixmap (dynamically sets colors, use BufEnter to make it work better)
  au BufNewFile,BufRead,BufEnter *.xpm		so $VIM/syntax/xpm.vim

  " X resources file
  au BufNewFile,BufRead *.Xdefaults,*/app-defaults/* so $VIM/syntax/xdefaults.vim

  " Xmath
  au BufNewFile,BufRead *.ms,*.msc,*.msf	so $VIM/syntax/xmath.vim

  " XML
  au BufNewFile,BufRead *.xml			so $VIM/syntax/xml.vim

  " Yacc
  au BufNewFile,BufRead *.y			so $VIM/syntax/yacc.vim

  " Various scripts, without a specific extension
  au BufNewFile,BufRead,StdinReadPost *		so $VIM/syntax/scripts.vim

  augroup END

  if has("gui")
    amenu 50.10.100 &Syntax.&ABCD.Ada			:so $VIM/syntax/ada.vim<CR>
    amenu 50.10.110 &Syntax.&ABCD.Amiga\ DOS		:so $VIM/syntax/amiga.vim<CR>
    amenu 50.10.120 &Syntax.&ABCD.assembly		:so $VIM/syntax/asm.vim<CR>
    amenu 50.10.130 &Syntax.&ABCD.ASN\.1		:so $VIM/syntax/asn.vim<CR>
    amenu 50.10.140 &Syntax.&ABCD.Atlas			:so $VIM/syntax/atlas.vim<CR>
    amenu 50.10.150 &Syntax.&ABCD.Awk			:so $VIM/syntax/awk.vim<CR>
    amenu 50.10.160 &Syntax.&ABCD.BASIC			:so $VIM/syntax/basic.vim<CR>
    amenu 50.10.170 &Syntax.&ABCD.BibFile		:so $VIM/syntax/bib.vim<CR>
    amenu 50.10.180 &Syntax.&ABCD.C			:so $VIM/syntax/c.vim<CR>
    amenu 50.10.190 &Syntax.&ABCD.C++			:so $VIM/syntax/cpp.vim<CR>
    amenu 50.10.200 &Syntax.&ABCD.Cascading\ Style\ Sheets :so $VIM/syntax/css.vim<CR>
    amenu 50.10.210 &Syntax.&ABCD.Century\ Term		:so $VIM/syntax/cterm.vim<CR>
    amenu 50.10.220 &Syntax.&ABCD.Clean			:so $VIM/syntax/clean.vim<CR>
    amenu 50.10.230 &Syntax.&ABCD.Clipper		:so $VIM/syntax/clipper.vim<CR>
    amenu 50.10.240 &Syntax.&ABCD.Csh\ shell\ script	:so $VIM/syntax/csh.vim<CR>
    amenu 50.10.250 &Syntax.&ABCD.Cobol			:so $VIM/syntax/cobol.vim<CR>
    amenu 50.10.260 &Syntax.&ABCD.Diff			:so $VIM/syntax/diff.vim<CR>
    amenu 50.10.270 &Syntax.&ABCD.Dracula		:so $VIM/syntax/dracula.vim<CR>
    amenu 50.20.100 &Syntax.&EFGHIJ.Eiffel		:so $VIM/syntax/eiffel.vim<CR>
    amenu 50.20.110 &Syntax.&EFGHIJ.Elm\ Filter		:so $VIM/syntax/elmfilt.vim<CR>
    amenu 50.20.120 &Syntax.&EFGHIJ.ESQL-C		:so $VIM/syntax/esqlc.vim<CR>
    amenu 50.20.130 &Syntax.&EFGHIJ.Expect		:so $VIM/syntax/expect.vim<CR>
    amenu 50.20.140 &Syntax.&EFGHIJ.Exports		:so $VIM/syntax/exports.vim<CR>
    amenu 50.20.150 &Syntax.&EFGHIJ.Fortran		:so $VIM/syntax/fortran.vim<CR>
    amenu 50.20.160 &Syntax.&EFGHIJ.Fvwm\ configuration	:so $VIM/syntax/fvwm.vim<CR>
    amenu 50.20.170 &Syntax.&EFGHIJ.GP			:so $VIM/syntax/gp.vim<CR>
    amenu 50.20.180 &Syntax.&EFGHIJ.HTML		:so $VIM/syntax/html.vim<CR>
    amenu 50.20.190 &Syntax.&EFGHIJ.IDL			:so $VIM/syntax/idl.vim<CR>
    amenu 50.20.200 &Syntax.&EFGHIJ.Inform		:so $VIM/syntax/inform.vim<CR>
    amenu 50.20.210 &Syntax.&EFGHIJ.Java		:so $VIM/syntax/java.vim<CR>
    amenu 50.20.220 &Syntax.&EFGHIJ.JavaCC		:so $VIM/syntax/javacc.vim<CR>
    amenu 50.20.230 &Syntax.&EFGHIJ.JavaScript		:so $VIM/syntax/javascript.vim<CR>
    amenu 50.20.240 &Syntax.&EFGHIJ.Jgraph		:so $VIM/syntax/jgraph.vim<CR>
    amenu 50.30.100 &Syntax.&KLM.Lace			:so $VIM/syntax/lace.vim<CR>
    amenu 50.30.110 &Syntax.&KLM.Lex			:so $VIM/syntax/lex.vim<CR>
    amenu 50.30.120 &Syntax.&KLM.Lilo			:so $VIM/syntax/lilo.vim<CR>
    amenu 50.30.130 &Syntax.&KLM.Lisp			:so $VIM/syntax/lisp.vim<CR>
    amenu 50.30.140 &Syntax.&KLM.Lite			:so $VIM/syntax/lite.vim<CR>
    amenu 50.30.150 &Syntax.&KLM.LOTOS			:so $VIM/syntax/lotos.vim<CR>
    amenu 50.30.160 &Syntax.&KLM.Lynx\ Style		:so $VIM/syntax/lss.vim<CR>
    amenu 50.30.170 &Syntax.&KLM.Mail			:so $VIM/syntax/mail.vim<CR>
    amenu 50.30.180 &Syntax.&KLM.Makefile		:so $VIM/syntax/make.vim<CR>
    amenu 50.30.190 &Syntax.&KLM.Man\ page		:so $VIM/syntax/man.vim<CR>
    amenu 50.30.200 &Syntax.&KLM.Maple			:so $VIM/syntax/maple.vim<CR>
    amenu 50.30.210 &Syntax.&KLM.Matlab			:so $VIM/syntax/matlab.vim<CR>
    amenu 50.30.220 &Syntax.&KLM.Metafont		:so $VIM/syntax/mf.vim<CR>
    amenu 50.30.230 &Syntax.&KLM.MetaPost		:so $VIM/syntax/mp.vim<CR>
    amenu 50.30.240 &Syntax.&KLM.Model			:so $VIM/syntax/model.vim<CR>
    amenu 50.30.250 &Syntax.&KLM.Modula\ 2		:so $VIM/syntax/modula2.vim<CR>
    amenu 50.30.260 &Syntax.&KLM.Msql			:so $VIM/syntax/msql.vim<CR>
    amenu 50.30.270 &Syntax.&KLM.MS-DOS\ \.bat\ file	:so $VIM/syntax/dosbatch.vim<CR>
    amenu 50.30.280 &Syntax.&KLM.MS-DOS\ \.ini\ file	:so $VIM/syntax/dosini.vim<CR>
    amenu 50.30.290 &Syntax.&KLM.MS\ Resource\ file	:so $VIM/syntax/rc.vim<CR>
    amenu 50.30.300 &Syntax.&KLM.Muttrc			:so $VIM/syntax/muttrc.vim<CR>
    amenu 50.40.100 &Syntax.&NOPQ.NASM			:so $VIM/syntax/nasm.vim<CR>
    amenu 50.40.110 &Syntax.&NOPQ.Nroff			:so $VIM/syntax/nroff.vim<CR>
    amenu 50.40.120 &Syntax.&NOPQ.Objective\ C		:so $VIM/syntax/objc.vim<CR>
    amenu 50.40.130 &Syntax.&NOPQ.Pascal		:so $VIM/syntax/pascal.vim<CR>
    amenu 50.40.140 &Syntax.&NOPQ.Perl			:so $VIM/syntax/perl.vim<CR>
    amenu 50.40.150 &Syntax.&NOPQ.Perl\ POD		:so $VIM/syntax/pod.vim<CR>
    amenu 50.40.160 &Syntax.&NOPQ.PHP\ 3		:so $VIM/syntax/php3.vim<CR>
    amenu 50.40.170 &Syntax.&NOPQ.Phtml			:so $VIM/syntax/phtml.vim<CR>
    amenu 50.40.180 &Syntax.&NOPQ.Pike			:so $VIM/syntax/pike.vim<CR>
    amenu 50.40.190 &Syntax.&NOPQ.PO\ (GNU\ gettext)	:so $VIM/syntax/po.vim<CR>
    amenu 50.40.200 &Syntax.&NOPQ.PostScript		:so $VIM/syntax/postscr.vim<CR>
    amenu 50.40.210 &Syntax.&NOPQ.Povray		:so $VIM/syntax/pov.vim<CR>
    amenu 50.40.220 &Syntax.&NOPQ.Printcap		:so $VIM/syntax/pcap.vim<CR>
    amenu 50.40.230 &Syntax.&NOPQ.Procmail		:so $VIM/syntax/procmail.vim<CR>
    amenu 50.40.240 &Syntax.&NOPQ.Prolog		:so $VIM/syntax/prolog.vim<CR>
    amenu 50.40.250 &Syntax.&NOPQ.Purify\ log		:so $VIM/syntax/purifylog.vim<CR>
    amenu 50.40.260 &Syntax.&NOPQ.Python		:so $VIM/syntax/python.vim<CR>
    amenu 50.50.100 &Syntax.&RS.Rexx			:so $VIM/syntax/rexx.vim<CR>
    amenu 50.50.110 &Syntax.&RS.S-lang			:so $VIM/syntax/slang.vim<CR>
    amenu 50.50.120 &Syntax.&RS.Sather			:so $VIM/syntax/sather.vim<CR>
    amenu 50.50.130 &Syntax.&RS.Scheme			:so $VIM/syntax/scheme.vim<CR>
    amenu 50.50.140 &Syntax.&RS.SDL			:so $VIM/syntax/sdl.vim<CR>
    amenu 50.50.150 &Syntax.&RS.Sendmail\.cf		:so $VIM/syntax/sm.vim<CR>
    amenu 50.50.160 &Syntax.&RS.SGML			:so $VIM/syntax/sgml.vim<CR>
    amenu 50.50.170 &Syntax.&RS.Sh\ shell\ script	:so $VIM/syntax/sh.vim<CR>
    amenu 50.50.180 &Syntax.&RS.SiCAD			:so $VIM/syntax/sicad.vim<CR>
    amenu 50.50.190 &Syntax.&RS.Simula			:so $VIM/syntax/simula.vim<CR>
    amenu 50.50.200 &Syntax.&RS.Skill			:so $VIM/syntax/skill.vim<CR>
    amenu 50.50.210 &Syntax.&RS.SLRN\ rc		:so $VIM/syntax/slrnrc.vim<CR>
    amenu 50.50.220 &Syntax.&RS.SLRN\ score		:so $VIM/syntax/slrnsc.vim<CR>
    amenu 50.50.230 &Syntax.&RS.SmallTalk		:so $VIM/syntax/st.vim<CR>
    amenu 50.50.240 &Syntax.&RS.SNMP\ MIB		:so $VIM/syntax/mib.vim<CR>
    amenu 50.50.250 &Syntax.&RS.SPEC\ (Linux\ RPM)	:so $VIM/syntax/spec.vim<CR>
    amenu 50.50.260 &Syntax.&RS.SQL			:so $VIM/syntax/sql.vim<CR>
    amenu 50.60.100 &Syntax.&TU.Tags			:so $VIM/syntax/tags.vim<CR>
    amenu 50.60.110 &Syntax.&TU.Tcl			:so $VIM/syntax/tcl.vim<CR>
    amenu 50.60.120 &Syntax.&TU.Telix\ Salt		:so $VIM/syntax/tsalt.vim<CR>
    amenu 50.60.130 &Syntax.&TU.Tex			:so $VIM/syntax/tex.vim<CR>
    amenu 50.60.140 &Syntax.&TU.TF\ mud\ client		:so $VIM/syntax/tf.vim<CR>
    amenu 50.60.150 &Syntax.&TU.UIT/UIL			:so $VIM/syntax/uil.vim<CR>
    amenu 50.70.110 &Syntax.&VWXYZ.Verilog\ HDL		:so $VIM/syntax/verilog.vim<CR>
    amenu 50.70.120 &Syntax.&VWXYZ.vgrindefs		:so $VIM/syntax/vgrindefs.vim<CR>
    amenu 50.70.130 &Syntax.&VWXYZ.VHDL			:so $VIM/syntax/vhdl.vim<CR>
    amenu 50.70.140 &Syntax.&VWXYZ.Vim\ help\ file	:so $VIM/syntax/help.vim<CR>
    amenu 50.70.150 &Syntax.&VWXYZ.Vim\ script		:so $VIM/syntax/vim.vim<CR>
    amenu 50.70.160 &Syntax.&VWXYZ.Viminfo\ file	:so $VIM/syntax/viminfo.vim<CR>
    amenu 50.70.170 &Syntax.&VWXYZ.VRML			:so $VIM/syntax/vrml.vim<CR>
    amenu 50.70.180 &Syntax.&VWXYZ.whitespace		:so $VIM/syntax/whitespace.vim<CR>
    amenu 50.70.190 &Syntax.&VWXYZ.X\ Pixmap		:so $VIM/syntax/xpm.vim<CR>
    amenu 50.70.200 &Syntax.&VWXYZ.X\ resources		:so $VIM/syntax/xdefaults.vim<CR>
    amenu 50.70.210 &Syntax.&VWXYZ.Xmath		:so $VIM/syntax/xmath.vim<CR>
    amenu 50.70.220 &Syntax.&VWXYZ.XML			:so $VIM/syntax/xml.vim<CR>
    amenu 50.70.230 &Syntax.&VWXYZ.Yacc			:so $VIM/syntax/yacc.vim<CR>
    amenu 50.70.240 &Syntax.&VWXYZ.Zsh\ shell\ script	:so $VIM/syntax/zsh.vim<CR>
    amenu 50.80 &Syntax.&on\ (this\ file)	:normal :if exists("syntax_on")<Bar>doau syntax BufRead<Bar>else<Bar>syn on<Bar>doau syntax BufRead<Bar>au! syntax<C-V><CR>:unlet syntax_on<Bar>aunmenu &Syntax.&manual<Bar>endif<C-V><CR><CR>
    amenu 50.82 &Syntax.o&ff\ (this\ file)	:syn clear<CR>
    amenu 50.84 &Syntax.&manual			:normal :au! syntax<C-V><CR>:unlet syntax_on<Bar>aunmenu &Syntax.&manual<Bar>amenu 50.30 &Syntax.auto&matic :aunmenu &Syntax.auto&matic<C-V><C-V><C-V><C-V><Bar>so $VIM/syntax/syntax.vim<C-V><C-V><C-V><CR><C-V><CR><CR>
    amenu 50.710 &Syntax.color\ &test		:sp $VIM/syntax/colortest.vim<Bar>so %<CR>
    amenu 50.720 &Syntax.&highlight\ test	:new<Bar>so $VIM/syntax/hitest.vim<CR>
    amenu 50.730 &Syntax.&convert\ to\ HTML	:so $VIM/syntax/2html.vim<CR>
  endif


" Source the user-specified syntax highlighting file
  if exists("mysyntaxfile")
    if file_readable(expand(mysyntaxfile))
      execute "source " . mysyntaxfile
    endif
  endif


  " Execute the syntax autocommands for the each buffer.
  doautoall syntax BufRead

  " Restore the previous value of 'cpoptions'.
  let &cpo = syntax_cpo_save
  unlet syntax_cpo_save

endif " has("syntax")

" vim: ts=8
