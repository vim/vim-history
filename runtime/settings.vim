" Vim support file to switch on loading settings for file types
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	2000 Aug 12

if exists("did_load_settings")
  finish
endif
let did_load_settings = 1

augroup filetypesettings
  au FileType * runtime! settings/<amatch>.vim
augroup END
