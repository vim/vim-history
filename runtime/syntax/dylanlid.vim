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

syn sync 	lines=50

if !exists("did_dylan_lid_syntax_inits")
  let did_dylan_lid_syntax_inits = 1
  hi link dylanlidInfo		Type
  hi link dylanlidEntry		String
endif

let b:current_syntax = "dylan-lid"

" vim:ts=8
