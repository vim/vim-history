" Vim support file to define the default menus
" You can also use this as a start for your own set of menus.
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2002 Mar 02

" Note that ":an" (short for ":anoremenu") is often used to make a menu work
" in all modes and avoid side effects from mappings defined by the user.

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
    " Remove "@euro", otherwise "LC_ALL=de_DE@euro gvim" will show English menus
    let s:lang = substitute(s:lang, "@euro", "", "")
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
an 9999.10 &Help.&Overview<Tab><F1>	:help<CR>
an 9999.20 &Help.&User\ Manual		:help usr_toc<CR>
an 9999.30 &Help.&How-to\ links		:help how-to<CR>
an <silent> 9999.40 &Help.&Find\.\.\.	:call <SID>Helpfind()<CR>
an 9999.45 &Help.-sep1-			<Nop>
an 9999.50 &Help.&Credits		:help credits<CR>
an 9999.60 &Help.Co&pying		:help copying<CR>
an 9999.70 &Help.O&rphans		:help kcc<CR>
an 9999.75 &Help.-sep2-			<Nop>
an 9999.80 &Help.&Version		:version<CR>
an 9999.90 &Help.&About			:intro<CR>

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
an 10.310 &File.&Open\.\.\.<Tab>:e		:browse confirm e<CR>
an 10.320 &File.Sp&lit-Open\.\.\.<Tab>:sp	:browse sp<CR>
an 10.325 &File.&New<Tab>:enew			:confirm enew<CR>
an <silent> 10.330 &File.&Close<Tab>:close
	\ :if winheight(2) < 0 <Bar>
	\   confirm enew <Bar>
	\ else <Bar>
	\   confirm close <Bar>
	\ endif<CR>
an 10.335 &File.-SEP1-				<Nop>
an <silent> 10.340 &File.&Save<Tab>:w		:if expand("%") == ""<Bar>browse confirm w<Bar>else<Bar>confirm w<Bar>endif<CR>
an 10.350 &File.Save\ &As\.\.\.<Tab>:sav	:browse confirm saveas<CR>

if has("diff")
  an 10.400 &File.-SEP2-			<Nop>
  an 10.410 &File.Split\ &Diff\ with\.\.\.	:browse vert diffsplit<CR>
  an 10.420 &File.Split\ Patched\ &By\.\.\.	:browse vert diffpatch<CR>
endif

if has("printer")
  an 10.500 &File.-SEP3-			<Nop>
  an 10.510 &File.&Print			:hardcopy<CR>
  vunmenu   &File.&Print
  vnoremenu &File.&Print			:hardcopy<CR>
elseif has("unix")
  an 10.500 &File.-SEP3-			<Nop>
  an 10.510 &File.&Print			:w !lpr<CR>
  vunmenu   &File.&Print
  vnoremenu &File.&Print			:w !lpr<CR>
endif
an 10.600 &File.-SEP4-				<Nop>
an 10.610 &File.Sa&ve-Exit<Tab>:wqa		:confirm wqa<CR>
an 10.620 &File.E&xit<Tab>:qa			:confirm qa<CR>

" Pasting blockwise and linewise selections is not possible in Insert and
" Visual mode without the +virtualedit feature.  They are pasted as if they
" were characterwise instead.
if has("virtualedit")
  nnoremap <silent> <script> <SID>Paste :call <SID>Paste()<CR>
  func! <SID>Paste()
    let ove = &ve
    set ve=all
    normal `^"+gPi
    let &ve = ove
  endfunc
else
  nnoremap <silent> <script> <SID>Paste "=@+.'xy'<CR>gPFx"_2x
endif

" Edit menu
an 20.310 &Edit.&Undo<Tab>u			u
an 20.320 &Edit.&Redo<Tab>^R			<C-R>
an 20.330 &Edit.Rep&eat<Tab>\.			.

an 20.335 &Edit.-SEP1-				<Nop>
vnoremenu 20.340 &Edit.Cu&t<Tab>"+x		"+x
vnoremenu 20.350 &Edit.&Copy<Tab>"+y		"+y
cnoremenu 20.350 &Edit.&Copy<Tab>"+y		<C-Y>
nnoremenu 20.360 &Edit.&Paste<Tab>"+P		"+gP
cnoremenu	 &Edit.&Paste<Tab>"+P		<C-R>+
if has("virtualedit")
  vnoremenu <script>	 &Edit.&Paste<Tab>"+P	"-c<Esc><SID>Paste
  inoremenu <script>	 &Edit.&Paste<Tab>"+P	<Esc><SID>Pastegi
else
  vnoremenu <script>	 &Edit.&Paste<Tab>"+P	"-c<Esc>gix<Esc><SID>Paste"_x
  inoremenu <script>	 &Edit.&Paste<Tab>"+P	x<Esc><SID>Paste"_s
endif
nnoremenu 20.370 &Edit.Put\ &Before<Tab>[p	[p
inoremenu	 &Edit.Put\ &Before<Tab>[p	<C-O>[p
nnoremenu 20.380 &Edit.Put\ &After<Tab>]p	]p
inoremenu	 &Edit.Put\ &After<Tab>]p	<C-O>]p
if has("win32") || has("win16")
  vnoremenu 20.390 &Edit.&Delete<Tab>x		x
endif
an <silent> 20.400 &Edit.&Select\ all<Tab>ggVG	:if &slm != ""<Bar>exe ":norm gggH<C-O>G"<Bar>else<Bar>exe ":norm ggVG"<Bar>endif<CR>

an 20.405	 &Edit.-SEP2-				<Nop>
if has("win32")  || has("win16") || has("gui_gtk") || has("gui_motif")
  an 20.410	 &Edit.&Find\.\.\.			:promptfind<CR>
  vunmenu	 &Edit.&Find\.\.\.
  vnoremenu	 &Edit.&Find\.\.\.			y:promptfind <C-R>"<CR>
  an 20.420	 &Edit.Find\ and\ Rep&lace\.\.\.	:promptrepl<CR>
  vunmenu	 &Edit.Find\ and\ Rep&lace\.\.\.
  vnoremenu	 &Edit.Find\ and\ Rep&lace\.\.\.	y:promptrepl <C-R>"<CR>
else
  an 20.410	 &Edit.&Find<Tab>/			/
  an 20.420	 &Edit.Find\ and\ Rep&lace<Tab>:%s	:%s/
  vunmenu	 &Edit.Find\ and\ Rep&lace<Tab>:%s
  vnoremenu	 &Edit.Find\ and\ Rep&lace<Tab>:s	:s/
endif

an 20.425	 &Edit.-SEP3-				<Nop>
an 20.430	 &Edit.Settings\ &Window		:options<CR>

" Edit/Global Settings
an 20.440.100 &Edit.&Global\ Settings.Toggle\ Pattern\ &Highlight<Tab>:set\ hls!	:set hls! hls?<CR>
an 20.440.110 &Edit.&Global\ Settings.Toggle\ &Ignore-case<Tab>:set\ ic!	:set ic! ic?<CR>
an 20.440.110 &Edit.&Global\ Settings.Toggle\ &Showmatch<Tab>:set\ sm!	:set sm! sm?<CR>

an 20.440.120 &Edit.&Global\ Settings.&Context\ lines.\ 1\  :set so=1<CR>
an 20.440.120 &Edit.&Global\ Settings.&Context\ lines.\ 2\  :set so=2<CR>
an 20.440.120 &Edit.&Global\ Settings.&Context\ lines.\ 3\  :set so=3<CR>
an 20.440.120 &Edit.&Global\ Settings.&Context\ lines.\ 4\  :set so=4<CR>
an 20.440.120 &Edit.&Global\ Settings.&Context\ lines.\ 5\  :set so=5<CR>
an 20.440.120 &Edit.&Global\ Settings.&Context\ lines.\ 7\  :set so=7<CR>
an 20.440.120 &Edit.&Global\ Settings.&Context\ lines.\ 10\  :set so=10<CR>
an 20.440.120 &Edit.&Global\ Settings.&Context\ lines.\ 100\  :set so=100<CR>

an 20.440.130.40 &Edit.&Global\ Settings.&Virtual\ Edit.Never :set ve=<CR>
an 20.440.130.50 &Edit.&Global\ Settings.&Virtual\ Edit.Block\ Selection :set ve=block<CR>
an 20.440.130.60 &Edit.&Global\ Settings.&Virtual\ Edit.Insert\ mode :set ve=insert<CR>
an 20.440.130.70 &Edit.&Global\ Settings.&Virtual\ Edit.Block\ and\ Insert :set ve=block,insert<CR>
an 20.440.130.80 &Edit.&Global\ Settings.&Virtual\ Edit.Always :set ve=all<CR>
an 20.440.140 &Edit.&Global\ Settings.Toggle\ Insert\ &Mode<Tab>:set\ im!	:set im!<CR>
an 20.440.145 &Edit.&Global\ Settings.Toggle\ Vi\ C&ompatible<Tab>:set\ cp!	:set cp!<CR>
an <silent> 20.440.150 &Edit.&Global\ Settings.Search\ &Path\.\.\.  :call <SID>SearchP()<CR>
an <silent> 20.440.160 &Edit.&Global\ Settings.Ta&g\ Files\.\.\.  :call <SID>TagFiles()<CR>
"
" GUI options
an 20.440.300 &Edit.&Global\ Settings.-SEP1-				<Nop>
an <silent> 20.440.310 &Edit.&Global\ Settings.Toggle\ &Toolbar		:call <SID>ToggleGuiOption("T")<CR>
an <silent> 20.440.320 &Edit.&Global\ Settings.Toggle\ &Bottom\ Scrollbar :call <SID>ToggleGuiOption("b")<CR>
an <silent> 20.440.330 &Edit.&Global\ Settings.Toggle\ &Left\ Scrollbar	:call <SID>ToggleGuiOption("l")<CR>
an <silent> 20.440.340 &Edit.&Global\ Settings.Toggle\ &Right\ Scrollbar :call <SID>ToggleGuiOption("r")<CR>

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
    if match(&guioptions, "\\C" . a:option) > -1
	exec "set go-=" . a:option
    else
	exec "set go+=" . a:option
    endif
endfun

" Edit/File Settings

" Boolean options
an 20.440.100 &Edit.F&ile\ Settings.Toggle\ Line\ &Numbering<Tab>:set\ nu!	:set nu! nu?<CR>
an 20.440.110 &Edit.F&ile\ Settings.Toggle\ &List\ Mode<Tab>:set\ list!	:set list! list?<CR>
an 20.440.120 &Edit.F&ile\ Settings.Toggle\ Line\ &Wrap<Tab>:set\ wrap!	:set wrap! wrap?<CR>
an 20.440.130 &Edit.F&ile\ Settings.Toggle\ W&rap\ at\ word<Tab>:set\ lbr!	:set lbr! lbr?<CR>
an 20.440.160 &Edit.F&ile\ Settings.Toggle\ &expand-tab<Tab>:set\ et!	:set et! et?<CR>
an 20.440.170 &Edit.F&ile\ Settings.Toggle\ &auto-indent<Tab>:set\ ai!	:set ai! ai?<CR>
an 20.440.180 &Edit.F&ile\ Settings.Toggle\ &C-indenting<Tab>:set\ cin!	:set cin! cin?<CR>

" other options
an 20.440.600 &Edit.F&ile\ Settings.-SEP2-		<Nop>
an 20.440.610.20 &Edit.F&ile\ Settings.&Shiftwidth.2	:set sw=2 sw?<CR>
an 20.440.610.30 &Edit.F&ile\ Settings.&Shiftwidth.3	:set sw=3 sw?<CR>
an 20.440.610.40 &Edit.F&ile\ Settings.&Shiftwidth.4	:set sw=4 sw?<CR>
an 20.440.610.50 &Edit.F&ile\ Settings.&Shiftwidth.5	:set sw=5 sw?<CR>
an 20.440.610.60 &Edit.F&ile\ Settings.&Shiftwidth.6	:set sw=6 sw?<CR>
an 20.440.610.80 &Edit.F&ile\ Settings.&Shiftwidth.8	:set sw=8 sw?<CR>

an 20.440.620.20 &Edit.F&ile\ Settings.Soft\ &Tabstop.2	:set sts=2 sts?<CR>
an 20.440.620.30 &Edit.F&ile\ Settings.Soft\ &Tabstop.3	:set sts=3 sts?<CR>
an 20.440.620.40 &Edit.F&ile\ Settings.Soft\ &Tabstop.4	:set sts=4 sts?<CR>
an 20.440.620.50 &Edit.F&ile\ Settings.Soft\ &Tabstop.5	:set sts=5 sts?<CR>
an 20.440.620.60 &Edit.F&ile\ Settings.Soft\ &Tabstop.6	:set sts=6 sts?<CR>
an 20.440.620.80 &Edit.F&ile\ Settings.Soft\ &Tabstop.8	:set sts=8 sts?<CR>

an <silent> 20.440.630 &Edit.F&ile\ Settings.Te&xt\ Width\.\.\.  :call <SID>TextWidth()<CR>
an <silent> 20.440.640 &Edit.F&ile\ Settings.&File\ Format\.\.\.  :call <SID>FileFormat()<CR>
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
  if !exists("g:menutrans_fileformat_choices")
    let g:menutrans_fileformat_choices = "&Unix\n&Dos\n&Mac\n&Cancel"
  endif
  if &ff == "dos"
    let def = 2
  elseif &ff == "mac"
    let def = 3
  else
    let def = 1
  endif
  let n = confirm(g:menutrans_fileformat_dialog, g:menutrans_fileformat_choices, def, "Question")
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
  exe "an 20.450." . s:idx . ' &Edit.C&olor\ Scheme.' . s:name . " :colors " . s:name . "<CR>"
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
    an 20.460.90 &Edit.&Keymap.None :set keymap=<CR>
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
      exe "an 20.460." . s:idx . ' &Edit.&Keymap.' . s:name . " :set keymap=" . s:name . "<CR>"
      unlet s:name
      unlet s:i
      let s:idx = s:idx + 10
    endwhile
    unlet s:idx
  endif
  unlet s:n
endif
if has("win32") || has("win16") || has("gui_gtk") || has("gui_photon")
  an 20.470 &Edit.Select\ Fo&nt\.\.\.	:set guifont=*<CR>
endif

" Programming menu
an 40.300 &Tools.&Jump\ to\ this\ tag<Tab>g^]	g<C-]>
vunmenu &Tools.&Jump\ to\ this\ tag<Tab>g^]
vnoremenu &Tools.&Jump\ to\ this\ tag<Tab>g^]	g<C-]>
an 40.310 &Tools.Jump\ &back<Tab>^T		<C-T>
if has("vms")
  an 40.320 &Tools.Build\ &Tags\ File		:!mc vim:ctags .<CR>
else
  an 40.320 &Tools.Build\ &Tags\ File		:!ctags -R .<CR>
endif

" Tools.Fold Menu
if has("folding")
  an 40.330 &Tools.-SEP1-						<Nop>
  " open close folds
  an 40.340.110 &Tools.&Folding.&Enable/Disable\ folds<Tab>zi		zi
  an 40.340.120 &Tools.&Folding.&View\ Cursor\ Line<Tab>zv		zv
  an 40.340.120 &Tools.&Folding.Vie&w\ Cursor\ Line\ only<Tab>zMzx	zMzx
  an 40.340.130 &Tools.&Folding.C&lose\ more\ folds<Tab>zm		zm
  an 40.340.140 &Tools.&Folding.&Close\ all\ folds<Tab>zM		zM
  an 40.340.150 &Tools.&Folding.O&pen\ more\ folds<Tab>zr		zr
  an 40.340.160 &Tools.&Folding.&Open\ all\ folds<Tab>zR		zR
  " fold method
  an 40.340.200 &Tools.&Folding.-SEP1-			<Nop>
  an 40.340.210 &Tools.&Folding.Fold\ Met&hod.M&anual	:set fdm=manual<CR>
  an 40.340.210 &Tools.&Folding.Fold\ Met&hod.I&ndent	:set fdm=indent<CR>
  an 40.340.210 &Tools.&Folding.Fold\ Met&hod.E&xpression :set fdm=expr<CR>
  an 40.340.210 &Tools.&Folding.Fold\ Met&hod.S&yntax	:set fdm=syntax<CR>
  an 40.340.210 &Tools.&Folding.Fold\ Met&hod.&Diff	:set fdm=diff<CR>
  an 40.340.210 &Tools.&Folding.Fold\ Met&hod.Ma&rker	:set fdm=marker<CR>
  " create and delete folds
  vnoremenu 40.340.220 &Tools.&Folding.Create\ &Fold<Tab>zf	zf
  an 40.340.230 &Tools.&Folding.&Delete\ Fold<Tab>zd		zd
  an 40.340.240 &Tools.&Folding.Delete\ &All\ Folds<Tab>zD	zD
  " moving around in folds
  an 40.340.300 &Tools.&Folding.-SEP2-				<Nop>
  an 40.340.310.10 &Tools.&Folding.Fold\ column\ &width.\ 0\ 	:set fdc=0<CR>
  an 40.340.310.20 &Tools.&Folding.Fold\ column\ &width.\ 2\ 	:set fdc=2<CR>
  an 40.340.310.30 &Tools.&Folding.Fold\ column\ &width.\ 3\ 	:set fdc=3<CR>
  an 40.340.310.40 &Tools.&Folding.Fold\ column\ &width.\ 4\ 	:set fdc=4<CR>
  an 40.340.310.50 &Tools.&Folding.Fold\ column\ &width.\ 5\ 	:set fdc=5<CR>
  an 40.340.310.60 &Tools.&Folding.Fold\ column\ &width.\ 6\ 	:set fdc=6<CR>
  an 40.340.310.70 &Tools.&Folding.Fold\ column\ &width.\ 7\ 	:set fdc=7<CR>
  an 40.340.310.80 &Tools.&Folding.Fold\ column\ &width.\ 8\ 	:set fdc=8<CR>
endif  " has folding

if has("diff")
  an 40.350.100 &Tools.&Diff.&Update		:diffupdate<CR>
  an 40.350.110 &Tools.&Diff.&Get\ Block	:diffget<CR>
  vunmenu &Tools.&Diff.&Get\ Block
  vnoremenu &Tools.&Diff.&Get\ Block		:diffget<CR>
  an 40.350.120 &Tools.&Diff.&Put\ Block	:diffput<CR>
  vunmenu &Tools.&Diff.&Put\ Block
  vnoremenu &Tools.&Diff.&Put\ Block		:diffput<CR>
endif

an 40.358 &Tools.-SEP2-					<Nop>
an 40.360 &Tools.&Make<Tab>:make			:make<CR>
an 40.370 &Tools.&List\ Errors<Tab>:cl			:cl<CR>
an 40.380 &Tools.L&ist\ Messages<Tab>:cl!		:cl!<CR>
an 40.390 &Tools.&Next\ Error<Tab>:cn			:cn<CR>
an 40.400 &Tools.&Previous\ Error<Tab>:cp		:cp<CR>
an 40.410 &Tools.&Older\ List<Tab>:cold			:colder<CR>
an 40.420 &Tools.N&ewer\ List<Tab>:cnew			:cnewer<CR>
an 40.430.50 &Tools.Error\ &Window.&Update<Tab>:cwin	:cwin<CR>
an 40.430.60 &Tools.Error\ &Window.&Open<Tab>:copen	:copen<CR>
an 40.430.70 &Tools.Error\ &Window.&Close<Tab>:cclose	:cclose<CR>

an 40.520 &Tools.-SEP3-					<Nop>
an <silent> 40.530 &Tools.&Convert\ to\ HEX<Tab>:%!xxd
	\ :call <SID>XxdConv()<CR>
an <silent> 40.540 &Tools.Conve&rt\ back<Tab>:%!xxd\ -r
	\ :call <SID>XxdBack()<CR>

" Use a function to do the conversion, so that it also works with 'insertmode'
" set.
func! s:XxdConv()
  let mod = &mod
  if has("vms")
    %!mc vim:xxd
  else
    call s:XxdFind()
    exe "%!" . g:xxdprogram
  endif
  set ft=xxd
  let &mod = mod
endfun

func! s:XxdBack()
  let mod = &mod
  if has("vms")
    %!mc vim:xxd -r
  else
    call s:XxdFind()
    exe "%!" . g:xxdprogram . " -r"
  endif
  set ft=
  doautocmd filetypedetect BufReadPost
  let &mod = mod
endfun

func! s:XxdFind()
  if !exists("g:xxdprogram")
    " On the PC xxd may not be in the path but in the install directory
    if (has("win32") || has("dos32")) && !executable("xxd")
      let g:xxdprogram = $VIMRUNTIME . (&shellslash ? '/' : '\') . "xxd.exe"
    else
      let g:xxdprogram = "xxd"
    endif
  endif
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
  exe "an 30.440." . s:idx . ' &Tools.&Set\ Compiler.' . s:name . " :compiler " . s:name . "<CR>"
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
  exe 'noremenu ' . g:bmenu_priority . ".1 &Buffers.Dummy l"
  silent! unmenu! &Buffers

  " create new menu; set 'cpo' to include the <CR>
  let cpo_save = &cpo
  set cpo&vim
  exe 'an <silent> ' . g:bmenu_priority . ".2 &Buffers.&Refresh\\ menu :call <SID>BMShow()<CR>"
  exe 'an ' . g:bmenu_priority . ".4 &Buffers.&Delete :bd<CR>"
  exe 'an ' . g:bmenu_priority . ".6 &Buffers.&Alternate :b #<CR>"
  exe 'an ' . g:bmenu_priority . ".7 &Buffers.&Next :bnext<CR>"
  exe 'an ' . g:bmenu_priority . ".8 &Buffers.&Previous :bprev<CR>"
  exe 'an ' . g:bmenu_priority . ".9 &Buffers.-SEP- :"
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
    let name = 'an ' . g:bmenu_priority . '.' . hash . ' &Buffers.' . munge
  else
    let name = 'an ' . g:bmenu_priority . '.' . hash . '.' . hash . ' &Buffers.' . <SID>BMHash2(munge) . munge
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
an 70.300 &Window.&New<Tab>^Wn			<C-W>n
an 70.310 &Window.S&plit<Tab>^Ws		<C-W>s
an 70.320 &Window.Sp&lit\ To\ #<Tab>^W^^	<C-W><C-^>
an 70.330 &Window.Split\ &Vertically<Tab>^Wv	<C-W>v
if has("vertsplit")
  an <silent> 70.332 &Window.Split\ File\ E&xplorer	:call MenuExplOpen()<CR>
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
an 70.335 &Window.-SEP1-				<Nop>
an 70.340 &Window.&Close<Tab>^Wc			:confirm close<CR>
an 70.345 &Window.Close\ &Other(s)<Tab>^Wo		:confirm only<CR>
an 70.350 &Window.-SEP2-				<Nop>
an 70.355 &Window.Move\ &To.&Top<Tab>^WK		<C-W>K
an 70.355 &Window.Move\ &To.&Bottom<Tab>^WJ		<C-W>J
an 70.355 &Window.Move\ &To.&Left\ side<Tab>^WH		<C-W>H
an 70.355 &Window.Move\ &To.&Right\ side<Tab>^WL	<C-W>L
an 70.360 &Window.Rotate\ &Up<Tab>^WR			<C-W>R
an 70.362 &Window.Rotate\ &Down<Tab>^Wr			<C-W>r
an 70.365 &Window.-SEP3-				<Nop>
an 70.370 &Window.&Equal\ Size<Tab>^W=			<C-W>=
an 70.380 &Window.&Max\ Height<Tab>^W_			<C-W>_
an 70.390 &Window.M&in\ Height<Tab>^W1_			<C-W>1_
an 70.400 &Window.Max\ &Width<Tab>^W\|			<C-W>\|
an 70.410 &Window.Min\ Widt&h<Tab>^W1\|			<C-W>1\|

" The popup menu
an 1.10 PopUp.&Undo			u
an 1.15 PopUp.-SEP1-			<Nop>
vnoremenu 1.20 PopUp.Cu&t		"+x
vnoremenu 1.30 PopUp.&Copy		"+y
cnoremenu 1.30 PopUp.&Copy		<C-Y>
nnoremenu 1.40 PopUp.&Paste		"+gP
cnoremenu 1.40 PopUp.&Paste		<C-R>+
if has("virtualedit")
  vnoremenu <script> 1.40 PopUp.&Paste	"-c<Esc><SID>Paste
  inoremenu <script> 1.40 PopUp.&Paste	<Esc><SID>Pastegi
else
  vnoremenu <script> 1.40 PopUp.&Paste	"-c<Esc>gix<Esc><SID>Paste"_x
  inoremenu <script> 1.40 PopUp.&Paste	x<Esc><SID>Paste"_s
endif
vnoremenu 1.50 PopUp.&Delete		x
an 1.55 PopUp.-SEP2-			<Nop>
vnoremenu 1.60 PopUp.Select\ Blockwise	<C-V>
an 1.70 PopUp.Select\ &Word		vaw
an 1.80 PopUp.Select\ &Line		V
an 1.90 PopUp.Select\ &Block		<C-V>
an 1.100 PopUp.Select\ &All		ggVG

" The GUI toolbar (for MS-Windows and GTK)
if has("toolbar")
  an 1.10 ToolBar.Open			:browse confirm e<CR>
  an <silent> 1.20 ToolBar.Save		:if expand("%") == ""<Bar>browse confirm w<Bar>else<Bar>confirm w<Bar>endif<CR>
  an 1.30 ToolBar.SaveAll		:wa<CR>

  if has("printer")
    an 1.40   ToolBar.Print		:hardcopy<CR>
    vunmenu   ToolBar.Print
    vnoremenu ToolBar.Print		:hardcopy<CR>
  elseif has("unix")
    an 1.40   ToolBar.Print		:w !lpr<CR>
    vunmenu   ToolBar.Print
    vnoremenu ToolBar.Print		:w !lpr<CR>
  endif

  an 1.45 ToolBar.-sep1-		<Nop>
  an 1.50 ToolBar.Undo			u
  an 1.60 ToolBar.Redo			<C-R>

  an 1.65 ToolBar.-sep2-		<Nop>
  vnoremenu 1.70 ToolBar.Cut		"+x
  vnoremenu 1.80 ToolBar.Copy		"+y
  cnoremenu 1.80 ToolBar.Copy		<C-Y>
  nnoremenu 1.90 ToolBar.Paste		"+gP
  cnoremenu	 ToolBar.Paste		<C-R>+
  if has("virtualedit")
    vnoremenu <script>	 ToolBar.Paste	"-c<Esc><SID>Paste
    inoremenu <script>	 ToolBar.Paste	<Esc><SID>Pastegi
  else
    vnoremenu <script>	 ToolBar.Paste	"-c<Esc>gix<Esc><SID>Paste"_x
    inoremenu <script>	 ToolBar.Paste	x<Esc><SID>Paste"_s
  endif

  if !has("gui_athena")
    an 1.95   ToolBar.-sep3-		<Nop>
    an 1.100  ToolBar.Find		:promptfind<CR>
    vunmenu   ToolBar.Find
    vnoremenu ToolBar.Find		y:promptfind <C-R>"<CR>
    an 1.110  ToolBar.FindNext		n
    an 1.120  ToolBar.FindPrev		N
    an 1.130  ToolBar.Replace		:promptrepl<CR>
    vunmenu   ToolBar.Replace
    vnoremenu ToolBar.Replace		y:promptrepl <C-R>"<CR>
  endif

if 0	" disabled; These are in the Windows menu
  an 1.135 ToolBar.-sep4-		<Nop>
  an 1.140 ToolBar.New			<C-W>n
  an 1.150 ToolBar.WinSplit		<C-W>s
  an 1.160 ToolBar.WinMax		:resize 200<CR>
  an 1.170 ToolBar.WinMin		:resize 1<CR>
  an 1.180 ToolBar.WinVSplit		<C-W>v
  an 1.190 ToolBar.WinMaxWidth		<C-W>500>
  an 1.200 ToolBar.WinMinWidth		<C-W>1\|
  an 1.210 ToolBar.WinClose		:close<CR>
endif

  an 1.215 ToolBar.-sep5-		<Nop>
  an <silent> 1.220 ToolBar.LoadSesn	:call <SID>LoadVimSesn()<CR>
  an <silent> 1.230 ToolBar.SaveSesn	:call <SID>SaveVimSesn()<CR>
  an 1.240 ToolBar.RunScript		:browse so<CR>

  an 1.245 ToolBar.-sep6-		<Nop>
  an 1.250 ToolBar.Make			:make<CR>
  an 1.260 ToolBar.Shell			:sh<CR>
  an 1.270 ToolBar.RunCtags		:!ctags -R .<CR>
  an 1.280 ToolBar.TagJump		g<C-]>

  an 1.295 ToolBar.-sep7-		<Nop>
  an 1.300 ToolBar.Help			:help<CR>
  an <silent> 1.310 ToolBar.FindHelp	:call <SID>Helpfind()<CR>

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
  if strlen(v:this_session) > 0
    let name = v:this_session
  else
    let name = "Session.vim"
  endif
  execute "browse so " . name
endfun

" Select a session to save; default to current session name if present
fun! s:SaveVimSesn()
  if strlen(v:this_session) == 0
    let v:this_session = "Session.vim"
  endif
  execute "browse mksession! " . v:this_session
endfun

endif

endif " !exists("did_install_default_menus")

" Define these items always, so that syntax can be switched on when it wasn't.
" But skip them when the Syntax menu was disabled by the user.
if !exists("did_install_syntax_menu")
  an 50.212 &Syntax.&Manual		:syn manual<CR>
  an 50.214 &Syntax.A&utomatic		:syn on<CR>
  an <silent> 50.216 &Syntax.on/off\ for\ &This\ file :call <SID>SynOnOff()<CR>
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

an 50.10.100 &Syntax.AB.Abaqus :cal SetSyn("abaqus")<CR>
an 50.10.110 &Syntax.AB.ABC\ music\ notation :cal SetSyn("abc")<CR>
an 50.10.120 &Syntax.AB.ABEL :cal SetSyn("abel")<CR>
an 50.10.130 &Syntax.AB.AceDB\ model :cal SetSyn("acedb")<CR>
an 50.10.140 &Syntax.AB.Ada :cal SetSyn("ada")<CR>
an 50.10.150 &Syntax.AB.AfLex :cal SetSyn("aflex")<CR>
an 50.10.160 &Syntax.AB.Altera\ AHDL :cal SetSyn("ahdl")<CR>
an 50.10.170 &Syntax.AB.Amiga\ DOS :cal SetSyn("amiga")<CR>
an 50.10.180 &Syntax.AB.AMPL :cal SetSyn("ampl")<CR>
an 50.10.190 &Syntax.AB.Ant\ build\ file :cal SetSyn("ant")<CR>
an 50.10.200 &Syntax.AB.ANTLR :cal SetSyn("antlr")<CR>
an 50.10.210 &Syntax.AB.Apache\ config :cal SetSyn("apache")<CR>
an 50.10.220 &Syntax.AB.Apache-style\ config :cal SetSyn("apachestyle")<CR>
an 50.10.230 &Syntax.AB.Applix\ ELF :cal SetSyn("elf")<CR>
an 50.10.240 &Syntax.AB.Arc\ Macro\ Language :cal SetSyn("aml")<CR>
an 50.10.250 &Syntax.AB.ASP\ with\ VBScript :cal SetSyn("aspvbs")<CR>
an 50.10.260 &Syntax.AB.ASP\ with\ Perl :cal SetSyn("aspperl")<CR>
an 50.10.270 &Syntax.AB.Assembly.680x0 :cal SetSyn("asm68k")<CR>
an 50.10.280 &Syntax.AB.Assembly.GNU :cal SetSyn("asm")<CR>
an 50.10.290 &Syntax.AB.Assembly.GNU\ H-8300 :cal SetSyn("asmh8300")<CR>
an 50.10.300 &Syntax.AB.Assembly.Intel\ IA-64 :cal SetSyn("ia64")<CR>
an 50.10.310 &Syntax.AB.Assembly.Microsoft :cal SetSyn("masm")<CR>
an 50.10.320 &Syntax.AB.Assembly.Netwide :cal SetSyn("nasm")<CR>
an 50.10.330 &Syntax.AB.Assembly.PIC :cal SetSyn("pic")<CR>
an 50.10.340 &Syntax.AB.Assembly.Turbo :cal SetSyn("tasm")<CR>
an 50.10.350 &Syntax.AB.Assembly.Z-80 :cal SetSyn("z8a")<CR>
an 50.10.360 &Syntax.AB.ASN\.1 :cal SetSyn("asn")<CR>
an 50.10.370 &Syntax.AB.Atlas :cal SetSyn("atlas")<CR>
an 50.10.380 &Syntax.AB.Automake :cal SetSyn("automake")<CR>
an 50.10.390 &Syntax.AB.Avenue :cal SetSyn("ave")<CR>
an 50.10.400 &Syntax.AB.Awk :cal SetSyn("awk")<CR>
an 50.10.410 &Syntax.AB.AYacc :cal SetSyn("ayacc")<CR>
an 50.10.430 &Syntax.AB.B :cal SetSyn("b")<CR>
an 50.10.440 &Syntax.AB.Baan :cal SetSyn("baan")<CR>
an 50.10.450 &Syntax.AB.BASIC :cal SetSyn("basic")<CR>
an 50.10.460 &Syntax.AB.BC\ calculator :cal SetSyn("bc")<CR>
an 50.10.470 &Syntax.AB.BDF\ font :cal SetSyn("bdf")<CR>
an 50.10.480 &Syntax.AB.BibTeX :cal SetSyn("bib")<CR>
an 50.10.490 &Syntax.AB.BIND.BIND\ config :cal SetSyn("named")<CR>
an 50.10.500 &Syntax.AB.BIND.BIND\ zone :cal SetSyn("bindzone")<CR>
an 50.10.510 &Syntax.AB.Blank :cal SetSyn("blank")<CR>
an 50.20.100 &Syntax.CD.C :cal SetSyn("c")<CR>
an 50.20.110 &Syntax.CD.C++ :cal SetSyn("cpp")<CR>
an 50.20.120 &Syntax.CD.C# :cal SetSyn("cs")<CR>
an 50.20.130 &Syntax.CD.CDL :cal SetSyn("cdl")<CR>
an 50.20.140 &Syntax.CD.Crontab :cal SetSyn("crontab")<CR>
an 50.20.150 &Syntax.CD.Cyn++ :cal SetSyn("cynpp")<CR>
an 50.20.160 &Syntax.CD.Cynlib :cal SetSyn("cynlib")<CR>
an 50.20.170 &Syntax.CD.Cascading\ Style\ Sheets :cal SetSyn("css")<CR>
an 50.20.180 &Syntax.CD.Century\ Term :cal SetSyn("cterm")<CR>
an 50.20.190 &Syntax.CD.CHILL :cal SetSyn("ch")<CR>
an 50.20.200 &Syntax.CD.ChangeLog :cal SetSyn("changelog")<CR>
an 50.20.210 &Syntax.CD.Clean :cal SetSyn("clean")<CR>
an 50.20.220 &Syntax.CD.Clever :cal SetSyn("cl")<CR>
an 50.20.230 &Syntax.CD.Clipper :cal SetSyn("clipper")<CR>
an 50.20.240 &Syntax.CD.Cold\ Fusion :cal SetSyn("cf")<CR>
an 50.20.250 &Syntax.CD.Config.Cfg\ Config\ file :cal SetSyn("cfg")<CR>
an 50.20.260 &Syntax.CD.Config.Generic Config\ file :cal SetSyn("conf")<CR>
an 50.20.270 &Syntax.CD.Config.Configure\.in :cal SetSyn("config")<CR>
an 50.20.280 &Syntax.CD.Ctrl-H :cal SetSyn("ctrlh")<CR>
an 50.20.290 &Syntax.CD.Cobol :cal SetSyn("cobol")<CR>
an 50.20.300 &Syntax.CD.CSP :cal SetSyn("csp")<CR>
an 50.20.310 &Syntax.CD.CUPL.CUPL :cal SetSyn("cupl")<CR>
an 50.20.320 &Syntax.CD.CUPL.Simulation :cal SetSyn("cuplsim")<CR>
an 50.20.330 &Syntax.CD.CVS\ commit :cal SetSyn("cvs")<CR>
an 50.20.350 &Syntax.CD.Debian.Debian\ ChangeLog :cal SetSyn("debchangelog")<CR>
an 50.20.360 &Syntax.CD.Debian.Debian\ Control :cal SetSyn("debcontrol")<CR>
an 50.20.370 &Syntax.CD.Diff :cal SetSyn("diff")<CR>
an 50.20.380 &Syntax.CD.Digital\ Command\ Lang :cal SetSyn("dcl")<CR>
an 50.20.390 &Syntax.CD.DNS/BIND\ zone :cal SetSyn("dns")<CR>
an 50.20.400 &Syntax.CD.DocBook :cal SetSyn("docbk")<CR>
an 50.20.410 &Syntax.CD.Dot :cal SetSyn("dot")<CR>
an 50.20.420 &Syntax.CD.Dracula :cal SetSyn("dracula")<CR>
an 50.20.430 &Syntax.CD.DSSSL :cal SetSyn("dsl")<CR>
an 50.20.440 &Syntax.CD.DTD :cal SetSyn("dtd")<CR>
an 50.20.450 &Syntax.CD.DTML\ (Zope) :cal SetSyn("dtml")<CR>
an 50.20.460 &Syntax.CD.Dylan.Dylan :cal SetSyn("dylan")<CR>
an 50.20.470 &Syntax.CD.Dylan.Dylan\ interface :cal SetSyn("dylanintr")<CR>
an 50.20.480 &Syntax.CD.Dylan.Dylan\ lid :cal SetSyn("dylanlid")<CR>
an 50.30.100 &Syntax.EFG.Eiffel :cal SetSyn("eiffel")<CR>
an 50.30.110 &Syntax.EFG.Elm\ filter\ rules :cal SetSyn("elmfilt")<CR>
an 50.30.120 &Syntax.EFG.Embedix\ Component\ Description :cal SetSyn("ecd")<CR>
an 50.30.130 &Syntax.EFG.ERicsson\ LANGuage :cal SetSyn("erlang")<CR>
an 50.30.140 &Syntax.EFG.ESQL-C :cal SetSyn("esqlc")<CR>
an 50.30.150 &Syntax.EFG.Essbase\ script :cal SetSyn("csc")<CR>
an 50.30.160 &Syntax.EFG.Eterm\ config :cal SetSyn("eterm")<CR>
an 50.30.170 &Syntax.EFG.Expect :cal SetSyn("expect")<CR>
an 50.30.180 &Syntax.EFG.Exports :cal SetSyn("exports")<CR>
an 50.30.200 &Syntax.EFG.Focus\ Executable :cal SetSyn("focexec")<CR>
an 50.30.210 &Syntax.EFG.Focus\ Master :cal SetSyn("master")<CR>
an 50.30.220 &Syntax.EFG.FORM :cal SetSyn("form")<CR>
an 50.30.230 &Syntax.EFG.Forth :cal SetSyn("forth")<CR>
an 50.30.240 &Syntax.EFG.Fortran :cal SetSyn("fortran")<CR>
an 50.30.250 &Syntax.EFG.FoxPro :cal SetSyn("foxpro")<CR>
an 50.30.260 &Syntax.EFG.Fvwm\ configuration :cal SetSyn("fvwm1")<CR>
an 50.30.270 &Syntax.EFG.Fvwm2\ configuration :cal SetSyn("fvwm2")<CR>
an 50.30.290 &Syntax.EFG.GDB\ command\ file :cal SetSyn("gdb")<CR>
an 50.30.300 &Syntax.EFG.GDMO :cal SetSyn("gdmo")<CR>
an 50.30.310 &Syntax.EFG.Gedcom :cal SetSyn("gedcom")<CR>
an 50.30.320 &Syntax.EFG.GP :cal SetSyn("gp")<CR>
an 50.30.330 &Syntax.EFG.GNU\ Server\ Pages :cal SetSyn("gsp")<CR>
an 50.30.340 &Syntax.EFG.GNUplot :cal SetSyn("gnuplot")<CR>
an 50.30.350 &Syntax.EFG.Groff :cal SetSyn("groff")<CR>
an 50.30.360 &Syntax.EFG.GTKrc :cal SetSyn("gtkrc")<CR>
an 50.40.100 &Syntax.HIJK.Haskell.Haskell :cal SetSyn("haskell")<CR>
an 50.40.110 &Syntax.HIJK.Haskell.Haskell-c2hs :cal SetSyn("chaskell")<CR>
an 50.40.120 &Syntax.HIJK.Haskell.Haskell-literate :cal SetSyn("lhaskell")<CR>
an 50.40.130 &Syntax.HIJK.Hercules :cal SetSyn("hercules")<CR>
an 50.40.140 &Syntax.HIJK.HTML :cal SetSyn("html")<CR>
an 50.40.150 &Syntax.HIJK.HTML\ with\ M4 :cal SetSyn("htmlm4")<CR>
an 50.40.160 &Syntax.HIJK.HTML/OS :cal SetSyn("htmlos")<CR>
an 50.40.170 &Syntax.HIJK.Hyper\ Builder :cal SetSyn("hb")<CR>
an 50.40.190 &Syntax.HIJK.Icon :cal SetSyn("icon")<CR>
an 50.40.200 &Syntax.HIJK.IDL :cal SetSyn("idl")<CR>
an 50.40.210 &Syntax.HIJK.Indent\ profile :cal SetSyn("indent")<CR>
an 50.40.220 &Syntax.HIJK.Inform :cal SetSyn("inform")<CR>
an 50.40.230 &Syntax.HIJK.Informix\ 4GL :cal SetSyn("fgl")<CR>
an 50.40.240 &Syntax.HIJK.Inittab :cal SetSyn("inittab")<CR>
an 50.40.250 &Syntax.HIJK.Inno\ setup :cal SetSyn("iss")<CR>
an 50.40.260 &Syntax.HIJK.InstallShield\ script :cal SetSyn("ishd")<CR>
an 50.40.270 &Syntax.HIJK.Interactive\ Data\ Lang :cal SetSyn("idlang")<CR>
an 50.40.290 &Syntax.HIJK.JAM :cal SetSyn("jam")<CR>
an 50.40.300 &Syntax.HIJK.Jargon :cal SetSyn("jargon")<CR>
an 50.40.310 &Syntax.HIJK.Java.Java :cal SetSyn("java")<CR>
an 50.40.320 &Syntax.HIJK.Java.JavaCC :cal SetSyn("javacc")<CR>
an 50.40.330 &Syntax.HIJK.Java.Java\ Server\ Pages :cal SetSyn("jsp")<CR>
an 50.40.340 &Syntax.HIJK.Java.Java\ Properties :cal SetSyn("jproperties")<CR>
an 50.40.350 &Syntax.HIJK.JavaScript :cal SetSyn("javascript")<CR>
an 50.40.360 &Syntax.HIJK.Jess :cal SetSyn("jess")<CR>
an 50.40.370 &Syntax.HIJK.Jgraph :cal SetSyn("jgraph")<CR>
an 50.40.390 &Syntax.HIJK.KDE\ script :cal SetSyn("kscript")<CR>
an 50.40.400 &Syntax.HIJK.Kimwitu++ :cal SetSyn("kwt")<CR>
an 50.40.410 &Syntax.HIJK.KixTart :cal SetSyn("kix")<CR>
an 50.50.100 &Syntax.L-Ma.Lace :cal SetSyn("lace")<CR>
an 50.50.110 &Syntax.L-Ma.LamdaProlog :cal SetSyn("lprolog")<CR>
an 50.50.120 &Syntax.L-Ma.Latte :cal SetSyn("latte")<CR>
an 50.50.130 &Syntax.L-Ma.Lex :cal SetSyn("lex")<CR>
an 50.50.140 &Syntax.L-Ma.LFTP\ config :cal SetSyn("lftp")<CR>
an 50.50.150 &Syntax.L-Ma.LifeLines\ script :cal SetSyn("lifelines")<CR>
an 50.50.160 &Syntax.L-Ma.Lilo :cal SetSyn("lilo")<CR>
an 50.50.170 &Syntax.L-Ma.Lisp :cal SetSyn("lisp")<CR>
an 50.50.180 &Syntax.L-Ma.Lite :cal SetSyn("lite")<CR>
an 50.50.190 &Syntax.L-Ma.LOTOS :cal SetSyn("lotos")<CR>
an 50.50.200 &Syntax.L-Ma.LotusScript :cal SetSyn("lscript")<CR>
an 50.50.210 &Syntax.L-Ma.Lout :cal SetSyn("lout")<CR>
an 50.50.220 &Syntax.L-Ma.Lua :cal SetSyn("lua")<CR>
an 50.50.230 &Syntax.L-Ma.Lynx\ Style :cal SetSyn("lss")<CR>
an 50.50.240 &Syntax.L-Ma.Lynx\ config :cal SetSyn("lynx")<CR>
an 50.50.260 &Syntax.L-Ma.M4 :cal SetSyn("m4")<CR>
an 50.50.270 &Syntax.L-Ma.MaGic\ Point :cal SetSyn("mgp")<CR>
an 50.50.280 &Syntax.L-Ma.Mail :cal SetSyn("mail")<CR>
an 50.50.290 &Syntax.L-Ma.Makefile :cal SetSyn("make")<CR>
an 50.50.300 &Syntax.L-Ma.MakeIndex :cal SetSyn("ist")<CR>
an 50.50.310 &Syntax.L-Ma.Man\ page :cal SetSyn("man")<CR>
an 50.50.320 &Syntax.L-Ma.Maple\ V :cal SetSyn("maple")<CR>
an 50.50.330 &Syntax.L-Ma.Mason :cal SetSyn("mason")<CR>
an 50.50.340 &Syntax.L-Ma.Mathematica :cal SetSyn("mma")<CR>
an 50.50.350 &Syntax.L-Ma.Matlab :cal SetSyn("matlab")<CR>
an 50.60.100 &Syntax.Me-NO.MEL\ (for\ Maya) :cal SetSyn("mel")<CR>
an 50.60.110 &Syntax.Me-NO.Metafont :cal SetSyn("mf")<CR>
an 50.60.120 &Syntax.Me-NO.MetaPost :cal SetSyn("mp")<CR>
an 50.60.130 &Syntax.Me-NO.Model :cal SetSyn("model")<CR>
an 50.60.140 &Syntax.Me-NO.Modsim\ III :cal SetSyn("modsim3")<CR>
an 50.60.150 &Syntax.Me-NO.Modula\ 2 :cal SetSyn("modula2")<CR>
an 50.60.160 &Syntax.Me-NO.Modula\ 3 :cal SetSyn("modula3")<CR>
an 50.60.170 &Syntax.Me-NO.Monk :cal SetSyn("monk")<CR>
an 50.60.180 &Syntax.Me-NO.MOO :cal SetSyn("moo")<CR>
an 50.60.190 &Syntax.Me-NO.MS-DOS/Windows.4DOS\ \.bat\ file :cal SetSyn("btm")<CR>
an 50.60.200 &Syntax.Me-NO.MS-DOS/Windows.\.bat\/\.cmd\ file :cal SetSyn("dosbatch")<CR>
an 50.60.210 &Syntax.Me-NO.MS-DOS/Windows.\.ini\ file :cal SetSyn("dosini")<CR>
an 50.60.220 &Syntax.Me-NO.MS-DOS/Windows.Module\ Definition :cal SetSyn("def")<CR>
an 50.60.230 &Syntax.Me-NO.MS-DOS/Windows.Registry :cal SetSyn("registry")<CR>
an 50.60.240 &Syntax.Me-NO.MS-DOS/Windows.Resource\ file :cal SetSyn("rc")<CR>
an 50.60.250 &Syntax.Me-NO.Msql :cal SetSyn("msql")<CR>
an 50.60.260 &Syntax.Me-NO.MUSHcode :cal SetSyn("mush")<CR>
an 50.60.270 &Syntax.Me-NO.Muttrc :cal SetSyn("muttrc")<CR>
an 50.60.290 &Syntax.Me-NO.Nastran\ input/DMAP :cal SetSyn("nastran")<CR>
an 50.60.300 &Syntax.Me-NO.Natural :cal SetSyn("natural")<CR>
an 50.60.310 &Syntax.Me-NO.Novell\ NCF\ batch :cal SetSyn("ncf")<CR>
an 50.60.320 &Syntax.Me-NO.Not\ Quite\ C\ (LEGO) :cal SetSyn("nqc")<CR>
an 50.60.330 &Syntax.Me-NO.Nroff :cal SetSyn("nroff")<CR>
an 50.60.340 &Syntax.Me-NO.NSIS\ script :cal SetSyn("nsis")<CR>
an 50.60.360 &Syntax.Me-NO.Objective\ C :cal SetSyn("objc")<CR>
an 50.60.370 &Syntax.Me-NO.OCAML :cal SetSyn("ocaml")<CR>
an 50.60.380 &Syntax.Me-NO.Omnimark :cal SetSyn("omnimark")<CR>
an 50.60.390 &Syntax.Me-NO.OpenROAD :cal SetSyn("openroad")<CR>
an 50.60.400 &Syntax.Me-NO.Open\ Psion\ Lang :cal SetSyn("opl")<CR>
an 50.60.410 &Syntax.Me-NO.Oracle\ config :cal SetSyn("ora")<CR>
an 50.70.100 &Syntax.PQ.Palm\ resource\ compiler :cal SetSyn("pilrc")<CR>
an 50.70.110 &Syntax.PQ.PApp :cal SetSyn("papp")<CR>
an 50.70.120 &Syntax.PQ.Pascal :cal SetSyn("pascal")<CR>
an 50.70.130 &Syntax.PQ.PCCTS :cal SetSyn("pccts")<CR>
an 50.70.140 &Syntax.PQ.PPWizard :cal SetSyn("ppwiz")<CR>
an 50.70.150 &Syntax.PQ.Perl.Perl :cal SetSyn("perl")<CR>
an 50.70.160 &Syntax.PQ.Perl.Perl\ POD :cal SetSyn("pod")<CR>
an 50.70.170 &Syntax.PQ.Perl.Perl\ XS :cal SetSyn("xs")<CR>
an 50.70.180 &Syntax.PQ.PHP\ 3-4 :cal SetSyn("php")<CR>
an 50.70.190 &Syntax.PQ.Phtml\ (PHP\ 2) :cal SetSyn("phtml")<CR>
an 50.70.200 &Syntax.PQ.Pike :cal SetSyn("pike")<CR>
an 50.70.210 &Syntax.PQ.Pine\ RC :cal SetSyn("pine")<CR>
an 50.70.220 &Syntax.PQ.PL/M :cal SetSyn("plm")<CR>
an 50.70.230 &Syntax.PQ.PL/SQL :cal SetSyn("plsql")<CR>
an 50.70.240 &Syntax.PQ.PO\ (GNU\ gettext) :cal SetSyn("po")<CR>
an 50.70.250 &Syntax.PQ.Postfix\ main\ config :cal SetSyn("pfmain")<CR>
an 50.70.260 &Syntax.PQ.PostScript :cal SetSyn("postscr")<CR>
an 50.70.270 &Syntax.PQ.PostScript\ Printer\ Description :cal SetSyn("ppd")<CR>
an 50.70.280 &Syntax.PQ.Povray\ scene\ descr :cal SetSyn("pov")<CR>
an 50.70.290 &Syntax.PQ.Povray\ configuration :cal SetSyn("povini")<CR>
an 50.70.300 &Syntax.PQ.Printcap :cal SetSyn("pcap")<CR>
an 50.70.310 &Syntax.PQ.Procmail :cal SetSyn("procmail")<CR>
an 50.70.320 &Syntax.PQ.Product\ Spec\ File :cal SetSyn("psf")<CR>
an 50.70.330 &Syntax.PQ.Progress :cal SetSyn("progress")<CR>
an 50.70.340 &Syntax.PQ.Prolog :cal SetSyn("prolog")<CR>
an 50.70.350 &Syntax.PQ.Purify\ log :cal SetSyn("purifylog")<CR>
an 50.70.360 &Syntax.PQ.Python :cal SetSyn("python")<CR>
an 50.80.100 &Syntax.R-Sg.R :cal SetSyn("r")<CR>
an 50.80.110 &Syntax.R-Sg.Radiance :cal SetSyn("radiance")<CR>
an 50.80.120 &Syntax.R-Sg.Ratpoison :cal SetSyn("ratpoison")<CR>
an 50.80.130 &Syntax.R-Sg.Readline\ config :cal SetSyn("readline")<CR>
an 50.80.140 &Syntax.R-Sg.RCS\ log\ output :cal SetSyn("rcslog")<CR>
an 50.80.150 &Syntax.R-Sg.Rebol :cal SetSyn("rebol")<CR>
an 50.80.160 &Syntax.R-Sg.Remind :cal SetSyn("remind")<CR>
an 50.80.170 &Syntax.R-Sg.Renderman\ Shader\ Lang :cal SetSyn("sl")<CR>
an 50.80.180 &Syntax.R-Sg.Rexx :cal SetSyn("rexx")<CR>
an 50.80.190 &Syntax.R-Sg.Robots\.txt :cal SetSyn("robots")<CR>
an 50.80.200 &Syntax.R-Sg.Rpcgen :cal SetSyn("rpcgen")<CR>
an 50.80.210 &Syntax.R-Sg.RPL/2 :cal SetSyn("rpl")<CR>
an 50.80.220 &Syntax.R-Sg.RTF :cal SetSyn("rtf")<CR>
an 50.80.230 &Syntax.R-Sg.Ruby :cal SetSyn("ruby")<CR>
an 50.80.250 &Syntax.R-Sg.S-Lang :cal SetSyn("slang")<CR>
an 50.80.260 &Syntax.R-Sg.Samba\ config :cal SetSyn("samba")<CR>
an 50.80.270 &Syntax.R-Sg.SAS :cal SetSyn("sas")<CR>
an 50.80.280 &Syntax.R-Sg.Sather :cal SetSyn("sather")<CR>
an 50.80.290 &Syntax.R-Sg.Scheme :cal SetSyn("scheme")<CR>
an 50.80.300 &Syntax.R-Sg.Scilab :cal SetSyn("scilab")<CR>
an 50.80.310 &Syntax.R-Sg.Screen\ RC :cal SetSyn("screen")<CR>
an 50.80.320 &Syntax.R-Sg.SDL :cal SetSyn("sdl")<CR>
an 50.80.330 &Syntax.R-Sg.Sed :cal SetSyn("sed")<CR>
an 50.80.340 &Syntax.R-Sg.Sendmail\.cf :cal SetSyn("sm")<CR>
an 50.80.350 &Syntax.R-Sg.Send-pr :cal SetSyn("sendpr")<CR>
an 50.80.360 &Syntax.R-Sg.SGML.SGML\ catalog :cal SetSyn("catalog")<CR>
an 50.80.370 &Syntax.R-Sg.SGML.SGML\ DTD :cal SetSyn("sgml")<CR>
an 50.80.380 &Syntax.R-Sg.SGML.SGML\ Declaration :cal SetSyn("sgmldecl")<CR>
an 50.80.390 &Syntax.R-Sg.SGML.SGML-linuxdoc :cal SetSyn("sgmllnx")<CR>
an 50.90.100 &Syntax.Sh-S.Shell\ script.sh\ and\ ksh :cal SetSyn("sh")<CR>
an 50.90.110 &Syntax.Sh-S.Shell\ script.csh :cal SetSyn("csh")<CR>
an 50.90.120 &Syntax.Sh-S.Shell\ script.zsh :cal SetSyn("zsh")<CR>
an 50.90.130 &Syntax.Sh-S.SiCAD :cal SetSyn("sicad")<CR>
an 50.90.140 &Syntax.Sh-S.Simula :cal SetSyn("simula")<CR>
an 50.90.150 &Syntax.Sh-S.Sinda.Sinda\ compare :cal SetSyn("sindacmp")<CR>
an 50.90.160 &Syntax.Sh-S.Sinda.Sinda\ input :cal SetSyn("sinda")<CR>
an 50.90.170 &Syntax.Sh-S.Sinda.Sinda\ output :cal SetSyn("sindaout")<CR>
an 50.90.180 &Syntax.Sh-S.SKILL.SKILL :cal SetSyn("skill")<CR>
an 50.90.190 &Syntax.Sh-S.SKILL.SKILL\ for\ Diva :cal SetSyn("diva")<CR>
an 50.90.200 &Syntax.Sh-S.SLRN.Slrn\ rc :cal SetSyn("slrnrc")<CR>
an 50.90.210 &Syntax.Sh-S.SLRN.Slrn\ score :cal SetSyn("slrnsc")<CR>
an 50.90.220 &Syntax.Sh-S.SmallTalk :cal SetSyn("st")<CR>
an 50.90.230 &Syntax.Sh-S.SMIL :cal SetSyn("smil")<CR>
an 50.90.240 &Syntax.Sh-S.SMITH :cal SetSyn("smith")<CR>
an 50.90.250 &Syntax.Sh-S.SNMP\ MIB :cal SetSyn("mib")<CR>
an 50.90.260 &Syntax.Sh-S.SNNS.SNNS\ network :cal SetSyn("snnsnet")<CR>
an 50.90.270 &Syntax.Sh-S.SNNS.SNNS\ pattern :cal SetSyn("snnspat")<CR>
an 50.90.280 &Syntax.Sh-S.SNNS.SNNS\ result :cal SetSyn("snnsres")<CR>
an 50.90.290 &Syntax.Sh-S.Snobol4 :cal SetSyn("snobol4")<CR>
an 50.90.300 &Syntax.Sh-S.Snort\ Configuration :cal SetSyn("hog")<CR>
an 50.90.310 &Syntax.Sh-S.SPEC\ (Linux\ RPM) :cal SetSyn("spec")<CR>
an 50.90.320 &Syntax.Sh-S.Specman :cal SetSyn("specman")<CR>
an 50.90.330 &Syntax.Sh-S.Spice :cal SetSyn("spice")<CR>
an 50.90.340 &Syntax.Sh-S.Speedup :cal SetSyn("spup")<CR>
an 50.90.350 &Syntax.Sh-S.Splint :cal SetSyn("splint")<CR>
an 50.90.360 &Syntax.Sh-S.Squid\ config :cal SetSyn("squid")<CR>
an 50.90.370 &Syntax.Sh-S.SQL.SQL :cal SetSyn("sql")<CR>
an 50.90.380 &Syntax.Sh-S.SQL.SQL\ Forms :cal SetSyn("sqlforms")<CR>
an 50.90.390 &Syntax.Sh-S.SQL.SQLJ :cal SetSyn("sqlj")<CR>
an 50.90.400 &Syntax.Sh-S.SQR :cal SetSyn("sqr")<CR>
an 50.90.410 &Syntax.Sh-S.Standard\ ML :cal SetSyn("sml")<CR>
an 50.90.420 &Syntax.Sh-S.Stored\ Procedures :cal SetSyn("stp")<CR>
an 50.90.430 &Syntax.Sh-S.Strace :cal SetSyn("strace")<CR>
an 50.100.100 &Syntax.TUV.TADS :cal SetSyn("tads")<CR>
an 50.100.110 &Syntax.TUV.Tags :cal SetSyn("tags")<CR>
an 50.100.120 &Syntax.TUV.TAK.TAK\ compare :cal SetSyn("takcmp")<CR>
an 50.100.130 &Syntax.TUV.TAK.TAK\ input :cal SetSyn("tak")<CR>
an 50.100.140 &Syntax.TUV.TAK.TAK\ output :cal SetSyn("takout")<CR>
an 50.100.150 &Syntax.TUV.Tcl/Tk :cal SetSyn("tcl")<CR>
an 50.100.160 &Syntax.TUV.TealInfo :cal SetSyn("tli")<CR>
an 50.100.170 &Syntax.TUV.Telix\ Salt :cal SetSyn("tsalt")<CR>
an 50.100.180 &Syntax.TUV.Termcap/Printcap :cal SetSyn("ptcap")<CR>
an 50.100.190 &Syntax.TUV.Terminfo :cal SetSyn("terminfo")<CR>
an 50.100.200 &Syntax.TUV.TeX.TeX :cal SetSyn("tex")<CR>
an 50.100.210 &Syntax.TUV.TeX.TeX\ configuration :cal SetSyn("texmf")<CR>
an 50.100.220 &Syntax.TUV.TeX.Texinfo :cal SetSyn("texinfo")<CR>
an 50.100.230 &Syntax.TUV.TF\ mud\ client :cal SetSyn("tf")<CR>
an 50.100.240 &Syntax.TUV.Tidy\ configuration :cal SetSyn("tidy")<CR>
an 50.100.250 &Syntax.TUV.Trasys\ input :cal SetSyn("trasys")<CR>
an 50.100.260 &Syntax.TUV.TSS.Command\ Line :cal SetSyn("tsscl")<CR>
an 50.100.270 &Syntax.TUV.TSS.Geometry :cal SetSyn("tssgm")<CR>
an 50.100.280 &Syntax.TUV.TSS.Optics :cal SetSyn("tssop")<CR>
an 50.100.300 &Syntax.TUV.UIT/UIL :cal SetSyn("uil")<CR>
an 50.100.310 &Syntax.TUV.UnrealScript :cal SetSyn("uc")<CR>
an 50.100.330 &Syntax.TUV.Verilog\ HDL :cal SetSyn("verilog")<CR>
an 50.100.340 &Syntax.TUV.Vgrindefs :cal SetSyn("vgrindefs")<CR>
an 50.100.350 &Syntax.TUV.VHDL :cal SetSyn("vhdl")<CR>
an 50.100.360 &Syntax.TUV.Vim.Vim\ help\ file :cal SetSyn("help")<CR>
an 50.100.370 &Syntax.TUV.Vim.Vim\ script :cal SetSyn("vim")<CR>
an 50.100.380 &Syntax.TUV.Vim.Viminfo\ file :cal SetSyn("viminfo")<CR>
an 50.100.390 &Syntax.TUV.Virata\ config :cal SetSyn("virata")<CR>
an 50.100.400 &Syntax.TUV.Visual\ Basic :cal SetSyn("vb")<CR>
an 50.100.410 &Syntax.TUV.VRML :cal SetSyn("vrml")<CR>
an 50.100.420 &Syntax.TUV.VSE\ JCL :cal SetSyn("vsejcl")<CR>
an 50.110.100 &Syntax.WXYZ.WEB.CWEB :cal SetSyn("cweb")<CR>
an 50.110.110 &Syntax.WXYZ.WEB.WEB :cal SetSyn("web")<CR>
an 50.110.120 &Syntax.WXYZ.WEB.WEB\ Changes :cal SetSyn("change")<CR>
an 50.110.130 &Syntax.WXYZ.Webmacro :cal SetSyn("webmacro")<CR>
an 50.110.140 &Syntax.WXYZ.Website\ MetaLanguage :cal SetSyn("wml")<CR>
an 50.110.160 &Syntax.WXYZ.wDiff :cal SetSyn("wdiff")<CR>
an 50.110.180 &Syntax.WXYZ.Wget\ config :cal SetSyn("wget")<CR>
an 50.110.190 &Syntax.WXYZ.Whitespace\ (add) :cal SetSyn("whitespace")<CR>
an 50.110.200 &Syntax.WXYZ.WinBatch/Webbatch :cal SetSyn("winbatch")<CR>
an 50.110.210 &Syntax.WXYZ.Windows\ Scripting\ Host :cal SetSyn("wsh")<CR>
an 50.110.220 &Syntax.WXYZ.WvDial :cal SetSyn("wvdial")<CR>
an 50.110.240 &Syntax.WXYZ.X\ Keyboard\ Extension :cal SetSyn("xkb")<CR>
an 50.110.250 &Syntax.WXYZ.X\ Pixmap :cal SetSyn("xpm")<CR>
an 50.110.260 &Syntax.WXYZ.X\ Pixmap\ (2) :cal SetSyn("xpm2")<CR>
an 50.110.270 &Syntax.WXYZ.X\ resources :cal SetSyn("xdefaults")<CR>
an 50.110.280 &Syntax.WXYZ.Xmodmap :cal SetSyn("xmodmap")<CR>
an 50.110.290 &Syntax.WXYZ.Xmath :cal SetSyn("xmath")<CR>
an 50.110.300 &Syntax.WXYZ.XML :cal SetSyn("xml")<CR>
an 50.110.310 &Syntax.WXYZ.XML\ Schema\ (XSD) :cal SetSyn("xsd")<CR>
an 50.110.320 &Syntax.WXYZ.Xslt :cal SetSyn("xslt")<CR>
an 50.110.330 &Syntax.WXYZ.XXD\ hex\ dump :cal SetSyn("xxd")<CR>
an 50.110.340 &Syntax.WXYZ.XFree86\ Config :cal SetSyn("xf86conf")<CR>
an 50.110.360 &Syntax.WXYZ.Yacc :cal SetSyn("yacc")<CR>

" The End Of The Syntax Menu


an 50.195 &Syntax.-SEP1-			<Nop>

an 50.200 &Syntax.Set\ '&syntax'\ only		:let s:syntax_menu_synonly=1<CR>
an <silent> 50.202 &Syntax.Set\ '&filetype'\ too :call <SID>Nosynonly()<CR>
fun! s:Nosynonly()
  if exists("s:syntax_menu_synonly")
    unlet s:syntax_menu_synonly
  endif
endfun

an 50.210 &Syntax.&Off			:syn off<CR>
an 50.700 &Syntax.-SEP3-		<Nop>
an 50.710 &Syntax.Co&lor\ test		:sp $VIMRUNTIME/syntax/colortest.vim<Bar>so %<CR>
an 50.720 &Syntax.&Highlight\ test	:so $VIMRUNTIME/syntax/hitest.vim<CR>
an 50.730 &Syntax.&Convert\ to\ HTML	:so $VIMRUNTIME/syntax/2html.vim<CR>

endif " !exists("did_install_syntax_menu")

" Restore the previous value of 'cpoptions'.
let &cpo = s:cpo_save
unlet s:cpo_save

" vim: set sw=2 :
