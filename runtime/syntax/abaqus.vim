" Vim syntax file
" Language:	Abaqus finite element input file
" Maintainer:	Carl Osterwisch <osterwischc@asme.org>
" Last Change:	8th Feb 2000

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Abaqus comment lines
syn match abaqusComment	"^\*\*.*$"

" Abaqus keyword lines
syn match abaqusKeywordLine "^\*\h.*" contains=abaqusKeyword,abaqusParameter,abaqusValue
syn match abaqusKeyword "^\*\h[^,]*" contained
syn match abaqusParameter ",[^,=]\+"lc=1 contained
syn match abaqusValue	"=\s*[^,]*"lc=1 contained

" Illegal syntax
syn match abaqusBadLine	"^\s\+\*.*"

" The default highlighting.
hi def link abaqusComment	Comment
hi def link abaqusKeyword	Statement
hi def link abaqusParameter	Identifier
hi def link abaqusValue		Constant
hi def link abaqusBadLine	Error

let b:current_syntax = "abaqus"
