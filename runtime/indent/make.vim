" Vim indent file
" Language:	Makefile
" Maintainer:	Sung-Hyun Nam <namsh@kldp.org>
" Last Change:	2000 Sep 02

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
  if getline(v:lnum - 1) =~ '^\S\+.*:'
    let ind = ind + &sw
  endif
  return ind
endfunction
