" Script to define the syntax menu in menu.vim
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 May 26

" This is used by "make menu" in the src directory.
edit <sfile>:p:h/menu.vim

/The Start Of The Syntax Menu/+1,/The End Of The Syntax Menu/-1d
let menu_lnum = line(".") - 1
call append(menu_lnum, "")
let menu_lnum = menu_lnum + 1


" Use the SynMenu command and function to define all menu entries
command -nargs=* SynMenu call SynMenu(<q-args>)

let menu__name = ""
let menu__nr = 0
let menu__item = 0
let menu__char = ""

fun SynMenu(arg)
  " isolate menu name: until the first dot
  let i = match(a:arg, '\.')
  let menu_name = strpart(a:arg, 0, i)
  let r = strpart(a:arg, i + 1, 999)
  " isolate submenu name: until the colon
  let i = match(r, ":")
  let submenu_name = strpart(r, 0, i)
  " after the colon is the syntax name
  let syntax_name = strpart(r, i + 1, 999)

  if g:menu__name != menu_name
    let g:menu__name = menu_name
    let g:menu__nr = g:menu__nr + 10
    let g:menu__item = 100
    let g:menu__char = submenu_name[0]
  else
    " When starting a new letter, insert a menu separator.
    " Make an exception for "4DOS", which is after "MS-DOS".
    let c = submenu_name[0]
    if c != g:menu__char && c != "4"
      exe 'am 50.' . g:menu__nr . '.' . g:menu__item . ' &Syntax.' . menu_name . ".-" . c . '- <nul>'
      let g:menu__item = g:menu__item + 10
      let g:menu__char = c
    endif
  endif
  call append(g:menu_lnum, 'am 50.' . g:menu__nr . '.' . g:menu__item . ' &Syntax.' . menu_name . "." . submenu_name . ' :cal SetSyn("' . syntax_name . '")<CR>')
  let g:menu__item = g:menu__item + 10
  let g:menu_lnum = g:menu_lnum + 1
endfun

SynMenu AB.Abaqus:abaqus
SynMenu AB.ABC:abc
SynMenu AB.ABEL:abel
SynMenu AB.AceDB:acedb
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
SynMenu AB.ASN\.1:asn
SynMenu AB.Atlas:atlas
SynMenu AB.Automake:automake
SynMenu AB.Avenue:ave
SynMenu AB.Awk:awk
SynMenu AB.Ayacc:ayacc
SynMenu AB.B:b
SynMenu AB.BASIC:basic
SynMenu AB.BC\ calculator:bc
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
SynMenu CD.CHILL:ch
SynMenu CD.Change:change
SynMenu CD.ChangeLog:changelog
SynMenu CD.Clean:clean
SynMenu CD.Clever:cl
SynMenu CD.Clipper:clipper
SynMenu CD.Cold\ Fusion:cf
SynMenu CD.Configure\ script:config
SynMenu CD.Configure\ file:cfg
SynMenu CD.Csh\ shell\ script:csh
SynMenu CD.Ctrl-H:ctrlh
SynMenu CD.Cobol:cobol
SynMenu CD.CSP:csp
SynMenu CD.CUPL:cupl
SynMenu CD.CUPL\ simulation:cuplsim
SynMenu CD.CVS\ commit:cvs
SynMenu CD.CWEB:cweb
SynMenu CD.Diff:diff
SynMenu CD.Digital\ Command\ Lang:dcl
SynMenu CD.Diva\ (with\ SKILL):diva
SynMenu CD.DNS:dns
SynMenu CD.Dracula:dracula
SynMenu CD.DTD:dtd
SynMenu CD.Zope\ DTML:dtml
SynMenu CD.Debian\ Changelog:debchangelog
SynMenu CD.Debian\ Control:debcontrol
SynMenu CD.Dylan:dylan
SynMenu CD.Dylan\ intr:dylanintr
SynMenu CD.Dylan\ lid:dylanlid

SynMenu EFGH.Eiffel:eiffel
SynMenu EFGH.Elm\ Filter:elmfilt
SynMenu EFGH.Embedix\ Component\ Description:ecd
SynMenu EFGH.ERicsson\ LANGuage:erlang
SynMenu EFGH.ESQL-C:esqlc
SynMenu EFGH.Essbase\ script:csc
SynMenu EFGH.Expect:expect
SynMenu EFGH.Exports:exports
SynMenu EFGH.Focus\ Executable:focexec
SynMenu EFGH.Focus\ Master:master
SynMenu EFGH.FORM:form
SynMenu EFGH.Forth:forth
SynMenu EFGH.Fortran:fortran
SynMenu EFGH.FoxPro:foxpro
SynMenu EFGH.Fvwm\ configuration:fvwm1
SynMenu EFGH.Fvwm2\ configuration:fvwm2
SynMenu EFGH.GDB\ command\ file:gdb
SynMenu EFGH.GDMO:gdmo
SynMenu EFGH.Gedcom:gedcom
SynMenu EFGH.GP:gp
SynMenu EFGH.GNU\ Server\ Pages:gsp
SynMenu EFGH.GNUplot:gnuplot
SynMenu EFGH.GTKrc:gtkrc
SynMenu EFGH.Haskell:haskell
SynMenu EFGH.Haskell-literal:lhaskell
SynMenu EFGH.Hercules:hercules
SynMenu EFGH.HTML:html
SynMenu EFGH.HTML\ with\ M4:htmlm4
SynMenu EFGH.HTML/OS:htmlos
SynMenu EFGH.Hyper\ Builder:hb

SynMenu IJKL.Icon:icon
SynMenu IJKL.IDL:idl
SynMenu IJKL.Interactive\ Data\ Lang:idlang
SynMenu IJKL.Inform:inform
SynMenu IJKL.Informix\ 4GL:fgl
SynMenu IJKL.Inittab:inittab
SynMenu IJKL.Inno\ Setup:iss
SynMenu IJKL.InstallShield\ Rules:ishd
SynMenu IJKL.Jam:jam
SynMenu IJKL.Java:java
SynMenu IJKL.JavaCC:javacc
SynMenu IJKL.JavaScript:javascript
SynMenu IJKL.Java\ Server\ Pages:jsp
SynMenu IJKL.Java\ Properties:jproperties
SynMenu IJKL.Jess:jess
SynMenu IJKL.Jgraph:jgraph
SynMenu IJKL.KDE\ script:kscript
SynMenu IJKL.Kimwitu:kwt
SynMenu IJKL.Kixtart:kix
SynMenu IJKL.Lace:lace
SynMenu IJKL.Lamda\ Prolog:lprolog
SynMenu IJKL.Latte:latte
SynMenu IJKL.Lex:lex
SynMenu IJKL.Lilo:lilo
SynMenu IJKL.Lisp:lisp
SynMenu IJKL.Lite:lite
SynMenu IJKL.LOTOS:lotos
SynMenu IJKL.Lout:lout
SynMenu IJKL.Lua:lua
SynMenu IJKL.Lynx\ Style:lss

SynMenu MNO.M4:m4
SynMenu MNO.MaGic\ Point:mgp
SynMenu MNO.Mail:mail
SynMenu MNO.Makefile:make
SynMenu MNO.MakeIndex:ist
SynMenu MNO.Man\ page:man
SynMenu MNO.Maple:maple
SynMenu MNO.Mason:mason
SynMenu MNO.Mathematica:mma
SynMenu MNO.Matlab:matlab
SynMenu MNO.MEL\ (for\ Maya):mel
SynMenu MNO.Metafont:mf
SynMenu MNO.MetaPost:mp
SynMenu MNO.MS\ Module\ Definition:def
SynMenu MNO.Model:model
SynMenu MNO.Modsim\ III:modsim3
SynMenu MNO.Modula\ 2:modula2
SynMenu MNO.Modula\ 3:modula3
SynMenu MNO.Msql:msql
SynMenu MNO.MS-DOS\ \.bat\ file:dosbatch
SynMenu MNO.4DOS\ \.bat\ file:btm
SynMenu MNO.MS-DOS\ \.ini\ file:dosini
SynMenu MNO.MS\ Resource\ file:rc
SynMenu MNO.Muttrc:muttrc
SynMenu MNO.Nastran\ input/DMAP:nastran
SynMenu MNO.Novell\ batch:ncf
SynMenu MNO.Not\ Quite\ C:nqc
SynMenu MNO.Nroff:nroff
SynMenu MNO.Objective\ C:objc
SynMenu MNO.OCAML:ocaml
SynMenu MNO.Omnimark:omnimark
SynMenu MNO.OpenROAD:openroad
SynMenu MNO.Open\ Psion\ Lang:opl
SynMenu MNO.Oracle\ config:ora

SynMenu PQR.PApp:papp
SynMenu PQR.Pascal:pascal
SynMenu PQR.PCCTS:pccts
SynMenu PQR.PPWizard:ppwiz
SynMenu PQR.Perl:perl
SynMenu PQR.Perl\ POD:pod
SynMenu PQR.Perl\ XS:xs
SynMenu PQR.PHP\ 3-4:php
SynMenu PQR.Phtml:phtml
SynMenu PQR.PIC\ assembly:pic
SynMenu PQR.Pike:pike
SynMenu PQR.Pine\ RC:pine
SynMenu PQR.PL/SQL:plsql
SynMenu PQR.PO\ (GNU\ gettext):po
SynMenu PQR.Postfix\ main\ config:pfmain
SynMenu PQR.PostScript:postscr
SynMenu PQR.Povray:pov
SynMenu PQR.Printcap:pcap
SynMenu PQR.Procmail:procmail
SynMenu PQR.Progress:progress
SynMenu PQR.Product\ Spec\ File:psf
SynMenu PQR.Prolog:prolog
SynMenu PQR.Purify\ log:purifylog
SynMenu PQR.Python:python
SynMenu PQR.R:r
SynMenu PQR.Radiance:radiance
SynMenu PQR.RCS\ log\ output:rcslog
SynMenu PQR.Rebol:rebol
SynMenu PQR.Registry\ of\ MS-Windows:registry
SynMenu PQR.Remind:remind
SynMenu PQR.Renderman\ Shader\ Lang:sl
SynMenu PQR.Rexx:rexx
SynMenu PQR.Robots\.txt:robots
SynMenu PQR.Rpcgen:rpcgen
SynMenu PQR.RTF:rtf
SynMenu PQR.Ruby:ruby

SynMenu S.S-lang:slang
SynMenu S.Samba\ config:samba
SynMenu S.SAS:sas
SynMenu S.Sather:sather
SynMenu S.Scheme:scheme
SynMenu S.SDL:sdl
SynMenu S.Sed:sed
SynMenu S.Sendmail\.cf:sm
SynMenu S.SETL:setl
SynMenu S.SGML\ DTD:sgml
SynMenu S.SGML\ Declarations:sgmldecl
SynMenu S.SGML\ linuxdoc:sgmllnx
SynMenu S.Sh\ shell\ script:sh
SynMenu S.SiCAD:sicad
SynMenu S.Simula:simula
SynMenu S.Sinda\ compare:sindacmp
SynMenu S.Sinda\ input:sinda
SynMenu S.Sinda\ output:sindaout
SynMenu S.SKILL:skill
SynMenu S.SLRN\ rc:slrnrc
SynMenu S.SLRN\ score:slrnsc
SynMenu S.SmallTalk:st
SynMenu S.SMIL:smil
SynMenu S.SMITH:smith
SynMenu S.SNMP\ MIB:mib
SynMenu S.SNNS\ network:snnsnet
SynMenu S.SNNS\ pattern:snnspat
SynMenu S.SNNS\ result:snnsres
SynMenu S.Snobol4:snobol4
SynMenu S.Snort\ Configuration:hog
SynMenu S.SPEC\ (Linux\ RPM):spec
SynMenu S.Spice:spice
SynMenu S.Speedup:spup
SynMenu S.Squid:squid
SynMenu S.SQL:sql
SynMenu S.SQR:sqr
SynMenu S.Standard\ ML:sml
SynMenu S.Stored\ Procedures:stp
SynMenu S.Strace:strace

SynMenu TUV.Tads:tads
SynMenu TUV.Tags:tags
SynMenu TUV.TAK\ compare:tak
SynMenu TUV.TAK\ input:tak
SynMenu TUV.TAK\ output:takout
SynMenu TUV.Tcl/Tk:tcl
SynMenu TUV.TealInfo:tli
SynMenu TUV.Telix\ Salt:tsalt
SynMenu TUV.Termcap:ptcap
SynMenu TUV.TeX:tex
SynMenu TUV.Texinfo:texinfo
SynMenu TUV.TeX\ configuration:texmf
SynMenu TUV.TF\ mud\ client:tf
SynMenu TUV.Trasys\ input:trasys
SynMenu TUV.TSS.Command\ Line:tsscl
SynMenu TUV.TSS.Geometry:tssgm
SynMenu TUV.TSS.Optics:tssop
SynMenu TUV.Turbo\ assembly:tasm
SynMenu TUV.UIT/UIL:uil
SynMenu TUV.UnrealScript:uc
SynMenu TUV.Verilog\ HDL:verilog
SynMenu TUV.Vgrindefs:vgrindefs
SynMenu TUV.VHDL:vhdl
SynMenu TUV.Vim\ help\ file:help
SynMenu TUV.Vim\ script:vim
SynMenu TUV.Viminfo\ file:viminfo
SynMenu TUV.Virata:virata
SynMenu TUV.Visual\ Basic:vb
SynMenu TUV.VRML:vrml
SynMenu TUV.VSE\ JCL:vsejcl

SynMenu WXYZ.WEB:web
SynMenu WXYZ.Webmacro:webmacro
SynMenu WXYZ.Website\ MetaLanguage:wml
SynMenu WXYZ.Wdiff:wdiff
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
SynMenu WXYZ.Z-80\ assembler:z8a
SynMenu WXYZ.Zsh\ shell\ script:zsh

call append(menu_lnum, "")

wq

unlet menu__name
unlet menu__nr
unlet menu__item
unlet menu__char
unlet menu_lnum
delcommand SynMenu
delfun SynMenu
