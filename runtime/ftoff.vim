" Vim support file to switch off detection of file types
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	2000 Oct 15

if exists("did_load_filetypes")
  unlet did_load_filetypes
endif

" Remove all autocommands in the filetypedetect group
au! filetypedetect *

" Also switch off loading settings and indent files
source <sfile>:p:h/ftplugof.vim
source <sfile>:p:h/indoff.vim
