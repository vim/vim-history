" Vim syntax file
" Language:	Man page
" Maintainer:	Gautam H. Mudunuri <gmudunur@informatica.com>
" Last Change:	2001 Jan 19
" Version Info:

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Get the CTRL-H syntax to handle backspaced text
runtime! syntax/ctrlh.vim

syn case ignore
syn match  manReference       "[a-z][a-z0-9_]*([1-9][a-z]\{0,1})"
syn match  manTitle           "^\i\+([0-9]\+[a-z]\=).*"
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
