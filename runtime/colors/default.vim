" Vim color file
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 May 02

" This is the default color scheme.  It doesn't define the Normal
" highlighting, it uses whatever the colors used to be.

let colors_name = "default"

" Remove all existing highlighting and set the defaults.
hi clear

" Load the syntax highlighting defaults, if it's enabled.
if exists("syntax_on")
  runtime syntax/syncolor.vim
endif

" vim: sw=2
