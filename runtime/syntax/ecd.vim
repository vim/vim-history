" Vim syntax file
" Language:	ecd (Embedix Component Description) files
" Maintainer:	John Beppu <beppu@lineo.com>
" URL:		none
" Last Change:	2001 Jan 15

" An ECD file contains meta-data for packages in the Embedix Linux distro.
" This syntax file is derived from apachestyle.vim 
"   by Christian Hammers <ch@westend.com>

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn case ignore

" specials
syn match  ecdComment	"^\s*#.*"

" options and values
syn match  ecdAttr	"^\s*[a-zA-Z]\S*\s*[=].*$" contains=ecdAttrN,ecdAttrV
syn match  ecdAttrN     contained ".*="me=e-1
syn match  ecdAttrV     contained "=.*$"ms=s+1

" tags
syn region ecdTag	start=+<+ end=+>+ contains=ecdTagN,ecdTagError
syn match  ecdTagN	contained +<[/\s]*[-a-zA-Z0-9_]\++ms=s+1
syn match  ecdTagError	contained "[^>]<"ms=s+1

" The default highlighting.
hi def link ecdComment			Comment
hi def link ecdAttr			Special
hi def link ecdAttrN			Identifier
hi def link ecdAttrV			String
hi def link ecdTag			Special
hi def link ecdTagN			Identifier
hi def link ecdTagError			Error

let b:current_syntax = "ecd"
" vim: ts=8
