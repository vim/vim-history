" Vim syntax file
" Language:	Yacc
" Maintainer:	Dr. Charles E. Campbell, Jr. <Charles.E.Campbell.1@gsfc.nasa.gov>
" Last Change:	March 1, 2001
" Option:
"   yacc_uses_cpp : if this variable exists, then C++ is loaded rather than C

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Read the C syntax to start with
if version >= 600
  if exists("yacc_uses_cpp")
    runtime! syntax/cpp.vim
  else
    runtime! syntax/c.vim
  endif
elseif exists("yacc_uses_cpp")
  so <sfile>:p:h/cpp.vim
else
  so <sfile>:p:h/c.vim
endif

" Clusters
syn cluster	yaccActionGroup	contains=yaccDelim,cInParen,cTodo,cIncluded,yaccDelim,yaccCurlyError,yaccUnionCurly,yaccUnion,cUserLabel,cOctalZero,cCppOut2,cCppSkip,cErrInBracket,cErrInParen,cOctalError,cCommentStartError,cParenError
syn cluster	yaccUnionGroup	contains=yaccKey,cComment,yaccCurly,cType,cStructure,cStorageClass,yaccUnionCurly

" Yacc stuff
syn match	yaccDelim	"^\s*[:|;]"
syn match	yaccOper	"@\d\+"

syn match	yaccKey	"^\s*%\(token\|type\|left\|right\|start\|ident\|nonassoc\)\>"
syn match	yaccKey	"\s%\(prec\|expect\)\>"
syn match	yaccKey	"\$\(<[a-zA-Z_][a-zA-Z_0-9]*>\)\=[\$0-9]\+"
syn keyword	yaccKeyActn	yyerrok yyclearin

syn match	yaccUnionStart	"^%union"	skipwhite skipnl nextgroup=yaccUnion
syn region	yaccUnion	contained matchgroup=yaccCurly start="{" matchgroup=yaccCurly end="}"	contains=@yaccUnionGroup
syn region	yaccUnionCurly	contained matchgroup=yaccCurly start="{" matchgroup=yaccCurly end="}" contains=@yaccUnionGroup
syn match	yaccBrkt	contained "[<>]"
syn match	yaccType	"<[a-zA-Z_][a-zA-Z0-9_]*>"	contains=yaccBrkt
syn match	yaccDefinition	"^[A-Za-z][A-Za-z0-9_]*[ \t]*:"

" special Yacc separators
syn match	yaccSectionSep	"^[ \t]*%%"
syn match	yaccSep	"^[ \t]*%{"
syn match	yaccSep	"^[ \t]*%}"

" I'd really like to highlight just the outer {}.  Any suggestions???
syn match	yaccCurlyError	"[{}]"
syn region	yaccAction	matchgroup=yaccCurly start="{" end="}" contains=ALLBUT,@yaccActionGroup

" The default highlighting.

  " Internal yacc highlighting links
hi def link yaccBrkt	yaccStmt
hi def link yaccKey	yaccStmt
hi def link yaccOper	yaccStmt
hi def link yaccUnionStart	yaccKey

  " External yacc highlighting links
hi def link yaccCurly	Delimiter
hi def link yaccCurlyError	Error
hi def link yaccDefinition	Function
hi def link yaccDelim	Function
hi def link yaccKeyActn	Special
hi def link yaccSectionSep	Todo
hi def link yaccSep	Delimiter
hi def link yaccStmt	Statement
hi def link yaccType	Type

  " since Bram doesn't like my Delimiter :|
hi def link Delimiter	Type

let b:current_syntax = "yacc"

" vim: ts=15
