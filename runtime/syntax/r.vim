" Vim syntax file
" Language:	R
" Maintainer:	Tom Payne <tom@tompayne.org>
" Last change:  2001 Jan 15
" Filenames:	*.r
" URL:		http://www.tompayne.org/vim/syntax/r.vim

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn case match
set iskeyword=@,48-57,.

" Comment
syn match rComment /\#.*/

" Constant
" string enclosed in double quotes
syn region rString start=/"/ skip=/\\\\\|\\"/ end=/"/
" string enclosed in single quotes
syn region rString start=/'/ skip=/\\\\\|\\'/ end=/'/
" number with no fractional part or exponent
syn match rNumber  /\d\+/
" floating point number with integer and fractional parts and optional exponent
syn match rFloat   /\d\+\.\d*\([Ee][-+]\=\d\+\)\=/
" floating point number with no integer part and optional exponent
syn match rFloat   /\.\d\+\([Ee][-+]\=\d\+\)\=/
" floating point number with no fractional part and optional exponent
syn match rFloat   /\d\+[Ee][-+]\=\d\+/

" Identifier
" identifier with leading letter and optional following keyword characters
"syn match rIdentifier /\a\k*/
" identifier with leading period, one or more digits, and at least one non-digit keyword character
"syn match rIdentifier /\.\d*\K\k*/

" Statement
syn keyword rStatement   break next return
syn keyword rConditional if else
syn keyword rRepeat      for in repeat while

" Constant
syn keyword rConstant LETTERS letters month.ab month.name pi
syn keyword rConstant NULL 
syn keyword rBoolean  FALSE TRUE
syn keyword rNumber   NA

" Type
syn keyword rType array category character complex double function integer list logical matrix numeric vector

" Special
syn match rDelimiter /[,;:]/

" Error
syn region rRegion matchgroup=Delimiter start=/(/ matchgroup=Delimiter end=/)/ transparent contains=ALLBUT,rError,rBraceError,rCurlyError
syn region rRegion matchgroup=Delimiter start=/{/ matchgroup=Delimiter end=/}/ transparent contains=ALLBUT,rError,rBraceError,rParenError
syn region rRegion matchgroup=Delimiter start=/\[/ matchgroup=Delimiter end=/]/ transparent contains=ALLBUT,rError,rCurlyError,rParenError
syn match rError      /[)\]}]/
syn match rBraceError /[)}]/ contained
syn match rCurlyError /[)\]]/ contained
syn match rParenError /[\]}]/ contained

" The default highlighting.
hi def link rComment     Comment
hi def link rConstant    Constant
hi def link rString      String
hi def link rNumber      Number
hi def link rBoolean     Boolean
hi def link rFloat       Float
hi def link rStatement   Statement
hi def link rConditional Conditional
hi def link rRepeat      Repeat
hi def link rIdentifier  Identifier
hi def link rType        Type
hi def link rDelimiter   Delimiter
hi def link rError       Error
hi def link rBraceError  Error
hi def link rCurlyError  Error
hi def link rParenError  Error

let b:current_syntax="r"
" vim: ts=8
