" Vim syntax file
" Language:     Blank 1.4.1
" Maintainer:   Rafal M. Sulejman <unefunge@friko2.onet.pl>
" Last change:  21 Jul 2000

" Remove any old syntax
syn clear
syn case ignore

" Blank instructions
syn match blankInstruction "{[:;,\.+\-*$#@/\\`'"!\|><{}\[\]()?xspo\^&\~=_%]}"

" Common strings
syn match blankString "\~[^}]"

" Numbers
syn match blankNumber "\[[0-9]\+\]"

syn case match

" The default highlighting.
hi def link blankInstruction      Statement
hi def link blankNumber           Number
hi def link blankString           String

let b:current_syntax = "blank"
" vim: ts=8
