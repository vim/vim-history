" Vim syntax file
"    Language: Oracle Procedureal SQL (PL/SQL)
"  Maintainer: Jeff Lanzarotta (frizbeefanatic@yahoo.com)
"         URL: http://lanzarotta.tripod.com/vim/syntax/plsql.vim.zip
" Last Change: March 6, 2001
"     Version: 6.0-5

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn case ignore

" Todo.
syn keyword	plsqlTodo		TODO FIXME XXX DEBUG NOTE

syn match   plsqlGarbage	"[^ \t()]"
syn match   plsqlIdentifier	"[a-z][a-z0-9$_#]*"
syn match   plsqlHostIdentifier	":[a-z][a-z0-9$_#]*"

" Symbols.
syn match   plsqlSymbol		"\(;\|,\|\.\)"

" Operators.
syn match   plsqlOperator	"\(+\|-\|\*\|/\|=\|<\|>\|@\|\*\*\|!=\|\~=\)"
syn match   plsqlOperator	"\(^=\|<=\|>=\|:=\|=>\|\.\.\|||\|<<\|>>\|\"\)"

" SQL keywords.
syn keyword plsqlSQLKeyword	ACCESS ADD ALL ALTER AND ANY AS ASC AUDIT
syn keyword plsqlSQLKeyword	BETWEEN BY CHECK CLUSTER COLUMN COMMENT
syn keyword plsqlSQLKeyword	COMPRESS CONNECT CREATE CURRENT
syn keyword plsqlSQLKeyword	DEFAULT DELETE DESC DISTINCT DROP ELSE
syn keyword plsqlSQLKeyword	EXCLUSIVE EXISTS FILE FROM GRANT
syn keyword plsqlSQLKeyword	GROUP HAVING IDENTIFIED IMMEDIATE IN INCREMENT
syn keyword plsqlSQLKeyword	INDEX INITIAL INSERT INTERSECT INTO IS
syn keyword plsqlSQLKeyword	LEVEL LIKE LOCK MAXEXTENTS MODE NOAUDIT
syn keyword plsqlSQLKeyword	NOCOMPRESS NOT NOWAIT OF OFFLINE
syn keyword plsqlSQLKeyword	ON ONLINE OPTION OR ORDER PCTFREE PRIOR
syn keyword plsqlSQLKeyword	PRIVILEGES PUBLIC RENAME RESOURCE REVOKE
syn keyword plsqlSQLKeyword	ROW ROWLABEL ROWS SELECT SESSION SET
syn keyword plsqlSQLKeyword	SHARE START SUCCESSFUL SYNONYM SYSDATE
syn keyword plsqlSQLKeyword	THEN TO TRIGGER TRUNCATE UID UNION UNIQUE UPDATE
syn keyword plsqlSQLKeyword	USER VALIDATE VALUES VIEW
syn keyword plsqlSQLKeyword	WHENEVER WHERE WITH
syn keyword plsqlSQLKeyword	REPLACE

" PL/SQL's own keywords.
syn keyword plsqlKeyword  ABORT ACCEPT ARRAY ARRAYLEN ASSERT ASSIGN AT
syn keyword plsqlKeyword  AUTHORIZATION AVG BASE_TABLE BEGIN BODY CASE
syn keyword plsqlKeyword  CHAR_BASE CLOSE CLUSTERS COLAUTH COMMIT
syn keyword plsqlKeyword  CONSTANT CRASH CURRVAL DATABASE DATA_BASE DBA
syn keyword plsqlKeyword  DEBUGOFF DEBUGON DECLARE DEFINTION DELAY
syn keyword plsqlKeyword  DIGITS DISPOSE DO ENTRY EXCEPTION
syn keyword plsqlKeyword  EXCEPTION_INIT EXIT FETCH FORM FUNCTION
syn keyword plsqlKeyword  GENERIC GOTO INDEXES INDICATOR INTERFACE
syn keyword plsqlKeyword  LIMITED MINUS MISLABEL NATURALN NEW NEXTVAL
syn keyword plsqlKeyword  NUMBER_BASE OFF OPEN OTHERS OUT PACKAGE PARTITION
syn keyword plsqlKeyword  PLS_INTEGER POSITIVEN	PRAGMA PRIVATE PROCEDURE
syn keyword plsqlKeyword  RAISE RANGE REF RELEASE REMR RETURN REVERSE
syn keyword plsqlKeyword  ROLLBACK ROWNUM ROWTYPE RUN SAVEPOINT SCHEMA
syn keyword plsqlKeyword  SEPERATE SPACE SPOOL SQL SQLCODE SQLERRM STATEMENT
syn keyword plsqlKeyword  STDDEV SUBTYPE SUM TABAUTH TABLES TASK TERMINATE
syn keyword plsqlKeyword  TYPE USE VARIABLE VARIANCE VIEWS WHEN WORK WRITE XOR
syn match   plsqlKeyword	"\<END\>"

" PL/SQL functions.
syn keyword plsqlFunction ABS ACOS ADD_MONTH ASCII ASIN ATAN ATAN2 AVG CEIL
syn keyword plsqlFunction CHARTOROWID CHR CONCAT CONVERT COS COSH COUNT DECODE
syn keyword plsqlFunction EXP FLOOR GREATEST HEXTORAW INITCAP INSTR INSTRB
syn keyword plsqlFunction LAST_DAY LEAST LENGTH LENGTHB LN LOG LOWER LPAD
syn keyword plsqlFunction LTRIM MAX MIN MOD MONTHS_BETWEEN NEW_TIME NEX_DAY
syn keyword plsqlFunction NLS_INITCAP NLS_LOWER NLS_UPPER NLSSORT NVL POWER
syn keyword plsqlFunction RAWTOHEX REPLACE ROUND ROUND ROWIDTOCHAR RPAD RTRIM
syn keyword plsqlFunction SIGN SIN SINH SOUNDEX SQRT STDDEV SUBSTR SUBSTRB SUM
syn keyword plsqlFunction SYSDATE TAN TANH TO_CHAR TO_DATE TO_NUMBER TRANSLATE
syn keyword plsqlFunction TRUNC TRUNC UID UPPER USER USERENV VARIANCE VSIZE

" Oracle Pseudo Colums.
syn keyword plsqlPseudo  CURRVAL LEVEL NEXTVAL ROWID ROWNUM

if exists("plsql_highlight_triggers")
  syn keyword plsqlTrigger  INSERTING UPDATING DELETING
endif

" Conditionals.
syn keyword plsqlConditional  ELSIF ELSE IF
syn match   plsqlConditional  "\<END\s\+IF\>"

" Loops.
syn keyword plsqlRepeat  FOR LOOP WHILE
syn match   plsqlRepeat  "\<END\s\+LOOP\>"

" Various types of comments.
syn match   plsqlComment	"--.*$" contains=plsqlTodo
syn region  plsqlComment	start="/\*" end="\*/" contains=plsqlTodo
syn sync ccomment plsqlComment

" To catch unterminated string literals.
syn match   plsqlStringError	"'.*$"

" Various types of literals.
syn match   plsqlIntLiteral	"[+-]\=[0-9]\+"
syn match   plsqlFloatLiteral	"[+-]\=\([0-9]*\.[0-9]\+\|[0-9]\+\.[0-9]\+\)\(e[+-]\=[0-9]\+\)\="
syn match   plsqlCharLiteral	"'[^']'"
syn match   plsqlStringLiteral	"'\([^']\|''\)*'"
syn keyword plsqlBooleanLiteral	TRUE FALSE NULL

" The built-in types.
syn keyword plsqlStorage  BINARY_INTEGER BOOLEAN CHAR CURSOR DATE DECIMAL
syn keyword plsqlStorage  FLOAT INTEGER LONG MLSLABEL NATURAL NUMBER
syn keyword plsqlStorage  POSITIVE RAW REAL RECORD ROWID SMALLINT TABLE
syn keyword plsqlStorage  VARCHAR VARCHAR2

" A type-attribute is really a type.
syn match   plsqlTypeAttribute	":\=[a-z][a-z0-9$_#]*%\(TYPE\|ROWTYPE\)\>"

" All other attributes.
syn match   plsqlAttribute	"%\(NOTFOUND\|ROWCOUNT\|FOUND\|ISOPEN\)\>"

" This'll catch mis-matched close-parens.
syn region plsqlParen		transparent start='(' end=')' contains=ALLBUT,plsqlParenError
syn match plsqlParenError	")"

" Syntax Synchronizing
syn sync minlines=10 maxlines=100

" The default highlighting.
" These are general categories that should maybe become standard.
hi def link Attribute Macro
hi def link Query Function
hi def link Event Function

hi def link plsqlAttribute Attribute
hi def link plsqlBooleanLiteral Boolean
hi def link plsqlCharLiteral Character
hi def link plsqlComment Comment
hi def link plsqlConditional Conditional
hi def link plsqlFloatLiteral Float
hi def link plsqlFunction Function
hi def link plsqlGarbage Error
hi def link plsqlHostIdentifier Label
hi def link plsqlIdentifier Normal
hi def link plsqlIntLiteral Number
hi def link plsqlOperator Operator
hi def link plsqlParen Normal
hi def link plsqlParenError Error
hi def link plsqlPseudo PreProc
hi def link plsqlKeyword Keyword
hi def link plsqlRepeat Repeat
hi def link plsqlStorage StorageClass
hi def link plsqlSQLKeyword Query
hi def link plsqlStringError Error
hi def link plsqlStringLiteral String
hi def link plsqlSymbol Normal
hi def link plsqlTrigger Event
hi def link plsqlTypeAttribute StorageClass
hi def link plsqlTodo Todo

let b:current_syntax = "plsql"

" vim: ts=3
