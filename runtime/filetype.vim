" Vim support file to detect file types
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	2000 Dec 11

" Listen very carefully, I will say this only once
if exists("did_load_filetypes")
  finish
endif
let did_load_filetypes = 1

" Line continuation is used here, remove 'C' from 'cpoptions'
let s:cpo_save = &cpo
set cpo&vim

augroup filetypedetect

" Ignored extensions
au BufNewFile,BufRead *.orig,*.bak,*.old
	\ exe "doau filetypedetect BufRead " . expand("<afile>:r")
au BufNewFile,BufRead *~
	\ exe "doau filetypedetect BufRead " . substitute(expand("<afile>"), '\~$', '', '')
au BufNewFile,BufRead *.in
	\ if expand("<afile>:t") != "configure.in" |
	\   exe "doau filetypedetect BufRead " . expand("<afile>:r") |
	\ endif


" Abaqus
au BufNewFile,BufRead *.inp
	\ if getline(1) =~ '^\*' | setf abaqus | endif

" ABC music notation
au BufNewFile,BufRead *.abc			setf abc

" ABEL
au BufNewFile,BufRead *.abl			setf abel

" ABEL
au BufNewFile,BufRead *.abl			setf abel

" Ada (83, 9X, 95)
au BufNewFile,BufRead *.adb,*.ads		setf ada

" AHDL
au BufNewFile,BufRead *.tdf			setf ahdl

" Apache style config file
au BufNewFile,BufRead proftpd.conf*		setf apachestyle

" Apache config file
au BufNewFile,BufRead httpd.conf*,srm.conf*,access.conf*,.htaccess,apache.conf* setf apache

" Applix ELF
au BufNewFile,BufRead *.am
	\ if expand("<afile>") !~? 'Makefile.am\>' | setf elf | endif

" Arc Macro Language
au BufNewFile,BufRead *.aml			setf aml

" ASN.1
au BufNewFile,BufRead *.asn,*.asn1		setf asn

" Active Server Pages (with Visual Basic Script)
au BufNewFile,BufRead *.asa			setf aspvbs

" Active Server Pages (with Perl or Visual Basic Script)
au BufNewFile,BufRead *.asp
	\ if getline(1) . getline(2) .  getline(3) =~? "perlscript" |
	\   setf aspperl |
	\ else |
	\   setf aspvbs |
	\ endif

" Assembly (all kinds)
" *.lst is not pure assembly, it has two extra columns (address, byte codes)
au BufNewFile,BufRead *.asm,*.[sS],*.i,*.mac,*.lst	call <SID>FTasm()

" This function checks for the kind of assembly that is wanted by the user, or
" can be detected from the first five lines of the file.
fun! <SID>FTasm()
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

  exe "setf " . b:asmsyntax
endfun

" Atlas
au BufNewFile,BufRead *.atl,*.as		setf atlas

" Automake
au BufNewFile,BufRead [mM]akefile.am		setf automake

" Avenue
au BufNewFile,BufRead *.ave			setf ave

" Awk
au BufNewFile,BufRead *.awk			setf awk

" B
au BufNewFile,BufRead *.mch,*.ref,*.imp		setf b

" BASIC or Visual Basic
au BufNewFile,BufRead *.bas			call <SID>FTVB("basic")

" Check if one of the first five lines contains "VB_Name".  In that case it is
" probably a Visual Basic file.  Otherwise it's assumed to be "alt" filetype.
fun! <SID>FTVB(alt)
  if getline(1).getline(2).getline(3).getline(4).getline(5) =~? 'VB_Name'
    setf vb
  else
    exe "setf " . a:alt
  endif
endfun

" Visual Basic Script (close to Visual Basic)
au BufNewFile,BufRead *.vbs			setf vb

" Batch file for MSDOS (*.cmd is close enough)
au BufNewFile,BufRead *.bat,*.cmd		setf dosbatch

" Batch file for 4DOS
au BufNewFile,BufRead *.btm			setf btm

" BC calculator
au BufNewFile,BufRead *.bc			setf bc

" BibTeX bibliography database file
au BufNewFile,BufRead *.bib			setf bib

" Blank
au BufNewFile,BufRead *.bl			setf blank

" C
au BufNewFile,BufRead *.c			setf c

" Cyn++
au BufNewFile,BufRead *.cyn			setf cynpp

" Cynlib
" .cc and .cpp files can be C++ or Cynlib.
au BufNewFile,BufRead *.cc
	\ if exists("cynlib_syntax_for_cc")|setf cynlib|else|setf cpp|endif
au BufNewFile,BufRead *.cpp
	\ if exists("cynlib_syntax_for_cpp")|setf cynlib|else|setf cpp|endif

" C++
if has("fname_case")
  au BufNewFile,BufRead *.cxx,*.c++,*.C,*.H,*.hh,*.hxx,*.hpp,*.tcc,*.inl,named.conf setf cpp
else
  au BufNewFile,BufRead *.cxx,*.c++,*.hh,*.hxx,*.hpp,*.tcc,*.inl,named.conf setf cpp
endif

" .h files can be C or C++, set c_syntax_for_h if you want C
au BufNewFile,BufRead *.h
	\ if exists("c_syntax_for_h") | setf c | else | setf cpp | endif

" Cascading Style Sheets
au BufNewFile,BufRead *.css			setf css

" Century Term Command Scripts (*.cmd too)
au BufNewFile,BufRead *.con			setf cterm

" Changelog
au BufNewFile,BufReadPost Change[lL]og		setf changelog

" CHILL
au BufNewFile,BufReadPost *..ch			setf ch

" Changes for WEB and CWEB or CHILL
au BufNewFile,BufRead *.ch			call <SID>FTchange()

" This function checks if one of the first ten lines start with a '@'.  In
" that case it is probably a change file, otherwise CHILL is assumed.
fun! <SID>FTchange()
  let lnum = 1
  while lnum <= 10
    if getline(lnum)[0] == '@'
      setf change
      return
    endif
    let lnum = lnum + 1
  endwhile
  setf ch
endfun

" Clean
au BufNewFile,BufReadPost *.dcl,*.icl		setf clean

" Clipper
au BufNewFile,BufRead *.prg			setf clipper

" Cobol
au BufNewFile,BufRead *.cbl,*.cob,*.cpy,*.lib	setf cobol

" Cold Fusion
au BufNewFile,BufRead *.cfm,*.cfi		setf cf

" Configure scripts
au BufNewFile,BufRead configure.in		setf config

" Communicating Sequential Processes
au BufNewFile,BufRead *.csp,*.fdr		setf csp

" CUPL logic description and simulation
au BufNewFile,BufRead *.pld			setf cupl
au BufNewFile,BufRead *.si			setf cuplsim

" Diff files
au BufNewFile,BufRead *.diff,*.rej		setf diff

" Diva (with Skill) or InstallShield
au BufNewFile,BufRead *.rul
	\ if getline(1).getline(2).getline(3).getline(4).getline(5).getline(6) =~? 'InstallShield' |
	\   setf ishd |
	\ else |
	\   setf diva |
	\ endif

" DCL (Digital Command Language - vms)
au BufNewFile,BufRead *.com			setf dcl

" Dylan - lid files
au BufNewFile,BufRead *.lid			setf dylanlid

" Dylan - intr files (melange)
au BufNewFile,BufRead *.intr			setf dylanintr

" Dylan
au BufNewFile,BufRead *.dylan			setf dylan

" Microsoft Module Definition
au BufNewFile,BufRead *.def			setf def

" Dracula
au BufNewFile,BufRead drac.*,*.drac,*.drc,*lvs,*lpe setf dracula

" DTD (Document Type Definition for XML)
au BufNewFile,BufRead *.dtd			setf dtd

" Embedix Component Description
au BufNewFile,BufRead *.ecd			setf ecd

" Eiffel
au BufNewFile,BufRead *.e,*.E			setf eiffel

" ERicsson LANGuage
au BufNewFile,BufRead *.erl			setf erlang

" Elm Filter Rules file
au BufNewFile,BufReadPost filter-rules		setf elmfilt

" ESQL-C
au BufNewFile,BufRead *.ec,*.EC			setf esqlc

" Essbase script
au BufNewFile,BufRead *.csc			setf csc

" Expect
au BufNewFile,BufRead *.exp			setf expect

" Exports
au BufNewFile,BufRead exports			setf exports

" Focus Executable
au BufNewFile,BufRead *.fex,*.focexec		setf focexec

" Focus Master file
au BufNewFile,BufRead *.mas,*.master		setf master

" Forth
au BufNewFile,BufRead *.fs,*.ft			setf forth

" Fortran
au BufNewFile,BufRead *.f,*.F,*.for,*.fpp,*.ftn,*.f77,*.f90,*.f95	setf fortran

" Fvwm
au BufNewFile,BufRead *fvwmrc*,*fvwm95*.hook
	\ let b:fvwm_version = 1 | setf fvwm
au BufNewFile,BufRead *fvwm2rc*
	\ let b:fvwm_version = 2 | setf fvwm

" GDB command files
au BufNewFile,BufRead .gdbinit			setf gdb

" GDMO
au BufNewFile,BufRead *.mo,*.gdmo		setf gdmo

" Gedcom
au BufNewFile,BufRead *.ged			setf gedcom

" GP scripts (2.0 and onward)
au BufNewFile,BufRead *.gp			setf gp

" Gnuplot scripts
au BufNewFile,BufRead *.gpi			setf gnuplot

" GNU Server Pages
au BufNewFile,BufRead *.gsp			setf gsp

" Haskell
au BufNewFile,BufRead *.hs			setf haskell
au BufNewFile,BufRead *.lhs			setf lhaskell

" HEX (Intel)
au BufNewFile,BufRead *.hex,*.h32		setf hex

" HTML (.shtml for server side)
au BufNewFile,BufRead *.html,*.htm,*.shtml	setf html

" HTML with M4
au BufNewFile,BufRead *.html.m4			setf htmlm4

" Hyper Builder
au BufNewFile,BufRead *.hb			setf hb

" Icon
au BufNewFile,BufRead *.icn			setf icon

" IDL (Interface Description Language)
au BufNewFile,BufRead *.idl			setf idl

" IDL (Interactive Data Language)
au BufNewFile,BufRead *.pro			setf idlang

" Inform
au BufNewFile,BufRead *.inf,*.INF		setf inform

" Informix 4GL (source - canonical, include file, I4GL+M4 preproc.)
au BufNewFile,BufRead *.4gl,*.4gh,*.m4gl	setf fgl

" .INI file for MSDOS
au BufNewFile,BufRead *.ini			setf dosini

" Inno Setup
au BufNewFile,BufRead *.iss			setf iss

" Jam
au BufNewFile,BufRead *.jpl,*.jpr,Prl*.*,JAM*.*	setf jam

" Java
au BufNewFile,BufRead *.java,*.jav		setf java

" JavaCC
au BufNewFile,BufRead *.jj,*.jjt		setf javacc

" JavaScript
au BufNewFile,BufRead *.js,*.javascript		setf javascript

" Java Server Pages
au BufNewFile,BufRead *.jsp			setf jsp

" Java Properties resource file (note: doesn't catch font.properties.pl)
au BufNewFile,BufRead *.properties,*.properties_??,*.properties_??_??,*.properties_??_??_*	setf jproperties

" Jess
au BufNewFile,BufRead *.clp			setf jess

" Jgraph
au BufNewFile,BufRead *.jgr			setf jgraph

" Kixtart
au BufNewFile,BufRead *.kix			setf kix

" Kimwitu[++]
au BufNewFile,BufRead *.k			setf kwt

" KDE script
au BufNewFile,BufRead *.ks			setf kscript

" Lace (ISE)
au BufNewFile,BufRead *.ace,*.ACE		setf lace

" Latte
au BufNewFile,BufRead *.latte,*.lte		setf latte

" LambdaProlog (*.mod too, see Modsim)
au BufNewFile,BufRead *.sig			setf lprolog

" Lex
au BufNewFile,BufRead *.lex,*.l			setf lex

" Lilo: Linux loader
au BufNewFile,BufRead lilo.conf*		setf lilo

" Lisp (*.el = ELisp, *.cl = Common Lisp)
if has("fname_case")
  au BufNewFile,BufRead *.lsp,*.el,*.cl,*.L	setf lisp
else
  au BufNewFile,BufRead *.lsp,*.el,*.cl		setf lisp
endif

" Lite
au BufNewFile,BufRead *.lite,*.lt		setf lite

" LOTOS
au BufNewFile,BufRead *.lot,*.lotos		setf lotos

" Lout (also: *.lt)
au BufNewFile,BufRead *.lou,*.lout		setf lout

" Lua
au BufNewFile,BufRead *.lua			setf lua

" Lynx style file
au BufNewFile,BufRead *.lss			setf lss

" M4
au BufNewFile,BufRead *.m4
	\ if expand("<afile>") !~? 'html.m4$' | setf m4 | endif

" MaGic Point
au BufNewFile,BufRead *.mgp			setf mgp

" Mail (for Elm, trn, mutt, rn, slrn)
au BufNewFile,BufRead snd.\d\+,.letter,.letter.\d\+,.followup,.article,.article.\d\+,pico.\d\+,mutt-*-\d\+,mutt\w\{6\},ae\d\+.txt,/tmp/SLRN[0-9A-Z.]\+ setf mail

" Makefile (but not Automake)
au BufNewFile,BufRead [mM]akefile*
	\ if expand("<afile>") !~? 'Makefile.am\>' | setf make | endif
au BufNewFile,BufRead GNUmakefile,*.mk,*.mak,*.dsp setf make

" MakeIndex
au BufNewFile,BufRead *.ist,*.mst		setf ist

" Manpage
au BufNewFile,BufRead *.man			setf man

" Maple V
au BufNewFile,BufRead *.mv,*.mpl,*.mws		setf maple

" Mason
au BufNewFile,BufRead *.mason			setf mason

" Matlab
au BufNewFile,BufRead *.m			setf matlab

" Maya Extension Language
au BufNewFile,BufRead *.mel			setf mel

" Metafont
au BufNewFile,BufRead *.mf			setf mf

" MetaPost
au BufNewFile,BufRead *.mp			setf mp

" Modsim III (or LambdaProlog)
au BufNewFile,BufRead *.mod
	\ if getline(1) =~ '\<module\>' |
	\   setf lprolog |
	\ else |
	\   setf modsim3 |
	\ endif

" Modula 2
au BufNewFile,BufRead *.m2,*.DEF,*.MOD,*.md,*.mi setf modula2

" Modula 3 (.m3, .i3, .mg, .ig)
au BufNewFile,BufRead *.[mi][3g]		setf modula3

" Moterola S record
au BufNewFile,BufRead *.s19,*.s28,*.s37		setf srec

" Msql
au BufNewFile,BufRead *.msql			setf msql

" M$ Resource files
au BufNewFile,BufRead *.rc			setf rc

" Mutt setup file
au BufNewFile,BufRead .muttrc*,Muttrc		setf muttrc

" Nastran input/DMAP
"au BufNewFile,BufRead *.dat			setf nastran

" Novell netware batch files
au BufNewFile,BufRead *.ncf			setf ncf

" Nroff/Troff (*.ms is checked below)
au BufNewFile,BufRead *.me,*.mm,*.tr,*.nr	setf nroff
au BufNewFile,BufRead *.[1-9]			call <SID>FTnroff()

" This function checks if one of the first five lines start with a dot.  In
" that case it is probably an nroff file: 'filetype' is set and 1 is returned.
fun! <SID>FTnroff()
  if getline(1)[0] . getline(2)[0] . getline(3)[0] . getline(4)[0] . getline(5)[0] =~ '\.'
    setf nroff
    return 1
  endif
  return 0
endfun

" OCAML
au BufNewFile,BufRead *.ml,*.mli,*.mll,*.mly	setf ocaml

" Omnimark
au BufNewFile,BufRead *.xom,*.xin		setf omnimark

" OpenROAD
au BufNewFile,BufRead *.or			setf openroad

" OPL
au BufNewFile,BufRead *.[Oo][Pp][Ll]		setf opl

" Oracle config file
au BufNewFile,BufRead *.ora			setf ora

" PApp
au BufNewFile,BufRead *.papp,*.pxml,*.pxsl	setf papp

" Pascal
au BufNewFile,BufRead *.p,*.pas			setf pascal

" Delphi project file
au BufNewFile,BufRead *.dpr			setf pascal

" Perl
if has("fname_case")
  au BufNewFile,BufRead *.pl,*.PL		setf perl
else
  au BufNewFile,BufRead *.pl			setf perl
endif

" Perl, XPM or XPM2
au BufNewFile,BufRead *.pm
	\ if getline(1) =~ "XPM2" |
	\   setf xpm2 |
	\ elseif getline(1) =~ "XPM" |
	\   setf xpm |
	\ else |
	\   setf perl |
	\ endif

" Perl POD
au BufNewFile,BufRead *.pod			setf pod

" Php3
au BufNewFile,BufRead *.php,*.php3		setf php

" Phtml
au BufNewFile,BufRead *.phtml			setf phtml

" Pike
au BufNewFile,BufRead *.pike,*.lpc,*.ulpc,*.pmod setf pike

" Pine config
au BufNewFile,BufRead .pinerc,pinerc		setf pine

" PL/SQL
au BufNewFile,BufRead *.pls,*.plsql		setf plsql

" PO (GNU gettext)
au BufNewFile,BufRead *.po			setf po

" PostScript
au BufNewFile,BufRead *.ps,*.eps		setf postscr

" Povray
au BufNewFile,BufRead *.pov,*.inc		setf pov

" Printcap and Termcap
au BufNewFile,BufRead *printcap
	\ let b:ptcap_type = "print" | setf ptcap
au BufNewFile,BufRead *termcap
	\ let b:ptcap_type = "term" | setf ptcap

" PCCTS / ANTRL
"au BufNewFile,BufRead *.g			setf antrl
au BufNewFile,BufRead *.g			setf pccts

" PPWizard
au BufNewFile,BufRead *.it,*.ih			setf ppwiz

" Procmail
au BufNewFile,BufRead .procmail,.procmailrc	setf procmail

" Software Distributor Product Specification File (POSIX 1387.2-1995)
au BufNewFile,BufRead *.psf			setf psf
au BufNewFile,BufRead INDEX,INFO
	\ if getline(1) =~ '^\s*\(distribution\|installed_software\|root\|bundle\|product\)\s*$' |
	\   setf psf |
	\ endif

" Prolog
au BufNewFile,BufRead *.pdb			setf prolog

" Python
au BufNewFile,BufRead *.py			setf python

" Radiance
au BufNewFile,BufRead *.rad,*.mat		setf radiance

" Registry for MS-Windows
au BufNewFile,BufRead *.reg
	\ if getline(1) =~? '^REGEDIT[0-9]*\s*$' | setf registry | endif

" Rexx
au BufNewFile,BufRead *.rexx,*.rex		setf rexx

" Rexx or Rebol
au BufNewFile,BufRead *.r
	\ if getline(1) =~ '^REBOL' | setf rebol | else | setf rexx | endif

" Remind
au BufNewFile,BufRead .reminders*		setf remind

" Robots.txt
au BufNewFile,BufRead robots.txt		setf robots

" Rpcgen
au BufNewFile,BufRead *.x			setf rpcgen

" RTF
au BufNewFile,BufRead *.rtf			setf rtf

" Ruby
au BufNewFile,BufRead *.rb			setf ruby

" S-lang (or shader language!)
au BufNewFile,BufRead *.sl			setf slang

" Samba config
au BufNewFile,BufRead smb.conf			setf samba

" SAS script
au BufNewFile,BufRead *.sas			setf sas

" Sather
au BufNewFile,BufRead *.sa			setf sather

" SDL
au BufNewFile,BufRead *.sdl,*.pr		setf sdl

" sed
au BufNewFile,BufRead *.sed			setf sed

" Sendmail
au BufNewFile,BufRead sendmail.cf		setf sm

" SGML
au BufNewFile,BufRead *.sgm,*.sgml
	\ if getline(1).getline(2).getline(3).getline(4).getline(5) =~? 'linuxdoc' |
	\   setf sgmllnx |
	\ elseif getline(1) =~ '<!DOCTYPE.*DocBook' || getline(2) =~ '<!DOCTYPE.*DocBook' |
	\   let b:docbk_type="sgml" |
	\   setf docbk |
	\ else |
	\   setf sgml |
	\ endif
au BufNewFile,BufRead *.ent			setf sgml

" SGML catalog file
au BufNewFile,BufRead sgml.catalog*,catalog	setf catalog

" Shell scripts (sh, ksh, bash, bash2, csh); Allow .profile_foo etc.
au BufNewFile,BufRead .bashrc*,bashrc,bash.bashrc,.bash_profile*,*.bash call SetFileTypeSH("bash")
au BufNewFile,BufRead .kshrc*,*.ksh call SetFileTypeSH("ksh")
au BufNewFile,BufRead /etc/profile,.profile*,*.sh,*.env call SetFileTypeSH(getline(1))
au BufNewFile,BufRead .login*,.cshrc*,csh.cshrc,csh.login,csh.logout,.tcshrc*,*.csh,*.tcsh,.alias setf csh

fun! SetFileTypeSH(name)
  if a:name =~ '\<ksh\>'
    let b:is_kornshell = 1
    if exists("b:is_bash")
      unlet b:is_bash
    endif
  elseif exists("g:bash_is_sh") || a:name =~ '\<bash\>' || a:name =~ '\<bash2\>'
    let b:is_bash = 1
    if exists("b:is_kornshell")
      unlet b:is_kornshell
    endif
  endif
  setf sh
endfun

" Z-Shell script
au BufNewFile,BufRead .zsh*,.zlog*,.zprofile,.zfbfmarks,.zcompdump*,zsh*,zlog*  setf zsh

" Scheme
au BufNewFile,BufRead *.scm			setf scheme

" Simula
au BufNewFile,BufRead *.sim			setf simula

" SKILL
au BufNewFile,BufRead *.il			setf skill

" SLRN
au BufNewFile,BufRead .slrnrc			setf slrnrc
au BufNewFile,BufRead *.score			setf slrnsc

" Smalltalk
au BufNewFile,BufRead *.st,*.cls		setf st

" SMIL or XML
au BufNewFile,BufReadPost *.smil
	\ if getline(1) =~ '<?\s*xml.*?>' |
	\   setf xml |
	\ else |
	\   setf smil |
	\ endif

" SMIL or SNMP MIB file
au BufNewFile,BufRead *.smi
	\ if getline(1) =~ '\<smil\>' |
	\   setf smil |
	\ else |
	\   setf mib |
	\ endif

" SMITH
au BufNewFile,BufRead *.smt,*.smith		setf smith

" Snobol4
au BufNewFile,BufRead *.sno			setf snobol4

" SNMP MIB files
au BufNewFile,BufReadPost *.mib			setf mib

" Spec (Linux RPM)
au BufNewFile,BufRead *.spec			setf spec

" Speedup (AspenTech plant simulator)
au BufNewFile,BufRead *.speedup,*.spdata,*.spd	setf spup

" Spice
au BufNewFile,BufRead *.sp,*.spice		setf spice

" Squid
au BufNewFile,BufRead squid.conf		setf squid

" SQL (all but the first one for Oracle Designer)
au BufNewFile,BufRead *.sql,*.tyb,*.typ,*.tyc,*.pkb,*.pks	setf sql

" SQR
au BufNewFile,BufRead *.sqr,*.sqi		setf sqr

" Stored Procedures
au BufNewFile,BufRead *.stp			setf stp

" Standard ML
au BufNewFile,BufRead *.sml			setf sml

" Tads
au BufNewFile,BufRead *.t			setf tads

" Tags
au BufNewFile,BufRead tags			setf tags

" Tcl
au BufNewFile,BufRead *.tcl,*.tk,*.itcl,*.itk	setf tcl

" TealInfo
au BufNewFile,BufRead *.tli			setf tli

" Telix Salt
au BufNewFile,BufRead *.slt			setf tsalt

" TeX
au BufNewFile,BufRead *.tex,*.sty,*.dtx,*.ltx	setf tex

" Texinfo
au BufNewFile,BufRead *.texinfo,*.texi,*.txi	setf texinfo

" TF mud client
au BufNewFile,BufRead *.tf			setf tf

" Motif UIT/UIL files
au BufNewFile,BufRead *.uit,*.uil		setf uil

" Verilog HDL
au BufNewFile,BufRead *.v			setf verilog

" VHDL
au BufNewFile,BufRead *.hdl,*.vhd,*.vhdl,*.vhdl_[0-9]*,*.vbe,*.vst  setf vhdl

" Vim script
au BufNewFile,BufRead *vimrc*,*.vim,.exrc,_exrc setf vim

" Viminfo file
au BufNewFile,BufRead .viminfo,_viminfo		setf viminfo

" Virata Config Script File
au BufRead,BufNewFile *.hw,*.module,*.pkg	setf virata

" Visual Basic (also uses *.bas) or FORM
au BufNewFile,BufRead *.frm			call <SID>FTVB("form")

" Vgrindefs file
au BufNewFile,BufRead vgrindefs			setf vgrindefs

" VRML V1.0c
au BufNewFile,BufRead *.wrl			setf vrml

" Webmacro
au BufNewFile,BufRead *.wm			setf webmacro

" Website MetaLanguage
au BufNewFile,BufRead *.wml			setf wml

" Winbatch
au BufNewFile,BufRead *.wbt			setf winbatch

" CVS commit file
au BufNewFile,BufRead cvs\d\+			setf cvs

" CWEB
au BufNewFile,BufRead *.w			setf cweb

" WEB (*.web is also used for Winbatch: Guess, based on expecting "%" comment
" lines in a WEB file).
au BufNewFile,BufRead *.web
	\ if getline(1)[0].getline(2)[0].getline(3)[0].getline(4)[0].getline(5)[0] =~ "%" |
	\   setf web |
	\ else |
	\   setf winbatch |
	\ endif

" Windows Scripting Host
au BufNewFile,BufRead *.wsf			setf wsh

" X Pixmap (dynamically sets colors, use BufEnter to make it work better)
au BufEnter *.xpm
	\ if getline(1) =~ "XPM2" |
	\   setf xpm2 |
	\ else |
	\   setf xpm |
	\ endif
au BufEnter *.xpm2				setf xpm2

" XS Perl extension interface language
au BufEnter *.xs				setf xs

" X resources file
au BufNewFile,BufRead .Xdefaults,.Xresources,Xresources*,*/app-defaults/*,*/Xresources/*,xdm-config setf xdefaults

" Xmath
au BufNewFile,BufRead *.msc,*.msf		setf xmath
au BufNewFile,BufRead *.ms
	\ if !<SID>FTnroff() | setf xmath | endif

" XML
au BufNewFile,BufRead *.xsl			setf xml
au BufNewFile,BufRead *.xml
	\ if getline(1) =~ '<!DOCTYPE.*DocBook' || getline(2) =~ '<!DOCTYPE.*DocBook' |
	\   let b:docbk_type="xml" |
	\   setf docbk |
	\ else |
	\   setf xml |
	\ endif

" Yacc
au BufNewFile,BufRead *.y			setf yacc

" Z80 assembler asz80
au BufNewFile,BufRead *.z8a			setf z8a

augroup END


" Source the user-specified filetype file, for backwards compatibility with
" Vim 5.x.
if exists("myfiletypefile") && file_readable(expand(myfiletypefile))
  execute "source " . myfiletypefile
endif


" Check for "*" after loading myfiletypefile, so that scripts.vim is only used
" when there are no matching file name extensions.
augroup filetypedetect
au BufNewFile,BufRead,StdinReadPost *
	\ if !did_filetype() | runtime! scripts.vim | endif

" Extra checks for when no filetype has been detected now

" Printcap and Termcap
au BufNewFile,BufRead *printcap*
	\ if !did_filetype() | let b:ptcap_type = "print" | setf ptcap | endif
au BufNewFile,BufRead *termcap*
	\ if !did_filetype() | let b:ptcap_type = "term" | setf ptcap | endif

" Crontab
au BufNewFile,BufRead crontab.*
	\ if !did_filetype()|setf crontab|endif

augroup END


" If the GUI is already running, may still need to install the Syntax menu.
" Don't do it when the 'M' flag is included in 'guioptions'.
if has("menu") && has("gui_running") && !exists("did_install_syntax_menu") && &guioptions !~# "M"
  source <sfile>:p:h/menu.vim
endif

" Restore 'cpoptions'
let &cpo = s:cpo_save
unlet s:cpo_save
