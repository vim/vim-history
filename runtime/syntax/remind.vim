" Vim syntax file
" Language:	Remind
" Maintainer:	Davide Alberani <alberanid@bigfoot.com>
" Last Change:	03 Dec 1999
" Version:	0.1
" URL:		http://members.xoom.com/alberanid/vim/syntax/remind.vim
"
" remind is a sophisticated reminder service
" you can download remind from ftp://ftp.doe.carleton.ca/pub/remind-3.0/

" clear any unwanted syntax defs
syn clear

" shut case off
syn case ignore

syn keyword remindCommands	REM OMIT SET FSET UNSET
syn keyword remindExpiry	UNTIL SCANFROM SCAN WARN SCHED
syn keyword remindTag		PRIORITY TAG
syn keyword remindTimed		AT DURATION
syn keyword remindMove		ONCE SKIP BEFORE AFTER
syn keyword remindSpecial	INCLUDE INC BANNER PUSH-OMIT-CONTEXT PUSH CLEAR-OMIT-CONTEXT CLEAR POP-OMIT-CONTEXT POP
syn keyword remindRun		MSG MSF RUN CAL SATISFY SPECIAL PS PSFILE SHADE MOON
syn keyword remindConditional	IF ELSE ENDIF IFTRIG
syn match remindComment		"#.*$"
syn region remindString		start=+'+ end=+'+ skip=+\\\\\|\\'+ oneline
syn region remindString		start=+"+ end=+"+ skip=+\\\\\|\\"+ oneline
syn keyword remindDebug		DEBUG DUMPVARS DUMP ERRMSG FLUSH PRESERVE
syn match remindVar		"\$[_a-zA-Z][_a-zA-Z0-9]*"
syn match remindSubst		"%[^ ]"
syn match remindAdvanceNumber	"\(\*\|+\|-\|++\|--\)[0-9]\+"

" The default highlighting.
hi def link remindCommands	Function
hi def link remindExpiry	Repeat
hi def link remindTag		Label
hi def link remindTimed		Statement
hi def link remindMove		Statement
hi def link remindSpecial	Include
hi def link remindRun		Function
hi def link remindConditional	Conditional
hi def link remindComment	Comment
hi def link remindString	String
hi def link remindDebug		Debug
hi def link remindVar		Identifier
hi def link remindSubst		Constant
hi def link remindAdvanceNumber	Number

let b:current_syntax = "remind"

"EOF	vim: ts=8 noet tw=100 sw=8 sts=0
