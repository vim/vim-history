" Set options and add mapping such that Vim behaves a lot like MS-Windows
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	2001 Apr 10

" set the 'cpoptions' to its Vim default
if 1	" only do this when compiled with expression evaluation
  let s:save_cpo = &cpoptions
endif
set cpo&vim

" set 'selection', 'selectmode', 'mousemodel' and 'keymodel' for MS-Windows
behave mswin

" backspace and cursor keys wrap to previous/next line
set backspace=2 whichwrap+=<,>,[,]

" backspace in Visual mode deletes selection
vnoremap <BS> d

" CTRL-X and SHIFT-Del are Cut
vnoremap <C-X> "+x
vnoremap <S-Del> "+x

" CTRL-C and CTRL-Insert are Copy
vnoremap <C-C> "+y
vnoremap <C-Insert> "+y

" CTRL-V and SHIFT-Insert are Paste
nnoremap <SID>Paste "=@+.'xy'<CR>gPFx"_2x:echo<CR>
map <C-V>				<SID>Paste
map <S-Insert>				<SID>Paste
if has("gui")
  nmenu 20.360 &Edit.&Paste		<SID>Paste
  nmenu  1.40  PopUp.&Paste		<SID>Paste
  nmenu        ToolBar.Paste		<SID>Paste
endif

imap <C-V>				x<Esc><SID>Paste"_s
imap <S-Insert>				x<Esc><SID>Paste"_s
if has("gui")
  imenu 20.360 &Edit.&Paste		x<Esc><SID>Paste"_s
  imenu  1.40  PopUp.&Paste		x<Esc><SID>Paste"_s
  imenu        ToolBar.Paste		x<Esc><SID>Paste"_s
endif

cmap <C-V>      <C-R>+
cmap <S-Insert> <C-R>+

vmap <C-V>				"-cx<Esc><SID>Paste"_x
vmap <S-Insert>				"-cx<Esc><SID>Paste"_x
if has("gui")
  vmenu 20.360 &Edit.&Paste		"-cx<Esc><SID>Paste"_x
  vmenu  1.40  PopUp.&Paste		"-cx<Esc><SID>Paste"_x
  vmenu        ToolBar.Paste		"-cx<Esc><SID>Paste"_x
endif

" Use CTRL-Q to do what CTRL-V used to do
noremap <C-Q> <C-V>

" For CTRL-V to work autoselect must be off.
" On Unix we have two selections, autoselect can be used.
if !has("unix")
  set guioptions-=a
endif

" CTRL-Z is Undo
noremap <C-Z> u
inoremap <C-Z> <C-O>u
cnoremap <C-Z> <C-C>u

" CTRL-Y is Redo (although not repeat)
noremap <C-Y> <C-R>
inoremap <C-Y> <C-O><C-R>
cnoremap <C-Y> <C-C><C-R>

" Alt-Space is System menu
if has("gui")
  noremap <M-Space> :simalt ~<CR>
  inoremap <M-Space> <C-O>:simalt ~<CR>
  cnoremap <M-Space> <C-C>:simalt ~<CR>
endif

" CTRL-A is Select all
noremap <C-A> gggH<C-O>G
inoremap <C-A> <C-O>gg<C-O>gH<C-O>G
cnoremap <C-A> <C-C>gggH<C-O>G

" CTRL-Tab is Next window
noremap <C-Tab> <C-W>w
inoremap <C-Tab> <C-O><C-W>w
cnoremap <C-Tab> <C-C><C-W>w

" CTRL-F4 is Close window
noremap <C-F4> <C-W>c
inoremap <C-F4> <C-O><C-W>c
cnoremap <C-F4> <C-C><C-W>c

" restore 'cpoptions'
set cpo&
if 1
  let &cpoptions = s:save_cpo
  unlet s:save_cpo
endif
