" Vim syntax file
" Language:		Clean
" Author:		Pieter van Engelen <pietere@sci.kun.nl>
" Co-Author:	Arthur van Leeuwen <arthurvl@sci.kun.nl>
" Last Change:	Fri Sep 29 11:35:34 CEST 2000

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Some Clean-keywords
syn keyword cleanConditional if case
syn keyword cleanLabel let! with where in of
syn keyword cleanInclude from import
syn keyword cleanSpecial Start
syn keyword cleanKeyword infixl infixr infix
syn keyword cleanBasicType Int Real Char Bool String
syn keyword cleanSpecialType World ProcId Void Files File
syn keyword cleanModuleSystem module implementation definition system
syn keyword cleanTypeClass class instance export

" To do some Denotation Highlighting
syn keyword cleanBoolDenot True False
syn region  cleanStringDenot start=+"+ end=+"+
syn match cleanCharDenot "'.'"
syn match cleanCharsDenot "'[^'\\]*\(\\.[^'\\]\)*'" contained
syn match cleanIntegerDenot "[+-~]\=\<\(\d\+\|0[0-7]\+\|0x[0-9A-Fa-f]\+\)\>"
syn match cleanRealDenot "[+-~]\=\<\d\+\.\d+\(E[+-~]\=\d+\)\="

" To highlight the use of lists, tuples and arrays
syn region cleanList start="\[" end="\]" contains=ALL
syn region cleanRecord start="{" end="}" contains=ALL
syn region cleanArray start="{:" end=":}" contains=ALL
syn match cleanTuple "([^=]*,[^=]*)" contains=ALL

" To do some Comment Highlighting
syn region cleanComment start="/\*"  end="\*/" contains=cleanComment
syn match cleanComment "//.*"

" Now for some useful typedefinitionrecognition
syn match cleanFuncTypeDef "\([a-zA-Z].*\|(\=[-~@#$%^?!+*<>\/|&=:]\+)\=\)[ \t]*\(infix[lr]\=\)\=[ \t]*\d\=[ \t]*::.*->.*" contains=cleanSpecial

" The default highlighting.
   " Comments
hi def link cleanComment      Comment
   " Constants and denotations
hi def link cleanCharsDenot   String
hi def link cleanStringDenot  String
hi def link cleanCharDenot    Character
hi def link cleanIntegerDenot Number
hi def link cleanBoolDenot    Boolean
hi def link cleanRealDenot    Float
   " Identifiers
   " Statements
hi def link cleanTypeClass    Keyword
hi def link cleanConditional  Conditional
hi def link cleanLabel        Label
hi def link cleanKeyword      Keyword
   " Generic Preprocessing
hi def link cleanInclude      Include
hi def link cleanModuleSystem PreProc
   " Type
hi def link cleanBasicType    Type
hi def link cleanSpecialType  Type
hi def link cleanFuncTypeDef  Typedef
   " Special
hi def link cleanSpecial      Special
hi def link cleanList         Special
hi def link cleanArray        Special
hi def link cleanRecord       Special
hi def link cleanTuple        Special
   " Error
   " Todo

let b:current_syntax = "clean"

" vim: ts=4
