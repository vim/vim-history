" Vim syntax file
" Language:	lite
" Maintainer:	Lutz Eymers <ixtab@polzin.com>
" URL:		http://www-public.rz.uni-duesseldorf.de/~eymers/stuff/syntax_vim.tgz
" Email:        Subject: send syntax_vim.tgz
" Last change:	1998 Apr 13

syn clear
syn case match

" Internal Variables
syn keyword liteIntVar ERRMSG contained

" Comment
syn region liteComment		start="/\*" end="\*/" contains=liteTodo

" Function names
syn keyword liteFunctions  echo printf fprintf open close read
syn keyword liteFunctions  readln readtok
syn keyword liteFunctions  split strseg chop tr sub substr
syn keyword liteFunctions  test unlink umask chmod mkdir chdir rmdir
syn keyword liteFunctions  rename truncate link symlink stat
syn keyword liteFunctions  sleep system getpid getpidd kill
syn keyword liteFunctions  time ctime time2unixtime unixtime2year
syn keyword liteFunctions  unixtime2year unixtime2month unixtime2day
syn keyword liteFunctions  unixtime2hour unixtime2min unixtime2sec
syn keyword liteFunctions  strftime
syn keyword liteFunctions  getpwnam getpwuid
syn keyword liteFunctions  gethostbyname gethostbyaddress
syn keyword liteFunctions  urlEncode setContentType includeFile
syn keyword liteFunctions  msqlConnect msqlClose msqlSelectDB 
syn keyword liteFunctions  msqlQuery msqlStoreResult msqlFreeResult
syn keyword liteFunctions  msqlFetchRow msqlDataSeek msqlListDBs
syn keyword liteFunctions  msqlListTables msqlInitFieldList msqlListField
syn keyword liteFunctions  msqlFieldSeek msqlNumRows msqlEncode
syn keyword liteFunctions  exit fatal typeof

" Identifier
syn match  liteIdentifier "$[a-zA-Z_][a-zA-Z0-9_]*" contains=liteIntVar
syn match  liteGlobalIdentifier "@[a-zA-Z_][a-zA-Z0-9_]*" contains=liteIntVar

" Conditional
syn keyword liteConditional  if else 

" Repeat
syn keyword liteRepeat  while

" Operator
syn keyword liteStatement  break return continue

" Operator
syn match liteOperator  "[-+=#*]" 
syn match liteOperator  "/[^*]"me=e-1
syn match liteOperator  "/$"
syn match liteRelation  "&&" 
syn match liteRelation  "||" 
syn match liteRelation  "[!=<>]=" 
syn match liteRelation  "[<>]" 

" Include
syn keyword liteInclude  load

" Define
syn keyword liteDefine  funct

" Type 
syn keyword liteType  int uint char real

" String
syn region liteString  keepend matchgroup=None start=+"+  skip=+\\\\\|\\"+  end=+"+ contains=liteIdentifier,liteSpecialChar 

" Number
syn match liteNumber  "-\=\<\d\+\>"

" Float
syn match liteFloat  "\(-\=\<\d+\|-\=\)\.\d\+\>" 

" SpecialChar
syn match liteSpecialChar "\\[abcfnrtv\\]" contained

syn match liteParentError "[)}\]]" 

" Todo
syn keyword liteTodo TODO Todo todo contained

" dont syn #!...
syn match liteExec "^#!.*$" 

" Parents
syn region liteParent1 matchgroup=Delimiter start="(" end=")" contains=liteComment,liteFunctions,liteIdentifier,liteGlobalIdentifier,liteConditional,liteRepeat,liteStatement,liteOperator,liteRelation,liteType,liteString,liteNumber,liteFloat,liteParent1,liteParent2,liteParent3
syn region liteParent2 matchgroup=Delimiter start="{" end="}" contains=liteComment,liteFunctions,liteIdentifier,liteGlobalIdentifier,liteConditional,liteRepeat,liteStatement,liteOperator,liteRelation,liteType,liteString,liteNumber,liteFloat,liteParent1,liteParent2,liteParent3
syn region liteParent3	matchgroup=Delimiter start="\[" end="\]" contains=liteComment,liteFunctions,liteIdentifier,liteGlobalIdentifier,liteConditional,liteRepeat,liteStatement,liteOperator,liteRelation,liteType,liteString,liteNumber,liteFloat,liteParent1,liteParent2,liteParent3

if !exists("did_lite_syntax_inits")
  let did_lite_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link liteComment                   Comment
  hi link liteString                    String
  hi link liteNumber                    Number
  hi link liteFloat                     Float
  hi liteIdentifier guifg=DarkGrey ctermfg=Brown
  hi liteGlobalIdentifier guifg=DarkGrey ctermfg=Brown
  hi link liteFunctions                 Function
  hi link liteRepeat                    Repeat 
  hi link liteConditional               Conditional 
  hi link liteStatement                 Statement
  hi link liteType                      Type
  hi link liteInclude                   Include
  hi link liteDefine                    Define
  hi link liteSpecialChar               SpecialChar
  hi link liteParentError	        liteError
  hi link liteError	                Error
  hi link liteTodo                      Todo
  hi liteRelation guifg=SeaGreen ctermfg=DarkGreen
  hi liteOperator guifg=SeaGreen ctermfg=DarkGreen
  hi liteIntVar guifg=Red ctermfg=DarkRed
endif
 
let b:current_syntax = "lite"
 
" vim: ts=8
