" Vim indent file
" Language:	Java
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 Jun 18

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
   finish
endif
let b:did_indent = 1

" Java is just like C, use the built-in C indenting.
setlocal cindent

" Indent Java anonymous classes correctly.
setlocal cinoptions+=j1
