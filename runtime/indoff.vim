" Vim support file to switch off loading indent files for file types
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	2000 Aug 25

if exists("did_load_indent")
  unlet did_load_indent
endif

" Remove all autocommands in the filetypeindent group
au! filetypeindent *
