" Script to define the syntax menu in menu.vim
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 Sep 24

" This is used by "make menu" in the src directory.
edit <sfile>:p:h/menu.vim

/The Start Of The Syntax Menu/+1,/The End Of The Syntax Menu/-1d
let s:lnum = line(".") - 1
call append(s:lnum, "")
let s:lnum = s:lnum + 1

" Use the SynMenu command and function to define all menu entries
command! -nargs=* SynMenu call <SID>Syn(<q-args>)

let s:cur_menu_name = ""
let s:cur_menu_nr = 0
let s:cur_menu_item = 0
let s:cur_menu_char = ""

fun! <SID>Syn(arg)
  " isolate menu name: until the first dot
  let i = match(a:arg, '\.')
  let menu_name = strpart(a:arg, 0, i)
  let r = strpart(a:arg, i + 1, 999)
  " isolate submenu name: until the colon
  let i = match(r, ":")
  let submenu_name = strpart(r, 0, i)
  " after the colon is the syntax name
  let syntax_name = strpart(r, i + 1, 999)

  if s:cur_menu_name != menu_name
    let s:cur_menu_name = menu_name
    let s:cur_menu_nr = s:cur_menu_nr + 10
    let s:cur_menu_item = 100
    let s:cur_menu_char = submenu_name[0]
  else
    " When starting a new letter, insert a menu separator.
    let c = submenu_name[0]
    if c != s:cur_menu_char
      exe 'am 50.' . s:cur_menu_nr . '.' . s:cur_menu_item . ' &Syntax.' . menu_name . ".-" . c . '- <nul>'
      let s:cur_menu_item = s:cur_menu_item + 10
      let s:cur_menu_char = c
    endif
  endif
  call append(s:lnum, 'am 50.' . s:cur_menu_nr . '.' . s:cur_menu_item . ' &Syntax.' . menu_name . "." . submenu_name . ' :cal SetSyn("' . syntax_name . '")<CR>')
  let s:cur_menu_item = s:cur_menu_item + 10
  let s:lnum = s:lnum + 1
endfun

SynMenu AB.Abaqus:abaqus
SynMenu AB.ABC:abc
SynMenu AB.ABEL:abel
SynMenu AB.Ada:ada
SynMenu AB.Aflex:aflex
SynMenu AB.AHDL:ahdl
SynMenu AB.Amiga\ DOS:amiga
SynMenu AB.Antlr:antlr
SynMenu AB.Apache\ config:apache
SynMenu AB.Apache-style\ config:apachestyle
SynMenu AB.Applix\ ELF:elf
SynMenu AB.Arc\ Macro\ Language:aml
SynMenu AB.ASP\ with\ VBSages:aspvbs
SynMenu AB.ASP\ with\ Perl:aspperl
SynMenu AB.Assembly.680x0:asm68k
SynMenu AB.Assembly.GNU:asm
SynMenu AB.Assembly.H8300:asmh8300
SynMenu AB.Assembly.Intel\ Itanium:ia64
SynMenu AB.Assembly.Microsoft:masm
SynMenu AB.Assembly.Netwide:nasm
SynMenu AB.Assembly.PIC:pic
SynMenu AB.Assembly.Turbo:tasm
SynMenu AB.Assembly.Z-80:z8a
SynMenu AB.ASN\.1:asn
SynMenu AB.Atlas:atlas
SynMenu AB.Automake:automake
SynMenu AB.Avenue:ave
SynMenu AB.Awk:awk
SynMenu AB.Ayacc:ayacc
SynMenu AB.B:b
SynMenu AB.BASIC:basic
SynMenu AB.BC\ calculator:bc
SynMenu AB.BDF\ font:bdf
SynMenu AB.BibFile:bib
SynMenu AB.BIND\ configuration:named
SynMenu AB.BIND\ zone:bindzone
SynMenu AB.Blank:blank

SynMenu CD.C:c
SynMenu CD.C++:cpp
SynMenu CD.Crontab:crontab
SynMenu CD.Cyn++:cynpp
SynMenu CD.Cynlib:cynlib
SynMenu CD.Cascading\ Style\ Sheets:css
SynMenu CD.Century\ Term:cterm
SynMenu CD.CFG:cfg
SynMenu CD.CHILL:ch
SynMenu CD.Change:change
SynMenu CD.ChangeLog:changelog
SynMenu CD.Clean:clean
SynMenu CD.Clever:cl
SynMenu CD.Clipper:clipper
SynMenu CD.Cold\ Fusion:cf
SynMenu CD.Configure\ script:config
SynMenu CD.Csh\ shell\ script:csh
SynMenu CD.Ctrl-H:ctrlh
SynMenu CD.Cobol:cobol
SynMenu CD.CSP:csp
SynMenu CD.CUPL.CUPL:cupl
SynMenu CD.CUPL.simulation:cuplsim
SynMenu CD.CVS\ commit:cvs
SynMenu CD.CWEB:cweb
SynMenu CD.Debian.Debian\ ChangeLog:debchangelog
SynMenu CD.Debian.Debian\ Control:debcontrol
SynMenu CD.Diff:diff
SynMenu CD.Digital\ Command\ Lang:dcl
SynMenu CD.Diva\ (with\ SKILL):diva
SynMenu CD.DNS:dns
SynMenu CD.DOT:dto
SynMenu CD.Dracula:dracula
SynMenu CD.DSSSL:dsl
SynMenu CD.DTD:dtd
SynMenu CD.DTML\ (Zope):dtml
SynMenu CD.Dylan.Dylan:dylan
SynMenu CD.Dylan.Dylan\ intr:dylanintr
SynMenu CD.Dylan.Dylan\ lid:dylanlid

SynMenu EFG.Eiffel:eiffel
SynMenu EFG.Elm\ Filter:elmfilt
SynMenu EFG.Embedix\ Component\ Description:ecd
SynMenu EFG.ERicsson\ LANGuage:erlang
SynMenu EFG.ESQL-C:esqlc
SynMenu EFG.Eterm\ config:eterm
SynMenu EFG.Expect:expect
SynMenu EFG.Exports:exports
SynMenu EFG.Focus\ Executable:focexec
SynMenu EFG.Focus\ Master:master
SynMenu EFG.FORM:form
SynMenu EFG.Forth:forth
SynMenu EFG.Fortran:fortran
SynMenu EFG.FoxPro:foxpro
SynMenu EFG.Fvwm\ configuration:fvwm1
SynMenu EFG.Fvwm2\ configuration:fvwm2
SynMenu EFG.GDB\ command\ file:gdb
SynMenu EFG.GDMO:gdmo
SynMenu EFG.Gedcom:gedcom
SynMenu EFG.GP:gp
SynMenu EFG.GNU\ Server\ Pages:gsp
SynMenu EFG.GNUplot:gnuplot
SynMenu EFG.GTKrc:gtkrc

SynMenu HIJK.Haskell:haskell
SynMenu HIJK.Haskell-literate:lhaskell
SynMenu HIJK.Hercules:hercules
SynMenu HIJK.HTML:html
SynMenu HIJK.HTML\ with\ M4:htmlm4
SynMenu HIJK.HTML/OS:htmlos
SynMenu HIJK.Hyper\ Builder:hb
SynMenu HIJK.Icon:icon
SynMenu HIJK.IDL:idl
SynMenu HIJK.Indent\ profile:indent
SynMenu HIJK.Inform:inform
SynMenu HIJK.Informix\ 4GL:fgl
SynMenu HIJK.Inittab:inittab
SynMenu HIJK.Inno\ Setup:iss
SynMenu HIJK.InstallShield\ Rules:ishd
SynMenu HIJK.Interactive\ Data\ Lang:idlang
SynMenu HIJK.Jam:jam
SynMenu HIJK.Jargon:jargon
SynMenu HIJK.Java.Java:java
SynMenu HIJK.Java.JavaCC:javacc
SynMenu HIJK.Java.Java\ Server\ Pages:jsp
SynMenu HIJK.Java.Java\ Properties:jproperties
SynMenu HIJK.JavaScript:javascript
SynMenu HIJK.Jess:jess
SynMenu HIJK.Jgraph:jgraph
SynMenu HIJK.KDE\ script:kscript
SynMenu HIJK.Kimwitu:kwt
SynMenu HIJK.Kixtart:kix

SynMenu L-Ma.Lace:lace
SynMenu L-Ma.Lamda\ Prolog:lprolog
SynMenu L-Ma.Latte:latte
SynMenu L-Ma.Lex:lex
SynMenu L-Ma.LFTP:lftp
SynMenu L-Ma.Lilo:lilo
SynMenu L-Ma.Lisp:lisp
SynMenu L-Ma.Lite:lite
SynMenu L-Ma.LOTOS:lotos
SynMenu L-Ma.Lout:lout
SynMenu L-Ma.Lua:lua
SynMenu L-Ma.Lynx\ Style:lss
SynMenu L-Ma.Lynx\ config:lynx
SynMenu L-Ma.M4:m4
SynMenu L-Ma.MaGic\ Point:mgp
SynMenu L-Ma.Mail:mail
SynMenu L-Ma.Makefile:make
SynMenu L-Ma.MakeIndex:ist
SynMenu L-Ma.Man\ page:man
SynMenu L-Ma.Maple:maple
SynMenu L-Ma.Mason:mason
SynMenu L-Ma.Mathematica:mma
SynMenu L-Ma.Matlab:matlab

SynMenu Me-NO.MEL\ (for\ Maya):mel
SynMenu Me-NO.Metafont:mf
SynMenu Me-NO.MetaPost:mp
SynMenu Me-NO.MS\ Module\ Definition:def
SynMenu Me-NO.Model:model
SynMenu Me-NO.Modsim\ III:modsim3
SynMenu Me-NO.Modula\ 2:modula2
SynMenu Me-NO.Modula\ 3:modula3
SynMenu Me-NO.Msql:msql
SynMenu Me-NO.MS-DOS.MS-DOS\ \.bat\ file:dosbatch
SynMenu Me-NO.MS-DOS.4DOS\ \.bat\ file:btm
SynMenu Me-NO.MS-DOS.MS-DOS\ \.ini\ file:dosini
SynMenu Me-NO.MS\ Resource\ file:rc
SynMenu Me-NO.Mush:mush
SynMenu Me-NO.Muttrc:muttrc
SynMenu Me-NO.Nastran\ input/DMAP:nastran
SynMenu Me-NO.Natural:natural
SynMenu Me-NO.Novell\ batch:ncf
SynMenu Me-NO.Not\ Quite\ C:nqc
SynMenu Me-NO.Nroff:nroff
SynMenu Me-NO.Objective\ C:objc
SynMenu Me-NO.OCAML:ocaml
SynMenu Me-NO.Omnimark:omnimark
SynMenu Me-NO.OpenROAD:openroad
SynMenu Me-NO.Open\ Psion\ Lang:opl
SynMenu Me-NO.Oracle\ config:ora

SynMenu PQ.Palm\ resource\ compiler:pilrc
SynMenu PQ.PApp:papp
SynMenu PQ.Pascal:pascal
SynMenu PQ.PCCTS:pccts
SynMenu PQ.PPWizard:ppwiz
SynMenu PQ.Perl.Perl:perl
SynMenu PQ.Perl.Perl\ POD:pod
SynMenu PQ.Perl.Perl\ XS:xs
SynMenu PQ.PHP\ 3-4:php
SynMenu PQ.Phtml:phtml
SynMenu PQ.Pike:pike
SynMenu PQ.Pine\ RC:pine
SynMenu PQ.PL/M:plm
SynMenu PQ.PL/SQL:plsql
SynMenu PQ.PO\ (GNU\ gettext):po
SynMenu PQ.Postfix\ main\ config:pfmain
SynMenu PQ.PostScript:postscr
SynMenu PQ.Povray:pov
SynMenu PQ.Povray\ configuration:povini
SynMenu PQ.Printcap:pcap
SynMenu PQ.Procmail:procmail
SynMenu PQ.Product\ Spec\ File:psf
SynMenu PQ.Progress:progress
SynMenu PQ.Prolog:prolog
SynMenu PQ.Purify\ log:purifylog
SynMenu PQ.Python:python

SynMenu R-Sg.R:r
SynMenu R-Sg.Radiance:radiance
SynMenu R-Sg.Ratpoison:ratpoison
SynMenu R-Sg.Readline\ config:readline
SynMenu R-Sg.RCS\ log\ output:rcslog
SynMenu R-Sg.Rebol:rebol
SynMenu R-Sg.Registry\ of\ MS-Windows:registry
SynMenu R-Sg.Remind:remind
SynMenu R-Sg.Renderman\ Shader\ Lang:sl
SynMenu R-Sg.Rexx:rexx
SynMenu R-Sg.Robots\.txt:robots
SynMenu R-Sg.Rpcgen:rpcgen
SynMenu R-Sg.RTF:rtf
SynMenu R-Sg.Ruby:ruby
SynMenu R-Sg.S-lang:slang
SynMenu R-Sg.Samba\ config:samba
SynMenu R-Sg.SAS:sas
SynMenu R-Sg.Sather:sather
SynMenu R-Sg.Scheme:scheme
SynMenu R-Sg.Screen\ RC:screen
SynMenu R-Sg.SDL:sdl
SynMenu R-Sg.Sed:sed
SynMenu R-Sg.Sendmail\.cf:sm
SynMenu R-Sg.SGML.SGML\ catalog:catalog
SynMenu R-Sg.SGML.SGML\ DTD:sgml
SynMenu R-Sg.SGML.SGML\ Declarations:sgmldecl
SynMenu R-Sg.SGML.SGML\ linuxdoc:sgmllnx

SynMenu Sh-S.Sh\ shell\ script:sh
SynMenu Sh-S.SiCAD:sicad
SynMenu Sh-S.Simula:simula
SynMenu Sh-S.Sinda.Sinda\ compare:sindacmp
SynMenu Sh-S.Sinda.Sinda\ input:sinda
SynMenu Sh-S.Sinda.Sinda\ output:sindaout
SynMenu Sh-S.SKILL:skill
SynMenu Sh-S.SLRN.SLRN\ rc:slrnrc
SynMenu Sh-S.SLRN.SLRN\ score:slrnsc
SynMenu Sh-S.SmallTalk:st
SynMenu Sh-S.SMIL:smil
SynMenu Sh-S.SMITH:smith
SynMenu Sh-S.SNMP\ MIB:mib
SynMenu Sh-S.SNNS.SNNS\ network:snnsnet
SynMenu Sh-S.SNNS.SNNS\ pattern:snnspat
SynMenu Sh-S.SNNS.SNNS\ result:snnsres
SynMenu Sh-S.Snobol4:snobol4
SynMenu Sh-S.Snort\ Configuration:hog
SynMenu Sh-S.SPEC\ (Linux\ RPM):spec
SynMenu Sh-S.Specman:specman
SynMenu Sh-S.Spice:spice
SynMenu Sh-S.Speedup:spup
SynMenu Sh-S.Squid:squid
SynMenu Sh-S.SQL:sql
SynMenu Sh-S.SQL\ Forms:sqlforms
SynMenu Sh-S.SQR:sqr
SynMenu Sh-S.Standard\ ML:sml
SynMenu Sh-S.Stored\ Procedures:stp
SynMenu Sh-S.Strace:strace

SynMenu TUV.Tads:tads
SynMenu TUV.Tags:tags
SynMenu TUV.TAK.TAK\ compare:tak
SynMenu TUV.TAK.TAK\ input:tak
SynMenu TUV.TAK.TAK\ output:takout
SynMenu TUV.Tcl/Tk:tcl
SynMenu TUV.TealInfo:tli
SynMenu TUV.Telix\ Salt:tsalt
SynMenu TUV.Termcap:ptcap
SynMenu TUV.Terminfo:terminfo
SynMenu TUV.TeX:tex
SynMenu TUV.TeX\ configuration:texmf
SynMenu TUV.Texinfo:texinfo
SynMenu TUV.TF\ mud\ client:tf
SynMenu TUV.Tidy\ configuration:tidy
SynMenu TUV.Trasys\ input:trasys
SynMenu TUV.TSS.Command\ Line:tsscl
SynMenu TUV.TSS.Geometry:tssgm
SynMenu TUV.TSS.Optics:tssop
SynMenu TUV.UIT/UIL:uil
SynMenu TUV.UnrealScript:uc
SynMenu TUV.Verilog\ HDL:verilog
SynMenu TUV.Vgrindefs:vgrindefs
SynMenu TUV.VHDL:vhdl
SynMenu TUV.Vim.Vim\ help\ file:help
SynMenu TUV.Vim.Vim\ script:vim
SynMenu TUV.Vim.Viminfo\ file:viminfo
SynMenu TUV.Virata:virata
SynMenu TUV.Visual\ Basic:vb
SynMenu TUV.VRML:vrml
SynMenu TUV.VSE\ JCL:vsejcl

SynMenu WXYZ.WEB:web
SynMenu WXYZ.Webmacro:webmacro
SynMenu WXYZ.Website\ MetaLanguage:wml
SynMenu WXYZ.Wdiff:wdiff
SynMenu WXYZ.Wget\ config:wget
SynMenu WXYZ.Whitespace\ (add):whitespace
SynMenu WXYZ.WinBatch/Webbatch:winbatch
SynMenu WXYZ.Windows\ Scripting\ Host:wsh
SynMenu WXYZ.X\ Keyboard\ Extension:xkb
SynMenu WXYZ.X\ Pixmap:xpm
SynMenu WXYZ.X\ Pixmap\ (2):xpm2
SynMenu WXYZ.X\ resources:xdefaults
SynMenu WXYZ.Xmath:xmath
SynMenu WXYZ.XML:xml
SynMenu WXYZ.XXD\ hex\ dump:xxd
SynMenu WXYZ.Yacc:yacc
SynMenu WXYZ.Zsh\ shell\ script:zsh

call append(s:lnum, "")

wq
