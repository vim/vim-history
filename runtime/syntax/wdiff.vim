" Vim syntax file
" Language:     wDiff (wordwise diff)
" Maintainer:   Gerfried Fuchs <alfie@innocent.com>
" URL:          http://alfie.ist.org/vim/syntax/wdiff.vim
" Last Change:  15 Sep 2000
"
" Comments are very welcome - but please make sure that you are commenting on
" the latest version of this file.

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn region wdiffOld start=+\[-+ end=+-]+
syn region wdiffNew start="{+" end="+}"


" The default highlighting.
hi def link wdiffOld      Special
hi def link wdiffNew      Identifier

let b:current_syntax = "wdiff"
