" Vim syntax file
" Language:	C
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	1998 Aug 13

" Remove any old syntax stuff hanging around
syn clear

" A bunch of useful C keywords
syn keyword	cStatement	goto break return continue asm
syn keyword	cLabel		case default
syn keyword	cConditional	if else switch
syn keyword	cRepeat		while for do

syn keyword	cTodo		contained TODO FIXME XXX

" String and Character constants
" Highlight special characters (those which have a backslash) differently
syn match	cSpecial	contained "\\x\x\+\|\\\o\{1,3\}\|\\.\|\\$"
syn region	cString		start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=cSpecial
syn match	cCharacter	"'[^\\]'"
syn match	cSpecialCharacter "'\\.'"
syn match	cSpecialCharacter "'\\\o\{1,3\}'"

"when wanted, highlight trailing white space
if exists("c_space_errors")
  syn match	cSpaceError	"\s*$"
  syn match	cSpaceError	" \+\t"me=e-1
endif

"catch errors caused by wrong parenthesis
syn cluster	cParenGroup	contains=cParenError,cIncluded,cSpecial,cTodo,cUserCont,cUserLabel,cBitField
syn region	cParen		transparent start='(' end=')' contains=ALLBUT,@cParenGroup
syn match	cParenError	")"
syn match	cInParen	contained "[{}]"

"integer number, or floating point number without a dot and with "f".
syn case ignore
syn match	cNumber		"\<\d\+\(u\=l\=\|lu\|f\)\>"
"floating point number, with dot, optional exponent
syn match	cFloat		"\<\d\+\.\d*\(e[-+]\=\d\+\)\=[fl]\=\>"
"floating point number, starting with a dot, optional exponent
syn match	cFloat		"\.\d\+\(e[-+]\=\d\+\)\=[fl]\=\>"
"floating point number, without dot, with exponent
syn match	cFloat		"\<\d\+e[-+]\=\d\+[fl]\=\>"
"hex number
syn match	cNumber		"\<0x\x\+\(u\=l\=\|lu\)\>"
"syn match cIdentifier	"\<[a-z_][a-z0-9_]*\>"
syn case match
" flag an octal number with wrong digits
syn match	cOctalError	"\<0\o*[89]"

if exists("c_comment_strings")
  " A comment can contain cString, cCharacter and cNumber.
  " But a "*/" inside a cString in a cComment DOES end the comment!  So we
  " need to use a special type of cString: cCommentString, which also ends on
  " "*/", and sees a "*" at the start of the line as comment again.
  " Unfortunately this doesn't very well work for // type of comments :-(
  syntax match	cCommentSkip	contained "^\s*\*\($\|\s\+\)"
  syntax region cCommentString	contained start=+"+ skip=+\\\\\|\\"+ end=+"+ end=+\*/+me=s-1 contains=cSpecial,cCommentSkip
  syntax region cComment2String	contained start=+"+ skip=+\\\\\|\\"+ end=+"+ end="$" contains=cSpecial
  syntax region cComment	start="/\*" end="\*/" contains=cTodo,cCommentString,cCharacter,cNumber,cFloat,cSpaceError
  syntax match  cComment	"//.*" contains=cTodo,cComment2String,cCharacter,cNumber,cSpaceError
else
  syn region	cComment	start="/\*" end="\*/" contains=cTodo,cSpaceError
  syn match	cComment	"//.*" contains=cTodo,cSpaceError
endif
syntax match	cCommentError	"\*/"

syn keyword	cOperator	sizeof
syn keyword	cType		int long short char void size_t
syn keyword	cType		signed unsigned float double
syn keyword	cStructure	struct union enum typedef
syn keyword	cStorageClass	static register auto volatile extern const

syn region	cPreCondit	start="^\s*#\s*\(if\>\|ifdef\>\|ifndef\>\|elif\>\|else\>\|endif\>\)" skip="\\$" end="$" contains=cComment,cString,cCharacter,cNumber,cCommentError,cSpaceError
syn region	cIncluded	contained start=+"+ skip=+\\\\\|\\"+ end=+"+
syn match	cIncluded	contained "<[^>]*>"
syn match	cInclude	"^\s*#\s*include\>\s*["<]" contains=cIncluded
"syn match cLineSkip	"\\$"
syn cluster	cPreProcGroup	contains=cPreCondit,cIncluded,cInclude,cDefine,cInParen,cUserLabel
syn region	cDefine		start="^\s*#\s*\(define\>\|undef\>\)" skip="\\$" end="$" contains=ALLBUT,@cPreProcGroup
syn region	cPreProc	start="^\s*#\s*\(pragma\>\|line\>\|warning\>\|warn\>\|error\>\)" skip="\\$" end="$" contains=ALLBUT,@cPreProcGroup

" Highlight User Labels
syn cluster	cMultiGroup	contains=cIncluded,cSpecial,cTodo,cUserCont,cUserLabel,cBitField
syn region	cMulti		transparent start='?' end=':' contains=ALLBUT,@cMultiGroup
" Avoid matching foo::bar() in C++ by requiring that the next char is not ':'
syn match	cUserCont	"^\s*\I\i*\s*:$" contains=cUserLabel
syn match	cUserCont	";\s*\I\i*\s*:$" contains=cUserLabel
syn match	cUserCont	"^\s*\I\i*\s*:[^:]"me=e-1 contains=cUserLabel
syn match	cUserCont	";\s*\I\i*\s*:[^:]"me=e-1 contains=cUserLabel

syn match	cUserLabel	"\I\i*" contained

" Avoid recognizing most bitfields as labels
syn match	cBitField	"^\s*\I\i*\s*:\s*[1-9]"me=e-1
syn match	cBitField	";\s*\I\i*\s*:\s*[1-9]"me=e-1

if !exists("c_minlines")
  let c_minlines = 15
endif
exec "syn sync ccomment cComment minlines=" . c_minlines

if !exists("did_c_syntax_inits")
  let did_c_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link cLabel	Label
  hi link cUserLabel	Label
  hi link cConditional	Conditional
  hi link cRepeat	Repeat
  hi link cCharacter	Character
  hi link cSpecialCharacter cSpecial
  hi link cNumber	Number
  hi link cFloat	Float
  hi link cOctalError	cError
  hi link cParenError	cError
  hi link cInParen	cError
  hi link cCommentError	cError
  hi link cSpaceError	cError
  hi link cOperator	Operator
  hi link cStructure	Structure
  hi link cStorageClass	StorageClass
  hi link cInclude	Include
  hi link cPreProc	PreProc
  hi link cDefine	Macro
  hi link cIncluded	cString
  hi link cError	Error
  hi link cStatement	Statement
  hi link cPreCondit	PreCondit
  hi link cType		Type
  hi link cCommentError	cError
  hi link cCommentString cString
  hi link cComment2String cString
  hi link cCommentSkip	cComment
  hi link cString	String
  hi link cComment	Comment
  hi link cSpecial	SpecialChar
  hi link cTodo		Todo
  "hi link cIdentifier	Identifier
endif

let b:current_syntax = "c"

" vim: ts=8
