" Vim filetype plugin file
" Language:	Changelog
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2000 Dec 10

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif

" Don't load another plugin for this buffer
let b:did_ftplugin = 1

setlocal tw=78
