" VIM syntax file
" Language:	Nroff/Troff
" Maintainer:	Matthias Burian <burian@grabner-instruments.com>
" Last Change:	2001 Jan 15

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

if exists("nroff_space_errors")
  syn match nroffError "\s\+$"
endif
syn match nroffCommand "^\.[a-zA-Z]" nextgroup=nroffCmdArg,nroffError
syn match nroffCommand "^\.[a-zA-Z][a-zA-Z0-9\\]" nextgroup=nroffCmdArg,nroffError

syn match nroffCmdArg contained ".*" contains=nroffString,nroffComArg,nroffError
syn region nroffString contained start=/"/ end=/"/ contains=nroffFont,nroffError
syn region nroffString contained start=/'/ end=/'/ contains=nroffFont,nroffError
syn match nroffComArg +\\["#].*+
syn match nroffComment +^\.\\".*+

syn region nroffFont start="\\f[A-Z]"hs=s+3 end="\\f[A-Z]"he=e-3 end="$"
syn region nroffFont start="\\\*<"hs=s+3 end="\\\*>"he=e-3
syn region nroffDefine start="\.ds\ [A-Za-z_]\+" end="$" contains=ALL
syn region nroffSize start="\\s[0-9]*" end="\\s[0-9]*"
syn region nroffSpecial start="^\.[TP]S$" end="^\.[TP]E$"
syn region nroffSpecial start="^\.EQ" end="^\.EN"

" The default highlighting.
hi def link nroffCommand		Statement
hi def link nroffComment		Comment
hi def link nroffComArg			Comment
hi def link nroffFont			PreProc
hi def link nroffSize			PreProc
hi def link nroffDefine			String
hi def link nroffString			String
hi def link nroffSpecial		Question
hi def link nroffError			Error

let b:current_syntax = "nroff"

" vim: ts=8
