" Set options and add mapping such that Vim behaves a lot like MS-Windows

" make 'cpoptions' empty
let mswin_save_cpo = &cpoptions
set cpoptions=

" set 'selection', 'selectmode', 'mousemodel' and 'keymodel' for MS-Windows
behave mswin

" backspace and cursor keys wrap to previous/next line
set backspace=2 whichwrap+=<,>,[,]

" CTRL-X and SHIFT-Del are Cut
vmap <C-X> "*x
vmap <S-Del> "*x

" CTRL-C and CTRL-Insert are Copy
vmap <C-C> "*y
vmap <C-Insert> "*y

" CTRL-V and SHIFT-Insert are Paste
function! NormalPaste()
   if @* != ""
      normal "*gP
   endif
endfunction
map <C-V>				:call NormalPaste()<CR>
map <S-Insert>				:call NormalPaste()<CR>
if has("gui")
  nmenu 20.360 &Edit.&Paste<Tab>"*p	:call NormalPaste()<CR>
  nmenu  1.40  PopUp.&Paste		:call NormalPaste()<CR>
  nmenu        ToolBar.Paste		:call NormalPaste()<CR>
endif

imap <C-V>				x<Esc>:call NormalPaste()<CR>s
imap <S-Insert>				x<Esc>:call NormalPaste()<CR>s
if has("gui")
  imenu 20.360 &Edit.&Paste<Tab>"*p	x<Esc>:call NormalPaste()<CR>s
  imenu  1.40  PopUp.&Paste		x<Esc>:call NormalPaste()<CR>s
  imenu        ToolBar.Paste		x<Esc>:call NormalPaste()<CR>s
endif

cmap <C-V>      <C-R>*
cmap <S-Insert> <C-R>*

function! SelectPaste()
   if @* != ""
      if col(".") < col("'<")
         normal "*gp
      else
         normal "*gP
      endif
   endif
endfunction
vmap <C-V>				""x:call SelectPaste()<CR>
vmap <S-Insert>				""x:call SelectPaste()<CR>
if has("gui")
  vmenu 20.360 &Edit.&Paste<Tab>"*p	""x:call SelectPaste()<CR>
  vmenu  1.40  PopUp.&Paste		""x:call SelectPaste()<CR>
  vmenu        ToolBar.Paste		""x:call SelectPaste()<CR>
endif

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
let &cpoptions = mswin_save_cpo
unlet mswin_save_cpo
