" Vim syntax file
" Language: Oracle Procedureal SQL (PL/SQL)
" Maintainer: Jeff Lanzarotta (jefflanzarotta@yahoo.com)
" Original Maintainer: C. Laurence Gonsalves (clgonsal@kami.com)
" URL: http://lanzarotta.tripod.com/vim/syntax/plsql.vim.zip
" Last Change: February 05, 2001
"
" For version 5.x, clear all syntax items.
" For version 6.x, quit when a syntax file was already loaded.
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

" Todo.
syn keyword plsqlTodo TODO FIXME XXX DEBUG NOTE
syn cluster plsqlCommentGroup contains=plsqlTodo

syn case ignore

syn match   plsqlGarbage "[^ \t()]"
syn match   plsqlIdentifier "[a-z][a-z0-9$_#]*"
syn match   plsqlHostIdentifier ":[a-z][a-z0-9$_#]*"

" Symbols.
syn match   plsqlSymbol "\(;\|,\|\.\)"

" Operators.
syn match   plsqlOperator "\(+\|-\|\*\|/\|=\|<\|>\|@\|\*\*\|!=\|\~=\)"
syn match   plsqlOperator "\(^=\|<=\|>=\|:=\|=>\|\.\.\|||\|<<\|>>\|\"\)"

" SQL keywords.
syn keyword plsqlSQLKeyword ACCESS ADD ALL ALTER AND ANY AS ASC AUDIT BETWEEN
syn keyword plsqlSQLKeyword BY BULK CHECK CLUSTER COLLECT COLUMN COMMENT
syn keyword plsqlSQLKeyword COMPRESS CONNECT CREATE CURRENT DEFAULT DELETE
syn keyword plsqlSQLKeyword DESC DISTINCT DROP ELSE EXCLUSIVE EXISTS FILE FROM
syn keyword plsqlSQLKeyword GRANT GROUP HAVING IDENTIFIED IMMEDIATE IN
syn keyword plsqlSQLKeyword INCREMENT INDEX INITIAL INSERT INTERSECT INTO IS
syn keyword plsqlSQLKeyword LEVEL LIKE LOCK MAXEXTENTS MODE NOAUDIT NOCOMPRESS
syn keyword plsqlSQLKeyword NOT NOWAIT OF OFFLINE ON ONLINE OPTION OR ORDER
syn keyword plsqlSQLKeyword PCTFREE PRIOR PRIVILEGES PUBLIC RENAME RESOURCE
syn keyword plsqlSQLKeyword REVOKE ROW ROWLABEL ROWS SELECT SESSION SET SHARE
syn keyword plsqlSQLKeyword START SUCCESSFUL SYNONYM SYSDATE THEN TO TRIGGER
syn keyword plsqlSQLKeyword TRUNCATE UID UNION UNIQUE UPDATE USER VALIDATE
syn keyword plsqlSQLKeyword VALUES VIEW WHENEVER WHERE WITH REPLACE

" PL/SQL's own keywords.
syn keyword plsqlKeyword ABORT ACCEPT ARRAY ARRAYLEN ASSERT ASSIGN AT
syn keyword plsqlKeyword AUTHORIZATION AVG BASE_TABLE BEGIN BODY CASE
syn keyword plsqlKeyword CHAR_BASE CLOSE CLUSTERS COLAUTH COMMIT CONSTANT
syn keyword plsqlKeyword CRASH CURRVAL DATABASE DATA_BASE DBA DEBUGOFF DEBUGON
syn keyword plsqlKeyword DECLARE DEFINTION DELAY DIGITS DISPOSE DO ENTRY
syn keyword plsqlKeyword EXCEPTION EXCEPTION_INIT EXECUTE EXIT FETCH FORM
syn keyword plsqlKeyword FUNCTION GENERIC GOTO INDEXES INDICATOR INTERFACE
syn keyword plsqlKeyword LIMITED MINUS MISLABEL NATURALN NEW NEXTVAL NOCOPY
syn keyword plsqlKeyword NUMBER_BASE OBJECT OFF OPEN OTHERS OUT PACKAGE
syn keyword plsqlKeyword PARTITION PLS_INTEGER POSITIVEN PRAGMA PRIVATE
syn keyword plsqlKeyword PROCEDURE RAISE RANGE REF RELEASE REMR RETURN REVERSE
syn keyword plsqlKeyword ROLLBACK ROWNUM ROWTYPE RUN SAVEPOINT SCHEMA SEPERATE
syn keyword plsqlKeyword SPACE SPOOL SQL SQLCODE SQLERRM STATEMENT STDDEV
syn keyword plsqlKeyword SUBTYPE SUM TABAUTH TABLES TASK TERMINATE TYPE USE
syn keyword plsqlKeyword VARIABLE VARIANCE VIEWS WHEN WORK WRITE XOR
syn match   plsqlKeyword "\<END\>"
syn match   plsqlKeyword "\.COUNT\>"hs=s+1
syn match   plsqlKeyword "\.FIRST\>"hs=s+1
syn match   plsqlKeyword "\.LAST\>"hs=s+1
syn match   plsqlKeyword "\.DELETE\>"hs=s+1
syn match   plsqlKeyword "\.PREV\>"hs=s+1
syn match   plsqlKeyword "\.NEXT\>"hs=s+1

" PL/SQL functions.
syn keyword plsqlFunction ABS ACOS ADD_MONTH ASCII ASIN ATAN ATAN2 AVG CEIL
syn keyword plsqlFunction CHARTOROWID CHR CONCAT CONVERT COS COSH COUNT DECODE
syn keyword plsqlFunction EXP FLOOR GREATEST HEXTORAW INITCAP INSTR INSTRB
syn keyword plsqlFunction LAST_DAY LEAST LENGTH LENGTHB LN LOG LOWER LPAD
syn keyword plsqlFunction LTRIM MAX MIN MOD MONTHS_BETWEEN NEW_TIME NEX_DAY
syn keyword plsqlFunction NLS_INITCAP NLS_LOWER NLS_UPPER NLSSORT NVL POWER
syn keyword plsqlFunction RAISE_APPLICATION_ERROR RAWTOHEX REPLACE ROUND
syn keyword plsqlFunction ROWIDTOCHAR RPAD RTRIM SIGN SIN SINH SOUNDEX SQRT
syn keyword plsqlFunction STDDEV SUBSTR SUBSTRB SUM SYSDATE TAN TANH TO_CHAR
syn keyword plsqlFunction TO_DATE TO_NUMBER TRANSLATE TRUNC TRUNC UID UPPER
syn keyword plsqlFunction USER USERENV VARIANCE VSIZE

" PL/SQL Exceptions
syn keyword plsqlExcept CURSOR_ALREADY_OPEN
syn keyword plsqlExcept DUP_VAL_ON_INDEX INVALID_CURSOR INVALID_NUMBER
syn keyword plsqlExcept LOGIN_DENIED
syn keyword plsqlExcept NO_DATA_FOUND NOT_LOGGED_ON
syn keyword plsqlExcept PROGRAM_ERROR ROWTYPE_MISMATCH STORAGE_ERROR
syn keyword plsqlExcept TIMEOUT_ON_RESOURCE TOO_MANY_ROWS
syn keyword plsqlExcept TRANSACTION_BACKED_OUT
syn keyword plsqlExcept VALUE_ERROR ZERO_DIVIDE

" Oracle Pseudo Colums.
syn keyword plsqlPseudo CURRVAL LEVEL NEXTVAL ROWID ROWNUM

if exists("plsql_highlight_triggers")
  syn keyword plsqlTrigger INSERTING UPDATING DELETING
endif

" Conditionals.
syn keyword plsqlConditional ELSIF ELSE IF
syn match   plsqlConditional "\<END\s\+IF\>"

" Loops.
syn keyword plsqlRepeat FOR LOOP WHILE
syn match   plsqlRepeat "\<END\s\+LOOP\>"

" Various types of comments.
syn match   plsqlComment "--.*$" contains=plsqlTodo
syn region  plsqlComment start="/\*" end="\*/" contains=plsqlTodo
syn sync ccomment plsqlComment

" To catch unterminated string literals.
syn match   plsqlStringError "'.*$"

" Various types of literals.
syn match   plsqlIntLiteral "[+-]\=[0-9]\+"
syn match   plsqlFloatLiteral "[+-]\=\([0-9]*\.[0-9]\+\|[0-9]\+\.[0-9]\+\)\(e[+-]\=[0-9]\+\)\="
syn match   plsqlCharLiteral "'[^']'"
syn match   plsqlStringLiteral "'\([^']\|''\)*'"
syn keyword plsqlBooleanLiteral TRUE FALSE NULL

" The built-in types.
syn keyword plsqlStorage BINARY_INTEGER BOOLEAN CHAR CURSOR DATE DECIMAL
syn keyword plsqlStorage FLOAT INTEGER LONG MLSLABEL NATURAL NUMBER
syn keyword plsqlStorage POSITIVE RAW REAL RECORD ROWID SMALLINT TABLE
syn keyword plsqlStorage VARCHAR VARCHAR2

" A type-attribute is really a type.
syn match plsqlTypeAttribute ":\=[a-z][a-z0-9$_#]*%\(TYPE\|ROWTYPE\)\>"

" All other attributes.
syn match plsqlAttribute "%\(NOTFOUND\|ROWCOUNT\|FOUND\|ISOPEN\)\>"

" This'll catch mis-matched close-parens.
syn region plsqlParen transparent start='(' end=')' contains=ALLBUT,plsqlParenError
syn match  plsqlParenError ")"

" Syntax Synchronizing
syn sync minlines=10 maxlines=100

" Define the default highlighting.
" For version 5.x and earlier, only when not done already.
" For version 5.8 and later, only when and item doesn't have highlighting yet.
if version >= 508 || !exists("did_plsql_syn_inits")
  if version < 508
    let did_plsql_syn_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  HiLink plsqlAttribute Macro
  HiLink plsqlBooleanLiteral Boolean
  HiLink plsqlCharLiteral Character
  HiLink plsqlComment Comment
  HiLink plsqlConditional Conditional
  HiLink plsqlExcept Exception
  HiLink plsqlFloatLiteral Float
  HiLink plsqlFunction Function
  HiLink plsqlGarbage Error
  HiLink plsqlHostIdentifier Label
  HiLink plsqlIdentifier Normal
  HiLink plsqlIntLiteral Number
  HiLink plsqlOperator Operator
  HiLink plsqlParen Normal
  HiLink plsqlParenError Error
  HiLink plsqlPseudo PreProc
  HiLink plsqlKeyword Keyword
  HiLink plsqlRepeat Repeat
  HiLink plsqlStorage StorageClass
  HiLink plsqlSQLKeyword Statement
  HiLink plsqlStringError Error
  HiLink plsqlStringLiteral String
  HiLink plsqlSymbol Normal
  HiLink plsqlTrigger Function
  HiLink plsqlTypeAttribute StorageClass
  HiLink plsqlTodo Todo

  delcommand HiLink
endif

let b:current_syntax = "plsql"

" vim: ts=8 sw=2
