" Vim settings file
" Language:	XS (Perl extension interface language)
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2000 Jul 21

" Only do these settings when not done yet for this buffer
if exists("b:did_settings")
  finish
endif

" Just use the C settings for now.
runtime settings/c.vim
