" Vim syntax file
" Language:	Configuraion File (ini file) for MSDOS/MS Windows
" Maintainer:	Sean M. McKee <mckee@misslink.net>
" Last change:	1997/12/15
" Version Info: @(#)dosini.vim	1.6	97/12/15 08:54:12

" clear any unwanted syntax defs
syn clear

" shut case off
syn case ignore

syn match  dosiniLabel		"^.\{-}="
syn region dosiniHeader		start="\[" end="\]"
syn match  dosiniComment	";.*$"

if !exists("did_dosini_syntax_inits")
	let did_dosini_syntax_inits = 1
	" The default methods for highlighting.  Can be overridden later
	hi link dosiniHeader	Special
	hi link dosiniComment	Comment
	hi link dosiniLabel	Type

endif

let b:current_syntax = "dosini"

" vim:ts=8
