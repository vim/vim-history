" Vim syntax file
" Language:	TCL
" Maintainer:	Robin Becker <robin@jessikat.demon.co.uk>
" Last change:	1997 July 11

" Remove any old syntax stuff hanging around
syn clear
" A bunch of useful C keywords
syn keyword tclStatement		proc global array set break return continue
syn keyword tclLabel 		case default
syn keyword tclConditional	if else switch
syn keyword tclRepeat		while for foreach

syn keyword tclTodo contained	TODO

" String and Character contstants
" Highlight special characters (those which have a backslash) differently
syn match   tclSpecial contained "\\[0-9][0-9][0-9]\|\\."
syn region  tclString		  start=+"+  skip=+\\\\\|\\"+  end=+"+  contains=tclSpecial

"integer number, or floating point number without a dot and with "f".
syn case ignore
syn match  tclNumber		"\<[0-9]\+\(u\=l\=\|lu\|f\)\>"
"floating point number, with dot, optional exponent
syn match  tclNumber		"\<[0-9]\+\.[0-9]*\(e[-+]\=[0-9]\+\)\=[fl]\=\>"
"floating point number, starting with a dot, optional exponent
syn match  tclNumber		"\.[0-9]\+\(e[-+]\=[0-9]\+\)\=[fl]\=\>"
"floating point number, without dot, with exponent
syn match  tclNumber		"\<[0-9]\+e[-+]\=[0-9]\+[fl]\=\>"
"hex number
syn match  tclNumber		"0x[0-9a-f]\+\(u\=l\=\|lu\)\>"
"syn match  tclIdentifier	"\<[a-z_][a-z0-9_]*\>"
syn case match

syn region  tclComment		start="^[ 	]*\#" end="$" contains=tclTodo

"syn sync ccomment tclComment

if !exists("did_tcl_syntax_inits")
  let did_tcl_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link tclLabel		Label
  hi link tclConditional	Conditional
  hi link tclRepeat		Repeat
  hi link tclNumber		Number
  hi link tclError		Error
  hi link tclStatement	Statement
  hi link tclString		String
  hi link tclComment		Comment
  hi link tclSpecial		Special
  hi link tclTodo		Todo
  "hi link tclIdentifier	Identifier
  "hi link tclOpSpecial	OpSpecial
endif

let b:current_syntax = "tcl"

" vim: ts=8
