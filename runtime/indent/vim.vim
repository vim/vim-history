" Vim indent file
" Language:	Vim script
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2000 Aug 27

setlocal indentexpr=GetVimIndent()
setlocal cinkeys+==end

" Only define the function once.
if exists("*GetVimIndent")
  finish
endif

function GetVimIndent()
  if v:lnum == 1
    return 0
  endif
  let ind = indent(v:lnum - 1)
  if getline(v:lnum - 1) =~ '^\s*\(if\>\|wh\|fu\|el\)'
    let ind = ind + &sw
  endif
  if getline(v:lnum) =~ '^\s*\(ene\@!\|el\)'
    let ind = ind - &sw
  endif
  return ind
endfunction

" vim:sw=2
