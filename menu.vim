" These menu commands create the default Vim menus.

" You can use this as a start for your own set of menus.

" Make sure the '<' flag is not included in 'cpoptions', otherwise <CR> would
" not be recognized.  See ":help 'cpoptions'".
let cpo_save = &cpo
let &cpo = ""

" Help menu
" Note that "amenu" is used to make it work in all modes
9999amenu Help.Overview\ \ <F1>		:help<CR>
9999amenu Help.How\ to\.\.\.		:help how-to<CR>
9999amenu Help.GUI			:help gui<CR>
9999amenu Help.Version			:version<CR>
9999amenu Help.Credits			:help credits<CR>
9999amenu Help.Copying			:help uganda<CR>

" File menu
10amenu File.Open\ \ \ \ \ \ \ \ \ :e	:e<Space>
10amenu File.Split-Open\ \ \ :sp	:sp<Space>
10amenu File.Save\ \ \ \ \ \ \ \ \ :w	:w<CR>
10amenu File.Close\ \ \ \ \ \ \ \ :q	:q<CR>
if has("unix")
  10amenu File.Print			:w !lpr<CR>
  10vmenu File.Print			:w !lpr<CR>
elseif has("win32")
  " Use Notepad for printing.
  10amenu File.Print :let ttt=tempname()<Bar>exec ":w! ".ttt<Bar>exec "!start notepad /p ".ttt<Bar>exec "!del ".ttt<CR>
  10vmenu File.Print <Esc>:let ttt=tempname()<Bar>exec ":'<,'>w! ".ttt<Bar>exec "!start notepad /p ".ttt<Bar>exec "!del ".ttt<CR>
endif
10amenu File.Save-Exit\ \ \ \ :wqa	:wqa<CR>
10amenu File.Exit\ \ \ \ \ \ \ \ \ :qa	:qa<CR>
10amenu File.Exit\ NO\ Save\ :qa!	:qa!<CR>

" Edit menu
20amenu Edit.Undo			u
20amenu Edit.Redo			<C-R>
20vmenu Edit.Cut			x
20vmenu Edit.Copy			y
20nmenu Edit.Put\ Before		[p
20imenu Edit.Put\ Before		<C-O>[p
20nmenu Edit.Put\ After			]p
20imenu Edit.Put\ After			<C-O>]p
20nmenu Edit.Paste			i<C-R>*<Esc>
20vmenu Edit.Paste			""xi<C-R>*<Esc>
20menu! Edit.Paste			<C-R>*
20amenu Edit.Search			/
20amenu Edit.Search\ \&\ Replace	:%s/
20vmenu Edit.Search\ \&\ Replace	:s/

" Window menu			    
30amenu Window.New			<C-W>n
30amenu Window.Split			<C-W>s
30amenu Window.Split\ To\ #		<C-W><C-^>
30amenu Window.Close			<C-W>c
30amenu Window.Close\ Other(s)		<C-W>o
30amenu Window.Next			<C-W>w
30amenu Window.Previous			<C-W>W
30amenu Window.Equal\ Height		<C-W>=
30amenu Window.Rotate\ Up		<C-W>R
30amenu Window.Rotate\ Down		<C-W>r
if has("gui_win32")
  30amenu Window.Select\ Font		:set guifont=*<CR>
endif

" Programming menu
40amenu IDE.Make			:make<CR>
40amenu IDE.List\ Errors		:cl<CR>
40amenu IDE.List\ Messages		:cl!<CR>
40amenu IDE.Next\ Error			:cn<CR>
40amenu IDE.Previous\ Error		:cp<CR>

" Restore the previous value of 'cpoptions'.
let &cpo = cpo_save
unlet cpo_save
