" A simple way to create a window which contains the diff of two existing
" windows.  Charles E Campbell, Jr, PhD   <cec@gryphon.gsfc.nasa.gov>
"
" Start Vim with:
"
"	vim -o newfile oldfile
"
" Then load these macros and type
"
"	\df
"
" The resulting three windows will look like this:
"
"	+----+
"	|diff|
"	+----+
"	|new |
"	+----+
"	|old |
"	+----+
"
" You can synchronize the files in the new&old windows to the current
" difference-block being considered.  Just move the cursor in the diff window
" to the difference of interest and hit F8.

map \df  <C-W>k<C-W>j:!diff -c <C-R>% <C-R># > /tmp/vimtmp.dif<C-M><C-W>k<C-W>s:e! /tmp/vimtmp.dif<C-M>:!/bin/rm /tmp/vimtmp.dif<C-M><C-M>

map <F8> <C-W>k<C-W>k?^\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*$<CR>jYpdwf,DAGz<C-V><CR><Esc>"aYdd<C-W>j<C-W>j@a<C-W>k<C-W>k?^\*\*\*\*<CR>/^--- <CR>Ypdwf,DAGz<C-V><CR><Esc>"aYdd<C-W>j@a<C-W>kuuz<CR>
