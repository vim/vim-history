" Vim syntax file
" Language:	Modula 2
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last change:	1997 Sep 14

" very basic things only (based on the vgrindefs file).
" If you use this language, please improve it, and send me the patches!

" Remove any old syntax stuff hanging around
syn clear

syn case ignore

" A bunch of keywords
syn keyword modula2Keyword and array by case const definition div do else
syn keyword modula2Keyword elsif exit export for from if implementation import
syn keyword modula2Keyword in loop mod not of or pointer qualified record
syn keyword modula2Keyword repeat return set then to type until var while with

" Special keywords
syn keyword modula2Block procedure function module
syn keyword modula2Block begin end

" Comments
syn region modula2Comment start="{" end="}"
syn region modula2Comment start="(\*" end="\*)"

" Strings
syn region modula2String start=+"+ end=+"+

if !exists("did_modula2_syntax_inits")
  let did_modula2_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link modula2Keyword	Statement
  hi link modula2Block	PreProc
  hi link modula2Comment	Comment
  hi link modula2String	String
endif

let b:current_syntax = "modula2"

" vim: ts=8
