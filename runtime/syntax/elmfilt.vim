" Vim syntax file
" Language:	Elm Filter rules
" Maintainer:	Dr. Charles E. Campbell, Jr. <Charles.E.Campbell.1@gsfc.nasa.gov>
" Last Change:	November 11, 1998

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn keyword	elmfiltAction	delete execute executec forward forwardc leave save savecopy
syn match	elmfiltArg	"[^\\]%[&0-9]"lc=1	contained
syn keyword	elmfiltCond	lines always subject sender from to lines received
syn region	elmfiltMatch	start="/" skip="\\/" end="/"
syn match	elmfiltNumber	"\d\+"
syn keyword	elmfiltOper	and not matches
syn match	elmfiltOper	"\~"
syn match	elmfiltOper	"<=\|>=\|!=\|<\|<\|=\|(\|)"
syn keyword	elmfiltRule	if then
syn region	elmfiltString	start='"' skip='"\(\\\\\)*\\"' end='"'	contains=elmfiltArg
syn match	elmfiltComment	"^#.*$"

" The default highlighting.
hi def link elmfiltAction	Statement
hi def link elmfiltArg	Special
hi def link elmfiltComment	Comment
hi def link elmfiltCond	Type
hi def link elmfiltMatch	Special
hi def link elmfiltNumber	Number
hi def link elmfiltOper	Operator
hi def link elmfiltRule	Statement
hi def link elmfiltString	String

let b:current_syntax = "elmfilt"
" vim: ts=9
