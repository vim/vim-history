" Vim syntax file
" Language:	strace output
" Maintainer:	David Ne\v{c}as (Yeti) <yeti@physics.muni.cz>
" Last Change:	2000 Nov 04

" Remove any old syntax stuff hanging around
syn clear

syn case match
" everything is oneline
syn match   straceSpecialChar	"\\\d\d\d\|\\." contained
syn region  straceString	start=+"+  skip=+\\\\\|\\"+  end=+"+  contains=straceSpecialChar oneline
syn match   straceNumber	"\W[+-]\=\(\d\+\)\=\.\=\d\+\([eE][+-]\=\d\+\)\="lc=1
syn match   straceNumber	"\W0x\x\+"lc=1
syn match   straceNumberRHS	"\W\(0x\x\+\|-\=\d\+\)"lc=1 contained
syn match   straceOtherRHS	"?" contained
syn match   straceConstant      "[A-Z_]\{2,}"
syn region  straceVerbosed      start="(" end=")" matchgroup=Normal contained oneline
syn region  straceReturned      start="\s=\s" end="$" contains=StraceEquals,straceNumberRHS,straceOtherRHS,straceConstant,straceVerbosed oneline transparent
syn match   straceEquals        "\s=\s"ms=s+1,me=e-1
syn match   straceSysCall       "^\w\+"
syn match   straceParenthesis   "[][(){}]"
syn match   straceOperator      "[-+=*/!%&|:,]"
syn region  straceComment       start="/\*" end="\*/" oneline

" the default methods for highlighting.  can be overridden later
hi def link straceComment       Comment
hi def link straceVerbosed	Comment
hi def link straceNumber	Number
hi def link straceNumberRHS     Type
hi def link straceOtherRHS      Type
hi def link straceString	String
hi def link straceConstant	Function
hi def link straceEquals	Type
hi def link straceSysCall	Statement
hi def link straceParenthesis   Statement
hi def link straceOperator      Normal
hi def link straceSpecialChar	Special

let b:current_syntax = "strace"

" vim: ts=8
