" Vim syntax file
" Language:	RCS log output
" Maintainer:	Joe Karthauser <joe@freebsd.org>
" Last Change:	2001 Jan 15

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn match rcslogRevision	"^revision.*$"
syn match rcslogFile		"^RCS file:.*"
syn match rcslogDate		"^date: .*$"

" The default highlighting.
hi def link rcslogFile		Type
hi def link rcslogRevision	Constant
hi def link rcslogDate		Identifier

let b:current_syntax = "rcslog"

" vim: ts=8
