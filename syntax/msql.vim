" Vim syntax file
" Language:	msql
" Maintainer:	Lutz Eymers <ixtab@polzin.com>
" URL:		http://www-public.rz.uni-duesseldorf.de/~eymers/stuff/syntax_vim.tgz
" Email:        Subject: send syntax_vim.tgz
" Last change:	1998 Apr 13

so $VIM/syntax/html.vim
syn clear htmlTag
syn clear javaScript
syn region  htmlTag                start=+<[^/]+ end=+>+              contains=htmlString,htmlTagName,htmlArg,htmlValue,htmlTagError,htmlEvent,msqlRegion
syn region  javaScript                           start=+<script+ end=+</script>+ contains=javaScriptSpecial,javaScriptNumber,javaScriptLineComment,javaScriptComment,javaScriptStringS,javaScriptStringD,javaStringCharacter,javaStringSpecialCharacter,javaScriptConditional,javaScriptRepeat,javaScriptBranch,javaScriptOperator,javaScriptType,javaScriptStatement,javaScriptFunction,javaScriptBoolean,javaScriptBraces,javaScriptParen,javaScriptParenError,msqlRegion

" Internal Variables
syn keyword msqlIntVar ERRMSG contained

" Env Variables
syn keyword msqlEnvVar SERVER_SOFTWARE SERVER_NAME SERVER_URL GATEWAY_INTERFACE contained
syn keyword msqlEnvVar SERVER_PROTOCOL SERVER_PORT REQUEST_METHOD PATH_INFO  contained
syn keyword msqlEnvVar PATH_TRANSLATED SCRIPT_NAME QUERY_STRING REMOTE_HOST contained
syn keyword msqlEnvVar REMOTE_ADDR AUTH_TYPE REMOTE_USER CONTEN_TYPE  contained
syn keyword msqlEnvVar CONTENT_LENGTH HTTPS HTTPS_KEYSIZE HTTPS_SECRETKEYSIZE  contained
syn keyword msqlEnvVar HTTP_ACCECT HTTP_USER_AGENT HTTP_IF_MODIFIED_SINCE  contained
syn keyword msqlEnvVar HTTP_FROM  contained

syn region msqlComment		start="/\*" end="\*/"  contained contains=msqlTodo

" Function names
syn keyword msqlFunctions  echo printf fprintf open close read contained
syn keyword msqlFunctions  readln readtok contained
syn keyword msqlFunctions  split strseg chop tr sub substr contained
syn keyword msqlFunctions  test unlink umask chmod mkdir chdir rmdir contained
syn keyword msqlFunctions  rename truncate link symlink stat contained
syn keyword msqlFunctions  sleep system getpid getpidd kill contained
syn keyword msqlFunctions  time ctime time2unixtime unixtime2year contained
syn keyword msqlFunctions  unixtime2year unixtime2month unixtime2day contained
syn keyword msqlFunctions  unixtime2hour unixtime2min unixtime2sec contained
syn keyword msqlFunctions  strftime contained
syn keyword msqlFunctions  getpwnam getpwuid contained
syn keyword msqlFunctions  gethostbyname gethostbyaddress contained
syn keyword msqlFunctions  urlEncode setContentType includeFile contained
syn keyword msqlFunctions  msqlConnect msqlClose msqlSelectDB contained
syn keyword msqlFunctions  msqlQuery msqlStoreResult msqlFreeResult contained
syn keyword msqlFunctions  msqlFetchRow msqlDataSeek msqlListDBs contained
syn keyword msqlFunctions  msqlListTables msqlInitFieldList msqlListField contained
syn keyword msqlFunctions  msqlFieldSeek msqlNumRows msqlEncode contained
syn keyword msqlFunctions  exit fatal typeof contained

" Identifier
syn match  msqlIdentifier "$[a-zA-Z_][a-zA-Z0-9_]*" contained contains=msqlEnvVar,msqlIntVar
syn match  msqlGlobalIdentifier "@[a-zA-Z_][a-zA-Z0-9_]*" contained contains=msqlEnvVar,msqlIntVar

" Conditional contained
syn keyword msqlConditional  if else  contained

" Repeat
syn keyword msqlRepeat  while contained

" Operator
syn keyword msqlStatement  break return continue contained

" Operator
syn match msqlOperator  "[-+=#*]" contained
syn match msqlOperator  "/[^*]"me=e-1 contained
syn match msqlOperator  "/$" contained
syn match msqlRelation  "&&" contained
syn match msqlRelation  "||" contained
syn match msqlRelation  "[!=<>]=" contained
syn match msqlRelation  "[<>]" contained

" Include
syn keyword msqlInclude  load contained

" Define
syn keyword msqlDefine  funct contained

" Type 
syn keyword msqlType  int uint char real contained

" String
syn region msqlString  keepend matchgroup=None start=+"+  skip=+\\\\\|\\"+  end=+"+ contains=msqlIdentifier,msqlSpecialChar,htmlTag,htmlEndTag contained 

" Number
syn match msqlNumber  "-\=\<\d\+\>" contained

" Float
syn match msqlFloat  "\(-\=\<\d+\|-\=\)\.\d\+\>" contained

" SpecialChar
syn match msqlSpecialChar "\\[abcfnrtv\\]" contained

syn match msqlParentError "[)}\]]" contained

" Parents
syn region msqlParent1	matchgroup=Delimiter start="(" end=")" contained contains=msqlComment,msqlFunctions,msqlIdentifier,msqlGlobalIdentifier,msqlConditional,msqlRepeat,msqlStatement,msqlOperator,msqlRelation,msqlType,msqlString,msqlNumber,msqlFloat,msqlParent1,msqlParent2,msqlParent3
syn region msqlParent2	matchgroup=Delimiter start="{" end="}" contained contains=msqlComment,msqlFunctions,msqlIdentifier,msqlGlobalIdentifier,msqlConditional,msqlRepeat,msqlStatement,msqlOperator,msqlRelation,msqlType,msqlString,msqlNumber,msqlFloat,msqlParent1,msqlParent2,msqlParent3
syn region msqlParent3	matchgroup=Delimiter start="\[" end="\]" contained contains=msqlComment,msqlFunctions,msqlIdentifier,msqlGlobalIdentifier,msqlConditional,msqlRepeat,msqlStatement,msqlOperator,msqlRelation,msqlType,msqlString,msqlNumber,msqlFloat,msqlParent1,msqlParent2,msqlParent3


" Todo
syn keyword msqlTodo TODO Todo todo contained

" Msql Region
syn region msqlRegion matchgroup=Delimiter start="<!$" start="<![^!->D]" end=">" contains=msql.*

if !exists("did_msql_syntax_inits")
  let did_msql_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link msqlComment                   Comment
  hi link msqlString                    String
  hi link msqlNumber                    Number
  hi link msqlFloat                     Float
  hi msqlIdentifier guifg=DarkGrey ctermfg=Brown
  hi msqlGlobalIdentifier guifg=DarkGrey ctermfg=Brown
  hi link msqlFunctions                 Function
  hi link msqlRepeat                    Repeat 
  hi link msqlConditional               Conditional 
  hi link msqlStatement                 Statement
  hi link msqlType                      Type
  hi link msqlInclude                   Include
  hi link msqlDefine                    Define
  hi link msqlSpecialChar               SpecialChar
  hi link msqlParentError	        Error
  hi link msqlTodo                      Todo
  hi msqlRelation guifg=SeaGreen ctermfg=DarkGreen
  hi msqlOperator guifg=SeaGreen ctermfg=DarkGreen
  hi msqlIntVar guifg=Red ctermfg=DarkRed
  hi msqlEnvVar guifg=Red ctermfg=DarkRed
endif
 
let b:current_syntax = "msql"
 
" vim: ts=8
