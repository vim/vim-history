" Vim syntax file
" Language:  	Lex
" Maintainer:	Dr. Charles E. Campbell, Jr. <cec@gryphon.gsfc.nasa.gov>
" Last change:	January 30, 1998

" Remove any old syntax stuff hanging around
syn clear

" Read the C syntax to start with
so <sfile>:p:h/c.vim

" <c.vim> includes several ALLBUTs; these have to be treated carefully
syn clear cParen cDefine cPreProc cMulti
syn region cParen start='(' end=')' transparent contains=ALLBUT,cParenError,cIncluded,cSpecial,cTodo,cUserLabel,lexAbbrv,lexAbbrvBlock,lexAbbrvRegExp,lexInclude,lexPat,lexPatBlock,lexPatCode,lexPatRegExp,lexPatTag,lexPatComment,lexPatTagRE
syn region cDefine		start="^\s*#\s*\(define\>\|undef\>\)" skip="\\$" end="$" contains=ALLBUT,cPreCondit,cIncluded,cInclude,cDefine,cInParen,lexAbbrv,lexAbbrvBlock,lexAbbrvRegExp,lexInclude,lexPat,lexPatBlock,lexPatCode,lexPatRegExp,lexPatTag,lexPatComent,lexPatTagRE
syn region cPreProc		start="^\s*#\s*\(pragma\>\|line\>\|warning\>\|warn\>\|error\>\)" skip="\\$" end="$" contains=ALLBUT,cPreCondit,cIncluded,cInclude,cDefine,cInParen,lexAbbrv,lexAbbrvBlock,lexAbbrvRegExp,lexInclude,lexPat,lexPatBlock,lexPatCode,lexPatRegExp,lexPatTag,,lexPatComent,lexPatTagRE
syn region cMulti		transparent start='?' end=':' contains=ALLBUT,cIncluded,cSpecial,cTodo,cUserLabel,lexAbbrv,lexAbbrvBlock,lexAbbrvRegExp,lexInclude,lexPat,lexPatBlock,lexPatCode,lexPatRegExp,lexPatTag,lexPatComent,lexPatTagRE

" --- Lex stuff ---

" Abbreviations Section
syn region lexAbbrvBlock	start="^\([a-zA-Z_]\+\t\|%{\)" end="^%%$"me=e-2		skipnl				nextgroup=lexPatBlock contains=lexAbbrv,lexInclude
syn match  lexAbbrv			"^[a-zA-Z_][a-zA-Z_0-9]*\s"me=e-1					contained skipwhite	nextgroup=lexAbbrvRegExp
syn match  lexAbbrv			"^%[sx]"											contained
syn match  lexAbbrvRegExp	"\s\S.*$"lc=1										contained 			nextgroup=lexAbbrv,lexInclude
syn region lexInclude		matchgroup=lexSep	start="^%{" end="%}"			contained			contains=ALLBUT,lexAbbrv,lexAbbrvBlock,lexAbbrvRegExp,lexInclude,lexPat,lexPatBlock,lexPatCode,lexPatRegExp,cParenError,cInParen,lexPatComent,lexPatTagRE

"%% : Patterns {Actions}
syn region lexPatBlock		matchgroup=Todo	start="^%%$" matchgroup=Todo end="^%%$"	skipnl contains=lexPat,lexPatRegExp,lexPatTag,lexPatComment
syn region lexPat matchgroup=String	start=+"+ skip=+\(\\\\\)*\\"+ matchgroup=String end=+"+ contained skipwhite nextgroup=lexMorePat,lexPatCode
syn region lexPatRegExp		start=+^[^"].\{-}+me=e-1	end="\t"me=e-1				contained skipwhite nextgroup=lexPatCode
syn match  lexPatTag		"^<[a-zA-Z]\+>"											contained nextgroup=lexPat,lexMorePat,lexPatTagRE
syn region lexPatTagRE		start=+[^\t]+				end="\t"me=e-1				contained skipwhite nextgroup=lexPatCode
syn region lexPatTagRE matchgroup=String start=+"+	matchgroup=String end=+"\s+me=e-1		contained skipwhite nextgroup=lexPatCode
syn region lexPatComment	start="^\s*/\*"				end="\*/"					skipnl contains=cTodo nextgroup=lexPatComment,lexPat,lexPatTag
syn match  lexMorePat		"\s*|\s*$"												contained nextgroup=lexPat
syn region lexPatCode matchgroup=Delimiter start="{" matchgroup=Delimiter end="}" 	contained contains=ALLBUT,lexAbbrv,lexAbbrvBlock,lexAbbrvRegExp,lexInclude,lexPat,lexPatBlock,lexPatCode,lexPatRegExp,lexPatComent,lexPatTagRE

syn keyword lexFunctions	BEGIN	input	unput	woutput	yyleng	yylook	yytext
syn keyword lexFunctions	ECHO	output	winput	wunput	yyless	yymore	yywrap

" Synchronization
syn sync clear
syn sync minlines=300
syn sync match lexSyncPat	grouphere	lexAbbrvBlock	"^%%$"
syn sync match lexSyncPat	groupthere	lexPatBlock		"^%%$"

if !exists("did_lex_syntax_inits")
  let did_lex_synax_inits = 1
  hi link	lexAbbrvRegExp	lexRegExp
  hi link	lexPatRegExp	lexRegExp
  hi link	lexPatTagRE		lexRegExp

  hi link	lexAbbrv		SpecialChar
  hi link	lexFunctions	Function
  hi link	lexMorePat		Comment
  hi link	lexPat			Function
  hi link	lexPatComment	Comment
  hi link	lexPatTag		Special
  hi link	lexRegExp		Macro
  hi link	lexSep			Delimiter
endif

let b:current_syntax = "lex"

" vim:ts=4
