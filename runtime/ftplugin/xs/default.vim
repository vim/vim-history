" Vim filetype plugin file
" Language:	XS (Perl extension interface language)
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2000 Oct 15

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif

" Just use the C plugin for now.
runtime! ftplugin/c/*.vim
