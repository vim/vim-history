" hdiffwin.vim: a simple way to use diff to compare two source files
"               and to synchronize the three windows to the current,
"               next, and previous difference blocks.
" 
"   Last Change: November 3, 2000
"   Maintainer:  Charles E. Campbell, Jr.   (Charles.E.Campbell.1@gsfc.nasa.gov)
"
"   NOTE: this script requires 6.0k or later!!!!
"
" To enable: put this file into your <.vimrc> or source it from there.
"       You may wish to modify the maps' temporary directory;
"       its easiest to use vms's version: copy it, then :s/tmp:/newpath/g
"
" To use: start Vim as shown below, use \df to generate differences
"
"       vim -o newfile oldfile
"       \df                         <-- sets up difference window and
"                                       synchronizes to 1st diff
"
" The resulting three windows will look like this:
"
"                        Diff Block Format:
"       +-------+
"       | diff  |       *** oldfilename date
"       +---+---+       --- newfilename date
"       |   |   |       ***************
"       |new|old|       *** #,# ****
"       |   |   |       how to convert new -> old (shows new stuff)
"       +---+---+       --- #,# ----
"                       how to convert old -> new (shows old stuff)
"                       ***************
"
" You can synchronize the files in the new&old windows to the current
" difference-block being considered: just move the cursor in the diff
" window to the difference of interest and hit the "\dc".  Use "\dn"
" and "\dp" to navigate to the next/previous difference block,
" respectively.
"
" Maps:
"  \df : initialize: opens a third window on top with the diff file.
"  \da : synchronize windows to alternate diff, cursor at old->new diff section
"  \dc : synchronize windows to current   diff, cursor at new->old diff section
"  \dh : brief help message
"  \dl : apply patch from right->left (old->new)
"  \dn : synchronize windows to next      diff
"  \dp : synchronize windows to previous  diff
"  \dr : apply patch from left->right (new->old)
"  \ds : reSet diff (re-runs diff on new/old files)

" allow for those who use compatability mode
let s:save_cpo= &cpo
set cpo&vim

" allow user to prevent loading
" and prevent duplicate loading
if exists("loaded_hdiffwin")
  finish
endif
let loaded_hdiffwin= 1

" Public Interface:
if !hasmapto('<Plug>HdiffwinAlt')
  map   <unique> <Leader>da <Plug>HdiffwinAlt
endif
if !hasmapto('<Plug>HdiffwinCurrent')
  map   <unique> <Leader>dc <Plug>HdiffwinCurrent
endif
if !hasmapto('<Plug>HdiffwinInit')
  map   <unique> <Leader>df <Plug>HdiffwinInit
endif
if !hasmapto('<Plug>HdiffwinHelp')
  map   <unique> <Leader>dh <Plug>HdiffwinHelp
endif
if !hasmapto('<Plug>HdiffwinLeft')
  map   <unique> <Leader>dl <Plug>HdiffwinLeft
endif
if !hasmapto('<Plug>HdiffwinNext')
  map   <unique> <Leader>dn <Plug>HdiffwinNext
endif
if !hasmapto('<Plug>HdiffwinPrev')
  map   <unique> <Leader>dp <Plug>HdiffwinPrev
endif
if !hasmapto('<Plug>HdiffwinRight')
  map   <unique> <Leader>dr <Plug>HdiffwinRight
endif
if !hasmapto('<Plug>HdiffwinReset')
  map   <unique> <Leader>ds <Plug>HdiffwinReset
endif

" Global Maps:
map     <unique> <script> <Plug>HdiffwinAlt		<SID>current/^--- <CR>z<CR><SID>rtol
map     <unique> <script> <Plug>HdiffwinHelp	:echo "\\df=init \\ds=reset \\dn=nxt \\dp=prv \\da=alt \\dc=current \\dl=cp2left \\dr=cp2right"<CR>
map     <unique> <script> <Plug>HdiffwinInit	:let lzs1=&lz<CR>:sba<CR>:set lz<CR>:let sb1=&sb<CR>:set nosb<CR><C-W>k<C-W>j<C-W>v:e #1<CR><C-W>k:ene<CR>:put =system(\"diff -c \".expand(\"#1\").\" \".expand(\"#2\"))<CR>:set nomod<CR>:set ft=diff<CR>gg:let &lz=lzs1<CR>:let &sb=sb1<CR><SID>next
map     <unique> <script> <Plug>HdiffwinLeft	:call <SID>DiffPatch(1)<CR><SID>reset
map     <unique> <script> <Plug>HdiffwinPrev	:let lzs4=&lz<CR>:set lz<CR><C-W>k?^\*\{15}$<CR>?<CR>j<SID>current:let &lz=lzs4<CR><SID>ltor
map     <unique> <script> <Plug>HdiffwinRight	:call <SID>DiffPatch(0)<CR><SID>reset

" Local And Global Map Interface:
map     <unique> <script> <Plug>HdiffwinCurrent	<SID>current<SID>ltor
map     <unique> <script> <Plug>HdiffwinNext	<SID>next
map     <unique> <script> <Plug>HdiffwinReset	<SID>reset<SID>ltor

" Local And Global Maps:
map     <unique> <script> <SID>next				:let lzs3=&lz<CR>:set lz<CR><C-W>k/^\*\{15}$<CR>j<SID>current:let &lz=lzs3<CR><SID>ltor
map     <unique> <script> <SID>reset			:let lzs5=&lz<CR>:set lz<CR><C-W>k:wa<CR>:%d<CR>:put =system(\"diff -c \".expand(\"#1\").\" \".expand(\"#2\"))<CR>:set nomod<CR>:set ft=diff<CR>gg:let &lz=lzs5<CR><CR><SID>next
noremap <unique> <script> <SID>current			:let lzs2=&lz<CR>:set lz<CR><C-W>k?^\*\{15}$<CR>jYpdwf,DAGz<C-V><CR><Esc>"aYdd<C-W>j@a<C-W>k?^\*\{4}<CR>/^--- <CR>Ypdwf,DAGz<C-V><CR><Esc>"aYdd<C-W>j<C-W>l@a<C-W>k?^\*\{3} <CR>z<CR>:set nomod<CR>:let &lz=lzs2<CR>:echo "diff converts left window to right window"<CR>
noremap <unique> <script> <SID>ltor				:echo "diff converts left window to right window"<CR>
noremap <unique> <script> <SID>rtol				:echo "diff converts right window to left window"<CR>

" ---------------------------------------------------------------------

" <SID>DiffPatch: applies current patch section to newfile/oldfile
"  Uses anonymous register
"  Variables:
"    old2new: =1 DiffPatch being used to convert old -> new
"             =0 DiffPatch being used to convert new -> old
function! <SID>DiffPatch(old2new)

  " use lazy updating
  let l:lzs8=&lz
  set lz

  exe "norm \<c-w>k?\\*\\{15}$\<CR>j"
  let l:newpat='\*\*\* \(\d\+\),\(\d\+\).*$'
  let l:new1=substitute(getline("."),l:newpat,'\1',"")
  let l:new2=substitute(getline("."),l:newpat,'\2',"")

  exe "norm /^--- \<CR>"
  let l:oldpat='--- \(\d\+\),\(\d\+\).*$'
  let l:old1=substitute(getline("."),l:oldpat,'\1',"")
  let l:old2=substitute(getline("."),l:oldpat,'\2',"")

  if a:old2new == 1	" \dl : copy from right to left window
    exe "norm \<c-w>k\<c-w>j:".l:new1.",".l:new2."d\<CR>"
    exe "norm \<c-w>k\<c-w>j\<c-w>l:".l:old1.",".l:old2."y\<CR>"
	let l:new1=l:new1-1
	if l:new1 > 0
	  exe "norm \<c-w>h".l:new1."Gp"
	else
	  exe "norm \<c-w>h1GP"
	endif
  else				" \dr : copy from left to right window
    exe "norm \<c-w>k\<c-w>j\<c-w>l:".l:old1.",".l:old2."d\<CR>"
    exe "norm \<c-w>k\<c-w>j:".l:new1.",".l:new2."y\<CR>"
	let l:old1=l:old1-1
	if l:old1 > 0
	  exe "norm \<c-w>l".l:old1."Gp"
	else
	  exe "norm \<c-w>l1GP"
	endif
  endif

  let &lz=l:lzs8
endfunction

" ---------------------------------------------------------------------
" Restoration

" restore cpo
let &cpo= s:save_cpo
unlet s:save_cpo

" ---------------------------------------------------------------------
" vim:ts=4

