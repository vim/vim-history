" Vim support file to switch off loading settings for file types
"
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	2000 Jul 15

if exists("did_load_settings")
  unlet did_load_settings
endif

" Remove all autocommands in the filetypesettings group
au! filetypesettings *
