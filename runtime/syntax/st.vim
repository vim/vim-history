" Vim syntax file
" Language:	Smalltalk
" Maintainer:	Arndt Hesse <hesse@self.de>
" Last Change:	2000 Nov 04

" remove any old syntax stuff hanging around
syn clear

" some Smalltalk keywords and standard methods
syn keyword	stKeyword	super self class true false new not
syn keyword	stKeyword	notNil isNil inspect out nil
syn match	stMethod	"\<do\>:"
syn match	stMethod	"\<whileTrue\>:"
syn match	stMethod	"\<whileFalse\>:"
syn match	stMethod	"\<ifTrue\>:"
syn match	stMethod	"\<ifFalse\>:"
syn match	stMethod	"\<put\>:"
syn match	stMethod	"\<to\>:"
syn match	stMethod	"\<at\>:"
syn match	stMethod	"\<add\>:"
syn match	stMethod	"\<new\>:"
syn match	stMethod	"\<for\>:"
syn match	stMethod	"\<methods\>:"
syn match	stMethod	"\<methodsFor\>:"
syn match	stMethod	"\<instanceVariableNames\>:"
syn match	stMethod	"\<classVariableNames\>:"
syn match	stMethod	"\<poolDictionaries\>:"
syn match	stMethod	"\<subclass\>:"

" the block of local variables of a method
syn region stLocalVariables	start="^[ \t]*|" end="|"

" the Smalltalk comment
syn region stComment	start="\"" end="\""

" the Smalltalk strings and single characters
syn region stString	start='\'' skip="''" end='\''
syn match  stCharacter	"$."

syn case ignore

" the symols prefixed by a '#'
syn match  stSymbol	"\(#\<[a-z_][a-z0-9_]*\>\)"
syn match  stSymbol	"\(#'[^']*'\)"

" the variables in a statement block for loops
syn match  stBlockVariable "\(:[ \t]*\<[a-z_][a-z0-9_]*\>[ \t]*\)\+|" contained

" some representations of numbers
syn match  stNumber	"\<\d\+\(u\=l\=\|lu\|f\)\>"
syn match  stFloat	"\<\d\+\.\d*\(e[-+]\=\d\+\)\=[fl]\=\>"
syn match  stFloat	"\<\d\+e[-+]\=\d\+[fl]\=\>"

syn case match

" a try to higlight paren mismatches
syn region stParen	transparent start='(' end=')' contains=ALLBUT,stParenError
syn match  stParenError	")"
syn region stBlock	transparent start='\[' end='\]' contains=ALLBUT,stBlockError
syn match  stBlockError	"\]"
syn region stSet	transparent start='{' end='}' contains=ALLBUT,stSetError
syn match  stSetError	"}"

" synchronization for syntax analysis
syn sync minlines=50

" The default highlighting.
hi def link stParenError	stError
hi def link stSetError		stError
hi def link stBlockError	stError

hi def link stKeyword		Statement
hi def link stMethod		Statement
hi def link stComment		Comment
hi def link stCharacter		Constant
hi def link stString		Constant
hi def link stSymbol		Special
hi def link stNumber		Type
hi def link stFloat		Type
hi def link stError		Error
hi def link stLocalVariables	Identifier
hi def link stBlockVariable	Identifier

let b:current_syntax = "st"
