" You can also use this as a start for your own set of menus.
" Note that ":amenu" is often used to make a menu work in all modes.
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 Sep 21

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
    " We always use a lowercase name.
    " Change "iso-8859" to "iso_8859", some systems appear to use this.
    " Change spaces to underscores.
    let s:lang = substitute(tolower(s:lang), "\\.iso-", "\\.iso_", "")
    let s:lang = substitute(s:lang, " ", "_", "g")
    menutrans clear
    exe "runtime! lang/menu_" . s:lang . ".vim"

    if !exists("did_menu_trans")
      " There is no exact match, try matching with a wildcard added
      " (e.g. find menu_de_de.iso_8859-1.vim if s:lang == de_DE).
      exe "runtime! lang/menu_" . s:lang . "*.vim"

      if !exists("did_menu_trans") && strlen($LANG) > 1
	" On windows locale names are complicated, try using $LANG, it might
	" have been set by set_init_1().
	exe "runtime! lang/menu_" . tolower($LANG) . "*.vim"
      endif
    endif
  endif
endif


" Help menu
amenu 9999.10 &Help.&Overview<Tab><F1>		:help<CR>
amenu 9999.20 &Help.&User\ Manual		:help usr_toc<CR>
amenu 9999.30 &Help.&How-to\ links		:help how-to<CR>
amenu 9999.40 &Help.&GUI			:help gui<CR>
amenu 9999.50 &Help.&Credits			:help credits<CR>
amenu 9999.60 &Help.Co&pying			:help uganda<CR>
amenu <silent> 9999.70 &Help.&Find\.\.\.	:call <SID>Helpfind()<CR>
amenu 9999.75 &Help.-sep-			<nul>
amenu 9999.80 &Help.&Version			:version<CR>
amenu 9999.90 &Help.&About			:intro<CR>

fun! s:Helpfind()
  if !exists("g:menutrans_help_dialog")
    let g:menutrans_help_dialog = "Enter a command or word to find help on:\n\nPrepend i_ for Input mode commands (e.g.: i_CTRL-X)\nPrepend c_ for command-line editing commands (e.g.: c_<Del>)\nPrepend ' for an option name (e.g.: 'shiftwidth')"
  endif
  let h = inputdialog(g:menutrans_help_dialog)
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
amenu <silent> 10.330 &File.&Close<Tab>:close
	\ :if winheight(2) < 0 <Bar>
	\   confirm enew <Bar>
	\ else <Bar>
	\   confirm close <Bar>
	\ endif<CR>
amenu 10.335 &File.-SEP1-			:
amenu <silent> 10.340 &File.&Save<Tab>:w	:if expand("%") == ""<Bar>browse confirm w<Bar>else<Bar>confirm w<Bar>endif<CR>
amenu 10.350 &File.Save\ &As\.\.\.<Tab>:sav	:browse confirm saveas<CR>

if has("diff")
  amenu 10.400 &File.-SEP2-			:
  amenu 10.410 &File.Split\ &Diff\ with\.\.\.	:browse vert diffsplit<CR>
  amenu 10.420 &File.Split\ Patched\ &By\.\.\.	:browse vert diffpatch<CR>
endif

if has("printer")
  amenu 10.500 &File.-SEP3-			:
  amenu 10.510 &File.&Print			:hardcopy<CR>
  vunmenu &File.&Print
  vmenu &File.&Print				:hardcopy<CR>
elseif has("unix")
  amenu 10.500 &File.-SEP3-			:
  amenu 10.510 &File.&Print			:w !lpr<CR>
  vunmenu &File.&Print
  vmenu &File.&Print				:w !lpr<CR>
elseif has("vms")
  amenu 10.500 &File.-SEP3-			:
  amenu <silent> 10.510 &File.&Print		:call VMSPrint(":")<CR>
  vunmenu &File.&Print
  vmenu <silent> &File.&Print			<Esc>:call VMSPrint(":'<,'>")<CR>
endif
amenu 10.600 &File.-SEP4-			:
amenu 10.610 &File.Sa&ve-Exit<Tab>:wqa		:confirm wqa<CR>
amenu 10.620 &File.E&xit<Tab>:qa		:confirm qa<CR>

if has("vms")
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
amenu <silent> 20.400 &Edit.&Select\ all<Tab>ggVG	:if &slm != ""<Bar>exe ":norm gggH<C-O>G"<Bar>else<Bar>exe ":norm ggVG"<Bar>endif<CR>
amenu 20.405 &Edit.-SEP2-			:
if has("win32")  || has("win16") || has("gui_gtk") || has("gui_motif")
  amenu 20.410 &Edit.&Find\.\.\.		:promptfind<CR>
  vunmenu      &Edit.&Find\.\.\.
  vmenu	       &Edit.&Find\.\.\.		y:promptfind <C-R>"<CR>
  amenu 20.420 &Edit.Find\ and\ Rep&lace\.\.\.	:promptrepl<CR>
  vunmenu      &Edit.Find\ and\ Rep&lace\.\.\.
  vmenu	       &Edit.Find\ and\ Rep&lace\.\.\.	y:promptrepl <C-R>"<CR>
else
  amenu 20.410 &Edit.&Find<Tab>/			/
  amenu 20.420 &Edit.Find\ and\ Rep&lace<Tab>:%s	:%s/
  vunmenu      &Edit.Find\ and\ Rep&lace<Tab>:%s
  vmenu	       &Edit.Find\ and\ Rep&lace<Tab>:s		:s/
endif
amenu 20.425 &Edit.-SEP3-			:
amenu 20.430 &Edit.Settings\ &Window		:options<CR>

" Edit/Global Settings
amenu 20.440.100 &Edit.&Global\ Settings.Toggle\ Pattern\ &Highlight<Tab>:set\ hls!	:set hls! hls?<CR>
amenu 20.440.110 &Edit.&Global\ Settings.Toggle\ &Ignore-case<Tab>:set\ ic!	:set ic! ic?<CR>
amenu 20.440.110 &Edit.&Global\ Settings.Toggle\ &Showmatch<Tab>:set\ sm!	:set sm! sm?<CR>

amenu 20.440.120 &Edit.&Global\ Settings.&Context\ lines.\ 1\  :set so=1<CR>
amenu 20.440.120 &Edit.&Global\ Settings.&Context\ lines.\ 2\  :set so=2<CR>
amenu 20.440.120 &Edit.&Global\ Settings.&Context\ lines.\ 3\  :set so=3<CR>
amenu 20.440.120 &Edit.&Global\ Settings.&Context\ lines.\ 4\  :set so=4<CR>
amenu 20.440.120 &Edit.&Global\ Settings.&Context\ lines.\ 5\  :set so=5<CR>
amenu 20.440.120 &Edit.&Global\ Settings.&Context\ lines.\ 7\  :set so=7<CR>
amenu 20.440.120 &Edit.&Global\ Settings.&Context\ lines.\ 10\  :set so=10<CR>
amenu 20.440.120 &Edit.&Global\ Settings.&Context\ lines.\ 100\  :set so=100<CR>

amenu 20.440.130.40 &Edit.&Global\ Settings.&Virtual\ Edit.Never :set ve=<CR>
amenu 20.440.130.50 &Edit.&Global\ Settings.&Virtual\ Edit.Block\ Selection :set ve=block<CR>
amenu 20.440.130.60 &Edit.&Global\ Settings.&Virtual\ Edit.Insert\ mode :set ve=insert<CR>
amenu 20.440.130.70 &Edit.&Global\ Settings.&Virtual\ Edit.Block\ and\ Insert :set ve=block,insert<CR>
amenu 20.440.130.80 &Edit.&Global\ Settings.&Virtual\ Edit.Always :set ve=all<CR>
amenu 20.440.140 &Edit.&Global\ Settings.Toggle\ Insert\ &Mode<Tab>:set\ im!	:set im!<CR>
amenu <silent> 20.440.150 &Edit.&Global\ Settings.Search\ &Path\.\.\.  :call <SID>SearchP()<CR>
amenu <silent> 20.440.160 &Edit.&Global\ Settings.Ta&g\ Files\.\.\.  :call <SID>TagFiles()<CR>
"
" GUI options
amenu 20.440.300 &Edit.&Global\ Settings.-SEP1-	    :
amenu <silent> 20.440.310 &Edit.&Global\ Settings.Toggle\ &Toolbar		:call <SID>ToggleGuiOption("T")<CR>
amenu <silent> 20.440.320 &Edit.&Global\ Settings.Toggle\ &Bottom\ Scrollbar	:call <SID>ToggleGuiOption("b")<CR>
amenu <silent> 20.440.330 &Edit.&Global\ Settings.Toggle\ &Left\ Scrollbar	:call <SID>ToggleGuiOption("l")<CR>
amenu <silent> 20.440.340 &Edit.&Global\ Settings.Toggle\ &Right\ Scrollbar	:call <SID>ToggleGuiOption("r")<CR>

fun! s:SearchP()
  if !exists("g:menutrans_path_dialog")
    let g:menutrans_path_dialog = "Enter search path for files.\nSeparate directory names with a comma."
  endif
  let n = inputdialog(g:menutrans_path_dialog, substitute(&path, '\\ ', ' ', 'g'))
  if n != ""
    let &path = substitute(n, ' ', '\\ ', 'g')
  endif
endfun

fun! s:TagFiles()
  if !exists("g:menutrans_tags_dialog")
    let g:menutrans_tags_dialog = "Enter names of tag files.\nSeparate the names with a comma."
  endif
  let n = inputdialog(g:menutrans_tags_dialog, substitute(&tags, '\\ ', ' ', 'g'))
  if n != ""
    let &tags = substitute(n, ' ', '\\ ', 'g')
  endif
endfun

fun! s:ToggleGuiOption(option)
    " If a:option is already set in guioptions, then we want to remove it
    if match(&guioptions, a:option) > -1
	exec "set go-=" . a:option
    else
	exec "set go+=" . a:option
    endif
endfun

" Edit/File Settings

" Boolean options
amenu 20.440.100 &Edit.F&ile\ Settings.Toggle\ Line\ &Numbering<Tab>:set\ nu!	:set nu! nu?<CR>
amenu 20.440.110 &Edit.F&ile\ Settings.Toggle\ &List\ Mode<Tab>:set\ list!	:set list! list?<CR>
amenu 20.440.120 &Edit.F&ile\ Settings.Toggle\ Line\ &Wrap<Tab>:set\ wrap!	:set wrap! wrap?<CR>
amenu 20.440.130 &Edit.F&ile\ Settings.Toggle\ W&rap\ at\ word<Tab>:set\ lbr!	:set lbr! lbr?<CR>
amenu 20.440.160 &Edit.F&ile\ Settings.Toggle\ &expand-tab<Tab>:set\ et!	:set et! et?<CR>
amenu 20.440.170 &Edit.F&ile\ Settings.Toggle\ &auto-indent<Tab>:set\ ai!	:set ai! ai?<CR>
amenu 20.440.180 &Edit.F&ile\ Settings.Toggle\ &C-indenting<Tab>:set\ cin!	:set cin! cin?<CR>

" other options
amenu 20.440.600 &Edit.F&ile\ Settings.-SEP2-	    :
amenu 20.440.610.20 &Edit.F&ile\ Settings.&Shiftwidth.2   :set sw=2 sw?<CR>
amenu 20.440.610.30 &Edit.F&ile\ Settings.&Shiftwidth.3   :set sw=3 sw?<CR>
amenu 20.440.610.40 &Edit.F&ile\ Settings.&Shiftwidth.4   :set sw=4 sw?<CR>
amenu 20.440.610.50 &Edit.F&ile\ Settings.&Shiftwidth.5   :set sw=5 sw?<CR>
amenu 20.440.610.60 &Edit.F&ile\ Settings.&Shiftwidth.6   :set sw=6 sw?<CR>
amenu 20.440.610.80 &Edit.F&ile\ Settings.&Shiftwidth.8   :set sw=8 sw?<CR>

amenu 20.440.620.20 &Edit.F&ile\ Settings.Soft\ &Tabstop.2   :set sts=2 sts?<CR>
amenu 20.440.620.30 &Edit.F&ile\ Settings.Soft\ &Tabstop.3   :set sts=3 sts?<CR>
amenu 20.440.620.40 &Edit.F&ile\ Settings.Soft\ &Tabstop.4   :set sts=4 sts?<CR>
amenu 20.440.620.50 &Edit.F&ile\ Settings.Soft\ &Tabstop.5   :set sts=5 sts?<CR>
amenu 20.440.620.60 &Edit.F&ile\ Settings.Soft\ &Tabstop.6   :set sts=6 sts?<CR>
amenu 20.440.620.80 &Edit.F&ile\ Settings.Soft\ &Tabstop.8   :set sts=8 sts?<CR>

amenu <silent> 20.440.630 &Edit.F&ile\ Settings.Te&xt\ Width\.\.\.  :call <SID>TextWidth()<CR>
amenu <silent> 20.440.640 &Edit.F&ile\ Settings.&File\ Format\.\.\.  :call <SID>FileFormat()<CR>
fun! s:TextWidth()
  if !exists("g:menutrans_textwidth_dialog")
    let g:menutrans_textwidth_dialog = "Enter new text width (0 to disable formatting): "
  endif
  let n = inputdialog(g:menutrans_textwidth_dialog, &tw)
  if n != ""
    " remove leading zeros to avoid it being used as an octal number
    let &tw = substitute(n, "^0*", "", "")
  endif
endfun

fun! s:FileFormat()
  if !exists("g:menutrans_fileformat_dialog")
    let g:menutrans_fileformat_dialog = "Select format for writing the file"
  endif
  if &ff == "dos"
    let def = 2
  elseif &ff == "mac"
    let def = 3
  else
    let def = 1
  endif
  let n = confirm(g:menutrans_fileformat_dialog, "&Unix\n&Dos\n&Mac\n&Cancel", def, "Question")
  if n == 1
    set ff=unix
  elseif n == 2
    set ff=dos
  elseif n == 3
    set ff=mac
  endif
endfun

" Setup the Edit.Color Scheme submenu
let s:n = globpath(&runtimepath, "colors/*.vim")
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
  let s:name = substitute(s:name, '.*[/\\:]\([^/\\:]*\)\.vim', '\1', '')
  exe "amenu 20.450." . s:idx . ' &Edit.C&olor\ Scheme.' . s:name . " :colors " . s:name . "<CR>"
  unlet s:name
  unlet s:i
  let s:idx = s:idx + 10
endwhile
unlet s:n
unlet s:idx

" Setup the Edit.Keymap submenu
if has("keymap")
  let s:n = globpath(&runtimepath, "keymap/*.vim")
  if s:n != ""
    let s:idx = 100
    amenu 20.460.90 &Edit.&Keymap.None :set keymap=<CR>
    while strlen(s:n) > 0
      let s:i = stridx(s:n, "\n")
      if s:i < 0
	let s:name = s:n
	let s:n = ""
      else
	let s:name = strpart(s:n, 0, s:i)
	let s:n = strpart(s:n, s:i + 1, 19999)
      endif
      let s:name = substitute(s:name, '.*[/\\:]\([^/\\:_]*\)\(_[0-9a-zA-Z-]*\)\=\.vim', '\1', '')
      exe "amenu 20.460." . s:idx . ' &Edit.&Keymap.' . s:name . " :set keymap=" . s:name . "<CR>"
      unlet s:name
      unlet s:i
      let s:idx = s:idx + 10
    endwhile
    unlet s:idx
  endif
  unlet s:n
endif
if has("win32") || has("win16") || has("gui_gtk") || has("gui_photon")
  amenu 20.470 &Edit.Select\ Fo&nt\.\.\.	:set guifont=*<CR>
endif

" Programming menu
amenu 40.300 &Tools.&Jump\ to\ this\ tag<Tab>g^] g<C-]>
vunmenu &Tools.&Jump\ to\ this\ tag<Tab>g^]
vmenu &Tools.&Jump\ to\ this\ tag<Tab>g^]	g<C-]>
amenu 40.310 &Tools.Jump\ &back<Tab>^T		<C-T>
if has("vms")
  amenu 40.320 &Tools.Build\ &Tags\ File		:!mc vim:ctags .<CR>
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
  amenu 40.350.110 &Tools.&Diff.&Get\ Block	:diffget<CR>
  vunmenu &Tools.&Diff.&Get\ Block
  vmenu &Tools.&Diff.&Get\ Block		:diffget<CR>
  amenu 40.350.120 &Tools.&Diff.&Put\ Block	:diffput<CR>
  vunmenu &Tools.&Diff.&Put\ Block
  vmenu &Tools.&Diff.&Put\ Block		:diffput<CR>
endif

amenu 40.358 &Tools.-SEP2-				:
amenu 40.360 &Tools.&Make<Tab>:make			:make<CR>
amenu 40.370 &Tools.&List\ Errors<Tab>:cl		:cl<CR>
amenu 40.380 &Tools.L&ist\ Messages<Tab>:cl!		:cl!<CR>
amenu 40.390 &Tools.&Next\ Error<Tab>:cn		:cn<CR>
amenu 40.400 &Tools.&Previous\ Error<Tab>:cp		:cp<CR>
amenu 40.410 &Tools.&Older\ List<Tab>:cold		:colder<CR>
amenu 40.420 &Tools.N&ewer\ List<Tab>:cnew		:cnewer<CR>
amenu 40.430.50 &Tools.Error\ &Window.&Update<Tab>:cwin	:cwin<CR>
amenu 40.430.60 &Tools.Error\ &Window.&Open<Tab>:copen	:copen<CR>
amenu 40.430.70 &Tools.Error\ &Window.&Close<Tab>:cclose :cclose<CR>
amenu 40.520 &Tools.-SEP3-			:
amenu <silent> 40.530 &Tools.&Convert\ to\ HEX<Tab>:%!xxd
	\ :call <SID>XxdConv()<CR>
amenu <silent> 40.540 &Tools.Conve&rt\ back<Tab>:%!xxd\ -r
	\ :call <SID>XxdBack()<CR>

" Use a function to do the conversion, so that it also works with 'insertmode'
" set.
func! s:XxdConv()
  let mod = &mod
  if has("vms")
    %!mc vim:xxd
  else
    %!xxd
  endif
  set ft=xxd
  let &mod = mod
endfun

func! s:XxdBack()
  let mod = &mod
  if has("vms")
    %!mc vim:xxd -r
  else
    %!xxd -r
  endif
  doautocmd filetypedetect BufReadPost
  let &mod = mod
endfun

" Setup the Tools.Compiler submenu
let s:n = globpath(&runtimepath, "compiler/*.vim")
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
  let s:name = substitute(s:name, '.*[/\\:]\([^/\\:]*\)\.vim', '\1', '')
  exe "amenu 30.440." . s:idx . ' &Tools.&Set\ Compiler.' . s:name . " :compiler " . s:name . "<CR>"
  unlet s:name
  unlet s:i
  let s:idx = s:idx + 10
endwhile
unlet s:n
unlet s:idx

if !exists("no_buffers_menu")

" Buffer list menu -- Setup functions & actions

" wait with building the menu until after loading 'session' files. Makes
" startup faster.
let s:bmenu_wait = 1

if !exists("bmenu_priority")
  let bmenu_priority = 60
endif

func! s:BMAdd()
  if s:bmenu_wait == 0
    " when adding too many buffers, redraw in short format
    if s:bmenu_count == &menuitems && s:bmenu_short == 0
      call s:BMShow()
    else
      call <SID>BMFilename(expand("<afile>"), expand("<abuf>"))
      let s:bmenu_count = s:bmenu_count + 1
    endif
  endif
endfunc

func! s:BMRemove()
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

" Create the buffer menu (delete an existing one first).
func! s:BMShow(...)
  let s:bmenu_wait = 1
  let s:bmenu_short = 1
  let s:bmenu_count = 0
  "
  " get new priority, if exists
  if a:0 == 1
    let g:bmenu_priority = a:1
  endif

  " remove old menu, if exists; keep one entry to avoid a torn off menu to
  " disappear.
  silent! unmenu &Buffers
  exe 'menu ' . g:bmenu_priority . ".1 &Buffers.Dummy l"
  silent! unmenu! &Buffers

  " create new menu; set 'cpo' to include the <CR>
  let cpo_save = &cpo
  set cpo&vim
  exe 'am <silent> ' . g:bmenu_priority . ".2 &Buffers.&Refresh\\ menu :call <SID>BMShow()<CR>"
  exe 'am ' . g:bmenu_priority . ".4 &Buffers.&Delete :bd<CR>"
  exe 'am ' . g:bmenu_priority . ".6 &Buffers.&Alternate :b #<CR>"
  exe 'am ' . g:bmenu_priority . ".7 &Buffers.&Next :bnext<CR>"
  exe 'am ' . g:bmenu_priority . ".8 &Buffers.&Previous :bprev<CR>"
  exe 'am ' . g:bmenu_priority . ".9 &Buffers.-SEP- :"
  let &cpo = cpo_save
  unmenu &Buffers.Dummy

  " figure out how many buffers there are
  let buf = 1
  while buf <= bufnr('$')
    if bufexists(buf) && !isdirectory(bufname(buf)) && buflisted(buf)
					    \ && !getbufvar(buf, "&bufsecret")
      let s:bmenu_count = s:bmenu_count + 1
    endif
    let buf = buf + 1
  endwhile
  if s:bmenu_count <= &menuitems
    let s:bmenu_short = 0
  endif

  " iterate through buffer list, adding each buffer to the menu:
  let buf = 1
  while buf <= bufnr('$')
    if bufexists(buf) && !isdirectory(bufname(buf)) && buflisted(buf)
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

func! s:BMHash(name)
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
  return (char2nr(nm[0]) - sp) * 0x800000 + (char2nr(nm[1]) - sp) * 0x20000 + (char2nr(nm[2]) - sp) * 0x1000 + (char2nr(nm[3]) - sp) * 0x80 + (char2nr(nm[4]) - sp) * 0x20 + (char2nr(nm[5]) - sp)
endfunc

func! s:BMHash2(name)
  let nm = substitute(a:name, ".", '\L\0', "")
  " Not exactly right for EBCDIC...
  if nm[0] < 'a' || nm[0] > 'z'
    return '&others.'
  elseif nm[0] <= 'd'
    return '&abcd.'
  elseif nm[0] <= 'h'
    return '&efgh.'
  elseif nm[0] <= 'l'
    return '&ijkl.'
  elseif nm[0] <= 'p'
    return '&mnop.'
  elseif nm[0] <= 't'
    return '&qrst.'
  else
    return '&u-z.'
  endif
endfunc

" insert a buffer name into the buffer menu:
func! s:BMFilename(name, num)
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

" Truncate a long path to fit it in a menu item.
if !exists("g:bmenu_max_pathlen")
  let g:bmenu_max_pathlen = 35
endif
func! s:BMTruncName(fname)
  let name = a:fname
  if g:bmenu_max_pathlen < 5
    let name = ""
  else
    let len = strlen(name)
    if len > g:bmenu_max_pathlen
      let amount = (g:bmenu_max_pathlen / 2) - 2
      let left = strpart(name, 0, amount)
      let amount = g:bmenu_max_pathlen - amount - 3
      let right = strpart(name, len - amount, amount)
      let name = left . '...' . right
    endif
  endif
  return name
endfunc

func! s:BMMunge(fname, bnum)
  let name = a:fname
  if name == ''
    if !exists("g:menutrans_no_file")
      let g:menutrans_no_file = "[No file]"
    endif
    let name = g:menutrans_no_file
  else
    let name = fnamemodify(name, ':p:~')
  endif
  " detach file name and separate it out:
  let name2 = fnamemodify(name, ':t')
  if a:bnum >= 0
    let name2 = name2 . ' (' . a:bnum . ')'
  endif
  let name = name2 . "\t" . <SID>BMTruncName(fnamemodify(name,':h'))
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
  amenu <silent> 70.332 &Window.Split\ File\ E&xplorer	:call MenuExplOpen()<CR>
  if !exists("*MenuExplOpen")
    fun MenuExplOpen()
      if @% == ""
	20vsp .
      else
	exe "20vsp " . expand("%:p:h")
      endif
    endfun
  endif
endif
amenu 70.335 &Window.-SEP1-				:
amenu 70.340 &Window.&Close<Tab>^Wc			:confirm close<CR>
amenu 70.345 &Window.Close\ &Other(s)<Tab>^Wo		:confirm only<CR>
amenu 70.350 &Window.-SEP2-				:
amenu 70.355 &Window.Move\ &To.&Top<Tab>^WK		<C-W>K
amenu 70.355 &Window.Move\ &To.&Bottom<Tab>^WJ		<C-W>J
amenu 70.355 &Window.Move\ &To.&Left\ side<Tab>^WH	<C-W>H
amenu 70.355 &Window.Move\ &To.&Right\ side<Tab>^WL	<C-W>L
amenu 70.360 &Window.Rotate\ &Up<Tab>^WR		<C-W>R
amenu 70.362 &Window.Rotate\ &Down<Tab>^Wr		<C-W>r
amenu 70.365 &Window.-SEP3-				:
amenu 70.370 &Window.&Equal\ Size<Tab>^W=		<C-W>=
amenu 70.380 &Window.&Max\ Height<Tab>^W_		<C-W>_
amenu 70.390 &Window.M&in\ Height<Tab>^W1_		<C-W>1_
amenu 70.400 &Window.Max\ &Width<Tab>^W\|		<C-W>\|
amenu 70.410 &Window.Min\ Widt&h<Tab>^W1\|		<C-W>1\|

" The popup menu
amenu 1.10 PopUp.&Undo			u
amenu 1.15 PopUp.-SEP1-			:
vmenu 1.20 PopUp.Cu&t			"+x
vmenu 1.30 PopUp.&Copy			"+y
cmenu 1.30 PopUp.&Copy			<C-Y>
nmenu 1.40 PopUp.&Paste			<SID>Paste
vmenu 1.40 PopUp.&Paste			"-cx<Esc><SID>Paste"_x
imenu 1.40 PopUp.&Paste			x<Esc><SID>Paste"_s
cmenu 1.40 PopUp.&Paste			<C-R>+
vmenu 1.50 PopUp.&Delete		x
amenu 1.55 PopUp.-SEP2-			:
vnoremenu 1.60 PopUp.Select\ Blockwise	<C-Q>
anoremenu 1.70 PopUp.Select\ &Word	vaw
anoremenu 1.80 PopUp.Select\ &Line	V
anoremenu 1.90 PopUp.Select\ &Block	<C-Q>
anoremenu 1.100 PopUp.Select\ &All	ggVG

" The GUI toolbar (for MS-Windows and GTK)
if has("toolbar")
  amenu 1.10 ToolBar.Open		:browse confirm e<CR>
  amenu <silent> 1.20 ToolBar.Save	:if expand("%") == ""<Bar>browse confirm w<Bar>else<Bar>confirm w<Bar>endif<CR>
  amenu 1.30 ToolBar.SaveAll		:wa<CR>

  if has("printer")
    amenu 1.40 ToolBar.Print	:hardcopy<CR>
    vunmenu ToolBar.Print
    vmenu ToolBar.Print		:hardcopy<CR>
  elseif has("unix")
    amenu 1.40 ToolBar.Print	:w !lpr<CR>
    vunmenu ToolBar.Print
    vmenu ToolBar.Print		:w !lpr<CR>
  elseif has("vms")
    amenu <silent> 1.40 ToolBar.Print	:call VMSPrint(":")<CR>
    vunmenu ToolBar.Print
    vmenu <silent> ToolBar.Print	<Esc>:call VMSPrint(":'<,'>")<CR>
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

  if !has("gui_athena")
    amenu 1.95 ToolBar.-sep3-		<nul>
    amenu 1.100 ToolBar.Find		:promptfind<CR>
    vunmenu ToolBar.Find
    vmenu ToolBar.Find			y:promptfind <C-R>"<CR>
    amenu 1.110 ToolBar.FindNext	n
    amenu 1.120 ToolBar.FindPrev	N
    amenu 1.130 ToolBar.Replace		:promptrepl<CR>
    vunmenu ToolBar.Replace
    vmenu ToolBar.Replace		y:promptrepl <C-R>"<CR>
  endif

if 0	" disabled; These are in the Windows menu
  amenu 1.135 ToolBar.-sep4-		<nul>
  amenu 1.140 ToolBar.New		<C-W>n
  amenu 1.150 ToolBar.WinSplit		<C-W>s
  amenu 1.160 ToolBar.WinMax		:resize 200<CR>
  amenu 1.170 ToolBar.WinMin		:resize 1<CR>
  amenu 1.180 ToolBar.WinVSplit		<C-W>v
  amenu 1.190 ToolBar.WinMaxWidth	<C-W>500>
  amenu 1.200 ToolBar.WinMinWidth	<C-W>1\|
  amenu 1.210 ToolBar.WinClose		:close<CR>
endif

  amenu 1.215 ToolBar.-sep5-		<nul>
  amenu <silent> 1.220 ToolBar.LoadSesn	:call <SID>LoadVimSesn()<CR>
  amenu <silent> 1.230 ToolBar.SaveSesn	:call <SID>SaveVimSesn()<CR>
  amenu 1.240 ToolBar.RunScript		:browse so<CR>

  amenu 1.245 ToolBar.-sep6-		<nul>
  amenu 1.250 ToolBar.Make		:make<CR>
  amenu 1.260 ToolBar.Shell		:sh<CR>
  amenu 1.270 ToolBar.RunCtags		:!ctags -R .<CR>
  amenu 1.280 ToolBar.TagJump		g]

  amenu 1.295 ToolBar.-sep7-		<nul>
  amenu 1.300 ToolBar.Help		:help<CR>
  amenu <silent> 1.310 ToolBar.FindHelp  :call <SID>Helpfind()<CR>

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
  if !has("gui_athena")
    tmenu ToolBar.Find		Find...
    tmenu ToolBar.FindNext	Find Next
    tmenu ToolBar.FindPrev	Find Previous
    tmenu ToolBar.Replace		Find / Replace...
  endif
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
fun! s:LoadVimSesn()
  if exists("this_session")
    let name = this_session
  else
    let name = "session.vim"
  endif
  execute "browse so " . name
endfun

" Select a session to save; default to current session name if present
fun! s:SaveVimSesn()
  if !exists("this_session")
    let this_session = "session.vim"
  endif
  execute "browse mksession! " . this_session
endfun

endif

endif " !exists("did_install_default_menus")

" Define these items always, so that syntax can be switched on when it wasn't.
am 50.212 &Syntax.&Manual		:syn manual<CR>
am 50.214 &Syntax.A&utomatic		:syn on<CR>
am <silent> 50.216 &Syntax.on/off\ for\ &This\ file :call <SID>SynOnOff()<CR>
if !exists("*s:SynOnOff")
  fun s:SynOnOff()
    if has("syntax_items")
      syn clear
    else
      if !exists("g:syntax_on")
	syn manual
      endif
      set syn=ON
    endif
  endfun
endif


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

" The following menu items are generated by makemenu.vim.
" The Start Of The Syntax Menu

am 50.10.100 &Syntax.AB.Abaqus :cal SetSyn("abaqus")<CR>
am 50.10.110 &Syntax.AB.ABC :cal SetSyn("abc")<CR>
am 50.10.120 &Syntax.AB.ABEL :cal SetSyn("abel")<CR>
am 50.10.130 &Syntax.AB.Ada :cal SetSyn("ada")<CR>
am 50.10.140 &Syntax.AB.Aflex :cal SetSyn("aflex")<CR>
am 50.10.150 &Syntax.AB.AHDL :cal SetSyn("ahdl")<CR>
am 50.10.160 &Syntax.AB.Amiga\ DOS :cal SetSyn("amiga")<CR>
am 50.10.170 &Syntax.AB.Antlr :cal SetSyn("antlr")<CR>
am 50.10.180 &Syntax.AB.Apache\ config :cal SetSyn("apache")<CR>
am 50.10.190 &Syntax.AB.Apache-style\ config :cal SetSyn("apachestyle")<CR>
am 50.10.200 &Syntax.AB.Applix\ ELF :cal SetSyn("elf")<CR>
am 50.10.210 &Syntax.AB.Arc\ Macro\ Language :cal SetSyn("aml")<CR>
am 50.10.220 &Syntax.AB.ASP\ with\ VBSages :cal SetSyn("aspvbs")<CR>
am 50.10.230 &Syntax.AB.ASP\ with\ Perl :cal SetSyn("aspperl")<CR>
am 50.10.240 &Syntax.AB.Assembly.680x0 :cal SetSyn("asm68k")<CR>
am 50.10.250 &Syntax.AB.Assembly.GNU :cal SetSyn("asm")<CR>
am 50.10.260 &Syntax.AB.Assembly.H8300 :cal SetSyn("asmh8300")<CR>
am 50.10.270 &Syntax.AB.Assembly.Intel\ Itanium :cal SetSyn("ia64")<CR>
am 50.10.280 &Syntax.AB.Assembly.Microsoft :cal SetSyn("masm")<CR>
am 50.10.290 &Syntax.AB.Assembly.Netwide :cal SetSyn("nasm")<CR>
am 50.10.300 &Syntax.AB.Assembly.PIC :cal SetSyn("pic")<CR>
am 50.10.310 &Syntax.AB.Assembly.Turbo :cal SetSyn("tasm")<CR>
am 50.10.320 &Syntax.AB.Assembly.Z-80 :cal SetSyn("z8a")<CR>
am 50.10.330 &Syntax.AB.ASN\.1 :cal SetSyn("asn")<CR>
am 50.10.340 &Syntax.AB.Atlas :cal SetSyn("atlas")<CR>
am 50.10.350 &Syntax.AB.Automake :cal SetSyn("automake")<CR>
am 50.10.360 &Syntax.AB.Avenue :cal SetSyn("ave")<CR>
am 50.10.370 &Syntax.AB.Awk :cal SetSyn("awk")<CR>
am 50.10.380 &Syntax.AB.Ayacc :cal SetSyn("ayacc")<CR>
am 50.10.400 &Syntax.AB.B :cal SetSyn("b")<CR>
am 50.10.410 &Syntax.AB.BASIC :cal SetSyn("basic")<CR>
am 50.10.420 &Syntax.AB.BC\ calculator :cal SetSyn("bc")<CR>
am 50.10.430 &Syntax.AB.BDF\ font :cal SetSyn("bdf")<CR>
am 50.10.440 &Syntax.AB.BibFile :cal SetSyn("bib")<CR>
am 50.10.450 &Syntax.AB.BIND\ configuration :cal SetSyn("named")<CR>
am 50.10.460 &Syntax.AB.BIND\ zone :cal SetSyn("bindzone")<CR>
am 50.10.470 &Syntax.AB.Blank :cal SetSyn("blank")<CR>
am 50.20.100 &Syntax.CD.C :cal SetSyn("c")<CR>
am 50.20.110 &Syntax.CD.C++ :cal SetSyn("cpp")<CR>
am 50.20.120 &Syntax.CD.Crontab :cal SetSyn("crontab")<CR>
am 50.20.130 &Syntax.CD.Cyn++ :cal SetSyn("cynpp")<CR>
am 50.20.140 &Syntax.CD.Cynlib :cal SetSyn("cynlib")<CR>
am 50.20.150 &Syntax.CD.Cascading\ Style\ Sheets :cal SetSyn("css")<CR>
am 50.20.160 &Syntax.CD.Century\ Term :cal SetSyn("cterm")<CR>
am 50.20.170 &Syntax.CD.CFG :cal SetSyn("cfg")<CR>
am 50.20.180 &Syntax.CD.CHILL :cal SetSyn("ch")<CR>
am 50.20.190 &Syntax.CD.Change :cal SetSyn("change")<CR>
am 50.20.200 &Syntax.CD.ChangeLog :cal SetSyn("changelog")<CR>
am 50.20.210 &Syntax.CD.Clean :cal SetSyn("clean")<CR>
am 50.20.220 &Syntax.CD.Clever :cal SetSyn("cl")<CR>
am 50.20.230 &Syntax.CD.Clipper :cal SetSyn("clipper")<CR>
am 50.20.240 &Syntax.CD.Cold\ Fusion :cal SetSyn("cf")<CR>
am 50.20.250 &Syntax.CD.Configure\ script :cal SetSyn("config")<CR>
am 50.20.260 &Syntax.CD.Csh\ shell\ script :cal SetSyn("csh")<CR>
am 50.20.270 &Syntax.CD.Ctrl-H :cal SetSyn("ctrlh")<CR>
am 50.20.280 &Syntax.CD.Cobol :cal SetSyn("cobol")<CR>
am 50.20.290 &Syntax.CD.CSP :cal SetSyn("csp")<CR>
am 50.20.300 &Syntax.CD.CUPL.CUPL :cal SetSyn("cupl")<CR>
am 50.20.310 &Syntax.CD.CUPL.simulation :cal SetSyn("cuplsim")<CR>
am 50.20.320 &Syntax.CD.CVS\ commit :cal SetSyn("cvs")<CR>
am 50.20.330 &Syntax.CD.CWEB :cal SetSyn("cweb")<CR>
am 50.20.350 &Syntax.CD.Debian.Debian\ ChangeLog :cal SetSyn("debchangelog")<CR>
am 50.20.360 &Syntax.CD.Debian.Debian\ Control :cal SetSyn("debcontrol")<CR>
am 50.20.370 &Syntax.CD.Diff :cal SetSyn("diff")<CR>
am 50.20.380 &Syntax.CD.Digital\ Command\ Lang :cal SetSyn("dcl")<CR>
am 50.20.390 &Syntax.CD.Diva\ (with\ SKILL) :cal SetSyn("diva")<CR>
am 50.20.400 &Syntax.CD.DNS :cal SetSyn("dns")<CR>
am 50.20.410 &Syntax.CD.DOT :cal SetSyn("dto")<CR>
am 50.20.420 &Syntax.CD.Dracula :cal SetSyn("dracula")<CR>
am 50.20.430 &Syntax.CD.DSSSL :cal SetSyn("dsl")<CR>
am 50.20.440 &Syntax.CD.DTD :cal SetSyn("dtd")<CR>
am 50.20.450 &Syntax.CD.DTML\ (Zope) :cal SetSyn("dtml")<CR>
am 50.20.460 &Syntax.CD.Dylan.Dylan :cal SetSyn("dylan")<CR>
am 50.20.470 &Syntax.CD.Dylan.Dylan\ intr :cal SetSyn("dylanintr")<CR>
am 50.20.480 &Syntax.CD.Dylan.Dylan\ lid :cal SetSyn("dylanlid")<CR>
am 50.30.100 &Syntax.EFG.Eiffel :cal SetSyn("eiffel")<CR>
am 50.30.110 &Syntax.EFG.Elm\ Filter :cal SetSyn("elmfilt")<CR>
am 50.30.120 &Syntax.EFG.Embedix\ Component\ Description :cal SetSyn("ecd")<CR>
am 50.30.130 &Syntax.EFG.ERicsson\ LANGuage :cal SetSyn("erlang")<CR>
am 50.30.140 &Syntax.EFG.ESQL-C :cal SetSyn("esqlc")<CR>
am 50.30.150 &Syntax.EFG.Eterm\ config :cal SetSyn("eterm")<CR>
am 50.30.160 &Syntax.EFG.Expect :cal SetSyn("expect")<CR>
am 50.30.170 &Syntax.EFG.Exports :cal SetSyn("exports")<CR>
am 50.30.190 &Syntax.EFG.Focus\ Executable :cal SetSyn("focexec")<CR>
am 50.30.200 &Syntax.EFG.Focus\ Master :cal SetSyn("master")<CR>
am 50.30.210 &Syntax.EFG.FORM :cal SetSyn("form")<CR>
am 50.30.220 &Syntax.EFG.Forth :cal SetSyn("forth")<CR>
am 50.30.230 &Syntax.EFG.Fortran :cal SetSyn("fortran")<CR>
am 50.30.240 &Syntax.EFG.FoxPro :cal SetSyn("foxpro")<CR>
am 50.30.250 &Syntax.EFG.Fvwm\ configuration :cal SetSyn("fvwm1")<CR>
am 50.30.260 &Syntax.EFG.Fvwm2\ configuration :cal SetSyn("fvwm2")<CR>
am 50.30.280 &Syntax.EFG.GDB\ command\ file :cal SetSyn("gdb")<CR>
am 50.30.290 &Syntax.EFG.GDMO :cal SetSyn("gdmo")<CR>
am 50.30.300 &Syntax.EFG.Gedcom :cal SetSyn("gedcom")<CR>
am 50.30.310 &Syntax.EFG.GP :cal SetSyn("gp")<CR>
am 50.30.320 &Syntax.EFG.GNU\ Server\ Pages :cal SetSyn("gsp")<CR>
am 50.30.330 &Syntax.EFG.GNUplot :cal SetSyn("gnuplot")<CR>
am 50.30.340 &Syntax.EFG.GTKrc :cal SetSyn("gtkrc")<CR>
am 50.40.100 &Syntax.HIJK.Haskell :cal SetSyn("haskell")<CR>
am 50.40.110 &Syntax.HIJK.Haskell-literate :cal SetSyn("lhaskell")<CR>
am 50.40.120 &Syntax.HIJK.Hercules :cal SetSyn("hercules")<CR>
am 50.40.130 &Syntax.HIJK.HTML :cal SetSyn("html")<CR>
am 50.40.140 &Syntax.HIJK.HTML\ with\ M4 :cal SetSyn("htmlm4")<CR>
am 50.40.150 &Syntax.HIJK.HTML/OS :cal SetSyn("htmlos")<CR>
am 50.40.160 &Syntax.HIJK.Hyper\ Builder :cal SetSyn("hb")<CR>
am 50.40.180 &Syntax.HIJK.Icon :cal SetSyn("icon")<CR>
am 50.40.190 &Syntax.HIJK.IDL :cal SetSyn("idl")<CR>
am 50.40.200 &Syntax.HIJK.Indent\ profile :cal SetSyn("indent")<CR>
am 50.40.210 &Syntax.HIJK.Inform :cal SetSyn("inform")<CR>
am 50.40.220 &Syntax.HIJK.Informix\ 4GL :cal SetSyn("fgl")<CR>
am 50.40.230 &Syntax.HIJK.Inittab :cal SetSyn("inittab")<CR>
am 50.40.240 &Syntax.HIJK.Inno\ Setup :cal SetSyn("iss")<CR>
am 50.40.250 &Syntax.HIJK.InstallShield\ Rules :cal SetSyn("ishd")<CR>
am 50.40.260 &Syntax.HIJK.Interactive\ Data\ Lang :cal SetSyn("idlang")<CR>
am 50.40.280 &Syntax.HIJK.Jam :cal SetSyn("jam")<CR>
am 50.40.290 &Syntax.HIJK.Jargon :cal SetSyn("jargon")<CR>
am 50.40.300 &Syntax.HIJK.Java.Java :cal SetSyn("java")<CR>
am 50.40.310 &Syntax.HIJK.Java.JavaCC :cal SetSyn("javacc")<CR>
am 50.40.320 &Syntax.HIJK.Java.Java\ Server\ Pages :cal SetSyn("jsp")<CR>
am 50.40.330 &Syntax.HIJK.Java.Java\ Properties :cal SetSyn("jproperties")<CR>
am 50.40.340 &Syntax.HIJK.JavaScript :cal SetSyn("javascript")<CR>
am 50.40.350 &Syntax.HIJK.Jess :cal SetSyn("jess")<CR>
am 50.40.360 &Syntax.HIJK.Jgraph :cal SetSyn("jgraph")<CR>
am 50.40.380 &Syntax.HIJK.KDE\ script :cal SetSyn("kscript")<CR>
am 50.40.390 &Syntax.HIJK.Kimwitu :cal SetSyn("kwt")<CR>
am 50.40.400 &Syntax.HIJK.Kixtart :cal SetSyn("kix")<CR>
am 50.50.100 &Syntax.L-Ma.Lace :cal SetSyn("lace")<CR>
am 50.50.110 &Syntax.L-Ma.Lamda\ Prolog :cal SetSyn("lprolog")<CR>
am 50.50.120 &Syntax.L-Ma.Latte :cal SetSyn("latte")<CR>
am 50.50.130 &Syntax.L-Ma.Lex :cal SetSyn("lex")<CR>
am 50.50.140 &Syntax.L-Ma.LFTP :cal SetSyn("lftp")<CR>
am 50.50.150 &Syntax.L-Ma.Lilo :cal SetSyn("lilo")<CR>
am 50.50.160 &Syntax.L-Ma.Lisp :cal SetSyn("lisp")<CR>
am 50.50.170 &Syntax.L-Ma.Lite :cal SetSyn("lite")<CR>
am 50.50.180 &Syntax.L-Ma.LOTOS :cal SetSyn("lotos")<CR>
am 50.50.190 &Syntax.L-Ma.Lout :cal SetSyn("lout")<CR>
am 50.50.200 &Syntax.L-Ma.Lua :cal SetSyn("lua")<CR>
am 50.50.210 &Syntax.L-Ma.Lynx\ Style :cal SetSyn("lss")<CR>
am 50.50.220 &Syntax.L-Ma.Lynx\ config :cal SetSyn("lynx")<CR>
am 50.50.240 &Syntax.L-Ma.M4 :cal SetSyn("m4")<CR>
am 50.50.250 &Syntax.L-Ma.MaGic\ Point :cal SetSyn("mgp")<CR>
am 50.50.260 &Syntax.L-Ma.Mail :cal SetSyn("mail")<CR>
am 50.50.270 &Syntax.L-Ma.Makefile :cal SetSyn("make")<CR>
am 50.50.280 &Syntax.L-Ma.MakeIndex :cal SetSyn("ist")<CR>
am 50.50.290 &Syntax.L-Ma.Man\ page :cal SetSyn("man")<CR>
am 50.50.300 &Syntax.L-Ma.Maple :cal SetSyn("maple")<CR>
am 50.50.310 &Syntax.L-Ma.Mason :cal SetSyn("mason")<CR>
am 50.50.320 &Syntax.L-Ma.Mathematica :cal SetSyn("mma")<CR>
am 50.50.330 &Syntax.L-Ma.Matlab :cal SetSyn("matlab")<CR>
am 50.60.100 &Syntax.Me-NO.MEL\ (for\ Maya) :cal SetSyn("mel")<CR>
am 50.60.110 &Syntax.Me-NO.Metafont :cal SetSyn("mf")<CR>
am 50.60.120 &Syntax.Me-NO.MetaPost :cal SetSyn("mp")<CR>
am 50.60.130 &Syntax.Me-NO.MS\ Module\ Definition :cal SetSyn("def")<CR>
am 50.60.140 &Syntax.Me-NO.Model :cal SetSyn("model")<CR>
am 50.60.150 &Syntax.Me-NO.Modsim\ III :cal SetSyn("modsim3")<CR>
am 50.60.160 &Syntax.Me-NO.Modula\ 2 :cal SetSyn("modula2")<CR>
am 50.60.170 &Syntax.Me-NO.Modula\ 3 :cal SetSyn("modula3")<CR>
am 50.60.180 &Syntax.Me-NO.Msql :cal SetSyn("msql")<CR>
am 50.60.190 &Syntax.Me-NO.MS-DOS.MS-DOS\ \.bat\ file :cal SetSyn("dosbatch")<CR>
am 50.60.200 &Syntax.Me-NO.MS-DOS.4DOS\ \.bat\ file :cal SetSyn("btm")<CR>
am 50.60.210 &Syntax.Me-NO.MS-DOS.MS-DOS\ \.ini\ file :cal SetSyn("dosini")<CR>
am 50.60.220 &Syntax.Me-NO.MS\ Resource\ file :cal SetSyn("rc")<CR>
am 50.60.230 &Syntax.Me-NO.Mush :cal SetSyn("mush")<CR>
am 50.60.240 &Syntax.Me-NO.Muttrc :cal SetSyn("muttrc")<CR>
am 50.60.260 &Syntax.Me-NO.Nastran\ input/DMAP :cal SetSyn("nastran")<CR>
am 50.60.270 &Syntax.Me-NO.Natural :cal SetSyn("natural")<CR>
am 50.60.280 &Syntax.Me-NO.Novell\ batch :cal SetSyn("ncf")<CR>
am 50.60.290 &Syntax.Me-NO.Not\ Quite\ C :cal SetSyn("nqc")<CR>
am 50.60.300 &Syntax.Me-NO.Nroff :cal SetSyn("nroff")<CR>
am 50.60.320 &Syntax.Me-NO.Objective\ C :cal SetSyn("objc")<CR>
am 50.60.330 &Syntax.Me-NO.OCAML :cal SetSyn("ocaml")<CR>
am 50.60.340 &Syntax.Me-NO.Omnimark :cal SetSyn("omnimark")<CR>
am 50.60.350 &Syntax.Me-NO.OpenROAD :cal SetSyn("openroad")<CR>
am 50.60.360 &Syntax.Me-NO.Open\ Psion\ Lang :cal SetSyn("opl")<CR>
am 50.60.370 &Syntax.Me-NO.Oracle\ config :cal SetSyn("ora")<CR>
am 50.70.100 &Syntax.PQ.Palm\ resource\ compiler :cal SetSyn("pilrc")<CR>
am 50.70.110 &Syntax.PQ.PApp :cal SetSyn("papp")<CR>
am 50.70.120 &Syntax.PQ.Pascal :cal SetSyn("pascal")<CR>
am 50.70.130 &Syntax.PQ.PCCTS :cal SetSyn("pccts")<CR>
am 50.70.140 &Syntax.PQ.PPWizard :cal SetSyn("ppwiz")<CR>
am 50.70.150 &Syntax.PQ.Perl.Perl :cal SetSyn("perl")<CR>
am 50.70.160 &Syntax.PQ.Perl.Perl\ POD :cal SetSyn("pod")<CR>
am 50.70.170 &Syntax.PQ.Perl.Perl\ XS :cal SetSyn("xs")<CR>
am 50.70.180 &Syntax.PQ.PHP\ 3-4 :cal SetSyn("php")<CR>
am 50.70.190 &Syntax.PQ.Phtml :cal SetSyn("phtml")<CR>
am 50.70.200 &Syntax.PQ.Pike :cal SetSyn("pike")<CR>
am 50.70.210 &Syntax.PQ.Pine\ RC :cal SetSyn("pine")<CR>
am 50.70.220 &Syntax.PQ.PL/M :cal SetSyn("plm")<CR>
am 50.70.230 &Syntax.PQ.PL/SQL :cal SetSyn("plsql")<CR>
am 50.70.240 &Syntax.PQ.PO\ (GNU\ gettext) :cal SetSyn("po")<CR>
am 50.70.250 &Syntax.PQ.Postfix\ main\ config :cal SetSyn("pfmain")<CR>
am 50.70.260 &Syntax.PQ.PostScript :cal SetSyn("postscr")<CR>
am 50.70.270 &Syntax.PQ.Povray :cal SetSyn("pov")<CR>
am 50.70.280 &Syntax.PQ.Povray\ configuration :cal SetSyn("povini")<CR>
am 50.70.290 &Syntax.PQ.Printcap :cal SetSyn("pcap")<CR>
am 50.70.300 &Syntax.PQ.Procmail :cal SetSyn("procmail")<CR>
am 50.70.310 &Syntax.PQ.Product\ Spec\ File :cal SetSyn("psf")<CR>
am 50.70.320 &Syntax.PQ.Progress :cal SetSyn("progress")<CR>
am 50.70.330 &Syntax.PQ.Prolog :cal SetSyn("prolog")<CR>
am 50.70.340 &Syntax.PQ.Purify\ log :cal SetSyn("purifylog")<CR>
am 50.70.350 &Syntax.PQ.Python :cal SetSyn("python")<CR>
am 50.80.100 &Syntax.R-Sg.R :cal SetSyn("r")<CR>
am 50.80.110 &Syntax.R-Sg.Radiance :cal SetSyn("radiance")<CR>
am 50.80.120 &Syntax.R-Sg.Ratpoison :cal SetSyn("ratpoison")<CR>
am 50.80.130 &Syntax.R-Sg.Readline\ config :cal SetSyn("readline")<CR>
am 50.80.140 &Syntax.R-Sg.RCS\ log\ output :cal SetSyn("rcslog")<CR>
am 50.80.150 &Syntax.R-Sg.Rebol :cal SetSyn("rebol")<CR>
am 50.80.160 &Syntax.R-Sg.Registry\ of\ MS-Windows :cal SetSyn("registry")<CR>
am 50.80.170 &Syntax.R-Sg.Remind :cal SetSyn("remind")<CR>
am 50.80.180 &Syntax.R-Sg.Renderman\ Shader\ Lang :cal SetSyn("sl")<CR>
am 50.80.190 &Syntax.R-Sg.Rexx :cal SetSyn("rexx")<CR>
am 50.80.200 &Syntax.R-Sg.Robots\.txt :cal SetSyn("robots")<CR>
am 50.80.210 &Syntax.R-Sg.Rpcgen :cal SetSyn("rpcgen")<CR>
am 50.80.220 &Syntax.R-Sg.RTF :cal SetSyn("rtf")<CR>
am 50.80.230 &Syntax.R-Sg.Ruby :cal SetSyn("ruby")<CR>
am 50.80.250 &Syntax.R-Sg.S-lang :cal SetSyn("slang")<CR>
am 50.80.260 &Syntax.R-Sg.Samba\ config :cal SetSyn("samba")<CR>
am 50.80.270 &Syntax.R-Sg.SAS :cal SetSyn("sas")<CR>
am 50.80.280 &Syntax.R-Sg.Sather :cal SetSyn("sather")<CR>
am 50.80.290 &Syntax.R-Sg.Scheme :cal SetSyn("scheme")<CR>
am 50.80.300 &Syntax.R-Sg.Screen\ RC :cal SetSyn("screen")<CR>
am 50.80.310 &Syntax.R-Sg.SDL :cal SetSyn("sdl")<CR>
am 50.80.320 &Syntax.R-Sg.Sed :cal SetSyn("sed")<CR>
am 50.80.330 &Syntax.R-Sg.Sendmail\.cf :cal SetSyn("sm")<CR>
am 50.80.340 &Syntax.R-Sg.SGML.SGML\ catalog :cal SetSyn("catalog")<CR>
am 50.80.350 &Syntax.R-Sg.SGML.SGML\ DTD :cal SetSyn("sgml")<CR>
am 50.80.360 &Syntax.R-Sg.SGML.SGML\ Declarations :cal SetSyn("sgmldecl")<CR>
am 50.80.370 &Syntax.R-Sg.SGML.SGML\ linuxdoc :cal SetSyn("sgmllnx")<CR>
am 50.90.100 &Syntax.Sh-S.Sh\ shell\ script :cal SetSyn("sh")<CR>
am 50.90.110 &Syntax.Sh-S.SiCAD :cal SetSyn("sicad")<CR>
am 50.90.120 &Syntax.Sh-S.Simula :cal SetSyn("simula")<CR>
am 50.90.130 &Syntax.Sh-S.Sinda.Sinda\ compare :cal SetSyn("sindacmp")<CR>
am 50.90.140 &Syntax.Sh-S.Sinda.Sinda\ input :cal SetSyn("sinda")<CR>
am 50.90.150 &Syntax.Sh-S.Sinda.Sinda\ output :cal SetSyn("sindaout")<CR>
am 50.90.160 &Syntax.Sh-S.SKILL :cal SetSyn("skill")<CR>
am 50.90.170 &Syntax.Sh-S.SLRN.SLRN\ rc :cal SetSyn("slrnrc")<CR>
am 50.90.180 &Syntax.Sh-S.SLRN.SLRN\ score :cal SetSyn("slrnsc")<CR>
am 50.90.190 &Syntax.Sh-S.SmallTalk :cal SetSyn("st")<CR>
am 50.90.200 &Syntax.Sh-S.SMIL :cal SetSyn("smil")<CR>
am 50.90.210 &Syntax.Sh-S.SMITH :cal SetSyn("smith")<CR>
am 50.90.220 &Syntax.Sh-S.SNMP\ MIB :cal SetSyn("mib")<CR>
am 50.90.230 &Syntax.Sh-S.SNNS.SNNS\ network :cal SetSyn("snnsnet")<CR>
am 50.90.240 &Syntax.Sh-S.SNNS.SNNS\ pattern :cal SetSyn("snnspat")<CR>
am 50.90.250 &Syntax.Sh-S.SNNS.SNNS\ result :cal SetSyn("snnsres")<CR>
am 50.90.260 &Syntax.Sh-S.Snobol4 :cal SetSyn("snobol4")<CR>
am 50.90.270 &Syntax.Sh-S.Snort\ Configuration :cal SetSyn("hog")<CR>
am 50.90.280 &Syntax.Sh-S.SPEC\ (Linux\ RPM) :cal SetSyn("spec")<CR>
am 50.90.290 &Syntax.Sh-S.Specman :cal SetSyn("specman")<CR>
am 50.90.300 &Syntax.Sh-S.Spice :cal SetSyn("spice")<CR>
am 50.90.310 &Syntax.Sh-S.Speedup :cal SetSyn("spup")<CR>
am 50.90.320 &Syntax.Sh-S.Squid :cal SetSyn("squid")<CR>
am 50.90.330 &Syntax.Sh-S.SQL :cal SetSyn("sql")<CR>
am 50.90.340 &Syntax.Sh-S.SQL\ Forms :cal SetSyn("sqlforms")<CR>
am 50.90.350 &Syntax.Sh-S.SQR :cal SetSyn("sqr")<CR>
am 50.90.360 &Syntax.Sh-S.Standard\ ML :cal SetSyn("sml")<CR>
am 50.90.370 &Syntax.Sh-S.Stored\ Procedures :cal SetSyn("stp")<CR>
am 50.90.380 &Syntax.Sh-S.Strace :cal SetSyn("strace")<CR>
am 50.100.100 &Syntax.TUV.Tads :cal SetSyn("tads")<CR>
am 50.100.110 &Syntax.TUV.Tags :cal SetSyn("tags")<CR>
am 50.100.120 &Syntax.TUV.TAK.TAK\ compare :cal SetSyn("tak")<CR>
am 50.100.130 &Syntax.TUV.TAK.TAK\ input :cal SetSyn("tak")<CR>
am 50.100.140 &Syntax.TUV.TAK.TAK\ output :cal SetSyn("takout")<CR>
am 50.100.150 &Syntax.TUV.Tcl/Tk :cal SetSyn("tcl")<CR>
am 50.100.160 &Syntax.TUV.TealInfo :cal SetSyn("tli")<CR>
am 50.100.170 &Syntax.TUV.Telix\ Salt :cal SetSyn("tsalt")<CR>
am 50.100.180 &Syntax.TUV.Termcap :cal SetSyn("ptcap")<CR>
am 50.100.190 &Syntax.TUV.Terminfo :cal SetSyn("terminfo")<CR>
am 50.100.200 &Syntax.TUV.TeX :cal SetSyn("tex")<CR>
am 50.100.210 &Syntax.TUV.TeX\ configuration :cal SetSyn("texmf")<CR>
am 50.100.220 &Syntax.TUV.Texinfo :cal SetSyn("texinfo")<CR>
am 50.100.230 &Syntax.TUV.TF\ mud\ client :cal SetSyn("tf")<CR>
am 50.100.240 &Syntax.TUV.Tidy\ configuration :cal SetSyn("tidy")<CR>
am 50.100.250 &Syntax.TUV.Trasys\ input :cal SetSyn("trasys")<CR>
am 50.100.260 &Syntax.TUV.TSS.Command\ Line :cal SetSyn("tsscl")<CR>
am 50.100.270 &Syntax.TUV.TSS.Geometry :cal SetSyn("tssgm")<CR>
am 50.100.280 &Syntax.TUV.TSS.Optics :cal SetSyn("tssop")<CR>
am 50.100.300 &Syntax.TUV.UIT/UIL :cal SetSyn("uil")<CR>
am 50.100.310 &Syntax.TUV.UnrealScript :cal SetSyn("uc")<CR>
am 50.100.330 &Syntax.TUV.Verilog\ HDL :cal SetSyn("verilog")<CR>
am 50.100.340 &Syntax.TUV.Vgrindefs :cal SetSyn("vgrindefs")<CR>
am 50.100.350 &Syntax.TUV.VHDL :cal SetSyn("vhdl")<CR>
am 50.100.360 &Syntax.TUV.Vim.Vim\ help\ file :cal SetSyn("help")<CR>
am 50.100.370 &Syntax.TUV.Vim.Vim\ script :cal SetSyn("vim")<CR>
am 50.100.380 &Syntax.TUV.Vim.Viminfo\ file :cal SetSyn("viminfo")<CR>
am 50.100.390 &Syntax.TUV.Virata :cal SetSyn("virata")<CR>
am 50.100.400 &Syntax.TUV.Visual\ Basic :cal SetSyn("vb")<CR>
am 50.100.410 &Syntax.TUV.VRML :cal SetSyn("vrml")<CR>
am 50.100.420 &Syntax.TUV.VSE\ JCL :cal SetSyn("vsejcl")<CR>
am 50.110.100 &Syntax.WXYZ.WEB :cal SetSyn("web")<CR>
am 50.110.110 &Syntax.WXYZ.Webmacro :cal SetSyn("webmacro")<CR>
am 50.110.120 &Syntax.WXYZ.Website\ MetaLanguage :cal SetSyn("wml")<CR>
am 50.110.130 &Syntax.WXYZ.Wdiff :cal SetSyn("wdiff")<CR>
am 50.110.140 &Syntax.WXYZ.Wget\ config :cal SetSyn("wget")<CR>
am 50.110.150 &Syntax.WXYZ.Whitespace\ (add) :cal SetSyn("whitespace")<CR>
am 50.110.160 &Syntax.WXYZ.WinBatch/Webbatch :cal SetSyn("winbatch")<CR>
am 50.110.170 &Syntax.WXYZ.Windows\ Scripting\ Host :cal SetSyn("wsh")<CR>
am 50.110.190 &Syntax.WXYZ.X\ Keyboard\ Extension :cal SetSyn("xkb")<CR>
am 50.110.200 &Syntax.WXYZ.X\ Pixmap :cal SetSyn("xpm")<CR>
am 50.110.210 &Syntax.WXYZ.X\ Pixmap\ (2) :cal SetSyn("xpm2")<CR>
am 50.110.220 &Syntax.WXYZ.X\ resources :cal SetSyn("xdefaults")<CR>
am 50.110.230 &Syntax.WXYZ.Xmath :cal SetSyn("xmath")<CR>
am 50.110.240 &Syntax.WXYZ.XML :cal SetSyn("xml")<CR>
am 50.110.250 &Syntax.WXYZ.XXD\ hex\ dump :cal SetSyn("xxd")<CR>
am 50.110.270 &Syntax.WXYZ.Yacc :cal SetSyn("yacc")<CR>
am 50.110.290 &Syntax.WXYZ.Zsh\ shell\ script :cal SetSyn("zsh")<CR>

" The End Of The Syntax Menu


am 50.195 &Syntax.-SEP1-				:

am 50.200 &Syntax.Set\ '&syntax'\ only		:let s:syntax_menu_synonly=1<CR>
am <silent> 50.202 &Syntax.Set\ '&filetype'\ too :call <SID>Nosynonly()<CR>
fun! s:Nosynonly()
  if exists("s:syntax_menu_synonly")
    unlet s:syntax_menu_synonly
  endif
endfun

am 50.210 &Syntax.&Off			:syn off<CR>
am 50.700 &Syntax.-SEP3-		:
am 50.710 &Syntax.Co&lor\ test		:sp $VIMRUNTIME/syntax/colortest.vim<Bar>so %<CR>
am 50.720 &Syntax.&Highlight\ test	:so $VIMRUNTIME/syntax/hitest.vim<CR>
am 50.730 &Syntax.&Convert\ to\ HTML	:so $VIMRUNTIME/syntax/2html.vim<CR>

endif " !exists("did_install_syntax_menu")

" Restore the previous value of 'cpoptions'.
let &cpo = s:cpo_save
unlet s:cpo_save

" vim: set sw=2 :
