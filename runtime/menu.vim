" These menu commands create the default Vim menus.
" You can also use this as a start for your own set of menus.
" Note that ":amenu" is often used to make a menu work in all modes.
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2000 Nov 12

" Make sure the '<' and 'C' flags are not included in 'cpoptions', otherwise
" <CR> would not be recognized.  See ":help 'cpoptions'".
let s:cpo_save = &cpo
set cpo&vim

" Avoid installing the menus twice
if !exists("did_install_default_menus")
let did_install_default_menus = 1


if exists("v:lang")
  " Try to find a menu translation file for the current language.
  if &langmenu != ""
    let s:lang = &langmenu
  else
    let s:lang = v:lang
  endif
  " A language name must be at least two characters, don't accept "C"
  if strlen(s:lang) > 1
    let s:lang = substitute(s:lang, ".*", "\\L&", "")
    exe "runtime lang/menu_" . s:lang . ".vim"
    " If there is no exact match, try matching with a wildcard added.
    " (e.g. find menu_de_DE.ISO_8859-1.vim if s:lang == de_DE)
    if !exists("did_menu_trans")
      " try to get the first long file name which matches v:lang.
      exe "runtime lang/menu_" . s:lang . "*.vim"
    endif
  endif
endif


" Help menu
amenu 9999.10 &Help.&Overview<Tab><F1>		:help<CR>
amenu 9999.20 &Help.&How-to\ links		:help how-to<CR>
amenu 9999.30 &Help.&GUI			:help gui<CR>
amenu 9999.40 &Help.&Credits			:help credits<CR>
amenu 9999.50 &Help.Co&pying			:help uganda<CR>
if has("gui_gtk")
  amenu 9999.60 &Help.&Find\.\.\.		:helpfind<CR>
endif
amenu 9999.65 &Help.-sep-			<nul>
amenu 9999.70 &Help.&Version			:version<CR>
amenu 9999.80 &Help.&About			:intro<CR>

" File menu
amenu 10.310 &File.&Open\.\.\.<Tab>:e		:browse confirm e<CR>
amenu 10.320 &File.Sp&lit-Open\.\.\.<Tab>:sp	:browse sp<CR>
amenu 10.325 &File.&New<Tab>:enew		:confirm enew<CR>
amenu 10.330 &File.&Close<Tab>:q		:confirm q<CR>
amenu 10.335 &File.-SEP1-			:
amenu 10.340 &File.&Save<Tab>:w			:confirm w<CR>
amenu 10.350 &File.Save\ &As\.\.\.<Tab>:w	:browse confirm w<Bar>silent edit #<CR>
if has("win32")
  amenu 10.355 &File.-SEP2-			:
  " Use Notepad for printing. ":w >> prn" doesn't work for PostScript printers.
  amenu 10.360 &File.&Print			:call Win32Print(":")<CR>
  vunmenu &File.&Print
  vmenu &File.&Print				<Esc>:call Win32Print(":'<,'>")<CR>
  if !exists("*Win32Print")
    fun Win32Print(range)
      let mod_save = &mod
      let ff_save = &ff
      set ff=dos
      let ttt = tempname()
      exec a:range . "w! " . ttt
      let &ff = ff_save
      let &mod = mod_save
      exec "!notepad /p " . ttt
      exec "!del " . ttt
    endfun
  endif
elseif has("unix")
  amenu 10.355 &File.-SEP2-			:
  amenu 10.360 &File.&Print			:w !lpr<CR>
  vunmenu &File.&Print
  vmenu &File.&Print				:w !lpr<CR>
elseif has("vms")
  amenu 10.355 &File.-SEP2-                     :
  amenu 10.360 &File.&Print			:call VMSPrint(":")<CR>
  vunmenu &File.&Print
  vmenu &File.&Print				<Esc>:call VMSPrint(":'<,'>")<CR>
  if !exists("*VMSPrint")
    fun VMSPrint(range)
      let mod_save = &mod
      let ttt = tempname()
      exec a:range . "w! " . ttt
      let &mod = mod_save
      exec "!print/delete " . ttt
    endfun
  endif
endif
amenu 10.365 &File.-SEP3-			:
amenu 10.370 &File.Sa&ve-Exit<Tab>:wqa		:confirm wqa<CR>
amenu 10.380 &File.E&xit<Tab>:qa		:confirm qa<CR>


" Edit menu
amenu 20.310 &Edit.&Undo<Tab>u			u
amenu 20.320 &Edit.&Redo<Tab>^R			<C-R>
amenu 20.330 &Edit.Rep&eat<Tab>\.		.
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
if has("win32") || has("win16")
  vmenu 20.390 &Edit.&Delete<Tab>x		x
endif
amenu 20.400 &Edit.&Select\ all<Tab>ggVG	:if &slm != ""<Bar>exe ":norm gggH<C-O>G"<Bar>else<Bar>exe ":norm ggVG"<Bar>endif<CR>
amenu 20.405 &Edit.-SEP2-			:
if has("win32")  || has("win16") || has("gui_gtk")
  amenu 20.410 &Edit.&Find\.\.\.		:promptfind<CR>
  amenu 20.420 &Edit.Find\ and\ Rep&lace\.\.\.	:promptrepl<CR>
  vunmenu      &Edit.Find\ and\ Rep&lace\.\.\.
  vmenu	       &Edit.Find\ and\ Rep&lace\.\.\.	y:promptrepl <C-R>"<CR>
else
  amenu 20.410 &Edit.&Find<Tab>/			/
  amenu 20.420 &Edit.Find\ and\ Rep&lace<Tab>:%s	:%s/
  vunmenu      &Edit.Find\ and\ Rep&lace
  vmenu	       &Edit.Find\ and\ Rep&lace<Tab>:s		:s/
endif
amenu 20.425 &Edit.-SEP3-			:
amenu 20.430 &Edit.Options\.\.\.		:options<CR>

" Programming menu
amenu 40.300 &Tools.&Jump\ to\ this\ tag<Tab>g^] g<C-]>
vunmenu &Tools.&Jump\ to\ this\ tag<Tab>g^]
vmenu &Tools.&Jump\ to\ this\ tag<Tab>g^]	g<C-]>
amenu 40.310 &Tools.Jump\ &back<Tab>^T		<C-T>
if has("vms")
  amenu 40.320 &Tools.Build\ &Tags\ File                :!mc vim:ctags -R .<CR>
else
  amenu 40.320 &Tools.Build\ &Tags\ File		:!ctags -R .<CR>
endif
amenu 40.330 &Tools.-SEP1-			:
amenu 40.340 &Tools.&Make<Tab>:make		:make<CR>
amenu 40.350 &Tools.&List\ Errors<Tab>:cl	:cl<CR>
amenu 40.360 &Tools.L&ist\ Messages<Tab>:cl!	:cl!<CR>
amenu 40.370 &Tools.&Next\ Error<Tab>:cn	:cn<CR>
amenu 40.380 &Tools.&Previous\ Error<Tab>:cp	:cp<CR>
amenu 40.390 &Tools.&Older\ List<Tab>:cold	:colder<CR>
amenu 40.400 &Tools.N&ewer\ List<Tab>:cnew	:cnewer<CR>
amenu 40.410 &Tools.Error\ &Window<Tab>:cwin	:cwin<CR>
amenu 40.500 &Tools.-SEP2-                      :
if has("vms")
  amenu 40.510 &Tools.Convert\ to\ HEX<Tab>:%!mc\ vim:xxd
	\ :let b:mod = &mod<CR>
	\ :%!mc vim:xxd<CR>
	\ :set ft=xxd<CR>
	\ :let &mod = b:mod<CR>
  amenu 40.520 &Tools.Convert\ back<Tab>:%!mc\ vim:xxd\ -r
	\ :let b:mod = &mod<CR>
	\ :%!mc vim:xxd -r<CR>
	\ :doautocmd filetypedetect BufReadPost<CR>
	\ :let &mod = b:mod<CR>
else
  amenu 40.510 &Tools.Convert\ to\ HEX<Tab>:%!xxd
	\ :let b:mod = &mod<CR>
	\ :silent %!xxd<CR>
	\ :set ft=xxd<CR>
	\ :let &mod = b:mod<CR>
  amenu 40.520 &Tools.Convert\ back<Tab>:%!xxd\ -r
	\ :let b:mod = &mod<CR>
	\ :%!xxd -r<CR>
	\ :doautocmd filetypedetect BufReadPost<CR>
	\ :let &mod = b:mod<CR>
endif

" Can't delete a menu in Athena version
if has("gui_athena")
  let no_buffers_menu = 1
endif

if !exists("no_buffers_menu")

" Buffer list menu -- Setup functions & actions

" wait with building the menu until after loading 'session' files. Makes
" startup faster.
let s:bmenu_wait = 1

if !exists("bmenu_priority")
  let bmenu_priority = 60
endif

func! <SID>BMAdd()
  if s:bmenu_wait == 0
    call <SID>BMFilename(expand("<afile>"), expand("<abuf>"))
  endif
endfunc

func! <SID>BMRemove()
  if s:bmenu_wait == 0
    let munge = <SID>BMMunge(expand("<afile>"), expand("<abuf>"))

    if s:bmenu_short == 0
      exe 'aun &Buffers.' . munge
    else
      exe 'aun &Buffers.' . <SID>BMHash2(munge) . munge
    endif
    let s:bmenu_count = s:bmenu_count - 1
  endif
endfunc

" buffer menu stuff
func! <SID>BMShow(...)
  let s:bmenu_wait = 1
  let s:bmenu_short = 1
  let s:bmenu_count = 0
  if version >= 600
    let bmenu_cutoff = &menuitems
  elseif exists('g:bmenu_cutoff')
    let bmenu_cutoff = g:bmenu_cutoff
  else
    let bmenu_cutoff = &lines / 2
  endif
  " remove old menu, if exists
  exe g:bmenu_priority . 'am &Buffers.x x'
  exe "aun &Buffers"

  " get new priority, if exists
  if a:0 == 1
    let g:bmenu_priority = a:1
  endif

  " create new menu; set 'cpo' to include the <CR>
  let cpo_save = &cpo
  set cpo&vim
  exe 'am ' . g:bmenu_priority . ".2 &Buffers.Refresh :call <SID>BMShow()<CR>"
  exe 'am ' . g:bmenu_priority . ".4 &Buffers.Delete :bd<CR>"
  exe 'am ' . g:bmenu_priority . ".6 &Buffers.Alternate :b #<CR>"
  exe 'am ' . g:bmenu_priority . ".8 &Buffers.-SEP- :"
  let &cpo = cpo_save

  " figure out how many buffers there are
  let buf = 1
  while buf <= bufnr('$')
    if bufexists(buf)
      let s:bmenu_count = s:bmenu_count + 1
    endif
    let buf = buf + 1
  endwhile
  if s:bmenu_count < bmenu_cutoff
    let s:bmenu_short = 0
  endif

  " iterate through buffer list, adding each buffer to the menu:
  let buf = 1
  while buf <= bufnr('$')
    if bufexists(buf)
      call <SID>BMFilename(bufname(buf), buf)
    endif
    let buf = buf + 1
  endwhile
  let s:bmenu_wait = 0
  aug buffer_list
  au!
  au BufCreate,BufFilePost * call <SID>BMAdd()
  au BufDelete,BufFilePre * call <SID>BMRemove()
  aug END
endfunc

func! <SID>BMHash(name)
  " Make name all upper case, so that chars are between 32 and 96
  let nm = substitute(a:name, ".*", '\U\0', "")
  if has("ebcdic")
    " HACK: Replace all non alphabetics with 'Z'
    "       Just to make it work for now.
    let nm = substitute(nm, "[^A-Z]", 'Z', "g")
    let sp = char2nr('A') - 1
  else
    let sp = char2nr(' ')
  endif
  " convert first six chars into a number for sorting:
  return (char2nr(nm[0]) - sp) * 0x1000000 + (char2nr(nm[1]) - sp) * 0x40000 + (char2nr(nm[2]) - sp) * 0x1000 + (char2nr(nm[3]) - sp) * 0x40 + (char2nr(nm[4]) - sp) * 0x40 + (char2nr(nm[5]) - sp)
endfunc

func! <SID>BMHash2(name)
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
  endif
endfunc

" take a buffer number, return a name to insert into a menu:
func! <SID>BMFilename(name, num)
  let munge = <SID>BMMunge(a:name, a:num)
  let hash = <SID>BMHash(munge)
  if s:bmenu_short == 0
    let name = 'am ' . g:bmenu_priority . '.' . hash . ' &Buffers.' . munge
  else
    let name = 'am ' . g:bmenu_priority . '.' . hash . '.' . hash . ' &Buffers.' . <SID>BMHash2(munge) . munge
  endif
  " set 'cpo' to include the <CR>
  let cpo_save = &cpo
  set cpo&vim
  exe name . ' :b' . a:num . '<CR>'
  let &cpo = cpo_save
endfunc

func! <SID>BMMunge(fname, bnum)
  let name = a:fname
  if name == ''
    let name = "[No File]"
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
  let name = escape(name, "\\. \t|")
  let name = substitute(name, "\n", "^@", "g")
  return name
endfunc

" When just starting Vim, load the buffer menu later
if has("vim_starting")
    augroup LoadBufferMenu
        au! VimEnter * if !exists("no_buffers_menu") | call <SID>BMShow() | endif
	au  VimEnter * au! LoadBufferMenu
    augroup END
else
    call <SID>BMShow()
endif

endif " !exists("no_buffers_menu")

" Window menu
amenu 70.300 &Window.&New<Tab>^Wn		<C-W>n
amenu 70.310 &Window.S&plit<Tab>^Ws		<C-W>s
amenu 70.320 &Window.Sp&lit\ To\ #<Tab>^W^^	<C-W><C-^>
amenu 70.330 &Window.Split\ &Vertically<Tab>^Wv	<C-W>v
amenu 70.335 &Window.-SEP1-			:
amenu 70.340 &Window.&Close<Tab>^Wc		:confirm close<CR>
amenu 70.345 &Window.Close\ &Other(s)<Tab>^Wo	:confirm only<CR>
amenu 70.350 &Window.-SEP2-			:
amenu 70.355 &Window.Ne&xt<Tab>^Ww		<C-W>w
amenu 70.360 &Window.P&revious<Tab>^WW		<C-W>W
amenu 70.365 &Window.-SEP3-			:
amenu 70.370 &Window.&Equal\ Height<Tab>^W=	<C-W>=
amenu 70.380 &Window.&Max\ Height<Tab>^W_	<C-W>_
amenu 70.390 &Window.M&in\ Height<Tab>^W1_	<C-W>1_
amenu 70.400 &Window.Max\ Width<Tab>^W\|	<C-W>\|
amenu 70.410 &Window.Min\ Width<Tab>^W1\|	<C-W>1\|
amenu 70.420 &Window.Rotate\ &Up<Tab>^WR	<C-W>R
amenu 70.430 &Window.Rotate\ &Down<Tab>^Wr	<C-W>r
if has("win32") || has("win16") || has("gui_gtk")
  amenu 70.440 &Window.-SEP4-			:
  amenu 70.450 &Window.Select\ &Font\.\.\.	:set guifont=*<CR>
endif

" The popup menu
amenu 1.10 PopUp.&Undo		u
amenu 1.15 PopUp.-SEP1-		:
vmenu 1.20 PopUp.Cu&t		"*x
vmenu 1.30 PopUp.&Copy		"*y
nmenu 1.40 PopUp.&Paste		"*P`]:if col(".")!=1<Bar>exe "norm l"<Bar>endif<CR>
vmenu 1.40 PopUp.&Paste		"-x"*P`]
imenu 1.40 PopUp.&Paste		<Esc>:if col(".")!=1<Bar>exe 'norm "*p'<Bar>else<Bar>exe 'norm "*P'<Bar>endif<CR>`]a
cmenu 1.40 PopUp.&Paste		<C-R>*
vmenu 1.50 PopUp.&Delete	x
amenu 1.55 PopUp.-SEP2-		:
vmenu 1.60 PopUp.Select\ Blockwise <C-Q>
amenu 1.70 PopUp.Select\ &Word	vaw
amenu 1.80 PopUp.Select\ &Line	V
amenu 1.90 PopUp.Select\ &Block	<C-Q>
amenu 1.100 PopUp.Select\ &All	ggVG

" The GUI toolbar (for MS-Windows and GTK)
if has("win32") || has("win16") || has("gui_gtk") || has("gui_motif")
  amenu 1.10 ToolBar.Open	:browse e<CR>
  amenu 1.20 ToolBar.Save	:w<CR>
  amenu 1.30 ToolBar.SaveAll	:wa<CR>

  if has("win32")
    amenu 1.40 ToolBar.Print	:call Win32Print(":")<CR>
    vunmenu ToolBar.Print
    vmenu ToolBar.Print		<Esc>:call Win32Print(":'<,'>")<CR>
  elseif has("vms")
    amenu 1.40 ToolBar.Print	:call VMSPrint(":")<CR>
    vunmenu ToolBar.Print
    vmenu ToolBar.Print		<Esc>:call VMSPrint(":'<,'>")<CR>
  else
    amenu 1.40 ToolBar.Print	:w !lpr<CR>
    vunmenu ToolBar.Print
    vmenu ToolBar.Print		<Esc>:w !lpr<CR>
  endif

  amenu 1.45 ToolBar.-sep1-	<nul>
  amenu 1.50 ToolBar.Undo	u
  amenu 1.60 ToolBar.Redo	<C-R>

  amenu 1.65 ToolBar.-sep2-	<nul>
  vmenu 1.70 ToolBar.Cut	"*x
  vmenu 1.80 ToolBar.Copy	"*y
  nmenu 1.90 ToolBar.Paste	i<C-R>*<Esc>
  vmenu ToolBar.Paste		"-xi<C-R>*<Esc>
  menu! ToolBar.Paste		<C-R>*

  amenu 1.95 ToolBar.-sep3-	<nul>
  amenu 1.100 ToolBar.Find	:promptfind<CR>
  amenu 1.110 ToolBar.FindNext	n
  amenu 1.120 ToolBar.FindPrev	N
  amenu 1.130 ToolBar.Replace	:promptrepl<CR>
  vunmenu ToolBar.Replace
  vmenu ToolBar.Replace		y:promptrepl <C-R>"<CR>

if 0	" disabled; These are in the Windows menu
  amenu 1.135 ToolBar.-sep4-	<nul>
  amenu 1.140 ToolBar.New	<C-W>n
  amenu 1.150 ToolBar.WinSplit	<C-W>s
  amenu 1.160 ToolBar.WinMax	:resize 200<CR>
  amenu 1.170 ToolBar.WinMin	:resize 1<CR>
  amenu 1.180 ToolBar.WinVSplit	<C-W>v
  amenu 1.190 ToolBar.WinMaxWidth	<C-W>500>
  amenu 1.200 ToolBar.WinMinWidth	<C-W>1\|
  amenu 1.210 ToolBar.WinClose	:close<CR>
endif

  amenu 1.215 ToolBar.-sep5-	<nul>
  amenu 1.220 ToolBar.LoadSesn	:call LoadVimSesn()<CR>
  amenu 1.230 ToolBar.SaveSesn	:call SaveVimSesn()<CR>
  amenu 1.240 ToolBar.RunScript	:browse so<CR>

  amenu 1.245 ToolBar.-sep6-	<nul>
  amenu 1.250 ToolBar.Make	:make<CR>
  amenu 1.260 ToolBar.Shell	:sh<CR>
  amenu 1.270 ToolBar.RunCtags	:!ctags -R .<CR>
  amenu 1.280 ToolBar.TagJump	g]

  amenu 1.295 ToolBar.-sep7-	<nul>
  amenu 1.300 ToolBar.Help	:help<CR>
  if has("gui_gtk")
    amenu 1.310 ToolBar.FindHelp :helpfind<CR>
  else
    amenu 1.310 ToolBar.FindHelp :help 
  endif

" Only set the tooltips here if not done in a language menu file
if exists("*Do_toolbar_tmenu")
  call Do_toolbar_tmenu()
else
  let did_toolbar_tmenu = 1
  tmenu ToolBar.Open		Open file
  tmenu ToolBar.Save		Save current file
  tmenu ToolBar.SaveAll		Save all files
  tmenu ToolBar.Print		Print
  tmenu ToolBar.Undo		Undo
  tmenu ToolBar.Redo		Redo
  tmenu ToolBar.Cut		Cut to clipboard
  tmenu ToolBar.Copy		Copy to clipboard
  tmenu ToolBar.Paste		Paste from Clipboard
  tmenu ToolBar.Find		Find...
  tmenu ToolBar.FindNext	Find Next
  tmenu ToolBar.FindPrev	Find Previous
  tmenu ToolBar.Replace		Find & Replace...
 if 0	" disabled; These are in the Windows menu
  tmenu ToolBar.New		New Window
  tmenu ToolBar.WinSplit	Split Window
  tmenu ToolBar.WinMax		Maximise Window
  tmenu ToolBar.WinMin		Minimise Window
  tmenu ToolBar.WinVSplit	Split Window Vertically
  tmenu ToolBar.WinMaxWidth	Maximise Window Width
  tmenu ToolBar.WinMinWidth	Minimise Window Width
  tmenu ToolBar.WinClose	Close Window
 endif
  tmenu ToolBar.LoadSesn	Load session
  tmenu ToolBar.SaveSesn	Save current session
  tmenu ToolBar.RunScript	Run a Vim Script
  tmenu ToolBar.Make		Make current project
  tmenu ToolBar.Shell		Open a command shell
  tmenu ToolBar.RunCtags	Build tags in current directory tree
  tmenu ToolBar.TagJump		Jump to tag under cursor
  tmenu ToolBar.Help		Vim Help
  tmenu ToolBar.FindHelp	Search Vim Help
endif

" Select a session to load; default to current session name if present
fun! LoadVimSesn()
  if exists("this_session")
    let name = this_session
  else
    let name = "session.vim"
  endif
  execute "browse so " . name
endfun

" Select a session to save; default to current session name if present
fun! SaveVimSesn()
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
  if a:name == "fvwm1"
    let use_fvwm_1 = 1
    let use_fvwm_2 = 0
    let name = "fvwm"
  elseif a:name == "fvwm2"
    let use_fvwm_2 = 1
    let use_fvwm_1 = 0
    let name = "fvwm"
  else
    let name = a:name
  endif
  if !exists("s:syntax_menu_synonly")
    exe "set ft=" . name
    if exists("g:syntax_manual")
      exe "set syn=" . name
    endif
  else
    exe "set syn=" . name
  endif
endfun

" Use the SynMenu command and function to define all menu entries
command -nargs=* SynMenu call <SID>Syn(<q-args>)

let s:cur_menu_name = ""
let s:cur_menu_nr = 0
let s:cur_menu_item = 0
let s:cur_menu_char = ""

fun <SID>Syn(arg)
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
    " Make an exception for "4DOS", which is after "MS-DOS".
    let c = submenu_name[0]
    if c != s:cur_menu_char && c != "4"
      exe 'am 50.' . s:cur_menu_nr . '.' . s:cur_menu_item . ' &Syntax.' . menu_name . ".-" . c . '- <nul>'
      let s:cur_menu_item = s:cur_menu_item + 10
      let s:cur_menu_char = c
    endif
  endif
  exe 'am 50.' . s:cur_menu_nr . '.' . s:cur_menu_item . ' &Syntax.' . menu_name . "." . submenu_name . ' :cal SetSyn("' . syntax_name . '")<CR>'
  let s:cur_menu_item = s:cur_menu_item + 10
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
SynMenu AB.Assembly\ (GNU):asm
SynMenu AB.Assembly\ (H8300):asmh8300
SynMenu AB.Assembly\ (Microsoft):masm
SynMenu AB.Assembly\ (Netwide):nasm
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
SynMenu AB.Blank:blank

SynMenu CD.C:c
SynMenu CD.C++:cpp
SynMenu CD.Cascading\ Style\ Sheets:css
SynMenu CD.Century\ Term:cterm
SynMenu CD.CHILL:ch
SynMenu CD.Change:change
SynMenu CD.Clean:clean
SynMenu CD.Clipper:clipper
SynMenu CD.Cold\ Fusion:cf
SynMenu CD.Configure\ script:config
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
SynMenu CD.Dracula:dracula
SynMenu CD.DTD:dtd
SynMenu CD.Zope\ DTML:dtml
SynMenu CD.Dylan:dylan
SynMenu CD.Dylan\ intr:dylanintr
SynMenu CD.Dylan\ lid:dylanlid

SynMenu EFGH.Eiffel:eiffel
SynMenu EFGH.Elm\ Filter:elmfilt
SynMenu EFGH.ERicsson\ LANGuage:erlang
SynMenu EFGH.ESQL-C:esqlc
SynMenu PQR.Product\ Spec\ File:psf
SynMenu EFGH.Expect:expect
SynMenu EFGH.Exports:exports
SynMenu EFGH.Focus\ Executable:focexec
SynMenu EFGH.Focus\ Master:master
SynMenu EFGH.FORM:form
SynMenu EFGH.Forth:forth
SynMenu EFGH.Fortran:fortran
SynMenu EFGH.Fvwm\ configuration:fvwm1
SynMenu EFGH.Fvwm2\ configuration:fvwm2
SynMenu EFGH.GDB\ command\ file:gdb
SynMenu EFGH.GDMO:gdmo
SynMenu EFGH.Gedcom:gedcom
SynMenu EFGH.GP:gp
SynMenu EFGH.GNUplot:gnuplot
SynMenu EFGH.Haskell:haskell
SynMenu EFGH.Haskell-literal:lhaskell
SynMenu EFGH.HTML:html
SynMenu EFGH.HTML\ with\ M4:htmlm4
SynMenu EFGH.Hyper\ Builder:hb

SynMenu IJKL.Icon:icon
SynMenu IJKL.IDL:idl
SynMenu IJKL.Interactive\ Data\ Lang:idlang
SynMenu IJKL.Inform:inform
SynMenu IJKL.Informix\ 4GL:fgl
SynMenu IJKL.Inno\ Setup:iss
SynMenu IJKL.InstallShield\ Rules:ishd
SynMenu IJKL.Java:java
SynMenu IJKL.JavaCC:javacc
SynMenu IJKL.JavaScript:javascript
SynMenu IJKL.Java\ Server\ Pages:jsp
SynMenu IJKL.Java\ Properties:jproperties
SynMenu IJKL.Jess:jess
SynMenu IJKL.Jgraph:jgraph
SynMenu IJKL.KDE\ script:kscript
SynMenu IJKL.Kimwitu:kwt
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
SynMenu MNO.Matlab:matlab
SynMenu MNO.Maya\ Ext\.\ Lang\.:mel
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
SynMenu MNO.Nroff:nroff
SynMenu MNO.Objective\ C:objc
SynMenu MNO.OCAML:ocaml
SynMenu MNO.Omnimark:omnimark
SynMenu MNO.OpenROAD:openroad
SynMenu MNO.Open\ Psion\ Lang:opl
SynMenu MNO.Oracle\ config:ora

SynMenu PQR.Pascal:pascal
SynMenu PQR.PCCTS:pccts
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
SynMenu PQR.PostScript:postscr
SynMenu PQR.Povray:pov
SynMenu PQR.Printcap:pcap
SynMenu PQR.Procmail:procmail
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
SynMenu S.SGML\ DTD:sgml
SynMenu S.SGML\ linuxdoc:sgmllnx
SynMenu S.Sh\ shell\ script:sh
SynMenu S.SiCAD:sicad
SynMenu S.Simula:simula
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
SynMenu TUV.Tcl/Tk:tcl
SynMenu TUV.TealInfo:tli
SynMenu TUV.Telix\ Salt:tsalt
SynMenu TUV.Termcap:ptcap
SynMenu TUV.TeX:tex
SynMenu TUV.Texinfo:texinfo
SynMenu TUV.TF\ mud\ client:tf
SynMenu TUV.Turbo\ assembly:tasm
SynMenu TUV.UIT/UIL:uil
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
SynMenu WXYZ.X\ Pixmap:xpm
SynMenu WXYZ.X\ Pixmap\ (2):xpm2
SynMenu WXYZ.X\ resources:xdefaults
SynMenu WXYZ.Xmath:xmath
SynMenu WXYZ.XML:xml
SynMenu WXYZ.XXD\ hex\ dump:xxd
SynMenu WXYZ.Yacc:yacc
SynMenu WXYZ.Z-80\ assembler:z8a
SynMenu WXYZ.Zsh\ shell\ script:zsh

unlet s:cur_menu_name
unlet s:cur_menu_nr
unlet s:cur_menu_item
unlet s:cur_menu_char
delcommand SynMenu
delfun <SID>Syn

am 50.95 &Syntax.-SEP1-				:

am 50.100 &Syntax.Set\ 'syntax'\ only		:let s:syntax_menu_synonly=1<CR>
am 50.101 &Syntax.Set\ 'filetype'\ too		:call <SID>Nosynonly()<CR>
fun! <Sid>Nosynonly()
  if exists("s:syntax_menu_synonly")
    unlet s:syntax_menu_synonly
  endif
endfun

am 50.110 &Syntax.&Off			:syn off<CR>
am 50.112 &Syntax.&Manual		:syn manual<CR>
am 50.114 &Syntax.A&utomatic		:syn on<CR>

am 50.116 &Syntax.o&n\ (this\ file)	:call <SID>Synoff()<CR>
fun! <SID>Synoff()
  if !exists("g:syntax_on")
    syn manual
  endif
  set syn=ON
endfun
am 50.118 &Syntax.o&ff\ (this\ file)	:syn clear<CR>

am 50.700 &Syntax.-SEP3-		:
am 50.710 &Syntax.Co&lor\ test		:sp $VIMRUNTIME/syntax/colortest.vim<Bar>so %<CR>
am 50.720 &Syntax.&Highlight\ test	:so $VIMRUNTIME/syntax/hitest.vim<CR>
am 50.730 &Syntax.&Convert\ to\ HTML	:so $VIMRUNTIME/syntax/2html.vim<CR>

endif " !exists("did_install_syntax_menu")

" Restore the previous value of 'cpoptions'.
let &cpo = s:cpo_save
unlet s:cpo_save

" vim: set sw=2 :
