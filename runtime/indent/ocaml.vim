" Vim indent file
" Language:	OCaml
" Maintainer:	Markus Mottl <markus@oefai.at>
" Last Change:	2001 Aug 27
"		2001 July 17 - first release

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
  finish
endif
let b:did_indent = 1

setlocal indentexpr=GetOCamlIndent()
setlocal indentkeys+=0=done,0=end,0=in,0=with,0],0)

" Only define the function once.
if exists("*GetOCamlIndent")
  finish
endif

function GetOCamlIndent()
  " Find a non-blank line above the current line.
  let lnum = prevnonblank(v:lnum - 1)

  " At the start of the file use zero indent.
  if lnum == 0
    return 0
  endif

  let ind=indent(lnum)
  let line=getline(lnum)

  " Add further 'shiftwidth' after lines match:
  if line =~ '^\s*|.*->$'
    let ind = ind + &sw + &sw

  " Add a 'shiftwidth' after lines ending with:
  elseif line =~ '\(=\|->\|(\|[\|{\|[<\|(\*\|begin\|struct\|sig\|functor\|object\|try\|do\|then\|else\|fun\|function\|parser\)\s*$'
    let ind = ind + &sw
  endif

  " Subtract a 'shiftwidth' if line begins with:
  if getline(v:lnum) =~ '^\s*\(\(done\|end\|in\)\>\|}\|]\|)\)'
    let ind = ind - &sw
  endif

  return ind
endfunction

" vim:sw=2
