" Vim syntax file
" Language:	DTD (Document Type Definition for XML)
" Maintainer:	Daniel Amyot <damyot@csi.uottawa.ca>
" Last change:	Fri Jul 9 1999
" Filenames:	*.dtd
" URL:		http://dtd.csi.uottawa.ca/~damyot/vim/dtd.vim
" This file is an adaptation of pascal.vim by Mario Eusebio
" I'm not sure I understand all of the syntax highlight language,
" but this file seems to do the job for simple DTD in XML.
" This would have to be extended to cover the whole of SGML DTDs though.
" Unfortunately, I don't know enough about the somewhat complex SGML
" to do it myself. Volunteers are most welcomed!

syn clear

"I prefer having the case takes into consideration.
"syn case ignore

"Comments in DTD are between <!-- and -->
syn region dtdComment	start="<!--"  end="-->"

"Strings are between quotes
syn region dtdString	start=+"+ skip=+\\\\\|\\"+  end=+"+

"Enumeration of elements or data between parenthesis
syn region dtdEnum	start="("  end=")" contains=dtdEnum,dtdCard
"Also, consider +, ?, * at the end.
syn match  dtdCard	"+"
syn match  dtdCard	"\*"
syn match  dtdCard	"\?"
"...and finally, special cases.
syn match  dtdCard	"ANY"
syn match  dtdCard	"EMPTY"

"Start tags (keywords)
syn match  dtdTag	"<!DOCTYPE"
syn match  dtdTag	"<!ELEMENT"
syn match  dtdTag	"<!ATTLIST"
syn match  dtdTag	"<!ENTITY"
syn match  dtdTag	"<!NOTATION"
"End tag
syn match  dtdTag	">"

"Attribute types
syn keyword dtdAttrType	NMTOKEN  ENTITIES  NMTOKENS  ID  CDATA
syn keyword dtdAttrType	IDREF  IDREFS  ENTITY

"Attribute Definitions
syn match  dtdAttrDef	"#REQUIRED"
syn match  dtdAttrDef	"#IMPLIED"
syn match  dtdAttrDef	"#FIXED"

syn sync lines=250

if !exists("did_dtd_syntax_inits")
  let did_dtd_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link dtdAttrType	Type
  hi link dtdAttrDef	Operator
  hi link dtdString	String
  hi link dtdEnum	String
  hi link dtdCard	String
  hi link dtdComment	Comment
  hi link dtdTag	Tag
endif

let b:current_syntax = "dtd"

" vim: ts=8
