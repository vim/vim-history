" Vim syntax file
" Language:	Matlab
" Maintainer:	Preben "Peppe" Guldberg <c928400@student.dtu.dk>
"		Original author: Mario Eusebio
" Last Change:	Tue Jul 28 17:44:06 1998

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn keyword matlabStatement		return
syn keyword matlabLabel			case switch
syn keyword matlabConditional		else elseif end if otherwise
syn keyword matlabRepeat		do for while

syn keyword matlabTodo			contained  TODO

" If you do not want these operators lit, uncommment them and the "hi link" below
syn match matlabArithmeticOperator	"[-+]"
syn match matlabArithmeticOperator	"\.\=[*/\\^]"
syn match matlabRelationalOperator	"[=~]="
syn match matlabRelationalOperator	"[<>]=\="
syn match matlabLogicalOperator		"[&|~]"

syn match matlabLineContinuation	"\.\{3}"

"syn match matlabIdentifier		"\<\a\w*\>"

" String
syn region matlabString			start=+'+ end=+'+	oneline

" If you don't like tabs
syn match matlabTab			"\t"

" Standard numbers
syn match matlabNumber		"\<\d\+[ij]\=\>"
" floating point number, with dot, optional exponent
syn match matlabFloat		"\<\d\+\(\.\d*\)\=\([edED][-+]\=\d\+\)\=[ij]\=\>"
" floating point number, starting with a dot, optional exponent
syn match matlabFloat		"\.\d\+\([edED][-+]\=\d\+\)\=[ij]\=\>"

" Transpose character and delimiters: Either use just [...] or (...) aswell
syn match matlabDelimiter		"[][]"
"syn match matlabDelimiter		"[][()]"
syn match matlabTransposeOperator	"[])a-zA-Z0-9.]'"lc=1

syn match matlabSemicolon		";"

syn match matlabComment			"%.*$"	contains=matlabTodo,matlabTab

syn keyword matlabOperator		break zeros default margin round ones rand
syn keyword matlabOperator		ceil floor size clear zeros eye mean std cov

syn keyword matlabFunction		error eval function

syn keyword matlabImplicit		abs acos atan asin cos cosh exp log prod sum
syn keyword matlabImplicit		log10 max min sign sin sqrt tan reshape

syn match matlabError	"-\=\<\d\+\.\d\+\.[^*/\\^]"
syn match matlabError	"-\=\<\d\+\.\d\+[eEdD][-+]\=\d\+\.\([^*/\\^]\)"

" The default highlighting.
hi def link matlabTransposeOperator	matlabOperator
hi def link matlabOperator		Operator
hi def link matlabLineContinuation	Special
hi def link matlabLabel			Label
hi def link matlabConditional		Conditional
hi def link matlabRepeat		Repeat
hi def link matlabTodo			Todo
hi def link matlabString		String
hi def link matlabDelimiter		Identifier
hi def link matlabTransposeOther	Identifier
hi def link matlabNumber		Number
hi def link matlabFloat			Float
hi def link matlabFunction		Function
hi def link matlabError			Error
hi def link matlabImplicit		matlabStatement
hi def link matlabStatement		Statement
hi def link matlabSemicolon		SpecialChar
hi def link matlabComment		Comment

hi def link matlabArithmeticOperator	matlabOperator
hi def link matlabRelationalOperator	matlabOperator
hi def link matlabLogicalOperator	matlabOperator

"optional highlighting
"hi def link matlabIdentifier		Identifier
"hi def link matlabTab			Error

let b:current_syntax = "matlab"

"EOF	vim: ts=8 noet tw=100 sw=8 sts=0
