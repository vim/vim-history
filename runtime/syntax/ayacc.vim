" Vim syntax file
" Language:	AYacc
" Maintainer:	Mathieu Clabaut <mathieu.clabaut@free.fr>
" Last change:	03 Jul 2000
" Original:	Yacc, maintained by Dr. Charles E. Campbell, Jr.
"			<Charles.Campbell@gsfc.nasa.gov>
" Comment:          Replaced sourcing c.vim file by ada.vim and rename yacc*
"		in ayacc*

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

" Read the Ada syntax to start with
runtime! syntax/ada.vim
unlet b:current_syntax

" Clusters
syn cluster	ayaccActionGroup	contains=ayaccDelim,cInParen,cTodo,cIncluded,ayaccDelim,ayaccCurlyError,ayaccUnionCurly,ayaccUnion,cUserLabel,cOctalZero,cCppOut2,cCppSkip,cErrInBracket,cErrInParen,cOctalError
syn cluster	ayaccUnionGroup	contains=ayaccKey,cComment,ayaccCurly,cType,cStructure,cStorageClass,ayaccUnionCurly

" Yacc stuff
syn match	ayaccDelim	"^[ \t]*[:|;]"
syn match	ayaccOper	"@\d\+"

syn match	ayaccKey	"^[ \t]*%\(token\|type\|left\|right\|start\|ident\)\>"
syn match	ayaccKey	"[ \t]%\(prec\|expect\|nonassoc\)\>"
syn match	ayaccKey	"\$\(<[a-zA-Z_][a-zA-Z_0-9]*>\)\=[\$0-9]\+"
syn keyword	ayaccKeyActn	yyerrok yyclearin

syn match	ayaccUnionStart	"^%union"	skipwhite skipnl nextgroup=ayaccUnion
syn region	ayaccUnion	contained matchgroup=ayaccCurly start="{" matchgroup=ayaccCurly end="}"	contains=@ayaccUnionGroup
syn region	ayaccUnionCurly	contained matchgroup=ayaccCurly start="{" matchgroup=ayaccCurly end="}" contains=@ayaccUnionGroup
syn match	ayaccBrkt	contained "[<>]"
syn match	ayaccType	"<[a-zA-Z_][a-zA-Z0-9_]*>"	contains=ayaccBrkt
syn match	ayaccDefinition	"^[A-Za-z][A-Za-z0-9_]*[ \t]*:"

" special Yacc separators
syn match	ayaccSectionSep	"^[ \t]*%%"
syn match	ayaccSep	"^[ \t]*%{"
syn match	ayaccSep	"^[ \t]*%}"

" I'd really like to highlight just the outer {}.  Any suggestions???
syn match	ayaccCurlyError	"[{}]"
syn region	ayaccAction	matchgroup=ayaccCurly start="{" end="}" contains=ALLBUT,@ayaccActionGroup

" The default highlighting.

" Internal ayacc highlighting links
hi def link ayaccBrkt	ayaccStmt
hi def link ayaccKey	ayaccStmt
hi def link ayaccOper	ayaccStmt
hi def link ayaccUnionStart	ayaccKey

" External ayacc highlighting links
hi def link ayaccCurly	Delimiter
hi def link ayaccCurlyError	Error
hi def link ayaccDefinition	Function
hi def link ayaccDelim	Function
hi def link ayaccKeyActn	Special
hi def link ayaccSectionSep	Todo
hi def link ayaccSep	Delimiter
hi def link ayaccStmt	Statement
hi def link ayaccType	Type

" since Bram doesn't like my Delimiter :|
hi def link Delimiter	Type

let b:current_syntax = "ayacc"

" vim: ts=15
