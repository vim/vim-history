" Vim syntax file
" Language:	SQL (Oracle 7)
" Maintainer:	Paul Moore <gustav@morpheus.demon.co.uk>
" Last change:	1997 April 20

" Remove any old syntax stuff hanging around
syn clear

syn case ignore

" The SQL reserved words, defined as keywords.

syn keyword sqlSpecial   null

syn keyword sqlKeyword	access add as asc by check cluster column
syn keyword sqlKeyword	compress connect current decimal default
syn keyword sqlKeyword	desc else exclusive file for from group
syn keyword sqlKeyword	having identified immediate increment index
syn keyword sqlKeyword	initial into is level maxextents mode modify
syn keyword sqlKeyword	nocompress nowait of offline on online start
syn keyword sqlKeyword	successful synonym table then to trigger uid
syn keyword sqlKeyword	unique user validate values view whenever
syn keyword sqlKeyword	where with option order pctfree privileges
syn keyword sqlKeyword	public resource row rowlabel rownum rows
syn keyword sqlKeyword	session share size smallint

syn keyword sqlOperator	not and or
syn keyword sqlOperator	in any some all between exists
syn keyword sqlOperator	like escape
syn keyword sqlOperator  union intersect minus
syn keyword sqlOperator  prior distinct
syn keyword sqlOperator	sysdate

syn keyword sqlStatement	alter analyze audit comment commit create
syn keyword sqlStatement	delete drop explain grant insert lock noaudit
syn keyword sqlStatement	rename revoke rollback savepoint select set
syn keyword sqlStatement truncate update

syn keyword sqlType		char character date long raw mlslabel number
syn keyword sqlType		rowid varchar varchar2 float integer

" Strings and characters:
syn region sqlString		start=+"+  skip=+\\\\\|\\"+  end=+"+
syn region sqlString		start=+'+  skip=+\\\\\|\\"+  end=+'+

" Numbers:
syn match sqlNumber		"-\=\<[0-9]*\.\=[0-9_]\>"

" Comments:
syn region sqlComment    start="/\*"  end="\*/"
syn match sqlComment 	"--.*"

syn sync ccomment sqlComment

if !exists("did_sql_syntax_inits")
  let did_sql_syntax_inits = 1
  " The default methods for highlighting. Can be overridden later.
  hi link sqlComment	Comment
  hi link sqlKeyword	sqlSpecial
  hi link sqlNumber	Number
  hi link sqlOperator	sqlStatement
  hi link sqlSpecial	Special
  hi link sqlStatement	Statement
  hi link sqlString	String
  hi link sqlType	Type
endif

let b:current_syntax = "sql"

" vim: ts=8
