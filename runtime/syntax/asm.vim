" Vim syntax file
" Language:	GNU Assembler
" Maintainer:	Kevin Dahlhausen <ap096@po.cwru.edu>
" Last Change:	2000 Nov 04

" Remove any old syntax stuff hanging around
syn clear

syn case ignore


" storage types
syn match asmType "\.long"
syn match asmType "\.ascii"
syn match asmType "\.asciz"
syn match asmType "\.byte"
syn match asmType "\.double"
syn match asmType "\.float"
syn match asmType "\.hword"
syn match asmType "\.int"
syn match asmType "\.octa"
syn match asmType "\.quad"
syn match asmType "\.short"
syn match asmType "\.single"
syn match asmType "\.space"
syn match asmType "\.string"
syn match asmType "\.word"

syn match asmLabel		"[a-z_][a-z0-9_]*:"he=e-1
syn match asmIdentifier		"[a-z_][a-z0-9_]*"

" Various #'s as defined by GAS ref manual sec 3.6.2.1
" Technically, the first decNumber def is actually octal,
" since the value of 0-7 octal is the same as 0-7 decimal,
" I prefer to map it as decimal:
syn match decNumber		"0\+[1-7]\=[\t\n$,; ]"
syn match decNumber		"[1-9]\d*"
syn match octNumber		"0[0-7][0-7]\+"
syn match hexNumber		"0[xX][0-9a-fA-F]\+"
syn match binNumber		"0[bB][0-1]*"


syn match asmSpecialComment	";\*\*\*.*"
syn match asmComment		";.*"hs=s+1

syn match asmInclude		"\.include"
syn match asmCond		"\.if"
syn match asmCond		"\.else"
syn match asmCond		"\.endif"
syn match asmMacro		"\.macro"
syn match asmMacro		"\.endm"

syn match asmDirective		"\.[a-z][a-z]\+"


syn case match

" The default highlighting.
hi def link asmSection		Special
hi def link asmLabel		Label
hi def link asmComment		Comment
hi def link asmDirective	Statement

hi def link asmInclude		Include
hi def link asmCond		PreCondit
hi def link asmMacro		Macro

hi def link hexNumber		Number
hi def link decNumber		Number
hi def link octNumber		Number
hi def link binNumber		Number

hi def link asmSpecialComment	Comment
hi def link asmIdentifier	Identifier
hi def link asmType		Type

" My default color overrides:
" hi def asmSpecialComment ctermfg=red
" hi def asmIdentifier ctermfg=lightcyan
" hi def asmType ctermbg=black ctermfg=brown

let b:current_syntax = "asm"

" vim: ts=8
