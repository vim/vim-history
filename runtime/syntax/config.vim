" Vim syntax file
" Language:		configure.in script: M4 with sh
" Maintainer:	Christian Hammesr <ch@lathspell.westend.com>
" Last Change:	2001 Jan 15

" Well, I actually even do not know much about m4. This explains why there
" is probably very much missing here, yet !
" But I missed a good hilighting when editing my GNU autoconf/automake
" script, so I wrote this quick and dirty patch.


" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" define the config syntax
syn match   configdelimiter "[()\[\];,]"
syn match   configoperator  "[=|&\*\+\<\>]"
syn match   configcomment   "\(dnl.*\)\|\(#.*\)"
syn match   configfunction  "\<[A-Z_][A-Z0-9_]*\>"
syn match   confignumber    "[-+]\=\<\d\+\(\.\d*\)\=\>"
syn keyword configkeyword   if then else fi test for in do done
syn keyword configspecial   cat rm eval
syn region  configstring    start=+"+ skip=+\\"+ end=+"+
syn region  configstring    start=+`+ skip=+\\'+ end=+'+
syn region  configstring    start=+`+ skip=+\\'+ end=+`+

" The default highlighting.
hi def link configdelimiter Delimiter
hi def link configoperator  Operator
hi def link configcomment   Comment
hi def link configfunction  Function
hi def link confignumber    Number
hi def link configkeyword   Keyword
hi def link configspecial   Special
hi def link configstring    String

let b:current_syntax = "config"

" vim: ts=4
