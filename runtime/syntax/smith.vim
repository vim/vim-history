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

" The default highlighting.
hi def link smithRegister	Identifier
hi def link smithKeyword	Keyword
hi def link smithComment Comment
hi def link smithString String
hi def link smithNumber	Number

let b:current_syntax = "smith"

" vim: ts=2
