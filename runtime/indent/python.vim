" Python indent file
" Language:	Python
" Maintainer:	David Bustos <bustos@caltech.edu>
" Last Change:	March 8, 2002

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
  finish
endif
let b:did_indent = 1

" Some preliminary settings
setlocal nolisp		" Make sure lisp indenting doesn't supersede us
setlocal autoindent	" indentexpr isn't much help otherwise

setlocal indentexpr=GetPythonIndent(v:lnum)
setlocal indentkeys+=<:>,=elif

" Only define the function once.
if exists("*GetPythonIndent")
  finish
endif

function GetPythonIndent(lnum)
  " Give up if this line is explicitly joined.
  if getline(a:lnum - 1) =~ '\\$'
    return -1
  endif

  " Search backwards for the first non-empty line.
  let plnum = prevnonblank(v:lnum - 1)

  if plnum == 0
    " This is the first non-empty line, use zero indent.
    return 0
  endif

  " If the previous line ended with a colon, indent this line
  if getline(plnum) =~ '^[^#]*:\s*\(#.*\)\=$'
    return indent(plnum) + &sw

  " If the previous line was a stop-execution statement...
  elseif getline(plnum) =~ '^\s*\(break\|continue\|raise\|return\)\>'
    " See if the user has already dedented
    if indent(a:lnum) > indent(plnum) - &sw
      " If not, recommend one dedent
      return indent(plnum) - &sw
    else
      " Otherwise, trust the user
      return -1
    endif

  " If the current line begins with a header keyword, dedent
  elseif getline(a:lnum) =~ '^\s*\(elif\|else\|except\|finaly\)\>'

    " Unless the previous line was a one-liner
    if getline(plnum) =~ '^\s*\(for\|if\|try\)\>'
      return indent(plnum)
    endif

    " Or the user has already dedented
    if indent(a:lnum) <= indent(plnum) - &sw
      return -1
    endif

    return indent(plnum) - &sw

  else
    return -1

  endif
endfunction

" vim:sw=2
