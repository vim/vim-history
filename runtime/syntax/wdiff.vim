" Vim syntax file
" Language:     wDiff (wordwise diff)
" Maintainer:   Gerfried Fuchs <alfie@innocent.com>
" URL:          http://alfie.ist.org/vim/syntax/wdiff.vim
" Last Change:  15 Sep 2000
"
" Comments are very welcome - but please make sure that you are commenting on
" the latest version of this file.

" Remove any old syntax stuff hanging around
syn clear

syn region wdiffOld start=+\[-+ end=+-]+
syn region wdiffNew start="{+" end="+}"


if !exists("did_wdiff_syntax_inits")
  let did_wdiff_syntax_inits = 1
  hi link wdiffOld      Special
  hi link wdiffNew      Identifier
endif

let b:current_syntax = "wdiff"
