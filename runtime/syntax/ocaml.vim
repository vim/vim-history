" Vim syntax file
" Language:   OCAML
" Maintainers:  Karl-Heinz Sylla <Karl-Heinz.Sylla@gmd.de>
" Last changes: 1999 June 30, ocamlString now handles "\\" correctly
"                            (hint by Markus Mottl <mottl@miss.wu-wien.ac.at>)
"               1998 August 26, adding ocaml 2.00 keywords
"

" Remove any old syntax stuff hanging around.
syn clear

" ocaml is case sensitive.
syn case match

" It respects ocaml strings and constructors

syn region   ocamlComment start=+(\*+ end=+\*)+ contains=ocamlComment
syn match    ocamlCommentError +\*)+

syn keyword  ocamlKeyword	and as asr begin class closed
syn keyword  ocamlKeyword	constraint do done downto else end and
syn keyword  ocamlKeyword	exception external for fun function
syn keyword  ocamlKeyword	functor if in include inherit initializer
syn keyword  ocamlKeyword	land let lor lsl lsr lxor match
syn keyword  ocamlKeyword	method mod module mutable new object of
syn keyword  ocamlKeyword	open or parser private rec sig
syn keyword  ocamlKeyword	struct then to try type
syn keyword  ocamlKeyword	unit val virtual when while with

syn keyword  ocamlType	int char string float bool exn
syn keyword  ocamlType	list array option format
syn match    ocamlType	+()+

syn keyword  ocamlBoolean	true false

syn keyword  ocamlKeyword	not

syn match    ocamlConstructor /\<[A-Z][0-9A-Za-z_]*\>/
syn match    ocamlCharacter	+'.'\|'\\[0-9][0-9][0-9]'\|'\\[\'ntbr]'+
syn match    ocamlCharErr	+'\\[0-9][0-9]'\|'\\[0-9]'+
syn match    ocamlCharErr	+'\\[0-9][0-9]'\|'\\[0-9]'+
syn match    ocamlCharErr	+'\\[^\'ntbr]'+
syn region   ocamlString        start=+"+ skip=+\\\\\|\\"+ end=+"+

syn match    ocamlFunDef	"->"
syn match    ocamlArray	"\[|\||\]"
syn match    ocamlList	"\[\|\]"
syn match    ocamlRefAssign	":="
syn match    ocamlTopStop	";;"
syn match    ocamlOperator	"\^"
syn match    ocamlOperator	"::"
syn match    ocamlOperator	"<-"
syn match    ocamlAnyVar	+\<_\>+
syn match    ocamlKeyChar	"!"
syn match    ocamlKeyChar	"|"
syn match    ocamlKeyChar	";"
syn match    ocamlKeyChar	"?"
syn match    ocamlKeyChar	"\*"
syn match    ocamlKeyChar	"="

syn match    ocamlModQualifier /\<[A-Z][0-9A-Za-z_]*\>[\b]*\./

"if !exists("ocaml_highlighting_clean")

  " some keywords
  " some common functions are also highlighted as keywords
  " is there a better solution?
  syn keyword ocamlKeyword	trace

  syn match   ocamlNumber	"\<[0123456789]*\>"

"endif

syn sync ccomment maxlines=50

if !exists("did_ocaml_syntax_inits")

  let did_ocaml_syntax_inits = 1

  " The default methods for highlighting.  Can be overridden later

  hi link ocamlComment	Comment

  hi link ocamlModQualifier	Keyword

  hi link ocamlEnvKeyword	Keyword
  hi link ocamlKeyword	Keyword
  hi link ocamlFunDef	Keyword
  hi link ocamlRefAssign	Keyword
  hi link ocamlKeyChar	Keyword
  hi link ocamlAnyVar	Keyword
  hi link ocamlArray	Keyword
  hi link ocamlList	Keyword
  hi link ocamlTopStop	Keyword
  hi link ocamlOperator	Keyword

  hi link ocamlConstructor	Constant

  hi link ocamlBoolean	Boolean
  hi link ocamlCharacter	Character
  hi link ocamlNumber	Number
  hi link ocamlString	String

  hi link ocamlType	Type

  hi link ocamlCommentError	Error
  hi link ocamlCharCodeErr	Error

endif

let b:current_syntax = "ocaml"

" vim: ts=8
