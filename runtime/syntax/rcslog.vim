" Vim syntax file
" Language:	RCS log output
" Maintainer:	Joe Karthauser <joe@freebsd.org>
" Last Change:	2000 Nov 04

" Remove any old syntax stuff hanging around
syn clear

syn match rcslogRevision	"^revision.*$"
syn match rcslogFile		"^RCS file:.*"
syn match rcslogDate		"^date: .*$"

" The default highlighting.
hi def link rcslogFile		Type
hi def link rcslogRevision	Constant
hi def link rcslogDate		Identifier

let b:current_syntax = "rcslog"

" vim: ts=8
