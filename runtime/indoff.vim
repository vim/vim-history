" Vim support file to switch off loading indent files for file types
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 Jan 15

if exists("did_indent_on")
  unlet did_indent_on
endif

" Remove all autocommands in the filetypeindent group
au! filetypeindent *
