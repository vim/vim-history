" Vim compiler file
" Compiler:     HTML Tidy
" Maintainer:   Doug Kearns <djkea2@mugca.cc.monash.edu.au>
" Last Change:  2001 Sep 02

if exists("current_compiler")
  finish
endif
let current_compiler = "tidy"

setlocal makeprg=tidy\ -quiet\ -errors\ %

" sample error:   line 5 column 1 - Error: <body> missing '>' for end of tag
" sample warning: line 22 column 3 - Warning: unknown attribute "alig"
setlocal errorformat=line\ %l\ column\ %c\ -\ Error:%m,
		    \line\ %l\ column\ %c\ -\ Warning:%m,
		    \%-G%.%# " ignore any lines that didn't match one of the patterns above
