" Vim syntax file
" Language:	generic ChangeLog file
" Maintainer:	Gediminas Paulauskas <menesis@delfi.lt>
" Last Change:	Jan 9, 2001

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn case ignore

syn match	changelogText	"^\s.*$" contains=changelogMail,changelogFiles,changelogBullet,changelogNumber,changelogMonth,changelogDay
syn match	changelogHeader	"^\S.*$" contains=changelogNumber,changelogMonth,changelogDay,changelogMail
syn match	changelogFiles	"^\s\+[+*]\s\+.\{-}:" contains=changelogBullet
syn match	changelogBullet	contained "^\s\+[+*]\s"
syn match	changelogMail	contained "<[A-Za-z0-9\._:+-]\+@[A-Za-z0-9\._-]\+>"
syn keyword	changelogMonth	contained jan feb mar apr may jun jul aug sep oct nov dec
syn keyword	changelogDay	contained mon tue wed thu fri sat sun
syn match	changelogNumber	contained "[.-]*[0-9]\+"

" The default highlighting.
hi def link changelogText	Normal
hi def link changelogBullet	Type
hi def link changelogFiles	Comment
hi def link changelogHeader	Statement
hi def link changelogMail	Special
hi def link changelogNumber	Number
hi def link changelogMonth	Number
hi def link changelogDay	Number

let b:current_syntax = "changelog"

" vim: ts=8
