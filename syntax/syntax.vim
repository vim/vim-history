" Vim syntax support file
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	1998 Feb 12

" This is the startup file for syntax highlighting.
" 1. Set the default highlight groups.
" 2. Install autocommands for all the available syntax files.
" 3. Install the Syntax menu.

if has("syntax")

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
    hi Identifier term=underline ctermfg=DarkCyan guifg=#40ffff
    hi Statement term=bold ctermfg=Yellow guifg=#ffff60 gui=bold
    hi PreProc	term=underline ctermfg=LightBlue guifg=#ff80ff
    hi Type	term=underline ctermfg=LightGreen guifg=#60ff60 gui=bold
  else
    hi Comment	term=bold ctermfg=Blue guifg=Blue
    hi Constant	term=underline ctermfg=Magenta guifg=Magenta
    hi Special	term=bold ctermfg=DarkBlue guifg=SlateBlue
    hi Identifier term=underline ctermfg=DarkCyan guifg=DarkCyan
    hi Statement term=bold ctermfg=Brown gui=bold guifg=Brown
    hi PreProc	term=underline ctermfg=Magenta guifg=Purple
    hi Type	term=underline ctermfg=DarkGreen guifg=SeaGreen gui=bold
  endif

  " These two change the background
  hi Error	term=reverse ctermbg=Red guibg=Orange
  hi Todo	term=standout ctermbg=Yellow guifg=Blue guibg=Yellow

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

  " Assembly (GNU)
  au BufNewFile,BufRead *.asm,*.s		so $VIM/syntax/asm.vim

  " ASN.1
  au BufNewFile,BufRead *.asn,*.asn1		so $VIM/syntax/asn.vim

  " Atlas
  au BufNewFile,BufRead *.atl,*.as		so $VIM/syntax/atlas.vim

  " Awk
  au BufNewFile,BufRead *.awk			so $VIM/syntax/awk.vim

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

  " Century Term Command Scripts
  au BufNewFile,BufRead *.cmd,*.con		so $VIM/syntax/cterm.vim

  " Clean
  au BufNewFile,BufReadPost *.dcl,*.icl		so $VIM/syntax/clean.vim

  " Cobol
  au BufNewFile,BufRead *.cbl,*.cob,*.cpy,*.lib	so $VIM/syntax/cobol.vim

  " Diff files
  au BufNewFile,BufRead *.diff,*.rej		so $VIM/syntax/diff.vim

  " Dracula
  au BufNewFile,BufRead drac.*,*.drac,*.drc,*lvs,*lpe so $VIM/syntax/dracula.vim

  " Eiffel
  au BufNewFile,BufReadPost *.e			so $VIM/syntax/eiffel.vim

  " Elm Filter Rules file
  au BufNewFile,BufReadPost filter-rules	so $VIM/syntax/elmfilt.vim

  " ESQL-C
  au BufNewFile,BufRead *.ec,*.EC		so $VIM/syntax/esqlc.vim

  " Fortran
  au BufNewFile,BufRead *.f,*.F,*.for,*.fpp	so $VIM/syntax/fortran.vim

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
  au BufNewFile,BufRead *.jj			so $VIM/syntax/javacc.vim

  " JavaScript
  au BufNewFile,BufRead *.js,*.javascript	so $VIM/syntax/javascript.vim

  " Lex
  au BufNewFile,BufRead *.lex,*.l		so $VIM/syntax/lex.vim

  " Lisp
  au BufNewFile,BufRead *.lsp,*.L		so $VIM/syntax/lisp.vim

  " Lynx style file
  au BufNewFile,BufRead *.lss			so $VIM/syntax/lss.vim

  " Mail (for Elm, trn and rn)
  au BufNewFile,BufRead snd.*,.letter,.followup,.article,.article.[0-9]\+,pico.[0-9]\+,mutt*[0-9] so $VIM/syntax/mail.vim

  " Makefile
  au BufNewFile,BufRead [mM]akefile*,*.mk,*.mak so $VIM/syntax/make.vim

  " Maple V
  au BufNewFile,BufRead *.mv			so $VIM/syntax/maple.vim

  " Matlab
  au BufNewFile,BufRead *.m			so $VIM/syntax/matlab.vim

  " Modula 2
  au BufNewFile,BufRead *.m2			so $VIM/syntax/modula2.vim

  " M$ Resource files
  au BufNewFile,BufRead *.rc			so $VIM/syntax/rc.vim

  " Mutt setup file
  au BufNewFile,BufRead .muttrc			so $VIM/syntax/muttrc.vim

  " Pascal
  au BufNewFile,BufRead *.p,*.pas		so $VIM/syntax/pascal.vim

  " Perl
  au BufNewFile,BufRead *.pl,*.pm		so $VIM/syntax/perl.vim

  " Perl POD
  au BufNewFile,BufRead *.pod			so $VIM/syntax/pod.vim

  " Pike
  au BufNewFile,BufRead *.pike,*.lpc,*.ulpc,*.pmod so $VIM/syntax/pike.vim

  " PostScript
  au BufNewFile,BufRead *.ps,*.eps		so $VIM/syntax/postscr.vim

  " Povray
  au BufNewFile,BufRead *.pov			so $VIM/syntax/pov.vim

  " Procmail
  au BufNewFile,BufRead .procmail		so $VIM/syntax/procmail.vim

  " Prolog
  au BufNewFile,BufRead *.pdb			so $VIM/syntax/prolog.vim

  " Python
  au BufNewFile,BufRead *.py			so $VIM/syntax/python.vim

  " S-lang
  au BufNewFile,BufRead *.sl			so $VIM/syntax/slang.vim

  " Sather
  au BufNewFile,BufRead *.sa			so $VIM/syntax/sather.vim

  " Sendmail
  au BufNewFile,BufRead sendmail.cf		so $VIM/syntax/sm.vim

  " Shell scripts (sh, ksh, bash, csh)
  au BufNewFile,BufRead .profile,.bashrc,.kshrc,*.sh,*.ksh,*.bash,*.env so $VIM/syntax/sh.vim
  au BufNewFile,BufRead .login,.cshrc,*.csh,*.tcsh so $VIM/syntax/csh.vim

  " Z-Shell script
  au BufNewFile,BufRead .z*,zsh*,zlog*		so $VIM/syntax/zsh.vim

  " Smalltalk
  au BufNewFile,BufRead *.st,*.cls		so $VIM/syntax/st.vim

  " SNMP MIB files
  au BufNewFile,BufReadPost *.mib,*.smi		so $VIM/syntax/mib.vim

  " SQL
  au BufNewFile,BufRead *.sql			so $VIM/syntax/sql.vim

  " Telix Salt
  au BufNewFile,BufRead *.slt			so $VIM/syntax/tsalt.vim

  " TeX
  au BufNewFile,BufRead *.tex,*.sty		so $VIM/syntax/tex.vim

  " Motif UIT/UIL files
  au BufNewFile,BufRead *.uit,*.uil		so $VIM/syntax/uil.vim

  " Verilog HDL
  au BufNewFile,BufRead *.v			so $VIM/syntax/verilog.vim

  " Tcl
  au BufNewFile,BufRead *.tcl			so $VIM/syntax/tcl.vim

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

  " Xmath
  au BufNewFile,BufRead *.ms,*.msc,*.msf	so $VIM/syntax/xmath.vim

  " Yacc
  au BufNewFile,BufRead *.y			so $VIM/syntax/yacc.vim

  " Various scripts, without a specific extension
  au BufNewFile,BufRead *			so $VIM/syntax/scripts.vim

  augroup END

  if has("gui")
    50amenu Syntax.off			:syn clear
    50amenu Syntax.ABCDE.Ada		:so $VIM/syntax/ada.vim
    50amenu Syntax.ABCDE.Amiga\ DOS	:so $VIM/syntax/amiga.vim
    50amenu Syntax.ABCDE.assembly	:so $VIM/syntax/asm.vim
    50amenu Syntax.ABCDE.ASN\.1		:so $VIM/syntax/asn.vim
    50amenu Syntax.ABCDE.Atlas		:so $VIM/syntax/atlas.vim
    50amenu Syntax.ABCDE.Awk		:so $VIM/syntax/awk.vim
    50amenu Syntax.ABCDE.BibFile	:so $VIM/syntax/bib.vim
    50amenu Syntax.ABCDE.C		:so $VIM/syntax/c.vim
    50amenu Syntax.ABCDE.C++		:so $VIM/syntax/cpp.vim
    50amenu Syntax.ABCDE.Century\ Term	:so $VIM/syntax/cterm.vim
    50amenu Syntax.ABCDE.Csh\ shell\ script :so $VIM/syntax/csh.vim
    50amenu Syntax.ABCDE.Clean		:so $VIM/syntax/clean.vim
    50amenu Syntax.ABCDE.Cobol		:so $VIM/syntax/cobol.vim
    50amenu Syntax.ABCDE.Diff		:so $VIM/syntax/diff.vim
    50amenu Syntax.ABCDE.Dracula	:so $VIM/syntax/dracula.vim
    50amenu Syntax.ABCDE.Eiffel		:so $VIM/syntax/eiffel.vim
    50amenu Syntax.ABCDE.Elm\ Filter	:so $VIM/syntax/elmfilt.vim
    50amenu Syntax.ABCDE.ESQL-C		:so $VIM/syntax/esqlc.vim
    50amenu Syntax.ABCDE.Expect		:so $VIM/syntax/expect.vim
    50amenu Syntax.FGHIJ.Fortran	:so $VIM/syntax/fortran.vim
    50amenu Syntax.FGHIJ.GP		:so $VIM/syntax/gp.vim
    50amenu Syntax.FGHIJ.HTML		:so $VIM/syntax/html.vim
    50amenu Syntax.FGHIJ.IDL		:so $VIM/syntax/idl.vim
    50amenu Syntax.FGHIJ.Inform		:so $VIM/syntax/inform.vim
    50amenu Syntax.FGHIJ.Java		:so $VIM/syntax/java.vim
    50amenu Syntax.FGHIJ.JavaCC		:so $VIM/syntax/javacc.vim
    50amenu Syntax.FGHIJ.JavaScript	:so $VIM/syntax/javascript.vim
    50amenu Syntax.KLMNO.Lex		:so $VIM/syntax/lex.vim
    50amenu Syntax.KLMNO.Lisp		:so $VIM/syntax/lisp.vim
    50amenu Syntax.KLMNO.Lynx\ Style	:so $VIM/syntax/lss.vim
    50amenu Syntax.KLMNO.Mail		:so $VIM/syntax/mail.vim
    50amenu Syntax.KLMNO.Makefile	:so $VIM/syntax/make.vim
    50amenu Syntax.KLMNO.Maple		:so $VIM/syntax/maple.vim
    50amenu Syntax.KLMNO.Matlab		:so $VIM/syntax/matlab.vim
    50amenu Syntax.KLMNO.Model		:so $VIM/syntax/model.vim
    50amenu Syntax.KLMNO.Modula\ 2	:so $VIM/syntax/modula2.vim
    50amenu Syntax.KLMNO.MS-DOS\ \.bat\ file :so $VIM/syntax/dosbatch.vim
    50amenu Syntax.KLMNO.MS-DOS\ \.ini\ file :so $VIM/syntax/dosini.vim
    50amenu Syntax.KLMNO.MS\ Resource\ file  :so $VIM/syntax/rc.vim
    50amenu Syntax.KLMNO.Muttrc		:so $VIM/syntax/muttrc.vim
    50amenu Syntax.KLMNO.Objective\ C	:so $VIM/syntax/objc.vim
    50amenu Syntax.PQRST.Pascal		:so $VIM/syntax/pascal.vim
    50amenu Syntax.PQRST.Perl		:so $VIM/syntax/perl.vim
    50amenu Syntax.PQRST.Perl\ POD	:so $VIM/syntax/pod.vim
    50amenu Syntax.PQRST.Pike		:so $VIM/syntax/pike.vim
    50amenu Syntax.PQRST.PostScript	:so $VIM/syntax/postscr.vim
    50amenu Syntax.PQRST.Povray		:so $VIM/syntax/pov.vim
    50amenu Syntax.PQRST.Procmail	:so $VIM/syntax/procmail.vim
    50amenu Syntax.PQRST.Prolog		:so $VIM/syntax/prolog.vim
    50amenu Syntax.PQRST.Python		:so $VIM/syntax/python.vim
    50amenu Syntax.PQRST.S-lang		:so $VIM/syntax/slang.vim
    50amenu Syntax.PQRST.Sather		:so $VIM/syntax/sather.vim
    50amenu Syntax.PQRST.Sendmail\.cf	:so $VIM/syntax/sm.vim
    50amenu Syntax.PQRST.Sh\ shell\ script :so $VIM/syntax/sh.vim
    50amenu Syntax.PQRST.SmallTalk	:so $VIM/syntax/st.vim
    50amenu Syntax.PQRST.SNMP\ MIB	:so $VIM/syntax/mib.vim
    50amenu Syntax.PQRST.SQL		:so $VIM/syntax/sql.vim
    50amenu Syntax.PQRST.Telix\ Salt	:so $VIM/syntax/tsalt.vim
    50amenu Syntax.PQRST.Tex		:so $VIM/syntax/tex.vim
    50amenu Syntax.PQRST.Tcl		:so $VIM/syntax/tcl.vim
    50amenu Syntax.UVWXYZ.UIT/UIL	:so $VIM/syntax/uil.vim
    50amenu Syntax.UVWXYZ.Verilog\ HDL	:so $VIM/syntax/verilog.vim
    50amenu Syntax.UVWXYZ.VHDL		:so $VIM/syntax/vhdl.vim
    50amenu Syntax.UVWXYZ.Vim\ help\ file :so $VIM/syntax/help.vim
    50amenu Syntax.UVWXYZ.Vim\ script	:so $VIM/syntax/vim.vim
    50amenu Syntax.UVWXYZ.Viminfo\ file	:so $VIM/syntax/viminfo.vim
    50amenu Syntax.UVWXYZ.vgrindefs	:so $VIM/syntax/vgrindefs.vim
    50amenu Syntax.UVWXYZ.VRML		:so $VIM/syntax/vrml.vim
    50amenu Syntax.UVWXYZ.Xmath		:so $VIM/syntax/xmath.vim
    50amenu Syntax.UVWXYZ.Yacc		:so $VIM/syntax/yacc.vim
    50amenu Syntax.UVWXYZ.Zsh\ shell\ script :so $VIM/syntax/zsh.vim
    50amenu Syntax.color\ test		:sp $VIM/syntax/colortest.vim|so %
    50amenu Syntax.highlight\ test	:sp $VIM/syntax/hitest.vim|so %
    50amenu Syntax.manual		:normal :augroup syntax:au!:augroup END:syn clear:aunmenu Syntax.manual:50amenu Syntax.automatic :aunmenu Syntax.automatic|so $VIM/syntax/syntax.vim
    50amenu Syntax.convert\ to\ HTML	:so $VIM/syntax/2html.vim
  endif


" Source the user-specified syntax highlighting file
  if exists("mysyntaxfile")
    if file_readable(expand(mysyntaxfile))
      execute "source " . mysyntaxfile
    endif
  endif


  " Execute the syntax autocommands for the each buffer.
  doautoall syntax BufRead

endif

" vim: ts=8
