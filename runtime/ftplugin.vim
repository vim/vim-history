" Vim support file to switch on loading plugins for file types
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	2000 Nov 10

if exists("did_load_ftplugin")
  finish
endif
let did_load_ftplugin = 1

augroup filetypeplugin
  au FileType * if expand("<amatch>") != "" | 
	\   runtime! ftplugin/<amatch>/*.vim |
	\ endif
augroup END
