" Vim syntax file
" Language:	Procmail definition file
" Maintainer:	vacancy [posted by Sonia Heimann, but she didn't feel like
"		maintaining this]
" Last change:	1997 Aug 18

" Remove any old syntax stuff hanging around
syn clear

syn match   procmailComment      "#.*$" contains=shTodo
syn keyword   procmailTodo      contained Todo TBD

syn region  procmailString       start=+"+  skip=+\\"+  end=+"+

syn region procmailVarDeclRegion start="^[ \t]*[a-zA-Z0-9_]\+[ \t]*="hs=e-1 skip=+\\$+ end=+$+ contains=procmailVar,procmailVarDecl,procmailString
syn match procmailVarDecl contained "^[ \t]*[a-zA-Z0-9_]\+"
syn match procmailVar "$[a-zA-Z0-9_]\+"

syn match procmailCondition contained "^[ \t]*\*.*"

syn match procmailActionFolder contained "^[ \t]*[-_a-zA-Z/]\+"
syn match procmailActionVariable contained "^[ \t]*$[a-zA-Z_]\+"
syn region procmailActionForward start=+^[ \t]*!+ skip=+\\$+ end=+$+
syn region procmailActionPipe start=+^[ \t]*|+ skip=+\\$+ end=+$+
syn region procmailActionNested start=+^[ \t]*{+ end=+^[ \t]*}+ contains=procmailRecipe,procmailComment,procmailVarDeclRegion

syn region procmailRecipe start=+^[ \t]*:.*$+ end=+^$+ contains=procmailComment,procmailCondition,procmailActionFolder,procmailActionVariable,procmailActionForward,procmailActionPipe,procmailActionNested,procmailVarDeclRegion

if !exists("did_procmail_syntax_inits")
  "let did_procmail_syntax_inits = 1
  hi link procmailComment Comment
  hi link procmailTodo    Todo

  hi link procmailRecipe   Statement
  "highlight link procmailCondition   Statement

  hi link procmailActionFolder procmailAction
  hi link procmailActionVariable procmailAction
  hi link procmailActionForward procmailAction
  hi link procmailActionPipe procmailAction
  hi link procmailAction 	Function
  hi link procmailVar 		Identifier
  hi link procmailVarDecl 	Identifier

  hi link procmailString String
endif

let b:current_syntax = "procmail"

" vim: ts=8
