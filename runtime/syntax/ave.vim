" Vim syntax file
" Language:	avenue
" Maintainer:	Jan-Oliver Wagner <Jan-Oliver.Wagner@usf.Uni-Osnabrueck.DE>
" Last Change:	2001 Jan 15

" Avenue is the ArcView built-in language. ArcView is
" a desktop GIS by ESRI. Though it is a built-in language
" and a built-in editor is provided, the use of VIM increases
" development speed.
" I use some technologies to automatically load avenue scripts
" into ArcView.

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Avenue is entirely case-insensitive.
syn case ignore

" The keywords

syn keyword aveStatement	if then elseif else end break exit return
syn keyword aveStatement	for each in continue while

" String

syn region aveString		start=+"+ end=+"+

" Integer number
syn match  aveNumber		"[+-]\=\<[0-9]\+\>"

" Operator

syn keyword aveOperator		or and max min xor mod by

" Variables

syn keyword aveFixVariables	av nil self false true nl tab cr tab
syn match globalVariables	"_[a-zA-Z][a-zA-Z0-9]*"
syn match aveVariables		"[a-zA-Z][a-zA-Z0-9_]*"
syn match aveConst		"#[A-Z][A-Z_]+"

" Procedures (class methods) - not working, why?

syn match avClassMethods	"\.[a-zA-Z][a-zA-Z]+"

" Comments

syn match aveComment	"'.*"

" Typical Typos

" for C programmers:
syn match aveTypos	"=="
syn match aveTypos	"!="

" The default highlighting.
hi def link aveStatement	Statement

hi def link aveString		String
hi def link aveNumber		Number

hi def link aveFixVariables	Special
hi def link aveVariables	Identifier
hi def link globalVariables	Special
hi def link aveConst		Special

hi def link aveClassMethods	Function

hi def link aveOperators	Operator
hi def link aveComment		Comment

hi def link aveTypos		Error

let b:current_syntax = "ave"

" vim: ts=8
