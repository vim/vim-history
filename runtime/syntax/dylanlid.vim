" Vim syntax file
" Language:	Dylan Library Interface Files
" Authors:	Justus Pendleton <justus@acm.org>
"               Brent Fulgham <bfulgham@debian.org>
" Last Change:	Fri Sep 29 13:50:20 PDT 2000
"

syn clear
syn case ignore

syn region	dylanlidInfo		matchgroup=Statement start="^" end=":" oneline
syn region	dylanlidEntry		matchgroup=Statement start=":%" end="$" oneline

syn sync	lines=50

" The default highlighting.
hi def link dylanlidInfo		Type
hi def link dylanlidEntry		String

let b:current_syntax = "dylan-lid"

" vim:ts=8
