" Vim syntax file
" Language:	Man page
" Maintainer:	Sung-Hyun Nam <namsh@kldp.org>
" Previous Maintainer:	Gautam H. Mudunuri <gmudunur@informatica.com>
" Last Change:	2001 Feb 02
" Version Info:

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Get the CTRL-H syntax to handle backspaced text
runtime! syntax/ctrlh.vim

syn case ignore
syn match  manReference       "\f\+([1-9][a-z]\=)"
syn match  manTitle           "^\f\+([0-9]\+[a-z]\=).*"
syn match  manSectionHeading  "^[a-z][a-z ]*[a-z]$"
syn match  manOptionDesc      "^\s*[+-][a-z0-9]\S*"
" syn match  manHistory         "^[a-z].*last change.*$"

" The default highlighting.
hi def link manTitle           Title
hi def link manSectionHeading  Statement
hi def link manOptionDesc      Constant
" hi def link manHistory       Comment
hi def link manReference       PreProc

let b:current_syntax = "man"

" vim:ts=8
