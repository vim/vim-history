" Vim syntax file
" Language:	Configuration File (ini file) for MSDOS/MS Windows
" Maintainer:	Sean M. McKee <mckee@misslink.net>
" Last Change:	2000 Nov 04
" Version Info: @(#)dosini.vim	1.6	97/12/15 08:54:12

" clear any unwanted syntax defs
syn clear

" shut case off
syn case ignore

syn match  dosiniLabel		"^.\{-}="
syn region dosiniHeader		start="\[" end="\]"
syn match  dosiniComment	"^;.*$"

" The default highlighting.
hi def link dosiniHeader	Special
hi def link dosiniComment	Comment
hi def link dosiniLabel		Type

let b:current_syntax = "dosini"

" vim:ts=8
