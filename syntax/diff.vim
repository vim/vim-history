" Vim syntax file
" Language:	Diff (context or unified)
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	1997 Nov 3

" Remove any old syntax stuff hanging around
syn clear

syn match diffOnly	"^Only in.*$"

syn match diffRemoved	"^-.*$"
syn match diffAdded	"^+.*$"
syn match diffChanged	"^! .*$"

syn match diffLine	"^@.*$"
syn match diffLine	"^\<\d\+\>.*$"
syn match diffLine	"^\*\*\*\*.*$"

"Some versions of diff have lines like "#c#" and "#d#" (where # is a number)
syn match diffAdded	"^\<\d\+a\d\+\(,\d\+\)\=\>"
syn match diffChanged	"^\<\d\+c\d\+\>"
syn match diffRemoved	"^\<\d\+d\d\+\>"

syn match diffFile	"^diff.*$"
syn match diffFile	"^+++ .*$"
syn match diffOldFile	"^\*\*\* .*$"
syn match diffNewFile	"^--- .*$"

if !exists("did_diff_syntax_inits")
  let did_diff_syntax_inits = 1
  hi link diffOldFile	diffFile
  hi link diffNewFile	diffFile
  hi link diffFile	Type
  hi link diffOnly	Constant
  hi link diffRemoved	Comment
  hi link diffChanged	PreProc
  hi link diffAdded	Identifier
  hi link diffLine	Statement
endif

let b:current_syntax = "diff"

" vim: ts=8
