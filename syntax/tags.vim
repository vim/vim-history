" Language   : tags
" Maintainer : Charles E. Campbell, Jr. (cec@gryphon.gsfc.nasa.gov)
" Last change: April 3, 1998

" Remove any old syntax stuff hanging around
syn clear

syn match	tagName	"^[^\t]\+"		skipwhite	nextgroup=tagPath
syn match	tagPath	"[^\t]\+"	contained	skipwhite	nextgroup=tagAddr	contains=tagBaseFile
syn match	tagBaseFile	"[a-zA-Z_]\+[\.a-zA-Z_0-9]*\t"me=e-1		contained
syn match	tagAddr	"[0-9]*"	contained skipwhite nextgroup=tagComment
syn region	tagAddr	matchgroup=tagDelim start="/" skip="\(\\\\\)*\\/" matchgroup=tagDelim end="$\|/" oneline contained skipwhite nextgroup=tagComment
syn match	tagComment	";.*$"	contained contains=tagField
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

" vim: ts=12
