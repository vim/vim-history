" Vim indent file
" Language:     OCaml
" Maintainers:  Mike Leary    <leary@nwlink.com>
"               Markus Mottl  <markus@oefai.at>
" URL:          http://www.ai.univie.ac.at/~markus/vim/indent/ocaml.vim
" Last Change:  2001 Oct 03 - minor addition  (MM)
"               2001 Sep 02 - set option 'expandtab'  (MM)
"               2001 Aug 29 - revised all rules  (MM)

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
  finish
endif
let b:did_indent = 1

setlocal expandtab
setlocal indentexpr=GetOCamlIndent()
setlocal indentkeys+=0=done,0=end,0=if,0=then,0=else,0=and,0=in,0=with,0=>],0=\|],0},0],0)
setlocal nolisp
setlocal nosmartindent
setlocal textwidth=80

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
  let lline=getline(lnum)

  " Return double 'shiftwidth' after lines matching:
  if lline =~ '^\s*|.*->\s*$'
    return ind + &sw + &sw
  endif

  " Add a 'shiftwidth' after lines ending with:
  if lline =~ '\(:\|=\|->\|<-\|(\|[\|{\|[|\|[<\|(\*\|\<\(begin\|struct\|sig\|functor\|object\|try\|do\|if\|then\|else\|fun\|function\|parser\)\>\)\s*$'
    let ind = ind + &sw
  endif

  let line=getline(v:lnum)

  " Subtract a 'shiftwidth' if current line begins with:
  if line =~ '^\s*\(\(done\|end\)\>\|>]\||]\|}\|]\|)\)'
    return ind - &sw

  " Subtract a 'shiftwidth' if current line begins with 'if' and last
  " line ends with 'else':
  elseif line =~ '^\s*if\>'
    if lline =~ '\<else\s*$'
      return ind - &sw
    else return ind
    endif

  " Subtract a 'shiftwidth' if current line begins with 'in' and last
  " line does not start with 'let' or 'and':
  elseif line =~ '^\s*in\>'
    if lline !~ '^\s*\(let\|and\)\>'
      return ind - &sw
    else return ind
    endif

  " Subtract a 'shiftwidth' if current line begins with 'else' or 'then'
  " and previous line does not start with 'if', 'then' or 'else'
  elseif line =~ '^\s*\(else\|then\)\>'
    if lline !~ '^\s*\(if\|else\|then\)\>'
      return ind - &sw
    else return ind
    endif

  " Subtract a 'shiftwidth' if current line begins with 'and' and previous
  " line does not start with 'let', 'and' or 'type' or end with 'end'
  " (for classes):
  elseif line =~ '^\s*and\>'
    if lline !~ '^\s*\(let\|and\|type\)\>\|\<end\s*$'
      return ind - &sw
    else return ind
    endif

  " Subtract a 'shiftwidth' if current line begins with 'with'
  " and previous line does not start with 'match' or 'try':
  elseif line =~ '^\s*with\>'
    if lline !~ '^\s*\(match\|try\)\>'
      return ind - &sw
    else return ind
    endif

  endif

  return ind

endfunction

" vim:sw=2
