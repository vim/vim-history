" Vim support file to switch on loading indent files for file types
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	2000 Nov 10

if exists("did_load_indent")
  finish
endif
let did_load_indent = 1

augroup filetypeindent
  au FileType * if expand("<amatch>") != "" | 
	\   runtime indent/<amatch>.vim |
	\ endif
augroup END
