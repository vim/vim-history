" Vim indent file
" Language: Lua script
" Maintainer: Max Ischenko <mfi@ukr.net>
" Last Change: 2001 Sep 02

" Only define the function once.
if exists("*GetLuaIndent") | finish | endif

setlocal indentexpr=GetLuaIndent()
setlocal autoindent

function! GetLuaIndent()
  " Find a non-blank line above the current line.
  let lnum = prevnonblank(v:lnum - 1)

  " Hit the start of the file, use zero indent.
  if lnum == 0
    return 0
  endif

  " If the line trailed with a *, +, comma, . or (, trust the user
  if getline(lnum) =~ '\(\*\|\.\|+\|,\|(\)\(\s*#.*\)\=$'
		" return -1
  endif

  " Add a 'shiftwidth' after lines beginning with:
  " module, class, dev, if, for, while, until, else, elsif, case, when, {
  let ind = indent(lnum)
  let flag = 0
  if getline(lnum) =~ '^\s*\(function\>\|if\>\|for\>\|while\>\|until\>\|else\>\|elsif\>\)' || getline(lnum) =~ '{\s*$' || getline(lnum) =~ '\({\|\<do\>\).*|.*|\s*$' || getline(lnum) =~ '\<do\>\(\s*#.*\)\=$' || getline(lnum) =~ '\(\s*=\s*function\)'
    let ind = ind + &sw
    let flag = 1
  endif

  " Subtract a 'shiftwidth' after lines ending with
  " "end" when they begin with while, if, for, until
  if flag == 1 && getline(lnum) =~ '\<end\>\(\s*#.*\)\=$'
    let ind = ind - &sw
  endif

  " Subtract a 'shiftwidth' on end, else and, elsif, when and }
  if getline(v:lnum) =~ '^\s*\(end\>\|else\>\|elsif\>\|}\)'
    let ind = ind - &sw
  endif

  " Subtract a 'shiftwidth' on end, else and, elsif, when and }
  if getline(v:lnum) =~ '\(\<end\>\)'
		" let ind = ind - &sw
  endif

  return ind
endfunction
