" Vim syntax file
" Language:	CA-OpenROAD
" Maintainer:	Luis Moreno <lmoreno@retemail.es>
" Last Change:	2001 Jan 15

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syntax keyword openroadKeyword	ABORT ALL ALTER AND ANY AS ASC AT AVG BEGIN
syntax keyword openroadKeyword	BETWEEN BY BYREF CALL CALLFRAME CALLPROC CASE
syntax keyword openroadKeyword	CLEAR CLOSE COMMIT CONNECT CONTINUE COPY COUNT
syntax keyword openroadKeyword	CREATE CURRENT DBEVENT DECLARE DEFAULT DELETE
syntax keyword openroadKeyword	DELETEROW DESC DIRECT DISCONNECT DISTINCT DO
syntax keyword openroadKeyword	DROP ELSE ELSEIF END ENDCASE ENDDECLARE ENDFOR
syntax keyword openroadKeyword	ENDIF ENDLOOP ENDWHILE ESCAPE EXECUTE EXISTS
syntax keyword openroadKeyword	EXIT FETCH FIELD FOR FROM GOTOFRAME GRANT GROUP
syntax keyword openroadKeyword	HAVING IF IMMEDIATE IN INDEX INITIALISE
syntax keyword openroadKeyword	INITIALIZE INQUIRE_INGRES INQUIRE_SQL INSERT
syntax keyword openroadKeyword	INSERTROW INSTALLATION INTEGRITY INTO KEY LIKE
syntax keyword openroadKeyword	LINK MAX MESSAGE METHOD MIN MODE MODIFY NEXT
syntax keyword openroadKeyword	NOECHO NOT NULL OF ON OPEN OPENFRAME OR ORDER
syntax keyword openroadKeyword	PERMIT PROCEDURE PROMPT QUALIFICATION RAISE
syntax keyword openroadKeyword	REGISTER RELOCATE REMOVE REPEAT REPEATED RESUME
syntax keyword openroadKeyword	RETURN RETURNING REVOKE ROLE ROLLBACK RULE SAVE
syntax keyword openroadKeyword	SAVEPOINT SELECT SET SLEEP SOME SUM SYSTEM TABLE
syntax keyword openroadKeyword	THEN TO TRANSACTION UNION UNIQUE UNTIL UPDATE
syntax keyword openroadKeyword	VALUES VIEW WHERE WHILE WITH WORK

syntax keyword openroadEvent	CHILDCLICK CHILDCLICKPOINT CHILDDETAILS
syntax keyword openroadEvent	CHILDDOUBLECLICK CHILDDRAGBOX CHILDDRAGSEGMENT
syntax keyword openroadEvent	CHILDENTRY CHILDEXIT CHILDMOVED CHILDPROPERTIES
syntax keyword openroadEvent	CHILDRESIZED CHILDSCROLL CHILDSELECT
syntax keyword openroadEvent	CHILDSETVALUE CHILDUNSELECT CHILDVALIDATE
syntax keyword openroadEvent	CLICK CLICKPOINT DBEVENT DETAILS DOUBLECLICK
syntax keyword openroadEvent	DRAGBOX DRAGSEGMENT ENTRY EXIT INSERTROW MOVED
syntax keyword openroadEvent	PROPERTIES RESIZED SCROLL SELECT
syntax keyword openroadEvent	SELECTIONCHANGED SETVALUE TERMINATE UNSELECT
syntax keyword openroadEvent	USEREVENT VALIDATE WINDOWCLOSE WINDOWICON
syntax keyword openroadEvent	WINDOWMOVED WINDOWRESIZED WINDOWVISIBLE

syntax keyword openroadTodo contained	TODO

" Catch errors caused by wrong parenthesis
"
syntax cluster	openroadParenGroup	contains=openroadParenError,openroadTodo
syntax region	openroadParen		transparent start='(' end=')' contains=ALLBUT,@openroadParenGroup
syntax match	openroadParenError	")"
hi def link	openroadParenError	cError

" Numbers
"
syntax case ignore
syntax match	openroadNumber		"\<[0-9]\+\>"
syntax case match

" String
"
syntax region	openroadString		start=+'+  end=+'+

" Operators and Data Types
"
syntax match	openroadOperator	/[\+\-\*\/=\<\>;\(\)]/
syntax keyword	openroadType		SMALLINT INTEGER1 INTEGER2 INTEGER4 INTEGER
syntax keyword	openroadType		INT1 INT2 INT4 FLOAT CHAR VARCHAR DATE
syntax keyword	openroadType		ARRAY IFNULL

" System Classes
"
syntax keyword	openroadClass		ActiveField DBSessionObject FrameExec
syntax keyword	openroadClass		ProcExec QueryObject StringObject
syntax keyword	openroadClass		CurFrame CurProcedure CurMethod CurObject

" System Constants
"
syntax keyword	openroadConst		FALSE IS NOT NULL TRUE
syntax keyword	openroadConst		FM_UPDATE FM_QUERY FM_READ FM_USER1 FM_USER2
syntax keyword	openroadConst		FM_USER3

" Identifiers
"
syntax match openroadIdent			/[a-zA-Z_][a-zA-Z_]*![a-zA-Z_][a-zA-Z_]*/

" Comments
"
if exists("openroad_comment_strings")
	syntax match openroadCommentSkip	contained "^\s*\*\($\|\s\+\)"
	syntax region openroadCommentString	contained start=+"+ skip=+\\\\\|\\"+ end=+"+ end="$"
	syntax region openroadComment		start="/\*" end="\*/" contains=openroadCommentString,openroadCharacter,openroadNumber
	syntax match openroadComment		"//.*" contains=openroadComment2String,openroadCharacter,openroadNumber
else
	syn region openroadComment		start="/\*" end="\*/"
	syn match openroadComment		"//.*"
endif

" The default highlighting.
hi def link openroadKeyword	Statement
hi def link openroadEvent	Statement
hi def link openroadNumber	Number
hi def link openroadString	String
hi def link openroadComment	Comment
hi def link openroadOperator	Operator
hi def link openroadType	Type
hi def link openroadClass	Type
hi def link openroadConst	Constant
hi def link openroadIdent	Identifier
hi def link openroadTodo	Todo

let b:current_syntax = "openroad"
