" hdiffwin.vim: a simple way to use diff to compare two source files
"               and to synchronize the three windows to the current,
"               next, and previous difference blocks.
" 
"   Author: Charles E. Campbell, Jr.   (Charles.E.Campbell.1@gsfc.nasa.gov)
"   Date:   9/22/2000
"
" To enable: put this file into your <.vimrc> or source it from there.
"       You may wish to modify the maps' temporary directory;
"       its easiest to use vms's version: copy it, then :s/tmp:/newpath/g
"
" To use: start Vim as shown below, use \df to generate differences,
"         and then hit the <F8> key:
"
"       vim -o newfile oldfile
"       \df                         <-- sets up difference window
"       <F8>                        <-- synchronize to current diff
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
" (or "F8") and "\dp" to navigate to the next/previous difference block,
" respectively.
"
" Maps:
"  \df : opens a third window on top with the diff file.
"  \dc : synchronize windows to current  diff, cursor at new->old diff section
"  \dC : synchronize windows to current  diff, cursor at old->new diff section
"  \dn : synchronize windows to next     diff
"  \dp : synchronize windows to previous diff
"  \dr : apply patch from left->right (new->old)
"  \dl : apply patch from right->left (old->new)
"  \ds : reSet diff (re-runs diff on new/old files)
"  <F8>: same as \dn (ie. go to next diff)
map \df :let lzs1=&lz<CR>:set lz<CR><C-W>k<C-W>j<C-W>v:e #1<CR><C-W>k:ene<CR>:exe "0r !diff -c ".expand("#1")." ".expand("#2")<CR>:set nomod<CR>:set ft=diff<CR>gg:let &lz=lzs1<CR><CR>\dn
map \dc :let lzs2=&lz<CR>:set lz<CR><C-W>k?^\*\{15}$<CR>jYpdwf,DAGz<C-V><CR><Esc>"aYdd<C-W>j@a<C-W>k?^\*\{4}<CR>/^--- <CR>Ypdwf,DAGz<C-V><CR><Esc>"aYdd<C-W>j<C-W>l@a<C-W>k?^\*\{3} <CR>z<CR>:set nomod<CR>:let &lz=lzs2<CR>:echo "diff converts left window to right window"<CR>
map \dC \dc/^--- <CR>z<CR>:echo "diff converts right window to left window"<CR>
map \dl :call DiffPatch(1)<CR>\ds
map \dn :let lzs3=&lz<CR>:set lz<CR><C-W>k/^\*\{15}$<CR>j\dc:let &lz=lzs3<CR>
map \dp :let lzs4=&lz<CR>:set lz<CR><C-W>k?^\*\{15}$<CR>?<CR>j\dc:let &lz=lzs4<CR>
map \dr :call DiffPatch(0)<CR>\ds
map \ds :let lzs5=&lz<CR>:set lz<CR><C-W>k:exe "0r !diff -c ".expand("#1")." ".expand("#2")<CR>:set nomod<CR>:set ft=diff<CR>gg:let &lz=lzs5<CR><CR>\dn
map <F8> \dn

" ---------------------------------------------------------------------

" DiffPatch: applies current patch section to newfile/oldfile
"  Uses anonymous register
"  Variables:
"    old2new: =1 DiffPatch being used to convert old -> new
"             =0 DiffPatch being used to convert new -> old
fu! DiffPatch(old2new)

  " use lazy updating
  let lzs8=&lz
  set lz

  exe "norm \<c-w>k?\\*\\{15}$\<CR>j"
  let newpat='\*\*\* \(\d\+\),\(\d\+\).*$'
  let new1=substitute(getline("."),newpat,'\1',"")
  let new2=substitute(getline("."),newpat,'\2',"")

  exe "norm /^--- \<CR>"
  let oldpat='--- \(\d\+\),\(\d\+\).*$'
  let old1=substitute(getline("."),oldpat,'\1',"")
  let old2=substitute(getline("."),oldpat,'\2',"")

  if a:old2new == 1
    exe "norm \<c-w>k\<c-w>j:".new1.",".new2."d\<CR>"
    exe "norm \<c-w>k\<c-w>j\<c-w>l:".old1.",".old2."y\<CR>"
	let new1=new1-1
	exe "norm \<c-w>h".new1."Gp"
  else
    exe "norm \<c-w>k\<c-w>j\<c-w>l:".old1.",".old2."d\<CR>"
    exe "norm \<c-w>k\<c-w>j:".new1.",".new2."y\<CR>"
	let old1=old1-1
	exe "norm \<c-w>l".old1."Gp"
  endif

  let &lz=lzs8
endfunction

" ---------------------------------------------------------------------
" vim:ts=4
