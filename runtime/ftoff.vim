" Vim support file to switch off detection of file types
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	2000 Jul 15

if exists("did_load_filetypes")
  unlet did_load_filetypes
endif

if exists("did_load_settings")
  unlet did_load_settings
endif

" Remove all autocommands in the filetypedetect group
au! filetypedetect *
" Remove all autocommands in the filetypesettings group
au! filetypesettings *
