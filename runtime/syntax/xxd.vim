" Vim syntax file
" Language:		bin using xxd
" Version:		5.4-3
" Maintainer:	Dr. Charles E. Campbell, Jr. <Charles.E.Campbell.1@gsfc.nasa.gov>
" Last Change:	March 19, 1999
"  Notes: use :help xxd   to see how to invoke it

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn match xxdAddress			"^[0-9a-f]\+:"		contains=xxdSep
syn match xxdSep	contained	":"
syn match xxdAscii				"  .\{,16\}\r\=$"hs=s+2	contains=xxdDot
syn match xxdDot	contained	"[.\r]"


" The default highlighting.
hi def link xxdAddress	Constant
hi def link xxdSep		Identifier
hi def link xxdAscii	Statement

let b:current_syntax = "xxd"

" vim: ts=4
