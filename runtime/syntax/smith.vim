" Vim syntax file
" Language:	SMITH
" Maintainer:	Rafal M. Sulejman <rms@poczta.onet.pl>
" Last Change:	21.07.2000

" Remove any old syntax stuff hanging around
syn clear

syn case ignore


syn match smithComment ";.*$"

syn match smithNumber		"\<[+-]*[0-9]\d*\>"

syn match smithRegister		"R[\[]*[0-9]*[\]]*"

syn match smithKeyword	"COR\|MOV\|MUL\|NOT\|STOP\|SUB\|NOP\|BLA\|REP"

syn region smithString		start=+"+  skip=+\\\\\|\\"+  end=+"+


syn case match

if !exists("did_smith_syntax_inits")
  let did_smith_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link smithRegister	Identifier
  hi link smithKeyword	Keyword
	hi link smithComment Comment
	hi link smithString String
  hi link smithNumber	Number
endif

let b:current_syntax = "smith"

" vim: ts=2
