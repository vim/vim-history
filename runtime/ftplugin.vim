" Vim support file to switch on loading plugins for file types
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	2001 Jan 15

if exists("did_load_ftplugin")
  finish
endif
let did_load_ftplugin = 1

augroup filetypeplugin
  au FileType * if expand("<amatch>") != "" | 
	\   runtime! ftplugin/<amatch>.vim
	\	 ftplugin/<amatch>_*.vim ftplugin/<amatch>/*.vim |
	\ endif
augroup END
