" Vim filetype plugin file
" Language:	C++
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2000 Dec 23

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif

" Behaves just like C
runtime! ftplugin/c/*.vim
