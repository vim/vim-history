" Vim syntax file
" Language   : po (GNU gettext)
" Maintainer : Sung-Hyun Nam <namsh@lgic.co.kr>
" Last change: 1998/05/27

" remove any old syntax stuff hanging around
syn clear

syn match poComment	"^#.*$"
syn match poSources	"^#:.*$"
syn match poStatement	"^\(msgid\|msgstr\)"
syn match poSpecial	contained "\\[0-7][0-7][0-7]\=\|\\."
syn region poString	start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=poSpecial

if !exists("did_po_syntax_inits")
  let did_po_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later.
  hi link poComment	Comment
  hi link poSources	PreProc
  hi link poStatement	Statement
  hi link poSpecial	Special
  hi link poString	String
endif

let b:current_syntax = "po"

" vim:set ts=8 sts=8 sw=8 noet:
