" Vim syntax file
" Language:	AYacc
" Maintainer:	Mathieu Clabaut <mathieu.clabaut@free.fr>
" Last change:	03 Jul 2000
" Original:	Yacc, maintained by Dr. Charles E. Campbell, Jr.
"			<Charles.Campbell@gsfc.nasa.gov>
" Comment:          Replaced sourcing c.vim file by ada.vim and rename yacc*
"		in ayacc*

" Remove any old syntax stuff hanging around
syn clear

" Read the Ada syntax to start with
source <sfile>:p:h/ada.vim

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

if !exists("did_ayacc_syntax_inits")
  " The default methods for highlighting.  Can be overridden later
  let did_ayacc_syntax_inits = 1

  " Internal ayacc highlighting links
  hi link ayaccBrkt	ayaccStmt
  hi link ayaccKey	ayaccStmt
  hi link ayaccOper	ayaccStmt
  hi link ayaccUnionStart	ayaccKey

  " External ayacc highlighting links
  hi link ayaccCurly	Delimiter
  hi link ayaccCurlyError	Error
  hi link ayaccDefinition	Function
  hi link ayaccDelim	Function
  hi link ayaccKeyActn	Special
  hi link ayaccSectionSep	Todo
  hi link ayaccSep	Delimiter
  hi link ayaccStmt	Statement
  hi link ayaccType	Type

  " since Bram doesn't like my Delimiter :|
  hi link Delimiter	Type
endif

let b:current_syntax = "ayacc"

" vim: ts=15
