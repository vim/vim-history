" These menu commands create the default Vim menus.
" You can also use this as a start for your own set of menus.
" Note that ":amenu" is often used to make a menu work in all modes.
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 Apr 29

" Make sure the '<' and 'C' flags are not included in 'cpoptions', otherwise
" <CR> would not be recognized.  See ":help 'cpoptions'".
let s:cpo_save = &cpo
set cpo&vim

" Avoid installing the menus twice
if !exists("did_install_default_menus")
let did_install_default_menus = 1


if exists("v:lang") || &langmenu != ""
  " Try to find a menu translation file for the current language.
  if &langmenu != ""
    if &langmenu =~ "none"
      let s:lang = ""
    else
      let s:lang = &langmenu
    endif
  else
    let s:lang = v:lang
  endif
  " A language name must be at least two characters, don't accept "C"
  if strlen(s:lang) > 1
    let s:lang = substitute(s:lang, ".*", "\\L&", "")
    menutrans clear
    exe "runtime! lang/menu_" . s:lang . ".vim"
    " If there is no exact match, try matching with a wildcard added.
    " (e.g. find menu_de_DE.ISO_8859-1.vim if s:lang == de_DE)
    if !exists("did_menu_trans")
      " try to get the first long file name which matches v:lang.
      exe "runtime! lang/menu_" . s:lang . "*.vim"
    endif
  endif
endif


" Help menu
amenu 9999.10 &Help.&Overview<Tab><F1>		:help<CR>
amenu 9999.20 &Help.&How-to\ links		:help how-to<CR>
amenu 9999.30 &Help.&GUI			:help gui<CR>
amenu 9999.40 &Help.&Credits			:help credits<CR>
amenu 9999.50 &Help.Co&pying			:help uganda<CR>
amenu 9999.60 &Help.&Find\.\.\.			:call <SID>Helpfind()<CR>
amenu 9999.65 &Help.-sep-			<nul>
amenu 9999.70 &Help.&Version			:version<CR>
amenu 9999.80 &Help.&About			:intro<CR>

fun! s:Helpfind()
  if !exists("g:find_help_dialog")
    let g:find_help_dialog = "Enter a command or word to find help on:\n\nPrepend i_ for Input mode commands (e.g.: i_CTRL-X)\nPrepend c_ for command-line editing commands (e.g.: c_<Del>)\nPrepend ' for an option name (e.g.: 'shiftwidth')"
  endif
  let h = inputdialog(g:find_help_dialog)
  if h != ""
    let v:errmsg = ""
    silent! exe "help " . h
    if v:errmsg != ""
      echo v:errmsg
    endif
  endif
endfun

" File menu
amenu 10.310 &File.&Open\.\.\.<Tab>:e		:browse confirm e<CR>
amenu 10.320 &File.Sp&lit-Open\.\.\.<Tab>:sp	:browse sp<CR>
amenu 10.325 &File.&New<Tab>:enew		:confirm enew<CR>
amenu 10.330 &File.&Close<Tab>:q		:confirm q<CR>
amenu 10.335 &File.-SEP1-			:
amenu 10.340 &File.&Save<Tab>:w			:confirm w<CR>
amenu 10.350 &File.Save\ &As\.\.\.<Tab>:sav	:browse confirm saveas<CR>

if has("diff")
  amenu 10.400 &File.-SEP2-			:
  amenu 10.410 &File.Show\ &Diff\ with\.\.\.	:browse vert diffsplit<CR>
  amenu 10.420 &File.Show\ &Patched\ by\.\.\.	:browse vert diffpatch<CR>
endif

if has("win32")
  amenu 10.500 &File.-SEP3-			:
  " Use Notepad for printing. ":w >> prn" doesn't work for PostScript printers.
  amenu 10.510 &File.&Print			:call Win32Print(":")<CR>
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
      exec "bdel " . ttt
    endfun
  endif
elseif has("unix")
  amenu 10.500 &File.-SEP3-			:
  amenu 10.510 &File.&Print			:w !lpr<CR>
  vunmenu &File.&Print
  vmenu &File.&Print				:w !lpr<CR>
elseif has("vms")
  amenu 10.500 &File.-SEP3-                     :
  amenu 10.510 &File.&Print			:call VMSPrint(":")<CR>
  vunmenu &File.&Print
  vmenu &File.&Print				<Esc>:call VMSPrint(":'<,'>")<CR>
  if !exists("*VMSPrint")
    fun VMSPrint(range)
      let mod_save = &mod
      let ttt = tempname()
      exec a:range . "w! " . ttt
      let &mod = mod_save
      exec "!print/delete " . ttt
      exec "bdel " . ttt
    endfun
  endif
endif
amenu 10.600 &File.-SEP4-			:
amenu 10.610 &File.Sa&ve-Exit<Tab>:wqa		:confirm wqa<CR>
amenu 10.620 &File.E&xit<Tab>:qa		:confirm qa<CR>


" Tricky stuff to make pasting work as expected.
nnoremap <SID>Paste "=@+.'xy'<CR>gPFx"_2x:echo<CR>

" Edit menu
amenu 20.310 &Edit.&Undo<Tab>u			u
amenu 20.320 &Edit.&Redo<Tab>^R			<C-R>
amenu 20.330 &Edit.Rep&eat<Tab>\.		.
amenu 20.335 &Edit.-SEP1-			:
vmenu 20.340 &Edit.Cu&t<Tab>"+x			"+x
vmenu 20.350 &Edit.&Copy<Tab>"+y		"+y
cmenu 20.350 &Edit.&Copy<Tab>"+y		<C-Y>
nmenu 20.360 &Edit.&Paste<Tab>"+P		<SID>Paste
vmenu	     &Edit.&Paste<Tab>"+P		"-cx<Esc><SID>Paste"_x
imenu	     &Edit.&Paste<Tab>"+P		x<Esc><SID>Paste"_s
cmenu	     &Edit.&Paste<Tab>"+P		<C-R>+
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
amenu 20.430 &Edit.Settings\ &Window		:options<CR>

" Edit/Global Settings
amenu 20.440.100 &Edit.Global\ Settings.Toggle\ Pattern\ Highlight<Tab>:set\ hls!	:set hls! hls?<CR>
amenu 20.440.110 &Edit.Global\ Settings.Toggle\ ignore-case<Tab>:set\ ic!	:set ic! ic?<CR>
amenu 20.440.110 &Edit.Global\ Settings.Toggle\ showmatch<Tab>:set\ sm!	:set sm! sm?<CR>

amenu 20.440.120 &Edit.Global\ Settings.Context\ lines.\ 1\  :set so=1<CR>
amenu 20.440.120 &Edit.Global\ Settings.Context\ lines.\ 2\  :set so=2<CR>
amenu 20.440.120 &Edit.Global\ Settings.Context\ lines.\ 3\  :set so=3<CR>
amenu 20.440.120 &Edit.Global\ Settings.Context\ lines.\ 4\  :set so=4<CR>
amenu 20.440.120 &Edit.Global\ Settings.Context\ lines.\ 5\  :set so=5<CR>
amenu 20.440.120 &Edit.Global\ Settings.Context\ lines.\ 7\  :set so=7<CR>
amenu 20.440.120 &Edit.Global\ Settings.Context\ lines.\ 10\  :set so=10<CR>
amenu 20.440.120 &Edit.Global\ Settings.Context\ lines.\ 100\  :set so=100<CR>

amenu 20.440.130.40 &Edit.Global\ Settings.Virtual\ Edit.Never :set ve=<CR>
amenu 20.440.130.50 &Edit.Global\ Settings.Virtual\ Edit.Block\ Selection :set ve=block<CR>
amenu 20.440.130.60 &Edit.Global\ Settings.Virtual\ Edit.Insert\ mode :set ve=insert<CR>
amenu 20.440.130.70 &Edit.Global\ Settings.Virtual\ Edit.Block\ and\ Insert :set ve=block,insert<CR>
amenu 20.440.130.80 &Edit.Global\ Settings.Virtual\ Edit.Always :set ve=all<CR>

amenu 20.440.140 &Edit.Global\ Settings.Toggle\ Insert\ mode<Tab>:set\ im!	:set im!<CR>

amenu 20.440.150 &Edit.Global\ Settings.Search\ Path\.\.\.  :call <SID>SearchP()<CR>
fun! s:SearchP()
  let n = inputdialog("Enter search path for files.\nSeparate directory names with a comma.", substitute(&path, '\\ ', ' ', 'g'))
  if n != ""
    let &path = substitute(n, ' ', '\\ ', 'g')
  endif
endfun

amenu 20.440.160 &Edit.Global\ Settings.Tag\ Files\.\.\.  :call <SID>TagFiles()<CR>
fun! s:TagFiles()
  let n = inputdialog("Enter names of tag files.\nSeparate the names with a comma.", substitute(&tags, '\\ ', ' ', 'g'))
  if n != ""
    let &tags = substitute(n, ' ', '\\ ', 'g')
  endif
endfun

" GUI options
amenu 20.440.300 &Edit.Global\ Settings.-SEP1-	    :
amenu 20.440.310 &Edit.Global\ Settings.Toggle\ Toolbar		:call <SID>ToggleGuiOption("T")<CR>
amenu 20.440.320 &Edit.Global\ Settings.Toggle\ Bottom\ Scrollbar	:call <SID>ToggleGuiOption("b")<CR>
amenu 20.440.330 &Edit.Global\ Settings.Toggle\ Left\ Scrollbar	:call <SID>ToggleGuiOption("l")<CR>
amenu 20.440.340 &Edit.Global\ Settings.Toggle\ Right\ Scrolbar	:call <SID>ToggleGuiOption("r")<CR>

fun <SID>ToggleGuiOption(option)
    " If a:option is already set in guioptions, then we want to remove it
    if match(&guioptions, a:option) > -1
	exec "set go-=" . a:option
    else
	exec "set go+=" . a:option
    endif
endfun

" Edit/File Settings

" Boolean options
amenu 20.440.100 &Edit.File\ Settings.Toggle\ Line\ Numbering<Tab>:set\ nu!	:set nu! nu?<CR>
amenu 20.440.110 &Edit.File\ Settings.Toggle\ List\ Mode<Tab>:set\ list!	:set list! list?<CR>
amenu 20.440.120 &Edit.File\ Settings.Toggle\ Line\ Wrap<Tab>:set\ wrap!	:set wrap! wrap?<CR>
amenu 20.440.130 &Edit.File\ Settings.Toggle\ Wrap\ at\ word<Tab>:set\ lbr!	:set lbr! lbr?<CR>
amenu 20.440.160 &Edit.File\ Settings.Toggle\ expand-tab<Tab>:set\ et!	:set et! et?<CR>
amenu 20.440.170 &Edit.File\ Settings.Toggle\ auto-indent<Tab>:set\ ai!	:set ai! ai?<CR>
amenu 20.440.180 &Edit.File\ Settings.Toggle\ C-indenting<Tab>:set\ cin!	:set cin! cin?<CR>

" other options
amenu 20.440.600 &Edit.File\ Settings.-SEP2-	    :
amenu 20.440.610.20 &Edit.File\ Settings.Shiftwidth.2   :set sw=2 sw?<CR>
amenu 20.440.610.30 &Edit.File\ Settings.Shiftwidth.3   :set sw=3 sw?<CR>
amenu 20.440.610.40 &Edit.File\ Settings.Shiftwidth.4   :set sw=4 sw?<CR>
amenu 20.440.610.50 &Edit.File\ Settings.Shiftwidth.5   :set sw=5 sw?<CR>
amenu 20.440.610.60 &Edit.File\ Settings.Shiftwidth.6   :set sw=6 sw?<CR>
amenu 20.440.610.80 &Edit.File\ Settings.Shiftwidth.8   :set sw=8 sw?<CR>

amenu 20.440.620.20 &Edit.File\ Settings.Soft\ Tabstop.2   :set sts=2 sts?<CR>
amenu 20.440.620.30 &Edit.File\ Settings.Soft\ Tabstop.3   :set sts=3 sts?<CR>
amenu 20.440.620.40 &Edit.File\ Settings.Soft\ Tabstop.4   :set sts=4 sts?<CR>
amenu 20.440.620.50 &Edit.File\ Settings.Soft\ Tabstop.5   :set sts=5 sts?<CR>
amenu 20.440.620.60 &Edit.File\ Settings.Soft\ Tabstop.6   :set sts=6 sts?<CR>
amenu 20.440.620.80 &Edit.File\ Settings.Soft\ Tabstop.8   :set sts=8 sts?<CR>

amenu 20.440.630 &Edit.File\ Settings.Text\ Width\.\.\.  :call <SID>TextWidth()<CR>
fun! s:TextWidth()
  let n = inputdialog("Enter new text width (0 to disable formatting): ", &tw)
  if n != ""
    " remove leading zeros to avoid it being used as an octal number
    let &tw = substitute(n, "^0*", "", "")
  endif
endfun
amenu 20.440.640 &Edit.File\ Settings.File\ Format\.\.\.  :call <SID>FileFormat()<CR>
fun! s:FileFormat()
  if &ff == "dos"
    let def = 2
  elseif &ff == "mac"
    let def = 3
  else
    let def = 1
  endif
  let n = confirm("Select format for writing the file", "&Unix\n&Dos\n&Mac",
	\ def, "Question")
  if n == 1
    set ff=unix
  elseif n == 2
    set ff=dos
  elseif n == 3
    set ff=mac
  endif
endfun

" Setup the Edit.Color Scheme submenu
if exists("*expandpath")
  let s:n = expandpath("colors/*.vim", &runtimepath)
  let s:idx = 100
  while strlen(s:n) > 0
    let s:i = stridx(s:n, "\n")
    if s:i < 0
      let s:name = s:n
      let s:n = ""
    else
      let s:name = strpart(s:n, 0, s:i)
      let s:n = strpart(s:n, s:i + 1, 19999)
    endif
    let s:name = substitute(s:name, '.*\W\(\w*\)\.vim', '\1', '')
    exe "amenu 20.450." . s:idx . ' &Edit.Color\ Scheme.' . s:name . " :colors " . s:name . "<CR>"
    unlet s:name
    unlet s:i
    let s:idx = s:idx + 10
  endwhile
  unlet s:n
  unlet s:idx
endif

" Setup the Edit.Keymap submenu
if has("keymap")
  amenu 20.460.90 &Edit.Keymap.None :set keymap=<CR>
  let s:n = expandpath("keymap/*.vim", &runtimepath)
  let s:idx = 100
  while strlen(s:n) > 0
    let s:i = stridx(s:n, "\n")
    if s:i < 0
      let s:name = s:n
      let s:n = ""
    else
      let s:name = strpart(s:n, 0, s:i)
      let s:n = strpart(s:n, s:i + 1, 19999)
    endif
    let s:name = substitute(s:name, '.*[/\\:]\([0-9a-zA-Z_-]*\)\.vim', '\1', '')
    exe "amenu 20.460." . s:idx . ' &Edit.Keymap.' . s:name . " :set keymap=" . s:name . "<CR>"
    unlet s:name
    unlet s:i
    let s:idx = s:idx + 10
  endwhile
  unlet s:n
  unlet s:idx
endif

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

" Tools.Fold Menu
if has("folding")
  amenu 40.330 &Tools.-SEP1-			:
  " open close folds 
  amenu 40.340.110 &Tools.&Folding.&Enable/Disable\ folds<Tab>zi	zi
  amenu 40.340.120 &Tools.&Folding.&View\ Cursor\ Line<Tab>zv	zv
  amenu 40.340.120 &Tools.&Folding.Vie&w\ Cursor\ Line\ only<Tab>zMzx	zMzx
  amenu 40.340.130 &Tools.&Folding.C&lose\ more\ folds<Tab>zm	zm
  amenu 40.340.140 &Tools.&Folding.&Close\ all\ folds<Tab>zM	zM
  amenu 40.340.150 &Tools.&Folding.O&pen\ more\ folds<Tab>zr	zr
  amenu 40.340.160 &Tools.&Folding.&Open\ all\ folds<Tab>zR	zR
  " fold method
  amenu 40.340.200 &Tools.&Folding.-SEP1-			:
  amenu 40.340.210 &Tools.&Folding.Fold\ Met&hod.M&anual	:set fdm=manual<CR>
  amenu 40.340.210 &Tools.&Folding.Fold\ Met&hod.I&ndent	:set fdm=indent<CR>
  amenu 40.340.210 &Tools.&Folding.Fold\ Met&hod.E&xpression	:set fdm=expr<CR>
  amenu 40.340.210 &Tools.&Folding.Fold\ Met&hod.S&yntax	:set fdm=syntax<CR>
  amenu 40.340.210 &Tools.&Folding.Fold\ Met&hod.&Diff		:set fdm=diff<CR>
  amenu 40.340.210 &Tools.&Folding.Fold\ Met&hod.Ma&rker	:set fdm=marker<CR>
  " create and delete folds
  vmenu 40.340.220 &Tools.&Folding.Create\ &Fold<Tab>zf		zf
  amenu 40.340.230 &Tools.&Folding.&Delete\ Fold<Tab>zd		zd
  amenu 40.340.240 &Tools.&Folding.Delete\ &All\ Folds<Tab>zD	zD
  " moving around in folds
  amenu 40.340.300 &Tools.&Folding.-SEP2-			:
  amenu 40.340.310.10 &Tools.&Folding.Fold\ column\ &width.\ 0\ 	:set fdc=0<CR>
  amenu 40.340.310.20 &Tools.&Folding.Fold\ column\ &width.\ 2\ 	:set fdc=2<CR>
  amenu 40.340.310.30 &Tools.&Folding.Fold\ column\ &width.\ 3\ 	:set fdc=3<CR>
  amenu 40.340.310.40 &Tools.&Folding.Fold\ column\ &width.\ 4\ 	:set fdc=4<CR>
  amenu 40.340.310.50 &Tools.&Folding.Fold\ column\ &width.\ 5\ 	:set fdc=5<CR>
  amenu 40.340.310.60 &Tools.&Folding.Fold\ column\ &width.\ 6\ 	:set fdc=6<CR>
  amenu 40.340.310.70 &Tools.&Folding.Fold\ column\ &width.\ 7\ 	:set fdc=7<CR>
  amenu 40.340.310.80 &Tools.&Folding.Fold\ column\ &width.\ 8\ 	:set fdc=8<CR>
endif  " has folding

if has("diff")
  amenu 40.350.100 &Tools.&Diff.&Update		:diffupdate<CR>
  amenu 40.350.110 &Tools.&Diff.&Get\ Block 	:diffget<CR>
  amenu 40.350.120 &Tools.&Diff.&Put\ Block 	:diffput<CR>
endif

amenu 40.358 &Tools.-SEP2-			:
amenu 40.360 &Tools.&Make<Tab>:make		:make<CR>
amenu 40.370 &Tools.&List\ Errors<Tab>:cl	:cl<CR>
amenu 40.380 &Tools.L&ist\ Messages<Tab>:cl!	:cl!<CR>
amenu 40.390 &Tools.&Next\ Error<Tab>:cn	:cn<CR>
amenu 40.400 &Tools.&Previous\ Error<Tab>:cp	:cp<CR>
amenu 40.410 &Tools.&Older\ List<Tab>:cold	:colder<CR>
amenu 40.420 &Tools.N&ewer\ List<Tab>:cnew	:cnewer<CR>
amenu 40.430 &Tools.Error\ &Window<Tab>:cwin	:cwin<CR>
amenu 40.520 &Tools.-SEP3-                      :
if has("vms")
  amenu 40.530 &Tools.Convert\ to\ HEX<Tab>:%!mc\ vim:xxd
	\ :let b:mod = &mod<CR>
	\ :%!mc vim:xxd<CR>
	\ :set ft=xxd<CR>
	\ :let &mod = b:mod<CR>
  amenu 40.540 &Tools.Convert\ back<Tab>:%!mc\ vim:xxd\ -r
	\ :let b:mod = &mod<CR>
	\ :%!mc vim:xxd -r<CR>
	\ :doautocmd filetypedetect BufReadPost<CR>
	\ :let &mod = b:mod<CR>
else
  amenu 40.530 &Tools.Convert\ to\ HEX<Tab>:%!xxd
	\ :let b:mod = &mod<CR>
	\ :silent %!xxd<CR>
	\ :set ft=xxd<CR>
	\ :let &mod = b:mod<CR>
  amenu 40.540 &Tools.Convert\ back<Tab>:%!xxd\ -r
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
    let name = expand("<afile>")
    if isdirectory(name)
      return
    endif
    let munge = <SID>BMMunge(name, expand("<abuf>"))

    if s:bmenu_short == 0
      exe 'silent! aun &Buffers.' . munge
    else
      exe 'silent! aun &Buffers.' . <SID>BMHash2(munge) . munge
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
  exe 'am ' . g:bmenu_priority . ".2 &Buffers.&Refresh\\ menu :call <SID>BMShow()<CR>"
  exe 'am ' . g:bmenu_priority . ".4 &Buffers.&Delete :bd<CR>"
  exe 'am ' . g:bmenu_priority . ".6 &Buffers.A&lternate :b #<CR>"
  exe 'am ' . g:bmenu_priority . ".7 &Buffers.&Next :bnext<CR>"
  exe 'am ' . g:bmenu_priority . ".8 &Buffers.&Previous :bprev<CR>"
  exe 'am ' . g:bmenu_priority . ".9 &Buffers.-SEP- :"
  let &cpo = cpo_save

  " figure out how many buffers there are
  let buf = 1
  while buf <= bufnr('$')
    if bufexists(buf) && !isdirectory(bufname(buf))
					    \ && !getbufvar(buf, "&bufsecret")
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
    if bufexists(buf) && !isdirectory(bufname(buf))
					    \ && !getbufvar(buf, "&bufsecret")
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

" insert a buffer name into the buffer menu:
func! <SID>BMFilename(name, num)
  if isdirectory(a:name)
    return
  endif
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
  else
    let name = fnamemodify(name, ':p:~')
  endif
  " detach file name and separate it out:
  let name2 = fnamemodify(name, ':t')
  if a:bnum >= 0
    let name2 = name2 . ' (' . a:bnum . ')'
  endif
  let name = name2 . "\t" . fnamemodify(name,':h')
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
if has("vertsplit")
  amenu 70.332 &Window.Split\ File\ E&xplorer	:call MenuExplOpen()<CR>
  if !exists("*MenuExplOpen")
    fun MenuExplOpen()
      if @% == ""
	20vsp .
      else
	exe "20vsp " . expand("%:p:h")
      endif
      set go+=lr
    endfun
  endif
endif
amenu 70.335 &Window.-SEP1-			:
amenu 70.340 &Window.&Close<Tab>^Wc		:confirm close<CR>
amenu 70.345 &Window.Close\ &Other(s)<Tab>^Wo	:confirm only<CR>
amenu 70.350 &Window.-SEP2-			:
amenu 70.355 &Window.Move\ &To.&Top<Tab>^WK	<C-W>K
amenu 70.355 &Window.Move\ &To.&Bottom<Tab>^WJ	<C-W>J
amenu 70.355 &Window.Move\ &To.&Left\ side<Tab>^WH	<C-W>H
amenu 70.355 &Window.Move\ &To.&Right\ side<Tab>^WL	<C-W>L
amenu 70.360 &Window.Rotate\ &Up<Tab>^WR	<C-W>R
amenu 70.362 &Window.Rotate\ &Down<Tab>^Wr	<C-W>r
amenu 70.365 &Window.-SEP3-			:
amenu 70.370 &Window.&Equal\ Size<Tab>^W=	<C-W>=
amenu 70.380 &Window.&Max\ Height<Tab>^W_	<C-W>_
amenu 70.390 &Window.M&in\ Height<Tab>^W1_	<C-W>1_
amenu 70.400 &Window.Max\ Width<Tab>^W\|	<C-W>\|
amenu 70.410 &Window.Min\ Width<Tab>^W1\|	<C-W>1\|
if has("win32") || has("win16") || has("gui_gtk") || has("qui_photon")
  amenu 70.440 &Window.-SEP4-			:
  amenu 70.450 &Window.Select\ &Font\.\.\.	:set guifont=*<CR>
endif

" The popup menu
amenu 1.10 PopUp.&Undo		u
amenu 1.15 PopUp.-SEP1-		:
vmenu 1.20 PopUp.Cu&t		"+x
vmenu 1.30 PopUp.&Copy		"+y
cmenu 1.30 PopUp.&Copy		<C-Y>
nmenu 1.40 PopUp.&Paste		<SID>Paste
vmenu 1.40 PopUp.&Paste		"-cx<Esc><SID>Paste"_x
imenu 1.40 PopUp.&Paste		x<Esc><SID>Paste"_s
cmenu 1.40 PopUp.&Paste		<C-R>+
vmenu 1.50 PopUp.&Delete	x
amenu 1.55 PopUp.-SEP2-		:
vmenu 1.60 PopUp.Select\ Blockwise <C-Q>
amenu 1.70 PopUp.Select\ &Word	vaw
amenu 1.80 PopUp.Select\ &Line	V
amenu 1.90 PopUp.Select\ &Block	<C-Q>
amenu 1.100 PopUp.Select\ &All	ggVG

" The GUI toolbar (for MS-Windows and GTK)
if has("toolbar")
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
  vmenu 1.70 ToolBar.Cut	"+x
  vmenu 1.80 ToolBar.Copy	"+y
  cmenu 1.80 ToolBar.Copy	<C-Y>
  nmenu 1.90 ToolBar.Paste	<SID>Paste
  vmenu      ToolBar.Paste	"-cx<Esc><SID>Paste"_x
  imenu      ToolBar.Paste	x<Esc><SID>Paste"_s
  cmenu      ToolBar.Paste	<C-R>+

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
  amenu 1.310 ToolBar.FindHelp  :call <SID>Helpfind()<CR>

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
    let c = submenu_name[0]
    if c != s:cur_menu_char
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
SynMenu AB.Assembly.Assembly\ (GNU):asm
SynMenu AB.Assembly.Assembly\ (H8300):asmh8300
SynMenu AB.Assembly.Assembly\ (Intel\ Itanum):ia64
SynMenu AB.Assembly.Assembly\ (Microsoft):masm
SynMenu AB.Assembly.Assembly\ (Netwide):nasm
SynMenu AB.Assembly.PIC\ assembly:pic
SynMenu AB.Assembly.Turbo\ assembly:tasm
SynMenu AB.Assembly.Z-80\ assembly:z8a
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
SynMenu CD.C#:cs
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
SynMenu CD.CUPL:cupl
SynMenu CD.CUPL\ simulation:cuplsim
SynMenu CD.CVS\ commit:cvs
SynMenu CD.CWEB:cweb
SynMenu CD.Debian.Debian\ ChangeLog:debchangelog
SynMenu CD.Debian.Debian\ Control:debcontrol
SynMenu CD.Diff:diff
SynMenu CD.Digital\ Command\ Lang:dcl
SynMenu CD.Diva\ (with\ SKILL):diva
SynMenu CD.DNS:dns
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

SynMenu HIJK.Haskell:haskell
SynMenu HIJK.Haskell-literal:lhaskell
SynMenu HIJK.Hercules:hercules
SynMenu HIJK.HTML:html
SynMenu HIJK.HTML\ with\ M4:htmlm4
SynMenu HIJK.HTML/OS:htmlos
SynMenu HIJK.Hyper\ Builder:hb
SynMenu HIJK.Icon:icon
SynMenu HIJK.IDL:idl
SynMenu HIJK.Interactive\ Data\ Lang:idlang
SynMenu HIJK.Inform:inform
SynMenu HIJK.Informix\ 4GL:fgl
SynMenu HIJK.Inno\ Setup:iss
SynMenu HIJK.InstallShield\ Rules:ishd
SynMenu HIJK.Jam:jam
SynMenu HIJK.Java.Java:java
SynMenu HIJK.Java.JavaCC:javacc
SynMenu HIJK.Java.JavaScript:javascript
SynMenu HIJK.Java.Java\ Server\ Pages:jsp
SynMenu HIJK.Java.Java\ Properties:jproperties
SynMenu HIJK.Jess:jess
SynMenu HIJK.Jgraph:jgraph
SynMenu HIJK.KDE\ script:kscript
SynMenu HIJK.Kimwitu:kwt
SynMenu HIJK.Kixtart:kix

SynMenu L-Ma.Lace:lace
SynMenu L-Ma.Lamda\ Prolog:lprolog
SynMenu L-Ma.Latte:latte
SynMenu L-Ma.Lex:lex
SynMenu L-Ma.Lilo:lilo
SynMenu L-Ma.Lisp:lisp
SynMenu L-Ma.Lite:lite
SynMenu L-Ma.LOTOS:lotos
SynMenu L-Ma.Lout:lout
SynMenu L-Ma.Lua:lua
SynMenu L-Ma.Lynx\ Style:lss
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
SynMenu Me-NO.Muttrc:muttrc
SynMenu Me-NO.Nastran\ input/DMAP:nastran
SynMenu Me-NO.Novell\ batch:ncf
SynMenu Me-NO.Not\ Quite\ C:nqc
SynMenu Me-NO.Nroff:nroff
SynMenu Me-NO.Objective\ C:objc
SynMenu Me-NO.OCAML:ocaml
SynMenu Me-NO.Omnimark:omnimark
SynMenu Me-NO.OpenROAD:openroad
SynMenu Me-NO.Open\ Psion\ Lang:opl
SynMenu Me-NO.Oracle\ config:ora

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
SynMenu PQ.PL/SQL:plsql
SynMenu PQ.PO\ (GNU\ gettext):po
SynMenu PQ.PostScript:postscr
SynMenu PQ.Povray:pov
SynMenu PQ.Printcap:pcap
SynMenu PQ.Procmail:procmail
SynMenu PQ.Product\ Spec\ File:psf
SynMenu PQ.Progress:progress
SynMenu PQ.Prolog:prolog
SynMenu PQ.Purify\ log:purifylog
SynMenu PQ.Python:python

SynMenu R-Sg.R:r
SynMenu R-Sg.Radiance:radiance
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
SynMenu Sh-S.SPEC\ (Linux\ RPM):spec
SynMenu Sh-S.Spice:spice
SynMenu Sh-S.Speedup:spup
SynMenu Sh-S.Squid:squid
SynMenu Sh-S.SQL:sql
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
SynMenu TUV.TeX:tex
SynMenu TUV.TeX\ configuration:texmf
SynMenu TUV.Texinfo:texinfo
SynMenu TUV.TF\ mud\ client:tf
SynMenu TUV.Trasys\ input:trasys
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

unlet s:cur_menu_name
unlet s:cur_menu_nr
unlet s:cur_menu_item
unlet s:cur_menu_char
delcommand SynMenu
delfun <SID>Syn

am 50.195 &Syntax.-SEP1-				:

am 50.200 &Syntax.Set\ 'syntax'\ only		:let s:syntax_menu_synonly=1<CR>
am 50.202 &Syntax.Set\ 'filetype'\ too		:call <SID>Nosynonly()<CR>
fun! <Sid>Nosynonly()
  if exists("s:syntax_menu_synonly")
    unlet s:syntax_menu_synonly
  endif
endfun

am 50.210 &Syntax.&Off			:syn off<CR>
am 50.212 &Syntax.&Manual		:syn manual<CR>
am 50.214 &Syntax.A&utomatic		:syn on<CR>

am 50.216 &Syntax.o&n\ (this\ file)	:call <SID>Synoff()<CR>
fun! <SID>Synoff()
  if !exists("g:syntax_on")
    syn manual
  endif
  set syn=ON
endfun
am 50.218 &Syntax.o&ff\ (this\ file)	:syn clear<CR>

am 50.700 &Syntax.-SEP3-		:
am 50.710 &Syntax.Co&lor\ test		:sp $VIMRUNTIME/syntax/colortest.vim<Bar>so %<CR>
am 50.720 &Syntax.&Highlight\ test	:so $VIMRUNTIME/syntax/hitest.vim<CR>
am 50.730 &Syntax.&Convert\ to\ HTML	:so $VIMRUNTIME/syntax/2html.vim<CR>

endif " !exists("did_install_syntax_menu")

" Restore the previous value of 'cpoptions'.
let &cpo = s:cpo_save
unlet s:cpo_save

" vim: set sw=2 :
