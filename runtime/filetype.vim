" Vim support file to detect file types
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	2000 Jan 02

if !exists("did_load_filetypes")
let did_load_filetypes = 1

" Line continuation is used here, remove 'C' from 'cpoptions'
let ft_cpo_save = &cpo
set cpo-=C

augroup filetype

" Ignored extensions
au BufNewFile,BufRead *.orig,*.bak,*.old	exe "doau filetype BufRead " . expand("<afile>:r")
au BufNewFile,BufRead *~		exe "doau filetype BufRead " . substitute(expand("<afile>"), '\~$', '', '')


" ABC music notation
au BufNewFile,BufRead *.abc			set ft=abc

" ABEL
au BufNewFile,BufRead *.abl			set ft=abel

" Ada (83, 9X, 95)
au BufNewFile,BufRead *.adb,*.ads		set ft=ada

" AHDL
au BufNewFile,BufRead *.tdf			set ft=ahdl

" Apache Style
au BufNewFile,BufRead proftpd.conf*,httpd.conf*,srm.conf*,access.conf* set ft=apachestyle

" Applix ELF
au BufNewFile,BufRead *.am			set ft=elf

" Arc Macro Language
au BufNewFile,BufRead *.aml			set ft=aml

" ASN.1
au BufNewFile,BufRead *.asn,*.asn1		set ft=asn

" Active Server Pages (with Visual Basic Script)
au BufNewFile,BufRead *.asp,*.asa		set ft=aspvbs

" Assembly (all kinds)
" *.lst is not pure assembly, it has two extra columns (address, byte codes)
au BufNewFile,BufRead *.asm,*.s,*.i,*.mac,*.lst	call FTCheck_asm()

" This function checks for the kind of assembly that is wanted by the user, or
" can be detected from the first five lines of the file.
fun! FTCheck_asm()
  " make sure b:asmsyntax exists
  if !exists("b:asmsyntax")
    let b:asmsyntax = ""
  endif

  if b:asmsyntax == ""
    " see if file contains any asmsyntax=foo overrides. If so, change
    " b:asmsyntax appropriately
    let head = " ".getline(1)." ".getline(2)." ".getline(3)." ".getline(4).
	\" ".getline(5)." "
    if head =~ '\sasmsyntax=\S\+\s'
      let b:asmsyntax = substitute(head, '.*\sasmsyntax=\(\S\+\)\s.*','\1', "")
    endif
  endif

  " if b:asmsyntax still isn't set, default to asmsyntax or GNU
  if b:asmsyntax == ""
    if exists("g:asmsyntax")
      let b:asmsyntax = g:asmsyntax
    else
      let b:asmsyntax = "asm"
    endif
  endif

  exe "set ft=" . b:asmsyntax
endfun

" Atlas
au BufNewFile,BufRead *.atl,*.as		set ft=atlas

" Avenue
au BufNewFile,BufRead *.ave			set ft=ave

" Awk
au BufNewFile,BufRead *.awk			set ft=awk

" BASIC or Visual Basic
" Check if one of the first five lines contains "VB_Name".  In that case it is
" probably a Visual Basic file.  Otherwise it's assumed to be 'normal' basic.
au BufNewFile,BufRead *.bas			if getline(1).getline(2).
	\getline(3).getline(4).getline(5) =~? 'VB_Name'|set ft=vb|
	\else|set ft=basic|endif

" Batch file for MSDOS
au BufNewFile,BufRead *.bat,*.sys		set ft=dosbatch

" Batch file for 4DOS
au BufNewFile,BufRead *.btm			set ft=btm

" BibTeX bibliography database file
au BufNewFile,BufRead *.bib			set ft=bib

" C
au BufNewFile,BufRead *.c			set ft=c

" C++
if has("fname_case")
  au BufNewFile,BufRead *.cpp,*.cc,*.cxx,*.c++,*.C,*.H,*.hh,*.hxx,*.hpp,*.inl set ft=cpp
else
  au BufNewFile,BufRead *.cpp,*.cc,*.cxx,*.c++,*.hh,*.hxx,*.hpp,*.inl set ft=cpp
endif

" .h files can be C or C++, set c_syntax_for_h if you want C
au BufNewFile,BufRead *.h			if exists("c_syntax_for_h")|set ft=c|else|set ft=cpp|endif

" Cascading Style Sheets
au BufNewFile,BufRead *.css			set ft=css

" Century Term Command Scripts
au BufNewFile,BufRead *.cmd,*.con		set ft=cterm

" Clean
au BufNewFile,BufReadPost *.dcl,*.icl		set ft=clean

" Clipper
au BufNewFile,BufRead *.prg			set ft=clipper

" Cobol
au BufNewFile,BufRead *.cbl,*.cob,*.cpy,*.lib	set ft=cobol

" Cold Fusion
au BufNewFile,BufRead *.cfm,*.cfi		set ft=cf

" Configure scripts
au BufNewFile,BufRead configure.in		set ft=config

" CUPL logic description and simulation
au BufNewFile,BufRead *.pld			set ft=cupl
au BufNewFile,BufRead *.si			set ft=cuplsim

" Diff files
au BufNewFile,BufRead *.diff,*.rej		set ft=diff

" Diva (with Skill) or InstallShield
au BufNewFile,BufRead *.rul			if getline(1).getline(2).getline(3).getline(4).getline(5).getline(6) =~? 'InstallShield'|set ft=ishd|else|set ft=diva|endif

" DCL (Digital Command Language - vms)
au BufNewFile,BufRead *.com			set ft=dcl

" Microsoft Module Definition
au BufNewFile,BufRead *.def			set ft=def

" Dracula
au BufNewFile,BufRead drac.*,*.drac,*.drc,*lvs,*lpe set ft=dracula

" DTD (Document Type Definition for XML)
au BufNewFile,BufRead *.dtd			set ft=dtd

" Eiffel
au BufNewFile,BufRead *.e,*.E			set ft=eiffel

" ERicsson LANGuage
au BufNewFile,BufRead *.erl			set ft=erlang

" Elm Filter Rules file
au BufNewFile,BufReadPost filter-rules		set ft=elmfilt

" ESQL-C
au BufNewFile,BufRead *.ec,*.EC			set ft=esqlc

" Exports
au BufNewFile,BufRead exports			set ft=exports

" Focus Executable
au BufNewFile,BufRead *.fex,*.focexec		set ft=focexec

" Focus Master file
au BufNewFile,BufRead *.mas,*.master		set ft=master

" Forth
au BufNewFile,BufRead *.fs,*.ft			set ft=forth

" Fortran
au BufNewFile,BufRead *.f,*.F,*.for,*.fpp	set ft=fortran

" Fvwm
au BufNewFile,BufRead *fvwmrc*
	\ let use_fvwm_1 = 1 | let use_fvwm_2 = 0 | set ft=fvwm
au BufNewFile,BufRead *fvwm2rc*
	\ let use_fvwm_2 = 1 | let use_fvwm_1 = 0 | set ft=fvwm

" GDB command files
au BufNewFile,BufRead .gdbinit			set ft=gdb

" GDMO
au BufNewFile,BufRead *.mo,*.gdmo		set ft=gdmo

" Gedcom
au BufNewFile,BufRead *.ged			set ft=gedcom

" GP scripts (2.0 and onward)
au BufNewFile,BufRead *.gp			set ft=gp

" Gnuplot scripts
au BufNewFile,BufRead *.gpi			set ft=gnuplot

" Haskell
au BufNewFile,BufRead *.hs			set ft=haskell
au BufNewFile,BufRead *.lhs			set ft=lhaskell

" HTML (.shtml for server side)
au BufNewFile,BufRead *.html,*.htm,*.shtml	set ft=html

" HTML with M4
au BufNewFile,BufRead *.html.m4			set ft=htmlm4

" Icon
au BufNewFile,BufRead *.icn			set ft=icon

" IDL (Interface Description Language)
au BufNewFile,BufRead *.idl			set ft=idl

" IDL (Interactive Data Language)
au BufNewFile,BufRead *.pro			set ft=idlang

" Inform
au BufNewFile,BufRead *.inf,*.INF		set ft=inform

" .INI file for MSDOS
au BufNewFile,BufRead *.ini			set ft=dosini

" Java
au BufNewFile,BufRead *.java,*.jav		set ft=java

" JavaCC
au BufNewFile,BufRead *.jj,*.jjt		set ft=javacc

" JavaScript
au BufNewFile,BufRead *.js,*.javascript		set ft=javascript

" Java Server Pages
au BufNewFile,BufRead *.jsp			set ft=jsp

" Jgraph
au BufNewFile,BufRead *.jgr			set ft=jgraph

" Kimwitu[++]
au BufNewFile,BufRead *.k			set ft=kwt

" Lace (ISE)
au BufNewFile,BufRead *.ace,*.ACE		set ft=lace

" Lex
au BufNewFile,BufRead *.lex,*.l			set ft=lex

" Lilo: Linux loader
au BufNewFile,BufRead lilo.conf*		set ft=lilo

" Lisp
if has("fname_case")
  au BufNewFile,BufRead *.lsp,*.L		set ft=lisp
else
  au BufNewFile,BufRead *.lsp			set ft=lisp
endif

" Lite
au BufNewFile,BufRead *.lite,*.lt		set ft=lite

" LOTOS
au BufNewFile,BufRead *.lot,*.lotos		set ft=lotos

" Lout (also: *.lt)
au BufNewFile,BufRead *.lou,*.lout		set ft=lout

" Lua
au BufNewFile,BufRead *.lua			set ft=lua

" Lynx style file
au BufNewFile,BufRead *.lss			set ft=lss

" M4
au BufNewFile,BufRead *.m4			if expand("<afile>") !~?  "html.m4$" | set ft=m4 | endif

" Mail (for Elm, trn and rn)
au BufNewFile,BufRead snd.*,.letter,.letter.[0-9]\+,.followup,.article,.article.[0-9]\+,pico.[0-9]\+,mutt*[0-9],ae[0-9]\+.txt set ft=mail

" Makefile
au BufNewFile,BufRead [mM]akefile*,GNUmakefile,*.mk,*.mak,*.dsp set ft=make

" MakeIndex
au BufNewFile,BufRead *.ist,*.mst		set ft=ist

" Manpage
au BufNewFile,BufRead *.man			set ft=man

" Maple V
au BufNewFile,BufRead *.mv,*.mpl,*.mws		set ft=maple

" Matlab
au BufNewFile,BufRead *.m			set ft=matlab

" Metafont
au BufNewFile,BufRead *.mf			set ft=mf

" MetaPost
au BufNewFile,BufRead *.mp			set ft=mp

" Modsim III
au BufNewFile,BufRead *.mod			set ft=modsim3

" Modula 2
au BufNewFile,BufRead *.m2,*.DEF,*.MOD,*.md,*.mi set ft=modula2

" Modula 3
au BufNewFile,BufRead *.m3,*.i3			set ft=modula3

" Msql
au BufNewFile,BufRead *.msql			set ft=msql

" M$ Resource files
au BufNewFile,BufRead *.rc			set ft=rc

" Mutt setup file
au BufNewFile,BufRead .muttrc*,Muttrc		set ft=muttrc

" Novell netware batch files
au BufNewFile,BufRead *.ncf			set ft=ncf

" Nroff/Troff (*.ms is checked below)
au BufNewFile,BufRead *.me,*.mm,*.tr,*.nr	set ft=nroff
au BufNewFile,BufRead *.[1-9]			call FTCheck_nroff()

" This function checks if one of the first five lines start with a dot.  In
" that case it is probably an nroff file: 'filetype' is set and 1 is returned.
fun! FTCheck_nroff()
  if getline(1)[0] . getline(2)[0] . getline(3)[0] . getline(4)[0] . getline(5)[0] =~ '\.'
    set ft=nroff
    return 1
  endif
  return 0
endfun

" OCAML
au BufNewFile,BufRead *.ml,*.mli,*.mll,*.mly	set ft=ocaml

" OPL
au BufNewFile,BufRead *.[Oo][Pp][Ll]		set ft=opl

" Pascal
au BufNewFile,BufRead *.p,*.pas			set ft=pascal

" Perl
if has("fname_case")
  au BufNewFile,BufRead *.pl,*.PL		set ft=perl
else
  au BufNewFile,BufRead *.pl			set ft=perl
endif

" Perl, XPM or XPM2
au BufNewFile,BufRead *.pm	if getline(1) =~ "XPM2"|set ft=xpm2|elseif getline(1) =~ "XPM"|set ft=xpm|else|set ft=perl|endif

" Perl POD
au BufNewFile,BufRead *.pod			set ft=pod

" Php3
au BufNewFile,BufRead *.php,*.php3		set ft=php3

" Phtml
au BufNewFile,BufRead *.phtml			set ft=phtml

" Pike
au BufNewFile,BufRead *.pike,*.lpc,*.ulpc,*.pmod set ft=pike

" Pine config
au BufNewFile,BufRead .pinerc,pinerc		set ft=pine

" PL/SQL
au BufNewFile,BufRead *.pls,*.plsql		set ft=plsql

" PO (GNU gettext)
au BufNewFile,BufRead *.po			set ft=po

" PostScript
au BufNewFile,BufRead *.ps,*.eps		set ft=postscr

" Povray
au BufNewFile,BufRead *.pov,*.inc		set ft=pov

" Printcap and Termcap
au BufNewFile,BufRead *printcap*,*termcap*	set ft=ptcap

" PCCTS
au BufNewFile,BufRead *.g			set ft=pccts

" Procmail
au BufNewFile,BufRead .procmail,.procmailrc	set ft=procmail

" Prolog
au BufNewFile,BufRead *.pdb			set ft=prolog

" Python
au BufNewFile,BufRead *.py			set ft=python

" Radiance
au BufNewFile,BufRead *.rad,*.mat		set ft=radiance

" Rexx
au BufNewFile,BufRead *.rexx,*.rex		set ft=rexx

" Rexx or Rebol
au BufNewFile,BufRead *.r			if getline(1) =~ '^REBOL'|set ft=rebol|else|set ft=rexx|endif

" Remind
au BufNewFile,BufRead .reminders*		set ft=remind

" Rpcgen
au BufNewFile,BufRead *.x			set ft=rpcgen

" S-lang (or shader language!)
au BufNewFile,BufRead *.sl			set ft=slang

" SAS script
au BufNewFile,BufRead *.sas			set ft=sas

" Sather
au BufNewFile,BufRead *.sa			set ft=sather

" SDL
au BufNewFile,BufRead *.sdl,*.pr		set ft=sdl

" sed
au BufNewFile,BufRead *.sed			set ft=sed

" Sendmail
au BufNewFile,BufRead sendmail.cf		set ft=sm

" SGML
au BufNewFile,BufRead *.sgm,*.sgml		if getline(1).getline(2).
	\getline(3).getline(4).getline(5) =~? 'linuxdoc'|set ft=sgmllnx|
	\else|set ft=sgml|endif
au BufNewFile,BufRead *.ent			set ft=sgml

" Shell scripts (sh, ksh, bash, csh); Allow .profile_foo etc.
au BufNewFile,BufRead /etc/profile,.profile*,.bashrc*,.bash_profile*,.kshrc*,*.sh,*.ksh,*.bash,*.env set ft=sh
au BufNewFile,BufRead .login*,.cshrc*,.tcshrc*,*.csh,*.tcsh set ft=csh

" Z-Shell script
au BufNewFile,BufRead .z*,zsh*,zlog*		set ft=zsh

" Scheme
au BufNewFile,BufRead *.scm			set ft=scheme

" Simula
au BufNewFile,BufRead *.sim			set ft=simula

" SKILL
au BufNewFile,BufRead *.il			set ft=skill

" SLRN
au BufNewFile,BufRead .slrnrc			set ft=slrnrc
au BufNewFile,BufRead *.score			set ft=slrnsc

" Smalltalk
au BufNewFile,BufRead *.st,*.cls		set ft=st

" SMIL or XML
au BufNewFile,BufReadPost *.smil
	\ if getline(1) =~ '<?\s*xml.*?>'|
	\   set ft=xml|
	\ else|
	\   set ft=smil|
	\ endif

" SMIL or SNMP MIB file
au BufNewFile,BufRead *.smi		if getline(1) =~ '\<smil\>'|set ft=smil|else|set ft=mib|endif

" SNMP MIB files
au BufNewFile,BufReadPost *.mib			set ft=mib

" Spec (Linux RPM)
au BufNewFile,BufRead *.spec			set ft=spec

" Speedup (AspenTech plant simulator)
au BufNewFile,BufRead *.speedup,*.spdata,*.spd	set ft=spup

" Spice
au BufNewFile,BufRead *.sp,*.spice		set ft=spice

" Squid
au BufNewFile,BufRead squid.conf		set ft=squid

" SQL (all but the first one for Oracle Designer)
au BufNewFile,BufRead *.sql,*.tyb,*.typ,*.tyc,*.pkb,*.pks	set ft=sql

" SQR
au BufNewFile,BufRead *.sqr,*.sqi		set ft=sqr

" Tads
au BufNewFile,BufRead *.t			set ft=tads

" Tags
au BufNewFile,BufRead tags			set ft=tags

" Tcl
au BufNewFile,BufRead *.tcl,*.tk,*.itcl,*.itk	set ft=tcl

" Telix Salt
au BufNewFile,BufRead *.slt			set ft=tsalt

" TeX
au BufNewFile,BufRead *.tex,*.sty,*.dtx,*.ltx	set ft=tex

" Texinfo
au BufNewFile,BufRead *.texinfo,*.texi,*.txi	set ft=texinfo

" TF mud client
au BufNewFile,BufRead *.tf			set ft=tf

" Motif UIT/UIL files
au BufNewFile,BufRead *.uit,*.uil		set ft=uil

" Verilog HDL
au BufNewFile,BufRead *.v			set ft=verilog

" VHDL
au BufNewFile,BufRead *.hdl,*.vhd,*.vhdl,*.vhdl_[0-9]*,*.vbe,*.vst  set ft=vhdl

" Vim Help file
if has("mac")
  au BufNewFile,BufRead *[/:]vim*[/:]doc[/:]*.txt,*[/:]runtime[/:]doc[/:]*.txt set ft=help
else
  au BufNewFile,BufRead */vim*/doc/*.txt,*/runtime/doc/*.txt	set ft=help
endif

" Vim script
au BufNewFile,BufRead *vimrc*,*.vim,.exrc,_exrc set ft=vim

" Viminfo file
au BufNewFile,BufRead .viminfo,_viminfo		set ft=viminfo

" Visual Basic (also uses *.bas)
au BufNewFile,BufRead *.frm			set ft=vb

" Vgrindefs file
au BufNewFile,BufRead vgrindefs			set ft=vgrindefs

" VRML V1.0c
au BufNewFile,BufRead *.wrl			set ft=vrml

" Winbatch
au BufNewFile,BufRead *.wbt			set ft=winbatch

" CWEB
au BufNewFile,BufRead *.w			set ft=cweb

" WEB (*.web is also used for Winbatch: Guess, based on expecting "%" comment
" lines in a WEB file).
au BufNewFile,BufRead *.web			if getline(1)[0].getline(2)[0].getline(3)[0].getline(4)[0].getline(5)[0] =~ "%" | set ft=web | else | set ft=winbatch | endif

" Changes for WEB and CWEB
au BufNewFile,BufRead *.ch			set ft=change

" X Pixmap (dynamically sets colors, use BufEnter to make it work better)
au BufEnter *.xpm				if getline(1) =~ "XPM2"|set ft=xpm2|else|set ft=xpm|endif
au BufEnter *.xpm2				set ft=xpm2

" XS Perl extension interface language
au BufEnter *.xs				set ft=xs

" X resources file
au BufNewFile,BufRead .Xdefaults,.Xresources,*/app-defaults/* set ft=xdefaults

" Xmath
au BufNewFile,BufRead *.msc,*.msf		set ft=xmath
au BufNewFile,BufRead *.ms			if !FTCheck_nroff() | set ft=xmath | endif

" vim: ts=8
" XML
au BufNewFile,BufRead *.xml,*.xsl		set ft=xml

" Yacc
au BufNewFile,BufRead *.y			set ft=yacc

" Z80 assembler asz80
au BufNewFile,BufRead *.z8a			set ft=z8a

augroup END


" Source the user-specified filetype file
if exists("myfiletypefile") && file_readable(expand(myfiletypefile))
  execute "source " . myfiletypefile
endif


" Check for "*" after loading myfiletypefile, so that scripts.vim is only used
" when there are no matching file name extensions.
augroup filetype
  au BufNewFile,BufRead,StdinReadPost *		if !did_filetype()|so <sfile>:p:h/scripts.vim|endif
augroup END


" If the GUI is already running, may still need to install the Syntax menu.
" Don't do it when the 'M' flag is included in 'guioptions'
if has("gui_running") && !exists("did_install_syntax_menu") && &guioptions !~# "M"
  source <sfile>:p:h/menu.vim
endif

" Restore 'cpoptions'
let &cpo = ft_cpo_save
unlet ft_cpo_save


endif " !exists("did_load_filetypes")

" vim: ts=8
