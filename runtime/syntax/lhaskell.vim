" Vim syntax file
" Language:	Haskell with literate comments
" Maintainer:	John Williams <jrw@pobox.com>
" Last Change:	2001 Jan 15

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Enable literate comments
let b:hs_literate_comments=1

" Include standard Haskell highlighting
runtime! syntax/haskell.vim

" vim: ts=8
