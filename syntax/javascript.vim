" Vim syntax file
" Language:	JavaScript
" Maintainer:	Claudio Fleiner <claudio@fleiner.com>
" URL:		http://www.fleiner.com/vim/syntax/javascript.vim
" Last change:	1998 Jan 12


" Remove any old syntax stuff hanging around
syn clear
syn case ignore

syn match   javaScriptLineComment      "\/\/.*$"
syn match   javaScriptCommentSkip      "^[ \t]*\*\($\|[ \t]\+\)"
syn region  javaScriptCommentString    start=+"+  skip=+\\\\\|\\"+  end=+"+ end=+\*/+me=s-1,he=s-1 contains=javaScriptSpecial,javaScriptCommentSkip
syn region  javaScriptComment2String   start=+"+  skip=+\\\\\|\\"+  end=+$\|"+  contains=javaScriptSpecial
syn region  javaScriptComment          start="/\*"  end="\*/" contains=javaScriptCommentString,javaScriptCharacter,javaScriptNumber
syn match   javaScriptSpecial          "\\[0-9][0-9][0-9]\|\\."
syn region  javaScriptStringD          start=+"+  skip=+\\\\\|\\"+  end=+"+  contains=javaScriptSpecial
syn region  javaScriptStringS          start=+'+  skip=+\\\\\|\\'+  end=+'+  contains=javaScriptSpecial
syn match   javaScriptSpecialCharacter "'\\.'"
syn match   javaScriptNumber           "-\=\<[0-9]\+L\=\>\|0[xX][0-9a-fA-F]\+\>"
syn keyword javaScriptConditionall     if else
syn keyword javaScriptRepeat           while for
syn keyword javaScriptBranch           break continue
syn keyword javaScriptOperator         new in
syn keyword javaScriptType             this var
syn keyword javaScriptStatement        return with
syn keyword javaScriptFunction         function
syn keyword javaScriptBoolean          true false
syn match   javaScriptBraces           "[{}]"

" catch errors caused by wrong parenthesis
syn region  javaScriptParen       transparent start="(" end=")" contains=ALLBUT,javaScriptParenError
syn match   javaScriptParenError  ")"
syn match   javaScriptInParen     contained "[{}]"

syn sync ccomment javaScriptComment


if !exists("did_javascript_syntax_inits")
  let did_javascript_syntax_inits = 1
  hi link javaScriptComment           Comment
  hi link javaScriptLineComment       Comment
  hi link javaScriptSpecial           Special
  hi link javaScriptStringS           String
  hi link javaScriptStringD           String
  hi link javaScriptCharacter         Character
  hi link javaScriptSpecialCharacter  javaScriptSpecial
  hi link javaScriptNumber            javaScriptValue
  hi link javaScriptConditional       Conditional
  hi link javaScriptRepeat            Repeat
  hi link javaScriptBranch            Conditional
  hi link javaScriptOperator          Operator
  hi link javaScriptType              Type
  hi link javaScriptStatement         Statement
  hi link javaScriptFunction          Function
  hi link javaScriptBraces            Function
  hi link javaScriptError             Error
  hi link javaScriptParenError        javaScriptError
  hi link javaScriptInParen           javaScriptError
  hi link javaScriptBoolean           Boolean
endif

let b:current_syntax = "javascript"

" vim: ts=8
