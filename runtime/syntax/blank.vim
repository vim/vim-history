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

if !exists("did_blank_syntax_inits")
  let did_blank_syntax_inits = 1

  " The default methods for highlighting
  hi link blankInstruction      Statement
  hi link blankNumber           Number
  hi link blankString           String

endif

let b:current_syntax = "blank"
" vim: ts=8
