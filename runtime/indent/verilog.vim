" Vim indent file
" Language:	Verilog HDL
" Maintainer:	Chih-Tsun Huang <cthuang@larc.ee.nthu.edu.tw>
" Last Change:	Thu Jan  4 11:00:19 CST 2001
" Buffer Variables:
"               b:verilog_indent_modules -- indenting after the declaration
"                                           of modules, functions, tasks, etc.

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
  finish
endif
let b:did_indent = 1

setlocal indentkeys=:,o,O,e,=begin,=else,=end,=join,=endcase
setlocal indentkeys+==endmodule,=endfunction,=endtask,=endspecify
setlocal indentexpr=GetVerilogIndent()

" Only define the function once.
if exists("*GetVerilogIndent")
  finish
endif

function GetVerilogIndent()

  if exists('b:verilog_indent_modules')
    let indent_modules = &sw
  else
    let indent_modules = 0
  endif

  " Find a non-blank line above the current line.
  let lnum = prevnonblank(v:lnum - 1)

  " At the start of the file use zero indent.
  if lnum == 0
    return 0
  endif

  " Add a 'shiftwidth' if the start of last line is a block statement
  let ind = indent(lnum)
  if getline(lnum) =~ '^\s*\<\(if\|else\|for\|case\)\>' ||
    \ getline(lnum) =~ '^\s*\<\(always\|initial\)\>' ||
    \ getline(lnum) =~ '^\s*\w\+\s*\(:\)\s*$'
    let ind = ind + &sw

  elseif getline(lnum) =~ '^\s*\<\(module\|function\|task\|specify\|fork\)\>'
    let ind = ind + indent_modules

  " Add a 'shiftwidth' after a 'begin' statement
  elseif getline(lnum) =~ '\(\<begin\>\)\(\s\|:\|\w\)*$'
    let ind = ind + &sw
  endif

  " Subtract a 'shiftwidth' on the end of the block or a stand-alone 'begin'
  if getline(v:lnum) =~ '^\s*\<\(join\|end\|endcase\|begin\)\>'
    let ind = ind - &sw
  elseif getline(v:lnum) =~
    \ '^\s*\<\(endmodule\|endfunction\|endtask\|endspecify\)\>'
    let ind = ind - indent_modules
  endif

  " Subtract a 'shiftwidth' 'else' for a one liner block
  if getline(v:lnum) =~ '^\s*\<\(else\)\>' &&
    \ getline(lnum) !~ '\<\(end\)\>\s*$'
    let ind = ind - &sw
  endif

  return ind
endfunction

" vim:sw=2
