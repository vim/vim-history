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

" The default highlighting.
hi def link snobol4Label	Label
hi def link snobol4Conditional	Conditional
hi def link snobol4Repeat	Repeat
hi def link snobol4Number	Number
hi def link snobol4Error	Error
hi def link snobol4Statement	Statement
hi def link snobol4String	String
hi def link snobol4Comment	Comment
hi def link snobol4Special	Special
hi def link snobol4Todo		Todo
hi def link snobol4Function	Identifier
hi def link snobol4Keyword	Keyword
hi def link snobol4MathsOperator Operator

let b:current_syntax = "snobol4"
" vim: ts=8
