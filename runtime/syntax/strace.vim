" Vim syntax file
" Language:	strace output
" Maintainer:	David Ne\v{c}as (Yeti) <yeti@physics.muni.cz>
" Last Change:	2000 Sep 01

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

if !exists("did_strace_syntax_inits")
  let did_strace_syntax_inits = 1
  " the default methods for highlighting.  can be overridden later
  hi link straceComment         Comment
  hi link straceVerbosed	Comment
  hi link straceNumber		Number
  hi link straceNumberRHS       Type
  hi link straceOtherRHS       	Type
  hi link straceString		String
  hi link straceConstant	Function
  hi link straceEquals  	Type
  hi link straceSysCall  	Statement
  hi link straceParenthesis     Statement
  hi link straceOperator        Normal
  hi link straceSpecialChar	Special
endif

let b:current_syntax = "strace"

" vim: ts=8
