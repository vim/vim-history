" Vim syntax file
" Language:	C
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	1998 Jan 6

" Remove any old syntax stuff hanging around
syn clear

" A bunch of useful C keywords
syn keyword cStatement	goto break return continue asm
syn keyword cLabel		case default
syn keyword cConditional	if else switch
syn keyword cRepeat		while for do

syn keyword cTodo contained	TODO FIXME XXX

" String and Character constants
" Highlight special characters (those which have a backslash) differently
syn match cSpecial contained	"\\[0-7][0-7][0-7]\=\|\\."
syn region cString		start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=cSpecial
syn match cCharacter		"'[^\\]'"
syn match cSpecialCharacter	"'\\.'"
syn match cSpecialCharacter	"'\\[0-7][0-7]'"
syn match cSpecialCharacter	"'\\[0-7][0-7][0-7]'"

"catch errors caused by wrong parenthesis
syn region cParen		transparent start='(' end=')' contains=ALLBUT,cParenError,cIncluded,cSpecial,cTodo,cUserLabel,cBitField
syn match cParenError		")"
syn match cInParen contained	"[{}]"

"integer number, or floating point number without a dot and with "f".
syn case ignore
syn match cNumber		"\<[0-9]\+\(u\=l\=\|lu\|f\)\>"
"floating point number, with dot, optional exponent
syn match cFloat		"\<[0-9]\+\.[0-9]*\(e[-+]\=[0-9]\+\)\=[fl]\=\>"
"floating point number, starting with a dot, optional exponent
syn match cFloat		"\.[0-9]\+\(e[-+]\=[0-9]\+\)\=[fl]\=\>"
"floating point number, without dot, with exponent
syn match cFloat		"\<[0-9]\+e[-+]\=[0-9]\+[fl]\=\>"
"hex number
syn match cNumber		"\<0x[0-9a-f]\+\(u\=l\=\|lu\)\>"
"syn match cIdentifier	"\<[a-z_][a-z0-9_]*\>"
syn case match
" flag an octal number with wrong digits
syn match cOctalError		"\<0[0-7]*[89]"

if exists("c_comment_strings")
  " A comment can contain cString, cCharacter and cNumber.
  " But a "*/" inside a cString in a cComment DOES end the comment!  So we
  " need to use a special type of cString: cCommentString, which also ends on
  " "*/", and sees a "*" at the start of the line as comment again.
  " Unfortunately this doesn't very well work for // type of comments :-(
  syntax match cCommentSkip	contained "^\s*\*\($\|\s\+\)"
  syntax region cCommentString	contained start=+"+ skip=+\\\\\|\\"+ end=+"+ end=+\*/+me=s-1 contains=cSpecial,cCommentSkip
  syntax region cComment2String	contained start=+"+ skip=+\\\\\|\\"+ end=+"+ end="$" contains=cSpecial
  syntax region cComment	start="/\*" end="\*/" contains=cTodo,cCommentString,cCharacter,cNumber,cFloat
  syntax match  cComment	"//.*" contains=cTodo,cComment2String,cCharacter,cNumber
else
  syn region cComment		start="/\*" end="\*/" contains=cTodo
  syn match cComment		"//.*" contains=cTodo
endif
syntax match cCommentError	"\*/"

syn keyword cOperator	sizeof
syn keyword cType		int long short char void size_t
syn keyword cType		signed unsigned float double
syn keyword cStructure	struct union enum typedef
syn keyword cStorageClass	static register auto volatile extern const

syn region cPreCondit	start="^\s*#\s*\(if\>\|ifdef\>\|ifndef\>\|elif\>\|else\>\|endif\>\)" skip="\\$" end="$" contains=cComment,cString,cCharacter,cNumber,cCommentError
syn region cIncluded contained start=+"+ skip=+\\\\\|\\"+ end=+"+
syn match cIncluded contained "<[^>]*>"
syn match cInclude		"^\s*#\s*include\>\s*["<]" contains=cIncluded
"syn match cLineSkip	"\\$"
syn region cDefine		start="^\s*#\s*\(define\>\|undef\>\)" skip="\\$" end="$" contains=ALLBUT,cPreCondit,cIncluded,cInclude,cDefine,cInParen
syn region cPreProc		start="^\s*#\s*\(pragma\>\|line\>\|warning\>\|warn\>\|error\>\)" skip="\\$" end="$" contains=ALLBUT,cPreCondit,cIncluded,cInclude,cDefine,cInParen

" Highlight User Labels
syn region	cMulti		transparent start='?' end=':' contains=ALLBUT,cIncluded,cSpecial,cTodo,cUserLabel,cBitField
" Avoid matching foo::bar() in C++ by requiring that the next char is not ':'
syn match	cUserLabel	"^\s*\I\i*\s*:$"
syn match	cUserLabel	";\s*\I\i*\s*:$"ms=s+1
syn match	cUserLabel	"^\s*\I\i*\s*:[^:]"me=e-1
syn match	cUserLabel	";\s*\I\i*\s*:[^:]"ms=s+1,me=e-1

" Avoid recognizing most bitfields as labels
syn match	cBitField	"^\s*\I\i*\s*:\s*[1-9]"me=e-1
syn match	cBitField	";\s*\I\i*\s*:\s*[1-9]"me=e-1

syn sync ccomment cComment minlines=10

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
