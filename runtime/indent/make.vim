" Vim indent file
" Language:	Makefile
" Maintainer:	Nam SungHyun <namsh@kldp.org>
" Last Change:	2001 Jul 12

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
  finish
endif
let b:did_indent = 1

setlocal indentexpr=GetMakeIndent()

" Only define the function once.
if exists("*GetMakeIndent")
  finish
endif

function GetMakeIndent()
  if v:lnum == 1
    return 0
  endif
  let ind = indent(v:lnum - 1)
  let line = getline(v:lnum - 1)
  if line == ''
    let ind = 0
  elseif line =~ '^[^ \t#:][^#:]*:\{1,2}\([^=:]\|$\)'
    let ind = ind + &ts
  else
    let ind = indent(v:lnum)
  endif
  return ind
endfunction
