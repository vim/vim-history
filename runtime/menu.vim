" These menu commands create the default Vim menus.
" You can also use this as a start for your own set of menus.
" Note that ":amenu" is often used to make a menu work in all modes.
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	1999 Sep 19

" Make sure the '<' flag is not included in 'cpoptions', otherwise <CR> would
" not be recognized.  See ":help 'cpoptions'".
let menu_cpo_save = &cpo
let &cpo = ""

" Avoid installing the menus twice
if !exists("did_install_default_menus")
let did_install_default_menus = 1

" Help menu
amenu 9999.10 &Help.&Overview<Tab><F1>		:help<CR>
amenu 9999.20 &Help.&How-to\ links		:help how-to<CR>
amenu 9999.30 &Help.&GUI			:help gui<CR>
amenu 9999.40 &Help.&Credits			:help credits<CR>
amenu 9999.50 &Help.Co&pying			:help uganda<CR>
amenu 9999.55 &Help.-sep-			<nul>
amenu 9999.60 &Help.&Version			:version<CR>
amenu 9999.70 &Help.&About			:intro<CR>

" File menu
amenu 10.310 &File.&Open\.\.\.<Tab>:e		:browse confirm e<CR>
amenu 10.320 &File.Sp&lit-Open\.\.\.<Tab>:sp	:browse sp<CR>
amenu 10.330 &File.&Close<Tab>:q		:confirm q<CR>
amenu 10.335 &File.-SEP1-			:
amenu 10.340 &File.&Save<Tab>:w			:confirm w<CR>
amenu 10.350 &File.Save\ &As\.\.\.<Tab>:w	:browse confirm w<CR>
amenu 10.355 &File.-SEP2-			:
if has("win32")
" Use Notepad for printing. ":w >> prn" doesn't work for PostScript printers.
  amenu 10.360 &File.&Print :let smod=&mod<Bar>:let ttt=tempname()<Bar>
	\exec ":w! ".ttt<Bar>let &mod=smod<Bar>exec "!notepad /p ".ttt<Bar>
	\exec "!del ".ttt<CR>
  vunmenu &File.&Print
  vmenu &File.&Print <Esc>:let smod=&mod<Bar>:let ttt=tempname()<Bar>
	\exec ":'<,'>w! ".ttt<Bar>let &mod=smod<Bar>exec "!notepad /p ".ttt<Bar>
	\exec "!del ".ttt<CR>
elseif has("unix")
  amenu 10.360 &File.&Print			:w !lpr<CR>
  vunmenu &File.&Print
  vmenu &File.&Print				:w !lpr<CR>
endif
amenu 10.365 &File.-SEP3-			:
amenu 10.370 &File.Sa&ve-Exit<Tab>:wqa		:confirm wqa<CR>
amenu 10.380 &File.E&xit<Tab>:qa		:confirm qa<CR>


" Edit menu
amenu 20.310 &Edit.&Undo<Tab>u			u
amenu 20.320 &Edit.&Redo<Tab>Ctrl+R		<C-R>
amenu 20.330 &Edit.Repea&t<Tab>\.		.
amenu 20.335 &Edit.-SEP1-			:
vmenu 20.340 &Edit.Cu&t<Tab>"*x			"*x
vmenu 20.350 &Edit.&Copy<Tab>"*y		"*y
nmenu 20.360 &Edit.&Paste<Tab>"*p		"*p
vmenu	     &Edit.&Paste<Tab>"*p		"*P`]:if col(".")!=1<Bar>exe "norm l"<Bar>endif<CR>
imenu	     &Edit.&Paste<Tab>"*p		<Esc>:if col(".")!=1<Bar>exe 'norm "*p'<Bar>else<Bar>exe 'norm "*P'<Bar>endif<CR>`]a
cmenu	     &Edit.&Paste<Tab>"*p		<C-R>*
nmenu 20.370 &Edit.Put\ &Before<Tab>[p		[p
imenu	     &Edit.Put\ &Before<Tab>[p		<C-O>[p
nmenu 20.380 &Edit.Put\ &After<Tab>]p		]p
imenu	     &Edit.Put\ &After<Tab>]p		<C-O>]p
if has("win32")
  vmenu 20.390 &Edit.&Delete<Tab>x		x
endif
amenu 20.400 &Edit.&Select\ all<Tab>ggvG	:if &slm != ""<Bar>exe ":norm gggH<C-O>G"<Bar>else<Bar>exe ":norm ggVG"<Bar>endif<CR>
amenu 20.405 &Edit.-SEP2-			:
if has("win32") || has("gui_gtk")
  amenu 20.410 &Edit.&Find\.\.\.		:promptfind<CR>
  amenu 20.420 &Edit.Find\ and\ R&eplace\.\.\.	:promptrepl<CR>
  vunmenu      &Edit.Find\ and\ R&eplace\.\.\.
  vmenu	       &Edit.Find\ and\ R&eplace\.\.\.	y:promptrepl <C-R>"<CR>
else
  amenu 20.410 &Edit.&Find<Tab>/			/
  amenu 20.420 &Edit.Find\ and\ R&eplace<Tab>:%s	:%s/
  vunmenu      &Edit.Find\ and\ R&eplace
  vmenu	       &Edit.Find\ and\ R&eplace<Tab>:s		:s/
endif
amenu 20.425 &Edit.-SEP3-			:
amenu 20.430 &Edit.Options\.\.\.		:options<CR>

" Programming menu
amenu 40.300 &Tools.&Jump\ to\ this\ tag	g<C-]>
vunmenu &Tools.&Jump\ to\ this\ tag
vmenu &Tools.&Jump\ to\ this\ tag	g<C-]>
amenu 40.310 &Tools.Jump\ &back			<C-T>
amenu 40.320 &Tools.Build\ &Tags\ File		:!ctags -R .<CR>
amenu 40.330 &Tools.-SEP1-			:
amenu 40.340 &Tools.&Make			:make<CR>
amenu 40.350 &Tools.&List\ Errors		:cl<CR>
amenu 40.360 &Tools.L&ist\ Messages		:cl!<CR>
amenu 40.370 &Tools.&Next\ Error		:cn<CR>
amenu 40.380 &Tools.&Previous\ Error		:cp<CR>
amenu 40.390 &Tools.&Older\ List		:colder<CR>
amenu 40.400 &Tools.N&ewer\ List		:cnewer<CR>


" Can't delete a menu in Athena version
if has("gui_athena")
  let no_buffers_menu = 1
endif

if !exists("no_buffers_menu")

" Buffer list menu -- Setup functions & actions

" wait with building the menu until after loading 'session' files. Makes
" startup faster.
let bmenu_wait = 1

if !exists("bmenu_priority")
    let bmenu_priority = 60
endif

func! Bmenu_add()
    if g:bmenu_wait == 0
	call Menufilename(expand("<afile>"), expand("<abuf>"))
    endif
endfunc

func! Bmenu_remove()
    if g:bmenu_wait == 0
	let munge = Mungename(expand("<afile>"), expand("<abuf>"))

	if g:bmenu_short == 0
	    exe 'aun &Buffers.' . munge
	else
	    exe 'aun &Buffers.' . Hash2(munge) . munge
	endif
	let g:bmenu_count = g:bmenu_count - 1
    endif
endfunc

" buffer menu stuff
func! Bmenu(...)
    let g:bmenu_wait = 1
    let g:bmenu_short = 1
    let g:bmenu_count = 0
    if !exists('g:bmenu_cutoff')
	let g:bmenu_cutoff = &lines / 2
    endif
    " remove old menu, if exists
    exe g:bmenu_priority . 'am &Buffers.x x'
    aun &Buffers

    " get new priority, if exists
    if a:0 == 1
	let g:bmenu_priority = a:1
    endif

    " create new menu; make 'cpo' empty to include the <CR>
    let cpo_save = &cpo
    let &cpo = ""
    exe 'am ' . g:bmenu_priority . ".2 &Buffers.Refresh :call Bmenu()<CR>"
    exe 'am ' . g:bmenu_priority . ".4 &Buffers.Delete :bd<CR>"
    exe 'am ' . g:bmenu_priority . ".6 &Buffers.Alternate :b #<CR>"
    exe 'am ' . g:bmenu_priority . ".8 &Buffers.-SEP- :"
    let &cpo = cpo_save

    " figure out how many buffers there are
    let buf = 1
    while buf <= bufnr('$')
	if bufexists(buf)
	    let g:bmenu_count = g:bmenu_count + 1
	endif
	let buf = buf + 1
    endwhile
    if g:bmenu_count < g:bmenu_cutoff
	let g:bmenu_short = 0
    endif

    " iterate through buffer list, adding each buffer to the menu:
    let buf = 1
    while buf <= bufnr('$')
	if bufexists(buf)
	    call Menufilename(bufname(buf), buf)
	endif
	let buf = buf + 1
    endwhile
    let g:bmenu_wait = 0
    aug buffer_list
	au!
	au BufCreate,BufFilePost * call Bmenu_add()
	au BufDelete,BufFilePre * call Bmenu_remove()
    aug END
endfunc

func! Hash(name)
    " Make name all upper case, so that chars are between 32 and 96
    let nm = substitute(a:name, ".*", '\U\0', "")
    " convert first six chars into a number for sorting:
    return (char2nr(nm[0]) - 32) * 0x1000000 + (char2nr(nm[1]) - 32) * 0x40000 + (char2nr(nm[2]) - 32) * 0x1000 + (char2nr(nm[3]) - 32) * 0x40 + (char2nr(nm[4]) - 32) * 0x40 + (char2nr(nm[5]) - 32)
endfunc

func! Hash2(name)
    let nm = substitute(a:name, ".", '\L\0', "")
    if nm[0] >= 'a' && nm[0] <= 'd'
	return '&abcd.'
    elseif nm[0] >= 'e' && nm[0] <= 'h'
	return '&efgh.'
    elseif nm[0] >= 'i' && nm[0] <= 'l'
	return '&ijkl.'
    elseif nm[0] >= 'm' && nm[0] <= 'p'
	return '&mnop.'
    elseif nm[0] >= 'q' && nm[0] <= 't'
	return '&qrst.'
    elseif nm[0] >= 'u' && nm[0] <= 'z'
	return '&u-z.'
    else
	return '&others.'
endfunc

" take a buffer number, return a name to insert into a menu:
func! Menufilename(name, num)
    let munge = Mungename(a:name, a:num)
    let hash = Hash(munge)
    if g:bmenu_short == 0
	let name = 'am ' . g:bmenu_priority . '.' . hash .' &Buffers.' . munge
    else
	let name = 'am ' . g:bmenu_priority . '.' . hash . '.' . hash .' &Buffers.' . Hash2(munge) . munge
    endif
    " make 'cpo' empty to include the <CR>
    let cpo_save = &cpo
    let &cpo = ""
    exe name . ' :b' . a:num . '<CR>'
    let &cpo = cpo_save
endfunc

func! Mungename(fname, bnum)
    let name = a:fname
    if name == ''
	let name = '[No File]'
    endif
    let name = fnamemodify(name, ':~')
    if !isdirectory(a:fname)
	" detach file name and separate it out:
	let name2 = fnamemodify(name, ':t')
	if a:bnum >= 0
	    let name2 = name2 . ' (' . a:bnum . ')'
	endif
	let name = name2 . "\t" . fnamemodify(name,':h')
    endif
    let name = escape(name, '\\. 	|')
    return name
endfunc

" When just starting Vim, load the buffer menu later
if has("vim_starting")
    augroup LoadBufferMenu
        au! VimEnter * if !exists("no_buffers_menu") | call Bmenu() | endif
	au  VimEnter * au! LoadBufferMenu
    augroup END
else
    call Bmenu()
endif

endif " !exists("no_buffers_menu")

" Window menu
amenu 70.300 &Window.&New		<C-W>n
amenu 70.310 &Window.S&plit		<C-W>s
amenu 70.320 &Window.Sp&lit\ To\ #	<C-W><C-^>
amenu 70.325 &Window.-SEP1-		:
amenu 70.330 &Window.&Close		:confirm close<CR>
amenu 70.340 &Window.Close\ &Other(s)	:confirm only<CR>
amenu 70.345 &Window.-SEP2-		:
amenu 70.350 &Window.Ne&xt		<C-W>w
amenu 70.360 &Window.P&revious		<C-W>W
amenu 70.365 &Window.-SEP3-		:
amenu 70.370 &Window.&Equal\ Height	<C-W>=
amenu 70.380 &Window.&Max\ Height	<C-W>_
amenu 70.390 &Window.M&in\ Height	<C-W>1_
amenu 70.400 &Window.Rotate\ &Up	<C-W>R
amenu 70.410 &Window.Rotate\ &Down	<C-W>r
if has("win32") || has("gui_gtk")
  amenu 70.420 &Window.-SEP4-		:
  amenu 70.430 &Window.Select\ &Font\.\.\.	:set guifont=*<CR>
endif

" The popup menu
amenu 1.10 PopUp.&Undo		u
amenu 1.15 PopUp.-SEP1-		:
vmenu 1.20 PopUp.Cu&t		"*x
vmenu 1.30 PopUp.&Copy		"*y
nmenu 1.40 PopUp.&Paste		"*P`]:if col(".")!=1<Bar>exe "norm l"<Bar>endif<CR>
vmenu      PopUp.&Paste		"-x"*P`]
imenu      PopUp.&Paste		<Esc>:if col(".")!=1<Bar>exe 'norm "*p'<Bar>else<Bar>exe 'norm "*P'<Bar>endif<CR>`]a
cmenu      PopUp.&Paste		<C-R>*
vmenu 1.50 PopUp.&Delete	x
amenu 1.55 PopUp.-SEP2-		:
vmenu 1.60 PopUp.Select\ Blockwise <C-V>
amenu 1.70 PopUp.Select\ &Word	vaw
amenu 1.80 PopUp.Select\ &Line	V
amenu 1.90 PopUp.Select\ &Block	<C-V>
amenu 1.100 PopUp.Select\ &All	ggVG

" The GUI toolbar (for Win32 or GTK)
if has("win32") || has("gui_gtk")
  amenu 1.10 ToolBar.Open	:browse e<CR>
  tmenu ToolBar.Open		Open file
  amenu 1.20 ToolBar.Save	:w<CR>
  tmenu ToolBar.Save		Save current file
  amenu 1.30 ToolBar.SaveAll	:wa<CR>
  tmenu ToolBar.SaveAll		Save all files

  if has("win32")
    amenu 1.40 ToolBar.Print	:let smod=&mod<Bar>:let ttt=tempname()<Bar>exec ":w! ".ttt<Bar>let &mod=smod<Bar>exec "!notepad /p ".ttt<Bar>exec "!del ".ttt<CR>
    vunmenu ToolBar.Print
    vmenu ToolBar.Print		<Esc>:let smod=&mod<Bar>:let ttt=tempname()<Bar>exec ":'<,'>w! ".ttt<Bar>let &mod=smod<Bar>exec "!notepad /p ".ttt<Bar>exec "!del ".ttt<CR>
  else
    amenu 1.40 ToolBar.Print	:w !lpr<CR>
    vunmenu ToolBar.Print
    vmenu ToolBar.Print		<Esc>:w !lpr<CR>
  endif
  tmenu ToolBar.Print		Print

  amenu 1.45 ToolBar.-sep1-	<nul>
  amenu 1.50 ToolBar.Undo	u
  tmenu ToolBar.Undo		Undo
  amenu 1.60 ToolBar.Redo	<C-R>
  tmenu ToolBar.Redo		Redo

  amenu 1.65 ToolBar.-sep2-	<nul>
  vmenu 1.70 ToolBar.Cut	"*x
  tmenu ToolBar.Cut		Cut to clipboard
  vmenu 1.80 ToolBar.Copy	"*y
  tmenu ToolBar.Copy		Copy to clipboard
  nmenu 1.90 ToolBar.Paste	i<C-R>*<Esc>
  vmenu ToolBar.Paste		"-xi<C-R>*<Esc>
  menu! ToolBar.Paste		<C-R>*
  tmenu ToolBar.Paste		Paste from Clipboard

  amenu 1.95 ToolBar.-sep3-	<nul>
  amenu 1.100 ToolBar.Find	:promptfind<CR>
  tmenu ToolBar.Find		Find...
  amenu 1.110 ToolBar.FindNext	n
  tmenu ToolBar.FindNext	Find Next
  amenu 1.120 ToolBar.FindPrev	N
  tmenu ToolBar.FindPrev	Find Previous
  amenu 1.130 ToolBar.Replace	:promptrepl<CR>
  vunmenu ToolBar.Replace
  vmenu ToolBar.Replace		y:promptrepl <C-R>"<CR>
  tmenu ToolBar.Replace		Find & Replace...

if 0	" disabled; These are in the Windows menu
  amenu 1.135 ToolBar.-sep4-	<nul>
  amenu 1.140 ToolBar.New	<C-W>n
  tmenu ToolBar.New		New Window
  amenu 1.150 ToolBar.WinSplit	<C-W>s
  tmenu ToolBar.WinSplit	Split Window
  amenu 1.160 ToolBar.WinMax	:resize 200<CR>
  tmenu ToolBar.WinMax		Maximise Window
  amenu 1.170 ToolBar.WinMin	:resize 1<CR>
  tmenu ToolBar.WinMin		Minimise Window
  amenu 1.180 ToolBar.WinClose	:close<CR>
  tmenu ToolBar.WinClose	Close Window
endif

  amenu 1.185 ToolBar.-sep5-	<nul>
  amenu 1.190 ToolBar.LoadSesn	:call LoadVimSesn()<CR>
  tmenu ToolBar.LoadSesn	Load session
  amenu 1.200 ToolBar.SaveSesn	:call SaveVimSesn()<CR>
  tmenu ToolBar.SaveSesn	Save current session
  amenu 1.210 ToolBar.RunScript	:browse so<CR>
  tmenu ToolBar.RunScript	Run a Vim Script

  amenu 1.215 ToolBar.-sep6-	<nul>
  amenu 1.220 ToolBar.Make	:make<CR>
  tmenu ToolBar.Make		Make current project
  amenu 1.230 ToolBar.Shell	:sh<CR>
  tmenu ToolBar.Shell		Open a command shell
  amenu 1.240 ToolBar.RunCtags	:!ctags -R .<CR>
  tmenu ToolBar.RunCtags	Build tags in current directory tree
  amenu 1.250 ToolBar.TagJump	g]
  tmenu ToolBar.TagJump		Jump to tag under cursor

  amenu 1.265 ToolBar.-sep7-	<nul>
  amenu 1.270 ToolBar.Help	:help<CR>
  tmenu ToolBar.Help		Vim Help
  if has("gui_gtk")
    amenu 1.280 ToolBar.FindHelp :helpfind<CR>
  else
    amenu 1.280 ToolBar.FindHelp :help 
  endif
  tmenu ToolBar.FindHelp	Search Vim Help

" Select a session to load; default to current session name if present
fun LoadVimSesn()
  if exists("this_session")
    let name = this_session
  else
    let name = "session.vim"
  endif
  execute "browse so " . name
endfun

" Select a session to save; default to current session name if present
fun SaveVimSesn()
  if !exists("this_session")
    let this_session = "session.vim"
  endif
  execute "browse mksession! " . this_session
endfun

endif " has("win32") || has("gui_gtk")

endif " !exists("did_install_default_menus")

" Install the Syntax menu only when filetype.vim has been loaded or when
" manual syntax highlighting is enabled.
" Avoid installing the Syntax menu twice.
if (exists("did_load_filetypes") || exists("syntax_on"))
	\ && !exists("did_install_syntax_menu")
let did_install_syntax_menu = 1

" Define the SetSyn function, used for the Syntax menu entries.
" Set 'filetype' and also 'syntax' if it is manually selected.
fun! SetSyn(name)
  if !exists("g:syntax_menu_synonly")
    exe "set ft=" . a:name
    if exists("g:syntax_manual")
      exe "set syn=" . a:name
    endif
  else
    exe "set syn=" . a:name
  endif
endfun

" Use the SynMenu command and function to define all menu entries
command -nargs=* SynMenu call SynMenu(<q-args>)

let current_menu_name = ""
let current_menu_nr = 0
let current_menu_item = 0
let current_menu_char = ""

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

  if g:current_menu_name != menu_name
    let g:current_menu_name = menu_name
    let g:current_menu_nr = g:current_menu_nr + 10
    let g:current_menu_item = 100
    let g:current_menu_char = submenu_name[0]
  else
    " When starting a new letter, insert a menu separator.
    " Make an exception for "4DOS", which is after "MS-DOS".
    let c = submenu_name[0]
    if c != g:current_menu_char && c != "4"
      exe 'am 50.' . g:current_menu_nr . '.' . g:current_menu_item . ' &Syntax.' . menu_name . ".-" . c . '- <nul>'
      let g:current_menu_item = g:current_menu_item + 10
      let g:current_menu_char = c
    endif
  endif
  exe 'am 50.' . g:current_menu_nr . '.' . g:current_menu_item . ' &Syntax.' . menu_name . "." . submenu_name . ' :cal SetSyn("' . syntax_name . '")<CR>'
  let g:current_menu_item = g:current_menu_item + 10
endfun

SynMenu &ABCD.ABC:abc
SynMenu &ABCD.Ada:ada
SynMenu &ABCD.Active\ Server\ Pages:aspvbs
SynMenu &ABCD.AHDL:ahdl
SynMenu &ABCD.Amiga\ DOS:amiga
SynMenu &ABCD.Applix\ ELF:elf
SynMenu &ABCD.Assembly\ (GNU):asm
SynMenu &ABCD.Assembly\ (H8300):asmh8300
SynMenu &ABCD.Assembly\ (Microsoft):masm
SynMenu &ABCD.Assembly\ (Netwide):nasm
SynMenu &ABCD.ASN\.1:asn
SynMenu &ABCD.Atlas:atlas
SynMenu &ABCD.Avenue:ave
SynMenu &ABCD.Awk:awk
SynMenu &ABCD.BASIC:basic
SynMenu &ABCD.BibFile:bib
SynMenu &ABCD.C:c
SynMenu &ABCD.C++:cpp
SynMenu &ABCD.Cascading\ Style\ Sheets:css
SynMenu &ABCD.Century\ Term:cterm
SynMenu &ABCD.Change:change
SynMenu &ABCD.Clean:clean
SynMenu &ABCD.Clipper:clipper
SynMenu &ABCD.Configure\ script:config
SynMenu &ABCD.Csh\ shell\ script:csh
SynMenu &ABCD.Cobol:cobol
SynMenu &ABCD.CWEB:cweb
SynMenu &ABCD.Diff:diff
SynMenu &ABCD.Digital\ Command\ Lang:dcl
SynMenu &ABCD.Diva\ (with\ SKILL):diva
SynMenu &ABCD.Dracula:dracula
SynMenu &ABCD.DTD:dtd

SynMenu &EFGHIJ.Eiffel:eiffel
SynMenu &EFGHIJ.Elm\ Filter:elmfilt
SynMenu &EFGHIJ.ESQL-C:esqlc
SynMenu &EFGHIJ.Expect:expect
SynMenu &EFGHIJ.Exports:exports
SynMenu &EFGHIJ.Focus\ Executable:focexec
SynMenu &EFGHIJ.Focus\ Master:master
SynMenu &EFGHIJ.Forth:forth
SynMenu &EFGHIJ.Fortran:fortran
SynMenu &EFGHIJ.Fvwm\ configuration:fvwm
SynMenu &EFGHIJ.GDB\ command\ file:gdb
SynMenu &EFGHIJ.GDMO:gdmo
SynMenu &EFGHIJ.GP:gp
SynMenu &EFGHIJ.GNUplot:gnuplot
SynMenu &EFGHIJ.Haskell:haskell
SynMenu &EFGHIJ.Haskell-literal:lhaskell
SynMenu &EFGHIJ.HTML:html
SynMenu &EFGHIJ.HTML\ with\ M4:htmlm4
SynMenu &EFGHIJ.IDL:idl
SynMenu &EFGHIJ.Interactive\ Data\ Lang:idlang
SynMenu &EFGHIJ.Inform:inform
SynMenu &EFGHIJ.InstallShield\ Rules:ishd
SynMenu &EFGHIJ.Java:java
SynMenu &EFGHIJ.JavaCC:javacc
SynMenu &EFGHIJ.JavaScript:javascript
SynMenu &EFGHIJ.Jgraph:jgraph

SynMenu &KLM.Kimwitu:kwt
SynMenu &KLM.Lace:lace
SynMenu &KLM.Lex:lex
SynMenu &KLM.Lilo:lilo
SynMenu &KLM.Lisp:lisp
SynMenu &KLM.Lite:lite
SynMenu &KLM.LOTOS:lotos
SynMenu &KLM.Lout:lout
SynMenu &KLM.Lua:lua
SynMenu &KLM.Lynx\ Style:lss
SynMenu &KLM.M4:m4
SynMenu &KLM.Mail:mail
SynMenu &KLM.Makefile:make
SynMenu &KLM.Man\ page:man
SynMenu &KLM.Maple:maple
SynMenu &KLM.Matlab:matlab
SynMenu &KLM.Metafont:mf
SynMenu &KLM.MetaPost:mp
SynMenu &KLM.MS\ Module\ Definition:def
SynMenu &KLM.Model:model
SynMenu &KLM.Modsim\ III:modsim3
SynMenu &KLM.Modula\ 2:modula2
SynMenu &KLM.Modula\ 3:modula3
SynMenu &KLM.Msql:msql
SynMenu &KLM.MS-DOS\ \.bat\ file:dosbatch
SynMenu &KLM.4DOS\ \.bat\ file:btm
SynMenu &KLM.MS-DOS\ \.ini\ file:dosini
SynMenu &KLM.MS\ Resource\ file:rc
SynMenu &KLM.Muttrc:muttrc

SynMenu &NOPQ.Novell\ batch:ncf
SynMenu &NOPQ.Nroff:nroff
SynMenu &NOPQ.Objective\ C:objc
SynMenu &NOPQ.OCAML:ocaml
SynMenu &NOPQ.Open\ Psion\ Lang:opl
SynMenu &NOPQ.Pascal:pascal
SynMenu &NOPQ.PCCTS:pccts
SynMenu &NOPQ.Perl:perl
SynMenu &NOPQ.Perl\ POD:pod
SynMenu &NOPQ.Perl\ XS:xs
SynMenu &NOPQ.PHP\ 3:php3
SynMenu &NOPQ.Phtml:phtml
SynMenu &NOPQ.Pike:pike
SynMenu &NOPQ.Pine\ RC:pine
SynMenu &NOPQ.PL/SQL:plsql
SynMenu &NOPQ.PO\ (GNU\ gettext):po
SynMenu &NOPQ.PostScript:postscr
SynMenu &NOPQ.Povray:pov
SynMenu &NOPQ.Printcap:pcap
SynMenu &NOPQ.Procmail:procmail
SynMenu &NOPQ.Prolog:prolog
SynMenu &NOPQ.Purify\ log:purifylog
SynMenu &NOPQ.Python:python

SynMenu &RS.Radiance:radiance
SynMenu &RS.Rebol:rebol
SynMenu &RS.Renderman\ Shader\ Lang:sl
SynMenu &RS.Rexx:rexx
SynMenu &RS.Rpcgen:rpcgen
SynMenu &RS.S-lang:slang
SynMenu &RS.SAS:sas
SynMenu &RS.Sather:sather
SynMenu &RS.Scheme:scheme
SynMenu &RS.SDL:sdl
SynMenu &RS.Sed:sed
SynMenu &RS.Sendmail\.cf:sm
SynMenu &RS.SGML\ DTD:sgml
SynMenu &RS.SGML\ linuxdoc:sgmllnx
SynMenu &RS.Sh\ shell\ script:sh
SynMenu &RS.SiCAD:sicad
SynMenu &RS.Simula:simula
SynMenu &RS.SKILL:skill
SynMenu &RS.SLRN\ rc:slrnrc
SynMenu &RS.SLRN\ score:slrnsc
SynMenu &RS.SmallTalk:st
SynMenu &RS.SMIL:smil
SynMenu &RS.SNMP\ MIB:mib
SynMenu &RS.SPEC\ (Linux\ RPM):spec
SynMenu &RS.Spice:spice
SynMenu &RS.Speedup:spup
SynMenu &RS.Squid:squid
SynMenu &RS.SQL:sql

SynMenu &TU.Tags:tags
SynMenu &TU.Tcl/Tk:tcl
SynMenu &TU.Telix\ Salt:tsalt
SynMenu &TU.Termcap:ptcap
SynMenu &TU.TeX:tex
SynMenu &TU.TF\ mud\ client:tf
SynMenu &TU.UIT/UIL:uil

SynMenu &VWXYZ.Verilog\ HDL:verilog
SynMenu &VWXYZ.Vgrindefs:vgrindefs
SynMenu &VWXYZ.VHDL:vhdl
SynMenu &VWXYZ.Vim\ help\ file:help
SynMenu &VWXYZ.Vim\ script:vim
SynMenu &VWXYZ.Viminfo\ file:viminfo
SynMenu &VWXYZ.Visual\ Basic:vb
SynMenu &VWXYZ.VRML:vrml
SynMenu &VWXYZ.WEB:web
SynMenu &VWXYZ.Whitespace\ (add):whitespace
SynMenu &VWXYZ.WinBatch/Webbatch:winbatch
SynMenu &VWXYZ.X\ Pixmap:xpm
SynMenu &VWXYZ.X\ resources:xdefaults
SynMenu &VWXYZ.Xmath:xmath
SynMenu &VWXYZ.XML:xml
SynMenu &VWXYZ.XXD\ hex\ dump:xxd
SynMenu &VWXYZ.Yacc:yacc
SynMenu &VWXYZ.Z-80\ assembler:z8a
SynMenu &VWXYZ.Zsh\ shell\ script:zsh

unlet current_menu_name
unlet current_menu_nr
unlet current_menu_item
unlet current_menu_char
delcommand SynMenu
delfun SynMenu

am 50.75 &Syntax.-SEP1-				:

am 50.80 &Syntax.Set\ 'syntax'\ only		:let syntax_menu_synonly=1<CR>
am 50.81 &Syntax.Set\ 'filetype'\ too		:if exists("syntax_menu_synonly")<Bar>unlet syntax_menu_synonly<Bar>endif<CR>

am 50.90 &Syntax.&Off			:syn off<CR>
am 50.92 &Syntax.&Manual		:syn manual<CR>
am 50.94 &Syntax.A&utomatic		:syn on<CR>

am 50.96 &Syntax.&on\ (this\ file)	:if !exists("syntax_on")<Bar>syn manual<Bar>endif<Bar>set syn=ON<CR>
am 50.98 &Syntax.o&ff\ (this\ file)	:syn clear<CR>

am 50.700 &Syntax.-SEP3-		:
am 50.710 &Syntax.Co&lor\ test		:sp $VIMRUNTIME/syntax/colortest.vim<Bar>so %<CR>
am 50.720 &Syntax.&Highlight\ test	:so $VIMRUNTIME/syntax/hitest.vim<CR>
am 50.730 &Syntax.&Convert\ to\ HTML	:so $VIMRUNTIME/syntax/2html.vim<CR>

endif " !exists("did_install_syntax_menu")

" Restore the previous value of 'cpoptions'.
let &cpo = menu_cpo_save
unlet menu_cpo_save
