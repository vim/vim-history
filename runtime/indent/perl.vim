" Vim indent file
" Language:	Perl
" Author:	Rafael Garcia-Suarez <rgarciasuarez@free.fr>
" URL:		http://rgarciasuarez.free.fr/vim/indent/perl.vim
" Last Change:	2001 Mar 02

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
  finish
endif
let b:did_indent = 1

setlocal indentexpr=GetPerlIndent()
setlocal indentkeys+=0=,0),=EO

" Only define the function once.
if exists("*GetPerlIndent")
  finish
endif

function GetPerlIndent()
  " Get the line to be indented
  let cline = getline(v:lnum)

  " Don't indent POD markers ; don't indent end-of-heredocs markers
  " (assuming they begin with 'EO')
  if cline =~ '^\s*\(=\|EO\)'
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

  " Indent to column 0 if previous line contains <<XXX
  if line =~ "<<[\"']*\\u\\+"
    return 0
  endif
  " Find the indent of the first line before the beginning of a here-doc
  if line =~ '^EO'
    let lnum = search("<<[\"']*EO", "bW")
    let line = getline(lnum)
    let ind = indent(lnum)
  endif
  " Don't indent comments
  if line =~ '^\s*#'
    return ind
  endif
  " Indent blocks enclosed by {} or ()
  if line =~ '[{(]\s*\(#[^)}]*\)\=$'
    let ind = ind + &sw
  endif
  if cline =~ '^\s*[)}]'
    let ind = ind - &sw
  endif

  return ind
endfunction
