" Set options and add mapping such that Vim behaves a lot like MS-Windows
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	1999 Aug 09

" make 'cpoptions' empty
if 1	" only do this when compiled with expression evaluation
  let mswin_save_cpo = &cpoptions
endif
set cpoptions=

" set 'selection', 'selectmode', 'mousemodel' and 'keymodel' for MS-Windows
behave mswin

" backspace and cursor keys wrap to previous/next line
set backspace=2 whichwrap+=<,>,[,]

" backspace in Visual mode deletes selection
vmap <BS> d

" CTRL-X and SHIFT-Del are Cut
vmap <C-X> "*x
vmap <S-Del> "*x

" CTRL-C and CTRL-Insert are Copy
vmap <C-C> "*y
vmap <C-Insert> "*y

" CTRL-V and SHIFT-Insert are Paste
nm \\Paste\\ "=@*.'xy'<CR>gPFx"_2x:echo<CR>
map <C-V>				\\Paste\\
map <S-Insert>				\\Paste\\
if has("gui")
  nmenu 20.360 &Edit.&Paste		\\Paste\\
  nmenu  1.40  PopUp.&Paste		\\Paste\\
  nmenu        ToolBar.Paste		\\Paste\\
endif

imap <C-V>				x<Esc>\\Paste\\"_s
imap <S-Insert>				x<Esc>\\Paste\\"_s
if has("gui")
  imenu 20.360 &Edit.&Paste		x<Esc>\\Paste\\"_s
  imenu  1.40  PopUp.&Paste		x<Esc>\\Paste\\"_s
  imenu        ToolBar.Paste		x<Esc>\\Paste\\"_s
endif

cmap <C-V>      <C-R>*
cmap <S-Insert> <C-R>*

vmap <C-V>				"-cx<Esc>\\Paste\\"_x
vmap <S-Insert>				"-cx<Esc>\\Paste\\"_x
if has("gui")
  vmenu 20.360 &Edit.&Paste		"-cx<Esc>\\Paste\\"_x
  vmenu  1.40  PopUp.&Paste		"-cx<Esc>\\Paste\\"_x
  vmenu        ToolBar.Paste		"-cx<Esc>\\Paste\\"_x
endif

" Use CTRL-Q to do what CTRL-V used to do
noremap <C-Q> <C-V>

" For CTRL-V to work autoselect must be off
set guioptions-=a

" CTRL-Z is Undo
map <C-Z> u
imap <C-Z> <C-O>u
cmap <C-Z> <C-C><C-Z>

" CTRL-Y is Redo (although not repeat)
map <C-Y> <C-R>
imap <C-Y> <C-O><C-R>
cmap <C-Y> <C-C><C-Y>

" Alt-Space is System menu
if has("gui")
  map <M-Space> :simalt ~<CR>
  imap <M-Space> <C-O>:simalt ~<CR>
  cmap <M-Space> <C-C><M-Space>
endif

" CTRL-A is Select all
map <C-A> gggH<C-O>G
imap <C-A> <C-O>gg<C-O>gH<C-O>G
cmap <C-A> <C-C><C-A>

" CTRL-Tab is Next window
map <C-Tab> <C-W>w
imap <C-Tab> <C-O><C-W>w
cmap <C-Tab> <C-C><C-Tab>

" CTRL-F4 is Close window
map <C-F4> <C-W>c
imap <C-F4> <C-O><C-W>c
cmap <C-F4> <C-C><C-F4>

" restore 'cpoptions'
set cpoptions&
if 1
  let &cpoptions = mswin_save_cpo
  unlet mswin_save_cpo
endif
