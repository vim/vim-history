" Vim support file to switch off loading plugins for file types
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	2000 Oct 15

if exists("did_load_ftplugin")
  unlet did_load_ftplugin
endif

" Remove all autocommands in the filetypeplugin group
au! filetypeplugin *
