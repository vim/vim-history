" Vim syntax file
" Language:  	Lex
" Maintainer:	Dr. Charles E. Campbell, Jr. <cec@gryphon.gsfc.nasa.gov>
" Last change:	March 29, 1998

" Remove any old syntax stuff hanging around
syn clear

" Read the C syntax to start with
so <sfile>:p:h/c.vim

" --- Lex stuff ---

"I'd prefer to use lex.* , but it doesn't handle forward definitions yet
let b:lexlist="lexAbbrvBlock,lexAbbrv,lexAbbrv,lexAbbrvRegExp,lexInclude,lexPatBlock,lexPat,lexBrace,lexPatString,lexPatTag,lexPatTag,lexPatComment,lexPatCodeLine,lexMorePat,lexPatSep,lexSlashQuote,lexPatCode,cInParen"
let b:lexlistpc="lexAbbrvBlock,lexAbbrv,lexAbbrv,lexAbbrvRegExp,lexInclude,lexPatBlock,lexPat,lexBrace,lexPatTag,lexPatTag,lexPatComment,lexPatCodeLine,lexMorePat,lexPatSep,lexSlashQuote,cInParen"

" Abbreviations Section
syn region lexAbbrvBlock	start="^\([a-zA-Z_]\+\t\|%{\)" end="^%%$"me=e-2	skipnl	nextgroup=lexPatBlock contains=lexAbbrv,lexInclude
syn match  lexAbbrv		"^\I\i*\s"me=e-1			skipwhite	contained nextgroup=lexAbbrvRegExp
syn match  lexAbbrv		"^%[sx]"					contained
syn match  lexAbbrvRegExp	"\s\S.*$"lc=1				contained nextgroup=lexAbbrv,lexInclude
exe 'syn region lexInclude	matchgroup=lexSep	start="^%{" end="%}"	contained	contains=ALLBUT,' . b:lexlist

"%% : Patterns {Actions}
syn region lexPatBlock	matchgroup=Todo	start="^%%$" matchgroup=Todo end="^%%$"	skipnl skipwhite contains=lexPat,lexPatTag,lexPatComment
syn region lexPat		start=+\S+ skip="\(\\\\\)*\\." end="\s"me=e-1 contained nextgroup=lexMorePat,lexPatSep contains=lexPatString,lexSlashQuote,lexBrace
syn region lexBrace	start="\[" skip=+\(\\\\\)*\\+ end="]"			contained
syn region lexPatString	matchgroup=String start=+"+ skip=+\(\\\\\)*\\"+ matchgroup=String end=+"+	contained
syn match  lexPatTag	"^<\I\i*\(,\I\i*\)*>*"			contained nextgroup=lexPat,lexPatTag,lexMorePat,lexPatSep
syn match  lexPatTag	+^<\I\i*\(,\I\i*\)*>*\(\\\\\)*\\"+		contained nextgroup=lexPat,lexPatTag,lexMorePat,lexPatSep
syn region lexPatComment	start="^\s*/\*" end="\*/"		skipnl	contained contains=cTodo nextgroup=lexPatComment,lexPat,lexPatString,lexPatTag
exe 'syn match  lexPatCodeLine	".*$"				oneline	contained contains=ALLBUT,' . b:lexlist
syn match  lexMorePat	"\s*|\s*$"			skipnl	contained nextgroup=lexPat,lexPatString,lexPatTag,lexPatComment
syn match  lexPatSep	"\s\+"					contained nextgroup=lexMorePat,lexPatCode,lexPatCodeLine
syn match  lexSlashQuote	+\(\\\\\)*\\"+				contained
exe 'syn region lexPatCode matchgroup=Delimiter start="{" matchgroup=Delimiter end="}" 	skipnl contained contains=ALLBUT,' . b:lexlistpc

syn keyword ClexFunctions	BEGIN	input	unput	woutput	yyleng	yylook	yytext
syn keyword ClexFunctions	ECHO	output	winput	wunput	yyless	yymore	yywrap

" <c.vim> includes several ALLBUTs; these have to be treated so as to exclude lex* groups
syn clear cParen cDefine cPreProc cMulti
syn region cParen	start='(' end=')'			transparent	contains=ALLBUT,cParenError,cIncluded,cSpecial,cTodo,cUserCont,cUserLabel,lex.*
syn region cDefine	start="^\s*#\s*\(define\>\|undef\>\)" skip="\\$" end="$"	contains=ALLBUT,cPreCondit,cIncluded,cInclude,cDefine,cInParen,lex.*
syn region cPreProc	start="^\s*#\s*\(pragma\>\|line\>\|warning\>\|warn\>\|error\>\)" skip="\\$" end="$" contains=ALLBUT,cPreCondit,cIncluded,cInclude,cDefine,cInParen,lex.*
syn region cMulti	start='?' end=':'			transparent 	contains=ALLBUT,cIncluded,cSpecial,cTodo,cUserCont,cUserLabel,lex.*

" Synchronization
syn sync clear
syn sync minlines=300
syn sync match lexSyncPat	grouphere  lexPatBlock	"^%[a-zA-Z]"
syn sync match lexSyncPat	groupthere lexPatBlock	"^<$"
syn sync match lexSyncPat	groupthere lexPatBlock	"^%%$"

if !exists("did_lex_syntax_inits")
  let did_lex_synax_inits = 1
  hi link	lexSlashQuote	lexPat
  hi link	lexBrace		lexPat

  hi link	ClexFunctions	Function
  hi link	lexAbbrv		SpecialChar
  hi link	lexAbbrvRegExp	Macro
  hi link	lexMorePat	SpecialChar
  hi link	lexPat		Function
  hi link	lexPatComment	Comment
  hi link	lexPatString	Function
  hi link	lexPatTag		Special
  hi link	lexSep		Delimiter
endif

let b:current_syntax = "lex"

" vim:ts=10
