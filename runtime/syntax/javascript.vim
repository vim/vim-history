" Vim syntax file
" Language:	JavaScript
" Maintainer:	Claudio Fleiner <claudio@fleiner.com>
" URL:		http://www.fleiner.com/vim/syntax/javascript.vim
" Last Change:	2000 Nov 04


" Remove any old syntax stuff hanging around
syn clear
syn case ignore

if !exists("main_syntax")
  let main_syntax = 'javascript'
endif

let b:current_syntax = "javascript"

syn match   javaScriptLineComment      "\/\/.*$"
syn match   javaScriptCommentSkip      "^[ \t]*\*\($\|[ \t]\+\)"
syn region  javaScriptCommentString    start=+"+  skip=+\\\\\|\\"+  end=+"+ end=+\*/+me=s-1,he=s-1 contains=javaScriptSpecial,javaScriptCommentSkip,@htmlPreproc
syn region  javaScriptComment2String   start=+"+  skip=+\\\\\|\\"+  end=+$\|"+  contains=javaScriptSpecial,@htmlPreproc
syn region  javaScriptComment          start="/\*"  end="\*/" contains=javaScriptCommentString,javaScriptCharacter,javaScriptNumber
syn match   javaScriptSpecial          "\\\d\d\d\|\\."
syn region  javaScriptStringD          start=+"+  skip=+\\\\\|\\"+  end=+"+  contains=javaScriptSpecial,@htmlPreproc
syn region  javaScriptStringS          start=+'+  skip=+\\\\\|\\'+  end=+'+  contains=javaScriptSpecial,@htmlPreproc
syn match   javaScriptSpecialCharacter "'\\.'"
syn match   javaScriptNumber           "-\=\<\d\+L\=\>\|0[xX][0-9a-fA-F]\+\>"
syn keyword javaScriptConditional      if else
syn keyword javaScriptRepeat           while for
syn keyword javaScriptBranch           break continue
syn keyword javaScriptOperator         new in
syn keyword javaScriptType             this var
syn keyword javaScriptStatement        return with
syn keyword javaScriptFunction         function
syn keyword javaScriptBoolean          true false
syn match   javaScriptBraces           "[{}]"

" catch errors caused by wrong parenthesis
syn match   javaScriptInParen     contained "[{}]"
syn region  javaScriptParen       transparent start="(" end=")" contains=javaScriptParen,javaScript.*
syn match   javaScrParenError  ")"

if main_syntax == "javascript"
  syn sync ccomment javaScriptComment
endif

hi def link javaScriptComment           Comment
hi def link javaScriptLineComment       Comment
hi def link javaScriptSpecial           Special
hi def link javaScriptStringS           String
hi def link javaScriptStringD           String
hi def link javaScriptCharacter         Character
hi def link javaScriptSpecialCharacter  javaScriptSpecial
hi def link javaScriptNumber            javaScriptValue
hi def link javaScriptConditional       Conditional
hi def link javaScriptRepeat            Repeat
hi def link javaScriptBranch            Conditional
hi def link javaScriptOperator          Operator
hi def link javaScriptType              Type
hi def link javaScriptStatement         Statement
hi def link javaScriptFunction          Function
hi def link javaScriptBraces            Function
hi def link javaScriptError             Error
hi def link javaScrParenError           javaScriptError
hi def link javaScriptInParen           javaScriptError
hi def link javaScriptBoolean           Boolean

let b:current_syntax = "javascript"
if main_syntax == 'javascript'
  unlet main_syntax
endif

" vim: ts=8
