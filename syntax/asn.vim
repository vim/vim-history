" Vim syntax file
" Language:	ASN.1
" Maintainer:	Claudio Fleiner <claudio@fleiner.com>
" URL:		http://www.fleiner.com/vim/syntax/asn.vim
" Last change:	1998 Jan 12

" Remove any old syntax stuff hanging around
syn clear

" keyword definitions
syn keyword asnExternal       DEFINITIONS TAGS BEGIN END IMPLICIT DEFAULT IMPORTS EXPORTS FROM
syn keyword asnTypeInfo       ABSENT PRESENT OPTIONAL FROM SIZE UNVERSAL APPLICATION PRIVATE
syn keyword asnBoolValue      TRUE FALSE
syn match   asnNumber         "\<PLUS-INFINITY\>"
syn match   asnNumber         "\<MINUS-INFINITY\>"
syn keyword asnType           INTEGER REAL STRING BIT BOOLEAN OCTET NULL EMBEDDED PDV
syn keyword asnType           BMPString IA5String TeletexString GeneralString GraphicString ISO646String NumericString PrintableString T61String UniversalString VideotexString VisibleString
syn match   asnType           "\.\.\."
syn match   asnType           "OBJECT\s\+IDENTIFIER"
syn match   asnType           "TYPE-IDENTIFIER"
syn keyword asnStructure      CHOICE SEQUENCE SET OF ENUMERATED CONSTRAINED BY WITH COMPONENTS CLASS

" Strings and constants
syn match   asnSpecial           contained "\\[0-9][0-9][0-9]\|\\."
syn region  asnString            start=+"+  skip=+\\\\\|\\"+  end=+"+  contains=asnSpecial
syn match   asnCharacter         "'[^\\]'"
syn match   asnSpecialCharacter  "'\\.'"
syn match   asnNumber            "-\=\<[0-9]\+L\=\>\|0[xX][0-9a-fA-F]\+\>"
syn match   asnLineComment       "--.*"
syn match   asnLineComment       "--.*--"

syn match asnDefinition "^\s*[a-zA-Z][-a-zA-Z0-9_.\[\] \t{}]* *::="me=e-3 contains=asnType
syn match asnBraces     "[{}]"

syn sync ccomment asnComment

if !exists("did_asn_syntax_inits")
  let did_asn_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link asnDefinition       Function
  hi link asnBraces           Function
  hi link asnStructure        Statement
  hi link asnBoolValue        Boolean
  hi link asnSpecial          Special
  hi link asnString           String
  hi link asnCharacter        Character
  hi link asnSpecialCharacter asnSpecial
  hi link asnNumber           asnValue
  hi link asnComment          Comment
  hi link asnLineComment      asnComment
  hi link asnType             Type
  hi link asnTypeInfo         PreProc
  hi link asnValue            Number
  hi link asnExternal         Include
endif

let b:current_syntax = "asn"

" vim: ts=8
