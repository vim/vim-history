" Language   : tags
" Maintainer : Dr. Charles E. Campbell, Jr. <Charles.Campbell@gsfc.nasa.gov>
" Last change: May 27, 1998

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

if !exists("did_drchip_tags_inits")
  let did_drchip_tags_inits = 1
  hi link tagAddr	Number
  hi link tagComment	Comment
  hi link tagDelim	Delimiter
  hi link tagField	PreProc
  hi link tagName	Identifier
  hi link tagPath	PreProc
  "hi link tagBaseFile	PreProc
endif

let b:current_syntax = "tags"

" vim: ts=12
