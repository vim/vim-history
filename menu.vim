" These menu commands create the default Vim menus.

" You can use this as a start for your own set of menus.

" Make sure the '<' flag is not included in 'cpoptions', otherwise <CR> would
" not be recognized.  See ":help 'cpoptions'".
let cpo_save = &cpo
let &cpo = ""

" Help menu
" Note that "amenu" is used to make it work in all modes
9999amenu &Help.&Overview<Tab><F1>		:help<CR>
amenu &Help.&How-to\ links			:help how-to<CR>
amenu &Help.&GUI				:help gui<CR>
amenu &Help.&Version				:version<CR>
amenu &Help.&Credits				:help credits<CR>
amenu &Help.Co&pying				:help uganda<CR>
amenu &Help.&About				:intro<CR>

" File menu
amenu 10.310 &File.&Open\.\.\.<Tab>:e		:browse confirm e<CR>
amenu 10.320 &File.Sp&lit-Open\.\.\.<Tab>:sp	:browse sp<CR>
amenu 10.330 &File.&Save<Tab>:w			:confirm w<CR>
amenu 10.340 &File.Save\ &As\.\.\.<Tab>:w	:browse w<CR>
amenu 10.350 &File.&Close<Tab>:q		:confirm q<CR>
if has("win32")
" Use Notepad for printing.
  amenu 10.360 &File.&Print :let smod=&mod<Bar>:let ttt=tempname()<Bar>exec ":w! ".ttt<Bar>let &mod=smod<Bar>exec "!notepad /p ".ttt<Bar>exec "!del ".ttt<CR>
  vmenu &File.&Print <Esc>:let smod=&mod<Bar>:let ttt=tempname()<Bar>exec ":'<,'>w! ".ttt<Bar>let &mod=smod<Bar>exec "!notepad /p ".ttt<Bar>exec "!del ".ttt<CR>
elseif has("unix")
  amenu 10.360 &File.&Print			:w !lpr<CR>
  vmenu &File.&Print				:w !lpr<CR>
endif
amenu 10.370 &File.Sa&ve-Exit<Tab>:wqa		:confirm wqa<CR>
amenu 10.380 &File.E&xit<Tab>:qa		:confirm qa<CR>


" Edit menu
amenu 20.310 &Edit.&Undo<Tab>u			u
amenu 20.320 &Edit.&Redo<Tab>Ctrl+R		<C-R>
amenu 20.330 &Edit.&Repeat<Tab>\.		.
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
if has("win32")
  amenu 20.410 &Edit.&Find\.\.\.		:promptfind<CR>
  amenu 20.420 &Edit.Find\ and\ R&eplace\.\.\.	:promptrepl<CR>
  vmenu		   &Edit.Find\ and\ R&eplace\.\.\.	y:promptrepl <C-R>"<CR>
else
  amenu 20.410 &Edit.&Find<Tab>/			/
  amenu 20.420 &Edit.Find\ and\ R&eplace<Tab>:%s	:%s/
  vmenu	       &Edit.Find\ and\ R&eplace<Tab>:s		:s/
endif

" Window menu
amenu 30.300 &Window.&New		<C-W>n
amenu 30.310 &Window.S&plit		<C-W>s
amenu 30.320 &Window.Sp&lit\ To\ #	<C-W><C-^>
amenu 30.330 &Window.&Close		<C-W>c
amenu 30.340 &Window.Close\ &Other(s)	<C-W>o
amenu 30.350 &Window.Ne&xt		<C-W>w
amenu 30.360 &Window.P&revious		<C-W>W
amenu 30.370 &Window.&Equal\ Height	<C-W>=
amenu 30.380 &Window.Rotate\ &Up	<C-W>R
amenu 30.390 &Window.Rotate\ &Down	<C-W>r
if has("gui_win32")
  amenu 30.400 &Window.Select\ &Font	:set guifont=*<CR>
endif

" Programming menu
amenu 40.300 &Tools.&Jump\ to\ this\ tag	g<C-]>
vmenu 40.300 &Tools.&Jump\ to\ this\ tag	g<C-]>
amenu 40.310 &Tools.Jump\ &back			<C-T>
amenu 40.320 &Tools.Build\ &Tags\ File		:!ctags -R .<CR>
amenu 40.330 &Tools.&Make			:make<CR>
amenu 40.340 &Tools.&List\ Errors		:cl<CR>
amenu 40.350 &Tools.L&ist\ Messages		:cl!<CR>
amenu 40.360 &Tools.&Next\ Error		:cn<CR>
amenu 40.370 &Tools.&Previous\ Error		:cp<CR>
amenu 40.380 &Tools.&Older\ List		:colder<CR>
amenu 40.390 &Tools.N&ewer\ List		:cnewer<CR>

" The popup menu
amenu 1.10 PopUp.&Undo		u
vmenu 1.20 PopUp.Cu&t		"*x
vmenu 1.30 PopUp.&Copy		"*y
nmenu 1.40 PopUp.&Paste		"*P`]:if col(".")!=1<Bar>exe "norm l"<Bar>endif<CR>
vmenu      PopUp.&Paste		"-x"*P`]
imenu      PopUp.&Paste		<Esc>:if col(".")!=1<Bar>exe 'norm "*p'<Bar>else<Bar>exe 'norm "*P'<Bar>endif<CR>`]a
cmenu      PopUp.&Paste		<C-R>*
vmenu 1.50 PopUp.&Delete	x
amenu 1.60 PopUp.Select\ &Word	vaw
amenu 1.70 PopUp.Select\ &Line	V
amenu 1.80 PopUp.Select\ &All	ggVG

" The Win32 GUI toolbar
if has("win32")
  amenu ToolBar.Open		:browse e<CR>
  tmenu ToolBar.Open		Open file
  amenu ToolBar.Save		:w<CR>
  tmenu ToolBar.Save		Save current file
  amenu ToolBar.SaveAll		:wa<CR>
  tmenu ToolBar.SaveAll		Save all files

  amenu ToolBar.Print		:let ttt=tempname()<Bar>exec ":w! ".ttt<Bar>exec "!start notepad /p ".ttt<Bar>exec "!del ".ttt<CR>
  vmenu ToolBar.Print		<Esc>:let ttt=tempname()<Bar>exec ":'<,'>w! ".ttt<Bar>exec "!start notepad /p ".ttt<Bar>exec "!del ".ttt<CR>
  tmenu ToolBar.Print		Print

  amenu ToolBar.Undo		u
  tmenu ToolBar.Undo		Undo
  amenu ToolBar.Redo		<C-R>
  tmenu ToolBar.Redo		Redo
  vmenu ToolBar.Cut		"*x
  tmenu ToolBar.Cut		Cut to clipboard
  vmenu ToolBar.Copy		"*y
  tmenu ToolBar.Copy		Copy to clipboard
  nmenu ToolBar.Paste		i<C-R>*<Esc>
  vmenu ToolBar.Paste		"-xi<C-R>*<Esc>
  menu! ToolBar.Paste		<C-R>*
  tmenu ToolBar.Paste		Paste from Clipboard

  amenu ToolBar.Find		/
  tmenu ToolBar.Find		Find
  amenu ToolBar.FindNext	n
  tmenu ToolBar.FindNext	Find Next
  amenu ToolBar.FindPrev	N
  tmenu ToolBar.FindPrev	Find Previous
  amenu ToolBar.Replace		:%s/
  vmenu ToolBar.Replace		:s/
  tmenu ToolBar.Replace		Find && Replace

  amenu ToolBar.New		<C-W>n
  tmenu ToolBar.New		New Window
  amenu ToolBar.WinSplit	<C-W>s
  tmenu ToolBar.WinSplit	Split Window
  amenu ToolBar.WinZoom		:resize 100<CR>
  tmenu ToolBar.WinZoom		Maximise Window
  amenu ToolBar.WinMin		:resize 1<CR>
  tmenu ToolBar.WinMin		Minimise Window
  amenu ToolBar.WinClose	:close<CR>
  tmenu ToolBar.WinClose	Close Window

  amenu ToolBar.LoadSession	:browse so $vim\\session<CR>
  tmenu ToolBar.LoadSession	Load session
  amenu ToolBar.SaveSession	:execute ("mksession " . this_session)
  tmenu ToolBar.SaveSession	Save current session
  amenu ToolBar.RunMacro	:browse so<CR>
  tmenu ToolBar.RunMacro	Run a Macro

  amenu ToolBar.Make		:make<CR>
  tmenu ToolBar.Make		Make current project
  amenu ToolBar.Shell		:sh<CR>
  tmenu ToolBar.Shell		Open a command shell
  amenu ToolBar.TagsBuild	:!ctags -R .<CR>
  tmenu ToolBar.TagsBuild	Build tags in current directory tree
  amenu ToolBar.TagsJump	g]
  tmenu ToolBar.TagsJump	Jump to tag under cursor

  amenu ToolBar.Help		:help<CR>
  tmenu ToolBar.Help		Vim Help
  amenu ToolBar.FindHelp	:help 
  tmenu ToolBar.FindHelp	Search Vim Help
endif

" Restore the previous value of 'cpoptions'.
let &cpo = cpo_save
unlet cpo_save
