" Language:    tags
" Maintainer:  Dr. Charles E. Campbell, Jr. <Charles.E.Campbell.1@gsfc.nasa.gov>
" Last Change: January 28, 1999

" Remove any old syntax stuff hanging around
syn clear

syn match	tagName	"^[^\t]\+"		skipwhite	nextgroup=tagPath
syn match	tagPath	"[^\t]\+"	contained	skipwhite	nextgroup=tagAddr	contains=tagBaseFile
syn match	tagBaseFile	"[a-zA-Z_]\+[\.a-zA-Z_0-9]*\t"me=e-1		contained
syn match	tagAddr	"\d*"	contained skipwhite nextgroup=tagComment
syn region	tagAddr	matchgroup=tagDelim start="/" skip="\(\\\\\)*\\/" matchgroup=tagDelim end="$\|/" oneline contained skipwhite nextgroup=tagComment
syn match	tagComment	";.*$"	contained contains=tagField
syn match	tagComment	"^!_TAG_.*$"
syn match	tagField	contained "[a-z]*:"

hi def link tagBaseFile	PreProc
hi def link tagComment	Comment
hi def link tagDelim	Delimiter
hi def link tagField	Number
hi def link tagName	Identifier
hi def link tagPath	PreProc

let b:current_syntax = "tags"

" vim: ts=12
