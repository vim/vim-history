" Vim indent file
" Language:	Vim script
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2000 Oct 13

setlocal indentexpr=GetVimIndent()
setlocal indentkeys+==end,=else

" Only define the function once.
if exists("*GetVimIndent")
  finish
endif

function GetVimIndent()
  " Find a non-blank line above the current line.
  let lnum = skipblank(v:lnum - 1)

  " At the start of the file use zero indent.
  if lnum == 0
    return 0
  endif

  " Add a 'shiftwidth' after :if, :while, :function and :else
  let ind = indent(lnum)
  if getline(lnum) =~ '^\s*\(if\>\|wh\|fu\|el\)'
    let ind = ind + &sw
  endif

  " Subtract a 'shiftwidth' on a :endif, :endwhile, :endfun and :else
  if getline(v:lnum) =~ '^\s*\(ene\@!\|el\)'
    let ind = ind - &sw
  endif

  return ind
endfunction

" vim:sw=2
