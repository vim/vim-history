" Vim syntax file
" Language   : binary processed by xxd
" Version    : 5.4-2
" Maintainer : Dr. Charles E. Campbell, Jr. <Charles.Campbell@gsfc.nasa.gov>
" Last change: March 19, 1999
"  Notes: use :help xxd   to see how to invoke it

" Removes any old syntax stuff hanging around
syn clear

syn match xxdAddress			"^[0-9a-f]\+:"	contains=xxdSep
syn match xxdSep	contained	":"
syn match xxdAscii				"  .\{,16\}$"hs=s+2	contains=xxdDot
syn match xxdDot	contained	"\."

if !exists("did_xxd_syntax_inits")
 let did_xxd_syntax_inits = 1

 hi link xxdAddress	Constant
 hi link xxdSep		Identifier
 hi link xxdAscii	Statement
endif

let b:current_syntax = "xxd"

" vim: ts=4
