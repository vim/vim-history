" Vim indent file
" Language:	Perl
" Author:	Rafael Garcia-Suarez <rgarciasuarez@free.fr>
" URL:		http://rgarciasuarez.free.fr/vim/indent/perl.vim
" Last Change:	2000-12-04

setlocal indentexpr=GetPerlIndent()
setlocal indentkeys-=0# indentkeys+=0=,0)

" Only define the function once.
if exists("*GetPerlIndent")
  finish
endif

function GetPerlIndent()
  " Don't indent POD
  let cline = getline(v:lnum)
  if cline =~ '^\s*='
    return 0
  endif

  " Find a non-blank line above the current line.
  let lnum = prevnonblank(v:lnum - 1)

  " Hit the start of the file, use zero indent.
  if lnum == 0
    return 0
  endif

  let line = getline(lnum)
  let ind = indent(lnum)
  " Don't indent comments
  if line =~ '^\s*#'
    return ind
  endif
  if line =~ '[{(]\s*$'
    let ind = ind + &sw
  endif
  if cline =~ '^\s*[)}]'
    let ind = ind - &sw
  endif
  " Indent to column 0 if previous line contains <<XXX
  if line =~ "<<[\"']*\\u\\+"
    let ind = 0
  endif

  return ind
endfunction
