" Vim syntax file
" Language:	occam
" Copyright:	Fred Barnes <frmb2@kent.ac.uk>, Mario Schweigler <ms44@kent.ac.uk>
" Maintainer:	Mario Schweigler <ms44@kent.ac.uk>
" Last Change:	07 May 2003

if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

"{{{  Settings
" Set shift width for indent
setlocal shiftwidth=2
" Set the tab key size to two spaces
setlocal softtabstop=2
" Let tab keys always be expanded to spaces
setlocal expandtab

" Dots are valid in occam identifiers
setlocal iskeyword+=.
"}}}

syn case match

syn keyword occamType		BYTE BOOL REAL INT INT16 INT32 INT64 REAL32 REAL64
syn keyword occamType		CHAN DATA OF TYPE TIMER INITIAL VAL PORT MOBILE PLACED
syn keyword occamType		PACKED RECORD PROTOCOL SHARED

syn keyword occamStructure	SEQ PAR IF ALT PRI FORKING

syn keyword occamKeyword	PROC IS TRUE FALSE SIZE AND OR XOR NOT RECURSIVE REC
syn keyword occamKeyword	RETYPES RESHAPES STEP FROM FOR RESCHEDULE STOP SKIP FORK
syn keyword occamKeyword	FUNCTION VALOF RESULT ELSE CLONE CLAIM
syn keyword occamBoolean	TRUE FALSE
syn keyword occamRepeat		WHILE
syn keyword occamConditional	CASE
syn keyword occamConstant	MOSTNEG MOSTPOS

syn keyword occamOperator	AFTER TIMES MINUS PLUS INITIAL

syn match occamOperator		/::\|:=\|?\|??\|!/
syn match occamOperator		/<\|>\|+\|-\|\*\|\/\|\\\|=\|\~/

syn match occamSpecialChar	/\M**\|*'\|*"\|*#\(\[0-9A-F\]\+\)/ contained
syn match occamChar		/\M\L\='\[^*\]'/
syn match occamChar		/L'[^']*'/ contains=occamSpecialChar

syn case ignore
syn match occamTodo		/\<todo\>:\=/ contained
syn match occamNote		/\<note\>:\=/ contained
syn case match
syn keyword occamNote		NOT contained

syn match occamComment		/--.*/ contains=occamCommentTitle,occamTodo,occamNote
syn match occamCommentTitle	/--\s*\u\a*\(\s\+\u\a*\)*:/hs=s+2 contained contains=occamTodo,occamNote
syn match occamCommentTitle	/--\s*KROC-LIBRARY\(\.so\|\.a\)\=\s*$/hs=s+2 contained
syn match occamCommentTitle	/--\s*\(KROC-OPTIONS:\|RUN-PARAMETERS:\)/hs=s+2 contained

syn match occamIdentifier	/\<[A-Z.][A-Z.0-9]*\>/
syn match occamFunction		/\<[A-Za-z.][A-Za-z0-9.]*\>/ contained

syn region occamString		start=/"/ skip=/\M*"/ end=/"/ contains=occamSpecialChar
syn region occamCharString	start=/'/ end=/'/ contains=occamSpecialChar

syn match occamNumber		/\<\d\+\(\.\d\+\)\=/
syn match occamNumber		/-\d\+\(\.\d\+\)\=/
syn match occamNumber		/#\(\d\|[A-F]\)\+/
syn match occamNumber		/-#\(\d\|[A-F]\)\+/

syn keyword occamCDstr		SHARED EXTERNAL DEFINED NOALIAS NOUSAGE contained

syn region occamCDirective	start=/^\s*#\(USE\|INCLUDE\|PRAGMA\|DEFINE\|UNDEF\|IF\|ELIF\|ELSE\|ENDIF\|WARNING\|ERROR\)\>/ end=/$/ contains=occamString,occamComment,occamCDStr

syn match occamBrackets		/\[\|\]/
syn match occamParantheses	/(\|)/

if version >= 508 || !exists("did_occam_syn_inits")
  if version < 508
    let did_occam_syntax_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  HiLink occamType Type
  HiLink occamKeyword Keyword
  HiLink occamComment Comment
  HiLink occamCommentTitle PreProc
  HiLink occamTodo Todo
  HiLink occamNote Todo
  HiLink occamString String
  HiLink occamCharString String
  HiLink occamNumber Number
  HiLink occamCDirective PreProc
  HiLink occamBoolean Boolean
  HiLink occamSpecialChar SpecialChar
  HiLink occamChar Character
  HiLink occamStructure Structure
  HiLink occamIdentifier Identifier
  HiLink occamConstant Constant
  HiLink occamOperator Operator
  HiLink occamCDStr String
  HiLink occamFunction Ignore
  HiLink occamRepeat Repeat
  HiLink occamConditional Conditional
  HiLink occamBrackets Type
  HiLink occamParantheses Delimiter

  delcommand HiLink
endif

let b:current_syntax = "occam"

