" Vim syntax file
" Language:	Yacc
" Maintainer:  Dr. Charles E. Campbell, Jr. <cec@gryphon.gsfc.nasa.gov>
" Last change:	November 10, 1997

" Remove any old syntax stuff hanging around
syn clear

" Read the C syntax to start with
source <sfile>:p:h/c.vim

" Yacc stuff
syn match	yaccDelim	"^[ \t]*[:|;]"
syn match	yaccOper	"@[0-9]\+"

syn match	yaccKey	"^[ \t]*%\(token\|type\|left\|right\|start\|ident\)\>"
syn match	yaccKey	"[ \t]%\(prec\|expect\|nonassoc\)\>"
syn match	yaccKey	"\$\(<[a-zA-Z_][a-zA-Z_0-9]*>\)\=[\$0-9]"
syn keyword	yaccKeyActn	yyerrok yyclearin

syn region	yaccUnion	matchgroup=yaccKey start="%union[\$ \t]*{\=" matchgroup=yaccBrace end="}"	contains=yaccKey,cComment,yaccBrace
syn match	yaccBrkt	contained "[<>]"
syn match	yaccType	"<[a-zA-Z_][a-zA-Z0-9_]*>"	contains=yaccBrkt
syn match	yaccDefinition	"^[A-Za-z][A-Za-z0-9_]*[ \t]*:"

" special Yacc separators
syn match	yaccSep	"^[ \t]*%%"
syn match	yaccSep	"^[ \t]*%{"
syn match	yaccSep	"^[ \t]*%}"

" I'd really like to highlight just the outer {}.  Any suggestions???
syn match	yaccBraceError	"[{}]"
syn region	yaccAction	matchgroup=yaccBrace start="{" end="}" contains=ALLBUT,yaccDelim,cInParen,cTodo,cIncluded,yaccDelim,yaccBraceError

if !exists("did_yacc_syntax_inits")
  " The default methods for highlighting.  Can be overridden later
  let did_yacc_syntax_inits = 1

  " Internal yacc highlighting links
  hi link yaccBrace	YaccDelim
  hi link yaccBrkt	YaccStmt
  hi link yaccKey	YaccStmt
  hi link yaccOper	YaccStmt

  " External yacc highlighting links
  hi link YaccStmt	Statement
  hi link yaccDefinition	Function
  hi link yaccDelim	Function
  hi link yaccKeyActn	Special
  hi link yaccSep	Delimiter
  hi link yaccType	Type
  hi link yaccBraceError	Error

  " since Bram doesn't like my Delimiter :|
  hi link Delimiter	Type
endif

let b:current_syntax = "yacc"

" vim: ts=15
