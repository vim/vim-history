" Vim support file to detect file types
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	2000 Jul 19

" Listen very carefully, I will say this only once
if exists("did_load_filetypes")
  finish
endif
let did_load_filetypes = 1

" Line continuation is used here, remove 'C' from 'cpoptions'
let ft_cpo_save = &cpo
set cpo-=C

" Command to set the filetype only when it wasn't set yet.
command -nargs=1 SetFT if !did_filetype() | setlocal ft=<args> | endif

" Function that does the same; a bit longer, but can be followed by a <Bar>
fun! SetFT(name)
  if !did_filetype()
    exe "setlocal ft=" . a:name
  endif
endfun

augroup filetypedetect

" Ignored extensions
au BufNewFile,BufRead *.orig,*.bak,*.old
	\ exe "doau filetypedetect BufRead " . expand("<afile>:r")
au BufNewFile,BufRead *~
	\ exe "doau filetypedetect BufRead " . substitute(expand("<afile>"), '\~$', '', '')
au BufNewFile,BufRead *.in
	\ if expand("<afile>") != "configure.in" |
	\   exe "doau filetypedetect BufRead " . expand("<afile>:r") |
	\ endif


" Abaqus
au BufNewFile,BufRead *.inp			if getline(1) =~ '^\*'|call SetFT("abaqus")|endif

" ABC music notation
au BufNewFile,BufRead *.abc			SetFT abc

" ABEL
au BufNewFile,BufRead *.abl			SetFT abel

" ABEL
au BufNewFile,BufRead *.abl			SetFT abel

" Ada (83, 9X, 95)
au BufNewFile,BufRead *.adb,*.ads		SetFT ada

" AHDL
au BufNewFile,BufRead *.tdf			SetFT ahdl

" Apache style config file
au BufNewFile,BufRead proftpd.conf*		SetFT apachestyle

" Apache config file
au BufNewFile,BufRead httpd.conf*,srm.conf*,access.conf*,.htaccess,apache.conf* SetFT apache

" Applix ELF
au BufNewFile,BufRead *.am			SetFT elf

" Arc Macro Language
au BufNewFile,BufRead *.aml			SetFT aml

" ASN.1
au BufNewFile,BufRead *.asn,*.asn1		SetFT asn

" Active Server Pages (with Visual Basic Script)
au BufNewFile,BufRead *.asa			SetFT aspvbs

" Active Server Pages (with Perl or Visual Basic Script)
au BufNewFile,BufRead *.asp			if getline(1) . getline(2) .  getline(3) =~? "perlscript" | call SetFT("aspperl") | else | call SetFT("aspvbs") | endif

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

  exe "SetFT " . b:asmsyntax
endfun

" Atlas
au BufNewFile,BufRead *.atl,*.as		SetFT atlas

" Avenue
au BufNewFile,BufRead *.ave			SetFT ave

" Awk
au BufNewFile,BufRead *.awk			SetFT awk

" BASIC or Visual Basic
au BufNewFile,BufRead *.bas			call FTCheck_VB("basic")

" Check if one of the first five lines contains "VB_Name".  In that case it is
" probably a Visual Basic file.  Otherwise it's assumed to be "alt" filetype.
fun! FTCheck_VB(alt)
  if getline(1).getline(2).getline(3).getline(4).getline(5) =~? 'VB_Name'
    SetFT vb
  else
    exe "SetFT " . a:alt
  endif
endfun

" Batch file for MSDOS
au BufNewFile,BufRead *.bat,*.sys		SetFT dosbatch

" Batch file for 4DOS
au BufNewFile,BufRead *.btm			SetFT btm

" BC calculator
au BufNewFile,BufRead *.bc			SetFT bc

" BibTeX bibliography database file
au BufNewFile,BufRead *.bib			SetFT bib

" C
au BufNewFile,BufRead *.c			SetFT c

" C++
if has("fname_case")
  au BufNewFile,BufRead *.cpp,*.cc,*.cxx,*.c++,*.C,*.H,*.hh,*.hxx,*.hpp,*.tcc,*.inl,named.conf SetFT cpp
else
  au BufNewFile,BufRead *.cpp,*.cc,*.cxx,*.c++,*.hh,*.hxx,*.hpp,*.tcc,*.inl,named.conf SetFT cpp
endif

" .h files can be C or C++, set c_syntax_for_h if you want C
au BufNewFile,BufRead *.h			if exists("c_syntax_for_h")|call SetFT("c")|else|call SetFT("cpp")|endif

" Cascading Style Sheets
au BufNewFile,BufRead *.css			SetFT css

" Century Term Command Scripts
au BufNewFile,BufRead *.cmd,*.con		SetFT cterm

" CHILL
au BufNewFile,BufReadPost *..ch			SetFT ch

" Changes for WEB and CWEB or CHILL
au BufNewFile,BufRead *.ch			call FTCheck_change()

" This function checks if one of the first ten lines start with a '@'.  In
" that case it is probably a change file, otherwise CHILL is assumed.
fun! FTCheck_change()
  let lnum = 1
  while lnum <= 10
    if getline(lnum)[0] == '@'
      SetFT change
      return
    endif
    let lnum = lnum + 1
  endwhile
  SetFT ch
endfun

" Clean
au BufNewFile,BufReadPost *.dcl,*.icl		SetFT clean

" Clipper
au BufNewFile,BufRead *.prg			SetFT clipper

" Cobol
au BufNewFile,BufRead *.cbl,*.cob,*.cpy,*.lib	SetFT cobol

" Cold Fusion
au BufNewFile,BufRead *.cfm,*.cfi		SetFT cf

" Configure scripts
au BufNewFile,BufRead configure.in		SetFT config

" Communicating Sequential Processes
au BufNewFile,BufRead *.csp,*.fdr		SetFT csp

" CUPL logic description and simulation
au BufNewFile,BufRead *.pld			SetFT cupl
au BufNewFile,BufRead *.si			SetFT cuplsim

" Diff files
au BufNewFile,BufRead *.diff,*.rej		SetFT diff

" Diva (with Skill) or InstallShield
au BufNewFile,BufRead *.rul			if getline(1).getline(2).getline(3).getline(4).getline(5).getline(6) =~? 'InstallShield'|call SetFT("ishd")|else|call SetFT("diva")|endif

" DCL (Digital Command Language - vms)
au BufNewFile,BufRead *.com			SetFT dcl

" Microsoft Module Definition
au BufNewFile,BufRead *.def			SetFT def

" Dracula
au BufNewFile,BufRead drac.*,*.drac,*.drc,*lvs,*lpe SetFT dracula

" DTD (Document Type Definition for XML)
au BufNewFile,BufRead *.dtd			SetFT dtd

" Eiffel
au BufNewFile,BufRead *.e,*.E			SetFT eiffel

" ERicsson LANGuage
au BufNewFile,BufRead *.erl			SetFT erlang

" Elm Filter Rules file
au BufNewFile,BufReadPost filter-rules		SetFT elmfilt

" ESQL-C
au BufNewFile,BufRead *.ec,*.EC			SetFT esqlc

" Exports
au BufNewFile,BufRead exports			SetFT exports

" Focus Executable
au BufNewFile,BufRead *.fex,*.focexec		SetFT focexec

" Focus Master file
au BufNewFile,BufRead *.mas,*.master		SetFT master

" Forth
au BufNewFile,BufRead *.fs,*.ft			SetFT forth

" Fortran
au BufNewFile,BufRead *.f,*.F,*.for,*.fpp,*.f90,*.f95	SetFT fortran

" Fvwm
au BufNewFile,BufRead *fvwmrc*,*fvwm95*.hook
	\ let b:fvwm_version = 1 | SetFT fvwm
au BufNewFile,BufRead *fvwm2rc*
	\ let b:fvwm_version = 2 | SetFT fvwm

" GDB command files
au BufNewFile,BufRead .gdbinit			SetFT gdb

" GDMO
au BufNewFile,BufRead *.mo,*.gdmo		SetFT gdmo

" Gedcom
au BufNewFile,BufRead *.ged			SetFT gedcom

" GP scripts (2.0 and onward)
au BufNewFile,BufRead *.gp			SetFT gp

" Gnuplot scripts
au BufNewFile,BufRead *.gpi			SetFT gnuplot

" Haskell
au BufNewFile,BufRead *.hs			SetFT haskell
au BufNewFile,BufRead *.lhs			SetFT lhaskell

" HTML (.shtml for server side)
au BufNewFile,BufRead *.html,*.htm,*.shtml	SetFT html

" HTML with M4
au BufNewFile,BufRead *.html.m4			SetFT htmlm4

" Hyper Builder
au BufNewFile,BufRead *.hb			SetFT hb

" Icon
au BufNewFile,BufRead *.icn			SetFT icon

" IDL (Interface Description Language)
au BufNewFile,BufRead *.idl			SetFT idl

" IDL (Interactive Data Language)
au BufNewFile,BufRead *.pro			SetFT idlang

" Inform
au BufNewFile,BufRead *.inf,*.INF		SetFT inform

" .INI file for MSDOS
au BufNewFile,BufRead *.ini			SetFT dosini

" Java
au BufNewFile,BufRead *.java,*.jav		SetFT java

" JavaCC
au BufNewFile,BufRead *.jj,*.jjt		SetFT javacc

" JavaScript
au BufNewFile,BufRead *.js,*.javascript		SetFT javascript

" Java Server Pages
au BufNewFile,BufRead *.jsp			SetFT jsp

" Java Properties resource file (note: doesn't catch font.properties.pl)
au BufNewFile,BufRead *.properties,*.properties_??,*.properties_??_??,*.properties_??_??_*		SetFT jproperties

" Jgraph
au BufNewFile,BufRead *.jgr			SetFT jgraph

" Kimwitu[++]
au BufNewFile,BufRead *.k			SetFT kwt

" KDE script
au BufNewFile,BufRead *.ks			SetFT kscript

" Lace (ISE)
au BufNewFile,BufRead *.ace,*.ACE		SetFT lace

" Latte
au BufNewFile,BufRead *.latte,*.lte		SetFT latte

" Lex
au BufNewFile,BufRead *.lex,*.l			SetFT lex

" Lilo: Linux loader
au BufNewFile,BufRead lilo.conf*		SetFT lilo

" Lisp (*.el = ELisp, *.cl = Common Lisp)
if has("fname_case")
  au BufNewFile,BufRead *.lsp,*.el,*.cl,*.L	SetFT lisp
else
  au BufNewFile,BufRead *.lsp,*.el,*.cl		SetFT lisp
endif

" Lite
au BufNewFile,BufRead *.lite,*.lt		SetFT lite

" LOTOS
au BufNewFile,BufRead *.lot,*.lotos		SetFT lotos

" Lout (also: *.lt)
au BufNewFile,BufRead *.lou,*.lout		SetFT lout

" Lua
au BufNewFile,BufRead *.lua			SetFT lua

" Lynx style file
au BufNewFile,BufRead *.lss			SetFT lss

" M4
au BufNewFile,BufRead *.m4			if expand("<afile>") !~?  "html.m4$" | call SetFT("m4") | endif

" Mail (for Elm, trn, mutt and rn)
au BufNewFile,BufRead snd.\d\+,.letter,.letter.\d\+,.followup,.article,.article.\d\+,pico.\d\+,mutt-*-\d\+,mutt\w\{6\},ae\d\+.txt SetFT mail

" Makefile
au BufNewFile,BufRead [mM]akefile*,GNUmakefile,*.mk,*.mak,*.dsp SetFT make

" MakeIndex
au BufNewFile,BufRead *.ist,*.mst		SetFT ist

" Manpage
au BufNewFile,BufRead *.man			SetFT man

" Maple V
au BufNewFile,BufRead *.mv,*.mpl,*.mws		SetFT maple

" Matlab
au BufNewFile,BufRead *.m			SetFT matlab

" Maya Extension Language
au BufNewFile,BufRead *.mel			SetFT mel

" Metafont
au BufNewFile,BufRead *.mf			SetFT mf

" MetaPost
au BufNewFile,BufRead *.mp			SetFT mp

" Modsim III
au BufNewFile,BufRead *.mod			SetFT modsim3

" Modula 2
au BufNewFile,BufRead *.m2,*.DEF,*.MOD,*.md,*.mi SetFT modula2

" Modula 3 (.m3, .i3, .mg, .ig)
au BufNewFile,BufRead *.[mi][3g]		SetFT modula3

" Msql
au BufNewFile,BufRead *.msql			SetFT msql

" M$ Resource files
au BufNewFile,BufRead *.rc			SetFT rc

" Mutt setup file
au BufNewFile,BufRead .muttrc*,Muttrc		SetFT muttrc

" Nastran input/DMAP
"au BufNewFile,BufRead *.dat			SetFT nastran

" Novell netware batch files
au BufNewFile,BufRead *.ncf			SetFT ncf

" Nroff/Troff (*.ms is checked below)
au BufNewFile,BufRead *.me,*.mm,*.tr,*.nr	SetFT nroff
au BufNewFile,BufRead *.[1-9]			call FTCheck_nroff()

" This function checks if one of the first five lines start with a dot.  In
" that case it is probably an nroff file: 'filetype' is set and 1 is returned.
fun! FTCheck_nroff()
  if getline(1)[0] . getline(2)[0] . getline(3)[0] . getline(4)[0] . getline(5)[0] =~ '\.'
    SetFT nroff
    return 1
  endif
  return 0
endfun

" OCAML
au BufNewFile,BufRead *.ml,*.mli,*.mll,*.mly	SetFT ocaml

" OPL
au BufNewFile,BufRead *.[Oo][Pp][Ll]		SetFT opl

" Oracle config file
au BufNewFile,BufRead *.ora			SetFT ora

" Pascal
au BufNewFile,BufRead *.p,*.pas			SetFT pascal

" Delphi project file
au BufNewFile,BufRead *.dpr			SetFT pascal

" Perl
if has("fname_case")
  au BufNewFile,BufRead *.pl,*.PL		SetFT perl
else
  au BufNewFile,BufRead *.pl			SetFT perl
endif

" Perl, XPM or XPM2
au BufNewFile,BufRead *.pm	if getline(1) =~ "XPM2"|call SetFT("xpm2")|elseif getline(1) =~ "XPM"|call SetFT("xpm")|else|call SetFT("perl")|endif

" Perl POD
au BufNewFile,BufRead *.pod			SetFT pod

" Php3
au BufNewFile,BufRead *.php,*.php3		SetFT php3

" Phtml
au BufNewFile,BufRead *.phtml			SetFT phtml

" Pike
au BufNewFile,BufRead *.pike,*.lpc,*.ulpc,*.pmod SetFT pike

" Pine config
au BufNewFile,BufRead .pinerc,pinerc		SetFT pine

" PL/SQL
au BufNewFile,BufRead *.pls,*.plsql		SetFT plsql

" PO (GNU gettext)
au BufNewFile,BufRead *.po			SetFT po

" PostScript
au BufNewFile,BufRead *.ps,*.eps		SetFT postscr

" Povray
au BufNewFile,BufRead *.pov,*.inc		SetFT pov

" Printcap and Termcap
au BufNewFile,BufRead *printcap			let b:ptcap_type = "print"|SetFT ptcap
au BufNewFile,BufRead *termcap			let b:ptcap_type = "term"|SetFT ptcap

" PCCTS
au BufNewFile,BufRead *.g			SetFT pccts

" Procmail
au BufNewFile,BufRead .procmail,.procmailrc	SetFT procmail

" Prolog
au BufNewFile,BufRead *.pdb			SetFT prolog

" Python
au BufNewFile,BufRead *.py			SetFT python

" Radiance
au BufNewFile,BufRead *.rad,*.mat		SetFT radiance

" Rexx
au BufNewFile,BufRead *.rexx,*.rex		SetFT rexx

" Rexx or Rebol
au BufNewFile,BufRead *.r			if getline(1) =~ '^REBOL'|call SetFT("rebol")|else|call SetFT("rexx")|endif

" Remind
au BufNewFile,BufRead .reminders*		SetFT remind

" Rpcgen
au BufNewFile,BufRead *.x			SetFT rpcgen

" Ruby
au BufNewFile,BufRead *.rb			SetFT ruby

" S-lang (or shader language!)
au BufNewFile,BufRead *.sl			SetFT slang

" Samba config
au BufNewFile,BufRead smb.conf			SetFT samba

" SAS script
au BufNewFile,BufRead *.sas			SetFT sas

" Sather
au BufNewFile,BufRead *.sa			SetFT sather

" SDL
au BufNewFile,BufRead *.sdl,*.pr		SetFT sdl

" sed
au BufNewFile,BufRead *.sed			SetFT sed

" Sendmail
au BufNewFile,BufRead sendmail.cf		SetFT sm

" SGML
au BufNewFile,BufRead *.sgm,*.sgml		if getline(1).getline(2).
	\getline(3).getline(4).getline(5) =~? 'linuxdoc'|call SetFT("sgmllnx")|
	\else|call SetFT("sgml")|endif
au BufNewFile,BufRead *.ent			SetFT sgml

" Shell scripts (sh, ksh, bash, csh); Allow .profile_foo etc.
au BufNewFile,BufRead .bashrc*,bashrc,bash.bashrc,.bash_profile*,*.bash call SetFileTypeSH("bash")
au BufNewFile,BufRead .kshrc*,*.ksh call SetFileTypeSH("ksh")
au BufNewFile,BufRead /etc/profile,.profile*,*.sh,*.env call SetFileTypeSH(getline(1))
au BufNewFile,BufRead .login*,.cshrc*,csh.cshrc,csh.login,csh.logout,.tcshrc*,*.csh,*.tcsh,.alias SetFT csh

fun SetFileTypeSH(name)
  if a:name =~ '\<ksh\>'
    let b:is_kornshell = 1
    if exists("b:is_bash")
      unlet b:is_bash
    endif
  elseif exists("g:bash_is_sh") || a:name =~ '\<bash\>'
    let b:is_bash = 1
    if exists("b:is_kornshell")
      unlet b:is_kornshell
    endif
  endif
  SetFT sh
endfun

" Z-Shell script
au BufNewFile,BufRead .zsh*,.zlog*,.zprofile,.zfbfmarks,.zcompdump*,zsh*,zlog*  SetFT zsh

" Scheme
au BufNewFile,BufRead *.scm			SetFT scheme

" Simula
au BufNewFile,BufRead *.sim			SetFT simula

" SKILL
au BufNewFile,BufRead *.il			SetFT skill

" SLRN
au BufNewFile,BufRead .slrnrc			SetFT slrnrc
au BufNewFile,BufRead *.score			SetFT slrnsc

" Smalltalk
au BufNewFile,BufRead *.st,*.cls		SetFT st

" Stored Procedures
au BufNewFile,BufRead *.stp			SetFT stp

" SMIL or XML
au BufNewFile,BufReadPost *.smil
	\ if getline(1) =~ '<?\s*xml.*?>'|
	\   call SetFT("xml")|
	\ else|
	\   call SetFT("smil")|
	\ endif

" SMIL or SNMP MIB file
au BufNewFile,BufRead *.smi		if getline(1) =~ '\<smil\>'|call SetFT("smil")|else|call SetFT("mib")|endif

" Standard ML
au BufNewFile,BufRead *.sml			SetFT sml

" SNMP MIB files
au BufNewFile,BufReadPost *.mib			SetFT mib

" Spec (Linux RPM)
au BufNewFile,BufRead *.spec			SetFT spec

" Speedup (AspenTech plant simulator)
au BufNewFile,BufRead *.speedup,*.spdata,*.spd	SetFT spup

" Spice
au BufNewFile,BufRead *.sp,*.spice		SetFT spice

" Squid
au BufNewFile,BufRead squid.conf		SetFT squid

" SQL (all but the first one for Oracle Designer)
au BufNewFile,BufRead *.sql,*.tyb,*.typ,*.tyc,*.pkb,*.pks	SetFT sql

" SQR
au BufNewFile,BufRead *.sqr,*.sqi		SetFT sqr

" Tads
au BufNewFile,BufRead *.t			SetFT tads

" Tags
au BufNewFile,BufRead tags			SetFT tags

" Tcl
au BufNewFile,BufRead *.tcl,*.tk,*.itcl,*.itk	SetFT tcl

" TealInfo
au BufNewFile,BufRead *.tli			SetFT tli

" Telix Salt
au BufNewFile,BufRead *.slt			SetFT tsalt

" TeX
au BufNewFile,BufRead *.tex,*.sty,*.dtx,*.ltx	SetFT tex

" Texinfo
au BufNewFile,BufRead *.texinfo,*.texi,*.txi	SetFT texinfo

" TF mud client
au BufNewFile,BufRead *.tf			SetFT tf

" Motif UIT/UIL files
au BufNewFile,BufRead *.uit,*.uil		SetFT uil

" Verilog HDL
au BufNewFile,BufRead *.v			SetFT verilog

" VHDL
au BufNewFile,BufRead *.hdl,*.vhd,*.vhdl,*.vhdl_[0-9]*,*.vbe,*.vst  SetFT vhdl

" Vim Help file
if has("mac")
  au BufNewFile,BufRead *[/:]vim*[/:]doc[/:]*.txt,*[/:]runtime[/:]doc[/:]*.txt SetFT help
else
  au BufNewFile,BufRead */vim*/doc/*.txt,*/runtime/doc/*.txt	SetFT help
endif

" Vim script
au BufNewFile,BufRead *vimrc*,*.vim,.exrc,_exrc SetFT vim

" Viminfo file
au BufNewFile,BufRead .viminfo,_viminfo		SetFT viminfo

" Visual Basic (also uses *.bas) or FORM
au BufNewFile,BufRead *.frm			call FTCheck_VB("form")

" Vgrindefs file
au BufNewFile,BufRead vgrindefs			SetFT vgrindefs

" VRML V1.0c
au BufNewFile,BufRead *.wrl			SetFT vrml

" Webmacro
au BufNewFile,BufRead *.wm			SetFT webmacro

" Website MetaLanguage
au BufNewFile,BufRead *.wml			SetFT wml

" Winbatch
au BufNewFile,BufRead *.wbt			SetFT winbatch

" CVS commit file
au BufNewFile,BufRead cvs\d\+			SetFT cvs

" CWEB
au BufNewFile,BufRead *.w			SetFT cweb

" WEB (*.web is also used for Winbatch: Guess, based on expecting "%" comment
" lines in a WEB file).
au BufNewFile,BufRead *.web			if getline(1)[0].getline(2)[0].getline(3)[0].getline(4)[0].getline(5)[0] =~ "%" | call SetFT("web") | else | call SetFT("winbatch") | endif

" X Pixmap (dynamically sets colors, use BufEnter to make it work better)
au BufEnter *.xpm				if getline(1) =~ "XPM2"|call SetFT("xpm2")|else|call SetFT("xpm")|endif
au BufEnter *.xpm2				SetFT xpm2

" XS Perl extension interface language
au BufEnter *.xs				SetFT xs

" X resources file
au BufNewFile,BufRead .Xdefaults,.Xresources,Xresources*,*/app-defaults/*,*/Xresources/*,xdm-config SetFT xdefaults

" Xmath
au BufNewFile,BufRead *.msc,*.msf		SetFT xmath
au BufNewFile,BufRead *.ms			if !FTCheck_nroff() | call SetFT("xmath") | endif

" vim: ts=8
" XML
au BufNewFile,BufRead *.xml,*.xsl		SetFT xml

" Yacc
au BufNewFile,BufRead *.y			SetFT yacc

" Z80 assembler asz80
au BufNewFile,BufRead *.z8a			SetFT z8a

augroup END


" Source the user-specified filetype file, for backwards compatibility with
" Vim 5.x.
if exists("myfiletypefile") && file_readable(expand(myfiletypefile))
  execute "source " . myfiletypefile
endif


" Check for "*" after loading myfiletypefile, so that scripts.vim is only used
" when there are no matching file name extensions.
augroup filetypedetect
au BufNewFile,BufRead,StdinReadPost *		if !did_filetype()|so <sfile>:p:h/scripts.vim|endif

" Extra checks for when no filetype has been detected now

" Printcap and Termcap
au BufNewFile,BufRead *printcap*		if !did_filetype()|let b:ptcap_type = "print"|set ft=ptcap|endif
au BufNewFile,BufRead *termcap*			if !did_filetype()|let b:ptcap_type = "term"|set ft=ptcap|endif

augroup END


" If the GUI is already running, may still need to install the Syntax menu.
" Don't do it when the 'M' flag is included in 'guioptions'
if has("gui_running") && !exists("did_install_syntax_menu") && &guioptions !~# "M"
  source <sfile>:p:h/menu.vim
endif

" Restore 'cpoptions'
let &cpo = ft_cpo_save
unlet ft_cpo_save
