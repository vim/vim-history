" Vim syntax file
" Language:	Dylan
" Authors:	Justus Pendleton <justus@acm.org>
" Last Change:	Fri Sep 29 13:53:27 PDT 2000
"

syn clear
syn case ignore

syn region	dylanintrInfo		matchgroup=Statement start="^" end=":" oneline
syn match	dylanintrInterface	"define interface"
syn match	dylanintrClass		"<.*>"
syn region	dylanintrType		start=+"+ skip=+\\\\\|\\"+ end=+"+

syn region	dylanintrIncluded	contained start=+"+ skip=+\\\\\|\\"+ end=+"+
syn match 	dylanintrIncluded	contained "<[^>]*>"
syn match 	dylanintrInclude	"^\s*#\s*include\>\s*["<]" contains=intrIncluded

"syn keyword intrMods pointer struct
if !exists("did_dylan_intr_syntax_inits")
  let did_dylan_intr_syntax_inits = 1
  hi link dylanintrInfo		Special
  hi link dylanintrInterface	Operator
  hi link dylanintrMods		Type
  hi link dylanintrClass	StorageClass
  hi link dylanintrType		Type
  hi link dylanintrIncluded	String
  hi link dylanintrInclude	Include
endif

let b:current_syntax = "dylan-intr"

" vim:ts=8
