" Vim indent file
" Language:	Vim script
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2001 Aug 27

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
  finish
endif
let b:did_indent = 1

setlocal indentexpr=GetVimIndent()
setlocal indentkeys+==end,=else,0\\

" Only define the function once.
if exists("*GetVimIndent")
  finish
endif

function GetVimIndent()
  " Find a non-blank line above the current line.
  let lnum = prevnonblank(v:lnum - 1)

  " If the current line doesn't start with '\' and below a line that starts
  " with '\', use the indent of the line above it.
  if getline(v:lnum) !~ '^\s*\\'
    while lnum > 0 && getline(lnum) =~ '^\s*\\'
      let lnum = lnum - 1
    endwhile
  endif

  " At the start of the file use zero indent.
  if lnum == 0
    return 0
  endif

  " Add a 'shiftwidth' after :if, :while, :function and :else.
  " Add it three times for a line that starts with '\' after a line that
  " doesn't.
  let ind = indent(lnum)
  if getline(v:lnum) =~ '^\s*\\' && v:lnum > 1 && getline(lnum) !~ '^\s*\\'
    let ind = ind + &sw * 3
  elseif getline(lnum) =~ '^\s*\(if\>\|wh\|fu\|el\)'
    let ind = ind + &sw
  endif

  " Subtract a 'shiftwidth' on a :endif, :endwhile, :endfun and :else.
  if getline(v:lnum) =~ '^\s*\(ene\@!\|el\)'
    let ind = ind - &sw
  endif

  return ind
endfunction

" vim:sw=2
