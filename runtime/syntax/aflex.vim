
" Vim syntax file
" Language:	AfLex (from Lex syntax file)
" Maintainer:	Mathieu Clabaut <mathieu.clabaut@free.fr>
" Last change:	03 Jul 2000
" Original:	Lex, maintained by Dr. Charles E. Campbell, Jr.
"				<Charles.Campbell@gsfc.nasa.gov>
" Comment:          Replaced sourcing c.vim file by ada.vim and rename lex*
"		in aflex*

" Remove any old syntax stuff hanging around
syn clear

" Read the Ada syntax to start with
so <sfile>:p:h/ada.vim

" --- AfLex stuff ---

"I'd prefer to use aflex.* , but it doesn't handle forward definitions yet
syn cluster aflexListGroup		contains=aflexAbbrvBlock,aflexAbbrv,aflexAbbrv,aflexAbbrvRegExp,aflexInclude,aflexPatBlock,aflexPat,aflexBrace,aflexPatString,aflexPatTag,aflexPatTag,aflexPatComment,aflexPatCodeLine,aflexMorePat,aflexPatSep,aflexSlashQuote,aflexPatCode,cInParen,cUserLabel,cOctalZero,cCppSkip,cErrInBracket,cErrInParen,cOctalError,cCppOut2
syn cluster aflexListPatCodeGroup	contains=aflexAbbrvBlock,aflexAbbrv,aflexAbbrv,aflexAbbrvRegExp,aflexInclude,aflexPatBlock,aflexPat,aflexBrace,aflexPatTag,aflexPatTag,aflexPatComment,aflexPatCodeLine,aflexMorePat,aflexPatSep,aflexSlashQuote,cInParen,cUserLabel,cOctalZero,cCppSkip,cErrInBracket,cErrInParen,cOctalError,cCppOut2

" Abbreviations Section
syn region aflexAbbrvBlock	start="^\([a-zA-Z_]\+\t\|%{\)" end="^%%$"me=e-2	skipnl	nextgroup=aflexPatBlock contains=aflexAbbrv,aflexInclude,aflexAbbrvComment
syn match  aflexAbbrv		"^\I\i*\s"me=e-1			skipwhite	contained nextgroup=aflexAbbrvRegExp
syn match  aflexAbbrv		"^%[sx]"					contained
syn match  aflexAbbrvRegExp	"\s\S.*$"lc=1				contained nextgroup=aflexAbbrv,aflexInclude
syn region aflexInclude	matchgroup=aflexSep	start="^%{" end="%}"	contained	contains=ALLBUT,@aflexListGroup
syn region aflexAbbrvComment	start="^\s\+/\*"	end="\*/"

"%% : Patterns {Actions}
syn region aflexPatBlock	matchgroup=Todo	start="^%%$" matchgroup=Todo end="^%%$"	skipnl skipwhite contains=aflexPat,aflexPatTag,aflexPatComment
syn region aflexPat		start=+\S+ skip="\\\\\|\\."	end="\s"me=e-1	contained nextgroup=aflexMorePat,aflexPatSep contains=aflexPatString,aflexSlashQuote,aflexBrace
syn region aflexBrace	start="\[" skip=+\\\\\|\\+		end="]"		contained
syn region aflexPatString	matchgroup=String start=+"+	skip=+\\\\\|\\"+	matchgroup=String end=+"+	contained
syn match  aflexPatTag	"^<\I\i*\(,\I\i*\)*>*"			contained nextgroup=aflexPat,aflexPatTag,aflexMorePat,aflexPatSep
syn match  aflexPatTag	+^<\I\i*\(,\I\i*\)*>*\(\\\\\)*\\"+		contained nextgroup=aflexPat,aflexPatTag,aflexMorePat,aflexPatSep
syn region aflexPatComment	start="^\s*/\*" end="\*/"		skipnl	contained contains=cTodo nextgroup=aflexPatComment,aflexPat,aflexPatString,aflexPatTag
syn match  aflexPatCodeLine	".*$"					contained contains=ALLBUT,@aflexListGroup
syn match  aflexMorePat	"\s*|\s*$"			skipnl	contained nextgroup=aflexPat,aflexPatTag,aflexPatComment
syn match  aflexPatSep	"\s\+"					contained nextgroup=aflexMorePat,aflexPatCode,aflexPatCodeLine
syn match  aflexSlashQuote	+\(\\\\\)*\\"+				contained
syn region aflexPatCode matchgroup=Delimiter start="{" matchgroup=Delimiter end="}"	skipnl contained contains=ALLBUT,@aflexListPatCodeGroup

syn keyword aflexCFunctions	BEGIN	input	unput	woutput	yyleng	yylook	yytext
syn keyword aflexCFunctions	ECHO	output	winput	wunput	yyless	yymore	yywrap

" <c.vim> includes several ALLBUTs; these have to be treated so as to exclude aflex* groups
syn cluster cParenGroup	add=aflex.*
syn cluster cDefineGroup	add=aflex.*
syn cluster cPreProcGroup	add=aflex.*
syn cluster cMultiGroup	add=aflex.*

" Synchronization
syn sync clear
syn sync minlines=300
syn sync match aflexSyncPat	grouphere  aflexPatBlock	"^%[a-zA-Z]"
syn sync match aflexSyncPat	groupthere aflexPatBlock	"^<$"
syn sync match aflexSyncPat	groupthere aflexPatBlock	"^%%$"

" The default highlighting.
hi def link aflexSlashQuote	aflexPat
hi def link aflexBrace	aflexPat
hi def link aflexAbbrvComment	aflexPatComment
hi def link aflexAbbrv	SpecialChar
hi def link aflexAbbrvRegExp	Macro
hi def link aflexCFunctions	Function
hi def link aflexMorePat	SpecialChar
hi def link aflexPat	Function
hi def link aflexPatComment	Comment
hi def link aflexPatString	Function
hi def link aflexPatTag	Special
hi def link aflexSep	Delimiter

let b:current_syntax = "aflex"

" vim:ts=10
