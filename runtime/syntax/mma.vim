" Vim syntax file
" Language:	Mathematica
" Maintainer:	Wolfgang Waltenberger <wwalten@ben.tuwien.ac.at>
" Last Change:	Mon Sep 11 14:39:30 2000

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn match mmaError "\*)"
syn match mmaFixme "FIXME"
syn region mmaComment        start=+(\*+ end=+\*)+ skipempty contains=mmaFixme
syn match mmaSpecialFunctions   "\a*::\a*"
syn region mmaString                start=+'+    end=+'+
syn region mmaString                start=+"+    end=+"+ 
syn region mmaString                start=+\\\"+ end=+\"+
syn region mmaString                start=+\"+   end=+\"+

syn match mmaVariable                "$\a*"

syn match mmaPattern "[A-Za-z01-9`]*_\{1,3}"
syn match mmaPattern "[A-Za-z01-9`]*_\{1,3}\(Integer\|Real\|Pattern\|Symbol\)"
syn match mmaPattern "[A-Za-z01-9`]*_\{1,3}\(Rational\|Complex\|Head\)"
syn match mmaPattern "[A-Za-z01-9`]*_\{1,3}?[A-Za-z01-9`]*"


" prefix/infix/postfix notations
syn match mmaGenericFunction "[A-Za-z01-9`]*\s*\(\[\|@\)"he=e-1
syn match mmaGenericFunction "[A-Za-z01-9`]*\s*\(/@\|@@\)"he=e-2
syn match mmaGenericFunction "\~\s*[A-Za-z01-9`]*\s*\~"hs=s+1,he=e-1 
syn match mmaGenericFunction "//\s*[A-Za-z01-9`]*"hs=s+2

syn match mmaOperator                "/;"

syn match mmaPureFunction            "#\d*"
syn match mmaPureFunction            "&"

syn match mmaUnicode  "\\\[[a-zA-Z01-9]*\]"

" The default highlighting.
hi def link mmaOperator			Operator
hi def link mmaVariable			Identifier
hi def link mmaString			String
hi def link mmaUnicode			String
hi def link mmaSpecialFunctions		Identifier
hi def link mmaPattern			Identifier
hi def link mmaGenericFunction		Function
hi def link mmaError			Error
hi def link mmaFixme			Error
hi def link mmaComment			Comment
hi def link mmaPureFunction		Operator

let b:current_syntax = "mma"
