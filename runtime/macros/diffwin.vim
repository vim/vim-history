" diffwin.vim: a simple way to use diff to compare two source files
"		       and to synchronize the three windows to the current,
"		       next, and previous difference blocks.
" 
"	Author : Charles E. Campbell, Jr.   (Charles.E.Campbell.1@gsfc.nasa.gov)
"	Date   : 9/22/2000
"
" To enable: put this file into your <.vimrc> or source it from there.
"		You may wish to modify the maps' temporary directory;
"		its easiest to use vms's version: copy it, then :s/tmp:/newpath/g
"
" To use: start Vim as shown below, use \df to generate differences,
"         and then hit the <F8> key:
"
"		vim -o newfile oldfile
"       \df
"		<F8>
"
" The resulting three windows will look like this:
"
"		             Diff Block Format:
"		+----+
"		|diff|		*** oldfilename date
"		+----+		--- newfilename date
"		|new |		***************
"		+----+		*** #,# ****
"		|old |      how to convert new -> old (shows new stuff)
"		+----+      --- #,# ----
"                   how to convert old -> new (shows old stuff)
"                   ***************
"
" You can synchronize the files in the new&old windows to the current
" difference-block being considered: just move the cursor in the diff
" window to the difference of interest and hit the "\dc".  Use "\dn"
" (or "F8") and "\dp" to navigate to the next/previous difference block,
" respectively.
"
" Maps:
"  \df : opens a third window on top with the diff file.
"  \dr : re-runs diff on new/old files
"  \dc : synchronize windows to current  diff, cursor at new->old diff section
"  \dC : synchronize windows to current  diff, cursor at old->new diff section
"  \dn : synchronize windows to next     diff
"  \dp : synchronize windows to previous diff
"  <F8>: same as \dn
map \df :let lzs1=&lz<CR><C-W>k<C-W>s:ene<CR>:exe "0r !diff -c ".expand("#1")." ".expand("#2")<CR>:set nomod<CR>:set ft=diff<CR>gg:let &lz=lzs1<CR><CR>
map \dr :let lzs2=&lz<CR>2<C-W>k:exe "0r !diff -c ".expand("#1")." ".expand("#2")<CR>:set nomod<CR>:set ft=diff<CR>gg:let &lz=lzs2<CR><CR>\dn
map \dc	:let lzs3=&lz<CR><C-W>k<C-W>k?^\*\{15}$<CR>jYpdwf,DAGz<C-V><CR><Esc>"bYdd<C-W>j@b<C-W>k?^\*\{4}<CR>/^--- <CR>Ypdwf,DAGz<C-V><CR><Esc>"aYdd2<C-W>j@a2<C-W>k?^\*\{3} <CR>z<CR>:set nomod<CR>:let &lz=lzs3<CR>:echo "diff converts middle window to lower window"<CR>
map \dC \dc/^--- <CR>z<CR>:echo "diff converts lower window to middle window"<CR>
map \dn :let lzs4=&lz<CR><C-W>k<C-W>k/^\*\{15}$<CR>j\dc:let &lz=lzs4<CR>
map \dp	:let lzs5=&lz<CR><C-W>k<C-W>k?^\*\{15}$<CR>?<CR>j\dc:let &lz=lzs5<CR>
map <F8> \dc
" vim:ts=4
