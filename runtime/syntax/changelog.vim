" Vim syntax file
" Language:	generic ChangeLog file
" Maintainer:	Gediminas Paulauskas <menesis@delfi.lt>
" Last Change:	2000 Dec 10

" remove any old syntax stuff hanging around
syn clear

syn match	changelogBullet	"^\t\*\s"
syn match	changelogFiles	"^\t\*\s\+.\{-}:"
syn match	changelogMail	"<[A-Za-z0-9\._-]\+@[A-Za-z0-9\._-]\+>"
syn match	changelogDate	"[0-9]\{4}-[0-9]\{2}-[0-9]\{2}"

" The default highlighting.
hi def link changelogBullet	Comment
hi def link changelogFiles	Comment
hi def link changelogMail	Statement
hi def link changelogDate	Number

let b:current_syntax = "changelog"

" vim: ts=8
