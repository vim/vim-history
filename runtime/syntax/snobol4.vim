" Vim syntax file
" Language:	SNOBOL4
" Maintainer:	Rafal Sulejman <rms@poczta.onet.pl>
" Last change:	21 Jun 2000

" Remove any old syntax stuff hanging around
syn clear
syntax case ignore
" A bunch of useful SNOBOL keywords
syn match snobol4Label	"^[^ \t]*"
syn keyword snobol4Statement	OUTPUT TERMINAL SCREEN INPUT
syn keyword snobol4Function	ARB ARBNO POS RPOS TAB TRIM SIZE
syn keyword snobol4Function	RTAB REPLACE DUPL DATATYPE CONVERT	
syn keyword snobol4Function	LEN DEFINE TRACE STOPTR CODE REM
syn keyword snobol4Function	DIFFER IDENT ARRAY TABLE
syn keyword snobol4Function	GT GE LE EQ LT NE LGT
syn keyword snobol4Function	ANY NOTANY BREAK SPAN DATE
syn keyword snobol4Function	SUBSTR OPSYN INTEGER REMDR BAL 
syn keyword snobol4Todo contained	TODO
syn match snobol4Keyword		"&TRIM\|&FULLSCAN\|&MAXLNGTH\|&ANCHOR\|&ERRLIMIT\|&ERRTEXT"
syn match snobol4Keyword		"&ALPHABET\|&LCASE\|&UCASE\|&DUMP\|&TRACE"
"integer number, or floating point number without a dot.
syn match  snobol4Number		"\<\d\+\>"
"floating point number, with dot
syn match  snobol4Number		"\<\d\+\.\d*\>"
"floating point number, starting with a dot
syn match  snobol4Number		"\.\d\+\>"

" String and Character contstants
syn region  snobol4String		  start=+"+  skip=+\\\\\|\\"+  end=+"+  
syn region  snobol4String		  start=+[^a-zA-Z0-9]'+  skip=+\\\\\|\\"+  end=+'+  

syn match   snobol4MathsOperator   "-\|=\|[:<>+\*^/\\]\||\|"
syn match  snobol4Comment     "^\*.*$"

if !exists("did_snobol4_syntax_inits")
  let did_snobol4_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link snobol4Label		Label
  hi link snobol4Conditional	Conditional
  hi link snobol4Repeat		Repeat
  hi link snobol4Number		Number
  hi link snobol4Error		Error
  hi link snobol4Statement	Statement
  hi link snobol4String		String
  hi link snobol4Comment	Comment
  hi link snobol4Special	Special
  hi link snobol4Todo		Todo
  hi link snobol4Function	Identifier
  hi link snobol4Keyword	Keyword
  hi link snobol4MathsOperator	Operator
endif

let b:current_syntax = "snobol4"
" vim: ts=8
