" Vim support file to switch on loading indent files for file types
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	2000 Aug 25

if exists("did_load_indent")
  finish
endif
let did_load_indent = 1

augroup filetypeindent
  au FileType * runtime indent/<amatch>.vim
augroup END
