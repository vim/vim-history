" Vim syntax file
" Language:	Quickfix window
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	1999 Sep 18

" Remove any old syntax stuff hanging around
syn clear

" A bunch of useful C keywords
syn match	qfFileName	"^[^|]*" nextgroup=qfSeparator
syn match	qfSeparator	"|" nextgroup=qfLineNr contained
syn match	qfLineNr	"[^|]*" contained contains=qfError
syn match	qfError		"error" contained

if !exists("did_qf_syntax_inits")
  let did_qf_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link qfFileName	Directory
  hi link qfLineNr	LineNr
  hi link qfError	Error
endif

let b:current_syntax = "qf"

" vim: ts=8
